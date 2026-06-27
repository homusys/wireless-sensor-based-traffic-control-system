/** +++++++++++++++++++++++++++++++++++++++++++++
 * Copyright (C) 2024 Carl Matthew Arzadon
 * All Rights Reserved 
 * 
 * Unauthorized copying of this file, via any 
 * medium is strictly prohibited. Propietary 
 * and confidential.
 * 
 * @author Carl Matthew Arzadon
 * +++++++++++++++++++++++++++++++++++++++++++ */

#ifndef _BOARDS_H_
#define _BOARDS_H_

#include "NewPing.h"
#include "inttypes.h"


// ========= NETWORK CONSTANTS ========= //
const uint16_t board1 = 000; // main_board  ARDUINO 1 (MEGA2560EMBED)
const uint16_t board2 = 001; // slave_board ARDUINO 2
const uint16_t board3 = 011; // slave_board ARDUINO 3

const uint16_t board4 = 021; // slave_board ARDUINO 4
const uint16_t board5 = 002; // slave_board ARDUINO 5

const uint16_t board6 = 003; // slave_board ARDUINO 6
const uint16_t board7 = 012; // slave_board ARDUINO 7
const uint16_t board8 = 013; // slave_board ARDUINO 8

const uint16_t board9 = 005; // controller_board

const uint16_t board_master = board1; // main_board

#if defined(BOARD_MASTER) 
    int8_t current_board = board1;

    #define NRF24L01_MOSI 51
    #define NRF24L01_MISO 50
    #define NRF24L01_SCK  52
    #define NRF24L01_CSN   8
    #define NRF24L01_CE    7

    #define LTR1_RELAY  9
    #define LTY1_RELAY 10
    #define LTG1_RELAY 11

    #define R1_RELAY 12
    #define Y1_RELAY 14
    #define G1_RELAY 15

    #define R3_RELAY 16
    #define Y3_RELAY 17
    #define G3_RELAY 18

    #define RTC_SCL  21
    #define RTC_SDA  20


#elif defined(BOARD_A2)
    /// @todo board 2 is a regular arduino thus map everything to
    ///       regular arduino 
    int8_t current_board = board2;

    #define NRF24L01_MOSI 11
    #define NRF24L01_MISO 12
    #define NRF24L01_SCK  13
    #define NRF24L01_CSN  10
    #define NRF24L01_CE    9
    
    #define LTR2_RELAY A2
    #define LTY2_RELAY A1
    #define LTG2_RELAY A0

    #define R2_RELAY 4
    #define Y2_RELAY 3
    #define G2_RELAY 2

    #define R4_RELAY 7
    #define Y4_RELAY 6
    #define G4_RELAY 5


#elif defined(BOARD_A3)
    int8_t current_board = board3;

    #define NRF24L01_MOSI 11
    #define NRF24L01_MISO 12
    #define NRF24L01_SCK  13
    #define NRF24L01_CSN  10
    #define NRF24L01_CE    9

    #define ECHO_PIN 2
    #define TRIG_PIN 3

    #define PR1_RELAY 7
    #define PG1_RELAY 6

    #define MAX_DIST 450
    NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DIST);


#elif defined(BOARD_A4)
    int8_t current_board = board4;

    #define NRF24L01_MOSI 11
    #define NRF24L01_MISO 12
    #define NRF24L01_SCK  13
    #define NRF24L01_CSN  10
    #define NRF24L01_CE    9

    #define ECHO_PIN 2
    #define TRIG_PIN 3

    #define PR2_RELAY 7
    #define PG2_RELAY 6

    #define MAX_DIST 450
    NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DIST);


#elif defined(BOARD_A5)
    int8_t current_board = board5;

    #define NRF24L01_MOSI 11
    #define NRF24L01_MISO 12
    #define NRF24L01_SCK  13
    #define NRF24L01_CSN  10
    #define NRF24L01_CE    9

    #define ECHO_PIN1 2
    #define TRIG_PIN1 3

    #define ECHO_PIN2 4
    #define TRIG_PIN2 5

    #define MAX_DIST 450
    NewPing sonar1(TRIG_PIN1, ECHO_PIN1, MAX_DIST);
    NewPing sonar2(TRIG_PIN2, ECHO_PIN2, MAX_DIST);


#elif defined(BOARD_A6)
    int8_t current_board = board6;

    #define NRF24L01_MOSI 11
    #define NRF24L01_MISO 12
    #define NRF24L01_SCK  13
    #define NRF24L01_CSN  10
    #define NRF24L01_CE    9

    #define ECHO_PIN1 2
    #define TRIG_PIN1 3

    #define ECHO_PIN2 4
    #define TRIG_PIN2 5

    #define MAX_DIST 450
    NewPing sonar1(TRIG_PIN1, ECHO_PIN1, MAX_DIST);
    NewPing sonar2(TRIG_PIN2, ECHO_PIN2, MAX_DIST);


#elif defined(BOARD_A7)
    int8_t current_board = board7;

    #define NRF24L01_MOSI 11
    #define NRF24L01_MISO 12
    #define NRF24L01_SCK  13
    #define NRF24L01_CSN  10
    #define NRF24L01_CE    9

    #define ECHO_PIN 2
    #define TRIG_PIN 3
    
    #define MAX_DIST 450
    NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DIST);


#elif defined(BOARD_A8)
    int8_t current_board = board8;

    #define NRF24L01_MOSI 11
    #define NRF24L01_MISO 12
    #define NRF24L01_SCK  13
    #define NRF24L01_CSN  10
    #define NRF24L01_CE    9

    #define ECHO_PIN 2
    #define TRIG_PIN 3

    #define MAX_DIST 450
    NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DIST);

#elif defined(BOARD_CONTROL)
    int8_t current_board = board9;

    #define NRF24L01_MOSI 51
    #define NRF24L01_MISO 50
    #define NRF24L01_SCK  52
    #define NRF24L01_CSN  30
    #define NRF24L01_CE   31
    

    /// @todo replace pins with proper pins
    #define B1_PIN  4
    #define B2_PIN  5
    #define B3_PIN  6
    #define B4_PIN  7
    #define B5_PIN  8
    #define B6_PIN  9
    #define B7_PIN 10
    #define B8_PIN 11
    #define B9_PIN 32

    #define M1_PIN 14 
    #define M2_PIN 15
    #define M3_PIN 16
    #define M4_PIN 17
    #define M5_PIN 18
    #define M6_PIN 19
    #define M7_PIN 20 // Sequence trigger
    #define M8_PIN 21 // Switch

    #define M1_LED 22
    #define M2_LED 23
    #define M3_LED 24
    #define M4_LED 25
    #define M5_LED 26
    #define M6_LED 27
    #define M7_LED 28
    #define M8_LED 29

#elif defined(BOARD_CONTROL_TEST)
    int8_t current_board = board9;

    #define NRF24L01_MOSI 11
    #define NRF24L01_MISO 12
    #define NRF24L01_SCK  13
    #define NRF24L01_CSN  10
    #define NRF24L01_CE    9

    #define B1_PIN  5
    #define B2_PIN A0
    #define B3_PIN A0
    #define B4_PIN A0
    #define B5_PIN A0
    #define B6_PIN A0
    #define B7_PIN A0
    #define B8_PIN A0
    
    #define M1_PIN  2
    #define M2_PIN A0
    #define M3_PIN A0
    #define M4_PIN A0
    #define M5_PIN A0
    #define M6_PIN A0
    #define M7_PIN  3
    #define M8_PIN  4
#endif


#endif