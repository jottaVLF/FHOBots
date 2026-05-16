# Visao geral do projeto

Este repositorio contem um sistema legado de VSSS (Very Small Size Soccer) escrito principalmente em C++ para Linux. O programa principal controla uma equipe de tres robos por meio de:

- captura de camera com OpenCV;
- calibracao manual de campo, lente e cores HSV;
- deteccao da bola, das cores individuais dos robos e da cor do time;
- atualizacao de um modelo global de mundo;
- maquinas de estado para atacante, defensor e goleiro;
- controle PD diferencial para gerar PWM de roda esquerda/direita;
- comunicacao com os robos por UDP/Wi-Fi no fluxo ativo atual, com uma implementacao serial/LibSerial ainda presente;
- firmware Arduino/ESP32 de teste para receber UDP e acionar motores.

O desenho geral e fortemente baseado em estado global: `Global.*` concentra posicoes, areas do campo, robos, bola, comunicacao e flags de teclado. A visao escreve nesses globais; as threads dos robos leem esses dados e escrevem comandos de motor; o loop principal envia o pacote acumulado para os robos.

Linguagem principal: C++.

Padrao de compilacao no `Makefile`: mistura `-std=c++11` no `INCLUDE` e `-std=c++17` em `CXXFLAGS`; na pratica os objetos usam `g++ -std=c++17 -pedantic -O3`.

Frameworks/bibliotecas principais:

- OpenCV 4: camera, janelas, mouse, trackbars, processamento HSV, morfologia, contornos e desenho.
- LibSerial: comunicacao serial legada via XBee/porta `/dev/ttyACM0`.
- POSIX sockets: comunicacao UDP/Wi-Fi em Linux.
- nlohmann/json 3.12.0: vendorizado em `communication/json.hpp`, usado para calibracoes recebidas por UDP e alguns parametros do atacante.
- Biblioteca padrao C++: threads, mapas, vetores, arquivos, matematica.
- Arduino/ESP32: `firmware/TesteCommander/TesteCommander.ino` usa `WiFi.h` e `WiFiUdp.h`.

Branch analisada: `desenvolvimento`.

# Como executar o programa atualmente

O caminho previsto pelo repositorio e Linux, nao Windows. O `Makefile` assume `make`, `g++`, OpenCV instalado em `/usr/local/include/opencv4` e `/usr/local/lib`, LibSerial em `/usr/include/libserial` e bibliotecas POSIX. No terminal atual, `make` nao esta disponivel, entao nao fiz build nem executei o programa para nao alterar binarios/objetos.

Com as dependencias instaladas no ambiente esperado, o fluxo indicado e:

```bash
make
./fhobotsTeam
```

ou:

```bash
make run
```

Ao iniciar, o programa:

1. le `config/appConfig.json`;
2. abre a camera configurada em `camera`, hoje `0`;
3. cria comunicacao UDP broadcast para `192.168.0.255:8089`;
4. cria um servidor UDP de calibracao em `192.168.0.108:8888`;
5. inicializa as maquinas de estado dos robos;
6. abre janelas OpenCV para calibracao de olho de peixe, campo e cores;
7. entra no loop de deteccao, exibicao e envio de comandos.

Controles observados no codigo:

- `espaco`: avanca etapas de calibracao e tambem tira robos do estado `idle` para `seeking`.
- `p`: retorna estados para `idle`.
- `Esc`: encerra o loop principal.

Observacoes praticas:

- O executavel `fhobotsTeam` e a pasta `bin/` existem no workspace, mas sao artefatos de build ignorados pelo git.
- O programa depende de camera fisica, janelas OpenCV e rede no range codificado.
- A comunicacao serial existe, mas esta comentada no `main.cpp`; o caminho ativo e Wi-Fi/UDP.
- A configuracao de hardware de `config/appConfig.json` nao parece ser enviada no fluxo Wi-Fi atual; ela era usada pelo caminho serial `configureRobots`, hoje comentado.

# Estrutura de arquivos

