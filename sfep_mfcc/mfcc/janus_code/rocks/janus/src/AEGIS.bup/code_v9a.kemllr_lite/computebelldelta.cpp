#include "util_new.h"
#include "computebelldelta.h"
#include <vector>

using namespace std;

/*
*  Handle beginning boundary of delta delta feature
*/
static void ComputeDeltaMinus1(float **allCep, float *outCep, int startSubVectorIdx, int subVectorDim, const float *weight, int context)
{
   float *cep_minus_3 = allCep[0] + startSubVectorIdx;
   float *cep_plus_1 = allCep[1] + startSubVectorIdx;
   const float *w = weight + context;

   // init outCep values
   for (int d=0;d<subVectorDim;d++) {
      outCep[d] = cep_minus_3[d] * w[-context] + cep_plus_1[d] * w[context];
   }
}

/*
*  Handle ending boundary of delta delta feature
*/
static void ComputeDeltaTplus1(float **allCep, int T, float *outCep, int startSubVectorIdx, int subVectorDim, const float *weight, int context)
{
   float *cep_t_minus_1 = allCep[T-2] + startSubVectorIdx;
   float *cep_t_plus_3 = allCep[T-1] + startSubVectorIdx;
   const float *w = weight + context;

   // init outCep values
   for (int d=0;d<subVectorDim;d++) {
      outCep[d] = cep_t_minus_1[d] * w[-context] + cep_t_plus_3[d] * w[context];
   }
}


/*
From BELL LAB:
delta-cepstrum is defined as:
                 dc_t^i = \sum^2_{k=-2} w_k C^i_t+k
                 Wk = {-0.7488 -0.3744 0 0.3744 0.7488}
*/

/*
*  allCep is a T x D matrix
*/
void ComputeBellDynamicFeature(float **allCep,
	 int T,
	 int startSubVectorIdx,
	 int subVectorDim,
	 const float *weight,
	 int context,
	 bool computeAcc)
{
   float *inCep = 0, *outCep = 0;
   int t,d,k;
   int t_plus_k = 0;
   const int T_minus_1 = T - 1;
   const float *w = weight + context;

   // generate delta cepstrum for each time t
   for (t=0;t<T;t++) {
      // obtain an output sub-vector at time t
      outCep = allCep[t] + startSubVectorIdx + subVectorDim;  

      // init outCep values
      for (d=0;d<subVectorDim;d++) {
         outCep[d] = 0.0;
      }

      // within the context window
      for (k=-context;k<=context;k++) {
         // get current vector
         t_plus_k = t + k;
	 if (t_plus_k < 0) {
	    // handle begin boundary
            inCep  = allCep[0] + startSubVectorIdx;  
	 } else if (t_plus_k >= T) {
	    // handle end boundary
            inCep = allCep[T_minus_1] + startSubVectorIdx;  
	 } else {
            inCep = allCep[t_plus_k] + startSubVectorIdx;  
	 }

	 // Perform linear combination for each dimension
	 for (d=0;d<subVectorDim;d++) {
	    outCep[d] += w[k] * inCep[d];
	 }
      } // end context k
   }

   // special case handling
   if (computeAcc) {
      if (context != 1){
          printf("Acc cepstrum special case handling error!\n");
          exit(-1);
      }
      /* calculate D(-1) */
      int d;
      static float deltaCep_minus_1[13];
      const float *deltaCep_plus_1 = allCep[1] + startSubVectorIdx;

      ComputeDeltaMinus1(allCep,deltaCep_minus_1,0,subVectorDim,deltaW,DELTA_WIN_CONTEXT);

      // DD(0) = D(-1) * w(-1) + D(1) * w(1)

      // output vector
      outCep = allCep[0] + startSubVectorIdx + subVectorDim;  

      for (d=0;d<subVectorDim;d++) {
         outCep[d] = deltaCep_minus_1[d] * w[-context] + deltaCep_plus_1[d] * w[context];
      }
/*********************************************************************/

/* calculate D(T) */
      static float deltaCep_t_plus_1[13];
      const float *deltaCep_t_minus_1 = allCep[T-2] + startSubVectorIdx;

      ComputeDeltaTplus1(allCep,T,deltaCep_t_plus_1,0,subVectorDim,deltaW,DELTA_WIN_CONTEXT);

      // DD(T) = D(T-1) * w(-1) + D(T+1) * w(1)

      // output vector
      outCep = allCep[T-1] + startSubVectorIdx + subVectorDim;  

      for (d=0;d<subVectorDim;d++) {
         outCep[d] = deltaCep_t_minus_1[d] * w[-context] + deltaCep_t_plus_1[d] * w[context];
      }
/*********************************************************************/
   }
}

void HTKCep39_2_BellCep39(float **cep, int T, int D)
{
   if (D != 39){
      printf("Dimension != 39\n");
      exit(-1);
   }

   int t,d;
   const float energyScale = 0.1;
   vector<float>   inVec(D,0.0);
   float *outVec = 0;

   // resize vector buffer
   inVec.resize(D,0.0);

   for (t=0;t<T;t++) {
      // get output vector
      outVec = cep[t];

      // copy input vector at time t
      for (d=0;d<D;d++) {
         inVec[d] = outVec[d];
      }
      
      // modify
      for (d=1;d<=12;d++) {
         outVec[d-1] = inVec[d];
      }

      // copy delta cepstrum
      for (d=14;d<=25;d++) {
         outVec[d-2] = inVec[d];
      }

      // copy acc cepstrum
      for (d=27;d<=38;d++) {
         outVec[d-3] = inVec[d];
      }

      // copy energy term
      outVec[38] = energyScale * inVec[0];

      // copy delta energy term
      outVec[36] = energyScale * inVec[13];

      // copy acc energy term
      outVec[37] = energyScale * inVec[26];
   }
}

void Bell_Energy_Scaling(float **cep, int T, int D, float energyScale)
{
   //if (D != 39) throw Exception("Dimension != 39");

   //const float energyScale = 0.1;
   float *outVec = 0;

   for (int t=0;t<T;t++) {
      // get output vector
      outVec = cep[t];

      outVec[0] *= energyScale;

      // delta cep
      if (D > 13)
         outVec[13] *= energyScale;

      // delta delta cep
      if (D > 26)
         outVec[26] *= energyScale;
   }
}

/*
*  allCep is a T x D matrix
*/
void Compute_Dynamic_Feature(
	 float **allCep,
	 int T,
	 int input_vec_idx,
	 int output_vec_idx,
	 int subVectorDim,
	 const float *weight,
	 int context)
{
   float *inCep = 0, *outCep = 0;
   int t,d,k;
   int t_plus_k = 0;
   const int T_minus_1 = T - 1;
   const float *w = weight + context;

   // generate delta cepstrum for each time t
   for (t=0;t<T;t++) {
      // obtain an output sub-vector at time t
      outCep = allCep[t] + output_vec_idx;  

      // init outCep values
      for (d=0;d<subVectorDim;d++) {
         outCep[d] = 0.0;
      }

      // within the context window
      for (k=-context;k<=context;k++) {
         // get current vector
         t_plus_k = t + k;
	 if (t_plus_k < 0) {
	    // handle begin boundary
            inCep  = allCep[0] + input_vec_idx;  
	 } else if (t_plus_k >= T) {
	    // handle end boundary
            inCep = allCep[T_minus_1] + input_vec_idx;  
	 } else {
            inCep = allCep[t_plus_k] + input_vec_idx;  
	 }

	 // Perform linear combination for each dimension
	 for (d=0;d<subVectorDim;d++) {
	    outCep[d] += w[k] * inCep[d];
	 }
      } // end context k
   }
}
