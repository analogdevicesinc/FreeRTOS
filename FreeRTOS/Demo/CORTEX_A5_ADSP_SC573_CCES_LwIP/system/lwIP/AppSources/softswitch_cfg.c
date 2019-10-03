/*****************************************************************************
    Copyright (C) 2012-2018 Analog Devices Inc. All Rights Reserved.
*****************************************************************************/

#include <adi_types.h>
#include <builtins.h>

#if defined(ADI_DEBUG)
#include <stdio.h>
#define CHECK_RESULT(result, message) \
    do { \
        if((result) != ADI_TWI_SUCCESS) \
        { \
            printf((message)); \
            printf("\r\n"); \
        } \
    } while (0)  /* do-while-zero needed for Misra Rule 19.4 */
#else
/* Done to prevent compiler warning */
#define CHECK_RESULT(result, message)  result = result
#endif

/***************************************************************
 *
 *   ADSP-BF609 or ADSP-SC573 family processor
 */
#if defined(__ADSPBF609__) || defined(__ADSP215xx__)
#include <drivers\twi\adi_twi.h>

/* TWI settings */
#define TWI_PRESCALE  (8u)
#define TWI_BITRATE   (100u)
#define TWI_DUTYCYCLE (50u)

#define BUFFER_SIZE (32u)

/* TWI hDevice handle */
static ADI_TWI_HANDLE hDevice;

/* TWI data buffer */
static uint8_t twiBuffer[BUFFER_SIZE];

/* TWI device memory */
static uint8_t deviceMemory[ADI_TWI_MEMORY_SIZE];

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

/* prototype */
int ConfigSoftSwitches(void);

/* disable misra diagnostics as necessary */
#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rule_8_7:"Objects shall be defined at block scope")
#pragma diag(suppress:misra_rule_17_4:"Array indexing shall be the only allowed form of pointer arithmetic")
#endif /* _MISRA_RULES */

