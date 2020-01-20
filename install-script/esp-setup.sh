#!/bin/bash

echo "**** Dev Environment Setup Started ****"
printf "Update Ubuntu packages\n"
sudo apt-get update
printf "Install Prerequisites\n"
sudo apt-get install gcc git wget make libncurses-dev flex bison gperf python python-pip python-setuptools python-serial

printf "\n"
#**Save Original Path - will need this when switching between versions**
printf "Create base path variable\n"
export SavePath=$PATH
printf "SavePath Env variable set to original Path:\n"
echo $SavePath
printf "\n"

#**Set up my Code Path**
mkdir -p /mnt/c/azure-iot/
export MyCodePath=/mnt/c/azure-iot
printf "\n"
printf "\nCode path: $MyCodePath\n"
cd $MyCodePath

# **Create ESP directory**
printf "Create ESP SDK and toolchain directory\n"
mkdir -p esp
cd esp

#**Set up ESP32 Toolchain**
printf "Get ESP32 64 bit (stable) Toolchain\n"
wget https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
printf "ESP32 64 bit stable Toolchain download completed\n"
printf "\n"
printf "Get ESP32 64 bit (latest) Toolchain\n"
wget https://dl.espressif.com/dl/xtensa-esp32-elf-gcc8_2_0-esp32-2019r1-linux-amd64.tar.gz
printf "ESP32 64 bit latest Toolchain download completed\n"

#**Get ESP32 Stable SDK/APIs**
printf "\nInstall Stable version of ESP32 Toolchain & SDK v3.3\n"
cd $MyCodePath/esp
mkdir -p stable
cd stable
printf "Extract 64 bit ESP32 Toolchain (stable) to Folder\n"
tar -xzf "$MyCodePath/esp/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz"
printf "64 bit ESP32 Toolchain (stable) Extraction Complete\n"
printf "Get Stable version of ESP32 SDK v3.3\n"
git clone -b v3.3 --recursive https://github.com/espressif/esp-idf.git
printf "Stable version of ESP32 SDK v3.3 Completed\n\n"

#**Get ESP32 Latest SDK/APIs**
printf "Install Latest version of ESP32 Toolchain & SDK v4.1-dev\n"
cd $MyCodePath/esp
mkdir -p latest
cd latest 
printf "Extract 64 bit ESP32 Toolchain (latest) to Folder\n"
tar -xzf "$MyCodePath/esp/xtensa-esp32-elf-gcc8_2_0-esp32-2019r1-linux-amd64.tar.gz"
printf "64 bit ESP32 Toolchain (latest) Extraction Complete\n"
printf "Get latest version of ESP32 SDK v4.1-dev\n"
git clone --recursive https://github.com/espressif/esp-idf.git
printf "Latest version of ESP32 SDK v4.1-dev Completed\n\n"

#**Set ESP32 path for stable toolchain and ESP tools by default"
export STABLE_PATH=$MyCodePath/esp/stable
export IDF_PATH=$MyCodePath/esp/stable/esp-idf 
printf "IDF_PATH is: $IDF_PATH\n"
export IDF_TOOLS_PATH=$MyCodePath/esp/stable/esp-idf/tools
echo "IDF tools path: $IDF_TOOLS_PATH\n"
export IDF_STABLE_PATH=$MyCodePath/esp/stable/esp-idf
echo "3.3 stable IDF_STABLE_PATH: $IDF_STABLE_PATH\n"
export IDF_LATEST_PATH=$MyCodePath/esp/latest/esp-idf
printf "4.1 beta latest IDF_LATEST_PATH: $IDF_LATEST_PATH\n"

#**Setup PATH for ESP toolchain, SDK, Tools
printf "Original Path is:\n $PATH\n\n"
printf "Add ESP toolchain, SDK, and Tools to Path\n"
export PATH="$STABLE_PATH/xtensa-esp32-elf/bin:$IDF_TOOLS_PATH:$PATH"
printf "Updated path with ESP IDF and Toolchain:\n $PATH\n\n"
echo "Install/Update python requirements for ESP Stable IDF & SDK\n"
/usr/bin/python -m pip install --user -r $IDF_PATH/requirements.txt
printf "**** Dev Environment Setup Completed ****\n"


