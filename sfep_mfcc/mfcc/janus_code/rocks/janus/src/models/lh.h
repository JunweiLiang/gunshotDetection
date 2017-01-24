/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: SampleSet
               ------------------------------------------------------------

    Author  :  Ivica Rogina
    Module  :  lh.h
    Date    :  $Id: lh.h 2390 2003-08-14 11:20:32Z fuegen $
    Remarks :

   ========================================================================

    This software was developed by

    the  University of Karlsruhe       and  Carnegie Mellon University
         Dept. of Informatics               Dept. of Computer Science
         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
         Am Fasanengarten 5                 5000 Forbes Ave
         D-76131 Karlsruhe                  Pittsburgh, PA 15213
         - West Germany -                   - USA -

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

    $Log$
    Revision 1.5  2003/08/14 11:20:15  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.4.20.1  2002/11/13 09:52:32  soltau
    lhCreate : don't pass gaussN argument anymore

    Revision 1.2  1997/07/31 15:33:04  rogina
    added prototype for lhPuts

    Revision 1.1  1997/07/16 11:31:40  rogina
    Initial revision


   ======================================================================== */

#ifndef _lhClust
#define _lhClust

typedef struct Lh_S
{
  int      frameN;
  int      gaussN;
  int      gaussX;

  short   *valN;     /* val[0..i..frameN] = number of Gaussians for frame i */
  float   *valG;     /* array of all used Gaussian values                   */
  short   *valX;     /* indices of all used Gaussians                       */
  int     *modX;     /* index of distribution for each frame                */
} Lh;

extern Lh*   lhCreate     (void);
extern int   lhAlloc      (Lh *lh, int gaussN);
extern float lhLikelihood (float *dsVal, Lh *lh);
extern int   lhPuts       (Lh *lh);
extern int   lhFlush      (Lh* lh, char* fileName);
extern int   Lh_Init();

#endif

/* ------------------------------------------------------------------------- */
/* The lh-struct can hold Gaussian information for any number of training    */
/* vectors (frames). The field frameN contains that number of frames.        */
/* For every frame (frameX), there are valN[frameX] Gaussians stored. Each   */
/* Gaussian is stored with one integer (the index of the Gaussian in its     */
/* codebook, and one float (the actual value of the Gaussian at the corres-  */
/* ponding frame). For every frame (frameX), the index of the model (i.e.    */
/* the index of the distribution in the distribution set) is stored in       */
/* modX[frameX].                                                             */
/* All the Gaussians (indices and values) are stored in two one-dimensional  */
/* arrays. The number valN[frameX] is the number of Gaussians that contri-   */
/* buted more than a floor-value to the score of the frame (frameX). This    */
/* is always at least one, sometimes there are more. The position of the     */
/* n-th contributing Gaussian index and Gaussian value in their arrays for   */
/* given frame (frameX) is: \Sum_{f<frameX} valN[f] + n                      */
/* ------------------------------------------------------------------------- */
