/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: melscale filterbanks
               ------------------------------------------------------------

    Author  :  Martin Maier
    Module  :  melscale.c
    Date    :  $Id: melscale.c 2390 2003-08-14 11:20:32Z fuegen $
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

   $Log$
   Revision 3.5  2003/08/14 11:19:52  fuegen
   Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

   Revision 3.4.4.2  2003/06/06 07:55:58  fuegen
   replaced C++ like comments

   Revision 3.4.4.1  2003/04/16 12:59:37  metze
   MVDR added (Matthias Wölfel)

   Revision 3.4  2001/01/15 09:49:59  janus
   Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

   Revision 3.3.4.1  2001/01/12 15:16:59  janus
   necessary changes for running janus under WINDOWS

   Revision 3.3  2000/11/14 12:35:24  janus
   Merging branch jtk-00-10-27-jmcd.

   Revision 3.2.30.1  2000/11/06 10:50:36  janus
   Made changes to enable compilation under -Wall.

   Revision 3.2  1997/07/14 12:29:01  rogina
   *** empty log message ***


   ======================================================================== */
#include <math.h>
#include "matrix.h"

static float mel(float hz)
{
   if (hz>=0) return (float)(2595.0 * log10(1.0 + (double)hz/700.0));
   else return 0.0;
}
static float hertz(float m)
{
   double d = m / 2595.0;
   return (float)(700.0 * (pow(10.0,d) - 1.0));
}

/* ============================================================================
;  fbmatrixMel
;      Precalculates the parameters of a melfilterbank.
;
;  IN:
;       rate                    : sampling rate in Hz
;       powN                    : how many points from power spectrum  analysis
;
;       low                     : lower frequency of 1st filter
;       up                      : upper frequency of last filter 
;
;       filterN                 : number of filters
;
;  OUT and RETURN:
;       A                       : band matrix
;
; ========================================================================== */
FBMatrix* fbmatrixMel( FBMatrix* A, int powN,  float rate, float low,
		       float up, int filterN)
{
   int   x, i;
   float df = rate / (4.0 * (powN/2));   /* spacing between FFT points in Hz */
   float mlow = mel(low);
   float mup  = mel(up);
   float dm   = (mup - mlow)/(filterN+1);    /* delta mel */

   if (low<0.0 || 2.0*up>rate || low>up) {
      ERROR("mel: something wrong with low = %f, up = %f, rate = %f\n",
	    low,up,rate);
      return NULL;
   }
   /* printf("lower = %fHz (%fmel), upper = %fHz (%fmel)\n",low,mlow,up,mup);*/

   /* -------------------------------------------
      free band matrix, allocate filterN pointer
      ------------------------------------------- */
   if (A->matPA) {
      for (i=0;i<A->m;i++)
	if (A->matPA[i]) free(A->matPA[i]); 
      free( A->matPA);
   }
   if (A->offset) free(A->offset);
   if (A->coefN) free(A->coefN);
   
   if ((A->matPA  = (float**)malloc(filterN * sizeof(float*))) == NULL ||
       (A->coefN  = (int*)malloc(filterN * sizeof(int))) == NULL ||
       (A->offset = (int*)malloc(filterN * sizeof(int))) == NULL)
      return NULL;
   A->m = filterN;
   
   /* ---------------------------
      loop over all filters
      --------------------------- */
   for (x=0;x<filterN;x++) {
      float freq, height;
      float slope1, slope2;
      int start, end;
      /* ---- left, center and right edge ---- */
      float left   = hertz( x     *dm + mlow);
      float center = hertz((x+1.0)*dm + mlow);
      float right  = hertz((x+2.0)*dm + mlow);
      /* printf("%3d: left = %fmel, center = %fmel, right = %fmel\n",
	     x,x*dm+mlow,(x+1.0)*dm+mlow,(x+2.0)*dm+mlow); */
      /* printf("%3d: left = %fHz, center = %fHz, right = %fHz\n",
	     x,left,center,right); */
      
      height = 2.0 / (right - left);          /* normalized height = 2/width */
      slope1 = height / (center - left);
      slope2 = height / (center - right);
      start  = (int)ceil(left / df);
      end    = (int)floor(right / df);
      
      A->offset[x] = start;
      A->coefN[x]  = end - start + 1;
      A->n         = end;
      if ((A->matPA[x] = malloc(A->coefN[x] * sizeof(float))) == NULL) {
	 /* free ... */
	 return NULL;
      }
      for ( freq=start*df, i=0; i<A->coefN[x]; freq += df, i++) {
	 if (freq <= center)
	    A->matPA[x][i] = slope1*(freq-left);
	 else
	    A->matPA[x][i] = slope2*(freq-right);
      }
   }
   A->rate = rate;
   return A;
}

