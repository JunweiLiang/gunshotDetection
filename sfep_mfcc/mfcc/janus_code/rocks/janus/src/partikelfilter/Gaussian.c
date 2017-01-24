/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Gaussian
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  Gaussian.c
    Date    :  $Id: Gaussian.c 2844 2008-11-25 12:00:00Z wolfel $
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
#include "Gaussian.h"

/*-----------------------------------------------------------------------------
/  static variables
/----------------------------------------------------------------------------*/
/*
static float* GAUSSIAN_TABLE_DATA         = 0;
static int    GAUSSIAN_TABLE_RESOLUTION   = 10000;
static float  GAUSSIAN_TABLE_CUTOFF_VALUE = 1E-20;
*/
/*===================================================================================
/  sampleFromDiagonalGaussian
===================================================================================*/
void sampleFromDiagonalGaussian(Gaussian* gaussian, float* sample)
{
    int	   dim			= (*gaussian).dim;
    float*     mean			= (*gaussian).mean;
    float*     sqrtVariance 	= (*gaussian).sqrtVariance;
    int i;

    /*-------------------------------------------------------------------------
    /  gererate sample
    /------------------------------------------------------------------------*/
    for (i=0; i<dim; i++) sample[i] = (RNG_Gaussian() * sqrtVariance[i]) + mean[i];
}

/*===================================================================================
/  evaluateDiagonalGaussian
===================================================================================*/
double evaluateDiagonalGaussian(Gaussian* gaussian, float* x)
{
    int 	dim			= (*gaussian).dim;
    float* 	mean			= (*gaussian).mean;
    float* 	invCovariance 		= (*gaussian).invCovariance;
    float 	normalizer		= (*gaussian).normalizer;
    int 	i;
    float 	sum, dist;
    double 	result;

    sum = 0;
    for (i=0; i<dim; i++) {
	dist = (x[i] - mean[i]);
	sum -= (dist*dist) * invCovariance[i];		/* invCovariance = covariance^(-1) */
    }
    result = normalizer * exp(sum*0.5);

    return(result);
}

/*===================================================================================
/  evaluateDiagonalGaussianDW (DW = dimensionwise)
===================================================================================*/
void evaluateDiagonalGaussianDW(Gaussian* gaussian, float* x, float* result)
{
    int	dim	= (*gaussian).dim;
    float*	mean		= (*gaussian).mean;
    float*	invCovariance 	= (*gaussian).invCovariance;	/* invCovariance = covariance^(-1) */
    float*	normalizerD	= (*gaussian).normalizerD;
    int 	i;
    float 	dist, exponent;

    for (i=0; i<dim; i++){
	dist = (x[i] - mean[i]);
	exponent = - (dist*dist) * invCovariance[i];
	result[i] = normalizerD[i] * exp(exponent*0.5);
    }
}

/*===================================================================================
/  updateMean
===================================================================================*/
void Gaussian_updateMean(Gaussian* gaussian, float* mean)
{
    if ((gaussian!=0) && (mean!=0))
        memcpy((*gaussian).mean, mean, (*gaussian).dim*sizeof(float));
}

/*===================================================================================
/  translateMean
===================================================================================*/
void Gaussian_translateMean(Gaussian* gaussian, float* offset)
{
    int         dim     = gaussian->dim;
    float*      mean    = gaussian->mean;
    int i;

    if ((gaussian!=0) && (mean!=0)) for (i=0; i<dim; i++) mean[i] += offset[i];
}

/*===================================================================================
/  initDiagonalCovariance
/
/  call this if the covariance has been changed
/  not intended for cases where the covariance changes often
===================================================================================*/
int initDiagonalCovariance(Gaussian* gaussian)
{

    int		dim			= (*gaussian).dim;
    float* 	covariance		= (*gaussian).covariance;
    float* 	invCovariance 		= (*gaussian).invCovariance;
    float* 	normalizerD 		= (*gaussian).normalizerD;
    float*  	sqrtVariance    	= (*gaussian).sqrtVariance;
    float*  	invSqrtVariance 	= (*gaussian).invSqrtVariance;
    int 	i;
    double	normalizer;

    /*-------------------------------------------------------------------------
    /  check covariance
    /------------------------------------------------------------------------*/
    for (i=0; i<dim; i++)
	if (covariance[i]<=0) {
	    printf("[ERROR] initDiagonalCovariance> COVARIANCE-MATRIX EXPECTED TO BE POSITIVE-DEFINITE!\n");
	    printf("  Your program will suck terribly ...\n");
	}

    /*-------------------------------------------------------------------------
    /  precalculated inverse covariance
    /------------------------------------------------------------------------*/
    for (i=0; i<dim; i++) invCovariance[i] = 1.0 / covariance[i];
    (*gaussian).invCovariance = invCovariance;

    /*-------------------------------------------------------------------------
    /  precalculated sqrt(covariance)
    /------------------------------------------------------------------------*/
    for (i=0; i<dim; i++) sqrtVariance[i] = sqrt(covariance[i]);
    (*gaussian).sqrtVariance = sqrtVariance;

    /*-------------------------------------------------------------------------
    /  precalculated sqrt(covariance)^(-1)
    /------------------------------------------------------------------------*/
    for (i=0; i<dim; i++) invSqrtVariance[i] = 1.0 / sqrtVariance[i];
    (*gaussian).invSqrtVariance = invSqrtVariance;

    /*-------------------------------------------------------------------------
    /  normalizer
    /------------------------------------------------------------------------*/
    normalizer = 1.0;
    float norm;
    for (i=0; i<dim; i++) {
	norm = 1.0 / sqrt(2.0*M_PI*covariance[i]);
	normalizerD[i] = norm;
	normalizer *= norm;
    }
    (*gaussian).normalizer = normalizer;
    (*gaussian).normalizerD = normalizerD;

    return(1);
}

