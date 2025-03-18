#include <Arduino.h>
#include "MySerialHandler.class.h"
#include "MySettings.class.h"
#include "MyLedMatrix.class.h"
#include "MyTimer.class.h"
#include "MyLedState.enum.h"
#include "MySocketManager.class.h"
#include "UserSettings.const.h"

// IMPORTANT
// Please look at the UserSettings file and check the configuration before using this sketch

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.println(F("Timed AC Plug\n"));
}

// init objects
MySettings Settings(true, true);
MyLedMatrix LedMatrix(
    &Settings, 
    LED_PIN_PAIRS_LAYOUT,
    LED_BLINK_DELAY_MS);
MyTimer Timer(0, 0, 0, &Settings);
MySerialHandler SerialHandler(&LedMatrix, &Settings, &Timer);
MySocketManager SocketManager(&Settings, &LedMatrix, &Timer);


bool _postSetup = false;
void postSetup() // this function exists because setup() needs to begin Serial before main objects are instantiated, but they also have some setup to do before loop()
{
    if (_postSetup)
        return;

    // Settings should load logic levels before led matrix is started
    Settings.loadTimeFromEEPROM();

    LedMatrix.testLEDs();
    LedMatrix.setStatusLed(LED_BLINK);

    Timer.setSecond(Settings.getSavedSecond());
    Timer.setMinute(Settings.getSavedMinute());
    Timer.setHour(Settings.getSavedHour());

    SerialHandler.flush();
    SerialHandler.motd();

    _postSetup = true;
}

unsigned long _lastTimeDisplay = 0;
void loop()
{
    // always at start of loop
    postSetup();
    SerialHandler.handleMessage();

    //update time
    Timer.update();
    MyTime time = Timer.getTime();

    //update sockets
    SocketManager.update();

    // autoshow time
    LedMatrix.displayTime(time.hour, time.minute);

    // always at end of loop
    LedMatrix.update();
}
