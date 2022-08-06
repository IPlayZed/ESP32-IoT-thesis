#ifndef DRIVER_H
#define DRIVER_H

typedef uint8_t DRIVER_ERR_T;

namespace Interfaces
{
    namespace Driver
    {
        DRIVER_ERR_T SendCommand();
        DRIVER_ERR_T RecieveData();
    }
}

#endif