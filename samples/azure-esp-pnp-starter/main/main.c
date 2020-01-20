#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "freertos/event_groups.h"
#include "inc/iot_pnp_client.h"
#include "utilities/digitaltwin_client_helper.h"
#include "inc/pnp_device.h"
#include "src/sensor.c"

#define ESP_WIFI_SSID           CONFIG_WIFI_SSID
#define ESP_WIFI_PASS           CONFIG_WIFI_PASSWORD
#define ESP_WIFI_MAXIMUM_RETRY  CONFIG_ESP_WIFI_MAXIMUM_RETRY


/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about one event
 * - are we connected to the AP with an IP? */
const int WIFI_CONNECTED_BIT = BIT0;

static const char *TAG = __FILE__;
static int s_retry_num = 0;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;

        case SYSTEM_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "got ip:%s",
                     ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
            s_retry_num = 0;
            xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            if (s_retry_num < strtol(ESP_WIFI_MAXIMUM_RETRY,NULL,0)) {
                esp_wifi_connect();
                xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
                s_retry_num++;
                ESP_LOGI(TAG,"retry to connect to the AP");
            }
            ESP_LOGI(TAG,"connect to the AP fail\n");
            break;

        default:
            break;
    }
    return ESP_OK;
}

void wifi_init_sta()
{
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    s_wifi_event_group = xEventGroupCreate();
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
            .sta = {
                    .ssid = ESP_WIFI_SSID,
                    .password = ESP_WIFI_PASS
            },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );
    ESP_LOGI(TAG, "wifi_init_sta finished.");
    ESP_LOGI(TAG, "connected to AP SSID:%s",
             ESP_WIFI_SSID);
}

void init_nvs()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void azure_task(void *pvParameter)
{
    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT,
                        false, true, portMAX_DELAY);
    ESP_LOGI(TAG, "Connected to AP success!");

    pnp_telemetry_task();

    vTaskDelete(NULL);
}

void app_main()
{
    ESP_LOGI(TAG, "Azure IoT PnP Eval ... Getting Started...");
    print_heap_summary("Init");

    // 1. NVS Init
    init_nvs();
    print_heap_summary("After NVS Init");

    // 2. WiFi Connect
    wifi_init_sta();
    print_heap_summary("After WiFi connection");

    // 3. Initialize sensors
    initialize_sensors();
    print_heap_summary("After Initialize sensors");

    // 4. Initiate PnP Tasks
    if (xTaskCreate(&azure_task, "azure_task", 1024 * 5, NULL, 5, NULL) != pdPASS)
    {
        printf("create azure task failed\r\n");
    }

    // 5. Exit
    ESP_LOGI(TAG, "exiting app... " );

}
