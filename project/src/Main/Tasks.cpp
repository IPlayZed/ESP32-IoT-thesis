#include "Tasks.h"
#include "SerialLogger.h"
#include "Common.h"
#include "CommonConfig.h"
#include "Network.h"
#include "RelativeHumidityTempSensor.h"

namespace Tasks
{
        void taskConfigureLocalPeripherials(void* RTOSparameter)
        {
                esp_sleep_enable_timer_wakeup(CONFIG_TIME_TO_SLEEP_IN_S * uS_TO_S_FACTOR);
                Serial.begin(CONFIG_SERIAL_BAUD_RATE);
                RHTempSensor::initializeSensor();
        }
        void taskInitializeConnection(void* RTOSparameter)
        {
                btStop();
                Network::setupNetworking();
        }
        void taskSendTelemetry(void* RTOSparameter)
        {
                Network::sendTelemetry();
        }
        void taskDoMeasurements(void* RTOSparameter)
        {
                RHTempSensor::makeMeasurements();
                // TODO: Handle results.
        }

        namespace Helpers
        {
            void taskHelperDelayForSerialConnection(void)
            {
                DelayForSerialConnection(CONFIG_DELAY_FOR_SERIAL_CONNECTION);
            }
            void taskHelperStartSleep()
            {
                esp_light_sleep_start();
                LogInfo("Woke up from sleep!");
            }
        }
}

