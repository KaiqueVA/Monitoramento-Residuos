#include "sensors_contentor.h"
#include "defines.h"
#include <Arduino.h>


void init_sensors(VL53L0X *sensor)
{
    Serial1.begin(9600, SERIAL_8N1, 16, 17);

    pinMode(GPIO_1, INPUT_PULLDOWN);
    pinMode(XSHUT, OUTPUT);
    pinMode(GPS_SLEEP, OUTPUT);
    pinMode(INCLINACAO, INPUT);

    digitalWrite(GPS_SLEEP, LOW);
    digitalWrite(XSHUT, HIGH);
    Wire.begin();
    sensor->setTimeout(500);
    if(!sensor->init())
    {
        Serial.println("Falha ao iniciar o sensor");
        delay(1000);
        ESP.restart();
    }

    sensor->setSignalRateLimit(0.1);
    sensor->setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
    sensor->setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
    sensor->setMeasurementTimingBudget(200000);

    digitalWrite(GPS_SLEEP, HIGH);
}

uint8_t leituraBat()
{
    long batSoma = 0;
    uint8_t batPorcent;
    for(int i = 0; i < 50; i++)
    {
        batSoma += analogRead(BAT_MON);
    }
    batPorcent = map(batSoma/50, 3755, 2555, 100, 0);
    if(batPorcent > 100)
    {
        batPorcent = 100;
        Serial.print("ALERTA! Passou de 100% - ");
    }

    if(batPorcent < 0)
        batPorcent = 0;
    Serial.print(batPorcent);
    Serial.println("%");
    return batPorcent;
}

uint16_t leituraSensor(VL53L0X *sensor)
{
    unsigned long contSensor = 0;
    for(int i = 0; i < 10; i++)
    {
        contSensor += sensor->readRangeSingleMillimeters();
    }
    Serial.print("Distancia media: ");
    Serial.print(contSensor/10);
    Serial.println("mm");
    return contSensor/10;
}

void getGPS(TinyGPSPlus *gps, double *p_lat, double *p_lon)
{
    *p_lat = gps->location.lat();
    *p_lon = gps->location.lng();
}
