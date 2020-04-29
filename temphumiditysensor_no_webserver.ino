#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <AsyncTCP.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <WiFiClient.h>
#include <HTTPClient.h>

const char* ssid = "sreddnas2g";
const char* password = "0004EDDABEAE";

#define DHTPIN 23     
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);

String readDHTTemperature() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float t = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(t);
    return String(t);
  }
}

String readDHTHumidity() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(h);
    return String(h);
  }
}

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return readDHTTemperature();
  }
  else if(var == "HUMIDITY"){
    return readDHTHumidity();
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  dht.begin();
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

}
 
void loop(){
  HTTPClient http;                     //Declare object of class HTTPClient
  String jsonData = "{\"temp\":";
  jsonData += "\"";
  jsonData += readDHTTemperature().c_str();
  jsonData += "\"";
  jsonData += ",\"humidity\":";
  jsonData += "\"";
  jsonData += readDHTHumidity().c_str();
  jsonData += "\"";
  jsonData += "}"; 
  Serial.println(jsonData); 
  http.begin("http://192.168.0.2/readings/write1.php");                                          //Specify request destination
  http.addHeader("Content-Type", "application/json" , "Content-Length", jsonData.length());    //Specify content-type header

  Serial.println( " POSTing to Server...");
 
  int httpCode = http.POST(jsonData);   //Send the request
  String payload = http.getString();    //Get the response payload

  Serial.println(httpCode);             //Print HTTP return code
  Serial.println(payload);            //Print request response payload

  http.end();                           //Close connection
 
  delay(300000);                         //Post Data at every 5 Minutes (300 seconds)

}
