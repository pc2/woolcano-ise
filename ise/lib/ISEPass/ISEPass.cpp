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
#include <llvm/Module.h>
#include <llvm/Pass.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Analysis/ProfileInfoLoader.h>
#include  <llvm/Support/Debug.h> 
#include <vector>
#include <map>
#include "IseAlgorithm.h"
#include "DataFlowGraph.h"
#include "ArchitectureVirtexFx.h"
#include "ArchitectureVirtual.h"
#include "Util.h"
#include "Benchmark.h"
#include "AlgoMaxMiso.h"
#include "AlgoSingleCut.h"
#include "AlgoMultipleCuts.h"
#include "AlgoUnion.h"
#include "RuntimeEstimation.h"
#include "SelectionAlgorithm.h"
#include "SelectionRandom.h"
#include "SelectionMethod1.h"
#include "Types.h"


using namespace llvm;
using namespace std;

static cl::opt<string> ISEProfInfoFilename("ise-profile-info-file", 
                                           cl::init("llvmprof.out"),
                                           cl::value_desc("filename"),
                                           cl::desc("Profile file used by -ise"));
static cl::opt<bool> ISEOutputBB("ise-output-bb", 
                                 cl::desc("output Graphviz files for BB"));
static cl::opt<bool> ISEOutputIdentCand("ise-output-ident-cand", 
                                        cl::desc("output Graphviz files for identificated candidates"));
static cl::opt<bool> ISEOutputSelCand("ise-output-sel-cand",
                                      cl::init(false),
                                      cl::desc("output Graphviz files for selected templates"));
static cl::opt<bool> ISEOutputSelCI("ise-output-sel-ci", 
                                    cl::desc("output Graphviz files for selected CI"));

static cl::opt<bool> ISEcreateCI("ise-create-ci", cl::init(false), cl::desc("create CI from selected templates"));

static cl::opt<bool> ISEBenchmark("ise-benchmark");
static cl::opt<bool> ISENoSplitConstants("ise-no-split-constants");
static cl::opt<bool> ISERuntimeEstimation("ise-runtime-estimation");
static cl::opt<unsigned int> ISEBenchmarkTicks("ise-benchmark-ticks", 
                                               cl::init(2500));

static cl::opt<string> ISEArchitecture("ise-architecture", 
                                       cl::init("virtex"),
                                       cl::value_desc("architecture"),
                                       cl::desc("Select ISE target architecture: virtual (def.), virtex"));

/* Communication settings */
static cl::opt<unsigned int>ISEArchCommNoInOverhead("ise-archi-comm-no-in-overhead",
                                                    cl::init(0),
                                                    cl::desc("How many input transactions are for free (def. 0)"));

static cl::opt<unsigned int>ISEArchCommNoOutOverhead("ise-archi-comm-no-out-overhead",
                                                     cl::init(0),
                                                     cl::desc("How many output transactions are for free (def. 0)"));

static cl::opt<int>ISEArchCommInBusWidth("ise-archi-comm-in-bus",
                                         cl::init(-1),
                                         cl::desc("Width of CI input bus [operand_size]"));

static cl::opt<int>ISEArchCommOutBusWidth("ise-archi-comm-out-bus",
                                          cl::init(-1),
                                          cl::desc("Width of CI output bus [operand_size]"));

static cl::opt<int>ISEArchCommInBusCLK("ise-archi-comm-in-bus-clk",
                                       cl::init(-1),
                                       cl::desc("Speed of CI input bus for transfering ise-archi-comm-in-bus operands"));

static cl::opt<int>ISEArchCommOutBusCLK("ise-archi-comm-out-bus-clk",
                                        cl::init(-1),
                                        cl::desc("Speed of CI output bus for transfering ise-archi-comm-out-bus operands"));

static cl::opt<int> ISEArchCommClk("ise-arch-comm-clk",
                                   cl::init(-1),
                                   cl::desc("CI Clock frequency [MHz]"));

/* Quantities of inputs & outputs */                                                   
static cl::opt<int> ISEArchMaxCI("ise-archi-max-ci", 
                                 cl::init(-1),
                                 cl::desc("Overwrite max number of custom_instruction"));

static cl::opt<int> ISEArchMaxInput("ise-archi-max-input", 
                                    cl::init(-1),
                                    cl::desc("Overwrite max number of inputs for custom_instruction"));

