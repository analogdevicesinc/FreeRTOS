/*****************************************************************************
 * Example: McapiMsgDemo
 * Project: RTOSDemo_SC589_MCAPI_Msg_Core0
 * File:    McapiMsgDemoCore0.c
 *
 * Copyright(c) 2013-2020 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary and confidential.  By using this software you
 * agree to the terms of the associated Analog Devices License Agreement.
 *****************************************************************************/

/* Scheduler include files. */
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <mcapi.h>

/* Demo program include files. */
#include "McapiMsgDemo.h"

typedef struct {
	mcapi_node_t localNode;
	mcapi_port_t localPort;
	mcapi_node_t remoteNode;
	mcapi_port_t remotePort;
	char send_buf[BUF_SIZE];
	char cmp_buf[BUF_SIZE];
	SemaphoreHandle_t xSemaphore;
} mcapi_msg_info_t;

static volatile int mcapi_run_round = 0;
static volatile bool test_pass = pdFALSE;

static portTASK_FUNCTION( vMcapiDemoTask, pvParameters );
static portTASK_FUNCTION( vMcapiFinalizeTask, pvParameters );

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
		printf("MCAPI Error %s, status = %d [%s]\r\n",
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
	BaseType_t xTask;
	mcapi_msg_info_t *p;
	p = pvPortMalloc(2*sizeof(mcapi_msg_info_t));

	if( p == NULL ) {
		printf("Malloc Failed !\r\n");
		return;
	}

	/* Fill the Core0 - Core1 transport structure */
	p[0].localNode = NODE_0;
	p[0].localPort = CORE0_PORT_NUM1;
	p[0].remoteNode = NODE_1;
	p[0].remotePort = CORE1_PORT_NUM;
	strcpy(p[0].send_buf, CORE0_SEND_STRING1);
	strcpy(p[0].cmp_buf, CORE0_RECV_STRING1);
	p[0].xSemaphore =NULL;

	/* Fill the Core0 - Core2 transport structure */
	p[1].localNode = NODE_0;
	p[1].localPort = CORE0_PORT_NUM2;
	p[1].remoteNode = NODE_2;
	p[1].remotePort = CORE2_PORT_NUM;
	strncpy(p[1].send_buf, CORE0_SEND_STRING2, sizeof(CORE0_SEND_STRING2));
	strncpy(p[1].cmp_buf, CORE0_RECV_STRING2, sizeof(CORE0_RECV_STRING2));
	p[1].xSemaphore =NULL;


	for (xTask = 0; xTask < NODE_NUM; xTask ++ )
	{
		xTaskCreate( vMcapiDemoTask,
					 "McapiDemo",
					 mainMCAPI_TASK_STACK_SIZE,
					 (void *) &p[xTask],
					 uxPriority,
					 ( TaskHandle_t * ) NULL );
	}

	xTaskCreate( vMcapiFinalizeTask,
				 "McapiFinalize",
				 MCAPI_END_TASK_STACK_SIZE,
				 ( void * ) p,
				 tskIDLE_PRIORITY + 1,
				 ( TaskHandle_t * ) NULL );

}
/*-----------------------------------------------------------*/

