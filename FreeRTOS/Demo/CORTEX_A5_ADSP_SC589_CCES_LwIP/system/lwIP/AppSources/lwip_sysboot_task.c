/*****************************************************************************
    Copyright (C) 2012-2018 Analog Devices Inc. All Rights Reserved.
*****************************************************************************/


#include "lwip_sysboot_task.h"

/*! size of the memory block to allocate to the stack.  */
#define ETHER_STACK_SIZE          (1024*1024*2)

/* lwip linkup semaphore wait period in ticks */
#define LWIP_APP_LINKUP_SEM_PERIOD  (200) 

/* lwip task TCB */
static StaticTask_t   g_lwipTaskTCB;
static StackType_t  g_lwip_TaskSTK  [APP_OS_CFG_LWIP_TASK_STK_SIZE];

/* LWIP boot semaphore, gets signaled when lwip finishes its booting */
SemaphoreHandle_t          g_semLWIPBootComplete;

/* initializes the stack */
static int  lwip_system_init(void);
static void lwip_sysboot_task(void *);
static bool get_mac_address(const ADI_ETHER_HANDLE,char *ptr);
static void system_init(void);

/* configures the soft switches */
int ConfigSoftSwitches(void);

/****************************************************************************************
 *
 * @brief            Configures the lwip subsystem
 *
 * @details          adi_lwip_Init() function creates lwip subsystem boot task. This api 
 *                   is called from  system initialization code,i.e adi_init_Components()
 *
 * @return           returns 1 upon successful initialization of lwip subsystem. 
 *
 * @note             Successful return does not imply lwip subsystem is initialized.
 *                   Actual initialization is carried out by lwip system boot task.
 *                   Application network tasks must use semaphore g_semLWIPBootComplete  
 *                   which gets posted once lwip subsystem is successfully initialized.
 *                   
 *                   Before calling this API the following systems must be initialized
 *
 *                   # Board support initialization
 *                   # Operating System - OSInit() 
 *                   # Core timer
 *
 *                   After adi_lwip_Init() applications must start OS using OSStart() 
 *                   which starts the operating system.
 */
int adi_lwip_Init(void)
{
    /* This semaphore is posted when lwip finishes its booting process */
	g_semLWIPBootComplete = xSemaphoreCreateBinary();

    if (NULL == g_semLWIPBootComplete)
    {
        PRINTF("failed to create lwip boot semaphore \r\n");
        return (-1);
    }

    /* Create lwip boot task */
    TaskHandle_t hBootTask = xTaskCreateStatic (
        lwip_sysboot_task,                  /* Function Pointer   */
        "Lwip Boot Task",                   /* The Task Name      */
        APP_OS_CFG_LWIP_TASK_STK_SIZE,      /* Stack Size         */
        (void*) NULL,                       /* Function Argument  */
        APP_OS_CFG_LWIP_TASK_PRIO,          /* Boot Task Priority */
        g_lwip_TaskSTK,                     /* Stack base Address */
        &g_lwipTaskTCB
        );

    if (NULL == hBootTask)
    {
        PRINTF("Failed to create lwip system boot task \r\n");
        return (-1);
    }

    return(0);
}

/****************************************************************************************
 *
 * @brief            lwip system boot task
 *
 * @details          This task initializes the lwip sub-system and suspends itself upon 
 *                   successful initialization. 
 *
 *                   After successful initialization application tasks can use BSD 
 *                   socket APIs. This task will post g_semLWIPBootComplete semaphore
 *                   which indicates the successful initialization of lwip subsystem.
 *
 *                   If network cable is not present then this task will periodically 
 *                   check for the status of the connection. 
 *
 *                   By default lwIP uses ETHER_STACK_SIZE (default 2MB) of memory
 *                   for the stack. This memory gets allocated from the default heap. 
 *                   Applications must configure the heap accordingly.
 */
