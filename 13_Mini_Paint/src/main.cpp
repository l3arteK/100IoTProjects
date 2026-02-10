#include <WiFi.h>
#include "time.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BTN_LEFT 26
#define BTN_RIGHT 25
#define BTN_UP 27
#define BTN_DOWN 14
#define POT_PIN 4

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int dotX = 64;
int dotY = 32;
const int step = 1;
void setup()
{
  Serial.begin(115200);

  pinMode(BTN_LEFT, INPUT);
  pinMode(BTN_RIGHT, INPUT);
  pinMode(BTN_UP, INPUT);
  pinMode(BTN_DOWN, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("SSD1306 allocation failed");
    for (;;)
      ;
  }

  display.setTextColor(WHITE);
  display.clearDisplay();
}
int dotSize = 1;
void loop()
{
  dotSize = analogRead(POT_PIN) / 210;
  if (digitalRead(BTN_UP) && dotY > 0)
    dotY -= step;
  if (digitalRead(BTN_DOWN) && dotY < SCREEN_HEIGHT - 1)
    dotY += step;
  if (digitalRead(BTN_LEFT) && dotX > 0)
    dotX -= step;
  if (digitalRead(BTN_RIGHT) && dotX < SCREEN_WIDTH - 1)
    dotX += step;

  if (digitalRead(BTN_UP) && digitalRead(BTN_DOWN) && digitalRead(BTN_LEFT) && digitalRead(BTN_RIGHT))
    display.clearDisplay();
  display.fillCircle(dotX, dotY, dotSize, WHITE);
  display.display();
  delay(10);
}