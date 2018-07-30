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

// An example of writing a string to a connected debug system using Arm's
// semihosting system. If you have openocd running and 'arm semihosting enable'
// has been run, you should see "Hello World!" in the openocd console logs.

#include "debug_log.h"

// test static initialization
static char foo_static[] = {'s', 't', 'a', 't', 'i', 'c', '\n'};
static long uninitialized_values[4];
static float float_static = -23.232323f;

void main(void) {
  DebugLog("Starting static initialization test\n");
  while (1) {
    // Busy wait and keep printing
    for (long k = 0; k < 100000; k++)
      ;
    DebugLog(foo_static);
    DebugLogHex(uninitialized_values[0]);
    DebugLog("\n");
    DebugLogFloat(float_static);
    DebugLog("\n");
  }
}
