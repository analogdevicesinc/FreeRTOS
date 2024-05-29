/*
 * Copyright (C) 2016-2021 Analog Devices Inc. All Rights Reserved.
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
 */

#include <runtime/int/interrupt.h>
#include <sys/platform.h>
#include <stdlib.h>
#include "FreeRTOS.h"

/* Dispatched interrupt vector table, defined by CCES runtime */
extern adi_dispatched_data_t adi_dispatched_int_vector_table[ADI_DISPATCHED_VECTOR_TABLE_SIZE];

/* vApplicationIRQHandler() is just a normal C function.
 * - the argument iid is the value which was read from the interrupt acknowledge register
 *   We don't need to mask the iid register as all other fields will be zero'd.
 */
void vApplicationIRQHandler( uint32_t iid )
{
    /* Re-enable interrupts, which were disabled on entry to the dispatcher (FreeRTOS_IRQ_Handler) */
    portENABLE_INTERRUPTS();

	/* Convert the interrupt ID into a index into the ADI_RTL interrupt vector table.
 	 * Currently just a straight assignment but kept for futureproofing.
 	*/
	const uint32_t idx = ADI_RTL_IID_TO_INDEX(iid);

	/* Call the interrupt handler, as a plain C function, passing the interrupt ID and the
	 * user-provided parameter as arguments.
	 */
	(*adi_dispatched_int_vector_table[idx].handler)(iid, (adi_dispatched_callback_arg_t)(adi_dispatched_int_vector_table[idx].callback_arg));
}
