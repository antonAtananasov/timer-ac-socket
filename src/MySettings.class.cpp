#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_HOUR_BYTE_ADDRES 0
#define EEPROM_MINUTE_BYTE_ADDRES 1
#define EEPROM_SECOND_BYTE_ADDRES 2

#define SECONDS_IN_ONE_MINUTE 60
#define MINUTES_IN_ONE_HOUR 60
#define HOURS_IN_ONE_DAY 24

class MySettings
{
private:
    bool LED_GROUP_ACTIVE_LOGIC;
    bool LED_INDIVIDUAL_ACTIVE_LOGIC;

    uint8_t hour = 0, minute = 0, second = 0;

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
        {
            Serial.print(F("Time is "));
            Serial.print(hour);
            Serial.print(' ');
            Serial.print(minute);
            Serial.println();
        }
        else
            Serial.println(F("Unable to set time"));
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

    bool setHour(uint8_t hr)
    {
        if (hr < 0 || hr > HOURS_IN_ONE_DAY)
            return false;

        EEPROM.write(EEPROM_HOUR_BYTE_ADDRES, hr);
        hour = hr;
        return true;
    };
    bool setMinute(uint8_t min)
    {
        if (min < 0 || min > MINUTES_IN_ONE_HOUR)
            return false;

        EEPROM.write(EEPROM_MINUTE_BYTE_ADDRES, min);
        minute = min;
        return true;
    };
    bool setSecond(uint8_t sec)
    {
        if (sec < 0 || sec > SECONDS_IN_ONE_MINUTE)
            return false;

        EEPROM.write(EEPROM_SECOND_BYTE_ADDRES, sec);
        second = sec;
        return true;
    };
};