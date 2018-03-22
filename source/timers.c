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

#include "timers.h"

#include "debug_log.h"

uint32_t g_tick_count = 0;
OnSysTickCallback g_tick_callback = 0;

// We've set up boot.s to call this function when there's a systick event.
// The frequency can be controlled by SysTick_Config.
void OnSysTick() {
  g_tick_count += 1;
  // DebugLog("Foo\n");
  //*(char*)(0) = 23;
  // if (g_tick_callback) {
  //  g_tick_callback(g_tick_count);
  //}
}
