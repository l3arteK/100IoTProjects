
#include "Arduino.h"

#define DETECT_PIN 4
#define BUZZER_PIN 5
void setup()
{
  Serial.begin(115200);

  pinMode(DETECT_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop()
{
  Serial.println(digitalRead(DETECT_PIN));
  if (digitalRead(DETECT_PIN))
    digitalWrite(BUZZER_PIN, HIGH);
  else
    digitalWrite(BUZZER_PIN, LOW);
}