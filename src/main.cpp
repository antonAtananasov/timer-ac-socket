#include <Arduino.h>

// 15 leds are multiplexed in 5 groups of 3
// LED Multiplex groups pins
#define LED_GROUP_1_PIN 2
#define LED_GROUP_2_PIN 3
#define LED_GROUP_3_PIN 4
#define LED_GROUP_4_PIN 5
#define LED_GROUP_5_PIN 6
#define LED_GROUP_6_PIN 7

#define LED_GROUP_ACTIVE_LOGIC HIGH

// LED Multiplex individual leds pins
#define LED_INDIVIDUAL_1_PIN 8
#define LED_INDIVIDUAL_2_PIN 9
#define LED_INDIVIDUAL_3_PIN 10

#define LED_INDIVIDUAL_ACTIVE_LOGIC HIGH

// LED states (12 clock leds, where the first is 1 o'clock; 5 socket leds; 1 status led)
bool ledStates[18];
// led pin combinations following state order (group, then individual)
static const int LED_PIN_PAIRS[6][3][2] = {
    {{LED_GROUP_1_PIN, LED_INDIVIDUAL_1_PIN}, {LED_GROUP_1_PIN, LED_INDIVIDUAL_2_PIN}, {LED_GROUP_1_PIN, LED_INDIVIDUAL_3_PIN}},
    {{LED_GROUP_2_PIN, LED_INDIVIDUAL_1_PIN}, {LED_GROUP_2_PIN, LED_INDIVIDUAL_2_PIN}, {LED_GROUP_2_PIN, LED_INDIVIDUAL_3_PIN}},
    {{LED_GROUP_3_PIN, LED_INDIVIDUAL_1_PIN}, {LED_GROUP_3_PIN, LED_INDIVIDUAL_2_PIN}, {LED_GROUP_3_PIN, LED_INDIVIDUAL_3_PIN}},
    {{LED_GROUP_4_PIN, LED_INDIVIDUAL_1_PIN}, {LED_GROUP_4_PIN, LED_INDIVIDUAL_2_PIN}, {LED_GROUP_4_PIN, LED_INDIVIDUAL_3_PIN}},
    {{LED_GROUP_5_PIN, LED_INDIVIDUAL_1_PIN}, {LED_GROUP_5_PIN, LED_INDIVIDUAL_2_PIN}, {LED_GROUP_5_PIN, LED_INDIVIDUAL_3_PIN}},
    {{LED_GROUP_6_PIN, LED_INDIVIDUAL_1_PIN}, {LED_GROUP_6_PIN, LED_INDIVIDUAL_2_PIN}, {LED_GROUP_6_PIN, LED_INDIVIDUAL_3_PIN}}};

static const int LED_GROUP_PINS[6] = {LED_GROUP_1_PIN, LED_GROUP_2_PIN, LED_GROUP_3_PIN, LED_GROUP_4_PIN, LED_GROUP_5_PIN, LED_GROUP_6_PIN};
static const int LED_INDIVIDUAL_PINS[3] = {LED_INDIVIDUAL_1_PIN, LED_INDIVIDUAL_2_PIN, LED_INDIVIDUAL_3_PIN};

// put function declarations here:
void updateLedMatrix();
void setClockLed(int hour, bool state);
void setSocketLed(int socket, bool state);
void setStatusLed(bool state);
bool getClockLed(int hour);
bool getSocketLed(int socket);
bool getStatusLed();

unsigned long lastLoopTime;
int ledGroupsCount;
int individualLedsCount;
void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.println("Hello World");

    ledGroupsCount = sizeof(LED_PIN_PAIRS) / sizeof(LED_PIN_PAIRS[0]);
    individualLedsCount = sizeof(LED_PIN_PAIRS[0]) / sizeof(LED_PIN_PAIRS[0][0]);
    for (int groupPin = 0; groupPin < ledGroupsCount; groupPin++)
        pinMode(LED_GROUP_PINS[groupPin], OUTPUT);
    for (int individualLed = 0; individualLed < individualLedsCount; individualLed++)
        pinMode(LED_INDIVIDUAL_PINS[individualLed], OUTPUT);

    lastLoopTime = millis();
}

unsigned long timeDelay = 1; // seconds
int selectedClockLed = 0;
int selectedSocketLed = 0;
void loop()
{
    if (millis() - lastLoopTime > 1000 * timeDelay)
    {
        lastLoopTime = millis();
        selectedClockLed = (selectedClockLed) % 12 + 1;
        selectedSocketLed = (selectedSocketLed) % 5 + 1;
        setClockLed(selectedClockLed, !getClockLed(selectedClockLed));
        setSocketLed(selectedSocketLed, !getSocketLed(selectedSocketLed));
        setStatusLed(!getStatusLed());
    }

    updateLedMatrix();
}

void updateLedMatrix()
{
    for (int activeLedGroup = 0; activeLedGroup < ledGroupsCount; activeLedGroup++)
    {
        // TODO: use direct port manipulation for faster switching
        for (int groupPin = 0; groupPin < ledGroupsCount; groupPin++)
        {
            // only enable active group
            digitalWrite(LED_GROUP_PINS[groupPin], !LED_GROUP_ACTIVE_LOGIC);
        }
        for (int individualLed = 0; individualLed < individualLedsCount; individualLed++)
        {
            // only enable active group
            digitalWrite(LED_INDIVIDUAL_PINS[individualLed], !LED_INDIVIDUAL_ACTIVE_LOGIC);
        }
        for (int individualLed = 0; individualLed < individualLedsCount; individualLed++)
        {
            int ledStateIndex = activeLedGroup * individualLedsCount + individualLed;
            bool ledState = ledStates[ledStateIndex];

            int ledGroupPin = LED_PIN_PAIRS[activeLedGroup][individualLed][0];
            int ledIndividualPin = LED_PIN_PAIRS[activeLedGroup][individualLed][1];
            // enable led with active state
            digitalWrite(ledGroupPin, LED_GROUP_ACTIVE_LOGIC);
            digitalWrite(ledIndividualPin, ledState ? LED_INDIVIDUAL_ACTIVE_LOGIC : !LED_INDIVIDUAL_ACTIVE_LOGIC);
        }
    }
}

void setClockLed(int hour, bool state)
{
    if (hour >= 1 && hour <= 12)
        ledStates[hour - 1] = state;
    else
        Serial.println("Invalid hour led");
}

void setSocketLed(int socket, bool state)
{
    if (socket >= 1 && socket <= 5)
        ledStates[12 + socket - 1] = state;
    else
        Serial.println("Invalid socket led");
}
void setStatusLed(bool state)
{
    ledStates[ledGroupsCount * individualLedsCount - 1] = state;
}
bool getClockLed(int hour)
{
    if (hour >= 1 && hour <= 12)
        return ledStates[hour - 1];
    else
        Serial.println("Invalid hour led");
}

bool getSocketLed(int socket)
{
    if (socket >= 1 && socket <= 5)
        return ledStates[12 + socket - 1];
    else
        Serial.println("Invalid socket led");
}
bool getStatusLed()
{
    return ledStates[ledGroupsCount * individualLedsCount - 1];
}
