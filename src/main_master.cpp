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

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "RF24.h"
#include "RF24Network.h"
#include "DS3231.h"


// ========= BOARD CONFIG ========= //
#define BOARD_MASTER 1


#include "boards.h"
#include "datapack.h"
#include "events.h"
#include "sensor.h"
#include "control.h"


// ========= VARIABLES ========= //
DS3231 rtc;
bool h24 = false;
bool hPM = false;
bool is_reset = false;
bool is_bypass = false;
bool is_manual = false;
int seq_run = 0;
byte hour, prev_hour, next_hour;

RF24 radio(NRF24L01_CE, NRF24L01_CSN);
RF24Network network(radio);
unsigned long sensor_times[SENSOR_COUNT] = {0};
enum SensorState sensor_previous_states[SENSOR_COUNT];
enum SensorState main_sensor_states[SENSOR_COUNT];
enum SensorState bypass_buttons[BM_TOTAL];

enum Events previous_event, current_event;
enum Events_Seq current_sequence, previous_sequence;
enum Events_Man current_manual, previous_manual;
unsigned long current_event_time_last, current_event_time_limit;
unsigned long current_seq_time_last, current_seq_time_limit;
int event_cooldowns[EVENT_COUNT];
int green_light_grant_counter;
int green_light_grant_limit;

bool is_pre_yellow, is_post_yellow, is_finished, default_mode_two;

unsigned long time_last = 0;

short blinker = 0;
unsigned long blink_time_last, blink_time_current;
bool is_switching = false;


void count_green(void) {
    if (default_mode_two) {
        green_light_grant_counter += 1;
    }
}


/// @brief turn off all relay pins.
void turn_off_relays(int force) {
    if ((previous_event != current_event) || force) {

        digitalWrite(LTR1_RELAY, LOW);
        digitalWrite(LTY1_RELAY, LOW);
        digitalWrite(LTG1_RELAY, LOW);
        digitalWrite(R1_RELAY , LOW);
        digitalWrite(Y1_RELAY , LOW);
        digitalWrite(G1_RELAY , LOW);
        digitalWrite(R3_RELAY , LOW);
        digitalWrite(Y3_RELAY , LOW);
        digitalWrite(G3_RELAY , LOW);
    }
}


void _print_event(enum Events e) {
    switch (e) {
    case EVENT_00: Serial.println("EVENT_00"); break;
    case EVENT_1A: Serial.println("EVENT_1A"); break;
    case EVENT_1B: Serial.println("EVENT_1B"); break;
    case EVENT_1C: Serial.println("EVENT_1C"); break;
    case EVENT_1D: Serial.println("EVENT_1D"); break;
    case EVENT_2A: Serial.println("EVENT_2A"); break;
    case EVENT_2B: Serial.println("EVENT_2B"); break;
    case EVENT_2C: Serial.println("EVENT_2C"); break;
    case EVENT_2D: Serial.println("EVENT_2D"); break;
    case EVENT_3A: Serial.println("EVENT_3A"); break;
    case EVENT_3B: Serial.println("EVENT_3B"); break;
    case EVENT_3C: Serial.println("EVENT_3C"); break;
    case EVENT_4A: Serial.println("EVENT_4A"); break;
    case EVENT_4B: Serial.println("EVENT_4B"); break;
    case EVENT_4C: Serial.println("EVENT_4C"); break;
    case EVENT_5A: Serial.println("EVENT_5A"); break;
    case EVENT_5B: Serial.println("EVENT_5B"); break;
    case EVENT_6A: Serial.println("EVENT_6A"); break;
    case EVENT_6B: Serial.println("EVENT_6B"); break;
    }
}



/// @brief Reset sensor time with current sensor state value of 0.
void reset_sensor_data_times(void) {
    // Serial.println("reset_sensor_data_times::start");
    for (size_t i=0; i<SENSOR_COUNT; ++i) {
        if (main_sensor_states[i] == INACTIVE) {
            // Serial.print("reset_sensor_data_times::reset_sensor_index_");
            // Serial.println(i);
            sensor_times[i] = millis();
        }
    }
    // Serial.println("reset_sensor_data_times::finish");
}


/**
 * Record the time the sensor data arrived to the mainboard. The time
 * is used to check for conditions and execute specified scenarios.
 * @param sensor_index the the id of the sensor.
*/
void record_sensor_data_time(int sensor_index) {
    Serial.println("record_sensor_data_times::start");
    if ((sensor_previous_states[sensor_index] != main_sensor_states[sensor_index] ) &&
         main_sensor_states[sensor_index] != INACTIVE) 
    {
        sensor_times[sensor_index] = millis();
    }
    Serial.println("record_sensor_data_times::finish");
}


void force_record_sensor_data_time(int sensor_index) {
    if (main_sensor_states[sensor_index] != INACTIVE) {
        sensor_times[sensor_index] = millis();
    }
}


