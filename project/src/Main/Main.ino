// Licensed under: GNU GENERAL PUBLIC LICENSE Version 2

#include "SerialLogger.h"

#include "Common.h"
#include "CommonConfig.h"
#include "Network.h"
#include "RelativeHumidityTempSensor.h"


// This namespace contains the tasks to be completed by the sketch.
namespace Tasks
{
        void taskConfigureLocalPeripherials()
        {
                esp_sleep_enable_timer_wakeup(CONFIG_TIME_TO_SLEEP_IN_S * uS_TO_S_FACTOR);
                Serial.begin(CONFIG_SERIAL_BAUD_RATE);
                RHTempSensor::initializeSensor();
        }
        void taskInitializeConnection()
        {
                btStop();
                Network::setupNetworking(true);
        }
        void taskSendTelemetry()
        {
                Network::sendTelemetry();
        }
        void taskDoMeasurements()
        {
                RHTempSensor::makeMeasurements();
                // TODO: Handle results.
        }
}

void setup()
{
        DelayForSerialConnection(CONFIG_DELAY_FOR_SERIAL_CONNECTION);
        Tasks::taskConfigureLocalPeripherials();
        Tasks::taskInitializeConnection();
}

void loop()
{
        Tasks::taskDoMeasurements();
        Tasks::taskSendTelemetry();
        esp_light_sleep_start();
        LogInfo("Woke up from sleep!");
}