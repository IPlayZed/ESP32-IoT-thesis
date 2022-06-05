#include "RelativeHumidityTempSensor.h"
#include "DHT.h"
#include "SerialLogger.h"

#define DHT_TYPE DHT11


// TODO: Implement moving avarage (so that failed measurements do not mess up the value).
namespace RHTempSensor
{
    static DHT dht_sensor(RHTempSensor::PIN, DHT_TYPE);

    static bool badResult = false;

    // Checks if the result is a number.
    // If it is one, then it stores it in storage and returns true.
    // If it is not, then it ignores it and returns false, leaving handling to caller.
    bool handleResult(float *checkable, float *storage)
    {
        if (isnan(*checkable))
        {
            badResult = true;
            return false;
        }
        else
        {
            *storage = *checkable;
            return true;
        }
    }

    void readFromSensor(float *storage, float result)
    {
        bool is_ok = RHTempSensor::handleResult(&result, storage);
        if (!is_ok)
            *storage = 0;
    }

    static void handleLogging(float *value, uint8_t *i, bool isHumidity)
    {
        if (!RHTempSensor::badResult)
        {
            if (!isHumidity)
            {
                LogInfo("Measured temperature: " + String(*(value + *i)) + "°C");
            }
            else
            {
                LogInfo("Measured humidity: " + String(*(value + *i)) + "%");
            }
        }
        else
        {
            RHTempSensor::badResult = false;
            LogError("Sensor read value was NaN!");
        }
    }

    void initializeSensor()
    {
        dht_sensor.begin();
    }

    void makeMeasurements()
    {
        for (uint8_t i = 0; i < TIMES; i++)
        {
            RHTempSensor::readFromSensor(arr_humidity + i, dht_sensor.readHumidity());

            RHTempSensor::handleLogging(arr_humidity, &i, true);

            RHTempSensor::readFromSensor(arr_temperature + i, dht_sensor.readTemperature());

            RHTempSensor::handleLogging(arr_temperature, &i, false);

            delay(2000);
        }
    }

    float getHumidity()
    {
        float average = 0;
        for (uint8_t i = 0; i < TIMES; i++)
        {
            average += RHTempSensor::arr_humidity[i];
        }
        return average / TIMES;
    }

    float getTemperature()
    {
        float average = 0;
        for (uint8_t i = 0; i < TIMES; i++)
        {
            average += RHTempSensor::arr_humidity[i];
        }
        return average / TIMES;
    }
}
