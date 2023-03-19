#include <Adafruit_MCP3008.h>

#include "COSensor.hpp"
#include "Adc.hpp"

using namespace COSensor;

bool initializeSensor()
{
    Adc::initializeAdc();
}

uint32_t getCarbonMonoxidePartsPerMillion()
{
    uint32_t adcResult = Adc::readAdc();
    // TODO: Calculate the PPM from the ADC result.
}