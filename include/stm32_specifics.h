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

// Provides useful definitions for processor-specific parts of the STM32
// platform, such as device register locations.

#ifndef INCLUDE_STM32_SPECIFICS_H
#define INCLUDE_STM32_SPECIFICS_H

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

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

// SysTick control.
typedef struct {
  __IO uint32_t CTRL;   // Control and Status.
  __IOM uint32_t LOAD;  // Reload Value.
  __IOM uint32_t VAL;   // Current Value.
  __IM uint32_t CALIB;  // Calibration.
} SysTick_t;

// ADC register layout.
typedef struct {
  __IO uint32_t SR;
  __IO uint32_t CR1;
  __IO uint32_t CR2;
  __IO uint32_t SMPR1;
  __IO uint32_t SMPR2;
  __IO uint32_t JOFR1;
  __IO uint32_t JOFR2;
  __IO uint32_t JOFR3;
  __IO uint32_t JOFR4;
  __IO uint32_t HTR;
  __IO uint32_t LTR;
  __IO uint32_t SQR1;
  __IO uint32_t SQR2;
  __IO uint32_t SQR3;
  __IO uint32_t JSQR;
  __IO uint32_t JDR1;
  __IO uint32_t JDR2;
  __IO uint32_t JDR3;
  __IO uint32_t JDR4;
  __IO uint32_t DR;
} ADC_t;

// DMA register layout.
typedef struct {
  __IO uint32_t ISR;
  __IO uint32_t IFCR;
  __IO uint32_t CCR1;
  __IO uint32_t CNDTR1;
  __IO uint32_t CPAR1;
  __IO uint32_t CMAR1;
  __IO uint32_t UNUSED1;
  __IO uint32_t CCR2;
  __IO uint32_t CNDTR2;
  __IO uint32_t CPAR2;
  __IO uint32_t CMAR2;
  __IO uint32_t UNUSED2;
  __IO uint32_t CCR3;
  __IO uint32_t CNDTR3;
  __IO uint32_t CPAR3;
  __IO uint32_t CMAR3;
  __IO uint32_t UNUSED3;
  __IO uint32_t CCR4;
  __IO uint32_t CNDTR4;
  __IO uint32_t CPAR4;
  __IO uint32_t CMAR4;
  __IO uint32_t UNUSED4;
  __IO uint32_t CCR5;
  __IO uint32_t CNDTR5;
  __IO uint32_t CPAR5;
  __IO uint32_t CMAR5;
  __IO uint32_t UNUSED5;
  __IO uint32_t CCR6;
  __IO uint32_t CNDTR6;
  __IO uint32_t CPAR6;
  __IO uint32_t CMAR6;
  __IO uint32_t UNUSED6;
  __IO uint32_t CCR7;
  __IO uint32_t CNDTR7;
  __IO uint32_t CPAR7;
  __IO uint32_t CMAR7;
  __IO uint32_t UNUSED7;
} DMA_t;

// Timer register layout.
typedef struct {
  __IO uint32_t CR1;
  __IO uint32_t CR2;
  __IO uint32_t SMCR;
  __IO uint32_t DIER;
  __IO uint32_t SR;
  __IO uint32_t EGR;
  __IO uint32_t CCMR1;
  __IO uint32_t CCMR2;
  __IO uint32_t CCER;
  __IO uint32_t CNT;
  __IO uint32_t PSC;
  __IO uint32_t ARR;
  __IO uint32_t RCR;
  __IO uint32_t CCR1;
  __IO uint32_t CCR2;
  __IO uint32_t CCR3;
  __IO uint32_t CCR4;
  __IO uint32_t CDTR;
  __IO uint32_t DCR;
  __IO uint32_t DMAR;
} TIM_t;

// Addresses of peripherals.
#define RCC_BASE ((uint32_t)0x40021000)
#define GPIOA_BASE ((uint32_t)0x40010800)
#define GPIOB_BASE ((uint32_t)0x40010C00)
#define GPIOC_BASE ((uint32_t)0x40011000)
#define ADC1_BASE ((uint32_t)0x40012400)
#define ADC2_BASE ((uint32_t)0x40012800)
#define TIM1_BASE ((uint32_t)0x40012C00)
#define TIM2_BASE ((uint32_t)0x40000000)
#define TIM3_BASE ((uint32_t)0x40000400)
#define TIM4_BASE ((uint32_t)0x40000800)
#define TIM5_BASE ((uint32_t)0x40000c00)
#define TIM6_BASE ((uint32_t)0x40001000)
#define TIM7_BASE ((uint32_t)0x40001400)
#define FLASH_ACR_BASE ((uint32_t)0x40022000)
#define DMA1_BASE ((uint32_t)0x40020000)
#define DMA2_BASE ((uint32_t)0x40020400)

