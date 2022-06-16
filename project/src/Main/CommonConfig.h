#ifndef COMMON_CONFIG_H
#define COMMON_CONFIG_H

#include "Network.h"

#define DEFAULT_MQTT_QOS MQTT_QOS_LEVEL_AT_LEAST_ONCE
#define DEFAULT_INBOUND_DATA_SIZE_BYTES 128
#define DEFAULT_SERIAL_BAUD_RATE 9600
#define DEFAULT_TIME_TO_SLEEP_IN_S 30
#define DEFAULT_DELAY_FOR_SERIAL_CONNECTION 5000

#define CONFIG_SERIAL_BAUD_RATE DEFAULT_SERIAL_BAUD_RATE
#define CONFIG_MQTT_QOS DEFAULT_MQTT_QOS
#define CONFIG_TIME_TO_SLEEP_IN_S DEFAULT_TIME_TO_SLEEP_IN_S
#define CONFIG_DELAY_FOR_SERIAL_CONNECTION DEFAULT_DELAY_FOR_SERIAL_CONNECTION

#endif
