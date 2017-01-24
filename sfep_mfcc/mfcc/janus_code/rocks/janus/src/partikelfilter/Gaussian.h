/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Gaussian
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  Gaussian.h
    Date    :  $Id: Gaussian.h 2844 2008-11-25 12:00:00Z wolfel $

   ========================================================================

    Revision 1.1  2008/11/25  12:00:00  wolfel
    Initial revision

   ======================================================================== */

#ifndef GAUSSIAN_YET_INCLUDED
	#define GAUSSIAN_YET_INCLUDED

/*-----------------------------------------------------------------------------
/  INCLUDES
/----------------------------------------------------------------------------*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RNG.h"
#include "mach_ind_io.h"

/*-----------------------------------------------------------------------------
/  CONSTANTS
/----------------------------------------------------------------------------*/
enum COVARIANCE_TYPE {SPHERICAL_COVARIANCE, DIAGONAL_COVARIANCE};

/******************************************************************************
/  struct
******************************************************************************/
struct Gaussian
{
    /*-------------------------------------------------------------------------
    /  VARIABLES
    /------------------------------------------------------------------------*/
    int         covarianceType;	
    int		dim;
    float*	mean;
    float*	covariance;

    /*-------------------------------------------------------------------------
    /  INTERNAL DATA (PRECALCULATIONS)
    /------------------------------------------------------------------------*/
    float*	invCovariance;		/* precalculated covariance^(-1) */
    float*      sqrtVariance;           /* precalculated sqrt(covariance) */
    float*      invSqrtVariance;        /* precalculated sqrt(covariance)^(-1) */
    double	normalizer;		/* overall normalizer for the gaussian distribution */
					/* can get pretty small, thererfore double */		
    float*	normalizerD;		/* normalizers for individual dimensions */

    /*-------------------------------------------------------------------------
    /  FUNCTIONS
    /------------------------------------------------------------------------*/
    double             (*evaluate)(struct Gaussian* gaussian, float* x);
    void               (*evaluateDW)(struct Gaussian* gaussian, float* x, float* result);
    void               (*updateMean)(struct Gaussian* gaussian, float* mean);
    void               (*translateMean)(struct Gaussian* gaussian, float* offset);
    void               (*sample)(struct Gaussian* gaussian, float* sample);
    void               (*destruct)(struct Gaussian* gaussian);
    struct Gaussian*   (*clone)(struct Gaussian* gaussian);
} typedef Gaussian;

/******************************************************************************
/  functions
******************************************************************************/
int         initGaussian(Gaussian* gaussian, int dim, float* mean, float* covariance, int covarianceType);
Gaussian*   createGaussian(int dim, float* mean, float* covariance, int covarianceType);

#endif