// Globals for accessing peripherals.
#define RCC ((RCC_t*)RCC_BASE)
#define GPIOA ((GPIO_t*)GPIOA_BASE)
#define GPIOB ((GPIO_t*)GPIOB_BASE)
#define GPIOC ((GPIO_t*)GPIOC_BASE)
#define ADC1 ((ADC_t*)ADC1_BASE)
#define ADC2 ((ADC_t*)ADC2_BASE)
#define FLASH_ACR ((volatile uint32_t*)FLASH_ACR_BASE)
#define DMA1 ((DMA_t*)DMA1_BASE)
#define DMA2 ((DMA_t*)DMA2_BASE)
#define TIM1 ((TIM_t*)TIM1_BASE)
#define TIM2 ((TIM_t*)TIM2_BASE)
#define TIM3 ((TIM_t*)TIM3_BASE)
#define TIM4 ((TIM_t*)TIM4_BASE)
#define TIM5 ((TIM_t*)TIM5_BASE)
#define TIM6 ((TIM_t*)TIM6_BASE)
#define TIM7 ((TIM_t*)TIM7_BASE)

// GPIO settings.
#define GPIO_MODE_OUT_2 (0x2)
#define GPIO_CONF_GP_UD (0x0)
#define GPIO_CONF_GP_OD (0x4)

#define GPIO_MODE_INPUT_ANALOG (0x0)
#define GPIO_MODE_INPUT_FLOAT (0x4)
#define GPIO_MODE_INPUT_PUPD (0x8)

// ADC interrupt number.
#define ADC_IRQ_NUMBER (18)

// ADC Status Register flag values.
#define ADC_SR_AWD (1 << 0)
#define ADC_SR_EOC (1 << 1)
#define ADC_SR_JEOC (1 << 2)
#define ADC_SR_JSTRT (1 << 3)
#define ADC_SR_STRT (1 << 4)

// ADC Control Register #1 flag values.
#define ADC_CR1_EOCIE (1 << 5)
#define ADC_CR1_AWDIE (1 << 6)
#define ADC_CR1_JEOCIE (1 << 7)
#define ADC_CR1_SCAN (1 << 8)
#define ADC_CR1_AWDSGL (1 << 9)
#define ADC_CR1_JAUTO (1 << 10)
#define ADC_CR1_DISCEN (1 << 11)
#define ADC_CR1_JDISCEN (1 << 12)
#define ADC_CR1_JAWDEN (1 << 22)
#define ADC_CR1_AWDEN (1 << 23)

// ADC Control Register #2 flag values.
#define ADC_CR2_ADON (1 << 0)
#define ADC_CR2_CONT (1 << 1)
#define ADC_CR2_CAL (1 << 2)
#define ADC_CR2_RSTCAL (1 << 3)
#define ADC_CR2_DMA (1 << 8)
#define ADC_CR2_ALIGN (1 << 11)
#define ADC_CR2_JEXTTRIG (1 << 15)
#define ADC_CR2_EXTTRIG (1 << 20)
#define ADC_CR2_JSWSTART (1 << 21)
#define ADC_CR2_SWSTART (1 << 22)
#define ADC_CR2_TSVREFE (1 << 23)

