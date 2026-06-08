#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <time.h>

// ================== WIFI ==================
const char* WIFI_SSID = "SUA_REDE";
const char* WIFI_PASS = "SUA_SENHA";

// ================== NTP ===================
const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = -3 * 3600;
const int DAYLIGHT_OFFSET_SEC = 0;

// ============== OPEN-METEO ===============
float LATITUDE  = -27.5954;   // Florianopolis
float LONGITUDE = -48.5480;

// ============== DISPLAY ==================
#define PIN_CLK 18   // D18
#define PIN_DIN 23   // D23
#define PIN_DC   4   // D4
#define PIN_CS   5   // D5
#define PIN_RST  2   // D2

Adafruit_PCD8544 display = Adafruit_PCD8544(PIN_CLK, PIN_DIN, PIN_DC, PIN_CS, PIN_RST);

// ============== ESTADOS ==================
String horaStr = "--:--";
String dataStr = "--/--";
String tempStr = "--C";
int weatherCode = 3;

int currentScreen = 0;   // 0=face, 1=clock, 2=weather
int faceFrame = 0;

unsigned long lastClockUpdate = 0;
unsigned long lastWeatherUpdate = 0;
unsigned long lastScreenChange = 0;
unsigned long lastFaceAnim = 0;

const unsigned long CLOCK_INTERVAL   = 1000;
const unsigned long WEATHER_INTERVAL = 600000;
const unsigned long SCREEN_INTERVAL  = 10000;
const unsigned long FACE_INTERVAL    = 350;

// ============== ICONES ===================
const int ICON_SUN   = 0;
const int ICON_CLOUD = 1;
const int ICON_RAIN  = 2;

// =========================================
String httpGET(String url) {
  HTTPClient http;
  WiFiClientSecure client;
  client.setInsecure();

  http.begin(client, url);
  int httpCode = http.GET();
  String payload = "";

  if (httpCode > 0) {
    payload = http.getString();
  }

  http.end();
  return payload;
}

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0, 0);
  display.println("Conectando");
  display.println("WiFi...");
  display.display();

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    delay(500);
  }
}

void updateClock() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char hbuf[6];
    char dbuf[6];
    strftime(hbuf, sizeof(hbuf), "%H:%M", &timeinfo);
    strftime(dbuf, sizeof(dbuf), "%d/%m", &timeinfo);
    horaStr = String(hbuf);
    dataStr = String(dbuf);
  }
}

void updateWeather() {
  String url = "https://api.open-meteo.com/v1/forecast?latitude=" + String(LATITUDE, 4) +
               "&longitude=" + String(LONGITUDE, 4) +
               "&current=temperature_2m,weather_code&timezone=auto";

  String payload = httpGET(url);
  if (payload.length() == 0) return;

  DynamicJsonDocument doc(2048);
  DeserializationError error = deserializeJson(doc, payload);
  if (error) return;

  float temp = doc["current"]["temperature_2m"] | 0.0;
  weatherCode = doc["current"]["weather_code"] | 3;

  int tempInt = (int)round(temp);
  tempStr = String(tempInt) + "C";
}

int getWeatherIcon(int code) {
  if (code == 0) return ICON_SUN;

  if (code == 1 || code == 2 || code == 3) {
    return ICON_CLOUD;
  }

  if (code == 51 || code == 53 || code == 55 ||
      code == 61 || code == 63 || code == 65 ||
      code == 80 || code == 81 || code == 82) {
    return ICON_RAIN;
  }

  return ICON_CLOUD;
}

void drawSunIcon(int x, int y) {
  display.drawCircle(x, y, 7, BLACK);
  display.drawLine(x, y - 11, x, y - 15, BLACK);
  display.drawLine(x, y + 11, x, y + 15, BLACK);
  display.drawLine(x - 11, y, x - 15, y, BLACK);
  display.drawLine(x + 11, y, x + 15, y, BLACK);
  display.drawLine(x - 8, y - 8, x - 11, y - 11, BLACK);
  display.drawLine(x + 8, y - 8, x + 11, y - 11, BLACK);
  display.drawLine(x - 8, y + 8, x - 11, y + 11, BLACK);
  display.drawLine(x + 8, y + 8, x + 11, y + 11, BLACK);
}

