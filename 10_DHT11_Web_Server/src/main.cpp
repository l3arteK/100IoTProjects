#include <Arduino.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ESPAsyncWebServer.h>

const char *ssid = "MyESP32AP";
const char *password = "1234567890";

#define DHTPIN 4
#define DHTTYPE DHT11

AsyncWebServer server(80);
DHT dht(DHTPIN, DHTTYPE);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP32 DHT11 Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 2.5rem;}
    p {font-size: 2.0rem;}
    .units {font-size: 1.2rem;}
    .dht-labels {font-size: 1.5rem; vertical-align:middle; padding-bottom: 15px;}
  </style>
</head>
<body>
  <h2>ESP32 DHT Server</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add8;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">%</sup>
  </p>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 5000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 5000 ) ;
</script>
</body>
</html>)rawliteral";

String processor(const String &var)
{
  if (var == "TEMPERATURE")
  {
    return String(dht.readTemperature());
  }
  else if (var == "HUMIDITY")
  {
    return String(dht.readHumidity());
  }
  return String();
}

String readDHTTemperature()
{
  float t = dht.readTemperature();
  if (isnan(t))
  {
    Serial.println("Failed to read temperature from DHT sensor!");
    return "--";
  }
  else
  {
    Serial.println("temperature: " + String(t));
    return String(t);
  }
}

String readDHTHumidity()
{
  float t = dht.readHumidity();
  if (isnan(t))
  {
    Serial.println("Failed to read humidity rom DHT sensor!");
    return "--";
  }
  else
  {
    Serial.println("humidity: " + String(t));
    return String(t);
  }
}

void setup()
{
  Serial.begin(115200);

  WiFi.softAP(ssid, password);

  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/html", index_html, processor); });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", String(dht.readTemperature())); });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", String(dht.readHumidity())); });

  server.begin();
}

void loop()
{
}
