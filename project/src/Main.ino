#include "Common.h"
#include "Network.h"
#include "SerialLogger.h"

// This is only for testing purposes, when not using multicore programming.
//#if CONFIG_FREERTOS_UNICORE
// static const BaseType_t APP_CPU = 0;
//#else
// static const BaseType_t APP_CPU = 1;
//#endif
// When writing actual multicore code, the next 2 lines should be commented out
// and used instead of the above.
static const BaseType_t PRO_CPU = PRO_CPU_NUM;
static const BaseType_t APP_CPU = APP_CPU_NUM;

void taskSendTelemetry(void *parameter)
{
    while (1)
    {

#ifdef DEBUG_MODE
        Logger.Info("\nWoke up from deep sleep!");
#endif

        if (WiFi.status() != WL_CONNECTED)
        {
            Setup::WiFi_Connect();
            (void)MQTT::initializeMQTTClient();
        }
        else if (MQTT::checkIfSasTokenInstanceIsExpired())
        {

#ifdef DEBUG_MODE
            Logger.Info("SAS token expired; reconnecting with a new one.");
#endif

            MQTT::destroyMQTTClientInstance();
            (void)MQTT::initializeMQTTClient();
        }

#ifdef DEBUG_MODE
        Logger.Info("Task now trying to send telemetry....");
#endif
        IoTHub::sendTelemetry();

#ifdef DEBUG_MODE
        Logger.Info("Telemetry sent, disconnecting wifi...");
#endif

        MQTT::destroyMQTTClientInstance();
        WiFi.disconnect(true, true);

#ifdef DEBUG_MODE
        Logger.Info("Telemetry sending done, entering deep sleep in 3 seconds...\n");
#endif
        vTaskDelay(3000 / portTICK_PERIOD_MS); // We leave some time before entering deep sleep.
        // TODO: Decide on which would make more sense architecturally.
        esp_light_sleep_start();
    }
}

void setup()
{
    // This is just to have time to connect via serial to monitor events.
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    // We disable the Bluetooth controller, because we do not want to use it and it saves energy.
    btStop();
    // Set the initial wakeup period.
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP_IN_S * uS_TO_S_FACTOR);
    // TODO: This should be configurable.
    Serial.begin(115200);

    // Initial connection.
    Setup::tryConnection();

    // Create all the tasks.
    xTaskCreatePinnedToCore(
        taskSendTelemetry,
        "Dummy Telemetry Task",
        20000,
        NULL,
        1,
        NULL,
        PRO_CPU);
}

// Execution must not reach here (because we use FreeRTOS)!
void loop()
{
}