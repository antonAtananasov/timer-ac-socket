#include <Arduino.h>
#include "./MyLedState.enum.h"
#include "./MyLedMatrix.class.cpp"

struct LedAndState
{
    uint8_t led;
    MyLedState state;
};

class MySerialHandler
{
private:
    const char TERMINATOR = (char)13;
    const char SEPARATOR = ' ';
    const uint8_t MAX_MSG_LEN = 20;

    String message = "";

public:
    bool ECHO = true; // echo input back through serial
    MyLedMatrix *LED_MATRIX;

    MySerialHandler(MyLedMatrix *LedMatrix, bool echo = true)
    {
        LED_MATRIX = LedMatrix;
        ECHO = echo;
    };

    void handleMessage()
    {
        while (Serial.available())
        {
            if (message.length() == MAX_MSG_LEN)
            {
                message = "";
                Serial.println("Message too long. Resetting command.");
            }
            char inChar = (char)Serial.read();
            if (inChar != TERMINATOR)
            {
                message += inChar;
                if (ECHO)
                    Serial.print(inChar);
            }
            else
            {
                Serial.println();
                message.trim();
                handleMessage(message);
                message = "";
            }
        }
    }

    void handleMessage(String msg)
    {
        bool successful = false;

        // CLOCKLED <HOUR> <STATE>
        // SOCKETLED <SOCKET> <STATE>
        if (msg.startsWith("CLOCKLED") || msg.startsWith("SOCKETLED"))
        {
            LedAndState values = readLedAndState(msg);

            if (msg.startsWith("CLOCKLED"))
                successful = LED_MATRIX->setClockLed(values.led, values.state);
            else if (msg.startsWith("SOCKETLED"))
                successful = LED_MATRIX->setSocketLed(values.led, values.state);
        }
        // STATUSLED <STATE>
        // ALLLEDS <STATE>
        else if (msg.startsWith("STATUSLED"))
            successful = LED_MATRIX->setStatusLed(readLedState(msg));
        else if (msg.startsWith("ALLLEDS"))
            successful = LED_MATRIX->setAllLeds(readLedState(msg));

        // always at end
        Serial.println(successful ? "OK." : "Err.:" + msg);
    }

    MyLedState readLedState(String msg)
    {
        int separatorIndex = msg.indexOf(SEPARATOR);
        return (MyLedState)msg.substring(separatorIndex).toInt();
    }

    LedAndState readLedAndState(String msg)
    {
        String cmd = msg.substring(msg.indexOf(SEPARATOR));
        int separatorIndex = cmd.indexOf(SEPARATOR);
        uint8_t led = cmd.substring(0, separatorIndex - 1).toInt();
        MyLedState state = readLedState(cmd.substring(1));
        return {led, state};
    }
};