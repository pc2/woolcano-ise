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
	Union Algorithm
	"Scalabe Custom Instructions Identification for Instruction-Set Extensible Processors"
*/

#include <algorithm>
#include "llvm/Support/Casting.h"
#include "AlgoUnion.h"
#include "Primitives.h"

using namespace std;
using namespace boost;
using namespace llvm;


void AlgoUnion::enumerateDownwardCones(DataFlowGraph::Vertex v, const DataFlowGraph &dfg,
									   const Architecture &arch)
{
	unsigned int region = orgRegions[v];
	// recursively process all immediate successors
	DataFlowGraph::DFG::out_edge_iterator edgeIt, edgeItEnd;
	tie(edgeIt, edgeItEnd) = dfg.out_edges(v);
	for (; edgeIt != edgeItEnd; ++edgeIt)
	{
		// only process nodes in current region
		DataFlowGraph::Vertex target = dfg.target(*edgeIt);
		if (orgRegions[target] != region)
			continue;
		// only process nodes that don't have a dc_set entry yet
		if (dc_set.find(target) == dc_set.end())
			enumerateDownwardCones(target, dfg, arch);
	}
	// dc_set(v) = {v}
	BitVector bv_v(topo.size());
	bv_v[v] = true;
	PatternSet ps_v;
	ps_v.insert(bv_v);
	pair<PatternMap::iterator, bool> pm_v_it = dc_set.insert(make_pair(v, ps_v));
	BitVectorMap::iterator ecs_it = ecs.find(v);
	BitVector maxDc = bv_v;
	// for all combinations of successor nodes
	tie(edgeIt, edgeItEnd) = dfg.out_edges(v);
	BitVector bv_succs(topo.size());
	// get successors
	for (; edgeIt != edgeItEnd; ++edgeIt)
	{
		DataFlowGraph::Vertex target = dfg.target(*edgeIt);
		if (orgRegions[target] == region && !exclude[target])
			bv_succs[target] = true;
	}
	// eliminate successors of successors
	for (BitVector::size_type i = bv_succs.find_first();
		i != BitVector::npos; i = bv_succs.find_next(i))
	{
		DataFlowGraph::DFG::out_edge_iterator outEdgeIt, outEdgeItEnd;
		tie(outEdgeIt, outEdgeItEnd) = dfg.out_edges(i);
		for (; outEdgeIt != outEdgeItEnd; ++outEdgeIt)
			bv_succs[dfg.target(*outEdgeIt)] = false;
	}
	DataFlowGraph::VertexList succ;
	for (BitVector::size_type i = bv_succs.find_first();
		i != BitVector::npos; i = bv_succs.find_next(i))
	{
		succ.push_back(i);
	}
	unsigned int succs = succ.size();
	if (succs > 0)
	{
		unsigned int comb = (1 << succs) - 1;
		for (; comb != 0; --comb)
		{
			DataFlowGraph::VertexVector selected_succ;
			DataFlowGraph::VertexList::iterator s_it = succ.begin();
			for (unsigned int i = 0; i < succs; ++i, ++s_it)
			{
				if (comb & (1 << i))
					selected_succ.push_back(*s_it);
			}
			// for all combinations of DCs of selected successor nodes
			vector<PatternSet::const_iterator> dc_its(selected_succ.size());
			vector<PatternSet::const_iterator> dc_end_its(selected_succ.size());
			unsigned int dcs = 1;
			for (unsigned int i = 0; i < selected_succ.size(); ++i)
			{
				PatternMap::const_iterator pm_it = dc_set.find(selected_succ[i]);
				dc_its[i] = pm_it->second.begin();
				dc_end_its[i] = pm_it->second.end();
				dcs *= pm_it->second.size();
			}
			vector<PatternSet::const_iterator> dc_its_cur = dc_its;
			for (; dcs != 0; --dcs)
			{
				for (unsigned int i = 0; i < dc_its_cur.size(); ++i)
				{
					dc_its_cur[i]++;
					if (dc_its_cur[i] != dc_end_its[i])
						break;
					dc_its_cur[i] = dc_its[i];
				}
				// combine and add v
				BitVector cone = bv_v;
				for (vector<PatternSet::const_iterator>::const_iterator it = dc_its_cur.begin();
					it != dc_its_cur.end(); ++it)
					cone |= **it;
				// check external conflicting set
				if (ecs_it != ecs.end())
				{
					if (ecs_it->second.intersects(cone))
						continue;
				}
				// add result to dc_set(v) if convex and output_check
				if (isConvex(cone, dfg) && outputCheck(cone, dfg, arch))
				{
					pm_v_it.first->second.insert(cone);
					maxDc |= cone;
				}
			}
		}
	}
	maxDcs.insert(make_pair(v, maxDc));
}


