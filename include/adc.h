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

// ADC utility functions.

#ifndef INCLUDE_ADC_H
#define INCLUDE_ADC_H

#include "core_stm32.h"
#include "timers.h"

// Sets the mode bits for the GPIO port. See GPIO_MODE_INPUT_* for values.
static inline void SetGpioMode(GPIO_t* gpio, int port, int mode) {
  __IO uint32_t* cr;
  if (port < 8) {
    cr = &(gpio->CRL);
  } else {
    cr = &(gpio->CRH);
  }

  // Set up speed and configuration.
  const int shift = (port % 8) * 4;
  const uint32_t old_config = *cr;
  const uint32_t cleared_config = (old_config & ~(0xf << shift));
  *cr = cleared_config | (mode << shift);
}

static inline void EnableNvic(int irq) {
  NVIC->ISER[irq/32] = 1 << (irq%32);
}

// Sets up the clock control system for ADC access.
static inline void RccInitForAdc(void) {
  RCC->CFGR =
    RCC_CFGR_PLLSRC_HSE |
    RCC_CFGR_PLLMUL_9 |
    RCC_CFGR_SW_HSI |
    RCC_CFGR_PPRE1_DIV_2;
  RCC->CR =
    RCC_CR_HSION |
    RCC_CR_HSEON |
    RCC_CR_PLLON;
  while (!(RCC->CR & RCC_CR_PLLRDY)) {
  }

  // Configure flash to work with the new speed, with prefetching enabled
  // and a two-clock wait for access.
  *FLASH_ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2;

  RCC->CFGR =
    RCC_CFGR_PLLSRC_HSE |
    RCC_CFGR_PLLMUL_9 |
    RCC_CFGR_SW_PLL |
    RCC_CFGR_ADCPRE_DIV_8 |
    RCC_CFGR_HPRE_DIV_2;

  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
  RCC->APB2ENR |= RCC_APB2ENR_ADC2EN;
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
}

// This needs to be called before the ADC can be accessed.
static inline void AdcInit(GPIO_t* gpio, int port, int do_interrupt) {
  if (do_interrupt) {
    EnableNvic(ADC_IRQ_NUMBER);
  }
  if (do_interrupt) {
    // Trigger an interrupt at the end of a conversion.
    ADC1->CR1 |= ADC_CR1_EOCIE;
  } else {
    ADC1->CR2 |= ADC_CR2_CONT;
    ADC1->CR2 |= ADC_CR2_DMA;
    ADC1->CR2 |= ADC_CR2_SWSTART | ADC_CR2_EXTTRIG;
  }

  ADC1->CR2 |= ADC_CR2_ADON | ADC_CR2_TSVREFE;
  BusyWaitMicroseconds(30 * 1000);
  ADC1->CR2 |= ADC_CR2_CAL;
  while (ADC1->CR2 & ADC_CR2_CAL) {
  }

  ADC1->CR2 |= ADC_CR2_ADON;
  
  SetGpioMode(gpio, port, GPIO_MODE_INPUT_ANALOG);
}

static inline void DmaInit() {
  EnableNvic(DMA1_Channel1_IRQn);
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;
  volatile uint32_t read_value = RCC->AHBENR;
}

static inline void AdcDmaOn(void* dma_buffer, int dma_buffer_count) {
  DMA1->CPAR1 = (uint32_t)(&ADC1->DR);
  DMA1->CMAR1 = (uint32_t)(dma_buffer);
  DMA1->CNDTR1 = dma_buffer_count;
  DMA1->CCR1 =
    DMA_CCR_TCIE |
    DMA_CCR_HTIE |
    DMA_CCR_TEIE |
    DMA_CCR_DIR_FROM_PERIPHERAL |
    DMA_CCR_CIRC |
    DMA_CCR_MINC |
    DMA_CCR_PSIZE_16 |
    DMA_CCR_MSIZE_16 |
    DMA_CCR_PL_LOW;
  DMA1->CCR1 |= DMA_CCR_EN;
}

static inline void AdcOn(void) {
  ADC1->CR2 |= ADC_CR2_ADON;
}

static inline void AdcOff(void) {
  ADC1->CR2 &= ~ADC_CR2_ADON;
}

#endif  // INCLUDE_LED_H