static cl::opt<int> ISEArchMaxOutput("ise-archi-max-output", 
                                     cl::init(-1),
                                     cl::desc("Overwrite max number of outputs for custom_instruction"));


/* Identification algorithm - extracts graphs from app */
static cl::opt<string> ISEAlgorithm("ise-algorithm", cl::init("maxmiso"),
                                    cl::value_desc("algorithm"),
                                    cl::desc("Select ISE identification algorithm: maxmiso (def.), singlecut, multicut, union"));
static cl::opt<bool> ISEAlgorithmStop("ise-algorithm-stop", 
                                      cl::init(false),
                                      cl::desc("Show only result of identification algorithm and then stop."));


/* Selection Algorithm - selects apropriate BB from identificated graphs */
static cl::opt<string> ISESelAlgorithm("ise-sel-algorithm", cl::init("method1"),
                                       cl::value_desc("algorithm"),
                                       cl::desc("Select ISE selection algorithm: method1 (def.), random"));

namespace {
	class ISEPass : public ModulePass
	{
	private:
		ProfileMap profileMap;
		ProfileList profileList;
		void readProfilingInfo(Module &M);
		IseAlgorithm* getIdentificationAlgorithm(void);
		SelectionAlgorithm* getSelectionAlgorithm(void);
		Architecture* getArchitecture(void);
		static bool ProfileListSortPredicate(const ProfilePair& lhs, const ProfilePair& rhs);
		typedef map<const Value*, const Value*> ValueMap;
		void moveSubgraphToFunction(BasicBlock* bb, const DataFlowGraph &dfg,
                                    const string &name, ValueMap &replaced);
		DataFlowGraph dfgFromBasicBlock(const BasicBlock* bb);
	public:
		static char ID;
		virtual bool runOnModule(Module &M);
		virtual void getAnalysisUsage(AnalysisUsage &AU) const;
		ISEPass() : ModulePass(&ID) {}
	};
    
	class RuntimeEstimationPass : public ModulePass
	{
	public:
		static char ID;
		virtual bool runOnModule(Module &M);
		virtual void getAnalysisUsage(AnalysisUsage &AU) const;
		RuntimeEstimationPass() : ModulePass(&ID) {}
	};
}

char ISEPass::ID = 0;
char RuntimeEstimationPass::ID = 0;
static RegisterPass<ISEPass> X("ise", "instruction-set extension (identification & selection)");
static RegisterPass<RuntimeEstimationPass> Y("runtime-estimation", "simple runtime estimation");

void ISEPass::getAnalysisUsage(AnalysisUsage &AU) const
{
	if (ISEBenchmark)
		AU.setPreservesAll();
}

bool ISEPass::ProfileListSortPredicate(const ProfilePair& lhs, const ProfilePair& rhs)
{
	return lhs.second.count > rhs.second.count;
}

/* builds DFG from BB */
DataFlowGraph ISEPass::dfgFromBasicBlock(const BasicBlock* bb)
{
	DataFlowGraph dfg(*bb);
	if (!ISENoSplitConstants)
		dfg.splitConstants();
	return dfg;
}

/*
 reads profiling information and stores it in profileMap, profileList
 each basic block is assigned a probability [0, 1]
 warning: loading the wrong profiling information may cause the pass to crash
 */
void ISEPass::readProfilingInfo(Module &M)
{
	if (Util::fileExists(ISEProfInfoFilename))
	{
		ProfileInfoLoader PIL("ise", ISEProfInfoFilename, M);
		if (PIL.hasAccurateBlockCounts())
		{
            /* the vector assosiates BB with the counter representing execution of BB */
			typedef vector< pair<BasicBlock*, unsigned> > ProfileInfoVector;
			ProfileInfoVector counts;
			PIL.getBlockCounts(counts);
            
            /* get total number of BB execution  */
			double totalExecutions = 0;
			for (ProfileInfoVector::const_iterator it = counts.begin();
                 it != counts.end(); ++it)
			{
				totalExecutions += it->second;
			}
            
            /* Assosiate BB with ExNum, Freq and store it as an list and hash */
			for (ProfileInfoVector::const_iterator it = counts.begin();
                 it != counts.end(); ++it)
			{
				ProfileInfo info(it->second, it->second / totalExecutions);
				profileMap.insert(make_pair(it->first, info));
				profileList.push_back(make_pair(it->first, info));
			}
		}
	}
    
    /* create artificial profiling stats */
	if (profileList.size() == 0)
	{
		llvm::cerr << "WARNING: no profiling information found, assuming uniform distribution\n";
        
        /* count the number of BB */
		double bbc = 0;
		for (Module::const_iterator I = M.begin(), E = M.end(); I != E; ++I)
			bbc += I->size();
        
        /* assume that each bb was executed once */
		ProfileInfo info(1, 1.0f / bbc);
		for (Module::const_iterator I = M.begin(), E = M.end(); I != E; ++I)
			for (Function::const_iterator BB = I->begin(), E = I->end(); BB != E; ++BB)
			{
				profileMap.insert(make_pair(&*BB, info));
				profileList.push_back(make_pair(&*BB, info));
			}
	}
	profileList.sort(ProfileListSortPredicate);
}

