#ifndef MySerialHandler_CLASS
#define MySerialHandler_CLASS

#include <Arduino.h>
#include "MyLedState.enum.h"
#include "MyLedMatrix.class.h"
#include "MySettings.class.h"
#include "MyTimer.class.h"

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

    MySerialHandler(MyLedMatrix *LedMatrix, MySettings *Settings, MyTimer *timer, bool echo = true)
    {
        LED_MATRIX = LedMatrix;
        SETTINGS = Settings;
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
            int *values = readCmdNumber(msg, 2);
            successful = LED_MATRIX->setClockLed(values[0], (MyLedState)values[1]);
        }
        else if (msg.startsWith("SOCKETLED"))
        {
            int *values = readCmdNumber(msg, 2);
            successful = LED_MATRIX->setSocketLed(values[0], (MyLedState)values[1]);
        }
        // STATUSLED <STATE>
        // ALLLEDS <STATE>
        else if (msg.startsWith("STATUSLED"))
            successful = LED_MATRIX->setStatusLed((MyLedState)readCmdNumber(msg));
        else if (msg.startsWith("ALLLEDS"))
            successful = LED_MATRIX->setAllLeds((MyLedState)readCmdNumber(msg));
        // LEDGRPLOGIC <0/1>
        // LEDINDLOGIC <0/1>
        // SOCKLOGIC <0/1>
        else if (msg.startsWith("LEDGRPLOGIC"))
            successful = SETTINGS->setLedGroupActiveLogic((bool)readCmdNumber(msg));
        else if (msg.startsWith("LEDINDLOGIC"))
            successful = SETTINGS->setLedIndividualActiveLogic((bool)readCmdNumber(msg));
        else if (msg.startsWith("SOCKLOGIC"))
            successful = SETTINGS->setSocketsActiveLogic((bool)readCmdNumber(msg));
        // GETTIME
        // SETTIME <h> <m> <s> <ms>
        else if (msg.startsWith("GETTIME"))
        {
            printTime();
            successful = true;
        }
        else if (msg.startsWith("SETTIME"))
            successful = setTime(msg);
        // GETACTIVE <HH> <MM> <socket 1-5>
        // SETACTIVE <HH> <MM> <socket 1-5>
        else if (msg.startsWith("GETACTIVE"))
        {
            int *values = readCmdNumber(msg, 3);

            successful = SETTINGS->getByteAndBitNumberForSocketActivity(values[0], values[1], values[2]).valid;

            if (successful)
                Serial.println(SETTINGS->getSocketActivity(values[0], values[1], values[2]));
        }
        else if (msg.startsWith("SETACTIVE"))
        {
            int *values = readCmdNumber(msg, 4);
            successful = SETTINGS->setSocketActivity(values[0], values[1], values[2], values[3]);
        }

        // always at end
        Serial.println(successful ? "OK." : "Err.:" + msg);
    }

    int readCmdNumber(String msg)
    {
        int separatorIndex = msg.indexOf(SEPARATOR);
        return msg.substring(separatorIndex).toInt();
    }
    int *readCmdNumber(String msg, uint8_t numberCount)
    {
        int separatorIndex;
        int valueIndex = 0;
        int values[numberCount];
        String cmd = msg;
        do
        {
            values[valueIndex] = readCmdNumber(cmd);
            Serial.println(values[valueIndex]);
            separatorIndex = cmd.indexOf(SEPARATOR);
            cmd = cmd.substring(separatorIndex);
            cmd.trim();

            valueIndex++;

        } while (separatorIndex > -1 && valueIndex < numberCount);

        int *valuesPtr = values;
        return valuesPtr;
    }

    void help()
    {
        uint8_t msgCount = 10;
        const char *messages[msgCount] = {
            "CLOCKLED <hour 1-12> <state 0-2>",
            "SOCKETLED <socket 1-5> <state 0-2>",
            "STATUSLED <state 0-2>",
            "LEDGRPLOGIC <0/1>",
            "LEDINDLOGIC <0/1>",
            "SOCKLOGIC <0/1>",
            "GETTIME",
            "SETTIME <HH> <MM> <SS>",
            "GETACTIVE <HH> <MM> <socket 1-5>",
            "SETACTIVE <HH> <MM> <socket 1-5>"};
        for (uint8_t i = 0; i < msgCount; i++)
            Serial.println(messages[i]);
    }

    bool setTime(String msg)
    {
        int *values = readCmdNumber(msg, 3);

        bool successfulSettingUpdate = SETTINGS->saveHour(values[0]) && SETTINGS->saveMinute(values[1]) && SETTINGS->saveSecond(values[2]);
        bool successfulTimerUpdate = successfulSettingUpdate && TIMER->setHour(values[0]) && TIMER->setMinute(values[1]) && TIMER->setSecond(values[2]) && TIMER->setMillisecond(0);

        return successfulTimerUpdate;
    }
    void printTime()
    {
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
#endif