/**
 * Receive sensor data from other boards.
*/
void process_network_data(void) {
    // Serial.println("process_network_data::start");
    RF24NetworkHeader recv_h;
    DataPack recv_dp;

    while (network.available()) {
        // Serial.println("process_network_data::available_data");
        network.read(recv_h, &recv_dp, sizeof(DataPack));

        
        if (recv_dp.type == SENSOR_T) {
            // Serial.println("process_network_data::sensor_data_available");
            if (is_bypass) { continue; } // ignore any sensor updates during bypass mode

            switch (recv_h.from_node) {
                case board3:
                    // Serial.println("process_network_data::case_3");
                    main_sensor_states[SENSOR_5] = recv_dp.ss[SENSOR_5];
                    record_sensor_data_time(SENSOR_5);
                    break;


                case board4:
                    // Serial.println("process_network_data::case_4");
                    main_sensor_states[SENSOR_6] = recv_dp.ss[SENSOR_6];
                    record_sensor_data_time(SENSOR_6);
                    break;


                case board5:
                    // Serial.println("process_network_data::case_5");
                    main_sensor_states[SENSOR_1] = recv_dp.ss[SENSOR_1];
                    main_sensor_states[SENSOR_3] = recv_dp.ss[SENSOR_3];
                    main_sensor_states[SENSOR_7] = recv_dp.ss[SENSOR_7];
                    record_sensor_data_time(SENSOR_1);
                    record_sensor_data_time(SENSOR_3);
                    record_sensor_data_time(SENSOR_7);
                    break;


                case board6:
                    // Serial.println("process_network_data::case_6");
                    main_sensor_states[SENSOR_2] = recv_dp.ss[SENSOR_2];
                    main_sensor_states[SENSOR_4] = recv_dp.ss[SENSOR_4];
                    main_sensor_states[SENSOR_8] = recv_dp.ss[SENSOR_8];
                    record_sensor_data_time(SENSOR_2);
                    record_sensor_data_time(SENSOR_4);
                    record_sensor_data_time(SENSOR_8);
                    break;

                case board7:
                    Serial.println("process_network_data::case_7");
                    Serial.println("process_network_data::invalid_case");
                    break;


                case board8:
                    case 8: // ARDUINO BOARD 8 SENSOR 8
                    Serial.println("process_network_data::case_8");
                    Serial.println("process_network_data::invalid_case");
                    break;
            }
        }
        else if (recv_dp.type == CONTR_T) {
            switch (recv_dp.ctrl) {
            case NOCONTROL:
                is_bypass = false;
                is_manual = false;
                break;

            case BYPASS:
                is_bypass = true;
                is_manual = false;
                for (size_t sensor_index=0; sensor_index<SENSOR_COUNT; ++sensor_index) {
                    main_sensor_states[sensor_index] = recv_dp.ss[sensor_index];
                    record_sensor_data_time(sensor_index);
                }
                break;
            case MANUAL:
                is_bypass = false;
                is_manual = true;
                is_switching = false;

                if (previous_manual != recv_dp.man) {
                    previous_manual = current_manual;
                    is_switching = true;
                    blinker = 0;
                    turn_off_relays(1);
                }
                current_manual = recv_dp.man;
                break;
            }
        }
    }

    reset_sensor_data_times();
    // Serial.println("process_network_data::finish");
}

/// @brief updates the current event and the event array
void update_current_event(enum Events e, unsigned long ct, int cdv) {
    Serial.println("update_current_event::start");

    Serial.print("is_pre_yellow: ");
    Serial.println(is_pre_yellow);

    _print_event(e);
    _print_event(current_event);

    previous_event = current_event;

    if (e == EVENT_00 || cdv > 0) {
        current_event = EVENT_00;
        current_event_time_limit = EVENT_00_ACTIVE_TIME_MS;
        return;
    }


    if (is_post_yellow) {
        switch (current_event) {
        case EVENT_1B:
        case EVENT_1C:
            current_event = EVENT_1D;
            current_event_time_limit = EVENT_1D_ACTIVE_TIME_MS;
            break;

        case EVENT_2B:
        case EVENT_2C:
            current_event = EVENT_2D;
            current_event_time_limit = EVENT_2D_ACTIVE_TIME_MS;
            break;

        case EVENT_3B:
            current_event = EVENT_3C;
            current_event_time_limit = EVENT_3C_ACTIVE_TIME_MS;
            break;

        case EVENT_4B:
            current_event = EVENT_4C;
            current_event_time_limit = EVENT_4C_ACTIVE_TIME_MS;
            break;
        }
        return;
    }


    /* If current event is a yellow light */
    if (is_pre_yellow) {
        switch (current_event) {
        case EVENT_1A:
            if (ct - sensor_times[SENSOR_1] >= ACTIVE_SENSOR_TIME_MS &&
                ct - sensor_times[SENSOR_7] >= ACTIVE_SENSOR_TIME_MS) 
            {
                current_event = EVENT_1C;
                current_event_time_limit = EVENT_1C_ACTIVE_TIME_MS;
            }
            else if (ct - sensor_times[SENSOR_1] >= ACTIVE_SENSOR_TIME_MS) 
            {
                current_event = EVENT_1B;
                current_event_time_limit = EVENT_1B_ACTIVE_TIME_MS;
            }
            break;

            
        case EVENT_2A:
            if (ct - sensor_times[SENSOR_2] >= ACTIVE_SENSOR_TIME_MS &&
                ct - sensor_times[SENSOR_8] >= ACTIVE_SENSOR_TIME_MS) 
            {
                current_event = EVENT_2C;
                current_event_time_limit = EVENT_2C_ACTIVE_TIME_MS;
            }
            else if (ct - sensor_times[SENSOR_2] >= ACTIVE_SENSOR_TIME_MS)
            {
                current_event = EVENT_2B;
                current_event_time_limit = EVENT_2B_ACTIVE_TIME_MS;
            }
            break;


        case EVENT_3A:
            if (ct - sensor_times[SENSOR_3] >= ACTIVE_SENSOR_TIME_MS) 
            {
                current_event = EVENT_3B;
                current_event_time_limit = EVENT_3B_ACTIVE_TIME_MS;
            }
            break;


        case EVENT_4A:
            if (ct - sensor_times[SENSOR_4] >= ACTIVE_SENSOR_TIME_MS) 
            {
                current_event = EVENT_4B;
                current_event_time_limit = EVENT_4B_ACTIVE_TIME_MS;
            }
            break;


        case EVENT_5A:
            if (ct - sensor_times[SENSOR_5] >= ACTIVE_SENSOR_TIME_MS) 
            {
                current_event = EVENT_5B;
                current_event_time_limit = EVENT_5B_ACTIVE_TIME_MS;
            }
            break;

            
        case EVENT_6A:
            if (ct - sensor_times[SENSOR_6] >= ACTIVE_SENSOR_TIME_MS) 
            {
                current_event = EVENT_6B;
                current_event_time_limit = EVENT_6B_ACTIVE_TIME_MS;
            }
            break;
        }
    }
    /* Must be a yellow event */
    else { 
        is_pre_yellow = true;
        current_event = e;
        switch (e) {
        case EVENT_1A:
            current_event_time_limit = EVENT_1A_ACTIVE_TIME_MS;
            break;

        case EVENT_2A:
            current_event_time_limit = EVENT_2A_ACTIVE_TIME_MS;
            break;

        case EVENT_3A:
            current_event_time_limit = EVENT_3A_ACTIVE_TIME_MS;
            break;

        case EVENT_4A:
            current_event_time_limit = EVENT_4A_ACTIVE_TIME_MS;
            break;

        case EVENT_5A:
            current_event_time_limit = EVENT_5A_ACTIVE_TIME_MS;
            break;

        case EVENT_6A:
            current_event_time_limit = EVENT_6A_ACTIVE_TIME_MS;
            break;
        }
    }
    Serial.println("update_current_event::finish");
}


