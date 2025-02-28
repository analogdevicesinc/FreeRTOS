/*
 * FreeRTOS Kernel V10.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* =============================================================================
 *
 *  Description: This file has processor specific options
 *
 * -----------------------------------------------------------------------------
 *  Comments:
 *
 * ===========================================================================*/
#include <Metrics.h>

//Processor Specific Options

#if defined(__ADSP219X__)
   unsigned long start_cycles;
   unsigned long timer_overhead;
   unsigned long cycles;
#else
   volatile int testid;
   long long start_cycles;
   long long  timer_overhead;
   long long  cycles;
   uint32_t  corefreq;
#endif

   uint32_t g_i = 0; /* for looping */

#if defined (__ADSPBF535__) || defined (__AD6900__)
   #include <cplb.h>
   #pragma retain_name
   // Turn on caching
   int __cplb_ctrl = CPLB_ENABLE_ANY_CPLBS |
                  CPLB_ENABLE_ICACHE |
                  CPLB_ENABLE_DCACHE;

   void metrics_init(void)
   {
      return;
   }

#elif !defined (__ADSPBF535__) && defined (__ADSPBLACKFIN__)
/* Initially just for BF527 and BF533 - this is being extended to
 * include all legacy blackfins for testing.
 */
   #include <services/pwr/adi_pwr.h>
   #include <sys\pll.h>
   #include <cplb.h>

   #define CCLKSPEED 594
   #define SCLKSPEED 118

   #if (CCLKSPEED == 513) && (SCLKSPEED == 128)
      #define SPEED_MULTIPLE 19
      #define BUS_DIVISOR    4
      #define DIVFREQ        0
   #elif (CCLKSPEED == 526) && (SCLKSPEED == 131)
      #define SPEED_MULTIPLE 39
      #define BUS_DIVISOR    4
      #define DIVFREQ        1
   #elif (CCLKSPEED == 594) && (SCLKSPEED == 118)
      #define SPEED_MULTIPLE 22
      #define BUS_DIVISOR    5
      #define DIVFREQ        0
   #elif (CCLKSPEED == 756) && (SCLKSPEED == 126)
      #define SPEED_MULTIPLE 28
      #define BUS_DIVISOR    6
      #define DIVFREQ        0
   #else
      #error CCLKSPEED and SCLKSPEED are not proper
   #endif

   #define CLKIN 27000000

   #ifdef CLOCKS_PER_SEC
      #undef CLOCKS_PER_SEC
   #endif

   #define CLOCKS_PER_SEC (SPEED_MULTIPLE *  CLKIN / (DIVFREQ+1))
   #define SCLOCKS_PER_SEC (CLOCKS_PER_SEC/BUS_DIVISOR)

   #define SDRAM_REFRESHRATE	(((SCLOCKS_PER_SEC / 1000) * 64) / 8192) - (6 + 3)

   void metrics_init(void)
   {
      //pll_set_system_vco(SPEED_MULTIPLE,DIVFREQ,0x300);
      //pll_set_system_clocks(0,BUS_DIVISOR);
      //*pEBIU_SDRRC = SDRAM_REFRESHRATE;
#if defined(__ADSPSC589_FAMILY__) || defined (__ADSPBF707_FAMILY__)
	   adi_pwr_GetCoreFreq(0, &corefreq);
#else
	   adi_pwr_GetCoreFreq(&corefreq);
#endif
      return;
   }
#elif defined(__ADSP219X__)

   void metrics_init(void)
   {
      // Switch on emu counter
      asm ("AX1 = ICNTL ; AR = AX1 OR  2048; ICNTL = AR ;NOP;" : : : "AR" , "AX1");
      return;
   }

#elif defined(__ADSP21160__) || defined (__ADSP21469__) || (defined(__ADSP215xx__) && !defined(__ADSPARM__))

    void metrics_init(void)
    {
       return;
    }
