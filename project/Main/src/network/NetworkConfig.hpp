#ifndef NETWORK_CONFIG_H
#define NETWORK_CONFIG_H

#include "Network.hpp"

#define DEFAULT_MQTT_QOS MQTT_QOS_LEVEL_AT_LEAST_ONCE
#define DEFAULT_SAS_TOKEN_DURATION_IN_MINUTES 60
#define DEFAULT_MQTT_QOS MQTT_QOS_LEVEL_AT_LEAST_ONCE
#define DEFAULT_WAIT_MSEC (uint32_t)1000
#define DEFAULT_TELEMETRY_DATA_MAXIMUM_SIZE 128
#define DEFAULT_TELEMETRY_DATA_TEMPERATURE_ID "temperature"
#define DEFAULT_TELEMETRY_DATA_HUMIDITY_ID "humidity"
#define DEFAULT_TELEMETRY_DATA_CO_ID "co_level"
#define DEFAULT_WIFI_WAIT_TICKS 10
#define DEFAULT_SNTP_WAIT_TICKS 30

#define CONFIG_MQTT_CLIENT_QOS DEFAULT_MQTT_QOS
#define CONFIG_MQTT_QOS DEFAULT_MQTT_QOS
#define CONFIG_MQTT_CLIENT_MESSAGE_RETAIN_POLICY MQTT_MESSAGE_RETAIN_POLICY_OFF
#define CONFIG_WIFI_WAIT_MSEC DEFAULT_WAIT_MSEC
#define CONFIG_TIME_WAIT_MSEC DEFAULT_WAIT_MSEC
#define CONFIG_TELEMETRY_DATA_MAXIMUM_SIZE DEFAULT_TELEMETRY_DATA_MAXIMUM_SIZE
#define CONFIG_TELEMETRY_DATA_TEMPERATURE_ID DEFAULT_TELEMETRY_DATA_TEMPERATURE_ID
#define CONFIG_TELEMETRY_DATA_HUMIDITY_ID DEFAULT_TELEMETRY_DATA_HUMIDITY_ID
#define CONFIG_TELEMETRY_DATA_CO_ID DEFAULT_TELEMETRY_DATA_CO_ID
#define CONFIG_WIFI_WAIT_TICKS DEFAULT_WIFI_WAIT_TICKS
#define CONFIG_SNTP_WAIT_TICKS DEFAULT_SNTP_WAIT_TICKS

#define CONFIG_SAS_TOKEN_DURATION_IN_MINUTES DEFAULT_SAS_TOKEN_DURATION_IN_MINUTES

namespace Configurations
{
    namespace Network
    {
        namespace Defaults
        {
            constexpr uint8_t INBOUND_DATA_SIZE_BYTES = 128;
            constexpr uint16_t SERIAL_BAUD_RATE = 9600;
            constexpr uint16_t TIME_TO_SLEEP_SEC = 30;
            constexpr uint16_t DELAY_FOR_SERIAL_MILISEC = 5000;
        }

        const uint8_t INBOUND_DATA_SIZE_BYTES = Defaults::INBOUND_DATA_SIZE_BYTES;
        const uint16_t SERIAL_BAUD_RATE = Defaults::SERIAL_BAUD_RATE;
        const uint16_t TIME_TO_SLEEP_SEC = Defaults::TIME_TO_SLEEP_SEC;
        const uint16_t DELAY_FOR_SERIAL_MILISEC = Defaults::DELAY_FOR_SERIAL_MILISEC;
    }
}

#endif // NETWORK_CONFIG_H
