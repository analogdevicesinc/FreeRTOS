/** 
 * @file  McapiDemo.c
 * 
 * Copyright 2020 Analog Devices, Inc.
***************************************************************************/

/* Scheduler include files. */
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include <mcapi.h>

/* Demo program include files. */
#include "McapiMsgDemo.h"

static volatile int mcapi_run_round = 0;
static volatile bool test_pass = pdFALSE;

static portTASK_FUNCTION( vMcapiDemoTask, pvParameters );
/*-----------------------------------------------------------*/
/*
 * mcapiErrorCheck()
 *
 * This function checks the status code, and if it is not MCAPI_SUCCESS then
 * an error message is output and the program exits. This should typically
 * be called after every MCAPI call which returns a status, during development
 * and debugging at least.
 */
static void mcapiErrorCheck(mcapi_status_t mcapi_status, const char *psContext, int result)
{
	char errorStringBuff[BUF_SIZE];

	if ((MCAPI_SUCCESS != mcapi_status) && (MCAPI_PENDING != mcapi_status))
	{
		mcapi_display_status(mcapi_status, errorStringBuff, sizeof(errorStringBuff));
		printf("MCAPI Error %s, status = %d [%s]\n",
				psContext,
				mcapi_status,
				errorStringBuff);
		test_pass = pdFALSE;
		exit(result);
	}
}

/*-----------------------------------------------------------*/

void vStartMcapiDemoTask( UBaseType_t uxPriority )
{
	xTaskCreate( vMcapiDemoTask, "McapiDemo", mainMCAPI_TASK_STACK_SIZE, NULL, uxPriority, ( TaskHandle_t * ) NULL );
}
/*-----------------------------------------------------------*/

static portTASK_FUNCTION( vMcapiDemoTask, pvParameters )
{
	int i;
	int recv_num = 0;
	int digit = 0;
	size_t msg_size = 0;
	char msg[BUF_SIZE];
	char vBuffer[BUF_SIZE];
	mcapi_status_t mcapi_status;
	mcapi_endpoint_t local_ep;
	mcapi_endpoint_t remote_ep;

	/* Just to stop compiler warnings. */
	( void ) pvParameters;

	printf("#### Hello MCAPI testing CORE1 ####\n");

	local_ep = mcapi_endpoint_create(CORE1_PORT_NUM, &mcapi_status);
	mcapiErrorCheck(mcapi_status, "create endpoint", 2);

	remote_ep = mcapi_endpoint_get(DOMAIN,
			                       NODE_0,
								   CORE0_PORT_NUM1,
								   MCAPI_TIMEOUT_INFINITE,
								   &mcapi_status);
	mcapiErrorCheck(mcapi_status, "get endpoint", 2);

	for (i = 0; i < TEST_ROUNDS; i++) {
		mcapi_msg_recv(local_ep,
					   msg,
					   sizeof(msg)-1,
					   &msg_size,
					   &mcapi_status);
		mcapiErrorCheck(mcapi_status, "msg_recv", 2);
		if (MCAPI_SUCCESS == mcapi_status) {
			msg[msg_size] = '\0';
			sscanf(msg, "core0 send to core1 %d", &digit);
			snprintf(vBuffer, sizeof(vBuffer), "%s %d", CORE1_SEND_STRING, ++digit);
		}

		/* Send a message back */
		mcapi_msg_send(local_ep,
					   remote_ep,
					   vBuffer,
					   strlen(vBuffer),
					   0,
					   &mcapi_status);
		mcapiErrorCheck(mcapi_status, "msg_send", 2);
		mcapi_run_round++;
	}
	printf("Core1 Test %d rounds Passed\n", i);
	mcapi_endpoint_delete(local_ep, &mcapi_status);
	mcapiErrorCheck(mcapi_status, "del_endpoint", 2);
	mcapi_finalize(&mcapi_status);
	mcapiErrorCheck(mcapi_status, "finalize", 2);

	for( ;; )
	{
		test_pass = pdPASS;
	}
} /*lint !e715 !e818 pvParameters is required for a task function even if it is not referenced. */
/*-----------------------------------------------------------*/

BaseType_t xAreMcapiDemoTaskStillRunning( void )
{
static int sLastRunCount = 0;
BaseType_t xReturn = pdPASS;

	/* return pdFALSE if any are found not to have changed since the last call. */
	if( mcapi_run_round == sLastRunCount && test_pass == pdFALSE)
	{
		xReturn = pdFALSE;
	}
	sLastRunCount = mcapi_run_round;

	return xReturn;
}
