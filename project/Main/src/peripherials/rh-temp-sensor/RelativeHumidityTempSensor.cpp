#include <DHT_U.h>
#include <DHT.h>

#include "../../common/serial-logger/SerialLogger.hpp"

#include "RelativeHumidityTempSensor.hpp"
#include "DHTConfig.hpp"

#define CPU_FREQ_DHT_COMPLIANT 240

namespace RHTempSensor
{
    static DHT dht_sensor(CONFIG_PIN, CONFIG_DHT_TYPE);

    static bool badResult = false;
    static uint8_t moving_avarage_actual_samples = CONFIG_MEASUREMENT_TIMES;

    /*!
     *    @brief  Checks if the result is a number. If it is one, then it stores it in storage and returns true. If it is not, then it ignores it and returns false, leaving handling to caller.
     *    @return true if measurement result could be handled, false if not
     */
    // FIXME: Float should not be handled as a memory address if the CPU has appropriate register handling for this non-complex type.
    bool _handleResult(float *checkable, float *storage)
    {
        if (checkable == nullptr or storage == nullptr)
        {
            LogError("Could not handle result becaus either the checked or storage pointer was null");
            RHTempSensor::badResult = true;
            return false;
        }
        else if (isnan(*checkable))
        {
            RHTempSensor::badResult = true;
            return false;
        }
        else
        {
            *storage = *checkable;
            return true;
        }
    }

    // FIXME: Float should not be handled as a memory address if the CPU has appropriate register handling for this non-complex type.
    void _readFromSensor(float *storage, float result)
    {
        bool is_ok = RHTempSensor::_handleResult(&result, storage);
        if (!is_ok)
        {
            RHTempSensor::moving_avarage_actual_samples--;
            *storage = 0;
        }
    }

    static void _handleLogging(float *value, uint8_t *i, bool isHumidity)
    {
        if (value == nullptr or i == nullptr)
        {
            LogError("Could not handle logging as either the value or the index was null");
            return;
        }

        if (!RHTempSensor::badResult)
        {
            if (!isHumidity)
            {
                return LogInfo("Measured temperature: " + String(*(value + *i)) + "°C");
            }
            else
            {
                return LogInfo("Measured humidity: " + String(*(value + *i)) + "%");
            }
        }
        else
        {
            RHTempSensor::badResult = false;
            LogError("Sensor read value was NaN!");
        }
    }

    void initializeSensor(void)
    {
#ifdef DEBUG_MODE
        LogInfo("Trying to initialize DHT sensor, but due to library limitations WILL NOT have proper feedback if it was successful!");
#endif
        dht_sensor.begin();
    }

    // FIXME: Find out why DHT11 only works when CPU freq is at 240 MHz. Maybe it should be reinitialized here?
    void makeMeasurements(void)
    {
        setCpuFrequencyMhz(CPU_FREQ_DHT_COMPLIANT);
        for (uint8_t i = 0; i < CONFIG_MEASUREMENT_TIMES; i++)
        {
            RHTempSensor::_readFromSensor(arr_humidity + i, dht_sensor.readHumidity());

            RHTempSensor::_handleLogging(arr_humidity, &i, true);

            RHTempSensor::_readFromSensor(arr_temperature + i, dht_sensor.readTemperature());

            RHTempSensor::_handleLogging(arr_temperature, &i, false);

            delay(DHT_SAMPLING_PERIOD);
        }
        setCpuFrequencyMhz(CPU_FREQ_POWER_SAVER);
    }

    float getHumidity(void)
    {
        float average = 0;
        for (uint8_t i = 0; i < CONFIG_MEASUREMENT_TIMES; i++)
        {
            average += RHTempSensor::arr_humidity[i];
        }
        uint8_t oldval = RHTempSensor::moving_avarage_actual_samples;
        RHTempSensor::moving_avarage_actual_samples = CONFIG_MEASUREMENT_TIMES;
        return average / (float)oldval;
    }

    float getTemperature(void)
    {
        float average = 0;
        for (uint8_t i = 0; i < CONFIG_MEASUREMENT_TIMES; i++)
        {
            average += RHTempSensor::arr_temperature[i];
        }
        uint8_t oldval = RHTempSensor::moving_avarage_actual_samples;
        RHTempSensor::moving_avarage_actual_samples = CONFIG_MEASUREMENT_TIMES;
        return average / (float)oldval;
    }
}
