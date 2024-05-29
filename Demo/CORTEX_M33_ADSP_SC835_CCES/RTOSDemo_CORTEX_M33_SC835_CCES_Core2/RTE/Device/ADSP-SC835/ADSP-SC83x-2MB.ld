/*******************************************************************************
 * @file     ADSP-SC83x-2MB.ld
 * @brief    GNU Linker Script for ADSP-SC83x devices with 2MB of L2
 * @version  1.0.4
 ******************************************************************************/
/**
 * Copyright (c) 2009-2020 Arm Limited. All rights reserved.
 * Portions Copyright (c) 2023-2024 Analog Devices. All rights reserved.
 *
 * Based on Device/ARM/ARMCM33/Source/GCC/gcc_arm.ld file in
 * ARM.CMSIS.5.9.0.pack.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 *----------- <<< Use Configuration Wizard in Context Menu >>> -----------------
 */

/*------------------------- L1_IRAM Configuration ------------------------------
  <h> L1_IRAM Configuration
    <o0> L1_IRAM Base Address    <0x0-0xFFFFFFFF:8>
    <o1> L1_IRAM Size (in Bytes) <0x0-0xFFFFFFFF:8>
  </h>
  ----------------------------------------------------------------------------*/
__L1_IRAM_BASE = 0x00040000;
__L1_IRAM_SIZE = 0x00010000;

/*------------------------- L1_DRAM Configuration ------------------------------
  <h> L1_DRAM Configuration
    <o0> L1_DRAM Base Address    <0x0-0xFFFFFFFF:8>
    <o1> L1_DRAM Size (in Bytes) <0x0-0xFFFFFFFF:8>
  </h>
  ----------------------------------------------------------------------------*/
__L1_DRAM_BASE = 0x28AC0000;
__L1_DRAM_SIZE = 0x00020000;

/*------------------------- L2_SRAM Configuration ------------------------------
  <h> L2_SRAM Configuration
    <o0> L2_SRAM Base Address    <0x0-0xFFFFFFFF:8>
    <o1> L2_SRAM Size (in Bytes) <0x0-0xFFFFFFFF:8>
  </h>
 -----------------------------------------------------------------------------*/
__L2_SRAM_BASE = 0x20000000;
__L2_SRAM_SIZE = 0x00020000;

/*------------------------- L3_SDRAM Configuration -----------------------------
  <h> L3_SDRAM Configuration
    <o0> L3_SDRAM Base Address    <0x0-0xFFFFFFFF:8>
    <o1> L3_SDRAM Size (in Bytes) <0x0-0xFFFFFFFF:8>
  </h>
 -----------------------------------------------------------------------------*/
__L3_SDRAM_BASE = 0x90000000;
__L3_SDRAM_SIZE = 0x10000000;

/*------------------------- Stack / Heap Configuration -------------------------
  <h> Stack / Heap Configuration
    <o0> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
    <o1> Heap Size (in Bytes)  <0x0-0xFFFFFFFF:8>
  </h>
  ----------------------------------------------------------------------------*/
__STACK_SIZE = 0x00008000;
__HEAP_SIZE  = 0x00010000;

/**
 *----------------------- <<< end of configuration section >>> -----------------
 */

/* ARMv8-M stack sealing:
   to use ARMv8-M stack sealing set __STACKSEAL_SIZE to 8 otherwise keep 0
 */
__STACKSEAL_SIZE = 8;

/**
 *------------------------- Memory Map -----------------------------------------
 */

MEMORY
{
  L1_IRAM (rwx)  : ORIGIN = __L1_IRAM_BASE, LENGTH  = __L1_IRAM_SIZE
  L1_DRAM (rw)   : ORIGIN = __L1_DRAM_BASE, LENGTH  = __L1_DRAM_SIZE
  L2_SRAM (rwx)  : ORIGIN = __L2_SRAM_BASE, LENGTH  = __L2_SRAM_SIZE
  L3_SDRAM (rwx) : ORIGIN = __L3_SDRAM_BASE, LENGTH = __L3_SDRAM_SIZE
}

