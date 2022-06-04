// Licensed under: GNU GENERAL PUBLIC LICENSE Version 2

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
                LogInfo("SAS token expired; reconnecting with a new one.");
                MQTT::destroyMQTTClientInstance();
                MQTT::initializeMQTTClient();
        }

        LogInfo("Task now trying to send telemetry....");
        IoTHub::sendTelemetry();

        delay(2000); // FIXME: Is this needed at all?

        LogInfo("Telemetry sending done disconnecting WiFi, and entering deep sleep in 2 seconds for " + String(TIME_TO_SLEEP_IN_S) + "...\n");
        MQTT::destroyMQTTClientInstance();
        WiFi.disconnect(true, true);
        delay(2000);
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
        delay(5000); // This is just to have time to connect via serial to monitor events.
        btStop();
        esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP_IN_S * uS_TO_S_FACTOR);
        Serial.begin(CONFIG_SERIAL_BAUD_RATE);

        Setup::tryConnection();
        RHTempSensor::initializeSensor();
}

void loop()
{
        doMeasurements();
        taskSendTelemetry();
        esp_light_sleep_start();
        LogInfo("Woke up from sleep!");
}