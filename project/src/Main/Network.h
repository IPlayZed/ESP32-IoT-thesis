// Licensed under: GNU GENERAL PUBLIC LICENSE Version 2

#ifndef NETWORK_H
#define NETWORK_H

#include <time.h>

#include <WiFi.h>
#include <mqtt_client.h>

#include <az_core.h>
#include <az_iot.h>

#include "SerialLogger.h"

#include "AzIoTSasToken.h"
#include "SecretsConfig.h"

#include "Common.h"

#define MQTT_QOS_LEVEL_AT_MOST_ONCE 0
#define MQTT_QOS_LEVEL_AT_LEAST_ONCE 1
#define MQTT_QOS_LEVEL_EXACTLY_ONCE 2

#define INBOUND_DATA_SIZE_BYTES DEFAULT_INBOUND_DATA_SIZE_BYTES
#define INBOUND_DATA_SIZE_BYTES_LAST_POS (INBOUND_DATA_SIZE_BYTES - 1)
#define AZURE_SDK_CLIENT_USER_AGENT "c/" AZ_SDK_VERSION_STRING "(ard;esp32)"


namespace Setup
{
    void WiFi_Connect();

    // For more information on the workings see https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/
    void setupTime();

    void tryConnection();
}

namespace IoTHub
{
    void initializeIoTHubClient();

    void sendTelemetry();
}

namespace MQTT
{
    void initializeMQTTClient();

    bool checkIfSasTokenInstanceIsExpired();

    void destroyMQTTClientInstance();
}

#endif