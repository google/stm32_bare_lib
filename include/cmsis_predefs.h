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

// Should not be included separately, only contains type and macro definitions
// that are needed before including the CMSIS M3 header. Include "core_stm32.h"
// instead.
#ifndef INCLUDE_CMSIS_PREDEFS_H
#define INCLUDE_CMSIS_PREDEFS_H

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// We need a couple of STM32-specific definitions before we include the main M3
// header file.
typedef enum IRQn {
  NonMaskableInt_IRQn = -14,
  MemoryManagement_IRQn = -12,
  BusFault_IRQn = -11,
  UsageFault_IRQn = -10,
  SVCall_IRQn = -5,
  DebugMonitor_IRQn = -4,
  PendSV_IRQn = -2,
  SysTick_IRQn = -1,
  WWDG_IRQn = 0,
  PVD_IRQn = 1,
  TAMPER_IRQn = 2,
  RTC_IRQn = 3,
  FLASH_IRQn = 4,
  RCC_IRQn = 5,
  EXTI0_IRQn = 6,
  EXTI1_IRQn = 7,
  EXTI2_IRQn = 8,
  EXTI3_IRQn = 9,
  EXTI4_IRQn = 10,
  DMA1_Channel1_IRQn = 11,
  DMA1_Channel2_IRQn = 12,
  DMA1_Channel3_IRQn = 13,
  DMA1_Channel4_IRQn = 14,
  DMA1_Channel5_IRQn = 15,
  DMA1_Channel6_IRQn = 16,
  DMA1_Channel7_IRQn = 17,
} IRQn_Type;
#define __NVIC_PRIO_BITS (2)

// We want to use the standard ARM SysTick implementation, so indicate there's
// no manufacturer-supplied version.
#define __Vendor_SysTickConfig (0)

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // INCLUDE_CMSIS_PREDEFS_H