// ADC Sample Time Register #1 flag values.
#define ADC_SMPR1_SMP10_1_5_CYCLES (0 << 0)
#define ADC_SMPR1_SMP10_7_5_CYCLES (1 << 0)
#define ADC_SMPR1_SMP10_13_5_CYCLES (2 << 0)
#define ADC_SMPR1_SMP10_28_5_CYCLES (3 << 0)
#define ADC_SMPR1_SMP10_41_5_CYCLES (4 << 0)
#define ADC_SMPR1_SMP10_55_5_CYCLES (5 << 0)
#define ADC_SMPR1_SMP10_71_5_CYCLES (6 << 0)
#define ADC_SMPR1_SMP10_239_5_CYCLES (7 << 0)
#define ADC_SMPR1_SMP11_1_5_CYCLES (0 << 3)
#define ADC_SMPR1_SMP11_7_5_CYCLES (1 << 3)
#define ADC_SMPR1_SMP11_13_5_CYCLES (2 << 3)
#define ADC_SMPR1_SMP11_28_5_CYCLES (3 << 3)
#define ADC_SMPR1_SMP11_41_5_CYCLES (4 << 3)
#define ADC_SMPR1_SMP11_55_5_CYCLES (5 << 3)
#define ADC_SMPR1_SMP11_71_5_CYCLES (6 << 3)
#define ADC_SMPR1_SMP11_239_5_CYCLES (7 << 3)
#define ADC_SMPR1_SMP12_1_5_CYCLES (0 << 6)
#define ADC_SMPR1_SMP12_7_5_CYCLES (1 << 6)
#define ADC_SMPR1_SMP12_13_5_CYCLES (2 << 6)
#define ADC_SMPR1_SMP12_28_5_CYCLES (3 << 6)
#define ADC_SMPR1_SMP12_41_5_CYCLES (4 << 6)
#define ADC_SMPR1_SMP12_55_5_CYCLES (5 << 6)
#define ADC_SMPR1_SMP12_71_5_CYCLES (6 << 6)
#define ADC_SMPR1_SMP12_239_5_CYCLES (7 << 6)
#define ADC_SMPR1_SMP13_1_5_CYCLES (0 << 9)
#define ADC_SMPR1_SMP13_7_5_CYCLES (1 << 9)
#define ADC_SMPR1_SMP13_13_5_CYCLES (2 << 9)
#define ADC_SMPR1_SMP13_28_5_CYCLES (3 << 9)
#define ADC_SMPR1_SMP13_41_5_CYCLES (4 << 9)
#define ADC_SMPR1_SMP13_55_5_CYCLES (5 << 9)
#define ADC_SMPR1_SMP13_71_5_CYCLES (6 << 9)
#define ADC_SMPR1_SMP13_239_5_CYCLES (7 << 9)
#define ADC_SMPR1_SMP14_1_5_CYCLES (0 << 12)
#define ADC_SMPR1_SMP14_7_5_CYCLES (1 << 12)
#define ADC_SMPR1_SMP14_13_5_CYCLES (2 << 12)
#define ADC_SMPR1_SMP14_28_5_CYCLES (3 << 12)
#define ADC_SMPR1_SMP14_41_5_CYCLES (4 << 12)
#define ADC_SMPR1_SMP14_55_5_CYCLES (5 << 12)
#define ADC_SMPR1_SMP14_71_5_CYCLES (6 << 12)
#define ADC_SMPR1_SMP14_239_5_CYCLES (7 << 12)
#define ADC_SMPR1_SMP15_1_5_CYCLES (0 << 15)
#define ADC_SMPR1_SMP15_7_5_CYCLES (1 << 15)
#define ADC_SMPR1_SMP15_13_5_CYCLES (2 << 15)
#define ADC_SMPR1_SMP15_28_5_CYCLES (3 << 15)
#define ADC_SMPR1_SMP15_41_5_CYCLES (4 << 15)
#define ADC_SMPR1_SMP15_55_5_CYCLES (5 << 15)
#define ADC_SMPR1_SMP15_71_5_CYCLES (6 << 15)
#define ADC_SMPR1_SMP15_239_5_CYCLES (7 << 15)
#define ADC_SMPR1_SMP16_1_5_CYCLES (0 << 18)
#define ADC_SMPR1_SMP16_7_5_CYCLES (1 << 18)
#define ADC_SMPR1_SMP16_13_5_CYCLES (2 << 18)
#define ADC_SMPR1_SMP16_28_5_CYCLES (3 << 18)
#define ADC_SMPR1_SMP16_41_5_CYCLES (4 << 18)
#define ADC_SMPR1_SMP16_55_5_CYCLES (5 << 18)
#define ADC_SMPR1_SMP16_71_5_CYCLES (6 << 18)
#define ADC_SMPR1_SMP16_239_5_CYCLES (7 << 18)
#define ADC_SMPR1_SMP17_1_5_CYCLES (0 << 21)
#define ADC_SMPR1_SMP17_7_5_CYCLES (1 << 21)
#define ADC_SMPR1_SMP17_13_5_CYCLES (2 << 21)
#define ADC_SMPR1_SMP17_28_5_CYCLES (3 << 21)
#define ADC_SMPR1_SMP17_41_5_CYCLES (4 << 21)
#define ADC_SMPR1_SMP17_55_5_CYCLES (5 << 21)
#define ADC_SMPR1_SMP17_71_5_CYCLES (6 << 21)
#define ADC_SMPR1_SMP17_239_5_CYCLES (7 << 21)