/*
	enumerates upward cones for a region R
*/
void AlgoUnion::enumerateUpwardCones(unsigned int region, const DataFlowGraph &dfg,
									 const Architecture &arch)
{
	//	for all v of R in topologically sorted order
	const BitVector bv_empty(topo.size());
	for (DataFlowGraph::VertexVector::reverse_iterator it = topo.rbegin();
		it != topo.rend(); ++it)
	{
		if (regions[*it] != region) continue;
		//cout << "processing vertex " << *it << "\n";
		// uc_set(v) = {v}
		BitVector bv_v(topo.size());
		bv_v[*it] = true;
		PatternSet ps_v;
		ps_v.insert(bv_v);
		pair<PatternMap::iterator, bool> pm_v_it = uc_set.insert(make_pair(*it, ps_v));
		BitVectorMap::const_iterator ecs_it = ecs.find(*it);
		const BitVector& ecs_bv = (ecs_it != ecs.end()) ? ecs_it->second : bv_empty;
		BitVector maxUc = bv_v;
		// for all combinations of predecessor nodes
		DataFlowGraph::DFG::in_edge_iterator inEdgeIt, inEdgeItEnd;
		tie(inEdgeIt, inEdgeItEnd) = dfg.in_edges(*it);
		BitVector bv_preds(topo.size());
		// get predecessors
		for (; inEdgeIt != inEdgeItEnd; ++inEdgeIt)
		{
			DataFlowGraph::Vertex source = dfg.source(*inEdgeIt);
			if (regions[source] == region && !ecs_bv[source])
				bv_preds[source] = true;
		}
		// eliminate predecessors of predecessors
		for (BitVector::size_type i = bv_preds.find_first();
			i != BitVector::npos; i = bv_preds.find_next(i))
		{
			DataFlowGraph::DFG::in_edge_iterator inEdgeIt, inEdgeItEnd;
			tie(inEdgeIt, inEdgeItEnd) = dfg.in_edges(i);
			for (; inEdgeIt != inEdgeItEnd; ++inEdgeIt)
				bv_preds[dfg.source(*inEdgeIt)] = false;
		}
		DataFlowGraph::VertexList pred;
		for (BitVector::size_type i = bv_preds.find_first();
			i != BitVector::npos; i = bv_preds.find_next(i))
		{
			pred.push_back(i);
		}
		unsigned int preds = pred.size();
		if (preds > 0)
		{
			assert(preds < 32);
			for (unsigned int comb = (1 << preds) - 1; comb != 0; --comb)
			{
				DataFlowGraph::VertexVector selected_pred;
				DataFlowGraph::VertexList::const_iterator p_it = pred.begin(); 
				for (unsigned int i = 0; i < preds; ++i, ++p_it)
				{
					if (comb & (1 << i))
						selected_pred.push_back(*p_it);
				}
				// for all combinations of UCs of selected predecessor nodes
				vector<PatternSet::const_iterator> uc_its(selected_pred.size());
				vector<PatternSet::const_iterator> uc_end_its(selected_pred.size());
				unsigned int ucs = 1;
				for (unsigned int i = 0; i < selected_pred.size(); ++i)
				{
					PatternMap::const_iterator pm_it = uc_set.find(selected_pred[i]);
					uc_its[i] = pm_it->second.begin();
					uc_end_its[i] = pm_it->second.end();
					ucs *= pm_it->second.size();
				}
				//cout << "ucs: " << ucs << "\n";
				vector<PatternSet::const_iterator> uc_its_cur = uc_its;
				for (; ucs != 0; --ucs)
				{
					for (unsigned int i = 0; i < uc_its_cur.size(); ++i)
					{
						uc_its_cur[i]++;
						if (uc_its_cur[i] != uc_end_its[i])
							break;
						uc_its_cur[i] = uc_its[i];
					}
					// combine and add v
					BitVector cone = bv_v;
					for (vector<PatternSet::const_iterator>::const_iterator it = uc_its_cur.begin();
						it != uc_its_cur.end(); ++it)
						cone |= **it;
					// check external conflicting set
					if (ecs_bv.intersects(cone))
						continue;
					// add result to uc_set(v) if convex and input_check
					if (isConvex(cone, dfg) && inputCheck(cone, dfg, arch))
					{
						pm_v_it.first->second.insert(cone);
						maxUc |= cone;
					}
				}
			}
		}
		maxUcs.insert(make_pair(*it, maxUc));
	}
}

