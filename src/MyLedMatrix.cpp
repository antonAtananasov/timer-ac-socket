#include <Arduino.h>

class MyLedMatrix
{
private:
    uint8_t LED_GROUP_ACTIVE_LOGIC;
    uint8_t LED_INDIVIDUAL_ACTIVE_LOGIC;

    // LED states (12 clock leds, where the first is 1 o'clock; 5 socket leds; 1 status led)
    bool ledStates[18];
    uint8_t LED_GROUP_PINS[6];
    uint8_t LED_INDIVIDUAL_PINS[3];
    // led pin combinations following state order (group, then individual)
    uint8_t LED_PIN_PAIRS[6][3][2];

public:
    const int LED_GROUPS_COUNT = 6;
    const int LED_INDIVIDUALS_COUNT = 3;

    MyLedMatrix(uint8_t groupPins[6], uint8_t individualLedPins[3], uint8_t ledGroupActiveLogic, uint8_t ledIndividualActiveLogic)
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
        ledStates[LED_GROUPS_COUNT * LED_INDIVIDUALS_COUNT - 1] = state;
    }
    bool getClockLed(int hour)
    {
        if (hour >= 1 && hour <= 12)
            return ledStates[hour - 1];

        Serial.println("Invalid hour led. Returning 'false'.");
        return false;
    }

    bool getSocketLed(int socket)
    {
        if (socket >= 1 && socket <= 5)
            return ledStates[12 + socket - 1];

        Serial.println("Invalid socket led. Returning 'false'.");
        return false;
    }
    bool getStatusLed()
    {
        return ledStates[LED_GROUPS_COUNT * LED_INDIVIDUALS_COUNT - 1];
    }

    void setAllLeds(bool state)
    {
        for (uint8_t i = 0; i < 18; i++)
            ledStates[i] = state;
    }

    void testLEDs(unsigned long delayms = 50)
    {
        Serial.println("Testing LEDs");
        unsigned long startTime = millis();

        setAllLeds(false);
        for (uint8_t round = 0; round < 2; round++)
            for (uint8_t i = 1; i <= 12; i++)
            {
                setClockLed(i, true);
                while (millis() - startTime < delayms)
                {
                    updateLedMatrix();
                    delay(1);
                }
                startTime = millis();

                setClockLed(i, false);
            }
        for (uint8_t round = 0; round < 2; round++)
            for (uint8_t i = 1; i <= 5; i++)
            {
                setSocketLed(i, true);
                while (millis() - startTime < delayms)
                {
                    updateLedMatrix();
                    delay(1);
                }
                startTime = millis();

                setSocketLed(i, false);
            }
        for (uint8_t round = 0; round < 4; round++)
        {
            setStatusLed((round + 1) % 2 == 0);
            while (millis() - startTime < delayms*2)
            {
                updateLedMatrix();
                delay(1);
            }
            startTime = millis();
        }
        setAllLeds(false);

        Serial.println("Done testing LEDs");
        updateLedMatrix();
    }
};