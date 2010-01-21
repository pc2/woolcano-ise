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
	Maximum Multiple Inputs Single Output
	"A DAG-based Design Approach for Reconfigurable VLIW Processors"
	No restriction on number of inputs
*/

#include "llvm/Support/Casting.h"
#include "AlgoMaxMiso.h"
#include "Primitives.h"

using namespace std;
using namespace boost;
using namespace llvm;

void AlgoMaxMiso::run(const DataFlowGraph &dfg, const Architecture &arch, ResultVector &result,
					  bool benchmark)
{
	if (dfg.num_vertices() == 0)
		return;
	m_benchmark = benchmark;
	DataFlowGraph::VertexList topo = dfg.reverseTopologicalSortL();
	BitVector invalid(topo.size());
	NumVector fanout(topo.size());
	BitVector processed(topo.size());
	// pre-processing step: check for invalid nodes & get fanout of all nodes
	NumVector fanout_org(topo.size());
	DataFlowGraph::DFG::vertex_iterator vIt, vItEnd;
	tie(vIt, vItEnd) = dfg.vertices();
	for (; vIt != vItEnd; ++vIt)
	{
		DataFlowGraph::Vertex v = *vIt;
		DataFlowGraph::operator_t type = dfg.getType(v);
		fanout_org[v] = dfg.out_degree(v) + 
			((type == DataFlowGraph::OUTPUT) ? 1 : 0);
		const llvm::Value* value = dfg.getValue(v);
		if (isa<Instruction>(value))
		{
			if (!arch.isValidInstruction(cast<Instruction>(value)) ||
				type == DataFlowGraph::INPUT)
				invalid[v] = true;
		}
	}
	PatternSet maxMisos;
	for (DataFlowGraph::VertexList::const_iterator it = topo.begin(); it != topo.end();)
	{
		fanout = fanout_org;
		const DataFlowGraph::Vertex first = *it;
		processed[first] = true;
		if (!invalid[first])
		{
			BitVector maxMiso(invalid.size());
			maxMiso[first] = true;
			unsigned int size = generateMaxMiso(first, dfg, maxMiso, 1, invalid,
				fanout, processed);
			if (size > 1 && !m_benchmark)
				maxMisos.insert(maxMiso);
		}
		// get next node in topo that has not been processed yet
		while (it != topo.end() && processed[*it])
			++it;
	}
	// return patterns
	for (PatternSet::const_iterator mm_it = maxMisos.begin(); mm_it != maxMisos.end(); ++mm_it)
	{
		result.push_back(*mm_it);
	}
}

unsigned int AlgoMaxMiso::generateMaxMiso(const DataFlowGraph::Vertex &v, const DataFlowGraph &dfg,
								  BitVector &result, unsigned int nodes,
								  const BitVector &invalid, NumVector &fanout,
								  BitVector &processed)
{
	// process all parent nodes
	DataFlowGraph::DFG::in_edge_iterator edgeIt, edgeItEnd;
	tie(edgeIt, edgeItEnd) = dfg.in_edges(v);
    for (; edgeIt != edgeItEnd; ++edgeIt)
	{
		// check if node is "nonlegal" or has "nonreconvergent fanout"
		DataFlowGraph::Vertex parent = dfg.source(*edgeIt);
		fanout[parent]--;
		if (!invalid[parent] && fanout[parent] == 0)
		{
			result[parent] = true;
			processed[parent] = true;
			nodes = generateMaxMiso(parent, dfg, result, nodes + 1,
				invalid, fanout, processed);
		}
	}
	return nodes;
}
