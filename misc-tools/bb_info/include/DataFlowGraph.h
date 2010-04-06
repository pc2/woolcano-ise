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
	VertexVector reverseTopologicalSort(void) const;
	std::pair<DFG::vertex_iterator, DFG::vertex_iterator> vertices(void) const { return boost::vertices(g); }
	Vertex source(const Edge &e) const { return boost::source(e, g); }
	Vertex target(const Edge &e) const { return boost::target(e, g); }
	const llvm::Value* getValue(const Vertex &v) const { return g[v].value; }
	operator_t getType(const Vertex &v) const { return g[v].op; }
	std::string writeGraphviz(bool outputCode = false, bool outputTopo = false) const;
	DataFlowGraph(const llvm::BasicBlock &bb);
private:
	void fromBasicBlock(const llvm::BasicBlock &bb);
	typedef std::map<const llvm::Value*, Vertex> VertexMap;
	boost::property_map<DFG, boost::vertex_index_t>::type index;
	unsigned int inputs, outputs;
	DFG g;
	VertexMap vertexMap;
};

#endif
