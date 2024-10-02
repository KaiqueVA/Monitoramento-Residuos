#ifndef DATA_PROCESSING_H
#define DATA_PROCESSING_H

#include <Arduino.h>
#include "sensors_contentor.h"


void dataProcessing( VL53L0X *sensor, gps_data_t *gps_data, char **p_dados, uint8_t *tamanhoStr);
void dataProcessing_no_gps(VL53L0X *sensor, gps_data_t *gps_data, char **p_dados, uint8_t *tamanhoStr);



#endif