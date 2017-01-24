/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Gaussian Mixture
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  GaussianMixture.c
    Date    :  $Id: GaussianMixture.c 2844 2008-11-25 12:00:00Z wolfel $
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
#include "GaussianMixture.h"

/*===================================================================================
/  translateMeans
===================================================================================*/
void GaussianMixture_translateMeans(GaussianMixture* mixture, float* offset)
{
    int         nMixtures  = (*mixture).nMixtures;
    Gaussian**  gaussians  = (*mixture).gaussians;
    int         i;
    Gaussian*   gaussian;

    for (i=0; i<nMixtures; i++) {
        gaussian = gaussians[i];
        (*gaussian).translateMean(gaussian, offset);
    }
}

/*===================================================================================
/  sampleFromGaussianMixture
===================================================================================*/
void sampleFromGaussianMixture(GaussianMixture* mixture, float* sample)
{
    int 			nMixtures		= (*mixture).nMixtures;
    Gaussian** 			gaussians		= (*mixture).gaussians;
    DiscreteDistribution* 	weightDistribution	= (*mixture).weightDistribution;
    int 			i;
    float			u;
    Gaussian*			gaussian;

    /*-------------------------------------------------------------------------
    /  make sure nMixtures > 0		|  otherwise return NAN 
    /------------------------------------------------------------------------*/
    if (nMixtures<=0) {
	for (i=0; i<nMixtures; i++) sample[i] = sqrt(-1);
	return;
    }

    /*-------------------------------------------------------------------------
    /  select a gaussian with prior probability "weights"
    /------------------------------------------------------------------------*/
    (*weightDistribution).sample(weightDistribution, &u);
    gaussian = gaussians[(int)u];

    /*-------------------------------------------------------------------------
    /  gererate sample
    /------------------------------------------------------------------------*/
    (*gaussian).sample(gaussian, sample);
}

/*===================================================================================
/  evaluateGaussianMixture
===================================================================================*/
double evaluateGaussianMixture(GaussianMixture* mixture, float* x)
{
    int         nMixtures  = (*mixture).nMixtures;
    Gaussian**  gaussians  = (*mixture).gaussians;
    float*      weights    = (*(*mixture).weightDistribution).probabilities;
    int         i;
    float       weight;
    double      result, gr;
    Gaussian*	gaussian;

    result = 0.0;
    for (i=0; i<nMixtures; i++) {
        gaussian = gaussians[i];
        weight   = weights[i];
        gr       = (*gaussian).evaluate(gaussian,x);
        result  += gr * weight;
    }

    return(result);
}

/*===================================================================================
/  evaluateGaussianMixtureGNA (GNA = Gaussian Normalized Activity)
===================================================================================*/
double evaluateGaussianMixtureGNA(GaussianMixture* mixture, float* x, double* activity)
{
    int 		nMixtures	= (*mixture).nMixtures;
    Gaussian** 		gaussians	= (*mixture).gaussians;
    float* 		weights		= (*(*mixture).weightDistribution).probabilities;
    
    int 		i;
    float 		weight;
    double      	gr, likelihood, likelihoodI;
    Gaussian*		gaussian;
    
    /*-------------------------------------------------------------------------
    /  calculate "activity" by evaluating the gaussians and
    /  and storing the results
    /------------------------------------------------------------------------*/
    likelihood = 0.0;
    for (i=0; i<nMixtures; i++)  {
        gaussian    = gaussians[i];
        weight      = weights[i];
        gr          = (*gaussian).evaluate(gaussian,x) * weight;
        activity[i] = gr;
        likelihood += gr;
    }

    /*-------------------------------------------------------------------------
    /  normalize
    /------------------------------------------------------------------------*/
    likelihoodI = 1.0 / likelihood; 
    for (i=0; i<nMixtures; i++) activity[i] *= likelihoodI;

    return(likelihood);
}

/*===================================================================================
/  evaluateGaussianMixtureGA (GA = Gaussian Activity)
===================================================================================*/
void evaluateGaussianMixtureGA(GaussianMixture* mixture, float* x, double* activity)
{
    int             	nMixtures           = (*mixture).nMixtures;
    Gaussian**      	gaussians           = (*mixture).gaussians;
    int         	i;
    Gaussian*   	gaussian;
    
    /*-------------------------------------------------------------------------
    /  calculate "activity" by evaluating the gaussians and
    /  and storing the results
    /------------------------------------------------------------------------*/
    for (i=0; i<nMixtures; i++)  {
        gaussian = gaussians[i];
        activity[i]  = (*gaussian).evaluate(gaussian,x);
    }
}

