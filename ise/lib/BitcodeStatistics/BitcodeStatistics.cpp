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
#include <limits.h>
#include <map>
#include <utility>
#include <algorithm>
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Streams.h"
#include "llvm/Instructions.h"
#include "llvm/Transforms/Instrumentation.h"
#include "DataFlowGraph.h"
#include "AlgoMaxMiso.h"
#include "Util.h"

using namespace llvm;
using namespace std;

namespace {
	class BitcodeStatsPass : public ModulePass
	{
	public:
		static char ID;
		virtual bool runOnModule(Module &M);
		virtual void getAnalysisUsage(AnalysisUsage &) const;
		BitcodeStatsPass() : ModulePass(&ID) {}
	private:
		typedef map<unsigned int, unsigned int> OpcodeMap;
		typedef pair<unsigned int, unsigned int> OpcodePair;
		typedef vector<OpcodePair> OpcodeVector;
		static bool opcpcmp(const OpcodePair &a, const OpcodePair &b);
	};

	template <class T>
	inline T const &_p_min( T const &a, T const &b )
	{
		return (a < b) ? a : b;
	}
	template <class T>
	inline T const &_p_max( T const &a, T const &b )
	{
		return (a > b) ? a : b;
	}
}

char BitcodeStatsPass::ID = 0;
static RegisterPass<BitcodeStatsPass> X("bcstats", "display bitcode statistics");

bool BitcodeStatsPass::opcpcmp(const OpcodePair &a, const OpcodePair &b)
{
	return a.second > b.second;
}

void BitcodeStatsPass::getAnalysisUsage(AnalysisUsage &au) const
{
	au.setPreservesAll();
}

bool BitcodeStatsPass::runOnModule(Module &M)
{
	int bbmin = INT_MAX, bbmax = 0, instc = 0, bbc = 0;
	double dfgmin = INT_MAX;
	double dfgmax = 0.0, dfgc = 0.0;
	float bbavg = 0.0f, dfgavg = 0.0f;
	OpcodeMap opcmap;

	for (Module::const_iterator I = M.begin(), E = M.end(); I != E; ++I)
	{
		bbc += I->size();
		for (Function::const_iterator BB = I->begin(), E = I->end(); BB != E; ++BB)
		{
			DataFlowGraph dfg(*BB);
			double mp = dfg.getLongestPath();
			if (mp != 0.0)
			{
				dfgc += mp;
				dfgmax = _p_max<int>(dfgmax, mp);
				dfgmin = _p_min<int>(dfgmin, mp);
			}
			const BasicBlock::InstListType &insts = BB->getInstList();
			instc += insts.size();
			bbmax = _p_max<int>(bbmax, insts.size());
			bbmin = _p_min<int>(bbmin, insts.size());
			for (BasicBlock::InstListType::const_iterator i = insts.begin(); i != insts.end(); ++i)
			{
				unsigned int opc = i->getOpcode();
				OpcodeMap::iterator it = opcmap.find(opc);
				if (it == opcmap.end())
					opcmap.insert(make_pair(opc, 1));
				else
					it->second++;
			}
		}
	}
	llvm::cout << "Module statistics:\n";
	llvm::cout << "\t" << instc << " inst.\n";
	llvm::cout << "\t" << bbc << " basic blocks\n\n";

	bbavg = static_cast<float>(instc) / static_cast<float>(bbc);
	llvm::cout << "Basic block statistics:\n";
	llvm::cout << "\tMin. size:\t" << bbmin << " inst.\n";
	llvm::cout << "\tMax. size:\t" << bbmax << " inst.\n";
	llvm::cout << "\tAvg. size:\t" << bbavg << " inst.\n\n";

	dfgavg = static_cast<float>(dfgc) / static_cast<float>(bbc);
	llvm::cout << "DFG statistics (critical path / basic block):\n";
	llvm::cout << "\tMin. size:\t" << dfgmin << "\n";
	llvm::cout << "\tMax. size:\t" << dfgmax << "\n";
	llvm::cout << "\tAvg. size:\t" << dfgavg << "\n\n";

	llvm::cout << "Opcode distribution:\n";
	OpcodeVector opcvec;
	for (OpcodeMap::const_iterator it = opcmap.begin(); it != opcmap.end(); ++it)
		opcvec.push_back(make_pair(it->first, it->second));
	sort(opcvec.begin(), opcvec.end(), opcpcmp);
	for (OpcodeVector::const_iterator it = opcvec.begin(); it != opcvec.end(); ++it)
	{
		string opcodeName = Instruction::getOpcodeName(it->first);
		string fill;
		for (string::size_type i = 0; i < 20 - opcodeName.length(); i++)
			fill += " ";
		llvm::cout << "\t" << opcodeName << fill << it->second <<
			"\t(" << (static_cast<float>(it->second) * 100.0f / static_cast<float>(instc)) <<
			"%" << ")\n";
	}

	return true;
}
