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

DataFlowGraph::VertexVector DataFlowGraph::in_vertices(const Vertex& v) const
{
	VertexVector vertices;
	DataFlowGraph::DFG::in_edge_iterator inEdgeIt, inEdgeItEnd;
	tie(inEdgeIt, inEdgeItEnd) = boost::in_edges(v,g);
	for (; inEdgeIt != inEdgeItEnd; ++inEdgeIt)
		vertices.push_back(boost::source(*inEdgeIt, g));
	return vertices;
}

DataFlowGraph::VertexList DataFlowGraph::in_verticesL(const Vertex& v) const
{
	VertexList vertices;
	DataFlowGraph::DFG::in_edge_iterator inEdgeIt, inEdgeItEnd;
	tie(inEdgeIt, inEdgeItEnd) = boost::in_edges(v,g);
	for (; inEdgeIt != inEdgeItEnd; ++inEdgeIt)
		vertices.push_back(boost::source(*inEdgeIt, g));
	return vertices;
}

BitVector DataFlowGraph::in_verticesBv(const Vertex& v) const
{
	BitVector vertices(boost::num_vertices(g));
	DataFlowGraph::DFG::in_edge_iterator inEdgeIt, inEdgeItEnd;
	tie(inEdgeIt, inEdgeItEnd) = boost::in_edges(v,g);
	for (; inEdgeIt != inEdgeItEnd; ++inEdgeIt)
		vertices[boost::source(*inEdgeIt, g)] = true;
	return vertices;
}

BitVector DataFlowGraph::out_verticesBv(const Vertex& v) const
{
	BitVector vertices(boost::num_vertices(g));
	DataFlowGraph::DFG::out_edge_iterator outEdgeIt, outEdgeItEnd;
	tie(outEdgeIt, outEdgeItEnd) = boost::out_edges(v,g);
	for (; outEdgeIt != outEdgeItEnd; ++outEdgeIt)
		vertices[boost::target(*outEdgeIt, g)] = true;
	return vertices;
}

DataFlowGraph::VertexVector DataFlowGraph::out_vertices(const Vertex& v) const
{
	VertexVector vertices;
	DataFlowGraph::DFG::out_edge_iterator outEdgeIt, outEdgeItEnd;
	tie(outEdgeIt, outEdgeItEnd) = boost::out_edges(v,g);
	for (; outEdgeIt != outEdgeItEnd; ++outEdgeIt)
		vertices.push_back(boost::target(*outEdgeIt, g));
	return vertices;
}

DataFlowGraph::VertexList DataFlowGraph::out_verticesL(const Vertex& v) const
{
	VertexList vertices;
	DataFlowGraph::DFG::out_edge_iterator outEdgeIt, outEdgeItEnd;
	tie(outEdgeIt, outEdgeItEnd) = boost::out_edges(v,g);
	for (; outEdgeIt != outEdgeItEnd; ++outEdgeIt)
		vertices.push_back(boost::target(*outEdgeIt, g));
	return vertices;
}

// returns list of vertices in reverse topological order
DataFlowGraph::VertexVector DataFlowGraph::reverseTopologicalSort(void) const
{
	VertexVector c;
	topological_sort(g, back_inserter(c));
	return c;
}

// returns list of vertices in reverse topological order
DataFlowGraph::VertexList DataFlowGraph::reverseTopologicalSortL(void) const
{
	VertexList c;
	topological_sort(g, back_inserter(c));
	return c;
}

double DataFlowGraph::getLongestPath(void)
{
	VertexVector c = reverseTopologicalSort();
	//std::cout << "A topological ordering: ";
	double maxDist = 0.0;
	for (VertexVector::const_iterator ii = c.begin(); ii != c.end(); ++ii)
	{
		//std::cout << *ii << " ";
		//std::cout << "Node " << *ii << " ";
		//std::cout << *g[*ii].value << std::endl;
		DFG::out_edge_iterator edgeIt, edgeItEnd;
		tie(edgeIt, edgeItEnd) = boost::out_edges(*ii, g);
        for (; edgeIt != edgeItEnd; ++edgeIt)
		{
			Vertex t = target(*edgeIt);
			double d = g[t].distance + g[*edgeIt].weight;
			if (g[*ii].distance < d)
				g[*ii].distance = d;
			maxDist = _p_max<double>(maxDist, d);
		}
	}
	//std::cout << std::endl;
	//std::cout << "Length of critical path: " << maxDist << std::endl;
	return maxDist;
}

