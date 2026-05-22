#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Eye Settings
int ref_eye_height = 40;
int ref_eye_width = 40;
int ref_space_between_eye = 10;
int ref_corner_radius = 10;

int left_eye_height = ref_eye_height;
int left_eye_width = ref_eye_width;
int left_eye_x = 32;
int left_eye_y = 32;

int right_eye_x = 32 + ref_eye_width + ref_space_between_eye;
int right_eye_y = 32;
int right_eye_height = ref_eye_height;
int right_eye_width = ref_eye_width;

// ========================= DRAW =========================

void draw_eyes(bool update = true)
{
  display.clearDisplay();

  int x = left_eye_x - left_eye_width / 2;
  int y = left_eye_y - left_eye_height / 2;

  display.fillRoundRect(
    x,
    y,
    left_eye_width,
    left_eye_height,
    ref_corner_radius,
    SSD1306_WHITE
  );

  x = right_eye_x - right_eye_width / 2;
  y = right_eye_y - right_eye_height / 2;

  display.fillRoundRect(
    x,
    y,
    right_eye_width,
    right_eye_height,
    ref_corner_radius,
    SSD1306_WHITE
  );

  if (update)
  {
    display.display();
  }
}

// ========================= CENTER =========================

void center_eyes(bool update = true)
{
  left_eye_height = ref_eye_height;
  left_eye_width = ref_eye_width;

  right_eye_height = ref_eye_height;
  right_eye_width = ref_eye_width;

  left_eye_x = SCREEN_WIDTH / 2 - ref_eye_width / 2 - ref_space_between_eye / 2;
  left_eye_y = SCREEN_HEIGHT / 2;

  right_eye_x = SCREEN_WIDTH / 2 + ref_eye_width / 2 + ref_space_between_eye / 2;
  right_eye_y = SCREEN_HEIGHT / 2;

  draw_eyes(update);
}

// ========================= BLINK =========================

void blink(int speed = 10)
{
  for (int i = 0; i < 3; i++)
  {
    left_eye_height -= speed;
    right_eye_height -= speed;

    draw_eyes();
    delay(20);
  }

  for (int i = 0; i < 3; i++)
  {
    left_eye_height += speed;
    right_eye_height += speed;

    draw_eyes();
    delay(20);
  }
}

// ========================= SLEEP =========================

void sleep_eye()
{
  left_eye_height = 2;
  right_eye_height = 2;

  draw_eyes(true);
}

// ========================= WAKE =========================

void wakeup()
{
  sleep_eye();

  for (int h = 2; h <= ref_eye_height; h += 2)
  {
    left_eye_height = h;
    right_eye_height = h;

    draw_eyes(true);
    delay(20);
  }
}

// ========================= HAPPY =========================

void happy_eye()
{
  center_eyes(false);

  int offset = ref_eye_height / 2;

  for (int i = 0; i < 10; i++)
  {
    display.fillTriangle(
      left_eye_x - left_eye_width / 2,
      left_eye_y + offset,

      left_eye_x + left_eye_width / 2,
      left_eye_y + 5 + offset,

      left_eye_x - left_eye_width / 2,
      left_eye_y + left_eye_height + offset,

      SSD1306_BLACK
    );

    display.fillTriangle(
      right_eye_x + right_eye_width / 2,
      right_eye_y + offset,

      right_eye_x - right_eye_width / 2,
      right_eye_y + 5 + offset,

      right_eye_x + right_eye_width / 2,
      right_eye_y + right_eye_height + offset,

      SSD1306_BLACK
    );

    offset -= 2;

    display.display();
    delay(30);
  }

  delay(700);
}

// ========================= ANGRY =========================

void angry_eye()
{
  center_eyes(false);

  display.fillTriangle(
    left_eye_x - 20,
    left_eye_y - 20,

    left_eye_x + 20,
    left_eye_y - 5,

    left_eye_x + 20,
    left_eye_y - 20,

    SSD1306_BLACK
  );

  display.fillTriangle(
    right_eye_x - 20,
    right_eye_y - 5,

    right_eye_x + 20,
    right_eye_y - 20,

    right_eye_x - 20,
    right_eye_y - 20,

    SSD1306_BLACK
  );

  display.display();

  delay(1000);
}

// ========================= SAD =========================

void sad_eye()
{
  center_eyes(false);

  display.fillTriangle(
    left_eye_x - 20,
    left_eye_y + 20,

    left_eye_x + 20,
    left_eye_y + 5,

    left_eye_x + 20,
    left_eye_y + 20,

    SSD1306_BLACK
  );

  display.fillTriangle(
    right_eye_x - 20,
    right_eye_y + 5,

    right_eye_x + 20,
    right_eye_y + 20,

    right_eye_x - 20,
    right_eye_y + 20,

    SSD1306_BLACK
  );

  display.display();

  delay(1000);
}

// ========================= LOOK LEFT =========================

void look_left()
{
  left_eye_x -= 10;
  right_eye_x -= 10;

  draw_eyes(true);

  delay(700);

  center_eyes(true);
}

// ========================= LOOK RIGHT =========================

void look_right()
{
  left_eye_x += 10;
  right_eye_x += 10;

  draw_eyes(true);

  delay(700);

  center_eyes(true);
}

// ========================= LOOK UP =========================

void look_up()
{
  left_eye_y -= 8;
  right_eye_y -= 8;

  draw_eyes(true);

  delay(700);

  center_eyes(true);
}

// ========================= LOOK DOWN =========================

void look_down()
{
  left_eye_y += 8;
  right_eye_y += 8;

  draw_eyes(true);

  delay(700);

  center_eyes(true);
}

// ========================= CRAZY =========================

void crazy_eye()
{
  for (int i = 0; i < 8; i++)
  {
    left_eye_x = random(25, 50);
    left_eye_y = random(20, 45);

    right_eye_x = random(75, 105);
    right_eye_y = random(20, 45);

    draw_eyes(true);

    delay(120);
  }

  center_eyes(true);
}

// ========================= WINK =========================

void wink_left()
{
  left_eye_height = 2;

  draw_eyes(true);

  delay(700);

  center_eyes(true);
}

void wink_right()
{
  right_eye_height = 2;

  draw_eyes(true);

  delay(700);

  center_eyes(true);
}

// ========================= BIG EYE =========================

void surprise_eye()
{
  left_eye_width += 10;
  left_eye_height += 10;

  right_eye_width += 10;
  right_eye_height += 10;

  draw_eyes(true);

  delay(1000);

  center_eyes(true);
}

// ========================= LOADING =========================

void loading_eye()
{
  for (int i = 0; i < 5; i++)
  {
    blink(8);
    delay(100);
  }
}

// ========================= SETUP =========================

void setup()
{
  Serial.begin(115200);

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);

  display.clearDisplay();
  display.display();

  randomSeed(analogRead(A0));

  wakeup();
}

// ========================= LOOP =========================

void loop()
{
  center_eyes(true);
  delay(500);

  blink(10);
  delay(300);

  happy_eye();
  delay(300);

  angry_eye();
  delay(300);

  sad_eye();
  delay(300);

  look_left();
  delay(300);

  look_right();
  delay(300);

  look_up();
  delay(300);

  look_down();
  delay(300);

  wink_left();
  delay(300);

  wink_right();
  delay(300);

  surprise_eye();
  delay(300);

  crazy_eye();
  delay(300);

  loading_eye();
  delay(300);

  sleep_eye();
  delay(1000);

  wakeup();
  delay(500);
}
