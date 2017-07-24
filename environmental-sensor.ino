#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;    

const char* ssid = "xxxxxxxx";
const char* password = "xxxxxxxx";

ESP8266WebServer server(80);

String mimeType;
String response;

String state;

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  response = "Environmental Sensor based on esp8266 & Bosch BME280.";
  mimeType = "text/plain";
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
  response += "Température = ";
  response += bme.readTemperature();
  response += " °C";
  response += "\n";
  response += "Pression = ";
  response += (bme.readPressure() / 100.0F);
  response += " hPa";
  response += "\n";
  response += "Humidité = ";
  response += bme.readHumidity();
  response += " %";
  response += "\n";
  response += "Altitude (approx.) = ";
  response += bme.readAltitude(SEALEVELPRESSURE_HPA);
  response += " mètres";
  response += "\n";
  mimeType = "text/plain";
  server.send(200, mimeType, response);
  Serial.print(response);
  digitalWrite(led, 0);
}

void handleTemperature() {
  digitalWrite(led, 1);
  response = "";
  response += "Température = ";
  response += bme.readTemperature();
  response += " °C";
  mimeType = "text/plain";
  server.send(200, mimeType, response);
  Serial.print(response);
  digitalWrite(led, 0);
}

void handlePressure() {
  digitalWrite(led, 1);
  response = "";
  response += "Pression = ";
  response += (bme.readPressure() / 100.0F);
  response += " hPa";
  mimeType = "text/plain";
  server.send(200, mimeType, response);
  Serial.print(response);
  digitalWrite(led, 0);
}

void handleHumidity() {
  digitalWrite(led, 1);
  response = "";
  response += "Humidité = ";
  response += bme.readHumidity();
  response += " %";
  mimeType = "text/plain";
  server.send(200, mimeType, response);
  Serial.print(response);
  digitalWrite(led, 0);
}

void handleAltitude() {
  digitalWrite(led, 1);
  response = "";
  response += "Altitude (approx.) = ";
  response += bme.readAltitude(SEALEVELPRESSURE_HPA);
  response += " mètres";
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
  message += "Connecté à ";
  message += ssid;
  message += "\n";
  message += "Adresse IP : ";
  message += WiFi.localIP().toString();
  message += "\n";
  state += message;
  Serial.println(message);

  if (MDNS.begin("esp8266")) {
    message = "Répondeur MDNS démarré\n";
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
  message = "Serveur HTTP démarré\n";
  state += message;
  Serial.print(message);

  //init BMP280
  if (!bme.begin((uint8_t)(0x76))) {
    message = "BME280 introuvable ! Vérifiez le branchement.\n";
  }
  else {
    message = "BME280 opérationnel !\n";
  }
  state += message;
  Serial.print(message);

}

void loop(void){
  server.handleClient();
}
