#ifndef MyLedState_ENUM
#define MyLedState_ENUM
#include <Arduino.h>
enum MyLedState : uint8_t
{
    LED_OFF = false,
    LED_ON = true,
    LED_BLINK = 2,
    UNKNOWN_LED_STATE = 255
};
#endif
