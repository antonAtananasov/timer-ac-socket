#ifndef MyErrorCode_ENUM
#define MyErrorCode_ENUM
#include <Arduino.h>
enum MyErrorCode : uint8_t
{
    ERRORCODE_NONE,
    ERRORCODE_REBOOT,
    ERRORCODE_UNKNOWN=255,
};
#endif
