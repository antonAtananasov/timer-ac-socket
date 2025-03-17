#ifndef UserSettings_CONST
#define UserSettings_CONST
#include <Arduino.h>

// USER SETTINGS

//  Time keeping
#define TIME_SHOW_DELAY MS_IN_ONE_SECOND //delay between led matrix updates when displaying current time
#define TIME_SAVE_DELAY MS_IN_ONE_MINUTE * FIVE_MINUTES //delay between saving current time to EEPROM // saveToEEPROMIntervalMs = 0 means do not save

//  Display settings
#define LED_BLINK_DELAY_MS 350 //how long an LED stays on and then off in ms when its supposed to blink


// PHYSICAL CONFIGURATION
//  Wiring Active logic configuration
#define DEFAULT_LED_GROUP_ACTIVE_LOGIC HIGH // LED Logic level for enabling a LED group when not valid in MEMORY CONFIGURATION below
#define DEFAULT_LED_INDIVIDUAL_ACTIVE_LOGIC HIGH // LED Logic level for enabling an individual LED in a group when not valid in MEMORY CONFIGURATION below
#define DEFAULT_SOCKET_ACTIVE_LOGIC LOW // Logic level for enabling an individual socket when not valid in MEMORY CONFIGURATION below

// LED Matrix wiring configuration
//  LED Multiplex groups pins // 18 leds are multiplexed in 6 groups of 3
#define LED_GROUP_1_PIN 2
#define LED_GROUP_2_PIN 3
#define LED_GROUP_3_PIN 4
#define LED_GROUP_4_PIN 5
#define LED_GROUP_5_PIN 6
#define LED_GROUP_6_PIN 7
//  LED Multiplex individual leds pins
#define LED_INDIVIDUAL_1_PIN 8
#define LED_INDIVIDUAL_2_PIN 9
#define LED_INDIVIDUAL_3_PIN 10
//  LED Pair combinations that describe which two pins to activate to address a specific led: 
//      12 clock leds from 1 to 12 o'clock
//      5 socket leds from 1 to 5
//      1 status led
#define LED_PIN_PAIRS_LAYOUT \
(uint8_t[6][3][2]){{{LED_GROUP_1_PIN, LED_INDIVIDUAL_1_PIN}, {LED_GROUP_1_PIN, LED_INDIVIDUAL_2_PIN}, {LED_GROUP_1_PIN, LED_INDIVIDUAL_3_PIN}},\
 {{LED_GROUP_2_PIN, LED_INDIVIDUAL_1_PIN}, {LED_GROUP_2_PIN, LED_INDIVIDUAL_2_PIN}, {LED_GROUP_2_PIN, LED_INDIVIDUAL_3_PIN}},\
 {{LED_GROUP_3_PIN, LED_INDIVIDUAL_1_PIN}, {LED_GROUP_3_PIN, LED_INDIVIDUAL_2_PIN}, {LED_GROUP_3_PIN, LED_INDIVIDUAL_3_PIN}},\
 {{LED_GROUP_4_PIN, LED_INDIVIDUAL_1_PIN}, {LED_GROUP_4_PIN, LED_INDIVIDUAL_2_PIN}, {LED_GROUP_4_PIN, LED_INDIVIDUAL_3_PIN}},\
 \
 {{LED_GROUP_5_PIN, LED_INDIVIDUAL_1_PIN}, {LED_GROUP_5_PIN, LED_INDIVIDUAL_2_PIN}, {LED_GROUP_5_PIN, LED_INDIVIDUAL_3_PIN}},\
 {{LED_GROUP_6_PIN, LED_INDIVIDUAL_1_PIN}, {LED_GROUP_6_PIN, LED_INDIVIDUAL_2_PIN},\
 \
 {LED_GROUP_6_PIN, LED_INDIVIDUAL_3_PIN}}}

 
// MEMORY CONFIGURATION
//  EEPROM Addresses
//      Time
#define EEPROM_HOUR_BYTE_ADDRES 0 //where to store current time hour
#define EEPROM_MINUTE_BYTE_ADDRES 1 //where to store current time minute
#define EEPROM_SECOND_BYTE_ADDRES 2 //where to store current time second
//      Logic level
#define EEPROM_LOGIC_LEVELS_BYTE_ADDRES 3 // where to store logic level bits
#define EEPROM_LOGIC_LEVEL_LED_GROUP_BIT_NUMBER 0 // LED Logic level bit for enabling a LED group
#define EEPROM_LOGIC_LEVEL_LED_INDIVIDUAL_BIT_NUMBER 1 // LED Logic level bit for enabling an individual LED in a group
#define EEPROM_LOGIC_LEVEL_SOCKET_BIT_NUMBER 2 // Logic level bit for enabling an individual socket
//      Active state
#define EEPROM_SOCKET_ACTIVITY_START_BYTE_ADDRES 4 // start address for storing when each socket should be active for every 5min period in 24hrs (takes EEPROM_SOCKET_ACTIVITY_BYTES_TOTAL (180) bytes)

// Ensure a valid config
#include "MyValidation.validator.h"
#endif
