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
#ifndef _ISE_TYPES_H_
#define _ISE_TYPES_H_

#include <llvm/BasicBlock.h>
#include <boost/dynamic_bitset.hpp>
#include <map>
#include <list>
#include <utility>

class ProfileInfo2
{
public:
	ProfileInfo2(unsigned int c, double p) : count(c), prob(p) {}
	unsigned int count;
	double prob;
};

typedef boost::dynamic_bitset<> BitVector;
typedef std::vector<BitVector> ResultVector;
typedef std::map<const llvm::BasicBlock*, ProfileInfo2> ProfileMap;
typedef std::pair<const llvm::BasicBlock*, ProfileInfo2> ProfilePair;
typedef std::list<ProfilePair> ProfileList;
typedef std::map<const llvm::BasicBlock*, ResultVector> ResultMap;
class DataFlowGraph;
typedef std::map<const llvm::BasicBlock*, DataFlowGraph> DfgMap;

#endif
