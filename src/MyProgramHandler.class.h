#ifndef MyProgramHandler_CLASS
#define MyProgramHandler_CLASS

#include <Arduino.h>
#include "MyInputHandler.class.h"
#include "MyTimer.class.h"
#include "MyProgramMode.enum.h"
#include "MyLedMatrix.class.h"
#include "MySettings.class.h"
#include "MySocketManager.class.h"

class MyProgramHandler
{
private:
    MyProgramMode currentMode = PROGRAM_DISPLAY_TIME;
    unsigned long enterModeTime = 0;
    uint8_t selectedHour, selectedMinute, selectedAmPm;
    uint8_t selectedHours[CLOCK_LEDS_COUNT], selectedMinutes[CLOCK_LEDS_COUNT];
    bool selectedSockets[SOCKETS_COUNT];
    int selectorValue = 0;

public:
    MyInputHandler *INPUT_HANDLER;
    MyTimer *TIMER;
    MyLedMatrix *LED_MATRIX;
    MySettings *SETTINGS;
    MySocketManager *SOCKET_MANAGER;

    MyProgramHandler(MyInputHandler *InputHandler, MySettings *Settings, MyTimer *Timer, MyLedMatrix *LedMatrix, MySocketManager *SocketManager)
    {
        INPUT_HANDLER = InputHandler;
        TIMER = Timer;
        LED_MATRIX = LedMatrix;
        SETTINGS = Settings;
        SOCKET_MANAGER = SocketManager;
    };

    MyProgramMode getProgramMode() { return currentMode; }
    bool setProgramMode(MyProgramMode mode)
    {
        switch (mode)
        {
        case PROGRAM_DISPLAY_TIME:
        case PROGRAM_SET_TIME:
        case PROGRAM_SET_TIME_AMPM:
        case PROGRAM_SET_TIME_HOUR:
        case PROGRAM_SET_TIME_MINUTE:
        case PROGRAM_SET_SOCKETS:
        case PROGRAM_SET_SOCKETS_AMPM:
        case PROGRAM_SET_SOCKETS_HOUR:
        case PROGRAM_SET_SOCKETS_MINUTE:
        case PROGRAM_SET_SOCKETS_STATES:
        case PROGRAM_TEST:
            enterModeTime = millis();
            currentMode = mode;

            return true;

        default:
            return false;
        }
    }

    void mainLoop(MyButtonAction buttonAction, MyScrollWheelAction scrollAction) // should be called continuously in void loop
    {

        // handle input and status led
        if (millis() - INPUT_HANDLER->getLastButtonActionTime() < INPUT_INDICATION_DELAY && (buttonAction == BUTTON_SHORT_PRESS || buttonAction == BUTTON_LONG_PRESS))
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
        case PROGRAM_TEST:
            /* leave all control to the serial mon */
            break;

        default:
            returnToMainMode(true);
            break;
        }

