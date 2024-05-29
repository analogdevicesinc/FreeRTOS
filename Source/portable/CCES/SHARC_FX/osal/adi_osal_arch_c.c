/*
 * Copyright (C) 2018-2023 Analog Devices Inc. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE
 */


/*=============  I N C L U D E S   =============*/


#include <stdlib.h>           /* for NULL definition */
#include <limits.h>           /* for INT_MAX */

#include "adi_osal.h"
#include "adi_osal_arch.h"
#include "adi_osal_arch_internal.h"
#include "osal_common.h"

/*=============  D E F I N E S  =============*/


#pragma file_attr(  "libGroup=adi_osal.h")
#pragma file_attr(  "libName=libosal")

#define ADI_OSAL_INVALID_HEAP_INDEX (-1)

/*=============  C O D E  =============*/



/*!
  ****************************************************************************
   @internal

   @brief This function abstracts the creation of a heap with the information
    provided.

   @details No available for SHARC-FX

   Parameters:
   @param[in] pHeapMemory - Pointer to the allocated memory
   @param[in] nHeapMemorySize  - Size of memory to be allocated

   @return ADI_OSAL_MEM_ALLOC_FAILED  - if the heap could not be created

   @endinternal

*****************************************************************************/

ADI_OSAL_STATUS _adi_osal_HeapInstall(uint32_t *pHeapMemory, uint32_t nHeapMemorySize)
{
    int32_t nHeapUserId = INT_MAX;
    return nHeapUserId;
}

/*!
  ****************************************************************************
   @internal

   @brief Abstracts which heap to use for dynamic memory allocations

   @param[out] ppData - Pointer that will store the allocated memory pointer
   @param[in] nSize  - Size of memory to be allocated

   @return ADI_OSAL_SUCCESS - if the memory was allocated correctly
   @return ADI_OSAL_MEM_ALLOC_FAILED  - if the memory could not be allocated

   @endinternal

*****************************************************************************/

ADI_OSAL_STATUS _adi_osal_MemAlloc(void** ppData, uint32_t nSize)
{
    void* pMemory;

    pMemory = malloc(nSize);

    if (pMemory != NULL)
    {
        *ppData = pMemory;
        return (ADI_OSAL_SUCCESS);
    }
    else
    {
        return (ADI_OSAL_MEM_ALLOC_FAILED);
    }
}



/*!
  ****************************************************************************
   @internal

   @brief Abstracts which heap to use to free dynamic memory

   @param[in] pData - Memory area to be freed (same argument as free)

   @endinternal

*****************************************************************************/

void  _adi_osal_MemFree(void* pData)
{
    free(pData);
}


/*!
  ****************************************************************************
    @brief  Activates a high-level interrupt handler
    .

    @param[in] iid - ID of the interrupt to be handled

    @return ADI_OSAL_SUCCESS      - If handler is successfully activated
    @return ADI_OSAL_FAILED       - If failed to activate handler

*****************************************************************************/
ADI_OSAL_STATUS
adi_osal_ActivateHandler (uint32_t iid)
{
    if (0 != adi_rtl_activate_dispatched_handler(iid))
    {
        /* Error - the IID doesn't correspond to a registered handler */
        return ADI_OSAL_FAILED;
    }
    return ADI_OSAL_SUCCESS;
}


/*!
  ****************************************************************************
    @brief  Deactivates a high-level interrupt handler

    @param[in] iid - ID of the interrupt to be handled

    @return ADI_OSAL_SUCCESS      - If handler is successfully deactivated
    @return ADI_OSAL_FAILED       - If failed to deactivate handler

*****************************************************************************/
ADI_OSAL_STATUS
adi_osal_DeactivateHandler (uint32_t iid)
{
    if (0 != adi_rtl_deactivate_dispatched_handler(iid))
    {
        /* Error - the IID doesn't correspond to a registered handler */
        return ADI_OSAL_FAILED;
    }
    return ADI_OSAL_SUCCESS;
}


/*!
  ****************************************************************************
    @brief  Uninstalls a high-level interrupt handler
    .

    @param[in] iid - ID of the interrupt to be handled

    @return ADI_OSAL_SUCCESS      - If handler is successfully uninstalled
    @return ADI_OSAL_FAILED       - If failed to uninstall handler
    @return ADI_OSAL_CALLER_ERROR - If function is invoked from an invalid
                                    location

*****************************************************************************/
ADI_OSAL_STATUS
adi_osal_UninstallHandler (uint32_t iid)
{
    int32_t status;

#ifdef OSAL_DEBUG
    if (CALLED_FROM_AN_ISR)
    {
        return ADI_OSAL_CALLER_ERROR;
    }
#endif /* OSAL_DEBUG */

    status = adi_rtl_unregister_dispatched_handler (iid);

    if (status != 0)
    {
        /* Error - the IID doesn't correspond to a registered handler */
        return ADI_OSAL_FAILED;
    }

    return ADI_OSAL_SUCCESS;
}


/*
**
** EOF:
**
*/
