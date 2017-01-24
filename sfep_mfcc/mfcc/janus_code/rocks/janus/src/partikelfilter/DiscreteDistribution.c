/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Discrete Distribution
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  DiscreteDistribution.c
    Date    :  $Id: DiscreteDistribution.c 2844 2008-11-25 12:00:00Z wolfel $
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

/* -----------------------------------------------------------------------------
   INCLUDES
   -------------------------------------------------------------------------- */
#include "DiscreteDistribution.h"

/*===================================================================================
/  sampleFromDiscreteDistribution
===================================================================================*/
void sampleFromDiscreteDistribution(DiscreteDistribution* distribution, float* sample)
{
	int 		dim						= (*distribution).dim;
	int 		nPoints					= (*distribution).nPoints;
	int 		log2NPoints				= (*distribution).log2NPoints;
	int 		pow2NPoints				= (*distribution).pow2NPoints;
	float** 	points					= (*distribution).points;
	float* 		cumulativeProbabilities	= (*distribution).cumulativeProbabilities;
	int 		i, pos, searchWidth;
	float		u;
	int			size = sizeof(float)*dim;

	/* -------------------------------------------------------------------------
	   make sure nPoints > 0		|  otherwise return NAN 
	  ---------------------------------------------------------------------- */
	if (nPoints<=0) {
		(*sample) = sqrt(-1);
		return;
	}

	/* -------------------------------------------------------------------------
	   get a unitary sample and initialize
	   ---------------------------------------------------------------------- */
	u = RNG_Float();
	searchWidth = (pow2NPoints >> 1);
	pos = searchWidth;

	/* -------------------------------------------------------------------------
	   walk through the ocean ("cumulativeProbabilities") in your search for u
	   meaning:
	       find "pos" such that 
	 	   cumulativeProbabilities[pos-1] < u < cumulativeProbabilities[pos]
	   - cumulativeProbabilities is sorted in ascending order
	   - cumulativeProbabilities is padded so that its length is a power of 2
	     making sure we don't get into trouble.
	  ---------------------------------------------------------------------- */
	for (i=1; i<log2NPoints; i++) {
		searchWidth >>= 1;							/* halve searchwidth */
		if (u<=cumulativeProbabilities[pos]) pos -= searchWidth;		/* search to the left/right ? */
		else pos += searchWidth;
	}

	/* -------------------------------------------------------------------------
	   final correction step (if necessary)
	   ---------------------------------------------------------------------- */
	if ((pos>0) && (u<cumulativeProbabilities[pos-1])) pos -= 1;
	if ((u>cumulativeProbabilities[pos]) && ((pos+1)<nPoints)) pos += 1;

	/* -------------------------------------------------------------------------
	   copy the point found at "pos" into "sample"
	   ---------------------------------------------------------------------- */
	memcpy(sample, points[pos], size);
}

/*===================================================================================
   evaluateDiscreteDistribution
===================================================================================*/
float evaluateDiscreteDistribution(DiscreteDistribution* distribution, float* x)
{
	int 	dim				= (*distribution).dim;
	int 	nPoints			= (*distribution).nPoints;
	float**	points 			= (*distribution).points;
	float*  probabilities 	= (*distribution).probabilities;
	int	i;
	int	size = sizeof(float)*dim;

	for (i=0; i<nPoints; i++) if (memcmp(points[i], x, size)==0) return(probabilities[i]);
	return(0.0);
}

/*===================================================================================
/  log2ceil
===================================================================================*/
/* inline */
unsigned int log2ceil(unsigned int i)
{
	unsigned int pow2, l;

	l = 0;
	pow2 = 1;
	while ((pow2<i) && (pow2!=0)) {
		pow2 <<= 1;
		l++;
	}

	return(l);
}

