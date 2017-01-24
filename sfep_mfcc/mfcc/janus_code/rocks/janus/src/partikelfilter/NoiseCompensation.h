/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Noise Compensation
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  NoiseCompensation.h
    Date    :  $Id: NoiseCompensation.h 2844 2008-11-25 12:00:00Z wolfel $

   ========================================================================

    Revision 1.1  2008/11/25  12:00:00  wolfel
    Initial revision

   ======================================================================== */

#ifndef NOISE_COMPENSATION_YET_INCLUDED
    #define NOISE_COMPENSATION_YET_INCLUDED

/*-----------------------------------------------------------------------------
/  INCLUDES
/----------------------------------------------------------------------------*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emath.h"
#include "Gaussian.h"
#include "GaussianMixture.h"

/*-----------------------------------------------------------------------------
/  CONSTANTS
/----------------------------------------------------------------------------*/
enum NC_CONFIG
{
    NC_CONFIG_METHOD                = 0x00000F0,
    NC_CONFIG_METHOD_SIA            = 0x0000010,
    NC_CONFIG_METHOD_VTS            = 0x0000020
};

/******************************************************************************
/  struct <ConfigNC>
******************************************************************************/
struct ConfigNC
{
    int         flags;
    int         phaseAccuracy;              /* [2,+inf) - Accuracy of phase-sensitive methods */
    float       compensationAccuracy;       /* (0,1] - This is an upper bound for a theoretical worst
                                                       case scenario that will never occur in parxis.
                                                       Values between 99% and 90% should be okay. */
} typedef ConfigNC;

/******************************************************************************
/  struct
******************************************************************************/
struct NoiseCompensation
{
    /*-------------------------------------------------------------------------
    /  CONFIG
    /------------------------------------------------------------------------*/
    ConfigNC            config;

    /*-------------------------------------------------------------------------
    /  VARIABLES
    /------------------------------------------------------------------------*/
    GaussianMixture**   speechGM;

    /*-------------------------------------------------------------------------
    /  INTERNAL DATA
    /------------------------------------------------------------------------*/
    double*             activity;
    float*              tempDFV;

    /*-------------------------------------------------------------------------
    /  FUNCTIONS
    /------------------------------------------------------------------------*/
    double (*likelihood)(struct NoiseCompensation* compensation, float* y, float* n);
    double (*compensate)(struct NoiseCompensation* compensation, float* y, float* n, float* imputedX);
} typedef NoiseCompensation;

/******************************************************************************
/  functions
******************************************************************************/
NoiseCompensation* createNoiseCompensation(GaussianMixture** speechGM, ConfigNC config);
void createGaussianTable(int resolution, float cutoffProb);

#endif
