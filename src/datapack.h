#ifndef _MAIN_H_
#define _MAIN_H_

#include "events.h"
#include "sensor.h"
#include "inttypes.h"


enum DataPackType {
    EVENT_T,
    SENSOR_T
};


struct DataPack {
    enum DataPackType type;                  // datapack type
    enum SensorState ss[SENSOR_COUNT];       // sensor states from 1 - 8
    enum Events event;                       // event
};

#endif