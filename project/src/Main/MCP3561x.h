#ifndef MCP3561x_H
#define MCP3561x_H

#include <string.h>

namespace Hardware
{
    namespace MCP3561x
    {
        namespace Commands
        {
            const uint8_t DontCares[] = { 0x0, 0x4, 0x8, 0xC, 0x10, 0x14, 0x18, 0x1C, 0x20, 0x24 };
            enum FastCommands {
                ADC_CONVERSION_START_CMD = 0x28, //00 1010 00
                ADC_STANDBY_CMD = 0x2C, //00 1011 00
                ADC_SHUTDOWN_CMD = 0x30, //00 1100 00
                ADC_FULL_SHUTDOWN_CMD = 0x34, //00 1101 00
                ADC_DEVICE_FULL_RESET = 0x3C //00 1111 00
            };
        }
    }
}

#endif
