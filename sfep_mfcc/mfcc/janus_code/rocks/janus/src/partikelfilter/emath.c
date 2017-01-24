/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Extended Math
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  emath.c
    Date    :  $Id: emath.c 2844 2008-11-25 12:00:00Z wolfel $
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
#include "emath.h"

/*-----------------------------------------------------------------------------
/  STATIC VARIABLE INITIALIZATION
/----------------------------------------------------------------------------*/
static float  log10fc  = 2.302585092994046e+00;
static double log10dc  = 2.302585092994046e+00;
static float  log10fic = 4.342944819032518e-01;
static double log10dic = 4.342944819032518e-01;
/*
static float  log2fc  = 6.931471805599453e-01;
static double log2dc  = 6.931471805599453e-01;
static float  log2fic = 1.442695040888963e+00;
static double log2dic = 1.442695040888963e+00;
*/
/*===================================================================================
/  log10F
===================================================================================*/
float log10F(float f)
{
    return(log(f)*log10fic);
}

/*===================================================================================
/  log10D
===================================================================================*/
double log10D(double d)
{
    return(log(d)*log10dic);
}

/*===================================================================================
/  exp10F
===================================================================================*/
float exp10F(float f)
{
    return(exp(f*log10fc));
}

/*===================================================================================
/  exp10D
===================================================================================*/
double exp10D(double d)
{
    return(exp(d*log10dc));
}
