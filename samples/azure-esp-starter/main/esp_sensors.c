/*Based on Espressif ESP32 Azure IoT Firmware code/example*/

#include "esp_log.h"
#include "esp_sensors.h"

/*********************
***** Variables ******
**********************/
static button_handle_t page_btn = NULL;
static i2c_bus_handle_t i2c_bus = NULL;
static bh1750_handle_t bh1750_dev = NULL;
static hts221_handle_t hts221_dev = NULL;
static ssd1306_handle_t ssd1306_dev = NULL;
static mpu6050_handle_t mpu6050_dev = NULL;

static xQueueHandle q_page_num;
static uint8_t g_page_num = 0;

//static complimentary_angle_t complimentary_angle;


/**Button Functions**/
/******************************************************************************
 * FunctionName : page_btn_tap_cb
 * Description  : Function to respond to tap to user defined button  
 *                QueueSend to send button 'page'/tap number
 * Parameters   : none
 * Returns      : void
*******************************************************************************/
void page_btn_tap_cb()
{
    ESP_LOGI(SENSOR_TAG, "User defined Button was pushed");
    g_page_num++;
    if (g_page_num >= 4) {
        g_page_num = 0;
    }
    xQueueSend(q_page_num, &g_page_num, 0);
}

/******************************************************************************
 * FunctionName : page_button_init
 * Description  : Function to init user defined button  
 * Parameters   : none
 * Returns      : void
*******************************************************************************/
void page_button_init()
{
    ESP_LOGI(SENSOR_TAG, "STARTED: Button initialization");
    page_btn = iot_button_create((gpio_num_t)BUTTON_IO_NUM, BUTTON_ACTIVE_LEVEL);
    iot_button_set_evt_cb(page_btn, BUTTON_CB_TAP, page_btn_tap_cb, "TAP");
    ESP_LOGI(SENSOR_TAG, "COMPLETED: Button initialization");
}

/**I2C Bus**/
/******************************************************************************
 * FunctionName : i2c_bus_init
 * Description  : Function to init I2C bus  
 * Parameters   : none
 * Returns      : void
*******************************************************************************/
void i2c_bus_init()
{
    ESP_LOGI(SENSOR_TAG, "STARTED: I2C initialization");

    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = (gpio_num_t)I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = (gpio_num_t)I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_bus = iot_i2c_bus_create(I2C_MASTER_NUM, &conf);
    
    ESP_LOGI(SENSOR_TAG, "COMPLETED: I2C initialization");
}

/**BH1750 - Light Brightness/Luminance**/
/******************************************************************************
 * FunctionName : bh1750_init
 * Description  : Function to init BH17590 Sensor  
 * Parameters   : none
 * Returns      : void
*******************************************************************************/
void bh1750_init()
{
    ESP_LOGI(SENSOR_TAG, "STARTED: BH1750 initialization");

    bh1750_dev = iot_bh1750_create(i2c_bus, BH1750_I2C_ADDRESS_DEFAULT);
    iot_bh1750_power_on(bh1750_dev);
    iot_bh1750_set_measure_mode(bh1750_dev, BH1750_CONTINUE_4LX_RES);

    ESP_LOGI(SENSOR_TAG, "COMPLETED: BH1750 initialization");
}

/**HTS221 - Humidity and Temperature**/
/******************************************************************************
 * FunctionName : hts221_init
 * Description  : Function to init HTS221 Sensor  
 * Parameters   : none
 * Returns      : void
*******************************************************************************/
void hts221_init()
{
    ESP_LOGI(SENSOR_TAG, "STARTED: HTS221 initialization");

    hts221_dev = iot_hts221_create(i2c_bus, HTS221_I2C_ADDRESS);

    hts221_config_t hts221_config;    
    iot_hts221_get_config(hts221_dev, &hts221_config);

    hts221_config.avg_h = HTS221_AVGH_32;
    hts221_config.avg_t = HTS221_AVGT_16;
    hts221_config.odr = HTS221_ODR_1HZ;
    hts221_config.bdu_status = HTS221_DISABLE;
    hts221_config.heater_status = HTS221_DISABLE;
    iot_hts221_set_config(hts221_dev, &hts221_config);
    
    iot_hts221_set_activate(hts221_dev);
    
    ESP_LOGI(SENSOR_TAG, "COMPLETED: HTS221 initialization");
}

