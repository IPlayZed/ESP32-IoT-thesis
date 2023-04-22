#include "Tasks.hpp"
#include "../SerialLogger/SerialLogger.hpp"
#include "../Common/Common.hpp"
#include "../Common/CommonConfig.hpp"
#include "../Network/Network.hpp"
#include "../RHSensor/RelativeHumidityTempSensor.hpp"
#include "../COSensor/COSensor.hpp"

namespace Tasks
{
        void taskConfigureLocalPeripherials(void *RTOSparameter)
        {
#ifdef DEBUG_MODE
                Helpers::taskHelperDelayForSerialConnection();
                Serial.begin(CONFIG_SERIAL_BAUD_RATE);
#endif
                LogInfo("Started task \"taskConfigureLocalPeripherials\"");
                LogInfo("Started peripherial initialization....");
                LogInfo("Trying to set power save clock to " + String(CPU_FREQ_POWER_SAVER));
                setCpuFrequencyMhz(CPU_FREQ_POWER_SAVER);
                esp_sleep_enable_timer_wakeup(CONFIG_TIME_TO_SLEEP_IN_S * uS_TO_S_FACTOR);

                RHTempSensor::initializeSensor();
                COSensor::initializeSensor();
                LogInfo("Completed peripherial initialization!");
                LogInfo("Completed task \"taskConfigureLocalPeripherials\"");
        }
        void taskInitializeConnection(void *RTOSparameter)
        {
                LogInfo("Started task \"taskInitializeConnection\"");
                btStop();
                Network::setupNetworking();
                LogInfo("Completed task \"taskInitializeConnection\"");
        }
        void taskSendTelemetry(void *RTOSparameter)
        {
                LogInfo("Started task \"taskSendTelemetry\"");
                Network::Telemetry::telemetryData_t sensorData;
                sensorData.temperature = RHTempSensor::getTemperature();
                sensorData.humidity = RHTempSensor::getHumidity();
                // sensorData.CO = 2.0;
                sensorData.CO = COSensor::getCarbonMonoxidePartsPerMillion();
                Network::Telemetry::processTelemetryData(&sensorData);
                Network::Telemetry::sendTelemetry();
                LogInfo("Completed task \"taskSendTelemetry\"");
        }
        void taskDoMeasurements(void *RTOSparameter)
        {
                LogInfo("Started task \"taskDoMeasurements\"");
                RHTempSensor::makeMeasurements();
                LogInfo("Completed task \"taskDoMeasurements\"");
        }

        namespace Helpers
        {
                void taskHelperDelayForSerialConnection(void)
                {
                        DelayForSerialConnection(CONFIG_DELAY_FOR_SERIAL_CONNECTION);
                }
                void taskHelperStartSleep()
                {
                        LogInfo("Going to sleep! Zzzzz...");
                        esp_light_sleep_start();
                        LogInfo("Woke up from sleep!");
                }
        }
}
