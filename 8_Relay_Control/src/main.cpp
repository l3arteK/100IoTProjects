#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "MyESP32AP";
const char *password = "1234567890";

WebServer server(80);

const int relayPin = 26;
bool relayState = LOW;

String page()
{
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">

<style>body{font-family:Arial; text-align:center;} .button{display:block; width:200px; margin:20px auto; padding:20px; font-size:24px; color:white; text-decoration:none; border-radius:10px;}
.on{background-color:#4CAF50;} .off{background-color:#f44336;}</style></head>
<body>

<h1>ESP32 Relay Control</h1>

)rawliteral";

  if (relayState)
  {
    html += "<p>Status: ON</p><a class='button off' href='/toggle'>TURN OFF</a>";
  }
  else
  {
    html += "<p>Status: OFF</p><a class='button on' href='/toggle'>TURN ON</a>";
  }
  html += "</body></html>";
  return html;
}

void handleRoot()
{
  server.send(200, "text/html", page());
}

void handleToggle()
{
  relayState = !relayState;
  digitalWrite(relayPin, relayState);
  server.send(200, "text/html", page());
}

void setup()
{
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, relayState);

  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);

  server.begin();
}

void loop()
{
  server.handleClient();
}
