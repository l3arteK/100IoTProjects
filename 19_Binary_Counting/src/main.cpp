
#include <Arduino.h>

static SemaphoreHandle_t count;

#define BTN 4
#define LED_0 21
#define LED_1 19
#define LED_2 18

void led_task(void *pvParameters)
{

  while (true)
  {
    while (true)
    {
      uint8_t value = uxSemaphoreGetCount(count);

      digitalWrite(LED_0, value & 0b001);
      digitalWrite(LED_1, value & 0b010);
      digitalWrite(LED_2, value & 0b100);

      vTaskDelay(pdMS_TO_TICKS(20));
    }
  }
}

void decay_task(void *pvParameters)
{
  while (true)
  {

    if (uxSemaphoreGetCount(count) > 0)
    {
      vTaskDelay(pdMS_TO_TICKS(1000));
      xSemaphoreTake(count, 0);
    }
  }
}

void button_task(void *pvParameters)
{

  while (true)
  {
    if (digitalRead(BTN) == HIGH)
    {
      xSemaphoreGive(count);
      vTaskDelay(pdMS_TO_TICKS(250));
    }
    else
    {
      vTaskDelay(pdMS_TO_TICKS(20));
    }
  }
}

void setup()
{
  Serial.begin(115200);
  count = xSemaphoreCreateCounting(7, 0);
  pinMode(BTN, INPUT);
  pinMode(LED_0, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  xTaskCreate(led_task, "LED", 2048, NULL, 1, NULL);
  xTaskCreate(button_task, "Button", 2048, NULL, 1, NULL);
  xTaskCreate(decay_task, "Decay", 2048, NULL, 1, NULL);
}

void loop()
{
}