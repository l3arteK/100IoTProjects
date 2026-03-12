
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

constexpr uint8_t BTN_PIN = 5;
constexpr uint8_t POT_PIN = 4;
constexpr uint8_t RED_PIN = 13;
constexpr uint8_t GREEN_PIN = 12;
constexpr uint8_t BLUE_PIN = 14;
constexpr uint16_t FREQ = 5000;
constexpr uint8_t RESOLUTION = 8;
constexpr uint8_t RED_CHANNEL = 0;
constexpr uint8_t BLUE_CHANNEL = 1;
constexpr uint8_t GREEN_CHANNEL = 2;
constexpr uint8_t SCREEN_HEIGHT = 64;
constexpr uint8_t SCREEN_WIDTH = 128;

uint8_t mode = 0;
uint8_t red_value = 0;
uint8_t blue_value = 0;
uint8_t green_value = 0;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void render_task(void *pvParameters)
{
  while (true)
  {
    display.clearDisplay();
    display.setCursor(0, 0);

    display.print("R: ");
    display.print(red_value);
    if (mode == 0)
      display.print(" <--");
    display.println("");
    display.print("B: ");
    display.print(blue_value);
    if (mode == 1)
      display.print(" <--");
    display.println("");
    display.print("G: ");
    display.print(green_value);
    if (mode == 2)
      display.print("<--");
    display.println("");
    display.display();
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

void input_task(void *pvParameters)
{
  boolean last = false;
  boolean current = false;
  int value = 0;
  while (true)
  {
    current = digitalRead(BTN_PIN);
    if (current && !last)
    {
      mode = (mode + 1) % 3;
    }

    value = analogRead(POT_PIN) / 16;
    if (mode == 0)
    {
      red_value = value;
    }
    else if (mode == 1)
    {
      blue_value = value;
    }
    else
    {
      green_value = value;
    }
    vTaskDelay(pdMS_TO_TICKS(50));
    last = current;
  }
}
void output_task(void *pvParameters)
{
  while (true)
  {
    ledcWrite(RED_CHANNEL, red_value);
    ledcWrite(BLUE_CHANNEL, blue_value);
    ledcWrite(GREEN_CHANNEL, green_value);
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(BTN_PIN, INPUT);
  ledcSetup(RED_CHANNEL, FREQ, RESOLUTION);
  ledcSetup(BLUE_CHANNEL, FREQ, RESOLUTION);
  ledcSetup(GREEN_CHANNEL, FREQ, RESOLUTION);

  ledcAttachPin(RED_PIN, RED_CHANNEL);
  ledcAttachPin(BLUE_PIN, BLUE_CHANNEL);
  ledcAttachPin(GREEN_PIN, GREEN_CHANNEL);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(2000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  xTaskCreate(input_task, "Input", 2048, NULL, 1, NULL);
  xTaskCreate(render_task, "Render", 2048, NULL, 1, NULL);
  xTaskCreate(output_task, "Output", 2048, NULL, 1, NULL);
}

void loop()
{
}