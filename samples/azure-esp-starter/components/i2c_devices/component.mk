#
# Component Makefile
#
 
# Component configuration in preprocessor defines

COMPONENT_ADD_INCLUDEDIRS := \
i2c_bus/include \
sensor/hts221/include \
sensor/mpu6050/include \
sensor/bh1750/include \
others/ssd1306/include 

COMPONENT_SRCDIRS := \
i2c_bus \
sensor/hts221 \
sensor/mpu6050 \
sensor/bh1750 \
others/ssd1306 