/// @brief queue events considering sensor time conditions
void process_events(void) {
    Serial.println("process_events::start");
    unsigned long current_time = millis();
    bool no_active = true;

    _print_event(current_event);

    for (size_t i=0; i<SENSOR_COUNT-2; ++i) { /// don't include sensor 7 and 8 on iteration.
        Serial.print("process_events::sensor_index_");
        Serial.println(i);


        if (current_time - sensor_times[i] >= PRE_ACTIVE_SENSOR_TIME_MS) {
            // Serial.println("check_sensors::case_ok");
            Serial.println(current_time - sensor_times[i]);
            no_active = false; /// there is an active sensor

            switch (i) {
            case SENSOR_1:
                Serial.println("check_sensors::case_solo_pre_active_sensor_1");
                // if (event_cooldowns[EVENT_1] != 0) { continue; }
                update_current_event(EVENT_1A, current_time, event_cooldowns[EVENT_1]);
                break;

            case SENSOR_2: 
                Serial.println("check_sensors::case_solo_pre_active_sensor_2");
                // if (event_cooldowns[EVENT_2] != 0) { continue; }
                update_current_event(EVENT_2A, current_time, event_cooldowns[EVENT_2]); 
                break;
                
            case SENSOR_3: 
                Serial.println("check_sensors::case_solo_pre_active_sensor_3");
                // if (event_cooldowns[EVENT_3] != 0) { continue; }
                update_current_event(EVENT_3A, current_time, event_cooldowns[EVENT_3]); 
                break;

            case SENSOR_4: 
                Serial.println("check_sensors::case_solo_pre_active_sensor_4");
                // if (event_cooldowns[EVENT_4] != 0) { continue; }
                update_current_event(EVENT_4A, current_time, event_cooldowns[EVENT_4]); 
                break;

            case SENSOR_5: 
                Serial.println("check_sensors::case_solo_pre_active_sensor_5");
                // if (event_cooldowns[EVENT_5] != 0) { Serial.println("---- ON COOLDOWN ----"); continue; }
                update_current_event(EVENT_5A, current_time, event_cooldowns[EVENT_5]); 
                break;

            case SENSOR_6: 
                Serial.println("check_sensors::case_solo_pre_active_sensor_6");
                // if (event_cooldowns[EVENT_6] != 0) { continue; }
                update_current_event(EVENT_6A, current_time, event_cooldowns[EVENT_6]); 
                break;
            }
        }
    }


    /* Go back to default states */
    if (no_active) {
        update_current_event(EVENT_00, current_time, 0);
    }

    Serial.println("process_events::finish");
    reset_sensor_data_times();
}


    

/**
 * Send current event data to other boards inorder to update their
 * own current event.
*/
void broadcast_event(void) {
    Serial.println("broadcast_event");


    RF24NetworkHeader send_b2(board2);
    RF24NetworkHeader send_b3(board3);
    RF24NetworkHeader send_b4(board4);
    RF24NetworkHeader send_b5(board5);
    RF24NetworkHeader send_b6(board6);
    RF24NetworkHeader send_b7(board7);
    RF24NetworkHeader send_b8(board8);
    
    DataPack send_dp;
    
    send_dp.type  = EVENT_T;
    send_dp.event = current_event;
    send_dp.event_active = true;
    send_dp.seq_active = false;
    send_dp.man_active = false;
    

    if (previous_event != current_event) {
        network.write(send_b2, &send_dp, sizeof(DataPack));
        network.write(send_b3, &send_dp, sizeof(DataPack));
        network.write(send_b4, &send_dp, sizeof(DataPack));
        network.write(send_b5, &send_dp, sizeof(DataPack));
        network.write(send_b6, &send_dp, sizeof(DataPack));
        network.write(send_b7, &send_dp, sizeof(DataPack));
        network.write(send_b8, &send_dp, sizeof(DataPack));
    }
}


