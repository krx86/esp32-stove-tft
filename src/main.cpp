
#include "iestatijumi.h"
#include "telnet.h"
#include "pogas.h"
//#include "temp_iestat.h"
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
bool suknis ;
bool suknis_old ;
int krx6 = 0;
int krx56 = 0;
bool krx106 = false;
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
     
     E= ds.getDeviceCount();  
    ds.begin();
    
     tft.init();
    tft.setRotation(3);
    tft.fillScreen(TFT_WHITE);
    bckg.setSwapBytes(true);

      Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
    
    analogReadResolution(9);
    //analogSetAttenuation(ADC_0db); 
      adcAttachPin(9);
    
    myservo.attach(5);  // attach servo object to pin 10
    myservo.write(50); 
    myservo.detach();    // detach servo object from its pin
    
    pinMode(buzzerPort, OUTPUT); // configure buzzer pin as an output
    pinMode(relayPort, OUTPUT);

    //digitalWrite(relayPort, HIGH);


   

     Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
/*   while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  } */

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

    delay(50);
    telnet.begin(port);   
      server.on("/", handle_OnConnect);
      server.on("/set_target_temp", HTTP_POST, handle_SetTargetTemp);
      server.on("/set_kP", HTTP_POST, handle_SetkP);
      server.on("/temperature", HTTP_GET, handle_GetTemperature);
      server.on("/damper", HTTP_GET, handle_GetDamper);
      server.on("/target_temp", HTTP_GET, handle_GetTargetTemp);
      server.on("/kP_value", HTTP_GET, []() {
        server.send(200, "text/plain", String(kP)); // Atgriež kP vērtību kā tekstu
    });
      server.begin();

  Serial.println("HTTP server started");
   

}

/// @brief 
void loop() {
server.handleClient();


telnet.loop();

  // send serial input to telnet as output
 /*  if (Serial.available()) {
    telnet.print(Serial.read());
  } */
esp_bluedroid_disable;

esp_sleep_enable_ext0_wakeup(GPIO_NUM_9,0);

ArduinoOTA.handle();


  unsigned long currentMillis = millis();

  if (currentMillis - lastExecutedMillis >= EXE_INTERVAL) {
    
    ds.requestTemperatures();
    temperature = ds.getTempC(sensor1);
     // save the last executed time
  //telnet.println("erri:" + String(errI) +" |  " + "temperatura:" + String(temperature) + " | " + "eerrp:" + String(errP));

  }

if (temperature < 2){
  ds.requestTemperatures();
    temperature = ds.getTempC(sensor1);
    kludas++;
    temperature = -1; 
 }
 else
 {kludas = 0;} 
  
//pot_raw = analogRead(15); 
pogas ();                           // pogas funkcijas
         
if (pot_raw>=30 && pot_raw<=50 && setti==false) {
       digitalWrite(TFT_BL, LOW);
       delay(500); 
       digitalWrite(TFT_BL, HIGH);
       
    } 


for (int i = 9; i > 0; i--) {
     raw2[i] = raw2[i-1];
   }

raw2[0]=pot_raw;   



/*     
    if(digitalRead(relayPort)==HIGH)
      {messageinfo = "VENTILATOR ON    ";}
      
    else 
      {messageinfo = "VENTILATOR OFF";}
    
         // read thermocouple temp in C */

 
  if (temperature >90)
    { tone(buzzerPort, buzzerRefillFrequency);
    delay(buzzerRefillDelay);
    noTone(buzzerPort);
    delay(buzzerRefillDelay);}
           
  
 
       
  //temp_iestat();

  /* int pow = digitalRead(relayPort);
  if(pow == LOW && temperature > targetTempC -2 && errI < endTrigger) {suknis = true;
   telnet.println("suknis iesledzas");
   digitalWrite(relayPort, HIGH);
   delay(18000);
    targetTempC = 69;
  }
         
  
if(temperature < targetTempC  && suknis==false && errI < endTrigger) {
    targetTempC = 76;
  digitalWrite(relayPort, LOW);
  telnet.println("NEuzsila");
}
else
{ 
telnet.println("uzsila");
}



if (suknis == true && errI > endTrigger ){
   digitalWrite(relayPort, LOW);
  telnet.println("suknis true");}

if (suknis == false ){telnet.println("suknis false");}
        */
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

        if(temperature>temperatureMin){
          refillTrigger = 1500;
          endTrigger = 2500;
        }
        
        //Refill Alarm
        if (errI > refillTrigger) 
          { messageDamp = "Fill";  
        }
        
        else {messageDamp = "AUTO";}

      }

    else {
        // End of combustion condition for errI >= endTrigger

        messageDamp = "End "; // set damper output message, end
       
      
        digitalWrite(TFT_BL, LOW);
       
/*  
              if (temperature > 75) {
          digitalWrite(relayPort, HIGH);
          delay(1500);
                    digitalWrite(relayPort, LOW);
          delay(1500);
     
          telnet.println("reset end");
        } */

      if (temperature < temperatureMin) {
          damper = zeroDamper;
          sleep_ = true;
          
        }

}
        
        }
    }
 
  

