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

//TODO: extract into MySettings class
// LED Logic level for enabling a LED group
#define DEFAULT_LED_GROUP_ACTIVE_LOGIC HIGH
// LED Logic level for enabling an individual LED in a group
#define DEFAULT_LED_INDIVIDUAL_ACTIVE_LOGIC HIGH

// init objects
MySettings Settings(DEFAULT_LED_GROUP_ACTIVE_LOGIC,DEFAULT_LED_INDIVIDUAL_ACTIVE_LOGIC);
MyLedMatrix LedMatrix(
    (uint8_t[6]){LED_GROUP_1_PIN, LED_GROUP_2_PIN, LED_GROUP_3_PIN, LED_GROUP_4_PIN, LED_GROUP_5_PIN, LED_GROUP_6_PIN},
    (uint8_t[3]){LED_INDIVIDUAL_1_PIN, LED_INDIVIDUAL_2_PIN, LED_INDIVIDUAL_3_PIN},
    &Settings,
    350);
MySerialHandler SerialHandler(&LedMatrix, &Settings);

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.println("Hello World");

    LedMatrix.testLEDs();
    LedMatrix.setStatusLed(LED_BLINK);
}



void loop()
{
    // always at start of loop
    SerialHandler.handleMessage();

    // always at end of loop
    LedMatrix.updateLedMatrix();
}
