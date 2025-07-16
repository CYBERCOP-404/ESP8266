// CODE WITH MD.NAHIDUL ISLAM

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>

// WiFi credentials (will be stored)
String ssid = "";
String pass = "";

// Telegram bot credentials
const String botToken = "7368406071:AAGkXw-AThfdPCKEV6gXqTAElVX4zV-ASZg";
const String chatId = "5316471518";

// Access Point details
const char* ap_ssid = "WiFi Setup";
const char* ap_password = "12345678";

// Web server
ESP8266WebServer server(80);

// Time tracking
unsigned long lastSentTime = 0;
const unsigned long interval = 60000; // 1 minute

bool connectedToWiFi = false;

// Generate random Bangladeshi girl Facebook-style name
String generateFacebookLink() {
  String first[] = {
    "sanjida", "mim", "nusrat", "ayesha", "tania",
    "shapla", "afroza", "shimu", "reshmi", "lima"
  };
  String last[] = {
    "akter", "khatun", "islam", "sultana", "jahan",
    "nahar", "rahman", "begum"
  };

  String name = first[random(0, sizeof(first)/sizeof(first[0]))];
  name += last[random(0, sizeof(last)/sizeof(last[0]))];
  name += String(random(10, 99));
  return "https://www.facebook.com/" + name;
}

// Send message to Telegram
void sendToTelegramMessage(String userLink) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient https;

    String url = "https://api.telegram.org/bot" + botToken + "/sendMessage";
    https.begin(client, url);
    https.addHeader("Content-Type", "application/json");

    String dateTime = __DATE__;
    String timeNow = __TIME__;
    String senderIP = WiFi.localIP().toString();

    String fullMessage = "<b>۝۝  𝙷𝙸 𝚈𝙾𝚄 𝙷𝙰𝚅𝙴 𝙽𝙴𝚆 𝙷𝙸𝚃  ツ.۝۝ \n\n🐉 ⊚-----------------------------------⊚ 🐉</b>\n"
                         "<b>💬 MAIN INFO : \n📧 ↝ Facebook ID » </b> <code>" + userLink + "</code>\n"
                         "<b>🇴 ↝ Country »</b> <i> Bangladesh 🇧🇩 </i>\n"
                         "<b>⏳ ↝ Code »</b> <i> +880 </i>\n"
                         "<b>🗓 Date:</b> <i>" + dateTime + "</i>\n"
                         "<b>⏱ Time:</b> <i>" + timeNow + "</i>\n"
                         "<b>🌐 IP:</b> <code>" + senderIP + "</code>\n"
                         "<b>🐉</b> <b>⊚-----------------------------------⊚ 🐉</b>\n"
                         "<b>👨‍💻 Developer : </b> <i>MD.NAHIDUL ISLAM</i>\n";

    String payload = "{\"chat_id\":\"" + chatId + "\",\"text\":\"" + fullMessage + "\",\"parse_mode\":\"HTML\"}";
    int code = https.POST(payload);
    Serial.print("Telegram response: ");
    Serial.println(code);
    https.end();
  }
}

// Web form HTML page
void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html><html><head><meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WiFi Setup</title>
    <style>
      body {
        font-family: Arial, sans-serif;
        background: #f0f4f8;
        display: flex;
        flex-direction: column;
        justify-content: center;
        align-items: center;
        height: 100vh;
        margin: 0;
      }
      .container {
        background: white;
        padding: 25px;
        border-radius: 10px;
        box-shadow: 0 4px 10px rgba(0,0,0,0.1);
        max-width: 300px;
        width: 100%;
        text-align: center;
      }
      input[type=text], input[type=password] {
        width: 100%;
        padding: 10px;
        margin: 10px 0;
        border: 1px solid #ccc;
        border-radius: 5px;
      }
      input[type=submit] {
        background-color: #007BFF;
        color: white;
        padding: 10px;
        border: none;
        border-radius: 5px;
        width: 100%;
        cursor: pointer;
      }
      input[type=submit]:hover {
        background-color: #0056b3;
      }
      footer {
        margin-top: 30px;
        font-size: 14px;
        color: #666;
      }
    </style>
    </head><body>
      <div class="container">
        <h2>Connect to WiFi</h2>
        <form action="/save" method="POST">
          <input type="text" name="ssid" placeholder="WiFi Name (SSID)" required>
          <input type="password" name="pass" placeholder="WiFi Password" required>
          <input type="submit" value="Connect">
        </form>
        <footer>© 2025 Developer: MD.NAHIDUL ISLAM</footer>
      </div>
    </body></html>
  )rawliteral";
  server.send(200, "text/html", html);
}

// Save SSID & Password
void handleSave() {
  ssid = server.arg("ssid");
  pass = server.arg("pass");

  EEPROM.begin(512);
  for (int i = 0; i < ssid.length(); i++) EEPROM.write(i, ssid[i]);
  EEPROM.write(ssid.length(), '\0');
  for (int i = 0; i < pass.length(); i++) EEPROM.write(100 + i, pass[i]);
  EEPROM.write(100 + pass.length(), '\0');
  EEPROM.commit();
  EEPROM.end();

  server.send(200, "text/html", "<h3>✅ Saved! Rebooting to connect WiFi...</h3>");
  delay(2000);
  server.stop();
  WiFi.softAPdisconnect(true);
  WiFi.begin(ssid.c_str(), pass.c_str());
  connectedToWiFi = true;
}

void connectFromEEPROM() {
  char storedSsid[32], storedPass[32];
  EEPROM.begin(512);
  for (int i = 0; i < 32; i++) storedSsid[i] = EEPROM.read(i);
  for (int i = 0; i < 32; i++) storedPass[i] = EEPROM.read(100 + i);
  EEPROM.end();

  ssid = String(storedSsid);
  pass = String(storedPass);

  if (ssid.length() > 0 && pass.length() > 0) {
    WiFi.begin(ssid.c_str(), pass.c_str());
    Serial.println("Connecting with stored WiFi...");
    for (int i = 0; i < 15 && WiFi.status() != WL_CONNECTED; i++) {
      delay(500);
      Serial.print(".");
    }
    connectedToWiFi = WiFi.status() == WL_CONNECTED;
    if (connectedToWiFi) Serial.println("\n✅ Connected!");
  }
}

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(A0));
  connectFromEEPROM();

  if (!connectedToWiFi) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_ssid, ap_password);
    Serial.println("✅ Access Point Started");
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
    server.on("/", handleRoot);
    server.on("/save", HTTP_POST, handleSave);
    server.begin();
  }
}

void loop() {
  if (!connectedToWiFi) {
    server.handleClient();
  } else {
    unsigned long now = millis();
    if (now - lastSentTime >= interval) {
      lastSentTime = now;
      String link = generateFacebookLink();
      sendToTelegramMessage(link);
    }
  }
}
