
/*
** app_heap_stack.c generated on December 16, 2021 at 13:29:15.
**
** Copyright (C) 2021 Analog Devices Inc., All Rights Reserved.
**
** This file is generated automatically based upon the options selected in the
** Heap and Stack Configuration Editor. Changes to the heap and stack
** configuration should be made by changing the appropriate options rather than
** editing this file.
**
*/

#include <stdint.h>

#define __ADI_B  (1u)
#define __ADI_kB (1024u)
#define __ADI_MB (1024u * 1024u)

#define __ADI_SYSTEM_HEAP_SIZE (4u)
#define __ADI_SYSTEM_HEAP_SIZE_UNITS __ADI_MB
#define __ADI_SYSTEM_HEAP_ALIGN (4u)
#define __ADI_EL0_STACK_SIZE (4u)
#define __ADI_EL0_STACK_SIZE_UNITS __ADI_kB
#define __ADI_EL0_STACK_ALIGN (64u)
#define __ADI_EL3_STACK_SIZE (4u)
#define __ADI_EL3_STACK_SIZE_UNITS __ADI_kB
#define __ADI_EL3_STACK_ALIGN (64u)

/* The object used for the system heap. */
uint8_t __adi_heap_object[__ADI_SYSTEM_HEAP_SIZE * __ADI_SYSTEM_HEAP_SIZE_UNITS]
             __attribute__ ((aligned (__ADI_SYSTEM_HEAP_ALIGN))) 
             __attribute__ ((section (".heap,\"aw\",%nobits //")));

/* The object used for the EL0 stack. */
uint8_t __adi_stack_el0_object[__ADI_EL0_STACK_SIZE * __ADI_EL0_STACK_SIZE_UNITS]
             __attribute__ ((aligned (__ADI_EL0_STACK_ALIGN))) 
             __attribute__ ((section (".stack_el0,\"aw\",%nobits //")));

/* The object used for the EL3 stack. */
uint8_t __adi_stack_el3_object[__ADI_EL3_STACK_SIZE * __ADI_EL3_STACK_SIZE_UNITS]
             __attribute__ ((aligned (__ADI_EL3_STACK_ALIGN))) 
             __attribute__ ((section (".stack_el3,\"aw\",%nobits //")));


