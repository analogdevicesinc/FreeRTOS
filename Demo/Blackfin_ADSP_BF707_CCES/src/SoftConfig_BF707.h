
/*****************************************************************************
    Copyright (C) 2016-2018 Analog Devices Inc. All Rights Reserved.
*****************************************************************************/

#ifndef SOFTSWITCH_H_
#define SOFTSWITCH_H_

/* switch register structure */
typedef struct {
	uint8_t Register;
	uint8_t Value;
} SWITCH_CONFIG;

/* switch parameter structure */
typedef struct {
	uint32_t TWIDevice;
	uint16_t HardwareAddress;
	uint32_t NumConfigSettings;
	SWITCH_CONFIG *ConfigSettings;
} SOFT_SWITCH;

typedef enum
{
	SS_DEFAULT = 0,
	SS_ETHERNET,
	SS_LINKPORT,
	SS_TWI_GPIO,
	SS_UART,
	SS_PARALLEL,
    SS_PB_LED,
    SS_CAN,
    SS_SRAM,
    SS_SPI,
    SS_SD
}eSOFTSWITCH;

/************************SoftConfig Information********************************

    ~ means the signal is active low

    Please see the ADSP-BF707 EZ-Board manual for more information on using
    Software-Controlled Switches(SoftConfig)

********************************************************************************/


#endif /* SOFTSWITCH_H_ */
