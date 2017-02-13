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
LoadStoreChecks::runOnFunction (Function &F) {
  llvm::errs() << "FROM NEW PASS\n";

  // Create prototypes for the functions
  Module *M = F.getParent();
  Type *VoidTy = Type::getVoidTy(M->getContext());
  std::vector<Type *> ArgTypes;
  ArgTypes.push_back(IntegerType::getInt32Ty(M->getContext()));
  FunctionType *TraceTy = FunctionType::get (VoidTy, ArgTypes, false);

  // Create the functions.
  TraceLoadFunc = dyn_cast<Function>(M->getOrInsertFunction ("trace_load",
                                                               TraceTy));

  TraceStoreFunc = dyn_cast<Function>(M->getOrInsertFunction ("trace_store",
                                                                TraceTy));
 
 
 
 
 
  LLVMContext &Context = F.getContext();
  //This will be replaced with the module id.
  Value *AllocType = ConstantInt::get(IntegerType::getInt32Ty(Context), 0); 
  args.push_back (AllocType);
 
 
   visit(F);
 


  return true;
}


void LoadStoreChecks::visitLoadInst(LoadInst &LI) {
  // Instrument a load instruction with a load check.
  llvm::errs() << "LOAD\n";
  CallInst * CI = CallInst::Create(TraceLoadFunc, args, "", &LI); 
}

void LoadStoreChecks::visitStoreInst(StoreInst &SI) {
  // Instrument a store instruction with a store check.
  llvm::errs() << "STORE\n";
  CallInst * CI = CallInst::Create(TraceStoreFunc, args, "", &SI); 



}

}