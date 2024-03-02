#include "temp_iestat.h"
#include "pogas.h"

extern int temperature;
extern int targetTempC;
extern int pot;
extern int kP;


void temp_iestat () {
    
 if (pot <= 100) 
    {targetTempC = 65;}       
  
    if (pot == 110) {
      targetTempC =64;}
   
    if (pot == 120) {
        targetTempC = 69; 
      } 
    
    if (pot == 130) {
      targetTempC =73; 
    }

      if (pot == 140) {
      targetTempC = 75; 
  }
        
  if (pot == 150) {
      targetTempC = 79; 
  }
      
}