// ADC Sample Time Register #2 flag values.
#define ADC_SMPR2_SMP0_1_5_CYCLES (0 << 0)
#define ADC_SMPR2_SMP0_7_5_CYCLES (1 << 0)
#define ADC_SMPR2_SMP0_13_5_CYCLES (2 << 0)
#define ADC_SMPR2_SMP0_28_5_CYCLES (3 << 0)
#define ADC_SMPR2_SMP0_41_5_CYCLES (4 << 0)
#define ADC_SMPR2_SMP0_55_5_CYCLES (5 << 0)
#define ADC_SMPR2_SMP0_71_5_CYCLES (6 << 0)
#define ADC_SMPR2_SMP0_239_5_CYCLES (7 << 0)
#define ADC_SMPR2_SMP1_1_5_CYCLES (0 << 3)
#define ADC_SMPR2_SMP1_7_5_CYCLES (1 << 3)
#define ADC_SMPR2_SMP1_13_5_CYCLES (2 << 3)
#define ADC_SMPR2_SMP1_28_5_CYCLES (3 << 3)
#define ADC_SMPR2_SMP1_41_5_CYCLES (4 << 3)
#define ADC_SMPR2_SMP1_55_5_CYCLES (5 << 3)
#define ADC_SMPR2_SMP1_71_5_CYCLES (6 << 3)
#define ADC_SMPR2_SMP1_239_5_CYCLES (7 << 3)
#define ADC_SMPR2_SMP2_1_5_CYCLES (0 << 6)
#define ADC_SMPR2_SMP2_7_5_CYCLES (1 << 6)
#define ADC_SMPR2_SMP2_13_5_CYCLES (2 << 6)
#define ADC_SMPR2_SMP2_28_5_CYCLES (3 << 6)
#define ADC_SMPR2_SMP2_41_5_CYCLES (4 << 6)
#define ADC_SMPR2_SMP2_55_5_CYCLES (5 << 6)
#define ADC_SMPR2_SMP2_71_5_CYCLES (6 << 6)
#define ADC_SMPR2_SMP2_239_5_CYCLES (7 << 6)
#define ADC_SMPR2_SMP3_1_5_CYCLES (0 << 9)
#define ADC_SMPR2_SMP3_7_5_CYCLES (1 << 9)
#define ADC_SMPR2_SMP3_13_5_CYCLES (2 << 9)
#define ADC_SMPR2_SMP3_28_5_CYCLES (3 << 9)
#define ADC_SMPR2_SMP3_41_5_CYCLES (4 << 9)
#define ADC_SMPR2_SMP3_55_5_CYCLES (5 << 9)
#define ADC_SMPR2_SMP3_71_5_CYCLES (6 << 9)
#define ADC_SMPR2_SMP3_239_5_CYCLES (7 << 9)
#define ADC_SMPR2_SMP4_1_5_CYCLES (0 << 12)
#define ADC_SMPR2_SMP4_7_5_CYCLES (1 << 12)
#define ADC_SMPR2_SMP4_13_5_CYCLES (2 << 12)
#define ADC_SMPR2_SMP4_28_5_CYCLES (3 << 12)
#define ADC_SMPR2_SMP4_41_5_CYCLES (4 << 12)
#define ADC_SMPR2_SMP4_55_5_CYCLES (5 << 12)
#define ADC_SMPR2_SMP4_71_5_CYCLES (6 << 12)
#define ADC_SMPR2_SMP4_239_5_CYCLES (7 << 12)
#define ADC_SMPR2_SMP5_1_5_CYCLES (0 << 15)
#define ADC_SMPR2_SMP5_7_5_CYCLES (1 << 15)
#define ADC_SMPR2_SMP5_13_5_CYCLES (2 << 15)
#define ADC_SMPR2_SMP5_28_5_CYCLES (3 << 15)
#define ADC_SMPR2_SMP5_41_5_CYCLES (4 << 15)
#define ADC_SMPR2_SMP5_55_5_CYCLES (5 << 15)
#define ADC_SMPR2_SMP5_71_5_CYCLES (6 << 15)
#define ADC_SMPR2_SMP5_239_5_CYCLES (7 << 15)
#define ADC_SMPR2_SMP6_1_5_CYCLES (0 << 18)
#define ADC_SMPR2_SMP6_7_5_CYCLES (1 << 18)
#define ADC_SMPR2_SMP6_13_5_CYCLES (2 << 18)
#define ADC_SMPR2_SMP6_28_5_CYCLES (3 << 18)
#define ADC_SMPR2_SMP6_41_5_CYCLES (4 << 18)
#define ADC_SMPR2_SMP6_55_5_CYCLES (5 << 18)
#define ADC_SMPR2_SMP6_71_5_CYCLES (6 << 18)
#define ADC_SMPR2_SMP6_239_5_CYCLES (7 << 18)
#define ADC_SMPR2_SMP7_1_5_CYCLES (0 << 21)
#define ADC_SMPR2_SMP7_7_5_CYCLES (1 << 21)
#define ADC_SMPR2_SMP7_13_5_CYCLES (2 << 21)
#define ADC_SMPR2_SMP7_28_5_CYCLES (3 << 21)
#define ADC_SMPR2_SMP7_41_5_CYCLES (4 << 21)
#define ADC_SMPR2_SMP7_55_5_CYCLES (5 << 21)
#define ADC_SMPR2_SMP7_71_5_CYCLES (6 << 21)
#define ADC_SMPR2_SMP7_239_5_CYCLES (7 << 21)
#define ADC_SMPR2_SMP8_1_5_CYCLES (0 << 24)
#define ADC_SMPR2_SMP8_7_5_CYCLES (1 << 24)
#define ADC_SMPR2_SMP8_13_5_CYCLES (2 << 24)
#define ADC_SMPR2_SMP8_28_5_CYCLES (3 << 24)
#define ADC_SMPR2_SMP8_41_5_CYCLES (4 << 24)
#define ADC_SMPR2_SMP8_55_5_CYCLES (5 << 24)
#define ADC_SMPR2_SMP8_71_5_CYCLES (6 << 24)
#define ADC_SMPR2_SMP8_239_5_CYCLES (7 << 24)
#define ADC_SMPR2_SMP9_1_5_CYCLES (0 << 27)
#define ADC_SMPR2_SMP9_7_5_CYCLES (1 << 27)
#define ADC_SMPR2_SMP9_13_5_CYCLES (2 << 27)
#define ADC_SMPR2_SMP9_28_5_CYCLES (3 << 27)
#define ADC_SMPR2_SMP9_41_5_CYCLES (4 << 27)
#define ADC_SMPR2_SMP9_55_5_CYCLES (5 << 27)
#define ADC_SMPR2_SMP9_71_5_CYCLES (6 << 27)
#define ADC_SMPR2_SMP9_239_5_CYCLES (7 << 27)

