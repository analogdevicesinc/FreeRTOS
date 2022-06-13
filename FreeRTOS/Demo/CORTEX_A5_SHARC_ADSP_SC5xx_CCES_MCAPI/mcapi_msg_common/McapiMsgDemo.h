/*****************************************************************************
 * Example: McapiMsgDemo
 * Project: RTOSDemo_SC589_MCAPI_Msg_Core0/2
 * File:    McapiMsgDemo.h
 *
 * Copyright(c) 2013-2020 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary and confidential.  By using this software you
 * agree to the terms of the associated Analog Devices License Agreement.
 *****************************************************************************/

#ifndef MCAPI_MSG_DEMO_H
#define MCAPI_MSG_DEMO_H
#include <stdio.h>
#include <string.h>
#define BUF_SIZE		64
#define TEST_ROUNDS		100

#define DOMAIN			0
#define NODE_NUM 		2
#define NODE_0			0
#define NODE_1			1
#define NODE_2			2
#define CORE0_PORT_NUM1	10
#define CORE0_PORT_NUM2 11
#define CORE1_PORT_NUM	5
#define CORE2_PORT_NUM	6

//#if defined(CORE0)
#define CORE0_SEND_STRING1	"core0 send to core1"
#define CORE0_RECV_STRING1	"core1 feedbcak to core0"
#define CORE0_SEND_STRING2	"core0 send to core2"
#define CORE0_RECV_STRING2	"core2 feedbcak to core0"
//#elif defined(CORE1)
#define CORE1_SEND_STRING	"core1 feedbcak to core0"
#define CORE1_RECV_STRING	"core0 send to core1"
//#elif defined(CORE2)
#define CORE2_SEND_STRING	"core2 feedbcak to core0"
#define CORE2_RECV_STRING	"core0 send to core2"
//#endif

#define mainMCAPI_TASK_STACK_SIZE	( configMINIMAL_STACK_SIZE + 300 )
#define MCAPI_END_TASK_STACK_SIZE	( configMINIMAL_STACK_SIZE )

void vStartMcapiDemoTask( UBaseType_t uxPriority );
BaseType_t xAreMcapiDemoTaskStillRunning( void );

#endif /* MCAPI_MSG_DEMO_H_ */