void decrease_cooldowns(void) {
    Serial.println("decrease_cooldowns::start");
    for (size_t i=0; i<EVENT_COUNT; ++i) {
        Serial.print("event_index: ");
        Serial.print(i);

        Serial.print(" | cooldown: ");
        Serial.println(event_cooldowns[i]);

        if (event_cooldowns[i] > 0) {
            event_cooldowns[i] -= 1;
        }
    }
    Serial.println("decrease_cooldowns::finish");
}


/// @brief run events specific to the master board.
void run_event(void) {
    Serial.println("run_event::start");

    Serial.print("current_event_time_limit: ");
    Serial.println(current_event_time_limit);

    switch (current_event) {

        case EVENT_00: 
            digitalWrite(LTR1_RELAY, HIGH);
            digitalWrite(G1_RELAY, HIGH);
            digitalWrite(R3_RELAY, HIGH);

            is_pre_yellow = false;
            decrease_cooldowns();
            break;


        case EVENT_1A: 
            digitalWrite(LTR1_RELAY, HIGH);
            digitalWrite(Y1_RELAY, HIGH);
            digitalWrite(R3_RELAY, HIGH);
            break;


        case EVENT_1B: 
            if (event_cooldowns[EVENT_1] > 0) { return; }
            digitalWrite(LTR1_RELAY, HIGH);
            digitalWrite(R1_RELAY, HIGH);
            digitalWrite(R3_RELAY, HIGH);


            delay(current_event_time_limit);
            is_pre_yellow = false;
            is_post_yellow = true;
            event_cooldowns[EVENT_1] = EVENT_COOLDOWN;

            force_record_sensor_data_time(SENSOR_1);
            force_record_sensor_data_time(SENSOR_7);

            count_green();

            break;


        case EVENT_1C: 
            if (event_cooldowns[EVENT_1] > 0) { return; }
            digitalWrite(LTR1_RELAY, HIGH);
            digitalWrite(R1_RELAY, HIGH);
            digitalWrite(R3_RELAY, HIGH);


            delay(current_event_time_limit);
            is_pre_yellow = false;
            is_post_yellow = true;
            event_cooldowns[EVENT_1] = EVENT_COOLDOWN;
            
            force_record_sensor_data_time(SENSOR_1);
            force_record_sensor_data_time(SENSOR_7);

            count_green();
            
            break;
        

        case EVENT_1D:
            // TURN ON Y4
            digitalWrite(LTR1_RELAY, HIGH);
            digitalWrite(R1_RELAY, HIGH);
            digitalWrite(R3_RELAY, HIGH);
            delay(current_event_time_limit);
            is_post_yellow = false;
            break;


        case EVENT_2A: 
            digitalWrite(LTR1_RELAY, HIGH);
            digitalWrite(Y1_RELAY, HIGH);
            digitalWrite(Y3_RELAY, HIGH);
            break;


        case EVENT_2B:
            if (event_cooldowns[EVENT_2] > 0) { return; }
            digitalWrite(LTR1_RELAY, HIGH);
            digitalWrite(R1_RELAY, HIGH);
            digitalWrite(G3_RELAY, HIGH);
            

            delay(current_event_time_limit);
            is_pre_yellow = false;
            is_post_yellow = true;
            event_cooldowns[EVENT_2] = EVENT_COOLDOWN;

            force_record_sensor_data_time(SENSOR_1);
            force_record_sensor_data_time(SENSOR_7);

            count_green();

            break;


        case EVENT_2C: 
            if (event_cooldowns[EVENT_2] > 0) { return; }
            digitalWrite(LTR1_RELAY, HIGH);
            digitalWrite(R1_RELAY, HIGH);
            digitalWrite(G3_RELAY, HIGH);


            delay(current_event_time_limit);
            is_pre_yellow = false;
            is_post_yellow = true;
            event_cooldowns[EVENT_2] = EVENT_COOLDOWN;

            force_record_sensor_data_time(SENSOR_1);
            force_record_sensor_data_time(SENSOR_7);
            
            count_green();
            
            break;


        case EVENT_2D:
            digitalWrite(LTR1_RELAY, HIGH);
            digitalWrite(R1_RELAY, HIGH);
            digitalWrite(Y3_RELAY, HIGH);
            delay(current_event_time_limit);
            is_post_yellow = false;
            break;



        case EVENT_3A: 
            /// @note TL1 is not a yellow event because it is
            ///       based on default mode 1 
            digitalWrite(LTY1_RELAY, HIGH);
            digitalWrite(G1_RELAY, HIGH);
            digitalWrite(R3_RELAY, HIGH);
            break;


        case EVENT_3B: 
            if (event_cooldowns[EVENT_3] > 0) { return; }
            digitalWrite(LTG1_RELAY, HIGH);
            digitalWrite(G1_RELAY,  HIGH);
            digitalWrite(R3_RELAY,  HIGH);


            delay(current_event_time_limit);
            is_pre_yellow = false;
            is_post_yellow = true;
            event_cooldowns[EVENT_3] = EVENT_COOLDOWN;

            force_record_sensor_data_time(SENSOR_3);

            count_green();
            
            break;

        
        case EVENT_3C:
            digitalWrite(LTY1_RELAY, HIGH);
            digitalWrite(G1_RELAY,  HIGH);
            digitalWrite(R3_RELAY,  HIGH);
            delay(current_event_time_limit);
            is_post_yellow = false;
            break;


        case EVENT_4A: 
            digitalWrite(LTR1_RELAY, HIGH);
            digitalWrite(Y1_RELAY, HIGH);
            digitalWrite(R3_RELAY, HIGH);
            break;


        case EVENT_4B: 
            if (event_cooldowns[EVENT_4] > 0) { return; }
            digitalWrite(LTR1_RELAY, HIGH);
            digitalWrite(R1_RELAY, HIGH);
            digitalWrite(R3_RELAY, HIGH);


            delay(current_event_time_limit);
            is_pre_yellow = false;
            is_post_yellow = true;
            event_cooldowns[EVENT_4] = EVENT_COOLDOWN;
            
            force_record_sensor_data_time(SENSOR_4);

            count_green();

            break;
        

        case EVENT_4C:
            // TURN ON LT2
            digitalWrite(LTR1_RELAY, HIGH);
            digitalWrite(R1_RELAY, HIGH);
            digitalWrite(R3_RELAY, HIGH);
            delay(current_event_time_limit);
            is_post_yellow = false;
            break;


        case EVENT_5A: 
            digitalWrite(LTR1_RELAY, HIGH);
            digitalWrite(Y1_RELAY, HIGH);
            digitalWrite(R3_RELAY, HIGH);
            break;


        case EVENT_5B: 
            if (event_cooldowns[EVENT_5] > 0) { return; }
            digitalWrite(LTR1_RELAY, HIGH);
            digitalWrite(R1_RELAY, HIGH);
            digitalWrite(R3_RELAY, HIGH);


            delay(current_event_time_limit);
            is_pre_yellow = false;
            event_cooldowns[EVENT_5] = EVENT_COOLDOWN;

            force_record_sensor_data_time(SENSOR_5);

            count_green();

            break;


        case EVENT_6A: 
            digitalWrite(LTR1_RELAY, HIGH);
            digitalWrite(Y1_RELAY, HIGH);
            digitalWrite(R3_RELAY, HIGH);
            break;


        case EVENT_6B:  
            if (event_cooldowns[EVENT_6] > 0) { return; }
            digitalWrite(LTR1_RELAY, HIGH);
            digitalWrite(R1_RELAY, HIGH);
            digitalWrite(R3_RELAY, HIGH);


            delay(current_event_time_limit);
            decrease_cooldowns();
            is_pre_yellow = false;
            event_cooldowns[EVENT_6] = EVENT_COOLDOWN;
            
            force_record_sensor_data_time(SENSOR_6);

            count_green();

            break;
    }
    Serial.println("run_event::finish");
}


