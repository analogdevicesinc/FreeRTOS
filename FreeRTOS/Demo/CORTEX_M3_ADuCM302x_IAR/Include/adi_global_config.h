/*****************************************************************************
    Copyright (C) 2016-2018 Analog Devices Inc. All Rights Reserved.
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