DataFlowGraph::Vertex DataFlowGraph::getVertex(const llvm::Value* value) const
{
	VertexMap::const_iterator it = vertexMap.find(value);
	assert(it != vertexMap.end());
	return it->second;
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

/*
	creates additional nodes for any constants that are used more than once
	side-effect: lookup through VertexMap will only return one node per value
*/
void DataFlowGraph::splitConstants(void)
{
	DFG::vertex_iterator vIt, vItEnd;
	tie(vIt, vItEnd) = vertices();
    for (; vIt != vItEnd; ++vIt)
	{
		if (g[*vIt].op == CONSTANT)
		{
			// replace all but the first edge
			Vertex v = *vIt;
			vector<Edge> deleteList;
			vector<Vertex> addList;
			DFG::out_edge_iterator edgeIt, edgeItEnd;
			tie(edgeIt, edgeItEnd) = boost::out_edges(*vIt, g);
	        for (++edgeIt; edgeIt != edgeItEnd; ++edgeIt)
			{
				addList.push_back(boost::target(*edgeIt, g));
				deleteList.push_back(*edgeIt);
			}
			// add new edges and vertices and remove old edges
			// (modifying the graph invalidates edge iterators...)
			for (unsigned int i = 0; i < addList.size(); ++i)
			{
				Vertex vNew = boost::add_vertex(g);
				g[vNew].value = g[v].value;
				g[vNew].op = g[v].op;
				g[vNew].distance = g[v].distance;
				bool inserted;
				Edge eNew;
				tie(eNew, inserted) = boost::add_edge(vNew, addList[i], g);
				const DFG_E &e = g[deleteList[i]];
				g[eNew].name = e.name;
				g[eNew].weight = e.weight;
				remove_edge(deleteList[i], g);
			}
		}
	}
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
	string result = "digraph G {\nsize = \"8.3,11.7\"; //a4 size\n";
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
			const Constant* c = cast<Constant>(value);
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

string DataFlowGraph::writeGraphviz2(bool outputCode, bool outputTopo, const BitVector &bv, const Architecture &arch, const std::string &dot_code) const
{
  // annotate graph with topoological order
	std::vector<unsigned int> topoVector;
	if (outputTopo)
	{
		VertexVector topo = reverseTopologicalSort();
		topoVector.resize(topo.size());
		for (unsigned int i = 0; i < topo.size(); i++)
			topoVector[topo[i]] = i;
	}
	string result = "digraph G {\n";
//	result += "size = \"8.3,11.7\"; //a4 size\n";

  if (! dot_code.empty()) 
    result += dot_code;

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
			const Constant* c = cast<Constant>(value);
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

		// change colors for nodes located in bv
    if (bv[*vIt]) {
      // when it is constant than mark is as template input
      if (isa<Constant>(value)) {
        result += " shape = \"invhouse\" rank = \"source\" style = \"filled\" \
                   bgcolor = \"black\" color = \"lightblue\" " ;
      } else {
        // mark nodes which belong to template
         result += " style = \"filled\" color = \"lightblue\" ";
      }
    }

    result += "];\n";
		if (outputCode)
		{
			std::ostringstream os;
			result += "/* ";
//			os << *value << "*/\n";
			os << *value; 
      std::string t = os.str();
      if (t[t.size()] = '\n')
        t.resize(t.size()-1);
			result += t + " */\n";
		}
	}

	// output all edges
	DFG::edge_iterator eIt, eItEnd;
	tie(eIt, eItEnd) = boost::edges(g);
  std::ostringstream mark_src_nodes;
  for (; eIt != eItEnd; ++eIt)
	{
		Vertex source = boost::source(*eIt, g);
		size_t idxS = index[source];
		Vertex target = boost::target(*eIt, g);
		size_t idxT = index[target];
		result += "node_" + Util::stringify(idxS);
		result += " -> ";
		result += "node_" + Util::stringify(idxT);

		result += " [ fontsize = 10 label = \"";
		const llvm::Value* value = g[source].value;
    if (isa<Instruction>(value)) {
      const llvm::Instruction *inst = cast<Instruction>(value);
      result += "sw:" +Util::stringify(arch.getSwInstructionTiming(inst)) + "\\n";
    } else if (isa<Constant>(value)) {
      result += "sw: 0\\n";
    }
		result += value->getType()->getDescription() + "\" ";

    // mark all edges going to bv
    if (bv[idxT] || bv[idxS]  ) {

      // when source node is not in bv than mark is as input to the template
      if (! bv[idxS]) {
        mark_src_nodes << "node_" << idxS << " [ shape = \"invhouse\" rank = \"source\" style = \"filled\" bgcolor = \"lightblue\" ];\n";
        result += " penwidth=2 color = \"gray\" label = \"comm\"";
      } else {
        if (isa<Constant>(value)) {
          result += " color = \"gray\" label = \"\"";
        } else {
          // edge counted in estimation
          result += " fontcolor = \"blue\" color = \"lightblue\" " ;
        }
      }
    }
		result += " ];\n";
	}

  // add marked nodes
  if (mark_src_nodes)
    result += "\n\n/* Additional source nodes */\n" + mark_src_nodes.str();

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
std::string DataFlowGraph::writeGraphviz(const BitVector &bv) const
{
  // create base dfg graph and remove from it the last line with '}' 
  std::string base_dfg = writeGraphviz(false,true);
  base_dfg.resize(base_dfg.size() - 2 );

  BitVector::size_type i;
  for (i = bv.find_first(); i != BitVector::npos; i = bv.find_next(i)) 
  {
    base_dfg += "node_" + Util::stringify(index[i]) + " [ color = \"lightblue\";\n";
//    cout << idx << getValue[i].getName() << "\n"; 
  }
  base_dfg += "}\n";

  return base_dfg;
}
/*
    creates a DFG from a subgraph defined by a bitvector
*/
void DataFlowGraph::fromBitVector(const DataFlowGraph &dfg, const BitVector &bv)
{
    // add all nodes first
	for (BitVector::size_type i = bv.find_first();
		i != BitVector::npos; i = bv.find_next(i))
	{
		addNode(dfg.getValue(i), dfg.getType(i));
	}
    // add edges
	for (BitVector::size_type i = bv.find_first();
		i != BitVector::npos; i = bv.find_next(i))
	{
    // outgoing edges first, set output nodes accordingly
		DataFlowGraph::DFG::out_edge_iterator edgeIt, edgeItEnd;
		tie(edgeIt, edgeItEnd) = dfg.out_edges(i);
		operator_t type = dfg.getType(i);
		for (; edgeIt != edgeItEnd; ++edgeIt)
		{
			DataFlowGraph::Vertex target = dfg.target(*edgeIt);
			if (bv[target])
			{
				addEdge(dfg.getValue(i), dfg.getValue(target), "",
					dfg.getWeight(*edgeIt));
			}
			else if (type != CONSTANT && type != OUTPUT)
			{
				setType(getVertex(dfg.getValue(i)), OUTPUT);
				outputs++;
				type = OUTPUT;
			}
		}
        // process incoming edges to add input nodes
		DataFlowGraph::DFG::in_edge_iterator inEdgeIt, inEdgeItEnd;
		tie(inEdgeIt, inEdgeItEnd) = dfg.in_edges(i);
		for (; inEdgeIt != inEdgeItEnd; ++inEdgeIt)
		{
			DataFlowGraph::Vertex source = dfg.source(*inEdgeIt);
			if (!bv[source])
			{
				const llvm::Value* vSource = dfg.getValue(source);
				addNode(vSource, INPUT);
				addEdge(vSource, dfg.getValue(i), "",
					dfg.getWeight(*inEdgeIt));
			}
		}
	}
}

DataFlowGraph::DataFlowGraph(const llvm::BasicBlock &bb) : inputs(0), outputs(0)
{
	fromBasicBlock(bb);
}

DataFlowGraph::DataFlowGraph(const DataFlowGraph &dfg, const BitVector &bv) : inputs(0), outputs(0)
{
	fromBitVector(dfg, bv);
}
