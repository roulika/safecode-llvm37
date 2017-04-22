//===- TagPointersToCanonical.cpp - Tag all the pointers to canonical form ===//
//
//                          The SAFECode Compiler
//
// This file was developed by the LLVM research group and is distributed under
// the University of Illinois Open Source License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements a pass that tags all the pointers being dereferenced
// to be of canonical format (i.e., sign extended).
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "tagpointerstocanonical"

#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"

#include "safecode/TagPointersToCanonical.h"

using namespace llvm;

char TagPointersToCanonical::ID = 0;
static RegisterPass<TagPointersToCanonical> X("tagPointersToCanonical",
                                              "Tag pointers to canonical format");

//
// Method: insertRuntimeCheck()
//
// Description:
//  This method inserts a runtime function call before a pointer dereference
//  instruction to make sure the pointer it is dereferencing is of the
//  canonical format (i.e., it has the valid sign extension of either all-zero
//  or all-one.
//
// Input:
//  Inst - A pointer dereference instruction which is dereferencing a pointer
//  Ptr - The pointer that is being dereferenced
//  index - the index of the pointer in the instruction's argument list
//
void
TagPointersToCanonical::insertRuntimeCheck(Instruction * Inst, Value * Ptr,
                                           unsigned index) {
  Module * M = Inst->getModule();
  Type * Int8PtrType = Type::getInt8PtrTy(M->getContext());

  // This is the runtime function call we are going to insert
  Constant * TagPtrCheck = M->getOrInsertFunction("__sc_bb_tag_ptr_check",
                                                  Int8PtrType,
                                                  Int8PtrType,
                                                  nullptr);

  // Maintain a last instruction pointer to chain instructions together
  Value * Last = Ptr;
  IRBuilder<> Builder(Inst);

  // Add wrapping pointer cast instructions if the pointer type doesn't match
  if (Int8PtrType != Ptr->getType()) {
    Value * Cast = Builder.CreatePointerCast(Last, Int8PtrType);
    Last = Cast;
  }

  // Insert the runtime function call
  CallInst * Call = Builder.CreateCall(TagPtrCheck, ArrayRef<Value *>(Last));
  Last = Call;

  // Cast back
  if (Int8PtrType != Ptr->getType()) {
    Value * Cast = Builder.CreatePointerCast(Last, Ptr->getType());
    Last = Cast;
  }

  // Change the pointer operand of the instruction
  Inst->setOperand(index, Last);
}

//
// Method: runOnModule()
//
// Description:
//  This method is called to run the pass on a module.
//
// Input:
//  M - A pointer to the module
//
// Output:
//  Whether the module was modified in this pass
//
bool
TagPointersToCanonical::runOnModule(Module & M) {
  bool modified = false;

  // Iterate over all the instructions, searching for pointer dereferences
  for (Function & F : M) {
    for (auto I = inst_begin(F), E = inst_end(F); I != E; ++I) {
      // Load instruction
      if (LoadInst * Load = dyn_cast<LoadInst>(&*I)) {
        Value * Ptr = Load->getPointerOperand();
        insertRuntimeCheck(Load, Ptr, 0);
        modified = true;
      }
      // Store instruction
      else if (StoreInst * Store = dyn_cast<StoreInst>(&*I)) {
        Value * Ptr = Store->getPointerOperand();
        insertRuntimeCheck(Store, Ptr, 1);
        modified = true;
      }
      // Atomic compare-and-exchange instruction
      else if (AtomicCmpXchgInst * CmpX = dyn_cast<AtomicCmpXchgInst>(&*I)) {
        Value * Ptr = CmpX->getPointerOperand();
        insertRuntimeCheck(CmpX, Ptr, 0);
        modified = true;
      }
      // Atomic read-modify-write instruction
      else if (AtomicRMWInst * RMW = dyn_cast<AtomicRMWInst>(&*I)) {
        Value * Ptr = RMW->getPointerOperand();
        insertRuntimeCheck(RMW, Ptr, 0);
        modified = true;
      }
    }
  }
  return modified;
}