text4.createSprite(130, 40);
text4.fillSprite(TFT_WHITE);
  if (kludas > 20)
    {
      img.createSprite(200, 200);
      img.pushImage(0,0,200,200,kluda);
      text4.setTextColor(TFT_BLACK);

messageDamp = "Error"; 
 
  tone(buzzerPort, buzzerRefillFrequency);
    delay(buzzerRefillDelay);
    noTone(buzzerPort);
    delay(buzzerRefillDelay);
 
    }  
else
  {text4.setTextColor(TFT_BLACK);}


    if (oddLoop) 
    { messageinfo = "Dmp " + String(damper) + "% Pot " + round(pot);} // set alt damper output message


  
  
  
    if (pot_raw >= 155 && pot_raw <= 175) {
  delay(500);
  setti = !setti;
  delay(100);
  tft.fillScreen(TFT_WHITE);
}

// Button
if (setti) {

  img2.createSprite(220, 30);
  img2.fillSprite(TFT_TRANSPARENT);
  bckg.createSprite(320, 240);
  //bckg.fillSprite(TFT_WHITE);
  bckg.pushImage(0, 0, 320, 240, setting);
  text.createSprite(200, 30);
  text.fillSprite(TFT_WHITE);
  text.setTextColor(TFT_BLACK);
  text5.createSprite(200, 30);
  text5.fillSprite(TFT_WHITE);
  text5.setTextColor(TFT_BLACK);
  text2.createSprite(200, 30);
  text2.fillSprite(TFT_WHITE);
  text2.setTextColor(TFT_BLACK);
  text3.createSprite(200, 30);
  text3.fillSprite(TFT_WHITE);
  text3.setTextColor(TFT_BLACK);
  text5.setFreeFont(FSSB12);
  text6.createSprite(200, 30);
  text6.fillSprite(TFT_WHITE);
  text6.setTextColor(TFT_BLACK);
  text5.drawString("TargetTemp.  " + String(targetTempC), 0, 0, GFXFF);
  text5.pushToSprite(&bckg, 120, 10);
  text2.setFreeFont(FSSB12);
  text2.drawString("koificients  " + String(kP), 0, 0, GFXFF);
  text2.pushToSprite(&bckg, 120, 60);
  text3.setFreeFont(FSSB12);
  text3.drawString("Max damp.  " + String(maxDamperx), 0, 0, GFXFF);
  text3.pushToSprite(&bckg, 120, 110);
  text.setFreeFont(FSSB12);
  text.drawString("Damper  " + String(pot), 0, 0, GFXFF);
  text.pushToSprite(&bckg, 120, 160);
  text6.setFreeFont(FSSB12);
  if (suknis == false){text6.drawString("Suknis OFF  ", 0, 0, GFXFF);} 
  if (suknis == true){text6.drawString("Suknis ON  ", 0, 0, GFXFF);}
  text6.pushToSprite(&bckg, 120, 210);
  img2.drawRect(0, 0, 200, 30, 10);
  img2.pushToSprite(&bckg, 116, krx, TFT_TRANSPARENT);
  bckg.pushSprite(0, 0);
  text2.unloadFont();
  text3.unloadFont();
  text3.unloadFont();
  text5.unloadFont();
  text2.deleteSprite();
  text3.deleteSprite();
  text4.deleteSprite();
  img.deleteSprite();
  img2.deleteSprite();
  text5.deleteSprite();
  bckg.deleteSprite();
  old_setti = setti;
} else {

  bckg.createSprite(110, 170);
  bckg.fillSprite(TFT_WHITE);
  y = 105 - temperature;
  img2.createSprite(17, y);
  img2.fillSprite(TFT_WHITE);
  text.createSprite(195, 85);
  text.fillSprite(TFT_WHITE);
  text.setTextWrap(true);
  text.setTextColor(TFT_RED);
  text3.createSprite(200, 30);
  text3.fillSprite(TFT_WHITE);
  text3.setTextColor(TFT_BLACK);
  text2.createSprite(100, 50);
  text2.fillSprite(TFT_WHITE);
  text2.setTextColor(TFT_BLACK);
  bckg.pushImage(0, 0, 110, 170, arrow);
  y = 105 - temperature;
  img2.fillRect(0, 0, 17, y, TFT_BLACK);
  img2.pushToSprite(&bckg, 63, 14, TFT_WHITE);
  bckg.pushSprite(0, 10, TFT_BLACK);
  text.setFreeFont(&FreeSansBold42pt7b);
  text.drawString(String(temperature) + " c", 0, 0, GFXFF);
  text.pushSprite(120, 60);
  text2.setFreeFont(FF35);
  text2.drawString(String(damper) + " %", 0, 0, GFXFF);
  text2.pushSprite(190, 190);
  text3.drawString(String(targetTempC) + "°c   Target.", 0, 0, 4);
  text3.pushSprite(120, 20);
  img.pushSprite(90, 0, TFT_BLACK);
  text4.setFreeFont(FF23);
  text4.drawString(messageDamp, 0, 0, GFXFF);
  text4.pushSprite(20, 188);
}

