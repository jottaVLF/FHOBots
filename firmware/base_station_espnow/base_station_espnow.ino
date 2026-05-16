#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <cstdarg>
#include <cstdio>
#include <cstring>

/*
 * ESP32 Base Station - USB Serial -> ESP-NOW broadcast
 *
 * Fluxo correto:
 *   PC C++ envia o pacote binario de 18 bytes pela USB Serial.
 *   Esta ESP32 valida 18 bytes/header/checksum.
 *   Pacote valido e retransmitido por ESP-NOW para FF:FF:FF:FF:FF:FF.
 *
 * Importante:
 *   - Nao ha UDP/IP entre PC e base station.
 *   - Nao ha SSID/senha.
 *   - Nao ha MAC fixo de robo.
 *   - Nao ha pareamento individual.
 *   - Todos os robos no mesmo canal ESP-NOW recebem o mesmo pacote.
 */

// ==========================
// Configuracao USB Serial
// ==========================
// Deve bater com o baud rate usado no C++ POSIX serial.
static constexpr uint32_t SERIAL_BAUD = 115200;

// Logs usam a mesma USB Serial. Como o PC so escreve pacotes binarios e nao
// depende de leitura, isso nao atrapalha o envio. Desabilite se quiser uma
// porta totalmente silenciosa.
static constexpr bool ENABLE_SERIAL_LOGS = true;
static constexpr uint32_t LOG_EVERY_VALID_PACKETS = 50;

// ==========================
// Configuracao ESP-NOW
// ==========================
// Base e robos devem usar o mesmo canal fixo. Nao depende de roteador.
static constexpr uint8_t ESPNOW_CHANNEL = 1;

// LED opcional para pulso rapido quando um pacote valido chega.
// Use -1 para desabilitar.
static constexpr int STATUS_LED_PIN = 2;
static constexpr uint32_t LED_PULSE_MS = 35;

// =====================
// Protocolo VSSS ESP-NOW
// =====================
static constexpr uint8_t HEADER_0 = 0xA5;
static constexpr uint8_t HEADER_1 = 0x5A;
static constexpr size_t PACKET_SIZE = 18;
static constexpr size_t CHECKSUM_INDEX = PACKET_SIZE - 1;

static constexpr uint8_t BROADCAST_MAC[6] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

uint8_t packetBuffer[PACKET_SIZE];
size_t packetIndex = 0;

uint32_t validPacketCount = 0;
uint32_t invalidHeaderCount = 0;
uint32_t invalidChecksumCount = 0;
uint32_t espNowErrorCount = 0;
uint32_t ledOffAtMs = 0;

void logPrintf(const char *format, ...)
{
    if(!ENABLE_SERIAL_LOGS)
        return;

    char buffer[180];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Serial.print(buffer);
}

uint8_t calculateChecksum(const uint8_t *data)
{
    uint8_t checksum = 0;
    for(size_t i = 0; i < CHECKSUM_INDEX; i++)
        checksum ^= data[i];
    return checksum;
}

uint16_t packetCounter(const uint8_t *data)
{
    return static_cast<uint16_t>(data[2]) |
           (static_cast<uint16_t>(data[3]) << 8);
}

void printPacketHex(const uint8_t *data, size_t length)
{
    if(!ENABLE_SERIAL_LOGS)
        return;

    for(size_t i = 0; i < length; i++)
    {
        if(data[i] < 0x10)
            Serial.print('0');
        Serial.print(data[i], HEX);
        Serial.print(i + 1 == length ? '\n' : ' ');
    }
}

bool validatePacket(const uint8_t *data)
{
    if(data[0] != HEADER_0 || data[1] != HEADER_1)
    {
        invalidHeaderCount++;
        logPrintf("Pacote Serial ignorado: header invalido %02X %02X\n",
                  data[0],
                  data[1]);
        return false;
    }

    const uint8_t expected = calculateChecksum(data);
    if(data[CHECKSUM_INDEX] != expected)
    {
        invalidChecksumCount++;
        logPrintf("Erro de checksum: recebido %02X, esperado %02X, contador PC %u\n",
                  data[CHECKSUM_INDEX],
                  expected,
                  packetCounter(data));
        return false;
    }

    return true;
}

