# Relatorio Kalman da bola

Etapa 5.1 do projeto VSSS: adicionar filtro de Kalman somente para a bola, mantendo estrategia, controle, comunicacao, protocolo e tracking temporal dos robos.

## Arquivos criados/alterados

Criados:

- `vision/Kalman2D.hpp`
- `vision/Kalman2D.cpp`
- `RELATORIO_KALMAN_BOLA.md`

Alterados:

- `vision/Vision.hpp`
- `vision/Vision.cpp`
- `Makefile`

## Modelo do Kalman

Foi criada a classe `Kalman2D`, baseada em `cv::KalmanFilter`.

Estado:

```text
[x, y, vx, vy]
```

Medicao:

```text
[x, y]
```

Modelo de transicao:

```text
x  = x  + vx * dt
y  = y  + vy * dt
vx = vx
vy = vy
```

Nesta etapa, `dt = 1.0`, ou seja, a velocidade e estimada em pixels por frame.

## Como a bola detectada entra no filtro

Em `Vision::detectionColors()`, a deteccao da cor da bola continua vindo de:

```cpp
_colorItens["ball"]->getPositionsColor()
```

Quando existe medicao:

1. a primeira deteccao inicializa o Kalman;
2. nos frames seguintes, o filtro faz `predict()`;
3. a medicao `(x, y)` corrige o estado com `correct()`;
4. `Global::ballPos` recebe a posicao filtrada;
5. `Global::ballVel` recebe a velocidade estimada;
6. `Global::ball` recebe a posicao prevista a frente.

## Como a predicao e usada

`Global::ball` continua sendo a posicao que a estrategia consulta como alvo/previsao, mas agora ela vem do Kalman:

```text
Global::ball = [x + vx * horizonte, y + vy * horizonte]
```

Isso substitui a extrapolacao antiga:

```text
posicao + velocidade * 10
```

A previsao e limitada para nao sair agressivamente do campo: se a previsao ultrapassar os limites do campo em algum eixo, esse eixo volta para a posicao filtrada atual.

## Perda temporaria da bola

Quando a bola some:

- por ate `MAX_BALL_LOST_FRAMES`, o filtro usa apenas predicao;
- `Global::ballPos`, `Global::ballVel` e `Global::ball` continuam coerentes com o estado previsto;
- depois de perda longa, o filtro e resetado e a bola volta ao estado invalido seguro.

Estado invalido seguro:

```text
Global::ballPos = (-1, -1)
Global::ballVel = (0, 0)
Global::ball    = (-10, -10)
```

Assim, o atacante nao passa a perseguir `(-10, -10)` imediatamente em uma oclusao curta.

## Parametros ajustaveis

Em `vision/Vision.cpp`:

```cpp
constexpr double BALL_KALMAN_PROCESS_NOISE = 1e-2;
constexpr double BALL_KALMAN_MEASUREMENT_NOISE = 4.0;
constexpr double BALL_KALMAN_ERROR_COVARIANCE = 10.0;
constexpr double BALL_KALMAN_DT = 1.0;
constexpr int MAX_BALL_LOST_FRAMES = 10;
constexpr double BALL_PREDICTION_HORIZON_FRAMES = 10.0;
```

Orientacao de ajuste:

- aumentar `BALL_KALMAN_PROCESS_NOISE` se o filtro estiver lento para acompanhar chutes rapidos;
- reduzir `BALL_KALMAN_PROCESS_NOISE` se a previsao estiver nervosa;
- aumentar `BALL_KALMAN_MEASUREMENT_NOISE` se a deteccao da bola tiver muito ruido/reflexo;
- reduzir `BALL_KALMAN_MEASUREMENT_NOISE` se a medicao estiver confiavel e o filtro parecer atrasado;
- aumentar `MAX_BALL_LOST_FRAMES` para tolerar oclusoes mais longas;
- ajustar `BALL_PREDICTION_HORIZON_FRAMES` para controlar quanto a estrategia mira a frente da bola.

## Debug visual

Foram adicionadas marcacoes discretas:

- circulo verde: medicao detectada da bola;
- circulo ciano: posicao filtrada;
- circulo laranja: posicao prevista;
- texto `ball hold`: bola temporariamente sem medicao, usando predicao.

## Riscos

- Parametros ruins podem atrasar demais a bola ou deixar a previsao instavel.
- Como `Global::ball` segue sendo uma previsao, estrategias que dependem disso continuam com comportamento parecido, mas agora suavizado pelo Kalman.
- Em oclusao longa, a bola e invalidada apos o limite configurado.
- Ainda nao ha Kalman nos robos nesta etapa.

## Como testar

1. Bola parada:
   - a posicao filtrada deve convergir para a medicao;
   - velocidade deve ficar proxima de zero;
   - a previsao deve ficar perto da bola.

2. Movimento lento:
   - a posicao filtrada deve seguir a medicao sem tremores bruscos;
   - a previsao deve aparecer levemente a frente.

3. Chute rapido:
   - a velocidade estimada deve aumentar;
   - a previsao deve apontar na direcao do movimento;
   - se atrasar muito, aumentar `BALL_KALMAN_PROCESS_NOISE`.

4. Oclusao curta:
   - esconder a bola por poucos frames;
   - deve aparecer `ball hold`;
   - a bola prevista deve continuar por alguns frames sem saltar para `(-10, -10)`.

5. Perda longa:
   - esconder a bola por mais de `MAX_BALL_LOST_FRAMES`;
   - o filtro deve resetar;
   - `Global::ball` deve voltar para `(-10, -10)`.

6. Comparacao visual:
   - comparar circulo verde com ciano e laranja;
   - verde e a medicao crua;
   - ciano e a bola filtrada;
   - laranja e o alvo previsto usado em `Global::ball`.

## Compatibilidade

Nao foram alterados:

- estrategia;
- controle;
- comunicacao;
- protocolo de 18 bytes;
- Univector;
- tracking temporal dos robos;
- Kalman dos robos.
