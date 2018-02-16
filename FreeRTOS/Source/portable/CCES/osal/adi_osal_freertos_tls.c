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
Copyright (c), 2010-2017 - Analog Devices Inc. All Rights Reserved.
This software is PROPRIETARY & CONFIDENTIAL to Analog Devices, Inc.
and its licensors.
*****************************************************************************/

/*!
    @file adi_osal_freertos_tls.c

    Operating System Abstraction Layer - OSAL for FreeRTOS - TLS
    functions

*/
/** @addtogroup ADI_OSAL_TLS ADI OSAL TLS
 *  @{
 *
 *  This module contains the Thread Local Storage APIs for the FreeRTOS
 *  implementation of OSAL. Since FreeRTOS does not support TLS feature
 *  the OSAL TLS APIs in this module return ADI_OSAL_OS_ERROR
 */
/*=============  I N C L U D E S   =============*/

#include <stdlib.h>
#include "adi_osal.h"
#include "osal_common.h"
#include "osal_freertos.h"

/* disable misra diagnostics as necessary
 *
 * Error[Pm008]: sections of code should not be 'commented out'
 *               (MISRA C 2004 rule 2.4)
 *
 * Error[Pm128]: illegal implicit conversion from underlying MISRA type
 *                (MISRA C 2004 rule 10.1)
 */
/*! @cond */
#if defined ( __ICCARM__ )
_Pragma ("diag_suppress= Pm001,Pm002,Pm008,Pm128")
#endif
/*! @endcond */



/* Thread local storage numbers/keys consist of two values:
 *  - The signature, indicating that the key refers to a TLS slot 0xNNNNNNXX
 *  - The index of the slot, from 0 to Max Slots 0xXXXXXXNN
 */
#define TLS_SIGNATURE ((uint32_t)(0x544C5300u))

/* Masks used to extract either the signature component, or the slot index component
 * of a TLS Key.
 */
#define TLS_MASK_SIG ((uint32_t)(0xFFFFFF00u))
#define TLS_MASK_NUM ((uint32_t)(0x000000FFu))

#if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS != 0 )

/*=============  D A T A  =============*/

/*!
    @internal
    @var _adi_osal_gnNumSlots
         stores the number of TLS slots requested by the user during
         initialization.
    @endinternal
*/
uint32_t _adi_osal_gnNumSlots = ADI_OSAL_MAX_NUM_TLS_SLOTS;

/*!
    @internal
    @var _adi_osal_gTLSUsedSlots
         Word used in the management of allocated TLS slots.
         Bits are used to represent the status of the slot. Bit 0 corresponds
         to slot number 0 and slot number 30 corresponds bit number 30.
         A slot is free if the corresponding bit is clear and a
         slot is acquired if the corresponding bit is set. Initially all
         the slot bits are clear.
    @endinternal
*/
static uint32_t _adi_osal_gTLSUsedSlots = 0u;

/*!
    @internal
    @var  _adi_osal_gaTLSCallbacks
          Hold the callback structure for each Thread Local Storage Slot. The
          Callback are per slot, all threads that are using that slot are using
          the same callback.  Make the array the maximum supported size
          (ADI_OSAL_MAX_THREAD_SLOTS)
    @endinternal
*/
/*
static ADI_OSAL_TLS_CALLBACK_PTR _adi_osal_gaTLSCallbacks[ADI_OSAL_MAX_NUM_TLS_SLOTS];
*/

bool IsValidTLSKey(ADI_OSAL_TLS_SLOT_KEY key);

/*=============  C O D E  =============*/


#if defined (__ECC__)
#pragma always_inline
#elif defined (__ICCARM__)
#pragma inline=forced
#endif
inline bool IsValidTLSKey(ADI_OSAL_TLS_SLOT_KEY key)
{
    return (TLS_SIGNATURE == (key & TLS_MASK_SIG));
}

