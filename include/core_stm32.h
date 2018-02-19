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

// Provides useful definitions for processor-specific parts of the STM32
// platform, such as device register locations.

#ifndef INCLUDE_CORE_STM32_H
#define INCLUDE_CORE_STM32_H

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
#define __NVIC_PRIO_BITS 4

// This include relies on CMSIS being downloaded. See README for details.
#include <core_cm3.h>

// Now define some device-specific structures. For more information, see
// http://www.st.com/content/ccc/resource/technical/document/reference_manual/59/b9/ba/7f/11/af/43/d5/CD00171190.pdf/files/CD00171190.pdf/jcr:content/translations/en.CD00171190.pdf

// Reset and Clock Control Layout.
typedef struct {
  __IO uint32_t CR;        // Clock Control.
  __IO uint32_t CFGR;      // Clock Configuration #1.
  __IO uint32_t CIR;       // Clock Interrupt.
  __IO uint32_t APB2RSTR;  // APB2 Peripheral Reset.
  __IO uint32_t APB1RSTR;  // APB1 Peripheral Reset.
  __IO uint32_t AHBENR;    // AHB Peripheral Clock.
  __IO uint32_t APB2ENR;   // APB2 Peripheral Clock Enable.
  __IO uint32_t APB1ENR;   // APB1 Peripheral Clock Enable.
  __IO uint32_t BDCR;      // Backup Domain Control.
  __IO uint32_t CSR;       // Clock Control and status.
  __IO uint32_t AHBRSTR;   // AHB Peripheral Reset.
  __IO uint32_t CFGR2;     // Clock Configuration #2.
  __IO uint32_t CFGR3;     // Clock Configuration #3.
} RCC_t;

// GPIO Control Layout.
typedef struct {
  __IO uint32_t CRL;      // Control Low.
  __IO uint32_t CRH;      // Control High.
  __IO uint16_t IDR;      // Port Input Data.
  __IO uint16_t UNUSED1;  // Unused.
  __IO uint16_t ODR;      // Output data register.
  __IO uint16_t UNUSED2;  // Unused.
  __IO uint32_t BSRR;     // Port Bit Set/Reset.
  __IO uint16_t BRR;      // Bit Reset.
  __IO uint16_t UNUSED3;  // Unused.
  __IO uint32_t LCKR;     // Port Configuration Lock.
} GPIO_t;

// Addresses of peripherals.
#define RCC_BASE ((uint32_t)0x40021000)
#define GPIOA_BASE ((uint32_t)0x40010800)
#define GPIOB_BASE ((uint32_t)0x40010C00)
#define GPIOC_BASE ((uint32_t)0x40011000)

// Globals for accessing peripherals.
#define RCC ((RCC_t*)RCC_BASE)
#define GPIOA ((GPIO_t*)GPIOA_BASE)
#define GPIOB ((GPIO_t*)GPIOB_BASE)
#define GPIOC ((GPIO_t*)GPIOC_BASE)

// Register access values.
#define RCC_GPIOA_ENABLE (0x04)
#define RCC_GPIOB_ENABLE (0x08)
#define RCC_GPIOC_ENABLE (0x10)

#define GPIO_MODE_OUT_2 (0x02)
#define GPIO_CONF_GP_UD (0x0)
#define GPIO_CONF_GP_OD (0x4)

#endif  // INCLUDE_CORE_STM32_H
