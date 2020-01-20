#include <stdio.h>
#include "driver/i2c.h"
#include "esp_system.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/xlogging.h"
#include "pnp_device.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "iothub.h"
#include "iothub_device_client.h"
#include "iothub_client_options.h"
#include "iothubtransportmqtt.h"

#include "inc/utilities/digitaltwin_client_helper.h"
#include "inc/utilities/digitaltwin_serializer.h"
#include "inc/esp32AzureKit_impl.h"
#include "inc/utilities/esp32azurekit_interface.h"
#include "inc/utilities/urn_azureiot_devicemanagement_deviceinformation_interface.h"

#define IOT_HUB_CONN_STR_MAX_LEN 512
#define IOTHUB_CONNECTION_STRING      CONFIG_IOTHUB_CONNECTION_STRING
#define DIGITALTWIN_SAMPLE_DEVICE_CAPABILITY_MODEL_ID CONFIG_DIGITALTWIN_DEVICE_CAPABILITY_MODEL_ID // sample DCM for esp32: "urn:pebj:esp32AzureKit:2"

static const char *certificates = NULL;
static bool iotHubConnected = false;
static char *iotHubUri = IOTHUB_CONNECTION_STRING;
static const char *TAG = "iot_pnp_client";

// Amount to sleep between querying state from the register interface loop
static const int digitalTwinSampleDevice_registerInterfacePollSleep = 1000;

// Maximum amount of times we'll poll for interface being ready.  Current
// defaults between this and 'digitalTwinSampleDevice_registerInterfacePollSleep' mean
// we'll wait up to a minute for interface registration.
static const int digitalTwinSampleDevice_registerInterfaceMaxPolls = 60;

// Amount to sleep in the main() thread for periodic maintenance tasks of interfaces, after registration
static const int digitalTwinSampleDevice_mainPollingInterval = 1000;

// Everytime the main loop wakes up, on the digitalTwinSampleDevice_sendTelemetryFrequency(th) pass will send a telemetry message
static const int digitalTwinSampleDevice_sendTelemetryFrequency = 20;

// State of DigitalTwin registration process.  We cannot proceed with DigitalTwin until we get into the state APP_DIGITALTWIN_REGISTRATION_SUCCEEDED.
typedef enum APP_DIGITALTWIN_REGISTRATION_STATUS_TAG
{
    APP_DIGITALTWIN_REGISTRATION_PENDING,
    APP_DIGITALTWIN_REGISTRATION_SUCCEEDED,
    APP_DIGITALTWIN_REGISTRATION_FAILED
} APP_DIGITALTWIN_REGISTRATION_STATUS;

static void setup()
{
    iotHubConnected = false;

    // Initialize device model application
    ESP_LOGI(TAG, "iothuburi: %s", iotHubUri);
    if (pnp_device_initialize(iotHubUri, certificates) == 0)
    {
        iotHubConnected = true;
        ESP_LOGI(TAG, "Connected to IoTHub Successfully! ");
    }
    ESP_LOGI(TAG, "Device Registration complete.. ");
}

void pnp_telemetry_task()
{
    print_heap_summary("Before IoTHub connect:");
    setup();

    print_heap_summary("After IoTHub connect: ");
    if (iotHubConnected)
    {
        ESP_LOGI(TAG, "Connected to IotHub ... Start Pnp Device Run...");
        while (true)
        {
            pnp_device_run();
            ThreadAPI_Sleep(100);
        }
    }
    else
    {
        ESP_LOGI(TAG, "IOTHUB CONNECTION FAILED !");
    }
    pnp_device_close();
}

