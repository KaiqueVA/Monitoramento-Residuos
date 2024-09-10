#include "deep_sleep.h"
#include "defines.h"
#include <Arduino.h>

void espSleep(uint32_t minutesSleep)
{
    esp_sleep_enable_timer_wakeup(minutesSleep * 60 * uS_TO_S_FACTOR);

    Serial.println("Dormindo...");
    esp_deep_sleep_start();
}