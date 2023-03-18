#ifndef MCP3561x_HPP
#define MCP3561x_HPP

#include <string.h>

namespace Hardware
{
    namespace Adc
    {
        namespace MCP3561x
        {
            namespace Commands
            {
                uint8_t storage = 0;
                const uint8_t REGISTER_ADDR_BITWIDTH = 4;
                const uint8_t DEVICE_ADDRESS_BITWIDTH = 2;

                /**
                 * These values can be used to check against command values which have no effect on the ADC.
                 * This is useful because we can skip communication with the ADC, thus saving power.
                 */
                const uint8_t dontCares[] = {0x0, 0x4, 0x8, 0xC, 0x10, 0x14, 0x18, 0x1C, 0x20, 0x24};

                /**
                 * These can be used as the fast command arguments for command construction.
                 */
                enum FastCommands
                {
                    CMD_TYPE_FST_CMD_VAL = 0x0,     // 0b00
                    ADC_CONVERSION_START_CMD = 0xA, // 0b1010
                    ADC_STANDBY_CMD = 0xB,          // 0b1011
                    ADC_SHUTDOWN_CMD = 0xC,         // 0b1100
                    ADC_FULL_SHUTDOWN_CMD = 0xD,    // 0b1101
                    ADC_DEVICE_FULL_RESET = 0xE,    // 0b1110
                };

                bool CheckIfCmdIsDontCare(uint8_t command);

                // TODO:: Check wheter it would be better to take storage as an argument. Other behaviour may be preferred.
                /**
                 * Constructs a command and saves it for later query.
                 * @param deviceAddr: The device address.
                 * @param registerOrFastCmdAddr: The register address or predefined fast command value.
                 * @param commandTypeBits: The value defining command type.
                 * @returns True if the command could be constructed and if it is one which is not a don't care value.
                 */
                bool ConstructCommand(uint8_t deviceAddr, uint8_t registerOrFastCmdAddr, uint8_t commandTypeBits);
            }
        }
    }
}

#endif
