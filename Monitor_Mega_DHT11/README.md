# Estação Meteorológica Local — Arduino Mega 2560 + TFT 3.2" + DHT11

Projeto de estação meteorológica local desenvolvida com Arduino Mega 2560, display TFT 3.2" colorido e sensor DHT11. Exibe temperatura, umidade, relógio em tempo real e status do sensor em uma interface gráfica organizada por caixas coloridas.

---

## Funcionalidades

- Leitura de temperatura e umidade via sensor DHT11
- Relógio digital em tempo real (conta a partir do boot)
- Interface gráfica colorida no display TFT 3.2" 240x320
- Atualização do relógio a cada 1 segundo
- Atualização dos sensores a cada 3 segundos com até 5 tentativas de leitura
- Indicador de status do sensor no rodapé (OK / erro)
- Conversão manual de float para string para compatibilidade com AVR/Mega

---

## Hardware utilizado

| Componente       | Modelo                           | Observação                  |
| ---------------- | -------------------------------- | --------------------------- |
| Microcontrolador | Arduino Mega 2560 R3             | Placa principal             |
| Display          | TFT 3.2" 240x320 TFT_320QDT_9341 | Controlador ILI9341         |
| Shield display   | TFT LCD Mega Shield V2.2         | Adaptador para Arduino Mega |
| Sensor           | DHT11                            | Temperatura e umidade       |

---

## Bibliotecas

| Biblioteca              | Versão recomendada | Instalação                            |
| ----------------------- | ------------------ | ------------------------------------- |
| UTFT                    | v2.83              | Manual — site Rinky-Dink Electronics  |
| DHT sensor library      | Qualquer atual     | Arduino Library Manager (by Adafruit) |
| Adafruit Unified Sensor | Qualquer atual     | Arduino Library Manager (by Adafruit) |

### Como instalar a UTFT manualmente

1. Baixe a UTFT v2.83 no site **Rinky-Dink Electronics**.
2. Extraia o arquivo ZIP.
3. Copie a pasta `UTFT` para:
   - Windows: `C:\Users\SEU_USUARIO\Documents\Arduino\libraries\`
4. Verifique que a estrutura está correta:

```text
Arduino/libraries/UTFT/UTFT.h
Arduino/libraries/UTFT/UTFT.cpp
Arduino/libraries/UTFT/memorysaver.h
Arduino/libraries/UTFT/hardware/avr/HW_AVR_defines.h
```

5. Reinicie a Arduino IDE.

---

## Ligações

### Display TFT + Shield

O display TFT_320QDT_9341 é encaixado diretamente no TFT LCD Mega Shield V2.2,
que por sua vez é encaixado no Arduino Mega 2560. Nenhuma fiação adicional é
necessária para o display.

O construtor correto para esse conjunto é:

```cpp
UTFT myGLCD(CTE32_R2, 38, 39, 40, 41);
```

### Sensor DHT11

O shield TFT cobre o pino de 5V do Mega. A solução adotada foi alimentar o
DHT11 via pino digital configurado como HIGH, o que fornece corrente suficiente
para o DHT11 (consumo tipico de 2.5mA).

| Pino DHT11 | Pino Mega 2560  | Observacao                              |
| ---------- | --------------- | --------------------------------------- |
| VCC        | Pino digital 8  | Configurado como OUTPUT HIGH no setup() |
| GND        | GND             | Qualquer GND exposto no shield ou Mega  |
| DATA       | Pino digital 14 | Sinal de dados                          |

> **Observacao:** O pino DATA deve ser um pino fora do barramento paralelo do
> TFT (D22-D41). Pinos com funcao PWM (como 9 e 10) causaram interferencia nos
> testes. O pino 14 mostrou-se estaveal e sem conflitos.

---

## Notas tecnicas

### Bug do sprintf com float no AVR

A funcao `sprintf("%.1f", valor)` gera caracteres invalidos na UTFT quando
usada no compilador AVR-GCC do Arduino Mega. A solucao adotada foi converter
o float manualmente:

```cpp
int parte_int = (int)valor;
int parte_dec = (int)((valor - parte_int) * 10);
if (parte_dec < 0) parte_dec = -parte_dec;
sprintf(buf, "%d.%d", parte_int, parte_dec);
```

### Construtor do display

O display retorna ID `0x0404` nos testes com MCUFRIEND_kbv, o que indica
um controlador "write-only" ou clone. O construtor `CTE32_R2` da biblioteca
UTFT e o unico que inicializa corretamente esse conjunto.

### Leitura do DHT11

O DHT11 nao suporta leituras mais rapidas que 2 segundos. O codigo usa
intervalo de 3 segundos com ate 5 tentativas por ciclo, preservando o ultimo
valor valido caso uma leitura falhe.

---

## Layout da tela

```text
+--------------------------------------------------+
|              Estacao Meteo                       |  <- Header azul
+--------------------------------------------------+
|                  12:35:42                        |  <- Relogio amarelo
+--------------------------------------------------+
|                                                  |
|  +-------------------+  +--------------------+  |
|  |   TEMPERATURA     |  |      UMIDADE       |  |
|  |                   |  |                    |  |
|  |      25.2         |  |        54          |  |
|  |       grC         |  |         %          |  |
|  +-------------------+  +--------------------+  |
|                                                  |
+--------------------------------------------------+
| DHT11 OK - leitura em tempo real                 |  <- Rodape
+--------------------------------------------------+
```

---

## Estrutura do repositorio

estacao-meteorologica-mega/
├── firmware/
│   └── estacao_mega_dht11/
│       └── estacao_mega_dht11.ino
├── README.md
└── LICENSE

---

## Proximos passos planejados

- [ ] Integracao com ESP32 para dados externos via Wi-Fi
- [ ] Hora real via NTP (ESP32 -> Mega por serial)
- [ ] Clima externo via API OpenWeatherMap
- [ ] Cotacao do dolar via AwesomeAPI
- [ ] Modulo RTC DS3231 para hora offline
- [ ] Touch screen para navegacao entre telas

---

## Licenca

O codigo-fonte deste projeto e distribuido sob a licenca MIT.
Consulte o arquivo `LICENSE` para mais detalhes.

---

## Autor

Orlando Castro