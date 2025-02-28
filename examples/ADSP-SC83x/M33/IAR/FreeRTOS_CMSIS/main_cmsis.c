/* Copyright(c) 2024 Analog Devices, Inc. All Rights Reserved. This software is
 * proprietary to Analog Devices, Inc. and its licensors.
 */

/*****************************************************************************
 * FreeRTOS on ADSP-SC839 M33 using CMSIS
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

void userTask( void *pvParameters )
{
   char *taskName = (char*)pvParameters;
   int i = 0;
   for ( ; ; )
   {
      if(i % 100 == 0)
         printf("Hello world %s #%d\n", taskName, i);
      i++;
      vTaskDelay(10);
   }
}


int main(int argc, char *argv[])
{
	/* Begin adding your custom code here */
   printf("FreeRTOS with CMSIS\n");

	/* Create a FreeRTOS task to run when the scheduler starts */
	BaseType_t xReturned;
	TaskHandle_t startupTaskHandle;
	xReturned = xTaskCreate( userTask,
				"Task A",
				configMINIMAL_STACK_SIZE,
				"A",
				tskIDLE_PRIORITY + 2,
				&startupTaskHandle );

	if( xReturned != pdPASS )
	{
      abort();
   }

   xReturned = xTaskCreate( userTask,
				"Task B",
				configMINIMAL_STACK_SIZE,
				"B",
				tskIDLE_PRIORITY + 2,
				&startupTaskHandle );

	if( xReturned != pdPASS )
	{
      abort();
   }

   xReturned = xTaskCreate( userTask,
				"Task C",
				configMINIMAL_STACK_SIZE,
				"C",
				tskIDLE_PRIORITY + 2,
				&startupTaskHandle );

	if( xReturned != pdPASS )
	{
      abort();
   }

   xReturned = xTaskCreate( userTask,
				"Task D",
				configMINIMAL_STACK_SIZE,
				"D",
				tskIDLE_PRIORITY + 2,
				&startupTaskHandle );

	if( xReturned != pdPASS )
	{
      abort();
   }

	/* Start the scheduler. */
	vTaskStartScheduler();

	return 0;
}

/*
  vApplicationGetIdleTaskMemory gets called when configSUPPORT_STATIC_ALLOCATION
  equals to 1 and is required for static memory allocation support.
*/
void vApplicationGetIdleTaskMemory (StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {
  /* Idle task control block and stack */
  static StaticTask_t Idle_TCB;
  static StackType_t  Idle_Stack[configMINIMAL_STACK_SIZE];

  *ppxIdleTaskTCBBuffer   = &Idle_TCB;
  *ppxIdleTaskStackBuffer = &Idle_Stack[0];
  *pulIdleTaskStackSize   = (uint32_t)configMINIMAL_STACK_SIZE;
}

/*
  vApplicationGetTimerTaskMemory gets called when configSUPPORT_STATIC_ALLOCATION
  equals to 1 and is required for static memory allocation support.
*/
void vApplicationGetTimerTaskMemory (StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize) {
  /* Timer task control block and stack */
  static StaticTask_t Timer_TCB;
  static StackType_t  Timer_Stack[configTIMER_TASK_STACK_DEPTH];

  *ppxTimerTaskTCBBuffer   = &Timer_TCB;
  *ppxTimerTaskStackBuffer = &Timer_Stack[0];
  *pulTimerTaskStackSize   = (uint32_t)configTIMER_TASK_STACK_DEPTH;
}

void vApplicationStackOverflowHook (TaskHandle_t xTask, char *pcTaskName) {
  (void)xTask;
  (void)pcTaskName;

  /* Assert when stack overflow is enabled but no application defined function exists */
  configASSERT(0);
}

void vApplicationMallocFailedHook (void) {

  configASSERT(0);
}
