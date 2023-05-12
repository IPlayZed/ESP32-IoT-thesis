#include <Adafruit_MCP3008.h>

#include "../../common/Common.hpp"
#include "Adc.hpp"

Adafruit_MCP3008 adc;

bool Adc::initializeAdc(uint8_t chipSelectPin)
{
#ifdef DEBUG_MODE
    LogInfo("Trying to initialize the ADC for CS pin: " + String(chipSelectPin));
    bool chip_init_success = adc.begin(chipSelectPin);
    if (true == chip_init_success)
    {
        LogInfo("Hardware SPI communication with ADC successful on CS pin " + String(chipSelectPin));
    }
    else
    {
        LogError("Hardware SPI communication with ADC failed on CS pin " + String(chipSelectPin));
    }
    return chip_init_success;
#endif
#ifndef DEBUG_MODE
    return adc.begin(chipSelectPin)
#endif
}

uint32_t Adc::readAdc(uint8_t channel)
{
#ifdef DEBUG_MODE
    LogInfo("Trying to read the ADC from channel " + String(channel));
    uint32_t value_read = adc.readADC(channel);
    if (-1 == value_read)
    {
        LogError("ADC channel " + String(channel) + " is incorrect!");
    }
    else if (0 == value_read)
    {
        LogWarning("Reading from ADC channel " + String(channel) + " resulted in 0. This might be correct, but it can also indicate incorrect functioning!");
    }
    else
    {
        LogInfo("ADC read successfully with value: " + String(value_read));
    }
    return value_read;
#endif
#ifndef DEBUG_MODE
    return adc.readADC(channel);
#endif
}
