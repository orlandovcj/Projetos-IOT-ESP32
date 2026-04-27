#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <time.h>

// Nokia 5110
// Ordem: CLK, DIN, D/C, CE, RST
Adafruit_PCD8544 display = Adafruit_PCD8544(18, 23, 17, 5, 16);

const char* ssid = "Aledani";
const char* password = "carlinhos7423";
const char* weatherApiKey = "be5ed92e9931ce5bb84ac79e89180a47";

String lat = "-27.6956";
String lon = "-48.4911";

String temperatura = "--";
String umidade = "--";
String climaMain = "--";
String dolar = "--";
String variacao = "--";

unsigned long lastUpdate = 0;
const unsigned long intervaloAPI = 600000; // 10 min

void drawSun(int x, int y) {
  display.drawCircle(x, y, 6, BLACK);
  display.drawLine(x, y - 10, x, y - 7, BLACK);
  display.drawLine(x, y + 7, x, y + 10, BLACK);
  display.drawLine(x - 10, y, x - 7, y, BLACK);
  display.drawLine(x + 7, y, x + 10, y, BLACK);
  display.drawLine(x - 7, y - 7, x - 5, y - 5, BLACK);
  display.drawLine(x + 7, y - 7, x + 5, y - 5, BLACK);
  display.drawLine(x - 7, y + 7, x - 5, y + 5, BLACK);
  display.drawLine(x + 7, y + 7, x + 5, y + 5, BLACK);
}

void drawCloud(int x, int y) {
  display.fillCircle(x, y, 5, BLACK);
  display.fillCircle(x + 7, y - 2, 6, BLACK);
  display.fillCircle(x + 14, y, 5, BLACK);
  display.fillRect(x, y, 14, 6, BLACK);
}

void drawRain(int x, int y) {
  drawCloud(x, y);
  display.drawLine(x + 2, y + 10, x, y + 15, BLACK);
  display.drawLine(x + 8, y + 10, x + 6, y + 15, BLACK);
  display.drawLine(x + 14, y + 10, x + 12, y + 15, BLACK);
}

void drawStorm(int x, int y) {
  drawCloud(x, y);
  display.drawLine(x + 8, y + 8, x + 4, y + 15, BLACK);
  display.drawLine(x + 4, y + 15, x + 9, y + 15, BLACK);
  display.drawLine(x + 9, y + 15, x + 6, y + 21, BLACK);
}

void drawMist(int x, int y) {
  display.drawLine(x, y, x + 18, y, BLACK);
  display.drawLine(x + 2, y + 4, x + 20, y + 4, BLACK);
  display.drawLine(x, y + 8, x + 18, y + 8, BLACK);
}

void drawDollarIcon(int x, int y) {
  display.drawCircle(x + 8, y + 8, 8, BLACK);
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(x + 5, y + 4);
  display.print("$");
}

void drawArrowUp(int x, int y) {
  display.fillTriangle(x, y + 8, x + 6, y, x + 12, y + 8, BLACK);
  display.fillRect(x + 5, y + 8, 2, 8, BLACK);
}

void drawArrowDown(int x, int y) {
  display.fillTriangle(x, y + 8, x + 12, y + 8, x + 6, y + 16, BLACK);
  display.fillRect(x + 5, y, 2, 8, BLACK);
}

String horaAtual() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "--:--";
  }

  char horaStr[6];
  strftime(horaStr, sizeof(horaStr), "%H:%M", &timeinfo);
  return String(horaStr);
}

void configurarRelogio() {
  configTime(-3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
}

void splashScreen() {
  display.clearDisplay();

  drawSun(12, 12);
  drawCloud(4, 24);
  drawDollarIcon(58, 8);

  display.drawLine(0, 38, 83, 38, BLACK);

  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(8, 40);
  display.print("CLIMA + USD");

  display.display();
}

void telaMensagem(String l1, String l2 = "", String l3 = "") {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);

  display.setCursor(0, 0);
  display.println(l1);

  if (l2.length()) {
    display.setCursor(0, 12);
    display.println(l2);
  }

  if (l3.length()) {
    display.setCursor(0, 24);
    display.println(l3);
  }

  display.display();
}

void conectarWiFi() {
  telaMensagem("Conectando", "WiFi...");
  Serial.println();
  Serial.print("Conectando em: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true, true);
  delay(1000);
  WiFi.begin(ssid, password);

  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 30) {
    delay(500);
    Serial.print(".");
    tentativas++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi conectado");
    Serial.print("IP local: ");
    Serial.println(WiFi.localIP());
    telaMensagem("WiFi OK", WiFi.localIP().toString());
    delay(1500);
  } else {
    Serial.println("Falha WiFi");
    telaMensagem("Falha WiFi", "Verifique", "SSID/senha");
    delay(2000);
  }
}