// Flash Access Control Register flag values.
#define FLASH_ACR_LATENCY_0 (0)
#define FLASH_ACR_LATENCY_1 (1)
#define FLASH_ACR_LATENCY_2 (2)
#define FLASH_ACR_HLFCYA (1 << 3)
#define FLASH_ACR_PRFTBE (1 << 4)
#define FLASH_ACR_PRFTBS (1 << 5)

// Reset and Clock Control Control Register flag values.
#define RCC_CR_HSION (1 << 0)
#define RCC_CR_HSIRDY (1 << 1)
#define RCC_CR_HSEON (1 << 16)
#define RCC_CR_HSERDY (1 << 17)
#define RCC_CR_HSEBYP (1 << 18)
#define RCC_CR_CSSON (1 << 19)
#define RCC_CR_PLLON (1 << 24)
#define RCC_CR_PLLRDY (1 << 25)

// Reset and Clock Control Configuration Register flag values.
#define RCC_CFGR_SW_HSI (0 << 0)
#define RCC_CFGR_SW_HSE (1 << 0)
#define RCC_CFGR_SW_PLL (2 << 0)
#define RCC_CFGR_HPRE_DIV_NONE (0 << 4)
#define RCC_CFGR_HPRE_DIV_2 ((1 << 7) | (0 << 4))
#define RCC_CFGR_HPRE_DIV_4 ((1 << 7) | (1 << 4))
#define RCC_CFGR_HPRE_DIV_8 ((1 << 7) | (2 << 4))
#define RCC_CFGR_HPRE_DIV_16 ((1 << 7) | (3 << 4))
#define RCC_CFGR_HPRE_DIV_64 ((1 << 7) | (4 << 4))
#define RCC_CFGR_HPRE_DIV_128 ((1 << 7) | (5 << 4))
#define RCC_CFGR_HPRE_DIV_256 ((1 << 7) | (6 << 4))
#define RCC_CFGR_HPRE_DIV_512 ((1 << 7) | (7 << 4))
#define RCC_CFGR_PPRE1_DIV_NONE (0 << 8)
#define RCC_CFGR_PPRE1_DIV_2 ((1 << 10) | (0 << 8))
#define RCC_CFGR_PPRE1_DIV_4 ((1 << 10) | (1 << 8))
#define RCC_CFGR_PPRE1_DIV_8 ((1 << 10) | (2 << 8))
#define RCC_CFGR_PPRE1_DIV_16 ((1 << 10) | (3 << 8))
#define RCC_CFGR_PPRE2_DIV_NONE (0 << 11)
#define RCC_CFGR_PPRE2_DIV_2 ((1 << 13) | (0 << 11))
#define RCC_CFGR_PPRE2_DIV_4 ((1 << 13) | (1 << 11))
#define RCC_CFGR_PPRE2_DIV_8 ((1 << 13) | (2 << 11))
#define RCC_CFGR_PPRE2_DIV_16 ((1 << 13) | (3 << 11))
#define RCC_CFGR_ADCPRE_DIV_2 (0 << 14)
#define RCC_CFGR_ADCPRE_DIV_4 (1 << 14)
#define RCC_CFGR_ADCPRE_DIV_6 (2 << 14)
#define RCC_CFGR_ADCPRE_DIV_8 (3 << 14)
#define RCC_CFGR_PLLSRC_HSE (1 << 16)
#define RCC_CFGR_PLLXTPRE (1 << 17)
#define RCC_CFGR_PLLMUL_2 (0 << 18)
#define RCC_CFGR_PLLMUL_3 (1 << 18)
#define RCC_CFGR_PLLMUL_4 (2 << 18)
#define RCC_CFGR_PLLMUL_5 (3 << 18)
#define RCC_CFGR_PLLMUL_6 (4 << 18)
#define RCC_CFGR_PLLMUL_7 (5 << 18)
#define RCC_CFGR_PLLMUL_8 (6 << 18)
#define RCC_CFGR_PLLMUL_9 (7 << 18)
#define RCC_CFGR_PLLMUL_10 (8 << 18)
#define RCC_CFGR_PLLMUL_11 (9 << 18)
#define RCC_CFGR_PLLMUL_12 (10 << 18)
#define RCC_CFGR_PLLMUL_13 (11 << 18)
#define RCC_CFGR_PLLMUL_14 (12 << 18)
#define RCC_CFGR_PLLMUL_15 (13 << 18)
#define RCC_CFGR_PLLMUL_16 (14 << 18)
#define RCC_CFGR_USBPRE (1 << 22)
#define RCC_CFGR_MCO_NO_CLOCK (0 << 24)
#define RCC_CFGR_MCO_SYS_CLOCK ((1 << 26) | (1 << 24))
#define RCC_CFGR_MCO_HSI_CLOCK ((1 << 26) | (2 << 24))
#define RCC_CFGR_MCO_PLL_CLOCK ((1 << 26) | (3 << 24))

