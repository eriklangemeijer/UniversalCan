#pragma once

#include <stdint.h>
struct CanMessage
{
    uint16_t ID;
    uint8_t Data[8];
};