/* Copyright 2018 Google Inc. All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

// Output strings to a debug console, using semihosting.
// You'll need to enable this option in the OpenOCD configuration using:
// arm semihosting enable

#ifndef INCLUDE_DEBUG_LOG_H
#define INCLUDE_DEBUG_LOG_H

#include <stdint.h>

#include "strings.h"

// Writes a string to the OpenOCD debug console. This can take hundreds of
// milliseconds, so don't call this in performance-intensive code.
static inline void DebugLog(char* s) {
  asm("mov r0, #0x04\n" // SYS_WRITE0
      "mov r1, %[str]\n"
      "bkpt #0xAB\n"
      :
      : [str] "r"(s)
      : "r0", "r1");
}

// Writes out a signed 32-bit number to the debug console.
static inline void DebugLogInt32(int32_t i) {
  char number_string[kFastToBufferSize];
  number_string[0] = 'b';
  number_string[1] = 0;
  FastInt32ToBufferLeft(i, number_string, 10);
  DebugLog(number_string);
}

// Writes out an unsigned 32-bit number to the debug console.
static inline void DebugLogUInt32(uint32_t i) {
  char number_string[kFastToBufferSize];
  FastUInt32ToBufferLeft(i, number_string, 10);
  DebugLog(number_string);
}

// Writes out an unsigned 32-bit number to the debug console as hex.
static inline void DebugLogHex(uint32_t i) {
  char number_string[kFastToBufferSize];
  FastUInt32ToBufferLeft(i, number_string, 16);
  DebugLog(number_string);
}

#endif  // INCLUDE_DEBUG_LOG_H
