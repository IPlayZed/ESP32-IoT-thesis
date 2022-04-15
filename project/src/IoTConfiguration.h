#ifndef IOT_CONFIGURATION_H
#define IOT_CONFIGURATION_H

#include "Common.h"

#define CONFIG_WIFI_SSID "<your_WiFi_SSID_goes_here>"
#define CONFIG_WIFI_PASSWORD "<your_WiFi_password_goes_here>"

#define CONFIG_MQTT_CLIENT_QOS DEFAULT_MQTT_QOS
#define CONFIG_MQTT_CLIENT_MESSAGE_RETAIN_POLICY MQTT_MESSAGE_RETAIN_POLICY_OFF

#define CONFIG_AZURE_FQDN "<your_FQDN_goes_here>.azure-devices.net"
#define CONFIG_AZURE_DEVICE_ID "<your_device_ID_goes_here>"
#define CONFIG_AZURE_DEVICE_KEY "<your_device_key_goes_here>"

#endif
