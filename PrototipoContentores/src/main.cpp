/*
#include "Arduino.h"
*/
#include "defines.h"
#include <Arduino.h>
#include "LoRaWAN.h"
#include "sensors_contentor.h"
#include "deep_sleep.h"



/* Constantes do LMIC */
const lmic_pinmap lmic_pins = {
    .nss = RADIO_NSS_PORT,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = RADIO_RESET_PORT,
    .dio = {RADIO_DIO0_PORT, RADIO_DIO1_PORT, RADIO_DIO2_PORT},
};





void dataProcessing(TinyGPSPlus *gps, VL53L0X *sensor, char **p_dados, uint8_t *tamanhoStr);


void setup() 
{
    u1_t NWKSKEY[16] = { 0x51, 0xA1, 0x58, 0x94, 0x25, 0x46, 0x31, 0x42, 0x02, 0x35, 0x15, 0x35, 0x89, 0x46, 0x32, 0x51 };
    u1_t APPSKEY[16] = { 0xFA, 0xCE, 0x15, 0x44, 0x41, 0x86, 0x45, 0x31, 0x85, 0x32, 0x64, 0x86, 0x34, 0x51, 0x84, 0x35 }; 
    u4_t DEVADDR = 0xF3E35972;
    
    char *p_dados = nullptr;
    uint8_t tamanhoStr = 0;

    TinyGPSPlus gps;
    VL53L0X sensor;



    Serial.begin(BAUDRATE_SERIAL_DEBUG);
    Serial.println("Init.....");
    init_sensors(&sensor);

    init_lorawan(NWKSKEY, sizeof(NWKSKEY), APPSKEY, sizeof(APPSKEY), DEVADDR);

    while(1)
    {
        //while(Serial1.available() > 0)
            //if(gps.encode(Serial1.read()) && gps.location.isValid()){
                dataProcessing(&gps, &sensor, &p_dados, &tamanhoStr);
                send_data(p_dados, &tamanhoStr);
            //}

        os_runloop_once();  
    }
}

void loop() {}








void dataProcessing(TinyGPSPlus *gps, VL53L0X *sensor, char **p_dados, uint8_t *tamanhoStr)
{
    uint8_t tombo = digitalRead(INCLINACAO);
    uint8_t batery = leituraBat();
    uint16_t volume = leituraSensor(sensor);
    double latitude = 0;
    double longitude = 0;

    getGPS(gps, &latitude, &longitude);

    *tamanhoStr = snprintf(nullptr, 0, "%03d%04d%03.6f%03.6f%d", batery, volume, latitude, longitude, tombo);
    *p_dados = (char*)malloc((*tamanhoStr + 1) * sizeof(char));
    if(*p_dados == nullptr)
    {
        Serial.println("Erro ao alocar memoria");
        delay(1000);
        ESP.restart();
    }
    else
    {
        snprintf(*p_dados, *tamanhoStr + 1, "%03d%04d%03.6f%03.6f%d", batery, volume, latitude, longitude, tombo);
        Serial.println(*p_dados);
    }

}