int fbmatrixMelItf( ClientData cd, int argc, char *argv[])
{
  FBMatrix* mat = (FBMatrix*)cd;
  int   N     = 30;
  int   powN  = 129;
  float rate  = 16000.0;
  float low   = 0;
  float up    = 0;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "-N",     ARGV_INT,    NULL,       &N,    cd, "number of filters",
    "-p",     ARGV_INT,    NULL,       &powN, cd, "number of (power) points",
    "-rate",  ARGV_FLOAT,  NULL,       &rate, cd, "sampling rate in Hz",
    "-low",   ARGV_FLOAT,  NULL,       &low,  cd, "lowest frequency in Hz",
    "-up",    ARGV_FLOAT,  NULL,       &up,   cd, "highest frequency in Hz"
		                                  ", 0 means rate/2",
		    NULL) != TCL_OK) return TCL_ERROR;
  if (up<=0) up = rate/2.0;
  if (fbmatrixMel( mat, powN,  rate, low, up, N))
     return TCL_OK;
  else return TCL_ERROR;
}

/* ============================================================================
;  fbmatrixMeltri
;      Precalculates the parameters of a triangular shaped melfilterbank.
;      For lower frequency we use linear spaced filters and then for higher
;      frequencies log spaced filters.
;      
;
;  IN:
;       rate                    : sampling rate in Hz
;       powN                    : how many points from power spectrum  analysis
;
;       low                     : lower frequency of 1st filter
;       up                      : upper frequency of last filter 
;
;       filterN                 : number of filters
;       lin                     : linear spacing
;       logN                    : how many of the filters in the log range
;       log                     : log spacing
;
;  OUT and RETURN:
;       A                       : band matrix
;
; ========================================================================== */
FBMatrix* fbmatrixMeltri( FBMatrix* A, int powN,  float rate, float low,
			 float up, int filterN, float lin, int logN, float log)
{
   int   x, i;
   float df = rate / (4.0 * (powN/2));       /* spacing between FFT points in Hz */
   int linN = filterN - logN;

   /* -------------------------------------------
      free band matrix, allocate filterN pointer
      ------------------------------------------- */
   if (A->matPA) {
      for (i=0;i<A->m;i++)
	if (A->matPA[i]) free(A->matPA[i]); 
      free( A->matPA);
   }
   if (A->offset) free(A->offset);
   if (A->coefN) free(A->coefN);
   
   if ((A->matPA  = (float**)malloc(filterN * sizeof(float*))) == NULL ||
       (A->coefN  = (int*)malloc(filterN * sizeof(int))) == NULL ||
       (A->offset = (int*)malloc(filterN * sizeof(int))) == NULL)
      return NULL;
   A->m = filterN;
   
   /* ---------------------------
      loop over all filters
      --------------------------- */
   for (x=0;x<filterN;x++) {
      float freq, left, center=0.0, right=0.0, height;
      float slope1, slope2;
      int start, end;

      /* ---- left, center and right edge ---- */
      if (x == 0) {
	 left    = low;
	 center  = low + lin;
      }
      else {
	 left   = center;             /* the new left edge = old center freq */
	 center = right;             /* the new center freq = old right edge */
      }
      if (x < (linN-1))	right = center + lin;   /* right edge is shifted out */
      else right = center * log;                 /* right edge for log scale */

      height = 2.0 / (right - left);          /* normalized height = 2/width */
      slope1 = height / (center - left);
      slope2 = height / (center - right);
      start  = (int)ceil(left / df);
      end    = (int)floor(right / df);
      
      A->offset[x] = start;
      A->coefN[x]  = end - start + 1;
      A->n         = end;
      if ((A->matPA[x] = malloc(A->coefN[x] * sizeof(float))) == NULL) {
	 /* free ... */
	 return NULL;
      }
      for ( freq=start*df, i=0; i<A->coefN[x]; freq += df, i++) {
	 if (freq <= center)
	    A->matPA[x][i] = slope1*(freq-left);
	 else
	    A->matPA[x][i] = slope2*(freq-right);
      }
   }
   A->rate = rate;
   return A;
}

