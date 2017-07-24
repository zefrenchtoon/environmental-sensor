#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;    

const boolean debug = false;
const uint8_t bme_i2c_address = 0x76;

const char* ssid = "xxxxxxxx";
const char* password = "xxxxxxxx";

ESP8266WebServer server(80);

String mimeType;
String response;

String state;

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  response = "";
  response += "<html>";
  response +=   "<head>";
  response +=     "<title>Environmental Sensor (ESP8266, BME280)</title>";
  response +=     "<style>* {font-family: 'Droid Sans', Arial, Verdana, sans-serif;}</style>";
  response +=   "</head>";
  response +=   "<body>";
  response +=     "<p>Environmental Sensor based on esp8266 & Bosch BME280.<br>Paths available:</p>";
  response +=     "<ul>";
  response +=       "<li><a href=\"/state\">Sensor State</a></li>";
  response +=       "<li><a href=\"/summary\">Sensor Summary</a></li>";
  response +=       "<li><a href=\"/temperature\">Temperature Value</a></li>";
  response +=       "<li><a href=\"/pressure\">Pressure Value</a></li>";
  response +=       "<li><a href=\"/humidity\">Humidity Value</a></li>";
  response +=       "<li><a href=\"/altitude\">Altitude Value (approx.)</a></li>";
  response +=     "</ul>";
  response +=   "</body>";
  response += "</html>";
  mimeType = "text/html";
  server.send(200, mimeType, response);
  digitalWrite(led, 0);
}

void handleState() {
  digitalWrite(led, 1);
  response = state;
  mimeType = "text/plain";
  server.send(200, mimeType, response);
  digitalWrite(led, 0);
}

void handleSummary() {
  digitalWrite(led, 1);
  response = "";
  response += "<html>";
  response +=   "<head>";
  response +=     "<title>Environmental Sensor (ESP8266, BME280)</title>";
  response +=     "<style>* {font-family: 'Droid Sans', Arial, Verdana, sans-serif;}</style>";
  response +=     "<meta http-equiv=\"refresh\" content=\"1\" >";
  response +=   "</head>";
  response +=   "<body>";
  response +=     "<p>Environmental Sensor based on esp8266 & Bosch BME280.<br>Paths available:</p>";
  response +=     "<table>";
  response +=       "<thead><td>Sensor</td><td>Value</td><td>Unit</td></tr>";
  response +=       "<tr>";
  response +=         "<td>Temperature</td>";
  response +=         "<td>";
  response +=           bme.readTemperature();
  response +=         "</td>";
  response +=         "<td>°C</td>";
  response +=       "</tr>";
  response +=       "<tr>";
  response +=         "<td>Pressure</td>";
  response +=         "<td>";
  response +=           (bme.readPressure() / 100.0F);
  response +=         "</td>";
  response +=         "<td>hPa</td>";
  response +=       "</tr>";
  response +=       "<tr>";
  response +=         "<td>Humidity</td>";
  response +=         "<td>";
  response +=           bme.readHumidity();
  response +=         "</td>";
  response +=         "<td>%</td>";
  response +=       "</tr>";
  response +=       "<tr>";
  response +=         "<td>Altitude (beta / approx.)</td>";
  response +=         "<td>";
  response +=           bme.readAltitude(SEALEVELPRESSURE_HPA);
  response +=         "</td>";
  response +=         "<td>m</td>";
  response +=       "</tr>";
  response +=     "</table>";
  response +=     "<a href=\"/\">Back</a>";
  response +=   "</body>";
  response += "</html>";
  mimeType = "text/html";
  server.send(200, mimeType, response);
  if (debug) {
    Serial.println(response);
  }
  digitalWrite(led, 0);
}

void handleTemperature() {
  digitalWrite(led, 1);
  response = "";
  response += bme.readTemperature();
  mimeType = "text/plain";
  server.send(200, mimeType, response);
  Serial.print(response);
  digitalWrite(led, 0);
}

void handlePressure() {
  digitalWrite(led, 1);
  response = "";
  response += (bme.readPressure() / 100.0F);
  mimeType = "text/plain";
  server.send(200, mimeType, response);
  Serial.print(response);
  digitalWrite(led, 0);
}

void handleHumidity() {
  digitalWrite(led, 1);
  response = "";
  response += bme.readHumidity();
  mimeType = "text/plain";
  server.send(200, mimeType, response);
  Serial.print(response);
  digitalWrite(led, 0);
}

void handleAltitude() {
  digitalWrite(led, 1);
  response = "";
  response += bme.readAltitude(SEALEVELPRESSURE_HPA);
  mimeType = "text/plain";
  server.send(200, mimeType, response);
  Serial.print(response);
  digitalWrite(led, 0);
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  mimeType = "text/plain";
  server.send(404, mimeType, message);
  digitalWrite(led, 0);
}

void setup(void){
  String message = "";
  
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  message += "Connected to ";
  message += ssid;
  message += "\n";
  message += "IP address : ";
  message += WiFi.localIP().toString();
  message += "\n";
  state += message;
  Serial.println(message);

  if (MDNS.begin("esp8266")) {
    message = "mDNS responder started.\n";
    state += message;
    Serial.print(message);
  }

  server.on("/", handleRoot);

  server.on("/state", handleState);

  server.on("/summary", handleSummary);

  server.on("/temperature", handleTemperature);

  server.on("/pressure", handlePressure);

  server.on("/humidity", handleHumidity);

  server.on("/altitude", handleAltitude);

  server.onNotFound(handleNotFound);

  server.begin();
  message = "HTTP server started.\n";
  state += message;
  Serial.print(message);

  //init BMP280
  if (!bme.begin(bme_i2c_address)) {
    message = "BME280 not found ! Check wiring & I2C address.\n";
  }
  else {
    message = "BME280 found.\n";
  }
  state += message;
  Serial.print(message);

}

void loop(void){
  server.handleClient();
}
