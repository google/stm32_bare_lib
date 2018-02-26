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

#ifndef INCLUDE_STM32_SPECIFICS_H
#define INCLUDE_STM32_SPECIFICS_H

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
  __IO uint32_t CTRL;    // Control and Status.
  __IOM uint32_t LOAD;   // Reload Value.
  __IOM uint32_t VAL;    // Current Value.
  __IM  uint32_t CALIB;  // Calibration.
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

// Addresses of peripherals.
#define RCC_BASE ((uint32_t)0x40021000)
#define GPIOA_BASE ((uint32_t)0x40010800)
#define GPIOB_BASE ((uint32_t)0x40010C00)
#define GPIOC_BASE ((uint32_t)0x40011000)
#define ADC1_BASE ((uint32_t)0x40012400)
#define ADC2_BASE ((uint32_t)0x40012800)
#define FLASH_ACR_BASE ((uint32_t)0x40022000)

// Globals for accessing peripherals.
#define RCC ((RCC_t*)RCC_BASE)
#define GPIOA ((GPIO_t*)GPIOA_BASE)
#define GPIOB ((GPIO_t*)GPIOB_BASE)
#define GPIOC ((GPIO_t*)GPIOC_BASE)
#define ADC1 ((ADC_t*)ADC1_BASE)
#define ADC2 ((ADC_t*)ADC2_BASE)
#define FLASH_ACR ((volatile uint32_t*)FLASH_ACR_BASE)

// Register access values.
#define RCC_GPIOA_ENABLE (0x4)
#define RCC_GPIOB_ENABLE (0x8)
#define RCC_GPIOC_ENABLE (0x10)
#define RCC_ADC1_ENABLE (0x200)
#define RCC_ADC2_ENABLE (0x400)
#define RCC_TIMER1_ENABLE (0x800)
#define RCC_UART1_ENABLE (0x4000)

#define RCC_TIMER2_ENABLE (0x1)
#define RCC_TIMER3_ENABLE (0x2)
#define RCC_TIMER4_ENABLE (0x4)
#define RCC_UART2_ENABLE (0x20000)
#define RCC_UART3_ENABLE (0x40000)
#define RCC_USB_ENABLE (0x800000)

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
#define RCC_CR_HSEBYP (1 <<18)
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

#endif  // INCLUDE_STM32_SPECIFICS_H
