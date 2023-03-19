#include <Adafruit_MCP3008.h>

#include "Adc.hpp"

using namespace Adc;

Adafruit_MCP3008 adc;

bool initializeAdc(uint8_t chipSelectPin)
{
    adc.begin(chipSelectPin);
}

uint32_t readAdc(uint8_t channel)
{
    return adc.readADC(channel);
}
