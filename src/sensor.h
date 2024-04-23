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

#ifndef _SENSOR_H_
#define _SENSOR_H_

#define SCAN_DISTANCE_CM 420
#define MAX_DISTANCE_CM  450

#define PRE_ACTIVE_SENSOR_TIME_MS 10 * 1000
#define ACTIVE_SENSOR_TIME_MS     15 * 1000

#define SENSOR_COUNT 8

// INDEX VALUES
#define SENSOR_1 0
#define SENSOR_2 1
#define SENSOR_3 2
#define SENSOR_4 3
#define SENSOR_5 4
#define SENSOR_6 5
#define SENSOR_7 6
#define SENSOR_8 7

#define S1_PRE_ACTIVE_MS  7000
#define S2_PRE_ACTIVE_MS  7000
#define S3_PRE_ACTIVE_MS  5000
#define S4_PRE_ACTIVE_MS  5000
#define S5_PRE_ACTIVE_MS 12000
#define S6_PRE_ACTIVE_MS 12000

#define S1_ACTIVE_MS 10000
#define S2_ACTIVE_MS 10000
#define S3_ACTIVE_MS  8000
#define S4_ACTIVE_MS  8000
#define S5_ACTIVE_MS 15000
#define S6_ACTIVE_MS 15000
#define S7_ACTIVE_MS 15000 // for both s1 and s7
#define S8_ACTIVE_MS 15000 // for both s2 and s8


enum SensorState {
    INACTIVE,
    ACTIVE
};


#endif