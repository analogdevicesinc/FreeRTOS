/*
 * FreeRTOS Kernel V10.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

#include "adi_initialize.h"

#include <FreeRTOS.h>
#include <task.h>
#include "Metrics.h"

#include <services/int/adi_int.h>

/* Prototypes */
void TimingTest(void* in);
void myISR(uint32_t iid, void* value);


/* Handler used to terminate and restart some timing metrics. */
void myISR(uint32_t iid, void* value)
{
	STOP_TIMER

	/* Set to test the interrupt return timings */
	TEST_ID(2)
	/* Start timer */
	START_TIMER
}


void TimingTest(void* in)
{
	BaseType_t result = 0;

	ESTABLISH_TIMER_OVERHEAD;

	/* Register an ISR for the IRQ0 interrupt. */
	adi_int_InstallHandler(INT_ID, myISR, NULL, true);

	for(g_i = 0; g_i < ITERATIONS; g_i++)
	{
		TEST_ID(1)
		START_TIMER
		INT_TRIGGER;
		while(testid == 1) asm volatile("NOP;");  /* Wait for the interrupt to fire */
		STOP_TIMER;

	} /* end ITERATIONS loop */

	PRINT_TEST(1, "Interrupt service time (FreeRTOS)")
	PRINT_TEST(2, "Time to return from an ISR (FreeRTOS, no task switch)")

	exit(result);
}

static TaskHandle_t myTaskTCB;

int main(void)
{
	/* Initialize managed drivers and/or services */
	adi_initComponents();

	ESTABLISH_TIMER_OVERHEAD;

    /* LOWER PRIORITY TASK */
	xTaskCreate ( TimingTest,    /* Task function */
                  "TimingTest",  /* Task name */
                  400u,          /* Stack size in CPU_STK */
                  NULL,          /* Argument for the task function*/
                  4u,            /* Task priority */
				  &myTaskTCB    /* Address of OS_TCB */
				                    );
    vTaskStartScheduler();
	/* Begin adding your custom code here */

	return 0;
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

