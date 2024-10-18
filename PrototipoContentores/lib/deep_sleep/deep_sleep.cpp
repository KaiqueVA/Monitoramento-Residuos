#include "deep_sleep.h"
#include "defines.h"
#include <Arduino.h>
#include <driver/adc.h>
#include <esp_sleep.h>
#include <SPI.h>
#include <Wire.h>	

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
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH,   ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_ON);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL,         ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH,   ESP_PD_OPTION_OFF);
    
    Serial.println("Dormindo...");
    esp_deep_sleep_start();
}

void lightSleep(uint32_t milliSeconds)
{
    esp_sleep_enable_timer_wakeup(milliSeconds * uS_TO_mS_FACTOR);
    
    Serial.println("Light Sleep");
    esp_light_sleep_start();
}

void gpsSleep(void)
{
    digitalWrite(GPS_SLEEP, LOW);
    //Serial1.print("$PMTK161,0*28\r\n");//standby mode
    Serial1.print("$PMTK225,4*2F\r\n");//backup mode
}
