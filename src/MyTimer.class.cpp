#include <Arduino.h>

#define MS_IN_ONE_SECOND 1000ULL
#define SECONDS_IN_ONE_MINUTE 60
#define MINUTES_IN_ONE_HOUR 60
#define HOURS_IN_ONE_DAY 24
#define MS_IN_ONE_MINUTE MS_IN_ONE_SECOND *SECONDS_IN_ONE_MINUTE
#define MS_IN_ONE_HOUR MS_IN_ONE_MINUTE *MINUTES_IN_ONE_HOUR

struct MyTime
{
    uint8_t hour, minute, second;
    uint16_t millisecond;
};

class MyTimer
{
private:
    uint8_t hour = 0, minute = 0, second = 0;
    uint16_t millisecond = 0;
    unsigned long startTime;

public:
    MyTimer(uint8_t hour, uint8_t minute, uint8_t second)
    {
        startTime = millis();

        setHour(hour);
        setMinute(minute);
        setSecond(second);
    };

    void update()
    {
        unsigned long ms = millis()-startTime;
        unsigned long sec =  ms / MS_IN_ONE_SECOND;
        unsigned long min = sec / SECONDS_IN_ONE_MINUTE;
        unsigned long hr =  min / MINUTES_IN_ONE_HOUR;

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