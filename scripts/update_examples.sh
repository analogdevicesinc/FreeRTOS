
# Copyright(c) 2024 Analog Devices, Inc. All Rights Reserved. This software is
# proprietary to Analog Devices, Inc. and its licensors.

# Copy source files into appropriate locations in freestanding examples. 

copy_sources () {
	EXAMPLE=$1
	TOOLS=$2
	PART=$3

	if [ "${TOOLS}" == "IAR" ] ; then
		FRTOS=examples/${EXAMPLE}/FreeRTOS/
	else
		FRTOS=examples/${EXAMPLE}/system/FreeRTOS/
	fi

	cp FreeRTOS/Source/include/*.h ${FRTOS}/include
	cp FreeRTOS/Source/*.c ${FRTOS}
	cp FreeRTOS/Source/portable/MemMang/heap_4.c ${FRTOS}/portable/
	cp -r FreeRTOS/Source/portable/${TOOLS}/${PART} ${FRTOS}/portable/${TOOLS}/
	if [ "${TOOLS}" == "CCES" ] ; then
		cp -r FreeRTOS/Source/portable/${TOOLS}/osal ${FRTOS}
	fi
}

copy_sources ADSP-SC83x/M33/CCES/M33_FreeRTOS_Source_Core2 GCC ARM_CM33_NTZ
copy_sources ADSP-SC83x/M33/IAR/FreeRTOS_Source_Only IAR ARM_CM33_NTZ
copy_sources ADSP-SC83x/SHARC-FX/SHARCFX_FreeRTOS_Source CCES SHARC_FX
