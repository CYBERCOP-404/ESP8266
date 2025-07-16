#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>

// সর্বোচ্চ রেকর্ড সংখ্যা
#define MAX_RECORDS 50  

struct Record {
  String fbLink;
  String timeStr;
  String dateStr;
  String ipStr;
};

Record records[MAX_RECORDS];
int recordCount = 0;

String ssid = "";
String pass = "";

// আপনার টেলিগ্রাম বট টোকেন ও চ্যাট আইডি
const String botToken = "token";
const String chatId = "5316471518";

// ESP এর AP মোডের জন্য SSID ও Password
const char* ap_ssid = "MAIN SERVER";
const char* ap_password = "12345678";

ESP8266WebServer server(80);

unsigned long lastSentTime = 0;
const unsigned long interval = 60000; // 60 সেকেন্ড (আপনি চাইলে 30000=30 সেকেন্ডও দিতে পারেন)

bool connectedToWiFi = false;

void debugPrintIP() {
  Serial.print("Connected IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
}

// র্যান্ডম Facebook নাম তৈরির ফাংশন
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

// টেলিগ্রামে মেসেজ পাঠানোর ফাংশন
void sendToTelegramMessage(String userLink) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();  // সার্টিফিকেট চেক বাদ দিচ্ছে (ক্লায়েন্টকে)
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
    Serial.print("Telegram response code: ");
    Serial.println(code);
    https.end();

    // রেকর্ড সংরক্ষণ
    if(recordCount < MAX_RECORDS){
      records[recordCount].fbLink = userLink;
      records[recordCount].timeStr = timeNow;
      records[recordCount].dateStr = dateTime;
      records[recordCount].ipStr = senderIP;
      recordCount++;
    } else {
      // ম্যাক্স হলে পুরানো রেকর্ড এক ধাপ সরিয়ে নতুন রেকর্ড যুক্ত করা
      for(int i=1; i<MAX_RECORDS; i++){
        records[i-1] = records[i];
      }
      records[MAX_RECORDS-1].fbLink = userLink;
      records[MAX_RECORDS-1].timeStr = timeNow;
      records[MAX_RECORDS-1].dateStr = dateTime;
      records[MAX_RECORDS-1].ipStr = senderIP;
    }
  }
}

// ওয়াইফাই সেটআপ পেজ
void handleRoot() {
  IPAddress localIP = WiFi.localIP();
  IPAddress apIP = WiFi.softAPIP();

  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>"
                "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
                "<title>WiFi Setup</title>"
                "<style>"
                "body{font-family:Arial,sans-serif;background:#f0f4f8;display:flex;flex-direction:column;justify-content:center;align-items:center;height:100vh;margin:0;}"
                ".container{background:white;padding:25px;border-radius:10px;box-shadow:0 4px 10px rgba(0,0,0,0.1);max-width:300px;width:100%;text-align:center;}"
                "input[type=text],input[type=password]{width:100%;padding:10px;margin:10px 0;border:1px solid #ccc;border-radius:5px;}"
                "input[type=submit]{background-color:#007BFF;color:white;padding:10px;border:none;border-radius:5px;width:100%;cursor:pointer;}"
                "input[type=submit]:hover{background-color:#0056b3;}"
                "footer{margin-top:30px;font-size:14px;color:#666;}"
                ".info{margin-top:10px; font-size:14px; color:#333;}"
                "a {color:#007BFF; text-decoration:none;}"
                "a:hover {text-decoration:underline;}"
                "</style>"
                "</head><body>"
                "<div class='container'>"
                "<h2>Connect to WiFi</h2>"
                "<form action='/save' method='POST'>"
                "<input type='text' name='ssid' placeholder='WiFi Name (SSID)' required>"
                "<input type='password' name='pass' placeholder='WiFi Password' required>"
                "<input type='submit' value='Connect'>"
                "</form>"
                "<div class='info'>";

  if (connectedToWiFi) {
    html += "🟢 Connected to WiFi<br>Device IP: <a href='http://" + localIP.toString() + "/sheet' target='_blank'>" + localIP.toString() + "</a><br>";
  } else {
    html += "🔴 Access Point Mode<br>Connect to AP: <b>" + String(ap_ssid) + "</b><br>"
            "AP IP: <a href='http://" + apIP.toString() + "/sheet' target='_blank'>" + apIP.toString() + "</a><br>";
  }

  html += "</div>"
          "<footer>© 2025 Developer: MD.NAHIDUL ISLAM</footer>"
          "</div></body></html>";

  server.send(200, "text/html", html);
}

