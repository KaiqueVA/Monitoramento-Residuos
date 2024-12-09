#include <Arduino.h>
#include "defines.h"
#include "LoRaWAN.h"
#include "sensors_contentor.h"
#include "deep_sleep.h"
#include "data_processing.h"

//==================Config========================
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <DNSServer.h>
//================================================

RTC_DATA_ATTR double latitude = 0;
RTC_DATA_ATTR double longitude = 0;

const lmic_pinmap lmic_pins = {
    .nss = RADIO_NSS_PORT,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = RADIO_RESET_PORT,
    .dio = {RADIO_DIO0_PORT, RADIO_DIO1_PORT, RADIO_DIO2_PORT},
};

//Config Contentor
const char* ssid = "Smart-Container";
const char* password = "Contentor123";

AsyncWebServer server(80);        // Porta 80 para o servidor HTTP
DNSServer dnsServer;



bool config_mode();
bool init_spiffs();
    

void setup() 
{
    
    u1_t NWKSKEY[16] = { 0x51, 0xA1, 0x58, 0x94, 0x25, 0x46, 0x31, 0x42, 0x02, 0x35, 0x15, 0x35, 0x89, 0x46, 0x32, 0x51 };
    u1_t APPSKEY[16] = { 0xFA, 0xCE, 0x15, 0x44, 0x41, 0x86, 0x45, 0x31, 0x85, 0x32, 0x64, 0x86, 0x34, 0x51, 0x84, 0x35 }; 
    u4_t DEVADDR = 0xF3E35972;
    
    char *p_dados = nullptr;
    uint8_t tamanhoStr = 0, confirm = 0;
    bool gpsFlag = false;


    gps_data_t gps_data;
    gps_data.latitude = latitude;
    gps_data.longitude = longitude;

    TinyGPSPlus gps;
    VL53L0X sensor;

    pinMode(0, INPUT_PULLUP);
    uint32_t prevMillis = 0;
    

    Serial.begin(BAUDRATE_SERIAL_DEBUG);

//INICIALIZAÇÃO DA SPIFFS E VERIFICAÇÃO DO AP

    init_spiffs();
    
    
    Serial.println("Pressione o botão para configurar...");
    bool config = config_mode();
    Serial.println(config);
    if(config)
    {
        setCpuFrequencyMhz(80);
        Serial.updateBaudRate(115200);
        Serial.println("Iniciando o WiFi");


        WiFi.softAP(ssid, password);

        Serial.println("Access Point criado!");
        Serial.print("IP do AP: ");
        Serial.println(WiFi.softAPIP());


        dnsServer.start(53, "*", WiFi.softAPIP());

        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
            Serial.println("Página principal acessada!");
            request->send(SPIFFS, "/index.html", "text/html");
        });

        server.serveStatic("/style.css", SPIFFS, "/style.css");
        server.serveStatic("/script.js", SPIFFS, "/script.js");

        server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->redirect("/");
            });

        server.on("/hotspot-detect.html", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->redirect("/");
        });

        server.on("/connecttest.txt", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->redirect("/");
        });


        server.on("/config-lorawan", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("body", true)) {
            String body = request->getParam("body", true)->value();
            Serial.println("Configuração recebida:");
            Serial.println(body);

            // Aqui você pode processar as chaves recebidas
            // Exemplo: salvar no SPIFFS ou aplicar diretamente
            request->send(200, "application/json", "{\"status\":\"success\"}");
        } else {
            request->send(400, "application/json", "{\"status\":\"error\"}");
        }
        });

        // Iniciar o servidor
        server.begin();
        Serial.println("Servidor web iniciado!");

        prevMillis = millis();
        while(1)
        {
            if(millis() - prevMillis > 300000)
            {
                Serial.println("Configuração encerrada!");
                break;
            }
            dnsServer.processNextRequest();
            delay(100);
            
        }
        esp_restart();
    }





    
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


bool init_spiffs()
{
    if (!SPIFFS.begin()) {
        Serial.println("Erro ao inicializar SPIFFS");
        return false;
    }
    return true;
}

bool config_mode()
{
    uint16_t prevMillis = millis();

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