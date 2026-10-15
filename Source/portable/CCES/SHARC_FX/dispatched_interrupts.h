/************************************************************************
 * dispatched_interrupts.h
 *
 *  Header detailing configuration of low-level interrupt dispatcher
 *  and associated functions.
 *
 * (c) Copyright 2020-2025 Analog Devices, Inc.  All rights reserved.
 ************************************************************************/

#ifndef _DISPATCHED_INTERRUPTS_H
#define _DISPATCHED_INTERRUPTS_H

#include <interrupt.h>
#include <sys/platform.h>

#ifdef __ADSPSC835_FAMILY__
#ifdef __XTENSA__
#define NUM_SYS_INTS (326U)       /* greater than 256 due to various
                                     error interrupts sharing one SID */
#define USING_CHIDS (251U)        /* SID+CHID table offset needed */
#define NUM_SIDS_USING_CHIDS (5U) /* size for __sys_int_chid_table */
#define NUM_CORE_INTS ((uint32_t)XCHAL_NUM_INTERRUPTS) /* Number of handled core interrupts */
#define NUM_EXCEPTIONS (64U) /* Total number of populated exception IDs */
/* Core interrupts, exceptions, and system interrupts each use an entry in
 * the dispatcher table.
 */
#define NUM_TABLE_ENTRIES (NUM_CORE_INTS + NUM_SYS_INTS + NUM_EXCEPTIONS)
#else
#define NUM_SYS_INTS (0)    /* ARM core does not use SEC */
#define NUM_CORE_INTS (326) /* Number of core interrupts on M33 */
/* Core exceptions and interrupts each use an entry in the dispatcher table.
 */
#define NUM_TABLE_ENTRIES (NVIC_USER_IRQ_OFFSET + NUM_CORE_INTS)
#endif

#elif defined __ADSPSC846_FAMILY__
#ifdef __XTENSA__
#define NUM_SYS_INTS (241U)
#define NUM_CORE_INTS ((uint32_t)XCHAL_NUM_INTERRUPTS) /* Number of handled core interrupts */
#define NUM_EXCEPTIONS (64U) /* Total number of populated exception IDs */
/* Core interrupts, exceptions, and system interrupts each use an entry in
 * the dispatcher table.
 */
#define NUM_TABLE_ENTRIES (NUM_CORE_INTS + NUM_SYS_INTS + NUM_EXCEPTIONS)
#else
#error This header is only for use with the Xtensa core on ADSP-SC84x family parts.
#endif
#else
#error Please configure dispatched_interrupts.h for your platform.
#endif

#ifdef __XTENSA__

/* Define macros for FCR bit masks.
*/
#define BITM_FCR_FP_INV_OP      (1U << 6)
#define BITM_FCR_FP_OVERFLOW    (1U << 4)
#define BITM_FCR_FP_UNDERFLOW   (1U << 3)
#define BITM_FCR_FP_DIV_BY_ZERO (1U << 5)
#define BITM_FCR_FP_INEXACT     (1U << 2)

#endif /* __XTENSA__ */


#include <stdint.h>

typedef struct {
    adi_dispatched_handler_t  handler;
    adi_dispatched_callback_t callback;
} adi_dispatched_data_t;

extern adi_dispatched_data_t __adi_dispatched_int_vector_table[NUM_TABLE_ENTRIES];

#ifdef __XTENSA__
extern int32_t __find_exception_id_index(uint32_t eid);

/* Macro for base of core interrupt table.
 */
#define __adi_dispatched_core_int_vector_table (&__adi_dispatched_int_vector_table[0])

/* Macro for base of SEC interrupt table.
 */
#define __adi_dispatched_sec_int_vector_table (&__adi_dispatched_int_vector_table[NUM_CORE_INTS])

/* Macro for base of exception table.
 */
#define __adi_dispatched_exception_vector_table (&__adi_dispatched_int_vector_table[NUM_CORE_INTS + NUM_SYS_INTS])

/* Table to record the table index associated with the sparse exception number.
 */
extern uint16_t __adi_dispatched_exception_mapping[];

/* Table to help translation from SECID + CHID to the raw interrupt ID.
 */
extern const uint8_t __sys_int_chid_table[];
#else
/* Macro for base of core interrupt table.
 */
#define __adi_dispatched_core_int_vector_table (&__adi_dispatched_int_vector_table[NVIC_USER_IRQ_OFFSET])

/* Macro for base of exception table.
 */
#define __adi_dispatched_exception_vector_table (&__adi_dispatched_int_vector_table[0])
#endif

#define DEFAULT_HANDLER ((adi_dispatched_handler_t) &adi_no_dispatched_handler)

#endif /* _DISPATCHED_INTERRUPTS_H */
