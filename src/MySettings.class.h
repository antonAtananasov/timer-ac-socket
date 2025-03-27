#ifndef MySettings_CLASS
#define MySettings_CLASS

#include <Arduino.h>
#include <EEPROM.h>
#include "MyConstants.const.h"
#include "UserSettings.const.h"
#include "MyByteAndBitNumber.struct.h"
#include "MyErrorCode.enum.h"

class MySettings
{
private:
    bool LED_GROUP_ACTIVE_LOGIC = DEFAULT_LED_GROUP_ACTIVE_LOGIC,
         LED_INDIVIDUAL_ACTIVE_LOGIC = DEFAULT_LED_INDIVIDUAL_ACTIVE_LOGIC,
         SOCKETS_ACTIVE_LOGIC = DEFAULT_SOCKET_ACTIVE_LOGIC,
         BUTTON_ACTIVE_LOGIC = DEFAULT_BUTTON_ACTIVE_LOGIC,
         ANIMATE_SOCKET_LEDS = DEFAULT_ANIMATE_SOCKET_LEDS;

    MyErrorCode errorCode = ERRORCODE_REBOOT;

public:
    MySettings(bool loadLogicLevelsFromEEPROMNow, bool loadTimeFromEEPROMNow)
    {
        if (loadLogicLevelsFromEEPROMNow && !loadLogicLevelsFromEEPROM())
        {
            saveHour(0);
            saveMinute(0);
            saveSecond(0);
        }
        if (loadTimeFromEEPROMNow)
            loadTimeFromEEPROM();
    };

    bool loadLogicLevelsFromEEPROM()
    {
        uint8_t logicLevelsByte = EEPROM.read(EEPROM_CONFIGURATION_BYTE_ADDRES);

        bool ledGroupActiveLogic = readEEPROMBit(logicLevelsByte, EEPROM_LOGIC_LEVEL_LED_GROUP_BIT_NUMBER);
        bool ledIndividualActiveLogic = readEEPROMBit(logicLevelsByte, EEPROM_LOGIC_LEVEL_LED_INDIVIDUAL_BIT_NUMBER);
        bool socketsActiveLogic = readEEPROMBit(logicLevelsByte, EEPROM_LOGIC_LEVEL_SOCKET_BIT_NUMBER);
        bool animateSocketLeds = readEEPROMBit(logicLevelsByte, EEPROM_ANIMATE_SOCKET_LEDS);

        return setSocketsActiveLogic(socketsActiveLogic) &&
               setLedGroupActiveLogic(ledGroupActiveLogic) &&
               setLedIndividualActiveLogic(ledIndividualActiveLogic) &&
               setSocketLedAnimationActive(animateSocketLeds);
    }
    bool loadTimeFromEEPROM()
    {
        bool successfulTime = isSavedTimeValid();
        if (successfulTime)
        {
            Serial.print(F("Time is "));
            Serial.print(getSavedHour());
            Serial.print(' ');
            Serial.print(getSavedMinute());
            Serial.println();
        }
        else
            Serial.println(F("Unable to load time"));
        return successfulTime;
    }

    bool setEEPROMBit(uint8_t byteAddress, uint8_t bitNumber, bool bitValue)
    {
        if (bitNumber > 8)
            return false;

        uint8_t logicLevelsByte = EEPROM.read(byteAddress);

        // replace target bit with zero and add it with the requested value
        logicLevelsByte = (logicLevelsByte &
                           ~(0b1 << bitNumber)) |
                          ((uint8_t)bitValue) << bitNumber;

        EEPROM.write(byteAddress, logicLevelsByte);

        return true;
    }
    bool readEEPROMBit(uint8_t byteAddress, uint8_t bitNumber) { return ((EEPROM.read(byteAddress) >> bitNumber) & 0b0001) == 1; }

    bool setLedGroupActiveLogic(bool level)
    {
        bool successful = setEEPROMBit(EEPROM_CONFIGURATION_BYTE_ADDRES, EEPROM_LOGIC_LEVEL_LED_GROUP_BIT_NUMBER, level);
        if (successful)
            LED_GROUP_ACTIVE_LOGIC = level;

        return successful;
    }
    bool setLedIndividualActiveLogic(bool level)
    {
        bool successful = setEEPROMBit(EEPROM_CONFIGURATION_BYTE_ADDRES, EEPROM_LOGIC_LEVEL_LED_INDIVIDUAL_BIT_NUMBER, level);
        if (successful)
            LED_INDIVIDUAL_ACTIVE_LOGIC = level;

        return successful;
    }
    bool setSocketsActiveLogic(bool level)
    {
        bool successful = setEEPROMBit(EEPROM_CONFIGURATION_BYTE_ADDRES, EEPROM_LOGIC_LEVEL_SOCKET_BIT_NUMBER, level);
        if (successful)
            SOCKETS_ACTIVE_LOGIC = level;

        return successful;
    }
    bool setButtonActiveLogic(bool level)
    {
        bool successful = setEEPROMBit(EEPROM_CONFIGURATION_BYTE_ADDRES, EEPROM_LOGIC_LEVEL_BUTTON_BIT_NUMBER, level);
        if (successful)
            BUTTON_ACTIVE_LOGIC = level;

        return successful;
    }
    bool setSocketLedAnimationActive(bool level)
    {
        bool successful = setEEPROMBit(EEPROM_CONFIGURATION_BYTE_ADDRES, EEPROM_ANIMATE_SOCKET_LEDS, level);
        if (successful)
            ANIMATE_SOCKET_LEDS = level;

        return successful;
    }
    bool getSocketLedAnimationActive(){return ANIMATE_SOCKET_LEDS;}