/**MPU6050 - Accelerometer and Gyroscope **/
/******************************************************************************
 * FunctionName : mpu6050_init
 * Description  : Function to init MPU6050 Sensor  
 * Parameters   : none
 * Returns      : void
*******************************************************************************/
void mpu6050_init()
{
    ESP_LOGI(SENSOR_TAG, "STARTED: MPU6050 initialization");

    mpu6050_dev = iot_mpu6050_create(i2c_bus, MPU6050_I2C_ADDRESS);
    iot_mpu6050_wake_up(mpu6050_dev);
    iot_mpu6050_set_acce_fs(mpu6050_dev, ACCE_FS_4G);
    iot_mpu6050_set_gyro_fs(mpu6050_dev, GYRO_FS_500DPS);

    ESP_LOGI(SENSOR_TAG, "COMPLETED: MPU6050 initialization");
}

/**SSD1306 - OLED Display**/
/******************************************************************************
 * FunctionName : ssd1306_show_signs()
 * Description  : Function to write symbols to OLED sensor as screen header  
 * Parameters   : dev (ssd1306_handle_t) 
 * Returns      : void
*******************************************************************************/
esp_err_t ssd1306_show_signs(ssd1306_handle_t dev)
{
    ESP_LOGI(SENSOR_TAG, "Writing Header Symbols to SSD1306 OLED sensor");

    iot_ssd1306_draw_bitmap(dev, 0, 2, &c_chSingal816[0], 16, 8);
    iot_ssd1306_draw_bitmap(dev, 24, 2, &c_chBluetooth88[0], 8, 8);
    //iot_ssd1306_draw_bitmap(dev, 40, 2, &c_chMsg816[0], 16, 8);
    //iot_ssd1306_draw_bitmap(dev, 64, 2, &c_chGPRS88[0], 8, 8);
    //iot_ssd1306_draw_bitmap(dev, 90, 2, &c_chAlarm88[0], 8, 8);
    iot_ssd1306_draw_bitmap(dev, 112, 2, &c_chBat816[0], 16, 8);

    return iot_ssd1306_refresh_gram(dev);
}

/******************************************************************************
 * FunctionName : ssd1306_init()
 * Description  : Function to init SSD1306 Sensor  
 * Parameters   : none
 * Returns      : void
*******************************************************************************/
void ssd1306_init()
{
    ESP_LOGI(SENSOR_TAG, "STARTED: SSD1306 initialization");

    ssd1306_dev = iot_ssd1306_create(i2c_bus, SSD1306_I2C_ADDRESS);
    iot_ssd1306_refresh_gram(ssd1306_dev);
    iot_ssd1306_clear_screen(ssd1306_dev, 0x00);
    ssd1306_show_signs(ssd1306_dev);

    ESP_LOGI(SENSOR_TAG, "COMPLETED: SSD1306 initialization");
}

/******************************************************************************
 * FunctionName : sensors_init
 * Description  : Function to initialize all of the onboard sensors
 *                button, i2c bus, bh1750, hts221, ssd1306, mpu6050
 * Parameters   : none
 * Returns      : void
*******************************************************************************/
void sensors_init()
{
    ESP_LOGI(SENSOR_TAG, "STARTED: Sensors initialization");

    page_button_init();
    i2c_bus_init();
    bh1750_init();
    hts221_init();
    ssd1306_init();
    mpu6050_init();

    ESP_LOGI(SENSOR_TAG, "COMPLETED: Sensors initialization");
}

/******************************************************************************
 * FunctionName : get_temperature_data
 * Description  : Getting Temperature from HTS221 sensor and returns float as
 *                celsius or fahrenheit; Pass bool, true == fahrenheit
 * Parameters   : fahrenheit (bool)
 * Returns      : float
*******************************************************************************/
float get_temperature_data(bool fahrenheit)
{
    ESP_LOGI(SENSOR_TAG, "Getting Temperature from HTS221 sensor");

    int16_t temp = 0;
    float temp_result = 0;

    iot_hts221_get_temperature(hts221_dev, &temp);

    temp_result = fahrenheit ? ((float)temp/10) * 9/5 + 32 
                             : (float)temp/10;
   
    return temp_result;
}

