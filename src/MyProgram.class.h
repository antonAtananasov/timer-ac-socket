#ifndef MyProgram_CLASS
#define MyProgram_CLASS

#include <Arduino.h>
#include "MyInputHandler.class.h"
#include "MyTimer.class.h"
#include "MyProgramMode.enum.h"
#include "MyLedMatrix.class.h"
#include "MySettings.class.h"

class MyProgram
{
private:
    MyProgramMode currentMode = PROGRAM_DISPLAY_TIME;
    unsigned long enterModeTime = 0;
    int primaryActiveNumber = 0, secondaryActiveNumber = 0;

public:
    MyInputHandler *INPUT_HANDLER;
    MyTimer *TIMER;
    MyLedMatrix *LED_MATRIX;
    MySettings *SETTINGS;

    MyProgram(MyInputHandler *InputHandler, MySettings *Settings, MyTimer *Timer, MyLedMatrix *LedMatrix)
    {
        INPUT_HANDLER = InputHandler;
        TIMER = Timer;
        LED_MATRIX = LedMatrix;
        SETTINGS = Settings;
    };

    MyProgramMode getProgramMode() { return currentMode; }
    bool setProgramMode(MyProgramMode mode)
    {
        enterModeTime = millis();
        currentMode = mode;
        return true;
    }

    void mainLoop(MyButtonAction buttonAction, MyScrollWheelAction scrollAction)
    {

        // handle input and status led
        if (millis() - INPUT_HANDLER->getLastButtonActionTime() < 50 && (buttonAction == BUTTON_SHORT_PRESS || buttonAction == BUTTON_LONG_PRESS))
            LED_MATRIX->setStatusLed(LED_ON);
        else if (SETTINGS->getErrorCode() != ERRORCODE_NONE)
            LED_MATRIX->setStatusLed(LED_BLINK_SLOW);
        else
            LED_MATRIX->setStatusLed(LED_OFF);

        // handle modes
        switch (currentMode & 0b11111000u) // mask out submodes
        {
        case PROGRAM_DISPLAY_TIME:
            displayTime(buttonAction, scrollAction);
            break;
        case PROGRAM_SET_TIME:
            setTime(buttonAction, scrollAction);
            break;
        case PROGRAM_SET_SOCKETS:
            setSockets(buttonAction, scrollAction);
            break;

        default:
            setProgramMode(PROGRAM_DISPLAY_TIME);
            break;
        }
    }

    void displayTime(MyButtonAction buttonAction, MyScrollWheelAction scrollAction)
    {
        MyTime time = TIMER->getTime();
        LED_MATRIX->displayTime(time.hour, time.minute);

        switch (SETTINGS->getErrorCode())
        {
        case ERRORCODE_REBOOT:
            if (buttonAction == BUTTON_LONG_PRESS)
                setProgramMode(PROGRAM_SET_TIME);
            break;

        case ERRORCODE_NONE:
            if (buttonAction == BUTTON_LONG_PRESS)
                setProgramMode(PROGRAM_SET_SOCKETS);
            break;

        default:
            break;
        }
    }

    void setTime(MyButtonAction buttonAction, MyScrollWheelAction scrollAction)
    {
        // update scroll selector
        if (scrollAction == SCROLL_CLOCKWISE)
            primaryActiveNumber++;
        else if (scrollAction == SCROLL_COUNTERCLOCKWISE)
            primaryActiveNumber--;
        while (primaryActiveNumber < 0)
            primaryActiveNumber += CLOCK_LEDS_COUNT;
        primaryActiveNumber %= CLOCK_LEDS_COUNT;

        switch (currentMode)
        {
        case PROGRAM_SET_TIME:
        {
            if (SETTINGS->getErrorCode() == ERRORCODE_REBOOT)
                SETTINGS->setErrorCode(ERRORCODE_NONE);
            // flash clock leds and init variables
            if (millis() - enterModeTime < LED_BLINK_FAST_DELAY_MS)
                LED_MATRIX->setClockLeds(LED_ON);
            else
            {
                LED_MATRIX->setClockLeds(LED_OFF);

                primaryActiveNumber = SETTINGS->getSavedHour() / 12; // 0 for AM, 1 for PM
                setProgramMode(PROGRAM_SET_TIME_AMPM);
            }

            break;
        }
        case PROGRAM_SET_TIME_AMPM:
        {
            LED_MATRIX->setClockLeds(LED_OFF);
            uint8_t amOrPm = primaryActiveNumber % 2; // 0 for AM, 1 for PM
            LED_MATRIX->setClockLed(amOrPm ? 7 : 11, LED_BLINK_FAST);
            LED_MATRIX->setClockLed(amOrPm ? 6 : 12, LED_BLINK_FAST);
            LED_MATRIX->setClockLed(amOrPm ? 5 : 1, LED_BLINK_FAST);

            if (buttonAction == BUTTON_SHORT_PRESS)
            {
                secondaryActiveNumber = amOrPm;
                primaryActiveNumber = SETTINGS->getSavedHour() % CLOCK_LEDS_COUNT;
                setProgramMode(PROGRAM_SET_TIME_HOUR);
            }
            break;
        }
        case PROGRAM_SET_TIME_HOUR:
        {
            LED_MATRIX->setClockLeds(LED_OFF);
            LED_MATRIX->setClockLed(primaryActiveNumber == 0 ? CLOCK_LEDS_COUNT : primaryActiveNumber, LED_BLINK_FAST);

            if (buttonAction == BUTTON_SHORT_PRESS)
            {
                if (SETTINGS->saveHour(primaryActiveNumber + (secondaryActiveNumber & CLOCK_LEDS_COUNT)))
                {
                    primaryActiveNumber = SETTINGS->getSavedMinute() / 5;
                    setProgramMode(PROGRAM_SET_TIME_MINUTE);
                }
                else
                    SETTINGS->setErrorCode(ERRORCODE_UNKNOWN);
            }

            break;
        }
        case PROGRAM_SET_TIME_MINUTE:
        {
            LED_MATRIX->setClockLeds(LED_OFF);
            LED_MATRIX->setClockLed(primaryActiveNumber == 0 ? CLOCK_LEDS_COUNT : primaryActiveNumber, LED_BLINK_FAST);

            if (buttonAction == BUTTON_SHORT_PRESS)
            {
                if (SETTINGS->saveMinute(primaryActiveNumber * 5) && SETTINGS->saveSecond(0))
                {
                    TIMER->setSecond(SETTINGS->getSavedSecond());
                    TIMER->setMinute(SETTINGS->getSavedMinute());
                    TIMER->setHour(SETTINGS->getSavedHour());

                    setProgramMode(PROGRAM_DISPLAY_TIME);
                }
                else
                    SETTINGS->setErrorCode(ERRORCODE_UNKNOWN);
            }

            break;
        }

        default:
            SETTINGS->setErrorCode(ERRORCODE_UNKNOWN);
            setProgramMode(PROGRAM_DISPLAY_TIME);
            break;
        }
    }

    void setSockets(MyButtonAction buttonAction, MyScrollWheelAction scrollAction) { displayTime(buttonAction, scrollAction); }
};

#endif