```text
.
├── main.cpp                         Entrada principal do programa
├── Global.hpp / Global.cpp          Estado global do jogo e utilitarios
├── Makefile                         Build Linux com g++, OpenCV, LibSerial
├── Coordinates.hpp                  Struct generico simples x/y
├── FakePhysics.*                    Simulacao simples, aparentemente legado/teste
├── config/
│   ├── appConfig.json               Configuracao de camera, time e robos
│   ├── Config.*                     Estruturas de configuracao
│   ├── ConfigParser.*               Parser manual do JSON de config
│   └── Token.*                      Token usado pelo parser manual
├── vision/
│   ├── Vision.*                     Captura, calibracao, deteccao e overlay
│   ├── ColorDetection.*             Filtro HSV, morfologia, contornos
│   ├── CustomTrackbar.*             Janelas/trackbars e arquivos de cor
│   ├── Mouse.*                      Selecao de retangulos do campo
│   ├── Field.*                      Esta em `model/`, mas apoia calibracao do campo
│   ├── IVision.hpp                  Interface de visao
│   └── FakeVision.*                 Simulador visual legado/incompleto
├── model/
│   ├── Vector2D.*                   Vetores, operadores e angulos
│   ├── Area.*                       Retangulos/areas do campo
│   ├── Field.*                      Selecao/desenho de areas via mouse
│   ├── Robot.*                      Classe base dos robos e controle PWM
│   ├── AttackerRobot.*              Robo atacante e seus estados
│   ├── DefenderRobot.*              Robo defensor e seus estados
│   ├── GoalkeeperRobot.*            Robo goleiro e seus estados
│   └── WorldModel.*                 Consultas geometricas e regras de campo
├── strategy/
│   ├── State.hpp                    Interface base de estado
│   ├── MachineState.*               Maquina de estados
│   ├── basic/                       Estados comuns: idle, backoff, exit area
│   ├── attacker/                    Estados especificos do atacante
│   ├── defender/                    Estados especificos do defensor
│   └── goalkeeper/                  Estados especificos do goleiro
├── control/
│   └── Control.*                    Controle PD e saturacao de PWM
├── communication/
│   ├── ICommunication.hpp           Interface de comunicacao
│   ├── Communication.*              Serial/LibSerial legado
│   ├── CommunicationWIFI.*          UDP broadcast ativo
│   ├── CommunicationWIFICalibration.* Servidor UDP de calibracao
│   ├── jsonRead.*                   Atualiza `calibrations.json`
│   └── json.hpp                     nlohmann/json vendorizado
├── logging/
│   └── Logger.*                     Logger simples em arquivo
├── firmware/
│   └── TesteCommander.ino           Receptor UDP ESP32/Arduino
├── files/
│   ├── colors/*.txt                 Limiares HSV por cor
│   └── logs/Vision.log              Log da visao
├── calibrations.json                Parametros dinamicos do atacante
├── fieldsSaves.txt                  Dados binarios salvos das areas do campo
└── fieldsCoeficients.txt            Dados binarios salvos da lente/camera
```

# Fluxo principal do sistema

1. Entrada:
   - `main.cpp` chama `ConfigParser::createConfiguration()` para carregar `config/appConfig.json`.
   - Cria `Vision(configuration.camera, &configuration)`.
   - Cria `Global::communication = new CommunicationWIFI("192.168.0.255", 8089)`.
   - Cria `CommunicationWIFICalibration server("192.168.0.108", 8888)` e uma thread para `receiveMessage()`.

2. Inicializacao:
   - `initializeModelAndStates()` chama `createMachineStates()` nos tres robos globais.
   - Zera posicoes iniciais e bola.
   - Chama `Global::communication->stopAll()`.

3. Calibracao antes do jogo:
   - `vision->calibratePreProcess()` tenta ler `fieldsCoeficients.txt`, abre trackbars para coeficientes de lente e aguarda `espaco`.
   - `vision->adjustFieldPosition()` le `fieldsSaves.txt`, permite selecionar/corrigir campo, areas e gols com mouse, salva de volta e calcula `Global::frameCentimetersConstant`.
   - `vision->calibration()` calibra sequencialmente bola, cores individuais dos robos, cor do time e cor adversaria; cada cor usa `ColorDetection` e `CustomTrackbar`.

