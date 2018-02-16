/*! *****************************************************************************
 * @file:     common.h
 * @brief:    common include file for all example
 * @version: $Revision: 35293 $
 * @date:    $Date: 2016-08-12 04:28:57 -0400 (Fri, 12 Aug 2016) $
 *-----------------------------------------------------------------------------
 Copyright (C) 2000-2017 Analog Devices Inc. All Rights Reserved.

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

 *****************************************************************************/


#ifndef __TEST_COMMON_H__
#define __TEST_COMMON_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <services/wd/adi_wd.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEBUG_MESSAGE_LENGTH 150
extern char aDebugString[DEBUG_MESSAGE_LENGTH];

#define DEBUG_MESSAGE(...) \
  do { \
    sprintf(aDebugString,__VA_ARGS__); \
    test_Perf(aDebugString); \
  } while(0)


#define DEBUG_RESULT(s,result,expected_value) \
  do { \
    if ((result) != (expected_value)) { \
      sprintf(aDebugString,"%s  %d", __FILE__,__LINE__); \
      test_Fail(aDebugString); \
      sprintf(aDebugString,"%s Error Code: 0x%08X\n\rFailed\n\r",(s),(result)); \
      test_Perf(aDebugString); \
      exit(0); \
    } \
  } while (0)

/********************************************************************************
* API function prototypes
*********************************************************************************/
void test_Init(void);
void test_Pass(void);
void test_Fail(char *FailureReason);
void test_Perf(char *InfoString);

#ifdef __cplusplus
}
#endif

#endif /* __TEST_COMMON_H__ */
