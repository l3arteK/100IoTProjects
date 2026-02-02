#include <Arduino.h>
#define MOTION_SENSOR 27
#define BUZZER 26

const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;
static bool motionDetected = false;
void setup()
{
  Serial.begin(9600);
  pinMode(MOTION_SENSOR, INPUT);
  pinMode(BUZZER, OUTPUT);
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(BUZZER, ledChannel);
}

void playBuzzer()
{
  for (int i = 0; i < 3; i++)
  {
    ledcWrite(ledChannel, 128);
    delay(100);
    ledcWrite(ledChannel, 0);
    delay(100);
  }
}
void loop()
{
  int sensorValue = digitalRead(MOTION_SENSOR);
  if (sensorValue == HIGH && !motionDetected)
  {
    Serial.println("Motion Detected!");
    motionDetected = true;
    playBuzzer();
  }
  else if (sensorValue == LOW && motionDetected)
  {
    Serial.println("No Motion.");
    motionDetected = false;
    ledcWrite(ledChannel, 0);
  }
  delay(500);
}
