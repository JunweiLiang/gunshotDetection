/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Speech Model Module
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  SpeechModel.h
    Date    :  $Id: SpeechModel.h 2844 2008-11-25 12:00:00Z wolfel $

   ========================================================================

    Revision 1.1  2008/11/25  12:00:00  wolfel
    Initial revision

   ======================================================================== */

#ifndef SPEECH_MODEL_YET_INCLUDED
    #define SPEECH_MODEL_YET_INCLUDED

/*-----------------------------------------------------------------------------
/  INCLUDES
/----------------------------------------------------------------------------*/
#include <math.h>
#include "GaussianMixture.h"
#include "PhonemeSet.h"

/*===================================================================================
/  struct <SpeechModel>
===================================================================================*/
struct SpeechModel
{
    /*-------------------------------------------------------------------------
    /  VARIABLES
    /------------------------------------------------------------------------*/
    int                 nSpecificGmms;

    GaussianMixture*    activeGMM;
    GaussianMixture*    generalGMM;
    GaussianMixture**   specificGMM;

    /*-------------------------------------------------------------------------
    /  FUNCTIONS
    /------------------------------------------------------------------------*/
    void (*set)(struct SpeechModel* speechModel, void* data);
    void (*destruct)(struct SpeechModel* speechModel);
    GaussianMixture* (*determineLargestGMM)(struct SpeechModel* speechModel);

} typedef SpeechModel;

/*===================================================================================
/  functions
===================================================================================*/
SpeechModel* createSpeechModel(GaussianMixture* generalGMM, GaussianMixture** specificGMMs, int nSpecificGmms);

#endif
