#include <Arduino.h>
#include <SPI.h>
#include "RF24.h"
#include "RF24Network.h"

// #define BOARD_CONTROL 1
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

int prev_bbtn_state[BM_TOTAL] = {0};
int curr_bbtn_state[BM_TOTAL] = {0};

int prev_mbtn_state[MM_TOTAL] = {0};
int curr_mbtn_state[MM_TOTAL] = {0};
int bypass_sw_state = LOW;
int manual_sw_state = LOW;

enum SensorState bypass_sensor_states[BM_TOTAL];
enum Events_Man previous_manual = SCENE_M6A;
enum Events_Man current_manual  = SCENE_M6A;
enum ControlMode current_mode = NOCONTROL;

RF24 radio(NRF24L01_CE, NRF24L01_CSN);
RF24Network network(radio);


void turn_off_manual_leds(void) {
    #if BOARD_CONTROL
    digitalWrite(M1_LED, LOW);
    digitalWrite(M2_LED, LOW);
    digitalWrite(M3_LED, LOW);
    digitalWrite(M4_LED, LOW);
    digitalWrite(M5_LED, LOW);
    digitalWrite(M6_LED, LOW);
    digitalWrite(M7_LED, LOW);
    #endif
}

enum Events_Man get_manual_event(short index) {
    switch (index) {
    case 0: 
        digitalWrite(M1_LED, LOW);
        return SCENE_M1A;
    case 1: 
        digitalWrite(M2_LED, LOW);
        return SCENE_M2A;
    case 2: 
        digitalWrite(M3_LED, LOW);
        return SCENE_M3A;
    case 3: 
        digitalWrite(M4_LED, LOW);
        return SCENE_M4A;
    case 4: 
        digitalWrite(M5_LED, LOW);
        return SCENE_M5A;
    case 5: 
        digitalWrite(M6_LED, LOW);
        return SCENE_M6A;
    case 6: 
        digitalWrite(M7_LED, LOW);
        return SCENE_M7A;
    }
    return SCENE_M6A;
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
        for (size_t sensor=0; sensor < BM_TOTAL; ++sensor) {
            send_dp.ss[sensor] = bypass_sensor_states[sensor];
        }
        break;


    case MANUAL:
        send_dp.man  = current_manual;
        break;
    }
    network.write(send_bmstr, &send_dp, sizeof(DataPack));
}


void setup(void) {
    SPI.begin();
    Serial.begin(115200);

    #if BOARD_CONTROL
    pinMode(B1_PIN, INPUT);
    pinMode(B2_PIN, INPUT);
    pinMode(B3_PIN, INPUT);
    pinMode(B4_PIN, INPUT);
    pinMode(B5_PIN, INPUT);
    pinMode(B6_PIN, INPUT);
    pinMode(B7_PIN, INPUT);
    pinMode(B8_PIN, INPUT);
    pinMode(B9_PIN, INPUT);
    
    pinMode(M1_PIN, INPUT);
    pinMode(M2_PIN, INPUT);
    pinMode(M3_PIN, INPUT);
    pinMode(M4_PIN, INPUT);
    pinMode(M5_PIN, INPUT);
    pinMode(M6_PIN, INPUT);
    pinMode(M7_PIN, INPUT);
    pinMode(M8_PIN, INPUT);

    pinMode(M1_LED, OUTPUT);
    pinMode(M2_LED, OUTPUT);
    pinMode(M3_LED, OUTPUT);
    pinMode(M4_LED, OUTPUT);
    pinMode(M5_LED, OUTPUT);
    pinMode(M6_LED, OUTPUT);
    pinMode(M7_LED, OUTPUT);
    pinMode(M8_LED, OUTPUT);

    #elif BOARD_CONTROL_TEST
    pinMode(B1_PIN, INPUT);
    pinMode(B2_PIN, INPUT);
    // pinMode(B3_PIN, INPUT);
    // pinMode(B4_PIN, INPUT);
    // pinMode(B5_PIN, INPUT);
    // pinMode(B6_PIN, INPUT);
    // pinMode(B7_PIN, INPUT);
    // pinMode(B8_PIN, INPUT);
    
    pinMode(M1_PIN, INPUT);
    // pinMode(M2_PIN, INPUT);
    // pinMode(M3_PIN, INPUT);
    // pinMode(M4_PIN, INPUT);
    // pinMode(M5_PIN, INPUT);
    // pinMode(M6_PIN, INPUT);
    pinMode(M7_PIN, INPUT);
    pinMode(M8_PIN, INPUT);
    
    #endif

    // ========= NRF24L01 SETUP ========= //
    radio.begin();
    radio.setPayloadSize(sizeof(DataPack));
    radio.setDataRate(RF24_2MBPS);
    radio.setPALevel(RF24_PA_MIN);

    network.begin(90, current_board);
}



