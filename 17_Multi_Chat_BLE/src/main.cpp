
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define MAX_LINES 8
#define MAX_LINE_LENGTH 22
#define RESET_BTN 4

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
uint8_t connectedCount = 0;

uint8_t head = 0;
uint8_t count = 0;
char chatHistory[MAX_LINES][MAX_LINE_LENGTH];

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void addMessage(const char *newMsg)
{
  int totalLen = strlen(newMsg);
  int offset = 0;

  if (totalLen == 0)
    return;

  while (offset < totalLen)
  {
    strncpy(chatHistory[head], newMsg + offset, MAX_LINE_LENGTH - 1);
    chatHistory[head][MAX_LINE_LENGTH - 1] = '\0';

    head = (head + 1) % MAX_LINES;
    if (count < MAX_LINES)
      count++;

    offset += (MAX_LINE_LENGTH - 1);
  }
}

void updateDisplay()
{
  display.clearDisplay();
  display.setCursor(0, 0);
  for (uint8_t i = 0; i < count; i++)
  {
    uint8_t index;
    if (count < MAX_LINES)
    {
      index = i;
    }
    else
    {
      index = (head + i) % MAX_LINES;
    }
    display.println(chatHistory[index]);
  }
  display.display();
}
void clearChat()
{
  memset(chatHistory, 0, sizeof(chatHistory));
  head = 0;
  count = 0;
  updateDisplay();
}

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    connectedCount++;
    Serial.print("New user! Current connected count: ");
    Serial.println(connectedCount);

    pServer->getAdvertising()->start();
  }

  void onDisconnect(BLEServer *pServer)
  {
    if (connectedCount > 0)
      connectedCount--;

    deviceConnected = false;
    Serial.print("Someone left the chat. Current connected count: ");
    Serial.println(connectedCount);
    pServer->getAdvertising()->start();
  }
};

class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pRxCharacteristic, esp_ble_gatts_cb_param_t *param)
  {
    std::string rxValue = pRxCharacteristic->getValue();
    if (rxValue.length() > 0)
    {
      uint8_t *bda = param->write.bda;
      char addrStr[18];
      sprintf(addrStr, "%02X:%02X:%02X:%02X:%02X:%02X",
              bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
      std::string shortId = std::string(addrStr).substr(12);

      std::string formattedMsg = "[" + shortId + "]: " + rxValue;

      pTxCharacteristic->setValue(formattedMsg.c_str());
      pTxCharacteristic->notify();
      Serial.println(formattedMsg.c_str());
      addMessage(formattedMsg.c_str());
      updateDisplay();
    }
  }
};

void setup()
{
  Serial.begin(115200);
  BLEDevice::init("ESP32_Multi_Chat");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);

  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
  pRxCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pServer->getAdvertising()->start();
  pinMode(RESET_BTN, INPUT);
}

void loop()
{
  if (digitalRead(RESET_BTN) == HIGH)
  {
    clearChat();
  }
}