void broadcast_default_mode_2(void) {
    Serial.println("broadcast_sequence");


    RF24NetworkHeader send_b2(board2);
    RF24NetworkHeader send_b3(board3);
    RF24NetworkHeader send_b4(board4);
    RF24NetworkHeader send_b5(board5);
    RF24NetworkHeader send_b6(board6);
    RF24NetworkHeader send_b7(board7);
    RF24NetworkHeader send_b8(board8);
    
    DataPack send_dp;
    
    send_dp.type  = SEQ_T;
    send_dp.seq   = current_sequence;
    send_dp.seq_active = true;
    send_dp.event_active = false;
    send_dp.man_active = false;
    

    if (previous_sequence != current_sequence) {
        network.write(send_b2, &send_dp, sizeof(DataPack));
        network.write(send_b3, &send_dp, sizeof(DataPack));
        network.write(send_b4, &send_dp, sizeof(DataPack));
        network.write(send_b5, &send_dp, sizeof(DataPack));
        network.write(send_b6, &send_dp, sizeof(DataPack));
        network.write(send_b7, &send_dp, sizeof(DataPack));
        network.write(send_b8, &send_dp, sizeof(DataPack));
    }
}


/// @brief runs the sequence of default mode 2 for the main board.
void run_default_mode_sequence(void) {
    
    if (seq_run == 1) {
        current_seq_time_last = millis();
    }

    Serial.println(millis() - current_seq_time_last);

    switch (current_sequence) { 
    case SEQ_01A:
        if (current_seq_time_limit != SEQ_01A_ACTIVE_TIME) {
            current_seq_time_limit = SEQ_01A_ACTIVE_TIME; 
        }
        digitalWrite(LTR1_RELAY, HIGH);
        digitalWrite(G1_RELAY, HIGH);
        digitalWrite(R3_RELAY, HIGH);
        
        
        if (millis() - current_seq_time_last >= current_seq_time_limit) {
            current_seq_time_last = millis();
            previous_sequence = current_sequence;
            current_sequence = SEQ_01B;
            turn_off_relays(1);
        }
        break;


    case SEQ_01B:
        if (current_seq_time_limit != SEQ_01B_ACTIVE_TIME) {
            current_seq_time_limit = SEQ_01B_ACTIVE_TIME; 
        }
        digitalWrite(LTR1_RELAY, HIGH);
        digitalWrite(G1_RELAY, HIGH);
        digitalWrite(R3_RELAY, HIGH);
        
        
        if (millis() - current_seq_time_last >= current_seq_time_limit) {
            current_seq_time_last = millis();
            previous_sequence = current_sequence;
            current_sequence = SEQ_02A;
            turn_off_relays(1);
        }
        break;

    
    case SEQ_02A:
        if (current_seq_time_limit != SEQ_02A_ACTIVE_TIME) {
            current_seq_time_limit = SEQ_02A_ACTIVE_TIME; 
        }

        digitalWrite(LTG1_RELAY, HIGH);
        digitalWrite(G1_RELAY, HIGH);
        digitalWrite(R3_RELAY, HIGH);

        if (millis() - current_seq_time_last >= current_seq_time_limit) {
            current_seq_time_last = millis();
            previous_sequence = current_sequence;
            current_sequence = SEQ_02B;
            turn_off_relays(1);
        }
        break;

    
    case SEQ_02B:
        if (current_seq_time_limit != SEQ_02B_ACTIVE_TIME) {
            current_seq_time_limit = SEQ_02B_ACTIVE_TIME; 
        }

        digitalWrite(LTY1_RELAY, HIGH);
        digitalWrite(Y1_RELAY, HIGH);
        digitalWrite(R3_RELAY, HIGH);

        if (millis() - current_seq_time_last >= current_seq_time_limit) {
            current_seq_time_last = millis();
            previous_sequence = current_sequence;
            current_sequence = SEQ_03A;
            turn_off_relays(1);
        }
        break;
    
    
    case SEQ_03A:
        if (current_seq_time_limit != SEQ_03A_ACTIVE_TIME) {
            current_seq_time_limit = SEQ_03A_ACTIVE_TIME;
        }

        digitalWrite(LTR1_RELAY, HIGH);
        digitalWrite(R1_RELAY, HIGH);
        digitalWrite(R3_RELAY, HIGH);
        
        if (millis() - current_seq_time_last >= current_seq_time_limit) {
            current_seq_time_last = millis();
            previous_sequence = current_sequence;
            current_sequence = SEQ_03B;
            turn_off_relays(1);
        }
        break;
    
    
    case SEQ_03B:
        if (current_seq_time_limit != SEQ_03B_ACTIVE_TIME) {
            current_seq_time_limit = SEQ_03B_ACTIVE_TIME;
        }

        digitalWrite(LTR1_RELAY, HIGH);
        digitalWrite(R1_RELAY, HIGH);
        digitalWrite(R3_RELAY, HIGH);
        
        if (millis() - current_seq_time_last >= current_seq_time_limit) {
            current_seq_time_last = millis();
            previous_sequence = current_sequence;
            current_sequence = SEQ_04A;
            turn_off_relays(1);
        }
        break;
    
    
    case SEQ_04A:
        if (current_seq_time_limit != SEQ_04A_ACTIVE_TIME) {
            current_seq_time_limit = SEQ_04A_ACTIVE_TIME; 
        }

        digitalWrite(LTR1_RELAY, HIGH);
        digitalWrite(R1_RELAY, HIGH);
        digitalWrite(G3_RELAY, HIGH);
        
        if (millis() - current_seq_time_last >= current_seq_time_limit) {
            current_seq_time_last = millis();
            previous_sequence = current_sequence;
            current_sequence = SEQ_04B;
            turn_off_relays(1);
        }
        break;
    
    
    case SEQ_04B:
        if (current_seq_time_limit != SEQ_04B_ACTIVE_TIME) {
            current_seq_time_limit = SEQ_04B_ACTIVE_TIME; 
        }

        digitalWrite(LTR1_RELAY, HIGH);
        digitalWrite(R1_RELAY, HIGH);
        /// @todo blink Y3 for 3 seconds
        digitalWrite(Y3_RELAY, HIGH);
        
        if (millis() - current_seq_time_last >= current_seq_time_limit) {
            current_seq_time_last = millis();
            previous_sequence = current_sequence;
            current_sequence = SEQ_05A;
            turn_off_relays(1);
        }
        break;
    
    
    case SEQ_05A:
        if (current_seq_time_limit != SEQ_05A_ACTIVE_TIME) {
            current_seq_time_limit = SEQ_05A_ACTIVE_TIME; 
        }

        digitalWrite(LTR1_RELAY, HIGH);
        digitalWrite(R1_RELAY, HIGH);
        digitalWrite(R3_RELAY, HIGH);
        
        if (millis() - current_seq_time_last >= current_seq_time_limit) {
            current_seq_time_last = millis();
            previous_sequence = current_sequence;
            current_sequence = SEQ_05B;
            turn_off_relays(1);
        }
        break;
    
    
    case SEQ_05B:
        if (current_seq_time_limit != SEQ_05B_ACTIVE_TIME) {
            current_seq_time_limit = SEQ_05B_ACTIVE_TIME; 
        }

        digitalWrite(LTR1_RELAY, HIGH);
        digitalWrite(R1_RELAY, HIGH);
        digitalWrite(G3_RELAY, HIGH);
        
        if (millis() - current_seq_time_last >= current_seq_time_limit) {
            current_seq_time_last = millis();
            previous_sequence = current_sequence;
            current_sequence = SEQ_01A;
            turn_off_relays(1);
        }
        break;
    }
}


