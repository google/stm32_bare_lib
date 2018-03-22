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

#include "core_stm32.h"
#include "debug_log.h"

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
__attribute__((section(".text.interrupt_handler"))) void _infinite_loop() {
  while (1)
    ;
}

// Overridable interrupt handlers
__attribute__((weak)) void MemFaultHandler() { _infinite_loop(); }
__attribute__((weak)) void BusFaultHandler() { _infinite_loop(); }
__attribute__((weak)) void OnAdcInterrupt() { _infinite_loop(); }
__attribute__((weak)) void OnDma1Channel1Interrupt() { _infinite_loop(); }
__attribute__((weak)) void OnDma1Channel2Interrupt() { _infinite_loop(); }
__attribute__((weak)) void OnDma1Channel3Interrupt() { _infinite_loop(); }
__attribute__((weak)) void OnDma1Channel4Interrupt() { _infinite_loop(); }
__attribute__((weak)) void OnDma1Channel5Interrupt() { _infinite_loop(); }
__attribute__((weak)) void OnDma1Channel6Interrupt() { _infinite_loop(); }
__attribute__((weak)) void OnDma1Channel7Interrupt() { _infinite_loop(); }

// We need this assembler to store the register information for debugging.
void HardFaultHandlerASM(void) {
  __asm(
      "TST lr, #4\n"  // Test for MSP or PSP
      "ITE EQ\n"
      "MRSEQ r0, MSP\n"
      "MRSNE r0, PSP\n"
      "B HardFaultHandler\n");
}

__attribute__((weak)) void HardFaultHandler(uint32_t stack[]) {
  LOG_HEX32(SCB->HFSR);
  if ((SCB->HFSR & SCB_HFSR_FORCED_Msk) != 0) {
    LOG_HEX32(SCB->CFSR);
    LOG_HEX32(SCB_CFSR_IBUSERR_Msk);
    if (SCB->CFSR & SCB_CFSR_DIVBYZERO_Msk) {
      DebugLog("DIVBYZERO Fault\n");
    } else if (SCB->CFSR & SCB_CFSR_UNALIGNED_Msk) {
      DebugLog("UNALIGNED Fault\n");
    } else if (SCB->CFSR & SCB_CFSR_NOCP_Msk) {
      DebugLog("NOCP fault\n");
    } else if (SCB->CFSR & SCB_CFSR_INVPC_Msk) {
      DebugLog("INVPC fault\n");
    } else if (SCB->CFSR & SCB_CFSR_INVSTATE_Msk) {
      DebugLog("INVSTATE fault\n");
    } else if (SCB->CFSR & SCB_CFSR_UNDEFINSTR_Msk) {
      DebugLog("UNDEFINSTR fault\n");
    } else if (SCB->CFSR & SCB_CFSR_IBUSERR_Msk) {
      DebugLog("IBUSERR fault\n");
    } else {
      DebugLog("Unrecognized fault\n");
    }
  }
  volatile uint32_t r0 = stack[0];
  volatile uint32_t r1 = stack[1];
  volatile uint32_t r2 = stack[2];
  volatile uint32_t r3 = stack[3];
  volatile uint32_t r12 = stack[4];
  volatile uint32_t lr = stack[5];
  volatile uint32_t pc = stack[6];
  volatile uint32_t psr = stack[7];
  LOG_HEX32(r0);
  LOG_HEX32(r1);
  LOG_HEX32(r2);
  LOG_HEX32(r3);
  LOG_HEX32(r12);
  LOG_HEX32(lr);
  LOG_HEX32(pc);
  LOG_HEX32(psr);
  // Trigger the debugger, if it's available.
  __asm volatile("BKPT #01");
  _infinite_loop();
}

// Copy read-write static initialized memory from flash to ram.
void _load_data_from_flash() {
  // Volatile is important here to make sure the loop isn't optimized away.
  volatile uint32* src = &_ld_data_load_source;
  for (volatile uint32* dest = &_ld_data_load_dest_start;
       dest != &_ld_data_load_dest_stop;)
    *dest++ = *src++;
}

