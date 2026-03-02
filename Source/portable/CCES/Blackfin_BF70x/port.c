/*
 * FreeRTOS Kernel V10.4.3 LTS Patch 2
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the 
 * Blackfin ADSP-BF70x port.
 *----------------------------------------------------------*/

#if defined(_MISRA_2004_RULES) || defined(_MISRA_2012_RULES)
#pragma diag(push)
#ifdef _MISRA_2012_RULES
#else
#pragma diag(suppress:misra_rule_5_1:"FreeRTOS uses identifiers > 31 characters")
#pragma diag(suppress:misra_rule_10_1_a:"Some macro operations appear to change sign")
#pragma diag(suppress:misra_rule_11_1:"Function pointers passed to void*")
#pragma diag(suppress:misra_rule_13_7:"Assert(0) used for debugging")
#pragma diag(suppress:misra_rule_17_4:"Pointer arithmetic is to manipulate stack")
#pragma diag(suppress:misra_rule_18_1:"FreeRTOS has incomplete external structures")
#pragma diag(suppress:misra_rule_19_4:"FreeRTOS uses macros to enable build-time configuration")
#endif /* _MISRA_2004_RULES */
#endif /* _MISRA_RULES */


#include <string.h> /* for memset() */

#include <sys/platform.h>
#include <blackfin.h>
#include <sys/exception.h>
#include <builtins.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

#ifndef configSYSTICK_CLOCK_HZ
	#define configSYSTICK_CLOCK_HZ configCPU_CLOCK_HZ
	/* Ensure the SysTick is clocked at the same frequency as the core. */
	#define portNVIC_SYSTICK_CLK_BIT	( 1UL << 2UL )
#else
	/* The way the SysTick is clocked is not modified in case it is not the same
	as the core. */
	#define portNVIC_SYSTICK_CLK_BIT	( 0 )
#endif


/* Let the user override the pre-loading of the initial LR with the address of
prvTaskExitError() in case it messes up unwinding of the stack in the
debugger. */
#ifdef configTASK_RETURN_ADDRESS
	#define portTASK_RETURN_ADDRESS	configTASK_RETURN_ADDRESS
#else
	#define portTASK_RETURN_ADDRESS	prvTaskExitError
#endif

#ifndef configTIMER_INTERRUPT
	#define configTIMER_INTERRUPT   (ADI_CID_IVTMR)
#endif


/*
 * Setup the timer to generate the tick interrupts.  The implementation in this
 * file is weak to allow application writers to change the timer used to
 * generate the tick interrupt.
 */
void vPortSetupTimerInterrupt( void );

extern void xPortIVG14Handler(void);

/*
 * Exception handlers.
 */
EX_DISPATCHED_HANDLER_NON_NESTED(xPortSysTickHandler, a, b, c);

/*
 * Start first task is implemented in portASM.asm
 */
extern void prvPortStartFirstTask( void );

/*
 * Used to catch tasks that attempt to return from their implementing function.
 */
static void prvTaskExitError( void );

/*-----------------------------------------------------------*/

/*
 * The number of SysTick increments that make up one tick period.
 */
#if configUSE_TICKLESS_IDLE == 1
	static uint32_t ulTimerCountsForOneTick = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * The maximum number of tick periods that can be suppressed is limited by the
 * 24 bit resolution of the SysTick timer.
 */
#if configUSE_TICKLESS_IDLE == 1
	static uint32_t xMaximumPossibleSuppressedTicks = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * Compensate for the CPU cycles that pass while the SysTick is stopped (low
 * power functionality only.
 */
#if configUSE_TICKLESS_IDLE == 1
	static uint32_t ulStoppedTimerCompensation = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * Used by the portASSERT_IF_INTERRUPT_PRIORITY_INVALID() macro to ensure
 * FreeRTOS API functions are not called from interrupts that have been assigned
 * a priority above configMAX_SYSCALL_INTERRUPT_PRIORITY.
 */
#if ( configASSERT_DEFINED == 1 )
	 static uint8_t ucMaxSysCallPriority = 0;
	 static uint32_t ulMaxPRIGROUPValue = 0;
#endif /* configASSERT_DEFINED */

/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
	/* Simulate the stack frame as it would be created by a context switch
	interrupt. */
    ContextRecord *pRec = (ContextRecord*)(pxTopOfStack - 3);

    /* Allocate a context record on the task's stack */
    pRec -= 1;

    /* Initialize the context record */
	memset(pRec, 0, sizeof(ContextRecord));
	pRec->_PC = (UINT32)pxCode;
	pRec->_R0 = (UINT32)pvParameters;
	pRec->_RETS = (UINT32)portTASK_RETURN_ADDRESS;

	/* Return the new stacktop */
	return (StackType_t*)pRec;
}
/*-----------------------------------------------------------*/

