#include <Arduino.h>
#define LED 2

int myFunction(int, int);

void setup()
{
  pinMode(LED, OUTPUT);
}

void loop()
{
  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite(LED, LOW);
  delay(1000);
}

int myFunction(int x, int y)
{
  return x + y;
}