#if defined(__ADSPBF609__)
/* switch 0 register settings */
static SWITCH_CONFIG SwitchConfig0[] =
{
/*
            U45 Port A                                  U45 Port B

    7--------------- RMII_CLK_EN        |       7--------------- ~TEMP_THERM_EN
    | 6------------- ~CNT0ZM_EN         |       | 6------------- ~TEMP_IRQ_EN
    | | 5----------- ~CNT0DG_EN         |       | | 5----------- ~UART0CTS_146_EN
    | | | 4--------- ~CNT0UD_EN         |       | | | 4--------- ~UART0CTS_RST_EN
    | | | | 3------- ~CAN0RX_EN         |       | | | | 3------- ~UART0CTS_RTS_LPBK
    | | | | | 2----- ~CAN0_ERR_EN       |       | | | | | 2----- ~UART0CTS_EN
    | | | | | | 1--- ~CAN_STB           |       | | | | | | 1--- ~UART0RX_EN
    | | | | | | | 0- CAN_EN             |       | | | | | | | 0- ~UART0RTS_EN
    | | | | | | | |                     |       | | | | | | | |
    N Y Y Y Y N N Y                     |       N N N N N N Y Y     ( Active Y or N )
    1 0 0 0 0 1 1 1                     |       1 1 1 1 1 1 0 0     ( value being set )
*/

  { 0x12u, 0x87u },                          { 0x13u, 0xFCu },

  /* specify inputs/outputs */
  { 0x0u, 0x0u },
  { 0x1u, 0x0u },
};
/* switch 1 register settings */
static SWITCH_CONFIG SwitchConfig1[] =
{
/*
            U46 Port A                                  U46 Port B

    7--------------- ~LED4_GPIO_EN          |       7--------------- EMPTY
    | 6------------- ~LED3_GPIO_EN          |       | 6------------- ~SPI0D3_EN
    | | 5----------- ~LED2_GPIO_EN          |       | | 5----------- ~SPI0D2_EN
    | | | 4--------- ~LED1_GPIO_EN          |       | | | 4--------- ~SPIFLASH_CS_EN
    | | | | 3------- SMC0_LP0_EN            |       | | | | 3------- ~SD_WP_EN
    | | | | | 2----- EMPTY                  |       | | | | | 2----- ~SD_CD_EN
    | | | | | | 1--- SMC0_EPPI2_LP1_SWITCH  |       | | | | | | 1--- ~PUSHBUTTON2_EN
    | | | | | | | 0- OVERRIDE_SMC0_LP0_BOOT |       | | | | | | | 0- ~PUSHBUTTON1_EN
    | | | | | | | |                         |       | | | | | | | |
    Y Y Y Y Y X Y Y                         |       X Y Y Y Y Y Y Y     ( Active Y or N )
    0 0 0 0 1 X 1 0                         |       X 0 0 0 0 0 0 0     ( value being set )
*/
  { 0x12u, 0xAu },                                  { 0x13u, 0x0u },

  /* specify inputs/outputs */
  { 0x0u, 0x0u },
  { 0x1u, 0x0u },
};
/* switch 2 register settings */
static SWITCH_CONFIG SwitchConfig2[] =
{
/*
            U47 Port A                                                  U47 Port B

    7--------------- ~PD2_SPI0MISO_EI3_EN                   |       7--------------- EMPTY
    | 6------------- ~PD1_SPI0D3_EPPI1D17_SPI0SEL2_EI3_EN   |       | 6------------- EMPTY
    | | 5----------- ~PD0_SPI0D2_EPPI1D16_SPI0SEL3_EI3_EN   |       | | 5----------- EMPTY
    | | | 4--------- ~WAKE_PUSHBUTTON_EN                    |       | | | 4--------- EMPTY
    | | | | 3------- ~ETHERNET_EN                           |       | | | | 3------- EMPTY
    | | | | | 2----- PHYAD0                                 |       | | | | | 2----- EMPTY
    | | | | | | 1--- PHY_PWR_DWN_INT                        |       | | | | | | 1--- ~PD4_SPI0CK_EI3_EN
    | | | | | | | 0- ~PHYINT_EN                             |       | | | | | | | 0- ~PD3_SPI0MOSI_EI3_EN
    | | | | | | | |                                         |       | | | | | | | |
    N N N N Y N N Y                                         |       X X X X X X N N     ( Active Y or N )
    1 1 1 1 0 0 0 0                                         |       X X X X X X 1 1     ( value being set )
*/
 { 0x12u, 0xF0u },                                                 { 0x13u, 0x3u },

 /* specify inputs/outputs */
  { 0x0u, 0x6u },
  { 0x1u, 0x0u },
};
#elif defined(__ADSPSC589__)
/* switch 0 register settings */
static SWITCH_CONFIG SwitchConfig0[] =
{

 /*
       U47 Port A                                  U47 Port B
  7--------------- ~CAN1_EN       		|       7--------------- ~SPDIF_DIGITAL EN
  | 6------------- ~CAN0_EN				|       | 6------------- ~SPDIF_OPTICAL EN
  | | 5----------- ~MLB3_EN             |       | | 5----------- ~SPI2D2_D3_EN
  | | | 4--------- ~ETH1_EN             |       | | | 4--------- ~SPI2FLASH_CS_EN
  | | | | 3------- ~ETH0_EN       		|       | | | | 3------- ~SD_WP_EN
  | | | | | 2----- ~UART0_EN        	|       | | | | | 2----- AUDIO_JACK_SEL
  | | | | | | 1--- ~UART0_FLOW_EN       |       | | | | | | 1--- ~ADAU1979_EN
  | | | | | | | 0- ~EEPROM_EN        	|       | | | | | | | 0- ~ADAU1962_EN
  | | | | | | | |                      	|       | | | | | | | |
  N N N Y Y N N N                       |       N N N N N N N N     ( Active Y or N )
  1 1 1 0 0 1 1 1                       |       1 1 1 1 1 0 1 1     ( value being set )
*/
  { 0x12u, 0xE7u },                               { 0x13u, 0xFBu },

 /*
  * specify inputs/outputs
  */

  { 0x0u, 0x00u },   /* Set IODIRA direction (all output) */
  { 0x1u, 0x00u },   /* Set IODIRB direction (all output) */
};
/* switch 1 register settings */
static SWITCH_CONFIG SwitchConfig1[] =
{

/*
            U48 Port A                                    U48 Port B

    7--------------- Not Used			|       7--------------- Not Used
    | 6------------- Not Used          	|       | 6------------- Not Used
    | | 5----------- Not Used          	|       | | 5----------- Not Used
    | | | 4--------- ~LED12_EN          |       | | | 4--------- Not Used
    | | | | 3------- ~LED11_EN          |       | | | | 3------- Not Used
    | | | | | 2----- ~LED10_EN          |       | | | | | 2----- Not Used
    | | | | | | 1--- ~PUSHBUTTON2_EN    |       | | | | | | 1--- Not Used
    | | | | | | | 0- ~PUSHBUTTON1_EN    |       | | | | | | | 0- Not Used
    | | | | | | | |                     |       | | | | | | | |
    X X X Y Y Y Y Y                     |       X X X X X X X X    ( Active Y or N )
    1 1 1 0 0 0 0 0                     |       1 1 1 1 1 1 1 1    ( value being set )
*/
  { 0x12u, 0xE0u },                               { 0x13u, 0xFFu },

  /*
   * specify inputs/outputs
   */

  { 0x0u, 0xE0u },    /* Set IODIRA direction (bit 7-5 input, all others output) */
  { 0x1u, 0xFFu },    /* Set IODIRB direction (all input) */
};