static void lwip_sysboot_task(void *arg)
{
    char    ipAddress[32];

    /* configures the switches */
    printf("Configuring switches for the ethernet operation \r\n");

    if (0 != ConfigSoftSwitches())
    {
        PRINTF ("lwip_sysboot_task: Failed to configure soft-switches\r\n");
        vTaskDelete(NULL);
    }

    /* Initialize the system */
    system_init();

    /* Initializes the TCP/IP Stack and returns */
    if(lwip_system_init() == -1)
    {
        PRINTF ("lwip_sysboot_task: Failed to initialize system\r\n");
        vTaskDelete(NULL);
    }
    
    /* start stack */
    if (!adi_lwip_Startstack())
    {
        PRINTF("lwip_sysboot_task: Failed to start stack\r\n");
        vTaskDelete(NULL);
    }

    /* Get and print the IP Address */
    memset(ipAddress,0,sizeof(ipAddress));

    if(gethostaddr(0,ipAddress))
    {
        printf("IP ADDRESS: %s\r\n",ipAddress);
    }

    /* 
     * LWIP subsystem is initialized posting the semaphore, all threads blocking on this
     * semaphore will be released.
     */
    if (pdFALSE == xSemaphoreGive(g_semLWIPBootComplete))
    {
        PRINTF("lwip_sysboot_task: Failed to post boot semaphore\r\n");
        vTaskDelete(NULL);
    }

    /* Suspend the lwip task */
    vTaskSuspend(NULL);

    return;
}

/****************************************************************************************
 *
 * @brief            Initializes lwip system
 *
 * @details          lwip_system_init function is called by the lwip system boot task. This
 *                   routine is responsible for initializing the lwip subsystem. lwip subsystem
 *                   initialization includes opening and setting up the device drivers, supplying
 *                   memory to the lwip subsystem, and starting it. 
 *
 *                   If network cable is not plugged in this routine will pend on a semaphore
 *                   and periodically checks for the link availability. The default period is
 *                   set to 200 ticks.This can be changed via LWIP_APP_LINKUP_SEM_PERIOD macro.
 *
 *                   Driver initialization parameters are supplied through EtherInitData variable.
 *                   By default data-cache is turned off. Applications enabling data cache should
 *                   set the first parameter to true to enable driver to work with cached 
 *                   buffers.
 *
 * @return           Returns 0 upon successful initialization of lwip subsystem  -1 upon failure
 */
static int lwip_system_init(void)
{
    ADI_ETHER_HANDLE   hEthernet;
    ADI_ETHER_RESULT   etherResult;
    ADI_ETHER_DEV_INIT EtherInitData= { true, &memtable, NULL }; /* data-cache,driver memory */
    SemaphoreHandle_t  semLinkUp;
    char               *ether_stack_block;
    char                hwaddr[6] = {0};

    
    /* open ethernet device */
    etherResult = adi_ether_Open( &(EMAC_DRIVER_ENTRY),       /* Driver Entry */
                                      &EtherInitData,         /* Driver Initialization data */
                                      adi_lwip_Stackcallback, /* Driver callback */
                                      &hEthernet              /* Pointer to get driver handle */
                                    );

    if (etherResult != ADI_ETHER_RESULT_SUCCESS)
    {
        PRINTF("lwip_system_init: failed to open ethernet driver\r\n");
        return (-1);
    }
 
    /* set the services with in stack */
    adi_lwip_Setdrvhandles(1,&hEthernet); 

    /* get the mac address */
    get_mac_address(hEthernet,(char*)&hwaddr[0]); 

    etherResult = adi_ether_SetMACAddress(hEthernet,(uint8_t*)&hwaddr);

    if (etherResult != ADI_ETHER_RESULT_SUCCESS)
    {
        PRINTF("lwip_system_init: failed to set MAC address\r\n");
        return (-1);
    }

    /* allocate memory to the stack */
    ether_stack_block = (char *) malloc ( ETHER_STACK_SIZE );

    if (ether_stack_block == NULL)
    {
        PRINTF("lwip_system_init: failed to allocate memory to the stack \r\n");
        return (-1);
    }

    if (adi_lwip_Initstack(ETHER_STACK_SIZE, ether_stack_block ) == -1)
    {
        PRINTF("lwip_system_init: failed to initalize lwip stack \r\n");
        return (-1);
    }

    /* Start the MAC */
    etherResult = adi_ether_EnableMAC(hEthernet);

    if (etherResult != ADI_ETHER_RESULT_SUCCESS)
    {
        PRINTF("lwip_system_init: failed to enable EMAC\r\n");
        return (-1);
    }
        
    /* wait for the link to be up */
    if (!adi_ether_GetLinkStatus (hEthernet))
    {
        printf("Waiting for the link to be established\r\n");

        /* This semaphore is posted when lwip finishes its booting process */
        semLinkUp = xSemaphoreCreateBinary();
        
        if (NULL == semLinkUp)
        {
            PRINTF("Failed to create semaphore,LWIP boot task may wait infinitely for linkup\r\n");
            return (-1);
        }

        /* check for link status */
        while (!adi_ether_GetLinkStatus (hEthernet))
        {
            /* pend on a semaphore if link is not up or network cable is not connected */
        	xSemaphoreTake(semLinkUp, LWIP_APP_LINKUP_SEM_PERIOD);
        }

        /* delete the semaphore */
        vSemaphoreDelete(semLinkUp);
    }
    printf("Link established\r\n");

    return (1);
}