bool AlgoUnion::inputCheck(const BitVector &p, const DataFlowGraph &dfg, 
						   const Architecture &arch)
{
	BitVector inputs(p.size());
	unsigned int maxInputs = arch.getMaxInputs();
	unsigned int count = 0;
	for (BitVector::size_type i = p.find_first();
		i != BitVector::npos; i = p.find_next(i))
	{
		DataFlowGraph::DFG::in_edge_iterator edgeIt, edgeItEnd;
		tie(edgeIt, edgeItEnd) = dfg.in_edges(i);
		for (; edgeIt != edgeItEnd; ++edgeIt)
		{
			DataFlowGraph::Vertex source = dfg.source(*edgeIt);
			if (!p[source] && !inputs[source])
			{
				count++;
				if (count > maxInputs)
					return false;
				inputs[source] = true;
			}
		}
	}
	return true;
}

bool AlgoUnion::outputCheck(const BitVector &p, const DataFlowGraph &dfg, 
							const Architecture &arch)
{
	unsigned int outputs = 0;
	unsigned int maxOutputs = arch.getMaxOutputs();
	for (BitVector::size_type i = p.find_first();
		i != BitVector::npos; i = p.find_next(i))
	{
		if (dfg.getType(i) == DataFlowGraph::OUTPUT)
		{
			outputs++;
			if (outputs > maxOutputs)
				return false;
		}
		else
		{
			DataFlowGraph::DFG::out_edge_iterator edgeIt, edgeItEnd;
			tie(edgeIt, edgeItEnd) = dfg.out_edges(i);
			for (; edgeIt != edgeItEnd; ++edgeIt)
			{
				if (!p[dfg.target(*edgeIt)])
				{
					outputs++;
					if (outputs > maxOutputs)
						return false;
					break;
				}
			}
		}
	}
	return true;
}

bool AlgoUnion::isConvex(const BitVector &p, const DataFlowGraph &dfg)
{
	for (BitVector::size_type i = p.find_first();
		i != BitVector::npos; i = p.find_next(i))
	{
		DataFlowGraph::DFG::out_edge_iterator edgeIt, edgeItEnd;
		tie(edgeIt, edgeItEnd) = dfg.out_edges(i);
		for (; edgeIt != edgeItEnd; ++edgeIt)
		{
			DataFlowGraph::Vertex target = dfg.target(*edgeIt);
			if (!p[target])
			{
				DataFlowGraph::DFG::out_edge_iterator edgeIt, edgeItEnd;
				tie(edgeIt, edgeItEnd) = dfg.out_edges(target);
				for (; edgeIt != edgeItEnd; ++edgeIt)
				{
					if (p[dfg.target(*edgeIt)])
						return false;
				}
			}
		}
	}
	return true;
}

