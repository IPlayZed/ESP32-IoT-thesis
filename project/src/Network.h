#ifndef NETWORK_H
#define NETWORK_h

#include <time.h>

#include <WiFi.h>
#include <mqtt_client.h>

#include "libraries/SerialLogger.h"
#include "libraries/IoTConfiguration.h"

#define AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC DEFAULT_TOPIC
#define AZ_IOT_HUB_CLIENT_QOS DEFAULT_QOS

static const char* SSID;
static const char* PASSWORD;

static esp_mqtt_client_handle_t mqtt_client;

static void WiFi_Connect();

static void setupTime();

static esp_err_t MQTTEventHandler(esp_mqtt_event_handle_t event);

#endif