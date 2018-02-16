
.file_attr ISR;
.file_attr OS_Internals;

#include <sys/defBF707.h>

.import "portASM.h";

.extern _vTaskSwitchContext;
.extern _pxCurrentTCB;

.global _xPortIVG14Handler;
.global _prvPortStartFirstTask;

.section program;


_prvPortStartFirstTask:
	/* On BF7xx, tasks run in user mode but with supervisor privilege. This
	 * is enabled by setting the SACC bit in the SYSCFG register.
	 */
	R0 = SYSCFG ;
	BITSET (R0, BITP_SYSCFG_SACC) ;
	SYSCFG = R0 ; /* Set SACC bit */
	
	/* ToDo: reset the supervisor SP to the base of the system stack,
	 * as it will only be used for interrupts and exceptions from this point
	 * so any space currently being used on the stack will be wasted. */
	 
	JUMP RestoreTaskContext;
	
/*
 * FreeRTOS_IVG14_Handler:
 *
 * This is the service routine for the kernel interrupt. It is the handler
 * for all rescheduling activity (pre-emptive and non-preeemptive) in the system.
 */
_xPortIVG14Handler:
	// Push some registers onto the system stack so we can clobber them
	[--SP] = P0;    // P0 is 1st on system stack
	[--SP] = R0;    // R0 is 2nd on system stack
    [--SP] = ASTAT; // ASTAT is 3rd on system stack

	// Get the user-mode stack pointer into P0
	P0 = USP;

	// To support self-nested interrupts (i.e. SYSGFG->SNEN = 1) we need to
	// explicitly mask out the reschedule interrupt, as the hardware won't hold
	// it off during execution of the ISR. We have to do this before saving RETI.
	// i.e. while interrupts are still globally disabled.
	//
	CLI R0;
	BITCLR (R0, 14); // mask out IVG14
	STI R0;	

	// Move P0 downwards in memory to allocate a context record on the thread stack,
	// working around the +63..-64 range limitation on immediate adds.
	P0 += -64;
	P0 += -64;
	P0 += -(SIZEOF(ContextRecord) - 128);

	// Move the thread's saved R0, P0, and ASTAT from the system stack into the context record
	R0 = [SP++]; [P0 + OFFSETOF(ContextRecord, _ASTAT)] = R0; // ASTAT is 3rd on system stack
	R0 = [SP++]; [P0 + OFFSETOF(ContextRecord, _R0)] = R0;    // R0 is 2nd on system stack
	R0 = [SP++]; [P0 + OFFSETOF(ContextRecord, _P0)] = R0;    // P0 is 1st on system stack

    // Save off the loop registers
	R0 = LC0; [P0 + OFFSETOF(ContextRecord, _LC0)] = R0;
	R0 = LT0; [P0 + OFFSETOF(ContextRecord, _LT0)] = R0;
	R0 = LB0; [P0 + OFFSETOF(ContextRecord, _LB0)] = R0;
	R0 = LC1; [P0 + OFFSETOF(ContextRecord, _LC1)] = R0;
	R0 = LT1; [P0 + OFFSETOF(ContextRecord, _LT1)] = R0;
	R0 = LB1; [P0 + OFFSETOF(ContextRecord, _LB1)] = R0;

    // Reset the loop counters. We have to do this before re-enabling interrupts,
	// due to anomaly 05-00-0312.
	R0 = 0;
    LC0 = R0;
    LC1 = R0;

    // Save off the RETI register
    [--SP] = RETI; //  re-enables nested interrupts
	R0 = [SP++];   // pop the interrupt return address from the system stack
	[P0 + OFFSETOF(ContextRecord, _PC)] = R0; // store the interrupt return address in the user stack frame

    // Save the subroutine return address register
	R0 = RETS; [P0 + OFFSETOF(ContextRecord, _RETS)] = R0;

	// Store the thread's R1:7 in the user stack frame
	[P0 + OFFSETOF(ContextRecord, _R1)] = R1;
	[P0 + OFFSETOF(ContextRecord, _R2)] = R2;
	[P0 + OFFSETOF(ContextRecord, _R3)] = R3;
	[P0 + OFFSETOF(ContextRecord, _R4)] = R4;
	[P0 + OFFSETOF(ContextRecord, _R5)] = R5;
	[P0 + OFFSETOF(ContextRecord, _R6)] = R6;
	[P0 + OFFSETOF(ContextRecord, _R7)] = R7;

	// Store the thread's P1:6 in the user stack frame
	[P0 + OFFSETOF(ContextRecord, _P1)] = P1;
	[P0 + OFFSETOF(ContextRecord, _P2)] = P2;
	[P0 + OFFSETOF(ContextRecord, _P3)] = P3;
	[P0 + OFFSETOF(ContextRecord, _P4)] = P4;
	[P0 + OFFSETOF(ContextRecord, _P5)] = P5;
	[P0 + OFFSETOF(ContextRecord, _FP)] = FP;    // Save the stack frame pointer

    // Save the DSP registers
	R0 = M0; [P0 + OFFSETOF(ContextRecord, _M0)] = R0;
	R0 = M1; [P0 + OFFSETOF(ContextRecord, _M1)] = R0;
	R0 = M2; [P0 + OFFSETOF(ContextRecord, _M2)] = R0;
	R0 = M3; [P0 + OFFSETOF(ContextRecord, _M3)] = R0;
	R0 = L0; [P0 + OFFSETOF(ContextRecord, _L0)] = R0;
	R0 = L1; [P0 + OFFSETOF(ContextRecord, _L1)] = R0;
	R0 = L2; [P0 + OFFSETOF(ContextRecord, _L2)] = R0;
	R0 = L3; [P0 + OFFSETOF(ContextRecord, _L3)] = R0;
	R0 = B0; [P0 + OFFSETOF(ContextRecord, _B0)] = R0;
	R0 = B1; [P0 + OFFSETOF(ContextRecord, _B1)] = R0;
	R0 = B2; [P0 + OFFSETOF(ContextRecord, _B2)] = R0;
	R0 = B3; [P0 + OFFSETOF(ContextRecord, _B3)] = R0;
	R0 = I0; [P0 + OFFSETOF(ContextRecord, _I0)] = R0;
	R0 = I1; [P0 + OFFSETOF(ContextRecord, _I1)] = R0;
	R0 = I2; [P0 + OFFSETOF(ContextRecord, _I2)] = R0;
	R0 = I3; [P0 + OFFSETOF(ContextRecord, _I3)] = R0;
	R0 = A0.W; [P0 + OFFSETOF(ContextRecord, _A0W)] = R0;
	R0 = A0.X; [P0 + OFFSETOF(ContextRecord, _A0X)] = R0;
	R0 = A1.W; [P0 + OFFSETOF(ContextRecord, _A1W)] = R0;
	R0 = A1.X; [P0 + OFFSETOF(ContextRecord, _A1X)] = R0;

	// Store the thread stack pointer in the thread structure
	P5 = [_pxCurrentTCB]; // Get the location of the current TCB
    [P5] = P0; // Save the new top of stack into the first member of the TCB

    // We've saved off the context, we need to setup the C runtime, and
    // then call the scheduler

	R0 = 0;
	L0 = R0;	// L0-L3 define the lengths of the DAG's circular buffers. They must be
	L1 = R0;	// set to zero when running compiled code, as the compiler uses the DAG
	L2 = R0;	// registers in non-circular buffer mode. (From C/C++ Compiler Manual).
	L3 = R0;	// R0 was set to zero above.
    
    FP = R0;    // terminate debugger stack walk (expects zero in R0)
	SP += -12;  // allocate argument stack space (may not be necessary)

	// Call the FreeRTOS scheduler
	call _vTaskSwitchContext;
	
	SP += 12;  // deallocate argument stack space

