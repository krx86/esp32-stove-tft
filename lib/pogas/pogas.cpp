#include "pogas.h"

extern int pot_raw;
extern int pot;
extern int relayPort;
extern int krx;
extern bool setti;
extern int kP;
extern int krx6;
extern int krx56;
extern int krx156;
extern int targetTempC;
extern int maxDamperx;
extern bool suknis;

// Adjust the target temperature within bounds
void adjustTempC(int step) {
    targetTempC += step;
    targetTempC = constrain(targetTempC, 60, 80); // Constrain to [60, 80]
}

// Adjust a value with bounds and wrap-around behavior
void adjustValue(int &value, int step, int minVal, int maxVal) {
    value += step;
    if (value > maxVal) value = minVal; // Wrap around if exceeding max
    if (value < minVal) value = maxVal; // Wrap around if below min
}

// Toggle the relay state based on suknis
void toggleRelay() {
    suknis = !suknis;
    digitalWrite(relayPort, suknis ? HIGH : LOW); // Use relayPort variable
}

// Main control logic based on potentiometer readings
void pogas() {
    pot_raw = analogRead(9);
    krx6 = krx56; // Update krx6 from krx56

    if (setti) {
        if (pot_raw >= 70 && pot_raw <= 85) {  // Down
            delay(500);
            adjustValue(krx, 50, 6, 250);
            adjustValue(krx6, 50, 6, 500);
            adjustValue(krx156, 50, 6, 500);
        } 
        else if (pot_raw >= 0 && pot_raw <= 7) {  // Up
            delay(500);
            adjustValue(krx, -50, 6, 206);
            krx6 -= 50;
        }

        // Adjust settings based on krx6 value
        if (krx6 == 0) {
            adjustTempC(pot_raw >= 30 && pot_raw <= 50 ? -1 : (pot_raw >= 8 && pot_raw <= 20 ? 1 : 0));
        } 
        else if (krx6 == 50) {
            kP += (pot_raw >= 30 && pot_raw <= 50 ? -1 : (pot_raw >= 8 && pot_raw <= 20 ? 1 : 0));
        } 
        else if (krx6 == 100) {
            maxDamperx += (pot_raw >= 30 && pot_raw <= 50 ? -10 : (pot_raw >= 8 && pot_raw <= 20 ? 10 : 0));
        } 
        else if (krx6 == 150) {
            if (pot_raw >= 30 && pot_raw <= 50) {
                delay(500);
                pot = max(0, pot - 10);
            } 
            else if (pot_raw >= 8 && pot_raw <= 20) {
                delay(500);
                pot = min(110, pot + 10);
            }
        } 
        else if (krx6 == 200) {
            toggleRelay();
        } 
        else {
            krx6 = 0; // Reset if krx6 is not handled
        }
    } 
    else {  // setti == false
        if (pot_raw >= 0 && pot_raw <= 7) {  // Up
            adjustTempC(1);
            delay(500);
        } 
        else if (pot_raw >= 8 && pot_raw <= 20) {  // Right
            ESP.restart(); // Restart the ESP
        } 
        else if (pot_raw >= 70 && pot_raw <= 85) {  // Down
            adjustTempC(-1);
            delay(500);
        }
    }

    krx56 = krx6; // Update krx56 with the current krx6 value
}