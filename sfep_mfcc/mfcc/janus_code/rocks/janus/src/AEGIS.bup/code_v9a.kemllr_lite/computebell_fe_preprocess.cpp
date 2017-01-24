#include "util_new.h"

void ENormalize(float **allCep, int T, int energyPos, float energy_offset)
{
   // perform E-normalization
   float max_e = LZERO;
   float cur_e = LZERO;
   float *cur_e_pos = 0;
   int t;

   for (t=0;t<T;t++) {
      cur_e = allCep[t][energyPos];
      if (max_e < cur_e) {
         max_e = cur_e;
      }
   }

   if (max_e < LSMALL) {
      printf("can't locate max energy!\n");
      exit(-1);
   }

#ifdef COMPUTEBELL_FE_PREPROCESS_DEBUG
   printf("Max energy = %f\n", max_e);
#endif

   // subtract all energy terms by the max energy
   for (t=0;t<T;t++) {
      cur_e_pos = allCep[t] + energyPos;

      // minus the max energy
      *cur_e_pos -= max_e;

      // plus an offset
      *cur_e_pos += energy_offset;
   }
}

void CMS(float **allCep, int T, int staticCepStartPos, int dimension)
{
   int t,d;
   int vectorEnd = staticCepStartPos + dimension;

   // compute the time average cepstral value for each dimension
   for (d=staticCepStartPos;d<vectorEnd;d++) {
      float avg = 0.0;

      // get the average
      for (t=0;t<T;t++) {
         avg += allCep[t][d];
      }
      avg /= (float)T;

      // perform CMS
      for (t=0;t<T;t++) {
         allCep[t][d] -= avg;
      }
#ifdef COMPUTEBELL_FE_PREPROCESS_DEBUG
      cout << "CMS of dimension " << d << " = " << avg << endl;
#endif
   }
}
