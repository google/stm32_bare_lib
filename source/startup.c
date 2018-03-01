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
extern uint32 _ld_data_load_source;
extern uint32 _ld_data_load_dest_stop;
extern uint32 _ld_data_load_dest_start;
extern uint32 _ld_bss_data_start;
extern uint32 _ld_bss_data_stop;

// Current main entry point. TODO(aselle): change this to main()
extern void OnReset();

// Used for rdimon semihosting (printf debugging)
extern void initialise_monitor_handles(void);

// Unhandled exceptions
__attribute__ ((section(".text.interrupt_handler")))
void _hard_fault_handler() {
  while(1);
}
__attribute__ ((section(".text.interrupt_handler")))
void _mem_fault_handler() {
  while(1);
}
__attribute__ ((section(".text.interrupt_handler")))
void _bus_fault_handler() {
  while(1);
}


// Copy read-write static initialized memory from flash to ram.
void _load_data_from_flash() {
  uint32* src = &_ld_data_load_source;
  for(uint32* dest = &_ld_data_load_dest_start; dest != &_ld_data_load_dest_stop;)
    *dest++ = *src++;
}

// "Zero" unitialized data with DEADBEEF
void _zero_initialize_bss_data() {
  for(uint32* dest = &_ld_bss_data_start; dest != &_ld_bss_data_stop; dest++)
    *dest = 0xDEADBEEF;
}
// Put this in the .text.reset segment so the linker script can
// make sure this is not pruned.
__attribute__ ((section(".text.reset")))
int _main() {
  _load_data_from_flash();
  _zero_initialize_bss_data();
  //  initialise_monitor_handles();
  OnReset();
}
