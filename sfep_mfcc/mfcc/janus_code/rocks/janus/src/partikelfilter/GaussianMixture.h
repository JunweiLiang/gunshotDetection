/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Gaussian Mixture
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  GaussianMixture.h
    Date    :  $Id: GaussianMixture.h 2844 2008-11-25 12:00:00Z wolfel $

   ========================================================================

    Revision 1.1  2008/11/25  12:00:00  wolfel
    Initial revision

   ======================================================================== */

#ifndef GAUSSIAN_MIXTURE_YET_INCLUDED
	#define GAUSSIAN_MIXTURE_YET_INCLUDED

/*-----------------------------------------------------------------------------
/  INCLUDES
/----------------------------------------------------------------------------*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RNG.h"
#include "Gaussian.h"
#include "DiscreteDistribution.h"
#include "mach_ind_io.h"

/******************************************************************************
/  struct <GaussianMixture>
******************************************************************************/
struct GaussianMixture
{
    /*-------------------------------------------------------------------------
    /  VARIABLES
    /------------------------------------------------------------------------*/
    int				dim;
    int				nMixtures;
    DiscreteDistribution*	weightDistribution;
    Gaussian**			gaussians;

    /*-------------------------------------------------------------------------
    /  INTERNAL DATA
    /------------------------------------------------------------------------*/
    float*			evaluationDWSupport;

    /*-------------------------------------------------------------------------
    /  FUNCTIONS
    /------------------------------------------------------------------------*/
    double      (*evaluate)(struct GaussianMixture* gaussianMixture, float* x);
    void        (*evaluateDW)(struct GaussianMixture* gaussianMixture, float* x, float* result);
    double      (*evaluateGNA)(struct GaussianMixture* gaussianMixture, float* x, double* activity);
    void        (*sample)(struct GaussianMixture* gaussianMixture, float* sample);
    void        (*translateMeans)(struct GaussianMixture* gaussianMixture, float* offset);
    void        (*destruct)(struct GaussianMixture* gmm);
} typedef GaussianMixture;

/******************************************************************************
/  constructors
******************************************************************************/
GaussianMixture*    createGaussianMixture(int dim, int nMixtures, Gaussian** gaussians, float* weights);
GaussianMixture*    createGaussianMixture2(GaussianMixture** gmix, float* weighting, int N);

#endif
