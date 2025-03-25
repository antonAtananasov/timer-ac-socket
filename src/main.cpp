#include <Arduino.h>
#include "MySerialHandler.class.h"
#include "MySettings.class.h"
#include "MyLedMatrix.class.h"
#include "MyTimer.class.h"
#include "MyLedState.enum.h"
#include "MySocketManager.class.h"
#include "MyInputHandler.class.h"
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

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.println(F("Timed AC Plug\n"));

    Settings = new MySettings(true, true);
    LedMatrix = new MyLedMatrix(
        Settings,
        LED_PIN_PAIRS_LAYOUT,
        LED_BLINK_DELAY_MS);
    Timer = new MyTimer(0, 0, 0, Settings);
    SerialHandler = new MySerialHandler(LedMatrix, Settings, Timer);
    SocketManager = new MySocketManager(Settings, LedMatrix, Timer);
    InputHandler = new MyInputHandler(Settings);

    // Settings should load logic levels before led matrix is started
    Settings->loadTimeFromEEPROM();

    LedMatrix->testLEDs();
    // start blinking to indicate running behind time by default
    LedMatrix->setStatusLed(LED_BLINK);

    Timer->setSecond(Settings->getSavedSecond());
    Timer->setMinute(Settings->getSavedMinute());
    Timer->setHour(Settings->getSavedHour());

    SerialHandler->flush();
    SerialHandler->printMOTD();
}


unsigned long _lastTimeDisplay = 0;
void loop()
{
    // always at start of loop
    SerialHandler->handleMessage();

    // update time
    Timer->update();
    MyTime time = Timer->getTime();

    // autoshow time
    LedMatrix->displayTime(time.hour, time.minute);

    // handle inputs
    // MyButtonAction buttonAction = InputHandler->checkButtonAction();
    // MyScrollWheelAction scrollAction = InputHandler->checkScrollAction();

    // update sockets
    SocketManager->update();

    // always at end of loop
    LedMatrix->update();
}
