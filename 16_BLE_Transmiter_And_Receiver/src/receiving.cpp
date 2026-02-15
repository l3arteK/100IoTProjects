
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define DEVICE_NAME "DHT_DATA"
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
struct SensorData
{
  int16_t temp;
  int16_t hum;
};

float temp = 0;
float hum = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
class MyAvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice oAdvertisedDevice)
  {
    if (oAdvertisedDevice.getName() == DEVICE_NAME)
    {
      std::string rawData = oAdvertisedDevice.getManufacturerData();
      if (rawData.length() >= sizeof(SensorData))
      {
        SensorData recivedData;
        memcpy(&recivedData, rawData.data(), sizeof(SensorData));

        temp = recivedData.temp / 100;
        hum = recivedData.hum / 100;
      }
    }
  }
};

void setup()
{
  Serial.begin(115200);

  BLEDevice::init("");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(2000);
  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  delay(100);
}

void loop()
{

  BLEDevice::getScan()->start(1, false);
  BLEDevice::getScan()->clearResults();
  display.clearDisplay();
  display.setCursor(0, 30);
  display.print("T: ");
  display.print(temp);
  display.print("C");
  display.print(" H: ");
  display.print(hum);
  display.print("%");
  display.display();
}