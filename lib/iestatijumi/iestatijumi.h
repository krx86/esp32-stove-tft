#pragma once

#include "Arduino.h"
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
#include <WebServer.h>


#define GFXFF 1


const char* host = "esp32";
const char* ssid = "HUAWEI-B525-90C8";
const char* password = "BTT6F1EA171";

WebServer server(80);   

  

OneWire oneWire(6);
DallasTemperature ds(&oneWire);
DeviceAddress sensor1 = {0x28, 0xFC, 0x70, 0x96, 0xF0, 0x01, 0x3C, 0xC0};

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
TFT_eSprite img = TFT_eSprite(&tft);
TFT_eSprite bckg = TFT_eSprite(&tft);
TFT_eSprite text = TFT_eSprite(&tft);
TFT_eSprite text2 = TFT_eSprite(&tft);
TFT_eSprite text3 = TFT_eSprite(&tft);
TFT_eSprite text4 = TFT_eSprite(&tft);
TFT_eSprite img2 = TFT_eSprite(&tft);
TFT_eSprite text5 = TFT_eSprite(&tft);
TFT_eSprite bckg2 = TFT_eSprite(&tft);
TFT_eSprite text6 = TFT_eSprite(&tft);


float error = 0.0;                  // Temperature compensation error
int t=0;
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

// Device objects - create servo, therocouple, and lcd objects 
Servo myservo;

// Servo calibration settings
float servoCalibration = 1.5;  // 1.0 is neutral cal - adjust value so servo arm drives closed damper when damper variable equals 0 (0%)
float servoOffset = 29;  // offset angle for servo angle adjustment
float servoAngle = 35;  // adjust value to define total angular travel of servo so that cable drives damper from fully opened to fully closed


int temperature = 0;       // initialize temperature variable for C
int temperatureMin = 55; // under this temperature (38C = 100F), the regulation closes the damper if end of fire conditions are met
int targetTempC = 69;   // the target temperature as measured by the thermocouple (135 C = 275 F)
float errP = 0.0;          // initialize the proportional term
float errD = 0.0;          // initialize the derivative term
float errI = 0.0;          // initialize the integral term
float errOld = 0.0;        // initialize term used to capture prior cycle ErrP
int kP = 30;            // Overall gain coefficient and P coefficient of the PID regulation
float tauI = 1000;         // Integral time constant (sec/repeat)
float tauD = 5;            // Derivative time constant (sec/reapeat)
float kI =  kP/tauI;        // I coefficient of the PID regulation
float kD = kP/tauD;        // D coefficient of the PID regulation

float refillTrigger = 6000;// refillTrigger used to notify need of a wood refill
float endTrigger = 9000;  // closeTrigger used to close damper at end of combustion

int pot_raw = 0;
int pot = 110;
int relayPort = 13;

int krx =6;
int krx156 =6;


int oldPot = 0;
float potMax = 1000.0;   // Potentiometer calibration
int potRelMax = 100;     // Potentiometer value above which the regulator runs in automatic mode

int difft = 0;
int angle = 0;
int damper = 0;
int oldDamper = 0;
int diff = 0;
int maxDamper = 100;  // Sets maximum damper setting
int maxDamperx = 100; 
float minDamper = 0.0;    // Sets minimum damper setting
float zeroDamper = 0.0;   // Sets zero damper setting - note that stove allows some amount of airflow at zero damper

int y = 0;
int z = 0;
int kludas = 0;


String messageDamp;    // Initialize message for damper 2
String messageinfo;    // Initialize message for damper 3
bool endBuzzer = true;
bool refillBuzzer = true;
bool oddLoop = true;
bool sleep_ = false;



#define EXE_INTERVAL 5000

unsigned long lastExecutedMillis = 0;

