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

// Simple string functions.

#ifndef INCLUDE_STRINGS_H
#define INCLUDE_STRINGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// Reverses a zero-terminated string in-place.
char* ReverseStringInPlace(char* start, char* end);

// Populates the provided buffer with an ASCII representation of the number.
char* FastUInt32ToBufferLeft(uint32_t i, char* buffer, int base);

// All input buffers to the number conversion functions must be this long.
static const int kFastToBufferSize = 48;

// Populates the provided buffer with an ASCII representation of the number.
char* FastInt32ToBufferLeft(int32_t i, char* buffer);

// Populates the provided buffer with ASCII representation of the float number.
// Avoids the use of any floating point instructions (since these aren't
// supported on many microcontrollers) and as a consequence prints values with
// power-of-two exponents.
char* FastFloatToBufferLeft(float i, char* buffer);

// Appends a string to a string, in-place. You need to pass in the maximum
// string length as the second argument.
char* StrCatStr(char* main, int main_max_length, char* to_append);

// Converts a number to a string and appends it to another.
char* StrCatInt32(char* main, int main_max_length, int32_t number);

// Converts a number to a string and appends it to another.
char* StrCatUInt32(char* main, int main_max_length, uint32_t number, int base);

char* StrCpy(char* main, int main_max_length, const char* source);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // INCLUDE_STRINGS_H