void AlgoUnion::joinPatternSets(PatternSet &ps1, const PatternSet &ps2)
{
	ps1.insert(ps2.begin(), ps2.end());
}

bool AlgoUnion::isValidNode(DataFlowGraph::Vertex v, const DataFlowGraph &dfg,
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

unsigned int AlgoUnion::defineRegions(DataFlowGraph::Vertex v, DataFlowGraph::VertexSet &nodes, 
							  unsigned int region, unsigned int nodeCount,
							  const DataFlowGraph &dfg, const Architecture &arch)
{
	// add node to current region if it's valid
	nodes.erase(v);
	if (invalid[v])
		return nodeCount;
	regions[v] = region;
	nodeCount++;
	// visit nodes on outgoing edges
	DataFlowGraph::DFG::out_edge_iterator edgeIt, edgeItEnd;
	tie(edgeIt, edgeItEnd) = dfg.out_edges(v);
	for (; edgeIt != edgeItEnd; ++edgeIt)
	{
		DataFlowGraph::Vertex target = dfg.target(*edgeIt);
		if (nodes.find(target) != nodes.end())
			nodeCount += defineRegions(target, nodes, region, nodeCount, dfg, arch);
	}
	// visit nodes on incoming edges
	DataFlowGraph::DFG::in_edge_iterator inEdgeIt, inEdgeItEnd;
	tie(inEdgeIt, inEdgeItEnd) = dfg.in_edges(v);
	for (; inEdgeIt != inEdgeItEnd; ++inEdgeIt)
	{
		DataFlowGraph::Vertex source = dfg.source(*inEdgeIt);
		if (nodes.find(source) != nodes.end())
			nodeCount += defineRegions(source, nodes, region, nodeCount, dfg, arch);
	}
	return nodeCount;
}


unsigned int AlgoUnion::defineRegions(const DataFlowGraph &dfg, const Architecture &arch)
{
	DataFlowGraph::VertexSet nodes;
	copy(topo.begin(), topo.end(), inserter(nodes, nodes.end()));
    regions.clear();
	regions.resize(topo.size(), 0);
	unsigned int region = 1;
	while (!nodes.empty())
	{
		if (defineRegions(*nodes.begin(), nodes, region, 0, dfg, arch) > 0)
			region++;
	}
	return region - 1;
}

DataFlowGraph::VertexVector AlgoUnion::bitVectorToVertexVector(const BitVector &bv)
{
	DataFlowGraph::VertexVector vv;
	for (BitVector::size_type i = bv.find_first();
		i != BitVector::npos; i = bv.find_next(i))
	{
		vv.push_back(i);
	}
	return vv;
}

BitVector AlgoUnion::getOutputNodes(const BitVector &bv, const DataFlowGraph &dfg)
{
	BitVector out(bv.size());
	for (BitVector::size_type i = bv.find_first();
		i != BitVector::npos; i = bv.find_next(i))
	{
		if (dfg.getType(i) == DataFlowGraph::OUTPUT)
			out[i] = true;
		else
		{
			DataFlowGraph::DFG::out_edge_iterator edgeIt, edgeItEnd;
			tie(edgeIt, edgeItEnd) = dfg.out_edges(i);
			for (; edgeIt != edgeItEnd; ++edgeIt)
			{
				if (!bv[dfg.target(*edgeIt)])
				{
					out[i] = true;
					break;
				}
			}
		}
	}
	return out;
}

BitVector AlgoUnion::getInputNodes(const BitVector &bv, const DataFlowGraph &dfg)
{
	BitVector in(bv.size());
	for (BitVector::size_type i = bv.find_first();
		i != BitVector::npos; i = bv.find_next(i))
	{
		DataFlowGraph::DFG::in_edge_iterator edgeIt, edgeItEnd;
		tie(edgeIt, edgeItEnd) = dfg.in_edges(i);
		for (; edgeIt != edgeItEnd; ++edgeIt)
		{
			if (!bv[dfg.source(*edgeIt)])
			{
				in[i] = true;
				break;
			}
		}
	}
	return in;
}

bool AlgoUnion::incrementBitVector(BitVector &bv)
{
	unsigned int size = bv.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		bool c = bv[i];
		bv[i] = !c;
		if (!c)
			return true;
	}
	return false;
}

