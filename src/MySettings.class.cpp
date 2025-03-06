#include <Arduino.h>

class MySettings
{
private:
    bool LED_GROUP_ACTIVE_LOGIC;
    bool LED_INDIVIDUAL_ACTIVE_LOGIC;
    
public:

    MySettings(bool ledGroupActiveLogic, bool ledIndividualActiveLogic)
    {
        LED_GROUP_ACTIVE_LOGIC = ledGroupActiveLogic;
        LED_INDIVIDUAL_ACTIVE_LOGIC = ledIndividualActiveLogic;
    };

    bool setLedGroupActiveLogic(bool level) {
        LED_GROUP_ACTIVE_LOGIC=level;
        //TODO: save to EEPROM
        return true;
    }
    bool setLedIndividualActiveLogic(bool level) {
        LED_INDIVIDUAL_ACTIVE_LOGIC=level;
        //TODO: save to EEPROM
        return true;
    }

    bool getLedGroupActiveLogic(){
        return LED_GROUP_ACTIVE_LOGIC;
    }
    bool getLedIndividualActiveLogic(){
        return LED_INDIVIDUAL_ACTIVE_LOGIC;
    }
};