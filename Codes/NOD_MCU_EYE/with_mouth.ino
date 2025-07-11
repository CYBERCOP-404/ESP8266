#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Display config
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Mono display color usage
#define BGCOLOR 0
#define MAINCOLOR 1

// Moods
#define DEFAULT 0
#define TIRED 1
#define ANGRY 2
#define HAPPY 3

// Positions
#define N 1
#define NE 2
#define E 3
#define SE 4
#define S 5
#define SW 6
#define W 7
#define NW 8

class roboEyes {
public:
  int screenWidth = 128, screenHeight = 64, frameInterval = 20;
  unsigned long fpsTimer = 0;
  bool tired = 0, angry = 0, happy = 0, curious = 0, cyclops = 0, eyeL_open = 0, eyeR_open = 0;
  int eyeLwidthDefault = 36, eyeLheightDefault = 36, eyeLwidthCurrent = 36, eyeLheightCurrent = 1, eyeLwidthNext = 36, eyeLheightNext = 36;
  int eyeRwidthDefault = 36, eyeRheightDefault = 36, eyeRwidthCurrent = 36, eyeRheightCurrent = 1, eyeRwidthNext = 36, eyeRheightNext = 36;
  int eyeLheightOffset = 0, eyeRheightOffset = 0;
  byte eyeLborderRadiusDefault = 8, eyeLborderRadiusCurrent = 8, eyeLborderRadiusNext = 8;
  byte eyeRborderRadiusDefault = 8, eyeRborderRadiusCurrent = 8, eyeRborderRadiusNext = 8;
  int spaceBetweenDefault = 10, spaceBetweenCurrent = 10, spaceBetweenNext = 10;
  int eyeLxDefault = 23, eyeLyDefault = 14;
  int eyeLx = 23, eyeLy = 14, eyeLxNext = 23, eyeLyNext = 14;
  int eyeRx = 69, eyeRy = 14, eyeRxNext = 69, eyeRyNext = 14;
  byte eyelidsHeightMax = 18, eyelidsTiredHeight = 0, eyelidsTiredHeightNext = 0;
  byte eyelidsAngryHeight = 0, eyelidsAngryHeightNext = 0;
  byte eyelidsHappyBottomOffsetMax = 21, eyelidsHappyBottomOffset = 0, eyelidsHappyBottomOffsetNext = 0;
  bool hFlicker = 0, hFlickerAlternate = 0, vFlicker = 0, vFlickerAlternate = 0;
  byte hFlickerAmplitude = 2, vFlickerAmplitude = 10;
  bool autoblinker = 0;
  int blinkInterval = 1, blinkIntervalVariation = 4;
  unsigned long blinktimer = 0;
  bool idle = 0;
  int idleInterval = 1, idleIntervalVariation = 3;
  unsigned long idleAnimationTimer = 0;
  bool confused = 0;
  unsigned long confusedAnimationTimer = 0;
  int confusedAnimationDuration = 500;
  bool confusedToggle = 1;
  bool laugh = 0;
  unsigned long laughAnimationTimer = 0;
  int laughAnimationDuration = 500;
  bool laughToggle = 1;

  // Timer for mood randomizer
  unsigned long moodTimer = 0;
  int moodChangeInterval = 10000; // 10 seconds

  void begin(int width, int height, byte frameRate) {
    screenWidth = width; screenHeight = height;
    display.clearDisplay(); display.display();
    eyeLheightCurrent = 1; eyeRheightCurrent = 1;
    setFramerate(frameRate);
    blinktimer = millis() + 1000;
    idleAnimationTimer = millis() + 1000;
    moodTimer = millis() + moodChangeInterval;
  }

  void update() {
    if (millis() - fpsTimer >= frameInterval) {
      // Randomly change mood every 10 seconds
      if (millis() >= moodTimer) {
        int newMood = random(0, 4);
        setMood(newMood);
        moodTimer = millis() + moodChangeInterval;
      }

      // Randomly move eyes when idle
      if (idle && millis() >= idleAnimationTimer) {
        eyeLxNext = random(getScreenConstraint_X());
        eyeLyNext = random(getScreenConstraint_Y());
        idleAnimationTimer = millis() + (idleInterval * 1000) + (random(idleIntervalVariation) * 1000);
      }

      drawEyes();
      fpsTimer = millis();
    }
  }

