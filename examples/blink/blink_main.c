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

// This example shows how to write a minimal "Blue Pill" program that blinks the
// LED continuously.

#include "led.h"
#include "timers.h"

// You need a function named "main" in your program to act like "main" in
// traditional C. This will be called when the processor starts up.
void main(void) {
  // Before the LED can be accessed, the device registers need to be set up.
  LedInit();
  // We'll keep looping forever, turning the LED on and off.
  while (1) {
    LedOn();
    // This delay function is inefficient because it has the processor spin in
    // a loop executing no-ops, rather than going to sleep and saving power.
    // It's simpler to call though, so we'll use it for this example.
    BusyWaitMicroseconds(200 * 1000);
    LedOff();
    BusyWaitMicroseconds(200 * 1000);
  }
}
