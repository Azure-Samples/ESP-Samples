/**
 * Name:            main.c
 * Description:     Source for entry point to application which 
 *                  send sensor telemetry and heap information to Azure
 *                  from ESP32 device
 * Initial Date:    11.01.2019
**/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "sdkconfig.h"

#include "esp_common.h"
#include "azure_hub_client.h"
#include "esp_sensors.h"



/******************************************************************************
 * FunctionName : board_init
 * Description  : Call functions to initialize ESP32 NVS, TCPIP Adapter/WIFI
 *                and Device Sensors
 * Parameters   : none
 * Returns      : void
*******************************************************************************/
void board_init()
{
    esp_err_t retvalue;
    retvalue = nvs_init();
    ESP_ERROR_CHECK(retvalue);

    retvalue = wifi_init();
    ESP_ERROR_CHECK(retvalue);

    sensors_init(); 
}

/******************************************************************************
 * FunctionName : azure_task
 * Description  : Task for sending data to Azure 
 * Parameters   : pvParameter
 * Returns      : none
*******************************************************************************/
void azure_task(void *pvParameter)
{
    char device_message[128]; 
    int counter = 0;
    float temperature, 
          humidity,
          luminance;
    uint32_t free_heap_sz,
             min_free_heap_sz;

    TickType_t msgdelay = ONE_SECOND_DELAY *5;
    mpu6050_acce_value_t accel_data;

    IOTHUB_DEVICE_CLIENT_LL_HANDLE azure_client = init_iothub_client();
    if (azure_client == NULL)
    {
        ESP_LOGE(TAG, "APP INFO: No Azure IoT Hub Handle was obtained");
        vTaskDelete(NULL);
        return;
    }

    while (1)
    {
       //Get Heap Info 
       free_heap_sz = esp_get_free_heap_size();
       min_free_heap_sz = esp_get_minimum_free_heap_size();
       ESP_LOGI(TAG, "Free Heap Size: %lu", (unsigned long)free_heap_sz);
       ESP_LOGI(TAG, "Minimum Free Heap Size: %lu", (unsigned long)min_free_heap_sz);

       //Get Sensor Data - #TODO Build generic get sensor data 
       //& update to use Sensor struct  - TEW
        temperature = get_temperature_data(false);  //leave as celsius
        humidity = get_humidity_data();
        luminance = get_light_luminance_data();
        ESP_LOGI(TAG, "temperature: %.1f", temperature);
        ESP_LOGI(TAG, "humidity: %.1f", humidity);
        ESP_LOGI(TAG, "luminance: %.1f", luminance);
        
        accel_data = get_accelerometer_data();
        ESP_LOGI(TAG, "Accelerometer Data- x: %.2f, y: %.2f, z: %.2f", 
                      accel_data.acce_x, accel_data.acce_y, accel_data.acce_z);        

        /* Create message to send to Azure IoT  - TEW*/
        snprintf(device_message, sizeof(device_message),
              "{\"MessageNum\": %d, \"FreeHeap\": %lu, \"Temperature\": %.1f, \"Humidity\": %.1f, \"Luminance\": %.1f}", 
                 ++counter, (unsigned long)free_heap_sz, temperature,
                 humidity, luminance);
        
        //Send message to Azure IoT Hub
        int sendresult = send_azure_msgs(device_message, azure_client);
        if (sendresult == 0)
        {
            printf("APP INFO: Azure Message Sent Successfully\n");
            ESP_LOGI(TAG, "SUCCESS: Message Queued to be sent to Azure");
        }
        else
        {
            printf("APP INFO: Error - Message not sent successfully\n");
            ESP_LOGE(TAG, "ERROR: Message Not Queued for Azure");
        }

        //Process Any Messages
        process_azure_msgs(azure_client);
        vTaskDelay(ONE_SECOND_DELAY *2);

        //Delay for 5 seconds
        vTaskDelay(msgdelay);     
    }
    
    //Clean up Azure IoT Connection
    destroy_iothub_client(azure_client);

    vTaskDelete(NULL);
}

/******************************************************************************
 * FunctionName : show_sensor_task
 * Description  : Task for sending temp, humidity, and brightness 
 *                to OLED/SSD1306 - Lower Task priority 
 * Parameters   : pvParameter
 * Returns      : none
*******************************************************************************/
void show_sensor_task(void *pvParameter)
{
    //If want to add MPU sensor and button - Insert Your Code Here
    
    oled_write_temp_header();

    while(1)
    {
        oled_write_temp_data();
        vTaskDelay(ONE_SECOND_DELAY *3);
    }
    
    vTaskDelete(NULL);  
}

/******************************************************************************
 * FunctionName : app_main
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void app_main()
{
    int taskResult;
    
    board_init();
    vTaskDelay(ONE_SECOND_DELAY);
    ESP_LOGI(TAG, "ESP32 Board Initialized...");
   
   //High Priority Task
    taskResult = xTaskCreate(&azure_task, "azure_task", 1024 * 6, NULL, 5, NULL); 
    if (taskResult != pdPASS)
    {   
        ESP_LOGE(TAG, "Create azure task failed"); 
    }

    //Low Medium Priority Task
    taskResult = xTaskCreate(&show_sensor_task, "show_sensor_task", 1024 * 4 , NULL, 3, NULL); 
    if (taskResult != pdPASS)
    {   
        ESP_LOGE(TAG, "Create show sensor task failed"); 
    }


}

