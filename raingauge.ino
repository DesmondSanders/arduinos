#include <Arduino.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <AsyncTCP.h>
#include <WiFiClient.h>
#include <HTTPClient.h>

// Define constants
const float Pf = 9503.32; // Funnel cross-sectional area in m²
const float k = 1000000 / Pf; // Conversion factor
const float v = 6.0; //Volume Scoop
const float volumePerTick = (k*v)/1000; // Volume per tick in ml
const char* ssid = "sreddnas2g";
const char* password = "0004EDDABEAE";

// Define variables
volatile int rainCounter = 0;
unsigned long previousMillis = 0;
unsigned long lastTickTime = 0; // Time of the last tick
const long interval = 3600000; // 1 hour in milliseconds

void setup() {
   int hallSensorValue = hallRead();
  Serial.begin(115200);
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
   //Register with Server
String deviceid = WiFi.macAddress().c_str();
HTTPClient http;                     //Declare object of class HTTPClient
  String jsonData = "{\"deviceid\":";
  jsonData += "\"";
  jsonData += deviceid;
  jsonData += "\"";
  jsonData += ",\"deviceip\":";
  jsonData += "\"";
  jsonData += WiFi.localIP().toString();
  jsonData += "\"";
  jsonData += "}"; 
  http.begin("https://192.168.0.2/deviceid/validateandwrite.php");                                          //Specify request destination
  http.addHeader("Content-Type", "application/json" , "Content-Length", jsonData.length());    //Specify content-type header
  int httpCode = http.POST(jsonData);   //Send the request
  String payload = http.getString();    //Get the response payload
  http.end();                           //Close connection 
}

void updateweb(float rainreadinmm){
HTTPClient http;                     //Declare object of class HTTPClient
  String jsonData = "{\"reading\":";
  jsonData += "\"";
  jsonData += rainreadinmm;
  jsonData += "\"";
  jsonData += ",\"ipaddr\":";
  jsonData += "\"";
  jsonData += WiFi.localIP().toString();
  jsonData += "\"";
  jsonData += "}"; 
  Serial.println(jsonData);
  http.begin("https://192.168.0.2/rain/validateandwrite.php");                                          //Specify request destination
  http.addHeader("Content-Type", "application/json" , "Content-Length", jsonData.length());    //Specify content-type header
  int httpCode = http.POST(jsonData);   //Send the request
  String payload = http.getString();    //Get the response payload
  http.end();                           //Close connection
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    Serial.print("Rainfall in the last hour: ");
    Serial.print(rainCounter);
    Serial.println(" counts");
    rainCounter = 0;
    previousMillis = currentMillis;
  }
    //Serial.println(hallRead());
  if (hallRead() <= 0) {
    Serial.println(hallRead());
    rainCounter++;
    unsigned long currentTime = millis();
    float timeElapsed = (currentTime - lastTickTime) / 1000.0; // Convert to seconds
    lastTickTime = currentTime;
    // Calculate rainfall rate
    float ticksPerHour = 3600.0 / timeElapsed;
    float totalVolumePerHour = volumePerTick * rainCounter * ticksPerHour; // Total volume in ml/hour
    float rainfallRateLPerM2 = totalVolumePerHour / 1000.0 / Pf; // Convert to l/m²/h
    updateweb(rainfallRateLPerM2);
    delay(100); // Debounce delay
  }
}
