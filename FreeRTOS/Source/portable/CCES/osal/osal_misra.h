/*
****************************************************************************
Copyright (c) 2016, Analog Devices, Inc.  All rights reserved.

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

******************************************************************************
Title: ADI MISRA C 2004 compliance matrix and suppression directives for ADuCxxx

Description:

    IAR Suppression Matrix:
    Error[Pm002]: compiler is configured to allow extensions - all code shall conform
                  to ISO 9899 standard C, with no extensions permitted (MISRA C 2004 rule 1.1)
    Error[Pm064]: functions with variable number of arguments shall not be used (MISRA C 2004 rule 16.1)
    Error[Pm073]: a function should have a single point of exit (MISRA C 2004 rule 14.7)
    Error[Pm081]: #undef should not be used (MISRA C 2004 rule 19.6)
    Error[Pm086]: there shall be at most one occurrence of the # or ## preprocessor operator
                  in a single macro definition (MISRA C 2004 rule 19.12)
    Error[Pm093]: use of union - overlapping storage shall not be used (MISRA C 2004 rule 18.4)
    Error[Pm143]: a function should have a single point of exit at the end of the function
                  (MISRA C 2004 rule 14.7)
    Error[Pm152]: array indexing shall only be applied to objects defined as an array type
                  (MISRA C 2004 rule 17.4)

    IAR Suppression Matrix:
    Error[Pm073]:  a function should have a single point of exit
                   (MISRA C 2004 rule 14.7)

    Error[Pm088]: pointer arithmetic should not be used (MISRA C 2004 rule 17.4)

    Error[Pm100]: dynamic heap memory allocation shall not be used
                  (MISRA C 2004 rule 20.4)

    Error[Pm114]: an 'if (expression)' construct shall be followed by a compound
                  statement. The 'else' keyword shall be followed by either a
                  compound statement, or another 'if'  statement
                  (MISRA C 2004 rule 14.9)

    Error[Pm120]: the # and ## preprocessor operators should not be used
                  (MISRA C 2004 rule 19.13)

    Error[Pm123]: there shall be no definition of objects or functions in a
                  header file (MISRA C 2004 rule 8.5)

    Error[Pm127]: a 'U' suffix shall be applied to all constants of 'unsigned' type
                  (MISRA C 2004 rule 10.6)

    Error[Pm128]: illegal implicit conversion from underlying MISRA type
                  (MISRA C 2004 rule 10.1)

    Error[Pm138]: conversions shall not be performed between a pointer to a
                  function and any other type other than an integral type
                  (MISRA C 2004 rule 11.1)

    Error[Pm139]: conversions shall not be performed between a pointer to object
                  and any type other than an integral type
                  (MISRA C 2004 rule 11.2)

    Error[Pm140]: a cast should not be performed between a pointer type and an
                  integral type (MISRA C 2004 rule 11.3)

    Error[Pm141]: a cast should not be performed between a pointer to object type
                  and a different pointer to object type(MISRA C 2004 rule 11.4)

    Error[Pm143]: a function should have a single point of exit at the end of
                  the function (MISRA C 2004 rule 14.7)

*****************************************************************************/
/*! @file osal_misra.h */
 /*! @cond */
#ifndef __ADI_OSAL_MISRA_H__
#define __ADI_OSAL_MISRA_H__


#if defined ( __ICCARM__ )  /* IAR compiler... */

/* assert */
/*#define  ASSERT_ADI_OSAL_MISRA_SUPPRESSIONS  _Pragma ("diag_suppress= Pm001,Pm002,Pm073,Pm088,Pm100,Pm114,Pm120,Pm123,Pm127,Pm128,Pm138,Pm139,Pm140,Pm141,Pm143")*/
/*deassert */
/*#define  REVERT_ADI_OSAL_MISRA_SUPPRESSIONS  _Pragma ("diag_default=  Pm001,Pm002,Pm073,Pm088,Pm100,Pm114,Pm120,Pm123,Pm127,Pm128,Pm138,Pm139,Pm140,Pm141,Pm143")*/
/*assert */

