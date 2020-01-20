/*Based on Espressif ESP32 Azure IoT Firmware code/example*/

#ifndef _ESP_SENSORS_H_
#define _ESP_SENSORS_H_

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_system.h"
#include "iot_button.h"
#include "iot_i2c_bus.h"
#include "iot_hts221.h"
#include "iot_bh1750.h"
#include "iot_ssd1306.h"
#include "ssd1306_fonts.h"
#include "iot_mpu6050.h"

#define I2C_MASTER_SCL_IO           26          /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO           25          /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM              I2C_NUM_1   /*!< I2C port number for master dev */
#define I2C_MASTER_TX_BUF_DISABLE   0           /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0           /*!< I2C master do not need buffer */
#define I2C_MASTER_FREQ_HZ          100000      /*!< I2C master clock frequency */

#define BUTTON_IO_NUM               0
#define BUTTON_ACTIVE_LEVEL         BUTTON_ACTIVE_LOW

typedef struct {
    float temp;
    float humi;
    float lumi;
} sensor_data_t;

void page_button_init();
void i2c_bus_init();
void bh1750_init();
void hts221_init();
void mpu6050_init();
esp_err_t ssd1306_show_signs(ssd1306_handle_t dev);
void ssd1306_init();
void sensors_init();
float get_temperature_data(bool fahrenheit);
float get_humidity_data();
float get_light_luminance_data();
mpu6050_acce_value_t get_accelerometer_data();
mpu6050_gyro_value_t get_gyroscope_data();
void oled_write_temp_header();
void oled_write_temp_data();

static const char* SENSOR_TAG = "esp32_sensors";

#endif