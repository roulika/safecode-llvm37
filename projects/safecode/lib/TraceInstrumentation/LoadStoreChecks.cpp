//===- LoadStoreChecks.cpp--------------- --//
// 
//                     The LLVM Compiler Infrastructure
//
// This file was developed by the LLVM research group and is distributed under
// the University of Illinois Open Source License. See LICENSE.TXT for details.
// 
//===----------------------------------------------------------------------===//
//
// DESCRIPTION OF PASS
//
//===----------------------------------------------------------------------===//


#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Constants.h"
#include "safecode/LoadStoreChecks.h"
#include "safecode/Utility.h"

#define DEBUG_TYPE "safecode"


namespace llvm {

char LoadStoreChecks::ID = 0;

static RegisterPass<LoadStoreChecks>
X ("loadstorechecks", "Instrument loads and stores");

// Pass Statistics
namespace {
  STATISTIC (Accesses, "Number of Instrumented Loads and Stores");
}

bool
LoadStoreChecks::runOnModule (Module & M) {
  //
  // Create a function prototype for the function that performs incomplete
  // function call checks.
  //
  Type *VoidTy    = Type::getVoidTy (M.getContext());
  Type *VoidPtrTy = getVoidPtrType (M.getContext());
//  FunctionCheckUI = cast<Function>(M.getOrInsertFunction ("funccheckui",
 //                                                         VoidTy,
 //                                                         VoidPtrTy,
 //                                                         VoidPtrTy,
 //                                                         NULL));
 // assert (FunctionCheckUI && "Function Check function has disappeared!\n");
 // FunctionCheckUI->addFnAttr (Attributes::ReadNone);

  //
  // Visit all of the instructions in the function.
  //
 // visit (M);
  llvm::errs() << "FROM NEW PASS\n";
  return true;
}

}