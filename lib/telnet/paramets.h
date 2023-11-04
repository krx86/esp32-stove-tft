#pragma once
#include "Arduino.h"
#include <ESP32Servo.h>

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