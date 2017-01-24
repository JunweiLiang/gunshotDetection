/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: NCPF
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  NCPF.h
    Date    :  $Id: NCPF.h 2844 2008-11-25 12:00:00Z wolfel $

   ========================================================================

    Revision 1.1  2008/11/25  12:00:00  wolfel
    Initial revision

   ======================================================================== */

#ifndef NCPF_YET_INCLUDED
#define NCPF_YET_INCLUDED

/* -------------------------------------------------------------------------
   INCLUDES
   ---------------------------------------------------------------------- */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "emath.h"
#include "RNG.h"
#include "ParticleFilter.h"
#include "GaussianMixture.h"
#include "NoiseCompensation.h"
#include "NoiseModel.h"
#include "SpeechModel.h"

/* -------------------------------------------------------------------------
   CONSTANTS
   ---------------------------------------------------------------------- */
enum NCPF_CONFIG {
    NCPF_CONFIG_ACCEPTANCE_TEST         = 0x00000F0,    /* FAST ACCEPTANCE TEST */
    NCPF_CONFIG_USE_ACCEPTANCE_TEST     = 0x0000010
};

/* ****************************************************************************
   struct <ConfigNCPF>
===================================================================================*/
struct ConfigNCPF
{
    ConfigPF    configPF;
    ConfigNC    configNC;

    int         flags;
    float       compensationAccuracy;      /* (0,1] - use with NCPF_CONFIG_SCALING, smaler values => faster */
    int         FAT_nRetries;              /* Fast Acceptance Test (FAT) - max number of retries */
} typedef ConfigNCPF;

/*===================================================================================
   struct <NCPF>
===================================================================================*/
struct NCPF
{
    ParticleFilter      pf;

    /* -------------------------------------------------------------------------
      VARIABLES
      ---------------------------------------------------------------------- */

    ConfigNCPF          config;
    NoiseModel*         noiseModel;
    SpeechModel*        speechModel;
    NoiseCompensation*  nc;

    float*              imputedX;
    float*              delay_noise;
    /* -------------------------------------------------------------------------
      INTERNAL VARIABLES
      ---------------------------------------------------------------------- */
    float*              imputedXS;
    double*             imputedXD;
    float*              spectra_noise;
    float*              observation_mslp;
    float*              noise_mslp;

    /* -------------------------------------------------------------------------
      FUNCTIONS
      ---------------------------------------------------------------------- */
    void (*initialize)(struct NCPF* pf, float* observation);
    void (*run)(struct NCPF* ncpf, float* observation, float* imputedX, float* delay_noise, int frame);
    void (*setNIO)(struct NCPF* ncpf, float nioInDB);
} typedef NCPF;

/*===================================================================================
   functions
===================================================================================*/
NCPF* createNCPF(NoiseModel* noiseModel, SpeechModel* speechModel, ConfigNCPF configNCPF);
#endif