/**
 * Reset the green light grant counter. This
 * routine is expected to run every hour (when
 * the hour variable increments.)
*/
void reset_counter(byte *h) {
    switch (*h) {
        case 7 : 
        case 8 :
        case 11:
        case 12:
        case 16:
        case 17:
            break;
        
        default:
                green_light_grant_counter = 0;
    }

}


void set_grant_limit(byte *h) {
    switch (*h) {
        case  6:
            green_light_grant_limit = GREEN_GRANT_06T07;
            break;
        case  9:
            green_light_grant_limit = GREEN_GRANT_09T10;
            break;
        case 10:
            green_light_grant_limit = GREEN_GRANT_10T11;
            break;
        case 13:
            green_light_grant_limit = GREEN_GRANT_13T14;
            break;
        case 14:
            green_light_grant_limit = GREEN_GRANT_14T15;
            break;
        case 15:
            green_light_grant_limit = GREEN_GRANT_15T16;
            break;
    }
}


void check_time_for_mode(byte *h) {

    switch (*h) {
        case  7: 
        case  8:
        case 11:
        case 12:
        case 16:
        case 17:
            default_mode_two = false;
            break;
        
        case  6:
        case  9:
        case 10:
        case 13:
        case 14:
        case 15:
            default_mode_two = true;
            break;
    }
}


