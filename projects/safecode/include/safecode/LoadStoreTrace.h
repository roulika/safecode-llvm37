//====================- LoadStoreTrace.h===============---------------- --//
// 
//       The University of Rochester Security Research Group
//
// This file was developed by the University of Rochester security research
// group.
// 
//===----------------------------------------------------------------------===//
//
// 
//
//===----------------------------------------------------------------------===//

#ifndef _SAFECODE_LOADSTORETRACE_H_
#define _SAFECODE_LOADSTORETRACE_H_

#include "llvm/Analysis/CallGraph.h"
#include "llvm/Pass.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/DataLayout.h"

namespace llvm {

//
// Pass: LoadStoreChecks
//
// Description:
//  
//
struct LoadStoreTrace : public ModulePass, InstVisitor<LoadStoreTrace> {
  public:
    static char ID;
    LoadStoreTrace ()  : ModulePass (ID) { }
    const char *getPassName() const {
        return "Insert Load-Store Trace Function Calls";
    }
    virtual bool runOnModule(Module &M);
    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      
    // Required passes
    AU.addRequired<CallGraphWrapperPass>();
    //AU.addRequired<llvm::DataLayout>();

      // Preserved passes
      AU.setPreservesCFG();
    };

    // Visitor methods
    void visitLoadInst(LoadInst &LI);
    void visitStoreInst(StoreInst &SI);
    void runAtExit(Module &M);


    Function *TraceLoadFunc;
    Function *TraceStoreFunc;

    Value *ModID;
    const DataLayout *TD;


};

}
#endif