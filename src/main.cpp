
#include "iestatijumi.h"
#include "telnet.h"
#include "pogas.h"
#include "temp_iestat.h"
#include "arrow.h"
#include "Free_Fonts.h"
#include "FreeSansBold42pt7b.h"
#include <kluda.h>
#include <setting.h>
int rawx=0;
int wood=0;
int wood2=0;
int damper2=0;
int raw2[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int TempHist[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Set temperature history array
bool setti = false;
bool old_setti = false;
 //Returns 'true' for refilled and temperature climbing or 'false' for temperature falling
bool WoodFilled(int CurrentTemp) {
   for (int i = 9; i > 0; i--) {
     TempHist[i] = TempHist[i-1];
   }
   TempHist[0] = CurrentTemp;

   if (float((TempHist[0]+TempHist[1]+TempHist[2]+TempHist[3]+TempHist[4])/5) > float((TempHist[5]+TempHist[6]+TempHist[7]+TempHist[8]+TempHist[9]+TempHist[10])/6)) {
     return true;
     telnet.print("dds");
  }
   else {
    return false;
    telnet.print("vv");
   }
 }

void setup(void) 
{
   
 Serial.begin(115200);
   

     Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
 
  
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
ds.begin();

   
    delay(50);
    telnet.begin(port);   

          server.on("/", handle_OnConnect);
  

  server.begin();
  Serial.println("HTTP server started");
   

}

/// @brief 
void loop() {
server.handleClient();


telnet.loop();

  // send serial input to telnet as output
  if (Serial.available()) {
    telnet.print(Serial.read());
  }
esp_bluedroid_disable;

esp_sleep_enable_ext0_wakeup(GPIO_NUM_15,0);

ArduinoOTA.handle();


  unsigned long currentMillis = millis();

  if (currentMillis - lastExecutedMillis >= EXE_INTERVAL) {
    
    ds.requestTemperatures();
    temperature = ds.getTempC(sensor1);
     // save the last executed time
  //telnet.println("erri:" + String(errI) +" |  " + "temperatura:" + String(temperature) + " | " + "eerrp:" + String(errP));

  }

if (temperature < 0){
  ds.requestTemperatures();
    temperature = ds.getTempC(sensor1);
    kludas++;
    temperature = -1; 
 }
 else
 {kludas = 0;}
  
//pot_raw = analogRead(15); 
pogas ();                           // pogas funkcijas
         



for (int i = 9; i > 0; i--) {
     raw2[i] = raw2[i-1];
   }

raw2[0]=pot_raw;   



    
    if(digitalRead(relayPort)==HIGH)
      {messageinfo = "VENTILATOR ON    ";}
      
    else 
      {messageinfo = "VENTILATOR OFF";}
    
         // read thermocouple temp in C

 
  if (temperature >90)
    { tone(buzzerPort, buzzerRefillFrequency);
    delay(buzzerRefillDelay);
    noTone(buzzerPort);
    delay(buzzerRefillDelay);}
           
  
 
       
  temp_iestat();
         
  

       
    if (pot <= potRelMax ) {  
      // Manual damper regulation mode if potentiometer reads 100% or less
      errI = 0;  // reset integral term based on user intent for manual control
      errD = 0;  // reset derivative term based on user intent for manual control
      endBuzzer = true;  // setting to disable end of fire buzzer
      refillBuzzer = true;  //setting to disable refill buzzer
      damper = round(pot * maxDamper / 100);  // scales damper setting according to max setting
      messageDamp = "Manual";} // set damper output message, manual
    
    else 
    { 
      if (currentMillis - lastExecutedMillis >= EXE_INTERVAL) {
      
      if (errI < endTrigger) {
        // Automatic PID regulation
        errP = targetTempC - temperature;  // set proportional term
        errI = errI + errP;                // update integral term
        errD = errP - errOld;              // update derivative term
        errOld = errP;
        WoodFilled(temperature);  // Call function that checks if wood is refilled to update array

        wood = (TempHist[0]+TempHist[1]+TempHist[2]+TempHist[3]+TempHist[4])/5;
        wood2 =(TempHist[5]+TempHist[6]+TempHist[7]+TempHist[8]+TempHist[9])/5;

              if (wood>wood2) {
          errI = 0;  // reset integral term after wood refill
          telnet.println("reset12");
        }

        if (errI<0){
          errI=0;
        }
        // set damper position and limit damper values to physical constraints
        damper = kP * errP + kI * errI + kD * errD;
        damper2 = kP * errP + kI * 8000 + kD * errD;
        if (damper < minDamper) damper = minDamper;
        if (damper > maxDamper) damper = maxDamper;

        messageDamp = "Auto"; // set damper output message, auto

        //Refill Alarm
        if (errI > refillTrigger) 
          { messageDamp = "Fill"; 
           
                
        }
        else {messageDamp = "AUTO";}

      }

    else {
        // End of combustion condition for errI >= endTrigger

        messageDamp = "End "; // set damper output message, end
        

      if (temperature < temperatureMin) {
          damper = zeroDamper;
          sleep_ = true;
          

        }

}
        
        }
    }
 
  telnet.println("raw2[1]:" + String(raw2[1]) +" |  " + "damper2:" + String(damper2) + "raw2[3]:" + String(raw2[3]) +" |  " + "raw2[4]:" + String(raw2[4]) +"raw2[5]:" + String(raw2[5]) +" |  " + "raw2[6]:" + String(raw2[6]));

text4.createSprite(130, 40);
  if (kludas > 20)
    {
      img.createSprite(200, 200);
      img.pushImage(0,0,200,200,kluda);
      text4.setTextColor(TFT_GREEN,TFT_BLACK);

messageDamp = "Error"; 
 
  tone(buzzerPort, buzzerRefillFrequency);
    delay(buzzerRefillDelay);
    noTone(buzzerPort);
    delay(buzzerRefillDelay);
 
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
  
  img2.createSprite(17, 100);
  
   if 
    (pot_raw>=12 && pot_raw<=25) {
       delay(500);    
      setti = !setti;
       delay(100);    
    } 
      //right 
      
     
     if (old_setti != setti) {
      if(setti == true){
      bckg.pushImage(0,0,320,240,setting);
      bckg.pushSprite(0,0,TFT_BLACK);
      
      
      }
      
       
      old_setti = setti;}
   

      
      if(setti == false) {
        bckg.pushImage(0,0,320,240,arrow);
        y=105-temperature;
img2.fillRect(0,0,17,y,TFT_WHITE),
img2.pushToSprite(&bckg,73,30, TFT_BLACK);

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
img2.deleteSprite();
}






    
  telnet.println("erri:" + String(errI) +" |  " + "wood:" + String(wood) + " > " + "wood2:" + String(wood2));     
  telnet.println("-");
  telnet.println("pot:" + String(pot) +" |  " + "temperatura:" + String(temperature) + " | " + "potrav:" + String(pot_raw));
telnet.println("-");
telnet.println("target:" + String(targetTempC) +" |  " + "pot_raw:" + String(pot_raw) + " > " + "pot:" + String(pot));  
telnet.println("-");
 


    // Drive servo and print damper position to the lcd
if (currentMillis - lastExecutedMillis >= EXE_INTERVAL){
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

       
if(sleep_==true)
  {delay(50);
    esp_deep_sleep_start();}

    // Toggle oddLoop that controls display message on line 2
oddLoop = !oddLoop;
    // Delay between loop cycles
      z=currentMillis - lastExecutedMillis;

lastExecutedMillis = currentMillis;

    }
    delay(200);

  
}


