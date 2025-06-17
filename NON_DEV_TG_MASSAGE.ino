#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

// আপনার ইন্টারনেট রাউটার তথ্য দিন এখানে
const char* ssid = "Eng Mase nich";
const char* password = "87654321";

// ESP AP মোডের জন্য WiFi
const char* ap_ssid = "BOSSBOSS";
const char* ap_password = "12345678";

// Telegram Bot Token & Chat ID
const String botToken = "5789106680:AAHOFB4NgoBzzCORzErqL0LAGBFbO8I85eA";
const String chatId = "5316471518";

ESP8266WebServer server(80);

void sendToTelegram(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;  // নিরাপদ কানেকশন জন্য
    client.setInsecure();     // সার্টিফিকেট চেক বাদ দেওয়া (বৈধ না হলেও কাজ করবে)

    HTTPClient https;

    String url = "https://api.telegram.org/bot" + botToken + "/sendMessage";

    https.begin(client, url);
    https.addHeader("Content-Type", "application/json");

    // JSON ফরম্যাটে মেসেজ তৈরি
    String jsonPayload = "{\"chat_id\":\"" + chatId + "\",\"text\":\"" + message + "\"}";

    int httpCode = https.POST(jsonPayload);

    Serial.print("HTTP Response Code: ");
    Serial.println(httpCode);

    if (httpCode == 200) {
      Serial.println("✅ Message sent to Telegram!");
    } else {
      Serial.print("❌ Failed to send message! Response: ");
      Serial.println(https.getString());
    }

    https.end();
  } else {
    Serial.println("❌ WiFi not connected. Cannot send to Telegram.");
  }
}

const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Send Telegram Message</title>
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <style>
    body { font-family: Arial, sans-serif; text-align:center; padding-top:50px; }
    input[type=text] { width: 250px; padding: 10px; font-size:16px; }
    input[type=submit] { padding: 10px 20px; font-size: 16px; cursor:pointer; }
  </style>
</head>
<body>
  <h2>Send Message to Telegram</h2>
  <form method="POST" action="/send">
    <input type="text" name="message" placeholder="Enter message" required>
    <br><br>
    <input type="submit" value="Send">
  </form>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleSend() {
  if (server.hasArg("message")) {
    String msg = server.arg("message");
    Serial.println("Message received: " + msg);
    sendToTelegram(msg);
    server.send(200, "text/plain", "Message sent to Telegram!");
  } else {
    server.send(400, "text/plain", "Bad Request: No message found");
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // WiFi Client হিসেবে রাউটারে কানেক্ট করুন
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Connected to WiFi");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());

  // ESP AP চালু করুন
  WiFi.softAP(ap_ssid, ap_password);
  Serial.print("ESP AP IP: ");
  Serial.println(WiFi.softAPIP());

  // ওয়েব সার্ভারের রুট এবং /send পাথ সেট করুন
  server.on("/", handleRoot);
  server.on("/send", HTTP_POST, handleSend);

  server.begin();
  Serial.println("✅ HTTP Server started");
}

void loop() {
  server.handleClient();
}
