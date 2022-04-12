#include "Common.h"

uint32_t getSecsSinceEpoch()
{
    return (uint32_t)time(NULL);
}