#include "deep_sleep.h"
#include "defines.h"
#include <Arduino.h>

RTC_DATA_ATTR uint8_t rtc_wockUp = 1;

bool verifyWokeUpTimes(void)
{
    if(rtc_wockUp == 1){
        rtc_wockUp++;
        return true;
    }
    
    rtc_wockUp++;
    if(rtc_wockUp > 8)
        rtc_wockUp = 1;
    return false;
}

void espSleep(uint32_t minutesSleep)
{
    esp_sleep_enable_timer_wakeup(minutesSleep * 60 * uS_TO_S_FACTOR);

    Serial.println("Dormindo...");
    esp_deep_sleep_start();
}

void gpsSleep(void)
{
    digitalWrite(GPS_SLEEP, LOW);
    Serial1.print("$PMTK161,0*28\r\n");
}