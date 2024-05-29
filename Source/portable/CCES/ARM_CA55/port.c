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
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 * 1 tab == 4 spaces!
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* ssl/dd includes */
#include <services/int/adi_gic.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

#ifndef configMAX_API_CALL_INTERRUPT_PRIORITY
	#error configMAX_API_CALL_INTERRUPT_PRIORITY must be defined.  See https://www.FreeRTOS.org/Using-FreeRTOS-on-Cortex-A-Embedded-Processors.html
#endif

#if configMAX_API_CALL_INTERRUPT_PRIORITY == 0
	#error configMAX_API_CALL_INTERRUPT_PRIORITY must not be set to 0
#endif

#if configMAX_API_CALL_INTERRUPT_PRIORITY > configUNIQUE_INTERRUPT_PRIORITIES
	#error configMAX_API_CALL_INTERRUPT_PRIORITY must be less than or equal to configUNIQUE_INTERRUPT_PRIORITIES as the lower the numeric priority value the higher the logical interrupt priority
#endif

#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1
	/* Check the configuration. */
	#if( configMAX_PRIORITIES > 32 )
		#error configUSE_PORT_OPTIMISED_TASK_SELECTION can only be set to 1 when configMAX_PRIORITIES is less than or equal to 32.  It is very rare that a system requires more than 10 to 15 difference priorities as tasks that share a priority will time slice.
	#endif
#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */

/* In case security extensions are implemented. */
#if configMAX_API_CALL_INTERRUPT_PRIORITY <= ( configUNIQUE_INTERRUPT_PRIORITIES / 2 )
	#error configMAX_API_CALL_INTERRUPT_PRIORITY must be greater than ( configUNIQUE_INTERRUPT_PRIORITIES / 2 )
#endif

/* Some vendor specific files default configCLEAR_TICK_INTERRUPT() in
portmacro.h. */
#ifndef configCLEAR_TICK_INTERRUPT
	#define configCLEAR_TICK_INTERRUPT()
#endif

/* A critical section is exited when the critical section nesting count reaches
this value. */
#define portNO_CRITICAL_NESTING			( ( size_t ) 0 )

/* In all GICs 255 can be written to the priority mask register to unmask all
(but the lowest) interrupt priority. */
#define portUNMASK_VALUE				( 0xFFUL )

/* Tasks are not created with a floating point context, but can be given a
floating point context after they have been created.  A variable is stored as
part of the tasks context that holds portNO_FLOATING_POINT_CONTEXT if the task
does not have an FPU context, or any other value if the task does have an FPU
context. */
#define portNO_FLOATING_POINT_CONTEXT	( ( StackType_t ) 0 )

/* Constants required to setup the initial task context. */
#define portSP_ELx						( ( StackType_t ) 0x01 )
#define portSP_EL0						( ( StackType_t ) 0x00 )

#define portEL3						( ( StackType_t ) 0x0c )
/* At the time of writing, the BSP only supports EL3. */
#define portINITIAL_PSTATE			( portEL3 | portSP_EL0 )


/* Used by portASSERT_IF_INTERRUPT_PRIORITY_INVALID() when ensuring the binary
point is zero. */
#define portBINARY_POINT_BITS			( ( uint8_t ) 0x03 )

/* Masks all bits in the APSR other than the mode bits. */
#define portAPSR_MODE_BITS_MASK			( 0x0C )

/* The I bit in the DAIF bits. */
#define portDAIF_I						( 0x80 )

/* Macro to unmask all interrupt priorities. */
#define portCLEAR_INTERRUPT_MASK()								\
{																			\
	portDISABLE_INTERRUPTS();										\
	adi_gic_SetIntPriorityMask(portUNMASK_VALUE);			\
	__asm volatile (	"DSB SY		\n"							\
						"ISB SY		\n" );							\
	portENABLE_INTERRUPTS();										\
}

/* Hardware specifics used when sanity checking the configuration. */
#define portINTERRUPT_PRIORITY_REGISTER_OFFSET		0x400UL
#define portMAX_8_BIT_VALUE							( ( uint8_t ) 0xff )
#define portBIT_0_SET								( ( uint8_t ) 0x01 )

/*-----------------------------------------------------------*/

/*
 * Starts the first task executing.  This function is necessarily written in
 * assembly code so is implemented in portASM.s.
 */
extern void vPortRestoreTaskContext( void );

/*-----------------------------------------------------------*/

/* A variable is used to keep track of the critical section nesting.  This
variable has to be stored as part of the task context and must be initialised to
a non zero value to ensure interrupts don't inadvertently become unmasked before
the scheduler starts.  As it is stored as part of the task context it will
automatically be set to 0 when the first task is started. */
volatile uint64_t ullCriticalNesting = 9999ULL;

/* Saved as part of the task context.  If ullPortTaskHasFPUContext is non-zero
then floating point context must be saved and restored for the task. */
uint64_t ullPortTaskHasFPUContext = pdFALSE;

/* Set to 1 to pend a context switch from an ISR. */
uint64_t ullPortYieldRequired = pdFALSE;

