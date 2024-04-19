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


#ifndef _EVENTS_H_
#define _EVENTS_H_

#define MAX_EVENTS     3
#define EVENT_COUNT    6
#define EVENT_COOLDOWN 2


/* ++++++++++++ LIGHT EVENT ACTIVE TIME ++++++++++++ */
#define EVENT_00_ACTIVE_TIME_MS 1000
#define EVENT_1A_ACTIVE_TIME_MS 5000  // Y
#define EVENT_1B_ACTIVE_TIME_MS 5000
#define EVENT_1C_ACTIVE_TIME_MS 10000
#define EVENT_1D_ACTIVE_TIME_MS 3000  // Y

#define EVENT_2A_ACTIVE_TIME_MS 5000
#define EVENT_2B_ACTIVE_TIME_MS 6000  // Y
#define EVENT_2C_ACTIVE_TIME_MS 12000
#define EVENT_2D_ACTIVE_TIME_MS 3000  // Y

#define EVENT_3A_ACTIVE_TIME_MS 5000  // Y
#define EVENT_3B_ACTIVE_TIME_MS 5000
#define EVENT_3C_ACTIVE_TIME_MS 3000  // Y

#define EVENT_4A_ACTIVE_TIME_MS 5000  // Y
#define EVENT_4B_ACTIVE_TIME_MS 4000
#define EVENT_4C_ACTIVE_TIME_MS 3000  // Y

#define EVENT_5A_ACTIVE_TIME_MS 5000  // Y
#define EVENT_5B_ACTIVE_TIME_MS 10000

#define EVENT_6A_ACTIVE_TIME_MS 5000  // Y
#define EVENT_6B_ACTIVE_TIME_MS 10000

#define RESOLUTION_TIME_MS      100


/* ++++++++++++ EVENT COOLDOWN INDEX ++++++++++++ */
#define EVENT_1 0
#define EVENT_2 1
#define EVENT_3 2
#define EVENT_4 3
#define EVENT_5 4
#define EVENT_6 5


/* ++++++++++++ EVENT ENUM DEFINITION ++++++++++++ */
/**
 * EVENT A = YELLOW LIGHTS
 * EVENT A = RED/GREEN LIGHTS
 * EVENT A = GREEN LIGHTS EXTENDED TIME
*/
enum Events {
    EVENT_00,
    EVENT_1A,
    EVENT_1B,
    EVENT_1C,
    EVENT_1D,
    EVENT_2A,
    EVENT_2B,
    EVENT_2C,
    EVENT_2D,
    EVENT_3A,
    EVENT_3B,
    EVENT_3C,
    EVENT_4A,
    EVENT_4B,
    EVENT_4C,
    EVENT_5A,
    EVENT_5B,
    EVENT_6A,
    EVENT_6B
};


/* ++++++++++++ EVENT SEQUENCE ENUM DEFINITION ++++++++++++ */
/// @brief During default mode 2.1 an event sequence happens
enum Events_Seq {
    SEQ_01,
    SEQ_02,
    SEQ_03,
    SEQ_04,
    SEQ_05
};


#define SEQ_01_ACTIVE_TIME 116660
#define SEQ_02_ACTIVE_TIME 3220
#define SEQ_03_ACTIVE_TIME 9360
#define SEQ_04_ACTIVE_TIME 13600
#define SEQ_05_ACTIVE_TIME 7160


/* ++++++++++++ DEFAULT MODE 2 CONSTANTS ++++++++++++ */
#define GREEN_GRANT_06T07  141
#define GREEN_GRANT_09T10  202
#define GREEN_GRANT_10T11  188
#define GREEN_GRANT_13T14  166
#define GREEN_GRANT_14T15  172
#define GREEN_GRANT_15T16  186
#define GREEN_GRANT_RESET   00

#endif