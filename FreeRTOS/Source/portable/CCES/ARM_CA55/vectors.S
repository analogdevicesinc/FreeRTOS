/*
 * Copyright (c) 2021 Analog Devices, Inc.  All rights reserved.
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

#include "runtime/int/interrupt.h"

.extern adi_dispatched_int_vector_table;
.global vectors_start

.extern FreeRTOS_SWI_Handler;
.extern FreeRTOS_IRQ_Handler;

.balign 0x800

vectors_start:


/* Synchronous exception handler using SP0 stack */

adi_rtl_sp0_sync_handler:
    /* On entry to the dispatcher, SPSel is set to 1 by the hardware */
    B       FreeRTOS_SWI_Handler


/* IRQ handler using SP0 stack */

.balign 0x80
adi_rtl_sp0_irq_handler:
    /* On entry to the dispatcher, SPSel is set to 1 by the hardware */
    B       FreeRTOS_IRQ_Handler


/* FIQ handler using SP0 stack */

.balign 0x80
adi_rtl_sp0_fiq_handler:
    /* On entry to the dispatcher, SPSel is set to 1 by the hardware */
    B       FreeRTOS_IRQ_Handler


/* System error handler using SP0 stack */

.balign 0x80
adi_rtl_sp0_serror_handler:
    /* On entry to the dispatcher, SPSel is set to 1 by the hardware */
    B       adi_rtl_spx_serror_handler


/* Synchronous exception handler using SPx stack */

