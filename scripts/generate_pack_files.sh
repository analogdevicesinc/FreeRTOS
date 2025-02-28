
# Copyright(c) 2024 Analog Devices, Inc. All Rights Reserved. This software is
# proprietary to Analog Devices, Inc. and its licensors.

cd pack
PACKS=`ls`
cd ..

TD=`pwd`

mkdir -p "packs"

for PACK in ${PACKS}
do

	echo "Generating: ${PACK}"
	cd pack/${PACK}

	# Update common files from main source dir
	cp ${TD}/FreeRTOS/Source/portable/CCES/osal/* Source/osal/

	PDSC=`ls *.pdsc`
	BASE=`basename ${PDSC} .pdsc`

	# Pull version number out of PDSC file. 
	VERSION=0.0.0
	CHUNKS=`grep "Cversion" ${PDSC} | tr " " "\n"`
	for CHUNK in ${CHUNKS}
	do
		if [ `echo ${CHUNK} | grep -c Cversion` -eq 1 ] ; then
			VERSION=`echo ${CHUNK} | sed -e 's/Cversion=//' | sed -e 's/"//g'`
		fi
	done

	PACK="${BASE}.${VERSION}.pack"


	# create new pack file
	rm -f ${PACK}
	zip -r9 ${TD}/packs/${PACK} *

	echo "Generated: ${PACK}"
	cd ${TD}

done
