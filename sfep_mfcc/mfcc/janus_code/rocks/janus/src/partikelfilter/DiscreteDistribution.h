/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Discrete Distribution
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  ARModul.h
    Date    :  $Id: ARModul.c 2844 2008-11-25 12:00:00Z wolfel $

   ========================================================================

    Revision 1.1  2008/11/25  12:00:00  wolfel
    Initial revision

   ======================================================================== */

#ifndef DISCRETE_DISTRIBUTION_YET_INCLUDED
	#define DISCRETE_DISTRIBUTION_YET_INCLUDED

/*-----------------------------------------------------------------------------
/  INCLUDES
/----------------------------------------------------------------------------*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RNG.h"

/*===================================================================================
/  struct
===================================================================================*/
struct DiscreteDistribution
{
	/*-------------------------------------------------------------------------
	/  VARIABLES
	/------------------------------------------------------------------------*/
	int		dim;
	int		nPoints;
	float**		points;
	float*		probabilities;

	/*-------------------------------------------------------------------------
	/  INTERNAL DATA
	/------------------------------------------------------------------------*/
	float*		cumulativeProbabilities;
	int		log2NPoints;				
	int		pow2NPoints;		/* closest power of 2 containing nPoints */
	
	/*-------------------------------------------------------------------------
	/  FUNCTIONS
	/------------------------------------------------------------------------*/
	float		(*evaluate)(struct DiscreteDistribution* distribution, float* x);
	void		(*sample)(struct DiscreteDistribution* distribution, float* sample);
	void		(*destruct)(struct DiscreteDistribution* distribution);
} typedef DiscreteDistribution;

/*===================================================================================
/  functions
===================================================================================*/
DiscreteDistribution* createDiscreteDistribution(int dim, int nValues, float** point, float* probabilities);
int initDiscreteDistribution(DiscreteDistribution* distribution, int dim, int nValues, float** points, float* probabilities);

#endif
