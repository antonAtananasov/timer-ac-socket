#include <Arduino.h>

enum MyLedState : uint8_t
{
    LED_OFF = false,
    LED_ON = true,
    LED_BLINK = 2
};

class MyLedMatrix
{
private:
    uint8_t LED_GROUP_ACTIVE_LOGIC;
    uint8_t LED_INDIVIDUAL_ACTIVE_LOGIC;

    // LED states (12 clock leds, where the first is 1 o'clock; 5 socket leds; 1 status led)
    MyLedState ledStates[18];
    uint8_t LED_GROUP_PINS[6];
    uint8_t LED_INDIVIDUAL_PINS[3];
    // led pin combinations following state order (group, then individual)
    uint8_t LED_PIN_PAIRS[6][3][2];
    unsigned long initTime;

public:
    unsigned long BLINK_DELAY_MS;
    const int LED_GROUPS_COUNT = 6;
    const int LED_INDIVIDUALS_COUNT = 3;
    const int CLOCK_LEDS_COUNT = 12;
    const int SOCKET_LEDS_COUNT = 5;

    MyLedMatrix(uint8_t groupPins[6], uint8_t individualLedPins[3], uint8_t ledGroupActiveLogic, uint8_t ledIndividualActiveLogic, unsigned long blinkDelay)
    {
        for (uint8_t i = 0; i < LED_GROUPS_COUNT; i++)
            LED_GROUP_PINS[i] = groupPins[i];
        for (uint8_t i = 0; i < LED_INDIVIDUALS_COUNT; i++)
            LED_INDIVIDUAL_PINS[i] = individualLedPins[i];

        // layout may change
        uint8_t layout[LED_GROUPS_COUNT][LED_INDIVIDUALS_COUNT][2] =
            {{{LED_GROUP_PINS[0], LED_INDIVIDUAL_PINS[0]}, {LED_GROUP_PINS[0], LED_INDIVIDUAL_PINS[1]}, {LED_GROUP_PINS[0], LED_INDIVIDUAL_PINS[2]}},
             {{LED_GROUP_PINS[1], LED_INDIVIDUAL_PINS[0]}, {LED_GROUP_PINS[1], LED_INDIVIDUAL_PINS[1]}, {LED_GROUP_PINS[1], LED_INDIVIDUAL_PINS[2]}},
             {{LED_GROUP_PINS[2], LED_INDIVIDUAL_PINS[0]}, {LED_GROUP_PINS[2], LED_INDIVIDUAL_PINS[1]}, {LED_GROUP_PINS[2], LED_INDIVIDUAL_PINS[2]}},
             {{LED_GROUP_PINS[3], LED_INDIVIDUAL_PINS[0]}, {LED_GROUP_PINS[3], LED_INDIVIDUAL_PINS[1]}, {LED_GROUP_PINS[3], LED_INDIVIDUAL_PINS[2]}},
             {{LED_GROUP_PINS[4], LED_INDIVIDUAL_PINS[0]}, {LED_GROUP_PINS[4], LED_INDIVIDUAL_PINS[1]}, {LED_GROUP_PINS[4], LED_INDIVIDUAL_PINS[2]}},
             {{LED_GROUP_PINS[5], LED_INDIVIDUAL_PINS[0]}, {LED_GROUP_PINS[5], LED_INDIVIDUAL_PINS[1]}, {LED_GROUP_PINS[5], LED_INDIVIDUAL_PINS[2]}}};

        for (uint8_t i = 0; i < LED_GROUPS_COUNT; i++)
            for (uint8_t j = 0; j < LED_INDIVIDUALS_COUNT; j++)
                for (uint8_t k = 0; k < 2; k++)
                    LED_PIN_PAIRS[i][j][k] = layout[i][j][k];

        LED_INDIVIDUAL_ACTIVE_LOGIC = ledIndividualActiveLogic;
        LED_GROUP_ACTIVE_LOGIC = ledGroupActiveLogic;

        for (int groupPin = 0; groupPin < LED_GROUPS_COUNT; groupPin++)
            pinMode(LED_GROUP_PINS[groupPin], OUTPUT);
        for (int individualLed = 0; individualLed < LED_INDIVIDUALS_COUNT; individualLed++)
            pinMode(LED_INDIVIDUAL_PINS[individualLed], OUTPUT);

        BLINK_DELAY_MS = blinkDelay;

        initTime = millis();
    }

