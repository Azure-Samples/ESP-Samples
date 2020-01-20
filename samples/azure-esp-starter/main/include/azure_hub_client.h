#ifndef _AZURE_HUB_CLIENT_H_
#define _AZURE_HUB_CLIENT_H_

#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility/platform.h"
#include "iothub_device_client_ll.h"
#include "iothub_client_options.h"
#include "iothub.h"
#include "iothub_message.h"
#include "parson.h"
#include "sdkconfig.h"
#include "iothubtransportmqtt.h"
#include "iothub_client_ll.h"
#include "certs.h"

#include "esp_log.h"

#include "iothub_client_version.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/tickcounter.h"
#include "azure_c_shared_utility/shared_util_options.h"

#include "azure_prov_client/prov_transport_mqtt_client.h"



IOTHUB_DEVICE_CLIENT_LL_HANDLE init_iothub_client();
int send_azure_msgs(char *msgToSend, IOTHUB_DEVICE_CLIENT_LL_HANDLE iothub_deviceclient);
void process_azure_msgs(IOTHUB_DEVICE_CLIENT_LL_HANDLE iothub_deviceclient);
void destroy_iothub_client(IOTHUB_DEVICE_CLIENT_LL_HANDLE iothub_deviceclient);

#endif