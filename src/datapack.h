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

#ifndef _MAIN_H_
#define _MAIN_H_

#include "events.h"
#include "sensor.h"
#include "control.h"
#include "inttypes.h"


enum DataPackType {
    EVENT_T,
    SEQ_T,
    MAN_T,
    SENSOR_T,
    CONTR_T
};


struct DataPack {
    enum DataPackType type;                  // datapack type
    enum SensorState ss[SENSOR_COUNT];       // sensor states from 1 - 8
    enum Events event;                       // event
    bool event_active;                       // event
    enum Events_Seq seq;                     // event sequence (default mode 2)
    bool seq_active;                         // event sequence (default mode 2)
    enum ControlMode ctrl;
    enum Events_Man man;
    bool man_active;
    short blink;
};

#endif