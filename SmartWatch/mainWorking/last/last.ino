#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "BluetoothSerial.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"

/*
#include "esp_system.h"

const int button = 0;         //gpio to use to trigger delay
const int wdtTimeout = 5000;  //time in ms to trigger the watchdog
hw_timer_t *timer = NULL;

void IRAM_ATTR resetModule() {
  ets_printf("reboot\n");
  esp_restart();
}*/

#define OUTPUT_READABLE_ACCELGYRO
#define LED_PIN 2

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

const char* ssid = "mehmet";
const char* password =  "malayani";
int16_t ax[20], ay[20], az[20];
int16_t gx[20], gy[20], gz[20];
bool blinkState = false;
int PulseSensorPurplePin = 34;  
int LED13 = 2;   
int Signal[20];
int Threshold = 3450;
int jsonPretty=0;
int jsonNormal=0;
int mainDelay=30000;

const char* sdCardName = "/data.txt";
/*
  1 : Wifi
  0 : Bluetooth
*/ 
int ConnectionFlag=0; 

/*
   1 : Ivme
   0 : Pulse
*/
int SensorFlag=0;

BluetoothSerial SerialBT;
MPU6050 accelgyro;
 
void setup() {
     
  Serial.begin(115200);
  delay(4000);
  SerialBT.begin("ESP3BBBbb");
  //WiFi.begin(ssid, password);
  ivmeSet();
  sdCardSet();
/*
   Serial.println("running setup");

  pinMode(button, INPUT_PULLUP);                    //init control pin
  timer = timerBegin(0, 80, true);                  //timer 0, div 80
  timerAttachInterrupt(timer, &resetModule, true);  //attach callback
  timerAlarmWrite(timer, wdtTimeout * 1000, false); //set time in us
  timerAlarmEnable(timer);  
*/
}

void sdCardSet(){
  if(!SD.begin()){
      Serial.println("Card Mount Failed");
      return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
      Serial.println("No SD card attached");
      return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
      Serial.println("MMC");
  } else if(cardType == CARD_SD){
      Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
      Serial.println("SDHC");
  } else {
      Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  writeFile(SD,"/data.txt"," ");
}

void ivmeSet(){
  
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
  #endif
  
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();
  
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  pinMode(LED_PIN, OUTPUT);  
}

void bluetooth(){
    Serial.println("GİRDİ-B");
    //SerialBT.begin("ESP3BBBbb");
    String res = returnValues(1);
    //Serial.println(res);
    Serial.print("Length:");
    Serial.println(res.length());
    for(int i=0;i<res.length();i++){
      SerialBT.write(res.charAt(i));  
    }
    SerialBT.write('*');
    //delay(20);
    //btStop();
    Serial.println("Cikti-b");
}

void sendHttp(String val){
    Serial.println("HTTPPOST");
    Serial.println(val);
   HTTPClient http;   
   http.begin("http://40.117.95.148:9080/add");  //Specify destination for HTTP request
   //////////
   http.addHeader("Content-Type", "application/json");             //Specify content-type header
   String temp="data=";
   temp+=val; 
   int httpResponseCode = http.POST(val);   //Send the actual POST request
   if(httpResponseCode>0){
    String response = http.getString();                       //Get the response to the request
    Serial.println(httpResponseCode);   //Print return code
    Serial.println(response);           //Print request answer
   }else{
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
   }
   http.end();  //Free resources
}

void wifi(){
    Serial.println("Wifi");
    WiFi.begin(ssid, password);
    for(int i=0;i<5 && (WiFi.status() != WL_CONNECTED) ; i++){
        delay(1000);
        Serial.println("Connecting to WiFi..");  
    }
    if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status 
        sendHttp(returnValues(0));
        /*if(!isEmpty(SD,"/data.txt")){
            SDtoServer(SD,"/data.txt");
        }*/
        /*while(!isEmpty(SD,"/data.txt")){
          SDtoServer(SD,"/data.txt");
        }*/
     }
     else{
            Serial.println("Error in WiFi connection");
            Serial.println("Values to SD Card");   
     }     
     WiFi.mode(WIFI_MODE_NULL);
  }

  //////////////////////////////////////////////////////////
  /////////////////////LOOP////////////////////////////
  /////////////////////////////////////////
