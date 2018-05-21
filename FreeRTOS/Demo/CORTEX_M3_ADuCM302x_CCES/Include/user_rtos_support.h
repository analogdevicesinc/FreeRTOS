/*****************************************************************************
    Copyright (C) 2016-2018 Analog Devices Inc. All Rights Reserved.
*****************************************************************************/


#ifndef USER_RTOS_SUPPORT_H__
#define USER_RTOS_SUPPORT_H__

#ifdef __STDC__
#include <FreeRTOS.h>

#include "task.h"
#include "semphr.h"

/*! Macro that declares the semaphore type that the services/drivers use */
#define PEND_VAR_DECLR \
                     SemaphoreHandle_t Semaphore;

/*! Macro that creates a semaphore and returns the error specified in case of failure. DEV is the handle to the device driver structure that contains the semaphore/semaphore handle */
#define CREATE_SEM(DEV, error) \
                   do { \
                     DEV->Semaphore = xSemaphoreCreateMutex(); \
                   } while (0)

/*! Macro that deletes a semaphore and returns the error specified in case of failure. DEV is the handle to the device driver structure that contains the semaphore/semaphore handle */
#define DELETE_SEM(DEV, error) \
                   do { \
                     vSemaphoreDelete(DEV->Semaphore); \
                   } while (0)


/*! Macro that blocks permanently on a semaphore. It does not return a failure. This macro changed
 * in the BSP version 1.0.1 the number of arguments for this macro changed */
#define PEND_EVENT(DEV, error) \
                    do { \
                      xSemaphoreTake(DEV->Semaphore, portMAX_DELAY); \
                    } while (0)



/*! Macro that posts a semaphore. It does not return a failure. */
#define POST_EVENT(DEV) \
                    do { \
                      xSemaphoreGive(DEV->Semaphore); \
                    } while (0)


/* Enter critical section */
#define ADI_ENTER_CRITICAL_REGION() portENTER_CRITICAL()

/* Exit critical section */
#define ADI_EXIT_CRITICAL_REGION() portEXIT_CRITICAL()

/*! Macro to install the handler */
#define ADI_INSTALL_HANDLER(IRQn, Handler)

/*! Macro to uninstall the handler */
#define ADI_UNINSTALL_HANDLER(IRQn)


#define portSAVE_CONTEXT() \
                    do { \
                    } while (0)

#define portRESTORE_CONTEXT() \
                    do { \
                    } while (0)

/*! Code to run at the beginning of interrupt handler */
#define ISR_PROLOG() portSAVE_CONTEXT()


/*! Code to run at the end of interrupt handler */
#define ISR_EPILOG() portRESTORE_CONTEXT()

/* This type is used in startup.c */
typedef void  ( *ADIIntFunc )( void );

/* This type is used in the services/drivers */
typedef void (*ADI_INT_HANDLER_PTR)(void);

/* Used to define the interrupt handler */
#define ADI_INT_HANDLER(Handler) \
        void (Handler) (void)

/* Enable a specific interrupt */
#define ADI_ENABLE_INT(IRQn) \
        NVIC_EnableIRQ(IRQn)

/* Disable a specific interrupt */
#define ADI_DISABLE_INT(IRQn) \
        NVIC_DisableIRQ(IRQn)


#define ADI_WEAK_PROTOTYPE(x)     WEAK_PROTO ( void (x) (void)) ATTRIBUTE_INTERRUPT ;
#define ADI_WEAK_FUNCTION(x)      WEAK_FUNC  ( void (x) (void)) { while(1) {} }

#define ADI_DEVICE_DRIVER_RTOS_MEMORY  68u

#define ADI_CRITICAL_REGION_VAR_DEFINE
#define ADI_SET_CRITICAL_REGION_VAR

#endif /* __STDC__ */

#ifdef __GNUC__
#define PENDSV_HANDLER xPortPendSVHandler
#define SYSTICK_HANDLER  xPortSysTickHandler
#define SVC_HANDLER      vPortSVCHandler
#elif __CC_ARM
#define PendSV_Handler xPortPendSVHandler
#define SysTick_Handler  xPortSysTickHandler
#define SVC_Handler      vPortSVCHandler
#elif __ICCARM__
#define PENDSV_HANDLER   xPortPendSVHandler
#define SYSTICK_HANDLER  xPortSysTickHandler
#define SVC_Handler      vPortSVCHandler
#endif

#endif /* USER_RTOS_SUPPORT_H__ */
