#ifndef SENSORS_CONTENTOR_H
#define SENSORS_CONTENTOR_H

#include "stdint.h"
#include <VL53L0X.h>
#include <TinyGPSPlus.h>

extern uint8_t flag_vl53l0v;

typedef struct
{
    double latitude;
    double longitude;
}gps_data_t;

void init_sensors(VL53L0X *sensor, bool *gpsFlag);
uint8_t leituraBat();
uint16_t leituraSensor(VL53L0X *sensor);
void getGPS(TinyGPSPlus *gps, gps_data_t *gps_data);




#endif