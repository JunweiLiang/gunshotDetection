/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: NCPF
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  NCPF.c
    Date    :  $Id: NCPF.c 2844 2008-11-25 12:00:00Z wolfel $
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
#include "NCPF.h"

#include <errno.h>
#include <math.h>
#include "ParticleFilter.h"
#include "formants.h"
#include "featureModalities.h"          /* methods for modalities           */
#include "common.h"
#include "codebook.h"
#include "distrib.h"

void NCPF_updateAR(NCPF* ncpf, float* observation)
{
    int                 nParticles  		= ncpf->pf.config.nParticles;
    float**             particles   		= ncpf->pf.particles;
    float**             particlesHistory   	= ncpf->pf.particlesHistory;
    double*             weights     		= ncpf->pf.weights;
    int                 dim         		= ncpf->pf.config.dim;
    int                 spectra_dim 		= ncpf->speechModel->activeGMM->dim;
    float**             coeffMatrix 		= ncpf->noiseModel->arm->coeffMatrix;
    float**             D1Matrix 		= ncpf->noiseModel->arm->D1;
    float**             D2Matrix 		= ncpf->noiseModel->arm->D2;
    float               smoothing   		= ncpf->noiseModel->arm->smoothing;
    DMatrix*		matD1                   = ncpf->noiseModel->arm->matD1;
    DMatrix*		matD2                   = ncpf->noiseModel->arm->matD2;
    DMatrix*		matD3                   = ncpf->noiseModel->arm->matD3;
    int     		p, i, j;
    float*  		particle, particleHistory;
    float   		normalize;
    double  		weight, sumWeights = 0.0;

    for (j=0; j<spectra_dim; j++) {
	for (i=0; i<spectra_dim; i++) {
	    matD1->matPA[j][i] = 0.0;
	    matD2->matPA[j][i] = 0.0;
	}
    }

    for (p=0; p<nParticles; p++) sumWeights += weights[p];

    for (p=0; p<nParticles; p++) {
	if (sumWeights>0.0) {
	    weight = weights[p] / sumWeights;
   	    particle = particles[p];
            for (j=0; j<spectra_dim; j++) {
                particleHistory = particlesHistory[p][j];
                for (i=0; i<spectra_dim; i++) {
	            matD1->matPA[j][i] += weight * particle[i] * particleHistory;
	            matD2->matPA[j][i] += weight * particle[i] * particle[j];
                }
	    }

	} else {
	    weight = 1.0;
	}
    }

    normalize = 1 / (1.0+smoothing);
    for (j=0; j<spectra_dim; j++) {
        for (i=0; i<spectra_dim; i++) {
	        matD1->matPA[j][i] += smoothing * D1Matrix[j][i];
	        matD2->matPA[j][i] += smoothing * D2Matrix[j][i];
	        D1Matrix[j][i] = matD1->matPA[j][i] * normalize;
		D2Matrix[j][i] = matD2->matPA[j][i] * normalize;
        }
    }

    dmatrixTrans(matD2,matD2);
    dmatrixInv(matD2,matD2);
    dmatrixTrans(matD1,matD1);
    dmatrixMul(matD3,matD1,matD2);

    for (j=0; j<dim; j++) {
        for (i=0; i<dim; i++) coeffMatrix[j][i] = 0.0;
    }

    for (j=0; j<spectra_dim; j++) {
        for (i=0; i<spectra_dim; i++) {
	    coeffMatrix[j][i] = matD3->matPA[j][i];
	}
    }
    for (j=spectra_dim; j<dim; j++) coeffMatrix[j][j] = 1.0;
}

/*===================================================================================
/  NCPF_weightParticles
===================================================================================*/
/* __inline__ */
int NCPF_acceptParticle(NCPF* ncpf, float* particle)
{
    int          dim = ncpf->speechModel->activeGMM->dim;
    int     i;
    float   mean;
    mean = 0.0;
    for (i=0; i<dim; i++) mean += particle[i];
    if (mean<0.0) return(0);

    return(1);
}

