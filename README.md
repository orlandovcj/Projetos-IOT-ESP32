# Projetos de Automação com Arduíno e ESP32

Repositório dedicado ao registro de projetos, experimentos e aprendizados
com microcontroladores Arduíno e ESP32, sensores, shields e displays.

O objetivo nao é apenas guardar codigo, mas documentar o processo completo:
ligações, problemas encontrados, soluções descobertas e evoluçao técnica
ao longo do tempo.

---

## Objetivo

Este repositório serve como:

- **Laboratório pessoal** de experimentos com hardware embarcado
- **Base de conhecimento** com soluções para problemas reais encontrados
- **Portfólio técnico** de projetos funcionais documentados
- **Referência futura** para reutilizar código, ligações e configurações

---

## Plataformas utilizadas

| Plataforma              | Uso principal                                                  |
| ----------------------- | -------------------------------------------------------------- |
| Arduino Mega 2560       | Projetos com muitos pinos, displays paralelos, shields grandes |
| Arduino Uno             | Projetos simples, testes de sensores, prototipagem rápida      |
| ESP32                   | Conectividade Wi-Fi, APIs, NTP, IoT                            |
| Integração Mega + ESP32 | Interface rica no Mega, dados de internet via ESP32            |

---

## Organização do repositório

Cada projeto fica em sua própria pasta com nome descritivo.
Dentro de cada pasta há o codigo-fonte e um README específico
detalhando componentes, ligações, bibliotecas e aprendizados.

    /
    ├── README.md
    ├── LICENSE
    ├── mega-tft32-dht11/
    │   ├── firmware/
    │   │   └── estacao_mega_dht11.ino
    │   └── README.md
    ├── mega-tft32-esp32-weather/
    │   └── README.md
    └── ...

---

## Convenção de nomes de pastas

As pastas seguem o padrão:

    [plataforma]-[componente_principal]-[funcionalidade]

| Pasta                    | Descrição                            |
| ------------------------ | ------------------------------------ |
| mega-tft32-dht11         | Mega + TFT 3.2 + sensor DHT11        |
| mega-tft32-esp32-weather | Mega + TFT 3.2 + ESP32 + clima/dolar |
| uno-oled-bme280          | Uno + OLED + sensor BME280           |
| esp32-oled-weather       | ESP32 + OLED + API de clima          |
| uno-shield-multifuncoes  | Uno + shield multifuncoes            |

---

## Componentes do laboratório

### Microcontroladores

- Arduino Mega 2560 R3
- Arduino Uno R3
- ESP32 (placa de desenvolvimento)

### Displays

- TFT 3.2" 240x320 TFT_320QDT_9341 (controlador ILI9341)
- TFT LCD Mega Shield V2.2
- Nokia 5110 LCD (controlador PCD8544)

### Sensores

- DHT11 — temperatura e umidade
- DHT22 — temperatura e umidade (maior precisão)
- LDR — luminosidade
- LM35 — temperatura analógica

### Shields e módulos

- Shield multifunções para Arduino Uno
- TFT LCD Mega Shield V2.2

---

## Aprendizados registrados

### Display TFT 3.2 ILI9341 + Mega Shield V2.2

- O display retorna ID 0x0404 — considerado write-only por MCUFRIEND_kbv
- O único construtor que funciona com esse conjunto e CTE32_R2 na UTFT:
    UTFT myGLCD(CTE32_R2, 38, 39, 40, 41);
- A biblioteca MCUFRIEND_kbv não inicializa corretamente esse shield
- Usar sempre a UTFT v2.83 oficial do site Rinky-Dink Electronics

### sprintf com float no AVR/Mega

- sprintf("%.1f", valor) gera caracteres inválidos na UTFT com AVR-GCC
- Solucao: converter float manualmente:
    int parte_int = (int)valor;
    int parte_dec = (int)((valor - parte_int) * 10);
    if (parte_dec < 0) parte_dec = -parte_dec;
    sprintf(buf, "%d.%d", parte_int, parte_dec);

### DHT11 com shield TFT no Mega

- O shield TFT cobre o pino de 5V do Mega
- Alimentar DHT11 via pino digital como OUTPUT HIGH funciona (consumo ~2.5mA)
- Pinos 8 (VCC) e 14 (DATA) mostraram-se estaveis e sem conflitos
- O barramento paralelo do TFT usa D22-D41; manter DATA do DHT11 fora dessa faixa

### Comunicacao ESP32 + Mega

- Usar Serial1 no Mega (pinos 18/19) para não conflitar com USB/debug
- Usar Serial2 no ESP32 (GPIO 16/17)
- Protocolo de linha simples com separador ; e terminador \n e suficiente

---

## Roadmap de projetos

### Concluídos

- [x] Estacao meteorologica local — Mega + TFT 3.2" + DHT11

### Em desenvolvimento

- [ ] Integracao ESP32 — hora NTP + clima externo + cotacao do dolar
- [ ] Touch screen — navegacao entre telas no TFT 3.2"

### Planejados

- [ ] Modulo RTC DS3231 — hora real sem depender de Wi-Fi
- [ ] Estacao com BME280 — pressao atmosferica + temperatura + umidade
- [ ] Monitor de qualidade do ar — sensor MQ-135
- [ ] Alarme de movimento — sensor PIR
- [ ] Dashboard com ESP32 + OLED — versao compacta da estacao
- [ ] Data logger com cartao SD — registro historico de sensores
- [ ] Automacao com rele — controle de dispositivos por temperatura
- [ ] Jogo no Nokia 5110 — Snake / menu interativo

---

## Como usar este repositório

Cada projeto é independente. Para reproduzir qualquer um:

1. Acesse a pasta do projeto
2. Leia o README.md específico
3. Abra o .ino na Arduino IDE
4. Instale as bibliotecas listadas
5. Ajuste os pinos conforme sua montagem se necessário
6. Grave na placa correta

---

## Licença

O código-fonte de todos os projetos neste repositório é distribuido sob
a licenca MIT. Consulte o arquivo LICENSE para mais detalhes.

---

## Autor

**Orlando Castro**
Florianopolis, Santa Catarina, Brasil

Desenvolvedor de software com interesse em sistemas embarcados, IoT,
automacao e analise de dados.