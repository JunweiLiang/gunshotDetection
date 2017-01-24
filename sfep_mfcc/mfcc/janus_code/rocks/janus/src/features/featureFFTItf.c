/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Feature I/O Module
               ------------------------------------------------------------

    Author  :  Martin Westphal
    Module  :  featureFFTItf.c
    Date    :  $Id: featureFFTItf.c 3416 2011-03-23 03:02:18Z metze $
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
    Revision 4.3  2001/01/15 09:49:56  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 4.2.4.1  2001/01/12 15:16:53  janus
    necessary changes for running janus under WINDOWS

    Revision 4.2  2000/11/14 12:01:12  janus
    Merged branch jtk-00-10-27-jmcd.

    Revision 4.1.26.1  2000/11/06 10:50:28  janus
    Made changes to enable compilation under -Wall.

    Revision 4.1  2000/09/14 11:36:58  janus
    merging branch jtk-00-09-11-fuegen-2

 * Revision 1.1.4.1  2000/09/11  16:04:40  janus
 * Merging outdated branch jtk-00-09-08-fuegen.
 * 
    Revision 1.1.2.1  2000/09/08 15:03:00  janus
    added M. Westphal signal adaption (MAM)

 * Revision 1.1  1998/09/30  12:09:49  westphal
 * Initial revision
 *
  
   ======================================================================== */

/* ------------------------------------------------------------------------
   includes
   ------------------------------------------------------------------------ */
#include <string.h>
#include <math.h>

#include "featureMethodItf.h"  /* includes matrix.h( itf.h(tcl.h), common.h) */
#include "featureFFT.h"

/* ------------------------------------------------------------------------
   static functions
   ------------------------------------------------------------------------ */
static float phase(float re, float im) {
  float ph;
  if (re==0.0)
    ph = (im==0.0)  ?  0.0  :  (im>0.0) ? PI/2.0 : PI/-2.0;
  else
    ph = atan2(im,re);
  /* ph = atan(im/re); if (re<0.0) ph = (im>=0.0) ? ph + PI : ph - PI; */
  return ph;
}

/* -------------------------------------------------------------------------
   window

   This routine will return a pointer to a float array with multipliers
   needed to filter a time signal window before passing it to the FFT.
   Several window types can be chosen by setting alpha and beta:
                         alpha      beta
   rectangle window       0.0        -
   Tukey window        0.0 - 0.5    0.5
   Hanning window         0.5       0.5
   Hamming window         0.5       0.46

   Sometimes the equations are given as /(N-1) sometimes as /N, so we
   use another parameter D (1 or 0) to enable both vesions.

   The allocated array must not be freed because the same pointer is 
   returned next time unless the window parameters are changed.
   ------------------------------------------------------------------------- */
static float* window( int N, float alpha, float beta, int D) {
  static float *wind = (float*)NULL;	/* window multipliers   */
  static int   prevN = -1;		/* previous window size */
  static float prevA, prevB;   		/* previous alpha, beta */
  static int   prevD;		        /* previous window size */

  if (N <= 0) return NULL;
  if (prevN != N || prevA != alpha || prevB != beta || prevD != D) {
    int    i    = 0;
    int    iE   = (N - D);
    int    i1   = alpha*N;
    int    i2   = iE - i1;
    double temp = (alpha==0.0 && N!=D) ?  0.0 : M_PI / (alpha * (N-D));

    if (wind) free(wind);
    if ((wind = (float*)malloc(N * sizeof(float))) == NULL) return wind;
    for ( ; i<i1; i++) wind[i] = (1-beta) - beta*cos(temp*i);
    for ( ; i<i2; i++) wind[i] = 1.0;
    for ( ; i<N;  i++) wind[i] = (1-beta) - beta*cos(temp*(iE-i));
    prevN = N;
    prevA = alpha; prevB = beta;
    prevD = D;
  }
  return wind;
}

/* ========================================================================
   Method 'adc2spec'
   ======================================================================== */
