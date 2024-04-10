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
const uint16_t board3 = 002; // slave_board ARDUINO 3

const uint16_t board4 = 012; // slave_board ARDUINO 4
const uint16_t board5 = 003; // slave_board ARDUINO 5

const uint16_t board6 = 013; // slave_board ARDUINO 6
const uint16_t board7 = 004; // slave_board ARDUINO 7
const uint16_t board8 = 005; // slave_board ARDUINO 8

const uint16_t board_master = board1; // main_board


#if defined(BOARD_MASTER) 
    int8_t current_board = board1;

    #define NRF24L01_MOSI 51
    #define NRF24L01_MISO 50
    #define NRF24L01_SCK  52
    #define NRF24L01_CSN   8
    #define NRF24L01_CE    7

    #define LT1_RELAY 9

    #define R1_RELAY 10
    #define Y1_RELAY 11
    #define G1_RELAY 12

    #define R3_RELAY 14
    #define Y3_RELAY 15
    #define G3_RELAY 16

    #define RTC_SCL  21
    #define RTC_SDA  20


#elif defined(BOARD_A2)
    int8_t current_board = board2;

    #define NRF24L01_MOSI 11
    #define NRF24L01_MISO 12
    #define NRF24L01_SCK  13
    #define NRF24L01_CSN  10
    #define NRF24L01_CE    9
    

    #define LT2_RELAY 8

    #define R2_RELAY 7
    #define Y2_RELAY 6
    #define G2_RELAY 5

    #define R4_RELAY 4
    #define Y4_RELAY 3
    #define G4_RELAY 2


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


#endif


#endif