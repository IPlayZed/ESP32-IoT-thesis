// Licensed under: GNU GENERAL PUBLIC LICENSE Version 2

#ifndef NETWORK_H
#define NETWORK_H

#include <time.h>

#include <WiFi.h>
#include <mqtt_client.h>

// Azure IoT SDK for C includes
#include <az_core.h>
#include <az_iot.h>

#include "SerialLogger.h"

#include "AzIoTSasToken.h"
#include "IoTConfiguration.h"

#include "Common.h"

#define SAS_TOKEN_DURATION_IN_MINUTES 60
#define INBOUND_DATA_SIZE_BYTES DEFAULT_INBOUND_DATA_SIZE_BYTES
#define INBOUND_DATA_SIZE_BYTES_LAST_POS (INBOUND_DATA_SIZE_BYTES - 1)
#define AZURE_SDK_CLIENT_USER_AGENT "c/" AZ_SDK_VERSION_STRING "(ard;esp32)"
#define MAGIC_TIMESTAMP 1510592825

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

    void getTelemetryPayload(az_span *payload);

    void sendTelemetry();
}

namespace MQTT
{
    void initializeMQTTClient();

    bool checkIfSasTokenInstanceIsExpired();

    void destroyMQTTClientInstance();
}

#endif