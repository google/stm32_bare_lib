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

// This example shows how to read an ADC using DMA. For a simpler but slower
// version, see examples/adc_interrupt.

#include "adc.h"
#include "debug_log.h"

#define DMA_BUFFER_SIZE (1024)
uint16_t* g_dma_buffer[DMA_BUFFER_SIZE];

int32_t g_error_count;
int32_t g_half_count;
int32_t g_complete_count;

void OnReset(void) {
  g_error_count = 0;
  g_half_count = 0;
  g_complete_count = 0;
  
  // Start up the clock system.
  RccInitForAdc();

  // TODO: At the moment, only port A0 seems to be working.
  AdcInit(GPIOA, 0, 0);
  DmaInit();
  AdcDmaOn(g_dma_buffer, DMA_BUFFER_SIZE);
  //AdcOn();
  while (1) {
    const int32_t adc_log_length = 256;
    char adc_log[adc_log_length];
    StrCpy(adc_log, adc_log_length, "DMA: ");
    StrCatInt32(adc_log, adc_log_length, g_error_count, 10);
    StrCatStr(adc_log, adc_log_length, " errors, ");
    StrCatInt32(adc_log, adc_log_length, g_half_count, 10);
    StrCatStr(adc_log, adc_log_length, " half, ");
    StrCatInt32(adc_log, adc_log_length, g_complete_count, 10);
    StrCatStr(adc_log, adc_log_length, " complete\n");
    DebugLog(adc_log);
  }
  AdcOff();
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
    return;
  }
  if (DMA1->ISR & DMA_ISR_TCIF1) {
    ++g_complete_count;
    DMA1->IFCR |= DMA_IFCR_CTCIF1;
    return;
  }
}
