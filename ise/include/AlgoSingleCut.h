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
#ifndef _ALGO_SINGLE_CUT_H_
#define _ALGO_SINGLE_CUT_H_

#include <vector>
#include <set>
#include "DataFlowGraph.h"
#include "Architecture.h"
#include "IseAlgorithm.h"
#include "Types.h"

class AlgoSingleCut : public IseAlgorithm
{
public:
	virtual void run(const DataFlowGraph &dfg, const Architecture &arch, ResultVector &result,
		bool benchmark = false);
private:
	void search(bool curChoice, unsigned int curIndex, const DataFlowGraph &dfg,
		const Architecture &arch, unsigned int inputs, unsigned int outputs,
		unsigned int perm_inputs, unsigned int nodes);
	void checkInvalid(const DataFlowGraph &dfg, const Architecture &arch);
	bool isValidNode(DataFlowGraph::Vertex v, const DataFlowGraph &dfg,
		const Architecture &arch);

	typedef std::vector<unsigned int> NumVector;
	typedef std::set<BitVector> PatternSet;

	bool m_benchmark;
	// contains the vertex indices in reverse topological order
	DataFlowGraph::VertexVector topo;
	// serves as lookup-table for vertex => index in topological order
	NumVector topoVector;
	// counts the number of times a certain vertex is referenced as an input
	NumVector inputVector;
	// stores the current cut
	BitVector cut;
	// entries set to true for nodes that lead to nodes that are 
	// included in the current cut
	BitVector lti;
	BitVector invalid;
	long long cuts, feasibleCuts;
	PatternSet allCuts;
/*
	bool checkConnected(const BitVector &p, const DataFlowGraph &dfg);
	unsigned int visit(BitVector &p, unsigned int nodeIndex, const DataFlowGraph &dfg);
*/
};

#endif
