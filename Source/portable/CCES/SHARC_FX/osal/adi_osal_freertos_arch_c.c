/*
 * Copyright (C) 2018-2025 Analog Devices Inc. All Rights Reserved.
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


#include <adi_osal.h>
#include <sys/platform.h>
#include "osal_common.h"
#include "adi_osal_arch_internal.h"
#include "dispatched_interrupts.h"

/*=============  D E F I N E S  =============*/


#pragma file_attr(  "libGroup=adi_osal.h")
#pragma file_attr(  "libName=libosal")
#pragma file_attr(  "prefersMem=internal")
#pragma file_attr(  "prefersMemNum=30")

/* Core interrupts are always mapped from the start of the interrupt
 * vector table.
 */
static inline int32_t adi_osal_register_core_handler(uint32_t cid,
                                                     adi_dispatched_handler_t handler,
                                                     adi_dispatched_callback_t callback)
{
    uint32_t interrupt_enable_state;

    if (cid >= NUM_CORE_INTS) // sanity check
        return -1;

    /* We protect the updates to the registration state from interrupts,
     * and hence also from thread switching.
     */
    interrupt_enable_state = disable_interrupts();

    /* Register the handler in the vector table. We don't need to
     * check if we're overwriting an existing handler, as this API simply
     * overwrites what was there without error.
     */
    __adi_dispatched_core_int_vector_table[cid].handler = handler;  /* set handler */
    __adi_dispatched_core_int_vector_table[cid].callback = callback;/* & callback  */

    if (interrupt_enable_state != 0U)
        enable_interrupts();

    /* Return the index into the combined table, which will be returned to
     * the user.
     */
    return cid;
}

static inline int32_t adi_osal_register_system_handler(uint32_t sid,
                                                       adi_dispatched_handler_t handler,
                                                       adi_dispatched_callback_t callback)
{
    uint32_t interrupt_enable_state;

    if (sid >= NUM_SYS_INTS) // sanity check
        return -1;

    /* We protect the updates to the registration state from interrupts,
     * and hence also from thread switching.
     */
    interrupt_enable_state = disable_interrupts();

    /* Register the handler in the vector table. We don't need to
     * check if we're overwriting an existing handler, as this API simply
     * overwrites what was there without error.
     */
    __adi_dispatched_sec_int_vector_table[sid].handler = handler;  /* set handler */
    __adi_dispatched_sec_int_vector_table[sid].callback = callback;/* & callback  */

    if (interrupt_enable_state != 0U)
        enable_interrupts();

    /* Return the index into the combined table, which will be returned to
     * the user.
     */
    return sid + NUM_CORE_INTS;
}

static inline int32_t adi_osal_register_exception_handler(uint32_t eid,
                                                          adi_dispatched_handler_t handler,
                                                          adi_dispatched_callback_t callback)
{
    uint32_t interrupt_enable_state;
    int32_t index;

    /* We protect the updates to the registration state from interrupts,
     * and hence also from thread switching.
     */
    interrupt_enable_state = disable_interrupts();

    if (eid > EXCCAUSE_FULLTYPE_MASK)
        return -1;

    index = __find_exception_id_index(eid);

    if (index >= NUM_EXCEPTIONS)
        return -1; /* This should not happen */

    __adi_dispatched_exception_mapping[index] = eid;

    __adi_dispatched_exception_vector_table[index].handler = handler;
    __adi_dispatched_exception_vector_table[index].callback = callback;

    if (interrupt_enable_state != 0U)
        enable_interrupts();

    /* Return the index into the combined table, which will be returned to
     * the user.
     */
    return index + NUM_CORE_INTS + NUM_SYS_INTS;
}

static void adi_osal_core_interrupt_handler(void *arg)
{
    adi_dispatched_data_t *dispatcher_data = (adi_dispatched_data_t *)arg;
    uint32_t cid = dispatcher_data - __adi_dispatched_core_int_vector_table;
    dispatcher_data->handler(ADI_CORE_INT_TO_IID(cid), dispatcher_data->callback);
}

