#include <Adafruit_MCP3008.h>

#include "../ADC/Adc.hpp"

#include "COSensor.hpp"

using namespace COSensor;

bool initializeSensor()
{
    Adc::initializeAdc();
}

double getCarbonMonoxidePartsPerMillion()
{
    uint32_t adcResult = Adc::readAdc();
    // TODO: Maybe extract this into a general ADC source file.
    double adcCurrent = (adcResult * Configurations::Adc::CONFIG_REFERENCE_VOLTAGE) / Configurations::Adc::CONFIG_PRECISION_BITS;
    return (adcCurrent * 1000000000.0) / 2.0; // Scale to nA. 1 PPM is per 2 nA.
}