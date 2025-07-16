// Portable WiFi Repeater - Modified by ChatGPT
// ESP8266 One-File .ino Project with Web Config + OLED

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include <CRC32.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <lwip/napt.h>
#include <lwip/dns.h>

#define LED_PIN 2
#define LED_ON LOW
#define LED_OFF HIGH
#define BTN_PIN 14
#define OLED_ADDR 0x3C
#define OLED_WIDTH 128
#define OLED_HEIGHT 32
#define CONFIG_SSID "WiFiRepeater"
#define EEPROM_ADDR 0
#define SSID_LEN 40
#define PASS_LEN 30

struct Config {
  char staSSID[SSID_LEN + 1];
  char staPASS[PASS_LEN + 1];
  char apSSID[SSID_LEN + 1];
  char apPASS[PASS_LEN + 1];
};

struct ConfigPackage {
  Config config;
  uint32_t crc;
};

ConfigPackage cfg;
ESP8266WebServer server(80);
DNSServer dns;
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
uint8_t MODE = 0; // 0:INIT, 1:CONFIG, 2:REPEATER

void blinkLED(int n, int speed = 100) {
  for (int i = 0; i < n; i++) {
    digitalWrite(LED_PIN, LED_ON);
    delay(speed);
    digitalWrite(LED_PIN, LED_OFF);
    delay(speed);
  }
}

void oledLine(uint8_t line, const String &msg) {
  if (line > 3) return;
  display.fillRect(0, line * 8, OLED_WIDTH, 8, BLACK);
  display.setCursor(0, line * 8);
  display.print(msg);
  display.display();
}

void clearConfig(bool resetEEPROM) {
  memset(&cfg, 0, sizeof(cfg));
  if (resetEEPROM) {
    EEPROM.put(EEPROM_ADDR, cfg);
    EEPROM.commit();
  }
}

bool saveConfig() {
  cfg.crc = CRC32::calculate((const uint8_t*)&cfg.config, sizeof(cfg.config));
  EEPROM.put(EEPROM_ADDR, cfg);
  return EEPROM.commit();
}

bool loadConfig() {
  EEPROM.begin(sizeof(cfg));
  EEPROM.get(EEPROM_ADDR, cfg);
  uint32_t crc = CRC32::calculate((const uint8_t*)&cfg.config, sizeof(cfg.config));
  if (crc == cfg.crc) return true;
  clearConfig(false);
  return false;
}

void handleRoot() {
  if (server.method() == HTTP_POST) {
    server.arg("staSSID").toCharArray(cfg.config.staSSID, SSID_LEN);
    server.arg("staPASS").toCharArray(cfg.config.staPASS, PASS_LEN);
    server.arg("apSSID").toCharArray(cfg.config.apSSID, SSID_LEN);
    server.arg("apPASS").toCharArray(cfg.config.apPASS, PASS_LEN);
    saveConfig();
    server.send(200, "text/html", "<h2>Configuration Saved</h2><footer><hr><p>&copy; 2025 MD.NAHIDUL ISLAM</p></footer>");
    delay(2000);
    ESP.restart();
  } else {
    String html = R"rawliteral(
      <form method='POST'>
        <h3>Connect to Main WiFi</h3>
        SSID: <input name='staSSID'><br>
        PASS: <input name='staPASS' type='password'><br>
        <h3>Create Repeater</h3>
        SSID: <input name='apSSID'><br>
        PASS: <input name='apPASS' type='password'><br>
        <input type='submit'>
      </form>
      <footer><hr><p>&copy; 2025 MD.NAHIDUL ISLAM</p></footer>
    )rawliteral";
    server.send(200, "text/html", html);
  }
}

void startConfigMode() {
  IPAddress local_ip(192,168,0,79);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_ip, local_ip, IPAddress(255,255,255,0));
  WiFi.softAP(CONFIG_SSID);
  dns.start(53, "*", local_ip);
  server.on("/", handleRoot);
  server.begin();
  oledLine(0, "Config Mode");
  oledLine(1, CONFIG_SSID);
  oledLine(2, local_ip.toString());
}

void startRepeater() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(cfg.config.staSSID, cfg.config.staPASS);
  int t = 0;
  while (WiFi.status() != WL_CONNECTED && t++ < 20) {
    delay(500);
  }
  IPAddress ap_ip(10, 0, 0, 1);
  WiFi.softAPConfig(ap_ip, ap_ip, IPAddress(255,255,255,0));
  WiFi.softAP(cfg.config.apSSID, cfg.config.apPASS);
  ip_napt_init(0, 0);
  ip_napt_enable_no(SOFTAP_IF, 1);
  oledLine(0, "Repeater Mode");
  oledLine(1, cfg.config.staSSID);
  oledLine(2, "<==>");
  oledLine(3, cfg.config.apSSID);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  if (!loadConfig() || digitalRead(BTN_PIN) == LOW) {
    MODE = 1;
    clearConfig(false);
  } else {
    MODE = 2;
  }

  if (MODE == 1) startConfigMode();
  if (MODE == 2) startRepeater();
}

void loop() {
  if (MODE == 1) {
    dns.processNextRequest();
    server.handleClient();
  }
}
