/**
 * @file Main.ino
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-04-02
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "Common.h"
#include "Network.h"
#include "SerialLogger.h"

// This is only for testing purposes, when not using multicore programming.
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t APP_CPU = 0;
#else
static const BaseType_t APP_CPU = 1;
#endif
// When writing actual multricore code, the next 2 lines should be commented out
// and used instead of the above.
// static const BaseType_t PRO_CPU = PRO_CPU_NUM;
// static const BaseType_t APP_CPU = APP_CPU_NUM;

void taskSendTelemetry(void *parameter)
{
    // Logger.Info("Entering deep sleep for the first time...");
    // esp_deep_sleep_start();
    while (1)
    {
        Logger.Info("\nWoke up from deep sleep!");
        if (WiFi.status() != WL_CONNECTED)
        {
            // Setup::WiFi_Connect();
            WiFi.reconnect();
        }
        else if (MQTT::checkIfSasTokenInstanceIsExpired())
        {
            Logger.Info("SAS token expired; reconnecting with a new one.");
            MQTT::destroyMQTTClientInstance();
            MQTT::initializeMQTTClient();
        }
        Logger.Info("Task now trying to send telemetry....");
        IoTHub::sendTelemetry();
        Logger.Info("Telemetry sent, disconnecting wifi...");
        WiFi.disconnect();
        Logger.Info("Telemetry sending done, entering deep sleep in 3 seconds...\n");
        vTaskDelay(3000 / portTICK_PERIOD_MS); // We leave some time
        // TODO: Decide on which would make more sense architecturally.
        esp_deep_sleep_start();
        // FIXME: WiFi is messed up when waking up using light sleep.
        // esp_light_sleep_start();
    }
}

void setup()
{
    btStop();
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP_IN_S * uS_TO_S_FACTOR);
    Serial.begin(115200);

    Setup::tryConnection();

    xTaskCreatePinnedToCore(
        taskSendTelemetry,
        "Dummy Telemetry Task",
        20000,
        NULL,
        1,
        NULL,
        APP_CPU);
}

void loop()
{
}