4. Loop de jogo:
   - `main.cpp` inicia threads para atacante e defensor: `Robot::updateRobot()`.
   - A thread do goleiro esta comentada.
   - A cada frame:
     - `vision->detectionColors()` captura imagem, corrige distorcao, corta no campo, detecta bola/robos/adversarios e atualiza `Global`.
     - `cv::waitKey(1)` atualiza `Global::bufferKeyboard`.
     - `Vision::show()` desenha overlays de estado, objetivo, PWM, erro e zona segura.
     - `Global::communication->sendMessage()` envia o buffer UDP para os robos.

5. Deteccao da bola:
   - `ColorDetection("ball")` filtra HSV, aplica erosao/dilatacao, encontra contornos e calcula centro por momentos.
   - `Vision::detectionColors()` grava `Global::ballPos`, `Global::lastBallPos`, `Global::ballVel`.
   - `Global::ball` pode ser uma previsao: posicao atual mais velocidade multiplicada por 10, com correcoes se sair do campo.

6. Deteccao dos robos:
   - Cada robo tem uma cor individual (`r0.color`, `r1.color`, `r2.color`) e todos compartilham a cor do time (`team-color`).
   - Para cada robo, `Vision::calculatePosRobot()` escolhe o par mais proximo entre marcador individual e marcador do time.
   - A posicao do robo e a media dos dois marcadores.
   - A orientacao e o vetor do marcador do time para o marcador individual.
   - O papel vem da string `role`; o codigo espera `attacker`, `deffender` ou qualquer outro valor como goleiro.

7. Estrategia:
   - Cada robo possui uma `MachineState`.
   - `Robot::updateRobot()` roda em loop chamando `_machineState.think()`.
   - `think()` executa `doActions()`, consulta `checkConditions()` e troca de estado se houver novo nome.
   - Os estados calculam destino/PWM e chamam `Global::communication->writeMessage(...)`.

8. Controle:
   - `Robot::calculatePwm()` calcula vetor destino - posicao.
   - `Control::calculatePwm()` aplica controle PD sobre erro angular.
   - PWM esquerdo/direito sao saturados por minimo/maximo.
   - Acoes especiais (`spinClockWise`, `moveBackward`, etc.) setam flags `reverseLeft`/`reverseRight`.

9. Comunicacao:
   - `CommunicationWIFI::writeMessage()` escreve em `_writeBuffer`: byte inicial `0x5B`, byte de flags de reversao, pares de PWM por robo.
   - `sendMessage()` define `_writeBuffer[8] = 1` e envia 255 bytes por UDP.
   - O firmware le `packetBuffer[id*2+2]` e `packetBuffer[id*2+3]`, e usa `packetBuffer[1]` como mascara de reversao.

# Modulos identificados

## Visao computacional

Arquivos: `vision/Vision.*`, `vision/ColorDetection.*`, `vision/CustomTrackbar.*`, `vision/Mouse.*`, `model/Field.*`.

Responsabilidades:

- abrir camera;
- aplicar correcao de lente;
- cortar frame pelo retangulo do campo;
- calibrar HSV com trackbars;
- detectar blobs por cor;
- calcular bola, robos e adversarios;
- desenhar interface OpenCV de debug.

## Deteccao de robos

Arquivos centrais: `vision/Vision.cpp`, `vision/ColorDetection.cpp`.

Metodo usado:

- uma cor individual identifica cada robo;
- uma cor comum identifica o time;
- o par individual/time mais proximo define posicao e orientacao.

Fragilidade importante: se houver multiplos blobs, reflexos ou perda de marcador, o emparelhamento pelo menor par pode trocar robo ou orientacao.

## Deteccao da bola

Arquivos centrais: `vision/Vision.cpp`, `vision/ColorDetection.cpp`, `files/colors/Ball.txt`.

Metodo usado:

- filtro HSV;
- morfologia;
- contornos;
- centro por momentos;
- previsao simples por velocidade de frame anterior.

## Identificacao de cores/time

Arquivos: `config/appConfig.json`, `vision/CustomTrackbar.*`, `files/colors/*.txt`.

O time atual e `blue`; adversario e calculado como `yellow` quando time e azul, e `blue` caso contrario. As cores individuais atuais sao `purple`, `green` e `red`.

## Estrategia

Arquivos: `strategy/*`, `model/*Robot.*`, `model/WorldModel.*`.

Cada robo tem uma maquina de estados. Estados principais:

- Atacante: `idle`, `backoff`, `attacking`, `seeking`, `waiting`, `spinning`, `align`, `exit`, `joystick`.
- Defensor: `idle`, `backoff`, `seeking`, `waiting`, `kicking`, `align`, `exit`, `spinning`.
- Goleiro: `idle`, `seeking`, `kicking`, `backoff`, `moveback`, `moveforward`, `spinning`, `align`, `waiting`, `return`, `exit`.

No `main.cpp` atual, apenas atacante e defensor rodam em threads; goleiro nao roda estrategia.

## Controle dos robos

Arquivos: `control/Control.*`, `model/Robot.*`.

Controle PD angular com PWM base, minimo e maximo. Nao ha controle de velocidade por realimentacao dos robos; a posicao vem da visao.

## Comunicacao serial/radio

Arquivos:

- `communication/Communication.*`: serial/LibSerial, handshake/configuracao por XBee, atualmente comentado no `main.cpp`.
- `communication/CommunicationWIFI.*`: UDP/Wi-Fi ativo.
- `firmware/TesteCommander/TesteCommander.ino`: receptor UDP no robo/ESP32.

## Interface grafica

Nao ha GUI dedicada. A interface e OpenCV:

- janelas `Game original`, `Game resized`;
- trackbars por cor;
- callback de mouse para selecionar retangulos;
- textos e linhas sobrepostos ao frame.

## Calibracao

Arquivos:

- `files/colors/*.txt`: limites HSV por cor.
- `fieldsSaves.txt`: retangulos de campo/areas/gols em formato binario.
- `fieldsCoeficients.txt`: coeficientes binarios de lente.
- `calibrations.json`: parametros PD/PWM de estados do atacante.
- `communication/jsonRead.*`: recebe JSON por UDP e atualiza `calibrations.json`.

## Configuracao

Arquivos: `config/appConfig.json`, `config/Config.*`, `config/ConfigParser.*`, `config/Token.*`.

O parser e manual, especifico para o formato atual, e nao usa `nlohmann/json`.

## Logs/debug

Arquivos: `logging/Logger.*`, `files/logs/Vision.log`, diversos `std::cout` espalhados.

`Vision::detectionColors()` registra a bola em todo frame, o que pode impactar desempenho e crescer arquivo rapidamente.

# Arquivos criticos

