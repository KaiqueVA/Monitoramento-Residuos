#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <ArduinoJson.h>


class Config_manageWiFi
{
    private:
        char* ssid;
        char* password;
        int port;
        DNSServer dnsServer;
        AsyncWebServer server;

    public:
        Config_manageWiFi(const char* ssid, const char* password, int port);
        ~Config_manageWiFi();
        void init_AP();
        void init_dnsServer();
        void setupRoutes();
        void startServer();
};