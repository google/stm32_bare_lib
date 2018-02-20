/* Copyright 2017 Pete Warden. All Rights Reserved.
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

// Reverses a zero-terminated string in-place.
static inline char* ReverseStringInPlace(char* start, char* end) {
  char *p1 = start;
  char *p2 = end - 1;
  while (p1 < p2) {
    char tmp = *p1;
    *p1++ = *p2;
    *p2-- = tmp;
  }
  return start;
}

// Populates the provided buffer with an ASCII representation of the number.
static inline char* FastUInt32ToBufferLeft(uint32_t i, char* buffer) {
  char* start = buffer;
  do {
    *buffer++ = ((i % 10) + '0');
    i /= 10;
  } while (i > 0);
  *buffer = 0;
  ReverseStringInPlace(start, buffer);
  return buffer;
}

// All input buffers to the number conversion functions must be this long.
static const int kFastToBufferSize = 32;

// Populates the provided buffer with an ASCII representation of the number.
static inline char* FastInt32ToBufferLeft(int32_t i, char* buffer) {
  uint32_t u = i;
  if (i < 0) {
    *buffer++ = '-';
    u = -u;
  }
  return FastUInt32ToBufferLeft(u, buffer);
}

// Appends a string to a string, in-place. You need to pass in the maximum string
// length as the second argument.
static inline char* StrCatStr(char* main, int main_max_length, char* to_append) {
  char* current = main;
  while (*current != 0) {
    ++current;
  }
  const int current_length = current - main;
  char* current_end = current + (main_max_length - 1);
  while ((*to_append != 0) && (current < current_end)) {
    *current = *to_append;
    ++current;
    ++to_append;
  }
  *current = 0;
}

// Converts a number to a string and appends it to another.
static inline char* StrCatInt32(char* main, int main_max_length, int32_t number) {
  char number_string[kFastToBufferSize];
  FastInt32ToBufferLeft(number, number_string);
  StrCatStr(main, main_max_length, number_string);
}

// Converts a number to a string and appends it to another.
static inline char* StrCatUInt32(char* main, int main_max_length, uint32_t number) {
  char number_string[kFastToBufferSize];
  FastUInt32ToBufferLeft(number, number_string);
  StrCatStr(main, main_max_length, number_string);
}

#endif  // INCLUDE_STRINGS_H
