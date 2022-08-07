#ifndef ADC_DRIVER_H
#define ADC_DRIVER_H

// TODO: Investigate what behaviours should be implemented.
namespace Interfaces
{
    namespace Drivers
    {
        namespace Adc
        {
            float ReadVoltage();
            uint8_t SendCommand();
        }
    }
}

#endif