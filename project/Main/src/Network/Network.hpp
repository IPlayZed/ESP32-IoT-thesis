// Licensed under: GNU GENERAL PUBLIC LICENSE Version 2

#ifndef NETWORK_H
#define NETWORK_H

#include "../Common/Common.hpp"

#define MQTT_QOS_LEVEL_AT_MOST_ONCE 0
#define MQTT_QOS_LEVEL_AT_LEAST_ONCE 1
#define MQTT_QOS_LEVEL_EXACTLY_ONCE 2

#define WIFI_TURN_OFF true
#define WIFI_KEEP_ON false

namespace Network
{
    namespace Telemetry
    {
        typedef struct telemetryData
        {
            float temperature;
            float humidity;
            float CO;
        } telemetryData_t;

        void processTelemetryData(Network::Telemetry::telemetryData_t *telemetryData);
        void sendTelemetry();
    }
    void turnOffWiFi(void);
    void setupNetworking(bool turnOffWifiAfterSetup = WIFI_KEEP_ON);
}

#endif