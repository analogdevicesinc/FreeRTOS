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

/* Portions Copyright (c) 2024 Analog Devices, Inc. All rights reserved. */

#if defined(_MISRA_2004_RULES) || defined(_MISRA_2012_RULES)
#pragma diag(push)
#ifdef _MISRA_2012_RULES
#else
#pragma diag(suppress:misra_rule_5_1:"FreeRTOS uses identifiers > 31 characters")
#pragma diag(suppress:misra_rule_8_7:"Symbol has global scope as it is used externally")
#endif /* _MISRA_2004_RULES */
#endif /* _MISRA_RULES */

/* Standard includes. */
#include <stdio.h>
#include <builtins.h>

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
#define mainFLASH_TASK_PRIORITY			( tskIDLE_PRIORITY + 1u )
#define mainBLOCK_Q_PRIORITY			( tskIDLE_PRIORITY + 3u )
#define mainQUEUE_POLL_PRIORITY		        ( tskIDLE_PRIORITY + 2u )
#define mainCHECK_TASK_PRIORITY			( tskIDLE_PRIORITY + 4u )
#define mainCREATOR_TASK_PRIORITY 	        ( tskIDLE_PRIORITY + 3u )

/* The check task uses the sprintf function so requires a little more stack. */
#define mainCHECK_TASK_STACK_SIZE		( configMINIMAL_STACK_SIZE + 50u )

/* Dimension the buffer used to write the error flag string. */
#define mainMAX_FLAG_STRING_LEN		        ( 32 )

/* The time between cycles of the 'check' task. */
#define mainCHECK_DELAY				( ( TickType_t ) 5000 / portTICK_PERIOD_MS )

/* Error status flag. */
static unsigned int ulErrorFlags = 0u;

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

/*
 * Get Memory sizes for timer task.
 */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );


/*-----------------------------------------------------------*/

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
 * Prints pass/fail information and clears flags.
 */
static void prvPrintInformation( void );

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
    for( ;; )
    {
    	; /* Infinite loop */
    }
}

/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
    /* Initialise the LEDs. */
    vParTestInitialise();
}

/*-----------------------------------------------------------*/

static void prvCheckOtherTasksAreStillRunning( void )
{
    if( xAreBlockingQueuesStillRunning() != pdTRUE )
    {
        ulErrorFlags |= 0x01u;
    }

    if ( xArePollingQueuesStillRunning() != pdTRUE )
    {
        ulErrorFlags |= 0x02u;
    }

    if( xIsCreateTaskStillRunning() != pdTRUE )
    {
        ulErrorFlags |= 0x04u;
    }

    if( xAreRecursiveMutexTasksStillRunning() != pdTRUE )
    {
        ulErrorFlags |= 0x08u;
    }

    if( xAreStaticAllocationTasksStillRunning() != pdTRUE )
    {
        ulErrorFlags |= 0x10u;
    }
}

/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t xTask, char * pcTaskName )
{
	( void ) pcTaskName;
	( void ) xTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; )
	{
		; /* Infinite loop */
	}
}

/*-----------------------------------------------------------*/

void vAssertCalled( const char * pcFile, unsigned long ulLine )
{
    ( void ) pcFile;
    ( void ) ulLine;

    portDISABLE_INTERRUPTS();
    while ( 1 )
    {
        __builtin_NOP();
    }

}

/*-----------------------------------------------------------*/

static void prvPrintInformation( void )
{
    if (ulErrorFlags)
    {
        printf("Test failed\n");
        ulErrorFlags = 0u;
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


#if defined(_MISRA_2004_RULES) || defined(_MISRA_2012_RULES)
#pragma diag(pop)
#endif
