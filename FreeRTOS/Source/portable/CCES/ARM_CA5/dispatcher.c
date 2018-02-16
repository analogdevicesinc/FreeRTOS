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
 * dispatcher.c
 *
 * Contains support function(s) to connect the FreeRTOS GIC interrupt dispatcher (in portASM.S) to the CCES
 * C runtime dispatched interrupt vector table.
 *
 *  Created on: Feb 14, 2017
 */

#include <runtime/int/interrupt.h>
#include <sys/platform.h>
#include <stdlib.h>

/* Dispatched interrupt vector table, defined by CCES runtime */
extern adi_dispatched_data_t adi_dispatched_int_vector_table[ADI_DISPATCHED_VECTOR_TABLE_SIZE];

/* vApplicationIRQHandler() is just a normal C function.
 * - the argument ulICCIAR is the value which was read from the interrupt acknowledge register
 */
void vApplicationIRQHandler( uint32_t ulICCIAR )
{
    /* Re-enable interrupts, which were disabled on entry to the dispatcher (FreeRTOS_IRQ_Handler) */
    __asm volatile( "CPSIE I" );

    /* Convert the hardware-supplied interrupt number to an IID by masking out everything except the interrupt ID.
     * Since the only other field in the INT_ACK register is the CPU ID, this step is optional on platforms where
     * the CPU ID will always be zero.
     *
     * (runtime/int/interrupt.h should really have an ADI_RTL_INTR_IID() macro to do this, but it doesn't as yet.)
     */
    const uint32_t iid = ulICCIAR & ADI_RTL_INTID_MASK;

	/* Check for the "spurious interrupt" IDs (0x3FF and 0x3FE). These are returned by the GIC if there is nothing
	 * to be done, e.g. if the interrupt has already been acknowledged by another core (0x3FE used in TrustZone).
	 * We can check for both spurious interrupts *and* for overrun of the interrupt table, simple by checking
	 * against the interrupt table size since this will always be <= 0x3FE.
	 */
	if (iid < ADI_DISPATCHED_VECTOR_TABLE_SIZE)
	{
		/* Convert the interrupt ID into a index into the ADI_RTL interrupt vector table */
		const uint32_t idx = ADI_RTL_IID_TO_INDEX(iid);

		/* Call the interrupt handler, as a plain C function, passing the interrupt ID and the
		 * user-provided parameter as arguments.
		 */
		(*adi_dispatched_int_vector_table[idx].handler)(iid, (adi_dispatched_callback_arg_t)(adi_dispatched_int_vector_table[idx].callback_arg));
	}
}
