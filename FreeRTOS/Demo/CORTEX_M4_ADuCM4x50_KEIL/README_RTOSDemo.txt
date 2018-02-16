IDE Platform:		Keil MDK5
Example Name: 		RTOSDemo
Support Processor: 	ADuCM4050
Test Board: 		ADuCM4050 EZ-KIT Board

# Introduction
The example consist mainly of two parts:
* LED flashing demo
* FreeRTOS OS core basic demo

# Configuration
## Hardware
* ADuCM4050 EZ-KIT Board
* SEGGER J-Link

Connect SEGGER J-Link to port P4 on board with usb cabel.

## Software
* Install ADuCM4x50 EZ-Kit BSP for Keil version 1.0.0 or later
* Install ADuCM4x50 DFP for Keil version 1.0.0 or later

# Running Example
The example can be executed as a normal example project using Keil MDK5 or you can create a loader file and program that into flash memory.

After making successfully, download the application and run it.

The demo result are:
* 1 LEDs begin to flash
* 2 The OS core demo will dump fowllowing message from UART(speed:9600).
	* Success: 		Test passed
	* Fail: 		Test failed
