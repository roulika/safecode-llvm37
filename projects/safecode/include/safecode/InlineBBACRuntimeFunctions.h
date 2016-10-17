//===- InlineBBACRuntimeFunctions.h - Inline BBAC RuntimeFunctions--------- --//
//
//                          The SAFECode Compiler
//
// This file was developed by the LLVM research group and is distributed under
// the University of Illinois Open Source License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This pass replaces calls to bbac runtime functions within inline code to perform
// the check.  It is designed to provide the advantage of libLTO without libLTO.
//
//===----------------------------------------------------------------------===//

#ifndef _INLINE_BBAC_RUNTIME_FUNCTIONS_H_
#define _INLINE_BBAC_RUNTIME_FUNCTIONS_H_

#include "llvm/Pass.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/AssumptionCache.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "safecode/DebugInstrumentation.h"

namespace llvm {

class InlineBBACRuntimeFunctions : public ModulePass {
 public:
  static char ID;
  InlineBBACRuntimeFunctions () : ModulePass(ID) {}
  virtual bool runOnModule (Module &M);
  const char *getPassName() const {
    return "Inline baggy bounds accurate checks(BBAC) runtime functions";
  }

  virtual void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<DebugInstrument>();
    AU.addRequired<AssumptionCacheTracker>();
    AU.addRequired<CallGraphWrapperPass>();
    AU.addRequired<AliasAnalysis>();
    return;
  }
 private:
  bool createPoolCheckUIBodyFor (Function * F);
  bool createBoundsCheckUIBodyFor (Function * F);
  bool createPoolRegisterBodyFor (Function * F);
  bool createPoolUnregisterBodyFor (Function * F);
  bool createGlobalDeclarations (Module & M);
  bool inlineCheck (Function * F);
};
} // end namespace llvm;


#endif