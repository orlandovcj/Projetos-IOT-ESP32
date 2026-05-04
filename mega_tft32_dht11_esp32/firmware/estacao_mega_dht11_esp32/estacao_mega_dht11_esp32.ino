#include <UTFT.h>
#include <DHT.h>

// Display
UTFT myGLCD(CTE32_R2, 38, 39, 40, 41);
extern uint8_t SmallFont[];
extern uint8_t BigFont[];

// DHT11
#define DHTPIN   14
#define DHT_VCC   8
#define DHTTYPE  DHT11
DHT dht(DHTPIN, DHTTYPE);

// Variáveis locais
unsigned long lastSensor = 0;
float tempLocal = 0;
float umidLocal = 0;
bool  leituraOk = false;

// Variáveis vindas do ESP32
float  tempExt   = 0;
float  umidExt   = 0;
float  usd       = 0;
String descClima = "Aguardando...";
byte   hh = 0, mm = 0, ss = 0;
bool   ntpOk = false;

// Buffer serial
String serialBuffer = "";

// -------------------------------------------------------
void desenhaHeader() {
  myGLCD.setColor(0, 0, 128);
  myGLCD.fillRect(0, 0, 319, 29);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 128);
  myGLCD.setFont(BigFont);
  myGLCD.print("Estacao Meteo", CENTER, 6);
}

void desenhaRodape() {
  myGLCD.setColor(0, 0, 60);
  myGLCD.fillRect(0, 218, 319, 239);
  myGLCD.setBackColor(0, 0, 60);
  myGLCD.setColor(200, 200, 200);
  myGLCD.setFont(SmallFont);

  // limpa linha
  myGLCD.fillRect(0, 218, 319, 239);

  char buf[40];
  if (ntpOk) {
    // mostra descrição do clima
    String desc = descClima;
    if (desc.length() > 30) desc = desc.substring(0, 30);
    desc.toCharArray(buf, 40);
    myGLCD.print(buf, CENTER, 224);
  } else {
    myGLCD.print("Aguardando ESP32...", CENTER, 224);
  }
}

void desenhaHora() {
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setColor(255, 255, 0);
  myGLCD.setFont(BigFont);
  char buf[9];
  sprintf(buf, "%02d:%02d:%02d", hh, mm, ss);
  myGLCD.print(buf, CENTER, 35);
}

void desenhaCaixa(int x1, int y1, int x2, int y2,
                  int r, int g, int b,
                  const char* rotulo, float valor,
                  const char* unidade, bool inteiro) {
  myGLCD.setColor(r, g, b);
  myGLCD.fillRect(x1, y1, x2, y2);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRect(x1, y1, x2, y2);

  int xCentro = x1 + (x2 - x1) / 2;

  myGLCD.setBackColor(r, g, b);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(SmallFont);
  int rotLen = strlen(rotulo) * 8;
  myGLCD.print(rotulo, xCentro - rotLen / 2, y1 + 5);

  char buf[10];
  if (inteiro) {
    sprintf(buf, "%d", (int)valor);
  } else {
    int pi = (int)valor;
    int pd = (int)((valor - pi) * 10);
    if (pd < 0) pd = -pd;
    sprintf(buf, "%d.%d", pi, pd);
  }

  myGLCD.setFont(BigFont);
  int valLen = strlen(buf) * 16;
  myGLCD.print(buf, xCentro - valLen / 2, y1 + 28);

  myGLCD.setFont(SmallFont);
  int uniLen = strlen(unidade) * 8;
  myGLCD.print(unidade, xCentro - uniLen / 2, y1 + 55);
}

void desenhaTela() {
  // linha divisória
  myGLCD.setColor(50, 50, 100);
  myGLCD.drawLine(0, 68, 319, 68);
  myGLCD.drawLine(0, 69, 319, 69);

  // label local / externo
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setColor(150, 150, 150);
  myGLCD.setFont(SmallFont);
  myGLCD.print("LOCAL", 30, 72);
  myGLCD.print("EXTERNO", 180, 72);

  // caixas locais
  desenhaCaixa(5,   84, 78,  170,  120,  0,  0,
               "TEMP",  tempLocal, "grC", false);
  desenhaCaixa(82,  84, 155, 170,    0, 80,  0,
               "UMID",  umidLocal, "%",   true);

  // caixas externas
  desenhaCaixa(160, 84, 233, 170,  100, 40,  0,
               "TEMP",  tempExt,  "grC", false);
  desenhaCaixa(237, 84, 310, 170,    0, 60, 100,
               "UMID",  umidExt,  "%",   true);

  // dólar
  myGLCD.setColor(30, 30, 30);
  myGLCD.fillRect(5, 175, 310, 213);
  myGLCD.setColor(255, 200, 0);
  myGLCD.setBackColor(30, 30, 30);
  myGLCD.setFont(SmallFont);
  myGLCD.print("USD/BRL", 10, 181);

  char bufUsd[10];
  int ui = (int)usd;
  int ud = (int)((usd - ui) * 100);
  if (ud < 0) ud = -ud;
  sprintf(bufUsd, "R$ %d.%02d", ui, ud);
  myGLCD.setFont(BigFont);
  myGLCD.print(bufUsd, 120, 190);
}