int fbmatrixMeltriItf( ClientData cd, int argc, char *argv[])
{
  FBMatrix* mat = (FBMatrix*)cd;
  int   powN  = 256;
  float rate  = 16000.0;
  float low   = 133.33334;
  float up    = 6855.4976;
  int filterN = 40;
  float lin   = 66.666664;
  int   logN  = 27;
  float log   = 1.0711703;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
		    NULL) != TCL_OK) return TCL_ERROR;
  if (fbmatrixMeltri( mat, powN,  rate, low, up, filterN, lin, logN, log))
     return TCL_OK;
  else return TCL_ERROR;
}

/* ============================================================================
;  fbmatrixMeltra
;      Precalculates the parameters of a trapezoid shaped melfilterbank.
;
;  See Waibel (ATR TR-I-0006) sec. 2.1 and
;  Waibel & Yegnanarayana (1981) for details on melscale filterbank.
;      
;
;  IN:
;       rate                    : sampling rate in Hz
;       powN                    : how many points from power spectrum  analysis
;
;  OUT and RETURN:
;       A                       : band matrix
;
|  The table below shows how the power FFT coefficients are condensed to 
|  melscale coeffients and the corresponding frequencies
|  (From Bernhard Suhms "Studienarbeit")
|
|	mel	power	 Hz		dHz
|      ------------------------------------
|	0	  0	   0		125
|	1	  2	 125		250
|	2	  6	 375		250
|	3	 10	 625		250
|	4	 14	 875		250
|	5	 18	1125		250
|	6	 22	1375		250
|	7	 26	1625		250
|	8	 30	1875		250
|	9	 35	2187		312
|	10	 41	2562		375
|	11	 48	3000		438
|	12	 57	3562		562
|	13	 68	4250		688
|	14	 81	5062		812
|	15	 97	6062		1000
|		116	7250		1188
|
| 
|  For sampling rate 8 kHz , i.e. a 4 kHz range (!!), we use a different scaling:
|
|
|		The first 13 mel-scale (range 0-8kHz) coefficients cover the range from 0 to 4 kHz:
|
|  mel-coef	0	1   	2   	3   	4   	5   	6   	7   	8   	9  	10  	11  	12 	13 
|		++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
|  FFT-coef	0   	2   	6  	10  	14  	18  	22  	26  	30  	35  	41  	48  	57	68
|  Freq		0											3000    3562    4250
|                   2      4        4       4       4       4       4        4      5       5       7       9       11
|
|		If we want to use 16 coefficients for 8kHz sampling, how do we have to position them in order to get
|		about the same mapping as we got for the 16 kHz sampling? :
|
|		0     1     2     3     4     5     6     7     8      9      10     11     12     13     14     15     16
|		++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
| FFT-coef	0   	2   	6  	10  	14  	18  	22  	26  	30  	35  	41  	48  	57	68
|		0     2     4     7     10    13    16    19    22     25     29     33     37     43     50     58     68
|		0     4     8    14     20    26    32    38    44     50     56     63     71     83     96     111    127
|		   4     4    6      6      6    6     6     6      6      6      7     9      11     13      15     16
|		0													4000
|
; ========================================================================== */
FBMatrix* fbmatrixMeltra( FBMatrix* A, int powN,  float rate)
{
   int   x, i;
   int   filterN = 16;
   float scaling = floor(powN/2.0)/64; /* in case we have N/2+1 points like 129 */
   /* scaling for 8 kHz range	*/
   static int  melA8[17] = {0,2,6,10,14,18,22,26,30,35,41,48,57,68,81,97,116};
   /* scaling for 4 kHz range	*/
   static int  melA4[17] = {0,4,8,14,20,26,32,38,44,50,56,63,71,83,96,111,127};
   int   *melA = (rate == 16000) ? melA8 : (rate == 8000) ? melA4 : NULL;

   if (!melA) {
      ERROR("Can only create MelMatrix for rate = 8 or 16 kHz not for %.0f Hz\n",rate);
      return NULL;
   }
   
   /* -------------------------------------------
      free band matrix, allocate filterN pointer
      ------------------------------------------- */
   if (A->matPA) {
      for (i=0;i<A->m;i++)
	if (A->matPA[i]) free(A->matPA[i]); 
      free( A->matPA);
   }
   if (A->offset) free(A->offset);
   if (A->coefN) free(A->coefN);
   
   if ((A->matPA  = (float**)malloc(filterN * sizeof(float*))) == NULL ||
       (A->coefN  = (int*)malloc(filterN * sizeof(int))) == NULL ||
       (A->offset = (int*)malloc(filterN * sizeof(int))) == NULL)
      return NULL;
   A->m = filterN;
   
   /* ---------------------------
      loop over all filters
      --------------------------- */
   for (x=0;x<filterN;x++) {
      int start = melA[x]   * scaling;
      int end   = melA[x+1] * scaling;
      
      A->offset[x] = start;
      A->coefN[x]  = end - start + 1;
      A->n         = end;
      if ((A->matPA[x] = (float*)malloc(A->coefN[x] * sizeof(float))) == NULL) {
	 /* free ... */
	 return NULL;
      }
      
      for ( i=0; i<A->coefN[x]; i++) {
	 if (i == 0) 	               A->matPA[x][i] = (start==0) ? 1.0 : 0.5;
	 else if (i+1 == A->coefN[x])  A->matPA[x][i] = 0.5;
	 else 	                       A->matPA[x][i] = 1.0;
      }
   }
   A->rate = rate;
   return A;
}

