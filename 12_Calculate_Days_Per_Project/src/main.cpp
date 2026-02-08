#include <WiFi.h>
#include "time.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

#define BTN_OK 25
#define BTN_PLUS 26
#define BTN_MINUS 27
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
int mode = 0;
int projectDone = 0;
float daysPerProject = 0;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

float calculateDaysPerProject()
{
  struct tm current_date;
  if (!getLocalTime(&current_date))
    return 0;
  time_t current_sec = mktime(&current_date);

  struct tm deadline_date = {0};
  deadline_date.tm_year = 2027 - 1900;
  deadline_date.tm_mon = 0;
  deadline_date.tm_mday = 31;
  time_t deadline_sec = mktime(&deadline_date);
  time_t difference = deadline_sec - current_sec;
  float days = (float)difference / 86400.0;
  float result = (float)days / float(100 - projectDone);
  return result;
}

void setup()
{
  Serial.begin(115200);
  WiFi.begin("NJU_Swiatlowod_5B9A", "EVLL2RTKK7W7");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  pinMode(BTN_OK, INPUT);
  pinMode(BTN_PLUS, INPUT);
  pinMode(BTN_MINUS, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("SSD1306 allocation failed");
    for (;;)
      ;
  }

  display.setTextColor(WHITE);
}

void loop()
{
  if (digitalRead(BTN_OK) == HIGH)
  {
    delay(200);
    mode = (mode == 0) ? 1 : 0;
    if (mode == 1)
    {
      daysPerProject = calculateDaysPerProject();
    }
    display.clearDisplay();
  }

  if (mode == 0)
  {
    if (digitalRead(BTN_PLUS) == HIGH)
    {
      projectDone++;
      delay(150);
    }

    if (digitalRead(BTN_MINUS) == HIGH)
    {
      projectDone--;
      delay(150);
    }

    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("How many project are done?: ");
    display.setTextSize(3);
    display.setCursor(40, 30);
    display.print(projectDone);
    display.display();
  }
  else
  {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("Days per project: ");
    display.setTextSize(3);
    display.setCursor(40, 30);
    display.print(daysPerProject, 2);
    display.display();
  }
}