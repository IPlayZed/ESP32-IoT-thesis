#include "../libs/SerialLogger.h"
#include "../config/Configuration.h"

static const BaseType_t PRO_CPU = PRO_CPU_NUM;
static const BaseType_t APP_CPU = APP_CPU_NUM;

static const char* SSID = WIFI_SSID;

static void WiFi_Connect()
{
    Logger.Info("Trying to connect to " + String(SSID));
}