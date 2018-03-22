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

#include "strings.h"

// Reverses a zero-terminated string in-place.
char* ReverseStringInPlace(char* start, char* end) {
  char* p1 = start;
  char* p2 = end - 1;
  while (p1 < p2) {
    char tmp = *p1;
    *p1++ = *p2;
    *p2-- = tmp;
  }
  return start;
}

// Populates the provided buffer with an ASCII representation of the number.
char* FastUInt32ToBufferLeft(uint32_t i, char* buffer, int base) {
  char* start = buffer;
  do {
    int32_t digit = i % base;
    char character;
    if (digit < 10) {
      character = '0' + digit;
    } else {
      character = 'a' + (digit - 10);
    }
    *buffer++ = character;
    i /= base;
  } while (i > 0);
  *buffer = 0;
  ReverseStringInPlace(start, buffer);
  return buffer;
}

// Populates the provided buffer with an ASCII representation of the number.
char* FastInt32ToBufferLeft(int32_t i, char* buffer) {
  uint32_t u = i;
  if (i < 0) {
    *buffer++ = '-';
    u = -u;
  }
  return FastUInt32ToBufferLeft(u, buffer, 10);
}

// Appends a string to a string, in-place. You need to pass in the maximum
// string
// length as the second argument.
char* StrCatStr(char* main, int main_max_length, char* to_append) {
  char* current = main;
  while (*current != 0) {
    ++current;
  }
  char* current_end = main + (main_max_length - 1);
  while ((*to_append != 0) && (current < current_end)) {
    *current = *to_append;
    ++current;
    ++to_append;
  }
  *current = 0;
}

// Converts a number to a string and appends it to another.
void StrCatInt32(char* main, int main_max_length, int32_t number) {
  char number_string[kFastToBufferSize];
  FastInt32ToBufferLeft(number, number_string);
  StrCatStr(main, main_max_length, number_string);
}

// Converts a number to a string and appends it to another.
void StrCatUInt32(char* main, int main_max_length, uint32_t number, int base) {
  char number_string[kFastToBufferSize];
  FastUInt32ToBufferLeft(number, number_string, base);
  StrCatStr(main, main_max_length, number_string);
}

void StrCpy(char* main, int main_max_length, const char* source) {
  char* current = main;
  char* current_end = main + (main_max_length - 1);
  while ((*source != 0) && (current < current_end)) {
    *current = *source;
    ++current;
    ++source;
  }
  *current = 0;
}
