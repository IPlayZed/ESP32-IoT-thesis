#ifndef ADC_DRIVER_H
#define ADC_DRIVER_H

#include <bits/stdc++.h>
#include "AdcDriverConfig.h"

// TODO: Investigate what behaviours should be implemented.
namespace Interfaces
{
    namespace Adc
    {
        std::bitset<Configurations::Adc::CONFIG_ADC_COMMAND_BIT_WIDTH>;
        float ReadVoltage();
        uint8_t SendCommand();
    }
}

#endif