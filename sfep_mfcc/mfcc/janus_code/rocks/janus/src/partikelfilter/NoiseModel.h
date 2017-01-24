/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Noise Model
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  NoiseModel.h
    Date    :  $Id: NoiseModel.h 2844 2008-11-25 12:00:00Z wolfel $

   ========================================================================

    Revision 1.1  2008/11/25  12:00:00  wolfel
    Initial revision

   ======================================================================== */

#ifndef NOISE_MODEL_YET_INCLUDED
    #define NOISE_MODEL_YET_INCLUDED

/*-----------------------------------------------------------------------------
/  INCLUDES
/----------------------------------------------------------------------------*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mach_ind_io.h"
#include "GaussianMixture.h"
#include "ARModel.h"

/*===================================================================================
/  struct <NoiseModel>
===================================================================================*/
struct NoiseModel
{
    /*-------------------------------------------------------------------------
    /  VARIABLES
    /------------------------------------------------------------------------*/
    int                 modelOrder;     /* number of previous noise-frames used by the model */

    ARModel*            arm;
    GaussianMixture*    gmm;

    /*-------------------------------------------------------------------------
    /  INTERNAL VARIABLES
    /------------------------------------------------------------------------*/
    float               nioInDB;    /* Noise Intensity Offset in DB */

    /*-------------------------------------------------------------------------
    /  FUNCTIONS
    /------------------------------------------------------------------------*/
    void (*setNIO)(struct NoiseModel* noiseModel, float nioInDB);
    void (*getNoiseSample)(struct NoiseModel* noiseModel, float* sample);
    void (*sampleFromPD)(struct NoiseModel* noiseModel, float* N, float* predicted_ntp1);
    void (*predict)(struct NoiseModel* noiseModel, float* N, float* predicted_ntp1);
    void (*destruct)(struct NoiseModel* noiseModel);
} typedef NoiseModel;

/*===================================================================================
/  functions
===================================================================================*/
NoiseModel* createNoiseModelAR(ARModel* arModel, GaussianMixture* gmm);

#endif