Architecture* ISEPass::getArchitecture(void)
{
  string architecture = ISEArchitecture;
	if (architecture.compare("virtex") == 0)
		return new ArchitectureVirtexFx();
	else if (architecture.compare("virtual") != 0)
		llvm::cerr << "WARNING: Invalid target architecture specified, reverting to virtual\n";
	return new ArchitectureVirtual();
}

SelectionAlgorithm* ISEPass::getSelectionAlgorithm(void)
{
	string algorithm = ISESelAlgorithm;
    llvm::cout << "\nSelection algorithm: " << algorithm<< "\n";
	if (algorithm.compare("random") == 0)
		return new SelectionRandom();
	else if (algorithm.compare("method1") != 0)
		llvm::cerr << "WARNING: Invalid selection algorithm specified, reverting to method1\n";
	return new SelectionMethod1();
}

IseAlgorithm* ISEPass::getIdentificationAlgorithm(void)
{
	string algorithm = ISEAlgorithm;
    llvm::cout << "\nIdentification algorithm: " << algorithm<< "\n";
	if (algorithm.compare("singlecut") == 0)
		return new AlgoSingleCut();
	else if (algorithm.compare("multicut") == 0)
		return new AlgoMultipleCuts();
	else if (algorithm.compare("union") == 0)
		return new AlgoUnion();
	else if (algorithm.compare("maxmiso") != 0)
		llvm::cerr << "WARNING: Invalid identification algorithm specified, reverting to MaxMISO\n";
	return new AlgoMaxMiso();
}