/* Counts the interrupt nesting depth.  A context switch is only performed if
if the nesting depth is 0. */
uint64_t ullPortInterruptNesting = 0;

/* Used in the ASM code. */
__attribute__(( used )) const uint64_t ullMaxAPIPriorityMask = ( configMAX_API_CALL_INTERRUPT_PRIORITY << portPRIORITY_SHIFT );

/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
	/* Setup the initial stack of the task.  The stack is set exactly as
	expected by the portRESTORE_CONTEXT() macro. */

	/* First all the general purpose registers. */
	pxTopOfStack--;
	*pxTopOfStack = 0x0101010101010101ULL;	/* R1 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) pvParameters; /* R0 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0303030303030303ULL;	/* R3 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0202020202020202ULL;	/* R2 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0505050505050505ULL;	/* R5 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0404040404040404ULL;	/* R4 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0707070707070707ULL;	/* R7 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0606060606060606ULL;	/* R6 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0909090909090909ULL;	/* R9 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0808080808080808ULL;	/* R8 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1111111111111111ULL;	/* R11 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1010101010101010ULL;	/* R10 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1313131313131313ULL;	/* R13 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1212121212121212ULL;	/* R12 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1515151515151515ULL;	/* R15 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1414141414141414ULL;	/* R14 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1717171717171717ULL;	/* R17 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1616161616161616ULL;	/* R16 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1919191919191919ULL;	/* R19 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1818181818181818ULL;	/* R18 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2121212121212121ULL;	/* R21 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2020202020202020ULL;	/* R20 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2323232323232323ULL;	/* R23 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2222222222222222ULL;	/* R22 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2525252525252525ULL;	/* R25 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2424242424242424ULL;	/* R24 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2727272727272727ULL;	/* R27 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2626262626262626ULL;	/* R26 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2929292929292929ULL;	/* R29 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2828282828282828ULL;	/* R28 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00;	/* XZR - has no effect, used so there are an even number of registers. */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00;	/* R30 - procedure call link register. */
	pxTopOfStack--;

	*pxTopOfStack = portINITIAL_PSTATE;
	pxTopOfStack--;

	*pxTopOfStack = ( StackType_t ) pxCode; /* Exception return address. */
	pxTopOfStack--;

	/* The task will start with a critical nesting count of 0 as interrupts are
	enabled. */
	*pxTopOfStack = portNO_CRITICAL_NESTING;
	pxTopOfStack--;

	/* The task will start without a floating point context.  A task that uses
	the floating point hardware must call vPortTaskUsesFPU() before executing
	any floating point instructions. */
	*pxTopOfStack = portNO_FLOATING_POINT_CONTEXT;

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void )
{
	uint32_t ulAPSR;
	register volatile uint64_t icc_bpr;

   #if( configASSERT_DEFINED == 1 )
   {
      uint32_t ulOriginalPriority;
      uint32_t ucMaxPriorityValue;

      /* Determine how many priority bits are implemented in the GIC.

      Save the interrupt priority value that is about to be clobbered. */

	  	adi_gic_GetIntPriorityMask(&ulOriginalPriority);

      /* Determine the number of priority bits available.  First write to
      all possible bits. */
      adi_gic_SetIntPriorityMask(portMAX_8_BIT_VALUE);

      /* Read the value back to see how many bits stuck. */
      adi_gic_GetIntPriorityMask(&ucMaxPriorityValue);

      /* Shift to the least significant bits. */
      while( ( ucMaxPriorityValue & portBIT_0_SET ) != portBIT_0_SET )
      {
         ucMaxPriorityValue >>= ( uint32_t ) 0x01;
      }

      /* Sanity check configUNIQUE_INTERRUPT_PRIORITIES matches the read
      value. */

      configASSERT( ucMaxPriorityValue >= portLOWEST_INTERRUPT_PRIORITY );


      /* Restore the clobbered interrupt priority register to its original
      value. */
      adi_gic_SetIntPriorityMask(ulOriginalPriority);

		/* Only continue if the binary point value is set to its lowest possible
   	setting. */
		/* Using special encoding s3_0_c12_c8_3 for ICC_BPR0_EL1 */
      asm volatile ("MRS %0, s3_0_c12_c8_3 \n" : "=r" (icc_bpr) );   	
		configASSERT( ( icc_bpr & portBINARY_POINT_BITS ) <= portMAX_BINARY_POINT_VALUE );

   }
   #endif /* conifgASSERT_DEFINED */


	/* At the time of writing, the BSP only supports EL3. */
	__asm volatile ( "MRS %0, CurrentEL" : "=r" ( ulAPSR ) );
	ulAPSR &= portAPSR_MODE_BITS_MASK;

	configASSERT( ulAPSR == portEL3 );
	/* Interrupts are turned off in the CPU itself to ensure a tick does
	not execute	while the scheduler is being started.  Interrupts are
	automatically turned back on in the CPU when the first task starts
	executing. */


	portDISABLE_INTERRUPTS();

	/* Start the timer that generates the tick ISR. */
	configSETUP_TICK_INTERRUPT();

	/* Start the first task executing. */
	vPortRestoreTaskContext();

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
	/* Mask interrupts up to the max syscall interrupt priority. */
	uxPortSetInterruptMask();

	/* Now interrupts are disabled ullCriticalNesting can be accessed
	directly.  Increment ullCriticalNesting to keep a count of how many times
	portENTER_CRITICAL() has been called. */
	ullCriticalNesting++;

	/* This is not the interrupt safe version of the enter critical function so
	assert() if it is being called from an interrupt context.  Only API
	functions that end in "FromISR" can be used in an interrupt.  Only assert if
	the critical nesting count is 1 to protect against recursive calls if the
	assert function also uses a critical section. */
	if( ullCriticalNesting == 1ULL )
	{
		configASSERT( ullPortInterruptNesting == 0 );
	}
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
	if( ullCriticalNesting > portNO_CRITICAL_NESTING )
	{
		/* Decrement the nesting count as the critical section is being
		exited. */
		ullCriticalNesting--;

		/* If the nesting level has reached zero then all interrupt
		priorities must be re-enabled. */
		if( ullCriticalNesting == portNO_CRITICAL_NESTING )
		{
			/* Critical nesting has reached zero so all interrupt priorities
			should be unmasked. */
			portCLEAR_INTERRUPT_MASK();
		}
	}
}
/*-----------------------------------------------------------*/

