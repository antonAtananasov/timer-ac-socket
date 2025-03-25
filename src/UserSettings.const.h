#ifndef UserSettings_CONST
#define UserSettings_CONST
#include <Arduino.h>

// USER SETTINGS

//  Time keeping
#define TIME_SHOW_DELAY MS_IN_ONE_SECOND //delay between led matrix updates when displaying current time
#define TIME_SAVE_DELAY MS_IN_ONE_MINUTE * FIVE_MINUTES //delay between saving current time to EEPROM // saveToEEPROMIntervalMs = 0 means do not save

//  Timing settings
#define LED_BLINK_DELAY_MS 350UL //how long an LED stays on and then off in ms when its supposed to blink
#define INPUT_INDICATION_DELAY 100UL //how long an LED stays on before returning to its state when an input action is detected
#define BUTTON_LONG_PRESS_DELAY 300UL //how long to hold button to register long press instead of normal press
#define BUTTON_DEBOUNCE_DELAY 50UL //how long to wait for button to stabilize


// PHYSICAL CONFIGURATION
//  Wiring Active logic configuration
#define DEFAULT_BUTTON_ACTIVE_LOGIC HIGH // Logic level for push button not valid in MEMORY CONFIGURATION below
#define DEFAULT_LED_GROUP_ACTIVE_LOGIC HIGH // LED Logic level for enabling a LED group when not valid in MEMORY CONFIGURATION below
#define DEFAULT_LED_INDIVIDUAL_ACTIVE_LOGIC HIGH // LED Logic level for enabling an individual LED in a group when not valid in MEMORY CONFIGURATION below
#define DEFAULT_SOCKET_ACTIVE_LOGIC LOW // Logic level for enabling an individual socket when not valid in MEMORY CONFIGURATION below

// Input controls wiring configuration
#define PUSH_BUTTON_PIN A3 // Pin with pull up/down resistor to read button press
#define SCROLL_WHEEL_PIN1 A2 // pin attached to CLK or primary pin of rotary encoder or mouse scroll wheel
#define SCROLL_WHEEL_PIN2 A1 // pin attached to DT or secondary pin of rotary encoder or mouse scroll wheel
#define SCROLL_WHEEL_LATCH_MODE 1 // latch mode number for rotary encoder from 1 to 3 as follows: 1 = 4 steps, Latch at position 3 only; 2 = 4 steps, Latch at position 0 (reverse wirings); 3 = 2 steps, Latch at position 0 and 3 


// Socket relays wiring configuration
#define SOCKET_RELAY_1_PIN 11u
#define SOCKET_RELAY_2_PIN 12u
#define SOCKET_RELAY_3_PIN 13u
#define SOCKET_RELAY_4_PIN A4
#define SOCKET_RELAY_5_PIN A5

// LED Matrix wiring configuration
//  LED Multiplex groups pins // 18 leds are multiplexed in 6 groups of 3
#define LED_GROUP_1_PIN 2u
#define LED_GROUP_2_PIN 3u
#define LED_GROUP_3_PIN 4u
#define LED_GROUP_4_PIN 5u
#define LED_GROUP_5_PIN 6u
#define LED_GROUP_6_PIN 7u
//  LED Multiplex individual leds pins
#define LED_INDIVIDUAL_1_PIN 8u
#define LED_INDIVIDUAL_2_PIN 9u
#define LED_INDIVIDUAL_3_PIN 10u
//  LED Pair combinations that describe which two pins to activate to address a specific led: 
//      12 clock leds from 1 to 12 o'clock
//      5 socket leds from 1 to 5
//      1 status led
#define LED_PIN_PAIRS_LAYOUT \
(uint8_t[6][3][2]){\
 /* clock leds */ \
 {{LED_GROUP_1_PIN, LED_INDIVIDUAL_1_PIN}, {LED_GROUP_1_PIN, LED_INDIVIDUAL_2_PIN}, {LED_GROUP_1_PIN, LED_INDIVIDUAL_3_PIN}},\
 {{LED_GROUP_2_PIN, LED_INDIVIDUAL_1_PIN}, {LED_GROUP_2_PIN, LED_INDIVIDUAL_2_PIN}, {LED_GROUP_2_PIN, LED_INDIVIDUAL_3_PIN}},\
 {{LED_GROUP_3_PIN, LED_INDIVIDUAL_1_PIN}, {LED_GROUP_3_PIN, LED_INDIVIDUAL_2_PIN}, {LED_GROUP_3_PIN, LED_INDIVIDUAL_3_PIN}},\
 {{LED_GROUP_4_PIN, LED_INDIVIDUAL_1_PIN}, {LED_GROUP_4_PIN, LED_INDIVIDUAL_2_PIN}, {LED_GROUP_4_PIN, LED_INDIVIDUAL_3_PIN}},\
 /* socket leds */ \
 {{LED_GROUP_5_PIN, LED_INDIVIDUAL_1_PIN}, {LED_GROUP_5_PIN, LED_INDIVIDUAL_2_PIN}, {LED_GROUP_5_PIN, LED_INDIVIDUAL_3_PIN}},\
 {{LED_GROUP_6_PIN, LED_INDIVIDUAL_1_PIN}, {LED_GROUP_6_PIN, LED_INDIVIDUAL_2_PIN},\
 /* status led */ \
 {LED_GROUP_6_PIN, LED_INDIVIDUAL_3_PIN}}}

 
// MEMORY CONFIGURATION
//  EEPROM Addresses
//      Time
#define EEPROM_HOUR_BYTE_ADDRES 0u //where to store current time hour
#define EEPROM_MINUTE_BYTE_ADDRES 1u //where to store current time minute
#define EEPROM_SECOND_BYTE_ADDRES 2u //where to store current time second
//      Logic level
#define EEPROM_LOGIC_LEVELS_BYTE_ADDRES 3u // where to store logic level bits
#define EEPROM_LOGIC_LEVEL_LED_GROUP_BIT_NUMBER 0u // LED Logic level bit for enabling a LED group
#define EEPROM_LOGIC_LEVEL_LED_INDIVIDUAL_BIT_NUMBER 1u // LED Logic level bit for enabling an individual LED in a group
#define EEPROM_LOGIC_LEVEL_SOCKET_BIT_NUMBER 2u // Logic level bit for enabling an individual socket
#define EEPROM_LOGIC_LEVEL_BUTTON_BIT_NUMBER 3u // Logic level bit for push button
//      Active state
#define EEPROM_SOCKET_ACTIVITY_START_BYTE_ADDRES 4u // start address for storing when each socket should be active for every 5min period in 24hrs (takes EEPROM_SOCKET_ACTIVITY_BYTES_TOTAL (180) bytes)


// Ensure a valid config
#include "MyValidation.validator.h"
#endif
