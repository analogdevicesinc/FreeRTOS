/**
 * Copyright (c) 2009-2023 Analog Devices Inc. All Rights Reserved. This
 * software is proprietary to Analog Devices, Inc. and its licensors.
 */

#ifndef __ADI_OSAL_ARCH_INTERNAL__
#define __ADI_OSAL_ARCH_INTERNAL__

#include <interrupt.h>
#include <sys/platform.h>
#include <stdint.h>

#if !defined (__ADSPCORTEXM33__)
   #error Unknown core
#endif


/* external data (shared across OSAL environments) */

/* external code (shared across OSAL environments) */
extern ADI_OSAL_STATUS _adi_osal_HeapInstall(uint32_t *pHeapMemory, uint32_t nHeapMemorySize);
extern ADI_OSAL_STATUS _adi_osal_MemAlloc(void** ppData, uint32_t nSize);
extern void            _adi_osal_MemFree(void* pData);
static inline bool     _adi_osal_IsCurrentLevelISR( void );
static inline bool     _adi_osal_IsMemoryAligned(const void *pMemory);


static inline uint32_t _adi_osal_InterruptsDisable(void)
{
    /* Get the current state of the interrupts and disable them */
    return disable_interrupts();
}


static inline void _adi_osal_InterruptsEnable(uint32_t previousState)
{
    if (previousState > 0u)
    {
        enable_interrupts();
    }
}

/*!
  ****************************************************************************

   @internal

   @brief Describes whether the API is called at interrupt level or not

   @return true  - if the current execution is at interrupt level
   @return false - if the current execution is not at interrupt level

   @endinternal
*****************************************************************************/
static inline bool _adi_osal_IsCurrentLevelISR( void )
{
    uint32_t local_ipsr;
    asm ("MRS %0, IPSR" :"=r"(local_ipsr): :);

    if (local_ipsr == 0u)
        return false;
    else
        return true;
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

static inline bool _adi_osal_IsMemoryAligned(const void *pMemory)
{
    return( 0u == ((uintptr_t)(pMemory) & 0x3u));
}

/*!
  ****************************************************************************
   @internal

   @brief This function provides an equivalent to the emuclk built-in

   @endinternal

*****************************************************************************/

static inline uint64_t emuclk(void) {
    static bool init_done = false;
    DWT_Type *dwt = DWT;
    if (!init_done) {
        DCB_Type *dcb = DCB;
        /* Enable Debug Exception and Monitor Control */
        dcb->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        /* Zero the cycle counter register */
        dwt->CYCCNT = 0u;
        /* Start the cycle counter */
        dwt->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
        init_done = true;
    }
    return dwt->CYCCNT;
}

#endif /* __ADI_OSAL_ARCH_INTERNAL__ */
