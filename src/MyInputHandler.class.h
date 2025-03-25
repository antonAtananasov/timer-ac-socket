#ifndef MyInputHandler_CLASS
#define MyInputHandler_CLASS

#include <Arduino.h>
#include <RotaryEncoder.h>
#include "MySettings.class.h"
#include "MyButtonAction.enum.h"
#include "MyScrollWheelAction.enum.h"

class MyInputHandler
{
private:
    unsigned long lastDebounceTime = 0;
    unsigned long lastButtonPress = 0, lastButtonRelease = 0, lastButtonActionTime = 0;
    bool buttonState, lastButtonState, longPressAvailable = true;
    MyButtonAction lastButtonAction = BUTTON_UNKNOWN;
    RotaryEncoder *ROTARY_ENCODER = new RotaryEncoder(SCROLL_WHEEL_PIN1, SCROLL_WHEEL_PIN2, (RotaryEncoder::LatchMode)SCROLL_WHEEL_LATCH_MODE);

public:
    MySettings *SETTINGS;

    MyInputHandler(MySettings *Settings)
    {
        SETTINGS = Settings;

        buttonState = lastButtonState = !Settings->getButtonActiveLogic();

        pinMode(PUSH_BUTTON_PIN, INPUT);
        pinMode(SCROLL_WHEEL_PIN1, INPUT);
        pinMode(SCROLL_WHEEL_PIN2, INPUT);
    };

    MyButtonAction checkButtonAction() // should be called in main loop
    {
        MyButtonAction action = BUTTON_NONE;
        int reading = digitalRead(PUSH_BUTTON_PIN);

        if (reading != lastButtonState)
            lastDebounceTime = millis();

        if ((millis() - lastDebounceTime) > BUTTON_DEBOUNCE_DELAY)
        {
            if (reading != buttonState)
            {
                buttonState = reading;

                bool buttonActiveLogic = SETTINGS->getButtonActiveLogic();
                // on button down
                if (buttonState == buttonActiveLogic)
                {
                    lastButtonPress = millis();
                    action = BUTTON_DOWN;
                }
                // on button up
                if (buttonState == !buttonActiveLogic)
                {
                    lastButtonRelease = millis();
                    longPressAvailable = true;

                    if (lastButtonRelease - lastButtonPress < BUTTON_LONG_PRESS_DELAY)
                        action = BUTTON_SHORT_PRESS;
                    else
                        action = BUTTON_UP;
                }
            }
            else if (longPressAvailable && lastButtonRelease < lastButtonPress && millis() - lastButtonPress > BUTTON_LONG_PRESS_DELAY)
            {
                action = BUTTON_LONG_PRESS;
                longPressAvailable = false;
            }
        }

        lastButtonState = reading;

        if (action != BUTTON_UNKNOWN && action != BUTTON_NONE)
        {
            // use this in case first detected action causes issues
            /*
            if (lastButtonActionTime == 0)
            {
                lastButtonActionTime = millis();
                return BUTTON_UNKNOWN;
            }
            else
                lastButtonActionTime = millis();
            */

            lastButtonActionTime = millis();
        }

        return action;
    }

    MyScrollWheelAction checkScrollAction() // should be called in main loop
    {
        ROTARY_ENCODER->tick();
        switch (ROTARY_ENCODER->getDirection())
        {
        case RotaryEncoder::Direction::NOROTATION:
            return SCROLL_NONE;
        case RotaryEncoder::Direction::CLOCKWISE:
            return SCROLL_COUNTERCLOCKWISE;
        case RotaryEncoder::Direction::COUNTERCLOCKWISE:
            return SCROLL_CLOCKWISE;

        default:
            return SCROLL_UNKNOWN;
        }
    }

    unsigned long getLastButtonActionTime() { return lastButtonActionTime; }
    MyButtonAction getLastButtonAction() { return lastButtonAction; }
};

#endif