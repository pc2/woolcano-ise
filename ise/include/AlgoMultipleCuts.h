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
#ifndef _ALGO_MULTIPLE_CUTS_H_
#define _ALGO_MULTIPLE_CUTS_H_

#include <iostream>
#include <vector>
#include "DataFlowGraph.h"
#include "Architecture.h"
#include "IseAlgorithm.h"
#include "Types.h"

using namespace std;

class CutParams
{
public:
	unsigned int inputs, outputs, perm_inputs, nodes;
	CutParams() : inputs(0), outputs(0), perm_inputs(0), nodes(0) {}
};

class AlgoMultipleCuts : public IseAlgorithm
{
public:
	virtual void run(const DataFlowGraph &dfg, const Architecture &arch, ResultVector &result,
		bool benchmark = false);
	void setNumCuts(unsigned int cuts);
	AlgoMultipleCuts() : k(2) {}
private:
	void checkInvalid(const DataFlowGraph &dfg, const Architecture &arch);
	bool isValidNode(DataFlowGraph::Vertex v, const DataFlowGraph &dfg,
		const Architecture &arch);
	typedef std::vector<unsigned int> NumVector;
	typedef std::vector<CutParams> ParamVector;
	void search(unsigned int curChoice, unsigned int curIndex, const DataFlowGraph &dfg,
		const Architecture &arch, ParamVector params, unsigned int nodes);

	bool m_benchmark;
	// contains the vertex indices in reverse topological order
	DataFlowGraph::VertexVector topo;
	// serves as lookup-table for vertex => index in topological order
	NumVector topoVector;
	// stores the current cut
	NumVector cut;
	// entries set to true for nodes that lead to nodes that are 
	// included in the current cut
	std::vector<BitVector> ltis;
	// counts the number of times a certain vertex is referenced as an input
	std::vector<NumVector> inputVectors;
	// number of cuts to find
	unsigned int k;
	long long n_cuts, feasibleCuts;
	BitVector invalid;
};

#endif
