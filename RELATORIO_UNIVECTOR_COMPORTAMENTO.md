# Relatorio Univector Comportamento do Atacante

## Objetivo

Melhorar o comportamento pratico do atacante usando o Univector ja existente, sem implementar o Univector classico completo e sem alterar Kalman, comunicacao, firmware ou protocolo.

## Arquivos alterados

- `strategy/attacker/AttackerUnivector.hpp`
- `strategy/attacker/AttackerUnivector.cpp`
- `strategy/attacker/AttackerStateAttacking.cpp`
- `RELATORIO_UNIVECTOR_COMPORTAMENTO.md`

## 1. Abordagem orientada ao gol

O ponto atras da bola deixou de usar offset fixo.

Antes:

```cpp
target = ball - normalize(goal - ball) * OFFSET
```

Agora o offset depende da distancia robo-bola:

- perto da bola: offset menor;
- longe da bola: offset maior.

Parametros em `AttackerUnivector.cpp`:

- `BALL_APPROACH_OFFSET_MIN = 24.0`
- `BALL_APPROACH_OFFSET_MAX = 58.0`
- `BALL_APPROACH_NEAR_DISTANCE = 35.0`
- `BALL_APPROACH_FAR_DISTANCE = 145.0`

Impacto esperado:

- o atacante chega mais atras da bola quando ainda esta distante;
- perto da bola, evita mirar um ponto atras demais e perder tempo;
- melhora o alinhamento para chute.

## 2. Controle de distancia da bola no attacking

O estado `AttackerStateAttacking` passou a usar tres zonas:

- FAR: mira a bola diretamente;
- MID: mira o ponto atras da bola;
- CLOSE: mira o gol.

Parametros:

- `ATTACKING_BALL_CLOSE_DISTANCE = 35.0`
- `ATTACKING_BALL_FAR_DISTANCE = 95.0`

Regra aplicada:

```cpp
if(distanceToBall > ATTACKING_BALL_FAR_DISTANCE)
    destination = Global::ball;
else if(distanceToBall > ATTACKING_BALL_CLOSE_DISTANCE)
    destination = AttackerUnivector::ballApproachTarget(_robot->getPosition());
else
    destination = Global::areaGoalAttack.getCenter();
```

Impacto esperado:

- longe, o atacante reacquire a bola sem tentar fazer curva longa demais;
- em media distancia, volta para a linha correta atras da bola;
- perto, sustenta o chute para o gol.

## 3. Evitar empurrao lateral da bola

Foi adicionada uma leitura do angulo entre:

- vetor robo -> bola;
- vetor bola -> gol.

Quando o robo esta entrando muito de lado, o helper reduz a influencia do waypoint gerado pelo Univector e mistura mais um waypoint direto para alinhamento.

Parametros:

- `LATERAL_ALIGNMENT_START = 0.45`
- `LATERAL_ALIGNMENT_FULL = 0.85`
- `LATERAL_ALIGNMENT_DIRECT_WEIGHT = 0.55`

Impacto esperado:

- menos chance de tocar a bola pela lateral;
- menos empurrao para fora da linha do gol;
- comportamento mais parecido com jogo real: alinhar primeiro, chutar depois.

## 4. Limitar desvio perto da bola

Quando o robo esta perto da bola, o helper reduz a escala dos obstaculos usados pelo atacante e mistura mais caminho direto ao waypoint final.

Parametros:

- `CLOSE_BALL_DISTANCE = 70.0`
- `CLOSE_BALL_DIRECT_WEIGHT = 0.65`
- `CLOSE_BALL_OBSTACLE_RADIUS_SCALE = 0.45`

A area defensiva continua com uma escala minima de `0.70` para nao perder totalmente a protecao virtual.

Impacto esperado:

- o atacante deixa de contornar demais quando ja tem a bola perto;
- obstaculos ainda influenciam, mas nao dominam o chute;
- trajetoria mais reta nos ultimos centimetros.

## Validacao executada

### Simulacao Univector

O alvo `make univector-test` nao foi executado diretamente porque `make` nao esta disponivel neste PowerShell. Foi usada a chamada equivalente com `g++`:

```bash
g++ -std=c++17 -pedantic -O3 tests/univector_simulation.cpp navigation/UnivectorField.cpp model/Vector2D.cpp -o %TEMP%/univector_simulation_check.exe
```

Resultado:

- alvo sem obstaculo: valido, sem re, direcao `(1, 0)`;
- obstaculo central: valido, desvio lateral e sem re;
- obstaculo lateral: valido, desvio para o lado oposto e sem re;
- dois obstaculos simetricos: valido, direcao reta;
- alvo atras: troca para re apenas apos persistencia de frames.

### Compilacao dos arquivos alterados

Os arquivos alterados do atacante foram compilados como objeto com o include real do OpenCV instalado no ambiente:

```bash
g++ -std=c++17 -pedantic -O3 -I. -IC:/msys64/mingw64/include/opencv4 -c strategy/attacker/AttackerUnivector.cpp
g++ -std=c++17 -pedantic -O3 -I. -IC:/msys64/mingw64/include/opencv4 -c strategy/attacker/AttackerStateAttacking.cpp
```

Resultado:

- compilacao concluida sem erros.

### Check de diff

Comando:

```bash
git diff --check
```

Resultado:

- sem erros de whitespace;
- apenas avisos esperados de conversao LF/CRLF do ambiente Windows.

## Validacao recomendada

### Simulacao

- cenario robo -> bola -> gol;
- robo longe: verificar alvo indo para bola;
- robo em media distancia: verificar alvo atras da bola;
- robo perto: verificar comando indo ao gol;
- colocar obstaculo na frente e observar que o desvio reduz quando o robo entra na zona `CLOSE`.

### Campo

- bola parada sem obstaculo: atacante deve alinhar por tras e chutar;
- bola parada com obstaculo: atacante deve desviar antes, mas nao contornar a bola quando estiver perto;
- chute direto: quando ja estiver alinhado, deve seguir para o gol sem reacquire desnecessario;
- aproximacao lateral: atacante deve corrigir alinhamento antes de empurrar a bola.

## Riscos restantes

- Os limites ainda dependem da escala real da camera e podem precisar ajuste fino em campo.
- A mistura direta reduz desvio, mas nao substitui uma avaliacao completa de posse ou controle da bola.
- Obstaculos adversarios ainda usam media movel simples por indice de deteccao.
