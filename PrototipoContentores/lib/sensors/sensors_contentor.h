#ifndef SENSORS_CONTENTOR_H
#define SENSORS_CONTENTOR_H

#include "stdint.h"
#include <VL53L0X.h>
#include <TinyGPSPlus.h>

void init_sensors(VL53L0X *sensor);
uint8_t leituraBat();
uint16_t leituraSensor(VL53L0X *sensor);
void getGPS(TinyGPSPlus *gps, double *p_lat, double *p_lon);




#endif