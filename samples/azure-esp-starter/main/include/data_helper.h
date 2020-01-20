#ifndef _DATA_HELPER_H_
#define _DATA_HELPER_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "parson.h"

typedef struct ESP_DEVICE_DATA
{
    char* chip_type;
    uint32_t free_heap_size;
    float temperature;
    float humidity;
    float luminance;
} DeviceData;

char* serializeToJson(DeviceData* devicedata);

#endif