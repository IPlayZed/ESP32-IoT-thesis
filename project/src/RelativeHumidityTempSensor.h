#ifndef RELATIVE_HUMIDITY_TEMP_SENSOR_H
#define RELATIVE_HUMIDITY_TEMP_SENSOR_H

#include "HumidityRelativeTempSensorConfiguration.h"
#include <stdint.h>

namespace RHTempSensor
{
    static const uint8_t PIN = CONFIG_PIN;
    static float humidity;
    static float temperature;

    void initializeSensor();
    void readHumidity();
    void readTemperature();

    float getHumidity();
    float getTemperature();
}

#endif
