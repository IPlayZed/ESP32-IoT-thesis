#include "SerialLogger.h"

#include "Common.h"
#include "Network.h"
#include "RelativeHumidityTempSensor.h"

void taskSendTelemetry(void)
{
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

        vTaskDelay(2000 / portTICK_PERIOD_MS);

#ifdef DEBUG_MODE
        Logger.Info("Telemetry sent, disconnecting wifi...");
        Logger.Info("Telemetry sending done, entering deep sleep in 2 seconds for " + String(TIME_TO_SLEEP_IN_S) + "...\n");
#endif
        MQTT::destroyMQTTClientInstance();
        WiFi.disconnect(true, true);
        vTaskDelay(2000 / portTICK_PERIOD_MS); // We leave some time before entering sleep.
}

void taskMeasureHumidity(void)
{
        RHTempSensor::makeMeasurements();
        delay(2000);
}

void doMeasurements()
{
        taskMeasureHumidity();
        getResults();
}

void getResults()
{
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
        Serial.begin(230400);

        Setup::tryConnection();
        RHTempSensor::initializeSensor();
}

void loop()
{
        doMeasurements();
        taskSendTelemetry();
        esp_light_sleep_start();
#ifdef DEBUG_MODE
        Logger.Info("Woke up from sleep!");
#endif
}