  void setFramerate(byte fps) { frameInterval = 1000 / fps; }
  void setMood(unsigned char mood) {
    tired = (mood == TIRED);
    angry = (mood == ANGRY);
    happy = (mood == HAPPY);
    if (mood == DEFAULT) {
      tired = angry = happy = false;
    }
  }
  void setAutoblinker(bool active, int interval = 3, int variation = 2) {
    autoblinker = active; blinkInterval = interval; blinkIntervalVariation = variation;
  }
  void setIdleMode(bool active, int interval = 3, int variation = 2) {
    idle = active; idleInterval = interval; idleIntervalVariation = variation;
  }
  void setCuriosity(bool curiousBit) { curious = curiousBit; }
  void setCyclops(bool cyclopsBit) { cyclops = cyclopsBit; }
  void open() { eyeL_open = 1; eyeR_open = 1; }
  void close() { eyeLheightNext = 1; eyeRheightNext = 1; eyeL_open = 0; eyeR_open = 0; }
  void blink() { close(); open(); }
  void anim_confused() { confused = 1; }
  void anim_laugh() { laugh = 1; }

  int getScreenConstraint_X() { return screenWidth - eyeLwidthCurrent - spaceBetweenCurrent - eyeRwidthCurrent; }
  int getScreenConstraint_Y() { return screenHeight - eyeLheightDefault; }

  void drawMouth() {
    int mouthWidth = 60;
    int mouthHeight = 12;
    int mouthX = (eyeLx + eyeRx + eyeRwidthCurrent) / 2 - mouthWidth / 2;
    int mouthY = eyeLy + eyeLheightDefault + 10; // 10 pixels below eyes

    if (happy) {
      // Smile shape
      display.fillTriangle(mouthX, mouthY + mouthHeight / 2,
                           mouthX + mouthWidth / 2, mouthY,
                           mouthX + mouthWidth, mouthY + mouthHeight / 2, MAINCOLOR);
      display.fillRect(mouthX + 5, mouthY + mouthHeight / 2, mouthWidth - 10, mouthHeight / 3, BGCOLOR);
    }
    else if (tired) {
      // Flat line mouth
      display.drawLine(mouthX, mouthY + mouthHeight / 2, mouthX + mouthWidth, mouthY + mouthHeight / 2, MAINCOLOR);
    }
    else if (angry) {
      // Angry frown
      display.drawLine(mouthX, mouthY + mouthHeight, mouthX + mouthWidth / 2, mouthY, MAINCOLOR);
      display.drawLine(mouthX + mouthWidth / 2, mouthY, mouthX + mouthWidth, mouthY + mouthHeight, MAINCOLOR);
    }
    else {
      // Neutral mouth
      display.drawLine(mouthX + 10, mouthY + mouthHeight / 2, mouthX + mouthWidth - 10, mouthY + mouthHeight / 2, MAINCOLOR);
    }
  }

