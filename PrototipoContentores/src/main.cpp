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

//Config Contentor
const char* ssid = "Smart-Container";
const char* password = "Contentor123";

AsyncWebServer server(80);        // Porta 80 para o servidor HTTP
DNSServer dnsServer;



bool config_mode();
bool init_spiffs();
void loadLoRaWanKeys(u1_t *NWKSKEY, size_t nwkskey_size, u1_t *APPSKEY, size_t appskey_size, u4_t *DEVADDR, size_t devaddr_size);
    

void setup() 
{
    
    u1_t NWKSKEY[16];
    u1_t APPSKEY[16]; 
    u4_t DEVADDR;
    
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


        server.on("/config-lorawan", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
            [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
                String body = "";
                for (size_t i = 0; i < len; i++) {
                    body += (char)data[i];
                }

                Serial.println("Dados recebidos:");
                Serial.println(body);

                StaticJsonDocument<512> doc;
                DeserializationError error = deserializeJson(doc, body);

                if (error) {
                    Serial.print("Erro ao parsear JSON: ");
                    Serial.println(error.c_str());
                    request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
                    return;
                }

                const char* devaddr = doc["devaddr"];
                const char* nwkskey = doc["nwkskey"];
                const char* appskey = doc["appskey"];

                if (!devaddr || strlen(devaddr) != 8 || !nwkskey || !appskey) {
                    Serial.println("Erro: Campos ausentes no JSON");
                    request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing fields\"}");
                    return;
                }

                // Salvando os dados no SPIFFS (como no exemplo anterior)
                File file = SPIFFS.open("/lorawan_config.bin", FILE_WRITE);
                if (!file) {
                    Serial.println("Erro ao abrir o arquivo para escrita");
                    request->send(500, "application/json", "{\"status\":\"error\",\"message\":\"Failed to save config\"}");
                    return;
                }

                uint32_t devaddr_bytes = (uint32_t)strtoul(devaddr, nullptr, 16);

                Serial.print("DevAddr: 0x");
                Serial.println(devaddr_bytes, HEX);

                file.write((uint8_t*)&devaddr_bytes, sizeof(devaddr_bytes));

                uint8_t nwkskey_bytes[16];
                for (int i = 0; i < 16; i++) {
                    nwkskey_bytes[i] = strtoul(String(nwkskey).substring(i * 2, i * 2 + 2).c_str(), nullptr, 16);
                }
                file.write(nwkskey_bytes, sizeof(nwkskey_bytes));

                uint8_t appskey_bytes[16];
                for (int i = 0; i < 16; i++) {
                    appskey_bytes[i] = strtoul(String(appskey).substring(i * 2, i * 2 + 2).c_str(), nullptr, 16);
                }
                file.write(appskey_bytes, sizeof(appskey_bytes));

                file.close();

                Serial.println("Configuração salva com sucesso!");
                request->send(200, "application/json", "{\"status\":\"success\"}");

                delay(100);
                esp_restart();
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


bool init_spiffs()
{
    if (!SPIFFS.begin()) {
        Serial.println("Erro ao inicializar SPIFFS");
        return false;
    }
    Serial.println("SPIFFS inicializado com sucesso!");
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


void loadLoRaWanKeys(u1_t *NWKSKEY, size_t nwkskey_size, u1_t *APPSKEY, size_t appskey_size, u4_t *DEVADDR, size_t devaddr_size)
{
    if (!SPIFFS.exists("/lorawan_config.bin")) {
        Serial.println("Arquivo de configuração não encontrado.");
        return;
    }

    File file = SPIFFS.open("/lorawan_config.bin", FILE_READ);
    if (!file) {
        Serial.println("Erro ao abrir o arquivo de configuração.");
        return;
    }

    file.read((uint8_t*)DEVADDR, devaddr_size);
    file.read(NWKSKEY, nwkskey_size);
    file.read(APPSKEY, appskey_size);

    file.close();

    Serial.println("Chaves carregadas com sucesso!");
    // Aqui desreferenciamos o ponteiro DEVADDR para exibir o valor real
    Serial.printf("DevAddr: 0x%08X\n", *DEVADDR);
    Serial.print("NwkSKey: ");
    for (int i = 0; i < 16; i++) {
        Serial.printf("0x%02X ", NWKSKEY[i]);
    }
    Serial.println();

    Serial.print("AppSKey: ");
    for (int i = 0; i < 16; i++) {
        Serial.printf("0x%02X ", APPSKEY[i]);
    }
    Serial.println();
}