#include <Arduino.h>
#include "MySerialHandler.class.h"
#include "MySettings.class.h"
#include "MyLedMatrix.class.h"
#include "MyTimer.class.h"
#include "MyLedState.enum.h"
#include "MySocketManager.class.h"
#include "MyInputHandler.class.h"
#include "MyProgram.class.h"
#include "UserSettings.const.h"

// IMPORTANT
// Please look at the UserSettings file and check the configuration before using this sketch

// init objects
MySettings *Settings;
MyLedMatrix *LedMatrix;
MyTimer *Timer;
MySerialHandler *SerialHandler;
MySocketManager *SocketManager;
MyInputHandler *InputHandler;
MyProgram *ProgramHandler;

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.println(F("Timed AC Plug\n"));

    Settings = new MySettings(true, true);
    LedMatrix = new MyLedMatrix(
        Settings,
        LED_PIN_PAIRS_LAYOUT,
        LED_BLINK_SLOW_DELAY_MS,
        LED_BLINK_FAST_DELAY_MS);
    Timer = new MyTimer(0, 0, 0, Settings);
    SocketManager = new MySocketManager(Settings, LedMatrix, Timer);
    InputHandler = new MyInputHandler(Settings);
    ProgramHandler = new MyProgram(InputHandler, Settings, Timer, LedMatrix, SocketManager);
    SerialHandler = new MySerialHandler(Settings, LedMatrix, SocketManager, Timer, ProgramHandler);

    LedMatrix->testLEDs();

    Timer->setSecond(Settings->getSavedSecond());
    Timer->setMinute(Settings->getSavedMinute());
    Timer->setHour(Settings->getSavedHour());

    SerialHandler->flush();
    SerialHandler->printMOTD();
}

unsigned long _lastTimeDisplay = 0;
void loop()
{
    // BEGIN "always at start of loop"
    Timer->update();
    // handle serial
    SerialHandler->handleMessage();
    // handle inputs
    MyButtonAction buttonAction = InputHandler->checkButtonAction();
    MyScrollWheelAction scrollAction = InputHandler->checkScrollAction();
    // END "always at start of loop"

    ProgramHandler->mainLoop(buttonAction, scrollAction);

    // BEGIN "always at end of loop"
    LedMatrix->update();
    // END "always at end of loop"
}
