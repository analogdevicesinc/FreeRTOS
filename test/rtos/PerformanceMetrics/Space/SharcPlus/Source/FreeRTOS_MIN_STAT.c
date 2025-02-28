/*****************************************************************************
 * uCOSIII_MIN.c
 *****************************************************************************/

#include "adi_initialize.h"
#include <os.h>
#include <stdlib.h>

CPU_STK TaskStack[400u];
OS_TCB myTaskTCB;

void TimingTest(void* in)
{
	/* The simplest of run functions */
	while(1);
}

int main(void)
{
	/* Initialize managed drivers and/or services */
	adi_initComponents();

    /* most uCOS-III APIs have an error as an output argument */
    OS_ERR err;

    OSTaskCreate (&myTaskTCB,    /* Address of OS_TCB */
                  "TimingTest",  /* Task name */
                  TimingTest,    /* Task function */
                  NULL,          /* Argument for the task function*/
                  5u,            /* Task priority */
                  TaskStack,     /* Base of the stack */
                  0u,            /* Limit for stack growth */
                  400u,          /* Stack size in CPU_STK */
                  0u,            /* Number of messages allowed */
                  (OS_TICK) 0u,  /* Time quanta */
                  NULL,          /* extension pointer */
                  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* task options */
                  &err);

    OSStart(&err);
	/* Begin adding your custom code here */

	return 0;
}