/*
	eliminates redundant combinations of extension nodes
*/
bool AlgoUnion::extCombEli(bool down, const BitVector &extComb,
	const BitVector &newExt, const DataFlowGraph &dfg)
{
	for (BitVector::size_type v = newExt.find_first();
		v != BitVector::npos; v = newExt.find_next(v))
	{
		DataFlowGraph::DFG::in_edge_iterator inEdgeIt, inEdgeItEnd;
		tie(inEdgeIt, inEdgeItEnd) = dfg.in_edges(v);
		for (; inEdgeIt != inEdgeItEnd; ++inEdgeIt)
		{
			DataFlowGraph::Vertex u = dfg.source(*inEdgeIt);
			if (newExt[u])
			{
				if (down)
				{
					if (extComb[u] && !extComb[v])
						return true;
				}
				else
				{
					if (extComb[v] && !extComb[u])
						return true;
				}
			}
		}
	}
	return false;
}

AlgoUnion::PatternSet AlgoUnion::Union(const PatternSet &core, const BitVector &ext, 
									   bool down, const DataFlowGraph &dfg, const Architecture &arch,
									   BitVector &remExts)
{
	PatternSet new_core = core;
	remExts -= ext;
	// for all combinations of ext
	DataFlowGraph::VertexVector extv = bitVectorToVertexVector(ext);
	unsigned int exts = extv.size();
	BitVector comb(exts);
	//cout << "trying 2^" << exts << " combinations of extension nodes\n";
	while (incrementBitVector(comb))
	{
		DataFlowGraph::VertexVector selected_ext;
		selected_ext.reserve(exts);
		BitVector selExtEcs(ext.size()), other_ext(ext.size()), selected_ext_bv(ext.size());
		for (unsigned int i = 0; i < exts; ++i)
			if (comb[i])
			{
				DataFlowGraph::Vertex v = extv[i];
				selected_ext.push_back(v);
				selected_ext_bv[v] = true;
				BitVectorMap::const_iterator ecs_it = ecs.find(v);
				if (ecs_it != ecs.end())
					selExtEcs |= ecs_it->second;
			}
			else
				other_ext[extv[i]] = true;
		if (extCombEli(down, selected_ext_bv, ext, dfg))
			continue;
		// select patterns p from core s.t.
		// p contains all extension points in combination
		// p does not contain any other extension points (i.e. EPs not in comb.)
		PatternSet P;
		for (PatternSet::const_iterator it = core.begin(); it != core.end(); ++it)
		{
			if (selected_ext_bv.is_subset_of(*it) && !it->intersects(other_ext))
				P.insert(*it);
		}
		if (P.size() == 0)
			continue;
		// calculate cross product of all DCs/UCs of EPs in comb. and P
		PatternMap &c_set = down ? dc_set : uc_set;
		vector<PatternSet::const_iterator> c_its(selected_ext.size() + 1);
		vector<PatternSet::const_iterator> c_end_its(selected_ext.size() + 1);
		c_its[0] = P.begin();
		c_end_its[0] = P.end();
		long long cs = P.size();
		//cerr << "selected_ext.size(): " << selected_ext.size() << "\n";
		for (unsigned int i = 0; i < selected_ext.size(); ++i)
		{
			PatternMap::const_iterator pm_it = c_set.find(selected_ext[i]);
			if (down && pm_it == c_set.end())
			{
				enumerateDownwardCones(selected_ext[i], dfg, arch);
				pm_it = c_set.find(selected_ext[i]);
			}
			c_its[i + 1] = pm_it->second.begin();
			c_end_its[i + 1] = pm_it->second.end();
			cs *= pm_it->second.size();
		}
		//cerr << "cs: " << cs << "\n";
		vector<PatternSet::const_iterator> c_its_cur = c_its;
		PatternSet tmp_core;
		for (; cs != 0; --cs)
		{
			for (unsigned int i = 0; i < c_its_cur.size(); ++i)
			{
				c_its_cur[i]++;
				if (c_its_cur[i] != c_end_its[i])
					break;
				c_its_cur[i] = c_its[i];
			}
			// combine
			BitVector t = *c_its_cur[0];
			for (unsigned int i = 1; i < c_its_cur.size(); ++i)
				t |= *c_its_cur[i];
			// check if pattern contains conflicting nodes
			selExtEcs |= exclude;
			if (!t.intersects(selExtEcs))
			{
				// for each pat in tmp
				//cout << "tmp.size(): " << tmp.size() << "\n";
				if (down)
				{
					if (!outputCheck(t, dfg, arch))
						continue;
				}
				else
				{
					if (!inputCheck(t, dfg, arch))
						continue;
				}
				if (!isConvex(t, dfg))
					continue;
				tmp_core.insert(t);
			}
		}
		//cout << "tmp_core.size(): " << tmp_core.size() << "\n";
		// get extension points of tmp
		BitVector tmp_ext(ext.size());
		if (down)
		{
			for (unsigned int i = 0; i < selected_ext.size(); ++i)
				tmp_ext |= getInputNodes(maxDcs.find(selected_ext[i])->second, dfg);
		}
		else
		{
			for (unsigned int i = 0; i < selected_ext.size(); ++i)
				tmp_ext |= getOutputNodes(maxUcs.find(selected_ext[i])->second, dfg);
		}
		// eliminate extension points that cannot generate new patterns
		// tmp_ext = REMOVE_EXT(tmp_ext CAP {vertices present in tmp_core})
		BitVector nc_v(ext.size());
		for (PatternSet::iterator it = tmp_core.begin(); it != tmp_core.end(); ++it)
			nc_v |= *it;
		tmp_ext &= nc_v & remExts;
		removeExt(tmp_ext, dfg, down, nc_v);
		//cout << "tmp_ext size " << tmp_ext.count() << "\n";

		if (!tmp_ext.none())
			joinPatternSets(new_core, Union(tmp_core, tmp_ext, !down, dfg, arch, remExts));
		else
			joinPatternSets(new_core, tmp_core);
	}
	// prune new_core for input and output constraints
	for (PatternSet::iterator it = new_core.begin(); it != new_core.end();)
	{
		if (!outputCheck(*it, dfg, arch) || !inputCheck(*it, dfg, arch))
			new_core.erase(it++);
		else
			++it;
	}
	return new_core;
}