// Reset and Clock Control AHB Peripheral Clock Enable Register flag values.
#define RCC_AHBENR_DMA1EN (1 << 0)
#define RCC_AHBENR_DMA2EN (1 << 1)
#define RCC_AHBENR_SRAMEN (1 << 2)
#define RCC_AHBENR_FLITFEN (1 << 4)
#define RCC_AHBENR_CRCEN (1 << 6)
#define RCC_AHBENR_FSMCEN (1 << 8)
#define RCC_AHBENR_SDIOEN (1 << 10)

// Reset and Clock Control APB1 Peripheral Clock Enable Register flag values.
#define RCC_APB1ENR_TIM2EN (1 << 0)
#define RCC_APB1ENR_TIM3EN (1 << 1)
#define RCC_APB1ENR_TIM4EN (1 << 2)
#define RCC_APB1ENR_TIM5EN (1 << 3)
#define RCC_APB1ENR_TIM6EN (1 << 4)
#define RCC_APB1ENR_TIM7EN (1 << 5)
#define RCC_APB1ENR_TIM12EN (1 << 6)
#define RCC_APB1ENR_TIM13EN (1 << 7)
#define RCC_APB1ENR_TIM14EN (1 << 8)
#define RCC_APB1ENR_WWDGEN (1 << 11)
#define RCC_APB1ENR_SPI2EN (1 << 14)
#define RCC_APB1ENR_SPI3EN (1 << 15)
#define RCC_APB1ENR_USART2EN (1 << 17)
#define RCC_APB1ENR_USART3EN (1 << 18)
#define RCC_APB1ENR_USART4EN (1 << 19)
#define RCC_APB1ENR_USART5EN (1 << 20)
#define RCC_APB1ENR_I2C1EN (1 << 21)
#define RCC_APB1ENR_I2C2EN (1 << 22)
#define RCC_APB1ENR_USBEN (1 << 23)
#define RCC_APB1ENR_CANEN (1 << 25)
#define RCC_APB1ENR_BKPEN (1 << 27)
#define RCC_APB1ENR_PWREN (1 << 28)
#define RCC_APB1ENR_DACEN (1 << 29)

