#ifndef DEEP_SLEEP_H
#define DEEP_SLEEP_H

#include "stdint.h"

extern uint8_t rtc_wockUp;

void espSleep(uint32_t minutesSleep);

void gpsSleep(void);

bool verifyWokeUpTimes(void);

#endif