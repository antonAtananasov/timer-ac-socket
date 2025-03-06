#include <Arduino.h>
#include "./MyLedState.enum.h"
#include "./MyLedMatrix.class.cpp"
#include "./MySettings.class.h"

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
    MySettings *SETTINGS;

    MySerialHandler(MyLedMatrix *LedMatrix, MySettings *settings, bool echo = true)
    {
                LED_MATRIX = LedMatrix;
        SETTINGS = settings;
        ECHO = echo;
    };

    void motd(){
        Serial.println("Type \"HELP\" for info.");
    }

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
        if (msg.startsWith("HELP"))
        {
            help();
            successful = true;
        }
        if (msg.startsWith("CLOCKLED"))
        {
            LedAndState values = readLedAndState(msg);
            successful = LED_MATRIX->setClockLed(values.led, values.state);
        }
        else if (msg.startsWith("SOCKETLED"))
        {
            LedAndState values = readLedAndState(msg);
            successful = LED_MATRIX->setSocketLed(values.led, values.state);
        }
        // STATUSLED <STATE>
        // ALLLEDS <STATE>
        else if (msg.startsWith("STATUSLED"))
            successful = LED_MATRIX->setStatusLed((MyLedState)readCmdNumber(msg));
        else if (msg.startsWith("ALLLEDS"))
            successful = LED_MATRIX->setAllLeds((MyLedState)readCmdNumber(msg));
        // LEDGRPLOGIC <0/1>
        // LEDINDLOGIC <0/1>
        else if (msg.startsWith("LEDGRPLOGIC"))
            successful = SETTINGS->setLedGroupActiveLogic((bool)readCmdNumber(msg));
        else if (msg.startsWith("LEDINDLOGIC"))
            successful = SETTINGS->setLedIndividualActiveLogic((bool)readCmdNumber(msg));
        // SETHOUR <hour 0-24>
        // SETMIN <minute 0-60>
        // SETSEC <second 0-60>
        else if (msg.startsWith("SETHOUR"))
            successful = SETTINGS->setHour(readCmdNumber(msg));
        else if (msg.startsWith("SETMIN"))
            successful = SETTINGS->setMinute(readCmdNumber(msg));
        else if (msg.startsWith("SETSEC"))
            successful = SETTINGS->setSecond(readCmdNumber(msg));

        // always at end
        Serial.println(successful ? "OK." : "Err.:" + msg);
    }

    int readCmdNumber(String msg)
    {
        int separatorIndex = msg.indexOf(SEPARATOR);
        return msg.substring(separatorIndex).toInt();
    }

    void help()
    {
        uint8_t msgCount = 8;
        const char *messages[msgCount] = {
            "CLOCKLED <hour 1-12> <state 0-2>",
            "SOCKETLED <socket 1-5> <state 0-2>",
            "STATUSLED <state 0-2>",
            "LEDGRPLOGIC <logic level 0/1>",
            "LEDINDLOGIC <logic level 0/1>",
            "SETHOUR <hour 0-24>",
            "SETMIN <minute 0-60>",
            "SETSEC <second 0-60>"};
        for (uint8_t i = 0; i < msgCount; i++)
            Serial.println(messages[i]);
    }

    LedAndState readLedAndState(String msg)
    {
        String cmd = msg.substring(msg.indexOf(SEPARATOR));
        int separatorIndex = cmd.indexOf(SEPARATOR);
        uint8_t led = cmd.substring(0, separatorIndex - 1).toInt();
        MyLedState state = (MyLedState)readCmdNumber(cmd.substring(1));
        return {led, state};
    }
};