static void adi_osal_exception_handler(XtExcFrame *arg) {
    ExcFrame *frame = (ExcFrame *)arg;
    uint32_t eid = frame->exccause & (uint32_t)EXCCAUSE_FULLTYPE_MASK;
    int32_t index = __find_exception_id_index(eid);
    if (__adi_dispatched_exception_mapping[index] != 0U)
    {
        adi_dispatched_data_t *dispatcher_data = &__adi_dispatched_exception_vector_table[index];
        dispatcher_data->handler(ADI_EXCEPTION_TO_IID(eid), dispatcher_data->callback);
    }
}

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

   if (ADI_IID_IS_EXCEPTION(iid))
   {
       index = adi_osal_register_exception_handler(ADI_IID_TO_EXCEPTION(iid),
                                                   highLevelHandler,
                                                   handlerArg);
   }
   else if (ADI_IID_IS_SEC_INT(iid))
   {
       index = adi_osal_register_system_handler(ADI_IID_TO_SEC_INT(iid),
                                                highLevelHandler,
                                                handlerArg);
   }
   else
   {
       index = adi_osal_register_core_handler(iid,
                                              highLevelHandler,
                                              handlerArg);
   }

   if (index == -1)
   {
       return ADI_OSAL_FAILED;
   }

   return ADI_OSAL_SUCCESS;
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
    xt_handler index;

    if (ADI_IID_IS_SEC_INT(iid))
    {
        return ADI_OSAL_SUCCESS;
    }
    else if (ADI_IID_IS_EXCEPTION(iid))
    {
        uint32_t eid = ADI_IID_TO_EXCEPTION(iid);
        uint32_t interrupt_enable_state;
        int32_t index;

        if (eid > EXCCAUSE_FULLTYPE_MASK)
        {
            return ADI_OSAL_FAILED;
        }

        /* Make sure mapping table doesn't get changed by another thread
         * after we get the index.
         */
        interrupt_enable_state = disable_interrupts();
        index = __find_exception_id_index(eid);
        if (index >= NUM_EXCEPTIONS)
        {
            return ADI_OSAL_FAILED;
        }

        __adi_dispatched_exception_mapping[index] = eid;
        if (interrupt_enable_state)
        {
            enable_interrupts();
        }

        if (eid == EXC_TYPE_FP_INV_OP)
        {
            uint32_t fcr = XT_RUR_FCR();
            XT_WUR_FCR(fcr | BITM_FCR_FP_INV_OP);
        }
        else if (eid == EXC_TYPE_FP_OVERFLOW)
        {
            uint32_t fcr = XT_RUR_FCR();
            XT_WUR_FCR(fcr | BITM_FCR_FP_OVERFLOW);
        }
        else if (eid == EXC_TYPE_FP_UNDERFLOW)
        {
            uint32_t fcr = XT_RUR_FCR();
            XT_WUR_FCR(fcr | BITM_FCR_FP_UNDERFLOW);
        }
        else if (eid == EXC_TYPE_FP_DIV_BY_ZERO)
        {
            uint32_t fcr = XT_RUR_FCR();
            XT_WUR_FCR(fcr | BITM_FCR_FP_DIV_BY_ZERO);
        }
        else if (eid == EXC_TYPE_FP_INEXACT)
        {
            uint32_t fcr = XT_RUR_FCR();
            XT_WUR_FCR(fcr | BITM_FCR_FP_INEXACT);
        }

        xt_set_exception_handler(ADI_EXCEPTION_CAUSE(eid),
                                 adi_osal_exception_handler);

        return ADI_OSAL_SUCCESS;
    }
    else
    {
        index = xt_set_interrupt_handler(iid,
                                         adi_osal_core_interrupt_handler,
                                         &__adi_dispatched_core_int_vector_table[iid]);
        if (index == NULL)
        {
            return ADI_OSAL_FAILED;
        }

        xt_interrupt_enable(iid);
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

    return adi_osal_InstallHandler(iid, DEFAULT_HANDLER, NULL);
}
