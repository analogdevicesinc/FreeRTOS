/*****************************************************************************
    Copyright (C) 2016-2018 Analog Devices Inc. All Rights Reserved.
*****************************************************************************/

#include <stdio.h>
#include <drivers/twi/adi_twi.h>

#include "SoftConfig_BF707.h"


#if defined(__DEBUG_FILE__)
#include <string.h>
extern FILE *pDebugFile;				/* debug file when directing output to a file */
#endif

/* TWI settings */
#define TWI_PRESCALE  (10u)
#define TWI_BITRATE   (100u)
#define TWI_DUTYCYCLE (50u)

#define BUFFER_SIZE (32u)

/* TWI hDevice handle */
static ADI_TWI_HANDLE hDevice;

/* TWI data buffer */
static uint8_t twiBuffer[BUFFER_SIZE];

/* TWI device memory */
uint8_t deviceMemory[ADI_TWI_MEMORY_SIZE];

/* switch 0 register settings */
static SWITCH_CONFIG DefSwitchConfig0[] =
{

 /*
       U39 Port A                                  U39 Port B
  7--------------- ~RF_SPI2_SEL2_EN       |       7--------------- ~CAN1_ERR_EN
  | 6------------- ~RF_SPI2_SEL1_EN       |       | 6------------- ~CAN0_ERR_EN
  | | 5----------- ~SD_WP_EN              |       | | 5----------- ~CAN1_STB
  | | | 4--------- ~SD_CD_EN              |       | | | 4---------  CAN1_EN
  | | | | 3------- ~RF_SPI2_SEL1_EN       |       | | | | 3------- ~CAN0_STB
  | | | | | 2----- ~SPIFLASH_D3_EN        |       | | | | | 2-----  CAN0_EN
  | | | | | | 1--- ~SPIFLASH_D2_EN        |       | | | | | | 1--- ~SD_WP_EN
  | | | | | | | 0- ~SPIFLASH_CS_EN        |       | | | | | | | 0- ~SD_CD_EN
  | | | | | | | |                         |       | | | | | | | |
  N X N N N Y Y Y                         |       N N N Y N Y X X     ( Active Y or N )
  1 1 1 1 1 0 0 0                         |       1 1 1 1 1 1 1 1     ( value being set )
*/
  { 0x12u, 0xF8u },                               { 0x13u, 0xFFu },

 /*
  * specify inputs/outputs
  */

  { 0x0u, 0x40u },   /* Set IODIRA direction (bit 6 input, all others output) */
  { 0x1u, 0x03u },   /* Set IODIRB direction (bit 0, 1 input, all others output) */
};
/* switch 2 register settings */
static SWITCH_CONFIG DefSwitchConfig1[] =
{

/*
            U38 Port A                                    U38 Port B

    7--------------- ~UART0CTS_RTS_LPBK    |       7---------------  Not Used
    | 6------------- ~UART0CTS_EN          |       | 6------------- ~PUSHBUTTON2_EN
    | | 5----------- ~UART0RTS_EN          |       | | 5----------- ~PUSHBUTTON1_EN
    | | | 4--------- ~UART0_EN             |       | | | 4--------- ~LED3_GPIO3_EN
    | | | | 3------- ~CAN1_RX_EN           |       | | | | 3------- ~LED2_GPIO2_EN
    | | | | | 2----- ~CAN0_RX_EN           |       | | | | | 2----- ~LED1_GPIO1_EN
    | | | | | | 1--- ~CAN1_TX_EN           |       | | | | | | 1--- ~UART0CTS_146_EN
    | | | | | | | 0- ~CAN0_TX_EN           |       | | | | | | | 0- ~UART0CTS_RST_EN
    | | | | | | | |                        |       | | | | | | | |
    N N N Y Y Y Y N                        |       X Y Y Y Y Y N N    ( Active Y or N )
    1 1 1 0 0 0 0 1                        |       0 0 0 0 0 0 1 1    ( value being set )
*/
  { 0x12u, 0xE1u },                               { 0x13u, 0x03u },

  /*
   * specify inputs/outputs
   */

  { 0x0u, 0x00u },    /* Set IODIRA direction (all output) */
  { 0x1u, 0x80u },    /* Set IODIRB direction (bit 7 input, all others output) */
};

/* switch configuration */
static SOFT_SWITCH DefSoftSwitch[] =
{
  {
    0u,
    0x21u,
    sizeof(DefSwitchConfig0)/sizeof(SWITCH_CONFIG),
    DefSwitchConfig0
  },
  {
    0u,
    0x22u,
    sizeof(DefSwitchConfig1)/sizeof(SWITCH_CONFIG),
    DefSwitchConfig1
  }
};

void ConfigSoftSwitches(void);

/* disable misra diagnostics as necessary */
#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rule_8_7:"Objects shall be defined at block scope")
#pragma diag(suppress:misra_rule_17_4:"Array indexing shall be the only allowed form of pointer arithmetic")
#endif /* _MISRA_RULES */


#define ADI_DEBUG 1
#if defined(ADI_DEBUG)
#include <stdio.h>
#define CHECK_RESULT(result, message) \
	do { \
		if((result) != ADI_TWI_SUCCESS) \
		{ \
			printf("Error %d calling %s\n", ( result ), ( message ) ); \
		} \
	} while (0)  /* do-while-zero needed for Misra Rule 19.4 */
#else
#define CHECK_RESULT(result, message)
#endif


void ConfigSoftSwitches(void)
{
	uint32_t switchNum;
	uint32_t configNum;
	uint32_t switches;

	SOFT_SWITCH *ss;

	SWITCH_CONFIG *configReg;
	ADI_TWI_RESULT result;


	SOFT_SWITCH *SoftSwitch = &DefSoftSwitch[0];
	uint32_t nNumber = (uint32_t)(sizeof(DefSoftSwitch)/sizeof(SOFT_SWITCH));
	switches = nNumber;

	for (switchNum=0u; switchNum<switches; switchNum++)
	{

		ss = &SoftSwitch[switchNum];

		result = adi_twi_Open(ss->TWIDevice, ADI_TWI_MASTER,
    		deviceMemory, ADI_TWI_MEMORY_SIZE, &hDevice);
		CHECK_RESULT(result, "adi_twi_Open failed");

		result = adi_twi_SetHardwareAddress(hDevice, ss->HardwareAddress);
		CHECK_RESULT(result, "adi_twi_SetHardwareAddress failed");

		result = adi_twi_SetPrescale(hDevice, TWI_PRESCALE);
		CHECK_RESULT(result, "adi_twi_Prescale failed");

		result = adi_twi_SetBitRate(hDevice, TWI_BITRATE);
		CHECK_RESULT(result, "adi_twi_SetBitRate failed");

		result = adi_twi_SetDutyCycle(hDevice, TWI_DUTYCYCLE);
		CHECK_RESULT(result, "adi_twi_SetDutyCycle failed");

		/* switch register settings */
		for (configNum=0u; configNum < ss->NumConfigSettings; configNum++)
		{
			configReg = &ss->ConfigSettings[configNum];

			/* write register value */
			twiBuffer[0] = configReg->Register;
			twiBuffer[1] = configReg->Value;
			result = adi_twi_Write(hDevice, twiBuffer, (uint32_t)2, false);
			CHECK_RESULT(result, "adi_twi_Write failed");
		}

		result = adi_twi_Close(hDevice);
		CHECK_RESULT(result, "adi_twi_Close failed");
	}
}

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif /* _MISRA_RULES */
