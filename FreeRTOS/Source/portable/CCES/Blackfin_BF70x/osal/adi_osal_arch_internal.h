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

*/
#ifndef __ADI_OSAL_ARCH_INTERNAL__
#define __ADI_OSAL_ARCH_INTERNAL__

#if !defined (__ADSPBLACKFIN__)
    #error "Unknown processor family"
#endif

#include <ccblkfn.h>
#include <blackfin.h>   /* for pIPEND definition */
#include <sys/exception.h> /* for EX_DISPATCHED_HANDLER_NESTED */
#include <sys/platform.h>
#include <adi_osal.h>

#pragma diag(push)
#pragma diag(suppress:misra_rule_8_12:"OSAL declares external global arrays without indicating their size")

/* external data (shared across OSAL environments) */

/*!
    @internal
    @var _adi_osal_gHandlerTable
	The OSAL dispatch table. It is an array of function pointers, of the
	type corresponding to OSAL's plain C interrupt handlers (i.e. the high-level
	handlers). The table needs to be large enough for any index that we can get
	back from adi_rtl_register_dispatched_handler().
    @endinternal
 */
extern ADI_OSAL_HANDLER_PTR _adi_osal_gHandlerTable[];
#pragma diag(pop)
/*!
    @internal
    @var _adi_osal_gHandlerTableSize
	The size of the OSAL dispatch table. The size needs to be large enough for
    any index that we can get back from adi_rtl_register_dispatched_handler().
    @endinternal
 */
extern uint32_t _adi_osal_gHandlerTableSize;

/* external code (shared across OSAL environments) */
extern ADI_OSAL_STATUS _adi_osal_HeapInstall(uint32_t *pHeapMemory, uint32_t nHeapMemorySize);
extern ADI_OSAL_STATUS _adi_osal_MemAlloc(void** ppData, uint32_t nSize);
extern void            _adi_osal_MemFree(void* pData);
extern bool            _adi_osal_IsCurrentLevelISR( void );
extern bool            _adi_osal_IsMemoryAligned(const void *pMemory);

#pragma diag(push)
/* Rule 6.3(Adv): Typedefs that indicate size and signedness should be used in place of basic numerical types. */
#pragma diag(suppress:misra_rule_6_3 : "The types used by EX_DISPATCHED_HANDLER_NESTED are defined by the RTL")
EX_DISPATCHED_HANDLER_NESTED(_adi_osal_stdWrapper, iid, index, arg);
EX_DISPATCHED_HANDLER_NESTED(_adi_osal_stdWrapper_isr_stk, iid, index, arg);
EX_DISPATCHED_HANDLER_NON_NESTED(_adi_osal_plainWrapper, iid, index, arg);
#if !defined(__ADI_HAS_SEC__) /* BF5xx Blackfin*/
EX_DISPATCHED_HANDLER_NESTED(_adi_osal_systemWrapper, iid,  index, arg);
EX_DISPATCHED_HANDLER_NESTED(_adi_osal_systemWrapper_isr_stk, iid, index, arg);
#endif /* BF5xx Blackfin*/
#pragma diag(pop)

#pragma diag(push)
#pragma diag(suppress:misra_rule_19_7:"Inline function doesn't work very well here.")

#pragma inline
static uint32_t _adi_osal_InterruptsDisable(void)
{
    return(cli());
}

#pragma inline
static void _adi_osal_InterruptsEnable(uint32_t previousState)
{
    sti(previousState);
}

/*!
  ****************************************************************************

   @internal

   @brief Describes whether the API is called at interrupt level or not

   @return true  - if the current execution ISR level is less than 15
   @return false - if the level is 15

   @endinternal
*****************************************************************************/
#pragma inline
bool _adi_osal_IsCurrentLevelISR( void )
{
#if defined(__ADI_GENERATED_DEF_HEADERS__)
    return ( 0ul != (*pIPEND & ~( (unsigned long) BITM_IPEND_IRPTEN | (unsigned long) BITM_IPEND_IVG15 ))) ;
#else
    return ( 0ul != (*pIPEND & ~( (1ul<<EVT_IRPTEN_P) | (1ul<< EVT_IVG15_P) ))) ;
#endif
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

#pragma diag(suppress:misra_rule_11_3 : "We need to convert the pointer to see if it is aligned")

#pragma inline
bool _adi_osal_IsMemoryAligned(const void *pMemory)
{
    return( 0u == ((uint32_t)(pMemory) & 0x3u));
}


#pragma diag(pop)

#endif /* __ADI_OSAL_ARCH_INTERNAL__ */
