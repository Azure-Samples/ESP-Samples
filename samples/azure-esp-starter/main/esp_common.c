#include "esp_common.h"

//Declare static prototype
static esp_err_t esp32_event_handler(void *ctx, system_event_t *event);

//WIFI settings
static EventGroupHandle_t wifi_event_grp;
static int wifi_retry_num = 0;
static ip4_addr_t* esp32_ip_addr;


/******************************************************************************
 * FunctionName : esp32_event_handler
 * Description  : Callback to handle base level events from the ESP32 
 *                System Start, WIFI, etc. 
 * Parameters   : *ctx (void) 
 *                *event (system_event_t)
 * Returns      : esp_err_t
*******************************************************************************/

static esp_err_t esp32_event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) 
    {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;

        case SYSTEM_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "got ip:%s",
                    ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
            esp32_ip_addr = &event->event_info.got_ip.ip_info.ip;	
            wifi_retry_num = 0;
            xEventGroupSetBits(wifi_event_grp, WIFI_CONNECTED_BIT);
            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG,"Event State: Disconnected from WiFi");

            if (wifi_retry_num < ESP_WIFI_MAXIMUM_RETRY) 
            {
                esp_wifi_connect();
                xEventGroupClearBits(wifi_event_grp, WIFI_CONNECTED_BIT);
                wifi_retry_num++;
                ESP_LOGI(TAG,"Retry connecting to the WiFi AP");
            }
            else 
            {
                ESP_LOGI(TAG,"Connection to the WiFi AP failure\n");
            }
            break;
            

        default:
            break;
    }
    
    return ESP_OK;
}
/******************************************************************************
 * FunctionName : nvs_init
 * Description  : Initialize NVS
 * Parameters   : none
 * Returns      : esp_err_t
*******************************************************************************/
esp_err_t nvs_init()
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || 
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    printf("NVS is initialized\n");
    return ESP_OK; 
}

/******************************************************************************
 * FunctionName : wifi_init
 * Description  : Initialize WiFI
 * Parameters   : none
 * Returns      : esp_err_t
*******************************************************************************/
esp_err_t wifi_init()
{
    //Initialize WiFi
    wifi_event_grp = xEventGroupCreate();

    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_init(esp32_event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = ESP_WIFI_SSID,
            .password = ESP_WIFI_PASS
        },
    };   

    //Check for errors
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init finished.");
    ESP_LOGI(TAG, "Connected to WiFi AP-SSID:%s password:%s",
             ESP_WIFI_SSID, ESP_WIFI_PASS);
 
    return ESP_OK;
}