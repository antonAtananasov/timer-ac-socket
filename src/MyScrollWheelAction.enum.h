#ifndef MyScrollWheelAction_ENUM
#define MyScrollWheelAction_ENUM

enum MyScrollWheelAction : uint8_t
{
    SCROLL_NONE,
    SCROLL_CLOCKWISE,
    SCROLL_COUNTERCLOCKWISE,
    SCROLL_UNKNOWN = 255
};

#endif