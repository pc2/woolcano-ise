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

#include <iostream>
#include <iomanip>
#include "SelectionMethod1.h"
#include "DataFlowGraph.h"
#include "RuntimeEstimation.h"
#include "llvm/BasicBlock.h"
#include "llvm/Function.h"
#include "Util.h"


const float SelectionMethod1::threshold = 1.0f;
const unsigned int LABEL_SIZE = 100;

void SelectionMethod1::run(
      const ProfileList &profInfo, const ResultMap &candidates,
      const DfgMap &dfgs, const Architecture &arch, ResultMap &selection,
      bool DisableComm, bool DisableMaxCI, bool DisableMaxInput, int MaxCI, 
      int MaxInput) 
{
	// construct DFGs for all candidates and estimate their 'value'
	CandidateList candidateList;
  std::ostringstream dot_label;
	for (ProfileList::const_iterator it = profInfo.begin(); it != profInfo.end(); ++it)
	{
		ResultMap::const_iterator cand_it = candidates.find(it->first);
		if (cand_it == candidates.end())  {
      std::cout << "\n# no candidates found for: " << it->first->getName() << "\n" ;
      continue;
    }
		const DataFlowGraph &parentDfg = dfgs.find(it->first)->second;

    const llvm::BasicBlock *BB = it->first;
    const llvm::Function   *FF = BB->getParent();
    const ProfileInfo info = it->second;

    char prof_info[LABEL_SIZE];
    snprintf(prof_info, LABEL_SIZE, "profile: [%.2f%%] %s.%s", info.prob*100 , FF->getName().c_str(), BB->getName().c_str());
    std::cout << "\n" << prof_info << "\n";

    // iterate over list of candidates for given BB
    int i = 0;
		for (ResultVector::const_iterator bv_it = cand_it->second.begin(); bv_it != cand_it->second.end(); ++bv_it, i++)
		{
      // build dfg representing candidate
			DataFlowGraph dfg(parentDfg, *bv_it);

			// filter templates with too many inputs (MAXMISO algo)
      if (!DisableMaxInput)
        if (dfg.num_inputs() > arch.getMaxInputs()) continue;

			unsigned int sw = RuntimeEstimation::estimateSwRuntime(dfg, arch);
			if (sw == 0) continue;

      unsigned int hwcomm = arch.getExecutionOverhead(dfg.num_inputs(), dfg.num_outputs());
      unsigned int hw = arch.convertHwToSwTiming( RuntimeEstimation::estimateHwRuntime(dfg, arch)) + hwcomm;

			float ratio = static_cast<float>(sw) / static_cast<float>(hw);


      std::string name = "cand_" + Util::stringify(i) + ".gv";
      char cand_res[LABEL_SIZE];
      snprintf(cand_res, LABEL_SIZE, "- %.35s: \t %3d nodes \t inputs: %3d \t sw: %3d \t hw: %3d[comm: %3d] \t ratio: %.2f \t ", 
          name.c_str(), bv_it->count(), dfg.num_inputs(), sw, hw, hwcomm, ratio);
      std::cout << cand_res;

			// ignore candidates with speedup below predefined threshold
      std::ostringstream dot_code;
      dot_code << "label = \"" << prof_info << "\\n" << cand_res;
			if (ratio > threshold)
			{
				// calculate 'value'
				float value = (1.0f + it->second.prob) * ratio * (dfg.num_vertices() - dfg.num_inputs());
				candidateList.push_back(CandidateInfo(value, it->first, *bv_it));
        std::cout << "selected\n";
        dot_code << "selected";
			} else {
        std::cout << "not selected\n";
        dot_code << "not selected";
      }
      dot_code << "\"\n";

      std::string graphName = FF->getName() + "-" + BB->getName() + "_cand_" + Util::stringify(i) + ".gv";
      Util::dumpToFile(graphName, parentDfg.writeGraphviz2(false,false, *bv_it, arch, dot_code.str()));
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