/* main - executed on every module */
bool ISEPass::runOnModule(Module &M)
{
	readProfilingInfo(M);
	// custom instruction identification
	IseAlgorithm *algo = getIdentificationAlgorithm();
	Architecture *arch = getArchitecture();
    
    if (ISEArchCommClk != -1)       arch->setClockRate(ISEArchCommClk);
    
    if (ISEArchMaxCI != -1)         arch->setMaxCI(ISEArchMaxCI); 
    if (ISEArchMaxInput != -1)      arch->setMaxInput(ISEArchMaxInput); 
    if (ISEArchMaxOutput != -1)     arch->setMaxOutput(ISEArchMaxOutput); 
    
    if (ISEArchCommNoInOverhead >0)     arch->setCommNoInOverhead(ISEArchCommNoInOverhead);
    if (ISEArchCommNoOutOverhead >0)    arch->setCommNoOutOverhead(ISEArchCommNoOutOverhead);
    
    if (ISEArchCommInBusWidth  != -1)   arch->setCommInBusWidth(ISEArchCommInBusWidth);
    if (ISEArchCommOutBusWidth != -1)   arch->setCommOutBusWidth(ISEArchCommOutBusWidth); 
    if (ISEArchCommInBusCLK != -1)      arch->setCommInBusCLK(ISEArchCommInBusCLK); 
    if (ISEArchCommOutBusCLK != -1)     arch->setCommOutBusCLK(ISEArchCommOutBusCLK); 
    
    
    if (ISEBenchmark) 
        llvm::cout << "#Nodes\tmsecs\titerations\n";
    
    if (ISEAlgorithmStop)
        DebugFlag = 1;
    
	ResultMap resultMap;
	DfgMap dfgMap;
	string modName = M.getModuleIdentifier().substr(M.getModuleIdentifier().find_last_of("/\\") + 1);
    
    /* iterate over functions in module */
	for (Module::const_iterator I = M.begin(), E = M.end(); I != E; ++I)
	{
        //TODO: make option out of it!
		// ignore main function (mainly for our benchmarks)
		// if (I->getName().compare("main") == 0 && !ISEBenchmark) continue;
		unsigned int nB = 0;
        
        /* iterate over BB in function */
		for (Function::const_iterator BB = I->begin(), E = I->end(); BB != E; ++BB, ++nB)
		{
            string IdentName = I->getName() + "-" + BB->getName();
			// ignore basic blocks that have not been executed
			if (profileMap.find(BB)->second.count == 0) continue;
            
            /* Create DFG from BB  && associate both in a map*/
			DataFlowGraph dfg = dfgFromBasicBlock(BB);
			if (dfg.num_vertices() == 0) continue;
			dfgMap.insert(make_pair(BB, dfg));
            
            
            /* in order to find the condidates from given dfg run identification algorithm */
			ResultVector candidateVector;
			if (!ISEBenchmark)
			{
                printf("- processing DFG of func: %-25s\t bb: %-25s \t with %d nodes. ",
                       I->getName().c_str(), BB->getName().c_str(), (int)dfg.num_vertices());
                
				algo->run(dfg, *arch, candidateVector);
                llvm::cout << "Found " << candidateVector.size() << " candidates.\n";
                
                /* if candidates are found then store them in a map */
                if (candidateVector.size() > 0)
                {
                    DEBUG(std::cout << "# storing " << IdentName << " with candidateVector.size()\
                          = " << candidateVector.size() << "\n");
                    
                    resultMap.insert(make_pair(BB, candidateVector));
                } 
                
                string blockName = IdentName + "_" + Util::stringify(nB);
                /* store to GraphViz files */
                if (ISEOutputBB)
                {
                    /* store whole DFG */
                    // string blockName = modName + "_" + IdentName + "_" + Util::stringify(nB);
                    Util::dumpToFile(blockName + ".gv", dfg.writeGraphviz(false,true));
                }
                if (ISEOutputIdentCand) {                    
                    /* store ident. candidates under *_cand_* name */
                    for (unsigned i = 0; i < candidateVector.size(); ++i)
                    {
                        string graphName = blockName + "_cand_" + Util::stringify(i) + ".gv";                    
                        Util::dumpToFile("_"+graphName, DataFlowGraph(dfgMap.find(BB)->second, 
                          candidateVector[i]).writeGraphviz(true));
                    }
                }
			} 
            else
			{
                // llvm::cout << "Benchmarking on\n";
				if (dfg.num_vertices() == 0) continue;
				float time = 0.0f;
				llvm::cout << dfg.num_vertices() << "\t";
				unsigned long ticks_start = Benchmark::getTicks();
				unsigned long ticks = 0, iterations = 0;

				do
				{
					algo->run(dfg, *arch, candidateVector, true);
                    ticks = Benchmark::getTicks() - ticks_start;
					candidateVector.clear();
					++iterations;

				} while (ticks < ISEBenchmarkTicks);
				if (iterations == 1 && ticks < 20000)
				{
                    candidateVector.clear();
					unsigned long ticks_start2 = Benchmark::getTicks();
					algo->run(dfg, *arch, candidateVector, true);
                    ticks = (ticks + (Benchmark::getTicks() - ticks_start2)) / 2;
				}
				time = static_cast<float>(ticks) / static_cast<float>(iterations);
				llvm::cout << time << "\t" << iterations << "\n";
                continue;  // jump to another block
			}
        }
	}
    
	if (ISEBenchmark) {
        llvm::cout << "#EOF\n";
        return true; // exit from pass
    }
		
	delete algo;
    
DEBUG(    
    /* iteratre over resultMap */
    std::cout << "Size: " << resultMap.size() << "\n";
    for (ResultMap::iterator it = resultMap.begin(); it != resultMap.end(); it ++) {
        const llvm::BasicBlock *bb = it->first;
        ResultVector &cand = it->second;
        std::cout << "# " << bb->getName() << " " << cand.size() << "\n";
    });
    
    if (ISEAlgorithmStop) 
        return true;


    
	// custom instruction selection
	SelectionAlgorithm* selectAlgo = getSelectionAlgorithm();
	ResultMap selected;
	selectAlgo->run(profileList, resultMap, dfgMap, *arch, selected, ISEOutputSelCand);
	delete selectAlgo;
    
	// replace selected DFGs
	if (ISEcreateCI)
	{
		unsigned int cur_udi = 0;
		ValueMap replaced;
		for (ResultMap::const_iterator it = selected.begin(); it != selected.end();
             ++it)
		{
			for (ResultVector::const_iterator bv_it = it->second.begin();
                 bv_it != it->second.end(); ++bv_it)
			{
				const DataFlowGraph& parentDfg = dfgMap.find(it->first)->second;
				assert(parentDfg.num_vertices() == bv_it->size());
				DataFlowGraph dfg(parentDfg, *bv_it);
				if (ISEOutputSelCI)
				{
					string graphName = modName + string("_udi") + Util::stringify(cur_udi) + ".gv";
					Util::dumpToFile(graphName, dfg.writeGraphviz(true));
				}
				if (arch->getMaxOutputs() == 1)
    				moveSubgraphToFunction(const_cast<BasicBlock*>(it->first), dfg,
                                           string("udi") + Util::stringify(cur_udi++), replaced);
			}
		}
		llvm::cout << "Selected " << cur_udi << " templates\n";
	}
    
    
	/*
     estimate runtime of whole program in pure software and with extended
     instruction set
     */
	if (ISERuntimeEstimation)
	{
		long long runtimeSw = 0, runtimeHw = 0;
		for (DfgMap::const_iterator it = dfgMap.begin(); it != dfgMap.end(); ++it)
		{
			ProfileMap::const_iterator prof_it = profileMap.find(it->first);
			long long sw = RuntimeEstimation::estimateSwRuntime(it->second, *arch);
			runtimeSw += sw * prof_it->second.count;
			ResultMap::const_iterator hw_it = selected.find(it->first);
			if (hw_it != selected.end())
			{
				long long hw = sw;
				const DataFlowGraph &parentDfg = dfgMap.find(hw_it->first)->second;
				for (ResultVector::const_iterator ise_it = hw_it->second.begin();
                     ise_it != hw_it->second.end(); ++ise_it)
				{
					DataFlowGraph dfg(parentDfg, *ise_it);
					long long swsg = RuntimeEstimation::estimateSwRuntime(dfg, *arch);
					long long hwsg = arch->convertHwToSwTiming(RuntimeEstimation::estimateHwRuntime(dfg, *arch));
                    hw = hw - swsg + hwsg +  arch->getExecutionOverhead(
                                                                        dfg.num_inputs(), dfg.num_outputs());
				}
				runtimeHw += hw * prof_it->second.count;
			}
			else
			{
				runtimeHw += sw * prof_it->second.count;
			}
		}
		llvm::cout << "Runtime estimation:\n";
		llvm::cout << "\testimated 'software' runtime: " << runtimeSw << " cycles\n";
		llvm::cout << "\testimated 'hardware' runtime: " << runtimeHw << " cycles\n";
		llvm::cout << "\tratio: " << static_cast<float>(runtimeSw) / static_cast<float>(runtimeHw) << "\n";
	}
	delete arch;
	return true;
}

