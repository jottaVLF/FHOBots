# VSSS Robotics System

Sistema de robótica para **VSSS (Very Small Size Soccer)**, desenvolvido em C++ com OpenCV e modernizado incrementalmente a partir de uma base legada iniciada em 2017.

O projeto controla robôs diferenciais por visão computacional: o PC detecta bola e robôs por marcadores coloridos, calcula estratégia e controle, envia comandos para uma ESP32 emissora via USB Serial, e a ESP32 retransmite os pacotes por ESP-NOW broadcast para os robôs.

## 📌 Visão Geral

Este sistema implementa a cadeia completa de controle de uma equipe VSSS:

- captura de câmera;
- detecção HSV da bola, aliados e adversários;
- tracking temporal para reduzir troca de identidade dos robôs;
- filtro de Kalman para bola e robôs;
- estratégia baseada em máquinas de estado;
- controle diferencial PD;
- comunicação em tempo real via ESP32 e ESP-NOW broadcast.

A arquitetura original foi preservada, com melhorias pontuais de estabilidade, comunicação e navegação.

## 🧠 Arquitetura do Sistema

Fluxo principal:

```text
Camera
  ↓
C++ / OpenCV
  ↓
Visão HSV + Tracking + Kalman
  ↓
Estado global sincronizado
  ↓
Estratégia por máquina de estados
  ↓
Controle diferencial PD
  ↓
USB Serial POSIX
  ↓
ESP32 emissora
  ↓
ESP-NOW broadcast FF:FF:FF:FF:FF:FF
  ↓
ESP32 dos robôs
  ↓
Filtro por ROBOT_ID + motores
```

O sistema usa múltiplas threads para visão, atacante e defensor. O acesso ao estado global e à comunicação é protegido por mutex para evitar condições de corrida.

## 🚀 Tecnologias Utilizadas

- C++17
- OpenCV
- POSIX Serial (`termios`)
- ESP32
- ESP-NOW
- Arduino framework para ESP32
- Threads C++ (`std::thread`)
- Mutex e atomic (`std::mutex`, `std::atomic`)
- Makefile

## ⚙️ Funcionalidades

- Detecção da bola por HSV.
- Detecção dos robôs por cor individual + cor comum do time.
- Tracking temporal dos robôs para reduzir troca de identidade.
- Rejeição de saltos impossíveis na visão.
- Kalman 2D para bola: posição e velocidade.
- Kalman 2D para robôs: posição e velocidade.
- Estratégia por máquinas de estado.
- Controle diferencial PD.
- Comunicação PC → ESP32 por USB Serial.
- Comunicação ESP32 → robôs por ESP-NOW broadcast.
- Watchdog nos robôs para parar motores em perda de pacote.
- Univector Field aplicado inicialmente no atacante, com fallback seguro.

## 🔌 Comunicação

A comunicação foi modernizada para usar uma ESP32 emissora conectada ao PC por USB.

```text
PC C++ / OpenCV
  → USB Serial
ESP32 Base Station
  → ESP-NOW broadcast
ESP32 Robôs
  → filtro por ROBOT_ID
```

Importante:

- Não há UDP/IP entre PC e ESP32 base station.
- ESP-NOW não usa IP para comunicar com os robôs.
- A base station transmite para `FF:FF:FF:FF:FF:FF`.
- Todos os robôs recebem o mesmo pacote.
- Cada robô executa apenas o comando correspondente ao seu `ROBOT_ID`.

### Protocolo de 18 bytes

| Byte | Conteúdo |
| --- | --- |
| 0 | Header `0xA5` |
| 1 | Header `0x5A` |
| 2 | Contador, byte baixo |
| 3 | Contador, byte alto |
| 4 | ID do robô 0 |
| 5 | PWM esquerdo do robô 0 |
| 6 | PWM direito do robô 0 |
| 7 | Flags do robô 0 |
| 8 | ID do robô 1 |
| 9 | PWM esquerdo do robô 1 |
| 10 | PWM direito do robô 1 |
| 11 | Flags do robô 1 |
| 12 | ID do robô 2 |
| 13 | PWM esquerdo do robô 2 |
| 14 | PWM direito do robô 2 |
| 15 | Flags do robô 2 |
| 16 | Reservado |
| 17 | Checksum XOR dos bytes `0..16` |

