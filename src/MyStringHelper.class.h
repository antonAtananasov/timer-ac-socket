#ifndef MyStringHelper_CLASS
#define MyStringHelper_CLASS

#include <Arduino.h>
#include <avr/pgmspace.h>
class MyStringHelper
{
public:
    static const PROGMEM char m1[];
    static const PROGMEM char m2[];
    static const PROGMEM char m3[];
    static const PROGMEM char m4[];
    static const PROGMEM char m5[];
    static const PROGMEM char m6[];
    static const PROGMEM char m7[];
    static const PROGMEM char m8[];
    static const PROGMEM char m9[];
    static const PROGMEM char m10[];
    static const PROGMEM char m11[];
    static const PROGMEM char m12[];
    static const PROGMEM char m13[];
    static const PROGMEM char m14[];
    static const PROGMEM char m15[];
    static const PROGMEM char m16[];

    static const uint8_t helpMessageCount = 16;
    static const PROGMEM char *const helpMessages[helpMessageCount];
};
const PROGMEM char MyStringHelper::m1[] = "CLOCKLED <hour 1-12> <state 0-3>";
const PROGMEM char MyStringHelper::m2[] = "SOCKETLED <socket 1-5> <state 0-3>";
const PROGMEM char MyStringHelper::m3[] = "STATUSLED <state 0-3>";
const PROGMEM char MyStringHelper::m4[] = "ALLLEDS <state 0-3>";
const PROGMEM char MyStringHelper::m5[] = "SETSOCK <socket 1-5> <state 0-1>";
const PROGMEM char MyStringHelper::m6[] = "GETSOCK <socket 1-5>";
const PROGMEM char MyStringHelper::m7[] = "LEDGRPLOGIC <0/1>";
const PROGMEM char MyStringHelper::m8[] = "LEDINDLOGIC <0/1>";
const PROGMEM char MyStringHelper::m9[] = "SOCKLOGIC <0/1>";
const PROGMEM char MyStringHelper::m10[] = "BTNLOGIC <0/1>";
const PROGMEM char MyStringHelper::m11[] = "GETTIME";
const PROGMEM char MyStringHelper::m12[] = "SETTIME <HH> <MM> <SS>";
const PROGMEM char MyStringHelper::m13[] = "GETACTIVE <HH> <MM> <socket 1-5>";
const PROGMEM char MyStringHelper::m14[] = "SETACTIVE <HH> <MM> <socket 1-5>";
const PROGMEM char MyStringHelper::m15[] = "SETMODE <mode 0-255>";
const PROGMEM char MyStringHelper::m16[] = "GETMODE";

const PROGMEM char *const MyStringHelper::helpMessages[MyStringHelper::helpMessageCount] = {
    MyStringHelper::m1,
    MyStringHelper::m2,
    MyStringHelper::m3,
    MyStringHelper::m4,
    MyStringHelper::m5,
    MyStringHelper::m6,
    MyStringHelper::m7,
    MyStringHelper::m8,
    MyStringHelper::m9,
    MyStringHelper::m10,
    MyStringHelper::m11,
    MyStringHelper::m12,
    MyStringHelper::m13,
    MyStringHelper::m14,
    MyStringHelper::m15,
    MyStringHelper::m16,
};

#endif