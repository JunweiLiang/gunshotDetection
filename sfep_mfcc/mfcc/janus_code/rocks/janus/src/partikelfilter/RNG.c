/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Random Number Generator
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  RNG.c
    Date    :  $Id: RNG.c 2844 2008-11-25 12:00:00Z wolfel $
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
#include "RNG.h"

/*-----------------------------------------------------------------------------
/  INTERNAL VARIABLES
/----------------------------------------------------------------------------*/
static const float		RNG_NORMALIZER        = 1.0 / (65536.0*65536.0);
static const float		RNG_NORMALIZER_SIGNED = 1.0 / (32768.0*65536.0);

static unsigned long		RNG_KISS_INT_1;
static unsigned long		RNG_KISS_INT_2;
static unsigned long	 	RNG_KISS_INT_3;
/* static unsigned long		RNG_KISS_INT_4; */

static int			RNG_ZIGGURAT_K_TABLE[128];
static float			RNG_ZIGGURAT_F_TABLE[128];
static float			RNG_ZIGGURAT_W_TABLE[128];
static float			RNG_ZIGGURAT_REM;

/*===================================================================================
/  gaussian  |  SUPPORT FUNCTIONS
===================================================================================*/
/* inline */
float gaussian(float x)
{
    return(exp(-.5*x*x));
}

/* inline */
double gaussianD(double x)
{
    return(exp(-.5*x*x));
}

/*===================================================================================
/  ZIGGURAT  |  Gaussian Sampling
/
/  see "Marsaglia, Tsang - The Ziggurat Method for Generating Random Variables"
===================================================================================*/
#if  RNG_GAUSSIAN_ALGORITHM == RNG_GAUSSIAN_ALGORITHM_ZIGGURAT
float RNG_Gaussian()
{
    int 		rv, i;
    float 		x, y, d;
    const float 	r  = 3.442619855899;
    const float 	rI = 1.0/r;

    while (1) {
	rv	= RNG_Int();
	i 	= (rv & 0x7F);
	x 	= rv*RNG_ZIGGURAT_W_TABLE[i];

	if (abs(rv) < RNG_ZIGGURAT_K_TABLE[i]) return(x);

	if (i>0) {
	    d  = RNG_ZIGGURAT_F_TABLE[i-1] - RNG_ZIGGURAT_F_TABLE[i];
	    if (d*RNG_Float()+RNG_ZIGGURAT_F_TABLE[i] < gaussian(x)) return(x);
	} else {
	    do {
		x = -log(RNG_Float()) * rI;
		y = -log(RNG_Float());
	    }
	    while (y+y < x*x);
	    x += r;
	    if (rv<0) x *= (-1);
	    return(x);
	}
    }

    return(x);
}
#endif

/*===================================================================================
/  BOXMULLER  |  Gaussian Sampling
/
/  see "Robert, Casella - Monte Carlo Statistical Methods", Springer, 2005, p. 43
===================================================================================*/
#if  RNG_GAUSSIAN_ALGORITHM == RNG_GAUSSIAN_ALGORITHM_BOXMULLER
float RNG_Gaussian()
{
    float u1,u2, x;

    u1 = RNG_Float() + 1E-31;			/* make sure log(u1)!=0*/
    u2 = RNG_Float();
    x  = sqrt(-2*log(u1)) * cos(2*M_PI*u2);

    return(x);
}
#endif

/*===================================================================================
/  KISS64 - GENERATE INT
/
/  George Marsaglia's newest KISS-version
/  http://forums.wolfram.com/mathgroup/archive/2003/Feb/msg00456.html
/  cashed locally
/  published there by the author himself
/
/  RNG_KISS_INT_1 = x,  	RNG_KISS_INT_2 = y
/  RNG_KISS_INT_3 = z,		RNG_KISS_INT_4 = c (carry)
===================================================================================*/
#if	 RNG_UNITARY_ALGORITHM == RNG_UNITARY_ALGORITHM_KISS64
/* inline */
unsigned int RNG_Int()
{
    unsigned int	random;
    unsigned long long 	temp;

    RNG_KISS_INT_1 = 69069*RNG_KISS_INT_1 + 12345;
    RNG_KISS_INT_2 ^= (RNG_KISS_INT_2<<13); 
    RNG_KISS_INT_2 ^= (RNG_KISS_INT_2>>17); 
    RNG_KISS_INT_2 ^= (RNG_KISS_INT_2<<5);
    temp = 698769069LL*RNG_KISS_INT_3 + RNG_KISS_INT_4; 
    RNG_KISS_INT_3 = temp;
    RNG_KISS_INT_4 = (temp>>32);

    random = RNG_KISS_INT_1 + RNG_KISS_INT_2 + RNG_KISS_INT_3;

    return(random); 
}
#endif

