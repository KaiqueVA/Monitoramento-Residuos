#ifndef DATA_PROCESSING_H
#define DATA_PROCESSING_H

#include <Arduino.h>
#include "sensors_contentor.h"


void dataProcessing(TinyGPSPlus *gps, VL53L0X *sensor, char **p_dados, uint8_t *tamanhoStr);


#endif