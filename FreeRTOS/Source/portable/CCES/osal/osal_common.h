/*
**************************************************************************
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

***************************************************************************

Title:   Common OSAL definitions

         This file contains definitions that are common to all OSAL implementations
         It is suppose to be included in the implementation file.
         IT IS NOT A PUBLIC FILE.

*****************************************************************************/
/*!    @file osal_common.h */

#ifndef __OSAL_COMMON_H__
#define __OSAL_COMMON_H__

/*=============  I N C L U D E S   =============*/
#include "adi_osal.h"
#include "osal_misra.h"

/*==============  D E F I N E S  ===============*/

#if defined(__STDC__)
/* C language specific macros and declarations*/

 /* True if running at ISR level. */
#define CALLED_FROM_AN_ISR              (_adi_osal_IsCurrentLevelISR())

/* number of microseconds per second */
#define     USEC_PER_SEC                    (1000000u)


/*=============  E X T E R N A L S  =============*/

/* Local global variables that are shared across files */

/* code */
extern  bool _adi_osal_AcquireGlobalLock( void );
extern  void _adi_osal_ReleaseGlobalLock( void );
extern ADI_OSAL_STATUS _adi_osal_InitInterrupts(void);

#else
/* assembly language specific macros and declarations*/


#endif  /* if !defined(__STDC__) */


#endif /*__OSAL_COMMON_H__ */

/*
**
** EOF:
**
*/
