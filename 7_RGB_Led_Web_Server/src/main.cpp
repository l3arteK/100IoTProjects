#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "MyESP32AP";
const char *password = "1234567890";

WebServer server(80);

const int redPin = 13;
const int greenPin = 12;
const int bluePin = 14;

String redString = "0";
String greenString = "0";
String blueString = "0";

const int freq = 5000;
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;
const int resolution = 8;

String page()
{
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">

<link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css">
<script src="https://cdnjs.cloudflare.com/ajax/libs/jscolor/2.0.4/jscolor.min.js"></script>

</head>
<body>

<div class="container">
  <div class="row">
    <h1>ESP Color Picker</h1>
  </div>

  <a class="btn btn-primary btn-lg" id="change_color" role="button">Change Color</a>

  <input class="jscolor {onFineChange:'update(this)'}" id="rgb">

</div>

<script>
function update(picker) {

  let r = Math.round(picker.rgb[0]);
  let g = Math.round(picker.rgb[1]);
  let b = Math.round(picker.rgb[2]);

  document.getElementById('rgb').value = r + ',' + g + ',' + b;

  document.getElementById("change_color").href =
    "/set?r=" + r + "&g=" + g + "&b=" + b;
}
</script>

</body>
</html>
)rawliteral";

  return html;
}

void handleRoot()
{
  server.send(200, "text/html", page());
}

void handleRGB()
{
  String r = server.arg("r");
  String g = server.arg("g");
  String b = server.arg("b");

  Serial.println("Received RGB values: " + r + ", " + g + ", " + b);
  redString = r;
  greenString = g;
  blueString = b;

  int redValue = r.toInt();
  int greenValue = g.toInt();
  int blueValue = b.toInt();

  ledcWrite(redChannel, redValue);
  ledcWrite(greenChannel, greenValue);
  ledcWrite(blueChannel, blueValue);

  handleRoot();
}

void setup()
{
  Serial.begin(115200);

  ledcSetup(redChannel, freq, resolution);
  ledcSetup(greenChannel, freq, resolution);
  ledcSetup(blueChannel, freq, resolution);

  ledcAttachPin(redPin, redChannel);
  ledcAttachPin(greenPin, greenChannel);
  ledcAttachPin(bluePin, blueChannel);

  WiFi.softAP(ssid, password);

  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/set", handleRGB);

  server.begin();
}

void loop()
{
  server.handleClient();
}
