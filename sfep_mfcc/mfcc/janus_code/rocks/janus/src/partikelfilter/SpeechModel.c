/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Speech Model Module
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  SpeechModel.c
    Date    :  $Id: SpeechModel.c 2844 2008-11-25 12:00:00Z wolfel $
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
#include "SpeechModel.h"

/*-----------------------------------------------------------------------------
/  FORWARD DECLARATIONS
/----------------------------------------------------------------------------*/
void SpeechModel_destruct(SpeechModel* speechModel);
void SpeechModel_set(SpeechModel* speechModel, void* data);
GaussianMixture* SpeechModel_determineLargestGMM(SpeechModel* speechModel);

/*===================================================================================
/  determineLargestGMM
===================================================================================*/
GaussianMixture* SpeechModel_determineLargestGMM(SpeechModel* speechModel)
{
    GaussianMixture*    largestGMM;
    int                 i;

    if (speechModel->generalGMM != 0) largestGMM = speechModel->generalGMM;
    else {
	if (speechModel->specificGMM[0] != 0) largestGMM = speechModel->specificGMM[0];
	else {
            printf("[ERROR] NO SPEECH MODELS!\n");
	    exit(1);
        }
    }
    for (i=0; i<(speechModel->nSpecificGmms); i++) {
	if (largestGMM->nMixtures < speechModel->specificGMM[i]->nMixtures)
	    largestGMM = speechModel->specificGMM[i];
    }

    return(largestGMM);
}

/*===================================================================================
/  createSpeechModel
===================================================================================*/
SpeechModel* createSpeechModel(GaussianMixture* generalGMM, GaussianMixture** specificGMMs, int nSpecificGmms)
{
    SpeechModel* speechModel;

    /*-------------------------------------------------------------------------
    /  allocate memory for <SpeechModel> structure
    /------------------------------------------------------------------------*/
    speechModel = (SpeechModel*) malloc(sizeof(SpeechModel));
    memset(speechModel, 0, sizeof(SpeechModel));

    /*-------------------------------------------------------------------------
    /  initialize variables
    /------------------------------------------------------------------------*/
    speechModel->nSpecificGmms  = nSpecificGmms;
    speechModel->specificGMM    = specificGMMs;
    speechModel->generalGMM     = generalGMM;

    /*-------------------------------------------------------------------------
    /  assign functions
    /------------------------------------------------------------------------*/
    speechModel->set      = SpeechModel_set;
    speechModel->destruct = SpeechModel_destruct;
    speechModel->determineLargestGMM = SpeechModel_determineLargestGMM;

    return(speechModel);
}

/*===================================================================================
/  SpeechModel_set
===================================================================================*/
void SpeechModel_set(SpeechModel* speechModel, void* data)
{
    int gmmN = *((int*)data);

    if (gmmN<0) {
        if (speechModel->generalGMM==0) {
            printf("[ERROR] General speech model [%i] not found in codebook!\n", gmmN);
            exit(1);
        }
        speechModel->activeGMM = speechModel->generalGMM;
    } else {
        if ((gmmN>=speechModel->nSpecificGmms) || (speechModel->specificGMM[gmmN]==0)) {
            printf("[ERROR] Acoustical unit speech model [%i] not found in codebook!\n", gmmN);
            exit(1);
        }
        speechModel->activeGMM = speechModel->specificGMM[gmmN];
    }
}

/*===================================================================================
/  SpeechModel_destruct
===================================================================================*/
void SpeechModel_destruct(SpeechModel* speechModel)
{
    GaussianMixture*    generalGMM  = speechModel->generalGMM;
    GaussianMixture**   specificGMM  = speechModel->specificGMM;
    int 		i;

    /*-------------------------------------------------------------------------
    /  destroy general speech gmm
    /------------------------------------------------------------------------*/
    if (generalGMM!=0) generalGMM->destruct(generalGMM);

    /*-------------------------------------------------------------------------
    /  destroy phoneme-specific speech gmms
    /------------------------------------------------------------------------*/
    if (specificGMM!=0) {
        for (i=0; i<(speechModel->nSpecificGmms); i++)
            specificGMM[i]->destruct(specificGMM[i]);
        free(specificGMM);
    }

    free(speechModel);
}
