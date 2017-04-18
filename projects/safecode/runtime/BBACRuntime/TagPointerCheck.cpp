//===- TagPointerCheck.cpp - Runtime functions for TagPointersToCanonical -===//
//
//                          The SAFECode Compiler
//
// This file was developed by the LLVM research group and is distributed under
// the University of Illinois Open Source License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the runtime functions for the TagPointersToCanonical
// pass that tags all the pointers to be of canonical format (i.e., sign
// extended).
//
//===----------------------------------------------------------------------===//

#include <sys/types.h>

extern "C" {

//
// Function: __sc_bb_tag_ptr_check()
//
// Description:
//  This function is called in runtime to tag all the pointers that are going
//  to be dereferenced to have a valid sign extension.
//
// Input:
//  ptr - A pointer that is being dereferenced
//
// Output:
//  The pointer that has a valid sign extension
//
void *
__sc_bb_tag_ptr_check(void * ptr) {
  static const uintptr_t SignMask = 0x0000800000000000ul;
  static const uintptr_t ExtMask = 0xffff000000000000ul;

  uintptr_t val = (uintptr_t)ptr;
  uintptr_t sign = val & SignMask;
  uintptr_t ext = val & ExtMask;

  // Kernel space pointer
  if (sign) {
    if (ext != ExtMask) {
      val |= ExtMask;
    }
  }
  // User space pointer
  else {
    if (ext != 0) {
      val &= ~ExtMask;
    }
  }
  return (void *)val;
}

} // end of extern C
