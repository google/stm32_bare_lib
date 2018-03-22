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

// Tests the speed of arithmetic operations and outputs the results to the
// debugger logs.

#include "core_stm32.h"
#include "debug_log.h"
#include "timers.h"

// The function that's called when the chip is started.
void main(void) {
  // Set up SysTick to call back every millisecond.
  g_tick_count = 0;
  SysTick_Config(CLOCK_RATE / 10000);
  DebugLog("Benchmarking started\n");

  volatile int32_t a = 42;
  volatile int32_t b = 23;
  volatile int32_t total = 0;
  const int iters = 10000;
  const int32_t start_time = g_tick_count;
  for (int i = 0; i < iters; ++i) {
    total += a * b;
  }
  const int32_t end_time = g_tick_count;
  const int32_t duration = end_time - start_time;

  DebugLog("Multiply-add of ");
  DebugLogInt32(iters);
  DebugLog(" elements took ");
  DebugLogInt32(duration);
  DebugLog("ms\n");
}
