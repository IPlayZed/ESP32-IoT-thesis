// Licensed under: GNU GENERAL PUBLIC LICENSE Version 2

#include <time.h>
#include "Common.hpp"

uint32_t getSecsSinceEpoch()
{
    return (uint32_t)time(NULL);
}