/*===================================================================================
/  evaluateDiagonalGaussianDW (DW = dimensionwise)
===================================================================================*/
void evaluateGaussianMixtureDW(GaussianMixture* mixture, float* x, float* result)
{
    int             	nMixtures           = (*mixture).nMixtures;
    int             	dim                 = (*mixture).dim;
    Gaussian**      	gaussians           = (*mixture).gaussians;
    float*          	evaluationDWSupport = (*mixture).evaluationDWSupport;
    float*          	weights             = (*(*mixture).weightDistribution).probabilities;
    int         	i,j;
    float       	weight;
    Gaussian*   	gaussian;
    
    /*-------------------------------------------------------------------------
    /  clear "result"
    /------------------------------------------------------------------------*/
    for (j=0; j<dim; j++) result[j] = 0.0;
    
    /*-------------------------------------------------------------------------
    /  calculate "result" by evaluating the gaussians
    /  and adding them up, weighted
    /------------------------------------------------------------------------*/
    for (i=0; i<nMixtures; i++) {
       gaussian = gaussians[i];
       weight   = weights[i];
       (*gaussian).evaluateDW(gaussian, x, evaluationDWSupport);
       for (j=0; j<dim; j++) result[j] += evaluationDWSupport[j] * weight;
    }
}

/*===================================================================================
/  GaussianMixture_destruct
===================================================================================*/
void GaussianMixture_destruct(GaussianMixture* gmm)
{
    int                     	nMixtures           = gmm->nMixtures;
    Gaussian**              	gaussians           = gmm->gaussians;
    int 			i;

    for (i=0; i<nMixtures; i++) gaussians[i]->destruct(gaussians[i]);

    free(gmm);
}

/*===================================================================================
/  initGaussianMixture
===================================================================================*/
int initGaussianMixture(GaussianMixture* mixture, int dim, int nMixtures, Gaussian** gaussians, float* weights)
{
    float*				evaluationDWSupport 	= (*mixture).evaluationDWSupport;
    int 				i;
    DiscreteDistribution* 		weightDistribution;
    float* 				points;
    float** 			ppoints;

    /*-------------------------------------------------------------------------
    /  check nMixtures>0
    /------------------------------------------------------------------------*/
    if (nMixtures==0) printf("[ERROR] initGaussianMixture> nMixtures == 0\n");

    /*-------------------------------------------------------------------------
    /  check that all gaussians are not null and have dimension "dim"
    /------------------------------------------------------------------------*/
    for (i=0; i<nMixtures; i++) {
	if (gaussians[i]==0) {
	    printf("[ERROR] initGaussianMixture>\n gaussian[%i] %d == null\n", i, dim);
	    return(0);
	}
	if ((*gaussians[i]).dim!=dim) {
	    printf("[ERROR] initGaussianMixture>\n"); 
	    printf("    gaussian[%i].dim=%i != %i=dim\n", i, (*gaussians[i]).dim, dim);
	    printf("    Might not be such a great idea ...\n");
	    return(0);			
	}
    }

    /*-------------------------------------------------------------------------
    /  allocate memory
    /------------------------------------------------------------------------*/
    points = malloc(sizeof(float)*nMixtures);
    if (points==0) {
	printf("[ERROR] initGaussianMixture>\n");
	printf("    MEMORY ALLOCATION FAILED!\n");
	return(0);
    }
    ppoints = malloc(sizeof(float*)*nMixtures);
    if (ppoints==0) {
	free(points);
	printf("[ERROR] initGaussianMixture>\n");
	printf("    MEMORY ALLOCATION FAILED!\n");
	return(0);
    }
    if (evaluationDWSupport!=0) free(evaluationDWSupport);
    evaluationDWSupport = malloc(sizeof(float)*dim);
    if (evaluationDWSupport==0) {
	free(points);
	free(ppoints);
	printf("[ERROR] initGaussianMixture>\n");
	printf("    MEMORY ALLOCATION FAILED!\n");
	return(0);
    }

    /*-------------------------------------------------------------------------
    /  create weight-distribution
    /------------------------------------------------------------------------*/
    for (i=0; i<nMixtures; i++) points[i] = (float)i;
    for (i=0; i<nMixtures; i++) ppoints[i] = &(points[i]);
    weightDistribution = createDiscreteDistribution(1, nMixtures, ppoints, weights);

    /*-------------------------------------------------------------------------
    /  fill "GaussianMixture"-structure
    /------------------------------------------------------------------------*/
    (*mixture).nMixtures 			= nMixtures;
    (*mixture).dim					= dim;
    (*mixture).gaussians			= gaussians;
    (*mixture).weightDistribution	= weightDistribution;
    (*mixture).evaluationDWSupport	= evaluationDWSupport;

    /*-------------------------------------------------------------------------
    /  link member-functions
    /------------------------------------------------------------------------*/
    (*mixture).evaluate        = evaluateGaussianMixture;
    (*mixture).evaluateDW      = evaluateGaussianMixtureDW;
    (*mixture).evaluateGNA     = evaluateGaussianMixtureGNA;
    (*mixture).sample          = sampleFromGaussianMixture;
    (*mixture).translateMeans  = GaussianMixture_translateMeans;
    (*mixture).destruct        = GaussianMixture_destruct;

    return(1);
}