/*===================================================================================
/  MCMC_MOVE_particle
===================================================================================*/
void MCMC_initialization(NCPF* ncpf, float* observation, float* particle, int iterations)
{
    int                 spectra_dim = ncpf->speechModel->activeGMM->dim;
    NoiseCompensation*  nc          = ncpf->nc;
    float*              imputedXS   = ncpf->imputedXS;
    int                 i, j;
    int                 accept;
    double              weight1, weight2;
    float               move;

    weight1 = nc->likelihood(nc, observation, particle);

    /* -------------------------------------------------------------------------
      MOVE: reamplify 
      ---------------------------------------------------------------------- */
    for (i=0; i<iterations; i++) {
        move = RNG_Gaussian() * 10.0;

        /* -------------------------------------------------------------------------
          FAST ACCEPTANCE TEST
          ---------------------------------------------------------------------- */
        accept = 1;
        for (j=0; j<spectra_dim; j++) if (observation[j]<(particle[j]+move)) accept = 0;

        /* -------------------------------------------------------------------------
          MCMC STEP
          ---------------------------------------------------------------------- */
        if (accept) {
            for (j=0; j<spectra_dim; j++) imputedXS[j] = particle[j] + move + RNG_Gaussian()*2.0; 
            weight2 = nc->likelihood(nc, observation, imputedXS);
            if ((weight2>0.0) && (!NCPF_acceptParticle(ncpf, imputedXS))) weight2 = 0.0;
            if (((weight1==0) && (weight2!=0)) || ((weight1>0))) {
                memcpy(particle, imputedXS, spectra_dim*sizeof(float));
                weight1 = weight2;
            }
        }
    }
}

/*===================================================================================
/  obtainNoiseSample
/
/  initialize the particle population with samples 
/  from the gmm noise distribution
===================================================================================*/
void obtainNoiseSample(NCPF* ncpf, float* noiseSample, float* observation, int iterations)
{
    int                 spectra_dim = ncpf->speechModel->activeGMM->dim;
    NoiseCompensation*  nc          = ncpf->nc;
    NoiseModel*         noiseModel  = ncpf->noiseModel;
    GaussianMixture*    speechGMM   = ncpf->speechModel->activeGMM;
    int     i, j;
    int     accept;
    double  likelihood;
    float   ed;

    accept = 0;
    if (observation!=0) {
        i = 0;
        do {
            /* -------------------------------------------------------------------------
             get speech hypothesis
             ---------------------------------------------------------------------- */
            speechGMM->sample(noiseModel->gmm, noiseSample);

            /* -------------------------------------------------------------------------
             quick acceptance test
              ---------------------------------------------------------------------- */
            accept = 1;
            for (j=0; j<spectra_dim; j++)
		if (noiseSample[j]>observation[j]) accept = 0;

            /* -------------------------------------------------------------------------
             impute corresponding noise hypothesis and "real" acceptance test
             ---------------------------------------------------------------------- */
            if (accept) {
                for (j=0; j<spectra_dim; j++) {
                    ed = exp10F((noiseSample[j] - observation[j])*0.1);
                    if (ed>=1.0) accept = 0;
                    noiseSample[j] = observation[j] + 10.0*log10F(1.0 - ed);
                }
                if (accept) {
                    likelihood = nc->likelihood(nc, observation, noiseSample);
                    if ((likelihood==0.0) || (!isfiniteD(likelihood)) || (!NCPF_acceptParticle(ncpf, noiseSample)))
                        accept = 0;
                }
            }
            i++;
        }
        while ((i<iterations) && (!accept));
    }
    if (!accept) noiseModel->getNoiseSample(noiseModel, noiseSample);
}

/*===================================================================================
/  NCPF_initialize
/
/  initialize the particle population with samples 
/  from the gmm noise distribution
===================================================================================*/
void NCPF_initializeParticles(NCPF* ncpf, float* observation)
{
    int                 nParticles  = ncpf->pf.config.nParticles;
    float**             particles   = ncpf->pf.particles;
    int                 spectra_dim = ncpf->speechModel->activeGMM->dim;
    int                 dim         = ncpf->pf.config.dim;
    int     		i,j;

    for (i=0; i<nParticles; i++) {
        obtainNoiseSample(ncpf, particles[i], observation, 100);
	for (j=spectra_dim; j<dim; j++) particles[i][j] = 1.0;
    }
}

/*===================================================================================
/  NCPF_sampleParticles
===================================================================================*/
void NCPF_sampleParticlesSIMPLE(NCPF* ncpf, float* observation)
{
    int                 nParticles          = ncpf->pf.config.nParticles;
    float**             particles           = ncpf->pf.particles;
    float**             particlesHistory    = ncpf->pf.particlesHistory;
    NoiseModel*         noiseModel          = ncpf->noiseModel;
    int i;

    for (i=0; i<nParticles; i++) noiseModel->sampleFromPD(noiseModel, particlesHistory[i], particles[i]);
}

