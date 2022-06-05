// Licensed under: GNU GENERAL PUBLIC LICENSE Version 2

#include "Common.h"

uint32_t getSecsSinceEpoch()
{
    return (uint32_t)time(NULL);
}