// "Zero" uninitialized data with DEADBEEF.
void _zero_initialize_bss_data() {
  // Volatile is important here to make sure the loop isn't optimized away.
  for (volatile uint32* dest = &_ld_bss_data_start; dest != &_ld_bss_data_stop;
       dest++)
    *dest = 0xDEADBEEF;
}
// Put this in the .text.reset segment so the linker script can
// make sure this is not pruned.
__attribute__((section(".text.reset"))) void _main() {
  _load_data_from_flash();
  _zero_initialize_bss_data();
  //  initialise_monitor_handles();
  main();
  _infinite_loop();
}

// Interrupt vector
typedef void (*interrupt_fn)();
__attribute__((section(".interrupt_vector")))
// This needs to be marked as volatile, or gcc can strip it out during
// optimization.
static volatile interrupt_fn interrupts[] = {
    (interrupt_fn)&_ld_stack_end_addr,  // Stack start end size (from linker
                                        // script)
    _main,                              // Reset.
    _infinite_loop,                     // NMI.
    HardFaultHandlerASM,                // Hard Fault.
    MemFaultHandler,                    // MM Fault.
    BusFaultHandler,                    // Bus Fault.
    _infinite_loop,                     // Usage Fault.
    _infinite_loop,                     // Unused.
    _infinite_loop,                     // Unused.
    _infinite_loop,                     // Unused.
    _infinite_loop,                     // Unused.
    _infinite_loop,                     // SV call.
    _infinite_loop,                     // Unused.
    _infinite_loop,                     // Unused.
    _infinite_loop,                     // PendSV.
    OnSysTick,                          // SysTick.
    _infinite_loop,                     // IRQ0.
    _infinite_loop,                     // IRQ1.
    _infinite_loop,                     // IRQ2.
    _infinite_loop,                     // IRQ3.
    _infinite_loop,                     // IRQ4.
    _infinite_loop,                     // IRQ5.
    _infinite_loop,                     // IRQ6.
    _infinite_loop,                     // IRQ7.
    _infinite_loop,                     // IRQ8.
    _infinite_loop,                     // IRQ9.
    _infinite_loop,                     // IRQ10.
    OnDma1Channel1Interrupt,            // IRQ11.
    OnDma1Channel2Interrupt,            // IRQ12.
    OnDma1Channel3Interrupt,            // IRQ13.
    OnDma1Channel4Interrupt,            // IRQ14.
    OnDma1Channel5Interrupt,            // IRQ15.
    OnDma1Channel6Interrupt,            // IRQ16.
    OnDma1Channel7Interrupt,            // IRQ17.
    OnAdcInterrupt,                     // IRQ18.
    _infinite_loop,                     // IRQ19.
    _infinite_loop,                     // IRQ20.
    _infinite_loop,                     // IRQ21.
    _infinite_loop,                     // IRQ22.
    _infinite_loop,                     // IRQ23.
    _infinite_loop,                     // IRQ24.
    _infinite_loop,                     // IRQ25.
    _infinite_loop,                     // IRQ26.
    _infinite_loop,                     // IRQ27.
    _infinite_loop,                     // IRQ28.
    _infinite_loop,                     // IRQ29.
    _infinite_loop,                     // IRQ30.
    _infinite_loop,                     // IRQ31.
    _infinite_loop,                     // IRQ32.
    _infinite_loop,                     // IRQ33.
    _infinite_loop,                     // IRQ34.
    _infinite_loop,                     // IRQ35.
    _infinite_loop,                     // IRQ36.
    _infinite_loop,                     // IRQ37.
    _infinite_loop,                     // IRQ38.
    _infinite_loop,                     // IRQ39.
    _infinite_loop,                     // IRQ40.
    _infinite_loop,                     // IRQ41.
    _infinite_loop,                     // IRQ42.
    _infinite_loop,                     // IRQ43.
    _infinite_loop,                     // IRQ44.
    _infinite_loop,                     // IRQ45.
    _infinite_loop,                     // IRQ46.
    _infinite_loop,                     // IRQ47.
    _infinite_loop,                     // IRQ48.
    _infinite_loop,                     // IRQ49.
    _infinite_loop,                     // IRQ50.
    _infinite_loop,                     // IRQ51.
    _infinite_loop                      // IRQ52.
};
