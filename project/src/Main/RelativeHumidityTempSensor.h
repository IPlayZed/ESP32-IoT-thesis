#ifndef RELATIVE_HUMIDITY_TEMP_SENSOR_H
#define RELATIVE_HUMIDITY_TEMP_SENSOR_H

#include "HumidityRelativeTempSensorConfiguration.h"
#include "Common.h"
#include <stdint.h>
#define TIMES 1

namespace RHTempSensor
{
    static const uint8_t PIN = CONFIG_PIN;
    static float humidity;
    static float temperature;
    static float arr_humidity[TIMES];
    static float arr_temperature[TIMES];

    void initializeSensor();
    void makeMeasurements();

    float getHumidity();
    float getTemperature();
}

#endif
