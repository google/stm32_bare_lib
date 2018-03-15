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

typedef long uint32;

// Symbols initialized by the linker script
// zero data initialization
extern uint32 _ld_stack_end_addr;
extern uint32 _ld_data_load_source;
extern uint32 _ld_data_load_dest_stop;
extern uint32 _ld_data_load_dest_start;
extern uint32 _ld_bss_data_start;
extern uint32 _ld_bss_data_stop;

extern void main();
extern void OnSysTick();

// Used for rdimon semihosting (printf debugging)
extern void initialise_monitor_handles(void);

// Unhandled exceptions
__attribute__ ((section(".text.interrupt_handler")))
void _infinite_loop() {
  while(1);
}

// Overridable interrupt handlers
__attribute__((weak))  void HardFaultHandler() {_infinite_loop();}
__attribute__((weak))  void MemFaultHandler() {_infinite_loop();}
__attribute__((weak))  void BusFaultHandler() {_infinite_loop();}
__attribute__((weak))  void OnAdcInterrupt() {_infinite_loop();}
__attribute__((weak))  void OnDma1Channel1Interrupt() {_infinite_loop();}
__attribute__((weak))  void OnDma1Channel2Interrupt() {_infinite_loop();}
__attribute__((weak))  void OnDma1Channel3Interrupt() {_infinite_loop();}
__attribute__((weak))  void OnDma1Channel4Interrupt() {_infinite_loop();}
__attribute__((weak))  void OnDma1Channel5Interrupt() {_infinite_loop();}
__attribute__((weak))  void OnDma1Channel6Interrupt() {_infinite_loop();}
__attribute__((weak))  void OnDma1Channel7Interrupt() {_infinite_loop();}

// Copy read-write static initialized memory from flash to ram.
void _load_data_from_flash() {
  // Volatile is important here to make sure the loop isn't optimized away.
  volatile uint32* src = &_ld_data_load_source;
  for(volatile uint32* dest = &_ld_data_load_dest_start; dest != &_ld_data_load_dest_stop;)
    *dest++ = *src++;
}

// "Zero" uninitialized data with DEADBEEF.
void _zero_initialize_bss_data() {
  // Volatile is important here to make sure the loop isn't optimized away.
  for(volatile uint32* dest = &_ld_bss_data_start; dest != &_ld_bss_data_stop; dest++)
    *dest = 0xDEADBEEF;
}
// Put this in the .text.reset segment so the linker script can
// make sure this is not pruned.
__attribute__ ((section(".text.reset")))
void _main() {
  _load_data_from_flash();
  _zero_initialize_bss_data();
  //  initialise_monitor_handles();
  main();
  _infinite_loop();
}

// Interrupt vector
typedef void (*interrupt_fn)();
__attribute__ ((section(".interrupt_vector")))
// This needs to be marked as volatile, or gcc can strip it out during optimization.
static volatile interrupt_fn interrupts[] = {
  (interrupt_fn)&_ld_stack_end_addr, // Stack start end size (from linker script)
  _main, // Reset.
  _infinite_loop, // NMI.
  HardFaultHandler, // Hard Fault.
  MemFaultHandler, // MM Fault.
  BusFaultHandler, // Bus Fault.
  _infinite_loop, // Usage Fault.
  _infinite_loop, // Unused.
  _infinite_loop, // Unused.
  _infinite_loop, // Unused.
  _infinite_loop, // Unused.
  _infinite_loop, // SV call.
  _infinite_loop, // Unused.
  _infinite_loop, // Unused.
  _infinite_loop, // PendSV.
  OnSysTick     , // SysTick.
  _infinite_loop, // IRQ0.
  _infinite_loop, // IRQ1.
  _infinite_loop, // IRQ2.
  _infinite_loop, // IRQ3.
  _infinite_loop, // IRQ4.
  _infinite_loop, // IRQ5.
  _infinite_loop, // IRQ6.
  _infinite_loop, // IRQ7.
  _infinite_loop, // IRQ8.
  _infinite_loop, // IRQ9.
  _infinite_loop, // IRQ10.
  OnDma1Channel1Interrupt, // IRQ11.
  OnDma1Channel2Interrupt, // IRQ12.
  OnDma1Channel3Interrupt, // IRQ13.
  OnDma1Channel4Interrupt, // IRQ14.
  OnDma1Channel5Interrupt, // IRQ15.
  OnDma1Channel6Interrupt, // IRQ16.
  OnDma1Channel7Interrupt, // IRQ17.
  OnAdcInterrupt, // IRQ18.
  _infinite_loop, // IRQ19.
  _infinite_loop, // IRQ20.
  _infinite_loop, // IRQ21.
  _infinite_loop, // IRQ22.
  _infinite_loop, // IRQ23.
  _infinite_loop, // IRQ24.
  _infinite_loop, // IRQ25.
  _infinite_loop, // IRQ26.
  _infinite_loop, // IRQ27.
  _infinite_loop, // IRQ28.
  _infinite_loop, // IRQ29.
  _infinite_loop, // IRQ30.
  _infinite_loop, // IRQ31.
  _infinite_loop, // IRQ32.
  _infinite_loop, // IRQ33.
  _infinite_loop, // IRQ34.
  _infinite_loop, // IRQ35.
  _infinite_loop, // IRQ36.
  _infinite_loop, // IRQ37.
  _infinite_loop, // IRQ38.
  _infinite_loop, // IRQ39.
  _infinite_loop, // IRQ40.
  _infinite_loop, // IRQ41.
  _infinite_loop, // IRQ42.
  _infinite_loop, // IRQ43.
  _infinite_loop, // IRQ44.
  _infinite_loop, // IRQ45.
  _infinite_loop, // IRQ46.
  _infinite_loop, // IRQ47.
  _infinite_loop, // IRQ48.
  _infinite_loop, // IRQ49.
  _infinite_loop, // IRQ50.
  _infinite_loop, // IRQ51.
  _infinite_loop // IRQ52.
};
