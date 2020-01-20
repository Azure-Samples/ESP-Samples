#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/platform.h"
#include "iothub_device_client.h"
#include "iothub_client_options.h"
#include "iothub.h"
#include "iothub_message.h"
#include "parson.h"
#include "sdkconfig.h"
#include "iothubtransportmqtt.h"
#include "iothub_client_ll.h"
#include "certs.h"

static const char *connectionString = CONFIG_IOTHUB_CONNECTION_STRING;

IOTHUB_DEVICE_CLIENT_LL_HANDLE getHubDeviceClient()
{
    // Initializing Iot Hub SDK
    int iotHubInitResult = IoTHub_Init();
    if (iotHubInitResult != 0)
    {
        (void)printf("Failed to initialize the IoT Hub SDK platform.\r\n");
        return -1;
    }
    // Creating IoT Hub Client Handle from Connection String
    IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubDeviceClient = IoTHubDeviceClient_LL_CreateFromConnectionString(connectionString, MQTT_Protocol);
    

    // Setting IoT Hub Device Client 
    // Enable verbose logging (e.g., for debugging).
    bool traceOn = true;
    bool encodeOn = true;
    (void)IoTHubDeviceClient_LL_SetOption(iotHubDeviceClient, OPTION_LOG_TRACE, &traceOn);
    (void)IoTHubDeviceClient_LL_SetOption(iotHubDeviceClient, OPTION_AUTO_URL_ENCODE_DECODE, &encodeOn);

    return iotHubDeviceClient;
}

int sendMessageToAzure(char *messageToSend, IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubDeviceClient)
{
    IOTHUB_MESSAGE_HANDLE deviceMsgHandle;
    int azureSendMsgResult;

    if (iotHubDeviceClient == NULL)
    {
        (void)printf("ERROR: iotHubDeviceClient is NULL!\r\n");
        (void)printf("Check IoT Hub Device Connection String in Config\r\n");
        return -1;
    }
    else
    {
        // Creating IoT Hub Message Byte Array
        deviceMsgHandle = IoTHubMessage_CreateFromByteArray((const unsigned char *)messageToSend, strlen(messageToSend));
        if (deviceMsgHandle == NULL)
        {
            (void)printf("ERROR: deviceMsgHandle is NULL! Create Msg Byte Array failed.\r\n");
            return -1;
        }
        else
        {
            IoTHubMessage_SetContentEncodingSystemProperty(deviceMsgHandle, "utf-8");
            IoTHubMessage_SetContentTypeSystemProperty(deviceMsgHandle, "application/json");
            // Sending Message to IoT Hub
            
            azureSendMsgResult = IoTHubClient_LL_SendEventAsync(iotHubDeviceClient, deviceMsgHandle, NULL, NULL);

            if (azureSendMsgResult != IOTHUB_CLIENT_OK)
            {
                (void)printf("ERROR: IoTHubClient_LL_SendEventAsync..........FAILED!\r\n");
                return -1;
            }
            else
            {
                time_t sendTime = time(NULL);
                (void)printf("SUCCESS: IoTHubClient_LL_SendEventAsync accepted message for transmission to IoT Hub.\r\n");
                (void)printf("Message sent at %s\r\n", asctime(gmtime(&sendTime)));
            }
        }
    }

    return 0;
}

void destroyDeviceClient(IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubDeviceClient)
{
    IoTHubClient_LL_Destroy(iotHubDeviceClient);
    iotHubDeviceClient = NULL;

    IoTHub_Deinit();
}