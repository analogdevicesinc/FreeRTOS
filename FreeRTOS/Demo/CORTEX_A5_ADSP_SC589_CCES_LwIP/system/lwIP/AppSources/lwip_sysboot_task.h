/*****************************************************************************
    Copyright (C) 2012-2018 Analog Devices Inc. All Rights Reserved.
*****************************************************************************/


#ifndef _LWIP_SYSBOOT_TASK_H_
#define _LWIP_SYSBOOT_TASK_H_

#include <lwip/lwipifce.h>
#include <lwip/sockets.h>
#include <lwip/inet.h>
#ifdef __ADSPBLACKFIN__
#include <sys/exception.h>
#include <cplb.h>
#endif
#include <drivers/ethernet/adi_ether.h>

#include <stdio.h>
#include <string.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <stddef.h>

/* enables debug prints for lwip boot thread */
#ifndef ADI_DEBUG
#define ADI_DEBUG
#endif

#ifdef __cplusplus
extern "C"  {
#endif

/*
 * processor independent macros
 */
#define APP_OS_CFG_LWIP_TASK_STK_SIZE  (2048) /*! LWIP task stack size */ 
#define APP_OS_CFG_LWIP_TASK_PRIO      (5)    /*! LWIP task priority   */

/* Enable debug outputs */
#ifdef ADI_DEBUG
    #define PRINTF printf
#else
    #define PRINTF
#endif /* ADI_DEBUG */

int adi_lwip_Init(void);
extern SemaphoreHandle_t g_semLWIPBootComplete; /*! Gets posted when lwip boot process is complete */

/***************************************************************
 *
 * ADSP-BF60x or ADSP-SC5xx family configuration
 */
#if defined( __ADSPBF60x__) || defined(__ADSP215xx__)
    #include <drivers/ethernet/adi_ether_gemac.h>
    #include <services/int/adi_int.h>
#ifdef __ADSPBF60x__
    #include <services/int/adi_sec.h>
#endif

    #if defined(__ADSPBF609__)
    #include <cdefbf609.h>
    #include <defbf609.h>
    #elif defined(__ADSPBF608__)
    #include <cdefbf608.h>
    #include <defbf608.h>
    #elif defined(__ADSPBF607__)
    #include <cdefbf607.h>
    #include <defbf607.h>
    #elif defined(__ADSPBF606__)
    #include <cdefbf606.h>
    #include <defbf606.h>
    #endif

    #define EMAC_DRIVER_ENTRY (GEMAC0DriverEntry) /* EMAC0 Driver Entry point */
    #define EMAC_NUM_RECV_DESC    (10)  /*! Number of receive DMA descriptors  */
    #define EMAC_NUM_XMIT_DESC    (10)  /*! Number of transmit DMA descriptors */

#ifdef __ADSP215xx__
	uint8_t BaseMemSize[32]  __attribute__((aligned(32)));
	uint8_t MemRcve[EMAC_NUM_RECV_DESC*32]  __attribute__((aligned(32))); /*! Receive DMA descriptor memory  */
	uint8_t MemXmit[EMAC_NUM_XMIT_DESC*32]  __attribute__((aligned(32))); /*! Transmit DMA descriptor memory */
#else
    #pragma alignment_region (32)
    uint8_t BaseMemSize[32];
    uint8_t MemRcve[EMAC_NUM_RECV_DESC*32]; /*! Receive DMA descriptor memory  */
    uint8_t MemXmit[EMAC_NUM_XMIT_DESC*32]; /*! Transmit DMA descriptor memory */
    #pragma alignment_region_end
#endif
#endif /* __ADSPBF60x__ or __ADSPSC589_FAMILY__*/

/***************************************************************
 *
 * ADSP-BF537 family configuration
 */
#if defined(__ADSPBF537__)  || defined(__ADSPBF526__) || defined(__ADSPBF527__) || defined(__ADSPBF518__)
   #include <drivers/ethernet/adi_ether_emac.h>

   #define EMAC_DRIVER_ENTRY (EmacDriverEntry) /* EMAC Driver Entry point */
   #define EMAC_NUM_RECV_DESC  (10)
   #define EMAC_NUM_XMIT_DESC  (10)
   #pragma alignment_region (32)
   uint8_t BaseMemSize[ADI_ETHER_MEM_EMAC_BASE_SIZE];
   uint8_t MemRcve[EMAC_NUM_RECV_DESC*ADI_ETHER_MEM_EMAC_PER_RECV];
   uint8_t MemXmit[EMAC_NUM_XMIT_DESC*ADI_ETHER_MEM_EMAC_PER_XMIT];
#pragma alignment_region_end
#endif /* __ADSPBF537__ __ADSPBF527__ __ADSPBF526__ __ADSPBF518__*/

#ifdef __ADSPBF527__
uint32_t GetMacAddressFromOTP(char *p);
#endif

/****************************************************************
 *  ADSP-BF533 EZ-KIT Lite w/ USBLAN specific settings
 */
#if defined(__ADSPBF533__) || defined(__ADSPBF561__)
   #include <drivers/ethernet/adi_ether_lan91c111.h>
   #pragma alignment_region (4)
   uint8_t BaseMemSize[ADI_ETHER_MEM_USBLAN_BASE_SIZE];
   #define EMAC_DRIVER_ENTRY (hLAN91C111DriverEntry) /* LAN91C111 Driver Entry point */
#pragma alignment_region_end
#endif /* __ADSPBF533__ */

/****************************************************************
 *  ADSP-BF548 EZ-KIT Lite w/ USBLAN specific settings
 */
#if defined(__ADSPBF548__) 
   #include <drivers/ethernet/adi_ether_lan9218.h>
   #define EMAC_NUM_RECV_DESC  (1)
   #define EMAC_NUM_XMIT_DESC  (1)
   #pragma alignment_region (4)
   uint8_t BaseMemSize[ADI_ETHER_MEM_LAN9218_BASE_SIZE];
   uint8_t MemRcve[EMAC_NUM_RECV_DESC*ADI_ETHER_MEM_LAN9218_PER_RECV];
   uint8_t MemXmit[EMAC_NUM_XMIT_DESC*ADI_ETHER_MEM_LAN9218_PER_XMIT];
   #define EMAC_DRIVER_ENTRY (hLAN9218DriverEntry) /* LAN9218 Driver Entry point */
#pragma alignment_region_end
#endif /* __ADSPBF548__ */


/* Initialize the Memory Table */
ADI_ETHER_MEM memtable = 
{
    #if defined(__ADSPBF60x__) || defined(__ADSPBF537__) || \
	defined(__ADSPBF526__) || defined(__ADSPBF527__) || \
	defined(__ADSPBF518__) || defined(__ADSPBF548__) || \
	defined(__ADSP215xx__)
        MemRcve,sizeof(MemRcve),
        MemXmit,sizeof(MemXmit),
        BaseMemSize,sizeof(BaseMemSize)
    #endif /* __ADSPBF609__ */

    #if defined(__ADSPBF533__) || defined(__ADSPBF561__)	
      0,0,
      0,0,
      BaseMemSize,sizeof(BaseMemSize)
    #endif 
};

/* reverses the given mac_address */
inline void reverse_mac(char *ptr)
{
int i,j;
char c;

    for(i=0,j=5; i < 3; i++,j--){
        c = *(ptr+i);
       *(ptr+i) = *(ptr+j);
       *(ptr+j) = c;
    }
}
#ifdef __cplusplus
}
#endif

#endif /* _LWIP_SYSBOOT_TASK_H_ */
