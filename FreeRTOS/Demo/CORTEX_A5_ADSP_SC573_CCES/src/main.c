/*
,     FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/* Standard includes. */
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


/* Demo app includes. */
#include "partest.h"
#include "flash.h"
#include "BlockQ.h"
#include "death.h"
#include "PollQ.h"
#include "recmutex.h"
#include "StaticAllocation.h"

/* CCES includes */
#include "adi_initialize.h"

/* The priorities assigned to the tasks. */
#define mainFLASH_TASK_PRIORITY			( tskIDLE_PRIORITY + 1 )
#define mainBLOCK_Q_PRIORITY			( tskIDLE_PRIORITY + 3 )
#define mainQUEUE_POLL_PRIORITY		        ( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY			( tskIDLE_PRIORITY + 4 )
#define mainCREATOR_TASK_PRIORITY 	        ( tskIDLE_PRIORITY + 3 )

/* The check task uses the sprintf function so requires a little more stack. */
#define mainCHECK_TASK_STACK_SIZE		( configMINIMAL_STACK_SIZE + 50 )

/* Dimension the buffer used to write the error flag string. */
#define mainMAX_FLAG_STRING_LEN		        ( 32 )

/* The time between cycles of the 'check' task. */
#define mainCHECK_DELAY				( ( TickType_t ) 5000 / portTICK_PERIOD_MS )

/* Error status flag. */
static unsigned int ulErrorFlags = 0;

/*-----------------------------------------------------------*/

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )
/* RTOS memory */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

void vApplicationGetIdleTaskMemory(StaticTask_t ** ppxIdleTaskTCBBuffer, StackType_t ** ppxIdleTaskStackBuffer, uint32_t * pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer   = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize   = configMINIMAL_STACK_SIZE;
}

#if ( configUSE_TIMERS == 1 )
/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

	/* Pass out a pointer to the StaticTask_t structure in which the Timer
	task's state will be stored. */
	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

	/* Pass out the array that will be used as the Timer task's stack. */
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;

	/* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
	Note that, as the array is necessarily of type StackType_t,
	configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
#endif /* configUSE_TIMERS == 1 */
#endif /* configSUPPORT_STATIC_ALLOCATION == 1 */

/*
 * Configure the hardware as necessary to run this demo.
 */
static void prvSetupHardware( void );

/*
 * Checks the status of all the demo tasks then prints a message to terminal
 * IO.  The message will be either PASS or a message that describes which of the
 * standard demo tasks an error has been discovered in.
 */
static void vCheckTask( void *pvParameters );

/*
 * Checks that all the demo application tasks are still executing without error
 * - as described at the top of the file.  Called by vCheckTask().
 */
static void prvCheckOtherTasksAreStillRunning( void );

/*-----------------------------------------------------------*/

int main(int argc, char *argv[])
{
	/* Initialize managed drivers and/or services */
	adi_initComponents();

    /* Configure the hardware ready to run the demo. */
    prvSetupHardware();

    /* Create a subset of the standard demo tasks. */
    vStartLEDFlashTasks( mainFLASH_TASK_PRIORITY );
    vStartPolledQueueTasks( mainQUEUE_POLL_PRIORITY );
    vStartRecursiveMutexTasks();
    vStartBlockingQueueTasks( mainBLOCK_Q_PRIORITY );
    vStartStaticallyAllocatedTasks();

    /* Start the tasks defined within this file/specific to this demo. */
    xTaskCreate( vCheckTask, "Check", mainCHECK_TASK_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY, NULL );

    /* The death demo tasks must be started last as the sanity checks performed
	require knowledge of the number of other tasks in the system. */
    vCreateSuicidalTasks( mainCREATOR_TASK_PRIORITY );

    /* Start the scheduler. */
    vTaskStartScheduler();

    /* If all is well then this line will never be reached.  If it is reached
    then it is likely that there was insufficient (FreeRTOS) heap memory space
    to create the idle task.  This may have been trapped by the malloc() failed
    hook function, if one is configured. */
    for( ;; );
}

/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
    /* Clock initialization */
   // SystemInit();
    /* test system initialization */
    //test_Init();
    /* Initialise the LEDs. */
    vParTestInitialise();
}

/*-----------------------------------------------------------*/

static void prvCheckOtherTasksAreStillRunning( void )
{
    if( xAreBlockingQueuesStillRunning() != pdTRUE )
    {
        ulErrorFlags |= 0x01;
    }

    if ( xArePollingQueuesStillRunning() != pdTRUE )
    {
        ulErrorFlags |= 0x02;
    }

    if( xIsCreateTaskStillRunning() != pdTRUE )
    {
        ulErrorFlags |= 0x04;
    }

    if( xAreRecursiveMutexTasksStillRunning() != pdTRUE )
    {
        ulErrorFlags |= 0x08;
    }

    if( xAreStaticAllocationTasksStillRunning() != pdTRUE )
    {
        ulErrorFlags |= 0x10;
    }
}

/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}

/*-----------------------------------------------------------*/

void vAssertCalled( const char * pcFile, unsigned long ulLine )
{
volatile unsigned long ul = 0;

    ( void ) pcFile;
    ( void ) ulLine;

    __asm volatile( "cpsid i" );
    while ( ul == 0 )
    {
        __asm volatile( "NOP" );
        __asm volatile( "NOP ");
    }
    __asm volatile( "cpsie i" );
}

/*-----------------------------------------------------------*/

static void prvPrintInformation( void )
{
    if (ulErrorFlags)
    {
        printf("Test failed\n");
        ulErrorFlags = 0;
    }
    else
    {
        printf("Test passed\n");
    }
}

/*-----------------------------------------------------------*/

static void vCheckTask( void *pvParameters )
{
TickType_t xLastExecutionTime;

    xLastExecutionTime = xTaskGetTickCount();

    for( ;; )
    {
     /* Delay until it is time to execute again. */
        vTaskDelayUntil( &xLastExecutionTime, mainCHECK_DELAY );

     /* Check all the other tasks to see if the error flag needs updating. */
        prvCheckOtherTasksAreStillRunning();

     /* print the test result */
        prvPrintInformation();
    }
}

/*-----------------------------------------------------------*/