void loop(void) {
    network.update();

    
    #if BOARD_CONTROL
        // ========= READ BYPASS BTNS ========= //
        curr_bbtn_state[0] = digitalRead(B1_PIN);
        curr_bbtn_state[1] = digitalRead(B2_PIN);
        curr_bbtn_state[2] = digitalRead(B3_PIN);
        curr_bbtn_state[3] = digitalRead(B4_PIN);
        curr_bbtn_state[4] = digitalRead(B5_PIN);
        curr_bbtn_state[5] = digitalRead(B6_PIN);
        curr_bbtn_state[6] = digitalRead(B7_PIN);
        curr_bbtn_state[7] = digitalRead(B8_PIN);
        bypass_sw_state    = digitalRead(B9_PIN);
        
        // ========= READ MANUAL BTNS ========= //
        curr_mbtn_state[0] = digitalRead(M1_PIN);
        curr_mbtn_state[1] = digitalRead(M2_PIN);
        curr_mbtn_state[2] = digitalRead(M3_PIN);
        curr_mbtn_state[3] = digitalRead(M4_PIN);
        curr_mbtn_state[4] = digitalRead(M5_PIN);
        curr_mbtn_state[5] = digitalRead(M6_PIN);
        curr_mbtn_state[6] = digitalRead(M7_PIN);
        manual_sw_state    = digitalRead(M8_PIN);


    #elif BOARD_CONTROL_TEST
        // ========= READ BYPASS BTNS ========= //
        curr_bbtn_state[0] = digitalRead(B1_PIN);

        // ========= READ MANUAL BTNS ========= //
        curr_mbtn_state[0] = digitalRead(M1_PIN);
        curr_mbtn_state[6] = digitalRead(M7_PIN);
        manual_sw_state    = digitalRead(M8_PIN);
    #endif
    

    // ========= PROCESS INPUTS ========= //
    
    is_bypass = false;
    if (bypass_sw_state == HIGH) {
        is_bypass = true;
    }


    // If switch is in manual state then manual mode is true
    // bypass mode is ignored during manual mode
    is_manual = false;
    if (manual_sw_state == HIGH) {
        is_manual = true;
        is_bypass = false;

        #if BOARD_CONTROL
        // light up LED
        digitalWrite(M8_LED, HIGH);
        #endif
    }
    else {
        turn_off_manual_leds();
        current_manual = SCENE_M6A;
        
        #if BOARD_CONTROL
        // turn off LED
        digitalWrite(M8_LED, LOW);
        #endif
    }


    if (is_manual) {
        // The controller is in manual mode
        Serial.println("Currently in manual mode");
        current_mode = MANUAL;


        for (size_t i=0; i<MM_TOTAL; ++i) {
            // only run when a current manual btn state changed.
            if (prev_mbtn_state[i] != curr_mbtn_state[i]) {

                // Execute when there is a high button state
                if (curr_mbtn_state[i] == HIGH) {

                    // Only one button state should be high
                    turn_off_manual_leds();
                    previous_manual = current_manual;
                    current_manual  = get_manual_event(i);
                    break;

                }
                prev_mbtn_state[i] = curr_mbtn_state[i];
            }
        }

    }


    else if (is_bypass) {
        // The controller is in bypass mode
        Serial.println("Currently in bypass mode");
        current_mode = BYPASS;

        for (size_t i=0; i<BM_TOTAL; ++i) {
            bypass_sensor_states[i] = INACTIVE;

            // if (prev_bbtn_state[i] != curr_bbtn_state[i]) {
            if (curr_bbtn_state[i] == HIGH) {
                current_mode = BYPASS;
                bypass_sensor_states[i] = ACTIVE;
                Serial.print("------------- BypassBtn: ");
                Serial.println(i+1);
            }
            //     prev_bbtn_state[i] = curr_bbtn_state[i];
            // }
        }
    }


    else {
        // The controller is not running
        Serial.println("Not running any mode");
        current_mode = NOCONTROL;

        previous_manual = SCENE_M6A;
        current_manual  = SCENE_M6A;

        memset(curr_bbtn_state, 0, sizeof(curr_bbtn_state));
        memset(prev_bbtn_state, 0, sizeof(prev_bbtn_state));

        memset(curr_mbtn_state, 0, sizeof(curr_mbtn_state));
        memset(prev_mbtn_state, 0, sizeof(prev_mbtn_state));
    }

    broadcast_control();
}