static portTASK_FUNCTION( vMcapiDemoTask, pvParameters )
{
	mcapi_msg_info_t *p = (mcapi_msg_info_t *) pvParameters;
	int task_id = (int) p->remoteNode;
	int i;
	int pass_num = 0;
	size_t msg_size = 0;
	char recv_buf[BUF_SIZE];
	char cmp_buf[BUF_SIZE];
	char send_buf[BUF_SIZE] = "";
	mcapi_status_t mcapi_status;
	mcapi_endpoint_t local_ep;
	mcapi_endpoint_t remote_ep;

	p->xSemaphore = xSemaphoreCreateMutex();

	if( p->xSemaphore == NULL )
	{
		test_pass = pdFALSE;
		return;
	}

	if( xSemaphoreTake( p->xSemaphore, ( TickType_t ) 10 ) != pdTRUE )
	{
		test_pass = pdFALSE;
		return;
	}

	printf("#### Hello MCAPI testing Task%d ####\r\n", task_id);

	local_ep = mcapi_endpoint_create(p->localPort, &mcapi_status);
	mcapiErrorCheck(mcapi_status, "create endpoint", 2);

	remote_ep = mcapi_endpoint_get(DOMAIN,
								   p->remoteNode,
								   p->remotePort,
								   MCAPI_TIMEOUT_INFINITE,
								   &mcapi_status);
	mcapiErrorCheck(mcapi_status, "get endpoint", 2);

	memcpy(send_buf, p->send_buf, strlen(p->send_buf));
	memcpy(cmp_buf, p->cmp_buf, strlen(p->cmp_buf));

	for (i = 0; i < TEST_ROUNDS; i++) {
		/* Send a message to NodeX */
		snprintf(send_buf + strlen(p->send_buf), sizeof(send_buf), " %d", i);
		mcapi_msg_send(local_ep,
					   remote_ep,
					   send_buf,
					   strlen(send_buf),
					   0,
					   &mcapi_status);
		mcapiErrorCheck(mcapi_status, "msg_send", 2);

		printf("Task%d: message send. The %d time sending\r\n", task_id, i);
		printf("Task%d: end of send() - endpoint=%i has sent: [%s]\r\n", task_id, (int)local_ep, send_buf);

		/* waiting to the Node1 feedback */
		mcapi_msg_recv(local_ep,
					   recv_buf,
					   sizeof(recv_buf)-1,
					   &msg_size,
					   &mcapi_status);
		mcapiErrorCheck(mcapi_status, "msg_recv", 2);

		if (MCAPI_SUCCESS == mcapi_status) {
			printf("Task%d: message recv. The %d time receiving\r\n", task_id, i);
			recv_buf[msg_size] = '\0';
			printf("Task%d: end of recv() - endpoint=%i size 0x%x has received: [%s]\r\n", task_id, (int)remote_ep, msg_size, recv_buf);

			snprintf(cmp_buf + strlen(p->cmp_buf), sizeof(cmp_buf), " %d", i+1);
			cmp_buf[msg_size] = '\0';
			if (!strncmp(recv_buf, cmp_buf, strlen(cmp_buf))) {
				pass_num++;
				printf("Task%d: %d time message received successfully\r\n", task_id, i);
			} else
				printf("Task%d: %d time message received failed\r\n", task_id, i);
		}

		mcapi_run_round++;
	}

	mcapi_endpoint_delete(local_ep, &mcapi_status);
	mcapiErrorCheck(mcapi_status, "del_endpoint", 2);

	if (pass_num == TEST_ROUNDS)
	{
		test_pass = pdPASS;
		printf("Task%d %d rounds demo Test PASSED!!\r\n", task_id, TEST_ROUNDS);
	}
	else
	{
		test_pass = pdFALSE;
		printf("Task%d only %d rounds test passed, demo Test FAILED!!\r\n", task_id, pass_num);

	}

	xSemaphoreGive( p->xSemaphore );

	for( ;; )
	{
		test_pass = pdPASS;
	}
} /*lint !e715 !e818 pvParameters is required for a task function even if it is not referenced. */
/*-----------------------------------------------------------*/

static portTASK_FUNCTION( vMcapiFinalizeTask, pvParameters )
{
	mcapi_msg_info_t *p = (mcapi_msg_info_t *) pvParameters;
	mcapi_status_t mcapi_status;
	BaseType_t i = 0, pass_num = 0;

	for ( ;; ) {
		if( xSemaphoreTake(p[i].xSemaphore, ( TickType_t ) 10 ) == pdTRUE ) {
			i ++;
			pass_num ++;
		}

		if (pass_num == NODE_NUM)
			break;

	}

	mcapi_finalize(&mcapi_status);
	mcapiErrorCheck(mcapi_status, "finalize", 2);

	vPortFree(p);

	vTaskDelete(NULL);
}

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
