/* Copyright (c) 2021-2023, Analog Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*!
* @brief     Abstract page table for ADSP-SC598 family parts.
*
* @details
*            This is the table which defines the MMU attributes of memory.
*
*            The default distribution of L2 and L3 memory between the ARM and
*            the SHARC cores is chosen to give similar amounts of memory to each
*            core. However, the memory requirements in your application for the
*            ARM and SHARC cores may not be equal. In addition, the L3 memory
*            specified is that which is available on the ADSP-SC598 SOM board.
*
*            If you have a different amount of L3 memory, or change the
*            distribution of L2 or L3 memory between the cores, please note
*            that these changes must also be reflected in the LDF files for
*            your SHARC projects, and in the LD file used for your ARM project.
*/

#if !defined(__ADSPSC598_FAMILY__)
 #error Target not supported
#endif

#include <runtime/mmu/adi_mmu.h>

/* See adi_mmu.h for definitions of the ADI_MMU_* memory class macros. */

#ifndef ADI_UNCACHED_SHARC_MEMORY
#define SHARC_L2 ADI_MMU_RO_CACHED
#define SHARC_L3 ADI_MMU_RO_CACHED
#else
#define SHARC_L2 ADI_MMU_RO_UNCACHED
#define SHARC_L3 ADI_MMU_RO_UNCACHED
#endif

/* ADI_CHANGES is defined for the default page table objects to ensure the
 * default symbols are weak.  Users should not define this macro when building
 * their application-specific page table.
 */
#ifdef ADI_CHANGES
const adi_mmu_AbstractPageEntry _adi_mmu_absPageTable[] __attribute__ ((weak)) =
#else
const adi_mmu_AbstractPageEntry _adi_mmu_absPageTable[] =
#endif
{
    /* THE ADDRESS RANGES IN THIS TABLE MUST BE IN ASCENDING ORDER. */

    /* L2 ROM */
    { 0x00000000u, 0x0000FFFFu, ADI_MMU_RO_CACHED           }, /* 64KB L2 ROM */

    /* L2 Cache Controller MMR space */
    { 0x10000000u, 0x10000FFFu, ADI_MMU_RW_DEVICE           }, /* 4KB L2CC MMRs */

    /* L2 SRAM */
#if defined(__ADSPSC595__) || defined(__ADSPSC595W__)
    { 0x20100000u, 0x20100FFFu, ADI_MMU_RW_UNCACHED         }, /* 4KB ICC */
    { 0x20101000u, 0x20104FFFu, ADI_MMU_RW_UNCACHED         }, /* 16KB MCAPI ARM */
    { 0x20105000u, 0x2010FFFFu, ADI_MMU_RO_UNCACHED         }, /* 44KB MCAPI SHARC */
    { 0x20110000u, 0x2011FFFFu, ADI_MMU_RW_UNCACHED         }, /* 64KB ARM uncached L2 */
    { 0x20120000u, 0x2018FFFFu, ADI_MMU_WB_CACHED           }, /* 448KB ARM cached L2 */
    { 0x20190000u, 0x201FFFFFu, SHARC_L2                    }, /* 440KB SHARC & 8KB boot code working area */
#else
    { 0x20000000u, 0x20000FFFu, ADI_MMU_RW_UNCACHED         }, /* 4KB ICC */
    { 0x20001000u, 0x20004FFFu, ADI_MMU_RW_UNCACHED         }, /* 16KB MCAPI ARM */
 #if defined(__ADSPSC596__) || defined(__ADSPSC596W__)
    { 0x20005000u, 0x2001FFFFu, ADI_MMU_RO_UNCACHED         }, /* 108KB MCAPI SHARC */
    { 0x20020000u, 0x2003FFFFu, ADI_MMU_RW_UNCACHED         }, /* 128KB ARM uncached L2 */
 #elif defined(__ADSPSC598__) || defined(__ADSPSC598W__)
    { 0x20005000u, 0x20024FFFu, ADI_MMU_RO_UNCACHED         }, /* 128KB MCAPI SHARC0 and SHARC1 */
    { 0x20025000u, 0x2003FFFFu, ADI_MMU_RW_UNCACHED         }, /* 108KB ARM uncached L2 */
 #else
  #error Target not supported
 #endif
    { 0x20040000u, 0x200FFFFFu, ADI_MMU_WB_CACHED           }, /* 768KB ARM cached L2 */
    { 0x20100000u, 0x201FFFFFu, SHARC_L2                    }, /* 1016KB SHARC & 8KB boot code working area */
#endif

    { 0x24000000u, 0x24001FFFu, ADI_MMU_RW_DEVICE           }, /* 8KB OTP space */

    /* L1 memory of SHARC0 in MP space */
    { 0x28240000u, 0x2826FFFFu, ADI_MMU_RO_UNCACHED         }, /* 192KB SHARC0 L1B0 */
    { 0x282C0000u, 0x282EFFFFu, ADI_MMU_RO_UNCACHED         }, /* 192KB SHARC0 L1B1 */
    { 0x28300000u, 0x2831FFFFu, ADI_MMU_RO_UNCACHED         }, /* 128KB SHARC0 L1B2 */
    { 0x28380000u, 0x2839FFFFu, ADI_MMU_RO_UNCACHED         }, /* 128KB SHARC0 L1B3 */

#if __NUM_SHARC_CORES__ > 1
    /* L1 memory of SHARC1 in MP space */
    { 0x28A40000u, 0x28A6FFFFu, ADI_MMU_RO_UNCACHED         }, /* 192KB SHARC1 L1B0 */
    { 0x28AC0000u, 0x28AEFFFFu, ADI_MMU_RO_UNCACHED         }, /* 192KB SHARC1 L1B1 */
    { 0x28B00000u, 0x28B1FFFFu, ADI_MMU_RO_UNCACHED         }, /* 128KB SHARC1 L1B2 */
    { 0x28B80000u, 0x28B9FFFFu, ADI_MMU_RO_UNCACHED         }, /* 128KB SHARC1 L1B3 */
#endif /* __NUM_SHARC_CORES__ */

    /* System MMR space */
    { 0x30000000u, 0x3FFFFFFFu, ADI_MMU_RW_DEVICE           }, /* 256MB System MMRs */

    /* SPI Flash memory-mapped address space.
     * By default we cover the 64MB Quad SPI on the ADSP-SC598 SOM board, rather
     * than the 128MB OSPI on the EZ-KIT SOM carrier board.
     */
    { 0x60000000u, 0x63FFFFFFu, ADI_MMU_RW_DEVICE           }, /* 64MB SPI flash */

    /* Dynamic Memory Controller 0 (DMC0) 512MB SDRAM */
    { 0x80000000u, 0x8FFFFFFFu, SHARC_L3                    }, /* 256MB DDR-A */
    { 0x90000000u, 0x9FFFFFFFu, ADI_MMU_WB_CACHED           }, /* 256MB DDR-A */
};

/* ADI_CHANGES is defined for the default page table objects to ensure the
 * default symbols are weak.  Users should not define this macro when building
 * their application-specific page table.
 */
#ifdef ADI_CHANGES
const uint32_t _adi_mmu_absPageTableSize __attribute__ ((weak)) = sizeof(_adi_mmu_absPageTable) / sizeof(_adi_mmu_absPageTable[0]);
#else
const uint32_t _adi_mmu_absPageTableSize = sizeof(_adi_mmu_absPageTable) / sizeof(_adi_mmu_absPageTable[0]);
#endif