#elif defined(__ADSP21369__)  
    #include <def21369.h>
    #include <cdef21369.h>
    
    /********************************************************************************************
    * metrics_init the DSP PLL for the required CCLK and HCLK rates.
    * CLKIN will be 24.576 MHz from an external oscillator.  The PLL is programmed
    * to generate a core clock (CCLK) of 331.776 MHz - PLL multiplier = 27 and
    * divider = 2.
    *********************************************************************************************/
    void metrics_init(void)
    {
    
       int i, pmctlsetting;
    
       //Change this value to optimize the performance for quazi-sequential accesses (step > 1)
       #define SDMODIFY 1
    
       pmctlsetting= *pPMCTL;
       pmctlsetting &= ~(0xFF); //Clear
    
       // CLKIN= 24.576 MHz, Multiplier= 27, Divisor= 2, CCLK_SDCLK_RATIO 2.
       // Core clock = (24.576 MHz * 27) /2 = 331.776 MHz
       pmctlsetting= SDCKR2|PLLM27|PLLD2|DIVEN;
       *pPMCTL= pmctlsetting;
       pmctlsetting|= PLLBP;
       *pPMCTL= pmctlsetting;
    
       //Wait for around 4096 cycles for the pll to lock.
       for (i=0; i<4096; i++)
          asm("nop;");
    
       *pPMCTL ^= PLLBP;       //Clear Bypass Mode
       *pPMCTL |= (CLKOUTEN);  //and start clkout
    
    
       // Programming SDRAM control registers and enabling SDRAM read optimization
       // CCLK_SDCLK_RATIO= 2.5
       // RDIV = ((f SDCLK X t REF )/NRA) - (tRAS + tRP )
       // (166*(10^6)*64*(10^-3)/4096) - (7+3) = 2583
    
       *pSDRRC= (0xA17)|(SDMODIFY<<17)|SDROPT;
    
       //===================================================================
       //
       // Configure SDRAM Control Register (SDCTL) for PART MT48LC4M32B2
       //
       //  SDCL3  : SDRAM CAS Latency= 3 cycles
       //  DSDCLK1: Disable SDRAM Clock 1
       //  SDPSS  : Start SDRAM Power up Sequence
       //  SDCAW8 : SDRAM Bank Column Address Width= 8 bits
       //  SDRAW12: SDRAM Row Address Width= 12 bits
       //  SDTRAS7: SDRAM tRAS Specification. Active Command delay = 7 cycles
       //  SDTRP3 : SDRAM tRP Specification. Precharge delay = 3 cycles.
       //  SDTWR2 : SDRAM tWR Specification. tWR = 2 cycles.
       //  SDTRCD3: SDRAM tRCD Specification. tRCD = 3 cycles.
       //
       //--------------------------------------------------------------------
    
       *pSDCTL= SDCL3|DSDCLK1|SDPSS|SDCAW8|SDRAW12|SDTRAS7|SDTRP3|SDTWR2|SDTRCD3;
    
       // Note that MS2 & MS3 pin multiplexed with flag2 & flag3.
       // MSEN bit must be enabled to access SDRAM, but LED7 cannot be driven with sdram
       *pSYSCTL |=MSEN;
    
       // Mapping Bank 2 to SDRAM
       // Make sure that jumper is set appropriately so that MS2 is connected to
       // chip select of 16-bit SDRAM device
       *pEPCTL |=B2SD;
       *pEPCTL &= ~(B0SD|B1SD|B3SD);
    
       //===================================================================
       //
       // Configure AMI Control Register (AMICTL0) Bank 0 for the ISSI IS61LV5128
       //
       //  WS2 : Wait States = 2 cycles
       //  HC1  : Bus Hold Cycle (at end of write access)= 1 cycle.
       //  AMIEN: Enable AMI
       //  BW8  : External Data Bus Width= 8 bits.
       //
       //--------------------------------------------------------------------
    
       //SRAM Settings
       *pAMICTL0 = WS2|HC1|AMIEN|BW8;
    }
#elif defined(__ADSPTS201__)

    extern void system_func(void);

    void metrics_init(void)
    {
       system_func();
       return;
    }
#elif defined(__ADSPTS101__)

    void metrics_init(void)
    {
       return;
    }

#elif defined(__ADSPARM__)

    void metrics_init(void)
    {
       CCNTR_INIT;
       CCNTR_START;

       return;
    }

#endif




/*-----------------------------------------------------------*/
#if defined(__ADSPARM__)
    void vAssertCalled( const char * pcFile, unsigned long ulLine )
    {
     volatile unsigned long ul = 0;

         ( void ) pcFile;
         ( void ) ulLine;

        __asm volatile( "cpsid i" );
         while ( ul == 0 )
         {
             __asm volatile( "NOP" );
             __asm volatile( "NOP ");
         }
         __asm volatile( "cpsie i" );
    }

#elif defined(__ADSPBLACKFIN__)

    void vAssertCalled( const char * pcFile, unsigned long ulLine )
    {
     volatile unsigned long ul = 0;

         ( void ) pcFile;
         ( void ) ulLine;

        const uint32_t imask = cli();

        while ( ul == 0 )
        {
               NOP();
               NOP();
        }

        sti(imask);
    }

#elif defined(__ADSP215xx__)

    void vAssertCalled( const char * pcFile, unsigned long ulLine )
      {
       volatile unsigned long ul = 0;

           ( void ) pcFile;
           ( void ) ulLine;

          while ( ul == 0 )
          {
                 NOP();
                 NOP();
          }

      }

#endif


#ifdef USE_NO_IO
   long long g_idbuff[TESTBUFSIZE] = {0};
   long long g_cyclebuff[TESTBUFSIZE] = {0};
   char g_printbuff[40][PRINTBUFFSIZE];
   char *g_psString[40];
   int g_buffidx = 0;
#else
   int vdk_testid = 0;
#endif
