# Relatorio Kalman hardening

Correcoes criticas aplicadas antes do Univector Field:

- decay de velocidade do Kalman dos robos em `hold`;
- gating da medicao da bola antes do `correct()`;
- fallback seguro quando a bola e perdida por tempo longo.

## Arquivos alterados

- `vision/Kalman2D.hpp`
- `vision/Kalman2D.cpp`
- `vision/Vision.cpp`
- `strategy/basic/StateExitArea.cpp`
- `RELATORIO_KALMAN_HARDENING.md`

## Mudancas feitas

### 1. Decay de velocidade no hold dos robos

Foi adicionado em `Kalman2D`:

```cpp
void dampVelocity(double factor);
```

Em `Vision::markRobotDetectionLost()`, quando o robo esta em `hold`, antes de `predict()`:

```cpp
_robotKalman[robotId].dampVelocity(ROBOT_HOLD_VELOCITY_DECAY);
_robotKalman[robotId].predict();
```

Parametro:

```cpp
constexpr double ROBOT_HOLD_VELOCITY_DECAY = 0.85;
```

Impacto esperado:

- reduz drift durante oclusao;
- evita movimento fantasma apos colisao;
- preserva o tracking temporal como camada principal.

### 2. Gating da bola

Foi adicionado:

```cpp
constexpr double BALL_KALMAN_GATING_DISTANCE_PIXELS = 60.0;
```

Quando ha medicao e o Kalman ja esta inicializado:

1. o filtro faz `predict()`;
2. calcula a distancia entre medicao HSV e predicao;
3. se a distancia for maior que `60 px`, a medicao e ignorada;
4. o frame e tratado como bola perdida;
5. o filtro segue com predicao, sem `correct()`.

Impacto esperado:

- reflexos HSV distantes nao puxam o filtro;
- jumps absurdos da bola sao rejeitados;
- a bola fica mais robusta em iluminacao ruim.

### 3. Fallback da bola perdida

Antes, apos perda longa, `Global::ball` podia voltar para `(-10, -10)`.

Agora:

```text
Global::ballPos = (-1, -1)
Global::ballVel = (0, 0)
Global::ball    = centro do campo
```

Assim `Global::ballPos.x < 0` continua indicando bola invalida, mas as estrategias que usam `Global::ball` como alvo nao perseguem `(-10, -10)`.

Tambem foi ajustado `StateExitArea`:

```cpp
if(Global::ballPos.x < 0)
    return "";
```

Impacto esperado:

- evita que robo saia do jogo procurando coordenada negativa;
- mantem comportamento simples: em perda longa, o alvo seguro e o meio-campo;
- nao introduz estrategia complexa.

## Renderizacao fora do mutex

Nao implementado nesta etapa.

Motivo: mover renderizacao exigiria criar snapshots consistentes de robos, bola, objetivos, texto de estado e debug. Isso e util, mas tem risco maior de mexer em mais superficie agora. A correcao ficou focada nos problemas criticos de robustez.

## Resultados dos testes

### 1. Compilacao

Status neste ambiente: falhou por falta de OpenCV.

Comando tentado:

```bash
mingw32-make kalman-test
```

Erro:

```text
fatal error: opencv2/video/tracking.hpp: No such file or directory
```

Conclusao:

- o ambiente atual Windows/MinGW nao possui os headers OpenCV usados pelo projeto;
- validar no Linux alvo com OpenCV instalado antes de seguir para Univector.

Comandos recomendados no Linux:

```bash
make kalman-test
./bin/kalman2d_simulation
make
```

### 2. Teste da bola com reflexo artificial

Status: pendente de camera/campo.

Resultado esperado:

- medicao HSV distante mais que `60 px` da predicao deve ser ignorada;
- bola filtrada nao deve saltar para o reflexo.

### 3. Teste de oclusao da bola

Status: pendente de camera/campo.

Resultado esperado:

- oclusao curta usa predicao;
- perda longa marca `ballPos.x < 0`;
- `Global::ball` fica no centro do campo, nao em `(-10, -10)`.

### 4. Teste de colisao do robo

Status: pendente de bancada/campo.

Resultado esperado:

- ao parar/cobrir marcador, velocidade do Kalman decai por `0.85` a cada frame em `hold`;
- posicao nao deve andar sozinha por muito tempo.

### 5. Teste de estabilidade

Status: pendente de execucao longa.

Resultado esperado:

- sem drift acumulado relevante;
- sem reflexos puxando bola;
- sem robo perseguindo coordenada negativa.

## Problemas encontrados

- Nao foi possivel validar compilacao neste ambiente por ausencia de OpenCV.
- Testes de camera e bancada dependem do Linux alvo e hardware.

## Limitacoes atuais

- O gating da bola usa limite fixo em pixels.
- O fallback da bola perdida e centro do campo, nao uma decisao tatica avancada.
- Renderizacao ainda ocorre dentro de `worldMutex`.
- O decay do robo em hold reduz drift, mas nao substitui um modelo fisico de colisao.

## Recomendacoes antes do Univector

1. Rodar `make kalman-test` no Linux alvo.
2. Rodar `make`.
3. Testar reflexo HSV com bola parada.
4. Testar oclusao curta e longa da bola.
5. Testar colisao/hold dos robos.
6. Ajustar:
   - `BALL_KALMAN_GATING_DISTANCE_PIXELS`;
   - `ROBOT_HOLD_VELOCITY_DECAY`;
   - ruidos de processo/medicao se houver atraso ou jitter.
