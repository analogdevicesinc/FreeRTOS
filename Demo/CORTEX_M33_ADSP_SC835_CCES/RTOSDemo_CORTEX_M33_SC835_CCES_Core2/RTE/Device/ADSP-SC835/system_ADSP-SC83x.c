/******************************************************************************
 * @file     system_ADSP-SC83x.c
 * @brief    CMSIS Device System Source File for ADSP-SC83x Devices
 * @version  1.0.0
 ******************************************************************************/
/*
 * Copyright (c) 2009-2019 Arm Limited. All rights reserved.
 * Portions Copyright (c) 2022-2023 Analog Devices, Inc. All rights reserved.
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

#include "ADSP-SC83x.h"
#include "sys/platform.h"


#if defined (_MISRA_SUPPRESSIONS_2012)

/* Rule 1.2: Language extensions should not be used.
*/
#pragma coverity compliance block(file) deviate MISRA_C_2012_Rule_1_2 \
"We use #pragma to document MISRA deviations in this block."

#endif /* defined (_MISRA_SUPPRESSIONS_2012) */


/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/

#define  SYSCLK_IN       (25000000UL)     /* Oscillator frequency */
#define  DCLK1_0_DIVIDER (5UL)            /* DCLK1_0 divider */
#define  SYSTEM_CLOCK    (400000000UL)    /* Maximum frequency */


/*----------------------------------------------------------------------------
  External objects and functions
 *----------------------------------------------------------------------------*/

#if defined (_MISRA_SUPPRESSIONS_2004)

/* Rule 8.7: Objects shall be defined at block scope if they are only accessed
** from within a single function.
*/
#pragma coverity compliance block(file) deviate MISRA_C_2004_Rule_8_7 \
"File defines SystemCoreClock for use in application sources."

/* Rule 8.10: Objects and functions shall have internal linkage unless external
** linkage is required.
*/
#pragma coverity compliance block(file) deviate MISRA_C_2004_Rule_8_10 \
"File defines objects and functions for use in application sources."

#elif defined (_MISRA_SUPPRESSIONS_2012)

/* Rule 8.7: Objects and functions shall have internal linkage unless external
** linkage is required.
*/
#pragma coverity compliance block(file) deviate MISRA_C_2012_Rule_8_7 \
"File defines objects and functions for use in application sources."

/* Rule 8.9: An object should be defined at block scope if its identifier
** only appears in a single function
*/
#pragma coverity compliance block(file) deviate MISRA_C_2012_Rule_8_9 \
"File defines SystemCoreClock for use in application sources."

#endif /* defined (_MISRA_SUPPRESSIONS_*) */


/*----------------------------------------------------------------------------
  System Core Clock Variable
 *----------------------------------------------------------------------------*/

uint32_t SystemCoreClock = SYSTEM_CLOCK;  /* System Core Clock Frequency */

#if defined (_MISRA_SUPPRESSIONS_2004)
#pragma coverity compliance end_block(file) MISRA_C_2004_Rule_8_7
#elif defined (_MISRA_SUPPRESSIONS_2012)
#pragma coverity compliance end_block(file) MISRA_C_2012_Rule_8_9
#endif /* defined (_MISRA_SUPPRESSIONS_*) */


/*----------------------------------------------------------------------------
  System Core Clock update function
 *----------------------------------------------------------------------------*/

#if defined (_MISRA_SUPPRESSIONS_2004)

/* Rule 11.3: A cast should not be performed between a pointer type and an
** integral type.
*/
#pragma coverity compliance block(file) deviate MISRA_C_2004_Rule_11_3 \
"Permit use of the pREG macros for addresses."

#elif defined (_MISRA_SUPPRESSIONS_2012)

/* Rule 11.4: A conversion should not be performed between a pointer to object
** and an integer type
*/
#pragma coverity compliance block(file) deviate MISRA_C_2012_Rule_11_4 \
"Permit use of the pREG macros for addresses."

#endif /* defined (_MISRA_SUPPRESSIONS_*) */

