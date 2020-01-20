#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "azure_hub_client.h"
#include "sdkconfig.h"
#include "esp_log.h"


static const char *connectionString = CONFIG_IOTHUB_CONNECTION_STRING;
const char* AZURE_TAG = "azure_iot_sdk";

IOTHUB_DEVICE_CLIENT_LL_HANDLE init_iothub_client()
{
    // Initializing Iot Hub SDK
    int iotHubInitResult = IoTHub_Init();
    if (iotHubInitResult != 0)
    {
        ESP_LOGE(AZURE_TAG,"AZURE INFO: Error initializing Azure IoT Client");
        return NULL;
    }

    // Creating IoT Hub Client Handle from Connection String
    IOTHUB_DEVICE_CLIENT_LL_HANDLE iothub_deviceclient = 
                IoTHubDeviceClient_LL_CreateFromConnectionString(connectionString, MQTT_Protocol);
    
    // Setting IoT Hub Device Client 
    // Enable verbose logging (e.g., for debugging).
    bool traceOn = true;
    bool encodeOn = true;
    (void)IoTHubDeviceClient_LL_SetOption(iothub_deviceclient, OPTION_LOG_TRACE, &traceOn);
    (void)IoTHubDeviceClient_LL_SetOption(iothub_deviceclient, OPTION_AUTO_URL_ENCODE_DECODE, &encodeOn);

    return iothub_deviceclient;
}

int send_azure_msgs(char *msgToSend, IOTHUB_DEVICE_CLIENT_LL_HANDLE iothub_deviceclient)
{
    IOTHUB_MESSAGE_HANDLE deviceMsgHandle;
    int azureSendMsgResult;

    if (iothub_deviceclient == NULL)
    {
        printf("ERROR: iothub_deviceclient is NULL!\n");
        printf("Check IoT Hub Device Connection String in Config\n");
        ESP_LOGE(AZURE_TAG, "AZURE INFO: ERROR-IoT Hub Client Handle is NULL");
        return -1;
    }
    
    // Creating IoT Hub Message 
    // NOTE: Two ways to create message, Azure accepts string based input
    //deviceMsgHandle = IoTHubMessage_CreateFromByteArray((const unsigned char *)msgToSend, sizeof(msgToSend));
    deviceMsgHandle = IoTHubMessage_CreateFromString(msgToSend);
 
    if (deviceMsgHandle == NULL)
    {
        printf("ERROR: deviceMsgHandle is NULL! Create Message failed.\n");
        ESP_LOGE(AZURE_TAG, "AZURE INFO: ERROR-Failed to Create Azure Message");
        return -1;
    }
    
    IoTHubMessage_SetContentEncodingSystemProperty(deviceMsgHandle, "utf-8");
    IoTHubMessage_SetContentTypeSystemProperty(deviceMsgHandle, "application/json");
    
    // Sending Message to IoT Hub
    azureSendMsgResult = IoTHubClient_LL_SendEventAsync(iothub_deviceclient, deviceMsgHandle, NULL, NULL);

    if (azureSendMsgResult != IOTHUB_CLIENT_OK)
    {
        printf("ERROR: IoTHubClient_LL_SendEventAsync..........FAILED!\n");
        ESP_LOGE(AZURE_TAG, "AZURE INFO: ERROR-Failed Azure Send Event");
        IoTHubMessage_Destroy(deviceMsgHandle);
        return -1;
    }
    
    time_t sendTime = time(NULL);
    printf("SUCCESS: IoTHubClient_LL_SendEventAsync accepted message for transmission to IoT Hub.\n");
    printf("Message sent at %s\n", asctime(gmtime(&sendTime)));
    ESP_LOGI(AZURE_TAG, "AZURE INFO: Completed Message successfully accepted to Azure transmission");
    
    IoTHubMessage_Destroy(deviceMsgHandle);

    return 0;
}

void process_azure_msgs(IOTHUB_DEVICE_CLIENT_LL_HANDLE iothub_deviceclient)
{
    IOTHUB_CLIENT_STATUS status;

    ESP_LOGI(AZURE_TAG, "AZURE INFO: Start Processing Azure Messages");
    printf("Do Work: Processing IoT Hub Send Message Requests %X\n", (int)iothub_deviceclient);
    while ((IoTHubClient_LL_GetSendStatus(iothub_deviceclient, &status) == IOTHUB_CLIENT_OK) 
                && (status == IOTHUB_CLIENT_SEND_STATUS_BUSY))
    {
        IoTHubClient_LL_DoWork(iothub_deviceclient);
    }
    
    ESP_LOGI(AZURE_TAG, "AZURE INFO: Stop Processing Azure Messages");
}

void destroy_iothub_client(IOTHUB_DEVICE_CLIENT_LL_HANDLE iothub_deviceclient)
{
    ESP_LOGI(AZURE_TAG, "AZURE INFO: Cleaning up - Removing Device Client");
    IoTHubClient_LL_Destroy(iothub_deviceclient);
    IoTHub_Deinit();
}