#include <Arduino.h>
#define BUTTON 4
#define LED 5

int buttonState = 0;
void setup()
{
  Serial.begin(9600);
  pinMode(BUTTON, INPUT);
  pinMode(LED, OUTPUT);
}

void loop()
{
  buttonState = digitalRead(BUTTON);

  if (buttonState == HIGH)
  {
    Serial.println("Button Pressed");
    digitalWrite(LED, HIGH);
  }
  else
  {
    Serial.println("Button Released");
    digitalWrite(LED, LOW);
  }
}