Flags:

- bit `0x01`: motor esquerdo em reverso;
- bit `0x02`: motor direito em reverso.

## 🧭 Navegação

O módulo de navegação atual implementa **Univector Field** apenas no atacante.

Características:

- atração em direção ao alvo;
- repulsão simples de obstáculos;
- obstáculos virtuais na área defensiva;
- abordagem da bola por trás em relação ao gol;
- suporte a chassi bidirecional, permitindo andar de ré;
- histerese e persistência para evitar alternância frente/ré;
- fallback para o comportamento antigo se a saída for inválida.

O Univector ainda não substitui a estratégia completa. Ele atua como camada auxiliar nos estados do atacante.

## 🛠️ Como Rodar o Projeto

### 🔹 Linux / WSL

Instale as dependências principais:

```bash
sudo apt update
sudo apt install build-essential make libopencv-dev
```

Configure a porta serial em:

```text
config/appConfig.json
```

Exemplo:

```json
"communication": "/dev/ttyUSB0"
```

Compile:

```bash
make clean
make
```

Execute:

```bash
./fhobotsTeam
```

Durante a inicialização, o sistema realiza calibrações de visão e campo antes de entrar no loop principal.

### 🔹 ESP32

Firmwares principais:

```text
firmware/base_station_espnow/base_station_espnow.ino
firmware/robot_receiver_espnow/robot_receiver_espnow.ino
```

Base station:

- gravar `base_station_espnow.ino` na ESP32 conectada ao PC;
- conferir `SERIAL_BAUD`;
- conferir `ESPNOW_CHANNEL`;
- manter o mesmo canal dos robôs.

Robôs:

- gravar `robot_receiver_espnow.ino` em cada ESP32 dos robôs;
- alterar `#define ROBOT_ID` para `0`, `1` ou `2`;
- configurar os pinos dos motores;
- usar o mesmo `ESPNOW_CHANNEL` da base station.

## 🧪 Testes

Teste simulado do Kalman:

```bash
make kalman-test
./bin/kalman2d_simulation
```

Teste simulado do Univector:

```bash
make univector-test
./bin/univector_simulation
```

Testes de bancada recomendados:

- robôs suspensos;
- verificar sentido dos motores;
- validar `ROBOT_ID`;
- validar watchdog sem pacotes;
- comparar Univector ligado/desligado.

Testes de campo recomendados:

- bola parada;
- bola em movimento;
- oclusão curta da bola;
- cruzamento de robôs;
- obstáculo entre atacante e bola;
- verificação de não entrada na área defensiva.

## 📂 Estrutura do Projeto

```text
communication/   Comunicação PC → ESP32 por USB Serial
config/          Configuração de câmera, comunicação, robôs e controle
control/         Controle diferencial PD
firmware/        Firmwares ESP32 para base station e robôs
model/           Modelos de robô, área, campo, vetores e mundo
navigation/      Univector Field
strategy/        Máquinas de estado de atacante, defensor e goleiro
tests/           Testes simulados auxiliares
vision/          Visão computacional, HSV, tracking e Kalman
```

## ⚠️ Limitações Atuais

- Depende de iluminação controlada.
- Calibração HSV ainda é manual.
- Univector Field está aplicado apenas no atacante.
- A estratégia original foi preservada e ainda possui comportamento legado.
- O filtro de adversários no Univector é uma média móvel simples.
- ESP-NOW exige que base station e robôs estejam no mesmo canal.
- Testes físicos ainda são necessários após cada ajuste de controle/navegação.

## 🔮 Próximos Passos

- Ajustar parâmetros do Univector em campo.
- Aplicar Univector no defensor depois da validação do atacante.
- Melhorar estratégia ofensiva e defensiva.
- Refinar calibração HSV.
- Melhorar tratamento de adversários com identidade temporal.
- Revisar organização da FSM legada.
- Ampliar testes automatizados sem câmera.

## 👨‍💻 Autor

Projeto VSSS legado modernizado incrementalmente.

Autor/equipe: **a definir**.
