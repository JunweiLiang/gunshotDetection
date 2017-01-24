/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Random Number Generator
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  RNG.h
    Date    :  $Id: RNG.h 2844 2008-11-25 12:00:00Z wolfel $

   ========================================================================

    Revision 1.1  2008/11/25  12:00:00  wolfel
    Initial revision

   ======================================================================== */

#ifndef RNG_YET_INCLUDED
#define RNG_YET_INCLUDED

/*-----------------------------------------------------------------------------
/  INCLUDES
/----------------------------------------------------------------------------*/
#include <math.h>
#include <stdlib.h>

/*-----------------------------------------------------------------------------
/  RANDOM NUMBER GENERATION ALGORITHMS FOR UNITARY DISTRIBUTIONS
/----------------------------------------------------------------------------*/
#define RNG_UNITARY_ALGORITHM_KISS32 	1
#define RNG_UNITARY_ALGORITHM_KISS64 	2

/*-----------------------------------------------------------------------------
/  RANDOM NUMBER GENERATION ALGORITHMS FOR GAUSSIAN DISTRIBUTIONS
/----------------------------------------------------------------------------*/
#define RNG_GAUSSIAN_ALGORITHM_BOXMULLER 	1
#define RNG_GAUSSIAN_ALGORITHM_ZIGGURAT 	2

/*-----------------------------------------------------------------------------
/  SELECT RNG-ALGORITHMS
/----------------------------------------------------------------------------*/
#define RNG_UNITARY_ALGORITHM 		RNG_UNITARY_ALGORITHM_KISS32
#define RNG_GAUSSIAN_ALGORITHM		RNG_GAUSSIAN_ALGORITHM_ZIGGURAT

/*===================================================================================
/  functions
===================================================================================*/
void            RNG_Init(unsigned int seed);
unsigned int 	RNG_Int();
float 		RNG_Float();
float 		RNG_Gaussian();

#endif