| Arquivo | Funcao no sistema | Classes/funcoes principais | Usa/importa | Estado aparente |
|---|---|---|---|---|
| `main.cpp` | Entrada, montagem de config/visao/comunicacao/calibracao/threads/loop | `main`, `initializeModelAndStates` | `ConfigParser`, `Vision`, `CommunicationWIFI`, `CommunicationWIFICalibration`, `Robot`, `Global` | Ativo e critico |
| `Global.hpp/.cpp` | Estado global compartilhado, areas, bola, robos e helpers geometricos/PWM | `Global`, `robotNearBall`, `robotNearRobot`, `isInsideOwnArea`, `pwm*` | `Vector2D`, `Area`, robos, comunicacao, controle | Ativo, central e fragil |
| `Makefile` | Build Linux | `all`, `clean`, `run` | OpenCV, LibSerial, pthread | Ativo, mas dependente de Linux |
| `config/appConfig.json` | Camera, porta serial, cor do time, papel/cor/hardware/controle dos robos | Dados `camera`, `team-color`, `r0..r2` | Lido por `ConfigParser` | Ativo |
| `config/Config.*` | Estruturas de config | `Config`, `RobotConfig`, `HardwareConfig`, `ControlConfig` | STL string | Ativo |
| `config/ConfigParser.*` | Parser manual do arquivo de config | `createConfiguration`, `getRobotConfig`, `getPwmConfig`, `getControlConstants`, `getToken` | `Token`, `Config`, `fstream` | Ativo, fragil |
| `vision/Vision.*` | Camera, calibracao, deteccao, overlay e atualizacao do mundo | `Vision`, `adjustFieldPosition`, `calibration`, `detectionColors`, `calculatePosRobot`, `readFrame`, `show` | OpenCV, `ColorDetection`, `Global`, `WorldModel`, `Logger`, `Config` | Ativo e muito critico |
| `vision/ColorDetection.*` | Segmentacao HSV e blobs por cor | `detect`, `filter`, `morphologicOperations`, `setPosition`, `getPositionsColor` | `CustomTrackbar`, OpenCV, `Global` | Ativo |
| `vision/CustomTrackbar.*` | Carrega/salva HSV e exibe sliders | `loadColors`, `saveColors`, `initTrackBar`, `exitTrackBar` | OpenCV, arquivos `files/colors` | Ativo |
| `vision/Mouse.*` | Callback de mouse para retangulos | `mouseHandler`, `mouseCallback`, `getRect` | OpenCV | Ativo via `Field` |
| `model/Field.*` | Desenho/selecao de areas do campo | `draw`, `setWindow`, `getRect`, `setRect` | `Mouse`, `Area`, OpenCV | Ativo na calibracao |
| `model/Robot.*` | Base dos robos, controle e thread de estado | `updateRobot`, `calculatePwm`, `moveForward`, `spinClockWise`, `getMessage` | `Control`, `MachineState`, `Global` | Ativo e critico |
| `model/AttackerRobot.*` | Cria estados do atacante e define id de mensagem 0 | `createMachineStates`, `setJoystickState` | Estados de atacante e basicos | Ativo |
| `model/DefenderRobot.*` | Cria estados do defensor e define id 1 | `createMachineStates` | Estados de defensor/basicos | Ativo; contem duplicidade de estado `spinning` |
| `model/GoalkeeperRobot.*` | Cria estados do goleiro e define id 2 | `createMachineStates` | Estados de goleiro/basicos | Parcial: estados criados, thread comentada |
| `model/WorldModel.*` | Regras geometricas do jogo/campo | `isOnDeffenseField`, `nearstRobotTo`, `getGoalKeeperDeffencePosition`, `getDeffenderDeffencePosition`, `isStuckAtDeffenseGoal` | `Global`, `Robot`, `Area`, `Vector2D` | Ativo e critico |
| `model/Vector2D.*` | Matematica 2D | operadores, `magnitude`, `angle`, `angleBetween`, `normalize` | `cmath` | Ativo |
| `model/Area.*` | Retangulos/areas | `isInside`, `isOnLeft`, `getCenter`, `set`, `copy` | `Vector2D`, OpenCV | Ativo |
| `control/Control.*` | Controle PD e PWM | `calculatePwm`, `calculatePD`, `calculateError`, setters/getters | `Vector2D`, `cmath` | Ativo |
| `strategy/State.hpp` | Interface de estado | `doActions`, `checkConditions`, `entryActions`, `exitActions` | STL string | Ativo |
| `strategy/MachineState.*` | Gerencia transicoes de estados | `addState`, `setState`, `think`, `currentState` | `State`, `map` | Ativo |
| `strategy/basic/StateIdle.*` | Estado parado; start com espaco | `checkConditions`, `entryActions` | `Global`, `Robot` | Ativo |
| `strategy/basic/StateBackOff.*` | Recuo quando alinhado com parede/risco | `doActions`, `checkConditions` | `WorldModel`, `Global` | Ativo |
| `strategy/basic/StateExitArea.*` | Saida de area para robos de linha | `doActions`, `checkConditions`, `entryActions` | `Global`, `Robot` | Ativo |
| `strategy/attacker/AttackerStateSeeking.*` | Atacante busca bola | `doActions`, `checkConditions`, `entryActions` | `Global`, `WorldModel`, `json.hpp` | Ativo; parametros em caminho absoluto |
| `strategy/attacker/AttackerStateAttacking.*` | Atacante segue para gol | `doActions`, `checkConditions`, `entryActions` | `Global`, `WorldModel`, `json.hpp` | Ativo; parametros em caminho absoluto |
| `strategy/attacker/AttackerStateWaiting.*` | Atacante espera quando outro robo deve agir | `doActions`, `checkConditions` | `Global`, `WorldModel` | Ativo |
| `strategy/attacker/AttackerStateAlign.*` | Alinha atacante com bola | `doActions`, `checkConditions` | `Global`, `WorldModel` | Ativo |
| `strategy/attacker/AttackerStateSpinning.*` | Giro do atacante perto da bola/parede | `doActions`, `checkConditions` | `Global`, `WorldModel` | Ativo |
| `strategy/attacker/AttackerJoystickControl.*` | Controle especial por destino de joystick | `doActions`, `checkConditions` | `Global` | Parcial/experimental; depende de `Global::joystickDestination` |
| `strategy/defender/DefenderStateSeeking.*` | Defensor vai a posicao defensiva ou bola | `doActions`, `checkConditions` | `Global`, `WorldModel` | Ativo |
| `strategy/defender/DefenderStateWaiting.*` | Defensor parado na posicao objetivo | `doActions`, `checkConditions` | `Global`, `WorldModel` | Ativo |
| `strategy/defender/DefenderStateKicking.*` | Defensor chuta/segue gol | `doActions`, `checkConditions` | `Global`, `WorldModel` | Ativo |
| `strategy/defender/DefenderStateAlign.*` | Alinha defensor com bola | `doActions`, `checkConditions` | `Global` | Ativo |
| `strategy/defender/DefenderStateSpinning.*` | Giro do defensor | `doActions`, `checkConditions` | `Global`, `WorldModel` | Ativo; substitui outro `spinning` no mapa |
| `strategy/goalkeeper/*` | Estados do goleiro | `Seeking`, `Waiting`, `MoveBack`, `MoveForward`, `Spinning`, `ReturnToArea`, `ExitGoal`, etc. | `Global`, `WorldModel`, `Robot` | Parcial: compilado, mas thread do goleiro esta comentada |
| `communication/ICommunication.hpp` | Interface comum de comunicacao | `writeMessage`, `sendMessage`, `stopAll`, `getLeftPwm`, `getRightPwm` | `Config` | Ativo |
| `communication/CommunicationWIFI.*` | Comunicacao UDP ativa | `writeMessage`, `sendMessage`, `stopAll` | POSIX sockets, `ICommunication` | Ativo e critico |
| `communication/CommunicationWIFICalibration.*` | Servidor UDP para atualizar calibracoes | `receiveMessage` | POSIX sockets, `jsonRead` | Ativo no `main.cpp`; loop infinito |
| `communication/jsonRead.*` | Consolida JSON recebido em `calibrations.json` | `loadData`, `saveData`, `handlePacket` | `nlohmann/json`, arquivos | Ativo no servidor de calibracao |
| `communication/Communication.*` | Comunicacao serial/LibSerial e configuracao XBee | `writeMessage`, `sendMessage`, `configureRobots`, `readMessage` | LibSerial, `Global`, `Config` | Legado/inativo no fluxo atual |
| `communication/json.hpp` | Biblioteca JSON vendorizada | `nlohmann::json` | Header-only | Dependencia vendorizada |
| `logging/Logger.*` | Log em arquivo | `logInformation`, `logError`, `logWarning` | `fstream`, `ctime` | Ativo na visao |
| `firmware/TesteCommander/TesteCommander.ino` | Firmware de teste UDP/motor | `setup`, `loop`, `motorValue` | ESP32 Wi-Fi/UDP | Ativo no lado embarcado/teste; contem configuracoes hardcoded |
| `FakePhysics.*` | Atualiza posicoes por modelo fisico simples | `Physics::updateRobots`, `updateRobot` | `Global`, `Robot` | Legado/teste; nao chamado no fluxo atual |
| `vision/FakeVision.*` | Simulador visual por OpenCV | `adjustFieldPosition`, `calibration`, `show`, `mouseActions` | `Global`, `WorldModel`, OpenCV | Legado/incompleto; nao implementa toda `IVision` |

