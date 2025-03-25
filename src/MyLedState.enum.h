#ifndef MyLedState_ENUM
#define MyLedState_ENUM
#include <Arduino.h>
enum MyLedState : uint8_t
{
    LED_OFF = false,
    LED_ON = true,
    LED_BLINK_SLOW,
    LED_BLINK_FAST,
    UNKNOWN_LED_STATE = 255
};
#endif
