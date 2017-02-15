//====================- LoadStoreTrace.cpp===============---------------- --//
// 
//       The University of Rochester Security Research Group
//
// This file was developed by the University of Rochester security research
// group.

//===----------------------------------------------------------------------===//
//
// DESCRIPTION OF PASS
//
//===----------------------------------------------------------------------===//


#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Constants.h"
#include "safecode/LoadStoreTrace.h"
#include "safecode/Utility.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"

#include <unistd.h>
#include <iostream>

#define DEBUG_TYPE "safecode"


namespace llvm {

char LoadStoreTrace::ID = 0;

static RegisterPass<LoadStoreTrace>
X ("loadstoretrace", "Instrument loads and stores");

// Pass Statistics
namespace {
  STATISTIC (LoadInsts, "Number of Instrumented Loads");
  STATISTIC (StoreInsts, "Number of Instrumented Stores");
}

bool 
LoadStoreTrace::runOnModule (Module &M) {
    
  // Get the Module Path
  char modulepath[1024];

  if (getcwd(modulepath, sizeof(modulepath)) == NULL)
      perror("getcwd() error");
 
  std::string modulepathstring(modulepath);  
  
  // Get the Module Identifier.
  std::string ModuleID = M.getModuleIdentifier();
  // std::cout << ModuleID << "\n";
  modulepathstring += "/";
  modulepathstring += ModuleID;
  // std::cout << cwdstring << "\n";

  // Create the declarations for the runtime functions.
  // Return type.
  Type *VoidTy = Type::getVoidTy(M.getContext());
 
  // Types of the arguments
  std::vector<Type *> ArgTypes;
  ArgTypes.push_back(getVoidPtrType(M.getContext()));
  ArgTypes.push_back(getVoidPtrType(M.getContext()));
  ArgTypes.push_back(getVoidPtrType(M.getContext()));
  ArgTypes.push_back(IntegerType::getInt32Ty(M.getContext()));
  ArgTypes.push_back(IntegerType::getInt32Ty(M.getContext()));

  

  //Function Type
  FunctionType *TraceTy = FunctionType::get(VoidTy, ArgTypes, false);

  // Create the functions.
  TraceLoadFunc = dyn_cast<Function>(M.getOrInsertFunction("trace_load", TraceTy));

  TraceStoreFunc = dyn_cast<Function>(M.getOrInsertFunction("trace_store", TraceTy));
 
  // The third argument of the function is a string. 
  // Therefore, we create a global variable containing the Module Identifier.
  Constant *ModIDInit = ConstantDataArray::getString (M.getContext(), modulepathstring);
  ModID = new GlobalVariable (M, ModIDInit->getType(),
                                        true,
                                        GlobalValue::InternalLinkage,
                                        ModIDInit,
                                        "ModID");

  //TD = &getAnalysis<DataLayout>();
  TD = &M.getDataLayout();

  // Visit all the instructions in the module.
  visit(M);

 // Add call to atexit()
 runAtExit(M);
 


  return true;
}


void LoadStoreTrace::visitLoadInst(LoadInst &LI) {
  // Instrument a load instruction with a trace_load call.
 
  //
  // Create a list with the arguments of the trace_load function.
  // The first argument is the pool handle.
  // The second argument is the pointer to check.
  // The third argument is the Module ID.
  //
  std::vector<Value *> args;
  LLVMContext &Context = LI.getContext();

  //
  // Create a value representing the amount of memory, in bytes, that will be
  // modified.
  //
  uint64_t TypeSize=TD->getTypeStoreSize(LI.getType());
  IntegerType *IntType = IntegerType::getInt32Ty(Context);
  Value *AccessSize = ConstantInt::get(IntType, TypeSize);




  
  args.push_back(ConstantPointerNull::get(getVoidPtrType(Context)));
  args.push_back(castTo(LI.getPointerOperand(), getVoidPtrType(Context), &LI));
  args.push_back(castTo(ModID,getVoidPtrType(Context), "ModID", &LI));  
  args.push_back(ConstantInt::get(IntegerType::getInt32Ty(Context), 82));
  args.push_back(AccessSize);


  // Create a call to trace_load.
  CallInst *CI = CallInst::Create(TraceLoadFunc, args, "", &LI); 

  // Update statistics.
  ++LoadInsts;


  return;
}

void LoadStoreTrace::visitStoreInst(StoreInst &SI) {
  // Instrument a store instruction with a trace_store call.

  //
  // Create a list with the arguments of the trace_store function.
  // The first argument is the pool handle.
  // The second argument is the pointer to check.
  // The third argument is the Module ID.
  //
  std::vector<Value *> args;
  LLVMContext &Context = SI.getContext();

  //
  // Create a value representing the amount of memory, in bytes, that will be
  // modified.
  //
  uint64_t TypeSize=TD->getTypeStoreSize(SI.getOperand(0)->getType());
  IntegerType *IntType = IntegerType::getInt32Ty(Context);
  Value *AccessSize = ConstantInt::get(IntType, TypeSize);


  
  args.push_back(ConstantPointerNull::get(getVoidPtrType(Context)));
  args.push_back(castTo(SI.getPointerOperand(), getVoidPtrType(Context), &SI));
  args.push_back(castTo(ModID,getVoidPtrType(Context), "ModID", &SI));
  args.push_back(ConstantInt::get(IntegerType::getInt32Ty(Context), 87));
  args.push_back(AccessSize);


  // Create a call to trace_store.
  CallInst *CI = CallInst::Create(TraceStoreFunc, args, "", &SI); 



  // Update statistics.
  ++StoreInsts;

  return;

}



void LoadStoreTrace::runAtExit(Module &M){
  for (Module::iterator I = M.begin(), E = M.end(); I != E; ++I){
    if(I->getName().equals("main")){
      //llvm::errs() << "Function: " << I->getName() << "\n";
      LLVMContext &Context = M.getContext();
      Type *VoidTy = Type::getVoidTy(Context);
      Constant *AtExitFn = M.getOrInsertFunction("call_atexit", VoidTy, (Type *)0);
      BasicBlock *BB = &I->getEntryBlock();
       Instruction *I = &BB->front();
      CallInst::Create(AtExitFn, "", I);
    }
  }
  return;
}

}