/*===================================================================================
/  NCPF_sampleParticlesFAT (Fast Acceptance Test)
===================================================================================*/
void NCPF_sampleParticlesFAT(NCPF* ncpf, float* observation)
{
    int                 dim                 = ncpf->pf.config.dim;
    int                 nParticles          = ncpf->pf.config.nParticles;
    float**             particles           = ncpf->pf.particles;
    float**             particlesHistory    = ncpf->pf.particlesHistory;
    NoiseModel*         noiseModel          = ncpf->noiseModel;
    int                 nRetries            = ncpf->config.FAT_nRetries;
    float*              spectra_delay       = ncpf->delay_noise;
    int                 spectra_dim         = ncpf->speechModel->activeGMM->dim;
    /*    float*              spectra_noise       = ncpf->spectra_noise; */
    float*              observation_mslp    = ncpf->observation_mslp;
    float*              noise_mslp          = ncpf->noise_mslp;
    int          	i, j, k;
    int          	accept;
    unsigned int 	next;
    float*       	particle;

    for (i=0; i<nParticles; i++) {
        particle = particles[i];
        k = 0;
        accept = 0;
        next = i;

        while ((k<nRetries) && (!accept)) {
	    noiseModel->sampleFromPD(noiseModel, particlesHistory[next], particle);
            accept = 1;
            for (j=0; j<spectra_dim; j++) {
		if (dim == spectra_dim) {
		    noise_mslp[j] = 10.0*log10F(exp10F(0.1*particles[i][j]));
		} else {
		    noise_mslp[j] = 10.0*log10F(exp10F(0.1*particles[i][j])+exp10F(0.1
		        *( particles[i][spectra_dim] + (particles[i][spectra_dim+1]-1.0)
			*((j-(spectra_dim-1.0)/2.0)/spectra_dim) ) * spectra_delay[j]));
		}
		observation_mslp[j] = observation[j];
                if (noise_mslp[j]>=observation_mslp[j]) accept = 0;
            }
            if (!accept) next = RNG_Int() % nParticles;
            k++;
        }
	for (j=0; j<dim; j++) particlesHistory[i][j]=particlesHistory[next][j];
    }
}

/*===================================================================================
/  MCMC_MOVE_particle
===================================================================================*/
double MCMC_MOVE_particle(NCPF* ncpf, float* observation, float* particle, double weight, int iterations)
{
    int                 dim         = ncpf->pf.config.dim;
    NoiseCompensation*  nc          = ncpf->nc;
    float*              imputedXS   = ncpf->imputedXS;
    int     i, j;
    int     accept;
    double  weight2;
    float   move;

    /* -------------------------------------------------------------------------
    /  MOVE: reamplify 
      ---------------------------------------------------------------------- */
    for (i=0; i<iterations; i++) {
        move = RNG_Gaussian() * 10.0;

        /* -------------------------------------------------------------------------
        /  QUICK ACCEPTANCE TEST
          ---------------------------------------------------------------------- */
        accept = 1;
        for (j=0; j<dim; j++) if (observation[j]<(particle[j]+move)) accept = 0;

        /* -------------------------------------------------------------------------
        /  MCMC STEP
          ---------------------------------------------------------------------- */
        if (accept) {
            for (j=0; j<dim; j++) imputedXS[j] = particle[j] + move + RNG_Float()*4.0; 
            weight2 = nc->likelihood(nc, observation, imputedXS);
            if ((weight2>0.0) && (!NCPF_acceptParticle(ncpf, imputedXS))) weight2 = 0.0;
            if (((weight==0) && (weight2!=0))) {
                memcpy(particle, imputedXS, dim*sizeof(float));
                weight = weight2;
            }
        }
    }

    return(weight);
}

/*===================================================================================
/  NCPF_weightParticles
===================================================================================*/
void NCPF_weightParticles_STD(NCPF* ncpf, float* observation)
{
    int                 nParticles  = ncpf->pf.config.nParticles;
    float**             particles   = ncpf->pf.particles;
    double*             weights     = ncpf->pf.weights;
    NoiseCompensation*  nc          = ncpf->nc;
    int     		i;
    float*  		particle;
    double  		weight;

    for (i=0; i<nParticles; i++) {
        particle = particles[i];
        weight = nc->likelihood(nc, observation, particle);
        if ((weight>0.0) && (!NCPF_acceptParticle(ncpf, particle))) weight = 0.0;
        weights[i] = weight;
    }
}