/****************************************************************************************
 *
 * @brief            Returns the mac address as per the processor/ez-kit
 *
 * @param[in]        ptr points to the buffer location where the MAC address will be stored.
 *                   The supplied buffer size must be at least 6 bytes.
 *
 * @details          Copies MAC address from the flash or other non-volatile memory.
 *     
 * @return           Returns true if successful. Upon incorrect MAC the routine flags error 
 *                   message and copies a valid MAC address.
 *
 * @note             Change this function to return the MAC address for custom boards
 */

static bool get_mac_address(const ADI_ETHER_HANDLE hEthernet, char *ptr)
{
    bool ret = false;

    /* ADSP-BF537 EZ-KIT LITE */
#ifdef __ADSPBF537__
    const unsigned int mac_address_in_flash = 0x203F0000;
    *pEBIU_AMGCTL = 0xFF | 0x100;
    memcpy ( ptr, (unsigned char *) mac_address_in_flash,6 );
    ret = true;
#endif /* __ADSPBF537__ */

    /* ADSP-BF526 EZ-KIT */
#if defined(__ADSPBF526__) || defined(__ADSPBF518__)
    const unsigned int mac_address_in_flash = 0x203F0096;
    *pEBIU_AMGCTL = 0xFF | 0x100;
    memcpy ( ptr, (unsigned char *) mac_address_in_flash,6 );
    ret = true;
#endif /* __ADSPBF526__ */

    /* ADSP-BF609 EZ-KIT */
#ifdef __ADSPBF609__
    const unsigned int mac_address_in_flash = 0xB1060000;
    memcpy (ptr, (unsigned char *)mac_address_in_flash,6);
    reverse_mac(ptr);
    ret = true;
#endif /* __ADSPBF609__ */

    /* ADSP-BF533 EZ-KIT Lite with USB_LAN EZ-EXTENDER */
#if defined(__ADSPBF533__) 
    *pEBIU_AMGCTL = 0xFF | 0x100;
    adi_ether_GetMACAddress(hEthernet,(uint8_t*)ptr);
#endif /*__ADSPBF533__ */

#if defined(__ADSPBF561__)
    /* Bit:7 is set to zero for 32-bit access to async-bank3 */
    *pEBIU_AMGCTL = 0x7F | 0x100;
    adi_ether_GetMACAddress(hEthernet,(uint8_t*)ptr);
#endif /* __ADSPBF561__ */

    /* ADSP-BF548 EZ-KIT LITE */
#if defined(__ADSPBF548__)
    adi_ether_GetMACAddress(hEthernet,(uint8_t*)ptr);
#endif 

    /* ADSP-BF527 EZ-KIT LITE */
#if defined(__ADSPBF527__)
    *pEBIU_AMGCTL = 0xFF | 0x100;
    GetMacAddressFromOTP(ptr);
    reverse_mac(ptr);
#endif

#if defined(__ADSP215xx__)
    /* SC589 EZ-KIT does not have MAC in flash memory and hence MAC address need to be set
     * by the user in the system.svc. If it is not done, the application will use a temporary MAC.
     */

    /* IF (MAC address is not specified in the system.svc) */
    if (memcmp(user_net_config_info[0].mac_addr,"\x00\x00\x00\x00\x00\x00",6) == 0)
    {
    	char hwaddr[6] = {0x00, 0x12, 0x34, 0x56, 0x78, 0x9A };
    	printf("User need to set the MAC address in system.svc as MAC address is not stored on board \r\n");
    	printf("Incorrect MAC address in system.svc Using temporary MAC: 0x00123456789A \r\n");

    	memcpy (ptr, (unsigned char *)&hwaddr,sizeof(hwaddr));
    }
    else
    {
    	memcpy (ptr, user_net_config_info[0].mac_addr, 6);
    }

#endif
    /* check the mac obtained from flash */
    if (!(memcmp(ptr,"\x00\x00\x00\x00\x00\x00",6)))
    {
        char hwaddr[6] = {0x00, 0x12, 0x34, 0x56, 0x78, 0x9A };
        printf("Incorrect MAC address in FLASH Using temporary MAC: 0x00123456789A \r\n");
        memcpy (ptr, (unsigned char *)&hwaddr,sizeof(hwaddr));
    }
    return(ret);
}

