#ifndef MyByteAndBitNumber_STRUCT
#define MyByteAndBitNumber_STRUCT

#include <Arduino.h>
struct MyByteAndBitNumber
{
    uint8_t byteAddress;
    uint8_t bitNumber;
    bool valid;
};
#endif