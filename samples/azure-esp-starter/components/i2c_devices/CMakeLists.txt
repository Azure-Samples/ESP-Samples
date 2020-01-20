set(IOT_COMPONENT_DIRS  ${IOT_COMPONENT_DIRS}
                        $ENV{PROJECT_PATH}/components
                        $ENV{PROJECT_PATH}/components/i2c_devices
                        $ENV{PROJECT_PATH}/components/i2c_devices/sensor
                        $ENV{PROJECT_PATH}/components/i2c_devices/others
)                        
set(EXTRA_COMPONENT_DIRS "${EXTRA_COMPONENT_DIRS} ${IOT_COMPONENT_DIRS}")

set (COMPONENT_ADD_INCLUDEDIRS "."
            "i2c_bus/include"
            "sensor/hts221/include"
            "sensor/mpu6050/include" 
            "sensor/bh1750/include"
            "others/ssd1306/include" 
)

SET(COMPONENT_SRCS
        "i2c_bus/i2c_bus.c"
        "sensor/bh1750/bh1750.c"
        "sensor/hts221/hts221.c"
        "sensor/mpu6050/mpu6050.c"
        "others/ssd1306/ssd1306.c"
        "others/ssd1306/ssd1306_fonts.c"
        )

register_component()