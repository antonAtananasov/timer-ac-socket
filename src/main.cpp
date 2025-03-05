#include <Arduino.h>

// 15 leds are multiplexed in 5 groups of 3
// LED Multiplex groups pins
#define LED_GROUP_1_PIN 1
#define LED_GROUP_2_PIN 2
#define LED_GROUP_3_PIN 3
#define LED_GROUP_4_PIN 4
#define LED_GROUP_5_PIN 5

#define LED_GROUP_ACTIVE_LOGIC HIGH

// LED Multiplex individual leds pins
#define LED_INDIVIDUAL_1_PIN 6
#define LED_INDIVIDUAL_2_PIN 7
#define LED_INDIVIDUAL_3_PIN 8

#define LED_INDIVIDUAL_ACTIVE_LOGIC HIGH

// LED states (12 clock leds, where the first is 1 o'clock; 5 socket leds; 1 status led)
bool ledStates[15];
// led pin combinations following state order (group, then individual)
static const int LED_PIN_PAIRS[5][3][2] = {
    {{LED_GROUP_1_PIN, LED_INDIVIDUAL_1_PIN}, {LED_GROUP_1_PIN, LED_INDIVIDUAL_2_PIN}, {LED_GROUP_1_PIN, LED_INDIVIDUAL_3_PIN}},
    {{LED_GROUP_2_PIN, LED_INDIVIDUAL_1_PIN}, {LED_GROUP_2_PIN, LED_INDIVIDUAL_2_PIN}, {LED_GROUP_2_PIN, LED_INDIVIDUAL_3_PIN}},
    {{LED_GROUP_3_PIN, LED_INDIVIDUAL_1_PIN}, {LED_GROUP_3_PIN, LED_INDIVIDUAL_2_PIN}, {LED_GROUP_3_PIN, LED_INDIVIDUAL_3_PIN}},
    {{LED_GROUP_4_PIN, LED_INDIVIDUAL_1_PIN}, {LED_GROUP_4_PIN, LED_INDIVIDUAL_2_PIN}, {LED_GROUP_4_PIN, LED_INDIVIDUAL_3_PIN}},
    {{LED_GROUP_5_PIN, LED_INDIVIDUAL_1_PIN}, {LED_GROUP_5_PIN, LED_INDIVIDUAL_2_PIN}, {LED_GROUP_5_PIN, LED_INDIVIDUAL_3_PIN}}};
static const int LED_GROUP_PINS[] = {
    LED_GROUP_1_PIN,
    LED_GROUP_2_PIN,
    LED_GROUP_3_PIN,
    LED_GROUP_4_PIN,
    LED_GROUP_5_PIN};

// put function declarations here:
int myFunction(int, int);

void setup()
{
    // put your setup code here, to run once:
    int result = myFunction(2, 3);
}

void loop()
{
    int ledGroupsCount = sizeof(LED_PIN_PAIRS) / sizeof(LED_PIN_PAIRS[0]);
    int individualLedsCount = sizeof(LED_PIN_PAIRS[0]) / sizeof(LED_PIN_PAIRS[0][0]);
    for (int activeLedGroup = 0; activeLedGroup < ledGroupsCount; activeLedGroup++)
    {
        for (int individualLed = 0; individualLed < individualLedsCount; individualLed++)
        {
            int ledGroupPin = LED_PIN_PAIRS[activeLedGroup][individualLed][0];
            int ledIndividualPin = LED_PIN_PAIRS[activeLedGroup][individualLed][1];

            int ledStateIndex = activeLedGroup * individualLedsCount + individualLed;
            bool ledState = ledStates[ledStateIndex];

            // TODO: use direct port manipulation for faster switching
            for (int groupPin = 0; groupPin < ledGroupsCount; groupPin++)
            {
                // only enable active group
                digitalWrite(ledGroupPin, groupPin == activeLedGroup ? LED_GROUP_ACTIVE_LOGIC : !LED_GROUP_ACTIVE_LOGIC);
            }
            // enable led with active state
            digitalWrite(ledIndividualPin, ledState ? LED_INDIVIDUAL_ACTIVE_LOGIC : !LED_INDIVIDUAL_ACTIVE_LOGIC);
        }
    }
}

// put function definitions here:
int myFunction(int x, int y)
{
    return x + y;
}