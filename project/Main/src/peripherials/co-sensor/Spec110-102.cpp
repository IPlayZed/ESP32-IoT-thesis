#include <Adafruit_MCP3008.h>

#include "../Common/Common.hpp"
#include "../ADC/Adc.hpp"

#include "COSensor.hpp"

double ppm = 0.0;

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

void COSensor::makeMeasurement()
{
    LogInfo("Trying to measure CO level.");
    uint32_t adcResult = Adc::readAdc();    
    double adcCurrent = (adcResult * Configurations::Adc::REFERENCE_VOLTAGE) / Configurations::Adc::PRECISION_BITS;
    LogInfo("ADC current is: " + String(adcCurrent) + "A");
    double _ppm = (adcCurrent * 1000000000.0) / 2.0; // Calculated by scaling to nA and 1 PPM is per 2 nA.
    LogInfo("CO level is: " + String(_ppm) + "PPM");
    ppm = _ppm;
}

double COSensor::getPPMLevel()
{
    return ppm;
}
