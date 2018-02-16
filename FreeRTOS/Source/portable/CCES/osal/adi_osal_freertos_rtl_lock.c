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

*****************************************************************************/

/*!
    @file adi_osal_freertos_rtl_lock.c

    Operating System Abstraction Layer - OSAL for FreeRTOS


*/
/** @addtogroup ADI_OSAL_RTL_LOCK ADI OSAL RTL_LOCK
 *  @{
 *
 *  This module contains the APIs designed to abstract the runtime library
 *  locking mechanism so that each RTOS can adapt to them according to its needs.
 */

/*=============  I N C L U D E S   =============*/


#include "osal_misra.h"
#include "adi_osal.h"
#include "osal_freertos.h"
#include "osal_common.h"


/*!
  ****************************************************************************
    @brief Acquires the RTL global lock

    @details
    API used by the runtime library for its internal locking mechanism. In the
    case of FreeRTOS this is implemented as a scheduler region lock because
    functions like heap_free must be allowed in the task destruction hook which
    is called with the scheduler lock held.

    Other RTOS might implement this API as a mutex pend.

    @return ADI_OSAL_SUCCESS - In all cases

    @see adi_osal_SchedulerLock
    @see adi_osal_RTLGlobalsUnlock

*****************************************************************************/

ADI_OSAL_STATUS adi_osal_RTLGlobalsLock( void)
{
    adi_osal_SchedulerLock();

    return( ADI_OSAL_SUCCESS);
}


/*!
  ****************************************************************************
    @brief Releases the RTL global lock

    @details
    API used by the runtime library for its internal locking mechanism. In the
    case of FreeRTOS this is implemented as a scheduler region unlock because
    functions like heap_free must be allowed in the task destruction hook which
    is called with the scheduler lock held.

    @return ADI_OSAL_SUCCESS - If the lock was released successfully
    @return ADI_OSAL_FAILED  - If the function call does not match a call to
                               adi_osal_RTLGlobalsLock/adi_osal_SchedulerLock

    @see adi_osal_SchedulerUnlock
    @see adi_osal_RTLGlobalsLock
*****************************************************************************/


ADI_OSAL_STATUS adi_osal_RTLGlobalsUnlock( void)
{
    return (adi_osal_SchedulerUnlock());

}

/*
**
** EOF:
**
*/
/*@}*/