.balign 0x80
adi_rtl_spx_sync_handler:
    STP     X0,  X1,  [SP, #-16]!
    STP     X20, X21, [SP, #-16]!
    MOV     X20, #-1                                         /* Mark as no need for ending the interrupt */

    MOV     X0, #ADI_RTL_EXCEPT_IID(ADI_RTL_EXCEPT_SYNC)     /* IID */
    LDR     X1, =adi_dispatched_int_vector_table+ADI_RTL_EXC0_OFFSET+ADI_RTL_EXCEPT_SYNC
                                                             /* Vector table entry address */
    B       .adi_rtl_dispatcher_core


/* IRQ handler using SPx stack */

.balign 0x80
adi_rtl_spx_irq_handler:
    B       FreeRTOS_IRQ_Handler


/* FIQ handler using SPx stack */

.balign 0x80
adi_rtl_spx_fiq_handler:
    B       FreeRTOS_IRQ_Handler


/* System error handler using SPx stack */

.balign 0x80
adi_rtl_spx_serror_handler:
    STP     X0,  X1,  [SP, #-16]!
    STP     X20, X21, [SP, #-16]!
    MOV     X20, #-1                                         /* Mark as no need for ending the interrupt */

    MOV     X0, #ADI_RTL_EXCEPT_IID(ADI_RTL_EXCEPT_SERR)     /* IID */
    LDR     X1, =adi_dispatched_int_vector_table+ADI_RTL_EXC0_OFFSET+ADI_RTL_EXCEPT_SERR
                                                             /* Vector table entry address */

/* This implements the core of the dispatcher, saving scratch registers (except X0 and X1) and calling the
 * handler.
 * It then restores all scratch registers (including X0 and X1) and the call-preserved registers X20 and X21,
 * and returns from the exception.
 * On entry, X0 contains the interrupt ID, X1 contains the address of the vector table entry, and X20
 * contains the interrupt group number (or -1 for an exception).
 */
.adi_rtl_dispatcher_core:
    /* Save the scratch registers X18 and LR, so we can use them */
    STP     X18, LR,  [SP, #-16]!

    /* Set the LR to the exception return address (which helps with showing the call stack
     * in the IDDE) and save both ELR_EL3 and SPSR_EL3 to the stack so that we can turn
     * interrupts back on.
     */
    MRS     LR,  ELR_EL3
    MRS     X18, SPSR_EL3
    STP     X18, LR,  [SP, #-16]!

    /* Now we can turn interrupts on */
    UBFX    X18, X18, #6, #4; /* Get DAIF bits from before we took the interrupt/exception */
    MSR     DAIF, X18

    /* Save the rest of the scratch registers */
    STP     X2,  X3,  [SP, #-16]!
    STP     X4,  X5,  [SP, #-16]!
    STP     X6,  X7,  [SP, #-16]!
    STP     X8,  X9,  [SP, #-16]!
    STP     X10, X11, [SP, #-16]!
    STP     X12, X13, [SP, #-16]!
    STP     X14, X15, [SP, #-16]!
    STP     X16, X17, [SP, #-16]!
    STP     Q0,  Q1,  [SP, #-32]!
    STP     Q2,  Q3,  [SP, #-32]!
    STP     Q4,  Q5,  [SP, #-32]!
    STP     Q6,  Q7,  [SP, #-32]!
    STP     Q8,  Q9,  [SP, #-32]! /* only bottom 64 bits are call-preserved */
    STP     Q10, Q11, [SP, #-32]! /* only bottom 64 bits are call-preserved */
    STP     Q12, Q13, [SP, #-32]! /* only bottom 64 bits are call-preserved */
    STP     Q14, Q15, [SP, #-32]! /* only bottom 64 bits are call-preserved */
    STP     Q16, Q17, [SP, #-32]!
    STP     Q18, Q19, [SP, #-32]!
    STP     Q20, Q21, [SP, #-32]!
    STP     Q22, Q23, [SP, #-32]!
    STP     Q24, Q25, [SP, #-32]!
    STP     Q26, Q27, [SP, #-32]!
    STP     Q28, Q29, [SP, #-32]!
    STP     Q30, Q31, [SP, #-32]!

    /* Call the handler */
    MOV     X21, X0                                            /* Preserve ID for end of interrupt acknowledgement */
    STP     X29, LR,  [SP, #-16]!
    MOV     X29, SP
    LDR     X2, [X1]                                           /* Handler address */
    LDR     X1, [X1, #ADI_RTL_DISPATCHED_CALLBACK_ARG_OFFSET]  /* Callback argument */
    BLR     X2
    LDP     X29, LR,  [SP], #16

    MSR     DAIFSET, #15                                       /* Disable interrupts before signalling the end of interrupt */

    /* Confirm the end of the interrupt */
    CMP     X20, #0
    BNE     .LnotGroup0
    MSR     s3_0_c12_c8_1,  X21                                /* Write interrupt ID to ICC_EOIR0_EL1 */
.LnotGroup0:
    BLE     .LendOfIntConfirmed
    MSR     s3_0_c12_c12_1, X21                                /* Write interrupt ID to ICC_EOIR1_EL1 */

.LendOfIntConfirmed:
    /* Restore the scratch registers */
    LDP     Q30, Q31, [SP], #32
    LDP     Q28, Q29, [SP], #32
    LDP     Q26, Q27, [SP], #32
    LDP     Q24, Q25, [SP], #32
    LDP     Q22, Q23, [SP], #32
    LDP     Q20, Q21, [SP], #32
    LDP     Q18, Q19, [SP], #32
    LDP     Q16, Q17, [SP], #32
    LDP     Q14, Q15, [SP], #32
    LDP     Q12, Q13, [SP], #32
    LDP     Q10, Q11, [SP], #32
    LDP     Q8,  Q9,  [SP], #32
    LDP     Q6,  Q7,  [SP], #32
    LDP     Q4,  Q5,  [SP], #32
    LDP     Q2,  Q3,  [SP], #32
    LDP     Q0,  Q1,  [SP], #32
    LDP     X16, X17, [SP], #16
    LDP     X14, X15, [SP], #16
    LDP     X12, X13, [SP], #16
    LDP     X10, X11, [SP], #16
    LDP     X8,  X9,  [SP], #16
    LDP     X6,  X7,  [SP], #16
    LDP     X4,  X5,  [SP], #16
    LDP     X2,  X3,  [SP], #16
    LDP     X18, LR,  [SP], #16

    /* Restore interrupt state registers ready for ERET */
    MSR     SPSR_EL3, X18
    MSR     ELR_EL3,  LR

    LDP     X18, LR,  [SP], #16

.Lspx_handler_end:
    /* Restore registers saved in non-common part of code */
    LDP     X20, X21, [SP], #16
    LDP     X0,  X1,  [SP], #16

    /* Return from the dispatcher back to interrupted code */
    ERET
