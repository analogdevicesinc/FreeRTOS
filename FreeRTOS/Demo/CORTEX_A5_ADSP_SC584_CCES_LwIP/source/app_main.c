/*****************************************************************************
    Copyright (C) 2016-2018 Analog Devices Inc. All Rights Reserved.
*****************************************************************************/

#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <stddef.h>
#include "dns_client.h"
#include <adi_types.h>

#ifdef __ADSP215xx__
#include <services\pwr\adi_pwr.h>
/* Managed drivers and/or services include */
#include "adi_initialize.h"
#endif

#define APPLICATION_TASK_STACK_SIZE (2048)
#define APPLICATION_TASK_PRIO       (5)

static StaticTask_t  ApplicationTaskTCB;
static StackType_t ApplicationTaskStack[APPLICATION_TASK_STACK_SIZE];

int32_t adi_initComponents(void);
static int system_init(void);


#ifdef __ADSP215xx__
/* Heap for ADSP-SC584 required by LwIP */
uint8_t __adi_heap_object[4*1024*1024] __attribute__ ((section (".heap")));
#endif

/*! main
 *
 *    Initializes the operating system and lwIP subsystem.
 *    LWIP subsystem initialization includes creating lwip
 *    system tasks. After successful initialization the main
 *    will start the operating system and then executes application
 *    specific code.
 *
 *    The dns client application prints the ip address of the
 *    queried domain name.
 */
int main(void)
{
    int32_t result;
    /* Initialize any managed drivers and/or services.This routine
     * will call init routines for the add-in components.For lwip
     * adi_lwip_Init() gets called from adi_initComponents() which
     * initializes the lwip subsystem.
     *
     * Once RTOS is started with OSStart() lwipboot task starts the
     * lwip subsystem.
     */
    result = adi_initComponents();

    if (result != 0)
    {
        printf("Failed to initialize system components \r\n");
        while(1){ ; }
    }

    /* Create DNS client task. Event the application task is
     * created, it will not run until operating system is initialized.
     * All network application tasks wait on g_semLWIPBootComplete
     * semaphore which gets posted after successful completion of lwip.
     */
    TaskHandle_t hDnsClientTask = xTaskCreateStatic (
    	DnsClientTask,                      /* Function Pointer   */
		"Dns Client Task",                  /* The Task Name      */
		APPLICATION_TASK_STACK_SIZE,        /* Stack Size         */
        (void*) NULL,                       /* Function Argument  */
		APPLICATION_TASK_PRIO,              /* Boot Task Priority */
		ApplicationTaskStack,               /* Stack base Address */
        &ApplicationTaskTCB
        );

    if (NULL == hDnsClientTask)
    {
		printf("Error creating application task\r\n");
		while(1){ ; }
	}


    /* Start FreeRTOS. This allows the start of lwip system boot task and
     * other application tasks
     */
    vTaskStartScheduler();

    return 0;
}

#ifdef __ADSP215xx__
/* default power settings */
#define DF_DEFAULT      (0x0u)
#define MSEL_DEFAULT    (0x10u)
#define SYSSEL_DEFAULT  (0x8u)
#define S0SEL_DEFAULT   (0x2u)
#define S1SEL_DEFAULT   (0x2u)
#define CSEL_DEFAULT    (0x4u)
/* Reset target to default power */
static int setDefaultPower(uint32_t const nDeviceNum)
{
	ADI_PWR_RESULT ePwrResult;

	ePwrResult = adi_pwr_SetClkControlRegister(nDeviceNum, ADI_PWR_CLK_CTL_DF, DF_DEFAULT);
	if (ePwrResult != ADI_PWR_SUCCESS) return -1;

	ePwrResult = adi_pwr_SetClkControlRegister(nDeviceNum, ADI_PWR_CLK_CTL_MSEL, MSEL_DEFAULT);
	if (ePwrResult != ADI_PWR_SUCCESS) return -1;

	ePwrResult = adi_pwr_SetPllControlRegister(nDeviceNum, ADI_PWR_PLL_CTL_ENABLE, 1u);
	if (ePwrResult != ADI_PWR_SUCCESS) return -1;

	ePwrResult = adi_pwr_SetPllControlRegister(nDeviceNum, ADI_PWR_PLL_CTL_EXIT_ACTIVE, 1u);
	if (ePwrResult != ADI_PWR_SUCCESS) return -1;

	ePwrResult = adi_pwr_SetClkDivideRegister(nDeviceNum, ADI_PWR_CLK_DIV_SYSSEL, SYSSEL_DEFAULT);
	if (ePwrResult != ADI_PWR_SUCCESS) return -1;

	ePwrResult = adi_pwr_SetClkDivideRegister(nDeviceNum, ADI_PWR_CLK_DIV_CSEL, CSEL_DEFAULT);
	if (ePwrResult != ADI_PWR_SUCCESS) return -1;

	return ePwrResult;
}

