# Relatorio de comunicacao ESP-NOW

Este relatorio foi corrigido apos a revisao da Etapa 2. A arquitetura valida do projeto nao usa UDP/IP entre o PC e a ESP32 emissora.

## Arquitetura correta

```text
[PC C++ / OpenCV]
    -> USB Serial
[ESP32 Emissor plugado no PC]
    -> ESP-NOW broadcast FF:FF:FF:FF:FF:FF
[ESP32 Robos]
    -> todos recebem
    -> cada um filtra pelo ROBOT_ID
```

## Decisoes mantidas

- O PC nao fala ESP-NOW diretamente.
- A ESP32 emissora recebe o pacote de 18 bytes pela USB Serial.
- A ESP32 emissora retransmite o pacote sem alterar bytes.
- ESP-NOW nao usa IP, porta UDP ou roteador.
- O envio ESP-NOW e broadcast para `FF:FF:FF:FF:FF:FF`.
- Nao ha MAC fixo dos robos.
- Nao ha pareamento individual por robo.
- Cada robo executa somente o comando correspondente ao seu `ROBOT_ID`.
- O protocolo de 18 bytes foi preservado.
- Nao foi reintroduzida dependencia de LibSerial/XBee.

## C++ ativo

O backend ativo continua sendo `CommunicationESPNow`, mas agora com serial POSIX simples:

```cpp
Global::communication = new CommunicationESPNow(configuration.communication);
```

A porta vem de `config/appConfig.json`:

```json
"communication" : "/dev/ttyUSB0"
```

Trocar para `/dev/ttyACM0` ou outra porta real conforme o Linux enumerar a ESP32 emissora.

## Protocolo

Tamanho total: `18` bytes.

| Byte(s) | Campo | Descricao |
|---|---|---|
| 0 | `HEADER_0` | `0xA5` |
| 1 | `HEADER_1` | `0x5A` |
| 2-3 | contador | `uint16_t` little-endian |
| 4-7 | robo 0 | `id`, `pwmLeft`, `pwmRight`, `flags` |
| 8-11 | robo 1 | `id`, `pwmLeft`, `pwmRight`, `flags` |
| 12-15 | robo 2 | `id`, `pwmLeft`, `pwmRight`, `flags` |
| 16 | reservado | atualmente `0x00` |
| 17 | checksum | XOR dos bytes `0..16` |

Flags por robo:

- bit 0: motor esquerdo reverso;
- bit 1: motor direito reverso;
- demais bits reservados.

## Firmware da Etapa 2

O detalhamento completo esta em:

```text
RELATORIO_FIRMWARE_ESPNOW.md
```
