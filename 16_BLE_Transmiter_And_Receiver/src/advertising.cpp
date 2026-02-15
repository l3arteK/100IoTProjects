
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN 22
#define LEDPIN 5
#define DHTTYPE DHT22
#define DEVICE_NAME "DHT_DATA"

DHT dht(DHTPIN, DHTTYPE);

BLEAdvertising *pAdvertising;

struct SensorData
{
  int16_t temp;
  int16_t hum;
};

void updateAdvertisement()
{
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  SensorData data;
  if (isnan(t) || isnan(h))
  {
    data.temp = -999;
    data.hum = -999;
  }
  else
  {
    data.temp = (int16_t)(t * 100);
    data.hum = (int16_t)(h * 100);
  }

  std::string payload((char *)&data, sizeof(data));

  BLEAdvertisementData oNewData;
  oNewData.setFlags(0x04);
  oNewData.setManufacturerData(payload);
  oNewData.setName(DEVICE_NAME);
  pAdvertising->setAdvertisementData(oNewData);
  pAdvertising->start();
}

void setup()
{
  Serial.begin(115200);
  dht.begin();

  BLEDevice::init(DEVICE_NAME);
  pAdvertising = BLEDevice::getAdvertising();

  pinMode(LEDPIN, OUTPUT);
}

void loop()
{

  updateAdvertisement();
  digitalWrite(LEDPIN, HIGH);
  delay(1000);
  digitalWrite(LEDPIN, LOW);
  delay(4000);
}