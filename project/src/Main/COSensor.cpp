#include <Adafruit_MCP3008.h>

#include "COSensor.hpp"

namespace COSensor{
    static Adafruit_MCP3008 adc;

    bool initializeSensor(){
        return adc.begin(5);
    }

    int16_t getCarbonMonoxidePartsPerMillion(){
        uint32_t adc_value = adc.
    }
}