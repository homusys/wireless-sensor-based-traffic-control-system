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

#define SCAN_DISTANCE_CM 5
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


enum SensorState {
    INACTIVE,
    ACTIVE
};


#endif