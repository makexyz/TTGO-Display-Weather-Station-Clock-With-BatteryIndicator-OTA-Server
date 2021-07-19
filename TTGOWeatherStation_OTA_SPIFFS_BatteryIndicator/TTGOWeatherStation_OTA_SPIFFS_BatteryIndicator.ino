#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include "SPIFFS.h"

#include <Pangodream_18650_CL.h>

#include <SimpleKalmanFilter.h>
SimpleKalmanFilter kalmanFilter(1, 1, 0.001);

//
#include "ani.h"
#include <SPI.h>
#include <TFT_eSPI.h>             // Hardware-specific library
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson.git
#include <NTPClient.h>            //https://github.com/taranais/NTPClient

TFT_eSPI tft = TFT_eSPI();        // Invoke custom library

#define TFT_GREY 0x5AEB
#define lightblue 0x01E9
#define darkred 0xA041
#define blue 0x5D9B
#include "Orbitron_Medium_20.h"

#include <WiFiUdp.h>
#include <HTTPClient.h>

#define MIN_USB_VOL 4.9
#define ADC_PIN 34
#define CONV_FACTOR 1.8
#define READS 20

#define BTN_1 0
#define BTN_2 35

Pangodream_18650_CL BL(ADC_PIN, CONV_FACTOR, READS);

const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 0;

const char* ssid     = "NhaVoi";       ///EDIIIT
const char* password = "Nhavoi@060418"; //EDI8IT

AsyncWebServer server(80);

String town="Ho%20Chi%20Minh";              
String Country="VN";                
//const String endpoint = "http://api.openweathermap.org/data/2.5/weather?q="+town+","+Country+"&units=metric&APPID=";
const String endpoint = "http://api.openweathermap.org/data/2.5/weather?q="+town+"&units=metric&APPID=";
const String key = "1b118e347eecdb6a19b40559801e9bcb"; /*EDDITTTTTTTTTTTTTTTTTTTTTTTT                      */

String payload=""; //whole json 
String tmp="" ; //temperatur
String hum="" ; //humidity
  

StaticJsonDocument<1000> doc;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

int backlight[5] = {10,30,60,120,220};
byte b=1;

// Variables for button interaction
int i=0;
String tt="";
int count=0;
bool isFirstScreen = 1;
int isButton1Pressed = 0; 
int isButton2Pressed = 0;

int frame=0;
String curSeconds="";
int curBattery=0;
int batteryLevel=0;

void setup(void) {
  pinoutInit();
  Serial.begin(115200);
  
  tft.init();
  portraitScreenSetup();
  SPIFFSInit();
  wifiInit();
  serverInit();

  screen1Init();
  
  ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
  ledcAttachPin(TFT_BL, pwmLedChannelTFT);
  ledcWrite(pwmLedChannelTFT, backlight[b]);  
  delay(3000);

// Initialize a NTPClient to get time
  timeClient.begin(); 
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(25200);   /*EDDITTTTTTTTTTTTTTTTTTTTTTTT                      */
  getData();
  delay(500);

  // xTaskCreate(battery_info, "battery_info", 2048, NULL, 1, NULL);
}

void loop() {
  if(digitalRead(BTN_2)==0){
    if(isButton2Pressed==0){
      isButton2Pressed=1;
      tft.fillRect(78,216,44,12,TFT_BLACK);
   
      b++;
      if(b>=5)b=0;
    
      for(int i=0;i<b+1;i++)
      tft.fillRect(78+(i*7),216,3,10,blue);
      ledcWrite(pwmLedChannelTFT, backlight[b]);
    }
  }
  else isButton2Pressed=0;
  
  if(digitalRead(BTN_1)==0){
    if(isButton1Pressed==0){ 
      isButton1Pressed=1;
      isFirstScreen=!isFirstScreen;
    }
  } 
  else isButton1Pressed=0;
  
  battery_info();
  
  if (!isFirstScreen) {
    if (isButton1Pressed) screen2Init();
  } else {
      if (isButton1Pressed) screen1Init();
      
      tft.pushImage(0, 88,  135, 65, ani[frame]);
      frame++;
      if(frame>=10)
      frame=0;
      
      if(count==0) getData();
      count++;
      if(count>2000)count=0;
       
      tft.setFreeFont(&Orbitron_Medium_20);
      tft.setCursor(2, 187);
      tft.println(tmp.substring(0,3));
      
      tft.setCursor(2, 227);
      tft.println(hum+"%");
      
      tft.setTextColor(TFT_ORANGE,TFT_BLACK);
      tft.setTextFont(2);
      tft.setCursor(6, 44);
      tft.println(dayStamp);
      tft.setTextColor(TFT_WHITE,TFT_BLACK);
    
      while(!timeClient.update()) {
        timeClient.forceUpdate();
      }
      // The formattedDate comes with the following format:
      // 2018-05-28T16:00:13Z
      // We need to extract date and time
      formattedDate = timeClient.getFormattedDate();
      Serial.println(formattedDate);
    
      int splitT = formattedDate.indexOf("T");
      dayStamp = formattedDate.substring(0, splitT);
     
      timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
             
//      if(curSeconds!=timeStamp.substring(6,8)){
//        tft.fillRect(78,170,48,28,darkred);
//        tft.setFreeFont(&Orbitron_Light_24);
//        tft.setCursor(81, 192);
//        tft.println(timeStamp.substring(6,8));
//        curSeconds=timeStamp.substring(6,8);
//      }
      
      if(curBattery !=  batteryLevel){
        tft.fillRect(78,170,48,28,darkred);
        tft.setFreeFont(&Orbitron_Light_24);
        tft.setCursor(81, 192);
        tft.println(batteryLevel);
        curBattery = batteryLevel;
      }
    
      tft.setFreeFont(&Orbitron_Light_32);
      String current=timeStamp.substring(0,5);
      if(current!=tt){
        tft.fillRect(3,8,120,30,TFT_BLACK);
        tft.setCursor(5, 34);
        tft.println(timeStamp.substring(0,5));
        tt=timeStamp.substring(0,5);
      } 
    }
  delay(100);
}