void AlgoUnion::removeExt(BitVector &ext, const DataFlowGraph &dfg, bool down,
						  const BitVector &core)
{
	// remove any extension points that do not lead to 'new' nodes
	for (BitVector::size_type i = ext.find_first();
		i != BitVector::npos; i = ext.find_next(i))
	{
		BitVector nodes = down ? dfg.out_verticesBv(i) : dfg.in_verticesBv(i);
		if (nodes.is_subset_of(core))
			ext[i] = false;
	}
}

void AlgoUnion::checkInvalid(const DataFlowGraph &dfg, const Architecture &arch)
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

void AlgoUnion::getAllPreds(DataFlowGraph::Vertex v, const DataFlowGraph &dfg, 
							BitVector &preds)
{
	DataFlowGraph::DFG::in_edge_iterator edgeIt, edgeItEnd;
	tie(edgeIt, edgeItEnd) = dfg.in_edges(v);
	for (; edgeIt != edgeItEnd; ++edgeIt)
	{
		DataFlowGraph::Vertex source = dfg.source(*edgeIt);
		if (!preds[source])
		{
			preds[source] = true;
			getAllPreds(source, dfg, preds);
		}
	}
}

void AlgoUnion::getAllSuccs(DataFlowGraph::Vertex v, const DataFlowGraph &dfg, 
							BitVector &succs)
{
	DataFlowGraph::DFG::out_edge_iterator edgeIt, edgeItEnd;
	tie(edgeIt, edgeItEnd) = dfg.out_edges(v);
	for (; edgeIt != edgeItEnd; ++edgeIt)
	{
		DataFlowGraph::Vertex target = dfg.target(*edgeIt);
		if (!succs[target])
		{
			succs[target] = true;
			getAllSuccs(target, dfg, succs);
		}
	}
}

