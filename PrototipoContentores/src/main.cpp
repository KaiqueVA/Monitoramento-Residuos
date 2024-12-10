#include <Arduino.h>
#include "defines.h"
#include "LoRaWAN.h"
#include "sensors_contentor.h"
#include "deep_sleep.h"
#include "data_processing.h"
#include "config_manageWiFi.h"
#include "armazenamento.h"

//==================Config========================
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
//================================================

RTC_DATA_ATTR double latitude = 0;
RTC_DATA_ATTR double longitude = 0;

const lmic_pinmap lmic_pins = {
    .nss = RADIO_NSS_PORT,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = RADIO_RESET_PORT,
    .dio = {RADIO_DIO0_PORT, RADIO_DIO1_PORT, RADIO_DIO2_PORT},
};

bool config_mode();
    

void setup() 
{
    
    u1_t NWKSKEY[16];
    u1_t APPSKEY[16]; 
    u4_t DEVADDR;
    
    char *p_dados = nullptr;
    uint8_t tamanhoStr = 0, confirm = 0;
    bool gpsFlag = false;

    const char* ssid = "Smart-Container";
    const char* password = "Contentor123";


    gps_data_t gps_data;
    gps_data.latitude = latitude;
    gps_data.longitude = longitude;

    TinyGPSPlus gps;
    VL53L0X sensor;

    pinMode(0, INPUT_PULLUP);
    uint32_t prevMillis = 0;
    
    Serial.begin(BAUDRATE_SERIAL_DEBUG);

    init_spiffs();
    
    
    bool config = config_mode();

    if(config)
    {
        Config_manageWiFi config_manageWiFi(ssid, password, 80);
        config_manageWiFi.init_AP();
        config_manageWiFi.init_dnsServer();
        config_manageWiFi.setupRoutes();
        config_manageWiFi.startServer();
    }

    loadLoRaWanKeys(NWKSKEY, sizeof(NWKSKEY), APPSKEY, sizeof(APPSKEY), &DEVADDR, sizeof(DEVADDR));
    
    Serial.println("Init.....");

    init_sensors(&sensor, &gpsFlag);

    init_lorawan(NWKSKEY, sizeof(NWKSKEY), APPSKEY, sizeof(APPSKEY), DEVADDR);
    while(1)
    {
        if(gpsFlag == 1)
        {
            if(Serial1.available() > 0){
                if(gps.encode(Serial1.read()) && gps.location.isValid()){
                    if(confirm == 0){
                        getGPS(&gps, &gps_data);
                        dataProcessing(&sensor, &gps_data, &p_dados, &tamanhoStr);
                        latitude = gps_data.latitude;
                        longitude = gps_data.longitude;
                        send_data(p_dados, &tamanhoStr, &confirm);
                        digitalWrite(XSHUT, LOW);
                    }
                }
            }
        }else
        {
            if(confirm == 0){
                dataProcessing_no_gps(&sensor, &gps_data, &p_dados, &tamanhoStr);
                send_data(p_dados, &tamanhoStr, &confirm);
                digitalWrite(XSHUT, LOW);
            }
        }

        os_runloop_once();  
    }
}

void loop() {}


bool config_mode()
{
    uint16_t prevMillis = millis();
    Serial.println("Pressione o botão para configurar...");

    while(1)
    {
        if(millis() - prevMillis > 3000)
        {
            Serial.println("Iniciando o sistema...");
            return false;
        }

        if(digitalRead(0) == LOW)
        {
            prevMillis = millis();
            while(digitalRead(0) == LOW)
            {
                if(millis() - prevMillis > 2000)
                {
                    Serial.println("Configurando...");
                    return true;
                }
            }
        }
        
    }
}