/*===================================================================================
/  NCPF weight particles including noise compensation
===================================================================================*/
void NCPF_weightParticles(NCPF* ncpf, float* observation)
{
    int                 dim                 = ncpf->pf.config.dim;
    int                 spectra_dim         = ncpf->speechModel->activeGMM->dim;
    int                 nParticles          = ncpf->pf.config.nParticles;
    float**             particles           = ncpf->pf.particles;
    double*             weights             = ncpf->pf.weights;
    NoiseCompensation*  nc                  = ncpf->nc;
    double*             imputedXD           = ncpf->imputedXD;
    float*              imputedXS           = ncpf->imputedXS;
    float*              imputedX            = ncpf->imputedX;
    float*              spectra_delay       = ncpf->delay_noise;
    /*    float*              spectra_noise       = ncpf->spectra_noise; */
    float*              observation_mslp    = ncpf->observation_mslp;
    float*              noise_mslp          = ncpf->noise_mslp;
    int     		i, j;
    double   weight, sumWeights, invSumWeights;

    /* -------------------------------------------------------------------------
    /  clear imputedX[j]
      ---------------------------------------------------------------------- */
    for (j=0; j<dim; j++) imputedXD[j] = 0.0;
    sumWeights = 0;

    /* -------------------------------------------------------------------------
    /  calculate particle-weights and impute clean speech (imputedX)
      ---------------------------------------------------------------------- */
    for (i=0; i<nParticles; i++) {
	for (j=0; j<spectra_dim; j++) {
	    observation_mslp[j] = observation[j];
	    if (dim == spectra_dim) {
		noise_mslp[j] = 10.0*log10F(exp10F(0.1*particles[i][j]));
	    } else {
	        noise_mslp[j] = 10.0*log10F(exp10F(0.1*particles[i][j])+exp10F(0.1
		    *( particles[i][spectra_dim] + (particles[i][spectra_dim+1]-1.0)
		    *((j-(spectra_dim-1.0)/2.0)/spectra_dim) ) * spectra_delay[j]));
	    }
	}
	weight = nc->compensate(nc, observation_mslp, noise_mslp, imputedXS);

        for (j=0; j<spectra_dim; j++) imputedXD[j] += weight*imputedXS[j];
        sumWeights += weight;
        weights[i] = weight;
    }

    /* -------------------------------------------------------------------------
      normalize & output
      ---------------------------------------------------------------------- */
    if (sumWeights>0.0) {
	invSumWeights = 1.0 / sumWeights;
        for (j=0; j<spectra_dim; j++) imputedX[j] = imputedXD[j]*invSumWeights;
    } else memcpy(imputedX, observation, spectra_dim*sizeof(float));
}

/*===================================================================================
/  NCPF move step
===================================================================================*/
void NCPF_moveStep(NCPF* ncpf, float* observation)
{
    int                 dim         = ncpf->pf.config.dim;
    int                 nParticles  = ncpf->pf.config.nParticles;
    float**             particles   = ncpf->pf.particles;
    double*             weights     = ncpf->pf.weights;
    NoiseCompensation*  nc          = ncpf->nc;
    float*              imputedXS   = ncpf->imputedXS;
    int     		i, j;
    double  		weight=0.5, weight1, weight2, rel;
    float   		move, u;
    float*  		particle;

    for (i=0; i<nParticles; i++) {
        move = RNG_Gaussian() * 4.0;

        /* -------------------------------------------------------------------------
          Variation
          ---------------------------------------------------------------------- */
        particle = particles[i];
        for (j=0; j<dim; j++) imputedXS[j] = particle[j] + move; /* RNG_Gaussian()*4.0; */

        /* -------------------------------------------------------------------------
          Likelihood Evaluation
          ---------------------------------------------------------------------- */
        weight1 = nc->likelihood(nc, observation, particle);
        if ((weight1>0.0) && (!NCPF_acceptParticle(ncpf, particle))) weight1 = 0.0;
        weight2 = nc->likelihood(nc, observation, imputedXS);
        if ((weight2>0.0) && (!NCPF_acceptParticle(ncpf, imputedXS))) weight2 = 0.0;

        /* -------------------------------------------------------------------------
          Metropolis Hastings Step
          ---------------------------------------------------------------------- */
        rel = 1.0;
        if (weight1!=0.0) rel = weight2/weight1; 
        u    = RNG_Float();
        weight = weight1;
        if (u <= rel) {
            memcpy(particle, imputedXS, dim*sizeof(float));
            weight = weight2;
        }
        weights[i] = weight;
    }
    weights[i] = weight;
}