void pulseLed()
{
    if(STATUS_LED_PIN < 0)
        return;

    digitalWrite(STATUS_LED_PIN, HIGH);
    ledOffAtMs = millis() + LED_PULSE_MS;
}

void updateLed()
{
    if(STATUS_LED_PIN < 0 || ledOffAtMs == 0)
        return;

    if(static_cast<int32_t>(millis() - ledOffAtMs) >= 0)
    {
        digitalWrite(STATUS_LED_PIN, LOW);
        ledOffAtMs = 0;
    }
}

void setupEspNow()
{
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.disconnect();
    delay(100);

    if(ESPNOW_CHANNEL < 1 || ESPNOW_CHANNEL > 13)
    {
        logPrintf("ERRO: ESPNOW_CHANNEL invalido: %u\n", ESPNOW_CHANNEL);
        while(true)
            delay(1000);
    }

    esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);

    if(esp_now_init() != ESP_OK)
    {
        logPrintf("ERRO: falha ao inicializar ESP-NOW.\n");
        while(true)
            delay(1000);
    }

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, BROADCAST_MAC, sizeof(BROADCAST_MAC));
    peerInfo.channel = ESPNOW_CHANNEL;
    peerInfo.encrypt = false;

    if(!esp_now_is_peer_exist(BROADCAST_MAC))
    {
        const esp_err_t result = esp_now_add_peer(&peerInfo);
        if(result != ESP_OK)
        {
            logPrintf("ERRO: falha ao adicionar peer broadcast ESP-NOW: %d\n", result);
            while(true)
                delay(1000);
        }
    }

    logPrintf("ESP-NOW pronto em broadcast FF:FF:FF:FF:FF:FF | canal %u | MAC STA %s\n",
              ESPNOW_CHANNEL,
              WiFi.macAddress().c_str());
}

void setup()
{
    Serial.begin(SERIAL_BAUD);
    delay(300);

    if(STATUS_LED_PIN >= 0)
    {
        pinMode(STATUS_LED_PIN, OUTPUT);
        digitalWrite(STATUS_LED_PIN, LOW);
    }

    logPrintf("\nBase Station USB Serial -> ESP-NOW Broadcast - VSSS\n");
    logPrintf("Serial binaria: %lu baud | pacote: %u bytes\n",
              static_cast<unsigned long>(SERIAL_BAUD),
              static_cast<unsigned>(PACKET_SIZE));

    setupEspNow();
}

void handleIncomingByte(uint8_t byteValue)
{
    // Re-sincronizacao simples para preservar fronteira de pacote no stream.
    if(packetIndex == 0 && byteValue != HEADER_0)
        return;

    if(packetIndex == 1 && byteValue != HEADER_1)
    {
        invalidHeaderCount++;
        packetIndex = (byteValue == HEADER_0) ? 1 : 0;
        return;
    }

    packetBuffer[packetIndex++] = byteValue;

    if(packetIndex < PACKET_SIZE)
        return;

    packetIndex = 0;

    if(!validatePacket(packetBuffer))
        return;

    const esp_err_t result = esp_now_send(BROADCAST_MAC, packetBuffer, PACKET_SIZE);
    if(result != ESP_OK)
    {
        espNowErrorCount++;
        logPrintf("ERRO ESP-NOW: falha ao enviar broadcast, codigo %d, contador PC %u\n",
                  result,
                  packetCounter(packetBuffer));
        return;
    }

    validPacketCount++;
    pulseLed();

    if(validPacketCount <= 3 || validPacketCount % LOG_EVERY_VALID_PACKETS == 0)
    {
        logPrintf("Pacote valido #%lu | contador PC %u | Serial USB -> ESP-NOW broadcast\n",
                  static_cast<unsigned long>(validPacketCount),
                  packetCounter(packetBuffer));
        printPacketHex(packetBuffer, PACKET_SIZE);
    }
}

void loop()
{
    updateLed();

    while(Serial.available() > 0)
        handleIncomingByte(static_cast<uint8_t>(Serial.read()));
}
