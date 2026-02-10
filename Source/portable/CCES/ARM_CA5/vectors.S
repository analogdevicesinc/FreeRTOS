/*
 * Copyright (C) 2016-2018 Analog Devices Inc. All Rights Reserved.
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


/*
 * vectors.S
 *
 */
#include "runtime/int/interrupt.h"

.code 32

.extern _mainCRTStartup;
.extern adi_rtl_undef_instr_hndlr;
.extern FreeRTOS_SWI_Handler;
.extern adi_rtl_prefetch_abort_hndlr;
.extern adi_rtl_data_abort_hndlr;
.extern adi_rtl_unhandled_handler;
.extern FreeRTOS_IRQ_Handler;
.extern adi_rtl_fiq_hndlr;

.global adi_os_vectors_start
.global adi_os_vectors_end
.global adi_InitExceptionHandlers
.type   adi_InitExceptionHandlers, %function

.align 5
adi_os_vectors_start:
                LDR     pc, =_mainCRTStartup
                LDR     pc, =adi_rtl_undef_instr_hndlr
                LDR     pc, =FreeRTOS_SWI_Handler
                LDR     pc, =adi_rtl_prefetch_abort_hndlr
                LDR     pc, =adi_rtl_data_abort_hndlr
                LDR     pc, =adi_rtl_unhandled_handler
                LDR     pc, =FreeRTOS_IRQ_Handler
                LDR     pc, =adi_rtl_fiq_hndlr
adi_os_vectors_end:

.align 0
.code 32

adi_InitExceptionHandlers:
  PUSH {R4-R11, LR}

  LDR R4, =adi_os_vectors_start
  MCR P15, 0, R4, C12, C0, 0

  LDR R4, =_init_dispatch_tables                                            /* Initialize the dispatched vector table */
  BLX R4

  MRS R4, CPSR                                                              /* Enable interrupts and Enable imprecise Data Abort */
  /* FIXME: The line below has been modified to avoid emulator issues. When
   * the emulator is fixed we should return back to
   LDR R5, =~(ADI_RTL_ARM_CONTROL_INT_DIS | ADI_RTL_ARM_CONTROL_ABORT_DIS)
   */

  LDR R5, =~(ADI_RTL_ARM_CONTROL_INT_DIS)
  AND R4, R4, R5
  MSR CPSR_cxsf, R4

  POP {R4-R11, LR}
  BX LR

