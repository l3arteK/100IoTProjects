#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "MyESP32AP";
const char *password = "1234567890";

WebServer server(80);

const int output26 = 26;
const int output27 = 27;

String output26State = "off";
String output27State = "off";

String page()
{
  String html = R"rawliteral(
<!DOCTYPE html><html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
html { font-family: Helvetica; text-align: center; }
.button {
  background-color:#4CAF50;
  border:none;
  color:white;
  padding:16px 40px;
  font-size:30px;
  margin:5px;
  cursor:pointer;
}
.button2 { background-color:#555; }
</style>
</head>
<body>
<h1>ESP32 Web Server</h1>
)rawliteral";

  html += "<p>GPIO 26 - State " + output26State + "</p>";
  if (output26State == "off")
    html += "<a href=\"/26/on\"><button class=\"button\">ON</button></a>";
  else
    html += "<a href=\"/26/off\"><button class=\"button button2\">OFF</button></a>";

  // GPIO 27
  html += "<p>GPIO 27 - State " + output27State + "</p>";
  if (output27State == "off")
    html += "<a href=\"/27/on\"><button class=\"button\">ON</button></a>";
  else
    html += "<a href=\"/27/off\"><button class=\"button button2\">OFF</button></a>";

  html += "</body></html>";

  return html;
}

void handleRoot()
{
  server.send(200, "text/html", page());
}

void handle26on()
{
  digitalWrite(output26, HIGH);
  output26State = "on";
  handleRoot();
}

void handle26off()
{
  digitalWrite(output26, LOW);
  output26State = "off";
  handleRoot();
}

void handle27on()
{
  digitalWrite(output27, HIGH);
  output27State = "on";
  handleRoot();
}

void handle27off()
{
  digitalWrite(output27, LOW);
  output27State = "off";
  handleRoot();
}

void setup()
{
  Serial.begin(115200);

  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);

  WiFi.softAP(ssid, password);

  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/26/on", handle26on);
  server.on("/26/off", handle26off);
  server.on("/27/on", handle27on);
  server.on("/27/off", handle27off);

  server.begin();
}

void loop()
{
  server.handleClient();
}
