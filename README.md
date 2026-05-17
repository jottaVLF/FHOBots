# vss
Repositório para versionamento do código da categoria VSS

## Build

```bash
make bin_dir
make
```

## Execução

```bash
make run
```

O executável atual usa apenas o fluxo real: câmera configurada em `config/appConfig.json`,
calibração por OpenCV e comunicação serial com os robôs.

Para rodar com o simulador:

```bash
make run sim
```

Para rodar dois times no simulador, use dois terminais:

```bash
make run sim config/appConfig.json
make run sim config/appConfig.blue.json
```

O controle por joystick foi removido.

Durante a execução, pressione espaço para iniciar as máquinas de estado, `p` para voltar ao estado
idle e `Esc` para encerrar.
