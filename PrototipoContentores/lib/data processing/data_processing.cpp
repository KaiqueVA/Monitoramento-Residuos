#include "data_processing.h"
#include "defines.h"

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
