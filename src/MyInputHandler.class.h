#ifndef MyInputHandler_CLASS
#define MyInputHandler_CLASS

#include <Arduino.h>
#include "MySettings.class.h"
#include "MyButtonAction.enum.h"

class MyInputHandler
{
private:
    unsigned long lastDebounceTime = 0;
    unsigned long lastButtonPress = 0, lastButtonRelease = 0, lastButtonActionTime = 0;
    bool buttonState, lastButtonState, longPressAvailable = true;
    MyButtonAction lastButtonAction = UNKNOWN_BUTTON_ACTION;

public:
    MySettings *SETTINGS;

    MyInputHandler(MySettings *Settings)
    {
        SETTINGS = Settings;

        buttonState = lastButtonState = !Settings->getButtonActiveLogic();

        pinMode(PUSH_BUTTON_PIN, INPUT);
    };

    MyButtonAction checkButtonAction()
    {
        MyButtonAction action = NONE;
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
                        action = SHORT_PRESS;
                    else
                        action = BUTTON_UP;
                }
            }
            else if (longPressAvailable && lastButtonRelease < lastButtonPress && millis() - lastButtonPress > BUTTON_LONG_PRESS_DELAY)
            {
                action = LONG_PRESS;
                longPressAvailable = false;
            }
        }

        lastButtonState = reading;

        if (action != UNKNOWN_BUTTON_ACTION && action != NONE)
        {
            // use this in case first detected action causes issues
            /*
            if (lastButtonActionTime == 0)
            {
                lastButtonActionTime = millis();
                return UNKNOWN_BUTTON_ACTION;
            }
            else
                lastButtonActionTime = millis();
            */

            lastButtonActionTime = millis();
        }

        return action;
    }

    unsigned long getLastButtonActionTime() { return lastButtonActionTime; }
    MyButtonAction getLastButtonAction() { return lastButtonAction; }
};

#endif