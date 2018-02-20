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

// Initialization parameters that are read when the STM32 processor boots.
//
// The first 32-bit word defines the starting address of the top of the stack,
// and the next is the address of the function that's called when the
// processor is first started. In this example, the rest of the possible vectors
// are left as infinite loops, but these could be replaced with useful routines
// if needed. For more examples, see:
// https://github.com/trebisky/stm32f103/blob/master/blink1/locore.s
// https://github.com/leaflabs/maple-bootloader/blob/master/stm32_lib/c_only_startup.s
//
// The .o output of this file must be linked as the first object file, so that
// .bin has this table of vectors right at the start. If you don't ensure this,
// you'll see weird hangs when you reset the device.	

.cpu cortex-m3
.thumb

.word 0x20005000     // Stack start address, at the top of SRAM.
.word _on_reset_asm  // Reset.
.word _infinite_loop // NMI.
.word _infinite_loop // Hard Fault.
.word _infinite_loop // MM Fault.
.word _infinite_loop // Bus Fault.
.word _infinite_loop // Usage Fault.
.word _infinite_loop // Unused.
.word _infinite_loop // Unused.
.word _infinite_loop // Unused.
.word _infinite_loop // Unused.
.word _infinite_loop // SV call.
.word _infinite_loop // Unused.
.word _infinite_loop // Unused.
.word _infinite_loop // PendSV.
.word OnSysTick      // SysTick.
.word _infinite_loop // IRQ0.
.word _infinite_loop // IRQ1.
.word _infinite_loop //
.word _infinite_loop //

_infinite_loop:   b _infinite_loop

.thumb_func
_on_reset_asm:
// OnReset() should be a C-linkage function symbol that's defined elsewhere in
// your program. It works like main() in a non-embedded context, but with no
// arguments.
  bl OnReset
  b .
