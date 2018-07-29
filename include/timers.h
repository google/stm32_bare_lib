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

// Timer utilities for the "Blue Pill" STM32 board.

#ifndef INCLUDE_TIMERS_H
#define INCLUDE_TIMERS_H

#include <stdint.h>

#include "core_stm32.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// We know the "Blue Pill" STM32 board runs at 72 MHz.
#define CLOCK_RATE (72 * 1000 * 1000)

// There's a default SysTick handler function defined in source/timers.c that
// just increments the g_tick_count variable by default. Optionally, you can
// set g_tick_callback and it will call into a user-defined function too.
// To control the frequency, you'll need to call the CMSIS SysTick_Config().
extern uint32_t g_tick_count;
typedef void (*OnSysTickCallback)(int tick_count);
extern OnSysTickCallback g_tick_callback;

// Spin in a loop for roughly the specified number of microseconds. This is
// using instruction timing, so it's not precise, and the processor will be
// consuming power while this is running.
static inline void BusyWaitMicroseconds(int32_t us) {
  // Each loop takes very roughly one microsecond on a Blue Pill.
  volatile int32_t count = us;
  for (; count > 0; --count)
    ;
}

// Indexes for the different timers.
#define TIMERID_TIM1 (0)
#define TIMERID_TIM2 (1)
#define TIMERID_TIM3 (2)
#define TIMERID_TIM4 (3)
#define TIMERID_TIM5 (4)
#define TIMERID_TIM6 (5)
#define TIMERID_TIM7 (6)
#define TIMERID_TIM8 (7)
#define TIMERID_TIM9 (8)
#define TIMERID_TIM10 (9)
#define TIMERID_TIM11 (10)
#define TIMERID_TIM12 (11)
#define TIMERID_TIM13 (12)
#define TIMERID_TIM14 (13)

// Converts from a logical time index defined above, to the actual area of
// memory holding the device control registers. These are in order, so we
// have to do a lookup.
static inline TIM_t* TimerIdToStruct(int timer_id) {
  static TIM_t* const id_to_struct_lookup[] = {
      TIM1, TIM2, TIM3, TIM4, TIM5, TIM6, TIM7,
  };
  return id_to_struct_lookup[timer_id];
}

// Initializes the given timer to count up every millisecond.
static inline void TimerInit(int timer_id) {
  // We need different flags to turn on the different timers, so look them up
  // from a table and apply them to the correct register.
  static const uint32_t id_to_flag_lookup[] = {
      RCC_APB2ENR_TIM1EN,  RCC_APB1ENR_TIM2EN,  RCC_APB1ENR_TIM3EN,
      RCC_APB1ENR_TIM4EN,  RCC_APB1ENR_TIM5EN,  RCC_APB1ENR_TIM6EN,
      RCC_APB1ENR_TIM7EN,  RCC_APB2ENR_TIM8EN,  RCC_APB2ENR_TIM9EN,
      RCC_APB2ENR_TIM10EN, RCC_APB2ENR_TIM11EN, RCC_APB1ENR_TIM12EN,
      RCC_APB1ENR_TIM13EN, RCC_APB1ENR_TIM14EN,
  };
  const uint32_t flag = id_to_flag_lookup[timer_id];
  if ((timer_id == TIMERID_TIM1) || (timer_id > TIMERID_TIM7)) {
    RCC->APB2ENR |= flag;
  } else {
    RCC->APB1ENR |= flag;
  }

  // Now we do some math to determine how fast the counter increments.
  // The prescale (PSC) value is what the timer clock is divided by, and
  // the timer clock is derived from the the system clock either divided
  // by 1, 2, or 4 (set by the TIM_CR1_CKD_DIV* flag).
  TIM_t* tim = TimerIdToStruct(timer_id);
  // Prescaled is 72,000 /4.
  tim->PSC = 18000;
  tim->ARR = 0xffff;
  tim->CR1 = TIM_CR1_DIR_UP | TIM_CR1_CMS_EDGE | TIM_CR1_CKD_DIV4;
  // Once everything's set up, turn the timer on.
  tim->CR1 |= TIM_CR1_CEN;
}

// Returns the current value of a timer's counter. This is only 16 bits, so it
// will overflow quickly (for example after 65 seconds, with the default setup
// above of incrementing every millisecond).
static inline uint16_t TimerGetCounter(int timer_id) {
  TIM_t* tim = TimerIdToStruct(timer_id);
  return tim->CNT;
}

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // INCLUDE_TIMERS_H