/*
 moves all instructions in DFG to a new function
 limited to a single output value
 */
void ISEPass::moveSubgraphToFunction(BasicBlock* bb, const DataFlowGraph &dfg,
									 const string &name, ValueMap &replaced)
{
	//llvm::cout << dfg.writeGraphviz(true, false) << "\n";
	//llvm::cout << "old basic block:\n" << *bb << "\n";
	assert(dfg.num_outputs() == 1);
	DataFlowGraph::VertexVector topo = dfg.reverseTopologicalSort();
	assert(dfg.getType(topo[0]) == DataFlowGraph::OUTPUT);
	// collect inputs and output
	DataFlowGraph::VertexVector inputNodes;
	DataFlowGraph::Vertex outputNode = dfg.null_vertex();
	for (DataFlowGraph::VertexVector::const_iterator it = topo.begin();
         it != topo.end(); ++it)
	{
		DataFlowGraph::operator_t type = dfg.getType(*it);
		if (type == DataFlowGraph::INPUT)
			inputNodes.push_back(*it);
		else if (type == DataFlowGraph::OUTPUT)
		{
			assert(outputNode == dfg.null_vertex());
			outputNode = *it;
		}
	}
	// the output node will be the function's return value
	Value* outputValue = const_cast<Value*>(dfg.getValue(outputNode));
	// the function call will be inserted after the 'last' original instruction
	BasicBlock::iterator insertCallPos(cast<Instruction>(outputValue));
	++insertCallPos;
	// input nodes will become the function's parameters
	vector<const Type*> paramTypes;
	vector<Value*> params;
	for (DataFlowGraph::VertexVector::const_iterator it = inputNodes.begin();
         it != inputNodes.end(); ++it)
	{
		const Value* value = dfg.getValue(*it);
		ValueMap::const_iterator rep_it = replaced.find(value);
		if (rep_it != replaced.end())
			value = rep_it->second;
		paramTypes.push_back(value->getType());
		params.push_back(const_cast<Value*>(value));
	}
	Function* function = Function::Create(FunctionType::get(outputValue->getType(), paramTypes, false), 
                                          GlobalValue::WeakAnyLinkage, name, bb->getParent()->getParent());
	BasicBlock* newBb = BasicBlock::Create(name + string("_entry"), function);
	// insert function call
	CallInst* call = CallInst::Create(function, params.begin(), params.end(), 
                                      name + string("_call"), insertCallPos);
	// keep track of replaced outputs
	replaced.insert(make_pair(outputValue, call));
	// replace references to output value
	for (Value::use_iterator use_it = outputValue->use_begin(); use_it != outputValue->use_end();)
	{
		Instruction* user = dyn_cast<Instruction>(*use_it);
		if (user && user->getParent() != newBb)
		{
			user->replaceUsesOfWith(outputValue, call);
			use_it = outputValue->use_begin();	// previous call messes with the iterators...
		}
		else
			++use_it;
	}
	// insert instructions in topological order
	ReturnInst* retInst = ReturnInst::Create(outputValue, newBb);
    
	DataFlowGraph::VertexVector::const_reverse_iterator it = topo.rbegin();
	DataFlowGraph::VertexVector::const_reverse_iterator ite = topo.rend();
	for (; it != ite; ++it )
	{
		DataFlowGraph::operator_t type = dfg.getType(*it);
		if (type == DataFlowGraph::OUTPUT || type == DataFlowGraph::OPERATOR)
		{
			Instruction* inst = cast<Instruction>(const_cast<Value*>(dfg.getValue(*it)));
			inst->moveBefore(retInst);
		}
	}
	// replace all references to inputs with references to function parameters
	Function::arg_iterator arg_it = function->arg_begin();
	for (vector<Value*>::const_iterator it = params.begin(); it != params.end();
         ++it)
	{
		for (Value::use_iterator use_it = (*it)->use_begin(); use_it != (*it)->use_end();)
		{
			Instruction* user = dyn_cast<Instruction>(*use_it);
			if (user && user->getParent() == newBb)
			{
				user->replaceUsesOfWith(*it, arg_it);
				use_it = (*it)->use_begin();
			}
			else
				++use_it;
		}
		++arg_it;
	}
	//llvm::cout << "new function:\n" << *function << "\n";
	//llvm::cout << "new basic block:\n" << *bb << "\n";
}

