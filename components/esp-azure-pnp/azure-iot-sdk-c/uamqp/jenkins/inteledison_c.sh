#!/bin/bash
# Copyright (c) Microsoft. All rights reserved. Licensed under the MIT 
# license. See LICENSE file in the project root for full license 
# information.

# Tested on Ubuntu version 14.04

toolchain_root="/opt/poky-edison"

install_root="$HOME" 
build_root=$(cd "$(dirname "$0")/.." && pwd) 

cd $build_root

# ----------------------------------------------------------------------------- 
# -- helper subroutines 
# -----------------------------------------------------------------------------
checkExists() {
    if hash $1 2>/dev/null;
    then
        return 1
    else
        echo "$1" not found. Please make sure that "$1" is installed and available in the path.
        exit 1
    fi
}

# ----------------------------------------------------------------------------- 
# -- setup Intel edison Tool chain for cross compilation on Ubuntu 
# -- This method is not called in this script and kept only for showing the set of instructions needed to setup the cross compilation tool chain.
# -- Tool chain is already installed on the Jenkins slave. 
# -- In future, If slave changes ,run these commands manually on new slave to setup the tool chain. 
# -----------------------------------------------------------------------------
setupIntelEdisonToolChain() {
   
    mkdir IntelEdisonSdk
    cd IntelEdisonSdk
    wget http://downloadmirror.intel.com/25028/eng/edison-sdk-linux64-ww25.5-15.zip
    unzip edison-sdk-linux64-ww25.5-15.zip
    SdkInstallScript=$(ls | grep .sh)
    printf '\n' | ./$SdkInstallScript
    DirName=$toolchain_root/$(ls $toolchain_root)
    EnvFileName=$(DirName)/$(ls $DirName | grep environment)
    source $EnvFileName
}

# ----------------------------------------------------------------------------- 
# -- Check for environment pre-requisites.
# -- This script requires that the following programs work:
#    -- curl build-essential(g++,gcc,make) cmake git 
# -----------------------------------------------------------------------------
checkExists g++ 
checkExists gcc 
checkExists make 
checkExists cmake 
checkExists git

# ----------------------------------------------------------------------------- 
# -- Check for Intel Edison tool-chain (default directory).
# -----------------------------------------------------------------------------
if [ ! -d $toolchain_root ];
then
   echo ---------- Intel Edison tool-chain absent ----------
   exit 1
fi

# ----------------------------------------------------------------------------- 
# -- Set environment variable
# -----------------------------------------------------------------------------
DirName=$toolchain_root/$(ls $toolchain_root)
cd $DirName
cd $(ls -d */)
cd core2-32-poky-linux
export INTELEDISON_ROOT=$(pwd)

# ----------------------------------------------------------------------------- 
# -- Create toolchain-inteledison.cmake 
# -----------------------------------------------------------------------------
mkdir $build_root/cmake
cd $build_root/cmake
echo ---------- Creating toolchain cmake file ---------- 
FILE="$build_root/cmake/toolchain-inteledison.cmake" 

/bin/cat <<EOM >$FILE
INCLUDE(CMakeForceCompiler) 

SET(CMAKE_SYSTEM_NAME Linux) # this one is important 
SET(CMAKE_SYSTEM_VERSION 1) # this one not so much

# this is the location of the amd64 toolchain targeting the Intel Edison
SET(CMAKE_C_COMPILER ${INTELEDISON_ROOT}/../x86_64-pokysdk-linux/usr/libexec/i586-poky-linux/gcc/i586-poky-linux/4.9.1/gcc)

# this is the file system root of the target
SET(CMAKE_FIND_ROOT_PATH ${INTELEDISON_ROOT})

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY) 
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY) 
EOM

# ----------------------------------------------------------------------------- 
# -- Build uAMQP
# -----------------------------------------------------------------------------
echo ---------- Building the SDK by executing build.sh script ---------- 
cd $build_root/cmake
cmake .. -DCMAKE_TOOLCHAIN_FILE=toolchain-inteledison.cmake -DcompileOption_C:STRING="--sysroot=$INTELEDISON_ROOT"
make 
[ $? -eq 0 ] || exit $?
