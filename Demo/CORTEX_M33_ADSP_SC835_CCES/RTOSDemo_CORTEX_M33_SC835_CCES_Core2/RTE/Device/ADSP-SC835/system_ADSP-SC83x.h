/******************************************************************************
 * @file     system_ADSP-SC83x.h
 * @brief    CMSIS Device System Header File for ADSP-SC83x Devices
 * @version  1.0.0
 ******************************************************************************/
/*
 * Copyright (c) 2009-2019 Arm Limited. All rights reserved.
 * Portions Copyright (c) 2022-2023 Analog Devices Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SYSTEM_ADSP_SC83X_H
#define SYSTEM_ADSP_SC83X_H


#if defined (_MISRA_SUPPRESSIONS_2008)

/* Rule 16.2.1: The pre-processor shall only be used for file inclusion and
** include guards.
*/
#pragma coverity compliance block(file) deviate MISRA_Cpp_2008_Rule_16_2_1 \
"The CMSIS headers use conditional compilation to enable them to be used in \
C and C++ code."

/* Rule 0.1.5: A project shall not contain unused type declarations.
*/
#pragma coverity compliance block(file) deviate MISRA_Cpp_2008_Rule_0_1_5 \
"Header defines types that may not all be used."

#endif /* defined (_MISRA_SUPPRESSIONS_2008) */


#if defined (_MISRA_SUPPRESSIONS_2012)

/* Rule 1.2: Language extensions should not be used.
*/
#pragma coverity compliance block(file) deviate MISRA_C_2012_Rule_1_2 \
"We use #pragma to suppress MISRA deviations in this block."

/* Rule 2.3: A project should not contain unused type declarations.
*/
#pragma coverity compliance block(file) deviate MISRA_C_2012_Rule_2_3 \
"Header defines types that may not all be used."

#endif /* defined (_MISRA_SUPPRESSIONS_2012) */


#ifdef __cplusplus
#include <cstdint>
#else /* not defined __cplusplus */
#include <stdint.h>
#endif /* __cplusplus */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
  \brief Exception / Interrupt Handler Function Prototype
*/
typedef void(*VECTOR_TABLE_Type)(void);

/**
  \brief System Clock Frequency (Core Clock)
*/
extern uint32_t SystemCoreClock;

/**
  \brief Setup the microcontroller system.

   Initialize the System and update the SystemCoreClock variable.
 */
extern void SystemInit (void);

/**
  \brief  Update SystemCoreClock variable.

   Updates the SystemCoreClock with current core Clock retrieved from cpu
   registers.
 */
extern void SystemCoreClockUpdate (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#if defined (_MISRA_SUPPRESSIONS_2012)
#pragma coverity compliance end_block(file) MISRA_C_2012_Rule_2_3
#pragma coverity compliance end_block(file) MISRA_C_2012_Rule_1_2
#endif /* defined (_MISRA_SUPPRESSIONS_2012) */

#if defined (_MISRA_SUPPRESSIONS_2008)
#pragma coverity compliance end_block(file) MISRA_Cpp_2008_Rule_0_1_5
#pragma coverity compliance end_block(file) MISRA_Cpp_2008_Rule_16_2_1
#endif /* defined (_MISRA_SUPPRESSIONS_2008) */

#endif /* SYSTEM_ADSP_SC83X_H */
