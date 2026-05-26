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
        ├── 01_uno_rgb_led_intro/
        │   ├── README.md
        │   ├── uno_rgb_led_intro.ino
        │   ├── LED_RGB_UNO.png
        │   ├── UNO_RGB_LED.png
        ├── 02_esp32_oled_weather/
        │   ├── firmware/
        │   │   └── esp32_oled_weather/
        │   │       └── esp32_oled_weather.ino
        │   └── ESP32.png
        │   └── LICENSE
        │   └── README.md
        ├── 03_mega_tft32_dht11/
        │   ├── firmware/
        │   │   └── estacao_mega_dht11/
        │   │       └── estacao_mega_dht11.ino
        │   └── README.md
        ├── 04_mega_tft32_dht11_esp32/
        │   ├── firmware/
        │   │   ├── estacao_mega_dht11_esp32/
        │   │   │   └── estacao_mega_dht11_esp32.ino
        │   │   └── estacao_esp32/
        │   │       └── estacao_esp32.ino
        │   └── README.md
        ├── 05_mega_tft32_calibration/
        │   ├── firmware/
        │   │   └── mega_tft32_calibration.ino
        │   └── README.md
        ├── 06_mega_tft32_touch_calculator/
        │   ├── firmware/
        │   │   └── mega_tft32_touch_calculator.ino
        │   └── README.md
        └── ...

---

## Convenção de nomes de pastas

As pastas seguem o padrão:

    [plataforma]-[componente_principal]-[funcionalidade]

| Pasta                    | Descrição                            |
| ------------------------ | ------------------------------------ |
| uno_rgb_led_intro        | Uno + LED RGB                        |
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

## Roadmap de projetos

### Concluídos

- [x] LED RGB controlado por Arduino UNO

- [x] Estação meteorologica local - Mega + TFT 3.2" + DHT11

- [x] Integração ESP32 - hora NTP + clima externo + cotação do dolar

- [x] Estação meteorológica local - Mega + TFT 3.2" + HDT11

- [x] Calibração e configuração do TFT 3,2" com touch XPT2046

- [x] Calculadora touch simples

### Em desenvolvimento

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
- [ ] Configurador WiFi via BLE - configurar rede sem regravar firmware
- [ ] Monitor BT Serial - visualizar dados da estação meteorologica no celular
- [ ] Alertas BLE - notificação no celular por limite de temperatura/umidade
- [ ] Controle de relé via BT - automação pelo celular
- [ ] ESP32 bridge BT-Serial - celular -> ESP32 -> Mega

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
