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

/* Portions Copyright (c) 2024 Analog Devices, Inc. All rights reserved. */

/*-----------------------------------------------------------
 * Simple parallel port IO routines.
 *-----------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Eval board includes. */
#include <services/pwr/adi_pwr.h>
#include <services/gpio/adi_gpio.h>

/* Demo app includes. */
#include "partest.h"

/*-----------------------------------------------------------*/

void vParTestInitialise( void )
{
	ADI_GPIO_RESULT Result;

	/*Configure the Port Pin PC_01 as output for LED blink*/
	Result = adi_gpio_SetDirection(ADI_GPIO_PORT_C,ADI_GPIO_PIN_1, ADI_GPIO_DIRECTION_OUTPUT);
	if(Result!= ADI_GPIO_SUCCESS)
	{
		printf("GPIO Initialization failed \n");
		exit(1);
	}

	/*Configure the Port Pin PC_02 as output for LED blink*/
	Result = adi_gpio_SetDirection(ADI_GPIO_PORT_C,ADI_GPIO_PIN_2, ADI_GPIO_DIRECTION_OUTPUT);
	if(Result!= ADI_GPIO_SUCCESS)
	{
		printf("GPIO Initialization failed \n");
		exit(1);
	}

	/*Configure the Port Pin PC_03 as output for LED blink*/
	Result = adi_gpio_SetDirection(ADI_GPIO_PORT_C,ADI_GPIO_PIN_3, ADI_GPIO_DIRECTION_OUTPUT);
	if(Result!= ADI_GPIO_SUCCESS)
	{
		printf("GPIO Initialization failed \n");
		exit(1);
	}
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
			case 0u:
				adi_gpio_Toggle(ADI_GPIO_PORT_C,ADI_GPIO_PIN_1);
				break;

			case 1u:
				adi_gpio_Toggle(ADI_GPIO_PORT_C,ADI_GPIO_PIN_2);
				break;

			case 2u:
				adi_gpio_Toggle(ADI_GPIO_PORT_C,ADI_GPIO_PIN_3);
				break;

			default:
                break;
		}
	}
	xTaskResumeAll();
}