String SendHTML(int temperature) {
    String ptr = "<!DOCTYPE html> <html>\n";
    ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
    ptr += "<meta charset=\"UTF-8\">\n";
    ptr += "<title>ESP32 Temperatūras Monitors</title>\n";
    ptr += "<style>\n";
    ptr += "html { font-family: 'Arial', sans-serif; display: inline-block; margin: 0px auto; text-align: center; background: #87CEFA; color: #ffffff; transition: background 0.5s ease-in-out; }\n"; // Fona krāsa: debesu zils (#87CEFA)
    ptr += "body { margin-top: 50px; }\n";
    ptr += "h1 { color: #ffffff; margin: 50px auto 30px; text-shadow: 2px 2px 4px #000000; transform: scale(0.95); transition: transform 0.3s; }\n";
    ptr += "h1:hover { transform: scale(1); }\n";
    ptr += "p { font-size: 24px; color: #ffffff; margin-bottom: 20px; transform: scale(0.95); transition: transform 0.3s; cursor: pointer; }\n";
    ptr += "p:hover { transform: scale(1); }\n";
    ptr += ".value { font-weight: bold; font-size: 30px; color: #ffdd57; transition: color 0.5s ease-in-out; }\n";
    ptr += "input { font-size: 18px; width: 100px; padding: 10px; margin: 10px; border: none; border-radius: 5px; box-shadow: 2px 2px 10px rgba(0, 0, 0, 0.3); }\n";
    ptr += "input[type=\"submit\"] { font-size: 18px; padding: 10px 20px; background: #ff6347; color: white; border-radius: 5px; cursor: pointer; transition: background 0.3s, transform 0.3s; }\n";
    ptr += "input[type=\"submit\"]:hover { background: #ff4500; transform: scale(1.05); }\n";
    ptr += ".modal { display: none; position: fixed; top: 0; left: 0; width: 100%; height: 100%; background-color: rgba(0, 0, 0, 0.5); justify-content: center; align-items: center; }\n";
    ptr += ".modal-content { background-color: #ffffff; padding: 20px; border-radius: 10px; box-shadow: 0 5px 15px rgba(0, 0, 0, 0.3); }\n";
    ptr += ".modal-content input[type='number'] { width: 60px; padding: 5px; }\n";
    ptr += ".close-btn { background: #ff6347; color: white; border: none; padding: 10px; border-radius: 5px; cursor: pointer; margin-top: 10px; transition: background 0.3s; }\n";
    ptr += ".close-btn:hover { background: #ff4500; }\n";
    ptr += "</style>\n";
    ptr += "<script>\n";
    ptr += "function updateData() {\n";
    ptr += "  fetch('/temperature').then(response => response.text()).then(data => {\n";
    ptr += "    const temperatureElement = document.getElementById('temperature');\n";
    ptr += "    if (temperatureElement.innerText !== data) {\n";
    ptr += "      temperatureElement.innerText = data;\n";
    ptr += "      temperatureElement.style.color = '#00ff00';\n";
    ptr += "      setTimeout(() => temperatureElement.style.color = '#ffdd57', 500);\n";
    ptr += "    }\n";
    ptr += "  });\n";
    ptr += "  fetch('/damper').then(response => response.text()).then(data => {\n";
    ptr += "    document.getElementById('damper').innerText = data;\n";
    ptr += "  });\n";
    ptr += "  fetch('/target_temp').then(response => response.text()).then(data => {\n";
    ptr += "    document.getElementById('targetTempC').innerText = data;\n";
    ptr += "  });\n";
    ptr += "  fetch('/kP_value').then(response => response.text()).then(data => {\n";
    ptr += "    document.getElementById('kP').innerText = data;\n";
    ptr += "  });\n";
    ptr += "} \n";
    ptr += "setInterval(updateData, 1000);\n";

    // Funkcija modālā loga atvēršanai
    ptr += "function openModal(modalId) {\n";
    ptr += "  document.getElementById(modalId).style.display = 'flex';\n";
    ptr += "}\n";

    // Funkcija modālā loga aizvēršanai
    ptr += "function closeModal(modalId) {\n";
    ptr += "  document.getElementById(modalId).style.display = 'none';\n";
    ptr += "}\n";
    ptr += "</script>\n";
    ptr += "</head>\n";
    ptr += "<body>\n";

    // Temperatūras un kP vērtības klikšķināmi elementi
    ptr += "<div id=\"webpage\">\n";
    ptr += "<h1>ESP32 Temperatūras Monitors</h1>\n";
    ptr += "<p>Ūdens Temperatūra: <span id=\"temperature\" class=\"value\">";
    ptr += temperature;
    ptr += "</span> °C</p>\n";
    ptr += "<p onclick=\"openModal('modalTargetTemp')\">Mērķa Temperatūra: <span id=\"targetTempC\" class=\"value\">";
    ptr += targetTempC;
    ptr += "</span> °C</p>\n";
    ptr += "<p>Slāpētāja Atvērums: <span id=\"damper\" class=\"value\">";
    ptr += damper;
    ptr += "</span> %</p>\n";
    ptr += "<p onclick=\"openModal('modalKP')\">kP Vērtība: <span id=\"kP\" class=\"value\">";
    ptr += kP;
    ptr += "</span></p>\n";

    // Slāpētāja režīma attēlošana
    if (potRelMax < 100) {
        ptr += "<p>Slāpētāja Režīms: <span class=\"value\">Manuāls</span></p>\n";
    } else {
        ptr += "<p>Slāpētāja Režīms: <span class=\"value\">Automātisks</span></p>\n";
    }

    // Modālais logs mērķa temperatūras maiņai
    ptr += "<div id=\"modalTargetTemp\" class=\"modal\">\n";
    ptr += "  <div class=\"modal-content\">\n";
    ptr += "    <h3>Mainīt Mērķa Temperatūru</h3>\n";
    ptr += "    <form action=\"/set_target_temp\" method=\"POST\">\n";
    ptr += "      <input type=\"number\" name=\"target_temp\" value=\"" + String(targetTempC) + "\" min=\"60\" max=\"80\" step=\"1\">\n";
    ptr += "      <input type=\"submit\" value=\"Saglabāt\">\n";
    ptr += "    </form>\n";
    ptr += "    <button class=\"close-btn\" onclick=\"closeModal('modalTargetTemp')\">Aizvērt</button>\n";
    ptr += "  </div>\n";
    ptr += "</div>\n";

    // Modālais logs kP vērtības maiņai
    ptr += "<div id=\"modalKP\" class=\"modal\">\n";
    ptr += "  <div class=\"modal-content\">\n";
    ptr += "    <h3>Mainīt kP Vērtību</h3>\n";
    ptr += "    <form action=\"/set_kP\" method=\"POST\">\n";
    ptr += "      <input type=\"number\" name=\"kP_value\" value=\"" + String(kP) + "\" min=\"1\" max=\"100\" step=\"1\">\n";
    ptr += "      <input type=\"submit\" value=\"Saglabāt\">\n";
    ptr += "    </form>\n";
    ptr += "    <button class=\"close-btn\" onclick=\"closeModal('modalKP')\">Aizvērt</button>\n";
    ptr += "  </div>\n";
    ptr += "</div>\n";

    ptr += "</div>\n";
    ptr += "</body>\n";
    ptr += "</html>\n";
    return ptr;
}


void handle_OnConnect() {
    server.send(200, "text/html; charset=UTF-8", SendHTML(temperature));
}

void handle_SetTargetTemp() {
    if (server.hasArg("target_temp")) {
        targetTempC = server.arg("target_temp").toInt(); // Update targetTempC based on user input
    }
    server.sendHeader("Location", "/"); // Redirect to the main page
    server.send(303); // Send a 303 response to redirect
}

void handle_SetkP() {
    if (server.hasArg("kP_value")) {
        kP = server.arg("kP_value").toInt(); // Update kP based on user input

    }
    server.sendHeader("Location", "/"); // Redirect to the main page
    server.send(303); // Send a 303 response to redirect
}



void handle_GetTemperature() {
    ds.requestTemperatures();
    temperature = ds.getTempC(sensor1); // Gets the values of the temperature
    server.send(200, "text/html; charset=UTF-8" , String(temperature));
}

void handle_GetDamper() {
    server.send(200, "text/html; charset=UTF-8", String(damper));
}

void handle_GetTargetTemp() {
    server.send(200, "text/html; charset=UTF-8", String(targetTempC));
}

