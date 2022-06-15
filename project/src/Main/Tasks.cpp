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
                setCpuFrequencyMhz(CPU_FREQ_POWER_SAVER);
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
                // NOTE: This is only POC.
                Network::Telemetry::telemetryData_t dummyData; // Dummy data for testing.
                dummyData.temperature = (float)1.1;
                dummyData.humidity = (float)2.2;
                dummyData.CO = (float)3.3;
                Network::Telemetry::processTelemetryData(&dummyData);
                Network::Telemetry::sendTelemetry();
        }
        void taskDoMeasurements(void* RTOSparameter)
        {
                RHTempSensor::makeMeasurements();
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

