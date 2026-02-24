
#include <Arduino.h>

static SemaphoreHandle_t mutex;

void task1(void *pvParameters)
{
  char ch = '1';
  while (true)
  {
    if (xSemaphoreTake(mutex, 0) == pdTRUE)
    {
      for (int i = 1; i < 10; i++)
      {
        Serial.print(ch);
      }

      Serial.println("");
      xSemaphoreGive(mutex);
    }
  }
}

void task2(void *pvParameters)
{
  char ch = '2';
  while (true)
  {
    if (xSemaphoreTake(mutex, 0) == pdTRUE)
    {
      for (int i = 1; i < 10; i++)
      {
        Serial.print(ch);
      }

      Serial.println("");
      xSemaphoreGive(mutex);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  mutex = xSemaphoreCreateMutex();
  xTaskCreate(task1, "Task1", 8192, NULL, 1, NULL);
  xTaskCreate(task2, "Task2", 8192, NULL, 1, NULL);
}

void loop()
{
}