unsigned long startTime=0;
unsigned long endTime=0;
unsigned long mainTime=millis();
//////////LOOP(sadece wifi şuan)
void loop() {
  //timerWrite(timer, 0); //reset timer (feed watchdog)
  //long loopTime = millis();
  
  ivme();
  nabiz();
  
  if(startTime==0) startTime=millis();
  if(ConnectionFlag){
    //wifi();
    //ConnectionFlag=0;
  }else{
    //allDataToSD();
    bluetooth();
    //ConnectionFlag=1;
  }
  ConnectionFlag=0;
  endTime=millis();
  unsigned long fark=endTime-startTime;
  /*
  if(fark>mainDelay){
    startTime=0;
    endTime=0;
    ConnectionFlag=1;
    Serial.println("Reset");
  }*/
  
  /*//while button is pressed, delay up to 3 seconds to trigger the timer
  while (!digitalRead(button)) {
    Serial.println("button pressed");
    delay(500);
  }*/
  //delay(1000); //simulate work
  //loopTime = millis() - loopTime;
  
  //Serial.print("loop time is = ");
  //Serial.println(loopTime); //should be under 3000
}

///////////////////////////
///////////ACCELOREMETER FUNCTION
void ivme(){
    int16_t a,s,d,f,g,h;  
    for(int i=0;i<20;i++){
      accelgyro.getMotion6(&a,&s,&d,&f,&g,&h);
      ax[i]=a ,ay[i]=s ,az[i]=d ,gx[i]=f , gy[i]=g , gz[i]=h ;

      //Serial.printf("%d %d %d %d %d %d\n",ax[i],ay[i],az[i],gx[i],gy[i],gz[i]);
        
      delay(25);
    }
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
}

///////////////PULSE FUNCTION
void nabiz(){
   int index=0,nabizInd=0;
   bool flag=true;
   while(index<100){
     if(flag){
         int temp = analogRead(PulseSensorPurplePin);  // Read the PulseSensor's value.
         Signal[nabizInd]=temp ;                   // Send the Signal value to Serial Plotter.
         if(temp > Threshold){                          // If the signal is above "550", then "turn-on" Arduino's on-Board LED.
           digitalWrite(LED13,HIGH);
         } else {
           digitalWrite(LED13,LOW);                //  Else, the sigal must be below "550", so "turn-off" this LED.
         }
         nabizInd++;
         if(nabizInd ==20) flag=false;
      }
    index++;
    delay(5);
   }
}

String returnValues(int param){
  StaticJsonBuffer<3200> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();
  char JSONmessageBuffer[3200];
  JsonArray& xArr = JSONencoder.createNestedArray("x_axis");
  JsonArray& yArr = JSONencoder.createNestedArray("y_axis");
  JsonArray& zArr = JSONencoder.createNestedArray("z_axis");

  JsonArray& xGarr = JSONencoder.createNestedArray("x_gyro");
  JsonArray& yGarr = JSONencoder.createNestedArray("y_gyro");
  JsonArray& zGarr = JSONencoder.createNestedArray("z_gyro");
  
  JsonArray& pulseArr = JSONencoder.createNestedArray("pulse_meter");

  for(int i=0;i<20;i++) xArr.add(ax[i]);
  for(int i=0;i<20;i++) yArr.add(ay[i]);
  for(int i=0;i<20;i++) zArr.add(az[i]);

  for(int i=0;i<20;i++) xGarr.add(gx[i]);
  for(int i=0;i<20;i++) yGarr.add(gy[i]);
  for(int i=0;i<20;i++) zGarr.add(gz[i]);
  
  for(int i=0;i<20;i++) pulseArr.add(Signal[i]);

  if(param==0){
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    
    Serial.print("Return:");
    Serial.print(JSONmessageBuffer);
    Serial.println();
    
    return JSONmessageBuffer;  
  }else if (param==1){
    JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.print("Return:");
    Serial.print(JSONmessageBuffer);
    Serial.println();
    return JSONmessageBuffer;
  }else{
    return "ERROR";
  }
}
void allDataToSD(){
   String sdData=returnValues(0);
   sdData.trim();
   appendFile(SD, "/data.txt", sdData);
   appendFile(SD, "/data.txt","\n");   
}

bool isEmpty(fs::FS &fs,const char * path){
    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return false;
    }

    int count=0;
    while(file.available()){
        char temp=file.read();
        if((temp=='{') || (temp=='}')) count++;
        if(count==2){
          file.close();
          return false;
        }
        
    }
    file.close();
    return true;
}

void SDtoServer(fs::FS &fs,const char * path){
  File file = fs.open(path);
  if(!file){
      Serial.println("Failed to open file for reading");
      return;
  }

  String result="";
  while(file.available()){
      char temp=file.read();
      result+=temp;
      if(temp=='}'){
        result+=temp;
        sendHttp(result);
        result="";
      }    
  }
  
  file.close();
  writeFile(SD,"/data.txt"," ");
}

///////////////FILE FUNCTIONS
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path,String message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path,String message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}
/////////////
