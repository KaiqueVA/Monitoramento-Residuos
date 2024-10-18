#include "LoRaWAN.h"
#include "stdint.h"
#include "defines.h"
#include <SPI.h>
#include <Arduino.h>
#include "deep_sleep.h"

void os_getArtEui (u1_t* buf) {}
void os_getDevEui (u1_t* buf) {}
void os_getDevKey (u1_t* buf) {}

void init_lorawan(u1_t *NWKSKEY, size_t nwkskey_size, u1_t *APPSKEY, size_t appskey_size, u4_t DEVADDR)
{
    int b;
    uint8_t appskey[appskey_size];
    uint8_t nwkskey[nwkskey_size];
    SPI.begin(RADIO_SCLK_PORT, RADIO_MISO_PORT, RADIO_MOSI_PORT);

    os_init();
    LMIC_reset();

    memcpy_P(appskey, APPSKEY, appskey_size);
    memcpy_P(nwkskey, NWKSKEY, nwkskey_size);
    LMIC_setSession (0x13, DEVADDR, nwkskey, appskey);

    for (b=0; b<8; ++b)
        LMIC_disableSubBand(b);

    LMIC_enableChannel(0); // 915.2 MHz
    LMIC_enableChannel(1); // 915.4 MHz
    LMIC_enableChannel(2); // 915.6 MHz
    LMIC_enableChannel(3); // 915.8 MHz
    LMIC_enableChannel(4); // 916.0 MHz
    LMIC_enableChannel(5); // 916.2 MHz
    LMIC_enableChannel(6); // 916.4 MHz
    LMIC_enableChannel(7); // 916.6 MHz

    LMIC_setAdrMode(0);
    LMIC_setLinkCheckMode(0);

    LMIC.dn2Dr = DR_SF12CR;


    LMIC_setDrTxpow(DR_SF12, GANHO_LORA_DBM);
}

void onEvent (ev_t e)
{
    Serial.print(os_getTime());
    Serial.print(": ");
    Serial.println(e);

    switch(e)
    {
        case EV_SCAN_TIMEOUT:
            Serial.println(F("EV_SCAN_TIMEOUT"));
            break;
        case EV_BEACON_FOUND:
            Serial.println(F("EV_BEACON_FOUND"));
            break;
        case EV_BEACON_MISSED:
            Serial.println(F("EV_BEACON_MISSED"));
            break;
        case EV_BEACON_TRACKED:
            Serial.println(F("EV_BEACON_TRACKED"));
            break;
        case EV_JOINING:
            Serial.println(F("EV_JOINING"));
            break;
        case EV_JOINED:
            Serial.println(F("EV_JOINED"));
            break;
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            break;
        case EV_TXCOMPLETE:
            Serial.println (millis());
            Serial.println(F("EV_TXCOMPLETE (incluindo espera pelas janelas de recepcao)"));

            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println(F("Ack recebido"));

            if (LMIC.dataLen)
            {
                Serial.println(F("Recebidos "));
                Serial.println(LMIC.dataLen);
                Serial.println(F(" bytes (payload) do gateway"));

                if (LMIC.dataLen == 1)
                {
                    uint8_t dados_recebidos = LMIC.frame[LMIC.dataBeg + 0];
                    Serial.print(F("Dados recebidos: "));
                    Serial.write(dados_recebidos);
                }

                delay(100);

            }

            LMIC_shutdown();
            digitalWrite(XSHUT, LOW);
            digitalWrite(SLEEP_LDO, LOW);
            gpsSleep();
            espSleep(60);


            break;

        case EV_LOST_TSYNC:
            Serial.println(F("EV_LOST_TSYNC"));
            break;
        case EV_RESET:
            Serial.println(F("EV_RESET"));
            break;
        case EV_RXCOMPLETE:
            Serial.println(F("EV_RXCOMPLETE"));
            break;
        case EV_LINK_DEAD:
            Serial.println(F("EV_LINK_DEAD"));
            break;
        case EV_LINK_ALIVE:
            Serial.println(F("EV_LINK_ALIVE"));
            break;
        case EV_TXSTART:
            Serial.println(F("EV_TXSTART"));
            Serial.println (millis());
            Serial.println(LMIC.freq);
            break;
        default:
            Serial.print(F("Evento desconhecido: "));
            Serial.println((unsigned) e);
            break;
    }
}


void send_data(char *data, uint8_t *size)
{
    if(LMIC.opmode & OP_TXRXPEND)
    {
        Serial.println(F("Transmission already in progress"));
        return;
    }
    LMIC_setTxData2(1, (uint8_t*)data, *size, 0);
    Serial.println(F("Enviando pacote"));

}

uint32_t verifyTransmition()
{
    return (LMIC.opmode & OP_TXRXPEND);	
}
