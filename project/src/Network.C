/**
 * @file Network.C
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-02 
 */

#pragma once

#include <WiFi.h>

#include "libraries/SerialLogger.h"
#include "libraries/IoTConfiguration.h"

static const char* SSID = CONFIG_WIFI_SSID;
static const char* PASSWORD = CONFIG_WIFI_PASSWORD;

static void WiFi_Connect()
{
    Logger.Info("Trying to connect to " + String(SSID));

    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }
    
    Serial.println();
    Logger.Info("Connected to " + String(SSID) + " with IP of " + WiFi.localIP().toString());
}

// For more information on the workings see https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/ 
static void setupTime() {
    Logger.Info("Initialize time via SNTP");
    configTime(TIME_ZONE_GMT_OFFSET * TIME_S_TO_H_FACTOR, TIME_DAYLIGHT_SAVING_SECS, NTP_SERVERS_URL);
      time_t now = time(NULL);
    while (now < 1510592825)
    {
        delay(500);
        Serial.print(".");
        now = time(nullptr);
    }
    Serial.println("");
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Logger.Error("Failed to get local time!");
    }
    else
    {
        Logger.Info("Got local time: ");
        Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    }
}