/*===================================================================================
/  KISS32 - GENERATE INT
/
/  see "Robert, Casella - Monte Carlo Statistical Methods", Springer, 2004, p.75
/
/  RNG_KISS_INT_1 = i
/  RNG_KISS_INT_2 = j
/  RNG_KISS_INT_3 = k
===================================================================================*/
#if	 RNG_UNITARY_ALGORITHM == RNG_UNITARY_ALGORITHM_KISS32
/* inline */
unsigned int RNG_Int()
{
    unsigned int random;

    RNG_KISS_INT_2 ^= (RNG_KISS_INT_2 << 17);
    RNG_KISS_INT_3  = (RNG_KISS_INT_3 ^ (RNG_KISS_INT_3 << 18)) & 0x7FFFFFFF;
    RNG_KISS_INT_1  = (69069*RNG_KISS_INT_1) + 23606797;
    RNG_KISS_INT_2 ^= (RNG_KISS_INT_2 >> 15);
    RNG_KISS_INT_3 ^= (RNG_KISS_INT_3 >> 13);

    random = RNG_KISS_INT_1+RNG_KISS_INT_2+RNG_KISS_INT_3;

    return(random); 
}
#endif

/*===================================================================================
/  GENERATE FLOAT RANDOM NUMBER
===================================================================================*/
/* inline */ 
float RNG_Float()
{
    return((float)RNG_Int() * RNG_NORMALIZER); 
}

/*===================================================================================
/  KISS32 - INIT
===================================================================================*/
#if	 RNG_UNITARY_ALGORITHM == RNG_UNITARY_ALGORITHM_KISS32
/* inline */
void RNG_UNITARY_Init(unsigned int seed)
{
    RNG_KISS_INT_1 = seed ^ 0x1F3A67D4;
    RNG_KISS_INT_2 = seed ^ 0xD0A51B83;
    RNG_KISS_INT_3 = seed ^ 0x726EF078;
}
#endif

/*===================================================================================
/  KISS64 - INIT
===================================================================================*/
#if	 RNG_UNITARY_ALGORITHM == RNG_UNITARY_ALGORITHM_KISS64
/* inline */
void RNG_UNITARY_Init(unsigned int seed)
{
    RNG_KISS_INT_1 = seed ^ 0x1F3A67D4;
    RNG_KISS_INT_2 = seed ^ 0xD0A51B83;
    RNG_KISS_INT_3 = seed ^ 0x726EF078;
    RNG_KISS_INT_4 = seed ^ 0x05C9B421;
}
#endif

/*===================================================================================
/  BOXMULLER - INIT
===================================================================================*/
#if  RNG_GAUSSIAN_ALGORITHM == RNG_GAUSSIAN_ALGORITHM_BOXMULLER
void RNG_GAUSSIAN_Init(unsigned int seed)
{
}
#endif

/*===================================================================================
/  ZIGGURAT - INIT
===================================================================================*/
#if  RNG_GAUSSIAN_ALGORITHM == RNG_GAUSSIAN_ALGORITHM_ZIGGURAT
void RNG_GAUSSIAN_Init(unsigned int seed)
{
    double 			X[128];
    int 			i;
    double 			x;
    const double 	r = 3.442619855899;
    const double 	v = 9.91256303526217E-3;
	
    /*-------------------------------------------------------------------------
    /  FILL X-TABLE
    /------------------------------------------------------------------------*/
    x = r;
    for (i=127; i>0; i--) {
	X[i] = x;
	x = sqrt(-2.0 * log(v/x+gaussianD(x)));
    }
    X[0] = 0;

    /*-------------------------------------------------------------------------
    /  FILL W-TABLE
    /------------------------------------------------------------------------*/
    for (i=1; i<128; i++) RNG_ZIGGURAT_W_TABLE[i] = X[i] * RNG_NORMALIZER_SIGNED;
    RNG_ZIGGURAT_W_TABLE[0] = v / gaussian(r) * RNG_NORMALIZER_SIGNED;

    /*-------------------------------------------------------------------------
    /  FILL F-TABLE
    /------------------------------------------------------------------------*/
    for (i=0; i<128; i++) RNG_ZIGGURAT_F_TABLE[i] = gaussian(X[i]);

    /*-------------------------------------------------------------------------
    /  FILL K-TABLE
    /------------------------------------------------------------------------*/
    for (i=2; i<128; i++) RNG_ZIGGURAT_K_TABLE[i] = (X[i-1] / X[i]) / RNG_NORMALIZER_SIGNED;
    RNG_ZIGGURAT_K_TABLE[0] = (r/v*gaussian(r)) / RNG_NORMALIZER_SIGNED;
    RNG_ZIGGURAT_K_TABLE[1] = 0;
    RNG_ZIGGURAT_REM = v / gaussian(r);
}	
#endif

/*===================================================================================
/  RNG INIT
===================================================================================*/
void RNG_Init(unsigned int seed)
{
    /* printf("RNG_Init(%i)\n",seed); */
    RNG_UNITARY_Init(seed);
    RNG_GAUSSIAN_Init(seed);
}
