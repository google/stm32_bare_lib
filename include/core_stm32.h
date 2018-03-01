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

// Provides useful definitions for access to system components of the STM32
// chip, including both standard Arm and vendor-specific parts.
// This is the main header you want to include to access devices.

#ifndef INCLUDE_CORE_STM32_H
#define INCLUDE_CORE_STM32_H

// These define some STM32-specific properties that are needed before the main
// CMSIS library header is included.
#include "cmsis_predefs.h"

// Include the CMSIS Arm Cortex M3 header, which defines useful functions and
// constants for system components that are common across all M3 chips.
// This include relies on CMSIS being downloaded. See README for details.
#include <core_cm3.h>

// Defines manufacturer-specific parts of the STM32 system.
#include "stm32_specifics.h"

#endif  // INCLUDE_CORE_STM32_H
