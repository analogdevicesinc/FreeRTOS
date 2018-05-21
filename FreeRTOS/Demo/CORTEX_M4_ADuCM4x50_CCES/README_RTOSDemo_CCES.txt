IDE Platform:       CCES 2.5.0 or later version
Example Name:       RTOSDemo_CCES
Support Processor:  ADuCM4x50
Test Board:         ADuCM4050FL-EZKIT Board

# Introduction
The example consist mainly of two parts:
* LED flashing demo
* FreeRTOS OS core basic demo

# Configuration
## Hardware
* ADuCM4x50 EZ-KIT Board
* ICE-1000 or ICE-2000 Debugger

Connect ICE-1000/ICE-2000 debugger to port P5 on board with usb cabel.

## Software
BSP pack of ADuCM4x50 series should be imported to CCES first.

# Running Example
The example can be executed as a normal example project using CCES or you can create a loader file and program that into flash memory.

If the example run as a normal example project by importing as existing project, the debug setting should be configured first.
* 1 In target tab of debug configuration, select Target, set processor to Analog Devices ADuCM4050, and set Interface to 
    Analog Devices ICE-1000/ICE-2000 Emulator according to your emulator.
* 2 In main tab of debug configuration, make sure CCES can find the target application. Or you can specify the target application manually.
* 3 Download the application and run it.

The demo result are:
* 1 LEDs begin to flash
* 2 The OS core demo will dump fowllowing message on console tab of CCES.
	* Success: 		Test passed.
	* Fail: 		Test failed.