int fesADC2SPECItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet* FS = (FeatureSet*)cd;
  Feature*    source = NULL; int srcI = -1;
  int         reaI = -1; char* reaH = "feature with real part spectrum";
  int         imaI = -1; char* imaH = "feature with complex part spectrum";
  int         magI = -1; char* magH = "feature with magnitude";
  int         phaI = -1; char* phaH = "feature with phase";
  int         powI = -1; char* powH = "feature with power spectrum";
  int         adcI = -1; char* adcH = "feature with windowed audio signal";
  char        string[20];
  char        *cwin  = "16msec", *cshift = string;
  char        *ctype = "hamming";
  int         D = 1;
  int         winPoints, shiftPoints;
  float       samplingRate = 0;
  sprintf(string,"%fmsec",FS->sfshift);
    
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<source_feature>", ARGV_CUSTOM, getFeI,   &srcI,   cd, SRC_FEAT,
      "<win>",            ARGV_STRING, NULL,     &cwin,   cd, "window size",
      "-shift",           ARGV_STRING, NULL,     &cshift, cd, "shift",
      "-win",             ARGV_STRING, NULL,     &ctype,  cd, 
		          "window type [hamming|hanning|tukey|rect]",
      "-rea",             ARGV_CUSTOM, createFeI,&reaI,   cd, reaH,
      "-ima",             ARGV_CUSTOM, createFeI,&imaI,   cd, imaH,
      "-mag",             ARGV_CUSTOM, createFeI,&magI,   cd, magH,
      "-pha",             ARGV_CUSTOM, createFeI,&phaI,   cd, phaH,
      "-pow",             ARGV_CUSTOM, createFeI,&powI,   cd, powH,
      "-adc",             ARGV_CUSTOM, createFeI,&adcI,   cd, adcH,
      "-D",               ARGV_INT,    NULL,     &D,      cd, "",
      NULL) != TCL_OK) return TCL_ERROR;

  /* -----------------------
     check source feature
     ----------------------- */
  source = FS->featA + srcI; 
  samplingRate = source->samplingRate;

  if (! IS_SVECTOR( source) && ! IS_FMATRIX( source)) {
     ERROR("Input feature %s must be of type SVector or FMatrix.\n", argv[2]);
  }
  else if (samplingRate <= 0) {
     ERROR("Sampling rate of source feature %s must be > 0.\n", argv[2]);
  }
  else if (getSampleX(FS,source,cwin,  &winPoints) || 
           getSampleX(FS,source,cshift,&shiftPoints)) {
  }
