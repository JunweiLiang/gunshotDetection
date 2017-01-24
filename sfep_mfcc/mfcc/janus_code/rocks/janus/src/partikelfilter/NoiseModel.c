/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Noise Model
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  NoiseModel.c
    Date    :  $Id: NoiseModel.c 2844 2008-11-25 12:00:00Z wolfel $
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

/* ----------------------------------------------------------------------------
/  INCLUDES
/----------------------------------------------------------------------------*/
#include "NoiseModel.h"

/* ----------------------------------------------------------------------------
/  FORWARD DECLARATIONS
/----------------------------------------------------------------------------*/
void NoiseModel_destruct(NoiseModel* noiseModel);

/*===================================================================================
/  NoiseModel_getNoiseSample
/
/  nioInDB  :  noise intensity offset in db
===================================================================================*/
void NoiseModel_getNoiseSample(NoiseModel* noiseModel, float* sample)
{
    float               nioInDB = noiseModel->nioInDB;
    GaussianMixture*    gmm     = noiseModel->gmm;
    int                 dim     = gmm->dim;
    int i;

    gmm->sample(gmm, sample);
    for (i=0; i<dim; i++) sample[i] += nioInDB;
}

/*===================================================================================
/  NoiseModel_setNIO (Noise Intensity Offset)
/
/  nioInDB  :  noise intensity offset in db
===================================================================================*/
void NoiseModel_setNIO(struct NoiseModel* noiseModel, float nioInDB)
{
    noiseModel->nioInDB = nioInDB;
}

/*===================================================================================
/  NoiseModel_predict_AR (AutoRegressive)
/
/  N               :  complete noise history for AR-model prediction at time t
/  predicted_ntp1  :  predicted noise for time (t+1)
===================================================================================*/
void NoiseModel_predict_AR(NoiseModel* noiseModel, float* N, float* predicted_ntp1)
{
    ARModel* arm            = noiseModel->arm;
    float    nioInDB        = noiseModel->nioInDB;

    arm->predict(arm, N, predicted_ntp1, nioInDB);
}

/*===================================================================================
/  NoiseModel_sampleFromPD_AR (Predictive Density, AutoRegressive)
/
/  N               :  complete noise history for AR-model prediction at time t
/  predicted_ntp1  :  predicted noise for time (t+1)
===================================================================================*/
void NoiseModel_sampleFromPD_AR(NoiseModel* noiseModel, float* N, float* predicted_ntp1)
{
    ARModel* arm            = noiseModel->arm;
    float    nioInDB        = noiseModel->nioInDB;

    arm->sampleFromPD(arm, N, predicted_ntp1, nioInDB);
}

/*===================================================================================
/  createNoiseModel
===================================================================================*/
NoiseModel* createNoiseModelAR(ARModel* arm, GaussianMixture* gmm)
{
    NoiseModel*  noiseModel;

    /* ------------------------------------------------------------------------
      allocate memory for <NoiseModel> structure
      --------------------------------------------------------------------- */
    noiseModel = (NoiseModel*) malloc(sizeof(NoiseModel));
    memset(noiseModel, 0, sizeof(NoiseModel));

    /* ------------------------------------------------------------------------
      initialize variables
      --------------------------------------------------------------------- */
    noiseModel->modelOrder = arm->modelOrder;
    noiseModel->arm        = arm;
    noiseModel->gmm        = gmm;
    
    /* ------------------------------------------------------------------------
      assign functions
      --------------------------------------------------------------------- */
    noiseModel->sampleFromPD    = NoiseModel_sampleFromPD_AR;
    noiseModel->predict         = NoiseModel_predict_AR;
    noiseModel->destruct        = NoiseModel_destruct;
    noiseModel->setNIO          = NoiseModel_setNIO;
    noiseModel->getNoiseSample  = NoiseModel_getNoiseSample;

    return(noiseModel);
}

/*===================================================================================
/  NoiseModel_destruct
===================================================================================*/
void NoiseModel_destruct(NoiseModel* noiseModel)
{
    noiseModel->arm->destruct(noiseModel->arm);
    noiseModel->gmm->destruct(noiseModel->gmm);
    free(noiseModel);
}
