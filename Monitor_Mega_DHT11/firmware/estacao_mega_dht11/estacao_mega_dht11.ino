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

// Variáveis
unsigned long lastUpdate = 0;
unsigned long lastSensor = 0;
float temp = 0;
float umid = 0;
bool  leituraOk = false;
byte  hh = 0, mm = 0, ss = 0;

// -------------------------------------------------------
void desenhaHeader() {
  myGLCD.setColor(0, 0, 128);
  myGLCD.fillRect(0, 0, 319, 29);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 128);
  myGLCD.setFont(BigFont);
  myGLCD.print("Estacao Meteo", CENTER, 6);
}

void desenhaRodape(bool ok) {
  myGLCD.setColor(0, 0, 80);
  myGLCD.fillRect(0, 210, 319, 239);
  myGLCD.setColor(ok ? 0 : 255, ok ? 200 : 0, 0);
  myGLCD.setBackColor(0, 0, 80);
  myGLCD.setFont(SmallFont);
  if (ok) {
    myGLCD.print("DHT11 OK - leitura em tempo real", 4, 220);
  } else {
    myGLCD.print("DHT11 erro - verifique ligacao ", 4, 220);
  }
}

void desenhaHora() {
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setColor(255, 255, 0);
  myGLCD.setFont(BigFont);
  char buf[9];
  sprintf(buf, "%02d:%02d:%02d", hh, mm, ss);
  myGLCD.print(buf, CENTER, 50);
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
  myGLCD.print(rotulo, xCentro - rotLen / 2, y1 + 8);

  // conversão manual do float para evitar bug sprintf/AVR
  char buf[8];
  if (inteiro) {
    sprintf(buf, "%d", (int)valor);
  } else {
    int parte_int = (int)valor;
    int parte_dec = (int)((valor - parte_int) * 10);
    if (parte_dec < 0) parte_dec = -parte_dec;
    sprintf(buf, "%d.%d", parte_int, parte_dec);
  }

  myGLCD.setFont(BigFont);
  int valLen = strlen(buf) * 16;
  myGLCD.print(buf, xCentro - valLen / 2, y1 + 38);

  myGLCD.setFont(SmallFont);
  int uniLen = strlen(unidade) * 8;
  myGLCD.print(unidade, xCentro - uniLen / 2, y1 + 68);
}

void desenhaBlocos() {
  if (leituraOk) {
    desenhaCaixa(10,  90, 155, 200, 120,  0,  0,
                 "TEMPERATURA", temp, "grC", false);
    desenhaCaixa(165, 90, 310, 200,   0, 100,  0,
                 "UMIDADE",     umid, "%",   true);
  } else {
    desenhaCaixa(10,  90, 155, 200, 80, 80, 80,
                 "TEMPERATURA", 0, "erro", true);
    desenhaCaixa(165, 90, 310, 200, 80, 80, 80,
                 "UMIDADE",     0, "erro", true);
  }
}

void lerSensor() {
  float t = NAN;
  float h = NAN;

  for (int i = 0; i < 5; i++) {
    float lt = dht.readTemperature();
    float lh = dht.readHumidity();

    if (!isnan(lt)) t = lt;
    if (!isnan(lh)) h = lh;

    if (!isnan(t) && !isnan(h)) break;
    delay(600);
  }

  if (!isnan(t)) temp = t;
  if (!isnan(h)) umid = h;
  leituraOk = (!isnan(temp) && !isnan(umid));

  Serial.println("----------------------------");
  Serial.print("Temperatura: ");
  isnan(t) ? Serial.println("ERRO") : Serial.println(t);
  Serial.print("Umidade: ");
  isnan(h) ? Serial.println("ERRO") : Serial.println(h);
  Serial.print("leituraOk: ");
  Serial.println(leituraOk);
}

// -------------------------------------------------------
void setup() {
  Serial.begin(9600);

  // VCC do DHT11 via pino digital
  pinMode(DHT_VCC, OUTPUT);
  digitalWrite(DHT_VCC, HIGH);

  // inicializa display primeiro
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myGLCD.setBackColor(0, 0, 0);

  // desenha estrutura da tela
  desenhaHeader();
  desenhaRodape(false);
  desenhaHora();

  // aguarda DHT11 estabilizar
  delay(2000);
  dht.begin();
  delay(1000);

  // primeira leitura
  lerSensor();

  // atualiza blocos com valores reais
  desenhaBlocos();
  desenhaRodape(leituraOk);
}

void loop() {
  unsigned long agora = millis();

  // relógio a cada 1 segundo
  if (agora - lastUpdate >= 1000) {
    lastUpdate = agora;

    ss++;
    if (ss >= 60) { ss = 0; mm++; }
    if (mm >= 60) { mm = 0; hh++; }
    if (hh >= 24) { hh = 0; }

    myGLCD.setColor(0, 0, 0);
    myGLCD.fillRect(0, 40, 319, 85);
    desenhaHora();
  }

  // sensor a cada 3 segundos
  if (agora - lastSensor >= 3000) {
    lastSensor = agora;
    lerSensor();
    desenhaBlocos();
    desenhaRodape(leituraOk);
  }
}