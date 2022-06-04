// Licensed under: GNU GENERAL PUBLIC LICENSE Version 2

#ifndef COMMON_H
#define COMMON_H

#include <time.h>
#include "SerialLogger.h"
#define DEBUG_MODE

#define DEFAULT_MQTT_QOS 1
#define DEFAULT_INBOUND_DATA_SIZE_BYTES 128
#define DEFAULT_SERIAL_BAUD_RATE 230400

#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP_IN_S 2
#define TIME_ZONE_GMT_OFFSET 2
#define TIME_S_TO_H_FACTOR 3600
#define TIME_DAYLIGHT_SAVING_ENABLED 1
#define TIME_DAYLIGHT_SAVING_SECS ((TIME_ZONE_GMT_OFFSET + TIME_DAYLIGHT_SAVING_ENABLED) * TIME_S_TO_H_FACTOR)
#define NTP_SERVERS_URL "pool.ntp.org"
#define CONFIG_SERIAL_BAUD_RATE DEFAULT_SERIAL_BAUD_RATE

#define MQTT_MESSAGE_RETAIN_POLICY_ON 1
#define MQTT_MESSAGE_RETAIN_POLICY_OFF 0

#define NULL_TERMINATOR '\0'

#define sizeofarray(a) (sizeof(a) / sizeof(a[0]))

uint32_t getSecsSinceEpoch();

#ifdef DEBUG_MODE
    #define LogInfo(text) Logger.Info(text)
    #define LogError(text) Logger.Error(text)
    #define SerialPrint(text) Serial.print(text)
    #define SerialPrintln() Serial.println()
#else
    #define LogInfo(text)
    #define LogError(text)
    #define SerialPrint(text)
    #define SerialPrintln()
#endif

#endif