# Relatorio de Correcoes Criticas do Univector

## Objetivo

Reduzir instabilidades antes dos testes fisicos do atacante com Univector Field, sem alterar Kalman, comunicacao, firmware ou a arquitetura geral da estrategia.

## Arquivos alterados

- `navigation/UnivectorField.hpp`
- `navigation/UnivectorField.cpp`
- `strategy/attacker/AttackerUnivector.hpp`
- `strategy/attacker/AttackerUnivector.cpp`
- `strategy/attacker/AttackerStateSeeking.cpp`
- `strategy/attacker/AttackerStateAttacking.cpp`
- `tests/univector_simulation.cpp`
- `RELATORIO_UNIVECTOR_FIXES.md`

## 1. Histerese frente/re

Foi adicionada histerese na escolha entre frente e re:

```cpp
abs(reverseError) + reverseHysteresis < abs(frontError)
```

Parametros em `UnivectorConfig`:

- `reverseHysteresis = 0.2`
- `reverseSwitchFrames = 3`

Tambem foi adicionada persistencia de modo: quando o campo ja esta em frente ou re, a troca so ocorre depois de 3 frames consecutivos pedindo o novo modo.

Impacto esperado:

- menos alternancia frente/re perto de 90 graus;
- menos jitter no comando;
- menos chance de travamento por troca rapida de direcao.

## 2. Bloqueio da area defensiva

O atacante agora adiciona obstaculos virtuais ao redor de `Global::areaToDeffend`.

Parametros em `AttackerUnivector.cpp`:

- `DEFENSE_AREA_OBSTACLE_RADIUS = 35.0`
- `DEFENSE_AREA_OBSTACLE_MARGIN = 18.0`

Foram criados pontos ao redor do retangulo da area, nas bordas superior, inferior, esquerda e direita.

Impacto esperado:

- o campo passa a repelir o atacante antes da area defensiva;
- reduz o risco de entrada ilegal na area proibida.

## 3. Abordagem correta da bola

No estado `seeking`, o alvo usado pelo Univector deixa de ser a bola diretamente e passa a ser um ponto atras da bola em relacao ao gol:

```cpp
target = ball - normalize(goal - ball) * OFFSET
```

Parametro:

- `BALL_APPROACH_OFFSET = 30.0`

O fallback antigo continua usando `Global::ball` se o Univector falhar.

Impacto esperado:

- atacante chega por tras da bola;
- melhora a orientacao para ataque;
- reduz aproximacao pelo lado errado.

## 4. Attacking sem perder a bola

Em `AttackerStateAttacking.cpp`, foi adicionada a regra:

- se a distancia robo-bola for maior que `ATTACKING_BALL_REACQUIRE_DISTANCE`, o atacante volta a mirar a bola;
- se estiver perto, mira o gol.

Parametro:

- `ATTACKING_BALL_REACQUIRE_DISTANCE = 55.0`

Quando esta longe, o Univector usa a abordagem atras da bola. Quando esta perto, usa o gol para manter o chute.

## 5. Filtro leve de inimigos

O helper do atacante agora aplica media movel simples de 3 frames em `Global::enemyTeam` antes de criar os obstaculos.

Parametro:

- `ENEMY_SMOOTHING_FRAMES = 3`

Impacto esperado:

- menos jitter no desvio;
- obstaculos adversarios menos ruidosos.

Limitacao:

- o filtro e por indice de deteccao, nao por identidade rastreada do adversario.

## Ajuste adicional

A repulsao radial de obstaculos a frente nao pode mais empurrar o vetor para tras diretamente. A componente contraria ao alvo e removida quando o obstaculo esta a frente, deixando o desvio atuar lateralmente.

Isso evita que dois obstaculos simetricos facam o atacante inverter para re sem necessidade.

## Validacao executada

### Build WSL

Comando:

```bash
make clean && make
```

Resultado:

- sucesso;
- binario `fhobotsTeam` gerado;
- aviso pre-existente em `communication/jsonRead.cpp` sobre structured bindings com flag efetiva `-std=c++11`.

### Simulacao Univector

Comando:

```bash
make univector-test && ./bin/univector_simulation
```

Resultados:

1. Alvo sem obstaculo:
   - `valid = 1`
   - `reverse = 0`
   - direcao `(1, 0)`

2. Obstaculo central:
   - `valid = 1`
   - `reverse = 0`
   - direcao aproximada `(0.860, 0.509)`
   - desvio lateral sem trocar para re.

3. Obstaculo lateral:
   - `valid = 1`
   - `reverse = 0`
   - direcao aproximada `(0.872, -0.489)`
   - desvio suave para o lado oposto.

4. Dois obstaculos simetricos:
   - `valid = 1`
   - `reverse = 0`
   - direcao aproximada `(1, 0)`
   - sem inversao indevida para re.

5. Alvo atras do robo:
   - frente mantida nos 2 primeiros frames apos troca brusca;
   - `reverse = 1` no 3o frame consecutivo;
   - confirma persistencia contra alternancia rapida.

### Check de diff

Comando:

```bash
git diff --check
```

Resultado:

- sem erros;
- apenas avisos de conversao LF/CRLF do ambiente Windows.

## Testes de bancada pendentes

Nao executados nesta maquina por dependerem do hardware:

- robo suspenso com Univector ligado;
- observar ausencia de alternancia frente/re;
- verificar que o comando nao oscila quando o alvo esta perto de 90 graus;
- comparar com `ENABLE_ATTACKER_UNIVECTOR = false`.

## Testes de campo pendentes

- atacante contra obstaculo central;
- atacante contra obstaculo lateral;
- atacante com obstaculo perto da area defensiva;
- verificar que o atacante nao entra em `Global::areaToDeffend`;
- bola parada e bola em movimento;
- verificar que o atacante reacquire a bola quando se afasta;
- verificar que, quando perto da bola, segue para o gol sem contornar demais.

## Riscos restantes

- Obstaculos virtuais da area defensiva ainda sao pontos discretos; podem precisar de raio/margem maiores no campo real.
- Media movel de inimigos por indice pode falhar se a ordem de deteccao variar muito.
- O offset atras da bola pode precisar ajuste conforme escala da camera.
- O limite robo-bola no attacking pode precisar ajuste conforme velocidade real dos motores.

## Parametros principais para ajuste em campo

- `reverseHysteresis`
- `reverseSwitchFrames`
- `BALL_APPROACH_OFFSET`
- `ATTACKING_BALL_REACQUIRE_DISTANCE`
- `DEFENSE_AREA_OBSTACLE_RADIUS`
- `DEFENSE_AREA_OBSTACLE_MARGIN`
- `ATTACKER_UNIVECTOR_OBSTACLE_REPULSION`
- `ATTACKER_UNIVECTOR_OBSTACLE_TANGENTIAL`