int fbmatrixMeltraItf( ClientData cd, int argc, char *argv[])
{
  FBMatrix* mat = (FBMatrix*)cd;
  int   powN  = 128;
  float rate  = 16000.0;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "-rate",  ARGV_FLOAT,  NULL,       &rate, cd,  "sampling rate in Hz",
    "-p",     ARGV_INT,    NULL,       &powN, cd,  "number of (power) points",
    NULL) != TCL_OK) return TCL_ERROR;
  if (fbmatrixMeltra( mat, powN,  rate))  return TCL_OK;
  else return TCL_ERROR;
}

/* ----------------------------------------------------------------------------
     The following source code is by Matthias Wölfel
   ---------------------------------------------------------------------------- */

/* ============================================================================
;  fbmatrixRectMel
;      Precalculates the parameters of a melfilterbank.
;
;  IN:
;       rate                    : sampling rate in Hz
;       powN                    : how many points from power spectrum  analysis
;
;       low                     : lower frequency of 1st filter
;       up                      : upper frequency of last filter 
;
;       filterN                 : number of filters
;
;  OUT and RETURN:
;       A                       : band matrix
;
; ========================================================================== */
FBMatrix* fbmatrixRectMel( FBMatrix* A, int powN,  float rate, float low,
		       float up, int filterN)
{
   int   x, i;
   float df = rate / (4.0 * (powN/2));   /* spacing between FFT points in Hz */
   float mlow = mel(low);
   float mup  = mel(up);
   float dm   = (mup - mlow)/(filterN);    /* delta mel */

   if (low<0.0 || 2.0*up>rate || low>up) {
      ERROR("mel: something wrong with low = %f, up = %f, rate = %f\n",
	    low,up,rate);
      return NULL;
   }
   /* printf("lower = %fHz (%fmel), upper = %fHz (%fmel)\n",low,mlow,up,mup);*/

   /* -------------------------------------------
      free band matrix, allocate filterN pointer
      ------------------------------------------- */
   if (A->matPA) {
      for (i=0;i<A->m;i++)
	if (A->matPA[i]) free(A->matPA[i]); 
      free( A->matPA);
   }
   if (A->offset) free(A->offset);
   if (A->coefN) free(A->coefN);
   
   if ((A->matPA  = (float**)malloc(filterN * sizeof(float*))) == NULL ||
       (A->coefN  = (int*)malloc(filterN * sizeof(int))) == NULL ||
       (A->offset = (int*)malloc(filterN * sizeof(int))) == NULL)
      return NULL;
   A->m = filterN;
   
   /* ---------------------------
      loop over all filters
      --------------------------- */
   for (x=0;x<filterN;x++) {
      float freq, height;
      int start, end;
      /* ---- left, center and right edge ---- */
      float left   = hertz( x     *dm + mlow);
      /* float center = hertz((x+1.0)*dm + mlow); */
      float right  = hertz((x+1.0)*dm + mlow);

      fprintf(stderr,"%d: left = %f Hz, right = %f Hz \n",
	     x,left,right);
      
      height = 2.0 / (right - left);          /* normalized height = 2/width */
      start  = (int)ceil(left / df);
      end    = (int)floor(right / df);
      
      A->offset[x] = start;
      A->coefN[x]  = end - start + 1;
      A->n         = end;
      if ((A->matPA[x] = malloc(A->coefN[x] * sizeof(float))) == NULL) {
	 /* free ... */
	 return NULL;
      }
      for ( freq=start*df, i=0; i<A->coefN[x]; freq += df, i++)
	    A->matPA[x][i] = height;
   }
   A->rate = rate;
   return A;
}

