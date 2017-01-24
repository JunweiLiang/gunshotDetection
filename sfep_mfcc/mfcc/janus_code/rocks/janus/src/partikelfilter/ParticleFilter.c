/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Particle Filter
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  ParticleFilter.c
    Date    :  $Id: ParticleFilter.c 2844 2008-11-25 12:00:00Z wolfel $
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

/* -------------------------------------------------------------------------
   INCLUDES
   ---------------------------------------------------------------------- */
#include <limits.h>
#include <errno.h>
#include <math.h>
#include "ParticleFilter.h"
#include "formants.h"
#include "featureModalities.h"          /* methods for modalities           */
#include "common.h"
#include "codebook.h"
#include "distrib.h"

int isfiniteF(float f)
{
    return(!(isnan(f) || isinf(f)));
}

int isfiniteD(double d)
{
    return(!(isnan(d) || isinf(d)));
}

/*===================================================================================
   particleFilterNormalizeWeights
===================================================================================*/
void PF_normalizeWeights(ParticleFilter* pf)
{
    int         nParticles  = pf->config.nParticles;
    double*     weights     = (*pf).weights;
    int      i;
    double   sumWeights, oneBySumWeights; 

    /* -------------------------------------------------------------------------
      calc sumWeights
      ---------------------------------------------------------------------- */
    sumWeights = 0.0;
    for (i=0; i<nParticles; i++) sumWeights += weights[i];

    /* -------------------------------------------------------------------------
      catch "all weights zero"
      ---------------------------------------------------------------------- */
    if (sumWeights==0.0) {
        for (i=0; i<nParticles; i++) weights[i] = 1.0;
        sumWeights = nParticles;
    }

    /* -------------------------------------------------------------------------
      catch "nan" weights
      ---------------------------------------------------------------------- */
    if (!isfiniteD(sumWeights)) {
        fprintf(stderr,"PF: weight calculation in normalize weights step wrong\n");
        fprintf(stderr,"weights: ");
        for (i=0; i<nParticles; i++) fprintf(stderr,"%e, ", weights[i]);
        fprintf(stderr,"\n");
        sumWeights = 0;
        for (i=0; i<nParticles; i++) sumWeights += weights[i];
        fprintf(stderr,"real sumwghts: %e\n", sumWeights);
        exit(1);
    }

    /* -------------------------------------------------------------------------
      normalize importance weights
      ---------------------------------------------------------------------- */
    oneBySumWeights = 1.0 / sumWeights;
    for (i=0; i<nParticles; i++) weights[i] *= oneBySumWeights;
}

/*===================================================================================
/  computeMF_SR
/
/  MF  = Multiplication Factor (for pruning/resampling)
/  SR  = Systematic Resampling 
===================================================================================*/
void PF_computeMF_SR(ParticleFilter* pf)
{
    int         nParticles  = pf->config.nParticles;
    double*     weights     = pf->weights;
    int*        mfs         = pf->mfs;
    int         i, k;
    double      stepWidth, unitarySample, cumulativeWeight;
    stepWidth = 1.0 / nParticles;
    unitarySample = RNG_Float() * stepWidth;
    cumulativeWeight = 0.0;

    for (i=0; i<nParticles; i++) {
        cumulativeWeight += weights[i];
        k = 0;
        while (unitarySample<=cumulativeWeight) {
            k++;
            unitarySample += stepWidth;
        }
        mfs[i] = k;
    }
}

/*===================================================================================
/  computeMF_RSR
/
/  MF  = Multiplication Factor (for pruning/resampling)
/  RSR = Residual Systematic Resampling 
/        (see Bolic, Djuric, Hong - New Resampling Algorithms for Particle
/         Filters, ICASSP 2003, IEEE)
===================================================================================*/
void PF_computeMF_RSR(ParticleFilter* pf)
{
    int         nParticles  = pf->config.nParticles;
    double*     weights     = (*pf).weights;
    int*        mfs         = (*pf).mfs;
    int         i, k;
    double      stepWidth, unitarySample;

    stepWidth = 1.0 / nParticles;
    unitarySample = RNG_Float() * stepWidth;

    for (i=0; i<nParticles; i++) {
        k = floor((weights[i]-unitarySample) * nParticles) + 1;
        unitarySample = unitarySample + k*stepWidth - weights[i];
        if (k<0) {
            fprintf(stderr,"PF: resampling error\n");
            fprintf(stderr,"    partcile-no: %i\n", i);
            fprintf(stderr,"    k: %i\n", k);
            fprintf(stderr,"    weights[i]: %e\n", weights[i]);
            fprintf(stderr,"    unitarySample: %e\n", unitarySample);
            exit(1);
        }
        mfs[i] = k;
    }
}