/******************************************************************************
 * FunctionName : get_humidity_data
 * Description  : Getting Humidity from HTS221 sensor return value as float
 * Parameters   : none
 * Returns      : float
*******************************************************************************/
float get_humidity_data()
{
    ESP_LOGI(SENSOR_TAG, "Getting Humidity from HTS221 sensor");

    int16_t humi;
    iot_hts221_get_humidity(hts221_dev, &humi);
    return (float)humi/10;
}

/******************************************************************************
 * FunctionName : get_light_luminance_data
 * Description  : Getting the light luminance/brightness from BH1750 sensor
 * Parameters   : none
 * Returns      : float
*******************************************************************************/
float get_light_luminance_data()
{
    ESP_LOGI(SENSOR_TAG, "Getting Luminance from BH1750 sensor");

    float lumi;
    iot_bh1750_get_data(bh1750_dev, &lumi);
    return lumi;
}

/******************************************************************************
 * FunctionName : get_accelerometer_data
 * Description  : Getting Accelerometer data from MPU6050 sensor
 * Parameters   : none
 * Returns      : mpu6050_acce_value_t (struct of floats)
*******************************************************************************/
mpu6050_acce_value_t get_accelerometer_data()
{
    ESP_LOGI(SENSOR_TAG, "Getting Accelerometer data from MPU6050 sensor");

    mpu6050_acce_value_t accel;
    iot_mpu6050_get_acce(mpu6050_dev, &accel);
    return accel;
}

/******************************************************************************
 * FunctionName : get_gyroscope_data
 * Description  : Getting Gyroscope data from MPU6050 sensor
 * Parameters   : none
 * Returns      : mpu6050_gyro_value_t (struct of floats)
*******************************************************************************/
mpu6050_gyro_value_t get_gyroscope_data()
{
    ESP_LOGI(SENSOR_TAG, "Getting Gyroscope data from MPU6050 sensor");
    mpu6050_gyro_value_t gyro;
    iot_mpu6050_get_gyro(mpu6050_dev, &gyro);
    return gyro;
}

/******************************************************************************
 * FunctionName : oled_write_temp_header
 * Description  : Writing Temperature data Headings to SSD1306 OLED sensor
 * Parameters   : none
 * Returns      : void
*******************************************************************************/
void oled_write_temp_header()
{
    ESP_LOGI(SENSOR_TAG, "Writing Temp Header to SSD1306 OLED sensor");

    iot_ssd1306_draw_string(ssd1306_dev, 0, 16, (const uint8_t *) "Temp :", 16, 1);
    iot_ssd1306_draw_string(ssd1306_dev, 0, 32, (const uint8_t *) "Humi :", 16, 1);
    iot_ssd1306_draw_string(ssd1306_dev, 0, 48, (const uint8_t *) "Light:", 16, 1);
    iot_ssd1306_refresh_gram(ssd1306_dev);
    
}

/******************************************************************************
 * FunctionName : oled_write_temp_data
 * Description  : Writing Temperature data from HTS221 sensor to SSD1306 OLED 
 *                sensor
 * Parameters   : none
 * Returns      : void
*******************************************************************************/
void oled_write_temp_data()
{
    float temperature = 0, 
          humidity = 0, 
          brightness = 0;
    char temp_sensor_data[10] = {0};

    ESP_LOGI(SENSOR_TAG, "Writing Temp Data to SSD1306 OLED sensor");

    temperature = get_temperature_data(true); //convert to fahrenheit
    humidity = get_humidity_data();
    brightness = get_light_luminance_data();

    ESP_LOGI(SENSOR_TAG, "Writing info: temperature: %.1f, humidity: %.1f, luminance: %.1f", 
                        temperature, humidity, brightness);
    
    sprintf(temp_sensor_data, "%.1f", temperature);
    iot_ssd1306_draw_string(ssd1306_dev, 71, 16, (const uint8_t *) temp_sensor_data, 16, 1);
    sprintf(temp_sensor_data, "%.1f", humidity);
    iot_ssd1306_draw_string(ssd1306_dev, 71, 32, (const uint8_t *) temp_sensor_data, 16, 1);
    sprintf(temp_sensor_data, "%.1f", brightness);
    iot_ssd1306_draw_string(ssd1306_dev, 71, 48, (const uint8_t *) temp_sensor_data, 16, 1);

    iot_ssd1306_refresh_gram(ssd1306_dev);
}