#include "llvm/Pass.h"
#include "llvm/Module.h"
#include "llvm/Function.h"
#include "DataFlowGraph.h"
#include "Util.h"

using namespace llvm;

namespace 
{
  class dotBBPass : public ModulePass
  {
    public:
      static char ID;
      virtual bool runOnModule(Module &M);
      virtual void getAnalysisUsage(AnalysisUsage &AU) const;
      dotBBPass() : ModulePass(&ID) {}
  };
}

char dotBBPass::ID = 0;
static RegisterPass<dotBBPass> Y("dot-from-bb", "simple runtime estimation");

// ***************************************************************************

void dotBBPass::getAnalysisUsage(AnalysisUsage &AU) const
{
  AU.setPreservesAll();
}

bool dotBBPass::runOnModule(Module &M)
{
  for (Module::const_iterator I = M.begin(), E = M.end(); I != E; ++I)
  {
    for (Function::const_iterator BB = I->begin(), E = I->end(); BB != E; ++BB)
    {

      DataFlowGraph dfg(*BB);
      Util::dumpToFile(BB->getName() + ".gv", dfg.writeGraphviz(true,true));
      llvm::cout << I->getName() << "() - " << BB->getName() << "\n";
    }
  }
  return true;
}