/*===================================================================================
/  NCPF_updateN
===================================================================================*/
void PF_updateHistory(ParticleFilter* pf)
{
    int                 dim              = pf->config.dim;
    int                 nParticles       = pf->config.nParticles;
    int                 historyLevel     = pf->config.historyLevel;
    float**             particles        = pf->particles;
    float**             particlesHistory = pf->particlesHistory;
    float*  floatP;
    float*  particle;
    float*  particleHistory;
    int     mvLen, i, j;

    mvLen = dim*(historyLevel-1);

    for (i=0; i<nParticles; i++) {
        particleHistory = particlesHistory[i];
        particle = particles[i];
        floatP = &(particleHistory[dim]);
        for (j=mvLen-1; j>=0; j--) floatP[j] = particleHistory[j];
        for (j=0; j<dim; j++) particleHistory[j] = particle[j];
    }
}

/*===================================================================================
/  particleFilterResample
===================================================================================*/
void PF_resample(ParticleFilter* pf)
{
    int         config           = pf->config.flags;
    int         dim              = pf->config.dim;
    int         historyLevel     = pf->config.historyLevel;
    int         nParticles       = pf->config.nParticles;
    float**     particles        = pf->particles;
    float**     particlesHistory = pf->particlesHistory;
    double*     weights          = pf->weights;
    double*     weightsHistory   = pf->weightsHistory;
    int*        mfs              = pf->mfs;
    // double*     weights          = pf->weights;
    int         i, j, k;
    int         sumMFS;
    int         particleSize;
    int         particleHistorySize;

    /* -------------------------------------------------------------------------
      obtain multiplication-factors (mfs) for resampling
      ---------------------------------------------------------------------- */
    switch (config&PF_CONFIG_RESAMPLING) {
        case PF_CONFIG_RESAMPLING_SR:
            PF_computeMF_SR(pf);
        break;
        case PF_CONFIG_RESAMPLING_RSR:
            PF_computeMF_RSR(pf);
        break;
        default:
            fprintf(stderr,"PF: unknown resampling-configuration [%i]\n", config&PF_CONFIG_RESAMPLING);
            exit(1);
        break;
    }

    /* -------------------------------------------------------------------------
      check that everything is fine  
      --- USE TO MAKE SURE COMPUTE-MF WORKS THE WAY IT SHOULD
      ---------------------------------------------------------------------- */
    sumMFS = 0;
    for (i=0; i<nParticles; i++) sumMFS += mfs[i];
    if (sumMFS!=nParticles) {
        fprintf(stderr,"PF: something wrong with PF resampling\n");
        exit(1);
    }

    /* -------------------------------------------------------------------------
      perform actual resampling
      ---------------------------------------------------------------------- */
    particleSize = dim*sizeof(float);
    particleHistorySize = historyLevel*dim*sizeof(float);
    j = 0;
    for (i=0; i<nParticles; i++) {
        k = mfs[i] - 1;
        while (k>0) {
            while (mfs[j]>0) j++;
            memcpy(particles[j], particles[i], particleSize);
            memcpy(particlesHistory[j], particlesHistory[i], particleHistorySize);
	    weights[j] = weights[i];
            weightsHistory[j] = weightsHistory[i];
            j++;
            k--;
        }
    }
}

/*===================================================================================
/  PF_initialize
===================================================================================*/
void PF_initialize(ParticleFilter* pf, float* observation)
{
    int         historyLevel = pf->config.historyLevel;
    float**     dataP        = pf->dataP;
    int i;

    pf->initializeParticles(dataP, observation);
    PF_updateHistory(pf);

    for (i=0; i<historyLevel-1; i++) {
        /* might want to use prediction here instead of sampling */
        (*pf).sampleParticles(dataP, observation);
        PF_updateHistory(pf);
    }

    pf->refresh_sllc = 0;
}

/*===================================================================================
/  PF_refresh
===================================================================================*/
int PF_needRefresh(ParticleFilter* pf)
{
    int         nParticles  = pf->config.nParticles;
    double      ollb        = pf->config.refresh_ollb;
    int         sllt        = pf->config.refresh_sllt;
    double*     weights     = (*pf).weights;
    int         sllc        = (*pf).refresh_sllc;
    int         i;
    double      ol;

    /* -------------------------------------------------------------------------
      compute overall likelihood
      ---------------------------------------------------------------------- */
    ol = 0.0;
    for (i=0; i<nParticles; i++) ol += weights[i];

    /* -------------------------------------------------------------------------
      refresh particle population if low likelihood persists for an extended period of time
      ---------------------------------------------------------------------- */
    if (ol<ollb) {
        sllc++;
        if (sllc>sllt) {
            pf->refresh_sllc = 0;
            return(1);
        }
    } else sllc = 0;

    /* -------------------------------------------------------------------------
      write back
      ---------------------------------------------------------------------- */
    pf->refresh_sllc = sllc;

    return(0);
}

