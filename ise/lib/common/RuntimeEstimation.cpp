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
#include <llvm/Instruction.h>
#include <llvm/Support/Casting.h>
#include <vector>
#include "RuntimeEstimation.h"
#include "Util.h"

using namespace llvm;
using namespace std;

/*
	estimate runtime on software platform by adding up instruction timings for all nodes
*/
unsigned int RuntimeEstimation::estimateSwRuntime(const DataFlowGraph &dfg, 
												  const Architecture &arch)
{
	unsigned int runtime = 0;
	DataFlowGraph::DFG::vertex_iterator vIt, vItEnd;
	tie(vIt, vItEnd) = dfg.vertices();
	for (; vIt != vItEnd; ++vIt)
	{
		DataFlowGraph::operator_t type = dfg.getType(*vIt);
		if (type == DataFlowGraph::OPERATOR || type == DataFlowGraph::OUTPUT)
		{
			const llvm::Value* value = dfg.getValue(*vIt);
			if (isa<Instruction>(value))
				runtime += arch.getSwInstructionTiming(cast<Instruction>(value));
		}
	}
	return runtime;
}

/*
	estimate runtime on hardware platform by adding up instruction timings
	along the DFG's critical path
*/
unsigned int RuntimeEstimation::estimateHwRuntime(const DataFlowGraph &dfg, 
												  const Architecture &arch)
{
	DataFlowGraph::VertexVector topo = dfg.reverseTopologicalSort();
	vector<unsigned int> dist(topo.size(), 0);
	unsigned int maxDist = 0;
	for (DataFlowGraph::VertexVector::const_iterator ii = topo.begin();
		ii != topo.end(); ++ii)
	{
		unsigned int d = 0;
		DataFlowGraph::DFG::out_edge_iterator edgeIt, edgeItEnd;
		tie(edgeIt, edgeItEnd) = dfg.out_edges(*ii);
		for (; edgeIt != edgeItEnd; ++edgeIt)
			d = _p_max<unsigned int>(dist[dfg.target(*edgeIt)], d);
		DataFlowGraph::operator_t type = dfg.getType(*ii);
		if (type == DataFlowGraph::OPERATOR || type == DataFlowGraph::OUTPUT)
		{
			const llvm::Value* value = dfg.getValue(*ii);
			if (isa<Instruction>(value))
				d += arch.getHwInstructionTiming(cast<Instruction>(value));
		}
		dist[*ii] = d;
		maxDist = _p_max<unsigned int>(maxDist, d);
	}
	return maxDist;
}
