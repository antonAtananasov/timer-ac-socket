#ifndef MyTime_STRUCT
#define MyTime_STRUCT

#include <Arduino.h>

struct MyTime
{
    uint8_t hour, minute, second;
    uint16_t millisecond;
};
#endif