# FreeRTOS from Analog Devices
This repository contains a copy of the FreeRTOS Real-Time Operating System with additional components from Analog Devices.
All FreeRTOS source code is provided as-is, unpacked an unmodified from the original FreeRTOS source zip/repositories unless explicitly stated.

## Getting Started
  To get started with FreeRTOS for Analog Devices processors please follow the instructions in the User Guide, which can be located in the ADI_Docs directory of the repository
### Branching Conventions
  Analog Devices provides formal releases of it's support for freeRTOS via a zip file which is available on [Analog.com](http://www.analog.com/en/design-center/processors-and-dsp/evaluation-and-development-software/freertos.html).
  These releases are provided via tags on release branches in this repository.
  Each release branch corresponds to the version of FreeRTOS which the ADI product is based.
  For example the 1.2.0 Analog Devices release was based on FreeRTOS 9.0.0, it can be found on the adi_master/release/FreeRTOSv9.0.0 branch and it is tagged using the REL-FRTOS-ADI-1.2.0 tag.

### What Platforms/Toolchains Are Supported?
  For the latest release on this branch the following platforms are supported by Analog Devices:
  * ADuCM302x
  * ADuCM4x5x
  * ADSP-BF7xx (Blackfin)
  * ADSP-SC5xx (Cortex-A Core only)

## Support
Support for the Analog Devices FreeRTOS ports is primarily provided via the [Analog Devices Engineer Zone Forum](https://ez.analog.com/community/dsp/software-and-development-tools/freertos)

## Licensing
Licensing terms for this release are detailed in the top level licensing file.
