#include "RelativeHumidityTempSensor.h"
#include "DHTConfig.h"
#include "DHT.h"
#include "DHTConfig.h"
#include "SerialLogger.h"

// TODO: Implement moving avarage (so that failed measurements do not mess up the value).
namespace RHTempSensor
{
    static DHT dht_sensor(CONFIG_PIN, CONFIG_DHT_TYPE);

    static bool badResult = false;
    static uint8_t moving_avarage_actual_samples = CONFIG_MEASUREMENT_TIMES;

    // Checks if the result is a number.
    // If it is one, then it stores it in storage and returns true.
    // If it is not, then it ignores it and returns false, leaving handling to caller.
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
        dht_sensor.begin();
    }

    void makeMeasurements(void)
    {
        for (uint8_t i = 0; i < CONFIG_MEASUREMENT_TIMES; i++)
        {
            RHTempSensor::_readFromSensor(arr_humidity + i, dht_sensor.readHumidity());

            RHTempSensor::_handleLogging(arr_humidity, &i, true);

            RHTempSensor::_readFromSensor(arr_temperature + i, dht_sensor.readTemperature());

            RHTempSensor::_handleLogging(arr_temperature, &i, false);

            delay(DHT_SAMPLING_PERIOD);
        }
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
        return average / oldval;
    }

    float getTemperature(void)
    {
        float average = 0;
        for (uint8_t i = 0; i < CONFIG_MEASUREMENT_TIMES; i++)
        {
            average += RHTempSensor::arr_humidity[i];
        }
        uint8_t oldval = RHTempSensor::moving_avarage_actual_samples;
        RHTempSensor::moving_avarage_actual_samples = CONFIG_MEASUREMENT_TIMES;
        return average / oldval;
    }
}
