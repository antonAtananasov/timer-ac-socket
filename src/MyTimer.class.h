#ifndef MyTimer_CLASS
#define MyTimer_CLASS

#include <Arduino.h>
#include "MySettings.class.h"
#include "MyTime.struct.h"

// USER SETTINGS:
// Time save interval
#define TIME_SAVE_DELAY MS_IN_ONE_MINUTE * 5

// Descriptive literal constants
#define MS_IN_ONE_SECOND 1000ULL
#define SECONDS_IN_ONE_MINUTE 60
#define MINUTES_IN_ONE_HOUR 60
#define HOURS_IN_ONE_DAY 24
#define MS_IN_ONE_MINUTE MS_IN_ONE_SECOND *SECONDS_IN_ONE_MINUTE
#define MS_IN_ONE_HOUR MS_IN_ONE_MINUTE *MINUTES_IN_ONE_HOUR



class MyTimer
{
private:
    uint8_t hour = 0, minute = 0, second = 0;
    uint16_t millisecond = 0;
    unsigned long startTime;
    unsigned long SAVE_TO_EEPROM_INTERVAL_MS = TIME_SAVE_DELAY;
    unsigned long lastTimeSave = 0;
    MySettings *SETTINGS;

public:
    MyTimer(uint8_t hour, uint8_t minute, uint8_t second, MySettings *Settings)
    {
        // saveToEEPROMIntervalMs = 0 means do not save
        startTime = millis();

        setHour(hour);
        setMinute(minute);
        setSecond(second);

        SETTINGS = Settings;
    };

    void update()
    {
        unsigned long ms = millis() - startTime;
        unsigned long sec = ms / MS_IN_ONE_SECOND;
        unsigned long min = sec / SECONDS_IN_ONE_MINUTE;
        unsigned long hr = min / MINUTES_IN_ONE_HOUR;

        // loopbacks
        ms %= MS_IN_ONE_SECOND;
        sec %= SECONDS_IN_ONE_MINUTE;
        min %= MINUTES_IN_ONE_HOUR;
        hr %= HOURS_IN_ONE_DAY;

        // sets
        setHour(hr);
        setMinute(min);
        setSecond(sec);
        setMillisecond(ms);

        // autosave time
        if (SAVE_TO_EEPROM_INTERVAL_MS > 0)
            if (millis() - lastTimeSave > SAVE_TO_EEPROM_INTERVAL_MS)
            {
                SETTINGS->setHour(hour);
                SETTINGS->setMinute(minute);
                SETTINGS->setSecond(second);
                lastTimeSave = millis();
            }
    }

    bool setHour(uint8_t hr)
    {
        if (hr < 0 || hr > HOURS_IN_ONE_DAY)
            return false;

        hour = hr;
        return true;
    }
    bool setMinute(uint8_t min)
    {
        if (min < 0 || min > MINUTES_IN_ONE_HOUR)
            return false;

        minute = min;
        return true;
    }
    bool setSecond(uint8_t sec)
    {
        if (sec < 0 || sec > SECONDS_IN_ONE_MINUTE)
            return false;

        second = sec;
        return true;
    }
    bool setMillisecond(uint8_t ms)
    {
        if (ms < 0 || ms > MS_IN_ONE_SECOND)
            return false;

        millisecond = ms;
        return true;
    }

    MyTime getTime()
    {
        return {hour, minute, second, millisecond};
    }
};

#endif