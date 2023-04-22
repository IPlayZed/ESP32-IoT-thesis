#ifndef ADC_DRIVER_CONFIG_H
#define ADC_DRIVER_CONFIG_H

namespace Configurations
{
    namespace Adc
    {

        // TODO: Maybe this should be in a per-device header file.
        /*!
         *    @brief  An enumeration of possible ADC channels.
         */
        enum AdcChannel : uint8_t
        {
            ch1 = 1,
            ch2 = 2,
            ch3 = 3,
            ch4 = 4,
            ch5 = 5,
            ch6 = 6,
            ch7 = 7,
            ch8 = 8

        };

        /*!
         *    @brief  This namespace contains the default values for configuration, as it is set in the hardware prototype.
         */
        namespace Defaults
        {
            /*!
             *    @brief  The number of possible values. 2^N, where N is the number of bits representing the value of the ADC.
             */
            constexpr uint8_t DEFAULT_ADC_CHANNEL = AdcChannel::ch1;

            /*!
             *    @brief  The number of possible values. 2^N, where N is the number of bits representing the value of the ADC.
             */
            constexpr uint8_t DEFAULT_SPI_CHIP_SELECT_PIN = 5;

            /*!
             *    @brief  The number of possible values. 2^N, where N is the number of bits representing the value of the ADC.
             */
            constexpr uint16_t DEFAULT_RESISTOR_VALUE = 10000;

            /*!
             *    @brief  The number of possible values. 2^N, where N is the number of bits representing the value of the ADC.
             */
            constexpr uint16_t DEFAULT_PRECISION_BITS = 1024;

            /*!
             *    @brief  The number of possible values. 2^N, where N is the number of bits representing the value of the ADC.
             */
            constexpr float DEFAULT_REFERENCE_VOLTAGE = 1.5;
        }

        /*!
         *    @brief  The number of possible values. 2^N, where N is the number of bits representing the value of the ADC.
         */
        const uint8_t CONFIG_ADC_CHANNEL = Defaults::DEFAULT_ADC_CHANNEL;

        /*!
         *    @brief  The number of possible values. 2^N, where N is the number of bits representing the value of the ADC.
         */
        const uint8_t CONFIG_SPI_CHIP_SELECT_PIN = Defaults::DEFAULT_SPI_CHIP_SELECT_PIN;

        /*!
         *    @brief  The number of possible values. 2^N, where N is the number of bits representing the value of the ADC.
         */
        const uint16_t CONFIG_RESISTOR_VALUE = Defaults::DEFAULT_RESISTOR_VALUE;

        /*!
         *    @brief  The number of possible values. 2^N, where N is the number of bits representing the value of the ADC.
         */
        const uint16_t CONFIG_PRECISION_BITS = Defaults::DEFAULT_PRECISION_BITS;

        /*!
         *    @brief  The number of possible values. 2^N, where N is the number of bits representing the value of the ADC.
         */
        const float CONFIG_REFERENCE_VOLTAGE = Defaults::DEFAULT_REFERENCE_VOLTAGE;
    }
}

#endif
