#include <Arduino.h>
#define POT 4

void setup()
{
  Serial.begin(9600);
  delay(1000);
}

void loop()
{
  int potValue = analogRead(POT);
  Serial.println(potValue);
  delay(500);
}
