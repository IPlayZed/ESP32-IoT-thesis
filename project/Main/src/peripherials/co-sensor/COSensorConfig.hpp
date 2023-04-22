#ifndef CO_SENSOR_CONFIG_HPP
#define CO_SENSOR_CONFIG_HPP

/*!
 *    @brief  Contains configurations.
 */
namespace Configurations
{
    /*!
     *    @brief  Contains configuration for the CO sensor.
     */
    namespace COSensor
    {
        /*!
         *    @brief  Contains the project defined default values for the CO sensor configuration.
         */
        namespace Defaults
        {
            /*!
             *    @brief  The project defined resistor value.
             */
            constexpr uint16_t RESISTOR_VALUE = 10000;
        }

        /*!
         *    @brief  The build defined resistor value.
         */
        const uint16_t RESISTOR_VALUE = Defaults::RESISTOR_VALUE;
    }
}

#endif // CO_SENSOR_CONFIG_HPP
