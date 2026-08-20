#include <Arduino.h>
#include <SPI.h>
#include "RF24.h"
#include "RF24Network.h"


// ========= BOARD CONFIG ========= //
#define BOARD_A4 1


#include "boards.h"
#include "datapack.h"
#include "events.h"
#include "sensor.h"


// ========= VARIABLES ========= //
RF24 radio(NRF24L01_CE, NRF24L01_CSN);
RF24Network network(radio);

enum Events previous_event, current_event;
enum Events_Seq previous_seq, current_seq;
enum Events_Man previous_manual, current_manual;
bool event_active, seq_active, man_active;

short blink = 0;

// previous sensor_states
#if BOARD_A3 
    enum SensorState s5_previous_state = INACTIVE;


#elif BOARD_A4
    enum SensorState s6_previous_state = INACTIVE;


#elif BOARD_A5 
    enum SensorState s1_previous_state = INACTIVE;
    enum SensorState s3_previous_state = INACTIVE;
    enum SensorState s7_received_state = INACTIVE;


#elif BOARD_A6
    enum SensorState s2_previous_state = INACTIVE;
    enum SensorState s4_previous_state = INACTIVE;
    enum SensorState s8_received_state = INACTIVE;


#elif BOARD_A7
    enum SensorState s7_previous_state = INACTIVE;


#elif BOARD_A8
    enum SensorState s8_previous_state = INACTIVE;
#endif


#if !(BOARD_A2)


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


/**
 * Check if sensor is not reaching its maximum distance, this would
 * mean that there is an object in the path thus it is assumed to be 
 * a vehicle. Send sensor state (1 or 0) to the master board every
 * second.
*/
void detect_vehicle(void) {
    Serial.println("detect_vehicle::start");

    #if   BOARD_A7
        RF24NetworkHeader send_h(board5); // target data to arduino 5
    #elif BOARD_A8
        RF24NetworkHeader send_h(board6); // target data to arduino 6
    #else 
        RF24NetworkHeader send_h(board_master); // target data to master board
    #endif

    DataPack send_dp;
    send_dp.type = SENSOR_T;
    for (size_t i=0; i<SENSOR_COUNT;++i) {
        send_dp.ss[i] = INACTIVE;
    }


    #if   BOARD_A3
        unsigned long distance = sonar.ping_cm();

        if (distance <= SCAN_DISTANCE_CM) {
            send_dp.ss[SENSOR_5] = ACTIVE;
        }

    
        network.write(send_h, &send_dp, sizeof(DataPack));
        s5_previous_state = send_dp.ss[SENSOR_5];
        


    #elif BOARD_A4
        unsigned long distance = sonar.ping_cm();

        if (distance <= SCAN_DISTANCE_CM) {
            send_dp.ss[SENSOR_6] = ACTIVE;
        }

        if (s6_previous_state != send_dp.ss[SENSOR_6]) {
            network.write(send_h, &send_dp, sizeof(DataPack));
            s6_previous_state = send_dp.ss[SENSOR_6];
        }


    #elif BOARD_A5
        unsigned long s1_distance = sonar1.ping_cm();
        unsigned long s3_distance = sonar2.ping_cm();

        if (s1_distance <= SCAN_DISTANCE_CM) {
            send_dp.ss[SENSOR_1] = ACTIVE;
        }

        if (s3_distance <= SCAN_DISTANCE_CM) {
            send_dp.ss[SENSOR_3] = ACTIVE;
        }

        send_dp.ss[SENSOR_7] = s7_received_state;
    
        if (s1_previous_state != send_dp.ss[SENSOR_1]) {
            network.write(send_h, &send_dp, sizeof(DataPack));
            s1_previous_state = send_dp.ss[SENSOR_1];
        } 
        
        if (s3_previous_state != send_dp.ss[SENSOR_3]) {
            network.write(send_h, &send_dp, sizeof(DataPack));
            s3_previous_state = send_dp.ss[SENSOR_3];
        }
    

    #elif BOARD_A6
        unsigned long s2_distance = sonar1.ping_cm();
        unsigned long s4_distance = sonar2.ping_cm();

        if (s2_distance <= SCAN_DISTANCE_CM) {
            send_dp.ss[SENSOR_2] = ACTIVE;
        }

        if (s4_distance <= SCAN_DISTANCE_CM) {
            send_dp.ss[SENSOR_4] = ACTIVE;
        }

        send_dp.ss[SENSOR_8] = s8_received_state;
    
        if (s2_previous_state != send_dp.ss[SENSOR_2]) {
            network.write(send_h, &send_dp, sizeof(DataPack));
            s2_previous_state = send_dp.ss[SENSOR_2];
        }

        if (s4_previous_state != send_dp.ss[SENSOR_4]) {   
            network.write(send_h, &send_dp, sizeof(DataPack));
            s4_previous_state = send_dp.ss[SENSOR_4];
        }


    #elif BOARD_A7
        unsigned long distance = sonar.ping_cm();

        if (distance <= SCAN_DISTANCE_CM) {
            send_dp.ss[SENSOR_7] = ACTIVE;
        }

        if (s7_previous_state != send_dp.ss[SENSOR_7]) {
            network.write(send_h, &send_dp, sizeof(DataPack));
            s7_previous_state = send_dp.ss[SENSOR_7];
        }


    #elif BOARD_A8
        unsigned long distance = sonar.ping_cm();

        if (distance <= SCAN_DISTANCE_CM) {
            send_dp.ss[SENSOR_8] = ACTIVE;
        }

        if (s8_previous_state != send_dp.ss[SENSOR_8]) {
            network.write(send_h, &send_dp, sizeof(DataPack));
            s8_previous_state = send_dp.ss[SENSOR_8];
        }
    #endif
}
#endif


