#include <Arduino.h>
#include <SPI.h>
#include "RF24.h"
#include "RF24Network.h"

#define BOARD_CONTROL 1

#include "boards.h"
#include "datapack.h"
#include "events.h"
#include "sensor.h"
#include "control.h"

#define INTERVAL_MS 350


// ========= VARIABLES ========= //
unsigned long previous_time = 0;

bool is_bypass = false;
bool is_manual = false;

short prev_bbtn_state[BM_TOTAL] = {0};
short curr_bbtn_state[BM_TOTAL] = {0};

short prev_mbtn_state[MM_TOTAL] = {0};
short curr_mbtn_state[MM_TOTAL] = {0};
short manual_sw_state = LOW;

enum SensorState bypass_sensor_states[BM_TOTAL];
enum Events_Man previous_manual = SCENE_M6A;
enum Events_Man current_manual  = SCENE_M6A;
enum ControlMode current_mode = NOCONTROL;

RF24 radio(NRF24L01_CE, NRF24L01_CSN);
RF24Network network(radio);


enum Events_Man get_manual_event(short index) {
    switch (index) {
    case 0: return SCENE_M1A;
    case 1: return SCENE_M2A;
    case 2: return SCENE_M3A;
    case 3: return SCENE_M4A;
    case 4: return SCENE_M5A;
    case 5: return SCENE_M6A;
    }
}


void broadcast_control(void) {
    RF24NetworkHeader send_bmstr(board_master);
    DataPack send_dp;

    send_dp.type = CONTR_T;
    send_dp.ctrl = current_mode;
    send_dp.man_active = is_manual;
    
    switch (send_dp.ctrl) {
    case NOCONTROL:
        break;


    case BYPASS:
        for (uint8_t sensor = 0; sensor < BM_TOTAL; ++sensor) {
            send_dp.ss[sensor] = bypass_sensor_states[sensor];
        }
        break;


    case MANUAL:
        break;
    }

    network.write(send_bmstr, &send_dp, sizeof(DataPack));
}


void setup(void) {
    /// @todo initialize sensor states, previous_manual, and current_manual
    pinMode(B1_PIN, INPUT);
    pinMode(B2_PIN, INPUT);
    pinMode(B3_PIN, INPUT);
    pinMode(B4_PIN, INPUT);
    pinMode(B5_PIN, INPUT);
    pinMode(B6_PIN, INPUT);
    pinMode(B7_PIN, INPUT);
    pinMode(B8_PIN, INPUT);
    
    pinMode(M1_PIN, INPUT);
    pinMode(M2_PIN, INPUT);
    pinMode(M3_PIN, INPUT);
    pinMode(M4_PIN, INPUT);
    pinMode(M5_PIN, INPUT);
    pinMode(M6_PIN, INPUT);

    // ========= NRF24L01 SETUP ========= //
    radio.begin();
    radio.setPayloadSize(sizeof(DataPack));
    radio.setDataRate(RF24_2MBPS);
    radio.setPALevel(RF24_PA_MIN);

    network.begin(90, current_board);
}



void loop(void) {
    network.update();

    
    // ========= READ BYPASS BTNS ========= //
    curr_bbtn_state[0] = digitalRead(B1_PIN);
    curr_bbtn_state[1] = digitalRead(B2_PIN);
    curr_bbtn_state[2] = digitalRead(B3_PIN);
    curr_bbtn_state[3] = digitalRead(B4_PIN);
    curr_bbtn_state[4] = digitalRead(B5_PIN);
    curr_bbtn_state[5] = digitalRead(B6_PIN);
    curr_bbtn_state[6] = digitalRead(B7_PIN);
    curr_bbtn_state[7] = digitalRead(B8_PIN);
    
    // ========= READ MANUAL BTNS ========= //
    curr_mbtn_state[0] = digitalRead(M1_PIN);
    curr_mbtn_state[1] = digitalRead(M2_PIN);
    curr_mbtn_state[2] = digitalRead(M3_PIN);
    curr_mbtn_state[3] = digitalRead(M4_PIN);
    curr_mbtn_state[4] = digitalRead(M5_PIN);
    curr_mbtn_state[5] = digitalRead(M6_PIN);
    
    manual_sw_state = digitalRead(M7_PIN);
    is_manual = false;
    
    // ========= PROCESS INPUTS ========= //
    if (manual_sw_state == HIGH) {
        is_manual = true;
    }
    else {
        current_manual = SCENE_M6A;
    }


    is_bypass = false;
    // ========= MANUAL ========= //
    if (is_manual) {
        for (short i=0; i<MM_TOTAL; ++i) {
            if (prev_mbtn_state[i] != curr_mbtn_state[i]) {
                if (curr_mbtn_state[i] == HIGH) {


                    previous_manual = current_manual;
                    current_manual  = get_manual_event(i);
                    break;

                }
                prev_mbtn_state[i] = curr_mbtn_state[i];
            }
        }
    }


    // To save computing cycles, do not process bypass mode on manual mode.
    else {
        for (short i=0; i<BM_TOTAL; ++i) {
            bypass_sensor_states[i] = INACTIVE;

            if (prev_bbtn_state[i] != curr_bbtn_state[i]) {
                if (curr_mbtn_state[i] == HIGH) {

                    is_bypass = true;
                    // ========= BYPASS ========= //
                    bypass_sensor_states[i] = ACTIVE;

                }
                prev_bbtn_state[i] = curr_bbtn_state[i];
            }
        }


        // ========= DEFAULT ========= //
        if (!is_bypass) {
            /// @todo reset everything to default
            previous_manual = SCENE_M6A;
            current_manual  = SCENE_M6A;

            memset(curr_bbtn_state, 0x0, sizeof(curr_bbtn_state));
            memset(prev_bbtn_state, 0x0, sizeof(prev_bbtn_state));

            memset(curr_mbtn_state, 0x0, sizeof(curr_mbtn_state));
            memset(prev_mbtn_state, 0x0, sizeof(prev_mbtn_state));
        }
    }

    /// @todo invoke send function
    broadcast_control();
}