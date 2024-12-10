#include <SPIFFS.h>

bool init_spiffs()
{
    if (!SPIFFS.begin()) {
        Serial.println("Erro ao inicializar SPIFFS");
        return false;
    }
    Serial.println("SPIFFS inicializado com sucesso!");
    return true;
}