/*===================================================================================
/  initDiagonalCovariance
/
/  call this if the covariance has changed
/  not intended for cases where the covariance changes often
===================================================================================*/
int initDiagonalGaussian(Gaussian* gaussian)
{
    int		dim			= (*gaussian).dim;
    float* 	invCovariance 		= (*gaussian).invCovariance;
    float* 	normalizerD 		= (*gaussian).normalizerD;
    float*  	sqrtVariance    	= (*gaussian).sqrtVariance;
    float*  	invSqrtVariance 	= (*gaussian).invSqrtVariance;

    /*-------------------------------------------------------------------------
    /  allocate memory
    /------------------------------------------------------------------------*/
    if (invCovariance!=0) free(invCovariance);
    if (sqrtVariance!=0) free(sqrtVariance);
    if (invSqrtVariance!=0) free(invSqrtVariance);
    if (normalizerD!=0) free(normalizerD);
    invCovariance = (float*) malloc(sizeof(float)*dim);
    sqrtVariance = (float*) malloc(sizeof(float)*dim);
    invSqrtVariance = (float*) malloc(sizeof(float)*dim);
    normalizerD = (float*) malloc(sizeof(float)*dim);
    (*gaussian).invCovariance = invCovariance;
    (*gaussian).sqrtVariance = sqrtVariance;
    (*gaussian).invSqrtVariance = invSqrtVariance;
    (*gaussian).normalizerD = normalizerD;

    return(initDiagonalCovariance(gaussian));
}

/*===================================================================================
/  Gaussian_destruct
===================================================================================*/
void Gaussian_destruct(Gaussian* gaussian)
{
    free(gaussian->normalizerD);
    free(gaussian->invCovariance);
    free(gaussian->invSqrtVariance);
    free(gaussian->covariance);
    free(gaussian->mean);
    free(gaussian);
}

/*===================================================================================
/  clone Gaussian
===================================================================================*/
Gaussian* Gaussian_clone(Gaussian* gaussian)
{
    int         dim             = gaussian->dim; 
    int         covarianceType  = gaussian->covarianceType;
    float*      mean            = gaussian->mean;
    float*      covariance      = gaussian->covariance; 
    Gaussian*   clonedGaussian;
    float*      clonedMean;
    float*      clonedCovariance;
    int         covarianceSize;

    /*-------------------------------------------------------------------------
    /  clone mean
    /------------------------------------------------------------------------*/    
    clonedMean = (float*) malloc(dim*sizeof(float));
    memcpy(clonedMean, mean, dim*sizeof(float));

    /*-------------------------------------------------------------------------
    /  determine covarianceSize
    /------------------------------------------------------------------------*/    
    covarianceSize = 0;
    covarianceSize = dim;

    /*-------------------------------------------------------------------------
    /  clone covariance
    /------------------------------------------------------------------------*/    
    clonedCovariance = (float*) malloc(covarianceSize*sizeof(float));
    memcpy(clonedCovariance, covariance, covarianceSize*sizeof(float));

    /*-------------------------------------------------------------------------
    /  create gaussian
    /------------------------------------------------------------------------*/
    clonedGaussian = createGaussian(dim, clonedMean, clonedCovariance, covarianceType);

    return(clonedGaussian);
}

/*===================================================================================
/  initGaussian
===================================================================================*/
int initGaussian(Gaussian* gaussian, int dim, float* mean, float* covariance, int covarianceType)
{
    /*-------------------------------------------------------------------------
    /  fill "Gaussian"-strcuture
    /------------------------------------------------------------------------*/
    memset(gaussian, 0, sizeof(Gaussian));
    (*gaussian).dim 			= dim;
    (*gaussian).mean 			= mean;
    (*gaussian).covariance		= covariance;

    if (!initDiagonalGaussian(gaussian)) return(0);
    (*gaussian).evaluate 	  = evaluateDiagonalGaussian;
    (*gaussian).evaluateDW 	  = evaluateDiagonalGaussianDW;
    (*gaussian).sample   	  = sampleFromDiagonalGaussian;

    /*-------------------------------------------------------------------------
    /  assign general functions
    /------------------------------------------------------------------------*/
    (*gaussian).clone         = Gaussian_clone;
    (*gaussian).updateMean    = Gaussian_updateMean;
    (*gaussian).translateMean = Gaussian_translateMean;
    (*gaussian).destruct      = Gaussian_destruct;

     return(1);
}

/*===================================================================================
/  createGaussian
===================================================================================*/
Gaussian* createGaussian(int dim, float* mean, float* covariance, int covarianceType)
{
    Gaussian* gaussian;

    /*-------------------------------------------------------------------------
    /  make sure the table is initialized before using it
    /------------------------------------------------------------------------*/    
    gaussian = (Gaussian*) malloc(sizeof(Gaussian));
    if (!initGaussian(gaussian, dim, mean, covariance, covarianceType)) {
	free(gaussian);
	gaussian = 0;
    }

    return(gaussian);
}
