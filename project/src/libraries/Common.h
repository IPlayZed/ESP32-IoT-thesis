/**
 * @file Common.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef COMMON_H
#define COMMON_H

#include <time.h>

#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP_IN_S 5
#define TIME_ZONE_GMT_OFFSET 2
#define TIME_S_TO_H_FACTOR 3600
#define TIME_DAYLIGHT_SAVING_ENABLED 1
#define TIME_DAYLIGHT_SAVING_SECS ((TIME_ZONE_GMT_OFFSET + TIME_DAYLIGHT_SAVING_ENABLED) * TIME_S_TO_H_FACTOR)
#define NTP_SERVERS_URL "pool.ntp.org"

#define DEFAULT_SERIAL_BAUD_RATE 115200

#define NULL_TERMINATOR '\0'

#define sizeofarray(a) (sizeof(a) / sizeof(a[0]))

uint32_t getSecsSinceEpoch();
#endif