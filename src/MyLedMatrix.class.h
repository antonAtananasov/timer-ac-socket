#ifndef MyLedMatrix_CLASS
#define MyLedMatrix_CLASS

#include <Arduino.h>
#include "MySettings.class.h"
#include "MyLedState.enum.h"
#include "UserSettings.const.h"

const uint8_t MY_LED_STATES[] = {LED_OFF, LED_ON, LED_BLINK};

class MyLedMatrix
{
private:
    // LED states (12 clock leds, where the first is 1 o'clock; 5 socket leds; 1 status led)
    MyLedState ledStates[LEDS_TOTAL];
    // led pin combinations following state order (group, then individual)
    uint8_t LED_PIN_PAIRS[LED_GROUPS_COUNT][LED_INDIVIDUALS_COUNT][2];
    uint8_t LED_GROUP_PINS[LED_GROUPS_COUNT], LED_INDIVIDUAL_PINS[LED_INDIVIDUALS_COUNT];
    unsigned long initTime;

public:
    unsigned long BLINK_DELAY_MS;
    MySettings *SETTINGS;

    MyLedMatrix(MySettings *Settings, const uint8_t layout[6][3][2], unsigned long blinkDelay)
    {
        // build logical led pin pairs
        for (uint8_t i = 0; i < LED_GROUPS_COUNT; i++)
            for (uint8_t j = 0; j < LED_INDIVIDUALS_COUNT; j++)
                for (uint8_t k = 0; k < 2; k++)
                {
                    LED_PIN_PAIRS[i][j][k] = layout[i][j][k];
                    if (k == 0)
                        LED_GROUP_PINS[i] = layout[i][j][k];
                    else
                        LED_INDIVIDUAL_PINS[j] = layout[i][j][k];
                }

        // set all led pins to output
        for (uint8_t groupPin = 0; groupPin < LED_GROUPS_COUNT; groupPin++)
            pinMode(LED_GROUP_PINS[groupPin], OUTPUT);
        for (uint8_t individualLed = 0; individualLed < LED_INDIVIDUALS_COUNT; individualLed++)
            pinMode(LED_INDIVIDUAL_PINS[individualLed], OUTPUT);

        BLINK_DELAY_MS = blinkDelay;

        // link to Settings class
        SETTINGS = Settings;

        initTime = millis();
    }

    void update()
    {
        bool ledIndividualActiveLogic = SETTINGS->getLedIndividualActiveLogic();
        bool ledGroupActiveLogic = SETTINGS->getLedGroupActiveLogic();

        for (uint8_t activeLedGroup = 0; activeLedGroup < LED_GROUPS_COUNT; activeLedGroup++)
        {
            // TODO: use direct port manipulation for faster switching
            for (uint8_t groupPin = 0; groupPin < LED_GROUPS_COUNT; groupPin++)
            {
                // only enable active group
                digitalWrite(LED_GROUP_PINS[groupPin], !ledGroupActiveLogic);
            }
            for (uint8_t individualLed = 0; individualLed < LED_INDIVIDUALS_COUNT; individualLed++)
            {
                // only enable active group
                digitalWrite(LED_INDIVIDUAL_PINS[individualLed], !ledIndividualActiveLogic);
            }
            for (uint8_t individualLed = 0; individualLed < LED_INDIVIDUALS_COUNT; individualLed++)
            {
                int ledStateIndex = activeLedGroup * LED_INDIVIDUALS_COUNT + individualLed;
                MyLedState ledState = ledStates[ledStateIndex];

                int ledGroupPin = LED_PIN_PAIRS[activeLedGroup][individualLed][0];
                int ledIndividualPin = LED_PIN_PAIRS[activeLedGroup][individualLed][1];

                // enable led group
                digitalWrite(ledGroupPin, ledGroupActiveLogic);

                // enable led with corresponding state
                if (ledState == LED_ON)
                    digitalWrite(ledIndividualPin, ledIndividualActiveLogic);
                else if (ledState == LED_BLINK)
                {
                    bool blinkState = ((millis() - initTime) / BLINK_DELAY_MS) % 2 == 0; // on for BLINK_DELAY_MS, then off for BLINK_DELAY_MS
                    digitalWrite(ledIndividualPin, blinkState ? ledIndividualActiveLogic : !ledIndividualActiveLogic);
                }
                else // if (ledState == LED_OFF) or unknown
                    digitalWrite(ledIndividualPin, !ledIndividualActiveLogic);
            }
        }
    }