// ***************************************************************************

void RuntimeEstimationPass::getAnalysisUsage(AnalysisUsage &AU) const
{
	AU.setPreservesAll();
}

bool RuntimeEstimationPass::runOnModule(Module &M)
{
	ProfileInfoLoader PIL("ise", "llvmprof.out", M);
	typedef vector< pair<BasicBlock*, unsigned> > ProfileInfoVector;
	ProfileInfoVector counts;
	if (PIL.hasAccurateBlockCounts())
	{
		PIL.getBlockCounts(counts);
	}
	ProfileInfoVector::const_iterator prof_it = counts.begin();
	// estimate runtime
	ArchitectureVirtexFx arch;
	for (Module::const_iterator I = M.begin(), E = M.end(); I != E; ++I)
	{
		for (Function::const_iterator BB = I->begin(), E = I->end(); BB != E; ++BB)
		{
			if (prof_it != counts.end() && prof_it->second == 0)
			{
				++prof_it;
				continue;
			}
			DataFlowGraph dfg(*BB);
			unsigned int swsg = RuntimeEstimation::estimateSwRuntime(dfg, arch) +
            arch.getSwInstructionTiming(BB->getTerminator());
			unsigned int hwsg = arch.convertHwToSwTiming(RuntimeEstimation::estimateHwRuntime(dfg, arch))
            + arch.getExecutionOverhead(dfg.num_inputs(), dfg.num_outputs());
			llvm::cout << I->getName() << "() - " << BB->getName() << "\tsw: " <<
            swsg << "\thw: " << hwsg;
			if (prof_it != counts.end())
			{
				llvm::cout << "\tcount: " << prof_it->second;
				++prof_it;
			}
			llvm::cout << "\n";
		}
	}
	return true;
}