/*!
  ****************************************************************************
    @brief Allocates a thread slot and returns the slot number

    Dummy function retuns ADI_OSAL_OS_ERROR.

    @param[out] pnThreadSlotKey       - Pointer to return the slot number if a
                                        free slot is found.  Must be populated
                                        with ADI_OSAL_TLS_UNALLOCATED.  If a
                                        valid slot is already present in the
                                        supplied address this API returns
                                        success.

    @param[in] pTerminateCallbackFunc - Pointer to a function that gets called
                                        when the slot is freed.  Can be NULL
                                        if the callback function is not
                                        required.

    @return ADI_OSAL_OS_ERROR        - FreeRTOS does not support TLS feature.

*****************************************************************************/
ADI_OSAL_STATUS
adi_osal_ThreadSlotAcquire(ADI_OSAL_TLS_SLOT_KEY     *pnThreadSlotKey,
                           ADI_OSAL_TLS_CALLBACK_PTR pTerminateCallbackFunc)
{
	/* FreeRTOS can't currently support destroy callbacks */
    if (NULL != pTerminateCallbackFunc)
	{
		return ADI_OSAL_OS_ERROR;
	}

#ifdef OSAL_DEBUG
    if (CALLED_FROM_AN_ISR)
    {
        return ADI_OSAL_CALLER_ERROR;
    }
#endif /* OSAL_DEBUG */

    ADI_OSAL_STATUS  eRetStatus = ADI_OSAL_FAILED;

    /* Lock the scheduler - as a task may be deleted while a callback is being
     * installed.  */
    vTaskSuspendAll();

    /* If the passed-in slot number has already been allocated, then we return
     * successfully.  We check that -
     *  - It has the correct TLS signature
     *  - The slot has been (and is still) allocated.
     */
    if (IsValidTLSKey(*pnThreadSlotKey))
    {
        /* Extract the slot number from the TLS key, and convert to a bit
         * position */
        uint32_t nSlotBit = 1ul << (*pnThreadSlotKey & TLS_MASK_NUM);

        if (_adi_osal_gTLSUsedSlots & nSlotBit)
        {
			eRetStatus = ADI_OSAL_SUCCESS;
        }
        else
        {
        	eRetStatus = ADI_OSAL_SLOT_NOT_ALLOCATED;
        }
    }
    else
    {
        /* Before we allocate a slot, the address to be written to must have
         * an "unallocated" key.
         */
		if (*pnThreadSlotKey != ADI_OSAL_TLS_UNALLOCATED)
		{
			*pnThreadSlotKey = ADI_OSAL_INVALID_THREAD_SLOT;
		}
		else
		{
		    BaseType_t nSlotIndex;

		    for (nSlotIndex = 0; nSlotIndex < configNUM_THREAD_LOCAL_STORAGE_POINTERS; ++nSlotIndex)
		    {
		    	const uint32_t slotBit = (1u << nSlotIndex);

		    	if (0u == (_adi_osal_gTLSUsedSlots & slotBit))
		    	{
		    		/* We've found an unused slot */
					_adi_osal_gTLSUsedSlots |= slotBit; /* mark the slot as in use */
					*pnThreadSlotKey = (nSlotIndex | TLS_SIGNATURE);
					eRetStatus = ADI_OSAL_SUCCESS;
					break;
		    	}
		    }
		}
    }

    xTaskResumeAll();

    return eRetStatus;
}



/*!
  ****************************************************************************
    @brief Frees the specified slot in the local storage buffer.

    @param[in] nThreadSlotKey     - slot which needs to be freed.

    @return ADI_OSAL_OS_ERROR        - FreeRTOS does not support TLS feature.

*****************************************************************************/
ADI_OSAL_STATUS
adi_osal_ThreadSlotRelease(ADI_OSAL_TLS_SLOT_KEY nThreadSlotKey)
{
	const uint32_t slotIndex = nThreadSlotKey & TLS_MASK_NUM;

#ifdef OSAL_DEBUG
	/* Range-check the key */
	if (slotIndex >= configNUM_THREAD_LOCAL_STORAGE_POINTERS)
	{

	    return ADI_OSAL_BAD_SLOT_KEY;
	}
#endif /* OSAL_DEBUG */

	const uint32_t slotBit = (1u << slotIndex);

	/* Mark the slot as free */
	_adi_osal_gTLSUsedSlots &= ~slotBit;

    return ADI_OSAL_SUCCESS;
}


/*!
  ****************************************************************************
    @brief Stores the given value in the specified TLS slot.

    @param[out] nThreadSlotKey     - Slot key for the Thread Local Buffer in
                                     which "SlotValue" to be stored.
    @param[in] slotValue           - Value to be stored.

    @return ADI_OSAL_OS_ERROR      - FreeRTOS does not support TLS feature.
*****************************************************************************/
ADI_OSAL_STATUS
adi_osal_ThreadSlotSetValue(ADI_OSAL_TLS_SLOT_KEY nThreadSlotKey,
                            ADI_OSAL_SLOT_VALUE   slotValue)
{
	const uint32_t slotIndex = nThreadSlotKey & TLS_MASK_NUM;

#ifdef OSAL_DEBUG
	/* Range-check the key */
	if (slotIndex >= configNUM_THREAD_LOCAL_STORAGE_POINTERS)
	{

	    return ADI_OSAL_BAD_SLOT_KEY;
	}
#endif /* OSAL_DEBUG */

	vTaskSetThreadLocalStoragePointer(NULL, slotIndex, slotValue);

    return ADI_OSAL_SUCCESS;
}



/*!
  ****************************************************************************

    @brief Gets a value for the specified TLS slot from the current thread.

    @param[in] nThreadSlotKey     - Slot key, from which the data needs
                                    to be retrieved.
    @param[out] pSlotValue        - Pointer to store the retrieved value from
                                    TLS.

    @return ADI_OSAL_OS_ERROR        - FreeRTOS does not support TLS feature.
*****************************************************************************/

ADI_OSAL_STATUS
adi_osal_ThreadSlotGetValue(ADI_OSAL_TLS_SLOT_KEY nThreadSlotKey,
                            ADI_OSAL_SLOT_VALUE   *pSlotValue)
{
	const uint32_t slotIndex = nThreadSlotKey & TLS_MASK_NUM;

#ifdef OSAL_DEBUG
	/* Range-check the key */
	if (slotIndex >= configNUM_THREAD_LOCAL_STORAGE_POINTERS)
	{

	    return ADI_OSAL_BAD_SLOT_KEY;
	}
#endif /* OSAL_DEBUG */

	*pSlotValue = pvTaskGetThreadLocalStoragePointer(NULL, slotIndex);

    return ADI_OSAL_SUCCESS;
}

#endif /* configNUM_THREAD_LOCAL_STORAGE_POINTERS != 0 */


/* enable misra diagnostics as necessary */
/*! @cond */
#if defined ( __ICCARM__ )
_Pragma ("diag_default= Pm001,Pm002,Pm008,Pm128")
#endif
/*! @endcond */
/*
**
** EOF:
**
*/
/*@}*/
