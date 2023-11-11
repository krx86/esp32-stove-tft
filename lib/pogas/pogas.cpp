#include "pogas.h"

extern int pot_raw;
extern int pot;
extern int relayPort;


void pogas (){
    
    pot_raw = analogRead(15); 
    
     if (pot_raw>=0 &&  pot_raw<=10)     //up
        {pot = pot +10;
        delay(800);}
    
    else if 
    (pot_raw>=12 && pot_raw<=25)     //right 
      {digitalWrite(relayPort, LOW);
      delay(800);}
     
    else if (pot_raw>=30 && pot_raw<=50)     //left
      {digitalWrite(relayPort, HIGH);
      delay(90000);
      }
  
     else if 
        (pot_raw>=70 && pot_raw<=85)     //down
        {pot = pot -10;
        delay(800);}

    else if (pot_raw>=140 && pot_raw<=180)
      { ESP.restart(); }

    else {
        digitalWrite(relayPort, LOW);
    }
}