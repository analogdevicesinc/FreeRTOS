/*
 * FreeRTOS Kernel V10.4.3 LTS Patch 2
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*-----------------------------------------------------------
 * Simple parallel port IO routines.
 *-----------------------------------------------------------*/

#if defined(_MISRA_2004_RULES) || defined(_MISRA_2012_RULES)
#pragma diag(push)
#ifdef _MISRA_2012_RULES
#else
#pragma diag(suppress:misra_rule_8_7:"Symbol has global scope for permanence")
#pragma diag(suppress:misra_rule_12_8:"SRU macro uses shifts to create bitmask")
#endif /* _MISRA_2004_RULES */
#endif /* _MISRA_RULES */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Eval board includes. */
#include <services/pwr/adi_pwr.h>
#include <services/gpio/adi_gpio.h>
#include <stdlib.h>
#include <stdio.h>

/* Demo app includes. */
#include "partest.h"

#include <sru.h>

/* Initialized states for LED 9, 10 */
static bool DAI_LED_STATE[2] = { false, false };

/*-----------------------------------------------------------*/

void vParTestInitialise( void )
{
        static uint8_t gpioMemory[ADI_GPIO_CALLBACK_MEM_SIZE];
        uint32_t numCallbacks;

        if(ADI_GPIO_SUCCESS != adi_gpio_Init(gpioMemory, sizeof(gpioMemory), &numCallbacks))
        {
            printf("adi_gpio_Init failed\n");
            exit(1);
        }

        /* set GPIO output LED 7 */
        if(ADI_GPIO_SUCCESS != adi_gpio_SetDirection(ADI_GPIO_PORT_B, (ADI_GPIO_PIN_3), ADI_GPIO_DIRECTION_OUTPUT))
        {
        	printf("adi_gpio_SetDirection failed\n");
            exit(1);
        }
    	/* Initialize DAI for LED 9, 10 control */
    	SRU(HIGH,DAI0_PBEN08_I);
    	SRU(HIGH,DAI0_PBEN11_I);
    	SRU(LOW,DAI0_PB08_I);
    	SRU(LOW,DAI0_PB11_I);
}

/*-----------------------------------------------------------*/

void vParTestSetLED( UBaseType_t uxLED, BaseType_t xValue )
{

}

/*-----------------------------------------------------------*/

void vParTestToggleLED( UBaseType_t uxLED )
{
	vTaskSuspendAll();
	{
		switch( uxLED )
		{
			case 0u: adi_gpio_Toggle(ADI_GPIO_PORT_B, ADI_GPIO_PIN_3);
					break;

			case 1u:
				if(DAI_LED_STATE[uxLED-1u] == false)
				{
					SRU(HIGH,DAI0_PB11_I);
					DAI_LED_STATE[uxLED-1u] = true;
				}
				else
				{
					SRU(LOW,DAI0_PB11_I);
					DAI_LED_STATE[uxLED-1u] = false;
				}
				break;

			case 2u:
				if(DAI_LED_STATE[uxLED-1u] == false)
				{
					SRU(HIGH,DAI0_PB08_I);
					DAI_LED_STATE[uxLED-1u] = true;
				}
				else
				{
					SRU(LOW,DAI0_PB08_I);
					DAI_LED_STATE[uxLED-1u] = false;
				}
				break;

			default:
				break;
		}
	}
	xTaskResumeAll();
}

#if defined(_MISRA_2004_RULES) || defined(_MISRA_2012_RULES)
#pragma diag(pop)
#endif
