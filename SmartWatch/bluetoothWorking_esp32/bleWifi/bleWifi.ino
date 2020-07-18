#include <WiFi.h>
#include "BLEDevice.h"

BLEClient* bleClient;
BLEAddress bleAddress = BLEAddress("<address>");

WiFiClient wifiClient;

void doBleStuff(void * parameter) {
  delay(6000);
  BLEDevice::init("");
  bleClient = BLEDevice::createClient();

  while(true) {
      Serial.println("Connecting (BLE)...");
      bleClient->connect(bleAddress);
      Serial.println("Reading...");
      std::string value = bleClient->getValue(BLEUUID("00001800-0000-1000-8000-00805f9b34fb"), BLEUUID("00002a00-0000-1000-8000-00805f9b34fb"));
      Serial.print("Value: ");
      Serial.println(value.c_str());
      bleClient->disconnect();

    Serial.println("Waiting (BLE)...");
    delay(10000);
  }

  delete bleClient;
  vTaskDelete(NULL);
}

void doWifiStuff(void * parameter) {
  wifiClient = WiFiClient();
  while(true) {  
    Serial.println("Connecting (WiFI)...");
    WiFi.begin("<SSID>", "<PASSWD>");

    delay(5000); 

    if (WiFi.waitForConnectResult() == WL_CONNECTED) { 
        Serial.print("WiFi connected. IP address: ");
        Serial.println(WiFi.localIP());
    }

    Serial.println("Waiting (WiFi)...");
    delay(500000);
    Serial.println("Disconnecting (WiFI)...");
    WiFi.disconnect();
  }

  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200);
  xTaskCreate(doWifiStuff, "WifiTask", 10000, NULL, 1, NULL);
  xTaskCreate(doBleStuff, "BleTask", 10000, NULL, 1, NULL);
}

void loop() {
  delay(10000); 
}
