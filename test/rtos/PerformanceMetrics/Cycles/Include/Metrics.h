#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* This file defines a set of macros to be used for timing metrics.
 *
 * The following macros should be used in your code:
 * 	ESTABLISH_TIMER_OVERHEAD    - To be called once.
 * 	TEST_ID(<number>)           - A unique identifier for this explicit metric.
 * 	START_TIMER()               - Start the timing.
 * 	<insert operation to time here>
 * 	STOP_TIMER()                - Stop the timing (can be in a different thread, for a blocking call).
 *
 *  PRINT_TEST(<test number>, "<test description>">
 *  Will print the test metric, in cycles, and also some text to describe the test.
 */

/* Test Specific Options */


/* USE_NO_IO needs to be defined in order to remove effects of
   printing on the test */
#define USE_NO_IO

/* SIZE_STACK needs to be defined in order to calculate the
   stack used by thread 1 */
//#define SIZE_STACK

/* REPEATING needs to be defined in order to get
   caching affected numbers else comment it out */
#define REPEATING

#ifdef REPEATING
   #define ITERATIONS 2
#else
   #define ITERATIONS 1
#endif

   #define TESTBUFSIZE 40*ITERATIONS
   #define PRINTBUFFSIZE 100
   extern long long g_idbuff[TESTBUFSIZE];
   extern long long g_cyclebuff[TESTBUFSIZE];
   extern char g_printbuff[40][PRINTBUFFSIZE];
   extern char *g_psString[40];
   extern int g_buffidx;

   /* For looping */
   extern    uint32_t g_i;

/* Processor Specific Options */
#if defined(__ADSP219X__)
   extern unsigned long start_cycles;
   extern unsigned long timer_overhead;
   extern unsigned long cycles;
#else
   extern long long start_cycles;
   extern long long timer_overhead;
   extern long long cycles;
   extern volatile int testid;
   extern uint32_t  corefreq;
#endif

void metrics_init(void);

#if defined(__ADSPBLACKFIN__)
	#include <blackfin.h>
    #include <time.h>
    #include <services/int/adi_int.h>

    #define DISABLE_TIMER_INTERRUPT adi_int_EnableInt(ADI_CID_IVTMR, false);

/* Use clock() */
#define CLOCK_READ clock()

#elif defined(__ADSP219X__)

    static inline unsigned long clock(void)
    {
       union { unsigned long l; unsigned int i[2]; } u;
       // Get the emu counter values
       asm(" %0 = REG (99); %1 = REG (98);" : "=e" (u.i[0]), "=e" (u.i[1]) );
       return (u.l);
    }

    #define DISABLE_TIMER_INTERRUPT adi_int_EnableInt(ADI_CID_TMZHI, false)
    //#define HEAPID 0 - does not work

    #define CLOCK_READ clock()

#elif defined(__ADSP21160__) || defined(__ADSP21369__) || defined (__ADSP21469__) || (defined(__ADSP215xx__) && !defined(__ADSPARM__))

    static inline int clock(void)
    {
       int c;
       asm volatile ("%0=emuclk;" : "=r" (c));
       return c;
    }

    #define DISABLE_TIMER_INTERRUPT adi_int_EnableInt(ADI_CID_TMZHI, false)
    #define HEAPID ksystem_heap

#define CLOCK_READ clock()

#elif defined (__ADSPARM__)
#include <adi/cycle_count.h>

#define CLOCK_READ CCNTR_READ

#define WARM_CACHE_FIRST

#else
    #define DISABLE_TIMER_INTERRUPT
#endif

#define ESTABLISH_TIMER_OVERHEAD               \
    metrics_init();                            \
    start_cycles = CLOCK_READ;               \
    timer_overhead = CLOCK_READ - start_cycles;\
    start_cycles = CLOCK_READ;               \
    timer_overhead = CLOCK_READ -start_cycles;


   #define TEST_ID(XXX) testid = XXX;

   #define START_TIMER  start_cycles = CLOCK_READ;

   #define STOP_TIMER           \
      cycles = CLOCK_READ;      \
      g_cyclebuff[testid]       \
          = (cycles-start_cycles)-timer_overhead;


#if 1 || defined(__ADSPBLACKFIN__)
   #define PRINT_THRUPUT(_ID, _NAME, _SIZE)  printf("METRIC-THRU: %s\t%lld\t%f\t%f Mb/s\r\n", _NAME, g_cyclebuff[_ID], (float)g_cyclebuff[_ID]/(float)corefreq,  1/(((float)1048576/(float)(_SIZE)) * ((float)g_cyclebuff[_ID]/(float)corefreq)));
   #define PRINT_TEST(_ID, _NAME)  printf("METRIC-CYC: %s:\t%lld\r\n", _NAME, g_cyclebuff[_ID]);
#else
   #define PRINT_THRUPUT(_ID, _NAME, _SIZE)  printf("METRIC-THRU: %s\t%lld\t%f\t%f Mb/s\r\n", _NAME, g_cyclebuff[_ID], (float)g_cyclebuff[_ID]/(float)corefreq,  1/(((float)1048576/(float)(_SIZE)) * ((float)g_cyclebuff[_ID]/(float)corefreq)));
   #define PRINT_TEST(_ID, _NAME)  snprintf(g_psString[_ID] = g_printbuff[_ID], PRINTBUFFSIZE-1, "METRIC-CYC: %s:\t%lld\r\n", _NAME, g_cyclebuff[_ID]);
#endif

#ifdef SIZE_STACK
    /* This might be useful later - currently unused. */
#else
   #define INSTRUMENT_STACK
   #define REPORT_STACKSIZE
#endif

#if defined(__ADSPBLACKFIN__)
/* Use the IVG10 core interrupt for this metric. */
#define INT_ID  ADI_CID_IVG10
#define INT_TRIGGER  asm volatile("RAISE 0xA;")
#elif defined(__ADSP215xx__) && !defined(__ADSPARM__) /* defined(SHARC+) */
#include <services/int/adi_int.h>
#include <services/int/adi_int.h>
#include <processor_include.h>
#define INT_ID  ADI_CID_SFT0I
#define INT_TRIGGER  asm volatile("#include <interrupt.h>"); \
		             asm volatile("BIT SET IRPTL (1 << (ADI_CID_SFT0I >> 24));")


#elif defined(__ADSPSHARC__) /* defined(SHARC) */
/* Use the IRQ0I core interrupt for this metric. */
#include <services/int/adi_int.h>
#include <processor_include.h>
#define INT_ID  ADI_CID_IRQ0I
#define INT_TRIGGER  asm volatile("#include <interrupt.h>"); \
		             asm volatile("BIT SET IRPTL (1 << (ADI_CID_IRQ0I >> 24));")
#else
#include <services/int/adi_gic.h>
/* Use a GIC SW interrupt */
#define INT_ID  (INTR_SOFT0)
#define INT_TRIGGER adi_gic_SetPendInt(INT_ID)

#endif




