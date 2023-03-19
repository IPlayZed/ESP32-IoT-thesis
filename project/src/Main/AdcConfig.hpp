#ifndef ADC_DRIVER_CONFIG_H
#define ADC_DRIVER_CONFIG_H

namespace Configurations
{
    namespace Adc
    {

        enum AdcChannel : uint8_t
        {
            ch1 = 0,
            ch2 = 1,
            ch3 = 2,
            ch4 = 3,
            ch5 = 4,
            ch6 = 5,
            ch7 = 6,
            ch8 = 7

        };

        namespace Defaults
        {
            constexpr uint8_t DEFAULT_ADC_COMMAND_BIT_WIDTH = 8;
            constexpr uint8_t DEFAULT_ADC_CHANNEL = AdcChannel::ch1; // TODO: Verify this.
            constexpr uint8_t DEFAULT_SPI_CHIP_SELECT_PIN = 5;
        }

        const uint8_t CONFIG_ADC_COMMAND_BIT_WIDTH = Defaults::DEFAULT_ADC_COMMAND_BIT_WIDTH;
        const uint8_t CONFIG_ADC_CHANNEL = Defaults::DEFAULT_ADC_CHANNEL;
        const uint8_t CONFIG_SPI_CHIP_SELECT_PIN = 5;
    }
}

#endif