void FreeRTOS_Tick_Handler( void )
{

   portDISABLE_INTERRUPTS();

   /* Set interrupt mask before altering scheduler structures.   The tick
   handler runs at the lowest priority, so interrupts cannot already be masked,
   so there is no need to save and restore the current mask value.  It is
   necessary to turn off interrupts in the CPU itself while the ICCPMR is being
   updated. */
   adi_gic_SetIntPriorityMask(( uint32_t ) ( configMAX_API_CALL_INTERRUPT_PRIORITY << portPRIORITY_SHIFT ));
   __asm volatile ("dsb sy     \n"
                   "isb sy     \n" ::: "memory" );

   /* Ok to enable interrupts after the interrupt source has been cleared. */
   configCLEAR_TICK_INTERRUPT();
   portENABLE_INTERRUPTS();

   /* Increment the RTOS tick. */
   if( xTaskIncrementTick() != pdFALSE )
   {
      ullPortYieldRequired = pdTRUE;
   }

   /* Ensure all interrupt priorities are active again. */
   portCLEAR_INTERRUPT_MASK();

}
/*-----------------------------------------------------------*/

void vPortTaskUsesFPU( void )
{
	/* A task is registering the fact that it needs an FPU context.  Set the
	FPU flag (which is saved as part of the task context). */
	ullPortTaskHasFPUContext = pdTRUE;

	/* Consider initialising the FPSR here - but probably not necessary in
	AArch64. */
}
/*-----------------------------------------------------------*/

static uint32_t uPreservedIntMask;
void vPortClearInterruptMask( UBaseType_t uxNewMaskValue )
{
	if( uxNewMaskValue == pdFALSE )
	{
		adi_gic_SetIntPriorityMask(uPreservedIntMask);
	}
}
/*-----------------------------------------------------------*/


UBaseType_t uxPortSetInterruptMask( void )
{
    uint32_t ulReturn = pdTRUE;
    uint32_t uNewIntMask = configMAX_API_CALL_INTERRUPT_PRIORITY << portPRIORITY_SHIFT;
    uint32_t uCurrIntMask;

    portDISABLE_INTERRUPTS();

    adi_gic_GetIntPriorityMask( &uCurrIntMask );

    if( uCurrIntMask != uNewIntMask )
    {
        uPreservedIntMask = uCurrIntMask;
	     adi_gic_SetIntPriorityMask(uNewIntMask);
        __asm volatile ("dsb sy		\n"
		             		"isb sy		\n" ::: "memory" );
        ulReturn = pdFALSE;
    }

    portENABLE_INTERRUPTS();

	return ulReturn;
}
/*-----------------------------------------------------------*/

#if( configASSERT_DEFINED == 1 )

	void vPortValidateInterruptPriority( void )
	{
		uint32_t pPriority;
		register volatile uint64_t icc_bpr;
		adi_gic_GetRunningIntPriority(&pPriority);

		pPriority >>= portPRIORITY_SHIFT;

	   configASSERT( pPriority <= configMAX_API_CALL_INTERRUPT_PRIORITY );

		/* Check binary point is at lowest setting */
		/* Using special encoding s3_0_c12_c8_3 for ICC_BPR0_EL1 */
      asm volatile ("MRS %0, s3_0_c12_c8_3 \n" : "=r" (icc_bpr) );   	
		configASSERT( ( icc_bpr & portBINARY_POINT_BITS ) <= portMAX_BINARY_POINT_VALUE );
	}

#endif /* configASSERT_DEFINED */
/*-----------------------------------------------------------*/

