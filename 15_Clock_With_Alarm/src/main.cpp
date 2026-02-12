#include <WiFi.h>
#include "time.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <sys/time.h>

#define BTN_SET 26
#define BTN_MODE 25
#define BUZZER 33

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

unsigned long previousMillis = 0;
unsigned long lastTick = 0, lastBlink = 0;
int hours = 0, minutes = 0, seconds = 0;
int alarmH = 0, alarmM = 0;
bool alarmActive = false;
bool alarmIsOn = true;
bool blockAlarm = false;
int mode = 0;

const unsigned char epd_bitmap_bell[] PROGMEM = {
    0x01, 0x80, 0x03, 0xc0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x0f, 0xf0, 0x0f, 0xf0, 0x0f, 0xf0,
    0x1f, 0xf8, 0x1f, 0xf8, 0x3f, 0xfc, 0x7f, 0xfe, 0x7f, 0xfe, 0x00, 0x00, 0x03, 0xc0, 0x01, 0x80};

void printDigits(int val, bool blink)
{
  if (blink && (millis() / 500) % 2 == 0)
    display.setTextColor(BLACK);
  else
    display.setTextColor(WHITE);

  if (val < 10)
    display.print("0");
  display.print(val);
  display.setTextColor(WHITE);
}
bool displayTime()
{
  return mode == 0 || mode == 1 || mode == 2;
}
bool displayAlarm()
{
  return !displayTime();
}
void drawing()
{
  int hoursToDisplay;
  int minutesToDisplay;
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);

  const char *labels[] = {"TIME", "SET TIME", "SET TIME", "SET ALARM", "SET ALARM"};
  display.print(labels[mode]);

  display.setCursor(20, 25);
  display.setTextSize(2);
  if (displayTime())
  {
    hoursToDisplay = hours;
    minutesToDisplay = minutes;
  }
  else
  {
    hoursToDisplay = alarmH;
    minutesToDisplay = alarmM;
  }
  printDigits(hoursToDisplay, (mode == 1 || mode == 3) || alarmActive);
  display.print(":");
  printDigits(minutesToDisplay, (mode == 2 || mode == 4) || alarmActive);

  display.setTextSize(1);
  if (displayTime())
  {
    if (seconds < 10)
      display.print("0");
    display.print(seconds);
  }

  if (alarmIsOn)
  {
    display.drawBitmap(110, 0, epd_bitmap_bell, 16, 16, WHITE);
  }

  display.display();
}

void setup()
{
  Serial.begin(115200);

  pinMode(BTN_SET, INPUT);
  pinMode(BTN_MODE, INPUT);
  pinMode(BUZZER, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("SSD1306 allocation failed");
    for (;;)
      ;
  }

  display.setTextColor(WHITE);
  display.clearDisplay();
}
unsigned long lastClick = 0;
void loop()
{
  unsigned long currentMillis = millis();

  if (digitalRead(BTN_MODE) && currentMillis - lastClick >= 500)
  {
    lastClick = currentMillis;
    mode = ++mode % 5;
  }
  if (digitalRead(BTN_SET) && currentMillis - lastClick >= 200)
  {
    lastClick = currentMillis;
    if (mode == 0)
      if (alarmActive)
        alarmActive = false;
      else
        alarmIsOn = !alarmIsOn;
    if (mode == 1)
      hours = ++hours % 24;
    if (mode == 2)
      minutes = ++minutes % 60;
    if (mode == 3)
      alarmH = ++alarmH % 24;
    if (mode == 4)
      alarmM = ++alarmM % 60;
  }
  if (currentMillis - lastTick >= 1000)
  {
    lastTick = currentMillis;
    seconds++;
    if (seconds >= 60)
    {
      seconds = 0;
      minutes++;
      blockAlarm = false;
    }
    if (minutes >= 60)
    {
      minutes = 0;
      hours++;
    }
    if (hours >= 24)
    {
      hours = 0;
    }

    if (alarmH == hours && alarmM == minutes && alarmIsOn && !blockAlarm)
    {
      alarmActive = true;
      blockAlarm = true;
    }
  }
  if (alarmActive)
  {
    if ((millis() / 500) % 2 == 0)
      digitalWrite(BUZZER, LOW);
    else
      digitalWrite(BUZZER, HIGH);
  }
  else
  {
    digitalWrite(BUZZER, HIGH);
  }

  drawing();
}