    bool isStateValid(MyLedState state)
    {
        for (uint8_t i = 0; i < sizeof(MY_LED_STATES) / sizeof(MY_LED_STATES[0]); i++)
            if (MY_LED_STATES[i] == i)
                return true;
        return false;
    }

    bool setClockLed(uint8_t hour, MyLedState state)
    {
        if (hour >= 1 && hour <= CLOCK_LEDS_COUNT && isStateValid(state))
        {
            ledStates[hour - 1] = state;
            return true;
        }
        else
        {
            Serial.println(F("Invalid hour led"));
            return false;
        }
    }

    bool setSocketLed(uint8_t socket, MyLedState state)
    {
        if (socket >= 1 && socket <= SOCKET_LEDS_COUNT && isStateValid(state))
        {
            ledStates[CLOCK_LEDS_COUNT + socket - 1] = state;
            return true;
        }
        else
        return false;
    }
    bool setStatusLed(MyLedState state)
    {
        if (isStateValid(state))
        {
            ledStates[LED_GROUPS_COUNT * LED_INDIVIDUALS_COUNT - 1] = state;
            return true;
        }
        else
            return false;
    }
    MyLedState getClockLed(uint8_t hour)
    {
        if (hour >= 1 && hour <= CLOCK_LEDS_COUNT)
            return ledStates[hour - 1];

        return UNKNOWN_LED_STATE;
    }

    MyLedState getSocketLed(uint8_t socket)
    {
        if (socket >= 1 && socket <= SOCKET_LEDS_COUNT)
            return ledStates[CLOCK_LEDS_COUNT + socket - 1];

        return UNKNOWN_LED_STATE;
    }
    MyLedState getStatusLed()
    {
        return ledStates[LED_GROUPS_COUNT * LED_INDIVIDUALS_COUNT - 1];
    }

    bool setAllLeds(MyLedState state)
    {
        if (!isStateValid(state))
            return false;
        for (uint8_t i = 0; i < LED_GROUPS_COUNT * LED_INDIVIDUALS_COUNT; i++)
            ledStates[i] = state;
        return true;
    }

    bool displayTime(uint8_t hour, uint8_t minute)
    {
        if (hour >= 0 && hour <= HOURS_IN_ONE_DAY && minute >= 0 && minute <= MINUTES_IN_ONE_HOUR)
        {
            hour = hour % 12;
            if (hour == 0)
                hour = 12;

            minute = (minute % MINUTES_IN_ONE_HOUR) / FIVE_MINUTES;
            if (minute == 0)
                minute = 12;

            for (uint8_t i = 1; i <= CLOCK_LEDS_COUNT; i++)
                setClockLed(i, LED_OFF);
            setClockLed(hour, LED_ON);
            setClockLed(minute, LED_BLINK);

            return true;
        }
        else
            return false;
    }

    void testLEDs(unsigned long delayms = 50)
    {
        Serial.println(F("Testing LEDs"));
        unsigned long startTime = millis();

        setAllLeds(LED_OFF);
        for (uint8_t round = 0; round < 2; round++)
            for (uint8_t i = 1; i <= CLOCK_LEDS_COUNT; i++)
            {
                setClockLed(i, LED_ON);
                while (millis() - startTime < delayms)
                {
                    update();
                    delay(1);
                }
                startTime = millis();

                setClockLed(i, LED_OFF);
            }
        for (uint8_t round = 0; round < 2; round++)
            for (uint8_t i = 1; i <= SOCKET_LEDS_COUNT; i++)
            {
                setSocketLed(i, LED_ON);
                while (millis() - startTime < delayms)
                {
                    update();
                    delay(1);
                }
                startTime = millis();

                setSocketLed(i, LED_OFF);
            }
        for (uint8_t round = 0; round < 2 * 2; round++)
        {
            setStatusLed((round + 1) % 2 == 0 ? LED_ON : LED_OFF);
            while (millis() - startTime < delayms * 2)
            {
                update();
                delay(1);
            }
            startTime = millis();
        }
        setAllLeds(LED_OFF);

        Serial.println(F("Done testing LEDs"));
        update();
    }
};
#endif