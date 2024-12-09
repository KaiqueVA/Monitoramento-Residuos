#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

class Config_manageWiFi
{
    private:
        const char* ssid;
        const char* password;

    public:
        Config_manageWiFi(const char* ssid, const char* password);
};