void broadcast_manual_mode(void) {
    RF24NetworkHeader send_b2(board2);
    RF24NetworkHeader send_b3(board3);
    RF24NetworkHeader send_b4(board4);
    RF24NetworkHeader send_b5(board5);
    RF24NetworkHeader send_b6(board6);
    RF24NetworkHeader send_b7(board7);
    RF24NetworkHeader send_b8(board8);
    
    DataPack send_dp;
    
    send_dp.type  = MAN_T;
    send_dp.man = current_manual;
    send_dp.seq_active = false;
    send_dp.event_active = false;
    send_dp.man_active = true;
    send_dp.blink = blinker;

    if (previous_manual != current_manual) {
        network.write(send_b2, &send_dp, sizeof(DataPack));
        network.write(send_b3, &send_dp, sizeof(DataPack));
        network.write(send_b4, &send_dp, sizeof(DataPack));
        network.write(send_b5, &send_dp, sizeof(DataPack));
        network.write(send_b6, &send_dp, sizeof(DataPack));
        network.write(send_b7, &send_dp, sizeof(DataPack));
        network.write(send_b8, &send_dp, sizeof(DataPack));
    }
}


void run_manual_blink_mode_scenarios(enum Events_Man event) {
    enum Events_Man ce = event;
    enum Events_Man pe = event;

    unsigned long prev = millis();
    unsigned long curr = millis();
    unsigned long const interval = 3000;
    short blink = 0;


    while (curr - prev >= interval) {

        if (curr - prev >= BLINK_INTERVAL) {
            blink += 1;
        }

        // broadcast
        RF24NetworkHeader send_b2(board2);
        RF24NetworkHeader send_b3(board3);
        RF24NetworkHeader send_b4(board4);
        RF24NetworkHeader send_b5(board5);
        RF24NetworkHeader send_b6(board6);
        RF24NetworkHeader send_b7(board7);
        RF24NetworkHeader send_b8(board8);
        
        DataPack send_dp;
        
        send_dp.type  = MAN_T;
        send_dp.man = ce;
        send_dp.seq_active = false;
        send_dp.event_active = false;
        send_dp.man_active = true;
        send_dp.blink = blink;

        if (pe != ce) {
            network.write(send_b2, &send_dp, sizeof(DataPack));
            network.write(send_b3, &send_dp, sizeof(DataPack));
            network.write(send_b4, &send_dp, sizeof(DataPack));
            network.write(send_b5, &send_dp, sizeof(DataPack));
            network.write(send_b6, &send_dp, sizeof(DataPack));
            network.write(send_b7, &send_dp, sizeof(DataPack));
            network.write(send_b8, &send_dp, sizeof(DataPack));

            digitalWrite(LTR1_RELAY, LOW);
            digitalWrite(LTY1_RELAY, LOW);
            digitalWrite(LTG1_RELAY, LOW);
            digitalWrite(R1_RELAY , LOW);
            digitalWrite(Y1_RELAY , LOW);
            digitalWrite(G1_RELAY , LOW);
            digitalWrite(R3_RELAY , LOW);
            digitalWrite(Y3_RELAY , LOW);
            digitalWrite(G3_RELAY , LOW);
        }

        switch (event) {
        case SCENE_M1B:
            if (blink % 2 == 0) {
                digitalWrite(LTR1_RELAY, HIGH);
                digitalWrite(Y1_RELAY, LOW);
                digitalWrite(R3_RELAY, HIGH);
            }
            else {
                digitalWrite(LTR1_RELAY, HIGH);
                digitalWrite(Y1_RELAY, HIGH);
                digitalWrite(R3_RELAY, HIGH);
            }
            break;


        case SCENE_M2B:
            if (blink % 2 == 0) {
                digitalWrite(LTY1_RELAY, LOW);
                digitalWrite(Y1_RELAY, LOW);
                digitalWrite(R3_RELAY, HIGH);
            }
            else {
                digitalWrite(LTY1_RELAY, HIGH);
                digitalWrite(Y1_RELAY, HIGH);
                digitalWrite(R3_RELAY, HIGH);
            }
            break;


        case SCENE_M3B:
            if (blink % 2 == 0) {
                digitalWrite(LTR1_RELAY, HIGH);
                digitalWrite(R1_RELAY, HIGH);
                digitalWrite(R3_RELAY, HIGH);
            }
            else {
                digitalWrite(LTR1_RELAY, HIGH);
                digitalWrite(R1_RELAY, HIGH);
                digitalWrite(R3_RELAY, HIGH);
            }
            break;


        case SCENE_M4B:
            if (blink % 2 == 0) {
                digitalWrite(LTR1_RELAY, HIGH);
                digitalWrite(R1_RELAY, HIGH);
                digitalWrite(Y3_RELAY, LOW);
            }
            else {
                digitalWrite(LTR1_RELAY, HIGH);
                digitalWrite(R1_RELAY, HIGH);
                digitalWrite(Y3_RELAY, HIGH);
            }
            break;


        case SCENE_M5B:
            if (blink % 2 == 0) {
                digitalWrite(LTR1_RELAY, HIGH);
                digitalWrite(R1_RELAY, HIGH);
                digitalWrite(R3_RELAY, HIGH);
            }
            else {
                digitalWrite(LTR1_RELAY, HIGH);
                digitalWrite(R1_RELAY, HIGH);
                digitalWrite(R3_RELAY, HIGH);
            }
            break;
        }

        curr = millis();
    }

    is_switching = false;
}


