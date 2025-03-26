#ifndef MySocketManager_CLASS
#define MySocketManager_CLASS

#include <Arduino.h>
#include "MySettings.class.h"
#include "MyLedMatrix.class.h"
#include "MyTimer.class.h"

class MySocketManager
{
private:
    uint8_t SOCKET_RELAY_PINS[SOCKETS_COUNT] = {SOCKET_RELAY_1_PIN, SOCKET_RELAY_2_PIN, SOCKET_RELAY_3_PIN, SOCKET_RELAY_4_PIN, SOCKET_RELAY_5_PIN};

public:
    MySettings *SETTINGS;
    MyLedMatrix *LED_MATRIX;
    MyTimer *TIMER;

    MySocketManager(MySettings *Settings, MyLedMatrix *LedMatrix, MyTimer *Timer)
    {
        SETTINGS = Settings;
        LED_MATRIX = LedMatrix;
        TIMER = Timer;

        for (uint8_t i = 0; i < SOCKETS_COUNT; i++)
            pinMode(SOCKET_RELAY_PINS[i], OUTPUT);
    };

    void update()
    {
        MyTime time = TIMER->getTime();

        for (uint8_t i = 1; i <= SOCKETS_COUNT; i++)
        {
            bool socketActive = SETTINGS->getSocketActivity(time.hour, (time.minute / 5) * 5, i);
            setSocketState(i, socketActive);
        }
    }

    bool setSocketState(uint8_t socket, bool state)
    {
        if (socket < 1 || socket > SOCKETS_COUNT)
            return false;

        bool socketActiveLogic = SETTINGS->getSocketsActiveLogic();
        digitalWrite(SOCKET_RELAY_PINS[socket - 1], state ? socketActiveLogic : !socketActiveLogic);
        return true;
    }
    bool getSocketState(uint8_t socket)
    {
        if (socket < 1 || socket > SOCKETS_COUNT)
            return false;
        return digitalRead(SOCKET_RELAY_PINS[socket - 1]) == SETTINGS->getSocketsActiveLogic();
    }
};

#endif