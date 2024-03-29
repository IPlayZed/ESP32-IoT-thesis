// Licensed under: GNU GENERAL PUBLIC LICENSE Version 2

#ifndef COMMON_HPP
#define COMMON_HPP

#include "serial-logger/SerialLogger.hpp"

#define DEBUG_MODE

#define uS_TO_S_FACTOR 1000000
#define TIME_ZONE_GMT_OFFSET 2
#define TIME_S_TO_H_FACTOR 3600
#define TIME_DAYLIGHT_SAVING_ENABLED 1
#define TIME_DAYLIGHT_SAVING_SECS ((TIME_ZONE_GMT_OFFSET + TIME_DAYLIGHT_SAVING_ENABLED) * TIME_S_TO_H_FACTOR)
#define NTP_SERVERS_URL "pool.ntp.org"

#define CPU_FREQ_POWER_SAVER 80

#define MQTT_MESSAGE_RETAIN_POLICY_ON 1
#define MQTT_MESSAGE_RETAIN_POLICY_OFF 0

#define NULL_TERMINATOR '\0'

// WARNING: Only use this macro if the argument is an actual 1-dimensional C-like array.
// Passing normal pointers will cause "sizeof(arg)" to return the system pointer width, resulting in a silent bug.
#define sizeofarray(arg) (sizeof(arg) / sizeof(arg[0]))

uint32_t getSecsSinceEpoch();

#ifdef DEBUG_MODE
#define LogInfo(text) Logger.Info(text)
#define LogWarning(text) Logger.Warning(text)
#define LogError(text) Logger.Error(text)
#define SerialPrint(text) Serial.print(text)
#define SerialNewLine() Serial.println()
#define SerialTimeinfo(timeinfo, formattedText) Serial.println(timeinfo, formattedText)
#define DelayForSerialConnection(time) delay(time);
#else
#define LogInfo(text)
#define LogWarning(text)
#define LogError(text)
#define SerialPrint(text)
#define SerialNewLine()
#define SerialTimeinfo(timeinfo, formattedText)
#define DelayForSerialConnection(time)
#endif

#endif // COMMON_HPP
