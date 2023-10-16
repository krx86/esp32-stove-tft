#include "Arduino.h"
#include <arrow.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP32Servo.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include <WiFi.h>

#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <esp_sleep.h>
#include <esp_bt_main.h>
#include "Free_Fonts.h"
#include "FreeSansBold42pt7b.h"
#include <kluda.h>

#define GFXFF 1



const char* host = "esp32";
const char* ssid = "HUAWEI-B525-90C8";
const char* password = "BTT6F1EA171";



OneWire oneWire(6);
DallasTemperature ds(&oneWire);
DeviceAddress sensor1 = {0x28, 0xA6, 0xE4, 0x49, 0xF6, 0xF2, 0x3C, 0xF};

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
TFT_eSprite img = TFT_eSprite(&tft);
TFT_eSprite bckg = TFT_eSprite(&tft);
TFT_eSprite text = TFT_eSprite(&tft);
TFT_eSprite text2 = TFT_eSprite(&tft);
TFT_eSprite text3 = TFT_eSprite(&tft);
TFT_eSprite text4 = TFT_eSprite(&tft);


float error = 0.0;                  // Temperature compensation error

int x=20;
// Buzzer setup variables:
int buzzerPort = 2;               // Buzzer port id
int buzzerRefillFrequency = 1900; // Buzzer tone frequency for refill alarm
int buzzerRefillRepeat = 1;       // Number of refill alarm tones
int buzzerRefillDelay = 1000;     // Delay between refill alarm tones
int buzzerEndFrequency = 950;     // Buzzer tone frequency for end of fire damper close alarm
int buzzerEndRepeat = 1;          // Number of tones for end of fire damper close alarm
int buzzerEndDelay = 200;        // Delay of tone for end of fire damper close alarm
byte E;

// Potentiometer variables
int servoPort = 5;
int potPort = A3;
int relayPort = 13;

// Device objects - create servo, therocouple, and lcd objects 
Servo myservo;

// Servo calibration settings
float servoCalibration = 1.5;  // 1.0 is neutral cal - adjust value so servo arm drives closed damper when damper variable equals 0 (0%)
float servoOffset = 29;  // offset angle for servo angle adjustment
float servoAngle = 35;  // adjust value to define total angular travel of servo so that cable drives damper from fully opened to fully closed


int temperature = 0;       // initialize temperature variable for C
int temperatureMin = 47; // under this temperature (38C = 100F), the regulation closes the damper if end of fire conditions are met
int targetTempC = 0;   // the target temperature as measured by the thermocouple (135 C = 275 F)
float errP = 0.0;          // initialize the proportional term
float errD = 0.0;          // initialize the derivative term
float errI = 0.0;          // initialize the integral term
float errOld = 0.0;        // initialize term used to capture prior cycle ErrP
int kP = 0;            // Overall gain coefficient and P coefficient of the PID regulation
float tauI = 1000;         // Integral time constant (sec/repeat)
float tauD = 5;            // Derivative time constant (sec/reapeat)
float kI =  kP/tauI;        // I coefficient of the PID regulation
float kD = kP/tauD;        // D coefficient of the PID regulation

float refillTrigger = 100000;// refillTrigger used to notify need of a wood refill
float endTrigger = 155000;  // closeTrigger used to close damper at end of combustion

int pot_raw = 0;
int pot = 120;
int oldPot = 0;
float potMax = 1000.0;   // Potentiometer calibration
int potRelMax = 100;     // Potentiometer value above which the regulator runs in automatic mode

int difft = 0;
int angle = 0;
int damper = 0;
int oldDamper = 0;
int diff = 0;
float maxDamper = 100.0;  // Sets maximum damper setting
float minDamper = 0.0;    // Sets minimum damper setting
float zeroDamper = 0.0;   // Sets zero damper setting - note that stove allows some amount of airflow at zero damper



String messageDamp;    // Initialize message for damper 2
String messageinfo;    // Initialize message for damper 3
bool endBuzzer = true;
bool refillBuzzer = true;
bool oddLoop = true;
bool sleep_ = false;

