#ifndef RELATIVE_HUMIDITY_TEMP_SENSOR_H
#define RELATIVE_HUMIDITY_TEMP_SENSOR_H

#include "Common.h"
#include "RelativeHumidityTempSensorConfig.h"

namespace RHTempSensor
{
    static float humidity;
    static float temperature;
    static float arr_humidity[CONFIG_MEASUREMENT_TIMES];
    static float arr_temperature[CONFIG_MEASUREMENT_TIMES];

    void initializeSensor();
    void makeMeasurements();

    float getHumidity();
    float getTemperature();
}

#endif
