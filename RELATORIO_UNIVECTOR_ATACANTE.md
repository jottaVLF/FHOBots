# Relatorio Univector Field no Atacante

## Objetivo

Implementar uma primeira camada de navegacao por Univector Field apenas no atacante, sem reescrever a FSM e sem alterar visao, tracking, Kalman, comunicacao, firmware ou protocolo.

Nesta etapa o Univector atua como auxiliar: os estados do atacante tentam gerar um waypoint seguro; se a saida for invalida, o comportamento antigo continua sendo usado.

## Arquivos criados

- `navigation/UnivectorField.hpp`
- `navigation/UnivectorField.cpp`
- `strategy/attacker/AttackerUnivector.hpp`
- `strategy/attacker/AttackerUnivector.cpp`
- `tests/univector_simulation.cpp`
- `RELATORIO_UNIVECTOR_ATACANTE.md`

## Arquivos alterados

- `strategy/attacker/AttackerStateSeeking.cpp`
- `strategy/attacker/AttackerStateAttacking.cpp`
- `Makefile`

## Como o Univector foi integrado

O modulo `UnivectorField` recebe:

- posicao do robo;
- orientacao do robo;
- posicao alvo;
- posicao da bola;
- lista de obstaculos.

A saida contem:

- `valid`: indica se a navegacao gerou resultado seguro;
- `reverse`: indica se o robo deve usar o chassi em re;
- `desiredDirection`: direcao desejada normalizada;
- `waypoint`: ponto intermediario usado pelo controle atual;
- `angleError`: erro angular escolhido considerando frente/re;
- `distanceToTarget`: distancia ate o alvo.

O helper `AttackerUnivector` concentra a integracao com a estrategia. Ele monta a lista de obstaculos com defensor, goleiro e adversarios detectados em `Global::enemyTeam`, chama o campo e aplica o waypoint usando o controle diferencial ja existente.

## Estados afetados

- `AttackerStateSeeking`: alvo `Global::ball`.
- `AttackerStateAttacking`: alvo `Global::areaGoalAttack.getCenter()`.

Nos dois estados, se `AttackerUnivector::applyNavigation()` retornar `false`, o codigo antigo e executado sem mudanca.

## Campo implementado

Esta primeira versao usa:

- campo atrativo simples em direcao ao alvo;
- repulsao radial simples para obstaculos proximos;
- componente tangencial simples quando um obstaculo esta no corredor entre robo e alvo, para provocar desvio lateral em vez de apenas reduzir a velocidade vetorial.

----+----Nao foi implementada ainda a formulacao matematica completa de Univector Field, nem previsao de obstaculos em movimento.----+----

## Chassi bidirecional

O Univector compara o erro angular usando a frente e a traseira do robo.

Se o alvo fica mais alinhado com a traseira, `reverse = true` e o comando e enviado com `reverseLeft = true` e `reverseRight = true`, reaproveitando `Robot::calculatePwmR()`.

Isso evita obrigar o atacante a girar sempre para apontar a frente quando andar de re e a trajetoria mais curta.

## Parametros ajustaveis

Em `strategy/attacker/AttackerUnivector.cpp`:

- `ENABLE_ATTACKER_UNIVECTOR = true`
- `ATTACKER_UNIVECTOR_OBSTACLE_INFLUENCE = 95.0`
- `ATTACKER_UNIVECTOR_OBSTACLE_REPULSION = 1.35`
- `ATTACKER_UNIVECTOR_OBSTACLE_TANGENTIAL = 0.85`
- `ATTACKER_UNIVECTOR_TARGET_ATTRACTION = 1.0`
- `ATTACKER_UNIVECTOR_LOOKAHEAD = 48.0`
- `ATTACKER_UNIVECTOR_OBSTACLE_RADIUS = 18.0`
- `SEEKING_LEFT_PWM_TRIM = -10`

Para desligar o Univector rapidamente, alterar:

```cpp
const bool ENABLE_ATTACKER_UNIVECTOR = false;
```

## Seguranca e fallback

O Univector nao e usado se:

- a flag estiver desligada;
- a bola estiver invalida (`Global::ballPos.x < 0`);
- o ponteiro do robo ou da comunicacao estiver invalido;
- alguma posicao tiver `NaN` ou infinito;
- o campo gerar vetor muito pequeno ou waypoint invalido.

Nesses casos, os estados do atacante continuam usando o comportamento antigo.

Os PWM enviados pelo helper sao saturados em `0..255` antes de chamar a comunicacao.

## Makefile

O `Makefile` agora:

- exclui `tests/%` do binario principal;
- cria diretorios `bin/...` antes de compilar objetos novos;
- adiciona o alvo `univector-test`;
- mantem o alvo `kalman-test`.

Tambem foi adicionado tratamento simples para `mkdir`/`clean` funcionar tanto no Linux/WSL quanto no MinGW do Windows.

## Resultados dos testes

### Compilacao no WSL

Comando:

```bash
make clean && make
```

Resultado:

- compilacao concluida com sucesso;
- binario `fhobotsTeam` gerado;
- houve apenas um aviso pre-existente em `communication/jsonRead.cpp` sobre structured bindings com flag efetiva `-std=c++11`.

### Teste simulado do Univector

Comando:

```bash
make univector-test && ./bin/univector_simulation
```

Resultado:

1. alvo sem obstaculo:
   - `valid = 1`
   - `reverse = 0`
   - direcao `(1, 0)`
   - waypoint `(45, 0)`

2. obstaculo no caminho:
   - `valid = 1`
   - `reverse = 0`
   - direcao aproximada `(0.335, 0.942)`
   - waypoint aproximado `(15.075, 42.400)`
   - confirma desvio lateral.

3. robo atras da bola:
   - `valid = 1`
   - `reverse = 0`
   - direcao aproximada `(0.988, -0.152)`
   - waypoint aproximado `(-5.523, 13.157)`

4. alvo atras do robo:
   - `valid = 1`
   - `reverse = 1`
   - direcao `(-1, 0)`
   - waypoint `(-45, 0)`
   - confirma selecao de movimento em re.

### Testes praticos

Nao foram executados nesta maquina, pois dependem de camera, robos e bancada. Devem ser feitos em campo:

- robo suspenso com Univector ligado;
- atacante buscando bola parada;
- atacante buscando bola em movimento;
- obstaculo entre atacante e bola;
- comparacao com `ENABLE_ATTACKER_UNIVECTOR = false`;
- verificacao de movimento em re quando o alvo esta atras do robo.

## Riscos e limitacoes

- Campo potencial simples pode ter minimos locais em configuracoes densas.
- Obstaculos adversarios sao tratados como pontos com raio fixo, sem velocidade.
- Ainda nao ha desvio explicito de parede ou area proibida dentro do Univector.
- A direcao tangencial usa uma escolha deterministica simples quando o obstaculo esta exatamente no eixo.
- O atacante ainda depende da FSM atual para decidir o alvo e o estado.

## Proximos passos

- Ajustar ganhos em bancada.
- Adicionar obstaculos de parede/area se necessario.
- Medir diferenca Univector ligado/desligado em trajetorias reais.
- Depois da estabilizacao no atacante, avaliar aplicacao no defensor/goleiro.
