#ifndef TASKS_HPP
#define TASKS_HPP

/*!
 *    @brief  Contains the tasks to be executed by firmware.
 */
namespace Tasks
{
    /*!
     *    @brief  Configure all local peripherials.
     */
    void configureLocalPeripherials(void *RTOSparameter = nullptr);
    /*!
     *    @brief  Initialize connection to remote.
     */
    void initializeConnection(void *RTOSparameter = nullptr);
    /*!
     *    @brief  Send the collected telemetry.
     */
    void sendTelemetry(void *RTOSparameter = nullptr);
    /*!
     *    @brief  Make local measurements.
     */
    void doMeasurements(void *RTOSparameter = nullptr);

    /*!
     *    @brief  Contains the tasks to be executed by firmware.
     */
    namespace Helpers
    {
        /*!
         *    @brief  Delay UART serial. Useful for debugging.
         */
        void delayForSerialConnection(void);
        /*!
         *    @brief  Change into sleep mode.
         */
        void startSleep();
    }
}

#endif // TASKS_HPP
