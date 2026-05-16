# Relatorio de tracking temporal

Etapa 4 do projeto VSSS: reduzir troca de identidade dos robos na visao computacional usando associacao temporal simples, sem Kalman e sem alterar estrategia, controle, comunicacao ou protocolo.

## Problema resolvido

Antes, `Vision::calculatePosRobot()` escolhia o par de blobs `cor individual + cor do time` apenas pela menor distancia entre os marcadores. Quando dois robos ficavam perto, cruzavam trajetorias ou surgia reflexo, o blob comum do time podia ser pareado com o robo errado, trocando a identidade entre frames.

Agora, quando o robo ja tem uma posicao valida recente, os candidatos continuam sendo pares `cor individual + cor do time`, mas a escolha prioriza o centro do par mais proximo da ultima posicao conhecida daquele mesmo robo.

## Arquivos alterados

- `vision/Vision.hpp`
- `vision/Vision.cpp`
- `RELATORIO_TRACKING_TEMPORAL.md`

## Regra de associacao temporal

Para cada robo:

1. sao gerados candidatos combinando cada blob da cor individual do robo com cada blob da cor comum do time;
2. para cada candidato, calcula-se:
   - centro do robo: media entre blob individual e blob do time;
   - orientacao: vetor do blob do time para o blob individual;
   - distancia entre marcadores;
   - distancia temporal ate a ultima posicao valida do mesmo robo;
3. se existe uma ultima posicao valida recente, o candidato escolhido e o mais proximo temporalmente;
4. a distancia entre marcadores entra como desempate leve;
5. se nao existe referencia temporal, ou se o robo ficou perdido por muitos frames, o algoritmo volta temporariamente ao criterio antigo de menor distancia entre marcadores.

## Regra de rejeicao de salto

Parametro:

```cpp
constexpr double MAX_TRACKING_JUMP_PIXELS = 90.0;
```

Se o robo tem uma posicao valida recente e o melhor candidato esta a mais de `90 px` da ultima posicao conhecida:

- a deteccao e considerada suspeita;
- a posicao/orientacao do robo nao sao atualizadas;
- a identidade nao e trocada;
- o contador de frames perdidos desse robo e incrementado.

## Regra de perda e reaquisicao

Parametro:

```cpp
constexpr int MAX_LOST_FRAMES_BEFORE_REACQUIRE = 12;
```

Se um robo some por poucos frames:

- a ultima posicao valida e mantida;
- o robo e marcado visualmente como `hold`;
- a estrategia continua lendo a ultima pose estavel.

Depois de mais de `12` frames perdidos:

- o sistema permite reaquisicao livre;
- o candidato volta a ser escolhido pelo criterio de menor distancia entre marcadores;
- quando uma deteccao e aceita, o contador volta para zero.

## Parametros usados

```cpp
constexpr int TRACKED_ROBOT_COUNT = 3;
constexpr double MAX_TRACKING_JUMP_PIXELS = 90.0;
constexpr int MAX_LOST_FRAMES_BEFORE_REACQUIRE = 12;
constexpr double MARKER_PAIR_TIE_WEIGHT = 0.05;
```

Onde ajustar:

```text
vision/Vision.cpp
```

Orientacao pratica:

- aumentar `MAX_TRACKING_JUMP_PIXELS` se robos rapidos estiverem sendo congelados;
- reduzir `MAX_TRACKING_JUMP_PIXELS` se ainda houver troca de identidade;
- aumentar `MAX_LOST_FRAMES_BEFORE_REACQUIRE` se reflexos curtos causarem reaquisicao errada;
- reduzir `MAX_LOST_FRAMES_BEFORE_REACQUIRE` se o robo demora demais para voltar apos perda real.

## Debug visual

Quando um robo esta usando a ultima posicao valida por perda temporaria ou salto suspeito:

- a tela desenha um pequeno circulo laranja ao redor do robo;
- aparece o texto `hold` perto da posicao mantida.

O objetivo e indicar perda temporaria sem poluir a tela.

## Riscos

- Se `MAX_TRACKING_JUMP_PIXELS` for baixo demais, movimentos rapidos podem ser rejeitados.
- Se for alto demais, algumas trocas de identidade ainda podem passar.
- O metodo ainda e simples e local; em situacoes com oclusao longa, um algoritmo global como Hungarian ou Kalman pode ser necessario em etapa futura.
- Como a estrategia usa a ultima pose mantida, um robo realmente perdido por poucos frames pode continuar agindo com uma posicao antiga.

## Como testar em campo

1. Rodar o sistema com os robos suspensos ou em baixa velocidade.
2. Aproximar dois robos ate os marcadores ficarem perto.
3. Cruzar trajetorias lentamente e observar se o ID visual/funcional permanece no mesmo robo.
4. Cobrir rapidamente um marcador individual e verificar se aparece `hold`.
5. Remover a oclusao antes de `12` frames e confirmar que a identidade nao troca.
6. Forcar uma perda mais longa e confirmar que o robo consegue reaquirir depois.
7. Se houver congelamento em movimentos normais, aumentar `MAX_TRACKING_JUMP_PIXELS`.
8. Se ainda houver troca de identidade, reduzir `MAX_TRACKING_JUMP_PIXELS` ou aumentar `MAX_LOST_FRAMES_BEFORE_REACQUIRE`.

## Compatibilidade

Nao foram alterados:

- estrategia;
- controle;
- comunicacao;
- protocolo de 18 bytes;
- arquitetura do `Global`;
- Kalman;
- Univector.
