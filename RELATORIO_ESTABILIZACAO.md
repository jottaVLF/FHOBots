# Relatorio de estabilizacao

Etapa 3 do projeto VSSS: reduzir race conditions na comunicacao, no estado global e na finalizacao das threads, sem alterar protocolo, visao, estrategia ou controle.

## Mudancas feitas

### Comunicacao

Arquivos:

```text
communication/CommunicationESPNow.hpp
communication/CommunicationESPNow.cpp
```

Foi adicionado:

```cpp
std::mutex _commMutex;
```

Foram protegidos por lock:

- `writeMessage()`;
- `sendMessage()`;
- `stopAll()`;
- `getMessage()`;
- `getLeftPwm()`;
- `getRightPwm()`.

Resultado esperado:

- `rebuildPacket()` nao executa ao mesmo tempo que `sendMessage()`;
- `writeAll()` escreve os 18 bytes sem outra thread alterar o pacote no meio;
- overlay/debug tambem le PWM sem corrida com escrita da estrategia.

### Estado global

Arquivos:

```text
Global.hpp
Global.cpp
Vision.cpp
Robot.cpp
main.cpp
```

Foi adicionado:

```cpp
std::mutex Global::worldMutex;
```

Protecoes aplicadas:

- `Vision::detectionColors()` calcula deteccoes fora do lock e publica bola, robos e inimigos dentro de `worldMutex`;
- `Robot::updateRobot()` executa cada ciclo de `_machineState.think()` dentro de `worldMutex`;
- `Vision::drawInfo()` le posicoes, objetivos, estado e parametros dos robos dentro de `worldMutex`;
- `initializeModelAndStates()` inicializa estado compartilhado dentro de `worldMutex`.

Resultado esperado:

- estrategia nao le posicao da bola/robo enquanto a visao esta atualizando esses dados;
- visao nao desenha estado de robo enquanto a thread de estrategia esta mudando objetivo/estado;
- leituras de `Global::enemyTeam`, areas, bola e posicoes ficam consistentes durante um ciclo de estrategia.

### Flag de execucao

Arquivos:

```text
Global.hpp
Global.cpp
main.cpp
Robot.cpp
```

Foi adicionado:

```cpp
std::atomic<bool> Global::running;
```

O loop das threads dos robos agora usa `Global::running`.

`bufferKeyboard` foi mantido como entrada de teclado para a UI/estrategia, mas passou a ser atomico:

```cpp
std::atomic<int> Global::bufferKeyboard;
```

Motivo: varias estrategias ainda consultam tecla `p`/espaco. Remover completamente esse valor mudaria comportamento de jogo. A flag de execucao, que antes dependia de `bufferKeyboard != 27`, foi separada corretamente em `running`.

### Frequencia das threads

Arquivo:

```text
model/Robot.cpp
```

Alterado:

```cpp
std::this_thread::sleep_for(std::chrono::microseconds(200));
```

Para:

```cpp
std::this_thread::sleep_for(std::chrono::milliseconds(5));
```

Isso limita cada thread de robo para aproximadamente `200 Hz`.

## Arquivos alterados

- `communication/CommunicationESPNow.hpp`
- `communication/CommunicationESPNow.cpp`
- `Global.hpp`
- `Global.cpp`
- `main.cpp`
- `model/Robot.cpp`
- `vision/Vision.cpp`
- `RELATORIO_ESTABILIZACAO.md`

## O que nao foi alterado

- protocolo de 18 bytes;
- formato do pacote ESP-NOW;
- logica das estrategias;
- logica da visao;
- controle PD;
- Kalman;
- Univector;
- estrutura geral do `Global`.

## Riscos

- O `worldMutex` serializa visao/desenho e estrategia. Isso reduz paralelismo, mas e uma mudanca simples e conservadora para estabilizar o sistema.
- Se alguma estrategia ficar lenta dentro de `_machineState.think()`, a visao pode esperar mais tempo para publicar novo estado.
- Como o pacote e enviado no loop principal, ainda pode representar o ultimo comando completo recebido de cada thread, nao necessariamente uma barreira sincronizada de todos os robos no mesmo instante. O pacote, porem, nao deve ser corrompido.

## Impacto esperado

- Pacotes de 18 bytes mais consistentes.
- Fim de concorrencia entre `writeMessage()` e `sendMessage()`.
- Menos leituras parciais de bola/robo durante atualizacao da visao.
- Encerramento das threads mais previsivel ao pressionar `Esc`.
- Menor uso de CPU nas threads dos robos por causa do ciclo em `200 Hz`.

## Como testar

1. Compilar no ambiente Linux alvo.
2. Conferir se a porta serial da ESP32 emissora esta correta em `config/appConfig.json`.
3. Executar o sistema com os robos suspensos.
4. Verificar se atacante e defensor iniciam e encerram ao pressionar `Esc`.
5. Observar no Serial da base station se pacotes de 18 bytes seguem chegando.
6. Confirmar que nao ha motores disparando de forma aleatoria.
7. Rodar por alguns minutos e observar se posicoes e comandos permanecem coerentes.
8. Se possivel, compilar com ThreadSanitizer no Linux para procurar races restantes em areas nao cobertas.

## Pendencias possiveis

- `FakeVision.cpp` tambem acessa `Global.*`; se ele voltar a ser usado em paralelo com threads reais, deve receber o mesmo padrao de lock.
- Existem muitos acessos diretos a `Global.*` em arquivos de estrategia. Hoje eles ficam protegidos porque `_machineState.think()` roda inteiro dentro de `worldMutex`.
