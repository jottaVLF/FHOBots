# Relatorio de firmware ESP-NOW

Etapa 2 corrigida do projeto VSSS: o PC nao envia UDP/IP para a ESP32 base station. A arquitetura correta usa USB Serial entre o PC e a ESP32 emissora, e ESP-NOW broadcast entre a ESP32 emissora e os robos.

## Estrutura criada/atualizada

```text
communication/
  CommunicationESPNow.hpp
  CommunicationESPNow.cpp

firmware/
  base_station_espnow/
    base_station_espnow.ino
  robot_receiver_espnow/
    robot_receiver_espnow.ino

config/
  appConfig.json
```

Nenhum firmware antigo foi removido. O firmware antigo continua em:

```text
firmware/TesteCommander/TesteCommander.ino
```

## Arquitetura correta

```text
[PC C++ / OpenCV]
    -> USB Serial
[ESP32 Emissor plugado no PC]
    -> ESP-NOW broadcast FF:FF:FF:FF:FF:FF
[ESP32 Robos]
    -> todos recebem
    -> cada um filtra pelo ROBOT_ID
```

Pontos importantes:

- ESP-NOW nao usa IP, porta UDP, roteador ou endereco `192.168.x.x`;
- o PC fala apenas com a ESP32 emissora pela USB Serial;
- a ESP32 emissora retransmite o pacote por ESP-NOW broadcast;
- o endereco ESP-NOW usado pela base e `FF:FF:FF:FF:FF:FF`;
- nao ha MAC fixo dos robos;
- nao ha pareamento individual por robo;
- todos os robos recebem o mesmo pacote;
- cada robo executa somente o comando cujo `id` corresponde ao seu `ROBOT_ID`.

## C++ no PC

Arquivos:

```text
communication/CommunicationESPNow.hpp
communication/CommunicationESPNow.cpp
main.cpp
config/appConfig.json
```

`CommunicationESPNow` deixou de usar socket UDP e passou a usar serial POSIX simples no Linux:

- abre a porta configurada, por exemplo `/dev/ttyUSB0` ou `/dev/ttyACM0`;
- configura `115200 8N1`;
- nao usa LibSerial;
- nao usa XBee;
- nao usa UDP/IP;
- envia exatamente os mesmos 18 bytes do protocolo atual.

Configuracao atual:

```json
"communication" : "/dev/ttyUSB0"
```

Se a ESP32 aparecer como `/dev/ttyACM0`, alterar esse campo em `config/appConfig.json`.

## ESP32 emissora

Arquivo:

```text
firmware/base_station_espnow/base_station_espnow.ino
```

Funcionamento:

1. inicializa USB Serial em `115200`;
2. inicializa Wi-Fi apenas em modo `WIFI_STA`, necessario internamente para ESP-NOW;
3. nao conecta em SSID;
4. nao abre UDP;
5. fixa o canal ESP-NOW configurado;
6. le o stream binario da Serial USB;
7. ressincroniza pelo header `0xA5 0x5A`;
8. monta pacotes de 18 bytes;
9. valida header e checksum XOR;
10. ignora pacotes invalidos;
11. retransmite pacote valido, sem alterar bytes, para `FF:FF:FF:FF:FF:FF`;
12. pisca LED opcional em pacote valido.

Configuracoes principais no topo do arquivo:

```cpp
static constexpr uint32_t SERIAL_BAUD = 115200;
static constexpr bool ENABLE_SERIAL_LOGS = true;
static constexpr uint32_t LOG_EVERY_VALID_PACKETS = 50;
static constexpr uint8_t ESPNOW_CHANNEL = 1;
static constexpr int STATUS_LED_PIN = 2;
```

Observacao sobre logs: os logs usam a mesma USB Serial. Pacotes validos sao logados de forma amostrada para reduzir carga na porta; erros continuam sendo registrados. Para deixar a porta silenciosa, usar:

```cpp
static constexpr bool ENABLE_SERIAL_LOGS = false;
```

## Robos receptores

Arquivo:

```text
firmware/robot_receiver_espnow/robot_receiver_espnow.ino
```

Funcionamento:

1. configura a ESP32 em modo `WIFI_STA`;
2. nao conecta em roteador;
3. fixa `ESPNOW_CHANNEL`;
4. inicializa ESP-NOW como receiver;
5. recebe pacotes broadcast;
6. valida tamanho, header e checksum XOR;
7. procura o comando cujo `id` corresponde ao `ROBOT_ID`;
8. aplica somente esse comando;
9. ignora pacotes invalidos ou sem comando para o robo;
10. para motores por watchdog se parar de receber comando valido.

Configuracoes principais no topo do arquivo:

```cpp
#define ROBOT_ID 0
static constexpr uint8_t ESPNOW_CHANNEL = 1;

static constexpr int MOTOR_LEFT_A_PIN = 32;
static constexpr int MOTOR_LEFT_B_PIN = 33;
static constexpr int MOTOR_RIGHT_A_PIN = 25;
static constexpr int MOTOR_RIGHT_B_PIN = 26;

static constexpr uint32_t WATCHDOG_TIMEOUT_MS = 300;
```

Para cada robo, alterar:

- robo 0: `#define ROBOT_ID 0`;
- robo 1: `#define ROBOT_ID 1`;
- robo 2: `#define ROBOT_ID 2`.

O `ESPNOW_CHANNEL` da base e dos robos deve ser igual. Esse canal e fixo e nao depende de roteador.

## Controle dos motores

A logica preserva a ideia do firmware antigo:

- motor esquerdo: pinos `32` e `33`;
- motor direito: pinos `25` e `26`;
- se o bit de reversao estiver ativo, o pino A recebe PWM e o pino B recebe `0`;
- se o bit de reversao estiver inativo, o pino A recebe `0` e o pino B recebe PWM.