// WiFi credentials সংরক্ষণ ও রিবুট
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

  server.send(200, "text/html", "<h3>✅ Saved! Rebooting to connect WiFi...</h3><script>setTimeout(()=>{window.location='/'},5000);</script>");
  delay(3000);

  // SoftAP বন্ধ ও WiFi শুরু করার পরিবর্তে ESP রিবুট দেয়া নিরাপদ
  ESP.restart();
}

// EEPROM থেকে WiFi credentials নিয়ে কানেক্ট করা
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
    for (int i = 0; i < 30 && WiFi.status() != WL_CONNECTED; i++) {
      delay(500);
      Serial.print(".");
    }
    connectedToWiFi = WiFi.status() == WL_CONNECTED;
    if (connectedToWiFi) Serial.println("\n✅ Connected!");
  }
}

// Telegram message sheet দেখানোর পেজ
void handleSheet() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <title>Telegram Message Sheet</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
      body {
        font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
        margin: 20px;
        background: #f9f9f9;
      }
      h2 {
        text-align: center;
        color: #333;
      }
      table {
        width: 100%;
        border-collapse: collapse;
        margin-top: 20px;
        box-shadow: 0 0 10px rgba(0,0,0,0.1);
        background: white;
      }
      th, td {
        padding: 10px;
        border: 1px solid #ddd;
        text-align: center;
        word-break: break-word;
      }
      th {
        background: linear-gradient(45deg, #4facfe, #00f2fe);
        color: white;
      }
      tr:nth-child(even) {
        background-color: #f2f2f2;
      }
      tr:hover {
        background-color: #c8e6f7;
      }
      .btn-download {
        display: block;
        width: 160px;
        margin: 20px auto;
        padding: 10px;
        background: #007BFF;
        color: white;
        text-align: center;
        border-radius: 5px;
        font-weight: bold;
        cursor: pointer;
        user-select: none;
      }
      .btn-download:hover {
        background: #0056b3;
      }
      a {
        cursor: pointer;
        color: #007BFF;
        text-decoration: none;
      }
      a:hover {
        text-decoration: underline;
      }
    </style>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/jspdf/2.5.1/jspdf.umd.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/jspdf-autotable/3.5.25/jspdf.plugin.autotable.min.js"></script>
  </head>
  <body>
    <h2>Telegram Messages Sheet</h2>
    <table id="msgTable">
      <thead>
        <tr>
          <th>#</th>
          <th>Facebook Link</th>
          <th>Time</th>
          <th>Date</th>
          <th>IP Address</th>
        </tr>
      </thead>
      <tbody>
  )rawliteral";

  for (int i = 0; i < recordCount; i++) {
    html += "<tr>";
    html += "<td>" + String(i+1) + "</td>";
    html += "<td><a href=\"" + records[i].fbLink + "\" target=\"_blank\">" + records[i].fbLink + "</a></td>";
    html += "<td>" + records[i].timeStr + "</td>";
    html += "<td>" + records[i].dateStr + "</td>";
    html += "<td>" + records[i].ipStr + "</td>";
    html += "</tr>";
  }

  html += R"rawliteral(
      </tbody>
    </table>
    <div class="btn-download" onclick="downloadPDF()">Download PDF</div>

    <script>
      async function downloadPDF() {
        const { jsPDF } = window.jspdf;
        const doc = new jsPDF();

        doc.setFontSize(18);
        doc.text("Telegram Messages Sheet", 14, 22);

        doc.setFontSize(11);
        doc.setTextColor(100);

        const res = doc.autoTableHtmlToJson(document.getElementById("msgTable"));
        doc.autoTable({
          head: [res.columns],
          body: res.data,
          startY: 30,
          theme: 'striped',
          headStyles: {fillColor: [79, 172, 254]},
          styles: {fontSize: 9}
        });

        doc.save("telegram_messages.pdf");
      }
    </script>

  </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", html);
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
  } else {
    Serial.print("✅ Connected to WiFi with IP: ");
    Serial.println(WiFi.localIP());
  }

  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.on("/sheet", handleSheet);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  if (connectedToWiFi) {
    unsigned long now = millis();
    if (now - lastSentTime >= interval) {
      lastSentTime = now;
      String link = generateFacebookLink();
      sendToTelegramMessage(link);
      Serial.println("Sent message to Telegram: " + link);
    }
  }
}
