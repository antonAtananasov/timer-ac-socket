#ifndef MyLedAndState_STRUCT
#define MyLedAndState_STRUCT

#include <Arduino.h>
#include "MyLedState.enum.h"
struct MyLedAndState
{
    uint8_t led;
    MyLedState state;
};
#endif