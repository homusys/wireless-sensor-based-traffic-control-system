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
#define EVENT_1A_ACTIVE_TIME_MS 5000
#define EVENT_1B_ACTIVE_TIME_MS 10000
#define EVENT_1C_ACTIVE_TIME_MS 20000
#define EVENT_2A_ACTIVE_TIME_MS 5000
#define EVENT_2B_ACTIVE_TIME_MS 10000
#define EVENT_2C_ACTIVE_TIME_MS 20000
#define EVENT_3A_ACTIVE_TIME_MS 5000
#define EVENT_3B_ACTIVE_TIME_MS 10000
#define EVENT_4A_ACTIVE_TIME_MS 5000
#define EVENT_4B_ACTIVE_TIME_MS 10000
#define EVENT_5A_ACTIVE_TIME_MS 5000
#define EVENT_5B_ACTIVE_TIME_MS 10000
#define EVENT_6A_ACTIVE_TIME_MS 5000
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
    EVENT_2A,
    EVENT_2B,
    EVENT_2C,
    EVENT_3A,
    EVENT_3B,
    EVENT_4A,
    EVENT_4B,
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
    SEQ_03
};


#define SEQ_01_ACTIVE_TIME 90000
#define SEQ_02_ACTIVE_TIME 15000
#define SEQ_03_ACTIVE_TIME 15000


/* ++++++++++++ DEFAULT MODE 2 CONSTANTS ++++++++++++ */
#define GREEN_GRANT_06T07  15
#define GREEN_GRANT_09T10  10
#define GREEN_GRANT_10T11  15
#define GREEN_GRANT_12T13  20
#define GREEN_GRANT_13T14  15
#define GREEN_GRANT_14T15  20
#define GREEN_GRANT_15T16  15


#endif