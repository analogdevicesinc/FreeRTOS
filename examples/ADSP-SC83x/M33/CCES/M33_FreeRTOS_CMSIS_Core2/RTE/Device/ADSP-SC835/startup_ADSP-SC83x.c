/******************************************************************************
 * @file     startup_ADSP-SC83x.c
 * @brief    Device Startup File for ADSP-SC83x Device
 *
 * Based on Device/ARM/ARMCM33/Source/startup_ARMCM33.c file in
 * ARM.CMSIS.5.9.0.pack.
 *
 * @version  V1.0.0
 * @date     20. January 2023
 ******************************************************************************/
/*
 * Copyright (c) 2009-2020 Arm Limited. All rights reserved.
 * Portions Copyright (c) 2023 Analog Devices, Inc. All rights reserved.
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

#include <ADSP-SC83x.h>
#include <sys/platform.h>

#if defined (_MISRA_SUPPRESSIONS_2012)

/* Rule 1.2: Language extensions should not be used.
*/
#pragma coverity compliance block(file) deviate MISRA_C_2012_Rule_1_2 \
"We use #pragma to document MISRA deviations in this block. We also use \
declare weak function aliases for the interrupt handlers."

#endif /* defined (_MISRA_SUPPRESSIONS_2012) */

#if defined (_MISRA_SUPPRESSIONS_2004)

/* Rule 8.9: An identifier with external linkage shall have exactly one external
** definition.
*/
#pragma coverity compliance block(file) deviate MISRA_C_2004_Rule_8_9 \
"Handlers are declared via aliases to the default handler, which Coverity \
doesn't analyze."

/* Rule 8.10: Objects and functions shall have internal linkage unless external
** linkage is required.
*/
#pragma coverity compliance block(file) deviate MISRA_C_2004_Rule_8_10 \
"File defines interrupt handler functions that may be overridden in application \
sources."

/* Rule 10.3: The value of a complex expression of integer type shall only be
** cast to a type of the same signedness that is no wider than the underlying
** type of the expression.
*/
#pragma coverity compliance block(file) deviate MISRA_C_2004_Rule_10_3 \
"The macros provided by the ARM CMSIS Pack for setting the stack pointer \
limit define the argument away to void in some configurations."

/* Rule 11.1: Conversions shall not be performed between a pointer to a
 * function and any type other than an integral type.
*/
#pragma coverity compliance block(file) deviate MISRA_C_2004_Rule_11_1 \
"The runtime model requires us to store the stack pointer in the interrupt \
table."

/* Rule 11.3: A cast should not be performed between a pointer type and an
** integral type.
*/
#pragma coverity compliance block(file) deviate MISRA_C_2004_Rule_11_3 \
"The functions provided by the ARM CMSIS Pack for setting the stack pointer \
take integers rather than pointers."

/* Rule 19.7: A function should be used in preference to a function-like macro.
*/
#pragma coverity compliance block(file) deviate MISRA_C_2004_Rule_19_7 \
"The functions provided by the ARM CMSIS Pack for setting the stack pointer \
are actually implemented as function-like macros."

#elif defined (_MISRA_SUPPRESSIONS_2012)

/* Directive 4.9: A function should be used in preference to a function-like
** macro where they are interchangeable.
*/
#pragma coverity compliance block(file) deviate MISRA_C_2012_Directive_4_9 \
"The functions provided by the ARM CMSIS Pack for setting the stack pointer \
are actually implemented as function-like macros."

/* Rule 8.6: An identifier with external linkage shall have exactly one external
** definition.
*/
#pragma coverity compliance block(file) deviate MISRA_C_2012_Rule_8_6 \
"Handlers are declared via aliases to the default handler, which Coverity \
doesn't analyze."

/* Rule 8.7: Objects and functions shall have internal linkage unless external
** linkage is required.
*/
#pragma coverity compliance block(file) deviate MISRA_C_2012_Rule_8_7 \
"File defines interrupt handler functions that may be overridden in application \
sources."

/* Rule 11.1: Conversions shall not be performed between a pointer to a
** function and any other type.
*/
#pragma coverity compliance block(file) deviate MISRA_C_2012_Rule_11_1 \
"The runtime model requires us to store the stack pointer in the interrupt \
table."

/* Rule 11.4: A conversion should not be performed between a pointer to object
** and an integer type.
*/
#pragma coverity compliance block(file) deviate MISRA_C_2012_Rule_11_4 \
"The functions provided by the ARM CMSIS Pack for setting the stack pointer \
take integers rather than pointers."

/* Rule 21.2: A reserved identifier or macro name shall not be declared.
*/
#pragma coverity compliance block(file) deviate MISRA_C_2012_Rule_21_2 \
"Vendor is permitted to use reserved identifiers."

#endif /* defined (_MISRA_SUPPRESSIONS_*) */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----------------------------------------------------------------------------
  External References
 *----------------------------------------------------------------------------*/
extern uint32_t __INITIAL_SP;
extern uint32_t __STACK_LIMIT;

#if !defined(__ICCARM__) /* IAR seals the stack in __iar_program_start */
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
extern uint32_t __STACK_SEAL;
#endif /* defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U) */
#endif /* defined(__ICCARM__) */

extern __NO_RETURN void __PROGRAM_START(void);

/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/
__NO_RETURN void Reset_Handler  (void);
            void Default_Handler(void);

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/

#if defined(__ICCARM__)

