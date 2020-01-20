
set(COMPONENT_SRCS "main.c" 
                   "esp_common.c" 
                   "azure_hub_client.c" 
                   "esp_sensors.c")

list(APPEND COMPONENT_ADD_INCLUDEDIRS "." 
                                      "include")

register_component()