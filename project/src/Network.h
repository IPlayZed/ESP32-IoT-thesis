#ifndef NETWORK_H
#define NETWORK_h

#include <time.h>

#include <WiFi.h>
#include <mqtt_client.h>

#include "libraries/SerialLogger.h"
#include "libraries/IoTConfiguration.h"

#define AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC DEFAULT_TOPIC
#define AZ_IOT_HUB_CLIENT_QOS DEFAULT_QOS
#define INBOUND_DATA_SIZE_BYTES DEFAULT_INBOUND_DATA_SIZE_BYTES
#define INBOUND_DATA_SIZE_BYTES_LAST_POS (INBOUND_DATA_SIZE_BYTES - 1)

void WiFi_Connect();

// For more information on the workings see https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/ 
void setupTime();

// The library used for event handling is ESP-MQTT.
// For further reference see: https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32/api-reference/protocols/mqtt.html
esp_err_t MQTTEventHandler(esp_mqtt_event_handle_t event);

int initializeMQTTClient();

#endif