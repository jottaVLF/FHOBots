#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <cstring>

#if __has_include(<esp_arduino_version.h>)
#include <esp_arduino_version.h>
#endif

/*
 * ESP32 Robot Receiver - ESP-NOW broadcast
 *
 * Todos os robos recebem o mesmo pacote broadcast.
 * Cada robo filtra pelo ROBOT_ID e executa somente o proprio comando.
 *
 * Pacote esperado: exatamente o mesmo enviado pelo C++ do PC e repetido pela
 * base station:
 *   18 bytes, header 0xA5 0x5A, contador, 3 comandos, reservado, checksum XOR.
 */

// ==========================
// Configuracao obrigatoria
// ==========================
// Grave um firmware por robo alterando somente este ID:
//   robo 0 -> #define ROBOT_ID 0
//   robo 1 -> #define ROBOT_ID 1
//   robo 2 -> #define ROBOT_ID 2
#define ROBOT_ID 0

// Canal ESP-NOW fixo. Deve ser igual ao ESPNOW_CHANNEL da base station.
// Nao depende de roteador, SSID, senha ou IP.
static constexpr uint8_t ESPNOW_CHANNEL = 1;

// Ajuste estes pinos conforme a ponte H/motor do robo.
// A logica preserva o firmware antigo:
//   reverso:     pino A recebe PWM, pino B recebe 0
//   nao reverso: pino A recebe 0,   pino B recebe PWM
static constexpr int MOTOR_LEFT_A_PIN = 32;
static constexpr int MOTOR_LEFT_B_PIN = 33;
static constexpr int MOTOR_RIGHT_A_PIN = 25;
static constexpr int MOTOR_RIGHT_B_PIN = 26;

// LED opcional para pulso rapido quando este robo aplica comando valido.
// Use -1 para desabilitar.
static constexpr int STATUS_LED_PIN = 2;

// Se nenhum pacote valido para este ROBOT_ID chegar dentro desse tempo,
// os motores sao parados.
static constexpr uint32_t WATCHDOG_TIMEOUT_MS = 300;
static constexpr uint32_t LED_PULSE_MS = 35;

// =====================
// Protocolo VSSS ESP-NOW
// =====================
static constexpr uint8_t HEADER_0 = 0xA5;
static constexpr uint8_t HEADER_1 = 0x5A;
static constexpr int ROBOT_COUNT = 3;
static constexpr size_t PACKET_SIZE = 18;
static constexpr size_t CHECKSUM_INDEX = PACKET_SIZE - 1;
static constexpr uint8_t FLAG_LEFT_REVERSE = 0x01;
static constexpr uint8_t FLAG_RIGHT_REVERSE = 0x02;

struct RobotCommand
{
    uint8_t id;
    uint8_t pwmLeft;
    uint8_t pwmRight;
    uint8_t flags;
};

portMUX_TYPE packetMux = portMUX_INITIALIZER_UNLOCKED;
uint8_t latestPacket[PACKET_SIZE];
volatile bool packetReady = false;

volatile uint32_t invalidSizeCount = 0;
volatile uint32_t invalidHeaderCount = 0;
volatile uint32_t invalidChecksumCount = 0;
volatile uint32_t ignoredRobotPacketCount = 0;

uint32_t appliedPacketCount = 0;
uint32_t lastValidCommandMs = 0;
uint32_t ledOffAtMs = 0;
bool watchdogAlreadyStopped = true;
bool receivedCommandForThisRobot = false;

