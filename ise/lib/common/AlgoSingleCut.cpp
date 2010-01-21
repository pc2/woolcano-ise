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
	Single-cut identification
	"Exact and Approximate Algorithms for the Extension of Embedded Processor Instruction Sets"
*/

#include "llvm/Support/Casting.h"
#include "Primitives.h"
#include "AlgoSingleCut.h"

using namespace std;
using namespace boost;
using namespace llvm;

void AlgoSingleCut::search(bool curChoice, unsigned int curIndex, const DataFlowGraph &dfg,
						   const Architecture &arch,
						   unsigned int inputs, unsigned int outputs,
						   unsigned int perm_inputs, unsigned int nodes)
{
	cut[curIndex] = curChoice;
	cuts++;
	if (!curChoice)
	{
		// check if this node is a new permanent input
		if (inputVector[curIndex] != 0)
		{	
			if (!invalid[topo[curIndex]])
				perm_inputs++;
		}
		assert(perm_inputs <= inputs);
		// update input references to this node
		DataFlowGraph::DFG::in_edge_iterator inEdgeIt, inEdgeItEnd;
		tie(inEdgeIt, inEdgeItEnd) = dfg.in_edges(topo[curIndex]);
		for (; inEdgeIt != inEdgeItEnd; ++inEdgeIt)
		{
			DataFlowGraph::Vertex source = dfg.source(*inEdgeIt);
			unsigned int sourceIndex = topoVector[source];
			assert(inputVector[sourceIndex] > 0);
			--inputVector[sourceIndex];
		}
	}
	else
	{
		// convexity is violated if there is an edge from the current node to
		// any node not included in the current cut leading to another
		// node that is included
		DataFlowGraph::DFG::out_edge_iterator edgeIt, edgeItEnd;
		tie(edgeIt, edgeItEnd) = dfg.out_edges(topo[curIndex]);
		bool curLti = false;
		bool convex = true;
		for (; edgeIt != edgeItEnd; ++edgeIt)
		{
			DataFlowGraph::Vertex target = dfg.target(*edgeIt);
			unsigned int targetIndex = topoVector[target];
			if (!cut[targetIndex])
			{
				if (lti[targetIndex])
					convex = false;
			}
			else
				curLti = true;
		}
		lti[curIndex] = curLti;
		// update number of in-/outputs
		// if this node leads to other nodes in the cut then it was
		// an input before.
		// if it leads to a node not in the cut then it becomes an output
		tie(edgeIt, edgeItEnd) = dfg.out_edges(topo[curIndex]);
		bool is_output = false, was_input = false;
		for (; edgeIt != edgeItEnd; ++edgeIt)
		{
			DataFlowGraph::Vertex target = dfg.target(*edgeIt);
			unsigned int targetIndex = topoVector[target];
			if (!cut[targetIndex])
				is_output = true;
			else
				was_input = true;
		}
		if (is_output || dfg.getType(topo[curIndex]) == DataFlowGraph::OUTPUT)
			outputs++;
		if (was_input)
		{
			assert(inputs > 0);
			inputs--;
		}

		// look at all ingoing edges and decide if they are
		// new (permanent) inputs
		DataFlowGraph::DFG::in_edge_iterator inEdgeIt, inEdgeItEnd;
		tie(inEdgeIt, inEdgeItEnd) = dfg.in_edges(topo[curIndex]);
		for (; inEdgeIt != inEdgeItEnd; ++inEdgeIt)
		{
			bool new_input = false;
			DataFlowGraph::Vertex source = dfg.source(*inEdgeIt);
			unsigned int sourceIndex = topoVector[source];
			if (inputVector[sourceIndex] == 0)
			{
				inputs++;
				new_input = true;
			}
			++inputVector[sourceIndex];

			if (new_input)
			{
				if (invalid[source])
					perm_inputs++;
			}
		}
		nodes++;
		// check if adding this node violates any constraints
		if (!convex)
			return;
		// number of outputs
		if (outputs > arch.getMaxOutputs())
			return;
		// number of permanent inputs
		if (perm_inputs > arch.getMaxInputs())
			return;
		// forbidden node?
		if (invalid[topo[curIndex]])
			return;
		// number of inputs
		if (inputs <= arch.getMaxInputs())
		{
            // feasible cut found
            // additional heuristics to reduce the total number of cuts
            // may be employed here
			feasibleCuts++;
			if (!m_benchmark)
				allCuts.insert(cut);
/*
			cout << "found candidate: " << nodes << " nodes, " << inputs <<
				" inputs, " << perm_inputs << " perm. inputs, " << outputs << " outputs\n";
*/
		}
	}
	if (curIndex + 1 == topo.size())
		return;
	curIndex++;
	search(true, curIndex, dfg, arch, inputs, outputs, perm_inputs, nodes);
	search(false, curIndex, dfg, arch, inputs, outputs, perm_inputs, nodes);
}

