#ifndef ADC_DRIVER_CONFIG_HPP
#define ADC_DRIVER_CONFIG_HPP

/*!
 *    @brief  Contains configurations.
 */
namespace Configurations
{
    /*!
     *    @brief  Contains configuration for the ADC peripherial.
     */
    namespace Adc
    {
        /*!
         *    @brief  The enumeration of possible ADC channels.
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
         *    @brief  The project defined defaults for the ADC.
         */
        namespace Defaults
        {
            /*!
             *    @brief  The project defined channel to read.
             */
            constexpr uint8_t ADC_CHANNEL = AdcChannel::ch1;

            /*!
             *    @brief  The project defined chip select (CS) pin for SPI communication with the ADC.
             */
            constexpr uint8_t SPI_CHIP_SELECT_PIN = 5;

            /*!
             *    @brief  The project defined number of ADC levels.
             */
            constexpr uint16_t PRECISION_BITS = 1024;

            /*!
             *    @brief  The project defined reference voltage the ADC to be measured against.
             */
            constexpr float REFERENCE_VOLTAGE = 1.5;
        }

        /*!
         *    @brief  The build defined channel to read.
         */
        const uint8_t ADC_CHANNEL = Defaults::ADC_CHANNEL;

        /*!
         *    @brief  The build defined chip select (CS) pin for SPI communication with the ADC.
         */
        const uint8_t SPI_CHIP_SELECT_PIN = Defaults::SPI_CHIP_SELECT_PIN;

        /*!
         *    @brief  The build defined number of ADC levels.
         */
        const uint16_t PRECISION_BITS = Defaults::PRECISION_BITS;

        /*!
         *    @brief  The build defined reference voltage the ADC to be measured against.
         */
        const float REFERENCE_VOLTAGE = Defaults::REFERENCE_VOLTAGE;
    }
}

#endif // ADC_DRIVER_CONFIG_HPP
