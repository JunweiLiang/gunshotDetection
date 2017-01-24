#ifndef COMPUTE_BELL_FE_PREPROCESS_H
#define COMPUTE_BELL_FE_PREPROCESS_H

/*
*  Energy normalization
*/ 
void ENormalize(float **allCep, int T, int energyPos, float energy_offset = 0.0);

/*
*  CMS
*/
void CMS(float **allCep, int T, int staticCepStartPos, int dimension);

#endif
