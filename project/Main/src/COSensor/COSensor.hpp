#ifndef CO_SENSOR_HPP
#define CO_SENSOR_HPP

namespace COSensor
{
    /*!
     *    @brief  Initialize the relevant hardware for the CO sensor.
     *    @return true if initialization was successful
     */
    bool initializeSensor();

    /*!
     *    @brief  Gets the carbon monoxide level.
     *    @return the PPM of CO in the air
     */
    uint32_t getCarbonMonoxidePartsPerMillion();
}

#endif // CO_SENSOR_HPP