void SystemCoreClockUpdate (void)
{
    /* The input clock frequency */
    uint32_t sys_clkin = SYSCLK_IN;

    /* Read the CDU configuration */
    uint32_t cdu0_cfg2 = *pREG_CDU0_CFG2;
    uint32_t sel = (cdu0_cfg2 & BITM_CDU_CFG_SEL);

    /* Read the CGU_CTL MSEL field */
    uint32_t cgu0_ctl = *pREG_CGU0_CTL;
    uint32_t msel = ((cgu0_ctl & BITM_CGU_CTL_MSEL)
                    >> (uint32_t)BITP_CGU_CTL_MSEL);

    /* Read the CGU0 divider */
    uint32_t cgu0_div = *pREG_CGU0_DIV;

    /* Read the SYSSEL divider */
    uint32_t syssel_divider = ((cgu0_div & BITM_CGU_DIV_SYSSEL)
                              >> (uint32_t)BITP_CGU_DIV_SYSSEL);

    if (sel == ENUM_CDU_CFG_IN0)
    {
        /* We're using SCLK1_0 */
        uint32_t divider;

        /* Read the S1SELEXEN bit */
        uint32_t s1selexen = ((cgu0_ctl & BITM_CGU_CTL_S1SELEXEN)
                             >> (uint32_t)BITP_CGU_CTL_S1SELEXEN);
        if (s1selexen == 1UL)
        {
            /* We're using S1SELEX */
            uint32_t cgu0_divex = *pREG_CGU0_DIVEX;

            /* The divider is S1SELEX */
            divider = ((cgu0_divex & BITM_CGU_DIVEX_S1SELEX)
                        >> (uint32_t)BITP_CGU_DIVEX_S1SELEX);
        }
        else
        {
            /* We're using S1SEL */
            uint32_t s1sel = ((cgu0_div & BITM_CGU_DIV_S1SEL)
                             >> (uint32_t)BITP_CGU_DIV_S1SEL);

            /* The divider is S1SEL * SYSSEL divider */
            divider = s1sel * syssel_divider;
        }
        SystemCoreClock = (sys_clkin * msel) / divider;
    }
    else if (sel == ENUM_CDU_CFG_IN1)
    {
        /* We're using SYSCLK0_0 */
        SystemCoreClock = (sys_clkin * msel) / syssel_divider;
    }
    else
    {
        /* We're using DCLK1_0/2 */
        SystemCoreClock = (sys_clkin * msel) / DCLK1_0_DIVIDER;
    }
}

#if defined (_MISRA_SUPPRESSIONS_2004)
#pragma coverity compliance end_block(file) MISRA_C_2004_Rule_11_3
#elif defined (_MISRA_SUPPRESSIONS_2012)
#pragma coverity compliance end_block(file) MISRA_C_2012_Rule_11_4
#endif /* defined (_MISRA_SUPPRESSIONS_*) */

/*----------------------------------------------------------------------------
  System initialization function
 *----------------------------------------------------------------------------*/

#if defined (_MISRA_SUPPRESSIONS_2004)

/* Rule 11.3: A cast should not be performed between a pointer type and an
** integral type.
*/
#pragma coverity compliance block(file) deviate MISRA_C_2004_Rule_11_3 \
"The CMSIS-defined SCB macro casts from an integer address to a pointer."

#elif defined (_MISRA_SUPPRESSIONS_2012)

/* Rule 11.4: A cast should not be performed between a pointer type and an
** integral type.
*/
#pragma coverity compliance block(file) deviate MISRA_C_2012_Rule_11_4 \
"The CMSIS-defined SCB macro casts from an integer address to a pointer."

#endif /* defined (_MISRA_SUPPRESSIONS_*) */

void SystemInit (void)
{

#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
    SCB->VTOR = (uint32_t) &(__VECTOR_TABLE[0]);
#endif /* defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U) */

#if defined (__FPU_USED) && (__FPU_USED == 1U)
    SCB->CPACR |= ((3U << 10U*2U) |           /* enable CP10 Full Access */
                   (3U << 11U*2U)  );         /* enable CP11 Full Access */
#endif /* defined (__FPU_USED) && (__FPU_USED == 1U) */

#if defined(UNALIGNED_SUPPORT_DISABLE)
    SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif /* UNALIGNED_SUPPORT_DISABLE */

    SystemCoreClockUpdate();
}


#if defined (_MISRA_SUPPRESSIONS_2004)
#pragma coverity compliance end_block(file) MISRA_C_2004_Rule_11_3
#pragma coverity compliance end_block(file) MISRA_C_2004_Rule_8_10
#elif defined (_MISRA_SUPPRESSIONS_2012)
#pragma coverity compliance end_block(file) MISRA_C_2012_Rule_11_4
#pragma coverity compliance end_block(file) MISRA_C_2012_Rule_8_7
#pragma coverity compliance end_block(file) MISRA_C_2012_Rule_1_2
#endif /* defined (_MISRA_SUPPRESSIONS_*) */
