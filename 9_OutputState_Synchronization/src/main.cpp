#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#else
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

const char *ssid = "MyESP32AP";
const char *password = "1234567890";

AsyncWebServer server(80);

const int ledPin = 25;
const int buttonPin = 26;
bool ledState = LOW;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Output State Sync</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 2.0rem;}
    .button {background-color: #04AA6D; border: none; color: white; padding: 16px 40px;
             text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}
    .button2 {background-color: #555555;}
  </style>
</head>
<body>
  <h2>ESP Web Server</h2>
  <p>State: <strong id="state">%STATE%</strong></p>
  <p><button class="button" onclick="toggle()">Switch</button></p>
<script>
function toggle() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("state").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/toggle", true);
  xhttp.send();
}

setInterval(function() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("state").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/state", true);
  xhttp.send();
}, 2000);
</script>
</body>
</html>)rawliteral";

String processor(const String &var)
{
  if (var == "STATE")
  {
    return ledState ? "ON" : "OFF";
  }
  return String();
}

void setup()
{
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  pinMode(buttonPin, INPUT);

  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/html", index_html, processor); });

  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    request->send(200, "text/plain", ledState ? "ON" : "OFF"); });

  server.on("/state", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", ledState ? "ON" : "OFF"); });

  server.begin();
}

void loop()
{
  static bool lastButtonState = LOW;
  bool currentButtonState = digitalRead(buttonPin);

  if (currentButtonState == HIGH && lastButtonState == LOW)
  {
    delay(20);
    if (digitalRead(buttonPin) == HIGH)
    {
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
    }
  }
  lastButtonState = currentButtonState;
}
