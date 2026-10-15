/*****************************************************************************
 * SHARCFX_ADSP_SC846_CCES_Core0.c
 *****************************************************************************/

#include "adi_initialize.h"
#include "SHARCFX_ADSP_SC846_CCES_Core0.h"

#include <sys/platform.h>
#include <sys/adi_core.h>

int main()
{
	/**
	 * Initialize managed drivers and/or services that have been added to 
	 * the project.
	 * @return zero on success 
	 */
	adi_initComponents();
	
	/**
	 * The default startup code does not include any functionality to allow
	 * core 0 to enable core 1 and core 2. A convenient way to enable
	 * core 1 and core 2 is to use the adi_core_enable function. 
	 */
	adi_core_enable(ADI_CORE_SHARC0);

	/* Begin adding your custom code here */

	return 0;
}

