//===- TagPointersToCanonical.h - Tag all the pointers to canonical form --===//
//
//                          The SAFECode Compiler
//
// This file was developed by the LLVM research group and is distributed under
// the University of Illinois Open Source License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a pass that tags all the pointers to be of canonical
// format (i.e., sign extended).
//
//===----------------------------------------------------------------------===//

#include "llvm/Pass.h"
#include "llvm/IR/Module.h"

namespace llvm {

//
// Pass: TagPointersToCanonical
//
// Description:
//   This pass tags all the pointers to be of canonical format (i.e., sign
//   extended).
//
class TagPointersToCanonical : public ModulePass {
public:
  static char ID;
  TagPointersToCanonical() : ModulePass(ID) {}
  const char * getPassName() const override {
    return "Tag Pointers to Canonical";
  }
  virtual bool runOnModule(Module & M) override;
private:
  void insertRuntimeCheck(Instruction * Inst, Value * Ptr);
}; // end of class TagPointersToCanonical

} // end namespace llvm
