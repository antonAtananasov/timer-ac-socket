#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_HOUR_BYTE_ADDRES 0
#define EEPROM_MINUTE_BYTE_ADDRES 1
#define EEPROM_SECOND_BYTE_ADDRES 2

class MySettings
{
private:
    bool LED_GROUP_ACTIVE_LOGIC;
    bool LED_INDIVIDUAL_ACTIVE_LOGIC;

    uint8_t hour=0, minute=0, second=0;

public:
    MySettings(bool ledGroupActiveLogic, bool ledIndividualActiveLogic)
    {
        setLedGroupActiveLogic(ledGroupActiveLogic);
        setLedIndividualActiveLogic(ledIndividualActiveLogic);
    };

    bool restoreTime()
    {
        bool successfulTime = setHour(EEPROM.read(EEPROM_HOUR_BYTE_ADDRES)) && setMinute(EEPROM.read(EEPROM_MINUTE_BYTE_ADDRES)) && setSecond(EEPROM.read(EEPROM_SECOND_BYTE_ADDRES));
        if (successfulTime)
            Serial.println("Time is " + String(hour) + " " + String(minute));
        else
            Serial.println("Unable to set time");
        return successfulTime;
    }

    bool setLedGroupActiveLogic(bool level)
    {
        LED_GROUP_ACTIVE_LOGIC = level;
        // TODO: save to EEPROM
        return true;
    }
    bool setLedIndividualActiveLogic(bool level)
    {
        LED_INDIVIDUAL_ACTIVE_LOGIC = level;
        // TODO: save to EEPROM
        return true;
    }

    bool getLedGroupActiveLogic()
    {
        return LED_GROUP_ACTIVE_LOGIC;
    }
    bool getLedIndividualActiveLogic()
    {
        return LED_INDIVIDUAL_ACTIVE_LOGIC;
    }

    uint8_t getHour() { return hour; };
    uint8_t getMinute() { return minute; };
    uint8_t getSecond() { return second; };
    // TODO:add validation and fix eeprom reading/writing
    bool setHour(uint8_t hr)
    {
        if (hr < 0 || hr > 24)
            return false;

        EEPROM.write(EEPROM_HOUR_BYTE_ADDRES, hr);
        hour = hr;
        return true;
    };
    bool setMinute(uint8_t min)
    {
        if (min < 0 || min > 60)
            return false;

        EEPROM.write(EEPROM_MINUTE_BYTE_ADDRES, min);
        minute = min;
        return true;
    };
    bool setSecond(uint8_t sec)
    {
        if (sec < 0 || sec > 60)
            return false;

        EEPROM.write(EEPROM_SECOND_BYTE_ADDRES, sec);
        second = sec;
        return true;
    };
};