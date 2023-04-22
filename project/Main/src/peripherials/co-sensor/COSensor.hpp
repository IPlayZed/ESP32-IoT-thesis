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
     *    @brief  Makes a measurement.
     */
    void makeMeasurement();

    /*!
     *    @brief  Gets the carbon monoxide level.
     *    @return the parts-per-million (PPM) of CO in the air
     */
    double getPPMLevel();
}

#endif // CO_SENSOR_HPP