static int system_init(void)
{
    ADI_PWR_RESULT ePwrResult;
    uint32_t cclk,sclk,sclk0,sclk1,dclk,oclk;

    /* ADSP-SC5xx EMAC0 require a clock of 125Mhz */

    ePwrResult = adi_pwr_Init (0, 25000000);
    if (ePwrResult != ADI_PWR_SUCCESS) return -1;

    /* Reset processor to default power settings */
    ePwrResult = setDefaultPower(0);
    if (ePwrResult != ADI_PWR_SUCCESS) return -1;
    ePwrResult = adi_pwr_SetFreq(0, 375000000, 187500000);
    if (ePwrResult != ADI_PWR_SUCCESS) return -1;

    ePwrResult = adi_pwr_SetClkDivideRegister(0,ADI_PWR_CLK_DIV_OSEL,3);
    if (ePwrResult != ADI_PWR_SUCCESS) return -1;

    ePwrResult = adi_pwr_GetCoreFreq(0, &cclk);
    if (ePwrResult != ADI_PWR_SUCCESS) return -1;
    ePwrResult = adi_pwr_GetSystemFreq(0, &sclk, &sclk0, &sclk1);
    if (ePwrResult != ADI_PWR_SUCCESS) return -1;
    ePwrResult = adi_pwr_GetDDRClkFreq(0, &dclk);
    if (ePwrResult != ADI_PWR_SUCCESS) return -1;
    ePwrResult = adi_pwr_GetOutClkFreq(0, &oclk);
    if (ePwrResult != ADI_PWR_SUCCESS) return -1;

    printf("Clock Configuration\r\n");
    printf("CCLK = %u, SCLK = %u, SCLK0 = %u, SCLK1 = %u, DCLK = %u, OCLK = %u, EMAC0_CLK = %u\r\n",
  			(unsigned int)cclk,
  			(unsigned int)sclk,
  			(unsigned int)sclk0,
  			(unsigned int)sclk1,
  			(unsigned int)dclk,
  			(unsigned int)oclk,
  			(unsigned int)oclk
  			);

    ePwrResult = adi_pwr_ConfigCduInputClock(ADI_PWR_CDU_CLKIN_0, ADI_PWR_CDU_CLKOUT_0);
    if (ePwrResult != ADI_PWR_SUCCESS) return -1;
    ePwrResult = adi_pwr_ConfigCduInputClock(ADI_PWR_CDU_CLKIN_0, ADI_PWR_CDU_CLKOUT_1);
    if (ePwrResult != ADI_PWR_SUCCESS) return -1;
    ePwrResult = adi_pwr_ConfigCduInputClock(ADI_PWR_CDU_CLKIN_0, ADI_PWR_CDU_CLKOUT_2);
    if (ePwrResult != ADI_PWR_SUCCESS) return -1;
    ePwrResult = adi_pwr_ConfigCduInputClock(ADI_PWR_CDU_CLKIN_0, ADI_PWR_CDU_CLKOUT_3);
    if (ePwrResult != ADI_PWR_SUCCESS) return -1;
    ePwrResult = adi_pwr_ConfigCduInputClock(ADI_PWR_CDU_CLKIN_3, ADI_PWR_CDU_CLKOUT_7);
    if (ePwrResult != ADI_PWR_SUCCESS) return -1;

    ePwrResult = adi_pwr_EnableCduClockOutput(ADI_PWR_CDU_CLKOUT_0, true);
    if (ePwrResult != ADI_PWR_SUCCESS) return -1;
    ePwrResult = adi_pwr_EnableCduClockOutput(ADI_PWR_CDU_CLKOUT_1, true);
    if (ePwrResult != ADI_PWR_SUCCESS) return -1;
    ePwrResult = adi_pwr_EnableCduClockOutput(ADI_PWR_CDU_CLKOUT_2, true);
    if (ePwrResult != ADI_PWR_SUCCESS) return -1;
    ePwrResult = adi_pwr_EnableCduClockOutput(ADI_PWR_CDU_CLKOUT_3, true);
    if (ePwrResult != ADI_PWR_SUCCESS) return -1;
    ePwrResult = adi_pwr_EnableCduClockOutput(ADI_PWR_CDU_CLKOUT_7, true);
    if (ePwrResult != ADI_PWR_SUCCESS) return -1;

    return 0;
}
#else
static int system_init(void)
{
	return 0;
}
#endif

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

#if ( configUSE_DAEMON_TASK_STARTUP_HOOK == 1 )
void vApplicationDaemonTaskStartupHook( void )
{
	/* This function will be called once only, when the daemon task(timer task) starts
	 * to execute. This is useful if the application includes initialization
	 * code that would benefit from executing after the scheduler has been started.
	 * The task priority of daemon task(timer task) is ( configMAX_PRIORITIES - 1 ),
	 * so it needs to pay attention to the execution sequence if using this function.
	 */
	/* Initialize the system */
	if (system_init() != 0)
	{
		printf("Failed to initialize the system\r\n");
	}
}
#endif

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
/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
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
