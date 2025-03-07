#include <Arduino.h>
#include "./MyLedState.enum.h"
#include "./MyLedMatrix.class.cpp"
#include "./MySettings.class.h"
#include "./MyTimer.class.cpp"

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
    MyTimer *TIMER;

    MySerialHandler(MyLedMatrix *LedMatrix, MySettings *settings, MyTimer *timer, bool echo = true)
    {
        LED_MATRIX = LedMatrix;
        SETTINGS = settings;
        TIMER = timer;
        ECHO = echo;
    };

    void motd()
    {
        Serial.println(F("Type \"HELP\" for info."));
    }

    void handleMessage()
    {
        while (Serial.available())
        {
            if (message.length() == MAX_MSG_LEN)
            {
                message = "";
                Serial.println(F("Message too long. Resetting command."));
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

    void flush() // clears errors in first command
    {
        while (Serial.available())
            Serial.read();
        message = "";
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
        // GETTIME
        else if (msg.startsWith("GETTIME"))
        {
            printTime();

            successful = true;
        }
        // SETTIME <h> <m> <s> <ms>
        else if (msg.startsWith("SETTIME"))
            successful = setTime(msg);

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
        uint8_t msgCount = 7;
        const char *messages[msgCount] = {
            "CLOCKLED <hour 1-12> <state 0-2>",
            "SOCKETLED <socket 1-5> <state 0-2>",
            "STATUSLED <state 0-2>",
            "LEDGRPLOGIC <logic level 0/1>",
            "LEDINDLOGIC <logic level 0/1>",
            "GETTIME",
            "SETTIME <HH> <MM> <SS>"};
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

    bool setTime(String msg)
    {
        int separatorIndex;
        int valueIndex = 0;
        uint16_t values[3] = {0, 0, 0};
        String cmd = msg;
        do
        {
            values[valueIndex] = readCmdNumber(cmd);
            Serial.println(values[valueIndex]);
            separatorIndex = cmd.indexOf(SEPARATOR);
            cmd = cmd.substring(separatorIndex);
            cmd.trim();

            valueIndex++;

        } while (separatorIndex > -1 && valueIndex < 3);

        bool successfulSettingUpdate = SETTINGS->setHour(values[0]) && SETTINGS->setMinute(values[1]) && SETTINGS->setSecond(values[2]);
        bool successfulTimerUpdate = false;
        if (successfulSettingUpdate)
            successfulTimerUpdate = TIMER->setHour(values[0]) && TIMER->setMinute(values[1]) && TIMER->setSecond(values[2]) && TIMER->setMillisecond(0);

        return successfulSettingUpdate && successfulTimerUpdate;
    }

    void printTime(){
        MyTime time = TIMER->getTime();
            Serial.print(time.hour);
            Serial.print(':');
            Serial.print(time.minute);
            Serial.print(':');
            Serial.print(time.second);
            Serial.print('.');
            Serial.print(time.millisecond);
            Serial.println();
    }
};