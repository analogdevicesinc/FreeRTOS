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
			printf("Error %d calling %s\n", result, message); \
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
