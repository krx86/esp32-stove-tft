#pragma once

#include "Arduino.h"
#include <ESP32Servo.h>
#include "arrow.h"
#include <OneWire.h>
#include <DallasTemperature.h>
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
#include "kluda.h"
#include <WebServer.h>



const char* host = "esp32";
const char* ssid = "HUAWEI-B525-90C8";
const char* password = "BTT6F1EA171";

#define GFXFF 1

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

int temperature = 0;       // initialize temperature variable for C
int temperatureMin = 50; // under this temperature (38C = 100F), the regulation closes the damper if end of fire conditions are met
int targetTempC = 0;   // the target temperature as measured by the thermocouple (135 C = 275 F)
float errP = 0.0;          // initialize the proportional term
float errD = 0.0;          // initialize the derivative term
float errI = 0.0;          // initialize the integral term
float errOld = 0.0;        // initialize term used to capture prior cycle ErrP
int kP = 0;            // Overall gain coefficient and P coefficient of the PID regulation
float tauI = 1000;         // Integral time constant (sec/repeat)
float tauD = 5;            // Derivative time constant (sec/reapeat)
float kI =  kP/tauI;        // I coefficient of the PID regulation
float kD = kP/tauD;  


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
int relayPort = 13;

// Servo calibration settings
float servoCalibration = 1.5;  // 1.0 is neutral cal - adjust value so servo arm drives closed damper when damper variable equals 0 (0%)
float servoOffset = 29;  // offset angle for servo angle adjustment
float servoAngle = 35;  // adjust value to define total angular travel of servo so that cable drives damper from fully opened to fully closed


      // D coefficient of the PID regulation

float refillTrigger = 20000;// refillTrigger used to notify need of a wood refill
float endTrigger = 45000;  // closeTrigger used to close damper at end of combustion

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

int y = 0;
int z = 0;
int kludas = 0;

Servo myservo;

String messageDamp;    // Initialize message for damper 2
String messageinfo;    // Initialize message for damper 3
bool endBuzzer = true;
bool refillBuzzer = true;
bool oddLoop = true;
bool sleep_ = false;

#define EXE_INTERVAL 5000

unsigned long lastExecutedMillis = 0;

int TempHist[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Set temperature history array

void handle_NotFound();