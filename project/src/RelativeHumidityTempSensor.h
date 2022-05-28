#ifndef RELATIVE_HUMIDITY_TEMP_SENSOR_H
#define RELATIVE_HUMIDITY_TEMP_SENSOR_H

#include "HumidityRelativeTempSensorConfiguration.h"
#include "Common.h"
#include <stdint.h>

namespace RHTempSensor
{
    static const uint8_t PIN = CONFIG_PIN;
    static float humidity;
    static float temperature;
    static float arr_humidity[10];
    static float arr_temperature[10];

    void initializeSensor();
    void makeMeasurements();

    float getHumidity();
    float getTemperature();
}

#endif
