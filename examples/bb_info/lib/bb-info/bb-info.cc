#include "llvm/Pass.h"
#include "llvm/Module.h"
#include "llvm/Function.h"
#include "Util.h"

#include <llvm/Support/CommandLine.h>


using namespace llvm;

static cl::opt<bool> detailed("bb-info-detailed", cl::desc("detailed infos for bc"));

namespace 
{
  class bb_info : public ModulePass
  {
    public:
      static char ID;
      virtual bool runOnModule(Module &M);
      virtual void getAnalysisUsage(AnalysisUsage &AU) const;
      bb_info() : ModulePass(&ID) {}
  };
}

char bb_info::ID = 0;
static RegisterPass<bb_info> Y("bb_info", "basic block infos");

// ***************************************************************************

void bb_info::getAnalysisUsage(AnalysisUsage &AU) const
{
  AU.setPreservesAll();
}

bool bb_info::runOnModule(Module &M)
{
 
  unsigned int module_no = 0;
  unsigned int bb_no = 0;
  unsigned int instr_no = 0;

  for (Module::const_iterator I = M.begin(), E = M.end(); I != E; ++I, ++module_no)
  {
    if (detailed)
      llvm::cout << "# Module: " << I->getName() << " " << " BB_no: " <<   I->size() << "\n";
      bb_no += I->size();

    for (Function::const_iterator BB = I->begin(), E = I->end(); BB != E; ++BB)
    {
      if (detailed)
        llvm::cout << " BB: " << BB->getName() << " inst: " << BB->size() << "\n";
      instr_no += BB->size();
    }
  }
  llvm::cout << "Modules: " << module_no << " BBs: " << bb_no << " nodes: " << instr_no << "\n";
  return true;
}