    bool getLedGroupActiveLogic() { return LED_GROUP_ACTIVE_LOGIC; }
    bool getLedIndividualActiveLogic() { return LED_INDIVIDUAL_ACTIVE_LOGIC; }
    bool getSocketsActiveLogic() { return SOCKETS_ACTIVE_LOGIC; }
    bool getButtonActiveLogic() { return BUTTON_ACTIVE_LOGIC; }

    uint8_t getSavedHour()
    {
        uint8_t hour = EEPROM.read(EEPROM_HOUR_BYTE_ADDRES);
        if (hour < 1 || hour >= HOURS_IN_ONE_DAY)
        {
            saveHour(0);
            hour = 0;
        }
        return hour;
    };
    uint8_t getSavedMinute()
    {
        uint8_t minute = EEPROM.read(EEPROM_MINUTE_BYTE_ADDRES);
        if (minute < 1 || minute >= MINUTES_IN_ONE_HOUR)
        {
            saveMinute(0);
            minute = 0;
        }
        return minute;
    };
    uint8_t getSavedSecond()
    {
        uint8_t second = EEPROM.read(EEPROM_SECOND_BYTE_ADDRES);
        if (second < 1 || second >= SECONDS_IN_ONE_MINUTE)
        {
            saveSecond(0);
            second = 0;
        }
        return second;
    };

    bool saveHour(uint8_t hr)
    {
        if (hr < 0 || hr >= HOURS_IN_ONE_DAY)
            return false;

        EEPROM.write(EEPROM_HOUR_BYTE_ADDRES, hr);
        return true;
    };
    bool saveMinute(uint8_t min)
    {
        if (min < 0 || min >= MINUTES_IN_ONE_HOUR)
            return false;

        EEPROM.write(EEPROM_MINUTE_BYTE_ADDRES, min);
        return true;
    };
    bool saveSecond(uint8_t sec)
    {
        if (sec < 0 || sec >= SECONDS_IN_ONE_MINUTE)
            return false;

        EEPROM.write(EEPROM_SECOND_BYTE_ADDRES, sec);
        return true;
    };
    bool isSavedTimeValid()
    {
        uint8_t hour = EEPROM.read(EEPROM_HOUR_BYTE_ADDRES);
        uint8_t minute = EEPROM.read(EEPROM_HOUR_BYTE_ADDRES);
        uint8_t second = EEPROM.read(EEPROM_HOUR_BYTE_ADDRES);
        return hour < HOURS_IN_ONE_DAY && minute < MINUTES_IN_ONE_HOUR && second < SECONDS_IN_ONE_MINUTE;
    }

    MyByteAndBitNumber getByteAndBitNumberForSocketActivity(uint8_t hour, uint8_t minute, uint8_t socket)
    {
        bool valid = true;
        if (hour < 0 || hour >= HOURS_IN_ONE_DAY)
            valid = false;
        if (minute < 0 || minute >= MINUTES_IN_ONE_HOUR)
            valid = false;
        if (minute % FIVE_MINUTES != 0) // minutes have to be a multiple of five
            valid = false;
        if (socket < 1 || socket > SOCKETS_COUNT)
            valid = false;

        socket -= 1;

        uint16_t timeIndex = hour * MINUTES_IN_ONE_HOUR / FIVE_MINUTES + minute / FIVE_MINUTES;
        uint16_t bitIndex = timeIndex * SOCKETS_COUNT + socket;
        uint8_t byteIndex = bitIndex / BITS_IN_ONE_BYTE;

        uint8_t byteAddress = EEPROM_SOCKET_ACTIVITY_START_BYTE_ADDRES + byteIndex;
        uint8_t bitNumber = bitIndex - byteIndex * BITS_IN_ONE_BYTE;

        if (byteAddress < EEPROM_SOCKET_ACTIVITY_START_BYTE_ADDRES || byteAddress > EEPROM_SOCKET_ACTIVITY_END_BYTE_ADDRES)
            valid = false;

        return {byteAddress, bitNumber, valid};
    }
    bool getSocketActivity(uint8_t hour, uint8_t minute, uint8_t socket)
    {
        MyByteAndBitNumber byteAndBitNumber = getByteAndBitNumberForSocketActivity(hour, minute, socket);
        return readEEPROMBit(byteAndBitNumber.byteAddress, byteAndBitNumber.bitNumber);
    }
    bool setSocketActivity(uint8_t hour, uint8_t minute, uint8_t socket, bool activity)
    {
        MyByteAndBitNumber byteAndBitNumber = getByteAndBitNumberForSocketActivity(hour, minute, socket);
        return byteAndBitNumber.valid && setEEPROMBit(byteAndBitNumber.byteAddress, byteAndBitNumber.bitNumber, activity);
    }

    bool setErrorCode(MyErrorCode code)
    {
        if (code != ERRORCODE_NONE)
        {
            Serial.print(F("Error "));
            Serial.println(code);
        }
        errorCode = code;
        return true;
    }
    MyErrorCode getErrorCode() { return errorCode; }
};
#endif