    void updateLedMatrix()
    {
        for (int activeLedGroup = 0; activeLedGroup < LED_GROUPS_COUNT; activeLedGroup++)
        {
            // TODO: use direct port manipulation for faster switching
            for (int groupPin = 0; groupPin < LED_GROUPS_COUNT; groupPin++)
            {
                // only enable active group
                digitalWrite(LED_GROUP_PINS[groupPin], !LED_GROUP_ACTIVE_LOGIC);
            }
            for (int individualLed = 0; individualLed < LED_INDIVIDUALS_COUNT; individualLed++)
            {
                // only enable active group
                digitalWrite(LED_INDIVIDUAL_PINS[individualLed], !LED_INDIVIDUAL_ACTIVE_LOGIC);
            }
            for (int individualLed = 0; individualLed < LED_INDIVIDUALS_COUNT; individualLed++)
            {
                int ledStateIndex = activeLedGroup * LED_INDIVIDUALS_COUNT + individualLed;
                MyLedState ledState = ledStates[ledStateIndex];

                int ledGroupPin = LED_PIN_PAIRS[activeLedGroup][individualLed][0];
                int ledIndividualPin = LED_PIN_PAIRS[activeLedGroup][individualLed][1];

                // enable led group
                digitalWrite(ledGroupPin, LED_GROUP_ACTIVE_LOGIC);

                // enable led with corresponding state
                if (ledState == LED_ON)
                    digitalWrite(ledIndividualPin, LED_INDIVIDUAL_ACTIVE_LOGIC);
                else if (ledState == LED_BLINK)
                {
                    bool blinkState = ((millis() - initTime) / BLINK_DELAY_MS) % 2 == 0; // on for BLINK_DELAY_MS, then off for BLINK_DELAY_MS
                    digitalWrite(ledIndividualPin, blinkState ? LED_INDIVIDUAL_ACTIVE_LOGIC : !LED_INDIVIDUAL_ACTIVE_LOGIC);
                }
                else // if (ledState == LED_OFF) or unknown
                    digitalWrite(ledIndividualPin, !LED_INDIVIDUAL_ACTIVE_LOGIC);
            }
        }
    }

    bool setClockLed(int hour, MyLedState state)
    {
        if (hour >= 1 && hour <= CLOCK_LEDS_COUNT)
        {
            ledStates[hour - 1] = state;
            return true;
        }
        else
        {
            Serial.println("Invalid hour led");
            return false;
        }
    }

    bool setSocketLed(int socket, MyLedState state)
    {
        if (socket >= 1 && socket <= SOCKET_LEDS_COUNT)
        {
            ledStates[CLOCK_LEDS_COUNT + socket - 1] = state;
            return true;
        }
        else
        {
            Serial.println("Invalid socket led");
            return false;
        }
    }
    bool setStatusLed(MyLedState state)
    {
        ledStates[LED_GROUPS_COUNT * LED_INDIVIDUALS_COUNT - 1] = state;
        return true;
    }
    bool getClockLed(int hour)
    {
        if (hour >= 1 && hour <= CLOCK_LEDS_COUNT)
            return ledStates[hour - 1];

        Serial.println("Invalid hour led. Returning 'false'.");
        return false;
    }

    bool getSocketLed(int socket)
    {
        if (socket >= 1 && socket <= SOCKET_LEDS_COUNT)
            return ledStates[CLOCK_LEDS_COUNT + socket - 1];

        Serial.println("Invalid socket led. Returning 'false'.");
        return false;
    }
    bool getStatusLed()
    {
        return ledStates[LED_GROUPS_COUNT * LED_INDIVIDUALS_COUNT - 1];
    }

    bool setAllLeds(MyLedState state)
    {
        for (uint8_t i = 0; i < LED_GROUPS_COUNT * LED_INDIVIDUALS_COUNT; i++)
            ledStates[i] = state;
        return true;
    }

    void testLEDs(unsigned long delayms = 50)
    {
        Serial.println("Testing LEDs");
        unsigned long startTime = millis();

        setAllLeds(LED_OFF);
        for (uint8_t round = 0; round < 2; round++)
            for (uint8_t i = 1; i <= CLOCK_LEDS_COUNT; i++)
            {
                setClockLed(i, LED_ON);
                while (millis() - startTime < delayms)
                {
                    updateLedMatrix();
                    delay(1);
                }
                startTime = millis();

                setClockLed(i, LED_OFF);
            }
        for (uint8_t round = 0; round < 2; round++)
            for (uint8_t i = 1; i <= SOCKET_LEDS_COUNT; i++)
            {
                setSocketLed(i, LED_ON);
                while (millis() - startTime < delayms)
                {
                    updateLedMatrix();
                    delay(1);
                }
                startTime = millis();

                setSocketLed(i, LED_OFF);
            }
        for (uint8_t round = 0; round < 2 * 2; round++)
        {
            setStatusLed((round + 1) % 2 == 0 ? LED_ON : LED_OFF);
            while (millis() - startTime < delayms * 2)
            {
                updateLedMatrix();
                delay(1);
            }
            startTime = millis();
        }
        setAllLeds(LED_OFF);

        Serial.println("Done testing LEDs");
        updateLedMatrix();
    }
};