#ifndef LORAWAN_H
#define LORAWAN_H

#include <lmic.h>
#include <hal/hal.h>
#include "stdbool.h"

void init_lorawan(u1_t *NWKSKEY, size_t nwkskey_size, u1_t *APPSKEY, size_t appskey_size, u4_t DEVADDR);
void os_getArtEui (u1_t* buf);
void os_getDevEui (u1_t* buf);
void os_getDevKey (u1_t* buf);
uint32_t verifyTransmition();
void send_data(char *data, uint8_t *size);

#endif