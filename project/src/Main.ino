#include <WiFi.h>

#include "libraries/SerialLogger.h"
#include "libraries/IoTConfiguration.h"

static const BaseType_t PRO_CPU = PRO_CPU_NUM;
static const BaseType_t APP_CPU = APP_CPU_NUM;

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

void setup()
{

}

void loop()
{

}