/*===================================================================================
/  pow2ceil
===================================================================================*/
/* inline */
unsigned int pow2ceil(unsigned int i)
{
	unsigned int pow2;

	pow2 = 1;
	while ((pow2<i) && (pow2!=0)) pow2 <<= 1;

	return(pow2);
}

/*===================================================================================
/  initDiscreteDistribution
===================================================================================*/
void DiscreteDistribution_destruct(DiscreteDistribution* distribution)
{
    free(distribution->cumulativeProbabilities);
    free(distribution->probabilities);
    free(distribution->points);
    free(distribution);
}

/*===================================================================================
/  initDiscreteDistribution
===================================================================================*/
int initDiscreteDistribution(DiscreteDistribution* distribution, int dim, int nPoints, float** points, float* probabilities)
{

	int 	i;
	int		pow2NPoints, log2NPoints;
	float 	cumulativeProbability;
	float* 	cumulativeProbabilities;

	/* -------------------------------------------------------------------------
	/  ENFORCE  nPoints>0
	  ---------------------------------------------------------------------- */
	if (nPoints==0) {
		fprintf(stderr,"PF: nPoints=0 in initDiscreteDistribution makes no sense\n");
		return(0);
	}

	/* -------------------------------------------------------------------------
	   check probabilities
	   ---------------------------------------------------------------------- */
	cumulativeProbability = 0;
	for (i=0; i<nPoints; i++) cumulativeProbability += probabilities[i];
	if (cumulativeProbability==0) {
		fprintf(stderr,"PF: sum of probabilities in initDiscreteDistribution is zero\n");
		return(0);
	}
	if ((cumulativeProbability<0.9) && (cumulativeProbability>1.1)) {
		fprintf(stderr,"PF: probability doesn't sum to one (%f), corrected ...\n", 
                cumulativeProbability);
		for (i=0; i<nPoints; i++) probabilities[i] /= cumulativeProbability;
	}

	/* -------------------------------------------------------------------------
	   obtain closest power of 2 containing nPoints
	   ---------------------------------------------------------------------- */
	log2NPoints = log2ceil(nPoints);
	pow2NPoints = 1 << log2NPoints;

	/* -------------------------------------------------------------------------
	/  allocate memory
	  ---------------------------------------------------------------------- */
	cumulativeProbabilities = malloc(sizeof(float)*pow2NPoints);

	/* -------------------------------------------------------------------------
	/  calculate cumulative probabilities
	  ---------------------------------------------------------------------- */
	cumulativeProbability = 0;
	for (i=0; i<nPoints; i++) {
		cumulativeProbability += probabilities[i];
		cumulativeProbabilities[i] = cumulativeProbability;
	}
	for (i=nPoints; i<pow2NPoints; i++) cumulativeProbabilities[i] = cumulativeProbability;

	/* -------------------------------------------------------------------------
	/  fill "DiscreteDistribution"-structure
	  ---------------------------------------------------------------------- */
	(*distribution).dim                     = dim;
	(*distribution).nPoints 				= nPoints;
	(*distribution).points 					= points;
	(*distribution).probabilities 			= probabilities;
	(*distribution).cumulativeProbabilities = cumulativeProbabilities;
	(*distribution).pow2NPoints				= pow2NPoints;
	(*distribution).log2NPoints				= log2NPoints;

	(*distribution).evaluate  				= evaluateDiscreteDistribution;
	(*distribution).sample 					= sampleFromDiscreteDistribution;
	(*distribution).destruct                = DiscreteDistribution_destruct;
	return(1);
}

/*===================================================================================
/  createDiscreteDistribution
===================================================================================*/
DiscreteDistribution* createDiscreteDistribution(int dim, int nPoints, float** points, float* probabilities)
{
	DiscreteDistribution* distribution = (DiscreteDistribution*) malloc(sizeof(DiscreteDistribution));
	if (initDiscreteDistribution(distribution, dim, nPoints, points, probabilities)==0) {
		free(distribution);
		distribution = 0;
	}

	return(distribution);
}
