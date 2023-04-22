#ifndef COMMON_CONFIG_HPP
#define COMMON_CONFIG_HPP

#include "../networking/Network.hpp"

#define DEFAULT_INBOUND_DATA_SIZE_BYTES 128
#define DEFAULT_SERIAL_BAUD_RATE 9600
#define DEFAULT_TIME_TO_SLEEP_IN_S 30
#define DEFAULT_DELAY_FOR_SERIAL_CONNECTION 5000

#define CONFIG_SERIAL_BAUD_RATE DEFAULT_SERIAL_BAUD_RATE
#define CONFIG_TIME_TO_SLEEP_IN_S DEFAULT_TIME_TO_SLEEP_IN_S
#define CONFIG_DELAY_FOR_SERIAL_CONNECTION DEFAULT_DELAY_FOR_SERIAL_CONNECTION

namespace Configurations
{
    namespace Common
    {
        namespace Defaults
        {
            constexpr uint8_t INBOUND_DATA_SIZE_BYTES = 128;
            constexpr uint16_t SERIAL_BAUD_RATE = 9600;
            constexpr uint16_t TIME_TO_SLEEP_SEC = 30;
            constexpr uint16_t DELAY_FOR_SERIAL_MILISEC = 5000;
        }

        const uint8_t INBOUND_DATA_SIZE_BYTES = Defaults::INBOUND_DATA_SIZE_BYTES;
        const uint16_t SERIAL_BAUD_RATE = Defaults::SERIAL_BAUD_RATE;
        const uint16_t TIME_TO_SLEEP_SEC = Defaults::TIME_TO_SLEEP_SEC;
        const uint16_t DELAY_FOR_SERIAL_MILISEC = Defaults::DELAY_FOR_SERIAL_MILISEC;
    }
}

#endif // COMMON_CONFIG_HPP
