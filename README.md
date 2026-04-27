# Painel IoT com ESP32, Nokia 5110, clima e cotação do dólar

Projeto de painel informativo com **ESP32 DevKit V1** e **display Nokia 5110 (PCD8544)** para exibir **temperatura**, **umidade**, **cotação USD/BRL**, **variação do dólar** e **hora via NTP**. O display Nokia 5110 usa interface serial compatível com SPI e opera em 3,3 V, o que o torna adequado para uso com ESP32.

A versão atual do firmware usa a API da **OpenWeather** para obter o clima atual e a **AwesomeAPI** para obter a cotação do dólar comercial. A OpenWeather retorna dados como `main.temp`, `main.humidity` e `weather[0].main`, enquanto a AwesomeAPI retorna campos como `USDBRL.bid` e `USDBRL.pctChange` no endpoint `json/last/USD-BRL`.

## Funcionalidades

- Exibição da temperatura em destaque no display.
- Exibição da umidade relativa do ar.
- Exibição da cotação USD/BRL.
- Exibição da variação percentual do dólar com seta de alta ou baixa.
- Exibição da hora atual sincronizada via NTP.
- Ícones simples de clima desenhados por software com Adafruit GFX.
- Tela inicial com identidade visual do projeto usando elementos gráficos simples.

## Componentes utilizados

| Componente | Função no projeto |
| --- | --- |
| ESP32 DevKit V1 | Microcontrolador principal com Wi‑Fi integrado para consumir APIs e atualizar o display |
| Display Nokia 5110 / PCD8544 | Interface gráfica monocromática 84x48 para exibir os dados |
| Resistor de 220 ohms | Limitação de corrente do backlight do display |
| Protoboard | Montagem sem solda |
| Jumpers macho-macho | Ligações entre ESP32 e display |
| Cabo USB | Alimentação e gravação do firmware |

A resolução de 84x48 pixels do Nokia 5110 é suficiente para pequenos dashboards e permite desenhar texto, linhas, triângulos, círculos e bitmaps simples usando bibliotecas gráficas compatíveis.

## Ligações elétricas

A pinagem adotada nesta versão do projeto é a seguinte:

| Pino do Nokia 5110 | Ligação no ESP32 DevKit V1 |
| --- | --- |
| 1. RST | GPIO 16 |
| 2. CE/CS | GPIO 5 |
| 3. D/C | GPIO 17 |
| 4. DIN | GPIO 23 |
| 5. CLK | GPIO 18 |
| 6. VCC | 3V3 |
| 7. LIGHT / BL | 3V3 por meio de resistor de 220 ohms |
| 8. GND | GND |

Na biblioteca Adafruit PCD8544, a inicialização por software SPI usa a ordem de pinos `SCLK, DIN, DC, CS, RST`, por isso a declaração do objeto foi ajustada para `Adafruit_PCD8544(18, 23, 17, 5, 16)`.

### Ligação do resistor do backlight

O resistor de **220 ohms** deve ser ligado **em série** com o pino de iluminação do display, no caminho entre o **3V3** do ESP32 e o pino **LIGHT/BL/LED** do Nokia 5110. Esse arranjo limita a corrente do LED de backlight e é prática comum em módulos desse tipo.

Exemplo simplificado:

```text
ESP32 3V3 ----[220R]---- BL/LIGHT do Nokia 5110
ESP32 GND --------------- GND do Nokia 5110
```

## Bibliotecas utilizadas

Instalar as seguintes bibliotecas na Arduino IDE:

- **Adafruit PCD8544 Nokia 5110 LCD library** — controle do display PCD8544.
- **Adafruit GFX Library** — primitivas gráficas, texto, linhas, triângulos, retângulos e círculos.
- **ArduinoJson** — leitura do JSON retornado pelas APIs.

Além disso, o código usa bibliotecas do ecossistema ESP32/Arduino já disponíveis com a plataforma da placa:

- `WiFi.h`
- `HTTPClient.h`
- `time.h`

A plataforma correta da placa na Arduino IDE é o pacote **esp32 by Espressif Systems**, com seleção típica da placa como **ESP32 Dev Module** em muitas DevKit V1.

## APIs utilizadas

### Clima

A API de clima usada é a **OpenWeather Current Weather API**, com consulta por latitude e longitude. O firmware utiliza parâmetros como `lat`, `lon`, `appid`, `units=metric` e `lang=pt_br` para receber temperatura em Celsius e dados do clima atual.

Exemplo de endpoint:

```text
https://api.openweathermap.org/data/2.5/weather?lat=-27.5954&lon=-48.5480&appid=SUA_CHAVE&units=metric&lang=pt_br
```

Campos usados no firmware:

- `main.temp`
- `main.humidity`
- `weather[0].main`

### Cotação do dólar

A API de câmbio usada é a **AwesomeAPI**, com o endpoint `https://economia.awesomeapi.com.br/json/last/USD-BRL`. O JSON retornado inclui o objeto `USDBRL`, contendo valores como `bid`, `ask`, `pctChange` e `create_date`.

Campos usados no firmware:

- `USDBRL.bid`
- `USDBRL.pctChange`

## Estrutura visual do firmware atual

A versão mais recente do script implementa três áreas principais de interface:

1. **Tela inicial** — mostra ícones simples representando clima e dólar, além do título do projeto.
2. **Tela de clima** — mostra o título `CLIMA`, um ícone conforme a condição da OpenWeather, temperatura em destaque, umidade e hora atual.
3. **Tela de dólar** — mostra o título `USD/BRL`, o valor atual, seta de tendência e uma barra visual proporcional à intensidade da variação percentual.

## Ícones e lógica visual

A condição principal do clima vem de `weather[0].main`, e o firmware a converte em ícones simples desenhados por software. Categorias como `Clear`, `Clouds`, `Rain`, `Drizzle`, `Thunderstorm`, `Mist`, `Fog`, `Haze` e `Smoke` podem ser tratadas diretamente na lógica de renderização.

A biblioteca Adafruit GFX disponibiliza funções como `drawLine()`, `drawCircle()`, `fillCircle()`, `drawRect()`, `fillRect()` e `fillTriangle()`, usadas no projeto para desenhar ícones, molduras, setas e barras sem depender de imagens externas.

#### Como usar

1. Instalar o pacote de placas **esp32 by Espressif Systems** na Arduino IDE.
2. Instalar as bibliotecas listadas acima.
3. Selecionar a placa **ESP32 Dev Module** na IDE, se compatível com a sua DevKit V1.
4. Ajustar os valores de `ssid`, `password` e `weatherApiKey` no código.
5. Revisar `lat` e `lon` para a localização desejada.
6. Compilar e enviar o sketch para o ESP32.

## Observações práticas

A OpenWeather pode retornar erro 401 quando a chave da API estiver inválida ou ainda não ativada, o que impede o preenchimento dos dados de clima.

A AwesomeAPI, por outro lado, permite consultar `USD-BRL` diretamente e retorna um JSON simples, adequado para microcontroladores com parsing leve de dados.

O display Nokia 5110 pode exigir ajuste fino de contraste via `display.setContrast(...)`, pois a leitura visual varia de módulo para módulo e conforme a alimentação utilizada.

## Melhorias futuras sugeridas

- Remover `delay()` e trocar a rotação de telas por `millis()` para deixar o firmware mais responsivo.
- Adicionar botão físico para troca manual de telas.
- Implementar mais ícones climáticos.
- Adicionar previsão estendida.
- Controlar o backlight por GPIO para modo noturno.

## Licença

Definir a licença do repositório conforme a finalidade do projeto, por exemplo MIT para compartilhamento aberto do firmware.