#elif defined(__ADSPSC573__)


/* switch 0 register settings */
static SWITCH_CONFIG SwitchConfig0[] =
{


 /*
	   U47 Port A                                  U47 Port B
  7--------------- ~CAN1_EN             |       7--------------- ~SPDIF_DIGITAL EN
  | 6------------- ~CAN0_EN             |       | 6------------- ~SPDIF_OPTICAL EN
  | | 5----------- ~MLB3_EN             |       | | 5----------- ~SPI2D2_D3_EN
  | | | 4--------- ~BCM89810_EN         |       | | | 4--------- ~SPI2FLASH_CS_EN
  | | | | 3------- ~DP83865_EN          |       | | | | 3------- NOT USED
  | | | | | 2----- ~UART0_EN            |       | | | | | 2----- AUDIO_JACK_SEL
  | | | | | | 1--- ~UART0_FLOW_EN       |       | | | | | | 1--- ~ADAU1979_EN
  | | | | | | | 0- ~EEPROM_EN           |       | | | | | | | 0- ~ADAU1962_EN
  | | | | | | | |                       |       | | | | | | | |
  N N N N Y N N N                       |       N N N N X N N N     ( Active Y or N )
  1 1 1 1 0 0 1 1                       |       1 1 1 1 0 0 1 1     ( value being set )
*/
  { 0x12u, 0xF3u },                               { 0x13u, 0xF3u },

 /*
  * specify inputs/outputs
  */

  { 0x0u, 0x00u },   /* Set IODIRA direction (all output) */
  { 0x1u, 0x00u },   /* Set IODIRB direction (all output) */
};
/* switch 1 register settings */
static SWITCH_CONFIG SwitchConfig1[] =
{

/*
	   U48 Port A                                  U48 Port B

	7--------------- ~FLG3_LOOP         |       7--------------- Not Used
	| 6------------- ~FLG2_LOOP         |       | 6------------- Not Used
	| | 5----------- ~FLG1_LOOP         |       | | 5----------- Not Used
	| | | 4--------- ~FLG0_LOOP         |       | | | 4--------- AD2410_MASTER_SLAVE
	| | | | 3------- ~LEDs_EN           |       | | | | 3------- ~ENGINE_RPM_OE
	| | | | | 2----- ~PUSHBUTTON1_EN    |       | | | | | 2----- ~THUMBWHEEL_OE
	| | | | | | 1--- ~PUSHBUTTON2_EN    |       | | | | | | 1--- ~ADAU1977_FAULT_RST_EN
	| | | | | | | 0- ~PUSHBUTTON3_EN    |       | | | | | | | 0- ~ADAU1977_EN
	| | | | | | | |                     |       | | | | | | | |
	N N N N Y Y Y Y                     |       X X X N N N N N    ( Active Y or N )
	1 1 1 1 0 0 0 0                     |       1 1 1 0 1 1 1 1    ( value being set )
*/
  { 0x12u, 0xF0u },                               { 0x13u, 0xEFu },

  /*
   * specify inputs/outputs
   */

  { 0x0u, 0xE0u },    /* Set IODIRA direction (bit 7-5 input, all others output) */
  { 0x1u, 0xFFu },    /* Set IODIRB direction (all input) */
};
#else

#error "Unsupported processor"

#endif


/* switch configuration */
static SOFT_SWITCH SoftSwitch[] =
{
  {
    0u,
    0x21u,
    sizeof(SwitchConfig0)/sizeof(SWITCH_CONFIG),
    SwitchConfig0
  },
  {
    0u,
    0x22u,
    sizeof(SwitchConfig1)/sizeof(SWITCH_CONFIG),
    SwitchConfig1
  },
#if defined(__ADSPBF609__)
  {
    0u,
    0x23u,
    sizeof(SwitchConfig2)/sizeof(SWITCH_CONFIG),
    SwitchConfig2
  },
#endif
};
     
/****************************************************************************************
 *
 * @brief     ADSP-BF609 platform this function configures the soft switches
 *            and sets up GPIO for EMAC0
 *
 * @details          
 *
 * @return          
 *
 */
