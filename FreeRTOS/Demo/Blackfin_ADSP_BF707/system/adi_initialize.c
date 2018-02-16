/*
** adi_initialize.c source file generated on June 8, 2017 at 16:30:53.
**
** Copyright (C) 2000-2017 Analog Devices Inc., All Rights Reserved.
**
** This file is generated automatically. You should not modify this source file,
** as your changes will be lost if this source file is re-generated.
*/

#include <sys/platform.h>
#include <services/int/adi_sec.h>

#include "adi_initialize.h"

extern int32_t adi_initpinmux(void);

int32_t adi_initComponents(void)
{
	int32_t result = 0;

	result = adi_sec_Init();


	if (result == 0) {
		result = adi_initpinmux(); /* auto-generated code (order:0) */
	}

	return result;
}

