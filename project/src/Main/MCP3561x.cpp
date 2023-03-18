#include "MCP3561x.hpp"
#include "Common.hpp"

namespace Hardware
{
    namespace Adc
    {
        namespace MCP3561x
        {
            namespace Commands
            {
                const uint8_t DEV_ADDR_LBITSHIFT = 6, REG_FST_CMD_ADDR_LBITSHIFT = 4;
                const uint8_t DEV_ADDR_MAX = 0b11, REG_FST_CMD_ADDR_MAX = 0b1111, CMD_TYP_BIT_MAX = 0b11;

                bool CheckIfCmdIsDontCare(uint8_t command)
                {
                    bool foundAnIdc = false;
                    for (uint8_t idc : dontCares)
                    {
                        if (command == idc) foundAnIdc = true;
                    }
                    LogInfo("The command constructed was a DON'T CARE command: " + String(command));
                    return foundAnIdc;
                }

                bool ConstructCommand(uint8_t deviceAddr, uint8_t registerOrFastCmdAddr, uint8_t commandTypeBits)
                {
                    if (deviceAddr > DEV_ADDR_MAX)
                    {
                        LogError("The device address provided during command construction was invalid: " + String(deviceAddr));
                        return false;
                    }
                    else if (registerOrFastCmdAddr > REG_FST_CMD_ADDR_MAX)
                    {
                        LogError("The register address or fast command provided was invalid: " + String(registerOrFastCmdAddr));
                        return false;
                    }
                    else if (commandTypeBits > CMD_TYP_BIT_MAX)
                    {
                        LogError("The command type bits provided were invalid: " + String(commandTypeBits));
                        return false;
                    }
                    
                    uint8_t resultingCommand = 0; // empty bitfield for efficient bitwise operation
                    resultingCommand |= ((deviceAddr << DEV_ADDR_LBITSHIFT) | (registerOrFastCmdAddr << REG_FST_CMD_ADDR_LBITSHIFT) | commandTypeBits);
                    
                    storage = resultingCommand;
                    return CheckIfCmdIsDontCare(resultingCommand);
                }
            }
        }
    }   
}