/*===================================================================================
/  createGaussianMixture
===================================================================================*/
GaussianMixture* createGaussianMixture(int dim, int nMixtures, Gaussian** gaussians, float* weights)
{
    /*-------------------------------------------------------------------------
    /  allocate memory
    /------------------------------------------------------------------------*/
    GaussianMixture* mixture = (GaussianMixture*) malloc(sizeof(GaussianMixture));
    if (mixture==0)	{
	printf("[ERROR] createGaussianMixture>\n");
	printf("    MEMORY ALLOCATION FAILED!\n");
	return(0);
    }
    memset(mixture, 0, sizeof(GaussianMixture));

    /*-------------------------------------------------------------------------
    /  initialize
    /------------------------------------------------------------------------*/
    if (!initGaussianMixture(mixture, dim, nMixtures, gaussians, weights)) {
	free(mixture);
	mixture = 0;
    }

    return(mixture);
}

/*===================================================================================
/  createGaussianMixture
/  create a mixture of "n" gaussian mixtures using the mixture-"weights".
===================================================================================*/
GaussianMixture* createGaussianMixture2(GaussianMixture** gmix, float* weighting, int N)
{
    GaussianMixture*    mixture;
    int                 nMixtures, dim;
    Gaussian**          gaussians;
    float*              weights;
    int                 i, j, nMixturesL, nMixturesC;
    float               sumWeighting;
    Gaussian**          gaussiansL;
    float*              weightsL;

    /*-------------------------------------------------------------------------
    /  catch bad weights
    /------------------------------------------------------------------------*/
    sumWeighting = 0.0;
    for (i=0; i<N; i++) sumWeighting += weighting[i];
    if (sumWeighting==0.0) {
        printf("[ERROR] mixNGaussianMixtures>\n");
        printf("    SUM OF WEIGHTS IS ZERO!\n");
        return(0);
    }
    if (sumWeighting!=1.0) {
        printf("[WARNING] mixNGaussianMixtures>\n");
        printf("    WEIGHTS DON'T SUM TO ONE [%f]! CORRECTED ...\n", 
               sumWeighting);
        for (i=0; i<N; i++) weighting[i] /= sumWeighting;
    }

    /*-------------------------------------------------------------------------
    /  catch dimensional mismatch
    /------------------------------------------------------------------------*/
    for (i=1; i<N; i++)
        if (gmix[0]->dim != gmix[i]->dim) {
            printf("[ERROR] mixTwoGaussianMixtures>\n");
            printf("    dimensional mismatch %i vs. %i \n", gmix[0]->dim, gmix[i]->dim);
            exit(1);
        }

    /*-------------------------------------------------------------------------
    /  init vars
    /------------------------------------------------------------------------*/
    dim = gmix[0]->dim;
    nMixtures = 0;
    for (i=0; i<N; i++) nMixtures += gmix[i]->nMixtures;

    /*-------------------------------------------------------------------------
    /  allocate memory
    /------------------------------------------------------------------------*/
    gaussians = (Gaussian**) malloc(nMixtures*sizeof(Gaussian*));
    if (gaussians==0) {
        printf("[ERROR] mixTwoGaussianMixtures>\n");
        printf("    MEMORY ALLCOATION FAILED!\n");
        exit(1);        
    }
    weights = (float*) malloc(nMixtures*sizeof(float));
    if (weights==0) {
        free(gaussians);
        printf("[ERROR] mixTwoGaussianMixtures>\n");
        printf("    MEMORY ALLCOATION FAILED!\n");
        exit(1);
    }

    /*-------------------------------------------------------------------------
    /  join gaussians & weights
    /------------------------------------------------------------------------*/
    nMixturesC = 0;
    for (i=0; i<N; i++) {
        nMixturesL = gmix[i]->nMixtures;
        gaussiansL = gmix[i]->gaussians;
        weightsL   = gmix[i]->weightDistribution->probabilities;
        for (j=0; j<nMixturesL; j++) {
            gaussians[j+nMixturesC] = gaussiansL[j];
            weights[j+nMixturesC]   = weightsL[j] * weighting[i];
        }
        nMixturesC += nMixturesL;
    }

    /*-------------------------------------------------------------------------
    /  create gaussian mixture
    /------------------------------------------------------------------------*/
    mixture = createGaussianMixture(dim, nMixtures, gaussians, weights);

    return(mixture);
}
