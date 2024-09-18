#ifndef SENSORS_H
#define SENSORS_H

#include <TinyGPSPlus.h>
#include <VL53L0X.h>
#include <Wire.h>

// Declaração dos objetos de sensores
extern TinyGPSPlus gps;
extern VL53L0X sensor;

// Prototipação das funções
void initSensors();
uint8_t leituraBat();
uint16_t leituraSensor();

#endif 