int TempHist[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Set temperature history array

 //Returns 'true' for refilled and temperature climbing or 'false' for temperature falling
bool WoodFilled(int CurrentTemp) {
   for (int i = 9; i > 0; i--) {
     TempHist[i] = TempHist[i-1];
   }
   TempHist[0] = CurrentTemp;

   if (float((TempHist[0]+TempHist[1]+TempHist[2]+TempHist[3]+TempHist[4]+TempHist[5])/5) > float((TempHist[6]+TempHist[7]+TempHist[8])+TempHist[9]+TempHist[10]/5)) {
     return true;
  }
   else {
    return false;
   }
 }


void setup(void) 
{
   
  
    Serial.begin(115200);  // opens serial port, sets data rate to 9600 bps

     Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
    
    analogReadResolution(9);
      adcAttachPin(15);
    
    myservo.attach(5);  // attach servo object to pin 10
    myservo.write(50); 
    myservo.detach();    // detach servo object from its pin
    
    pinMode(buzzerPort, OUTPUT); // configure buzzer pin as an output
    pinMode(relayPort, OUTPUT);

    digitalWrite(relayPort, LOW);

    E= ds.getDeviceCount();  

    
    tft.init();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    
    bckg.createSprite(320, 240);
    
    bckg.setSwapBytes(true);

   
    delay(50);
    
   
}

/// @brief 
void loop() {


esp_bluedroid_disable;

esp_sleep_enable_ext0_wakeup(GPIO_NUM_15,0);

ArduinoOTA.handle();

ds.requestTemperatures();
ds.begin();

pot_raw = analogRead(15); 

    if (pot_raw>=70 && pot_raw<=85)     //down
    {pot = pot -10;}
    
     if (pot_raw>=0 &&  pot_raw<=10)     //up
     {pot = pot +10;}
   
    if (pot_raw>=30 && pot_raw<=50)     //left
      {digitalWrite(relayPort, LOW);}   

    if (pot_raw>=12 && pot_raw<=25)     //right 
      {
        digitalWrite(relayPort, HIGH);}
  
         
    if(digitalRead(relayPort)==HIGH)
      {messageinfo = "VENTILATOR ON    ";}
      
    else 
      {messageinfo = "VENTILATOR OFF";}

    
    if (pot_raw>=140 && pot_raw<=180)
      { ESP.restart(); }

temperature = ds.getTempC(sensor1);          // read thermocouple temp in C

 
  if (temperature >90)
    { tone(buzzerPort, buzzerRefillFrequency);
    delay(buzzerRefillDelay);
    noTone(buzzerPort);
    delay(buzzerRefillDelay);}
           
  if (pot <= 100) 
    {targetTempC = 65; 
      if (temperature > 0 && temperature < 40) {kP = 2;}
      if (temperature > 41 && temperature < 54) {kP = 4;}
      if (temperature > 55 && temperature < 62) {kP = 7;}
      if (temperature > 63 && temperature < 100) {kP = 10;}}       
  
  if (pot == 110) {
      targetTempC = 67; 
      if (temperature > 0 && temperature < 50) {kP = 2;}
      if (temperature > 51 && temperature < 55) {kP = 4;}
      if (temperature > 56 && temperature < 60) {kP = 7;}
      if (temperature > 61 && temperature < 64) {kP = 9;}
      if (temperature > 65 && temperature < 100) {kP = 11;}}
       
  if (pot == 120) {
      targetTempC = 73; 
      if (temperature > 0 && temperature < 50) {kP = 2;}
      if (temperature > 51 && temperature < 55) {kP = 4;}
      if (temperature > 56 && temperature < 60) {kP = 6;}
      if (temperature > 61 && temperature < 65) {kP = 7;}
      if (temperature > 66 && temperature < 100) {kP = 9;}}
         
  if (pot == 130) {
      targetTempC = 75; 
      if (temperature > 0 && temperature < 50) {kP = 2;}
      if (temperature > 51 && temperature < 55) {kP = 4;}
      if (temperature > 56 && temperature < 60) {kP = 6;}
      if (temperature > 61 && temperature < 65) {kP = 7;}
      if (temperature > 66 && temperature < 100) {kP = 9;}}
        
  if (pot == 140) {
      targetTempC = 79; 
      if (temperature > 0 && temperature < 50) {kP = 2;}
      if (temperature > 51 && temperature < 60) {kP = 4;}
      if (temperature > 61 && temperature < 68) {kP = 6;}
      if (temperature > 69 && temperature < 73) {kP = 8;}
      if (temperature > 76 && temperature < 100) {kP = 9;}}
      

       
    if (pot <= potRelMax ) {  
      // Manual damper regulation mode if potentiometer reads 100% or less
      errI = 0;  // reset integral term based on user intent for manual control
      errD = 0;  // reset derivative term based on user intent for manual control
      endBuzzer = true;  // setting to disable end of fire buzzer
      refillBuzzer = true;  //setting to disable refill buzzer
      damper = round(pot * maxDamper / 100);  // scales damper setting according to max setting
      messageDamp = "Manual";} // set damper output message, manual
    
    else 
    { if (errI < endTrigger) {
        // Automatic PID regulation
        errP = targetTempC - temperature;  // set proportional term
        errI = errI + errP;                // update integral term
        errD = errP - errOld;              // update derivative term
        errOld = errP;
        WoodFilled(temperature);  // Call function that checks if wood is refilled to update array

        // set damper position and limit damper values to physical constraints
        damper = kP * errP + kI * errI + kD * errD;
        if (damper < minDamper) damper = minDamper;
        if (damper > maxDamper) damper = maxDamper;

        messageDamp = "Auto"; // set damper output message, auto

        //Refill Alarm
        if (errI > refillTrigger) 
          { messageDamp = "Fill"; 
           
          

        if (WoodFilled(temperature)) {
            errI = 0;  // reset integral term after wood refill
           
            }
          
        }

      }

    else {
        // End of combustion condition for errI >= endTrigger

        messageDamp = "End "; // set damper output message, end
        

      if (temperature < temperatureMin) {
          damper = zeroDamper;
          sleep_ = true;
          

        }

      if (WoodFilled(temperature)) {
          errI = 0;  // reset integral term after wood refill
        }}}
 

text4.createSprite(130, 40);
  if (temperature <0)
    {
      img.createSprite(200, 200);
      img.pushImage(0,0,200,200,kluda);
      text4.setTextColor(TFT_GREEN,TFT_BLACK);

temperature = -1;
messageDamp = "Error"; 
 
  tone(buzzerPort, buzzerRefillFrequency);
    delay(buzzerRefillDelay);
    noTone(buzzerPort);
    delay(buzzerRefillDelay);
    temperature = -1;  
    }  
else
  {text4.setTextColor(10,TFT_BLACK);}


    if (oddLoop) 
    { messageinfo = "Dmp " + String(damper) + "% Pot " + round(pot);} // set alt damper output message

   text.createSprite(90, 65);
    text.setTextColor(TFT_RED,TFT_WHITE);

    text3.createSprite(50, 50);
    text3.setTextColor(10,TFT_BLACK);

    text2.createSprite(100, 50);
    text2.setTextColor(10,TFT_BLACK);
  
  
bckg.pushImage(0,0,320,240,arrow);

 text.setFreeFont(&FreeSansBold42pt7b);
text.drawString(String(temperature),0,0,GFXFF);
text.pushToSprite(&bckg,125,60, TFT_BLACK);

text2.setFreeFont(FF35);
text2.drawString(String(damper) + " %" ,0,0,GFXFF);
text2.pushToSprite(&bckg,190,190, TFT_BLACK);

text3.drawString(String(targetTempC) ,0,0,4);
text3.pushToSprite(&bckg,120,23, TFT_BLACK);

img.pushToSprite(&bckg,90,0, TFT_BLACK);

text4.setFreeFont(FF23);
text4.drawString(messageDamp,0,0,GFXFF);
text4.pushToSprite(&bckg,20,188, TFT_BLACK); 

bckg.pushSprite(0,0,TFT_BLACK);


 text.unloadFont();
text2.unloadFont();
text3.unloadFont();
text3.unloadFont();

text.deleteSprite();
text2.deleteSprite();
text3.deleteSprite();
text4.deleteSprite(); 
img.deleteSprite();

    // Drive servo and print damper position to the lcd
diff = damper - oldDamper;
    
  if (abs(diff) > 8) {  // move servo only if damper position change is more than 2%
      if (!oddLoop) {
        //gfx->println("x");   // print x after damper message to indicate active movement
      }
      delay(50);
      myservo.attach(5);
      if (diff > 0) {  // action if damper should be moved in the opened direction
        for (int i = 0; i < diff; i++) {
          angle = (oldDamper + i + 1) * servoAngle / (maxDamper * servoCalibration) + servoOffset;
          myservo.write(angle);
          delay(50);
        }}
         
  if (diff < 0) {  // action if damper should be moved in the closed direction
        for (int i = 0; i < abs(diff); i++) {
          angle = (oldDamper - i - 1) * servoAngle / (maxDamper * servoCalibration) + servoOffset;
          myservo.write(angle);
          delay(50);
        }}
                
      myservo.detach();
      oldPot = pot;
      oldDamper =  damper;}
  //Serial.print(String(damper));

        // Regulator model data via serial output
    // Output: tempC, tempF, damper%, damper(calculated), damperP, damperI, damperD, errP, errI, errD
    //Serial.println(pot_raw);
    
  /*   
        
  Serial.print(temperature);
  Serial.print(",");
  Serial.print(CurrentTemp);
    Serial.print(",");
    Serial.print(round(damper));
    Serial.print(",");
    Serial.print(round(kP*errP + kI*errI + kD+errD));
    Serial.print(",");
    Serial.print(round(kP*errP));
    Serial.print(",");
    Serial.print(round(kI*errI));
    Serial.print(",");
    Serial.print(round(kD*errD));
    Serial.print(",");  
    Serial.print(errP);
    Serial.print(",");
    Serial.print(errI);
    Serial.print(",");
    Serial.print(errD);
    Serial.print(",");    
    Serial.println(); */
 
//Serial.print(angle);
if(sleep_==true)
  {delay(50);
    esp_deep_sleep_start();}

    // Toggle oddLoop that controls display message on line 2
oddLoop = !oddLoop;
    // Delay between loop cycles
    delay(20);

      
}