bool AlgoSingleCut::isValidNode(DataFlowGraph::Vertex v, const DataFlowGraph &dfg,
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

void AlgoSingleCut::checkInvalid(const DataFlowGraph &dfg, const Architecture &arch)
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

void AlgoSingleCut::run(const DataFlowGraph &dfg, const Architecture &arch,
						ResultVector &result, bool benchmark)
{
	if (dfg.num_vertices() == 0)
		return;
	m_benchmark = benchmark;
	// initialize data structures
	allCuts.clear();
	topo = dfg.reverseTopologicalSort();
	inputVector = NumVector(topo.size());
	topoVector = NumVector(topo.size());
	for (unsigned int i = 0; i < topo.size(); i++)
		topoVector[topo[i]] = i;
	cut = BitVector(topo.size());
	lti = BitVector(topo.size());
	cuts = feasibleCuts = 0;
	checkInvalid(dfg, arch);

	// perform the search
	search(true, 0, dfg, arch, 0, 0, 0, 0);
	search(false, 0, dfg, arch, 0, 0, 0, 0);
/*
	//cout << "considered " << cuts << " cuts\n";
	//cout << "feasible cuts: " << feasibleCuts << "\n";
*/
	// return all cuts
	result.reserve(allCuts.size());
	for (PatternSet::const_iterator it = allCuts.begin(); it != allCuts.end(); ++it)
	{
		const BitVector &bv = *it;
		BitVector conv(topo.size());
		for (BitVector::size_type i = bv.find_first();
			i != BitVector::npos; i = bv.find_next(i))
		{
			conv[topo[i]] = true;
		}
		result.push_back(conv);
	}

/*
// TESTCODE
	for (PatternSet::const_iterator it = allCuts.begin();
		it != allCuts.end();)

	{
		if (!checkConnected(*it, dfg))
		{
			it = allCuts.erase(it);
		}
		else
		{
			for (BitVector::size_type i = it->find_first();
				i != BitVector::npos; i = it->find_next(i))
			{
				cout << i << " ";
			}
			cout << "\n";
			++it;
		}
	}
	cout << "patterns remaining: " << allCuts.size() << "\n";
*/
}
/*
bool AlgoSingleCut::checkConnected(const BitVector &p, const DataFlowGraph &dfg)
{
	BitVector p_copy = p;
	BitVector::size_type i = p.find_first();
	if (i == BitVector::npos) return true;
	return visit(p_copy, i, dfg) == p.count();
}

unsigned int AlgoSingleCut::visit(BitVector &p, unsigned int nodeIndex, const DataFlowGraph &dfg)
{
	unsigned int visited = 1;
	p[nodeIndex] = false;
	DataFlowGraph::DFG::out_edge_iterator edgeIt, edgeItEnd;
	tie(edgeIt, edgeItEnd) = dfg.out_edges(topo[nodeIndex]);
	for (; edgeIt != edgeItEnd; ++edgeIt)
	{
		DataFlowGraph::Vertex target = dfg.target(*edgeIt);
		if (p[topoVector[target]])
			visited += visit(p, topoVector[target], dfg);
	}

	DataFlowGraph::DFG::in_edge_iterator inEdgeIt, inEdgeItEnd;
	tie(inEdgeIt, inEdgeItEnd) = dfg.in_edges(topo[nodeIndex]);
	for (; inEdgeIt != inEdgeItEnd; ++inEdgeIt)
	{
		DataFlowGraph::Vertex source = dfg.source(*inEdgeIt);
		if (p[topoVector[source]])
			visited += visit(p, topoVector[source], dfg);
	}

	return visited;
}*/
