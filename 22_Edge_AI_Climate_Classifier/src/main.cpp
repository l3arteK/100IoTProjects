#include <Arduino.h>
#include "DHT.h"
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(115200);
  dht.begin();
  Serial.println("Edge Impulse - temperature + humidity");
}

void loop()
{
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity))
  {
    Serial.println("DHT Error!");
    delay(2000);
    return;
  }

  Serial.printf("Temp: %.1f C, Hum: %.1f %%\n", temperature, humidity);

  float input_buf[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = {temperature, humidity};

  signal_t signal;
  int err = numpy::signal_from_buffer(input_buf, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
  if (err != 0)
  {
    Serial.printf("Error signal_from_buffer: %d\n", err);
    delay(2000);
    return;
  }

  ei_impulse_result_t result = {0};
  EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);
  if (res != EI_IMPULSE_OK)
  {
    Serial.printf("Error Classifier: %d\n", res);
    delay(2000);
    return;
  }

  float best_val = -1.0f;
  uint32_t best_idx = 0;
  for (uint32_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++)
  {
    Serial.printf("  %s: %.4f\n", result.classification[i].label,
                  result.classification[i].value);
    if (result.classification[i].value > best_val)
    {
      best_val = result.classification[i].value;
      best_idx = i;
    }
  }

  Serial.printf(">> Result: %s (%.2f%%)\n\n",
                result.classification[best_idx].label,
                best_val * 100.0f);

  delay(3000);
}