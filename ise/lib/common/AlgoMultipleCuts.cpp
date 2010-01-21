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
	Multiple-cuts identification
	"Exact and Approximate Algorithms for the Extension of Embedded Processor Instruction Sets"

	Identifies k nonoverlapping cuts (default k = 2)
	Does not return any cuts - benchmarking/testing only
*/

#include <llvm/Support/Casting.h>
#include "Primitives.h"
#include "AlgoMultipleCuts.h"

using namespace std;
using namespace boost;
using namespace llvm;

void AlgoMultipleCuts::search(unsigned int curChoice, unsigned int curIndex, const DataFlowGraph &dfg,
						   const Architecture &arch, ParamVector params, unsigned int nodes)
{
	n_cuts++;
	cut[curIndex] = curChoice;
	if (curChoice < k)
	{
		// check if this node is a new permanent input
		NumVector &inputVector = inputVectors[curChoice];
		if (inputVector[curIndex] != 0)
		{
			if (!invalid[topo[curIndex]])
				params[curChoice].perm_inputs++;
		}
		// update input references to this node
		DataFlowGraph::DFG::in_edge_iterator inEdgeIt, inEdgeItEnd;
		tie(inEdgeIt, inEdgeItEnd) = dfg.in_edges(topo[curIndex]);
		for (; inEdgeIt != inEdgeItEnd; ++inEdgeIt)
		{
			DataFlowGraph::Vertex source = dfg.source(*inEdgeIt);
			unsigned int sourceIndex = topoVector[source];
			--inputVector[sourceIndex];
		}
		assert(params[curChoice].perm_inputs <= params[curChoice].inputs);
	}
	if (curChoice > 0)
	{
		CutParams &curParams = params[curChoice - 1];
		// convexity is violated if there is an edge from the current node to
		// any node not included in the current cut leading to another
		// node that is included
		DataFlowGraph::DFG::out_edge_iterator edgeIt, edgeItEnd;
		tie(edgeIt, edgeItEnd) = dfg.out_edges(topo[curIndex]);
		bool lti = false;
		bool convex = true;
		BitVector &curLti = ltis[curChoice - 1];
		for (; edgeIt != edgeItEnd; ++edgeIt)
		{
			DataFlowGraph::Vertex target = dfg.target(*edgeIt);
			unsigned int targetIndex = topoVector[target];
			if (cut[targetIndex] != curChoice)
			{
				if (curLti[targetIndex])
					convex = false;
			}
			else
				lti = true;
		}
		curLti[curIndex] = lti;
		// update number of in-/outputs
		// if this node leads to another node in the cut then it was
		// an input before.
		// if it leads to a node not in the cut then it becomes an output
		tie(edgeIt, edgeItEnd) = dfg.out_edges(topo[curIndex]);
		bool is_output = false;
		bool was_input = false;
		for (; edgeIt != edgeItEnd; ++edgeIt)
		{
			DataFlowGraph::Vertex target = dfg.target(*edgeIt);
			unsigned int targetIndex = topoVector[target];
			if (cut[targetIndex] != curChoice)
				is_output = true;
			else
				was_input = true;
		}
		if (is_output || dfg.getType(topo[curIndex]) == DataFlowGraph::OUTPUT)
			curParams.outputs++;
		if (was_input) curParams.inputs--;

		// look at all ingoing edges and decide if they are
		// new (permanent) inputs
		DataFlowGraph::DFG::in_edge_iterator inEdgeIt, inEdgeItEnd;
		tie(inEdgeIt, inEdgeItEnd) = dfg.in_edges(topo[curIndex]);
		NumVector &curInputVector = inputVectors[curChoice - 1];
		for (; inEdgeIt != inEdgeItEnd; ++inEdgeIt)
		{
			bool new_input = false;
			DataFlowGraph::Vertex source = dfg.source(*inEdgeIt);
			unsigned int sourceIndex = topoVector[source];
			if (curInputVector[sourceIndex] == 0)
			{
				curParams.inputs++;
				new_input = true;
			}
			++curInputVector[sourceIndex];

			if (new_input)
			{
				if (invalid[source])
					curParams.perm_inputs++;
			}
		}
		curParams.nodes++;
		nodes++;

		// check if adding this node violates any constraints
		if (!convex)
			return;
		// number of outputs
		if (curParams.outputs > arch.getMaxOutputs())
			return;
		// number of permanent inputs
		if (curParams.perm_inputs > arch.getMaxInputs())
			return;
		// forbidden node?
		if (invalid[topo[curIndex]])
			return;

		assert(curParams.perm_inputs <= curParams.inputs);

		// number of inputs
		if (curParams.inputs <= arch.getMaxInputs())
		{
            // feasible cut found
            // additional heuristics to reduce the total number of cuts
            // may be employed here
			feasibleCuts++;
		}
	}
	if (curIndex + 1 == topo.size())
		return;
	curIndex++;
	for (unsigned int i = k; i > 0; i--)
		search(i, curIndex, dfg, arch, params, nodes);
	search(0, curIndex, dfg, arch, params, nodes);
}

bool AlgoMultipleCuts::isValidNode(DataFlowGraph::Vertex v, const DataFlowGraph &dfg,
							const Architecture &arch)
{
	DataFlowGraph::operator_t type = dfg.getType(v);
	if (type == DataFlowGraph::INPUT)
		return false;
	else if (type == DataFlowGraph::CONSTANT)
		return true;
	const llvm::Value* value = dfg.getValue(v);
	if (isa<Instruction>(value))
		if (!arch.isValidInstruction(cast<Instruction>(value)))
			return false;
	return true;
}

void AlgoMultipleCuts::checkInvalid(const DataFlowGraph &dfg, const Architecture &arch)
{
	invalid.resize(topo.size());
	invalid.reset();
	DataFlowGraph::DFG::vertex_iterator vIt, vItEnd;
	tie(vIt, vItEnd) = dfg.vertices();
	for (; vIt != vItEnd; ++vIt)
	{
		if (!isValidNode(*vIt, dfg, arch))
			invalid[*vIt] = true;
	}
}

void AlgoMultipleCuts::setNumCuts(unsigned int cuts)
{
	if (cuts >= 1)
		k = cuts;
}

void AlgoMultipleCuts::run(const DataFlowGraph &dfg, const Architecture &arch,
						   ResultVector &result, bool benchmark)
{
	if (dfg.num_vertices() == 0)
		return;
	m_benchmark = benchmark;
	topo = dfg.reverseTopologicalSort();
	topoVector.resize(topo.size());
	for (unsigned int i = 0; i < topo.size(); i++)
		topoVector[topo[i]] = i;
	cut = NumVector(topo.size(), 0);
	n_cuts = feasibleCuts = 0;
	checkInvalid(dfg, arch);

	inputVectors = std::vector<NumVector>(k);
	ltis = std::vector<BitVector>(k);
	for (unsigned int i = 0; i < k; i++)
	{
		ltis[i] = BitVector(topo.size());
		inputVectors[i] = NumVector(topo.size(), 0);
	}
	ParamVector params = ParamVector(k);
	for (unsigned int i = k; i > 0; i--)
		search(i, 0, dfg, arch, params, 0);
	search(0, 0, dfg, arch, params, 0);
/*
	cout << feasibleCuts << "\t" << n_cuts << "\n";
	cout << "considered " << n_cuts << " cuts\n";
	cout << "feasible cuts " << feasibleCuts << "\n";
*/
}
