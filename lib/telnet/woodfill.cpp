#include "woodfill.h"

bool WoodFilled() {
   for (int i = 9; i > 0; i--) {
     TempHist[i] = TempHist[i-1];
   }
   TempHist[0] = CurrentTemp;

   if (float((TempHist[0]+TempHist[1]+TempHist[2]+TempHist[3]+TempHist[4]+TempHist[5])/5) > float((TempHist[6]+TempHist[7]+TempHist[8])+TempHist[9]+TempHist[10]/5)) {
     return true;
  }
   else {
    return false;
   }
 }