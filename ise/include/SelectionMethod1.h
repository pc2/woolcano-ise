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
#ifndef _SELECTION_METHOD_1_H_
#define _SELECTION_METHOD_1_H_

#include <list>
#include <llvm/BasicBlock.h>
#include "SelectionAlgorithm.h"
#include <sstream>

class SelectionMethod1 : public SelectionAlgorithm
{
public:
	virtual void run(const ProfileList &profInfo, const ResultMap &candidates,
		const DfgMap &dfgs, const  Architecture &arch, ResultMap &selection, 
        bool WriteGraphs) ;
private:
	static const float threshold;

	typedef std::map<const llvm::BasicBlock*, BitVector> BitVectorMap;

	class CandidateInfo
	{
	public:
		float value;
		const llvm::BasicBlock* basicBlock;
		BitVector pattern;
		CandidateInfo(float v, const llvm::BasicBlock* bb, const BitVector& bv)
			: value(v), basicBlock(bb), pattern(bv) {}
	};

	typedef std::list<CandidateInfo> CandidateList;

	static bool CandidateListSortPredicate(const CandidateInfo& lhs, const CandidateInfo& rhs);
};

#endif
