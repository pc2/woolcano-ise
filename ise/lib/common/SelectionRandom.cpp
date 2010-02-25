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
	randomly selects candidates to be used as custom instructions
	(prefers often executed basic blocks)
*/
#include <stdlib.h>
#include <set>
#include <algorithm>
#include "SelectionRandom.h"
#include "Util.h"

using namespace std;

void SelectionRandom::run(
      const ProfileList &profInfo, const ResultMap &candidates,
      const DfgMap &dfgs, const Architecture &arch, ResultMap &selection,
      bool DisableComm, bool DisableMaxCI, bool DisableMaxInput, int MaxCI, 
      int MaxInput) 
{
	unsigned int maxInsts = arch.getMaxCustomInstructions(), selected = 0;
	ProfileList::const_iterator prof_it = profInfo.begin();
	if (prof_it == profInfo.end())
		return;
	ResultMap::const_iterator cand_it = candidates.find(prof_it->first);
	while (true)
	{
		if (cand_it == candidates.end())
		{
			++prof_it;
			if (prof_it == profInfo.end())
				return;
			else
				cand_it = candidates.find(prof_it->first);
		}
		else
		{
			// select candidates from current basic block
			const ResultVector &bvs = cand_it->second;
			if (bvs.size() > 0)
			{
				set<BitVector> selectionSet;
				unsigned int maxSelect = bvs.size() > 2 ? (rand() % (bvs.size() / 2)) : bvs.size();
				maxSelect = _p_min<unsigned int>(maxSelect, maxInsts - selected);
				for (unsigned int i = maxSelect; i > 0; --i)
				{
					int choice = rand() % bvs.size();
					selectionSet.insert(bvs[choice]);
				}
				ResultVector rv;
				copy(selectionSet.begin(), selectionSet.end(), rv.end());
				selection.insert(make_pair(cand_it->first, rv));
				selected += rv.size();
				if (selected == maxInsts)
					return;
			}
			cand_it = candidates.end();
		}
	}
}
