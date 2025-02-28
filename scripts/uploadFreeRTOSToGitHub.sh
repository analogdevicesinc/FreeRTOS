#!/usr/bin/env bash

# Copyright(c) 2024 Analog Devices, Inc. All Rights Reserved. This software is
# proprietary to Analog Devices, Inc. and its licensors.

# Script to upload a release of FreeRTOS to the ADI FreeRTOS Github repository.
# The external repository already contains the upstream FreeRTOS release
# This script simply overlays our release components and then pushes them back
# upstream and provides a release tag.
# How to use this script:
# 1. Configure the variables with CAPITAL names below for your release
# 2. Download the adi freeRTOS product zip before you run this script
# 3. Run the script with no arguments
# 4. Examine the output to ensure the correct files have been added
# 5. Check the tag has been applied to the upstream repo and that all new/modified files bave been committed
# 6. Push the changes back upstream (Requires you to log into your github account)

# VARIABLES TO CONFIGURE
UPSTREAM_REPO=https://github.com/analogdevicesinc/FreeRTOS.git
UPSTREAM_BRANCH=release/FreeRTOSv10.5.x

PRODUCT_TO_UPLOAD=output/freertos_2.2.0_full
NEW_PRODUCT_TAG=REL-FRTOS-ADI-2.2.0
NEW_PRODUCT_NAME="ADI FreeRTOS Release 2.2.0"

# NO CHANGES REQUIRED FROM HERE

top_dir=`pwd`
work_dir=`pwd`/upstreaming_work_dir
upstream_local_repo_dir=${work_dir}/freertos_upstream
kit_local_dir=${work_dir}/adi_frtos_product

if [ ! -d ${top_dir}/${PRODUCT_TO_UPLOAD} ]
then
  echo "You need to ensure that the released product directory (${PRODUCT_TO_UPLOAD}) is available in the current working directory"
  exit -1
fi

if [ -d ${work_dir} ]
then
  echo "Going to remove existing work directory: ${work_dir}"
  sleep 10
  rm -rf ${work_dir}
fi
echo "Creating work directory ${work_dir}"
mkdir ${work_dir}

cd ${work_dir}
echo "Cloning upstream repo from ${UPSTREAM_REPO}"
git clone ${UPSTREAM_REPO} ${upstream_local_repo_dir}
cd ${upstream_local_repo_dir}
echo "Switching to release branch: ${UPSTREAM_BRANCH}"
git checkout -b ${UPSTREAM_BRANCH}
git pull
cd ${upstream_local_repo_dir}

# All that we copy upstream is the FreeRTOS source and demo changes.
# User guide, Licensing and test components are not uploaded.

cp -r ${top_dir}/${PRODUCT_TO_UPLOAD}/* ${upstream_local_repo_dir}/

echo "Files copied to upstream repo but NOT committed"
cd ${upstream_local_repo_dir}
echo "Here's what's going to be added:"
git status
git add -A
git commit -m "${NEW_PRODUCT_NAME}"
git tag ${NEW_PRODUCT_TAG}
echo "All done. Files committed and tagged. You need to push upstream"
echo "You need to:"
echo "    cd ${upstream_local_repo_dir}"
echo "    check that the commit happened correctly"
echo "    git push"
echo "    git push --tags"
