#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>

// Wi-Fi
const char* ssid = "Aledani";
const char* password = "carlinhos7423";

// NTP
const char* ntpServer   = "pool.ntp.org";
const long  gmtOffset   = -3 * 3600;  // UTC-3 (Brasilia)
const int   dstOffset   = 0;

// APIs
// OpenWeatherMap - crie conta gratis em openweathermap.org
const String OWM_KEY    = "be5ed92e9931ce5bb84ac79e89180a47";
const String OWM_CITY   = "Florianopolis,BR";
const String OWM_URL    = "http://api.openweathermap.org/data/2.5/weather?q="
                          + OWM_CITY + "&appid=" + OWM_KEY
                          + "&units=metric&lang=pt_br";

// AwesomeAPI (dólar) - gratuita, sem chave
const String USD_URL    = "https://economia.awesomeapi.com.br/json/last/USD-BRL";

// Variáveis
float  tempExt  = 0;
float  umidExt  = 0;
String descClima = "---";
float  usd      = 0;
String horaStr  = "00:00:00";

unsigned long lastFetch = 0;
const unsigned long INTERVALO = 30000; // 30 segundos

// -------------------------------------------------------
void conectaWifi() {
  Serial.print("Conectando WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" OK");
  Serial.println(WiFi.localIP());
}

void sincronizaNTP() {
  configTime(gmtOffset, dstOffset, ntpServer);
  Serial.println("NTP sincronizado");
}

String getHora() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "00:00:00";
  char buf[9];
  sprintf(buf, "%02d:%02d:%02d",
          timeinfo.tm_hour,
          timeinfo.tm_min,
          timeinfo.tm_sec);
  return String(buf);
}

void fetchClima() {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.begin(OWM_URL);
  int code = http.GET();

  if (code == 200) {
    String payload = http.getString();
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, payload);

    tempExt  = doc["main"]["temp"].as<float>();
    umidExt  = doc["main"]["humidity"].as<float>();
    descClima = doc["weather"][0]["description"].as<String>();

    // capitaliza primeira letra
    if (descClima.length() > 0) {
      descClima[0] = toupper(descClima[0]);
    }

    Serial.println("Clima OK: " + String(tempExt) + "C " + descClima);
  } else {
    Serial.println("Erro clima: " + String(code));
  }
  http.end();
}

void fetchDolar() {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.begin(USD_URL);
  int code = http.GET();

  if (code == 200) {
    String payload = http.getString();
    StaticJsonDocument<512> doc;
    deserializeJson(doc, payload);
    usd = doc["USDBRL"]["bid"].as<String>().toFloat();
    Serial.println("Dolar OK: R$ " + String(usd, 2));
  } else {
    Serial.println("Erro dolar: " + String(code));
  }
  http.end();
}

void enviaDadosMega() {
  horaStr = getHora();

  // monta string de dados
  String dados = "NTP:" + horaStr
               + ";TEMP_EXT:" + String(tempExt, 1)
               + ";UMID_EXT:" + String((int)umidExt)
               + ";DESC:" + descClima
               + ";USD:" + String(usd, 2)
               + "\n";

  // envia pela serial para o Mega
  Serial2.print(dados);

  Serial.println("Enviado: " + dados);
}

// -------------------------------------------------------
void setup() {
  Serial.begin(115200);   // debug USB
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17 → Mega

  conectaWifi();
  sincronizaNTP();
  delay(1000);

  // primeira busca imediata
  fetchClima();
  fetchDolar();
  enviaDadosMega();
}

void loop() {
  if (millis() - lastFetch >= INTERVALO) {
    lastFetch = millis();
    fetchClima();
    fetchDolar();
    enviaDadosMega();
  }

  // envia hora atualizada a cada 1 segundo
  static unsigned long lastHora = 0;
  if (millis() - lastHora >= 1000) {
    lastHora = millis();
    String h = "HORA:" + getHora() + "\n";
    Serial2.print(h);
  }
}