#ifndef NETWORK_H
#define NETWORK_h

#include <time.h>

#include <WiFi.h>
#include <mqtt_client.h>

// Azure IoT SDK for C includes
#include <az_core.h>
#include <az_iot.h>
#include <azure_ca.h>

#include "libraries/SerialLogger.h"
#include "libraries/AzIoTSasToken.h"
#include "libraries/IoTConfiguration.h"

#include "libraries/Common.h"

#define AZ_IOT_HUB_CLIENT_QOS DEFAULT_QOS
#define SAS_TOKEN_DURATION_IN_MINUTES 60
#define INBOUND_DATA_SIZE_BYTES DEFAULT_INBOUND_DATA_SIZE_BYTES
#define INBOUND_DATA_SIZE_BYTES_LAST_POS (INBOUND_DATA_SIZE_BYTES - 1)
#define AZURE_SDK_CLIENT_USER_AGENT "c/" AZ_SDK_VERSION_STRING "(ard;esp32)"

void WiFi_Connect();

// For more information on the workings see https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/ 
void setupTime();

// The library used for event handling is ESP-MQTT.
// For further reference see: https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32/api-reference/protocols/mqtt.html
esp_err_t MQTTEventHandler(esp_mqtt_event_handle_t event);

int initializeMQTTClient();

void initializeIoTHubClient();

#endif