/**
 * Linker script to place sections and symbol values.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __copy_table_start__
 *   __copy_table_end__
 *   __zero_table_start__
 *   __zero_table_end__
 *   __etext
 *   __l1_data_start__
 *   __l1_data_end__
 *   __l2_data_start__
 *   __l2_data_end__
 *   __l3_data_start__
 *   __l3_data_end__
 *   __data_start__
 *   __data_end__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __11_bss_start__
 *   __l1_bss_end__
 *   __bss_start__
 *   __bss_end__
 *   __l2_bss_start__
 *   __l2_bss_end__
 *   __13_bss_start__
 *   __l3_bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 *   __StackSeal      (only if ARMv8-M stack sealing is used)
 */

ENTRY(Reset_Handler)

SECTIONS
{
  .text :
  {
    KEEP(*(.vectors))
    *(.text*)

    KEEP(*(.init))
    KEEP(*(.fini))
  } > L1_IRAM

  .l1_text :
  {
    *(.l1_text)
    *(.l1_text.*)
  } > L1_IRAM

  /**
   * SG veneers:
   * All SG veneers are placed in the special output section .gnu.sgstubs.
   * Its start address must be set, either with the command line option
   * '--section-start' or in a linker script, to indicate where to place
   * these veneers in memory.
   */

  .gnu.sgstubs :
  {
    . = ALIGN(32);
  } > L1_IRAM

  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > L1_IRAM

  __exidx_start = .;
  .ARM.exidx :
  {
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
  } > L1_IRAM
  __exidx_end = .;

  /**
   * Location counter can end up 2byte aligned with narrow Thumb code but
   * __etext is assumed by startup code to be the LMA of a section in RAM
   * which must be 4byte aligned
   */
  __etext = ALIGN (4);

  .l2_text :
  {
    *(.l2_text)
    *(.l2_text.*)
  } > L2_SRAM

  .l3_text :
  {
    *(.l3_text)
    *(.l3_text.*)
  } > L3_SDRAM

  /**
   *----------------------- Const Data Sections --------------------------------
   */

  .rodata :
  {
    /* .ctors */
    *crtbegin.o(.ctors)
    *crtbegin?.o(.ctors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
    *(SORT(.ctors.*))
    *(.ctors)

    /* .dtors */
    *crtbegin.o(.dtors)
    *crtbegin?.o(.dtors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
    *(SORT(.dtors.*))
    *(.dtors)

    KEEP(*(.eh_frame*))

    *(.rodata*)
  } > L2_SRAM

  .l1_rodata :
  {
    . = ALIGN(4);
    __l1_rodata_start__ = .;
    *(.l1_rodata)
    *(.l1_rodata.*)
    . = ALIGN(4);
    __l1_rodata_end__ = .;
  } > L1_DRAM

  .l2_rodata :
  {
    . = ALIGN(4);
    __l2_rodata_start__ = .;
    *(.l2_rodata)
    *(.l2_rodata.*)
    . = ALIGN(4);
    __l2_rodata_end__ = .;
  } > L2_SRAM

  .l3_rodata :
  {
    . = ALIGN(4);
    __l3_rodata_start__ = .;
    *(.l3_rodata)
    *(.l3_rodata.*)
    . = ALIGN(4);
    __l3_rodata_end__ = .;
  } > L3_SDRAM

  /**
   *----------------------- Data Sections --------------------------------------
   */

  .data :
  {
    __data_start__ = .;
    *(vtable)
    *(.data)
    *(.data.*)

    . = ALIGN(4);
    /* preinit data */
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP(*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);

    . = ALIGN(4);
    /* init data */
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array))
    PROVIDE_HIDDEN (__init_array_end = .);

    . = ALIGN(4);
    /* finit data */
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array))
    PROVIDE_HIDDEN (__fini_array_end = .);

    KEEP(*(.jcr*))
    . = ALIGN(4);
    /* All data end */
    __data_end__ = .;
  } > L2_SRAM

  .l1_data :
  {
    . = ALIGN(4);
    __l1_data_start__ = .;
    *(.l1_data)
    *(.l1_data.*)
    . = ALIGN(4);
    __l1_data_end__ = .;
  } > L1_DRAM

  .l2_data :
  {
    . = ALIGN(4);
    __l2_data_start__ = .;
    *(.l2_data)
    *(.l2_data.*)
    . = ALIGN(4);
    __l2_data_end__ = .;
  } > L2_SRAM

  .l3_data :
  {
    . = ALIGN(4);
    __l3_data_start__ = .;
    *(.l3_data)
    *(.l3_data.*)
    . = ALIGN(4);
    __l3_data_end__ = .;
  } > L3_SDRAM

  /**
   *----------------------- bss Sections for L1, L2 and L3 memory --------------
   */

  .l1_bss :
  {
    . = ALIGN(4);
    __l1_bss_start__ = .;
    *(.l1_bss)
    *(.l1_bss.*)
    . = ALIGN(4);
    __l1_bss_end__ = .;
  } > L1_DRAM

  .bss :
  {
    . = ALIGN(4);
    __bss_start__ = .;
    *(.bss)
    *(.bss.*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;
  } > L2_SRAM

  .l2_bss :
  {
    . = ALIGN(4);
    __l2_bss_start__ = .;
    *(.l2_bss)
    *(.l2_bss.*)
    . = ALIGN(4);
    __l2_bss_end__ = .;
  } > L2_SRAM

  .l3_bss :
  {
    . = ALIGN(4);
    __l3_bss_start__ = .;
    *(.l3_bss)
    *(.l3_bss.*)
    . = ALIGN(4);
    __l3_bss_end__ = .;
  } > L3_SDRAM

  /**
   *----------------------- noinit Sections for L1, L2 and L3 memory -----------
   */

  .l1_noinit :
  {
    . = ALIGN(4);
    *(.l1_noinit)
    *(.l1_noinit.*)
  } > L1_DRAM

  .noinit :
  {
    . = ALIGN(4);
    *(.noinit)
    *(.noinit.*)
  } > L2_SRAM

  .l2_noinit :
  {
    . = ALIGN(4);
    *(.l2_noinit)
    *(.l2_noinit.*)
  } > L2_SRAM

  .l3_noinit :
  {
    . = ALIGN(4);
    *(.l3_noinit)
    *(.l3_noinit.*)
  } > L3_SDRAM

  /**
   *----------------------- Copy Table Section  --------------------------------
   */

  .copy.table :
  {
    . = ALIGN(4);
    __copy_table_start__ = .;

    /* Add three-word entry for each data section that should be (re)initialized
     * during startup. They would normally be initialized during load.
     * Entries consist of load time address, run time address and size in words.
     */

    __copy_table_end__ = .;
  } > L2_SRAM

  /**
   *----------------------- Zero Table Section ---------------------------------
   */

  .zero.table :
  {
    . = ALIGN(4);
    __zero_table_start__ = .;
    /* Add two-word entry for each additional bss section here, consisting of
     * start address and size in words. The .bss section itself is initialized
     * by the _start routine.
     */
    LONG(__l1_bss_start__)
    LONG((__l1_bss_end__ - __l1_bss_start__) / 4)
    LONG(__l2_bss_start__)
    LONG((__l2_bss_end__ - __l2_bss_start__) / 4)
    LONG(__l3_bss_start__)
    LONG((__l3_bss_end__ - __l3_bss_start__) / 4)

    __zero_table_end__ = .;
  } > L2_SRAM

  /**
   *----------------------- Heap Table Section  --------------------------------
   */

  .heap (NOLOAD) :
  {
    . = ALIGN(8);
    __end__ = .;
    PROVIDE(end = .);
    . = . + __HEAP_SIZE;
    . = ALIGN(8);
    __HeapLimit = .;
  } > L2_SRAM

  /**
   *----------------------- Stack Table Section  -------------------------------
   */

  .stack (ORIGIN(L1_DRAM) + LENGTH(L1_DRAM) - __STACK_SIZE - __STACKSEAL_SIZE) (NOLOAD) :
  {
    . = ALIGN(8);
    __StackLimit = .;
    . = . + __STACK_SIZE;
    . = ALIGN(8);
    __StackTop = .;
  } > L1_DRAM
  PROVIDE(__stack = __StackTop);

  /**
   *----------------------- Stack Seal Table Section  --------------------------
   */

  .stackseal (ORIGIN(L1_DRAM) + LENGTH(L1_DRAM) - __STACKSEAL_SIZE) (NOLOAD) :
  {
    . = ALIGN(8);
    __StackSeal = .;
    . = . + 8;
    . = ALIGN(8);
  } > L1_DRAM

}