/*  else if (shiftPoints % samplingRate) {
     ERROR("Shift must be exactly some msec but it's %f msec.\n",
	   (float)shiftPoints/(float)samplingRate);
  } */
  /* -----------------------------------------------
     everything is ok
     ----------------------------------------------- */
  else {
    int log2N, fftN, fftN2;
    FMatrix *rea = NULL, *ima = NULL, *mag = NULL, *pha = NULL, *pow = NULL, *adc = NULL;
    FMatrix *mat1 = NULL;
    SVector *vec  = NULL;
    float   *buf;
    int frameA = 0, frameX, frameN;
    int error  = 0;
    float *win = NULL;
    float shift;

    if (IS_FMATRIX( source)) {
      mat1   = source->data.fmat;
      shift  = source->shift;
      frameN = mat1->m;
      shiftPoints = (int)(samplingRate * shift);
      if (winPoints !=  mat1->n) {
	WARN("WinPoints (%d) are not equal to given feature (%d) -> will take %d\n",winPoints, mat1->n, mat1->n);
      }
      winPoints = mat1->n;
    }
    else {
      vec    = source->data.svec;
      frameN = (int)((vec->n - (winPoints - shiftPoints)) / shiftPoints);
      shift  = shiftPoints/samplingRate;
    }
    log2N = ceil(  log((double)winPoints) / log(2.0) );
    fftN  = 0x1<<log2N; /* 2^log2N */
    fftN2 = fftN / 2;
    buf   = (float*)malloc( fftN * sizeof(float) );

    if (FE_VER) fprintf(STDERR,
      "spec: winPoints = %d, shiftPoints = %d, frameN = %d fftN = %d\n",
      winPoints, shiftPoints, frameN, fftN);

    /* ---- calculate window ---- */
    Switch (ctype) 
      Case("hamming") {win = window(winPoints, 0.5, 0.46, D);}
      Case("hanning") {win = window(winPoints, 0.5, 0.50, D);}
      Case("tukey")   {
	float alpha   = 1.0 - (float)shiftPoints/(float)winPoints;
	win = window( winPoints, alpha, 0.50, D);
      }
      Case("rect")    {}
      Default {
        ERROR("Don't know window type %s!\n",ctype);
        ERROR("Use 'hamming' or 'rect'!\n");
        return TCL_ERROR;
      }
    End_switch;
    if (FE_VER) {
      fprintf(STDERR,"spec: Using %s window\n",ctype);
      if (win) {
	int i; for (i=0;i<winPoints;i++) fprintf(STDERR,"%d: %f\n",i,win[i]);
      }
    }

    /* ---- allocate feature matrices ---- */
    if (reaI >= 0) {rea  = fmatrixCreate(frameN, fftN2 + 1);}
    if (imaI >= 0) {ima  = fmatrixCreate(frameN, fftN2 + 1);}
    if (magI >= 0) {mag  = fmatrixCreate(frameN, fftN2 + 1);}
    if (phaI >= 0) {pha  = fmatrixCreate(frameN, fftN2 + 1);}
    if (powI >= 0) {pow  = fmatrixCreate(frameN, fftN2 + 1);}
    if (adcI >= 0) {adc  = fmatrixCreate(frameN, fftN);}

    foreach_frame {
      int i;
      /* window */
      if (IS_FMATRIX( source)) {
	if (win) for (i=0; i<winPoints; i++) buf[i] = mat1->matPA[frameX][i]*win[i];
	else for (i=0; i<winPoints; i++) buf[i] = mat1->matPA[frameX][i];
      }
      else {
	short* inA = vec->vecA + (frameX * shiftPoints);
	if (win) for (i=0; i<winPoints; i++) buf[i] = inA[i]*win[i];
	else for (i=0; i<winPoints; i++) buf[i] = inA[i];
      }

      /* fill with 0 */
      for (i=winPoints; i<fftN; i++) buf[i] = 0.0;

      if (adc) {
	for (i=0; i<fftN; i++) adc->matPA[frameX][i] = buf[i];
      }

      /* FFT */
      if (FAST(buf, fftN) == 0) {
	ERROR("Calculating FFT for frame %d\n,frameX");
	free(buf);
	if (rea) fmatrixFree(rea); if (ima) fmatrixFree(ima);
	if (mag) fmatrixFree(mag); if (pha) fmatrixFree(pha);
	if (pow) fmatrixFree(pow); if (adc) fmatrixFree(adc);
	return TCL_ERROR;
      }

      /* fill matrices with data */
      if (rea) {
	for (i=0; i<fftN2; i++) rea->matPA[frameX][i] = buf[2*i];
	rea->matPA[frameX][fftN2] = buf[1];
      }
      if (ima) {
	ima->matPA[frameX][0] = 0.0;
	for (i=1; i<fftN2; i++) ima->matPA[frameX][i] = buf[2*i+1];
	ima->matPA[frameX][fftN2] = 0.0;
      }
      if (pha) {
	pha->matPA[frameX][0] = phase(buf[0],0.0);
	for (i=1; i<fftN2; i++) 
	  pha->matPA[frameX][i] = phase(buf[2*i],buf[2*i+1]);
	pha->matPA[frameX][fftN2] = phase(buf[1],0.0);
      }
      if (pow) {
	pow->matPA[frameX][0] = buf[0] * buf[0];
	for (i=1; i<fftN2; i++) 
	  pow->matPA[frameX][i] = buf[2*i]*buf[2*i] + buf[2*i+1]*buf[2*i+1];
	pow->matPA[frameX][fftN2] = buf[1] * buf[1];
	if (mag) {
	  for (i=0; i<=fftN2; i++)
	    mag->matPA[frameX][i] = sqrt( pow->matPA[frameX][i] );
	}
      } else if (mag) {
	mag->matPA[frameX][0] = sqrt( buf[0] * buf[0] );
	for (i=1; i<fftN2; i++) 
	  mag->matPA[frameX][i] = sqrt( buf[2*i]*buf[2*i] + 
					buf[2*i+1]*buf[2*i+1] );
	mag->matPA[frameX][fftN2] = sqrt( buf[1] * buf[1] );
      }
    }
    free(buf);
#define DO1( _MAT_ , _MATI_ ) \
    if (_MAT_) {\
       Feature* dest = FS->featA + _MATI_;\
       if (feFMatrix( FS, dest, _MAT_, 1)) {\
         error = 1; fmatrixFree(_MAT_);\
       } else {\
         CONFIG_FE(dest, samplingRate, shift);\
       };\
    }
    DO1(rea,reaI);
    DO1(ima,imaI);
    DO1(mag,magI);
    DO1(pha,phaI);
    DO1(pow,powI);
    DO1(adc,adcI);
    if (!error) return TCL_OK;
  }
  return TCL_ERROR;
}	 
	   
