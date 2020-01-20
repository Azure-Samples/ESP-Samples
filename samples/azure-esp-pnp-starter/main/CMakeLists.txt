
set (IOT-PNP "${CMAKE_CURRENT_LIST_DIR}")

set(COMPONENT_ADD_INCLUDEDIRS "."
        "inc"
        "inc/utilities"
        )

set(COMPONENT_SRCS "main.c"
        "src/esp32AzureKit_impl.c"
        "src/pnp_device.c"
        "src/iot_pnp_client.c"
        "src/utilities/digitaltwin_client_helper.c"
        "src/utilities/digitaltwin_serializer.c"
        "src/utilities/esp32azurekit_interface.c"
        "src/utilities/urn_azureiot_devicemanagement_deviceinformation_interface.c"
        "src/sensor.c"
        )

register_component()