# Dependencias

Dependencias de compilacao/runtime:

- `g++`
- `make`
- OpenCV 4: `core`, `videoio`, `highgui`, `imgproc`, `shape`, `imgcodecs`, `calib3d`, `features2d`
- LibSerial (`-lserial`)
- pthread
- POSIX sockets/unistd/arpa/inet/netinet
- nlohmann/json 3.12.0 vendorizado em `communication/json.hpp`
- Arduino/ESP32 com `WiFi.h` e `WiFiUdp.h` para o firmware

Arquivos de dados esperados em runtime:

- `config/appConfig.json`
- `files/colors/<cor>.txt`
- `fieldsSaves.txt`
- `fieldsCoeficients.txt`
- `calibrations.json`
- `files/logs/Vision.log`

# Pontos de risco

1. Concorrencia sem sincronizacao:
   - `Global` e compartilhado entre loop de visao, threads dos robos e comunicacao sem `mutex`/`atomic`.
   - `Global::bufferKeyboard`, posicoes dos robos, bola e `_writeBuffer` podem sofrer race conditions.

2. Thread do defensor nao e finalizada:
   - `main.cpp` cria `tDeffender`, mas o `join()` esta comentado.
   - Ao sair do escopo com uma `std::thread` ainda joinable, o programa pode chamar `std::terminate`.

