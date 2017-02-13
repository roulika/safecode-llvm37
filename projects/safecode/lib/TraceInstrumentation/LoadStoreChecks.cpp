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
  // Return type for the functions
  Type *VoidTy = Type::getVoidTy(M->getContext());
 
  // Types of the arguments for the functions
  std::vector<Type *> ArgTypes;
  ArgTypes.push_back(getVoidPtrType(M->getContext()));
  ArgTypes.push_back(getVoidPtrType(M->getContext()));
  ArgTypes.push_back(IntegerType::getInt32Ty(M->getContext()));
  

  //Function prototype
  FunctionType *TraceTy = FunctionType::get(VoidTy, ArgTypes, false);

  // Create the functions.
  TraceLoadFunc = dyn_cast<Function>(M->getOrInsertFunction ("trace_load",
                                                               TraceTy));

  TraceStoreFunc = dyn_cast<Function>(M->getOrInsertFunction ("trace_store",
                                                                TraceTy));
 
  visit(F);
 


  return true;
}


void LoadStoreChecks::visitLoadInst(LoadInst &LI) {
  // Instrument a load instruction with a load check.
  llvm::errs() << "LOAD\n";
 
  //
  // Create a list with the arguments of the trace_load function.
  // The first argument is the pool handle.
  // The second argument is the pointer to check.
  // The third argument is the Module ID.
  //
  std::vector<Value *> args;
  LLVMContext &Context = LI.getContext();


  //This will be replaced with the module id.
  Value *ModuleID = ConstantInt::get(IntegerType::getInt32Ty(Context), 0); 
  
  args.push_back(ConstantPointerNull::get(getVoidPtrType(Context)));
  args.push_back(castTo (LI.getPointerOperand(), getVoidPtrType(Context), &LI));
  args.push_back (ModuleID);
  
  CallInst *CI = CallInst::Create(TraceLoadFunc, args, "", &LI); 

  return;
}

void LoadStoreChecks::visitStoreInst(StoreInst &SI) {
  // Instrument a store instruction with a store check.
  llvm::errs() << "STORE\n";

  //
  // Create a list with the arguments of the trace_store function.
  // The first argument is the pool handle.
  // The second argument is the pointer to check.
  // The third argument is the Module ID.
  //
  std::vector<Value *> args;
  LLVMContext &Context = SI.getContext();


  //This will be replaced with the module id.
  Value *ModuleID = ConstantInt::get(IntegerType::getInt32Ty(Context), 0); 
  
  args.push_back(ConstantPointerNull::get(getVoidPtrType(Context)));
  args.push_back(castTo(SI.getPointerOperand(), getVoidPtrType(Context), &SI));
  args.push_back (ModuleID);
  
  CallInst *CI = CallInst::Create(TraceStoreFunc, args, "", &SI); 

  return;

}

}