// -------------------------------------------------------
void lerSensorLocal() {
  float t = NAN, h = NAN;
  for (int i = 0; i < 5; i++) {
    float lt = dht.readTemperature();
    float lh = dht.readHumidity();
    if (!isnan(lt)) t = lt;
    if (!isnan(lh)) h = lh;
    if (!isnan(t) && !isnan(h)) break;
    delay(600);
  }
  if (!isnan(t)) tempLocal = t;
  if (!isnan(h)) umidLocal = h;
  leituraOk = (!isnan(tempLocal) && !isnan(umidLocal));
}

void parseSerial(String linha) {
  linha.trim();

  // apenas hora: "HORA:12:35:01"
  if (linha.startsWith("HORA:")) {
    String horaStr = linha.substring(5);
    hh = horaStr.substring(0, 2).toInt();
    mm = horaStr.substring(3, 5).toInt();
    ss = horaStr.substring(6, 8).toInt();
    ntpOk = true;

    myGLCD.setColor(0, 0, 0);
    myGLCD.fillRect(0, 30, 319, 67);
    desenhaHora();
    return;
  }

  // pacote completo: "NTP:...;TEMP_EXT:...;..."
  if (linha.startsWith("NTP:")) {
    // hora
    int p = linha.indexOf(';');
    String horaStr = linha.substring(4, p);
    hh = horaStr.substring(0, 2).toInt();
    mm = horaStr.substring(3, 5).toInt();
    ss = horaStr.substring(6, 8).toInt();
    ntpOk = true;

    // temp ext
    int i1 = linha.indexOf("TEMP_EXT:") + 9;
    int i2 = linha.indexOf(';', i1);
    tempExt = linha.substring(i1, i2).toFloat();

    // umid ext
    i1 = linha.indexOf("UMID_EXT:") + 9;
    i2 = linha.indexOf(';', i1);
    umidExt = linha.substring(i1, i2).toFloat();

    // desc clima
    i1 = linha.indexOf("DESC:") + 5;
    i2 = linha.indexOf(';', i1);
    descClima = linha.substring(i1, i2);

    // usd
    i1 = linha.indexOf("USD:") + 4;
    usd = linha.substring(i1).toFloat();

    // redesenha tudo
    myGLCD.setColor(0, 0, 0);
    myGLCD.fillRect(0, 30, 319, 215);
    desenhaHora();
    desenhaTela();
    desenhaRodape();
  }
}

// -------------------------------------------------------
void setup() {
  Serial.begin(9600);   // debug USB
  Serial1.begin(9600);  // comunicação com ESP32
  
  // GND adicional via pino digital
  pinMode(11, OUTPUT);
  digitalWrite(11, LOW);   // << pino 11 como GND

  // VCC do DHT11 via pino digital
  pinMode(DHT_VCC, OUTPUT);
  digitalWrite(DHT_VCC, HIGH);

  myGLCD.InitLCD();
  myGLCD.clrScr();
  myGLCD.setBackColor(0, 0, 0);

  desenhaHeader();
  desenhaRodape();
  desenhaHora();

  delay(2000);
  dht.begin();
  delay(1000);

  lerSensorLocal();
  desenhaTela();
}

void loop() {
  // debug: imprime tudo que chega do ESP32
  while (Serial1.available()) {
    char c = Serial1.read();
    Serial.print(c);  // espelha no monitor USB

    if (c == '\n') {
      Serial.println(">> linha completa recebida");
      parseSerial(serialBuffer);
      serialBuffer = "";
    } else {
      serialBuffer += c;
    }
  }

  // lê serial do ESP32
  while (Serial1.available()) {
    char c = Serial1.read();
    if (c == '\n') {
      parseSerial(serialBuffer);
      serialBuffer = "";
    } else {
      serialBuffer += c;
    }
  }

  // lê sensor local a cada 5 segundos
  if (millis() - lastSensor >= 5000) {
    lastSensor = millis();
    lerSensorLocal();
    // atualiza só caixas locais sem redesenhar tudo
    desenhaCaixa(5,   84, 78,  170, 120,  0,  0,
                 "TEMP", tempLocal, "grC", false);
    desenhaCaixa(82,  84, 155, 170,   0, 80,  0,
                 "UMID", umidLocal, "%",   true);
  }
}