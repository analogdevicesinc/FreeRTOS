/*****************************************************************************
 * RTOSDemo_CORTEX_M33_SC835_CCES_Core1.c
 *****************************************************************************/

/* Copyright(c) 2024 Analog Devices, Inc. All Rights Reserved. This software is
 * proprietary to Analog Devices, Inc. and its licensors.
 */

#include <sys/platform.h>
#include <sys/adi_core.h>
/* Managed drivers and/or services include */
#include "../system/adi_initialize.h"

/** 
 * If you want to use command program arguments, then place them in the following string. 
 */
char __argv_string[] = "";

int main(int argc, char *argv[])
{
	/* Initialize managed drivers and/or services */
	adi_initComponents();

	/**
	 * The default startup code does not include any functionality to allow
	 * core 1 to enable core 2. A convenient way to enable
	 * core 2 is to use the adi_core_enable function. 
	 */
	adi_core_enable(ADI_CORE_ARM);

	/* Begin adding your custom code here */

	return 0;
}