  void drawEyes() {
    if (curious) {
      eyeLheightOffset = (eyeLxNext <= 10) ? 8 : 0;
      eyeRheightOffset = (eyeRxNext >= screenWidth - eyeRwidthCurrent - 10) ? 8 : 0;
    } else {
      eyeLheightOffset = eyeRheightOffset = 0;
    }

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

    if (laugh) {
      if (laughToggle) {
        vFlicker = 1; vFlickerAmplitude = 5; laughAnimationTimer = millis(); laughToggle = 0;
      }
      else if (millis() >= laughAnimationTimer + laughAnimationDuration) {
        vFlicker = 0; vFlickerAmplitude = 0; laughToggle = 1; laugh = 0;
      }
    }

    if (confused) {
      if (confusedToggle) {
        hFlicker = 1; hFlickerAmplitude = 20; confusedAnimationTimer = millis(); confusedToggle = 0;
      }
      else if (millis() >= confusedAnimationTimer + confusedAnimationDuration) {
        hFlicker = 0; hFlickerAmplitude = 0; confusedToggle = 1; confused = 0;
      }
    }

    if (idle && millis() >= idleAnimationTimer) {
      eyeLxNext = random(getScreenConstraint_X());
      eyeLyNext = random(getScreenConstraint_Y());
      idleAnimationTimer = millis() + (idleInterval * 1000) + (random(idleIntervalVariation) * 1000);
    }

    if (hFlicker) {
      int offset = (hFlickerAlternate) ? hFlickerAmplitude : -hFlickerAmplitude;
      eyeLx += offset; eyeRx += offset; hFlickerAlternate = !hFlickerAlternate;
    }
    if (vFlicker) {
      int offset = (vFlickerAlternate) ? vFlickerAmplitude : -vFlickerAmplitude;
      eyeLy += offset; eyeRy += offset; vFlickerAlternate = !vFlickerAlternate;
    }

    if (cyclops) {
      eyeRwidthCurrent = 0; eyeRheightCurrent = 0; spaceBetweenCurrent = 0;
    }

    display.clearDisplay();

    // Draw eyes
    display.fillRoundRect(eyeLx, eyeLy, eyeLwidthCurrent, eyeLheightCurrent, eyeLborderRadiusCurrent, MAINCOLOR);
    if (!cyclops) display.fillRoundRect(eyeRx, eyeRy, eyeRwidthCurrent, eyeRheightCurrent, eyeRborderRadiusCurrent, MAINCOLOR);

    eyelidsTiredHeight = (eyelidsTiredHeight + (tired ? eyeLheightCurrent / 2 : 0)) / 2;
    eyelidsAngryHeight = (eyelidsAngryHeight + (angry ? eyeLheightCurrent / 2 : 0)) / 2;
    eyelidsHappyBottomOffset = (eyelidsHappyBottomOffset + (happy ? eyeLheightCurrent / 2 : 0)) / 2;

    if (tired) {
      display.fillTriangle(eyeLx, eyeLy - 1, eyeLx + eyeLwidthCurrent, eyeLy - 1, eyeLx, eyeLy + eyelidsTiredHeight - 1, BGCOLOR);
      if (!cyclops) display.fillTriangle(eyeRx, eyeRy - 1, eyeRx + eyeRwidthCurrent, eyeRy - 1, eyeRx + eyeRwidthCurrent, eyeRy + eyelidsTiredHeight - 1, BGCOLOR);
    }

    if (angry) {
      display.fillTriangle(eyeLx, eyeLy - 1, eyeLx + eyeLwidthCurrent, eyeLy - 1, eyeLx + eyeLwidthCurrent, eyeLy + eyelidsAngryHeight - 1, BGCOLOR);
      if (!cyclops) display.fillTriangle(eyeRx, eyeRy - 1, eyeRx + eyeRwidthCurrent, eyeRy - 1, eyeRx, eyeRy + eyelidsAngryHeight - 1, BGCOLOR);
    }

    display.fillRoundRect(eyeLx - 1, (eyeLy + eyeLheightCurrent) - eyelidsHappyBottomOffset + 1, eyeLwidthCurrent + 2, eyeLheightDefault, eyeLborderRadiusCurrent, BGCOLOR);
    if (!cyclops) display.fillRoundRect(eyeRx - 1, (eyeRy + eyeRheightCurrent) - eyelidsHappyBottomOffset + 1, eyeRwidthCurrent + 2, eyeRheightDefault, eyeRborderRadiusCurrent, BGCOLOR);

    // Draw mouth under eyes
    drawMouth();

    display.display();
  }
};

roboEyes eyes;

void setup() {
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 init failed");
    while (true);
  }
  display.clearDisplay();
  display.display();

  eyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 60);
  eyes.setAutoblinker(true, 2, 2);
  eyes.setIdleMode(true, 3, 2);
  eyes.setMood(HAPPY);
  eyes.setCuriosity(true);
  eyes.open();
}

void loop() {
  eyes.update();
}
