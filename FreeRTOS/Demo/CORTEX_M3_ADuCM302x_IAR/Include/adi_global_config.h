/*!
 *****************************************************************************
   @file:    adi_global_config.h
   @brief:   Global configuration options which will affect all the drivers.
   @version: $Revision: 254 $
   @date:    $Date: 2016-02-09 15:01:45 -0500 (Tue, 09 Feb 2016) $
  -----------------------------------------------------------------------------

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

*****************************************************************************/

/* This file contains all the global configurations */
#ifndef __ADI_GLOBAL_CONFIG_H__
#define __ADI_GLOBAL_CONFIG_H__

/**  @defgroup Configuration_macros Driver Configuration Macros
 *   @ingroup VSM_cfg
 */

/** @defgroup Global_Config Global Configuration Settings
 *  @ingroup Configuration_macros
 */

/** @addtogroup Global_Config Global Configuration Settings
 *  @{
 */

/*! Set this macro to 1 to have the IVT in writeable memory instead of ROM.
    In the case of an RTOS, the drivers install their OS handlers at runtime
    and therefore the macro needs to be set.
 */
#if (ADI_CFG_ENABLE_RTOS_SUPPORT ==1)
/* #define RELOCATE_IVT 1 */
#endif

/*! Set this macro to put the processor in low power mode when waiting for the buffer processing to complete */
#define ADI_CFG_ENTER_LOW_PWR_MODE_SUPPORT                0

#if (ADI_CFG_ENTER_LOW_PWR_MODE_SUPPORT ==1 ) && (ADI_CFG_ENABLE_RTOS_SUPPORT == 1)
#error " Low power mode supported only in NON-RTOS environment"
#endif
/*! @} */

/*! Set this macro to enable the profiling*/
#define ADI_PROFILE_ENABLED             0

#if (ADI_CFG_ENABLE_RTOS_SUPPORT ==1)
#if !defined(USER_SPECIFIED_RTOS)
#define ADI_DEVICE_DRIVER_RTOS_MEMORY  68u
#endif
#else
#define ADI_DEVICE_DRIVER_RTOS_MEMORY  4u
#endif

#endif /* __ADI_GLOBAL_CONFIG_H__ */
