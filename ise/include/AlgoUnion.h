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
#ifndef _ALGO_UNION_H_
#define _ALGO_UNION_H_

#include <vector>
#include <set>
#include <map>
#include "DataFlowGraph.h"
#include "Architecture.h"
#include "IseAlgorithm.h"
#include "Types.h"

class AlgoUnion : public IseAlgorithm
{
public:
	virtual void run(const DataFlowGraph &dfg, const Architecture &arch, 
		ResultVector &result, bool benchmark = false);
private:
	bool extCombEli(bool down, const BitVector &extComb,
		const BitVector &newExt, const DataFlowGraph &dfg);
	bool isValidNode(DataFlowGraph::Vertex v, const DataFlowGraph &dfg,
		const Architecture &arch);
	unsigned int defineRegions(const DataFlowGraph &dfg, const Architecture &arch);
	unsigned int defineRegions(DataFlowGraph::Vertex v, DataFlowGraph::VertexSet &nodes, 
		unsigned int region, unsigned int nodeCount, const DataFlowGraph &dfg, 
		const Architecture &arch);
	void enumerateUpwardCones(unsigned int region, const DataFlowGraph &dfg,
		const Architecture &arch);
	void enumerateDownwardCones(DataFlowGraph::Vertex v, const DataFlowGraph &dfg,
		const Architecture &arch);
	typedef std::vector<unsigned int> NumVector;
	typedef std::set<BitVector> PatternSet;
	typedef std::map<DataFlowGraph::Vertex, PatternSet> PatternMap;
	typedef std::map<DataFlowGraph::Vertex, BitVector> BitVectorMap;
	bool incrementBitVector(BitVector &bv);
	void joinPatternSets(PatternSet &ps1, const PatternSet &ps2);
	bool isConvex(const BitVector &p, const DataFlowGraph &dfg);
	bool inputCheck(const BitVector &p, const DataFlowGraph &dfg,
		const Architecture &arch);
	bool outputCheck(const BitVector &p, const DataFlowGraph &dfg, 
		const Architecture &arch);
	DataFlowGraph::VertexVector bitVectorToVertexVector(const BitVector &bv);
	BitVector getOutputNodes(const BitVector &bv, const DataFlowGraph &dfg);
	BitVector getInputNodes(const BitVector &bv, const DataFlowGraph &dfg);
	PatternSet Union(const PatternSet &core, const BitVector &ext, bool down,
		const DataFlowGraph &dfg, const Architecture &arch,
		BitVector &remExts);
	void checkInvalid(const DataFlowGraph &dfg, const Architecture &arch);
	void calcEcs(const DataFlowGraph &dfg);
	void removeExt(BitVector &ext, const DataFlowGraph &dfg, bool down,
		const BitVector &core);
	void getAllSuccs(DataFlowGraph::Vertex v, const DataFlowGraph &dfg, 
		BitVector &succs);
	void getAllPreds(DataFlowGraph::Vertex v, const DataFlowGraph &dfg, 
		BitVector &preds);
	// contains upward and downward cones
	PatternMap uc_set, dc_set;
	// contains the vertex indices in topological order
	DataFlowGraph::VertexVector topo;
	// regions of DFG, indexed by Vertex
	NumVector regions, orgRegions;
	BitVector invalid, exclude;
	BitVectorMap ecs, maxUcs, maxDcs;
};

#endif
