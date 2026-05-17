# vss

Codigo da categoria VSS da FHOBOts.

## Build

```bash
make bin_dir
make
```

## Configs

- `config/real.json`: execucao com camera e comunicacao serial.
- `config/sim-yellow.json`: time amarelo no simulador.
- `config/sim-blue.json`: time azul no simulador.

No simulador, a numeracao esperada e:

- robo `0`: goleiro
- robo `1`: defensor
- robo `2`: atacante

Os parametros de simulacao ficam em `simulation`:

- `max-speed`: velocidade maxima enviada ao simulador.
- `smoothing`: suavizacao aplicada entre o comando anterior e o novo comando.
- `deadband`: valores muito pequenos sao zerados.

## Execucao Real

```bash
make run config/real.json
```

## Execucao No Simulador

Um time:

```bash
make run sim config/sim-yellow.json
```

Dois times, em dois terminais:

```bash
make run sim config/sim-yellow.json
make run sim config/sim-blue.json
```

Depois clique em cada janela `Debug` e pressione `espaco` para sair do estado `idle`.

## Teclas

- `espaco`: inicia as maquinas de estado.
- `p`: volta para `idle`.
- `Esc`: encerra.

## Portas Do Simulador

- Entrada de visao: multicast `224.0.0.1:10002`.
- Saida de comandos: `127.0.0.1:20011`.

## Estrategia

Os robos usam `UnivectorField` nos deslocamentos principais. O campo combina atracao pelo alvo
com repulsao de robos e paredes para suavizar trajetorias e reduzir colisoes. Giros, re e manobras
especiais continuam controlados pelos estados especificos de cada papel.

O controle por joystick foi removido.
