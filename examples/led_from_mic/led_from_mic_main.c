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

// This example shows how to read an ADC using DMA. For a simpler but slower
// version, see examples/adc_interrupt.

#include "led.h"
#include "adc.h"
#include "debug_log.h"

#define DMA_BUFFER_SIZE (1024)
uint16_t g_dma_buffer[DMA_BUFFER_SIZE];

int32_t g_error_count;
int32_t g_half_count;
int32_t g_complete_count;
int32_t g_current_volume;

void main(void) {
  g_error_count = 0;
  g_half_count = 0;
  g_complete_count = 0;
  g_current_volume = 0;

  // Start up the clock system.
  RccInitForAdc();

  // TODO: At the moment, only port A0 seems to be working.
  AdcInit(GPIOA, 0, 0);
  DmaInit();
  AdcDmaOn(g_dma_buffer, DMA_BUFFER_SIZE);
  LedInit();
  // AdcOn();
  while (1) {
    if (g_current_volume > 10) {
      LedOn();
    } else {
      LedOff();
    }
  }
  AdcOff();
}

void ProcessDmaBuffer(const uint16_t* buffer, int start_index, int end_index) {
  const uint16_t* start = (buffer + start_index);
  const uint16_t* current;
  const uint16_t* const end = (buffer + end_index);
  int32_t total = 0;
  for (current = start; current != end; ++current) {
    total += *current;
  }
  const int count = (end_index - start_index);
  const int32_t mean = (total / count);
  uint32_t total_volume = 0;
  for (current = start; current != end; ++current) {
    const uint16_t current_value = *current;
    const int32_t delta = (current_value - mean);
    int32_t abs_delta;
    if (delta < 0) {
      abs_delta = -delta;
    } else {
      abs_delta = delta;
    }
    total_volume += abs_delta;
  }
  g_current_volume = (total_volume / count);
}

void OnDma1Channel1Interrupt() {
  if (DMA1->ISR & DMA_ISR_TEIF1) {
    ++g_error_count;
    DMA1->IFCR |= DMA_IFCR_CTEIF1;
    return;
  }
  if (DMA1->ISR & DMA_ISR_HTIF1) {
    ++g_half_count;
    DMA1->IFCR |= DMA_IFCR_CHTIF1;
    ProcessDmaBuffer(g_dma_buffer, 0, (DMA_BUFFER_SIZE / 2));
    return;
  }
  if (DMA1->ISR & DMA_ISR_TCIF1) {
    ++g_complete_count;
    DMA1->IFCR |= DMA_IFCR_CTCIF1;
    ProcessDmaBuffer(g_dma_buffer, (DMA_BUFFER_SIZE / 2), DMA_BUFFER_SIZE);
    return;
  }
}
