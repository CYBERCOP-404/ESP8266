#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED config
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// WiFi AP credentials
const char* ssid = "FREE WIFI";
const char* password = "12345678";

ESP8266WebServer server(80);

// Mood definitions
#define MOOD_DEFAULT 0
#define MOOD_TIRED 1
#define MOOD_ANGRY 2
#define MOOD_HAPPY 3

// Display color
#define BGCOLOR 0
#define MAINCOLOR 1

class roboEyes {
public:
  int screenWidth = 128, screenHeight = 64, frameInterval = 20;
  unsigned long fpsTimer = 0;
  bool tired = false, angry = false, happy = false;
  bool eyeL_open = false, eyeR_open = false;
  int eyeLwidthDefault = 36, eyeLheightDefault = 36;
  int eyeLwidthCurrent = 36, eyeLheightCurrent = 1;
  int eyeLwidthNext = 36, eyeLheightNext = 36;
  int eyeRwidthDefault = 36, eyeRheightDefault = 36;
  int eyeRwidthCurrent = 36, eyeRheightCurrent = 1;
  int eyeRwidthNext = 36, eyeRheightNext = 36;
  int eyeLheightOffset = 0, eyeRheightOffset = 0;
  byte eyeLborderRadiusDefault = 8, eyeLborderRadiusCurrent = 8, eyeLborderRadiusNext = 8;
  byte eyeRborderRadiusDefault = 8, eyeRborderRadiusCurrent = 8, eyeRborderRadiusNext = 8;
  int spaceBetweenDefault = 10, spaceBetweenCurrent = 10, spaceBetweenNext = 10;
  int eyeLxDefault = 23, eyeLyDefault = 14;
  int eyeLx = 23, eyeLy = 14, eyeLxNext = 23, eyeLyNext = 14;
  int eyeRx = 69, eyeRy = 14, eyeRxNext = 69, eyeRyNext = 14;
  bool autoblinker = false;
  int blinkInterval = 3, blinkIntervalVariation = 2;
  unsigned long blinktimer = 0;

  void begin(int width, int height, byte frameRate) {
    screenWidth = width; screenHeight = height;
    display.clearDisplay(); display.display();
    eyeLheightCurrent = 1; eyeRheightCurrent = 1;
    setFramerate(frameRate);
  }

  void update() {
    if (millis() - fpsTimer >= frameInterval) {
      drawEyes();
      drawMouth();
      fpsTimer = millis();
    }
  }

  void setFramerate(byte fps) { frameInterval = 1000 / fps; }
  void setMood(uint8_t mood) {
    tired = angry = happy = false;
    switch (mood) {
      case MOOD_TIRED: tired = true; break;
      case MOOD_ANGRY: angry = true; break;
      case MOOD_HAPPY: happy = true; break;
      default: break;
    }
  }
  void setAutoblinker(bool active, int interval = 3, int variation = 2) {
    autoblinker = active; blinkInterval = interval; blinkIntervalVariation = variation;
  }
  void open() { eyeL_open = true; eyeR_open = true; }
  void close() { eyeLheightNext = 1; eyeRheightNext = 1; eyeL_open = false; eyeR_open = false; }
  void blink() { close(); open(); }

  void drawEyes() {
    eyeLheightOffset = 0; eyeRheightOffset = 0;

    eyeLheightCurrent = (eyeLheightCurrent + eyeLheightNext + eyeLheightOffset) / 2;
    eyeRheightCurrent = (eyeRheightCurrent + eyeRheightNext + eyeRheightOffset) / 2;

    eyeLy += ((eyeLheightDefault - eyeLheightCurrent) / 2) - eyeLheightOffset / 2;
    eyeRy += ((eyeRheightDefault - eyeRheightCurrent) / 2) - eyeRheightOffset / 2;

    if (eyeL_open && eyeLheightCurrent <= 1 + eyeLheightOffset) eyeLheightNext = eyeLheightDefault;
    if (eyeR_open && eyeRheightCurrent <= 1 + eyeRheightOffset) eyeRheightNext = eyeRheightDefault;

    eyeLwidthCurrent = (eyeLwidthCurrent + eyeLwidthNext) / 2;
    eyeRwidthCurrent = (eyeRwidthCurrent + eyeRwidthNext) / 2;

    spaceBetweenCurrent = (spaceBetweenCurrent + spaceBetweenNext) / 2;

    eyeLx = (eyeLx + eyeLxNext) / 2;
    eyeLy = (eyeLy + eyeLyNext) / 2;

    eyeRxNext = eyeLxNext + eyeLwidthCurrent + spaceBetweenCurrent;
    eyeRyNext = eyeLyNext;

    eyeRx = (eyeRx + eyeRxNext) / 2;
    eyeRy = (eyeRy + eyeRyNext) / 2;

    eyeLborderRadiusCurrent = (eyeLborderRadiusCurrent + eyeLborderRadiusNext) / 2;
    eyeRborderRadiusCurrent = (eyeRborderRadiusCurrent + eyeRborderRadiusNext) / 2;

    if (autoblinker && millis() >= blinktimer) {
      blink();
      blinktimer = millis() + (blinkInterval * 1000) + (random(blinkIntervalVariation) * 1000);
    }

    display.clearDisplay();
    display.fillRoundRect(eyeLx, eyeLy, eyeLwidthCurrent, eyeLheightCurrent, eyeLborderRadiusCurrent, MAINCOLOR);
    display.fillRoundRect(eyeRx, eyeRy, eyeRwidthCurrent, eyeRheightCurrent, eyeRborderRadiusCurrent, MAINCOLOR);

    // Eyelids for tired mood
    if (tired) {
      display.fillTriangle(eyeLx, eyeLy - 1, eyeLx + eyeLwidthCurrent, eyeLy - 1, eyeLx, eyeLy + eyeLheightCurrent / 2 - 1, BGCOLOR);
      display.fillTriangle(eyeRx, eyeRy - 1, eyeRx + eyeRwidthCurrent, eyeRy - 1, eyeRx + eyeRwidthCurrent, eyeRy + eyeRheightCurrent / 2 - 1, BGCOLOR);
    }

    // Eyelids for angry mood
    if (angry) {
      display.fillTriangle(eyeLx, eyeLy - 1, eyeLx + eyeLwidthCurrent, eyeLy - 1, eyeLx + eyeLwidthCurrent, eyeLy + eyeLheightCurrent / 2 - 1, BGCOLOR);
      display.fillTriangle(eyeRx, eyeRy - 1, eyeRx + eyeRwidthCurrent, eyeRy - 1, eyeRx, eyeRy + eyeRheightCurrent / 2 - 1, BGCOLOR);
    }

    display.display();
  }

