#include <Arduino.h>
#include "defines.h"
#include "LoRaWAN.h"
#include "sensors_contentor.h"
#include "deep_sleep.h"
#include "data_processing.h"


uint8_t flag_vl53l0v = 0;
RTC_DATA_ATTR double latitude = 0;
RTC_DATA_ATTR double longitude = 0;


void IRAM_ATTR gpio1()
{
    flag_vl53l0v = 1;
}

/* Constantes do LMIC */
const lmic_pinmap lmic_pins = {
    .nss = RADIO_NSS_PORT,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = RADIO_RESET_PORT,
    .dio = {RADIO_DIO0_PORT, RADIO_DIO1_PORT, RADIO_DIO2_PORT},
};


void setup() 
{
    u1_t NWKSKEY[16] = { 0x51, 0xA1, 0x58, 0x94, 0x25, 0x46, 0x31, 0x42, 0x02, 0x35, 0x15, 0x35, 0x89, 0x46, 0x32, 0x51 };
    u1_t APPSKEY[16] = { 0xFA, 0xCE, 0x15, 0x44, 0x41, 0x86, 0x45, 0x31, 0x85, 0x32, 0x64, 0x86, 0x34, 0x51, 0x84, 0x35 }; 
    u4_t DEVADDR = 0xF3E35972;
    
    char *p_dados = nullptr;
    uint8_t tamanhoStr = 0;
    bool gpsFlag = false;


    gps_data_t gps_data;
    gps_data.latitude = latitude;
    gps_data.longitude = longitude;

    TinyGPSPlus gps;
    VL53L0X sensor;

    Serial.begin(BAUDRATE_SERIAL_DEBUG);
    Serial.println("Init.....");

    attachInterrupt(GPIO_1, gpio1, RISING);

    init_sensors(&sensor, &gpsFlag);

    init_lorawan(NWKSKEY, sizeof(NWKSKEY), APPSKEY, sizeof(APPSKEY), DEVADDR);
    while(1)
    {
        if(gpsFlag == 1)
        {
            if(Serial1.available() > 0){
                if(gps.encode(Serial1.read()) && gps.location.isValid()){
                    getGPS(&gps, &gps_data);
                    dataProcessing(&sensor, &gps_data, &p_dados, &tamanhoStr);
                    latitude = gps_data.latitude;
                    longitude = gps_data.longitude;
                    Serial.printf("Verificando transmissao: %d\n", verifyTransmition());
                    send_data(p_dados, &tamanhoStr);
                    digitalWrite(XSHUT, LOW);
                }
            }
        }else
        {
            dataProcessing_no_gps(&sensor, &gps_data, &p_dados, &tamanhoStr);
            send_data(p_dados, &tamanhoStr);
            digitalWrite(XSHUT, LOW);
        }

        os_runloop_once();  
    }
}

void loop() {}