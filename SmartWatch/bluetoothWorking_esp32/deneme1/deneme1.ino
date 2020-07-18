#include "SimpleBLE.h"

#define SLEEP_SECS 30

SimpleBLE ble;

String beaconMsg = "ESP32xx"; // the x's get overwritten

// this variable maintained over deep sleeps
RTC_DATA_ATTR static uint16_t wakeupCount = 0;

void setup() {
 Serial.begin(115200);

 ble.begin(beaconMsg);

 // a delay before shutdown otherwise the packets aren't sent completely
 // the delay required seems to depend on the payload length 
 delay(20000); 

 ble.end();

 Serial.printf("Wakeup count=%i, awake for %i ms, deep sleeping for %i secs...\n", 
     wakeupCount++, millis(), SLEEP_SECS); 
 esp_deep_sleep(SLEEP_SECS * 1000000);
}

void loop() {
 // doesn't ever get here
} 
