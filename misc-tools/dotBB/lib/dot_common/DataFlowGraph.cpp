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
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/graphviz.hpp>
#include <llvm/BasicBlock.h>
#include <llvm/Constant.h>
#include <llvm/Instructions.h>
#include <sstream>
#include <set>
#include "DataFlowGraph.h"
#include "Primitives.h"
#include "Util.h"

using namespace std;
using namespace llvm;



// returns list of vertices in reverse topological order
DataFlowGraph::VertexVector DataFlowGraph::reverseTopologicalSort(void) const
{
	VertexVector c;
	topological_sort(g, back_inserter(c));
	return c;
}

void DataFlowGraph::addNode(const llvm::Value* value, operator_t type)
{
	if (vertexMap.find(value) == vertexMap.end())
	{
		Vertex v = boost::add_vertex(g);
		g[v].value = value;
		g[v].op = type;
		g[v].distance = 0;
		vertexMap[value] = v;
		if (type == OUTPUT)
			outputs++;
		else if (type == INPUT)
			inputs++;
	}
}

void DataFlowGraph::addEdge(const llvm::Value *from, const llvm::Value *to,
							const string &name, double weight)
{
	VertexMap::const_iterator it_from = vertexMap.find(from);
	VertexMap::const_iterator it_to = vertexMap.find(to);
	assert(it_from != vertexMap.end() && it_to != vertexMap.end());
	bool inserted;
	Edge e;
	tie(e, inserted) = boost::add_edge(it_from->second, it_to->second, g);
	g[e].name = name;
	g[e].weight = weight;
}

void DataFlowGraph::fromBasicBlock(const llvm::BasicBlock &bb)
{
	const BasicBlock::InstListType &insts = bb.getInstList();
	typedef set<const llvm::Value*> ValueSet;
	ValueSet forcedOutputs;
	for (BasicBlock::InstListType::const_iterator i = insts.begin(); i != insts.end(); ++i)
	{
		if (isa<TerminatorInst>(*i))
			break;
		else if (isa<PHINode>(*i))
		{
			// phi node - add as input, do not add any of its arguments
			addNode(i, INPUT);
			// any arguments that appear later in the basic block will explicitly
			// be marked as output values
			for (User::const_op_iterator oi = i->op_begin(), oe = i->op_end(); oi != oe; ++oi)
			{
				const llvm::Value* value = oi->get();
				if (isa<Instruction>(value))
				{
					if (cast<Instruction>(value)->getParent() == &bb)
						forcedOutputs.insert(value);
				}
			}
		}
		else
		{
			operator_t type = OPERATOR;
			// output value?
			ValueSet::const_iterator it = forcedOutputs.find(i);
			if (it != forcedOutputs.end())
			{
				type = OUTPUT;
				forcedOutputs.erase(i);
			}
			else if (Primitives::isOutputForBasicBlock(i))
				type = OUTPUT;
			addNode(i, type);
			for (User::const_op_iterator oi = i->op_begin(), oe = i->op_end(); oi != oe; ++oi)
			{
				const llvm::Value* value = oi->get();
				if (isa<Instruction>(value))
				{
					if (cast<Instruction>(value)->getParent() != &bb)
					{
						addNode(value, INPUT);
					}
				}
				else if (isa<Argument>(value) || isa<GlobalValue>(value))
					addNode(value, INPUT);
				else if (isa<Constant>(oi->get()))
					addNode(value, CONSTANT);
				addEdge(value, i, "");
			}
		}
	}	
}

string DataFlowGraph::writeGraphviz(bool outputCode, bool outputTopo) const
{
	std::vector<unsigned int> topoVector;
	if (outputTopo)
	{
		VertexVector topo = reverseTopologicalSort();
		topoVector.resize(topo.size());
		for (unsigned int i = 0; i < topo.size(); i++)
			topoVector[topo[i]] = i;
	}
	string result = "digraph G {\n";
	// output all nodes
	DFG::vertex_iterator vIt, vItEnd;
	tie(vIt, vItEnd) = vertices();
    for (; vIt != vItEnd; ++vIt)
	{
		// label
		size_t idx = index[*vIt];
		result += "node_" + Util::stringify(idx) + " [ label = \"";
		const llvm::Value* value = g[*vIt].value;
		if (isa<Instruction>(value))
		{
			const Instruction* inst = cast<Instruction>(value);
			result += inst->getOpcodeName();
		}
		else if (isa<Constant>(value))
		{
			result += "constant";
		}
		else
			result += value->getName();
		if (outputTopo)
			result += "(" + Util::stringify(topoVector[*vIt]) + ")";
		result += "\" ";
		// shape and rank
		operator_t type = g[*vIt].op;
		switch (type)
		{
		case INPUT:
			result += "shape = \"invhouse\" rank = \"source\"";
			break;
		case OUTPUT:
			result += "shape = \"doublecircle\" rank = \"sink\"";
			break;
		case OPERATOR:
			result += "shape = \"circle\"";
			break;
		case CONSTANT:
			result += "shape = \"diamond\"";
			break;
		default:
			break;
		}
		result += "];\n";
		if (outputCode)
		{
			std::ostringstream os;
			result += "/* ";
			os << *value << "*/\n";
			result += os.str();
		}
	}
	// output all edges
	DFG::edge_iterator eIt, eItEnd;
	tie(eIt, eItEnd) = boost::edges(g);
    for (; eIt != eItEnd; ++eIt)
	{
		Vertex source = boost::source(*eIt, g);
		size_t idxS = index[source];
		Vertex target = boost::target(*eIt, g);
		size_t idxT = index[target];
		result += "node_" + Util::stringify(idxS);
		result += " -> ";
		result += "node_" + Util::stringify(idxT);
		result += " [ label = \"";
		const llvm::Value* value = g[source].value;
		result += value->getType()->getDescription();
		result += "\" ];\n";
	}
	result += "}\n";
#if 0
	if (outputCode)
	{
		result += "/*\n";
		VertexVector topo = topologicalSort();
		std::ostringstream os;
		for (VertexVector::const_reverse_iterator topoIt = topo.rbegin(); topoIt != topo.rend();
			++topoIt)
		{
			if (g[*topoIt].op != INPUT)
			{
				const llvm::Value* value = g[*topoIt].value;
				os << *value << "\n";
			}
		}
		result += os.str();
		result += "*/\n";
	}
#endif
	return result;
}

DataFlowGraph::DataFlowGraph(const llvm::BasicBlock &bb) : inputs(0), outputs(0)
{
	fromBasicBlock(bb);
}
