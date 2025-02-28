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

#ifndef __ADI_OSAL_ARCH_INTERNAL__
#define __ADI_OSAL_ARCH_INTERNAL__

#if !defined (__ADSPSHARCFX__)
    #error "Unknown processor family"
#endif

/* includes (platform specific) */
#include <interrupt.h>

/* external code (shared across OSAL environments) */
extern ADI_OSAL_STATUS _adi_osal_HeapInstall(uint32_t *pHeapMemory, uint32_t nHeapMemorySize);
extern ADI_OSAL_STATUS _adi_osal_MemAlloc(void** ppData, uint32_t nSize);
extern void            _adi_osal_MemFree(void* pData);
static bool            _adi_osal_IsCurrentLevelISR( void );
static bool            _adi_osal_IsMemoryAligned(const void *pMemory);

/*!
  ****************************************************************************

   @internal

   @brief Describes whether the API is called at interrupt level or not

   @return true  - if the current execution is at interrupt level
   @return false - if the current execution is not at interrupt level

   @endinternal
*****************************************************************************/
#pragma inline
static bool _adi_osal_IsCurrentLevelISR( void )
{
	return ( xthal_intlevel_get() != 0u );
}

/*!
  ****************************************************************************
   @internal

   @brief This function indicates whether a pointer is aligned and can be used
          to store variables in the particular architecture

   @param[in] pMemory - Pointer to the allocated memory

   @return true    - if the memory was aligned
   @return false   - if the memory was not aligned

   @endinternal

*****************************************************************************/

#pragma inline
static bool _adi_osal_IsMemoryAligned(const void *pMemory)
{
    return(true);
}

#endif /* __ADI_OSAL_ARCH_INTERNAL__ */
