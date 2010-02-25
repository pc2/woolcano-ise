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
#ifndef _DATAFLOWGRAPH_H_
#define _DATAFLOWGRAPH_H_

#include <string>
#include <map>
#include <vector>
#include <list>
#include <set>
#include <utility>
#include <algorithm>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <llvm/Value.h>
#include "Types.h"
#include "Architecture.h"

class DataFlowGraph
{
public:
	typedef enum { INPUT, OUTPUT, OPERATOR, CONSTANT } operator_t;

	typedef struct
	{
		operator_t			op;
		const llvm::Value*	value;
		double				distance;
	} DFG_V;

	typedef struct
	{
		std::string		name;
		double			weight;
	} DFG_E;

	typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
		DFG_V, DFG_E> DFG;
	typedef DFG::vertex_descriptor Vertex;
	typedef std::vector<Vertex> VertexVector;
	typedef std::list<Vertex> VertexList;
	typedef std::set<Vertex> VertexSet;
	typedef DFG::edge_descriptor Edge;

	void addNode(const llvm::Value *value, operator_t type);
	void addEdge(const llvm::Value *from, const llvm::Value *to, const std::string &name,
		double weight = 1.0);
	double getLongestPath(void);
	VertexVector reverseTopologicalSort(void) const;
	VertexList reverseTopologicalSortL(void) const;
	VertexVector in_vertices(const Vertex& v) const;
	VertexList in_verticesL(const Vertex& v) const;
	BitVector in_verticesBv(const Vertex& v) const;
	VertexVector out_vertices(const Vertex& v) const;
	VertexList out_verticesL(const Vertex& v) const;
	BitVector out_verticesBv(const Vertex& v) const;
	std::pair<DFG::in_edge_iterator, DFG::in_edge_iterator> in_edges(const Vertex& v) const { return boost::in_edges(v,g); }
	std::pair<DFG::out_edge_iterator, DFG::out_edge_iterator> out_edges(const Vertex& v) const { return boost::out_edges(v,g); }
	std::pair<DFG::vertex_iterator, DFG::vertex_iterator> vertices(void) const { return boost::vertices(g); }
	DFG::vertices_size_type num_vertices(void) const { return boost::num_vertices(g); }
	unsigned int num_inputs(void) const { return inputs; }
	unsigned int num_outputs(void) const { return outputs; }
	Vertex source(const Edge &e) const { return boost::source(e, g); }
	Vertex target(const Edge &e) const { return boost::target(e, g); }
	Vertex getVertex(const llvm::Value* value) const;
	Vertex null_vertex(void) const { return g.null_vertex(); }
	DFG::degree_size_type in_degree(const Vertex &v) const { return boost::in_degree(v,g); }
	DFG::degree_size_type out_degree(const Vertex &v) const { return boost::out_degree(v,g); }
	const llvm::Value* getValue(const Vertex &v) const { return g[v].value; }
	operator_t getType(const Vertex &v) const { return g[v].op; }
	void setType(const Vertex &v, operator_t type) { g[v].op = type; }
	double getWeight(const Edge &e) const { return g[e].weight; }
	std::string writeGraphviz(const BitVector &bv) const;
	std::string writeGraphviz(bool outputCode = false, bool outputTopo = false) const;
  std::string writeGraphviz2(bool outputCode = false, bool outputTopo = false,
      const BitVector &bv = 0, const Architecture &arch = 0, const std::string &dot_code = 0 ) const;
	void splitConstants(void);
	DataFlowGraph(const llvm::BasicBlock &bb);
	DataFlowGraph(const DataFlowGraph &dfg, const BitVector &bv);
private:
	void fromBitVector(const DataFlowGraph &dfg, const BitVector &bv);
	void fromBasicBlock(const llvm::BasicBlock &bb);
	typedef std::map<const llvm::Value*, Vertex> VertexMap;
	boost::property_map<DFG, boost::vertex_index_t>::type index;
	unsigned int inputs, outputs;
	DFG g;
	VertexMap vertexMap;
};

#endif
