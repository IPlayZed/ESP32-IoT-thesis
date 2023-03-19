#include <Adafruit_MCP3008.h>

#include "Adc.hpp"

Adafruit_MCP3008 adc;

bool Adc::initializeAdc(uint8_t chipSelectPin)
{
    adc.begin(chipSelectPin);
}

uint32_t Adc::readAdc(uint8_t channel)
{
    return adc.readADC(channel);
}
