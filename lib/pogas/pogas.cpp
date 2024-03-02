#include "pogas.h"

extern int pot_raw;
extern int pot;
extern int relayPort;
extern int krx;
extern bool setti;
extern int kP;
extern int krx6;
extern int krx56;
extern int targetTempC;
extern int maxDamperx;



void pogas (){
    
    pot_raw = analogRead(9); 
    krx6=krx56;

    if(setti == true){



  

   if(pot_raw>=70 && pot_raw<=85)     //down
        {
      delay(500);
    krx=krx + 50;
    krx6=krx6 +50;  
      
      if (krx>250){krx=6;}
      
      }

 if (pot_raw>=0 &&  pot_raw<=7){
          krx=krx - 50;
             krx6=krx6 - 50;  
      delay(500);
      if (krx<6){krx=206;}
    }
    
    
    if (krx6==0){
        if (pot_raw>=30 && pot_raw<=50) {
        targetTempC--;
        if(targetTempC<60){targetTempC=60;}
        }   
    
    if (pot_raw>=8 && pot_raw<=20){
      targetTempC++;
      if(targetTempC>80){targetTempC=80;}
    }
      } 
      
      if (krx6==50){
             if (pot_raw>=30 && pot_raw<=50) {
        kP--;}   
    
    else if (pot_raw>=8 && pot_raw<=20){
      kP++;
    }
      }
      
     else if (krx6==100){
        if (pot_raw>=30 && pot_raw<=50) {
        maxDamperx=maxDamperx-10;}   
    
    if (pot_raw>=8 && pot_raw<=20){
      maxDamperx=maxDamperx+10;
    }
      }
      
        else if (krx6==150){
        if (pot_raw>=30 && pot_raw<=50) {
        pot=pot-10;
              delay(500);
        if(pot<0){pot=0;}
        }   
    
    if (pot_raw>=8 && pot_raw<=20){
      pot=pot+10;
            delay(500);
      if(pot>110){pot=110;}
    }
      }

       else if (krx6==200){
        if (pot_raw>=30 && pot_raw<=50) {
        delay(200);}   
    
    if (pot_raw>=8 && pot_raw<=20){
    
      delay(200);
    }
      } 
      else {krx6 =0;}
    
   
    
    }
   
krx56=krx6;

if(setti==false) {

     if (pot_raw>=0 &&  pot_raw<=7)     //up
        {targetTempC++;
        if(targetTempC>80){targetTempC=80;}
        delay(500);}

     
    if (pot_raw>=8 && pot_raw<=20)     //right
      { ESP.restart(); 
      }
  
    if 
        (pot_raw>=70 && pot_raw<=85)     //down
        {targetTempC--;
        if(targetTempC<60){targetTempC=60;}
        delay(500);}

    
   //left

    
}      


}


  
      
      
    
   
       




    
    
    
