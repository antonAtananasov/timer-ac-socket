#include <Arduino.h>
#include "./MySerialHandler.class.cpp"

// 18 leds are multiplexed in 6 groups of 3
// LED Multiplex groups pins
#define LED_GROUP_1_PIN 2
#define LED_GROUP_2_PIN 3
#define LED_GROUP_3_PIN 4
#define LED_GROUP_4_PIN 5
#define LED_GROUP_5_PIN 6
#define LED_GROUP_6_PIN 7

// LED Multiplex individual leds pins
#define LED_INDIVIDUAL_1_PIN 8
#define LED_INDIVIDUAL_2_PIN 9
#define LED_INDIVIDUAL_3_PIN 10

// LED Logic level for enabling a LED group
#define DEFAULT_LED_GROUP_ACTIVE_LOGIC HIGH
// LED Logic level for enabling an individual LED in a group
#define DEFAULT_LED_INDIVIDUAL_ACTIVE_LOGIC HIGH

#define MS_IN_ONE_SECOND 1000ULL
#define SECONDS_IN_ONE_MINUTE 60
#define MINUTES_IN_ONE_HOUR 60
#define HOURS_IN_ONE_DAY 24
#define MS_IN_ONE_MINUTE MS_IN_ONE_SECOND *SECONDS_IN_ONE_MINUTE
#define MS_IN_ONE_HOUR MS_IN_ONE_MINUTE *MINUTES_IN_ONE_HOUR

#define TIME_SAVE_DELLAY MS_IN_ONE_MINUTE * 5
#define TIME_SHOW_DELLAY MS_IN_ONE_SECOND

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.println(F("Timed AC Plug\n"));
}

// init objects
MySettings Settings(DEFAULT_LED_GROUP_ACTIVE_LOGIC, DEFAULT_LED_INDIVIDUAL_ACTIVE_LOGIC);
MyLedMatrix LedMatrix(
    (uint8_t[6]){LED_GROUP_1_PIN, LED_GROUP_2_PIN, LED_GROUP_3_PIN, LED_GROUP_4_PIN, LED_GROUP_5_PIN, LED_GROUP_6_PIN},
    (uint8_t[3]){LED_INDIVIDUAL_1_PIN, LED_INDIVIDUAL_2_PIN, LED_INDIVIDUAL_3_PIN},
    &Settings,
    350);
MyTimer Timer(0, 0, 0);
MySerialHandler SerialHandler(&LedMatrix, &Settings, &Timer);

bool _postSetup = false;
void postSetup()
{
    if (_postSetup)
        return;

    LedMatrix.testLEDs();
    LedMatrix.setStatusLed(LED_BLINK);

    Settings.restoreTime();
    Timer.setSecond(Settings.getSecond());
    Timer.setMinute(Settings.getMinute());
    Timer.setHour(Settings.getHour());

    SerialHandler.flush();
    SerialHandler.motd();

    _postSetup = true;
}

unsigned long _lastTimeSave = 0;
unsigned long _lastTimeDisplay = 0;
void loop()
{
    postSetup();
    // always at start of loop
    SerialHandler.handleMessage();

    Timer.update();
    MyTime time = Timer.getTime();

    // autosave time
    if (millis() - _lastTimeSave > TIME_SAVE_DELLAY)
    {
        Settings.setHour(time.hour);
        Settings.setMinute(time.minute);
        Settings.setSecond(time.second);
        Serial.print(F("Saving time: "));
        SerialHandler.printTime();
        _lastTimeSave = millis();
    }

    // autoshow time
    if (millis() - _lastTimeDisplay > TIME_SHOW_DELLAY)
    {
        LedMatrix.displayTime(time.hour, time.minute);
        _lastTimeDisplay = millis();
    }

    // always at end of loop
    LedMatrix.update();
}
