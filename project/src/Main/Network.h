// Licensed under: GNU GENERAL PUBLIC LICENSE Version 2

#ifndef NETWORK_H
#define NETWORK_H

// TODO: Clean up includes.
#include <time.h>

#include <WiFi.h>
#include <mqtt_client.h>

#include <az_core.h>
#include <az_iot.h>

#include "SerialLogger.h"

#include "AzIoTSasToken.h"
#include "SecretsConfig.h"

#include "Common.h"
#include "NetworkConfig.h"

#define MQTT_QOS_LEVEL_AT_MOST_ONCE 0
#define MQTT_QOS_LEVEL_AT_LEAST_ONCE 1
#define MQTT_QOS_LEVEL_EXACTLY_ONCE 2

#define WIFI_TURN_OFF true
#define WIFI_KEEP_ON false

namespace Network
{
    namespace Telemetry
    {
        //typedef struct telemetryData telemetryData_t; // TODO: Ok, how tf do I make this accessible from other files.
        // This is ugly, I would prefer the one before.
        // NOTE: This might be needed if we want to manipulate this struct from other TUs.
        typedef struct telemetryData {
            float temperature;
            float humidity;
            float CO;
        } telemetryData_t;

        char* getSerializedTelemetryDataPtr(void);
        // TODO: Move these into the correct namespace in the source.
        char* processTelemetryData(Network::Telemetry::telemetryData_t* telemetryData, char* serializationLocation);
        void sendTelemetry(char* serializedTelemetryData);
    }
    void turnOffWiFi(void);
    void setupNetworking(bool turnOffWifiAfterSetup = WIFI_KEEP_ON);
}

#endif