void obterClima() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.openweathermap.org/data/2.5/weather?lat=" + lat +
                 "&lon=" + lon +
                 "&appid=" + weatherApiKey +
                 "&units=metric&lang=pt_br";

    http.begin(url);
    int httpCode = http.GET();
    Serial.print("HTTP clima: ");
    Serial.println(httpCode);

    if (httpCode == 200) {
      String payload = http.getString();
      DynamicJsonDocument doc(4096);
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        temperatura = String(doc["main"]["temp"].as<float>(), 1);
        umidade = String(doc["main"]["humidity"].as<int>());
        climaMain = doc["weather"][0]["main"].as<String>();
      } else {
        temperatura = "json";
        umidade = "erro";
        climaMain = "Err";
      }
    } else {
      temperatura = "api";
      umidade = String(httpCode);
      climaMain = "Err";
    }

    http.end();
  } else {
    temperatura = "sem";
    umidade = "wifi";
    climaMain = "Off";
  }
}

void obterDolar() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://economia.awesomeapi.com.br/json/last/USD-BRL";

    http.begin(url);
    int httpCode = http.GET();
    Serial.print("HTTP dolar: ");
    Serial.println(httpCode);

    if (httpCode == 200) {
      String payload = http.getString();
      DynamicJsonDocument doc(2048);
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        dolar = doc["USDBRL"]["bid"].as<String>();
        variacao = String(doc["USDBRL"]["pctChange"].as<float>(), 2);
      } else {
        dolar = "json";
        variacao = "erro";
      }
    } else {
      dolar = "api";
      variacao = String(httpCode);
    }

    http.end();
  } else {
    dolar = "sem";
    variacao = "wifi";
  }
}

void desenharIconeClima(int x, int y, String cond) {
  if (cond == "Clear") {
    drawSun(x, y);
  } else if (cond == "Clouds") {
    drawCloud(x, y);
  } else if (cond == "Rain" || cond == "Drizzle") {
    drawRain(x, y);
  } else if (cond == "Thunderstorm") {
    drawStorm(x, y);
  } else if (cond == "Mist" || cond == "Fog" || cond == "Haze" || cond == "Smoke") {
    drawMist(x, y);
  } else {
    drawCloud(x, y);
  }
}

void telaClimaBonita() {
  display.clearDisplay();

  display.drawRoundRect(0, 0, 84, 48, 3, BLACK);
  desenharIconeClima(65, 10, climaMain);

  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(4, 4);
  display.print("CLIMA");

  display.drawLine(4, 14, 79, 14, BLACK);

  display.setTextSize(2);
  display.setCursor(4, 18);
  display.print(temperatura);

  display.setTextSize(1);
  display.setCursor(52, 24);
  display.print("C");

  display.setCursor(4, 38);
  display.print("U:");
  display.print(umidade);
  display.print("% ");

  display.setCursor(50, 38);
  display.print(horaAtual());

  display.display();
}

void telaDolarBonita() {
  display.clearDisplay();

  display.drawRoundRect(0, 0, 84, 48, 3, BLACK);
  drawDollarIcon(62, 3);

  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(4, 4);
  display.print("USD/BRL");

  display.drawLine(4, 14, 79, 14, BLACK);

  display.setCursor(4, 20);
  display.print("R$ ");
  display.print(dolar);

  float varf = variacao.toFloat();

  if (varf > 0) {
    drawArrowUp(56, 18);
  } else if (varf < 0) {
    drawArrowDown(56, 18);
  }

  float intensidade = abs(varf);
  if (intensidade > 5.0) intensidade = 5.0;

  int larguraBarra = (int)(intensidade * 4.0);

  display.drawRect(4, 34, 22, 8, BLACK);
  display.fillRect(5, 35, larguraBarra, 6, BLACK);

  display.setCursor(30, 35);
  display.print(variacao);
  display.print("%");

  display.setCursor(50, 4);
  display.print(horaAtual());

  display.display();
}

void setup() {
  Serial.begin(115200);
  delay(1500);

  display.begin();
  display.setContrast(60);
  display.clearDisplay();
  display.display();

  splashScreen();
  delay(2500);

  conectarWiFi();
  configurarRelogio();
  obterClima();
  obterDolar();
  lastUpdate = millis();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    conectarWiFi();
    configurarRelogio();
  }

  if (millis() - lastUpdate > intervaloAPI) {
    obterClima();
    obterDolar();
    lastUpdate = millis();
  }

  telaClimaBonita();
  delay(4000);

  telaDolarBonita();
  delay(4000);
}