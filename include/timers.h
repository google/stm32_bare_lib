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

// Timer utilities for the "Blue Pill" STM32 board.

#ifndef INCLUDE_TIMERS_H
#define INCLUDE_TIMERS_H

#include <stdint.h>

// We know the "Blue Pill" STM32 board runs at 72 MHz.
#define CLOCK_RATE (72 * 1000 * 1000)

// There's a default SysTick handler function defined in source/timers.c that
// just increments the g_tick_count variable by default. Optionally, you can
// set g_tick_callback and it will call into a user-defined function too.
// To control the frequency, you'll need to call the CMSIS SysTick_Config().
extern uint32_t g_tick_count;
typedef void (*OnSysTickCallback)(int tick_count);
extern OnSysTickCallback g_tick_callback;

// Spin in a loop for roughly the specified number of microseconds. This is
// using instruction timing, so it's not precise, and the processor will be
// consuming power while this is running.
static inline void BusyWaitMicroseconds(int32_t us) {
  // Each loop takes very roughly one microsecond on a Blue Pill.
  volatile int32_t count = us;
  for (; count > 0; --count)
    ;
}

#endif  // INCLUDE_TIMERS_H
