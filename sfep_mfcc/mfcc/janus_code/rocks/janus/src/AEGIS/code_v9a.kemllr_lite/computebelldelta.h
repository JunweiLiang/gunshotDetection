#ifndef COMPUTE_BELL_DELTA_H
#define COMPUTE_BELL_DELTA_H

static float deltaW[] = {-0.7488,-0.3744,0,0.3744,0.7488};   // delta window
static float accW[] = {-0.375,0,0.375};   // acc window

#define DELTA_WIN_CONTEXT (sizeof(deltaW)/sizeof(float)-1)/2
#define DELTA_WIN_LEN  sizeof(deltaW)/sizeof(float)

#define ACC_WIN_CONTEXT (sizeof(accW)/sizeof(float)-1)/2



/* Imported from Brian */

// Regression coefficients for delta delta cep w.r.t. cep
static float accel_window[] = 
    { 0.2808, 0.1404, -0.2808, -0.2808, -0.2808, 0.1404, 0.2808};

#define ACCEL_WIN_LEN  sizeof(accel_window)/sizeof(float)
#define ACCEL_WIN_CONTEXT (sizeof(accel_window)/sizeof(float)-1)/2

//#define DELTA_WIN_OFFSET  DELTA_WIN_LEN/2
//#define ACCEL_WIN_OFFSET  ACCEL_WIN_LEN/2

/* End */

/*
*  startSubVectorIdx means the input starting cepstrum index
*/
void ComputeBellDynamicFeature(float **allCep, int T, int startSubVectorIdx, int subVectorDim, const float *weight, int context, bool computeAcc);

void Compute_Dynamic_Feature(
	 float **allCep,
	 int T,
	 int input_vec_idx,
	 int output_vec_idx,
	 int subVectorDim,
	 const float *weight,
	 int context);

/*
*  obey the vector format in bell lab
*  <CEP + dCEP + ddCEP + 0.1dE + 0.1ddE + 0.1E>
*  The original vector format is <C1-C12 + E, Delta, Acc> (HTK format)
*/
void HTKCep39_2_BellCep39(float **inCep, int T, int D);

/*
* Scale the energy term by 0.01 on cep[0], cep[13] and cep[26] which are energy terms
*/
//void Bell_Energy_Scaling(float **cep, int T, int D);
void Bell_Energy_Scaling(float **cep, int T, int D, float energyScale=0.1);

#endif
