/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Particle Filter
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  ParticleFilter.h
    Date    :  $Id: ParticleFilter.h 2844 2008-11-25 12:00:00Z wolfel $

   ========================================================================

    Revision 1.1  2008/11/25  12:00:00  wolfel
    Initial revision

   ======================================================================== */

#ifndef PARTICLE_FILTER_YET_INCLUDED
    #define PARTICLE_FILTER_YET_INCLUDED

/* -----------------------------------------------------------------------------
   INCLUDES
   -------------------------------------------------------------------------- */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "RNG.h"

/* -----------------------------------------------------------------------------
   CONSTANTS
   -------------------------------------------------------------------------- */
enum PF_CONFIG
{
    PF_CONFIG_RESAMPLING        = 0x000000F,
    PF_CONFIG_RESAMPLING_SR     = 0x0000001,    /* Systematic Resampling */
    PF_CONFIG_RESAMPLING_RSR    = 0x0000002,    /* Residual Systematic Resampling */

    PF_CONFIG_USE_REFRESH       = 0x0000010,
    PF_CONFIG_USE_MOVESTEP      = 0x0000020
};

/*===================================================================================
   struct <ConfigPF>
===================================================================================*/
struct ConfigPF
{
    int     flags;
    int     dim;
    int     nParticles;
    int     historyLevel;
    double  refresh_ollb; 
    int     refresh_sllt; 
} typedef ConfigPF;

/*===================================================================================
   struct <ParticleFilter>
===================================================================================*/
struct ParticleFilter
{
    ConfigPF            config;

    /* -------------------------------------------------------------------------
      VARIABLES
      ---------------------------------------------------------------------- */
    float**             particles;
    float**             particlesHistory;
    double*             weights;
    double*             weightsHistory;
    void*               dataP;

    /* -------------------------------------------------------------------------
      VARIABLES for refresh
      ---------------------------------------------------------------------- */
    int                 refresh_sllc;       /* successive low likelihood count */

    /* -------------------------------------------------------------------------
      INTERNAL VARIABLES
      ---------------------------------------------------------------------- */
    int*                mfs;                 /* Multpliation FactorS */

    /* -------------------------------------------------------------------------
      FUNCTIONS
      ---------------------------------------------------------------------- */
    void (*initialize)(struct ParticleFilter* pf, float* observation);
    void (*run)(struct ParticleFilter* pf, float* observation, int frame);

    /* -------------------------------------------------------------------------
      INTERNAL FUNCTIONS
      ---------------------------------------------------------------------- */
    void (*initializeParticles)(void* dataP, float* observation);
    void (*sampleParticles)(void* dataP, float* observation);
    void (*weightParticles)(void* dataP, float* observation);
    void (*updateAR)(void* dataP, float* observation);
    void (*moveStep)(void* dataP, float* observation);
} typedef ParticleFilter;

/*===================================================================================
   functions
===================================================================================*/
void PF_normalizeWeights(ParticleFilter* pf);
void PF_resample(ParticleFilter* pf);
ParticleFilter* createParticleFilter(ConfigPF config, 
void (*initializeParticles)(void* dataP, float* observation),
void (*sampleParticles)(void* dataP, float* observation),
void (*weightParticles)(void* dataP, float* observation),
void (*updateAR)(void* dataP, float* observation),
void (*moveStep)(void* dataP, float* observation),
void* dataP);
int isfiniteD(double d);
int initParticleFilter(ParticleFilter* pf, ConfigPF config, 
                    void (*initializeParticles)(void* dataP, float* observation),
                    void (*sampleParticles)(void* dataP, float* observation),
                    void (*weightParticles)(void* dataP, float* observation),
                    void (*updateAR)(void* dataP, float* observation),
                    void (*moveStep)(void* dataP, float* observation),
		       void* dataP);

#endif
