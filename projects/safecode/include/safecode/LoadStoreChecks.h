//===- LoadStoreChecks.h  -------//
// 
//                     The LLVM Compiler Infrastructure
//
// This file was developed by the LLVM research group and is distributed under
// the University of Illinois Open Source License. See LICENSE.TXT for details.
// 
//===----------------------------------------------------------------------===//
//
// 
//
//===----------------------------------------------------------------------===//

#ifndef _SAFECODE_LOADSTORECHECKS_H_
#define _SAFECODE_LOADSTORECHECKS_H_

#include "llvm/Analysis/CallGraph.h"
#include "llvm/Pass.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {

//
// Pass: LoadStoreChecks
//
// Description:
//  
//
struct LoadStoreChecks : public ModulePass, InstVisitor<LoadStoreChecks> {
  public:
    static char ID;
    LoadStoreChecks () : ModulePass (ID) { }
    const char *getPassName() const {
      return "Insert Load-Store Checks";
    }
    virtual bool runOnModule(Module &M);
    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      // Required passes
      //AU.addRequired<CallGraph>();
      AU.addRequired<CallGraphWrapperPass>();

      // Preserved passes
      AU.setPreservesCFG();
    };

    // Visitor methods
    void visitLoadInst(LoadInst &LI);
    void visitStoreInst(StoreInst &SI);

    Function *TraceLoadFunc;
    Function *TraceStoreFunc;

    std::string ModuleID;
    Constant *ModNameInit;
    Value *ModName;

};

}
#endif