#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Config
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// WiFi Setup
const char* ssid = "ROBOEYES";
const char* password = "12345678";

ESP8266WebServer server(80);

// Moods (OK renamed to MOOD_OK to avoid conflict)
enum MoodType { SAD, GOOD, BAD, ANGRY, EXCITED, MOOD_OK, CUTE, LOVE, ALL };
MoodType currentMood = GOOD;

void drawEyes(String mood) {
  display.clearDisplay();

  // Common Eyes
  display.fillRoundRect(20, 20, 30, 30, 8, WHITE); // Left Eye
  display.fillRoundRect(78, 20, 30, 30, 8, WHITE); // Right Eye

  // Mood-specific expressions
  if (mood == "SAD") {
    display.drawLine(30, 50, 50, 55, WHITE);
    display.drawLine(90, 55, 110, 50, WHITE);
  } else if (mood == "GOOD") {
    display.drawCircle(64, 55, 8, WHITE);
  } else if (mood == "BAD") {
    display.drawLine(30, 55, 50, 50, WHITE);
    display.drawLine(90, 50, 110, 55, WHITE);
  } else if (mood == "ANGRY") {
    display.drawLine(20, 20, 50, 10, BLACK);
    display.drawLine(108, 10, 78, 20, BLACK);
  } else if (mood == "EXCITED") {
    display.drawCircle(64, 55, 12, WHITE);
    display.drawCircle(64, 55, 6, WHITE);
  } else if (mood == "OK") {
    display.drawLine(50, 55, 78, 55, WHITE);
  } else if (mood == "CUTE") {
    display.drawRoundRect(30, 55, 70, 5, 2, WHITE);
  } else if (mood == "LOVE") {
    display.fillCircle(55, 55, 4, WHITE);
    display.fillCircle(72, 55, 4, WHITE);
    display.drawLine(50, 58, 64, 65, WHITE);
    display.drawLine(64, 65, 78, 58, WHITE);
  } else if (mood == "ALL") {
    display.setCursor(30, 55);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print("ALL MOODS!");
  }

  display.display();
}

void handleRoot() {
  String html = R"====(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width">
<style>
body{margin:0;background:#000;color:#0f0;font-family:monospace;text-align:center;height:100vh;display:flex;flex-direction:column;justify-content:center}
.grid{display:grid;grid-template-columns:repeat(3,1fr);gap:3vmin;margin:3vmin auto;width:90%}
button{background:none;border:2px solid #0f0;color:#0f0;font-size:5vmin;padding:3vmin;border-radius:2vmin}
footer{position:fixed;bottom:0;width:100%;background:#020;color:#0f0;font-size:3.5vmin;padding:2vmin;display:flex;justify-content:space-around;align-items:center;border-top:1px solid #0f0}
footer a{color:#0f0;text-decoration:none;font-weight:bold}
</style></head><body>
<nav style="margin-top:0px;"><marquee><h2>WELLCOME TO MY WEB PAGE. PLEASE CONTRACT WITH MD.NAHIDUL ISLAM FOR MORE SAPORT . </h2> </marquee></nav>
<div class="grid">
<button onclick="sendMood('SAD')">SAD</button><button onclick="sendMood('GOOD')">GOOD</button><button onclick="sendMood('BAD')">BAD</button>
<button onclick="sendMood('ANGRY')">ANGRY</button><button onclick="sendMood('EXCITED')">EXCITED</button><button onclick="sendMood('OK')">OK</button>
<button onclick="sendMood('CUTE')">CUTE</button><button onclick="sendMood('LOVE')">LOVE</button><button onclick="sendMood('ALL')">ALL</button>
</div>
<footer>
<button><a href="tel:+8801328276240">Call</a></button>
<span><a href="https://cybercop-404.github.io/">MD.NAHIDUL ISLAM</a></span>
<button><a href="https://wa.link/ojiyzz">Msg</a></button>
</footer>
<script>
function sendMood(mood){
  fetch("/set?mood="+mood);
}
</script>
</body></html>
)====";
  server.send(200, "text/html", html);
}

void handleSetMood() {
  if (server.hasArg("mood")) {
    String mood = server.arg("mood");
    Serial.print("Mood: ");
    Serial.println(mood);
    drawEyes(mood);
    server.send(200, "text/plain", "");
  } else {
    server.send(400, "text/plain", "No mood found");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  Serial.println("WiFi started");
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found!");
    while (1);
  }
  display.clearDisplay();
  display.display();
  drawEyes("GOOD"); // Default mood

  server.on("/", handleRoot);
  server.on("/set", handleSetMood);
  server.begin();
  Serial.println("Server started");
}

void loop() {
  server.handleClient();
}