bool robotIdIsValid()
{
    return ROBOT_ID >= 0 && ROBOT_ID < ROBOT_COUNT;
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

bool validatePacket(const uint8_t *data, int length)
{
    if(length != static_cast<int>(PACKET_SIZE))
    {
        invalidSizeCount++;
        return false;
    }

    if(data[0] != HEADER_0 || data[1] != HEADER_1)
    {
        invalidHeaderCount++;
        return false;
    }

    if(data[CHECKSUM_INDEX] != calculateChecksum(data))
    {
        invalidChecksumCount++;
        return false;
    }

    return true;
}

bool extractCommandForThisRobot(const uint8_t *data, RobotCommand &command)
{
    if(!robotIdIsValid())
        return false;

    for(int robotIndex = 0; robotIndex < ROBOT_COUNT; robotIndex++)
    {
        const size_t offset = 4 + static_cast<size_t>(robotIndex) * 4;
        const uint8_t commandId = data[offset];
        if(commandId == static_cast<uint8_t>(ROBOT_ID))
        {
            command.id = commandId;
            command.pwmLeft = data[offset + 1];
            command.pwmRight = data[offset + 2];
            command.flags = data[offset + 3];
            return true;
        }
    }

    return false;
}

uint8_t saturatePwm(int value)
{
    if(value < 0)
        return 0;
    if(value > 255)
        return 255;
    return static_cast<uint8_t>(value);
}

void writeMotorPair(int pinA, int pinB, uint8_t pwm, bool reverse)
{
    const uint8_t safePwm = saturatePwm(pwm);

    if(safePwm == 0)
    {
        analogWrite(pinA, 0);
        analogWrite(pinB, 0);
        return;
    }

    if(reverse)
    {
        analogWrite(pinA, safePwm);
        analogWrite(pinB, 0);
    }
    else
    {
        analogWrite(pinA, 0);
        analogWrite(pinB, safePwm);
    }
}

void stopMotors()
{
    analogWrite(MOTOR_LEFT_A_PIN, 0);
    analogWrite(MOTOR_LEFT_B_PIN, 0);
    analogWrite(MOTOR_RIGHT_A_PIN, 0);
    analogWrite(MOTOR_RIGHT_B_PIN, 0);
}

void applyCommand(const RobotCommand &command)
{
    const bool reverseLeft = (command.flags & FLAG_LEFT_REVERSE) != 0;
    const bool reverseRight = (command.flags & FLAG_RIGHT_REVERSE) != 0;

    writeMotorPair(MOTOR_LEFT_A_PIN,
                   MOTOR_LEFT_B_PIN,
                   command.pwmLeft,
                   reverseLeft);
    writeMotorPair(MOTOR_RIGHT_A_PIN,
                   MOTOR_RIGHT_B_PIN,
                   command.pwmRight,
                   reverseRight);

    lastValidCommandMs = millis();
    receivedCommandForThisRobot = true;
    watchdogAlreadyStopped = false;
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

void handleWatchdog()
{
    if(!receivedCommandForThisRobot || watchdogAlreadyStopped)
        return;

    if(millis() - lastValidCommandMs > WATCHDOG_TIMEOUT_MS)
    {
        stopMotors();
        watchdogAlreadyStopped = true;
        Serial.printf("Watchdog: sem pacote valido por %lu ms. Motores parados.\n",
                      static_cast<unsigned long>(WATCHDOG_TIMEOUT_MS));
    }
}

#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
void onEspNowReceive(const esp_now_recv_info_t *recvInfo, const uint8_t *data, int length)
{
    (void)recvInfo;
#else
void onEspNowReceive(const uint8_t *macAddress, const uint8_t *data, int length)
{
    (void)macAddress;
#endif
    if(!validatePacket(data, length))
        return;

    portENTER_CRITICAL(&packetMux);
    memcpy(latestPacket, data, PACKET_SIZE);
    packetReady = true;
    portEXIT_CRITICAL(&packetMux);
}

void configureMotorPins()
{
    pinMode(MOTOR_LEFT_A_PIN, OUTPUT);
    pinMode(MOTOR_LEFT_B_PIN, OUTPUT);
    pinMode(MOTOR_RIGHT_A_PIN, OUTPUT);
    pinMode(MOTOR_RIGHT_B_PIN, OUTPUT);
    stopMotors();
}

void setupEspNow()
{
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.disconnect();
    delay(100);

    if(ESPNOW_CHANNEL < 1 || ESPNOW_CHANNEL > 13)
    {
        Serial.printf("ERRO: ESPNOW_CHANNEL invalido: %u\n", ESPNOW_CHANNEL);
        while(true)
            delay(1000);
    }

    esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);

    if(esp_now_init() != ESP_OK)
    {
        Serial.println("ERRO: falha ao inicializar ESP-NOW.");
        while(true)
            delay(1000);
    }

    if(esp_now_register_recv_cb(onEspNowReceive) != ESP_OK)
    {
        Serial.println("ERRO: falha ao registrar callback ESP-NOW.");
        while(true)
            delay(1000);
    }

    Serial.printf("ESP-NOW receiver broadcast pronto. ROBOT_ID=%d | canal fixo=%u | MAC STA=%s\n",
                  ROBOT_ID,
                  ESPNOW_CHANNEL,
                  WiFi.macAddress().c_str());
}

void setup()
{
    Serial.begin(115200);
    delay(300);

    if(STATUS_LED_PIN >= 0)
    {
        pinMode(STATUS_LED_PIN, OUTPUT);
        digitalWrite(STATUS_LED_PIN, LOW);
    }

    configureMotorPins();

    Serial.println();
    Serial.println("Robot Receiver ESP-NOW Broadcast - VSSS");

    if(!robotIdIsValid())
    {
        Serial.printf("ERRO: ROBOT_ID invalido (%d). Comandos nao serao executados.\n", ROBOT_ID);
        stopMotors();
    }

    setupEspNow();
}

void loop()
{
    updateLed();
    handleWatchdog();

    bool hasPacket = false;
    uint8_t packetCopy[PACKET_SIZE];

    portENTER_CRITICAL(&packetMux);
    if(packetReady)
    {
        memcpy(packetCopy, latestPacket, PACKET_SIZE);
        packetReady = false;
        hasPacket = true;
    }
    portEXIT_CRITICAL(&packetMux);

    if(!hasPacket)
        return;

    RobotCommand command = {};
    if(!extractCommandForThisRobot(packetCopy, command))
    {
        ignoredRobotPacketCount++;
        if(robotIdIsValid())
        {
            Serial.printf("Pacote valido ignorado: nao ha comando para ROBOT_ID=%d, contador PC %u\n",
                          ROBOT_ID,
                          packetCounter(packetCopy));
        }
        return;
    }

    applyCommand(command);
    appliedPacketCount++;
    pulseLed();

    Serial.printf("Comando aplicado #%lu | contador PC %u | robo %u | PWM L=%u R=%u | flags=0x%02X\n",
                  static_cast<unsigned long>(appliedPacketCount),
                  packetCounter(packetCopy),
                  command.id,
                  command.pwmLeft,
                  command.pwmRight,
                  command.flags);
}
