#include "util_new.h"

float LogAdd(float lx, float ly) {
   static float minLogExp = -log(-LZERO);
   float temp,diff,z;

   // swap
   if (lx<ly) {
      temp = lx; lx = ly; ly = temp;
   }
   diff = ly-lx;
   if (diff<minLogExp)
      return  (lx<LSMALL)?LZERO:lx;
   else {
      z = exp(diff);
      return lx+log(1.0+z);
   }
}

double addLogProbs2 (double p1,double p2) {
  if (p1 == 0.0) return p2;
  if (p2 == 0.0) return p1;
  if (p1 < p2) {
    return p1 - log1p(exp(p1-p2));
  } else {
    return p2 - log1p(exp(p2-p1));
  }
}


void swapShort(char *val) {
   char tmpV;

   tmpV = val[0];
   val[0] = val[1];
   val[1] = tmpV;
}


void swapLong(char *val) {
   char tmpV;

   tmpV = val[0];
   val[0] = val[3];
   val[3] = tmpV;

   tmpV = val[1];
   val[1] = val[2];
   val[2] = tmpV;
}