PWM e saturado para a faixa `0..255` antes da escrita.

## Segurancas implementadas

ESP32 emissora:

- ressincroniza a leitura pelo header `0xA5 0x5A`;
- monta somente pacotes de 18 bytes;
- valida checksum XOR;
- ignora pacote invalido;
- retransmite somente pacote valido por ESP-NOW broadcast.

Robos:

- ignoram pacote com tamanho diferente de `18`;
- ignoram header invalido;
- ignoram checksum XOR incorreto;
- ignoram comando sem `ROBOT_ID` correspondente;
- nao executam comando se `ROBOT_ID` for invalido;
- saturam PWM em `0..255`;
- watchdog para motores apos `WATCHDOG_TIMEOUT_MS` sem pacote valido para o robo.

## Formato do pacote

O protocolo de 18 bytes nao foi alterado.

| Byte(s) | Campo | Descricao |
|---|---|---|
| 0 | `HEADER_0` | `0xA5` |
| 1 | `HEADER_1` | `0x5A` |
| 2 | contador LSB | byte baixo do contador |
| 3 | contador MSB | byte alto do contador |
| 4 | robo 0 id | esperado `0` |
| 5 | robo 0 PWM esquerdo | `0..255` |
| 6 | robo 0 PWM direito | `0..255` |
| 7 | robo 0 flags | bits de direcao |
| 8 | robo 1 id | esperado `1` |
| 9 | robo 1 PWM esquerdo | `0..255` |
| 10 | robo 1 PWM direito | `0..255` |
| 11 | robo 1 flags | bits de direcao |
| 12 | robo 2 id | esperado `2` |
| 13 | robo 2 PWM esquerdo | `0..255` |
| 14 | robo 2 PWM direito | `0..255` |
| 15 | robo 2 flags | bits de direcao |
| 16 | reservado | atualmente `0x00` |
| 17 | checksum | XOR dos bytes `0..16` |

Flags por robo:

| Bit | Mascara | Significado |
|---|---|---|
| 0 | `0x01` | motor esquerdo reverso |
| 1 | `0x02` | motor direito reverso |
| 2-7 | `0xFC` | reservado |

Checksum:

```text
checksum = byte0 XOR byte1 XOR ... XOR byte16
```

## Como gravar

Arduino IDE:

1. abrir `firmware/base_station_espnow/base_station_espnow.ino`;
2. configurar `ESPNOW_CHANNEL` da base;
3. gravar na ESP32 emissora plugada no PC;
4. abrir `firmware/robot_receiver_espnow/robot_receiver_espnow.ino`;
5. configurar `ROBOT_ID`, `ESPNOW_CHANNEL` e pinos dos motores;
6. gravar um robo por vez.

Arduino CLI, exemplo:

```bash
arduino-cli compile --fqbn esp32:esp32:esp32 firmware/base_station_espnow
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32 firmware/base_station_espnow

arduino-cli compile --fqbn esp32:esp32:esp32 firmware/robot_receiver_espnow
arduino-cli upload -p /dev/ttyUSB1 --fqbn esp32:esp32:esp32 firmware/robot_receiver_espnow
```

Trocar as portas pelas portas reais.

## Teste manual PC -> Serial -> ESP-NOW -> robo

1. gravar a ESP32 emissora com `base_station_espnow.ino`;
2. gravar os robos com `robot_receiver_espnow.ino`;
3. garantir que `ESPNOW_CHANNEL` e igual na base e nos robos;
4. configurar `config/appConfig.json` com a porta serial da ESP32 emissora, por exemplo `/dev/ttyUSB0`;
5. dar permissao de acesso a serial se necessario, por exemplo adicionando o usuario ao grupo `dialout`;
6. levantar os robos da mesa ou desconectar carga mecanica no primeiro teste;
7. executar o programa C++ atual;
8. confirmar no Serial/log da base que pacotes validos chegam pela USB Serial;
9. confirmar que a base retransmite para ESP-NOW broadcast;
10. confirmar no Serial do robo que apenas o comando do seu `ROBOT_ID` e aplicado;
11. parar o C++ e verificar se o watchdog para os motores em cerca de `300 ms`.

## Pendencias de hardware

- confirmar qual porta Linux a ESP32 emissora recebeu: `/dev/ttyUSB0`, `/dev/ttyACM0`, etc.;
- confirmar permissao de acesso a porta serial;
- confirmar canal fixo ESP-NOW usado por todos os ESP32;
- confirmar pinos finais dos motores em cada robo;
- confirmar se a ponte H usa a mesma polaridade do firmware antigo;
- confirmar se o LED onboard e realmente GPIO `2` nas placas usadas;
- testar alimentacao dos motores com rodas suspensas antes de colocar em campo;
- validar se `analogWrite()` esta disponivel na versao do core ESP32 instalada no Arduino IDE.

## Riscos e observacoes

- ESP-NOW nao usa IP; qualquer configuracao de IP/UDP nao participa desta etapa.
- Broadcast ESP-NOW nao entrega ACK individual por robo, entao a base sabe que enviou, mas nao sabe qual robo recebeu.
- Base e robos precisam estar no mesmo canal ESP-NOW fixo.
- Ao abrir a porta serial, algumas placas ESP32 reiniciam; os primeiros pacotes podem ser perdidos durante o boot, mas o loop do C++ continua enviando.
- Se logs da base atrapalharem alguma ferramenta de teste serial, desabilitar `ENABLE_SERIAL_LOGS`.
- O watchdog esta no robo; se o PC parar de enviar pela Serial ou a base cair, o robo para por timeout local.
- O protocolo de 18 bytes nao foi alterado.
- Visao, estrategia, controle, Kalman e Univector nao foram alterados nesta etapa.
