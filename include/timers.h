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

// Timer utilities for the "Blue Pill" STM32 board.

#ifndef INCLUDE_TIMERS_H
#define INCLUDE_TIMERS_H

#include <stdint.h>

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
