// ✅ Telegram Controlled Light + Buzzer System
// ✅ 100% Safe & Legal Project
// ✅ Developer: MD. Nahidul Islam

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

// ================== WIFI INFO ==================
const char* WIFI_SSID = "MASUM";
const char* WIFI_PASS = "1234567890";

// ================== TELEGRAM INFO ==================
const String BOT_TOKEN = "8586515243:AAEhW2Yo-L2V9V9RmUCRjMMvDvTBJbqkNTw";
const String CHAT_ID  = "5316471518";

// ================== PIN SETUP (GPIO) ==================
// D5 = GPIO14, D6 = GPIO12
#define LED_PIN     14
#define BUZZER_PIN  12

// ================== GLOBAL ==================
WiFiClientSecure client;
bool ledStatus = false;
unsigned long lastCheck = 0;
String lastUpdateId = "0";

// ================== TELEGRAM SEND ==================
void sendTelegram(String msg) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient https;
    client.setInsecure();

    String url = "https://api.telegram.org/bot" + BOT_TOKEN + "/sendMessage";
    https.begin(client, url);
    https.addHeader("Content-Type", "application/json");

    String payload = "{\"chat_id\":\"" + CHAT_ID + "\",\"text\":\"" + msg + "\"}";
    https.POST(payload);
    https.end();
  }
}

// ================== TELEGRAM READ ==================
void readTelegram() {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient https;
  client.setInsecure();

  String url = "https://api.telegram.org/bot" + BOT_TOKEN + "/getUpdates?offset=" + lastUpdateId;
  https.begin(client, url);
  int httpCode = https.GET();

  if (httpCode == 200) {
    String payload = https.getString();

    if (payload.indexOf("\"text\":\"") != -1) {
      int idIndex = payload.lastIndexOf("\"update_id\":");
      if (idIndex != -1) {
        int idEnd = payload.indexOf(",", idIndex);
        lastUpdateId = payload.substring(idIndex + 12, idEnd);
      }

      if (payload.indexOf(CHAT_ID) == -1) return;  // Security

      // ===== COMMAND CHECK =====
      if (payload.indexOf("/on") != -1) {
        digitalWrite(LED_PIN, HIGH);
        ledStatus = true;

        digitalWrite(BUZZER_PIN, HIGH); delay(150);
        digitalWrite(BUZZER_PIN, LOW);  delay(150);
        digitalWrite(BUZZER_PIN, HIGH); delay(150);
        digitalWrite(BUZZER_PIN, LOW);

        sendTelegram("✅ Light ON");
      }

      else if (payload.indexOf("/off") != -1) {
        digitalWrite(LED_PIN, LOW);
        ledStatus = false;

        digitalWrite(BUZZER_PIN, HIGH); delay(150);
        digitalWrite(BUZZER_PIN, LOW);  delay(150);
        digitalWrite(BUZZER_PIN, HIGH); delay(150);
        digitalWrite(BUZZER_PIN, LOW);  delay(150);
        digitalWrite(BUZZER_PIN, HIGH); delay(150);
        digitalWrite(BUZZER_PIN, LOW);

        sendTelegram("❌ Light OFF");
      }

      else if (payload.indexOf("/status") != -1) {
        String st = ledStatus ? "ON ✅" : "OFF ❌";
        sendTelegram("💡 Light Status: " + st);
      }
    }
  }

  https.end();
}

// ================== SETUP ==================
void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  String bootMsg = "✅ Device Online!\nIP: " + WiFi.localIP().toString();
  sendTelegram(bootMsg);
}

// ================== LOOP ==================
void loop() {
  if (millis() - lastCheck > 3000) {
    lastCheck = millis();
    readTelegram();
  }
}
