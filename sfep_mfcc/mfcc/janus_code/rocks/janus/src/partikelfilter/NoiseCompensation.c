/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Noise Compensation
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  NoiseCompensation.c
    Date    :  $Id: NoiseCompensation.c 2844 2008-11-25 12:00:00Z wolfel $
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
#include "NoiseCompensation.h"

static int GAUSSIAN_TABLE_INITIALIZED = 0;

/******************************************************************************
   imputeX
 *****************************************************************************/
/* __inline__ */
double imputeX(int dim, float* y, float* n, float* imputedX)
{
    float               diff, mod;
    double              normalizer;
    int                 j;

    normalizer = 1.0;
    for (j=0; j<dim; j++) {
        diff = n[j] - y[j];
        if (diff>=0) return(0.0);
        mod = 1.0 - exp10F(diff*0.1);
        normalizer *= mod;
        imputedX[j] = y[j] + 10.0*log10F(mod);
    }
    return(normalizer);
}

/*===================================================================================
/  NoiseCompensationMethod Statistical Interference Approach
===================================================================================*/
double NoiseCompensationMethodSIA(NoiseCompensation* compensation, float* y, float* n, float* imputedX)
{
    GaussianMixture*    speechGM    = (*compensation->speechGM);
    int                 dim         = speechGM->dim;
    double              likelihoodNormalizer, likelihood;
    int                 j;

    /*-------------------------------------------------------------------------
    /  obtain imputed x to put into p(x) and calculate the normalization factor
    /------------------------------------------------------------------------*/
    likelihoodNormalizer = imputeX(dim, y, n, imputedX);
    if (likelihoodNormalizer==0.0) {
        for (j=0; j<dim; j++) imputedX[j] = 0; 
    	return(0.0);
    }
    /*-------------------------------------------------------------------------
    /  Calculate p(y|n)
    /------------------------------------------------------------------------*/
    likelihood = (*speechGM).evaluate(speechGM, imputedX) / likelihoodNormalizer;

    return(likelihood);
}

/*===================================================================================
/  evaluateGNA(GNA = Gaussian Activity)
===================================================================================*/
double evaluateGNA(NoiseCompensation* compensation, float* y, float* n)
{
    GaussianMixture*    speechGM    = (*compensation->speechGM);
    double*             activity    = compensation->activity;
    float*              imputedX    = compensation->tempDFV;
    int                 dim         = speechGM->dim;
    double   		likelihood, likelihoodNormalizer;
    int      		j;

    /*-----------------------------------------------------------------
    /  obtain imputed x to put into p(x) and calculate the normalization factor
    /----------------------------------------------------------------*/   
    likelihoodNormalizer = imputeX(dim, y, n, imputedX);
    if (likelihoodNormalizer==0.0) {
        for (j=0; j<dim; j++) imputedX[j] = 0.0; 
    	return(0.0);
    }

    /*-----------------------------------------------------------------
    /  Calculate p(j|y,n) for all j    |   where j = mixtureNumber
    /----------------------------------------------------------------*/  
    likelihood = speechGM->evaluateGNA(speechGM, imputedX, activity) / likelihoodNormalizer;
    
    return(likelihood);
}

/*===================================================================================
/  NoiseCompensationLikelihood - OLDER, BUT FASTER VERSION
===================================================================================*/
double NoiseCompensationLikelihood(NoiseCompensation* compensation, float* y, float* n)
{
    double  likelihood;

    likelihood = evaluateGNA(compensation, y, n);

    return(likelihood);
}

