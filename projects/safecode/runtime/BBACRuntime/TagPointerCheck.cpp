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
// Function: is_power_of_2()
//
// Description:
//  This function determines whether a number is a power of 2.
//
// Input:
//  size - A number
//
// Output:
//  A non-zero value if @size is a power of 2, zero otherwise
//
static inline int
is_power_of_2(size_t size) {
  return size != 0 && (size & (size - 1)) == 0;
}

//
// Function: get_obj_size_exp()
//
// Description:
//  This function gets the log-2-based exponent of the size of a memory
//  object from a pointer value.
//
// Input:
//  ptr_val - The value of a pointer pointing to the memory object
//
// Output:
//  The log-2-based exponent
//
static inline size_t
get_obj_size_exp(uintptr_t ptr_val) {
  static const int SizeShift = 48;
  static const uintptr_t SizeMask = 0x003f000000000000ul;

  return (ptr_val & SizeMask) >> SizeShift;
}

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

//
// Function: __sc_bb_tag_ptr_size()
//
// Description:
//  This function tags the size information (log-2-based exponent) of a
//  memory object into the first 6 bits of a pointer's sign extension.
//
// Input:
//  ptr - A pointer pointing to the memory object
//  size - The size of the memory object (must be a power of 2)
//
// Output:
//  The tagged pointer
//
void *
__sc_bb_tag_ptr_size(void * ptr, size_t size) {
  static const int SizeShift = 48;
  static const uintptr_t SizeMask = 0x003f000000000000ul;

  // Don't tag the pointer if the size is not a power of 2
  if (!is_power_of_2(size)) {
    return ptr;
  }

  // Get exponent
  size_t exp = 0;
  while (size > 1) {
    size >>= 1;
    ++exp;
  }

  // Do tag the pointer
  uintptr_t val = (uintptr_t)ptr;
  val &= ~SizeMask;               // Clear the size bits
  val |= exp << SizeShift;        // Set the size bits
  return (void *)val;
}

//
// Function: __sc_bb_extract_obj_size()
//
// Description:
//  This function extracts the size of a memory object from a tagged pointer
//  pointing to that memory object.
//
// Input:
//  ptr - The tagged pointer
//
// Output:
//  The size of the memory object
//
uint64_t
__sc_bb_extract_obj_size(void * ptr) {
  return 1ul << get_obj_size_exp((uintptr_t)ptr);
}

} // end of extern C
