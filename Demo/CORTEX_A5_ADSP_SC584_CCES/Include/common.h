/*****************************************************************************
    Copyright (C) 2000-2018 Analog Devices Inc. All Rights Reserved.
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