/*===================================================================================
/  NoiseCompensationMethod based on Vector Taylor Series
/
/  basics, see:
/    (1) Moreno, Raj, Stern - A Vector Taylor Series Approach for Environment-
/        independent Speech Recognition
/    (2) Segura et al. - Model Based Compensation of the Additive Noise for
/        Continuous Speech Recognition.
===================================================================================*/
double NoiseCompensationMethodVTS(NoiseCompensation* compensation, float* y, float* n, float* imputedX)
{
    float               compensationAccuracy    = compensation->config.compensationAccuracy;
    double*             activity                = compensation->activity;
    GaussianMixture*    speechGM                = (*compensation->speechGM);
    Gaussian**          gaussians               = speechGM->gaussians;
    int                 dim                     = speechGM->dim;
    int                 nMixtures               = speechGM->nMixtures;
    float               diff, act;
    double              likelihood;
    int                 i,j;
    float*              mean;
    float               maxAct, actTreshold;
    float               actLoss, savedCC;

    likelihood = evaluateGNA(compensation, y, n);

    /*-------------------------------------------------------------------------
    /  initialize result with y
    /------------------------------------------------------------------------*/
    for (j=0; j<dim; j++) imputedX[j] = y[j];

    /*-------------------------------------------------------------------------
    /  zero likelihood => early exit
    /------------------------------------------------------------------------*/
    if (likelihood==0.0) return(likelihood);

    /*-------------------------------------------------------------------------
    /  get maxAct
    /------------------------------------------------------------------------*/
    maxAct = 0.0;
    for (i=0; i<nMixtures; i++) {
        act = (float) activity[i];
        if (act>maxAct) maxAct = act;
    }
    actTreshold = maxAct / ((nMixtures-1)*100.0) * ((1.0/compensationAccuracy)-1.0);
    
    /*-------------------------------------------------------------------------
    /  calculate result
    /------------------------------------------------------------------------*/
    actLoss = 0;
    savedCC = 0;
    for (i=0; i<nMixtures; i++)
    {
        mean = (*gaussians[i]).mean;
        act = (float) activity[i];
        if (act>=actTreshold) {              /* accelerated processing */
            for (j=0; j<dim; j++) {
                diff = (n[j] - mean[j]);
                imputedX[j] -= act * 10*log10F(1.0 + exp10F(diff*0.1));
            }
        } else {
            actLoss += act;             /* diagnostics, not necessary */
            savedCC ++;
        }
    }
    savedCC /= nMixtures;

    return(likelihood);
}

/*===================================================================================
/  createNoiseCompensation
===================================================================================*/
NoiseCompensation* createNoiseCompensation(GaussianMixture** speechGM, ConfigNC config)
{
    int                 dim         = (*speechGM)->dim;
    int                 nMixtures   = (*speechGM)->nMixtures;
    NoiseCompensation*  compensation;
    double*             activity;
    float*              tempDFV;

    /*-------------------------------------------------------------------------
    /  check nMixtures>0
    /------------------------------------------------------------------------*/
    if (nMixtures==0) {
        printf("[ERROR] createNoiseCompensation>\n");
        printf("    GaussianMixture::nMixtures == 0\n");
        printf("    Are you serious?\n");
        return(0);
    }

	/*-------------------------------------------------------------------------
	/  allocate memory for <NoiseCompensation> structure
	/------------------------------------------------------------------------*/
	compensation = (NoiseCompensation*) malloc(sizeof(NoiseCompensation));
	memset(compensation, 0, sizeof(NoiseCompensation));

    /*-------------------------------------------------------------------------
    /  allocate internal memory
    /------------------------------------------------------------------------*/
    activity = (double*) malloc(nMixtures*sizeof(double));
    tempDFV  = (float*) malloc(dim*sizeof(float));

    /*-------------------------------------------------------------------------
    /  fill structure
    /------------------------------------------------------------------------*/
    (*compensation).speechGM    = speechGM;
    (*compensation).config      = config;
    (*compensation).activity    = activity;
    (*compensation).tempDFV     = tempDFV;
    (*compensation).likelihood  = NoiseCompensationLikelihood;
   
    /*-------------------------------------------------------------------------
    /  select noise compensation method
    /------------------------------------------------------------------------*/
    switch (config.flags&NC_CONFIG_METHOD) {
        /*-------------------------------------------------------------------------
        /  very crude method, just for test purposes
        /------------------------------------------------------------------------*/
        case NC_CONFIG_METHOD_SIA:
            (*compensation).compensate  = NoiseCompensationMethodSIA;
        break;
        /*-------------------------------------------------------------------------
        /  stern's method and its derivations
        /------------------------------------------------------------------------*/
        case NC_CONFIG_METHOD_VTS:
            (*compensation).compensate  = NoiseCompensationMethodVTS;
        break;
        /*-------------------------------------------------------------------------
        /  unknown method ?
        /------------------------------------------------------------------------*/
        default:
            printf("[ERROR] Unknown compensation method [%i]!\n", config.flags&NC_CONFIG_METHOD);
            exit(1);
        break;
    }

    /*-------------------------------------------------------------------------
    /  make sure the tables are initialized
    /------------------------------------------------------------------------*/
    if (GAUSSIAN_TABLE_INITIALIZED==0) {
        createGaussianTable(100000, 1E-30);
        GAUSSIAN_TABLE_INITIALIZED = 1;
    }

    return(compensation);
}