/*===================================================================================
/  PF_run
===================================================================================*/
void PF_run(ParticleFilter* pf, float* observation, int frame)
{
    int         config      = pf->config.flags;
    float**     dataP       = pf->dataP;

    /* -------------------------------------------------------------------------
      obtain new samples
      ---------------------------------------------------------------------- */
    (*pf).sampleParticles(dataP, observation);

    /* -------------------------------------------------------------------------
      evaluate weights including inference step
      ---------------------------------------------------------------------- */
    (*pf).weightParticles(dataP, observation);

    /* -------------------------------------------------------------------------
    /  refresh particle population if neccessary
     ---------------------------------------------------------------------- */
    if ((config&PF_CONFIG_USE_REFRESH) && PF_needRefresh(pf)) {
        fprintf(stderr,"PF: refresh noise at frame %i\n", frame);
        PF_initialize(pf, observation);
        (*pf).weightParticles(dataP, observation);
    }

    /* -------------------------------------------------------------------------
      normalize weights
      ---------------------------------------------------------------------- */
    PF_normalizeWeights(pf);

    /* -------------------------------------------------------------------------
      update AR matrix
      ---------------------------------------------------------------------- */
    (*pf).updateAR(dataP, observation);

    /* -------------------------------------------------------------------------
      resample
      ---------------------------------------------------------------------- */
    PF_resample(pf);

    /* -------------------------------------------------------------------------
      Markov Chain Monte Carlo (MCMC) Step
      ---------------------------------------------------------------------- */
    if (config&PF_CONFIG_USE_MOVESTEP) (*pf).moveStep(dataP, observation);

    /* -------------------------------------------------------------------------
    /  update particles' history
     ---------------------------------------------------------------------- */
    PF_updateHistory(pf);
}

/*===================================================================================
/  initParticleFilter
===================================================================================*/
int initParticleFilter(ParticleFilter* pf, ConfigPF config, 
                    void (*initializeParticles)(void* dataP, float* observation),
                    void (*sampleParticles)(void* dataP, float* observation),
                    void (*weightParticles)(void* dataP, float* observation),
                    void (*updateAR)(void* dataP, float* observation),
                    void (*moveStep)(void* dataP, float* observation),
                    void* dataP)
{
    int     dim          = config.dim;
    int     nParticles   = config.nParticles;
    int     historyLevel = config.historyLevel;
    int     i;
    float*  particles_data;
    float*  particlesHistory_data;
    int     historyLen;

    /* -------------------------------------------------------------------------
      allocate memory
      ---------------------------------------------------------------------- */
    (*pf).weights = (double*) malloc(nParticles*sizeof(double));
    (*pf).weightsHistory = (double*) malloc(nParticles*sizeof(double));
    (*pf).mfs = (int*) malloc(nParticles*sizeof(int));
    particles_data = (float*) malloc(nParticles*dim*sizeof(float));
    (*pf).particles = (float**) malloc(nParticles*sizeof(float*));

    /* -------------------------------------------------------------------------
      build particles
      ---------------------------------------------------------------------- */
    for (i=0; i<nParticles; i++) pf->particles[i] = &(particles_data[i*dim]);

    /* -------------------------------------------------------------------------
      allocate memory for particlesHistory
      ---------------------------------------------------------------------- */
    historyLen = historyLevel*dim;
    particlesHistory_data = (float*) malloc(nParticles*historyLen*sizeof(float));
    (*pf).particlesHistory = (float**) malloc(nParticles*sizeof(float*));

    /* -------------------------------------------------------------------------
    /  build particlesHistory
     ---------------------------------------------------------------------- */
    for (i=0; i<nParticles; i++) pf->particlesHistory[i] = &(particlesHistory_data[i*historyLen]);

    /* -------------------------------------------------------------------------
    /  initialize variables
     ---------------------------------------------------------------------- */
    (*pf).config       = config;
    (*pf).dataP        = dataP;

    /* -------------------------------------------------------------------------
    /  link member functions
     ---------------------------------------------------------------------- */
    (*pf).run                       = PF_run;
    (*pf).initialize                = PF_initialize;

    /* -------------------------------------------------------------------------
    /  link internal functions
     ---------------------------------------------------------------------- */
    (*pf).initializeParticles        = initializeParticles;
    (*pf).sampleParticles            = sampleParticles;
    (*pf).weightParticles            = weightParticles;
    (*pf).updateAR                   = updateAR;
    (*pf).moveStep                   = moveStep;

    return(1);
}

/*===================================================================================
/  createParticle
===================================================================================*/
ParticleFilter* createParticleFilter(ConfigPF config, 
                    void (*initializeParticles)(void* dataP, float* observation),
                    void (*sampleParticles)(void* dataP, float* observation),
                    void (*weightParticles)(void* dataP, float* observation),
                    void (*updateAR)(void* dataP, float* observation),
                    void (*moveStep)(void* dataP, float* observation),
                    void* dataP)
{
    ParticleFilter*  pf;

    /* -------------------------------------------------------------------------
    /  allocate memory for <Particle> structure
     ---------------------------------------------------------------------- */
    pf = (ParticleFilter*) malloc(sizeof(ParticleFilter));
    memset(pf, 0, sizeof(ParticleFilter));

    /* -------------------------------------------------------------------------
    /  initialize PF
     ---------------------------------------------------------------------- */
    if (!initParticleFilter(pf, config, initializeParticles, sampleParticles, weightParticles, updateAR, moveStep, dataP))
        return(0);

    return(pf);
}
