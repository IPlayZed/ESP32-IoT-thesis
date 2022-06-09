// Licensed under: GNU GENERAL PUBLIC LICENSE Version 2

#include "SerialLogger.h"

#include "Common.h"
#include "CommonConfig.h"
#include "Network.h"
#include "RelativeHumidityTempSensor.h"


// This namespace contains the tasks to be completed by the sketch.
namespace Tasks
{
        void taskConfigureLocalPeripherials(void* RTOSParameters)
        {
                esp_sleep_enable_timer_wakeup(CONFIG_TIME_TO_SLEEP_IN_S * uS_TO_S_FACTOR);
                Serial.begin(CONFIG_SERIAL_BAUD_RATE);
                RHTempSensor::initializeSensor();
        }
        void taskInitializeConnection(void* RTOSParameters)
        {
                btStop();
                Network::setupNetworking();
                Network::turnOffWiFi();
        }
        void taskSendTelemetry(void* RTOSParameters)
        {
                Network::sendTelemetry();
        }
        void taskDoMeasurements(void* RTOSParameters)
        {
                RHTempSensor::makeMeasurements();
                delay(2000); // TODO: Is this wait really needed?
                // TODO: Handle results.
        }
}

void setup()
{
        delay(5000); // This is just to have time to connect via serial to monitor events.
        Tasks::taskConfigureLocalPeripherials(nullptr);
        Tasks::taskInitializeConnection(nullptr);
}

void loop()
{
        Tasks::taskDoMeasurements(nullptr);
        Tasks::taskSendTelemetry(nullptr);
        esp_light_sleep_start();
        LogInfo("Woke up from sleep!");
}