        if (currentMode != PROGRAM_TEST) // updates are blocked only in test mode
            SOCKET_MANAGER->update();
    }

    void displayTime(MyButtonAction buttonAction, MyScrollWheelAction scrollAction)
    {
        MyTime time = TIMER->getTime();
        LED_MATRIX->displayTime(time.hour, time.minute);

        switch (SETTINGS->getErrorCode())
        {
        case ERRORCODE_REBOOT:
            if (buttonAction == BUTTON_LONG_PRESS)
            {
                SETTINGS->setErrorCode(ERRORCODE_NONE);
                setProgramMode(PROGRAM_SET_TIME);
            }
            break;

        case ERRORCODE_NONE:
            if (buttonAction == BUTTON_LONG_PRESS)
                setProgramMode(PROGRAM_SET_SOCKETS);
            break;

        default:
            break;
        }
    }
    void selectTime(MyButtonAction buttonAction, MyScrollWheelAction scrollAction, MyProgramMode selectionMode)
    {
        // update scroll selector
        if (scrollAction == SCROLL_CLOCKWISE)
            selectorValue++;
        else if (scrollAction == SCROLL_COUNTERCLOCKWISE)
            selectorValue--;
        while (selectorValue < 0)
            selectorValue += CLOCK_LEDS_COUNT;
        selectorValue %= CLOCK_LEDS_COUNT;

        switch (currentMode)
        {
        case PROGRAM_SET_TIME:
        case PROGRAM_SET_SOCKETS:
        {
            // flash clock leds and init variables
            if (millis() - enterModeTime < LED_BLINK_FAST_DELAY_MS)
                LED_MATRIX->setClockLeds(LED_ON);
            else
            {
                LED_MATRIX->setClockLeds(LED_OFF);

                selectorValue = selectedAmPm = SETTINGS->getSavedHour() / CLOCK_LEDS_COUNT; // 0 for AM, 1 for PM
                if (selectionMode == PROGRAM_SET_TIME)
                    setProgramMode(PROGRAM_SET_TIME_AMPM);
                if (selectionMode == PROGRAM_SET_SOCKETS)
                    setProgramMode(PROGRAM_SET_SOCKETS_AMPM);
            }

            break;
        }
        case PROGRAM_SET_TIME_AMPM:
        case PROGRAM_SET_SOCKETS_AMPM:
        {
            LED_MATRIX->setClockLeds(LED_OFF);
            uint8_t amOrPm = selectorValue % 2; // 0 for AM, 1 for PM
            LED_MATRIX->setClockLed(amOrPm ? 7 : 11, LED_BLINK_FAST);
            LED_MATRIX->setClockLed(amOrPm ? 6 : 12, LED_BLINK_FAST);
            LED_MATRIX->setClockLed(amOrPm ? 5 : 1, LED_BLINK_FAST);

            selectedAmPm = amOrPm;

            if (buttonAction == BUTTON_SHORT_PRESS)
            {
                selectorValue = selectedHour = SETTINGS->getSavedHour() % CLOCK_LEDS_COUNT;
                if (selectionMode == PROGRAM_SET_TIME)
                    setProgramMode(PROGRAM_SET_TIME_HOUR);
                if (selectionMode == PROGRAM_SET_SOCKETS)
                {
                    for (uint8_t i = 0; i < CLOCK_LEDS_COUNT; i++)
                    {
                        selectedHours[i] = false;
                        selectedMinutes[i] = false;
                    }
                    for (uint8_t i = 0; i < SOCKETS_COUNT; i++)
                        selectedSockets[i] = false;

                    setProgramMode(PROGRAM_SET_SOCKETS_HOUR);
                }
            }
            else if (buttonAction == BUTTON_LONG_PRESS)
                returnToMainMode();

            break;
        }
        case PROGRAM_SET_TIME_HOUR:
        case PROGRAM_SET_SOCKETS_HOUR:
        {
            LED_MATRIX->setClockLeds(LED_OFF);
            LED_MATRIX->setClockLed(selectorValue == 0 ? CLOCK_LEDS_COUNT : selectorValue, LED_BLINK_FAST);

            selectedHour = selectorValue + (selectedAmPm ? CLOCK_LEDS_COUNT : 0);
            if (buttonAction == BUTTON_SHORT_PRESS)
            {
                if (selectionMode == PROGRAM_SET_TIME)
                {
                    if (SETTINGS->saveHour(selectedHour))
                    {
                        TIMER->setHour(SETTINGS->getSavedHour());
                        selectorValue = selectedMinute = SETTINGS->getSavedMinute() / FIVE_MINUTES;
                        setProgramMode(PROGRAM_SET_TIME_MINUTE);
                    }
                    else
                    {
                        SETTINGS->setErrorCode(ERRORCODE_UNKNOWN);
                        setProgramMode(PROGRAM_SET_SOCKETS_MINUTE);
                    }
                }
                else if (selectionMode == PROGRAM_SET_SOCKETS)
                {
                    selectedHours[selectorValue] = !selectedHours[selectorValue];
                    selectorValue++;
                }
            }
            else if (buttonAction == BUTTON_LONG_PRESS)
            {
                if (selectionMode == PROGRAM_SET_TIME)
                    returnToMainMode();
                else if (selectionMode == PROGRAM_SET_SOCKETS)
                {
                    selectorValue = 0;
                    setProgramMode(PROGRAM_SET_SOCKETS_MINUTE);
                }
            }
            break;
        }
        case PROGRAM_SET_TIME_MINUTE:
        case PROGRAM_SET_SOCKETS_MINUTE:
        {
            LED_MATRIX->setClockLeds(LED_OFF);
            LED_MATRIX->setClockLed(selectorValue == 0 ? CLOCK_LEDS_COUNT : selectorValue, LED_BLINK_FAST);

            selectedMinute = selectorValue * 5;

            if (buttonAction == BUTTON_SHORT_PRESS)
            {
                if (selectionMode == PROGRAM_SET_TIME)
                {
                    if (SETTINGS->saveMinute(selectedMinute) && SETTINGS->saveSecond(0))
                    {
                        TIMER->setSecond(0);
                        TIMER->setMinute(SETTINGS->getSavedMinute());
                        returnToMainMode();
                    }
                    else
                        SETTINGS->setErrorCode(ERRORCODE_UNKNOWN);
                }
                else if (selectionMode == PROGRAM_SET_SOCKETS)
                {
                    selectedMinutes[selectorValue] = !selectedMinutes[selectorValue];
                    selectorValue++;
                }
            }
            else if (buttonAction == BUTTON_LONG_PRESS)
            {
                if (selectionMode == PROGRAM_SET_TIME)
                    returnToMainMode();
                else if (selectionMode == PROGRAM_SET_SOCKETS)
                {
                    selectorValue = 0;
                    setProgramMode(PROGRAM_SET_SOCKETS_STATES);
                }
            }

            break;
        }

        default:
            returnToMainMode(true);
            break;
        }
    }

    void setTime(MyButtonAction buttonAction, MyScrollWheelAction scrollAction)
    {
        switch (currentMode)
        {
        case PROGRAM_SET_TIME:
        case PROGRAM_SET_TIME_AMPM:
        case PROGRAM_SET_TIME_HOUR:
        case PROGRAM_SET_TIME_MINUTE:
            selectTime(buttonAction, scrollAction, PROGRAM_SET_TIME);
            break;

        default:
            returnToMainMode(true);
            break;
        }
    }

    void setSockets(MyButtonAction buttonAction, MyScrollWheelAction scrollAction)
    {
        switch (currentMode)
        {
        case PROGRAM_SET_SOCKETS:
        case PROGRAM_SET_SOCKETS_HOUR:
        case PROGRAM_SET_SOCKETS_MINUTE:
            selectTime(buttonAction, scrollAction, PROGRAM_SET_SOCKETS);
            for (uint8_t i = 0; i < CLOCK_LEDS_COUNT; i++)
            {
                uint8_t ledNumber = i == 0 ? CLOCK_LEDS_COUNT : i;
                LED_MATRIX->setClockLed(ledNumber, LED_OFF);
                if (selectedHours[i])
                    LED_MATRIX->setClockLed(ledNumber, LED_ON);
                if (selectedMinutes[i])
                    LED_MATRIX->setClockLed(ledNumber, LED_BLINK_SLOW);
                if (i == selectorValue && (currentMode == PROGRAM_SET_SOCKETS_MINUTE || currentMode == PROGRAM_SET_SOCKETS_HOUR))
                    LED_MATRIX->setClockLed(ledNumber, LED_BLINK_FAST);
            }
            break;
        case PROGRAM_SET_SOCKETS_AMPM:
            selectTime(buttonAction, scrollAction, PROGRAM_SET_SOCKETS);

            break;
        case PROGRAM_SET_SOCKETS_STATES:
            // update scroll selector
            if (scrollAction == SCROLL_CLOCKWISE)
                selectorValue++;
            else if (scrollAction == SCROLL_COUNTERCLOCKWISE)
                selectorValue--;
            while (selectorValue < 0)
                selectorValue += SOCKETS_COUNT;
            selectorValue %= SOCKETS_COUNT;

            if (millis() - enterModeTime < INPUT_INDICATION_DELAY)
            {
                for (uint8_t i = 1; i <= SOCKETS_COUNT; i++)
                    LED_MATRIX->setSocketLed(i, LED_ON);
                break;
            }

            for (uint8_t i = 0; i < SOCKETS_COUNT; i++)
            {
                uint8_t socketLedNumber = i + 1;
                LED_MATRIX->setSocketLed(socketLedNumber, selectedSockets[i] ? LED_ON : LED_OFF);
                if (i == selectorValue)
                    LED_MATRIX->setSocketLed(socketLedNumber, LED_BLINK_FAST);
            }
            if (buttonAction == BUTTON_SHORT_PRESS)
            {
                selectedSockets[selectorValue] = !selectedSockets[selectorValue];
                selectorValue++;
            }
            else if (buttonAction == BUTTON_LONG_PRESS)
            {
                for (uint8_t selectedHourIndex = 0; selectedHourIndex < CLOCK_LEDS_COUNT; selectedHourIndex++)
                {
                    if (!selectedHours[selectedHourIndex])
                        continue;

                    for (uint8_t selectedMinuteIndex = 0; selectedMinuteIndex < CLOCK_LEDS_COUNT; selectedMinuteIndex++)
                    {
                        if (!selectedMinutes[selectedMinuteIndex])
                            continue;

                        for (uint8_t selectedSocketIndex = 0; selectedSocketIndex < SOCKETS_COUNT; selectedSocketIndex++)
                            SETTINGS->setSocketActivity(selectedHourIndex + (selectedAmPm ? CLOCK_LEDS_COUNT : 0), selectedMinuteIndex * FIVE_MINUTES, selectedSocketIndex + 1, selectedSockets[selectedSocketIndex]);
                    }
                }
                returnToMainMode();
            }
            break;

        default:
            returnToMainMode(true);
            break;
        }
    }

    void returnToMainMode(bool error = false)
    {
        setProgramMode(PROGRAM_DISPLAY_TIME);
        if (error)
            SETTINGS->setErrorCode(ERRORCODE_UNKNOWN);
    }
};

#endif