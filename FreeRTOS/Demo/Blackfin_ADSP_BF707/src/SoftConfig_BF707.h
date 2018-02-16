/*
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

 */

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
    SS_SD,
}eSOFTSWITCH;

/************************SoftConfig Information********************************

    ~ means the signal is active low

    Please see the ADSP-BF707 EZ-Board manual for more information on using
    Software-Controlled Switches(SoftConfig)

********************************************************************************/

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

#endif /* SOFTSWITCH_H_ */
