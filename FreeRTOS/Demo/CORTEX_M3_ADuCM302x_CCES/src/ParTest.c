/*
 * FreeRTOS Kernel V10.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Eval board includes. */
#include <services/pwr/adi_pwr.h>
#include <services/gpio/adi_gpio.h>
#include "common.h"

/* Demo app includes. */
#include "partest.h"

/*-----------------------------------------------------------*/

void vParTestInitialise( void )
{
        static uint8_t gpioMemory[ADI_GPIO_MEMORY_SIZE];
        if(ADI_GPIO_SUCCESS != adi_gpio_Init(gpioMemory, ADI_GPIO_MEMORY_SIZE))
        {
            DEBUG_MESSAGE("adi_gpio_Init failed\n");
        }

        /* set GPIO output LED 3, 4 and 5 */
        if(ADI_GPIO_SUCCESS != adi_gpio_OutputEnable(ADI_GPIO_PORT0, ADI_GPIO_PIN_13, true))
        {
            DEBUG_MESSAGE("adi_gpio_SetDirection failed\n");
        }
        if(ADI_GPIO_SUCCESS != adi_gpio_OutputEnable(ADI_GPIO_PORT1, (ADI_GPIO_PIN_12 | ADI_GPIO_PIN_13), true))
        {
            DEBUG_MESSAGE("adi_gpio_SetDirection failed\n");
        }
}

/*-----------------------------------------------------------*/

void vParTestSetLED( unsigned portBASE_TYPE uxLED, signed portBASE_TYPE xValue )
{

}

/*-----------------------------------------------------------*/

void vParTestToggleLED( unsigned portBASE_TYPE uxLED )
{
	vTaskSuspendAll();
	{
		switch( uxLED )
		{
			case 0: adi_gpio_Toggle(ADI_GPIO_PORT0, ADI_GPIO_PIN_13);
					break;

			case 1: adi_gpio_Toggle(ADI_GPIO_PORT1, ADI_GPIO_PIN_12);
					break;

			case 2: adi_gpio_Toggle(ADI_GPIO_PORT1, ADI_GPIO_PIN_13);
					break;

			default:
                                        break;
		}
	}
	xTaskResumeAll();
}
