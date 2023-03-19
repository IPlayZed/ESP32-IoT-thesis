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
                setCpuFrequencyMhz(CPU_FREQ_POWER_SAVER);
                esp_sleep_enable_timer_wakeup(CONFIG_TIME_TO_SLEEP_IN_S * uS_TO_S_FACTOR);
                Serial.begin(CONFIG_SERIAL_BAUD_RATE);
                RHTempSensor::initializeSensor();
        }
        void taskInitializeConnection(void *RTOSparameter)
        {
                btStop();
                Network::setupNetworking();
        }
        void taskSendTelemetry(void *RTOSparameter)
        {
                Network::Telemetry::telemetryData_t sensorData;
                sensorData.temperature = RHTempSensor::getTemperature();
                sensorData.humidity = RHTempSensor::getHumidity();
                sensorData.CO = COSensor::getCarbonMonoxidePartsPerMillion();
                Network::Telemetry::processTelemetryData(&sensorData);
                Network::Telemetry::sendTelemetry();
        }
        void taskDoMeasurements(void *RTOSparameter)
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
