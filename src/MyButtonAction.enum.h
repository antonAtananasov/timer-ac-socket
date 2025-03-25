#ifndef MyButtonAction_ENUM
#define MyButtonAction_ENUM

enum MyButtonAction : uint8_t
{
    BUTTON_NONE,
    BUTTON_DOWN,
    BUTTON_UP,
    BUTTON_SHORT_PRESS,
    BUTTON_LONG_PRESS,
    BUTTON_UNKNOWN = 255
};

#endif