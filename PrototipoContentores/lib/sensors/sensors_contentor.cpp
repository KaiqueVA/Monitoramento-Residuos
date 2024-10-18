#include "sensors_contentor.h"
#include "defines.h"
#include <Arduino.h>
#include "deep_sleep.h"

#define R1              39000.0
#define R2              47000.0
#define MIN_VOLTAGE     3.0
#define MAX_VOLTAGE     3.7



void init_sensors(VL53L0X *sensor, bool *gpsFlag)
{
    *gpsFlag = verifyWokeUpTimes();
    
    Serial1.begin(9600, SERIAL_8N1, 16, 17);
    analogReadResolution(12);
    pinMode(GPIO_1, INPUT);
    pinMode(XSHUT, OUTPUT);
    pinMode(SLEEP_LDO, OUTPUT);
    pinMode(GPS_SLEEP, OUTPUT);
    pinMode(INCLINACAO, INPUT);

    if(*gpsFlag){
        Serial.println("GPS ON");
        digitalWrite(GPS_SLEEP, HIGH);
        Serial1.print("$PMTK225,0*2B\r\n");
        Serial.write(Serial1.read());
    }
    lightSleep(1);
    digitalWrite(GPS_SLEEP, LOW);

    //gpsSleep();
    digitalWrite(SLEEP_LDO, HIGH);
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

}

uint8_t leituraBat()
{
    int batReading = analogReadMilliVolts(BAT_MON);
    int batPorcent;
    float batVoltage;
    float batVoltageBattery;
    

    batVoltage = (((float)batReading) / 4095.0) * 3.3;

    batVoltageBattery = batVoltage * (R1 + R2) / R2;

    Serial.printf("Tensão medida: %f V\n", batVoltageBattery);

    batPorcent = (((batVoltageBattery - MIN_VOLTAGE) / (MAX_VOLTAGE - MIN_VOLTAGE)) * 100.0);

    if(batPorcent > 100)
        batPorcent = 100;
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

void getGPS(TinyGPSPlus *gps, gps_data_t *gps_data)
{
    gps_data->latitude = gps->location.lat();
    gps_data->longitude = gps->location.lng();
}