#if BOARD_A5 || BOARD_A6
void process_lesser_slave_sensor_data(RF24NetworkHeader &recv_h, DataPack *recv_dp) {
    Serial.println("process_lesser_slave_sensor_data::start");

    if (recv_dp->type == SENSOR_T) {
        Serial.println("process_lesser_slave_sensor_data::receive_type_ok");
        #if    BOARD_A5
            if (recv_h.from_node == board7) {
                Serial.println("process_lesser_slave_sensor_data::receive_src_ok");
                s7_received_state = recv_dp->ss[SENSOR_7];
            }
        #elif BOARD_A6
            if (recv_h.from_node == board8) { 
                Serial.println("process_lesser_slave_sensor_data::receive_src_ok");
                s8_received_state = recv_dp->ss[SENSOR_8];
            }
        #endif
    }
    Serial.println("process_lesser_slave_sensor_data::finish");
}
#endif


#if BOARD_A2 || BOARD_A3 || BOARD_A4

void turn_off_relay(void) {
    if ((previous_event == current_event) && event_active)
        return;

    if ((previous_seq == current_seq) && seq_active) 
        return;

    if ((previous_manual == current_manual) && man_active)
        return;

    #if   BOARD_A2

        digitalWrite(LTR2_RELAY, LOW);
        digitalWrite(LTY2_RELAY, LOW);
        digitalWrite(LTG2_RELAY, LOW);
        digitalWrite(R2_RELAY, LOW);
        digitalWrite(Y2_RELAY, LOW);
        digitalWrite(G2_RELAY, LOW);
        digitalWrite(R4_RELAY, LOW);
        digitalWrite(Y4_RELAY, LOW);
        digitalWrite(G4_RELAY, LOW);

    #elif BOARD_A3
    
        digitalWrite(PR1_RELAY, LOW);
        digitalWrite(PG1_RELAY, LOW);
    
    
    #elif BOARD_A4
    
        digitalWrite(PR2_RELAY, LOW);
        digitalWrite(PG2_RELAY, LOW);
    
    #endif
    
}

#endif

void process_events(enum Events *e, bool *ea) {
    current_event = *e;

    if (event_active != *ea) {
        event_active = *ea;
    }
}


void process_sequence(enum Events_Seq *s, bool *sa) {
    current_seq = *s;

    if (seq_active != *sa) {
        seq_active = *sa;
    }
}


void process_manual(enum Events_Man *m, bool *ma) {
    current_manual = *m;

    if (man_active != *ma) {
        man_active = *ma;
    }
}


