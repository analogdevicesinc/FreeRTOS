/*
 * FreeRTOS Kernel V10.4.3 LTS Patch 1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 * 1 tab == 4 spaces!
 */

	.text

	/* Variables and functions. */
	.extern ullMaxAPIPriorityMask
	.extern pxCurrentTCB
	.extern vTaskSwitchContext
	.extern vApplicationIRQHandler
	.extern ullPortInterruptNesting
	.extern ullPortTaskHasFPUContext
	.extern ullCriticalNesting
	.extern ullPortYieldRequired

	.global FreeRTOS_IRQ_Handler
	.global FreeRTOS_SWI_Handler
	.global vPortRestoreTaskContext


.macro portSAVE_CONTEXT

	/* Switch to use the EL0 stack pointer. */
	MSR 	SPSEL, #0

	/* Save the entire context. */
	STP 	X0, X1, [SP, #-0x10]!
	STP 	X2, X3, [SP, #-0x10]!
	STP 	X4, X5, [SP, #-0x10]!
	STP 	X6, X7, [SP, #-0x10]!
	STP 	X8, X9, [SP, #-0x10]!
	STP 	X10, X11, [SP, #-0x10]!
	STP 	X12, X13, [SP, #-0x10]!
	STP 	X14, X15, [SP, #-0x10]!
	STP 	X16, X17, [SP, #-0x10]!
	STP 	X18, X19, [SP, #-0x10]!
	STP 	X20, X21, [SP, #-0x10]!
	STP 	X22, X23, [SP, #-0x10]!
	STP 	X24, X25, [SP, #-0x10]!
	STP 	X26, X27, [SP, #-0x10]!
	STP 	X28, X29, [SP, #-0x10]!
	STP 	X30, XZR, [SP, #-0x10]!

	/* Save the SPSR. */
	MRS		X3, SPSR_EL3
	/* Save the ELR. */
	MRS		X2, ELR_EL3

	STP 	X2, X3, [SP, #-0x10]!

	/* Save the critical section nesting depth. */
	LDR		X0, ullCriticalNestingConst
	LDR		X3, [X0]

	/* Save the FPU context indicator. */
	LDR		X0, ullPortTaskHasFPUContextConst
	LDR		X2, [X0]

	/* Save the FPU context, if any (32 128-bit registers). */
	CMP		X2, #0
	B.EQ	1f
	STP		Q0, Q1, [SP,#-0x20]!
	STP		Q2, Q3, [SP,#-0x20]!
	STP		Q4, Q5, [SP,#-0x20]!
	STP		Q6, Q7, [SP,#-0x20]!
	STP		Q8, Q9, [SP,#-0x20]!
	STP		Q10, Q11, [SP,#-0x20]!
	STP		Q12, Q13, [SP,#-0x20]!
	STP		Q14, Q15, [SP,#-0x20]!
	STP		Q16, Q17, [SP,#-0x20]!
	STP		Q18, Q19, [SP,#-0x20]!
	STP		Q20, Q21, [SP,#-0x20]!
	STP		Q22, Q23, [SP,#-0x20]!
	STP		Q24, Q25, [SP,#-0x20]!
	STP		Q26, Q27, [SP,#-0x20]!
	STP		Q28, Q29, [SP,#-0x20]!
	STP		Q30, Q31, [SP,#-0x20]!

1:
	/* Store the critical nesting count and FPU context indicator. */
	STP 	X2, X3, [SP, #-0x10]!

	LDR 	X0, pxCurrentTCBConst
	LDR 	X1, [X0]
	MOV 	X0, SP   /* Move SP into X0 for saving. */
	STR 	X0, [X1]

	/* Switch to use the ELx stack pointer. */
	MSR 	SPSEL, #1

	.endm

; /**********************************************************************/

.macro portRESTORE_CONTEXT

	/* Switch to use the EL0 stack pointer. */
	MSR 	SPSEL, #0

	/* Set the SP to point to the stack of the task being restored. */
	LDR		X0, pxCurrentTCBConst
	LDR		X1, [X0]
	LDR		X0, [X1]
	MOV		SP, X0

	LDP 	X2, X3, [SP], #0x10  /* Critical nesting and FPU context. */

	/* Set the PMR register to be correct for the current critical nesting
	depth. */
	LDR		X0, ullCriticalNestingConst /* X0 holds the address of ullCriticalNesting. */
	MOV		X1, #255					/* X1 holds the unmask value. */
	CMP		X3, #0
	B.EQ	1f
	LDR		X6, ullMaxAPIPriorityMaskConst
	LDR		X1, [X6]					/* X1 holds the mask value. */
1:
	MSR		s3_0_c4_c6_0, X1			/* Using special encoding s3_0_c4_c6_0 for ICC_PMR_EL1 */
	DSB 	SY							/* _RB_Barriers probably not required here. */
	ISB 	SY
	STR		X3, [X0]					/* Restore the task's critical nesting count. */

	/* Restore the FPU context indicator. */
	LDR		X0, ullPortTaskHasFPUContextConst
	STR		X2, [X0]

	/* Restore the FPU context, if any. */
	CMP		X2, #0
	B.EQ	1f
	LDP		Q30, Q31, [SP], #0x20
	LDP		Q28, Q29, [SP], #0x20
	LDP		Q26, Q27, [SP], #0x20
	LDP		Q24, Q25, [SP], #0x20
	LDP		Q22, Q23, [SP], #0x20
	LDP		Q20, Q21, [SP], #0x20
	LDP		Q18, Q19, [SP], #0x20
	LDP		Q16, Q17, [SP], #0x20
	LDP		Q14, Q15, [SP], #0x20
	LDP		Q12, Q13, [SP], #0x20
	LDP		Q10, Q11, [SP], #0x20
	LDP		Q8, Q9, [SP], #0x20
	LDP		Q6, Q7, [SP], #0x20
	LDP		Q4, Q5, [SP], #0x20
	LDP		Q2, Q3, [SP], #0x20
	LDP		Q0, Q1, [SP], #0x20
1:
	LDP 	X2, X3, [SP], #0x10  /* SPSR and ELR. */

	/* Restore the SPSR. */
	MSR		SPSR_EL3, X3 /*_RB_ Assumes started in EL3. */
	/* Restore the ELR. */
	MSR		ELR_EL3, X2

	LDP 	X30, XZR, [SP], #0x10
	LDP 	X28, X29, [SP], #0x10
	LDP 	X26, X27, [SP], #0x10
	LDP 	X24, X25, [SP], #0x10
	LDP 	X22, X23, [SP], #0x10
	LDP 	X20, X21, [SP], #0x10
	LDP 	X18, X19, [SP], #0x10
	LDP 	X16, X17, [SP], #0x10
	LDP 	X14, X15, [SP], #0x10
	LDP 	X12, X13, [SP], #0x10
	LDP 	X10, X11, [SP], #0x10
	LDP 	X8, X9, [SP], #0x10
	LDP 	X6, X7, [SP], #0x10
	LDP 	X4, X5, [SP], #0x10
	LDP 	X2, X3, [SP], #0x10
	LDP 	X0, X1, [SP], #0x10

	/* Switch to use the ELx stack pointer.  _RB_ Might not be required. */
	MSR 	SPSEL, #1

	ERET

	.endm


/******************************************************************************
 * FreeRTOS_SWI_Handler handler is used to perform a context switch.
 *****************************************************************************/
.align 8
.type FreeRTOS_SWI_Handler, %function
FreeRTOS_SWI_Handler:
	/* Save the context of the current task and select a new task to run. */
	portSAVE_CONTEXT

	MRS		X0, ESR_EL3
	LSR		X1, X0, #26
	CMP		X1, #0x17 	/* 0x17 = SMC instruction. */

	B.NE	FreeRTOS_Abort
	BL 		vTaskSwitchContext

	portRESTORE_CONTEXT

FreeRTOS_Abort:
	/* Full ESR is in X0, exception class code is in X1. */
	B		.

/******************************************************************************
 * vPortRestoreTaskContext is used to start the scheduler.
 *****************************************************************************/
.align 8
.type vPortRestoreTaskContext, %function
vPortRestoreTaskContext:

	DSB		SY
	ISB		SY

	/* Start the first task. */
	portRESTORE_CONTEXT


/******************************************************************************
 * FreeRTOS_IRQ_Handler handles IRQ entry and exit.
 *****************************************************************************/
.align 8
.type FreeRTOS_IRQ_Handler, %function
FreeRTOS_IRQ_Handler:
	/* Save volatile registers. */
	STP		X0, X1, [SP, #-0x10]!
	STP		X2, X3, [SP, #-0x10]!
	STP		X4, X5, [SP, #-0x10]!
	STP		X6, X7, [SP, #-0x10]!
	STP		X8, X9, [SP, #-0x10]!
	STP		X10, X11, [SP, #-0x10]!
	STP		X12, X13, [SP, #-0x10]!
	STP		X14, X15, [SP, #-0x10]!
	STP		X16, X17, [SP, #-0x10]!
	STP		X18, X19, [SP, #-0x10]!
	STP		X29, X30, [SP, #-0x10]!

	/* Save the SPSR and ELR. */
	MRS		X3, SPSR_EL3
	MRS		X2, ELR_EL3
	STP 	X2, X3, [SP, #-0x10]!

	/* Increment the interrupt nesting counter. */
	LDR		X5, ullPortInterruptNestingConst
	LDR		X1, [X5]	/* Old nesting count in X1. */
	ADD		X6, X1, #1
	STR		X6, [X5]	/* Address of nesting count variable in X5. */

	/* Maintain the interrupt nesting information across the function call. */
	STP		X1, X5, [SP, #-0x10]!

	/* Read value from the interrupt acknowledge register, which is stored in W0
	for future parameter and interrupt clearing use. */
    MRS		X3, s3_0_c12_c8_0  /* Using special encoding s3_0_c12_c8_0 for ICC_IAR0_EL1 */
	CMP		X3, #1020			/* IAR0 == 1020 || IAR0 == 1021 means */
	BEQ		.read_iar1        /* Interrupt is in Group 1 (IAR1)     */
	CMP		X3, #1021
	BEQ		.read_iar1
	MOV		W4, #0        		/* Group = 0 */
	B		.verify_spurious_1023

.read_iar1:
	MOV		W4, #1				/* Group = 1 */
	MRS		X3, s3_0_c12_c12_0 /* Using special encoding s3_0_c12_c12_0 for ICC_IAR1_EL1 */

	CMP		X3, #1020			/* IAR1 == 1020 || IAR1 == 1021 means */
	BEQ		.spurious_int     /* interrupt is spurious */
	CMP		X3, #1021
	BEQ		.spurious_int
.verify_spurious_1023:
	CMP		X3, #1023			/* IAR[01] == 1023 means interrupt is spurious */
	BEQ		.spurious_int

	MOV		W0, W3				/* ICCIAR in W0 as parameter. */

	/* Maintain the ICCIAR & Group across the function call. */
	STP		X0, X4, [SP, #-0x10]!

	/* Call the C handler. */
	BL vApplicationIRQHandler


	/* Disable interrupts. */
	MSR 	DAIFSET, #3
	DSB		SY
	ISB		SY

	/* Restore the ICCIAR & Group value. */
	LDP		X0, X4, [SP], #0x10


	/* End IRQ processing by writing ICCIAR to the EOI register. */
	CMP		X4, #0				/* Test for Group 0 */
	BNE		.acknowledge_g1
	MSR		s3_0_c12_c8_1, X0	/* Using special encoding s3_0_c12_c8_1 for 
                                 ICC_EOIR0_EL1 */
	B		.acknowledge_done
.acknowledge_g1:
	MSR		s3_0_c12_c12_1, X0	/* Using special encoding s3_0_c12_c12_1 for 
                                 ICC_EOIR1_EL1 */
.acknowledge_done:

	/* Restore the interrupt nesting count. */
	LDP		X1, X5, [SP], #0x10
	STR		X1, [X5]

	/* Has interrupt nesting unwound? */
	CMP		X1, #0
	B.NE	Exit_IRQ_No_Context_Switch

	/* Is a context switch required? */
	LDR		X0, ullPortYieldRequiredConst
	LDR		X1, [X0]
	CMP		X1, #0
	B.EQ	Exit_IRQ_No_Context_Switch

	/* Reset ullPortYieldRequired to 0. */
	MOV		X2, #0
	STR		X2, [X0]

	/* Restore volatile registers. */
	LDP 	X4, X5, [SP], #0x10  /* SPSR and ELR. */
	MSR		SPSR_EL3, X5 /*_RB_ Assumes started in EL3. */
	MSR		ELR_EL3, X4
	DSB		SY
	ISB		SY

	LDP		X29, X30, [SP], #0x10
	LDP		X18, X19, [SP], #0x10
	LDP		X16, X17, [SP], #0x10
	LDP		X14, X15, [SP], #0x10
	LDP		X12, X13, [SP], #0x10
	LDP		X10, X11, [SP], #0x10
	LDP		X8, X9, [SP], #0x10
	LDP		X6, X7, [SP], #0x10
	LDP		X4, X5, [SP], #0x10
	LDP		X2, X3, [SP], #0x10
	LDP		X0, X1, [SP], #0x10

	/* Save the context of the current task and select a new task to run. */
	portSAVE_CONTEXT
	BL vTaskSwitchContext
	portRESTORE_CONTEXT

Exit_IRQ_No_Context_Switch:
	/* Restore volatile registers. */
	LDP 	X4, X5, [SP], #0x10  /* SPSR and ELR. */
	MSR		SPSR_EL3, X5 /*_RB_ Assumes started in EL3. */
	MSR		ELR_EL3, X4
	DSB		SY
	ISB		SY

	LDP		X29, X30, [SP], #0x10
	LDP		X18, X19, [SP], #0x10
	LDP		X16, X17, [SP], #0x10
	LDP		X14, X15, [SP], #0x10
	LDP		X12, X13, [SP], #0x10
	LDP		X10, X11, [SP], #0x10
	LDP		X8, X9, [SP], #0x10
	LDP		X6, X7, [SP], #0x10
	LDP		X4, X5, [SP], #0x10
	LDP		X2, X3, [SP], #0x10
	LDP		X0, X1, [SP], #0x10

	ERET

.spurious_int:
	/* Restore the interrupt nesting count. */
	LDP		X1, X5, [SP], #0x10
	STR		X1, [X5]
	B			Exit_IRQ_No_Context_Switch


.align 8
pxCurrentTCBConst: .dword pxCurrentTCB
ullCriticalNestingConst: .dword ullCriticalNesting
ullPortTaskHasFPUContextConst: .dword ullPortTaskHasFPUContext

ullMaxAPIPriorityMaskConst: .dword ullMaxAPIPriorityMask
vApplicationIRQHandlerConst: .word vApplicationIRQHandler
ullPortInterruptNestingConst: .dword ullPortInterruptNesting
ullPortYieldRequiredConst: .dword ullPortYieldRequired



.end