void AlgoUnion::calcEcs(const DataFlowGraph &dfg)
{
	for (BitVector::size_type i = invalid.find_first();
		i != BitVector::npos; i = invalid.find_next(i))
	{
		// none of this node's predecessors and successors can coexist in a convex pattern
		BitVector succs(topo.size()), preds(topo.size());
		getAllPreds(i, dfg, preds);
		getAllSuccs(i, dfg, succs);
		for (BitVector::size_type index = preds.find_first();
			index != BitVector::npos; index = preds.find_next(index))
		{
			BitVectorMap::iterator it = ecs.find(index);
			if (it != ecs.end())
				it->second |= succs;
			else
				ecs.insert(make_pair(index, succs));
		}
		for (BitVector::size_type index = succs.find_first();
			index != BitVector::npos; index = succs.find_next(index))
		{
			BitVectorMap::iterator it = ecs.find(index);
			if (it != ecs.end())
				it->second |= preds;
			else
				ecs.insert(make_pair(index, preds));
		}
	}
}

void AlgoUnion::run(const DataFlowGraph &dfg, const Architecture &arch, ResultVector &result,
					bool benchmark)
{
	if (dfg.num_vertices() == 0)
		return;
	topo = dfg.reverseTopologicalSort();
	uc_set.clear();
	exclude.resize(topo.size());
    exclude.reset();
	dc_set.clear();
	ecs.clear();
	maxUcs.clear();
	maxDcs.clear();
	checkInvalid(dfg, arch);
	unsigned int nregions = defineRegions(dfg, arch);
	orgRegions = regions;
	calcEcs(dfg);
	unsigned int feasiblePatterns = 0;
	BitVector remExts(topo.size());
	PatternSet allPatterns;
	for (unsigned int region = 1; region < nregions + 1; ++region)
	{
		enumerateUpwardCones(region, dfg, arch);
		// for all v of R in reverse topological order
		for (DataFlowGraph::VertexVector::const_iterator it = topo.begin();
			it != topo.end(); ++it)
		{
			if (regions[*it] != region) continue;
			// PATTERNS(v) = UC_SET(v)
			PatternSet patterns = uc_set.find(*it)->second;
			// ext = OUT(MAX_UC(v,R))
			BitVector ext = getOutputNodes(maxUcs.find(*it)->second, dfg);
			// if ext not empty then
			if (!ext.none())
			{
				// PATTERNS(v) = UNION(PATTERNS(v), ext, down)
				remExts.set();
				patterns = Union(patterns, ext, true, dfg, arch, remExts);
				//cout << "identified " << patterns.size() << " patterns\n";
				feasiblePatterns += patterns.size();
				if (!benchmark)
					joinPatternSets(allPatterns, patterns);
			}
			// remove v from R
			regions[*it] = 0;
			exclude[*it] = true;
		}
	}
	//cout << "feasiblePatterns: " << feasiblePatterns << "\n";
	//cout << "allPatterns.size(): " << allPatterns.size() << "\n";

	result.reserve(allPatterns.size());
	for (PatternSet::const_iterator it = allPatterns.begin();
		it != allPatterns.end(); ++it)
	{
		result.push_back(*it);
	}
}