// Reset and Clock Control APB2 Peripheral Clock Enable Register flag values.
#define RCC_APB2ENR_AFIOEN (1 << 0)
#define RCC_APB2ENR_IOPAEN (1 << 2)
#define RCC_APB2ENR_IOPBEN (1 << 3)
#define RCC_APB2ENR_IOPCEN (1 << 4)
#define RCC_APB2ENR_IOPDEN (1 << 5)
#define RCC_APB2ENR_IOPEEN (1 << 6)
#define RCC_APB2ENR_IOPFEN (1 << 7)
#define RCC_APB2ENR_IOPGEN (1 << 8)
#define RCC_APB2ENR_ADC1EN (1 << 9)
#define RCC_APB2ENR_ADC2EN (1 << 10)
#define RCC_APB2ENR_TIM1EN (1 << 11)
#define RCC_APB2ENR_SPI1EN (1 << 12)
#define RCC_APB2ENR_TIM8EN (1 << 13)
#define RCC_APB2ENR_USART1EN (1 << 14)
#define RCC_APB2ENR_ADC3EN (1 << 15)
#define RCC_APB2ENR_TIM9EN (1 << 19)
#define RCC_APB2ENR_TIM10EN (1 << 20)
#define RCC_APB2ENR_TIM11EN (1 << 21)

// DMA Configuration Register flag values.
#define DMA_CCR_EN (1 << 0)
#define DMA_CCR_TCIE (1 << 1)
#define DMA_CCR_HTIE (1 << 2)
#define DMA_CCR_TEIE (1 << 3)
#define DMA_CCR_DIR_FROM_PERIPHERAL (0 << 4)
#define DMA_CCR_DIR_FROM_MEMORY (1 << 4)
#define DMA_CCR_CIRC (1 << 5)
#define DMA_CCR_PINC (1 << 6)
#define DMA_CCR_MINC (1 << 7)
#define DMA_CCR_PSIZE_8 (0 << 8)
#define DMA_CCR_PSIZE_16 (1 << 8)
#define DMA_CCR_PSIZE_32 (2 << 8)
#define DMA_CCR_MSIZE_8 (0 << 10)
#define DMA_CCR_MSIZE_16 (1 << 10)
#define DMA_CCR_MSIZE_32 (2 << 10)
#define DMA_CCR_PL_LOW (0 << 12)
#define DMA_CCR_PL_MEDIUM (1 << 12)
#define DMA_CCR_PL_HIGH (2 << 12)
#define DMA_CCR_PL_VERY_HIGH (3 << 12)
#define DMA_CCR_MEM2MEM (1 << 14)

// DMA Interrupt Status Register flag values.
#define DMA_ISR_GIF1 (1 << 0)
#define DMA_ISR_TCIF1 (1 << 1)
#define DMA_ISR_HTIF1 (1 << 2)
#define DMA_ISR_TEIF1 (1 << 3)
#define DMA_ISR_GIF2 (1 << 4)
#define DMA_ISR_TCIF2 (1 << 5)
#define DMA_ISR_HTIF2 (1 << 6)
#define DMA_ISR_TEIF2 (1 << 7)
#define DMA_ISR_GIF3 (1 << 8)
#define DMA_ISR_TCIF3 (1 << 9)
#define DMA_ISR_HTIF3 (1 << 10)
#define DMA_ISR_TEIF3 (1 << 11)
#define DMA_ISR_GIF4 (1 << 12)
#define DMA_ISR_TCIF4 (1 << 13)
#define DMA_ISR_HTIF4 (1 << 14)
#define DMA_ISR_TEIF4 (1 << 15)
#define DMA_ISR_GIF5 (1 << 16)
#define DMA_ISR_TCIF5 (1 << 17)
#define DMA_ISR_HTIF5 (1 << 18)
#define DMA_ISR_TEIF5 (1 << 19)
#define DMA_ISR_GIF6 (1 << 20)
#define DMA_ISR_TCIF6 (1 << 21)
#define DMA_ISR_HTIF6 (1 << 22)
#define DMA_ISR_TEIF6 (1 << 23)
#define DMA_ISR_GIF7 (1 << 24)
#define DMA_ISR_TCIF7 (1 << 25)
#define DMA_ISR_HTIF7 (1 << 26)
#define DMA_ISR_TEIF7 (1 << 27)

