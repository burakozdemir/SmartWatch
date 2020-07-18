#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "BluetoothSerial.h"
/*
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
*/
BluetoothSerial SerialBT;
 
const char* ssid = "brkasus";
const char* password =  ".,.,.,00";
 
void setup() {
 
  Serial.begin(115200);
  delay(4000);   //Delay needed before calling the WiFi.begin

  SerialBT.begin("ESP32test"); //Bluetooth device name
  //Serial.println("The device started, now you can pair it with bluetooth!");
 
  WiFi.begin(ssid, password); 
 
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
 
}

String returnValues(){
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();
 
  JSONencoder["X"] = 1;
  JSONencoder["Y"] = 2;
  JSONencoder["Z"] = 3;

  char JSONmessageBuffer[300];
  JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.print("Return:");
  Serial.print(JSONmessageBuffer);
  Serial.println();
  return JSONmessageBuffer;
}
 
void loop() {
/*
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }
  delay(20);
 */
 if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status 
   HTTPClient http;   
   http.begin("http://13.80.253.206:5000/jsonDeneme");  //Specify destination for HTTP request
   http.addHeader("Content-Type", "application/json");             //Specify content-type header


   String temp="data=";
   temp+=returnValues(); 
   int httpResponseCode = http.POST(returnValues());   //Send the actual POST request
 
   if(httpResponseCode>0){
 
    String response = http.getString();                       //Get the response to the request
 
    Serial.println(httpResponseCode);   //Print return code
    Serial.println(response);           //Print request answer
 
   }else{
 
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
 
   }
 
   http.end();  //Free resources
 
 }else{
 
    Serial.println("Error in WiFi connection");   
 
 }
 
  delay(10000);  //Send a request every 10 seconds
 
}
