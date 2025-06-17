#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

// WiFi Info
const char* ssid = "Eng Mase nich";
const char* password = "87654321";
// AP Mode Info
const char* ap_ssid = "FREE WIFI";
const char* ap_password = "12345678";

// Telegram Bot Info
const String botToken = "5789106680:AAHOFB4NgoBzzCORzErqL0LAGBFbO8I85eA";
const String chatId = "5316471518";

// Web server on port 80
ESP8266WebServer server(80);

// Send message to Telegram with decoration
void sendToTelegram(String userMessage) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure(); // Skip SSL certificate check
    HTTPClient https;

    String url = "https://api.telegram.org/bot" + botToken + "/sendMessage";
    https.begin(client, url);
    https.addHeader("Content-Type", "application/json");

    // Create full message with decoration
    String dateTime = __DATE__;
    String timeNow = __TIME__;
    String senderIP = WiFi.localIP().toString();

    String fullMessage = 
      "<b>📩 New Message Received!</b>\n\n"
      "<b>💬 Message:</b> <code>" + userMessage + "</code>\n"
      "<b>📅 Date:</b> <i>" + dateTime + "</i>\n"
      "<b>⏱ Time:</b> <i>" + timeNow + "</i>\n"
      "<b>🌐 Sender IP:</b> <code>" + senderIP + "</code>\n\n"
      "<b>✅ Sent From:</b> <i>ESP8266 Web Interface</i>";

    String jsonPayload = "{\"chat_id\":\"" + chatId + "\",\"text\":\"" + fullMessage + "\",\"parse_mode\":\"HTML\"}";

    int httpCode = https.POST(jsonPayload);
    Serial.print("HTTP Response Code: ");
    Serial.println(httpCode);

    if (httpCode == 200) {
      Serial.println("✅ Message sent to Telegram!");
    } else {
      Serial.print("❌ Failed: ");
      Serial.println(https.getString());
    }
    https.end();
  } else {
    Serial.println("❌ WiFi not connected!");
  }
}

// HTML Web Page with Form, Navbar, Footer, Animation
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0"/>
  <title>Secure Telegram Sender</title>
  <style>
    body {
      margin: 0;
      font-family: 'Segoe UI', sans-serif;
      background: linear-gradient(-45deg, #ffecd2, #fcb69f, #ff9a9e, #fad0c4);
      background-size: 400% 400%;
      animation: gradientBG 15s ease infinite;
      color: #333;
      display: flex;
      flex-direction: column;
      min-height: 100vh;
    }

    @keyframes gradientBG {
      0% {background-position: 0% 50%;}
      50% {background-position: 100% 50%;}
      100% {background-position: 0% 50%;}
    }

    nav {
      background-color: rgba(0, 0, 0, 0.7);
      padding: 15px;
      text-align: center;
      font-size: 22px;
      color: #fff;
    }

    main {
      flex: 1;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      padding: 20px;
    }

    form {
      background: rgba(255,255,255,0.9);
      padding: 30px;
      border-radius: 10px;
      box-shadow: 0 0 15px rgba(0,0,0,0.3);
      max-width: 400px;
      width: 100%;
      text-align: center;
    }

    h2 {
      margin-bottom: 20px;
      color: #222;
    }

    input[type=text] {
      width: 100%;
      padding: 12px;
      font-size: 16px;
      border: 1px solid #ccc;
      border-radius: 5px;
      margin-bottom: 15px;
    }

    input[type=submit] {
      background-color: #ff6f61;
      color: white;
      padding: 12px 20px;
      border: none;
      border-radius: 5px;
      font-size: 16px;
      cursor: pointer;
    }

    input[type=submit]:hover {
      background-color: #ff3b2e;
    }

    footer {
      background-color: rgba(0, 0, 0, 0.6);
      text-align: center;
      padding: 10px;
      font-size: 14px;
      color: #fff;
    }
  </style>
</head>
<body>
  <nav>📡 ESP8266 WiFi Message Portal</nav>
  <main>
    <form method="POST" action="/send">
      <h2>🔐 Send Message to Telegram</h2>
      <input type="text" name="message" placeholder="Type your message..." required>
      <input type="submit" value="📨 Send to Telegram">
    </form>
  </main>
  <footer>© 2025 Developed by MD. NAHIDUL ISLAM | All Rights Reserved</footer>
</body>
</html>
)rawliteral";

// Web handler functions
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleSend() {
  if (server.hasArg("message")) {
    String msg = server.arg("message");
    Serial.println("📨 Received: " + msg);
    sendToTelegram(msg);
    server.send(200, "text/html", "<h3>✅ Message sent successfully! Go back to <a href='/'>home</a>.</h3>");
  } else {
    server.send(400, "text/plain", "❌ No message found!");
  }
}

// Setup
void setup() {
  Serial.begin(115200);
  delay(100);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Connected to WiFi!");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());

  WiFi.softAP(ap_ssid, ap_password);
  Serial.print("AP Mode IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/send", HTTP_POST, handleSend);

  server.begin();
  Serial.println("✅ HTTP Server started");
}

// Loop
void loop() {
  server.handleClient();
}