3. Goleiro configurado mas estrategia desativada:
   - `tGoalKeeper` esta comentada.
   - O robo pode ser detectado e desenhado, mas nao decide movimento pelo fluxo atual.

4. Arquivo da bola tem problema de caixa:
   - O codigo carrega `files/colors/ball.txt`, mas o repositorio tem `files/colors/Ball.txt`.
   - Em Linux, isso quebra a carga dos limiares da bola.

5. Caminho absoluto em estados do atacante:
   - `AttackerStateSeeking` e `AttackerStateAttacking` leem `/home/fhobots/vss/calibrations.json`.
   - O servidor UDP escreve `calibrations.json` relativo ao diretorio atual.
   - Em outra maquina/pasta, a calibracao dinamica pode ser ignorada ou o estado pode falhar.

6. Comunicacao Wi-Fi hardcoded:
   - IP broadcast `192.168.0.255`, servidor `192.168.0.108`, portas `8089` e `8888` estao no codigo.
   - Firmware tambem possui rede e credenciais hardcoded.

7. Buffer UDP possivelmente nao inicializado:
   - `CommunicationWIFI` nao zera `_writeBuffer` no construtor.
   - `sendMessage()` envia 255 bytes embora o protocolo efetivo use 9 bytes.

8. Parser de config fragil:
   - `ConfigParser` tokeniza por whitespace e sintaxe muito especifica.
   - Pequenas mudancas no JSON podem quebrar parsing.
   - Aloca `Token` com `new` repetidamente sem liberar.

9. Calibracao de camera com variaveis nao inicializadas:
   - `Vision::writeCoeficients()` e `readCoeficients()` usam `for(int i; i<4; i++)`, com `i` nao inicializado.
   - `_cameraCoeficients` pode ficar indefinido se arquivo estiver vazio ou leitura falhar.

10. Deteccao de adversarios acumula indefinidamente:
   - `Global::enemyTeam.push_back(...)` ocorre a cada frame e nao ha `clear()` em `Vision::detectionColors()`.

11. Retangulos binarios em `.txt`:
   - `fieldsSaves.txt` e `fieldsCoeficients.txt` sao tratados como binarios.
   - Isso e pouco portavel, dificil de versionar e sujeito a incompatibilidade se classes mudarem.

12. Possiveis excecoes OpenCV sem tratamento:
   - `readFrame()` corta o frame com `Global::fieldRect`; se valores forem invalidos, OpenCV pode lancar excecao.
   - Falha de camera chama `exit(1)`.

13. Estados e parametros inconsistentes:
   - `initializeModelAndStates()` aplica PD de `r0` no goleiro, `r1` no defensor e `r2` no atacante, aparentemente invertendo atacante/goleiro.
   - O papel `deffender` esta escrito errado e o codigo depende dessa grafia.

14. Estado duplicado no defensor:
   - `DefenderRobot::createMachineStates()` adiciona `AttackerStateSpinning` e depois `DefenderStateSpinning`, ambos com nome `spinning`; o segundo sobrescreve o primeiro no `map`.

15. Funcoes matematicas podem gerar NaN:
   - `Vector2D::angleBetween` e `operator||` usam `acos` sem proteger magnitude zero ou arredondamento fora de `[-1,1]`.

16. Log por frame:
   - `Vision::detectionColors()` escreve a posicao da bola em todo frame.
   - Isso pode degradar desempenho e gerar arquivos grandes em treino/competicao.

17. Falta de tratamento de erro em rede/serial:
   - `socket`, `setsockopt`, `inet_aton`, `sendto`, `recvfrom` e serial open/write tem pouca ou nenhuma verificacao robusta.

# Pontos que parecem obsoletos

