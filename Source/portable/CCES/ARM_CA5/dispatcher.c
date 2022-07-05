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
