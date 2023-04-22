#include <Adafruit_MCP3008.h>

#include "../Common/Common.hpp"
#include "../ADC/Adc.hpp"

#include "COSensor.hpp"

bool COSensor::initializeSensor()
{
    LogInfo("CO sensor triggered ADC initialization.");
    bool chip_init_success = Adc::initializeAdc();
    if (true == chip_init_success)
    {
        LogInfo("CO initialization successful!");
    }
    else
    {
        LogError("CO initialization failed!");
    }
    return chip_init_success;
}

double COSensor::getCarbonMonoxidePartsPerMillion()
{
    LogInfo("Trying to measure CO level.");
    uint32_t adcResult = Adc::readAdc();
    double adcCurrent = (adcResult * Configurations::Adc::CONFIG_REFERENCE_VOLTAGE) / Configurations::Adc::CONFIG_PRECISION_BITS;
    LogInfo("ADC current is: " + String(adcCurrent) + "A");
    double ppm = (adcCurrent * 1000000000.0) / 2.0;
    LogInfo("CO level is: " + String(ppm) + "PPM");
    return ppm; // Scale to nA. 1 PPM is per 2 nA.
}