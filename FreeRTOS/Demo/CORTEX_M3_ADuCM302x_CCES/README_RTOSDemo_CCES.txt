IDE Platform:       CCES 2.5.0
Example Name:       RTOSDemo_CCES
Support Processor:  ADuCM3029
Test Board:         ADuCM3029 EZ-KIT Board

# Introduction
The example consist mainly of two parts:
* LED flashing demo
* FreeRTOS OS core basic demo

# Configuration
## Hardware
* ADuCM3029 EZ-KIT Board
* ICE1000 Debugger

Connect ICE1000 debugger to port P5 on board with usb cabel.

## Software
ADuCM3029 bsp can be used by config CCES System Configuration.

# Running Example
The example can be executed as a normal example project using CCES or you can create a loader file and program that into flash memory.

If the example run as a normal example project by importing as existing project, the debug setting should be configured first.
* 1 In target tab of debug configuration, select Target, set processor to Analog Devices ADuCM3029, and set Interface to Analog Devices ICE-1000 Emulator.
* 2 In main tab of debug configuration, make sure CCES can find the target application. Or you can specify the target application manually.
* 3 Download the application and run it.

The demo result are:
* 1 LEDs begin to flash
* 2 The OS core demo will dump fowllowing message on console tab of CCES.
	* Success: 		Test passed.
	* Fail: 		Test failed.