void drawCloudIcon(int x, int y) {
  display.fillCircle(x - 8, y, 5, BLACK);
  display.fillCircle(x, y - 3, 6, BLACK);
  display.fillCircle(x + 8, y, 5, BLACK);
  display.fillRect(x - 12, y, 24, 8, BLACK);
}

void drawRainIcon(int x, int y) {
  drawCloudIcon(x, y);
  display.drawLine(x - 8, y + 10, x - 10, y + 15, BLACK);
  display.drawLine(x,     y + 10, x - 2,  y + 15, BLACK);
  display.drawLine(x + 8, y + 10, x + 6,  y + 15, BLACK);
}

void drawFace(int frame) {
  display.clearDisplay();

  display.drawRoundRect(8, 6, 68, 36, 4, BLACK);

  if (frame == 0) {
    display.fillRoundRect(24, 14, 10, 12, 3, BLACK);
    display.fillRoundRect(50, 14, 10, 12, 3, BLACK);
    display.drawLine(33, 31, 37, 34, BLACK);
    display.drawLine(37, 34, 42, 35, BLACK);
    display.drawLine(42, 35, 47, 34, BLACK);
    display.drawLine(47, 34, 51, 31, BLACK);
  }
  else if (frame == 1) {
    display.drawLine(24, 20, 34, 20, BLACK);
    display.drawLine(50, 20, 60, 20, BLACK);
    display.drawLine(33, 31, 37, 34, BLACK);
    display.drawLine(37, 34, 42, 35, BLACK);
    display.drawLine(42, 35, 47, 34, BLACK);
    display.drawLine(47, 34, 51, 31, BLACK);
  }
  else {
    display.fillRoundRect(24, 14, 10, 12, 3, BLACK);
    display.fillRoundRect(50, 14, 10, 12, 3, BLACK);
    display.drawLine(35, 33, 49, 33, BLACK);
  }

  display.display();
}

void drawClockScreen() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(18, 2);
  display.print("DATA/HORA");

  display.drawLine(0, 12, 83, 12, BLACK);

  display.setTextSize(2);
  display.setCursor(10, 18);
  display.print(horaStr);

  display.setTextSize(1);
  display.setCursor(26, 38);
  display.print(dataStr);

  display.display();
}

void drawWeatherScreen() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(22, 2);
  display.print("CLIMA");

  display.drawLine(0, 12, 83, 12, BLACK);

  int icon = getWeatherIcon(weatherCode);

  if (icon == ICON_SUN) {
    drawSunIcon(20, 29);
  } else if (icon == ICON_CLOUD) {
    drawCloudIcon(20, 26);
  } else {
    drawRainIcon(20, 24);
  }

  display.setTextSize(2);
  display.setCursor(38, 20);
  display.print(tempStr);

  display.display();
}

void renderCurrentScreen() {
  if (currentScreen == 0) {
    drawFace(faceFrame);
  } else if (currentScreen == 1) {
    drawClockScreen();
  } else {
    drawWeatherScreen();
  }
}

void setup() {
  Serial.begin(115200);

  display.begin();
  display.setContrast(60);
  display.clearDisplay();
  display.display();

  connectWiFi();

  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

  updateClock();
  updateWeather();

  randomSeed(micros());
  renderCurrentScreen();
}

void loop() {
  unsigned long now = millis();

  if (now - lastClockUpdate >= CLOCK_INTERVAL) {
    lastClockUpdate = now;
    updateClock();
  }

  if (WiFi.status() == WL_CONNECTED && now - lastWeatherUpdate >= WEATHER_INTERVAL) {
    lastWeatherUpdate = now;
    updateWeather();
  }

  if (now - lastScreenChange >= SCREEN_INTERVAL) {
    lastScreenChange = now;
    currentScreen = (currentScreen + 1) % 3;
  }

  if (currentScreen == 0 && now - lastFaceAnim >= FACE_INTERVAL) {
    lastFaceAnim = now;

    int r = random(0, 10);
    if (r < 7) faceFrame = 0;
    else if (r < 9) faceFrame = 1;
    else faceFrame = 2;
  }

  renderCurrentScreen();
  delay(50);
}