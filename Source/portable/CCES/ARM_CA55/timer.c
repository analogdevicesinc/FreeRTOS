/*
 * Copyright (C) 2016-2021 Analog Devices Inc. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE
 */


/*
 * timer.c
 *
 */

#include <runtime/int/interrupt.h>
#include <services/int/adi_gic.h>
#include <sys/platform.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include <adi_osal.h>

/* FreeRTOS_Tick_Handler() is defined in the RTOS port layer */
extern void FreeRTOS_Tick_Handler( void );

/* Static functions used for timer setup. */
static void tickHandler(uint32_t id, void *param);

/* The constant ADI_CFG_GP_TMR_NUM should probably be defined in FreeRTOSConfig.h */
#ifndef ADI_CFG_GP_TMR_NUM
#define ADI_CFG_GP_TMR_NUM 0
#endif

#if !defined (__ADSPSC598_FAMILY__)
#error "This processor family is not supported"
#endif

#if !defined (ADI_CFG_GP_TMR_NUM)
#error "Set up tick management timer is enabled but no GP timer is specified"
#endif

#if (ADI_CFG_GP_TMR_NUM >= PARAM_TIMER0_NUMTIMERS)
#error "Incorrect GP Timer number specified for the processor."
#endif

#ifndef configSCLK0_HZ
/* SCLK0 is normally one quarter of the CPU clock */
#define configSCLK0_HZ (125000000)
#endif

/* Macros to construct the desired register name from the timer number. */
#define  REG_NAME_STR(RN, END)  pREG_TIMER0_TMR##RN##_##END
#define  REG_NAME(RN, END)      REG_NAME_STR(RN, END)

/* Macros to construct the desired interrupt name from the timer number. */
#define  INT_NAME_STR(RN)       INTR_TIMER0_TMR0##RN
#define  INT_NAME(RN)           INT_NAME_STR(RN)

#define TIMER_NUM (ADI_CFG_GP_TMR_NUM)

#define pTIMERCFG REG_NAME(ADI_CFG_GP_TMR_NUM, CFG)
#define pTIMERCNT REG_NAME(ADI_CFG_GP_TMR_NUM, CNT)
#define pTIMERPER REG_NAME(ADI_CFG_GP_TMR_NUM, PER)
#define pTIMERWID REG_NAME(ADI_CFG_GP_TMR_NUM, WID)
#define pTIMERDLY REG_NAME(ADI_CFG_GP_TMR_NUM, DLY)
#define TIMERINT (uint32_t)INT_NAME(ADI_CFG_GP_TMR_NUM)

#define SET_TIMER_MSK_BIT ((uint16_t)(1u << TIMER_NUM))
#define CLR_TIMER_MSK_BIT ((uint16_t)~(1u << TIMER_NUM))

/* Handler for GP Timer interrupt. */
static void tickHandler(uint32_t id, void *param)
{
	/* Call the RTOS tick handler */
	FreeRTOS_Tick_Handler();

    /* Clear the timer interrupt latch before returning (write-1-to-clear) */
    *pREG_TIMER0_DATA_ILAT = SET_TIMER_MSK_BIT;
}

/*
 * Set up a GP Timer as the RTOS tick interrupt source.
 *
 * This function configures a GP Timer to be used for timing services.
 * The GP Timer to be used is selected by a ADI_CFG_GP_TMR_NUM macro, which is
 * defined by FreeRTOSConfig.h.  This function only modifies the registers, and
 * parts of registers, that apply to the given GP timer.
 */

void vConfigureTickInterrupt(void)
{
	/* GP timers are clocked at the SCLK0 frequency */
	const uint32_t nCycles = configSCLK0_HZ / configTICK_RATE_HZ;

    /* Write 1 to configure the timer to "abrupt halt" configuration (doesn't stop timer) */
    *pREG_TIMER0_STOP_CFG_SET    =  SET_TIMER_MSK_BIT;

    /* Write 1 to set the timer to stop (does stop timer). */
    *pREG_TIMER0_RUN_CLR         =  SET_TIMER_MSK_BIT;

    /* Clear any interrupts (write 1 to clear) */
    *pREG_TIMER0_DATA_ILAT       =  SET_TIMER_MSK_BIT;
    *pREG_TIMER0_STAT_ILAT       =  SET_TIMER_MSK_BIT;

    /* Hide all the interrupts (write 1 to disable interrupt)*/
    *pREG_TIMER0_DATA_IMSK       |=  SET_TIMER_MSK_BIT;
    *pREG_TIMER0_STAT_IMSK       |=  SET_TIMER_MSK_BIT;

    /* Clear the trigger output mask (1 to disable) */
    *pREG_TIMER0_TRG_MSK         |=  SET_TIMER_MSK_BIT;

    /* Clear the trigger input mask (0 to disable) */
    *pREG_TIMER0_TRG_IE          &=  CLR_TIMER_MSK_BIT;

    /* Clear the trigger input and output masks */
    *pREG_TIMER0_TRG_MSK         &=  CLR_TIMER_MSK_BIT;
    *pREG_TIMER0_TRG_IE          &=  CLR_TIMER_MSK_BIT;

    adi_osal_InstallHandler(TIMERINT,   /* GP timer        */
                            tickHandler,  /* Timer handler   */
                            NULL);      /* No callback arg */


    adi_gic_EnableInt(TIMERINT, true);

    /* Clear all registers before starting */
    *pTIMERCFG = 0u;
    *pTIMERDLY = 0u;
    *pTIMERWID = 0u;
    *pTIMERPER = 0u;

    /* Configure the individual timer */
    *pTIMERCFG = (uint16_t)(  ENUM_TIMER_TMR_CFG_CLKSEL_SCLK   /* Use the system clock as a  source */
                            | ENUM_TIMER_TMR_CFG_IRQMODE1      /* Generate pulse when Delay is over */
                            | ENUM_TIMER_TMR_CFG_PWMCONT_MODE  /* Continuous PWM mode */
                            | ENUM_TIMER_TMR_CFG_POS_EDGE      /* Interrupt on the positive pulse edge */
                            | ENUM_TIMER_TMR_CFG_EMU_NOCNT);   /* Timer stops during emulation */

    /* Set the timer delay */
    *pTIMERDLY = nCycles;          /* Interrupt generated at this count */
    *pTIMERWID = 0x1u;             /* Pulse held high for 1 cycle       */
    *pTIMERPER = nCycles + 0x1u;   /* Sequence restarts after these cycles       */

    /* Enable data interrupts (0 for enable, 1 for disable) */
    *pREG_TIMER0_DATA_IMSK   &=  CLR_TIMER_MSK_BIT;

    /* Start the timer */
    *pREG_TIMER0_RUN_SET = SET_TIMER_MSK_BIT;
}



