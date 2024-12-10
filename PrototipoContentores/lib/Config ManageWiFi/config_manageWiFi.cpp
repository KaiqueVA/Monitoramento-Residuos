#include "config_manageWiFi.h"
#include "armazenamento.h"


Config_manageWiFi::Config_manageWiFi(const char* ssid, const char* password, int port) : server(port)
{
    this->ssid = new char[strlen(ssid) + 1];
    this->password = new char[strlen(password) + 1];
    strcpy(this->ssid, ssid);
    strcpy(this->password, password);
}

Config_manageWiFi::~Config_manageWiFi()
{
    delete[] ssid;
    delete[] password;
}

void Config_manageWiFi::init_AP()
{
    if(getCpuFrequencyMhz() < 80)
    {
        setCpuFrequencyMhz(80);
    }
    Serial.updateBaudRate(115200);

    Serial.println("Iniciando AP");

    WiFi.softAP(ssid, password);
    Serial.println("APcriado!");
    Serial.print("IP do AP: ");
    Serial.println(WiFi.softAPIP());
}

void Config_manageWiFi::init_dnsServer()
{
    dnsServer.start(53, "*", WiFi.softAPIP());
    Serial.println("DNS Server iniciado!");
}

void Config_manageWiFi::setupRoutes() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        Serial.println("Página principal acessada!");
        request->send(SPIFFS, "/index.html", "text/html");
    });

    server.serveStatic("/style.css", SPIFFS, "/style.css");
    server.serveStatic("/script.js", SPIFFS, "/script.js");

    server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->redirect("/");
    });

    server.on("/hotspot-detect.html", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->redirect("/");
    });

    server.on("/connecttest.txt", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->redirect("/");
    });

    server.on("/config-lorawan", HTTP_POST, [](AsyncWebServerRequest* request) {}, NULL,
                [](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
                    String body = "";
                    for (size_t i = 0; i < len; i++) {
                        body += (char)data[i];
                    }

                    Serial.println("Dados recebidos:");
                    Serial.println(body);

                    JsonDocument doc;
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

                    File file = SPIFFS.open("/lorawan_config.bin", FILE_WRITE);
                    if (!file) {
                        Serial.println("Erro ao abrir o arquivo para escrita");
                        request->send(500, "application/json", "{\"status\":\"error\",\"message\":\"Failed to save config\"}");
                        return;
                    }

                    uint32_t devaddr_bytes = (uint32_t)strtoul(devaddr, nullptr, 16);
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

    Serial.println("Rotas configuradas!");
}


void Config_manageWiFi::startServer()
{
    unsigned long prevMillis = millis();
    server.begin();
    Serial.println("Servidor iniciado!");
    while(1){
        if(millis() - prevMillis > 300000)
        {
            Serial.println("Reiniciando o sistema...");
            esp_restart();
        }

        dnsServer.processNextRequest();
        vTaskDelay(100);
    }
}

