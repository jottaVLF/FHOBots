# Relatorio Kalman dos robos

Etapa 5.2 do projeto VSSS: adicionar Kalman para os robos como camada de suavizacao apos o tracking temporal, sem substituir a associacao temporal e sem alterar estrategia, controle, comunicacao ou protocolo.

## Arquivos alterados

- `vision/Vision.hpp`
- `vision/Vision.cpp`
- `model/Robot.hpp`
- `model/Robot.cpp`
- `Makefile`
- `tests/kalman2d_simulation.cpp`
- `RELATORIO_KALMAN_ROBOS.md`

## Como o Kalman foi integrado

Cada robo agora possui uma instancia de `Kalman2D` em `Vision`:

```cpp
Kalman2D _robotKalman[3];
```

Associacao:

- robo `id 0`: atacante;
- robo `id 1`: defensor;
- robo `id 2`: goleiro.

O tracking temporal continua sendo a primeira camada:

1. `calculatePosRobot()` gera candidatos por `cor individual + cor do time`;
2. o tracking temporal escolhe o par usando a ultima posicao valida;
3. saltos impossiveis continuam sendo rejeitados;
4. somente a medicao aceita alimenta o Kalman;
5. a posicao filtrada do Kalman atualiza `robot->setPosition(...)`;
6. a orientacao continua vindo dos marcadores, sem Kalman angular.

## Modelo

Foi reaproveitada a classe `Kalman2D`.

Estado:

```text
[x, y, vx, vy]
```

Medicao:

```text
[x, y]
```

`Robot` recebeu um campo de velocidade estimada:

```cpp
Vector2D _velocity;
```

Com acesso por:

```cpp
setVelocity(...)
getVelocity()
```

Isso nao altera a estrategia atual, mas deixa a velocidade estimada disponivel para proximas etapas.

## Perda de deteccao

Quando o tracking entra em `hold`:

- a medicao rejeitada nao alimenta o Kalman;
- o Kalman faz apenas `predict()`;
- a posicao do robo continua suave;
- o texto `hold` permanece no debug visual.

Depois de perda longa:

- o Kalman daquele robo e resetado;
- a velocidade estimada e zerada;
- o tracking temporal volta a permitir reaquisicao livre pelo criterio antigo.

## Parametros usados

Em `vision/Vision.cpp`:

```cpp
constexpr double ROBOT_KALMAN_PROCESS_NOISE = 5e-2;
constexpr double ROBOT_KALMAN_MEASUREMENT_NOISE = 9.0;
constexpr double ROBOT_KALMAN_ERROR_COVARIANCE = 25.0;
constexpr double ROBOT_KALMAN_DT = 1.0;
```

Orientacao de ajuste:

- aumentar `ROBOT_KALMAN_PROCESS_NOISE` se o filtro atrasar demais em movimentos rapidos;
- reduzir `ROBOT_KALMAN_PROCESS_NOISE` se a posicao filtrada ficar nervosa;
- aumentar `ROBOT_KALMAN_MEASUREMENT_NOISE` se a visao tiver muito jitter/reflexo;
- reduzir `ROBOT_KALMAN_MEASUREMENT_NOISE` se o filtro estiver suave demais e atrasado.

## Debug visual

Para cada robo:

- circulo verde: medicao aceita pelo tracking temporal;
- circulo ciano: posicao filtrada pelo Kalman;
- texto `hold`: tracking manteve/perdeu temporariamente e o Kalman esta em predicao.

## Teste simulado sem camera

Foi criado:

```text
tests/kalman2d_simulation.cpp
```

Rodar no Linux alvo com:

```bash
make kalman-test
./bin/kalman2d_simulation
```

O teste imprime:

```text
frame,measured_x,measured_y,filtered_x,filtered_y,velocity_x,velocity_y
```

Ele alimenta o filtro com uma trajetoria em linha reta com ruido deterministico. O esperado e:

- posicao filtrada mais suave que a medida;
- velocidade estimada aproximando o deslocamento real por frame;
- sem camera e sem depender do restante do sistema.

## Resultados dos testes

### 1. Compilacao

Status neste ambiente: nao concluido.

Comando tentado:

```bash
mingw32-make kalman-test
```

Motivo: o ambiente atual e Windows/MinGW e nao possui os headers OpenCV usados pelo projeto. A tentativa de compilar o teste Kalman falha com:

```text
fatal error: opencv2/video/tracking.hpp: No such file or directory
```

Acao necessaria: validar no Linux alvo com OpenCV instalado usando:

```bash
make kalman-test
make
```

### 2. Teste unitario/simulado sem camera

Status neste ambiente: nao executado, pois depende do mesmo header OpenCV ausente.

Comando preparado:

```bash
make kalman-test
./bin/kalman2d_simulation
```

### 3. Teste visual com camera

Status: pendente de bancada/campo.

Casos a executar:

- robo parado: ciano deve estabilizar sem tremor;
- movimento lento: ciano deve seguir suave;
- movimento rapido: filtro deve acompanhar sem atraso grande.

### 4. Teste com tracking temporal

Status: pendente de campo.

Procedimento:

- cruzar dois robos;
- confirmar que o circulo verde fica no robo correto;
- confirmar que o ciano nao puxa um robo para o outro.

### 5. Teste de oclusao

Status: pendente de campo.

Procedimento:

- cobrir marcador por poucos frames;
- confirmar `hold`;
- confirmar movimento suave por predicao.

### 6. Teste de perda longa

Status: pendente de campo.

Procedimento:

- esconder robo por mais que `MAX_LOST_FRAMES_BEFORE_REACQUIRE`;
- confirmar reset do Kalman e ausencia de teleporte brusco na reaquisicao.

### 7. Teste em bancada com robos suspensos

Status: pendente de bancada.

Verificar:

- motores suaves;
- sem jitter excessivo;
- sem comandos erraticos.

### 8. Teste de estabilidade

Status: pendente de execucao longa.

Verificar:

- sem drift acumulado;
- sem comportamento instavel apos alguns minutos.

## Problemas encontrados

- Nao foi possivel concluir compilacao neste ambiente por ausencia de OpenCV.
- Testes com camera e robos dependem do hardware e do ambiente Linux alvo.

## Limitacoes atuais

- Kalman suaviza apenas posicao e velocidade linear.
- Orientacao ainda e diretamente calculada pelos marcadores.
- Tracking temporal continua responsavel por identidade; o Kalman nao resolve troca de identidade sozinho.
- Parametros ainda sao constantes no codigo.

## Recomendacoes

- Validar primeiro `make kalman-test` no Linux alvo.
- Depois testar com robos suspensos.
- Ajustar `ROBOT_KALMAN_PROCESS_NOISE` e `ROBOT_KALMAN_MEASUREMENT_NOISE` antes de seguir para Univector.
- Se houver atraso perceptivel, aumentar um pouco o ruido de processo.
- Se houver jitter, aumentar ruido de medicao.
