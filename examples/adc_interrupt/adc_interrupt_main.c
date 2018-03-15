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

// This example shows how to read an ADC using interrupts. This is slower than
// continuously DMA-ing the data into a buffer, but simpler.

#include "adc.h"
#include "debug_log.h"

// You need a function named "main" in your program to act like "main" in
// traditional C. This will be called when the processor starts up.
void main(void) {
  // Start up the clock system.
  RccInitForAdc();

  // TODO: At the moment, only port A0 seems to be working.
  AdcInit(GPIOA, 0, 1);
  while (1) {
    // Calls to AdcOn() cause the interrupt to be called back once
    // a value is available.
    AdcOn();
  }
  AdcOff();
}

// If you have a function named OnAdcInterrupt() in your program, this will be
// called once an ADC value is available, if you've set up the system as shown
// in the main() function above.
void OnAdcInterrupt() {
  // We're expecting an EOC signal to be marked in the status register.
  if (!(ADC1->SR & ADC_SR_EOC)) {
    DebugLog("ADC Interrupt called outside of an End Of Conversion event.\n");
    return;
  }
  // Read the value from the data register and output it to the debug log.
  const int32_t adc_value = ADC1->DR;
  const int32_t adc_log_length = 256;
  char adc_log[adc_log_length];
  StrCpy(adc_log, adc_log_length, "ADC: ");
  StrCatInt32(adc_log, adc_log_length, adc_value);
  StrCatStr(adc_log, adc_log_length, "\n");
  DebugLog(adc_log);
}