#ifdef __ADSP215xx__
#define SLEEP_MSEC(NUM_MSEC)  {volatile int _x; int _y = (NUM_MSEC); while (_y-- > 0) {for (_x = 0; _x < 20000; _x++){}}}
static void system_init(void)
{
#ifdef 	__ADSPSC589_FAMILY__
	/* Bring the phy dp83865 out of reset */
	*pREG_PORTB_FER_CLR  = BITM_PORT_FER_CLR_PX14;
	*pREG_PORTB_DIR_SET  = BITM_PORT_DIR_SET_PX14; //choose output mode

	*pREG_PORTB_DATA_CLR = BITM_PORT_DATA_CLR_PX14; //drive 0
	SLEEP_MSEC(500)

	*pREG_PORTB_DATA_SET = BITM_PORT_DATA_SET_PX14; //drive 1
	SLEEP_MSEC(500);
#endif /* __ADSPSC589_FAMILY__ */

#ifdef 	__ADSPSC573_FAMILY__
	/* Bring the phy dp83865 out of reset */
	*pREG_PORTA_FER_CLR  = BITM_PORT_FER_CLR_PX5;
	*pREG_PORTA_DIR_SET  = BITM_PORT_DIR_SET_PX5; //choose output mode

	*pREG_PORTA_DATA_CLR = BITM_PORT_DATA_CLR_PX5; //drive 0
	SLEEP_MSEC(500)

	*pREG_PORTA_DATA_SET = BITM_PORT_DATA_SET_PX5; //drive 1
	SLEEP_MSEC(500);
#endif /* __ADSPSC573_FAMILY__ */

	/* Select RGMII for EMAC0 */
#ifdef __ADSPSC573_FAMILY__
	*pREG_PADS0_PCFG0 &= ~(BITM_PADS_PCFG0_EMACPHYISEL);
	*pREG_PADS0_PCFG0 |=  (1<<BITP_PADS_PCFG0_EMACPHYISEL);
#endif
#ifdef 	__ADSPSC589_FAMILY__
    *pREG_PADS0_PCFG0 |= BITM_PADS_PCFG0_EMACPHYISEL;
#endif

    /* Bring EMAC0 out of reset */
    *pREG_PADS0_PCFG0 |= BITM_PADS_PCFG0_EMACRESET;

    /* SPU Settings for EMAC0 and EMAC1 */
#ifdef __ADSPSC573_FAMILY__
    *pREG_SPU0_SECUREP40 = 0x03;
#endif /* __ADSPSC573_FAMILY__ */

#ifdef __ADSPSC589_FAMILY__
	*pREG_SPU0_SECUREP55 = 0x03;
	*pREG_SPU0_SECUREP56 = 0x03;
#endif	/* __ADSPSC589_FAMILY__ */

}
#else
static void system_init(void)
{

}

#endif