#define  ADI_OSAL_MISRA_SUPPRESS_ALL _Pragma ("diag_suppress=                \
     Pm001,Pm002,Pm003,Pm004,Pm005,Pm006,Pm007,Pm008,Pm009,Pm010,Pm011, \
     Pm012,Pm013,Pm014,Pm015,Pm016,Pm017,Pm018,Pm019,Pm020,Pm021,Pm022, \
     Pm023,Pm024,Pm025,Pm026,Pm027,Pm028,Pm029,Pm030,Pm031,Pm032,Pm033, \
     Pm034,Pm035,Pm036,Pm037,Pm038,Pm039,Pm040,Pm041,Pm042,Pm043,Pm044, \
     Pm045,Pm046,Pm047,Pm048,Pm049,Pm050,Pm051,Pm052,Pm053,Pm054,Pm055, \
     Pm056,Pm057,Pm058,Pm059,Pm060,Pm061,Pm062,Pm063,Pm064,Pm065,Pm066, \
     Pm067,Pm068,Pm069,Pm070,Pm071,Pm072,Pm073,Pm074,Pm075,Pm076,Pm077, \
     Pm078,Pm079,Pm080,Pm081,Pm082,Pm083,Pm084,Pm085,Pm086,Pm087,Pm088, \
     Pm089,Pm090,Pm091,Pm092,Pm093,Pm094,Pm095,Pm096,Pm097,Pm098,Pm099, \
     Pm100,Pm101,Pm102,Pm103,Pm104,Pm105,Pm106,Pm107,Pm108,Pm109,Pm110, \
     Pm111,Pm112,Pm113,Pm114,Pm115,Pm116,Pm117,Pm118,Pm119,Pm120,Pm121, \
     Pm122,Pm123,Pm124,Pm125,Pm126,Pm127,Pm128,Pm129,Pm130,Pm131,Pm132, \
     Pm133,Pm134,Pm135,Pm136,Pm137,Pm138,Pm139,Pm140,Pm141,Pm142,Pm143, \
     Pm144,Pm145,Pm146,Pm147,Pm148,Pm149,Pm150,Pm151,Pm152,Pm153,Pm154, \
     Pm155")

#define  ADI_OSAL_MISRA_RESTORE_ALL _Pragma ("diag_default=                  \
     Pm001,Pm002,Pm003,Pm004,Pm005,Pm006,Pm007,Pm008,Pm009,Pm010,Pm011, \
     Pm012,Pm013,Pm014,Pm015,Pm016,Pm017,Pm018,Pm019,Pm020,Pm021,Pm022, \
     Pm023,Pm024,Pm025,Pm026,Pm027,Pm028,Pm029,Pm030,Pm031,Pm032,Pm033, \
     Pm034,Pm035,Pm036,Pm037,Pm038,Pm039,Pm040,Pm041,Pm042,Pm043,Pm044, \
     Pm045,Pm046,Pm047,Pm048,Pm049,Pm050,Pm051,Pm052,Pm053,Pm054,Pm055, \
     Pm056,Pm057,Pm058,Pm059,Pm060,Pm061,Pm062,Pm063,Pm064,Pm065,Pm066, \
     Pm067,Pm068,Pm069,Pm070,Pm071,Pm072,Pm073,Pm074,Pm075,Pm076,Pm077, \
     Pm078,Pm079,Pm080,Pm081,Pm082,Pm083,Pm084,Pm085,Pm086,Pm087,Pm088, \
     Pm089,Pm090,Pm091,Pm092,Pm093,Pm094,Pm095,Pm096,Pm097,Pm098,Pm099, \
     Pm100,Pm101,Pm102,Pm103,Pm104,Pm105,Pm106,Pm107,Pm108,Pm109,Pm110, \
     Pm111,Pm112,Pm113,Pm114,Pm115,Pm116,Pm117,Pm118,Pm119,Pm120,Pm121, \
     Pm122,Pm123,Pm124,Pm125,Pm126,Pm127,Pm128,Pm129,Pm130,Pm131,Pm132, \
     Pm133,Pm134,Pm135,Pm136,Pm137,Pm138,Pm139,Pm140,Pm141,Pm142,Pm143, \
     Pm144,Pm145,Pm146,Pm147,Pm148,Pm149,Pm150,Pm151,Pm152,Pm153,Pm154, \
     Pm155")

#else
/* assert */
#define  ASSERT_OSAL_ADI_MISRA_SUPPRESSIONS

/* deassert */
#define  REVERT_OSAL_ADI_MISRA_SUPPRESSIONS

#define  ADI_OSAL_MISRA_SUPPRESS_ALL

#define  ADI_OSAL_MISRA_RESTORE_ALL

#endif

#endif /* __ADI_OSAL_MISRA_H__ */
/*! @endcond */
