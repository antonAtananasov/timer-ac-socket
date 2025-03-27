#ifndef MySerialHandler_CLASS
#define MySerialHandler_CLASS

#include <Arduino.h>
#include "MyLedState.enum.h"
#include "MyLedMatrix.class.h"
#include "MySettings.class.h"
#include "MyTimer.class.h"
#include "MyProgramHandler.class.h"
#include "MySocketManager.class.h"
#include "MyStringHelper.class.h"

class MySerialHandler
{
private:
    const char TERMINATOR = (char)13;
    const char SEPARATOR = ' ';
    const uint8_t MAX_MSG_LEN = 20;

    String message = "";



public:
    bool ECHO = true; // echo input back through serial
    MySettings *SETTINGS;
    MyLedMatrix *LED_MATRIX;
    MySocketManager *SOCKET_MANAGER;
    MyTimer *TIMER;
    MyProgramHandler *PROGRAM_HANDLER;

    MySerialHandler(MySettings *Settings, MyLedMatrix *LedMatrix, MySocketManager *SocketManager, MyTimer *timer, MyProgramHandler *ProgramHandler, bool echo = true)
    {
        LED_MATRIX = LedMatrix;
        SETTINGS = Settings;
        TIMER = timer;
        ECHO = echo;
        PROGRAM_HANDLER = ProgramHandler;
        SOCKET_MANAGER = SocketManager;
    };

    void printMOTD()
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

        // HELP
        if (msg.startsWith(F("HELP")))
        {
            help();
            successful = true;
        }
        // CLOCKLED <HOUR> <STATE>
        // SOCKETLED <SOCKET> <STATE>
        if (msg.startsWith(F("CLOCKLED")))
        {
            int *values = readCmdNumber(msg, 2);
            successful = LED_MATRIX->setClockLed(values[0], (MyLedState)values[1]);
        }
        else if (msg.startsWith(F("SOCKETLED")))
        {
            int *values = readCmdNumber(msg, 2);
            successful = LED_MATRIX->setSocketLed(values[0], (MyLedState)values[1]);
        }
        // SETSOCK <socket 1-5> <state 0-1>
        // GETSOCK <socket 1-5>
        else if (msg.startsWith(F("SETSOCK")))
        {
            int *values = readCmdNumber(msg, 2);
            successful = SOCKET_MANAGER->setSocketState(values[0], (bool)values[1]);
        }
        else if (msg.startsWith(F("GETSOCK")))
        {
            int value = readCmdNumber(msg);
            Serial.println(SOCKET_MANAGER->getSocketState(value));
            successful = true;
        }
        // STATUSLED <STATE>
        // ALLLEDS <STATE>
        else if (msg.startsWith(F("STATUSLED")))
            successful = LED_MATRIX->setStatusLed((MyLedState)readCmdNumber(msg));
        else if (msg.startsWith(F("ALLLEDS")))
            successful = LED_MATRIX->setAllLeds((MyLedState)readCmdNumber(msg));
        // LEDGRPLOGIC <0/1>
        // LEDINDLOGIC <0/1>
        // SOCKLOGIC <0/1>
        // BTNLOGIC <0/1>
        else if (msg.startsWith(F("LEDGRPLOGIC")))
            successful = SETTINGS->setLedGroupActiveLogic((bool)readCmdNumber(msg));
        else if (msg.startsWith(F("LEDINDLOGIC")))
            successful = SETTINGS->setLedIndividualActiveLogic((bool)readCmdNumber(msg));
        else if (msg.startsWith(F("SOCKLOGIC")))
            successful = SETTINGS->setSocketsActiveLogic((bool)readCmdNumber(msg));
        else if (msg.startsWith(F("BTNLOGIC")))
            successful = SETTINGS->setButtonActiveLogic((bool)readCmdNumber(msg));
        // GETTIME
        // SETTIME <h> <m> <s>
        else if (msg.startsWith(F("GETTIME")))
        {
            printTime();
            successful = true;
        }
        else if (msg.startsWith(F("SETTIME")))
            successful = setTime(msg);
        // GETACTIVE <HH> <MM> <socket 1-5>
        // SETACTIVE <HH> <MM> <socket 1-5>
        else if (msg.startsWith(F("GETACTIVE")))
        {
            int *values = readCmdNumber(msg, 3);

            successful = SETTINGS->getByteAndBitNumberForSocketActivity(values[0], values[1], values[2]).valid;

            if (successful)
                Serial.println(SETTINGS->getSocketActivity(values[0], values[1], values[2]));
        }
        else if (msg.startsWith(F("SETACTIVE")))
        {
            int *values = readCmdNumber(msg, 4);
            successful = SETTINGS->setSocketActivity(values[0], values[1], values[2], values[3]);
        }
        // SETMODE <mode>
        // GETMODE
        else if (msg.startsWith(F("SETMODE")))
        {
            int value = readCmdNumber(msg);
            successful = PROGRAM_HANDLER->setProgramMode((MyProgramMode)value);
        }
        else if (msg.startsWith(F("GETMODE")))
        {
            Serial.println(PROGRAM_HANDLER->getProgramMode());
            successful = true;
        }

        // always at end
        if (successful)
            Serial.println(F("OK."));
        else
        {
            Serial.print(F("Err.:"));
            Serial.print(msg);
            Serial.println();
        }
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
        for (uint8_t i = 0; i < MyStringHelper::helpMessageCount; i++)
            Serial.println((const __FlashStringHelper*)pgm_read_ptr(&MyStringHelper::helpMessages[i]));
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