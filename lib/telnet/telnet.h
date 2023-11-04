#pragma once

#include "Arduino.h"
#include "ESPTelnet.h"
//#include "WiFi.h"

#define SERIAL_SPEED    115200

ESPTelnet telnet;
IPAddress ip;
uint16_t  port = 23;

void setupSerial(String ip);
void onTelnetConnect(String ip);
void onTelnetDisconnect(String ip);
void onTelnetReconnect(String ip);
void onTelnetConnectionAttempt(String ip);
void onTelnetInput(String ip);
void setupTelnet(String ip);