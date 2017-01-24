/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Gaussian Table
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  GaussianTable.c
    Date    :  $Id: GaussianTable.c 2844 2008-11-25 12:00:00Z wolfel $
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
#include <math.h>
#include <stdlib.h>
#include "NoiseCompensation.h"

/*-----------------------------------------------------------------------------
/  static variables
/----------------------------------------------------------------------------*/
static float* GAUSSIAN_TABLE_DATA = 0;
static int    GAUSSIAN_TABLE_RESOLUTION = 0;
static float  GAUSSIAN_TABLE_RESOLUTION_BY2 = 0;
static float  GAUSSIAN_TABLE_CUTOFF_VALUE = 0;
static float  GAUSSIAN_TABLE_ICOV = 0;

/*===================================================================================
/  createGaussianTable
===================================================================================*/

void createGaussianTable(int resolution, float cutoffProb)
{
    float x, y, normalizer;
    int   i;

    /* printf("creating table [res:%i]\n", resolution); */

    /*-------------------------------------------------------------------------
    /  set values
    /------------------------------------------------------------------------*/    
    GAUSSIAN_TABLE_RESOLUTION     = resolution;
    GAUSSIAN_TABLE_RESOLUTION_BY2 = 0.5*resolution;
    GAUSSIAN_TABLE_CUTOFF_VALUE   = sqrt(-2.0*log(cutoffProb*sqrt(2.0*M_PI)));
    GAUSSIAN_TABLE_ICOV           = 1.0/GAUSSIAN_TABLE_CUTOFF_VALUE;

    /*-------------------------------------------------------------------------
    /  allocate memory  
    /------------------------------------------------------------------------*/    
    if (GAUSSIAN_TABLE_DATA!=0) free(GAUSSIAN_TABLE_DATA);
    GAUSSIAN_TABLE_DATA = (float*) malloc(GAUSSIAN_TABLE_RESOLUTION*sizeof(float));

    /*-------------------------------------------------------------------------
    /  compute values
    /------------------------------------------------------------------------*/
    normalizer = 1.0/sqrt(2.0*M_PI);    
    for(i=0; i<GAUSSIAN_TABLE_RESOLUTION; i++) {
        x = GAUSSIAN_TABLE_CUTOFF_VALUE * (-1.0 + (float)i/GAUSSIAN_TABLE_RESOLUTION_BY2);
        y = normalizer * exp(-0.5*x*x);
        GAUSSIAN_TABLE_DATA[i] = y;
    }
}

/*===================================================================================
/  evaluateGaussianTable
===================================================================================*/
__inline__ static float evaluateGaussianTable(float x, float mu, float oneBySigma)
{
    float xm, y;
    int   index;

    /*-------------------------------------------------------------------------
    /  calc
    /------------------------------------------------------------------------*/
    xm = (x-mu)*oneBySigma;
    y = 0;
    if (fabs(xm)<GAUSSIAN_TABLE_CUTOFF_VALUE) {
        index = (int)((xm*GAUSSIAN_TABLE_ICOV+1.0)*GAUSSIAN_TABLE_RESOLUTION_BY2);
        y = GAUSSIAN_TABLE_DATA[index]*oneBySigma;
    }

    return(y);
}