- `communication/Communication.*`: serial/LibSerial esta completo, mas comentado no fluxo atual.
- `FakePhysics.*`: simulacao simples nao chamada pelo `main.cpp`.
- `vision/FakeVision.*`: simulador nao usado; alem disso nao implementa `calibratePreProcess()` da interface `IVision`, entao nao substitui `Vision` diretamente.
- `strategy/goalkeeper/GoalkeeperStateRetreating.*`: existe, mas nao e adicionado na maquina de estados do goleiro atual.
- `strategy/goalkeeper/GoalkeeperStateTurnaround.*`: existe, mas nao e adicionado na maquina de estados do goleiro atual.
- `CommunicationWIFICalibration::sendMessage()`: declarado no header, sem implementacao observada.
- `files/colors/coeficients.txt`: arquivo vazio, sem uso claro.
- `bin/` e `fhobotsTeam`: artefatos de build locais/ignorados, nao fonte.
- `README.md`: muito curto e com encoding quebrado.
- Muitos blocos comentados em estados do atacante/defensor/goleiro parecem experimentos antigos.

# Sugestoes iniciais de modernizacao

Estas sugestoes sao para discussao futura; nenhuma alteracao foi feita agora.

1. Congelar comportamento atual:
   - documentar hardware, rede, camera, iluminacao, cores e procedimento de calibracao;
   - criar um checklist de partida para competicao.

2. Corrigir riscos pequenos e objetivos primeiro:
   - padronizar `ball.txt`/`Ball.txt`;
   - remover caminhos absolutos de `calibrations.json`;
   - inicializar buffers e coeficientes;
   - fechar/juntar threads corretamente;
   - limpar `Global::enemyTeam` por frame.

3. Melhorar configuracao sem mudar estrategia:
   - usar `nlohmann/json` tambem para `appConfig.json`;
   - mover IPs, portas e caminhos para config;
   - validar campos obrigatorios e mensagens de erro.

4. Isolar estado compartilhado:
   - proteger escrita/leitura de mundo e comandos;
   - separar `WorldState` de `Global`;
   - criar snapshot imutavel por frame para a estrategia.

5. Tornar calibracao reprodutivel:
   - salvar campo, coeficientes e HSV em JSON legivel;
   - carregar sem obrigar recalibracao manual sempre;
   - criar modo calibracao separado do modo jogo.

6. Fortalecer protocolo de comunicacao:
   - enviar apenas tamanho real do pacote;
   - incluir checksum/contador/frame id;
   - registrar perda de pacotes;
   - parametrizar id/pinos no firmware.

7. Adicionar verificacoes sem reescrever:
   - testes unitarios para `Vector2D`, `WorldModel`, `Control` e parser;
   - um modo replay/simulacao com frames gravados;
   - logs com nivel e taxa limitada.

# Perguntas que precisam ser respondidas antes de alterar o codigo

1. O fluxo oficial de competicao hoje e Wi-Fi/UDP ou ainda existe uso real de XBee/serial?
2. O goleiro deve estar ativo? Se sim, por que `tGoalKeeper` esta comentado?
3. O robo defensor deve mesmo usar o papel escrito `deffender` no config, ou podemos migrar para `defender` com compatibilidade?
4. Qual maquina/IP deve receber a calibracao UDP em competicao? `192.168.0.108` e fixo?
5. O broadcast `192.168.0.255:8089` e sempre a rede dos robos?
6. O firmware em `TesteCommander.ino` e o firmware usado nos robos reais ou apenas teste?
7. Os ids de robo do firmware sao regravados manualmente por robo ou deveriam vir do pacote/config?
8. `calibrations.json` deve ser carregado do repositorio atual ou de `/home/fhobots/vss/`?
9. Quais estados do goleiro realmente foram usados em competicao?
10. `fieldsSaves.txt` e `fieldsCoeficients.txt` atuais sao calibracoes boas e devem ser preservadas?
11. As cores individuais atuais (`purple`, `green`, `red`) ainda correspondem aos robos fisicos?
12. Qual camera/lente e usada, e os coeficientes atuais ainda fazem sentido?
13. A estrategia deve manter os tres papeis fixos ou permitir troca dinamica de papeis?
14. O arquivo `files/colors/Ball.txt` deveria ser renomeado para `ball.txt`, ou existe dependencia externa usando maiuscula?
15. Ha videos ou logs de uma partida boa para validar comportamento antes/depois?