/* ========================================================================
   Method 'spec2adc'
   ======================================================================== */
int fesSPEC2ADCItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet* FS = (FeatureSet*)cd;
  Feature*    destin  = NULL;
  Feature*    source1 = NULL;
  Feature*    source2 = NULL;
  /* char     *cwin   = "16msec";
  int         winPoints; */
  char        *ctype = "none";
  int         D = 1;
  float       sr      = FS->samplingRate;
    
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>",        ARGV_CUSTOM, createFe, &destin, cd, NEW_FEAT,
      "<source_feature1>",ARGV_CUSTOM, getFe,    &source1,cd, "magnitude",
      "<source_feature2>",ARGV_CUSTOM, getFe,    &source2,cd, "phase",
   /* "<win>",            ARGV_STRING, NULL,     &cwin,   cd, "window size", */
      "-win",             ARGV_STRING, NULL,     &ctype,  cd, 
		          "window type [tukey|none]",
      "-sr",              ARGV_FLOAT,  NULL,     &sr,     cd, "sampling rate in kHz",
      "-D",               ARGV_INT,    NULL,     &D,      cd, "",
      NULL) != TCL_OK) return TCL_ERROR;

  if (! BOTH_FMATRIX( source1, source2)) {
     MUST_FMATRIX2( source1, source2);
  }
  else {
    FMatrix *mat1 = source1->data.fmat;
    FMatrix *mat2 = source2->data.fmat;
    int frameN    = MIN(mat1->m,mat2->m);
    SVector *vec;

    if (frameN==0) vec = svectorCreate(0);
    else if (frameN>0 && (mat1->n != mat2->n)) { 
      ERROR(""); return TCL_ERROR;
    }
    else { 
      int fftN      = 2 * (mat1->n - 1);
      int winPoints = fftN; /* could be <fftN and should be set by the user */ 
      int shift     = ROUND(source1->shift * sr);
      int over      = winPoints - shift;
      float   *buf  = (float*)malloc( fftN * sizeof(float) );/* IFFT result */ 
      float   *buf2 = (float*)malloc( over * sizeof(float) );/* overlap */
      int frameA    = 0, frameX;
      int sampleN   = shift * (frameN-1) + winPoints; 
      int i;
      short* out = NULL;
      float *win = NULL;

      vec  = svectorCreate(sampleN);
      assert(buf && buf2 && vec);

      for ( i=0; i<fftN; i++) buf[i]  = 0.;  /* clear buffer */
      for ( i=0; i<over; i++) buf2[i] = 0.; /* clear buffer for overlap */

      /* ---- calculate window ---- */
      Switch (ctype) 
        Case("tukey")   {
	  float alpha   = 1.0 - (float)shift/(float)winPoints;
	  win = window( winPoints, alpha, 0.50, D);
        }
        Case("none")    {}
        Default {
          ERROR("Don't know window type %s!\n",ctype);
          ERROR("Use 'tukey' or 'none'!\n");
          return TCL_ERROR;
        }
      End_switch;


      foreach_frame {
	/* IFFT */
	maPh2real(buf, fftN, mat1->matPA[frameX], mat2->matPA[frameX]);

	/* window */
	if (win) for (i=0; i<winPoints; i++) buf[i] = buf[i]*win[i];
	
	/* add old overlap to result and store new overlap */
	for ( i=0; i<over; i++) buf[i] += buf2[i];
	for ( i=0; i<over; i++) buf2[i] = buf[shift+i];
	
	out = vec->vecA + frameX*shift;
	assert (out); // XCode says could be NULL
	for ( i=0; i<shift; i++) out[i] = SLIMIT(buf[i]);
      }
      assert (out); // XCode says could be NULL
      for ( i=shift; i<winPoints; i++) out[i] = SLIMIT(buf[i]);
      free(buf); free(buf2);
    }
    if(!feSVector( FS, destin, vec, 0)) {
      CONFIG_FE( destin, sr, 0); return TCL_OK;
    }
    svectorFree(vec);
  }
  return TCL_ERROR;
}

