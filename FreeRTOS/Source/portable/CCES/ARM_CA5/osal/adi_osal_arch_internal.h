/*
****************************************************************************
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
#ifndef __ADI_OSAL_ARCH_INTERNAL__
#define __ADI_OSAL_ARCH_INTERNAL__

#if !defined (__ADSPCORTEXA5__)
    #error "Unknown processor family"
#endif

#include <sys/platform.h>
#include <runtime/int/interrupt.h>

/* external data (shared across OSAL environments) */

/* external code (shared across OSAL environments) */
extern ADI_OSAL_STATUS _adi_osal_HeapInstall(uint32_t *pHeapMemory, uint32_t nHeapMemorySize);
extern ADI_OSAL_STATUS _adi_osal_MemAlloc(void** ppData, uint32_t nSize);
extern void            _adi_osal_MemFree(void* pData);
static inline bool     _adi_osal_IsCurrentLevelISR( void );
static inline bool     _adi_osal_IsMemoryAligned(const void *pMemory);

#if 0 /* Not currently used in the FreeRTOS version of OSAL */
static inline uint32_t _adi_osal_InterruptsDisable(void)
{
    uint32_t local_cpsr;
    uint32_t state;

    asm ("MRS %0, CPSR" :"=r"(local_cpsr): :);

    /* Get the state of the interrupts */
    state = (local_cpsr & (ADI_RTL_ARM_CONTROL_FIQ_DIS | ADI_RTL_ARM_CONTROL_IRQ_DIS));

    /* If not already disabled disable them */
    if(state != 0u)
    {
        __builtin_disable_interrupts();
    }

     return state;
}


static inline void _adi_osal_InterruptsEnable(uint32_t previousState)
{
    if(previousState > 0u)
    {
        __builtin_enable_interrupts();
    }
}
#endif
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
  	uint32_t local_cpsr;
  	uint32_t mode;

	asm ("MRS %0, CPSR" :"=r"(local_cpsr): :);

	mode = (local_cpsr & ADI_RTL_ARM_MODE_MASK);

	/* Under FreeRTOS all GIC interrupts are dispatched in SVC mode, but
	 * tasks run in SYSTEM mode (or possibly USER mode if protection was enabled).
	 * Therefore, any mode other than USR or SVC indicated that we're in an interrupt.
	 *
	 */
    if ((mode == ADI_RTL_ARM_MODE_USR) || (mode == ADI_RTL_ARM_MODE_SYS))
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
    return( 0u == ((uint32_t)(pMemory) & 0x3u));
}

#endif /* __ADI_OSAL_ARCH_INTERNAL__ */