void getData()
{
  tft.fillRect(1,170,64,20,TFT_BLACK);
  tft.fillRect(1,210,64,20,TFT_BLACK);
  if ((WiFi.status() == WL_CONNECTED)){ //Check the current connection status
    HTTPClient http;
    http.begin(endpoint + key); //Specify the URL
    int httpCode = http.GET();  //Make the request
 
    if (httpCode > 0) { //Check for the returning code
      payload = http.getString();
      // Serial.println(httpCode);
      Serial.println(payload);
    }
    else {
      Serial.println("Error on HTTP request");
    }
    http.end(); //Free the resources
  }
  char inp[1000];
  payload.toCharArray(inp,1000);
  deserializeJson(doc,inp);
  
  String tmp2 = doc["main"]["temp"];
  String hum2 = doc["main"]["humidity"];
  String town2 = doc["name"];
  tmp=tmp2;
  hum=hum2;
  
  Serial.println("Temperature"+String(tmp));
  Serial.println("Humidity"+hum);
  Serial.println(town);
 }

///////////////////////////////////////////////////////////////
void portraitScreenSetup(){
  tft.setRotation(2); //0 | 1 | 2 | 3
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE,TFT_BLACK);  
  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.setSwapBytes(true);
}

void landscapeScreenSetup(){
  tft.setRotation(1); //0 | 1 | 2 | 3
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE,TFT_BLACK);  
  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.setSwapBytes(true);
}

//void drawingText(String text){
//  tft.fillRect(0, 0, 240, 36 ,TFT_BLACK);
//  tft.drawString(text,0,0, 4);
//}

void battery_info()
{
    batteryLevel = kalmanFilter.updateEstimate(BL.getBatteryChargeLevel());
    if(!isFirstScreen) {
      if(BL.getBatteryVolts() >= MIN_USB_VOL){
        tft.drawString("Charging",0,0,4);
      }
    }
}

void wifiInit() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  tft.print("Connecting to Wifi");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    tft.print(".");
  }
  tft.println("");
  tft.print("Connected to ");
  tft.println(ssid);
  tft.print("IP address: ");
  tft.println(WiFi.localIP());
}


///////////////////////////////////////////////////////////////

void serverInit(){
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to load style.css file
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script.js", "text/javascript");
  });

  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  tft.println("HTTP server started");
}

 void pinoutInit(){
  pinMode(BTN_1,INPUT_PULLUP);
  pinMode(BTN_2,INPUT);
  pinMode(14, OUTPUT);
  digitalWrite(14, HIGH);
}

void SPIFFSInit(){
  if(!SPIFFS.begin(true)){
    tft.println("An Error has occurred while mounting SPIFFS");
    tft.println("Can not create Web Server");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  tft.println("\r\nInitialisation done.");
}

void screen1Init() {     
  portraitScreenSetup();
  
  tft.setCursor(2, 232, 1);
  tft.println(WiFi.localIP());
  tft.setCursor(80, 204, 1);
  tft.println("BRIGHT:");      
  
  tft.setCursor(80, 152, 2);
  tft.println("BAT:");
  tft.setTextColor(TFT_WHITE,lightblue);
  tft.setCursor(4, 152, 2);
  tft.println("TEMP:");
  
  tft.setCursor(4, 192, 2);
  tft.println("HUM: ");
  tft.setTextColor(TFT_WHITE,TFT_BLACK);
  
  tft.setFreeFont(&Orbitron_Medium_20);
  tft.setCursor(6, 82);
  tft.println("HoChiMinh");
  
  tft.fillRect(68,152,1,74,TFT_GREY);
  
  for(int i=0;i<b+1;i++)
  tft.fillRect(78+(i*7),216,3,10,blue);
}

void screen2Init() {     
  landscapeScreenSetup();
  
  tft.drawString("Screen 2", (240-8*4)/2, 136/2-16);
  tft.drawString("LANDSCAPE", (240-9*4)/2, 136/2+8);
}
         

     
