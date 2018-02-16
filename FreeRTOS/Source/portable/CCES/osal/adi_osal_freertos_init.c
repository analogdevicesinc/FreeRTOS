/*
Copyright (c) 2016, Analog Devices, Inc.  All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted (subject to the limitations in the
 disclaimer below) provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the
   distribution.

 * Neither the name of Analog Devices, Inc.  nor the names of its
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

 NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
 GRANTED BY THIS LICENSE.  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
 HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

******************************************************************************
*/

/*!
    @file adi_osal_freertos_init.c

    Operating System Abstraction Layer - OSAL for FreeRTOS

*/
/** @addtogroup ADI_OSAL_Init ADI OSAL Initialization
 *  @{
 *
 *   This module contains the APIs designed to initialize the OSAL and
 *   abstract the FreeRTOS operating system from the user.
 */
/*=============  I N C L U D E S   =============*/

#include <limits.h>
#include <stdlib.h>

#include "osal_misra.h"
#include "adi_osal.h"
#include "osal_freertos.h"
#include "osal_common.h"

/* disable misra diagnostics as necessary
 * Error[Pm073]:  a function should have a single point of exit
 *               (MISRA C 2004 rule 14.7)
 *
 * Error[Pm143]: a function should have a single point of exit at the end of
 *                the function (MISRA C 2004 rule 14.7)
 */
/*! @cond */
#if defined ( __ICCARM__ )
_Pragma ("diag_suppress= Pm001,Pm002,Pm073,Pm143")
#endif
/*! @endcond */


/* "version.h" contain macro "ADI_BUILD_VER"  which gives the
    build version for OSAL for FreeRTOS. This is a generated file.
*/
#include "version.h"

/*=============  D E F I N E S  =============*/


/*! @details priority of the virtual startup "thread" */
#define STARTUP_PRIORITY    0u
#define ADI_OSAL_INVALID_HEAP_INDEX (-1)



/*=============  D A T A  =============*/
/*!
    @internal
    @var _adi_osal_gnTickPeriod
         defines the length of system ticks in microseconds
    @endinternal
 */
uint32_t _adi_osal_gnTickPeriod = UINT_MAX;


/*!
    @internal
    @var _adi_osal_oStartupVirtualThread
         This thread is not a real thread, but is active until the OS starts.
         It will allow the TLS functions to operate until the OS takes over.
    @endinternal
*/

ADI_OSAL_THREAD_INFO _adi_osal_oStartupVirtualThread;

/*=============  C O D E  =============*/

/*!
  ****************************************************************************
    @brief Initializes OSAL.

    This function initializes the internal OSAL data structure. It should be
    called during the system startup.

    @return ADI_OSAL_SUCCESS          - Initialization is done successfully.
    @return ADI_OSAL_OS_ERROR         - The version of OSAL is not compatible
                                        with the FreeRTOS version
    @return ADI_OSAL_MEM_ALLOC_FAILED - Error initializing dynamic memory heap

*****************************************************************************/
ADI_OSAL_STATUS adi_osal_Init(void)
{
    uint32_t* pHeapMemory = NULL; /*ptr to the memory to use for the heap */
    uint32_t nHeapMemorySize = 0u;/*size of memory pointed by pHeapMemory */
    /*!
        @internal
        @var snOsalInitializationState
             Static variable to record if OSAL has already been initialized
        @endinternal
    */
#ifdef OSAL_DEBUG
    static uint32_t snOsalInitializationState = 0u ;
#endif

    /* Checks that the version of FreeRTOS is compatible with this version of OSAL */
    if (GetOSVersion() < COMPATIBLE_OS_VERSION)
    {
        return (ADI_OSAL_OS_ERROR);
    }

#ifdef OSAL_DEBUG
    /* Check if already initialized. If the parameters are the same then the
     * call succeeds. Otherwise the call fails. The priority inheritance
     * setting is not recorded so we cannot check it
     */
    if (OSAL_INITIALIZED == snOsalInitializationState)
    {
        return (ADI_OSAL_SUCCESS);
    }
#endif

    /* automatically sets the tick period based on the FreeRTOS settings */
    _adi_osal_gnTickPeriod = USEC_PER_SEC / configTICK_RATE_HZ;

    if (ADI_OSAL_SUCCESS != _adi_osal_HeapInstall(pHeapMemory,nHeapMemorySize))
    {
        return (ADI_OSAL_MEM_ALLOC_FAILED);
    }


    /* Create the thread that represents the current execution as a thread until
     * the OS actually starts
     */
//    _adi_osal_oStartupVirtualThread.nThreadSignature = ADI_OSAL_THREAD_SIGNATURE;

#ifdef OSAL_DEBUG
    snOsalInitializationState = OSAL_INITIALIZED;
#endif


    return( ADI_OSAL_SUCCESS);
}

/* enable misra diagnostics as necessary */
/*! @cond */
#if defined ( __ICCARM__ )
_Pragma ("diag_default= Pm001,Pm002,Pm073,Pm143")
#endif
/*! @endcond */

/*
**
** EOF:
**
*/
/*@}*/
