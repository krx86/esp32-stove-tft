#include "temp_iestat.h"
#include "pogas.h"

extern int temperature;
extern int targetTempC;
extern int pot;
extern int kP;


void temp_iestat () {
    
 if (pot <= 100) 
    {targetTempC = 65; 
      if (temperature > 0 && temperature < 40) {kP = 2;}
      if (temperature > 41 && temperature < 54) {kP = 4;}
      if (temperature > 55 && temperature < 62) {kP = 7;}
      if (temperature > 63 && temperature < 100) {kP = 10;}}       
  
    if (pot == 110) {
      targetTempC =64; 
      kP = 37; }
   
    if (pot == 120) {
        targetTempC = 67; 
       kP =35; } 
    
    if (pot == 130) {
      targetTempC =73; 
      kP = 30;}

      if (pot == 140) {
      targetTempC = 75; 
      if (temperature > 0 && temperature < 50) {kP = 2;}
      if (temperature > 51 && temperature < 55) {kP = 4;}
      if (temperature > 56 && temperature < 60) {kP = 6;}
      if (temperature > 61 && temperature < 65) {kP = 7;}
      if (temperature > 66 && temperature < 100) {kP = 9;}}
        
  if (pot == 150) {
      targetTempC = 79; 
      kP =25;}
      
}