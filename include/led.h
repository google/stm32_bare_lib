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

// Control the LED on a "Blue Pill" STM32-compatible board.

#ifndef INCLUDE_LED_H
#define INCLUDE_LED_H

#include "core_stm32.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// Known location of the LED on the "Blue Pill" STM32 SoCs.
#define LED_PORT (13)

// This needs to be called before the LED can be accessed.
static inline void LedInit() {
  // Enable GPIOC.
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

  // Set up speed and configuration.
  const int shift = (LED_PORT - 8) * 4;
  const uint32_t old_config = GPIOC->CRH;
  const uint32_t cleared_config = (old_config & ~(0xf << shift));
  GPIOC->CRH = cleared_config | ((GPIO_MODE_OUT_2 | GPIO_CONF_GP_OD) << shift);
}

// LedInit() must be called before these will work.
static inline void LedOn() { GPIOC->BSRR = (1 << (LED_PORT + 16)); }
static inline void LedOff() { GPIOC->BSRR = (1 << LED_PORT); }

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // INCLUDE_LED_H
