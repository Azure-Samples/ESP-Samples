#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "parson.h"
#include "data_helper.h"


char* serializeToJson(DeviceData* devicedata)
{
    char *serialized_string = NULL;
    char *result=NULL;
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    

    json_object_set_string(root_object, "Chip Type", devicedata->chip_type);
    json_object_set_number(root_object, "Free Heap Size",devicedata->free_heap_size);
    json_object_set_number(root_object, "Temperature", devicedata->temperature);
    json_object_set_number(root_object, "Humidity", devicedata->humidity);
    json_object_set_number(root_object, "Luminance", devicedata->luminance);
    serialized_string = json_serialize_to_string_pretty(root_value);
    snprintf(result, sizeof(serialized_string), serialized_string);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);

    return result;
}