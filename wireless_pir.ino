#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <AsyncTCP.h>
#include <WiFiClient.h>
#include <HTTPClient.h>

const char* ssid = "sreddnas2g";
const char* password = "0004EDDABEAE";

void setup(){
  pinMode(5, INPUT);
  // Serial port for debugging purposes
  Serial.begin(115200);
   bool status;
  // default settings
  // (you can also pass in a Wire library object like &Wire2)
    
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
//Register with HUE
String deviceid = WiFi.macAddress().c_str();
HTTPClient http;                     //Declare object of class HTTPClient
  String jsonData = "{\"devicetype\":";
  jsonData += "\"";
  jsonData += "MyArduino";
  jsonData += "\"";
  jsonData += "}"; 
  http.begin("http://192.168.0.7/api");                                          //Specify request destination
  http.addHeader("Content-Type", "application/json" , "Content-Length", jsonData.length());    //Specify content-type header
 
  int httpCode = http.POST(jsonData);   //Send the request
  String payload = http.getString();    //Get the response payload

  http.end();                           //Close connection
  Serial.println(jsonData);
  Serial.println(payload);
  Serial.println(httpCode);
}


void turnOn(){
HTTPClient http;                     //Declare object of class HTTPClient
String jsonDataOn = "{\"on\":";  
  jsonDataOn += "true";
  jsonDataOn += "}"; 

  
//Turn On
  http.begin("http://192.168.0.7/api/xOICDVMHLssbId57Mdl7iF7sfhQ-Njy80ONJF6tW/lights/4/state");                                          //Specify request destination
  http.addHeader("Content-Type", "application/json" , "Content-Length", jsonDataOn.length());    //Specify content-type header
  int httpCode = http.PUT(jsonDataOn);   //Send the request
  String payload = http.getString();    //Get the response payload
  http.end();                           //Close connection
  Serial.println(jsonDataOn);
  Serial.println(payload);
  Serial.println(httpCode);
}

void turnOff(){
HTTPClient http;                     //Declare object of class HTTPClient
String jsonDataOff = "{\"on\":";  
  jsonDataOff += "false";
  jsonDataOff += "}";
  
  //Turn Off
  http.begin("http://192.168.0.7/api/xOICDVMHLssbId57Mdl7iF7sfhQ-Njy80ONJF6tW/lights/4/state");                                          //Specify request destination
  http.addHeader("Content-Type", "application/json" , "Content-Length", jsonDataOff.length());    //Specify content-type header
  int httpCode = http.PUT(jsonDataOff);   //Send the request
  String payload = http.getString();    //Get the response payload
  http.end();                           //Close connection
  Serial.println(jsonDataOff);
  Serial.println(payload);
  Serial.println(httpCode);
}

void loop(){

bool value = digitalRead(5);
if (value == 1) {
  Serial.println("Motion Detected");
  //turnOn();
} else {
  Serial.println("No Motion Detected");
  //Serial.println("1");
  //turnOff();
}
delay(60000);

}