void observe_events(void) {
    RF24NetworkHeader recv_h;
    DataPack recv_dp;

    previous_event = current_event;
    previous_seq = current_seq;
    previous_manual = current_manual;

    network.update();
    while (network.available()) {
        Serial.println("observe_events::network_available");
        network.read(recv_h, &recv_dp, sizeof(DataPack));
        
        #if BOARD_A5 || BOARD_A6
            process_lesser_slave_sensor_data(recv_h, &recv_dp);
        #endif

        if (recv_dp.type == EVENT_T) {
            current_event = recv_dp.event;
            event_active = recv_dp.event_active;
            seq_active = recv_dp.seq_active;
            man_active = recv_dp.man_active;
        }
        else if(recv_dp.type == SEQ_T) {
            current_seq = recv_dp.seq;
            event_active = recv_dp.event_active;
            seq_active = recv_dp.seq_active;
            man_active = recv_dp.man_active;
        }
        else if (recv_dp.type == MAN_T) {
            current_manual = recv_dp.man;
            event_active = recv_dp.event_active;
            seq_active = recv_dp.seq_active;
            man_active = recv_dp.man_active;
            blink = recv_dp.blink;
        }
    }
}


#if BOARD_A2 || BOARD_A3 || BOARD_A4

/// @brief run current event received from master board. 
void run_event(void) {
    turn_off_relay();

    // Serial.println("run_event::start");
    Serial.print("current_event ");
    Serial.println(current_event);
    Serial.println();
    
    Serial.print("current_seq ");
    Serial.println(current_seq);
    Serial.println();
    
    Serial.print("current_manual ");
    Serial.println(current_manual);
    Serial.println();

    Serial.print("event_active ");
    Serial.println(event_active);
    Serial.println();


    Serial.print("sequence_active ");
    Serial.println(seq_active);
    Serial.println();

    Serial.print("manual_active ");
    Serial.println(man_active);
    Serial.println();
    

    if (event_active) {
        switch (current_event) {
        case EVENT_00: 
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(G2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3

                digitalWrite(PG1_RELAY, LOW);
                digitalWrite(PR1_RELAY, HIGH);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case EVENT_1A: 
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(Y2_RELAY, HIGH);
                digitalWrite(Y4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PG1_RELAY, LOW);
                digitalWrite(PR1_RELAY, HIGH);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case EVENT_1B: 
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(R2_RELAY, HIGH);
                digitalWrite(G4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PG1_RELAY, LOW);
                digitalWrite(PR1_RELAY, HIGH);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case EVENT_1C: 
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(R2_RELAY, HIGH); 
                digitalWrite(G4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PG1_RELAY, LOW);
                digitalWrite(PR1_RELAY, HIGH);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;
        

        case EVENT_1D:
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(R2_RELAY, HIGH); 
                digitalWrite(Y4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PG1_RELAY, LOW);
                digitalWrite(PR1_RELAY, HIGH);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case EVENT_2A: 
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(Y2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PG1_RELAY, LOW);
                digitalWrite(PR1_RELAY, HIGH);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case EVENT_2B: 
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(R2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PG1_RELAY, LOW);
                digitalWrite(PR1_RELAY, HIGH);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case EVENT_2C: 
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(R2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PG1_RELAY, LOW);
                digitalWrite(PR1_RELAY, HIGH);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;
        

        case EVENT_2D:
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(R2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PG1_RELAY, LOW);
                digitalWrite(PR1_RELAY, HIGH);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;



        case EVENT_3A: 
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(Y2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PG1_RELAY, LOW);
                digitalWrite(PR1_RELAY, HIGH);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case EVENT_3B: 
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(R2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PG1_RELAY, LOW);
                digitalWrite(PR1_RELAY, HIGH);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case EVENT_3C:  
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(R2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PG1_RELAY, LOW);
                digitalWrite(PR1_RELAY, HIGH);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case EVENT_4A: 
            #if   BOARD_A2
                /// @note TL2 is not a yellow event because it is 
                ///       based on default mode 1 
                digitalWrite(LTY2_RELAY, HIGH);
                digitalWrite(G2_RELAY, HIGH);
                digitalWrite(Y4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PG1_RELAY, LOW);
                digitalWrite(PR1_RELAY, HIGH);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case EVENT_4B: 
            #if   BOARD_A2
                digitalWrite(LTG2_RELAY, HIGH);
                digitalWrite(G2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PG1_RELAY, LOW);
                digitalWrite(PR1_RELAY, HIGH);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;
        

        case EVENT_4C: 
            #if   BOARD_A2
                digitalWrite(LTY2_RELAY, HIGH);
                digitalWrite(G2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PG1_RELAY, LOW);
                digitalWrite(PR1_RELAY, HIGH);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case EVENT_5A: 
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(Y2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PG1_RELAY, LOW);
                digitalWrite(PR1_RELAY, HIGH);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case EVENT_5B: 
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(R2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PR1_RELAY, LOW);
                digitalWrite(PG1_RELAY, HIGH);
            #elif BOARD_A4
                digitalWrite(PG2_RELAY, HIGH);
            #endif
            break;


        case EVENT_6A: 
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(Y2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PR1_RELAY, LOW);
                digitalWrite(PR1_RELAY, HIGH);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case EVENT_6B: 
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(R2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PR1_RELAY, LOW);
                digitalWrite(PG1_RELAY, HIGH);
            #elif BOARD_A4
                digitalWrite(PG2_RELAY, HIGH);
            #endif
            break;
        }
    }


    else if (seq_active) {
        switch (current_seq) {
        case SEQ_01A:
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(G2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PR1_RELAY, HIGH);
                digitalWrite(PG1_RELAY, LOW);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case SEQ_01B:
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(Y2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PR1_RELAY, HIGH);
                digitalWrite(PG1_RELAY, LOW);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case SEQ_02A:
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(R2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PR1_RELAY, HIGH);
                digitalWrite(PG1_RELAY, LOW);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case SEQ_02B:
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(R2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PR1_RELAY, HIGH);
                digitalWrite(PG1_RELAY, LOW);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case SEQ_03A:
            #if   BOARD_A2
                digitalWrite(LTG2_RELAY, HIGH);
                digitalWrite(G2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PR1_RELAY, HIGH);
                digitalWrite(PG1_RELAY, LOW);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case SEQ_03B:
            #if   BOARD_A2
                digitalWrite(LTY2_RELAY, HIGH);
                digitalWrite(Y2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PR1_RELAY, HIGH);
                digitalWrite(PG1_RELAY, LOW);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case SEQ_04A:
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(R2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PR1_RELAY, HIGH);
                digitalWrite(PG1_RELAY, LOW);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case SEQ_04B:
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(R2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PR1_RELAY, HIGH);
                digitalWrite(PG1_RELAY, LOW);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case SEQ_05A:
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(R2_RELAY, HIGH);
                digitalWrite(G4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PG1_RELAY, HIGH);
                digitalWrite(PR1_RELAY, LOW);
            #elif BOARD_A4
                digitalWrite(PG2_RELAY, HIGH);
            #endif
            break;


        case SEQ_05B:
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(R2_RELAY, HIGH);
                digitalWrite(Y4_RELAY, HIGH);
            #elif BOARD_A3
                /// @todo blink PG1 and PG2 for 3 seconds
                digitalWrite(PG1_RELAY, HIGH);
                digitalWrite(PR1_RELAY, LOW);
            #elif BOARD_A4
                digitalWrite(PG2_RELAY, HIGH);
            #endif
            break;
        }

    }


    else if (man_active) {
        switch (current_manual) {
        case SCENE_M1A:
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(G2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PR1_RELAY, HIGH);
                digitalWrite(PG1_RELAY, LOW);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case SCENE_M1B:
            if (blink % 2 == 0) {
                #if   BOARD_A2
                    digitalWrite(LTR2_RELAY, HIGH);
                    digitalWrite(Y2_RELAY, LOW);
                    digitalWrite(R4_RELAY, HIGH);
                #elif BOARD_A3
                    digitalWrite(PR1_RELAY, HIGH);
                    digitalWrite(PG1_RELAY, LOW);
                #elif BOARD_A4
                    digitalWrite(PR2_RELAY, HIGH);
                #endif
            }
            else {
                #if   BOARD_A2
                    digitalWrite(LTR2_RELAY, HIGH);
                    digitalWrite(Y2_RELAY, HIGH);
                    digitalWrite(R4_RELAY, HIGH);
                #elif BOARD_A3
                    digitalWrite(PR1_RELAY, HIGH);
                    digitalWrite(PG1_RELAY, LOW);
                #elif BOARD_A4
                    digitalWrite(PR2_RELAY, HIGH);
                #endif
            }
            break;


        case SCENE_M2A:
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(R2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PR1_RELAY, HIGH);
                digitalWrite(PG1_RELAY, LOW);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case SCENE_M2B:
            if (blink % 2 == 0) {
                #if   BOARD_A2
                    digitalWrite(LTR2_RELAY, HIGH);
                    digitalWrite(R2_RELAY, HIGH);
                    digitalWrite(R4_RELAY, HIGH);
                #elif BOARD_A3
                    digitalWrite(PR1_RELAY, HIGH);
                    digitalWrite(PG1_RELAY, LOW);
                #elif BOARD_A4
                    digitalWrite(PR2_RELAY, HIGH);
                #endif
            }
            else {
                #if   BOARD_A2
                    digitalWrite(LTR2_RELAY, HIGH);
                    digitalWrite(R2_RELAY, HIGH);
                    digitalWrite(R4_RELAY, HIGH);
                #elif BOARD_A3
                    digitalWrite(PR1_RELAY, HIGH);
                    digitalWrite(PG1_RELAY, LOW);
                #elif BOARD_A4
                    digitalWrite(PR2_RELAY, HIGH);
                #endif
            }
            break;


        case SCENE_M3A:
            #if   BOARD_A2
                digitalWrite(LTG2_RELAY, HIGH);
                digitalWrite(G2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PR1_RELAY, HIGH);
                digitalWrite(PG1_RELAY, LOW);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case SCENE_M3B:
            if (blink % 2 == 0) {
                #if   BOARD_A2
                    digitalWrite(LTY2_RELAY, LOW);
                    digitalWrite(Y2_RELAY, LOW);
                    digitalWrite(R4_RELAY, HIGH);
                #elif BOARD_A3
                    digitalWrite(PR1_RELAY, HIGH);
                    digitalWrite(PG1_RELAY, LOW);
                #elif BOARD_A4
                    digitalWrite(PR2_RELAY, HIGH);
                #endif
            }
            else {
                #if   BOARD_A2
                    digitalWrite(LTY2_RELAY, HIGH);
                    digitalWrite(Y2_RELAY, HIGH);
                    digitalWrite(R4_RELAY, HIGH);
                #elif BOARD_A3
                    digitalWrite(PR1_RELAY, HIGH);
                    digitalWrite(PG1_RELAY, LOW);
                #elif BOARD_A4
                    digitalWrite(PR2_RELAY, HIGH);
                #endif
            }
            break;
            


        case SCENE_M4A:
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(R2_RELAY, HIGH);
                digitalWrite(R4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PR1_RELAY, HIGH);
                digitalWrite(PG1_RELAY, LOW);
            #elif BOARD_A4
                digitalWrite(PR2_RELAY, HIGH);
            #endif
            break;


        case SCENE_M4B:
            if (blink % 2 == 0) {
                #if   BOARD_A2
                    digitalWrite(LTR2_RELAY, HIGH);
                    digitalWrite(R2_RELAY, HIGH);
                    digitalWrite(R4_RELAY, HIGH);
                #elif BOARD_A3
                    digitalWrite(PR1_RELAY, HIGH);
                    digitalWrite(PG1_RELAY, LOW);
                #elif BOARD_A4
                    digitalWrite(PR2_RELAY, HIGH);
                #endif
            }
            else {
                #if   BOARD_A2
                    digitalWrite(LTR2_RELAY, HIGH);
                    digitalWrite(R2_RELAY, HIGH);
                    digitalWrite(R4_RELAY, HIGH);
                #elif BOARD_A3
                    digitalWrite(PR1_RELAY, HIGH);
                    digitalWrite(PG1_RELAY, LOW);
                #elif BOARD_A4
                    digitalWrite(PR2_RELAY, HIGH);
                #endif
            }
            break;


        case SCENE_M5A:
            #if   BOARD_A2
                digitalWrite(LTR2_RELAY, HIGH);
                digitalWrite(R2_RELAY, HIGH);
                digitalWrite(G4_RELAY, HIGH);
            #elif BOARD_A3
                digitalWrite(PG1_RELAY, HIGH);
                digitalWrite(PR1_RELAY, LOW);
            #elif BOARD_A4
                digitalWrite(PG2_RELAY, HIGH);
            #endif
            break;


        case SCENE_M5B:
            if (blink % 2 == 0) {
                #if   BOARD_A2
                    digitalWrite(LTR2_RELAY, HIGH);
                    digitalWrite(R2_RELAY, HIGH);
                    digitalWrite(Y4_RELAY, HIGH);
                #elif BOARD_A3
                    digitalWrite(PG1_RELAY, HIGH);
                    digitalWrite(PR1_RELAY, LOW);
                #elif BOARD_A4
                    digitalWrite(PG2_RELAY, HIGH);
                #endif
            }
            else {
                #if   BOARD_A2
                    digitalWrite(LTR2_RELAY, HIGH);
                    digitalWrite(R2_RELAY, HIGH);
                    digitalWrite(Y4_RELAY, HIGH);
                #elif BOARD_A3
                    digitalWrite(PG1_RELAY, HIGH);
                    digitalWrite(PR1_RELAY, LOW);
                #elif BOARD_A4
                    digitalWrite(PG2_RELAY, HIGH);
                #endif
            }
            break;


        case SCENE_M6A:
            if (blink % 2 == 0) {
                #if   BOARD_A2
                    digitalWrite(LTR2_RELAY, LOW);
                    digitalWrite(Y2_RELAY, LOW);
                    digitalWrite(Y4_RELAY, LOW);
                #elif BOARD_A3
                    digitalWrite(PG1_RELAY, LOW);
                    digitalWrite(PR1_RELAY, LOW);
                #elif BOARD_A4
                    digitalWrite(PG2_RELAY, LOW);
                #endif
            }
            else {
                #if   BOARD_A2
                    digitalWrite(LTR2_RELAY, LOW);
                    digitalWrite(Y2_RELAY, HIGH);
                    digitalWrite(Y4_RELAY, HIGH);
                #elif BOARD_A3
                    digitalWrite(PG1_RELAY, LOW);
                    digitalWrite(PR1_RELAY, LOW);
                #elif BOARD_A4
                    digitalWrite(PG2_RELAY, LOW);
                #endif
            }
            break;
        }
    }
    // Serial.println("run_event::start");       
}

#endif


void setup(void) {

    previous_event = EVENT_00;
    current_event  = EVENT_00;
    previous_seq = SEQ_01A;
    current_seq  = SEQ_01A;
    previous_manual = SCENE_M6A;
    current_manual  = SCENE_M6A;

    event_active = true;
    seq_active = false;
    man_active = false;

    SPI.begin();
    Serial.begin(115200);

    #if BOARD_A2

        pinMode(LTR2_RELAY, OUTPUT);
        pinMode(LTY2_RELAY, OUTPUT);
        pinMode(LTG2_RELAY, OUTPUT);
        pinMode(R2_RELAY, OUTPUT);
        pinMode(Y2_RELAY, OUTPUT);
        pinMode(G2_RELAY, OUTPUT);
        pinMode(R4_RELAY, OUTPUT);
        pinMode(Y4_RELAY, OUTPUT);
        pinMode(G4_RELAY, OUTPUT);


    #elif BOARD_A3

        pinMode(PR1_RELAY, OUTPUT);
        pinMode(PG1_RELAY, OUTPUT);

    #elif BOARD_A4

        pinMode(PR2_RELAY, OUTPUT);
        pinMode(PG2_RELAY, OUTPUT);

    #endif

    // ========= NRF24L01 SETUP ========= //
    radio.begin();
    radio.setPayloadSize(sizeof(DataPack));
    radio.setDataRate(RF24_2MBPS);
    radio.setPALevel(RF24_PA_MIN);

    network.begin(90, current_board);
}   



void loop(void) {
    Serial.println("+++++loop::start+++++");
    network.update();

    observe_events();

    #if BOARD_A2 || BOARD_A3 || BOARD_A4
        run_event();
    #endif

    #if !(BOARD_A2)
        detect_vehicle();
    #endif


    Serial.println("-----loop::ended-----");
}
