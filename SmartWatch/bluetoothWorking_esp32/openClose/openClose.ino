#include "WiFi.h"
#define STA_SSID "your-ssid"
#define STA_PASS "your-pass"
#define AP_SSID  "esp32"

enum { STEP_BTON, STEP_BTOFF, STEP_STA, STEP_AP, STEP_AP_STA, STEP_OFF, STEP_BT_STA, STEP_END };

void onButton(char param){
  char step = param;
  switch(step){
    case 'a'://BT Only
      Serial.println("** Starting BT");
      btStart();
    break;
    case 'b'://All Off
      Serial.println("** Stopping BT");
      btStop();
    break;
    case 'c'://STA Only
      Serial.println("** Starting STA");
      WiFi.begin(STA_SSID, STA_PASS);
    break;
    case 'd'://AP Only
      Serial.println("** Stopping STA");
      WiFi.mode(WIFI_AP);
      Serial.println("** Starting AP");
      WiFi.softAP(AP_SSID);
    break;
    case 'e'://AP+STA
      Serial.println("** Starting STA");
      WiFi.begin(STA_SSID, STA_PASS);
    break;
    case 'f'://All Off
      Serial.println("** Stopping WiFi");
      WiFi.mode(WIFI_OFF);
    break;
    case 'g'://BT+STA
      Serial.println("** Starting STA+BT");
      WiFi.begin(STA_SSID, STA_PASS);
      btStart();
    break;
    case 'h'://All Off
      Serial.println("** Stopping WiFi+BT");
      WiFi.mode(WIFI_OFF);
      btStop();
    break;
    default:
    break;
  }
  if(step == STEP_END){
    step = STEP_BTON;
  } else {
    step++;
  }
  //little debounce
  delay(100);
}

void WiFiEvent(WiFiEvent_t event){
    switch(event) {
        case SYSTEM_EVENT_AP_START:
            Serial.println("AP Started");
            WiFi.softAPsetHostname(AP_SSID);
            break;
        case SYSTEM_EVENT_AP_STOP:
            Serial.println("AP Stopped");
            break;
        case SYSTEM_EVENT_STA_START:
            Serial.println("STA Started");
            WiFi.setHostname(AP_SSID);
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            Serial.println("STA Connected");
            WiFi.enableIpV6();
            break;
        case SYSTEM_EVENT_AP_STA_GOT_IP6:
            Serial.print("STA IPv6: ");
            Serial.println(WiFi.localIPv6());
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            Serial.print("STA IPv4: ");
            Serial.println(WiFi.localIP());
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            Serial.println("STA Disconnected");
            break;
        case SYSTEM_EVENT_STA_STOP:
            Serial.println("STA Stopped");
            break;
        default:
            break;
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(0, INPUT_PULLUP);
    WiFi.onEvent(WiFiEvent);
    Serial.print("ESP32 SDK: ");
    Serial.println(ESP.getSdkVersion());
    Serial.println("Press the button to select the next mode");
}

char temp;
void loop() {
    char a;
    if (Serial.available()) {
        a = Serial.read();   
    }
    if(temp!=a){
       temp=a;
       onButton(temp);
    }  
}
