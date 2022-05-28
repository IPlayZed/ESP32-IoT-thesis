#include "RelativeHumidityTempSensor.h"
#include "DHT.h"
#include "SerialLogger.h"
#define DHT_DEBUG

#define DHT_TYPE DHT11

namespace RHTempSensor
{
    static DHT dht_sensor(RHTempSensor::PIN, DHT_TYPE);

    bool handleResult(float *checkable, float *storage)
    {
        if (isnan(*checkable))
        {
            return false;
        }
        else
        {
            *storage = *checkable;
            return true;
        }
    }

    void initializeSensor()
    {
        dht_sensor.begin();
    }

    void readHumidity(float *humidity)
    {
        float humidity_result = dht_sensor.readHumidity();
        if (handleResult(&humidity_result, humidity))
        {
#ifdef DEBUG_MODE
            Logger.Info("Measured humidity: " + String(RHTempSensor::humidity_result) + "%");
#endif
        }
        else
        {
#ifdef DEBUG_MODE
            Logger.Error("Failed to read humidity from sensor.");
#endif
        }
    }

    void readTemperature(float *temperature)
    {
        float temperature_result = dht_sensor.readTemperature();
        if (handleResult(&temperature_result, temperature))
        {
#ifdef DEBUG_MODE
            Logger.Info("Measured temperature: " + String(RHTempSensor::temperature_result) + "°C");
#endif
        }
        else
        {
#ifdef DEBUG_MODE
            Logger.Error("Failed to read humidity from sensor.");
#endif
        }
    }

    void makeMeasurements()
    {
        for (uint8_t i = 0; i < 10; i++)
        {
            readHumidity(arr_humidity + i);
            readTemperature(arr_temperature + i);
        }
    }

    float getHumidity()
    {
        float avarage = 0;
        for (uint8_t i = 0; i < 10; i++)
        {
            avarage += arr_humidity[i];
        }
        return avarage / 10;
    }

    float getTemperature()
    {
        float avarage = 0;
        for (uint8_t i = 0; i < 10; i++)
        {
            avarage += arr_humidity[i];
        }
        return avarage / 10;
    }
}
