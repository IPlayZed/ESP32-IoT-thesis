#ifndef IOT_CONFIGURATION_H
#define IOT_CONFIGURATION_H

#define CONFIG_WIFI_SSID "<your_WiFi_SSID_goes_here>"
#define CONFIG_WIFI_PASSWORD "<your_WiFi_password_goes_here>"

#define CONFIG_AZURE_FQDN "<your_FQDN_goes_here>.azure-devices.net"
#define CONFIG_AZURE_DEVICE_ID "<your_device_ID_goes_here>"
#define CONFIG_AZURE_DEVICE_KEY "<your_device_key_goes_here>"

#define DEFAULT_TOPIC "devices/+/messages/devicebound/#"
#define DEFAULT_QOS 1
#define DEFAULT_INBOUND_DATA_SIZE_BYTES 128

#endif
