// Licensed under: GNU GENERAL PUBLIC LICENSE Version 2

#include <time.h>
#include "Common.h"

uint32_t getSecsSinceEpoch()
{
    return (uint32_t)time(NULL);
}