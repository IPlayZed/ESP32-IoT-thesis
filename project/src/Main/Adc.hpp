#ifndef ADC_HPP
#define ADC_HPP

#include "AdcConfig.hpp"

namespace Adc
{
    bool initializeAdc(uint8_t chipSelectPin =);
    uint32_t readAdc(uint8_t channel = Configurations::Adc::CONFIG_ADC_CHANNEL);
}

#endif // ADC_HPP