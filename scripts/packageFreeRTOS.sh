#!/bin/bash

# Copyright(c) 2016-2024 Analog Devices, Inc. All Rights Reserved. This
# software is proprietary to Analog Devices, Inc. and its licensors.

# Script to zip up files produced by ADI for FreeRTOS product
# Creates 7 output zips:
# 1. Full downloadable zip for all parts
#    Source-only part specific zips as used by the Add-In for:
# 2. ARM A5
# 3. ARM A55
# 4. ARM M33
# 5. Blackfin BF70x
# 6. SHARC+
# 7. SHARC-FX


####   Functions    ####

copy_sources_common () {
	# Copy the common source structure into an output directory
	DST=$1
	OSAL=$2
	
	mkdir -p ${DST}/include         \
            ${DST}/portable/CCES
            
	cp ${FREERTOS_DIR}/Source/*.c ${DST}/
	cp ${FREERTOS_DIR}/Source/include/*.h ${DST}/include/
	cp ${FREERTOS_DIR}/Source/portable/MemMang/heap_4.c ${DST}/portable/
	if [ ${OSAL} -eq 1 ] ; then
		cp -r ${FREERTOS_DIR}/Source/portable/CCES/osal ${DST}/portable/CCES/
	fi
}

create_license_and_readme () {
	# Copy license file into output directory
	DST=$1
	SKINNY=$2
	DATE=`date +%B-%Y`
	
	cp ${FREERTOS_DIR}/LICENSE ${DST}
	cp ${FREERTOS_DIR}/NOTICE ${DST}

	# Create README.md
	if [ ${SKINNY} -eq 1 ] ; then
		# For skinny Add-In Repo
		echo "# FreeRTOS Add-In Kernel sources" > ${DST}/README.md
		echo "Bare bones FreeRTOS OS for ADI platforms only." >> ${DST}/README.md
   	echo "Only to be used from within the FreeRTOS Add-In." >> ${DST}/README.md
	else
		# For main repo
		echo "# FreeRTOS Kernel sources" > ${DST}/README.md
	fi
	# For both repos
   echo "" >> ${DST}/README.md
   echo "## Documentation" >> ${DST}/README.md
   echo "See the [ADI FreeRTOS Wiki](https://wiki.analog.com/resources/tools-software/freertos/) for information." >> ${DST}/README.md
   echo "" >> ${DST}/README.md
   echo "## Support" >> ${DST}/README.md
   echo "Support for the Analog Devices FreeRTOS ports is primarily provided via the [Analog Devices Engineer Zone Forum](https://ez.analog.com/community/dsp/software-and-development-tools/freertos)" >> ${DST}/README.md
   echo "" >> ${DST}/README.md
   echo "ADI version ${RELEASE} based on FreeRTOS version ${FREERTOS_VERSION}.\\" >> ${DST}/README.md
   echo "Date: ${DATE}\\" >> ${DST}/README.md
   echo "Hash: ${HASH}" >> ${DST}/README.md
}

copy_sources_part () {
	# Copy the part specific sources into an output directory
	DSP=$1
	PART=$2

	mkdir -p ${DST}/portable/${PART}
	cp -r ${FREERTOS_DIR}/Source/portable/${PART}/* ${DST}/portable/${PART}
}

copy_full_package () {
	# Copy everything needed for standalone zip
	DST=$1

	cp -r ${FREERTOS_DIR}/* ${DST}
	create_license_and_readme ${DST} 0 
}

generate_manifest () {
	# Create a list of files in the directory
	DIR=$1
	FILE=$2

	cd ${DIR}
	find .  -type f > ${TD}/${OUT_DIR}/${FILE}
	cd ${TD}
}

archive_output () {
	# Create a zip file of the output directories
	DIR=$1
	FILE=$2

	cd ${DIR}
	zip -9r ../${FILE} *
   cd ${TD}

}

####   Main script    ####

if [ ! -d "FreeRTOS" ]
then
  echo "Error: Script should be run from the top level where FreeRTOS dir"
  echo " exists"
  exit -1
fi

TD=`pwd`
 
RELEASE=2.2.0
FREERTOS_VERSION="10.5.1"
HASH=`git rev-parse HEAD`
OUT_DIR=output
OUT_DIR_FULL=${OUT_DIR}/freertos_${RELEASE}_full
OUT_DIR_A5=${OUT_DIR}/git/freertos_${RELEASE}_cortexa5
OUT_DIR_A55=${OUT_DIR}/git/freertos_${RELEASE}_cortexa55
OUT_DIR_M33=${OUT_DIR}/git/freertos_${RELEASE}_m33
OUT_DIR_M33_IAR=${OUT_DIR}/git/freertos_${RELEASE}_m33_IAR
OUT_DIR_BF=${OUT_DIR}/git/freertos_${RELEASE}_blackfin
OUT_DIR_SHARC=${OUT_DIR}/git/freertos_${RELEASE}_sharc
OUT_DIR_SHARCFX=${OUT_DIR}/git/freertos_${RELEASE}_sharcfx
FREERTOS_DIR=FreeRTOS

rm -rf ${OUT_DIR}

mkdir -p ${OUT_DIR_FULL} ${OUT_DIR_A5} ${OUT_DIR_A55} ${OUT_DIR_BF} ${OUT_DIR_SHARC}

echo "Copying sources..."
# Populate A5 support
copy_sources_common ${OUT_DIR_A5} 1 
copy_sources_part ${OUT_DIR_A5} "CCES/ARM_CA5" 
copy_sources_part ${OUT_DIR_A5} "GCC/ARM_CA9" 
create_license_and_readme ${OUT_DIR_A5} 1 
generate_manifest ${OUT_DIR_A5} manifest.a5.txt

# Populate A55 support
copy_sources_common ${OUT_DIR_A55} 1
copy_sources_part ${OUT_DIR_A55} "CCES/ARM_CA55" 
create_license_and_readme ${OUT_DIR_A55} 1
generate_manifest ${OUT_DIR_A55} manifest.a55.txt

# Populate M33 support
copy_sources_common ${OUT_DIR_M33} 0
copy_sources_part ${OUT_DIR_M33} "GCC/ARM_CM33_NTZ"
create_license_and_readme ${OUT_DIR_M33} 1
generate_manifest ${OUT_DIR_M33} manifest.m33.txt

# Populate M33 (IAR) support
copy_sources_common ${OUT_DIR_M33_IAR} 0
copy_sources_part ${OUT_DIR_M33_IAR} "IAR/ARM_CM33_NTZ"
create_license_and_readme ${OUT_DIR_M33_IAR} 1
generate_manifest ${OUT_DIR_M33_IAR} manifest.m33.iar.txt

# Populate Blackfin support
copy_sources_common ${OUT_DIR_BF} 1
copy_sources_part ${OUT_DIR_BF} "CCES/Blackfin_BF70x" 
create_license_and_readme ${OUT_DIR_BF} 1
generate_manifest ${OUT_DIR_BF} manifest.bf.txt


# Populate SHARC support
copy_sources_common ${OUT_DIR_SHARC} 1
create_license_and_readme ${OUT_DIR_SHARC} 1 
copy_sources_part ${OUT_DIR_SHARC} "CCES/SHARC_215xx" 
generate_manifest ${OUT_DIR_SHARC} manifest.sharc.txt

# Populate SHARC-FX support
copy_sources_common ${OUT_DIR_SHARCFX} 1
create_license_and_readme ${OUT_DIR_SHARCFX} 1 
copy_sources_part ${OUT_DIR_SHARCFX} "CCES/SHARC_FX" 
generate_manifest ${OUT_DIR_SHARCFX} manifest.sharcfx.txt

# Populate the full package
copy_full_package ${OUT_DIR_FULL}
generate_manifest ${OUT_DIR_FULL} manifest.full.txt


# Don't compress the archives since for now.
#echo "Compressing sources..."
# Create zip files for each package
#archive_output ${OUT_DIR_FULL} freertos_${RELEASE}.zip 
# Archives for the part specific builds shouldn't be necessary
#archive_output ${OUT_DIR_A5} freertos_${RELEASE}_cortexa5.zip 
#archive_output ${OUT_DIR_A55} freertos_${RELEASE}_cortexa55.zip 
#archive_output ${OUT_DIR_M33} freertos_${RELEASE}_m33.zip 
#archive_output ${OUT_DIR_BF} freertos_${RELEASE}_bf.zip 
#archive_output ${OUT_DIR_SHARC} freertos_${RELEASE}_sharc.zip 
#archive_output ${OUT_DIR_SHARCFX} freertos_${RELEASE}_sharcfx.zip 
