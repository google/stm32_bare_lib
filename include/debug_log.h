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

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// Writes a string to the OpenOCD debug console. This can take hundreds of
// milliseconds, so don't call this in performance-intensive code.
void DebugLog(char* s);

// Writes out a signed 32-bit number to the debug console.
void DebugLogInt32(int32_t i);

// Writes out an unsigned 32-bit number to the debug console.
void DebugLogUInt32(uint32_t i);

// Writes out an unsigned 32-bit number to the debug console as hex.
void DebugLogHex(uint32_t i);

// Writes out a signed 32-bit number to the debug console.
void DebugLogFloat(float i);

// An easy way of logging labeled numerical variables for debugging.
#define LOG_INT32(x)                  \
  do {                                \
    const int log_length = 64;        \
    char log[log_length];             \
    StrCpy(log, log_length, #x);      \
    StrCatStr(log, log_length, "=");  \
    StrCatInt32(log, log_length, x);  \
    StrCatStr(log, log_length, "\n"); \
    DebugLog(log);                    \
  } while (0)

#define LOG_HEX32(x)                      \
  do {                                    \
    const int log_length = 64;            \
    char log[log_length];                 \
    StrCpy(log, log_length, #x);          \
    StrCatStr(log, log_length, "=0x");    \
    StrCatUInt32(log, log_length, x, 16); \
    StrCatStr(log, log_length, "\n");     \
    DebugLog(log);                        \
  } while (0)

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // INCLUDE_DEBUG_LOG_H
