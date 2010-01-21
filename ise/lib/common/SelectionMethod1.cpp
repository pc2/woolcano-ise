/*
Copyright (c) 2009, Martin Tofall
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * The name of the author may not be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*
	selection strategy:
		- assign 'value' to each instruction candidate
		- select best candidates, exclude overlapping ones
		- exclude candidates with speedup below predefined threshold
*/
#include "SelectionMethod1.h"
#include "DataFlowGraph.h"
#include "RuntimeEstimation.h"

const float SelectionMethod1::threshold = 1.0f;

void SelectionMethod1::run(const ProfileList &profInfo, const ResultMap &candidates,
						   const DfgMap &dfgs, const Architecture &arch, ResultMap &selection)
{
	// construct DFGs for all candidates and estimate their 'value'
	CandidateList candidateList;
	for (ProfileList::const_iterator it = profInfo.begin(); it != profInfo.end(); ++it)
	{
		ResultMap::const_iterator cand_it = candidates.find(it->first);
		if (cand_it == candidates.end()) continue;
		const DataFlowGraph &parentDfg = dfgs.find(it->first)->second;
		for (ResultVector::const_iterator bv_it = cand_it->second.begin();
			bv_it != cand_it->second.end(); ++bv_it)
		{
			DataFlowGraph dfg(parentDfg, *bv_it);
			// filter templates with too many inputs (MAXMISO algo)
			if (dfg.num_inputs() > arch.getMaxInputs()) continue;
			unsigned int sw = RuntimeEstimation::estimateSwRuntime(dfg, arch);
			if (sw == 0) continue;
			unsigned int hw = arch.convertHwToSwTiming(RuntimeEstimation::estimateHwRuntime(dfg, arch))
				+ arch.getExecutionOverhead(dfg.num_inputs(), dfg.num_outputs());
			float ratio = static_cast<float>(sw) / static_cast<float>(hw);
/*
			if (bv_it->count())
			{
				//std::cout << "nodes: " << bv_it->count() << "\n";
				//std::cout << "sw: " << sw << "\n";
				//std::cout << "hw: " << hw << "\n";
				std::cout << "nodes: " << bv_it->count() << "\tsw: " << sw << "\thw: " << hw << "\tratio: " << ratio << "\n";
			}
*/
			// ignore candidates with speedup below predefined threshold
			if (ratio > threshold)
			{
				// calculate 'value'
				float value = (1.0f + it->second.prob) * ratio * (dfg.num_vertices() - dfg.num_inputs());
				candidateList.push_back(CandidateInfo(value, it->first, *bv_it));
			}
		}
	}
	// select best candidates, exclude overlapping patterns
	candidateList.sort(CandidateListSortPredicate);
	BitVectorMap masks;
	bool done = false;
	unsigned int selected = 0;
	while (!done)
	{
		if (candidateList.empty() || selected == arch.getMaxCustomInstructions())
		{
			done = true;
			continue;
		}
		const CandidateInfo& curCand = candidateList.front();
		ResultMap::iterator res_it = selection.find(curCand.basicBlock);
		if (res_it == selection.end())
		{
			selection.insert(make_pair(curCand.basicBlock, ResultVector(1, curCand.pattern)));
			masks.insert(make_pair(curCand.basicBlock, curCand.pattern));
			++selected;
		}
		else
		{
			BitVectorMap::iterator bv_it = masks.find(curCand.basicBlock);
			if (!bv_it->second.intersects(curCand.pattern))
			{
				bv_it->second |= curCand.pattern;
				ResultMap::iterator sel_it = selection.find(curCand.basicBlock);
				sel_it->second.push_back(curCand.pattern);
				++selected;
			}
		}
		candidateList.pop_front();
	}
}

bool SelectionMethod1::CandidateListSortPredicate(const CandidateInfo& lhs, const CandidateInfo& rhs)
{
	return lhs.value > rhs.value;
}