int fbmatrixMelRectItf( ClientData cd, int argc, char *argv[])
{
  FBMatrix* mat = (FBMatrix*)cd;
  int   N     = 30;
  int   powN  = 129;
  float rate  = 16000.0;
  float low   = 0;
  float up    = 0;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "-N",     ARGV_INT,    NULL,       &N,    cd, "number of filters",
    "-p",     ARGV_INT,    NULL,       &powN, cd, "number of (power) points",
    "-rate",  ARGV_FLOAT,  NULL,       &rate, cd, "sampling rate in Hz",
    "-low",   ARGV_FLOAT,  NULL,       &low,  cd, "lowest frequency in Hz",
    "-up",    ARGV_FLOAT,  NULL,       &up,   cd, "highest frequency in Hz"
		                                  ", 0 means rate/2",
		    NULL) != TCL_OK) return TCL_ERROR;
  if (up<=0) up = rate/2.0;
  if (fbmatrixRectMel( mat, powN,  rate, low, up, N))
     return TCL_OK;
  else return TCL_ERROR;
}


/* ============================================================================
;  fbmatrixLinear
;      Precalculates the parameters of a linear filterbank.
;
;  IN:
;       rate                    : sampling rate in Hz
;       powN                    : how many points from power spectrum  analysis
;
;       low                     : lower frequency of 1st filter
;       up                      : upper frequency of last filter 
;
;       filterN                 : number of filters
;
;  OUT and RETURN:
;       A                       : band matrix
;
; ========================================================================== */
FBMatrix* fbmatrixLinear( FBMatrix* A, int powN,  float rate, float low,
		       float up, int filterN)
{
   int   x, i;
   float df = rate / (4.0 * (powN/2));   /* spacing between FFT points in Hz */
   /*   float mlow = mel(low);
	float mup  = mel(up); */
   float dm   = (up - low)/(filterN+1);    /* delta mel */

   if (low<0.0 || 2.0*up>rate || low>up) {
      ERROR("mel: something wrong with low = %f, up = %f, rate = %f\n",
	    low,up,rate);
      return NULL;
   }
   /* printf("lower = %fHz (%fmel), upper = %fHz (%fmel)\n",low,mlow,up,mup);*/

   /* -------------------------------------------
      free band matrix, allocate filterN pointer
      ------------------------------------------- */
   if (A->matPA) {
      for (i=0;i<A->m;i++)
	if (A->matPA[i]) free(A->matPA[i]); 
      free( A->matPA);
   }
   if (A->offset) free(A->offset);
   if (A->coefN) free(A->coefN);
   
   if ((A->matPA  = (float**)malloc(filterN * sizeof(float*))) == NULL ||
       (A->coefN  = (int*)malloc(filterN * sizeof(int))) == NULL ||
       (A->offset = (int*)malloc(filterN * sizeof(int))) == NULL)
      return NULL;
   A->m = filterN;
   
   /* ---------------------------
      loop over all filters
      --------------------------- */
   for (x=0;x<filterN;x++) {
      float freq, height;
      float slope1, slope2;
      int start, end;
      /* ---- left, center and right edge ---- */
	  float left   =  x     *dm + low;
	  float center = (x+1.0)*dm + low;
	  float right  = (x+2.0)*dm + low;

      /* printf("%3d: left = %fmel, center = %fmel, right = %fmel\n",
	     x,x*dm+mlow,(x+1.0)*dm+mlow,(x+2.0)*dm+mlow); */
      /* printf("%3d: left = %fHz, center = %fHz, right = %fHz\n",
	     x,left,center,right); */
      
      height = 2.0 / (right - left);          /* normalized height = 2/width */
      slope1 = height / (center - left);
      slope2 = height / (center - right);
      start  = (int)ceil(left / df);
      end    = (int)floor(right / df);
      
      A->offset[x] = start;
      A->coefN[x]  = end - start + 1;
      A->n         = end;
      if ((A->matPA[x] = malloc(A->coefN[x] * sizeof(float))) == NULL) {
	 /* free ... */
	 return NULL;
      }
      for ( freq=start*df, i=0; i<A->coefN[x]; freq += df, i++) {
	 if (freq <= center)
	    A->matPA[x][i] = slope1*(freq-left);
	 else
	    A->matPA[x][i] = slope2*(freq-right);
      }
   }
   A->rate = rate;
   return A;
}

int fbmatrixLinearItf( ClientData cd, int argc, char *argv[])
{
  FBMatrix* mat = (FBMatrix*)cd;
  int   N     = 30;
  int   powN  = 129;
  float rate  = 16000.0;
  float low   = 0;
  float up    = 0;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "-N",     ARGV_INT,    NULL,       &N,    cd, "number of filters",
    "-p",     ARGV_INT,    NULL,       &powN, cd, "number of (power) points",
    "-rate",  ARGV_FLOAT,  NULL,       &rate, cd, "sampling rate in Hz",
    "-low",   ARGV_FLOAT,  NULL,       &low,  cd, "lowest frequency in Hz",
    "-up",    ARGV_FLOAT,  NULL,       &up,   cd, "highest frequency in Hz"
		                                  ", 0 means rate/2",
		    NULL) != TCL_OK) return TCL_ERROR;
  if (up<=0) up = rate/2.0;
  if (fbmatrixLinear( mat, powN,  rate, low, up, N))
     return TCL_OK;
  else return TCL_ERROR;
}

/* ----------------------------------------------------------------------------
     The above source code is by Matthias Wölfel
   ---------------------------------------------------------------------------- */
