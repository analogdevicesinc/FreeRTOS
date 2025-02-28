#!/usr/bin/env bash

# Copyright(c) 2024 Analog Devices, Inc. All Rights Reserved. This software is
# proprietary to Analog Devices, Inc. and its licensors.

# Script to upload the skinny version of a release for use by the
# FreeRTOS Add-In.
# Creates the branches and tags, needs manually pushed up

# Public REPO - Don't use until Eagle-HP is announced!
#GIT_REPO=https://github.com/analogdevicesinc/freertos-addin-rtos.git

GIT_REPO=ssh://git@bitbucket.analog.com:7999/dte-ap/freertos-addin-rtos-internal.git
REL=2.2.0

create_git_branch_for_arch () {
	ARCH=$1
	TAG_ARCH=$2

	echo "Creating branch for ${ARCH}..."
	git checkout rel_frtos_${REL}_${ARCH}
	if [ $? -ne 0 ] ; then
		git checkout -b rel_frtos_${REL}_${ARCH}
	fi
	cp -r ../freertos_${REL}_${ARCH}/* .

	git status
	git add -A
	git commit -m "FreeRTOS Version ${REL} for ${ARCH}"
	git tag -f REL-ADIFRTOS-${REL}_FREERTOSv10.4.3_${TAG_ARCH}

	git checkout main
}

cd output/git
TD=`pwd`

rm -rf repo
git clone ${GIT_REPO} repo
cd repo

create_git_branch_for_arch cortexa5 CORTEX-A5
create_git_branch_for_arch cortexa55 CORTEX-A55
create_git_branch_for_arch blackfin BLACKFIN
create_git_branch_for_arch sharc SHARC
create_git_branch_for_arch sharcfx SHARC-FX

FILE=${TD}/repo/commit_all.sh
echo "git checkout rel_frtos_${REL}_cortexa55" > ${FILE}
echo "git push --set-upstream origin rel_frtos_${REL}_cortexa55" >> ${FILE}

echo "git checkout main" >> ${FILE}
echo "git checkout rel_frtos_${REL}_cortexa5" >> ${FILE}
echo "git push --set-upstream origin rel_frtos_${REL}_cortexa5" >> ${FILE}

echo "git checkout main" >> ${FILE}
echo "git checkout rel_frtos_${REL}_blackfin" >> ${FILE}
echo "git push --set-upstream origin rel_frtos_${REL}_blackfin" >> ${FILE}

echo "git checkout main" >> ${FILE}
echo "git checkout rel_frtos_${REL}_sharc" >> ${FILE}
echo "git push --set-upstream origin rel_frtos_${REL}_sharc" >> ${FILE}

echo "git checkout main" >> ${FILE}
echo "git checkout rel_frtos_${REL}_sharcfx" >> ${FILE}
echo "git push --set-upstream origin rel_frtos_${REL}_sharcfx" >> ${FILE}
echo "git push -f --tags" >> ${FILE}

echo "All done. Files committed and tagged. Git repo details"
git remote show origin
echo "You need to push upstream:"
echo "    cd ${TD}/repo"
echo "    check that the commit happened correctly"
echo "    sh commit_all.sh"
