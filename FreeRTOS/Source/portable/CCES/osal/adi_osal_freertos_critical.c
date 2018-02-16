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
    @file adi_osal_freertos_critical.c

    Operating System Abstraction Layer - OSAL for FreeRTOS - Critical section
    functions

*/
/** @addtogroup ADI_OSAL_Critical ADI OSAL Critical
 *  @{
 *
 * This module contains the critical section & scheduler locking APIs for the
 * FreeRTOS implementation of OSAL
 */

/*=============  I N C L U D E S   =============*/


#include "osal_misra.h"
#include "adi_osal.h"
#include "osal_common.h"
#include "osal_freertos.h"
#include "task.h"

/*  disable misra diagnostics as necessary
 * Error[Pm073]:  a function should have a single point of exit
 *               (MISRA C 2004 rule 14.7)
 *
 * Error[Pm128]: illegal implicit conversion from underlying MISRA type
 *                (MISRA C 2004 rule 10.1)
 *
 * Error[Pm143]: a function should have a single point of exit at the end of
 *                the function (MISRA C 2004 rule 14.7)
 */
/*! @cond */
#if defined ( __ICCARM__ )
_Pragma ("diag_suppress= Pm001,Pm002,Pm073,Pm128,Pm143")
#endif
/*! @endcond */

/*=============  D A T A  =============*/

#ifdef OSAL_DEBUG
/*!
    @internal
    @var _adi_osal_gnSchedulerLockCnt
         Indicates if the code is within a Scheduler lock section. It is only
         used in debug mode to check if Unlock is called with Lock being called
         first. This needs to be a counter to allow for nesting
         Initially the scheduler is not locked
    @endinternal
*/
uint32_t _adi_osal_gnSchedulerLockCnt = 0u ;

#endif /* OSAL_DEBUG */

/*!
    @internal
    @var _adi_osal_gnCriticalRegionNestingCnt
         This variable is a counter which is incremented when
         adi_osal_EnterCriticalRegion() is called and decremented when
         adi_osal_ExitCriticalRegion is called.
         Initially we are not in a critical region.
    @endinternal
*/
int32_t _adi_osal_gnCriticalRegionNestingCnt = 0;


/*!
    @internal
    @var snCriticalRegionState
         Holds the state of the interrupt mask as of the first call to
         adi_osal_EnterCriticalRegion
    @endinternal
*/
static UBaseType_t snCriticalRegionState = 0u;


/*=============  C O D E  =============*/




/*!
  ****************************************************************************
    @brief Determines whether the scheduler is running.

    @return true  - If the scheduler is running,
    @return false - If the scheduler is not running
*****************************************************************************/

bool adi_osal_IsSchedulerActive(void)
{
    return (taskSCHEDULER_NOT_STARTED != xTaskGetSchedulerState());
}


/*!
  ****************************************************************************
    @brief Prevents rescheduling until adi_osal_SchedulerUnlock is called.

    After this function is called, the current thread does not become
    de-scheduled , even if a high-priority thread becomes ready to run.

    Note that calls to adi_osal_SchedulerLock may be nested. A count is
    maintained to ensure that a matching number of calls to
    adi_osal_SchedulerUnlock are made before scheduling is re-enabled.

    @see adi_osal_SchedulerUnlock
*****************************************************************************/

void adi_osal_SchedulerLock( void )
{
    vTaskSuspendAll();

#ifdef OSAL_DEBUG
    /* FreeRTOS xTaskResumeAll function takes care of nesting itself, so _adi_osal_gnSchedulerLockCnt
     * is only useful for diagnostics or debugging.
     */
     _adi_osal_gnSchedulerLockCnt++;
#endif /* OSAL_DEBUG */
}


/*!
  ****************************************************************************
    @brief Re-enables thread scheduling.

    This function decrements the internal count which tracks how many times
    adi_osal_SchedulerLock was called. The API relies on the RTOS to
    enable scheduling when appropriate

    @return ADI_OSAL_SUCCESS - If thread scheduling was enabled successfully
    @return ADI_OSAL_FAILED  - If the function call does not match a call to
                               adi_osal_SchedulerLock

    @see adi_osal_SchedulerLock
*****************************************************************************/

ADI_OSAL_STATUS adi_osal_SchedulerUnlock( void )
{
#ifdef OSAL_DEBUG
	/* FreeRTOS xTaskResumeAll function takes care of nesting itself, so _adi_osal_gnSchedulerLockCnt
	 * is only useful for diagnostics or debugging.
	 */
	if (0u == _adi_osal_gnSchedulerLockCnt)
	{
		return ADI_OSAL_FAILED;       /* if the Unlock function is called before the lock, return an error */
	}
	_adi_osal_gnSchedulerLockCnt--;             /* it must be done before unlocking */
#endif /* OSAL_DEBUG */

	xTaskResumeAll();

	return ADI_OSAL_SUCCESS;
}

/*!
  ****************************************************************************
    @brief Disables interrupts to enable atomic execution of a critical region
    of code.

    Note that critical regions may be nested. A count is maintained to ensure a
    matching number of calls to adi_ExitCriticalRegion are made before
    restoring interrupts. Each critical region is also (implicitly) a scheduler
    lock.

    @see adi_osal_ExitCriticalRegion
*****************************************************************************/
void adi_osal_EnterCriticalRegion( void )
{
	if (CALLED_FROM_AN_ISR)
	{
		/* Accessing to the global count variable needs to be protected from thread
		 * switches and nested interrupts so interrupt disable is called at the very
		 * beginning. The value of the interrupts state is only saved the first time.
		 */
		UBaseType_t istate = taskENTER_CRITICAL_FROM_ISR();

		if (0 == _adi_osal_gnCriticalRegionNestingCnt)
		{
			/* Only save the state for the outermost call */
			snCriticalRegionState = istate;
		}
	}
	else
	{
		taskENTER_CRITICAL();
	}

	_adi_osal_gnCriticalRegionNestingCnt++;
}

/*!
  ****************************************************************************
    @brief Re-enables interrupts and restores the interrupt status.

    This function decrements the count of nested critical regions. Use it as a
    closing bracket to adi_osal_EnterCriticalRegion. OSAL ignores additional
    calls to adi_osal_ExitCriticalRegion while interrupts are enabled.

    @see adi_osal_EnterCriticalRegion
*****************************************************************************/

void adi_osal_ExitCriticalRegion( void )
{
	_adi_osal_gnCriticalRegionNestingCnt--;

	if (CALLED_FROM_AN_ISR)
	{
		/* When the last nesting level is reached, reenable the interrupts */
		if (_adi_osal_gnCriticalRegionNestingCnt <= 0)
		{
			_adi_osal_gnCriticalRegionNestingCnt = 0;
			taskEXIT_CRITICAL_FROM_ISR(snCriticalRegionState);
		}
	}
	else
	{
		taskEXIT_CRITICAL();
	}
}

/* enable misra diagnostics as necessary */
/*! @cond */
#if defined ( __ICCARM__ )
_Pragma ("diag_default= Pm001,Pm002,Pm073,Pm128,Pm143")
#endif
/*! @endcond */
/*
**
** EOF:
**
*/
/*@}*/
