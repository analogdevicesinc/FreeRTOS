IDE Platform:		IAR 7.60
Example Name: 		RTOSDemo_IAR
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
* Install ADuCM4x50 EZ-KIT Lite BSP for IAR(PROD).
* Add a windows Environment Variable: 
   Variable name: ADI_CM4X50_BSP_PATH
   Variable value: Point to the ADuCM4x50_EZ_Kit_Lite/ directory of your Muska BSP install.

# Running Example
The example can be executed as a normal example project using IAR or you can create a loader
file and program that into flash memory. If the example is made as a normal example project 
by added as existing project for the first time, you will be asked to create a new workspace.

After making successfully, download the application and run it.

The demo result are:
* 1 LEDs begin to flash
* 2 The OS core demo will dump fowllowing message on in Terminal I/O window of IAR.
	* Success: 		Test passed.
	* Fail: 		Test failed.