static void prvTaskExitError( void )
{
	/* A function that implements a task must not exit or attempt to return to
	its caller as there is nothing to return to.  If a task wants to exit it
	should instead call vTaskDelete( NULL ).

	Artificially force an assert() to be triggered if configASSERT() is
	defined, then stop here so application writers can catch the error. */
	configASSERT( 0UL );
	portDISABLE_INTERRUPTS(); 
	for( ;; ) {
		__builtin_NOP();
	}
}
/*-----------------------------------------------------------*/



/*
 * See header file for description.
 */
BaseType_t xPortStartScheduler( void )
{
	/* Start the timer that generates the tick ISR.  Interrupts are disabled
	here already. */
	vPortSetupTimerInterrupt();

	*pEVT14 = (void*) xPortIVG14Handler;

	/* Start the first task. */
	prvPortStartFirstTask();

	/* Should never get here as the tasks will now be executing!  Call the task
	exit error function to prevent compiler warnings about a static function
	not being called in the case that the application writer overrides this
	functionality by defining configTASK_RETURN_ADDRESS. */
	prvTaskExitError();

	/* Should not get here! */
	return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* Not implemented in ports where there is nothing to return to.
	Artificially force an assert. */
	configASSERT( 0UL );
}
/*-----------------------------------------------------------*/


void vPortEnterCritical( void )
{
	adi_rtl_disable_interrupts();
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
	adi_rtl_reenable_interrupts();
}
/*-----------------------------------------------------------*/

uint32_t ulPortSetInterruptMask( void )
{
  return cli();

}

/*-----------------------------------------------------------*/

void vPortClearInterruptMask( uint32_t ulNewMaskValue )
{
       sti(ulNewMaskValue);
}

/*-----------------------------------------------------------*/


EX_DISPATCHED_HANDLER_NON_NESTED(xPortSysTickHandler, a, b, c)
{
	/* Increment the RTOS tick. */
	if( xTaskIncrementTick() != pdFALSE )
	{
		/* A context switch is required.  */
		portYIELD();
	}
}

/*-----------------------------------------------------------*/

#if configUSE_TICKLESS_IDLE == 1

#error Tickless IDLE not currently implemented for BlackFin

	#pragma weak_entry
	void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
	{
		/* Not supported */
	}

#endif /* #if configUSE_TICKLESS_IDLE */
/*-----------------------------------------------------------*/

/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
void vPortSetupTimerInterrupt( void )
{
	/* Configure SysTick to interrupt at the requested rate. */
	const uint32_t ulTimerCountsForOneTick = ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ );

	/* Calculate the constants required to configure the tick interrupt. */
	#if configUSE_TICKLESS_IDLE == 1
	{
		xMaximumPossibleSuppressedTicks = portMAX_24_BIT_NUMBER / ulTimerCountsForOneTick;
		ulStoppedTimerCompensation = portMISSED_COUNTS_FACTOR / ( configCPU_CLOCK_HZ / configSYSTICK_CLOCK_HZ );
	}
	#endif /* configUSE_TICKLESS_IDLE */

	*pTCNTL   = 1u;
	*pTSCALE  = 0u;
	*pTCOUNT  = ulTimerCountsForOneTick;
	*pTPERIOD = ulTimerCountsForOneTick;
	adi_rtl_register_dispatched_handler(configTIMER_INTERRUPT, xPortSysTickHandler,	NULL);
	adi_rtl_activate_dispatched_handler(configTIMER_INTERRUPT);

	/* Power-up the core timer, enable it, and set it for auto-reload */
	*pTCNTL   = BITM_TCNTL_PWR | BITM_TCNTL_EN | BITM_TCNTL_AUTORLD;
}
/*-----------------------------------------------------------*/

#if( configASSERT_DEFINED == 1 )

	void vPortValidateInterruptPriority( void )
	{
		/* On BF7xx we don't currently distinguish between interrupt priorities which are valid
		 * for use with FreeRTOS and those which are not - all interrupt priorities can call
		 * FreeRTOS APIs. For this reason this function performs no checking.
		 */
	}

#endif /* configASSERT_DEFINED */

#if defined(_MISRA_2004_RULES) || defined(_MISRA_2012_RULES)
#pragma diag(pop)
#endif





















