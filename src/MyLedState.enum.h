#ifndef MyLedState_ENUM
#define MyLedState_ENUM
#include <Arduino.h>
enum MyLedState : uint8_t
{
    UNKNOWN = 255,
    LED_OFF = false,
    LED_ON = true,
    LED_BLINK = 2
};
#endif