RestoreTaskContext:
    // Load the stack pointer for the (possibly new) thread.
	// The context is saved in stack top.
	P5 = [_pxCurrentTCB]; // Get the location of the current TCB
    P0 = [P5]; // The first item in pxCurrentTCB is the task top of stack

    // Restore the thread's R1:7 from the user stack frame
	R1 = [P0 + OFFSETOF(ContextRecord, _R1)];
	R2 = [P0 + OFFSETOF(ContextRecord, _R2)];
	R3 = [P0 + OFFSETOF(ContextRecord, _R3)];
	R4 = [P0 + OFFSETOF(ContextRecord, _R4)];
	R5 = [P0 + OFFSETOF(ContextRecord, _R5)];
	R6 = [P0 + OFFSETOF(ContextRecord, _R6)];
	R7 = [P0 + OFFSETOF(ContextRecord, _R7)];
		
	// Restore the thread's P1:6 from the user stack frame
	P1 = [P0 + OFFSETOF(ContextRecord, _P1)];
	P2 = [P0 + OFFSETOF(ContextRecord, _P2)];
	P3 = [P0 + OFFSETOF(ContextRecord, _P3)];
	P4 = [P0 + OFFSETOF(ContextRecord, _P4)];
	P5 = [P0 + OFFSETOF(ContextRecord, _P5)];
	FP = [P0 + OFFSETOF(ContextRecord, _FP)];    // restore the stack frame pointer

    // Save the DSP registers
	R0 = [P0 + OFFSETOF(ContextRecord, _M0)]; M0 = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _M1)]; M1 = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _M2)]; M2 = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _M3)]; M3 = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _L0)]; L0 = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _L1)]; L1 = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _L2)]; L2 = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _L3)]; L3 = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _B0)]; B0 = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _B1)]; B1 = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _B2)]; B2 = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _B3)]; B3 = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _I0)]; I0 = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _I1)]; I1 = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _I2)]; I2 = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _I3)]; I3 = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _A0W)]; A0.W = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _A0X)]; A0.X = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _A1W)]; A1.W = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _A1X)]; A1.X = R0;

    // Restore the subroutine return address register
	R0 = [P0 + OFFSETOF(ContextRecord, _RETS)]; RETS = R0;

    // Load the return-from-interrupt address into the RETI register, via the system stack,
    // as we are going to use the RTI instruction to get back to runtime level.
	R0 = [P0 + OFFSETOF(ContextRecord, _PC)]; [--SP] = R0; // move the thread PC from the context record onto the system stack, via R0
    RETI = [SP++];   // pop the thread PC from the system stack into RETI, disabling interrupts

    // Restore the loop registers. We must do this with interrupts disabled (anomaly 05-00-0312)
	R0 = [P0 + OFFSETOF(ContextRecord, _LC0)]; LC0 = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _LT0)]; LT0 = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _LB0)]; LB0 = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _LC1)]; LC1 = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _LT1)]; LT1 = R0;
	R0 = [P0 + OFFSETOF(ContextRecord, _LB1)]; LB1 = R0;

	// Move the thread's saved R0, P0, and ASTAT from the context record onto the system stack
	R0 = [P0 + OFFSETOF(ContextRecord, _P0)]; [--SP] = R0;    // P0 is 1st on system stack
	R0 = [P0 + OFFSETOF(ContextRecord, _R0)]; [--SP] = R0;    // R0 is 2nd on system stack
	R0 = [P0 + OFFSETOF(ContextRecord, _ASTAT)]; [--SP] = R0; // ASTAT is 3rd on system stack

	// Move P0 upwards in memory to deallocate the context record on the thread stack,
	// working around the +63..-64 range limitation on immediate adds.
	P0 += 60;
	P0 += 60;
	P0 += (SIZEOF(ContextRecord) - 120);

	USP = P0; // restore the (adusted) thread stack pointer

	// For self-nesting support, we cleared the IVG14 bit in IMASK on entry to
	// the reschedule ISR so we have to set it here. This has to be done between
	// the reload of RETI and the RTI. We know that the IVG14 bit was originally
	// set, since otherwise the reschedule ISR wouldn't have been entered.
	//
	CLI R0;
	BITSET (R0, 14); // unmask IVG14
	STI R0;

    // Restore ASTAT and the working data and address registers
    ASTAT = [SP++]; // ASTAT is 3rd on system stack
    R0 = [SP++];    // R0 is 2nd on system stack
    P0 = [SP++];    // P0 is 1st on system stack

    // We're done, resume the thread by returning from the IVG14 interrupt (or IVG15 if thisi s the first thread).
    RTI;
    
._prvPortStartFirstTask.end:
._xPortIVG14Handler.end:

