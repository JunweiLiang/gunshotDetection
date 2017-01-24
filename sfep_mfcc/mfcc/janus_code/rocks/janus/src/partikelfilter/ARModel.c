/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: AR Module
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  ARModul.c
    Date    :  $Id: ARModul.c 2844 2008-11-25 12:00:00Z wolfel $
    Remarks :

   ========================================================================

    This software was developed by

    the  University of Karlsruhe       and  Carnegie Mellon University
         Dept. of Informatics               Dept. of Computer Science
         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
         Am Fasanengarten 5                 5000 Forbes Ave
         D-76131 Karlsruhe                  Pittsburgh, PA 15213
         - West Germany -                   - USA -

    Copyright (C) 1990-1994.   All rights reserved.

    This software is part of the JANUS Speech- to Speech Translation Project

    USAGE PERMITTED ONLY FOR MEMBERS OF THE JANUS PROJECT
    AT CARNEGIE MELLON UNIVERSITY OR AT UNIVERSITAET KARLSRUHE
    AND FOR THIRD PARTIES ONLY UNDER SEPARATE WRITTEN PERMISSION
    BY THE JANUS PROJECT

    It may be copied  only  to members of the JANUS project
    in accordance with the explicit permission to do so
    and  with the  inclusion  of  the  copyright  notices.

    This software  or  any  other duplicates thereof may
    not be copied or otherwise made available to any other person.

    No title to and ownership of the software is hereby transferred.

   ========================================================================

    Revision 1.1  2008/11/25  12:00:00  wolfel
    Initial revision

   ======================================================================== */

/*-----------------------------------------------------------------------------
/  INCLUDES
/----------------------------------------------------------------------------*/
#include "ARModel.h"

/*-----------------------------------------------------------------------------
/  FORWARD DECLARATIONS
/----------------------------------------------------------------------------*/
void destructARModel(ARModel* arModel);

/*===================================================================================
/  ARModel_predict
/
/  N                   :  complete noise history for AR-model prediction at time t
/  predicted_ntp1      :  predicted noise for time (t+1)
/  noiseIntensityInDB  :  current noise intensity in db
===================================================================================*/
void ARModel_predict(ARModel* arModel, float* N, float* predicted_ntp1, float nioInDB)
{
    int         dim          = arModel->dim;
    int         modelOrder   = arModel->modelOrder;
    float**     coeffMatrix  = arModel->coeffMatrix;
    float*      residualErr  = arModel->errorDistr->mean;

    int     i, j, rowLen;
    float*  row;
    double  res;

    /*-------------------------------------------------------------------------
    /  predicted_ntp1 = (coeffMatrix.-nioInDB) * N' + nioInDB + residual
    /------------------------------------------------------------------------*/
    rowLen = dim*modelOrder;
    for (i=0; i<dim; i++) {
        row = coeffMatrix[i];
        res = 0.0;
        for (j=0; j<rowLen; j++) res += row[j] * (N[j]-nioInDB);
        predicted_ntp1[i] = res - residualErr[i] + nioInDB;
    }
}

/*===================================================================================
/  ARModel_sampleFromPD
/
/  N                   :  complete noise history for AR-model prediction at time t
/  predicted_ntp1      :  predicted noise for time (t+1)
/  noiseIntensityInDB  :  current noise intensity in db
===================================================================================*/
void ARModel_sampleFromPD(ARModel* arModel, float* N, float* predicted_ntp1, float nioInDB)
{
    Gaussian*   workingErrorDistr   = arModel->workingErrorDistr;
    int         dim          = arModel->dim;
    int p;

    /*-------------------------------------------------------------------------
    /  predict ntp1
    /------------------------------------------------------------------------*/
    ARModel_predict(arModel, N, predicted_ntp1, nioInDB);

    /*-------------------------------------------------------------------------
    /  sample from gaussian error-distribution
    /------------------------------------------------------------------------*/
    workingErrorDistr->updateMean(workingErrorDistr, predicted_ntp1);
    workingErrorDistr->sample(workingErrorDistr, predicted_ntp1);

    for (p=0; p<dim; p++) if (predicted_ntp1[p] < 0.0) predicted_ntp1[p] = -predicted_ntp1[p];
}

/*===================================================================================
/  createNoiseModel
===================================================================================*/
ARModel* createARModel(int dim, int modelOrder, Gaussian* errorDistr)
{
    float**     coeffMatrixHistory;
    float*      coeffArrayHistory;
    float**     coeffMatrix;
    float*      coeffArray;
    float**     D1;
    float**     D2;
    float*      D1a;
    float*      D2a;
    float**     D1temp;
    float**     D2temp;
    float*      D1atemp;
    float*      D2atemp;
    DMatrix*    matD1 = NULL;
    DMatrix*    matD2 = NULL;
    DMatrix*    matD3 = NULL;
    /* static necessary for online mode */
    static ARModel*    arModel;
    int         i,j;

    /*-------------------------------------------------------------------------
    /  allocate memory for <ARModel> structure
    /------------------------------------------------------------------------*/
    arModel = (ARModel*) malloc(sizeof(ARModel));
    if (arModel==0) {
        printf("[ERROR] loadNoiseModel> MEMORY ALLOCATION FAILED!\n");
        return(0);
    }
    memset(arModel, 0, sizeof(ARModel));

    /*-------------------------------------------------------------------------
    /  allocate memory for AR coeff matrix
    /------------------------------------------------------------------------*/
    coeffArrayHistory = (float*) malloc(dim*dim*sizeof(float));
    coeffArray = (float*) malloc(dim*dim*sizeof(float));
    D1a = (float*) malloc(dim*dim*sizeof(float));
    D2a = (float*) malloc(dim*dim*sizeof(float));
    D1atemp = (float*) malloc(dim*dim*sizeof(float));
    D2atemp = (float*) malloc(dim*dim*sizeof(float));

    coeffMatrixHistory = malloc(dim*sizeof(float*));
    coeffMatrix = malloc(dim*sizeof(float*));
    D1 = malloc(dim*sizeof(float*));
    D2 = malloc(dim*sizeof(float*));
    D1temp = malloc(dim*sizeof(float*));
    D2temp = malloc(dim*sizeof(float*));

    matD1 = dmatrixCreate(dim, dim);
    matD2 = dmatrixCreate(dim, dim);
    matD3 = dmatrixCreate(dim, dim);

    if (coeffMatrix==0) return(0);

    /*-------------------------------------------------------------------------
    /  build AR coeff matrix
    /------------------------------------------------------------------------*/
    for (i=0; i<dim; i++) {
	coeffMatrix[i] = &(coeffArray[i*dim]);
	coeffMatrixHistory[i] = &(coeffArrayHistory[i*dim]);
	D1[i] = &(D1a[i*dim]);
	D2[i] = &(D2a[i*dim]);
	D1temp[i] = &(D1atemp[i*dim]);
	D2temp[i] = &(D2atemp[i*dim]);
    }

    /*-------------------------------------------------------------------------
    /  clone errorDistr
    /------------------------------------------------------------------------*/
    arModel->workingErrorDistr = errorDistr->clone(errorDistr);

    /*-------------------------------------------------------------------------
    /  initialize variables
    /------------------------------------------------------------------------*/
    arModel->dim                     = dim;
    arModel->modelOrder              = modelOrder;
    arModel->coeffMatrix             = coeffMatrix;
    arModel->coeffMatrixHistory      = coeffMatrixHistory;
    arModel->D1                      = D1;
    arModel->D2                      = D2;
    arModel->D1temp                  = D1temp;
    arModel->D2temp                  = D2temp;
    arModel->matD1                   = matD1;
    arModel->matD2                   = matD2;
    arModel->matD3                   = matD3;

    for (i=0; i<dim; i++) {
        for (j=0; j<dim; j++) {
	    arModel->D1[i][j] = 0.0;
	    arModel->D2[i][j] = 0.0;
	    arModel->D1temp[i][j] = 0.0;
	    arModel->D2temp[i][j] = 0.0;
	}
    }

    for (i=0; i<dim; i++) {
	arModel->D1[i][j] = 1.0;
	arModel->D2[i][j] = 1.0;
    }

    arModel->errorDistr       = errorDistr;

    /*-------------------------------------------------------------------------
    /  assign functions
    /------------------------------------------------------------------------*/
    arModel->sampleFromPD   = ARModel_sampleFromPD;
    arModel->predict        = ARModel_predict;
    arModel->destruct       = destructARModel;

    return(arModel);
}

/*===================================================================================
/  destructARModel
===================================================================================*/
void destructARModel(ARModel* arModel)
{
    int         dim                 = (*arModel).dim;
    float**     coeffMatrix         = (*arModel).coeffMatrix;
    Gaussian*   errorDistr          = (*arModel).errorDistr;
    Gaussian*   workingErrorDistr   = (*arModel).workingErrorDistr;
    int 	i;

    workingErrorDistr->destruct(workingErrorDistr);
    errorDistr->destruct(errorDistr);
    for (i=0; i<dim; i++) free(coeffMatrix[i]);
    free(coeffMatrix);
    free(arModel);
}