/*===================================================================================
/  NCPF_run
===================================================================================*/
void NCPF_run(NCPF* ncpf, float* observation, float* imputedX, float* delay_noise, int frame)
{
    ParticleFilter* pf      = (ParticleFilter*) &(ncpf->pf);

    /* -------------------------------------------------------------------------
      remember where to store imputed clean speech
      ---------------------------------------------------------------------- */
    ncpf->imputedX = imputedX;
    ncpf->delay_noise = delay_noise;

    /* -------------------------------------------------------------------------
      run the pf
      ---------------------------------------------------------------------- */
    pf->run(pf, observation, frame);
}

/*===================================================================================
/  NCPF_initialize
===================================================================================*/
void NCPF_initialize(NCPF* ncpf, float* observation)
{
    ParticleFilter* pf      = (ParticleFilter*) &(ncpf->pf);
    pf->initialize(pf, observation);
}

/*===================================================================================
/  NCPF_setNIO
===================================================================================*/
void NCPF_setNIO(NCPF* ncpf, float nioInDB)
{
    NoiseModel*     noiseModel      = ncpf->noiseModel;
    noiseModel->setNIO(noiseModel, nioInDB);
}

/*===================================================================================
/  createNCPF
===================================================================================*/
NCPF* createNCPF(NoiseModel* noiseModel, SpeechModel* speechModel, ConfigNCPF configNCPF)
{
    void(*NCPF_sampleParticles)(NCPF* ncpf, float* observation);
    int     dim			= configNCPF.configPF.dim;
    NCPF*   ncpf;

    /* -------------------------------------------------------------------------
      allocate memory
      ---------------------------------------------------------------------- */
    ncpf = (NCPF*) malloc(sizeof(NCPF));
    memset(ncpf, 0, sizeof(NCPF));
    ncpf->imputedXS = (float*) malloc(dim*sizeof(float));
    ncpf->imputedXD = (double*) malloc(dim*sizeof(double));
    ncpf->spectra_noise = (float*) malloc((unsigned)(dim*sizeof(float)));
    ncpf->observation_mslp = (float*) malloc((unsigned)(dim*sizeof(float)));
    ncpf->noise_mslp = (float*) malloc((unsigned)(dim*sizeof(float)));

    /* -------------------------------------------------------------------------
      create noise compensation
      ---------------------------------------------------------------------- */
    speechModel->activeGMM = speechModel->determineLargestGMM(speechModel);
    (*ncpf).nc = createNoiseCompensation(&(speechModel->activeGMM), configNCPF.configNC);

    /* -------------------------------------------------------------------------
      initialize variables
      ---------------------------------------------------------------------- */
    (*ncpf).config                  = configNCPF;
    (*ncpf).noiseModel              = noiseModel;
    (*ncpf).speechModel             = speechModel;

    /* -------------------------------------------------------------------------
      link member functions
      ---------------------------------------------------------------------- */
    (*ncpf).run             = NCPF_run;
    (*ncpf).initialize      = NCPF_initialize;
    (*ncpf).setNIO          = NCPF_setNIO;

    /* -------------------------------------------------------------------------
      sampling method
      ---------------------------------------------------------------------- */
    NCPF_sampleParticles = NCPF_sampleParticlesSIMPLE;
    if ((configNCPF.flags & NCPF_CONFIG_ACCEPTANCE_TEST)==NCPF_CONFIG_USE_ACCEPTANCE_TEST)
        NCPF_sampleParticles = NCPF_sampleParticlesFAT;

    /* -------------------------------------------------------------------------
      initialize PF-part
      ---------------------------------------------------------------------- */
    initParticleFilter((ParticleFilter*)&(ncpf->pf), configNCPF.configPF, 
                    (void(*)(void* ncpf, float* observation))NCPF_initializeParticles,
                    (void(*)(void* ncpf, float* observation))NCPF_sampleParticles,
                    (void(*)(void* ncpf, float* observation))NCPF_weightParticles,
                    (void(*)(void* ncpf, float* observation))NCPF_updateAR,
                    (void(*)(void* ncpf, float* observation))NCPF_moveStep,
                    ncpf);
    return(ncpf);
}
