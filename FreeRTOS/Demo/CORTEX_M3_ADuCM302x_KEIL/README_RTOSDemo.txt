IDE Platform:       Keil MDK5
Example Name:       RTOSDemo
Support Processor:  ADuCM3029
Test Board:         ADuCM3029 EZ-KIT Board

# Introduction
The example consist mainly of two parts:
* LED flashing demo
* FreeRTOS OS core basic demo

# Configuration
## Hardware
* ADuCM3029 EZ-KIT Board
* SEGGER J-Link

Connect SEGGER J-Link to port P4 on board with usb cabel.

## Software
* Install latest ADuCM302x_DFP.pack

# Running Example
The example can be executed as a normal example project using Keil MDK5 or you can create a loader file and program that into flash memory.

After making successfully, download the application and run it.

The demo result are:
* 1 LEDs begin to flash
* 2 The OS core demo will dump fowllowing message from UART(speed:57600).
	* Success: 		Test passed.
	* Fail: 		Test failed.