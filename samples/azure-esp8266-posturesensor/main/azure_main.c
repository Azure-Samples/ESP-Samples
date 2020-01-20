
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/adc.h"
#include "driver/gpio.h"

#include "esp_system.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "nvs_flash.h"
#include "postureSensor.h"

#define EXAMPLE_WIFI_SSID CONFIG_WIFI_SSID
#define EXAMPLE_WIFI_PASS CONFIG_WIFI_PASSWORD

// Sets GPIO Pins for LED and Buzzer -SR
#define GPIO_OUTPUT_IO_LED 0
#define GPIO_OUTPUT_IO_BUZZER 5
#define GPIO_OUTPUT_PIN_SEL (1ULL << GPIO_OUTPUT_IO_BUZZER) | (1ULL << GPIO_OUTPUT_IO_LED)   // Bit mask -SR

// FreeRTOS event group to signal when we are connected & ready to make a request
static EventGroupHandle_t wifi_event_group;

#ifndef BIT0
#define BIT0 (0x1 << 0)
#endif
/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int CONNECTED_BIT = BIT0;
const int MSG_BUF_SIZE = 64;

// define a constant the corresponds to a 1 second delay in ticks.
const int ONE_SECOND_DELAY = 1000 / portTICK_PERIOD_MS;

// Set the delay message and the duration the device is buzzing/vibrating - SR
const TickType_t MESSAGE_DELAY_SECONDS = 1;
const int BUZZER_DURATION_SECONDS = 3;

const int GPIO_PULL_DOWN = 0;
const int GPIO_PULL_UP = 1;

IOTHUB_DEVICE_CLIENT_LL_HANDLE hubDeviceClient;

static const char *TAG = "azure";
static PostureSensor *postureSensorCurrent;

// Initialize the posture sensor -SR
static void initPostureSensor()
{
    postureSensorCurrent = malloc(sizeof(PostureSensor));
    postureSensorCurrent->desired_operating_state = false;
    postureSensorCurrent->desired_threshold = 800;
    printf("pointer: %X", (int)postureSensorCurrent);
}

// To change duration in ticks to duration in seconds -SR
static TickType_t get_duration_in_ticks(int duration_seconds)
{
    return ONE_SECOND_DELAY * duration_seconds;
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP platform WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static void initialize_gpio(void)
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO15/16
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0; 
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
    gpio_set_level(GPIO_OUTPUT_IO_BUZZER, 0);
        
}

// Method sets the Buzzer to high for the duration given -SR
static void send_to_buzzer(int seconds_to_buzz)
{
    TickType_t duration = get_duration_in_ticks(seconds_to_buzz);
    gpio_set_level(GPIO_OUTPUT_IO_BUZZER, GPIO_PULL_UP);
    vTaskDelay(duration);
    gpio_set_level(GPIO_OUTPUT_IO_BUZZER, GPIO_PULL_DOWN);
}

extern int sendMessageToAzure(char *messageToSend, IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubDeviceClient);
extern IOTHUB_DEVICE_CLIENT_LL_HANDLE getHubDeviceClient();
extern void destroyDeviceClient(IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubDeviceClient);

// The azure task where everything happens! FreeRTOS works in tasks -SR
void azure_task(void *pvParameter)
{
    TickType_t message_delay = get_duration_in_ticks(MESSAGE_DELAY_SECONDS);
    uint16_t strain_level;
    char deviceMsg[MSG_BUF_SIZE];

    initPostureSensor();

    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                        false, true, portMAX_DELAY);
    ESP_LOGI(TAG, "Connected to AP success!");

    hubDeviceClient = getHubDeviceClient();
    (void)IoTHubDeviceClient_LL_SetDeviceTwinCallback(hubDeviceClient, deviceTwinCallback, &postureSensorCurrent); 

    while (1)
    {
        if (ESP_OK == adc_read(&strain_level))
        {
            printf("\nStrain Level:  %d\n\n", strain_level);
            snprintf(deviceMsg, sizeof(deviceMsg), "{\"StrainLevel\":%d}", strain_level);

            //Send message from ADC/Strain Guage to Azure IoT Hub
            int result = sendMessageToAzure(deviceMsg, hubDeviceClient);
            if (result == 0)
            {
                printf("Message Sent Successfully\r\n");
                gpio_set_level(GPIO_OUTPUT_IO_LED, 1); // red led blinks when message sent -SR
            }
            else
            {
                printf("ERROR: Message not sent successfully\r\n");
            }
        }
        //trace statements here for debugging - SR
        //printf("\nCurrent value for desired_operating_state is: %d\n", (int)postureSensorCurrent->desired_operating_state);
        //printf("\nCurrent value for desired_threshold is: %d\n", (int)postureSensorCurrent->desired_threshold);

        if (strain_level > postureSensorCurrent->desired_threshold && postureSensorCurrent->desired_operating_state)
        {
            send_to_buzzer(BUZZER_DURATION_SECONDS);
        }

        IOTHUB_CLIENT_STATUS status;

        (void)printf("Do Work: Processing  IoT Hub Send Message Requests %X\r\n", (int)hubDeviceClient);
        while ((IoTHubClient_LL_GetSendStatus(hubDeviceClient, &status) == IOTHUB_CLIENT_OK) && (status == IOTHUB_CLIENT_SEND_STATUS_BUSY))
        {
            IoTHubClient_LL_DoWork(hubDeviceClient);
        }
        vTaskDelay(message_delay);
        gpio_set_level(GPIO_OUTPUT_IO_LED, 0);
    }
    vTaskDelete(NULL);
}

void deviceTwinCallback(DEVICE_TWIN_UPDATE_STATE update_state, const unsigned char *payLoad, size_t size, void *userContextCallback)
{
    (void)update_state;
    (void)size;
    char payload2[size];

    memcpy(payload2, payLoad, size);
    PostureSensor* newPostureSensor = parseFromJson((const char *)payload2, update_state, postureSensorCurrent);

    if (newPostureSensor != NULL)
    {
        if (postureSensorCurrent->desired_operating_state != newPostureSensor->desired_operating_state)
        {
            //handle a new value for desired operating state
            printf("\nNew value for desired_operating_state is: %d\n", (int)newPostureSensor->desired_operating_state);
            printf("Old value for desired_operating_state is: %d\n\n", (int)postureSensorCurrent->desired_operating_state);
            postureSensorCurrent->desired_operating_state = newPostureSensor->desired_operating_state;
        }

        if (postureSensorCurrent->desired_threshold != newPostureSensor->desired_threshold)
        {
            printf("New value for desired_threshold is: %d\n", newPostureSensor->desired_threshold);
            printf("Old value for desired_threshold is: %d\n", postureSensorCurrent->desired_threshold);
            postureSensorCurrent->desired_threshold = newPostureSensor->desired_threshold;
        }
    }
    free(newPostureSensor);
    return;
}

void app_main()
{
    // Initialize NVS
    adc_config_t adc_config;

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    initialise_wifi();
    initialize_gpio();

    //Initialize AD
    adc_config.mode = ADC_READ_TOUT_MODE;
    adc_config.clk_div = 8; // Sample collection clock/clk div
    ESP_ERROR_CHECK(adc_init(&adc_config));

    if (xTaskCreate(&azure_task, "azure_task", 1024 * 6, NULL, 5, NULL) != pdPASS)
    {
        printf("create azure task failed\r\n");
    }
}