// DMA Interrupt Clear Register flag values.
#define DMA_IFCR_CGIF1 (1 << 0)
#define DMA_IFCR_CTCIF1 (1 << 1)
#define DMA_IFCR_CHTIF1 (1 << 2)
#define DMA_IFCR_CTEIF1 (1 << 3)
#define DMA_IFCR_CGIF2 (1 << 4)
#define DMA_IFCR_CTCIF2 (1 << 5)
#define DMA_IFCR_CHTIF2 (1 << 6)
#define DMA_IFCR_CTEIF2 (1 << 7)
#define DMA_IFCR_CGIF3 (1 << 8)
#define DMA_IFCR_CTCIF3 (1 << 9)
#define DMA_IFCR_CHTIF3 (1 << 10)
#define DMA_IFCR_CTEIF3 (1 << 11)
#define DMA_IFCR_CGIF4 (1 << 12)
#define DMA_IFCR_CTCIF4 (1 << 13)
#define DMA_IFCR_CHTIF4 (1 << 14)
#define DMA_IFCR_CTEIF4 (1 << 15)
#define DMA_IFCR_CGIF5 (1 << 16)
#define DMA_IFCR_CTCIF5 (1 << 17)
#define DMA_IFCR_CHTIF5 (1 << 18)
#define DMA_IFCR_CTEIF5 (1 << 19)
#define DMA_IFCR_CGIF6 (1 << 20)
#define DMA_IFCR_CTCIF6 (1 << 21)
#define DMA_IFCR_CHTIF6 (1 << 22)
#define DMA_IFCR_CTEIF6 (1 << 23)
#define DMA_IFCR_CGIF7 (1 << 24)
#define DMA_IFCR_CTCIF7 (1 << 25)
#define DMA_IFCR_CHTIF7 (1 << 26)
#define DMA_IFCR_CTEIF7 (1 << 27)

// Timer Control Register #1 flag values.
#define TIM_CR1_CEN (1 << 0)
#define TIM_CR1_UDIS (1 << 1)
#define TIM_CR1_URS (1 << 2)
#define TIM_CR1_OPM (1 << 3)
#define TIM_CR1_DIR_UP (0 << 4)
#define TIM_CR1_DIR_DOWN (0 << 4)
#define TIM_CR1_CMS_EDGE (0 << 5)
#define TIM_CR1_CMS_MODE1 (1 << 5)
#define TIM_CR1_CMS_MODE2 (2 << 5)
#define TIM_CR1_CMS_MODE3 (3 << 5)
#define TIM_CR1_ARPE (1 << 7)
#define TIM_CR1_CKD_DIV1 (0 << 8)
#define TIM_CR1_CKD_DIV2 (1 << 8)
#define TIM_CR1_CKD_DIV4 (2 << 8)

// Timer Control Register #2 flag values.
#define TIM_CR2_CCPC (1 << 0)
#define TIM_CR2_CCUS (1 << 2)
#define TIM_CR2_CCDS (1 << 3)
#define TIM_CR2_MMS_RESET (0 << 4)
#define TIM_CR2_MMS_ENABLE (1 << 4)
#define TIM_CR2_MMS_UPDATE (2 << 4)
#define TIM_CR2_MMS_COMPARE_PULSE (3 << 4)
#define TIM_CR2_MMS_COMPARE_OC1 (4 << 4)
#define TIM_CR2_MMS_COMPARE_OC2 (5 << 4)
#define TIM_CR2_MMS_COMPARE_OC3 (6 << 4)
#define TIM_CR2_MMS_COMPARE_OC4 (7 << 4)
#define TIM_CR2_MMS_TI1S (1 << 7)
#define TIM_CR2_MMS_OIS1 (1 << 8)
#define TIM_CR2_MMS_OIS1N (1 << 9)
#define TIM_CR2_MMS_OIS2 (1 << 10)
#define TIM_CR2_MMS_OIS2N (1 << 11)
#define TIM_CR2_MMS_OIS3 (1 << 12)
#define TIM_CR2_MMS_OIS3N (1 << 13)
#define TIM_CR2_MMS_OIS4 (1 << 14)

// Timer Status Register flag values.
#define TIM_SR_UIF (1 << 0)
#define TIM_SR_CC1IF (1 << 1)
#define TIM_SR_CC2IF (1 << 2)
#define TIM_SR_CC3IF (1 << 3)
#define TIM_SR_CC4IF (1 << 4)
#define TIM_SR_COMIF (1 << 5)
#define TIM_SR_TIF (1 << 6)
#define TIM_SR_BIF (1 << 7)
#define TIM_SR_CC1OF (1 << 9)
#define TIM_SR_CC2OF (1 << 9)
#define TIM_SR_CC3OF (1 << 9)
#define TIM_SR_CC4OF (1 << 9)

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // INCLUDE_STM32_SPECIFICS_H
