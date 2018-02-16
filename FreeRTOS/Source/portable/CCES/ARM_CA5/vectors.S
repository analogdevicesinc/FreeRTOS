/*******************************************************************************
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

*******************************************************************************/

/*
 * vectors.S
 *
 */
#include "runtime/int/interrupt.h"
#include <fatal_error_code.h>

.code 32

.extern _mainCRTStartup;
.extern adi_rtl_undef_instr_hndlr;
.extern FreeRTOS_SWI_Handler;
.extern adi_rtl_prefetch_abort_hndlr;
.extern adi_rtl_data_abort_hndlr;
.extern FreeRTOS_IRQ_Handler;
.extern adi_rtl_fiq_hndlr;

.global adi_rtl_unhandled_handler;
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

/********************************
  Default interrupt handler (may not be needed if we can use the one in the runtime library)
*********************************/
adi_rtl_unhandled_handler:
  PUSH {R4-R11, LR}

  LDR r0, =_AFE_G_RunTimeError                                                  /* Report a fatal error */
  LDR r1, =_AFE_S_NoDispatchedHandler
  LDR R4, =adi_fatal_error
  BLX R4

/* This instruction cannot be replaced with POP because POP does not have the option restore CPSR (indicated by ^)*/
  LDMFD   SP!, {R4-R11, LR, PC}^
