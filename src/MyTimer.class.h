#ifndef MyTimer_CLASS
#define MyTimer_CLASS

#include <Arduino.h>
#include "MySettings.class.h"
#include "MyTime.struct.h"
#include "MyConstants.const.h"
#include "UserSettings.const.h"

class MyTimer
{
private:
    uint8_t hour = 0, minute = 0, second = 0;
    uint16_t millisecond = 0;
    unsigned long startTime = 0, lastUpdate = 0;
    unsigned long SAVE_TO_EEPROM_INTERVAL_MS = TIME_SAVE_DELAY;
    unsigned long lastTimeSave = 0;
    MySettings *SETTINGS;

public:
    MyTimer(uint8_t hour, uint8_t minute, uint8_t second, MySettings *Settings)
    {
        startTime = millis();

        setHour(hour);
        setMinute(minute);
        setSecond(second);

        SETTINGS = Settings;
    };

    void update()
    {
        unsigned long currentMs = millis();
        unsigned long ms = currentMs - lastUpdate;
        uint8_t elapsedDays = ms / MS_IN_ONE_DAY;
        uint8_t elapsedHours = (ms - elapsedDays * MS_IN_ONE_DAY) / MS_IN_ONE_HOUR;
        uint8_t elapsedMinutes = (ms - elapsedDays * MS_IN_ONE_DAY - elapsedHours * MS_IN_ONE_HOUR) / MS_IN_ONE_MINUTE;
        uint8_t elapsedSeconds = (ms - elapsedDays * MS_IN_ONE_DAY - elapsedHours * MS_IN_ONE_HOUR - elapsedMinutes * MS_IN_ONE_MINUTE) / MS_IN_ONE_SECOND;
        unsigned long elapsedMs = (ms - elapsedDays * MS_IN_ONE_DAY - elapsedHours * MS_IN_ONE_HOUR - elapsedMinutes * MS_IN_ONE_MINUTE - elapsedSeconds * MS_IN_ONE_SECOND);

        elapsedMs += millisecond;

        elapsedSeconds += second + elapsedMs / MS_IN_ONE_SECOND;
        elapsedMs %= MS_IN_ONE_SECOND;

        elapsedMinutes += minute + elapsedSeconds / SECONDS_IN_ONE_MINUTE;
        elapsedSeconds %= SECONDS_IN_ONE_MINUTE;

        elapsedHours += hour + elapsedMinutes / MINUTES_IN_ONE_HOUR;
        elapsedMinutes %= MINUTES_IN_ONE_HOUR;

        elapsedHours %= HOURS_IN_ONE_DAY;

        // sets
        setHour(elapsedHours);
        setMinute(elapsedMinutes);
        setSecond(elapsedSeconds);
        setMillisecond(elapsedMs);

        // autosave time
        if (SAVE_TO_EEPROM_INTERVAL_MS > 0)
            if (millis() - lastTimeSave > SAVE_TO_EEPROM_INTERVAL_MS)
            {
                SETTINGS->saveHour(hour);
                SETTINGS->saveMinute(minute);
                SETTINGS->saveSecond(second);
                lastTimeSave = millis();
            }

        lastUpdate = currentMs;
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
    bool setMillisecond(uint16_t ms)
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