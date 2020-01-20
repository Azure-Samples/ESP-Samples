#!/bin/bash

echo "*** Setting ESP32 Dev Environment Started***"
DEV_VERSION=$1
echo "Dev version is $DEV_VERSION"

export MyCodePath=/mnt/c/azure-iot
#VERSION_PATH = "/mnt/c/azure-iot/"
echo "Code path: $MyCodePath"
printf "\n"

export IDF_STABLE_PATH=$MyCodePath/esp/stable/esp-idf
export IDF_LATEST_PATH=$MyCodePath/esp/latest/esp-idf
export IDF_8266_PATH=$MyCodePath/esp/8266/ESP8266_RTOS_SDK

if [[  -z "$DEV_VERSION" || "$DEV_VERSION" == "stable" ]]; then
   export IDF_PATH=$IDF_STABLE_PATH
   echo "IDF_PATH set to stable at: $IDF_PATH"
   VERSION_PATH=$MyCodePath/esp/stable/xtensa-esp32-elf/bin
   printf "Version path is: $VERSION_PATH\n"
fi

if [ "$DEV_VERSION" = "latest" ]; then
   export IDF_PATH=$IDF_LATEST_PATH
   echo "IDF_PATH set to latest at: $IDF_PATH"
   VERSION_PATH=$MyCodePath/esp/latest/xtensa-esp32-elf/bin
   printf "Version path is: $VERSION_PATH\n"	

fi

if [ "$DEV_VERSION" = "8266" ]; then
   export IDF_PATH=$IDF_8266_PATH
   echo "IDF_PATH set to latest at: $IDF_PATH"
   VERSION_PATH=$MyCodePath/esp/8266/xtensa-lx106-elf/bin
   printf "Version path is: $VERSION_PATH\n"	

fi

echo "Update ESP TOOLS PATH"
export IDF_TOOLS_PATH=$IDF_PATH/tools
echo "IDF_TOOLS_PATH: $IDF_TOOLS_PATH"

echo "Current Path: "
echo $PATH
printf "\n"

echo "Reset Original Path"
if [  -z "$SavePath" ]; then
   printf "NO Save Path\n"
   export SavePath=$PATH
else
  printf "Found Save Path\n"
  printf "Save Path: $SavePath\n"
  export PATH=$SavePath
fi

printf "Updated PATH back to original:\n"
echo $PATH
printf "\n"

echo "Add updated ESP IDF env variables to path"
export PATH="$VERSION_PATH:$IDF_TOOLS_PATH:$PATH"
printf "New updated PATH with ESP IDF variables: \n"
echo $PATH
printf "\n"

echo "Update python requirements for update ESP IDF\n"
/usr/bin/python -m pip install --user -r $IDF_PATH/requirements.txt
printf "\n"

echo "*** Setting ESP32 Dev Environment Completed***"