void run_manual_mode_scenarios(void) {

    switch (current_manual) {
    case SCENE_M1A:
        digitalWrite(LTR1_RELAY, HIGH);
        digitalWrite(G1_RELAY, HIGH);
        digitalWrite(R3_RELAY, HIGH);
        if (is_switching)
            run_manual_blink_mode_scenarios(SCENE_M1B);
        break;


    case SCENE_M2A:
        digitalWrite(LTG1_RELAY, HIGH);
        digitalWrite(G1_RELAY, HIGH);
        digitalWrite(R3_RELAY, HIGH);
        if (is_switching)
            run_manual_blink_mode_scenarios(SCENE_M2B);
        break;


    case SCENE_M3A:
        digitalWrite(LTR1_RELAY, HIGH);
        digitalWrite(R1_RELAY, HIGH);
        digitalWrite(R3_RELAY, HIGH);
        if (is_switching)
            run_manual_blink_mode_scenarios(SCENE_M3B);
        break;


    case SCENE_M4A:
        digitalWrite(LTR1_RELAY, HIGH);
        digitalWrite(R1_RELAY, HIGH);
        digitalWrite(G3_RELAY, HIGH);
        if (is_switching)
            run_manual_blink_mode_scenarios(SCENE_M4B);
        break;


    case SCENE_M5A:
        digitalWrite(LTR1_RELAY, HIGH);
        digitalWrite(R1_RELAY, HIGH);
        digitalWrite(R3_RELAY, HIGH);
        if (is_switching)
            run_manual_blink_mode_scenarios(SCENE_M5B);
        break;


    case SCENE_M6A: // Default Mode
        if (millis() - blink_time_last >= BLINK_INTERVAL) {
            blinker += 1;
        }

        if (blinker % 2 == 0) {
            digitalWrite(LTR1_RELAY, LOW);
            digitalWrite(Y1_RELAY, LOW);
            digitalWrite(Y3_RELAY, LOW);
        }
        else {
            digitalWrite(LTR1_RELAY, HIGH);
            digitalWrite(Y1_RELAY, HIGH);
            digitalWrite(Y3_RELAY, HIGH);
        }
        break;
    }
}


/// @brief Starting execution routine
void setup(void) {
    SPI.begin();
    Serial.begin(115200);
    
    h24 = false;
    Wire.begin();

    // set Clock to 24-hour format
    rtc.setClockMode(false); 

    current_event  = EVENT_00;
    previous_event = EVENT_00;
    current_sequence  = SEQ_01A;
    previous_sequence = SEQ_01A;
    is_pre_yellow = false;
    is_post_yellow = false;
    default_mode_two = false;
    green_light_grant_counter = 0;
    green_light_grant_limit = 0;
    
    prev_hour = 0;
    hour = rtc.getHour(h24, hPM);
    next_hour = hour + 1;

    pinMode(LTR1_RELAY, OUTPUT);
    pinMode(LTY1_RELAY, OUTPUT);
    pinMode(LTG1_RELAY, OUTPUT);
    pinMode(R1_RELAY, OUTPUT);
    pinMode(Y1_RELAY, OUTPUT);
    pinMode(G1_RELAY, OUTPUT);
    pinMode(R3_RELAY, OUTPUT);
    pinMode(Y3_RELAY, OUTPUT);
    pinMode(G3_RELAY, OUTPUT);

    // ========= NRF24L01 SETUP ========= //
    radio.begin();
    radio.setPayloadSize(sizeof(DataPack));
    radio.setDataRate(RF24_2MBPS);
    radio.setPALevel(RF24_PA_MIN);

    network.begin(90, current_board);
}   


void loop(void) {
    Serial.println("+++++loop::start+++++");
    
    // This if block is expected to run every hour
    // in order to reset the green light grant counter.
    if (prev_hour != hour) {
        prev_hour = hour;
        reset_counter(&hour);
    }
    
    hour = rtc.getHour(h24, hPM);

    network.update();
    process_network_data();
    
    check_time_for_mode(&hour);
    set_grant_limit(&hour);


    /// MANUAL MODE
    if (is_manual) {
        if (is_bypass) 
            goto invalid_manual;

        broadcast_manual_mode();
        run_manual_mode_scenarios();

        goto manual;
    }
    else {
        previous_manual = SCENE_M6A;
        current_manual  = SCENE_M6A;
    }


    invalid_manual:

    if ((green_light_grant_counter >= green_light_grant_limit) && default_mode_two && !is_bypass) {
        seq_run += 1;

        broadcast_default_mode_2();
        run_default_mode_sequence();

    } else {
        seq_run = 0;
        process_events();

        // run sequence
        broadcast_event();
        turn_off_relays(0);
        run_event();
    }

    manual:

    if (next_hour <= hour)
        next_hour = hour+1;

    Serial.println("-----loop::ended-----");
}