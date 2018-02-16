/******************************************************************************
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

 ******************************************************************************/


/*Rule 14.7 indicates that a function shall have a single exit point */
#pragma diag(suppress:misra_rule_14_7:"Allowing several point of exit (mostly for handling parameter error checking) increases the code readability and therefore maintainability")


/*=============  I N C L U D E S   =============*/


#include <adi_osal.h>
#include <sys/platform.h>
#include <sys/exception.h>
#include "osal_common.h"
#include "adi_osal_arch_internal.h"

/*=============  D E F I N E S  =============*/


#pragma file_attr(  "libGroup=adi_osal.h")
#pragma file_attr(  "libName=libosal")
#pragma file_attr(  "prefersMem=internal")
#pragma file_attr(  "prefersMemNum=30")

/*
 * The following macros test an interrupt identifier (IID) to see whether it is:
 *  - a system interrupt (bit 20 is clear)
 *  - an exception (bit 20 is set and IVG is 3)
 *  - the non-maskable interrupt (bit 20 is set and IVG is 2)
 * other bits are don't cares.
 *
 * The organisation of the IID is:
 *
 * 31 30 29 28 27 26 25 24 23 22 21 |    20    | 19 18 17 16 | 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
 * < --------- reserved �------ > |  <SYS?>  |   < IVG >   | < ------------ SID ----------------->
 * Bitmasks: 0xFFE00000             |0x00100000|  0x000F0000 |              0x0000FFFF
 */
#pragma diag(push)
/* Rule 19.7(Adv): A function shall be used in preference to a function-like macro. */
#pragma diag(suppress:misra_rule_19_7:"This source needs to use function-like macros")
#define IID_IS_SYSTEM(iid) (((iid) & 0x00100000u) == 0u)
#define IID_IS_EXCEPTION(iid) (((iid) & 0x001F0000u) == 0x00130000u)
#define IID_IS_NMI(iid) (((iid) & 0x001F0000u) == 0x00120000u)
#pragma diag(pop)

/* Constant used to indicate whether the OS is using a dedicated stack for ISRs.
 * This value is set by the OS, and indicates which type of interrupt dispatcher
 * should be used for ISRs.  If this is zero, then interrupt dispatching will run on
 * the current stack.  If nonzero, the interrupt dispatcher will switch from the
 * current stack onto a dedicated stack for all ISRs.
 * This constant is used to determine which OSAL dispatcher to install, below.
 */
extern const uint32_t adi_ucos_iii_use_isr_stack;


/*!
  ****************************************************************************
    @brief  Installs a high-level interrupt handler
    .

    @param[in] iid - ID of the interrupt to be handled
    @param[in] highLevelHandler - Function address of the handler
    @param[in] handlerArg - Generic argument to be passed to the handler

    @return ADI_OSAL_SUCCESS      - If handler is successfully installed
    @return ADI_OSAL_FAILED       - If failed to install handler
    @return ADI_OSAL_CALLER_ERROR - If function is invoked from an invalid
                                    location

*****************************************************************************/
ADI_OSAL_STATUS
adi_osal_InstallHandler (
   uint32_t iid,
   ADI_OSAL_HANDLER_PTR highLevelHandler,
   void* handlerArg
)
{
    int32_t index;

#ifdef OSAL_DEBUG
    if (CALLED_FROM_AN_ISR)
    {
        return ADI_OSAL_CALLER_ERROR;
    }
#endif /* OSAL_DEBUG */

    /* In the BF7xx port of FreeRTOS, no special wrapping of interrupts is required as
     * there are no RTOS-specific ISR prologs or epilogs.
     */
    index = adi_rtl_register_dispatched_handler (iid,
    		                                     _adi_osal_stdWrapper,
                                                 handlerArg);

    if (index < 0)
    {
        /* error */
        return ADI_OSAL_FAILED;
    }

    if (index >= (int)_adi_osal_gHandlerTableSize)
    {
        /* Register succeeded but OSAL's dispatch table is
         * too small for the returned index.
         */
        adi_rtl_unregister_dispatched_handler (iid);
        return ADI_OSAL_FAILED;
    }

    _adi_osal_gHandlerTable[index] = highLevelHandler;
    return ADI_OSAL_SUCCESS;
}

/*
 * This is the environment-specific interrupt wrapper,
 * i.e. the wrapper that is used for interrupts that
 * may call operating system APIs, and hence must support
 * rescheduling. In the FreeRTOS OSAL they have essentially the
 * same implementation as the plain wrapper, no special wrapping
 * of interrupts is required as there are no RTOS-specific ISR
 * prologs or epilogs. We let the compiler generate the appropriate
 * code for saving and restoring registers, and for setting up the C runtime.
 */
EX_DISPATCHED_HANDLER_NESTED(_adi_osal_stdWrapper, iid,  index, arg)
{
	(_adi_osal_gHandlerTable[index])(iid, (void*) arg);
}