void NMI_Handler        (void);
#pragma weak NMI_Handler=Default_Handler
void HardFault_Handler  (void);
void MemManage_Handler  (void);
#pragma weak MemManage_Handler=Default_Handler
void BusFault_Handler   (void);
#pragma weak BusFault_Handler=Default_Handler
void UsageFault_Handler (void);
#pragma weak UsageFault_Handler=Default_Handler
void SecureFault_Handler(void);
#pragma weak SecureFault_Handler=Default_Handler
void SVC_Handler        (void);
#pragma weak SVC_Handler=Default_Handler
void DebugMon_Handler   (void);
#pragma weak DebugMon_Handler=Default_Handler
void PendSV_Handler     (void);
#pragma weak PendSV_Handler=Default_Handler
void SysTick_Handler    (void);
#pragma weak SysTick_Handler=Default_Handler

#else /* not __ICCARM__ */

void NMI_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler  (void) __attribute__ ((weak));
void MemManage_Handler  (void) __attribute__ ((weak, alias("Default_Handler")));
void BusFault_Handler   (void) __attribute__ ((weak, alias("Default_Handler")));
void UsageFault_Handler (void) __attribute__ ((weak, alias("Default_Handler")));
void SecureFault_Handler(void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler   (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler    (void) __attribute__ ((weak, alias("Default_Handler")));

#endif /* __ICCARM__ */

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
__NO_RETURN void Reset_Handler(void)
{
    __set_MSP((uint32_t)(&__INITIAL_SP));
    __set_PSP((uint32_t)(&__INITIAL_SP));

    __set_MSPLIM((uint32_t)(&__STACK_LIMIT));
    __set_PSPLIM((uint32_t)(&__STACK_LIMIT));

#if !defined(__ICCARM__) /* IAR seals the stack in __iar_program_start */
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    __TZ_set_STACKSEAL_S((uint32_t *)(&__STACK_SEAL));
#endif /* defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U) */
#endif /* defined(__ICCARM__) */

    SystemInit();        /* CMSIS System Initialization */

#if !defined(SKIP_CORE_ENABLE_WAIT_LOOP)
    if (((*pREG_RCU0_CTL) & BITM_RCU_CTL_CRSTREQEN) != 0u) 
    {
        /* Wait for the core to be enabled,
         * if the RCU is configured to allow core resets.
         * This means we are running on hardware rather than a simulator.
         */
        while (((*pREG_RCU0_MSG) & BITM_RCU_MSG_C2ACTIVATE) == 0U)
        {
        }
    }
#endif /* !defined(SKIP_CORE_ENABLE_WAIT_LOOP) */

    __PROGRAM_START();   /* Enter PreMain (C library entry point) */
}


#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#endif /* defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) */

#ifndef LOOP_FOREVER
#define LOOP_FOREVER (1U == 1U)
#endif /* LOOP_FOREVER */

/*----------------------------------------------------------------------------
  Hard Fault Handler
 *----------------------------------------------------------------------------*/
void HardFault_Handler(void)
{
    /* Infinite loop */
    do
    {
    } while (LOOP_FOREVER);
}

/*----------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
void Default_Handler(void)
{
    /* Infinite loop */
    do
    {
    } while (LOOP_FOREVER);
}

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma clang diagnostic pop
#endif /* defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) */

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif /* __GNUC__ */

VECTOR_TABLE_Type __VECTOR_TABLE[496] __VECTOR_TABLE_ATTRIBUTE = {
    (VECTOR_TABLE_Type)(&__INITIAL_SP),       /*     Initial Stack Pointer */

    /* Exceptions */
    &Reset_Handler,                           /*     Reset Handler */
    &NMI_Handler,                             /* -14 NMI Handler */
    &HardFault_Handler,                       /* -13 Hard Fault Handler */
    &MemManage_Handler,                       /* -12 MPU Fault Handler */
    &BusFault_Handler,                        /* -11 Bus Fault Handler */
    &UsageFault_Handler,                      /* -10 Usage Fault Handler */
    &SecureFault_Handler,                     /*  -9 Secure Fault Handler */
    0,                                        /*     Reserved */
    0,                                        /*     Reserved */
    0,                                        /*     Reserved */
    &SVC_Handler,                             /*  -5 SVCall Handler */
    &DebugMon_Handler,                        /*  -4 Debug Monitor Handler */
    0,                                        /*     Reserved */
    &PendSV_Handler,                          /*  -2 PendSV Handler */
    &SysTick_Handler,                         /*  -1 SysTick Handler */

    /* Interrupts */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
    &Default_Handler,                         /*     Default Handler */
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif /* __GNUC__ */

#if defined (_MISRA_SUPPRESSIONS_2004)

#pragma coverity compliance end_block(file) MISRA_C_2004_Rule_8_9
#pragma coverity compliance end_block(file) MISRA_C_2004_Rule_8_10
#pragma coverity compliance end_block(file) MISRA_C_2004_Rule_10_3
#pragma coverity compliance end_block(file) MISRA_C_2004_Rule_11_1
#pragma coverity compliance end_block(file) MISRA_C_2004_Rule_11_3
#pragma coverity compliance end_block(file) MISRA_C_2004_Rule_19_7

#elif defined (_MISRA_SUPPRESSIONS_2012)

#pragma coverity compliance end_block(file) MISRA_C_2012_Directive_4_9
#pragma coverity compliance end_block(file) MISRA_C_2012_Rule_8_6
#pragma coverity compliance end_block(file) MISRA_C_2012_Rule_8_7
#pragma coverity compliance end_block(file) MISRA_C_2012_Rule_11_1
#pragma coverity compliance end_block(file) MISRA_C_2012_Rule_11_4
#pragma coverity compliance end_block(file) MISRA_C_2012_Rule_21_2
#pragma coverity compliance end_block(file) MISRA_C_2012_Rule_1_2

#endif /* defined (_MISRA_SUPPRESSIONS_*) */
