#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* ssid = "No Somking";
const char* password = "iloveyou";

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 6 * 3600, 60000);

void drawFace() {
  display.setTextSize(1);
  display.setCursor(90, 0);
  display.print(":)");
  display.fillCircle(110, 25, 2, SSD1306_WHITE);
  display.fillCircle(120, 25, 2, SSD1306_WHITE);
  display.drawLine(112, 38, 118, 38, SSD1306_WHITE);
  display.drawCircle(115, 38, 6, SSD1306_WHITE);
}

void setup() {
  Wire.begin(4, 5);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("WiFi Connect...");
  display.display();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  timeClient.begin();
}

void loop() {
  timeClient.update();
  time_t rawTime = timeClient.getEpochTime();
  struct tm* ti = localtime(&rawTime);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("NAHID CLOCK");
  display.setTextSize(2);
  display.setCursor(10, 20);
  display.printf("%02d:%02d:%02d", ti->tm_hour, ti->tm_min, ti->tm_sec);
  display.setTextSize(1);
  display.setCursor(10, 50);
  display.printf("%02d/%02d/%04d", ti->tm_mday, ti->tm_mon + 1, ti->tm_year + 1900);
  drawFace();
  display.display();
  delay(1000);
}
