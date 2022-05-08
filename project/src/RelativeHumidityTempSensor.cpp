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

    void readHumidity()
    {
        float humidity_result = dht_sensor.readHumidity();
        if (handleResult(&humidity_result, &humidity))
        {
            Logger.Info("Measured humidity: " + String(RHTempSensor::humidity) + "%");
        }
        else
        {
            Logger.Error("Failed to read humidity from sensor.");
        }
    }

    void readTemperature()
    {
        float temperature_result = dht_sensor.readTemperature();
        if (handleResult(&temperature_result, &temperature))
        {
            Logger.Info("Measured temperature: " + String(RHTempSensor::temperature) + "°C");
        }
        else
        {
            Logger.Error("Failed to read humidity from sensor.");
        }
    }

    float getHumidity()
    {
        return humidity;
    }

    float getTemperature()
    {
        return temperature;
    }
}