text.unloadFont();
text2.unloadFont();
text3.unloadFont();
text5.unloadFont();
text.deleteSprite();
text2.deleteSprite();
text3.deleteSprite();
text4.deleteSprite();
img.deleteSprite();
img2.deleteSprite();
text5.deleteSprite();
bckg.deleteSprite();
   
 

    // Drive servo and print damper position to the lcd
if (currentMillis - lastExecutedMillis >= EXE_INTERVAL){
diff = damper - oldDamper;
    
  if (abs(diff) > 8) {  // move servo only if damper position change is more than 2%
      if (!oddLoop) {
        //gfx->println("x");   // print x after damper message to indicate active movement
      }
      delay(50);
      myservo.attach(5);
      if (diff > 0) {  // # Perform the specified action if the damper should be moved in the opened direction



        for (int i = 0; i < diff; i++) {
          angle = (oldDamper + i + 1) * servoAngle / (maxDamperx * servoCalibration) + servoOffset;
          myservo.write(angle);
          delay(50);
        }}
         
  if (diff < 0) {  // action if damper should be moved in the closed direction
        for (int i = 0; i < abs(diff); i++) {
          angle = (oldDamper - i - 1) * servoAngle / (maxDamperx * servoCalibration) + servoOffset;
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
      telnet.println("krx156:" + String(krx156) +" |  " + "temperatura:" + String(temperature) + " | " + "potrav:" + String(pot_raw));

    delay(200);

  
}



