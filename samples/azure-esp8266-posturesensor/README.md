# Posture sensor project

This sample uses an ESP board to create an Azure Internet of Things wearable posture sensor device. The device measures strain with a flex sensor and sends data to Azure IoT Hub about a user's posture, vibrating when the user begins to slouch. The device is also configurable through Azure IoT Hub. 

![Sarah's project](https://github.com/Azure-Samples/ESP-Samples/blob/master/media/sarah_project.JPG)

See the electric [schematic of the project](./PostureSensorSchematicLabeled.pdf).

For any questions or suggestions, please open an issue and tag Sarah Rashid,@sarahrashid

## Contents

The sample folder structure respects the structure specified by Espressif for ESP-IDF code project. See [more information about ESP-IDF build system](https://docs.espressif.com/projects/esp-idf/en/v3.3/api-guides/build-system.html).

## Prerequisites

See prerequisite in the readme at the [root of this repository](https://github.com/Azure-Samples/ESP-Samples#prerequisites). You need to setup your development environment and install the toolchain to be able to build and deploy the code to an ESP8266 Board.

Once you have cloned this repository.  Go to the root folder for the repo by typing:

```cd ESP-Samples```

Within this folder you will see four (4) subfolders

* components
* install-script
* media
* samples

Note, that the components folder here contains two (2) versions of the ESP Azure IoT component; one for Azure without using Plug and Play (PnP) and one for Azure using Plug and Play (PnP).

Go into the samples directory:

`cd samples`

Inside this folder are three projects. For this sample, we will be working with the azure-esp8266-sarah-posturesensor project.

In reviewing the contents of this directory you will see the following:

* CMakeLists.txt
* Makefile
* components folder
* main folder
* ReadMe (what you are reading now)

In order to build this sample correctly, we need to add the ESP Azure IoT component we want to use inside the sample's components directory.

Let's choose the ESP Azure IoT component that don't have PnP capabilities. To add this component to the sample following these steps:

Go to the components directory

`cd components`

Copy the esp-azure component into the samples component directory:
`cp -r ../../../components/esp-azure`

Now you are ready to build!

## Build and deploy the sample

Navigate to the sample folder and run `make menuconfig`.

This command will display a configuration screen. In this screen complete the information needed for this project:

1. Specify the Wi-Fi connection (SSID and Password)
2. Specify the IoT Hub configuration (SAS Key)
3. Specify the port to be used for flashing the device

   > Note: the port to use to communicate with the board will depend on your machine configuration. When using WSL on Windows, look in the device manager to see which COM port is used when you plug it in. In the case of the "ESP32 Azure IoT Kit, the board uses the USB to UART Bridge.

   ![DeviceManager](https://github.com/Azure-Samples/ESP-Samples/blob/master/media/deviceManager.JPG)

   In the case above, one should use /dev/ttyS5, as the port COM used in Windows is COM5. See more about [serial communication](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/establish-serial-connection.html#connect-esp32-to-pc) with ESP32.

![ESP32 sample](https://github.com/Azure-Samples/ESP-Samples/blob/master/media/cfg_menu.JPG)

![ESP32 sample](https://github.com/Azure-Samples/ESP-Samples/blob/master/media/cfg_IoTHub.JPG)

> Note: all the configuration made with menuconfig option, is placed in the file: Kconfig.projbuild. You can edit it manually to set the default value.

Once configured, you can build it running `make`.

Once built, you can flash your ESP Board from your computer by running `make flash monitor`.

This command will flash the board with your code (upload) and start it. The monitor argument displays all the messages from your code (notably ESP-IDF Log trace and printf()) in the console.

> Note: For this sample we used make, you can also use CMake. In this case see documentation about [ESP CMake build system](https://docs.espressif.com/projects/esp-idf/en/v3.3/api-guides/build-system-cmake.html). and use `idf.py` instead of `make` in the previous commands.
