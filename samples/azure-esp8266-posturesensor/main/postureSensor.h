#include "iothub_device_client.h"
#include "iothub_client_options.h"
#include "iothub.h"
#include "iothub_message.h"
#include "parson.h"
#include "sdkconfig.h"
#include "iothubtransportmqtt.h"
#include "iothub_client_ll.h"

// Defining types for posture sensor - SR

typedef struct POSTURE_SENSOR
{
    bool desired_operating_state;
    uint32_t desired_threshold;
} PostureSensor;


static PostureSensor *postureSensorCurrent;

extern void deviceTwinCallback(DEVICE_TWIN_UPDATE_STATE , const unsigned char *, size_t , void *);
extern PostureSensor *parseFromJson(const char *json, DEVICE_TWIN_UPDATE_STATE update_state, PostureSensor *currentPostureSensor);