  void drawMouth() {
    int mouthWidth = 50;
    int mouthHeight = 10;
    int mouthX = (screenWidth / 2) - (mouthWidth / 2);
    int mouthY = eyeLy + eyeLheightDefault + 10;

    display.fillRect(mouthX - 2, mouthY - 2, mouthWidth + 4, mouthHeight + 4, BGCOLOR);

    if (happy) {
      // Smile (arc)
      drawArc(mouthX + mouthWidth / 2, mouthY + mouthHeight / 2, mouthWidth / 2, mouthHeight / 2, 20, 160, MAINCOLOR);
    } else if (tired) {
      // flat tired mouth line
      display.drawLine(mouthX, mouthY + mouthHeight / 2, mouthX + mouthWidth, mouthY + mouthHeight / 2, MAINCOLOR);
    } else if (angry) {
      // angry mouth line with down curve
      display.drawLine(mouthX, mouthY + mouthHeight / 2 + 3, mouthX + mouthWidth / 2, mouthY + mouthHeight / 2, MAINCOLOR);
      display.drawLine(mouthX + mouthWidth / 2, mouthY + mouthHeight / 2, mouthX + mouthWidth, mouthY + mouthHeight / 2 + 3, MAINCOLOR);
    } else {
      // default neutral line
      display.drawLine(mouthX, mouthY + mouthHeight / 2, mouthX + mouthWidth, mouthY + mouthHeight / 2, MAINCOLOR);
    }
  }

  void drawArc(int x0, int y0, int rx, int ry, int startAngle, int endAngle, uint16_t color) {
    float step = 5.0;
    for (float angle = startAngle; angle < endAngle; angle += step) {
      float rad1 = angle * 3.14159265 / 180.0;
      float rad2 = (angle + step) * 3.14159265 / 180.0;
      int x1 = x0 + rx * cos(rad1);
      int y1 = y0 + ry * sin(rad1);
      int x2 = x0 + rx * cos(rad2);
      int y2 = y0 + ry * sin(rad2);
      display.drawLine(x1, y1, x2, y2, color);
    }
  }
};

roboEyes eyes;

// Current mood, starts happy
uint8_t currentMood = MOOD_HAPPY;

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html><html>
<head>
<title>RoboEyes Control</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body { font-family: Arial; text-align: center; background: #222; color: #eee; }
button { font-size: 20px; margin: 10px; padding: 15px 30px; border-radius: 8px; border:none; cursor:pointer; }
button:hover { background-color: #555; }
</style>
</head>
<body>
<h1>RoboEyes Animation Control</h1>
<button onclick="setMood('happy')">Happy</button>
<button onclick="setMood('tired')">Tired</button>
<button onclick="setMood('angry')">Angry</button>
<button onclick="setMood('default')">Default</button>

<script>
function setMood(mood) {
  fetch('/set?mood=' + mood).then(response => response.text()).then(data => alert(data));
}
</script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

void handleSetMood() {
  if (server.hasArg("mood")) {
    String mood = server.arg("mood");
    if (mood == "happy") {
      currentMood = MOOD_HAPPY;
    } else if (mood == "tired") {
      currentMood = MOOD_TIRED;
    } else if (mood == "angry") {
      currentMood = MOOD_ANGRY;
    } else {
      currentMood = MOOD_DEFAULT;
    }
    eyes.setMood(currentMood);
    server.send(200, "text/plain", "Mood changed to: " + mood);
  } else {
    server.send(400, "text/plain", "Mood parameter missing");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("Starting ESP8266 WiFi AP...");

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    while (true);
  }
  display.clearDisplay();
  display.display();

  eyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 60);
  eyes.setAutoblinker(true, 2, 2);
  eyes.setMood(currentMood);
  eyes.open();

  // Web server routes
  server.on("/", handleRoot);
  server.on("/set", handleSetMood);
  server.begin();

  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  eyes.update();
}
