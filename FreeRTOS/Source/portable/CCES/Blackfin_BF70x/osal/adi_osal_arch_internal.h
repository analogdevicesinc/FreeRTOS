/*****************************************************************************
    Copyright (C) 2016-2018 Analog Devices Inc. All Rights Reserved.
*****************************************************************************/

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
