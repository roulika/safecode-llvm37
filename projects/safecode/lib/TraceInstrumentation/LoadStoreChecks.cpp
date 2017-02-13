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

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"

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
LoadStoreChecks::runOnModule (Module &M) {
  llvm::errs() << "FROM NEW PASS\n";

  // Create prototypes for the functions
  // Module *M = F.getParent();

  ModuleID = M.getModuleIdentifier();
  llvm::errs() << "ModuleID: " << ModuleID << "\n";

  // Return type for the functions
  Type *VoidTy = Type::getVoidTy(M.getContext());
 
  // Types of the arguments for the functions
  std::vector<Type *> ArgTypes;
  ArgTypes.push_back(getVoidPtrType(M.getContext()));
  ArgTypes.push_back(getVoidPtrType(M.getContext()));
  ArgTypes.push_back(getVoidPtrType(M.getContext()));
  

  //Function prototype
  FunctionType *TraceTy = FunctionType::get(VoidTy, ArgTypes, false);

  // Create the functions.
  TraceLoadFunc = dyn_cast<Function>(M.getOrInsertFunction ("trace_load",
                                                               TraceTy));

  TraceStoreFunc = dyn_cast<Function>(M.getOrInsertFunction ("trace_store",
                                                                TraceTy));
 
  ModNameInit = ConstantDataArray::getString (M.getContext(), ModuleID);

  ModName = new GlobalVariable (M, ModNameInit->getType(),
                                        true,
                                        GlobalValue::InternalLinkage,
                                        ModNameInit,
                                        "modname");


  visit(M);
 


  return true;
}


void LoadStoreChecks::visitLoadInst(LoadInst &LI) {
  // Instrument a load instruction with a load check.
 // llvm::errs() << "LOAD\n";
 
  //
  // Create a list with the arguments of the trace_load function.
  // The first argument is the pool handle.
  // The second argument is the pointer to check.
  // The third argument is the Module ID.
  //
  std::vector<Value *> args;
  LLVMContext &Context = LI.getContext();

  LI.dump();

  //llvm::errs() << *ModName << "\n";

  
  args.push_back(ConstantPointerNull::get(getVoidPtrType(Context)));
  args.push_back(castTo(LI.getPointerOperand(), getVoidPtrType(Context), &LI));
  args.push_back(castTo(ModName,getVoidPtrType(Context), "modname", &LI));
  
  CallInst *CI = CallInst::Create(TraceLoadFunc, args, "", &LI); 

  return;
}

void LoadStoreChecks::visitStoreInst(StoreInst &SI) {
  // Instrument a store instruction with a store check.
  //llvm::errs() << "STORE\n";

  //
  // Create a list with the arguments of the trace_store function.
  // The first argument is the pool handle.
  // The second argument is the pointer to check.
  // The third argument is the Module ID.
  //
  std::vector<Value *> args;
  LLVMContext &Context = SI.getContext();

  
  args.push_back(ConstantPointerNull::get(getVoidPtrType(Context)));
  args.push_back(castTo(SI.getPointerOperand(), getVoidPtrType(Context), &SI));
  args.push_back(castTo(ModName,getVoidPtrType(Context), "modname", &SI));

  CallInst *CI = CallInst::Create(TraceStoreFunc, args, "", &SI); 

  return;

}

}