int ConfigSoftSwitches(void)
{
    uint32_t switchNum;
    uint32_t configNum;
    uint32_t switches;

    SOFT_SWITCH *ss;
    SWITCH_CONFIG *configReg;
    ADI_TWI_RESULT result;

    switches = (uint32_t)(sizeof(SoftSwitch)/sizeof(SOFT_SWITCH));
    for (switchNum=0u; switchNum<switches; switchNum++)
    {
        ss = &SoftSwitch[switchNum];

        result = adi_twi_Open(ss->TWIDevice, ADI_TWI_MASTER,
                              deviceMemory, ADI_TWI_MEMORY_SIZE, &hDevice);
        CHECK_RESULT(result, "adi_twi_Open failed");

        if (ADI_TWI_SUCCESS != result)
        {
        	break;
        }

        result = adi_twi_SetHardwareAddress(hDevice, ss->HardwareAddress);
        CHECK_RESULT(result, "adi_twi_SetHardwareAddress failed");

        if (ADI_TWI_SUCCESS != result)
        {
        	adi_twi_Close(hDevice);
        	break;
        }

        result = adi_twi_SetPrescale(hDevice, TWI_PRESCALE);
        CHECK_RESULT(result, "adi_twi_Prescale failed");

        if (ADI_TWI_SUCCESS != result)
        {
        	adi_twi_Close(hDevice);
        	break;
        }

        result = adi_twi_SetBitRate(hDevice, TWI_BITRATE);
        CHECK_RESULT(result, "adi_twi_SetBitRate failed");

        if (ADI_TWI_SUCCESS != result)
        {
        	adi_twi_Close(hDevice);
        	break;
        }

        result = adi_twi_SetDutyCycle(hDevice, TWI_DUTYCYCLE);
        CHECK_RESULT(result, "adi_twi_SetDutyCycle failed");

        /* switch register settings */
        for (configNum=0u; configNum<ss->NumConfigSettings; configNum++)
        {
            configReg = &ss->ConfigSettings[configNum];

            /* write register value */
            twiBuffer[0] = configReg->Register;
            twiBuffer[1] = configReg->Value;
            result = adi_twi_Write(hDevice, twiBuffer, (uint32_t)2, false);
            CHECK_RESULT(result, "adi_twi_Write failed");
            if (ADI_TWI_SUCCESS != result)
            {
            	adi_twi_Close(hDevice);
            	break;
            }

        }

        result = adi_twi_Close(hDevice);
        CHECK_RESULT(result, "adi_twi_Close failed");
    } 

    return (int)result;
}
#elif defined(__ADSPBF518__)
#include <cdefbf518.h>
void ConfigSoftSwitches(void) {
	*pPORTG_FER &= 0xfffd;
}
#else
void ConfigSoftSwitches(void) { return;}
#endif 

#if defined( __ADSPBF527__)
#include <bfrom.h>
#include <cdefbf527.h>

/* all ez-kits has 25Mhz oscillator, custom boards needs this to change */
#define CLKIN_VAL            (25)
#define OTP_TP3              (0x15548000) /* constant */
#define OTP_MAC_ADDRESS_PAGE (0xDF)

/****************************************************************************************
 *
 * @brief           Configures OTP memory and reads MAC address from it.
 *
 * @param[in]        ptr points to the buffer location where the MAC address will be stored.
 *                   The supplied buffer size must be at least 6 bytes.
 *
 * @details          Copies MAC address from OTP memory to the supplied memory
 *     
 * @return           return 0 upon success
 */
uint32_t GetMacAddressFromOTP(char *ptr)
{
    uint32_t Result=0;                                              /* return value */
    uint16_t uCoreClock = CLKIN_VAL * ((*pPLL_CTL >> 9) & 0x3F);  /* calculate CCLK */
    uint16_t uSysClock = uCoreClock / *pPLL_DIV;                  /* calculate SCLK */
    float    fSclkPeriod = (1/(float)uSysClock) * 1000;           /* calculate SCLK period */
    uint32_t uOTP_TP1 = ( 1000 / fSclkPeriod );                   /* TP1 of timing value */
    uint32_t uOTP_TP2 = 0;                                        /* TP2 of timing value */
    uint32_t uOTP_TP3 = OTP_TP3;                                  /* TP3 of timing value */
    uint32_t uOTPTimingValue;

    /* calculate the timing value based on CCLK and SCLK */
    uOTP_TP2 = ( 120 / (u32)( 2 * fSclkPeriod ) ) << 8;

    /* compute otp timing value */
    uOTPTimingValue = uOTP_TP3 | uOTP_TP2 | uOTP_TP1;

    /* issue init */
    Result = bfrom_OtpCommand( OTP_INIT,uOTPTimingValue);

    /* check the return value */
    if (Result == 0) 
    {
        /* read mac address from OTP, this address is in reverse order */
        Result = bfrom_OtpRead(OTP_MAC_ADDRESS_PAGE,OTP_LOWER_HALF,(u64*)ptr);
    }
    return(Result);
}
#endif /* __ADSPBF527__ */

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif /* _MISRA_RULES */

