/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Feature Module
               ------------------------------------------------------------

    Author  :  Martin Westphal
    Module  :  featureMethodItf.h
    Date    :  $Id: featureMethodItf.h 2864 2009-02-16 21:18:17Z metze $

    Remarks :  This file should be included by files 
               defining 'FeatureSet' methods.

   ========================================================================

    $ Log: featureMethodItf.h,v $
    Revision 4.6 2008/11/24 14:52:28  wolfel
    added non-linear mapping by VTS

    Revision 4.6  2007/03/02 13:06:36  fuegen
    added code for PORTAUDIO support (Florian Metze, Friedrich Faubel)

    Revision 4.5  2003/08/14 11:19:55  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 4.4.4.2  2001/02/27 18:20:42  metze
    Typo

    Revision 4.4.4.1  2001/02/27 17:31:51  metze
    Cleaned up warnings

    Revision 4.4  2001/01/15 09:49:57  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 4.3.32.1  2001/01/12 15:16:54  janus
    necessary changes for running janus under WINDOWS

    Revision 4.3  2000/09/14 11:37:00  janus
    merging branch jtk-00-09-11-fuegen-2

 * Revision 4.2.14.1  2000/09/11  16:04:41  janus
 * Merging outdated branch jtk-00-09-08-fuegen.
 * 
    Revision 4.2.8.1  2000/09/08 15:00:08  janus
    added M. Westphal signal adaption (MAM)

 * Revision 4.2  2000/08/27  15:31:14  jmcd
 * Merging branch 'jtk-00-08-24-jmcd'.
 * 
    Revision 4.1.4.1  2000/08/25 22:19:43  jmcd
    Just checking.

    Revision 4.1  2000/01/13 08:47:22  fuegen
    Added methods for modalities

    Revision 4.0  1998/11/30 18:07:14  fuegen
    *** empty log message ***

    Revision 3.5  1998/09/30 12:10:49  westphal
    adc2spec spec2adc

    Revision 3.4  1997/08/11 15:22:00  westphal
    *** empty log message ***

    Revision 3.3  1997/07/30 13:41:23  westphal
    changed FRAME to IN_RANGE

    Revision 3.2  1997/07/09 17:42:22  westphal
    *** empty log message ***

  
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#include "feature.h"        /* includes: matrix.h(itf.h(tcl.h), common.h)   */

/* ------------------------------------------------------------------------
    constants
   ------------------------------------------------------------------------ */
#define ADCRANGE      65536 /* adcrange is from -ADCRANGE/2 .. ADCRANGE/2 -1 */
#define FE_MELSCALES_N   16 /* for do_FFT() only   */
#define SLOW         -32768
#define SHIGH         32767

/* ========================================================================
>
>   How does a 'featureSet method' look like?
>
> ~~~~~~~~~~ 
>    Ia    
> ~~~~~~~~~~
int fesMethodItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet *FS = (FeatureSet*)cd;
  Feature    *destin = NULL;
  Feature    *source = NULL;
   
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<new_feature>", ARGV_CUSTOM, createFe,&destin, cd, NEW_FEAT,
    "<src_feature>", ARGV_CUSTOM, getFe,   &source, cd, SRC_FEAT,
    "<int>",         ARGV_INT,    NULL,    &i,      cd, "",
    "<float>",       ARGV_INT,    NULL,    &f,      cd, "",
    ...
    NULL) != TCL_OK) return TCL_ERROR;

> ATTENTION: If you CREATE features after you already created or got other
>            features you must use 'createFeI' and 'getFeI' because a pointer
>	     could change while new features are being created.
> ~~~~~~~~~~ 
>    Ib
> ~~~~~~~~~~ 
  FeatureSet* FS = (FeatureSet*)cd;
  int         dst = 0, src = 0;
  Feature     *destin, *source;

  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<destin_feature>", ARGV_CUSTOM, createFeI, &dst, cd,
    "name of new feature",
    "<source_feature>", ARGV_CUSTOM, getFeI,    &src, cd,
    "name of source feature",
    ...,
    NULL) != TCL_OK) return TCL_ERROR;
  destin = FS->featA + dst; 
  source = FS->featA + src; 

>   
>   Now check 'type's or other properties of source features (see
>   the macros below).
>   Do your processing and set 'samplingRate' and 'shift' with the 
>   CONFIG macro.
> ~~~~~~~~~~ 
>   II
> ~~~~~~~~~~

   if (!IS_FMATRIX(source)) {
     MUST_FMATRIX(source);
     free(source); return TCL_ERROR;
   }
   else {
     FMatrix      *mat1 = source1->data.fmat;
     int         frameA = 0;
     int frameX, frameN = mat1->m + ..;
     int coeffX, coeffN = mat1->n + ..;
     FMatrix *mat  = fmatrixFeature( frameN, coeffN, FS, destin, &frameA);
     if (!mat) return TCL_ERROR;

     foreach_coeff
        mat->matPA[frameX][coeffX] = mat1->matPA[frameX][coeffX] + ...;
	 
     if (!feFMatrix( FS, destin, mat, 1)) {
        COPY_CONFIG( destin, source1);  return TCL_OK;
     }
     fmatrixFree(mat); return TCL_ERROR;
   }
  } 
}

>Some remarks:
>  FMatrix *mat  = fmatrixFeature( frameN, coeffN, FS, destin, &frameA);
>     In runon mode a matrix pointer will be returned containing the
>     data calculated in the previous run up to frameA-1 and zeros thereafter.
>     Not in runon mode it behaves like 'fmatrixCreate'.
>  FMatrix *mat  = fmatrixCreate( frameN, coeffN);
>     Note that the MACROS 'foreach_coeff', 'foreach_frame' start from frameA. 
>
>  feFMatrix( FS, destin, mat, 1))
>     Please read comment there about 'runonok'.   
   ======================================================================== */
/* ========================================================================
    macros for FeatureSet methods
   ======================================================================== */
/* --- verbosity macros --- */
/* 0 nothing
   1 warnings
   2 important messages
   3 verbose
   4 debugging
*/

#define FE_WAR (FS->verbosity >= 1)
#define FE_MES (FS->verbosity >= 2)
#define FE_VER (FS->verbosity >= 3)
#define FE_DEB (FS->verbosity >= 4)

/* --- strings --- */
#define NEW_FEAT  "name of the new feature"
#define SRC_FEAT  "name of the source feature"
#define SRC_FEAT1 "name of source feature 1"
#define SRC_FEAT2 "name of source feature 2"

/* --- checking --- */
#define IS_FMATRIX( fe)         ( fe->type  == FE_FMATRIX)
#define IS_SVECTOR( fe)         ( fe->type  == FE_SVECTOR)
#define BOTH_FMATRIX( feA, feB) ((feA->type == FE_FMATRIX) && (feB->type == FE_FMATRIX))
#define BOTH_SVECTOR( feA, feB) ((feA->type == FE_SVECTOR) && (feB->type == FE_SVECTOR))
#define COPY_CONFIG(  feA, feB) { feA->samplingRate = feB->samplingRate;\
				  feA->shift        = feB->shift; }
#define IF_EMPTY_RANGE(_from, _to, _N)               \
     _to   = (_to >= _N || to < 0) ? _N - 1 : _to;   \
     _from = (_from < 0) ? 0 : _from;                \
     if (_to < _from)
#define MUST_FMATRIX(_feP) \
   ERROR("for command %s feature '%s' must be of type FMatrix.\n",\
	 argv[0], _feP->name)
#define MUST_SVECTOR(_feP) \
   ERROR("for command %s feature '%s' must be of type SVector.\n",\
	 argv[0], _feP->name)
#define MUST_FMATRIX2(_fe1P,_fe2P) \
ERROR("for command %s features '%s' and '%s' must be both of type FMatrix.\n",\
      argv[0], _fe1P->name, _fe2P->name)
#define MUST_SVECTOR2(_fe1P,_fe2P) \
ERROR("for command %s features '%s' and '%s' must be both of type SVECTOR.\n",\
      argv[0], _fe1P->name, _fe2P->name)
#define MUST_SAME(_fe1P,_fe2P) \
   ERROR("command %s: feature types of '%s' and '%s' differ.\n", \
	 argv[0], _fe1P->name, _fe2P->name)
#define DONT_KNOW(_feP) \
   ERROR("command %s: Don't know feature type of '%s'.\n", \
	 argv[0], _feP->name)
#define ERR_RUNON(_method) \
   ERROR("%s not a runon command\n", _method)
#define WARN_RUNON(_method) \
   WARN("%s not a runon command\n", _method)

/* --- processing --- */
#define foreach_coeff      for ( frameX=frameA; frameX<frameN; frameX++) \
                              for (coeffX=0; coeffX<coeffN; coeffX++)
#define foreach_frame      for ( frameX=frameA; frameX<frameN; frameX++) 
#define IN_RANGE( index, min, max) ((index)  < (min)) ? (min) : \
                                   (((index) > (max)) ? (max) : (index))

#define CONFIG_FE( fe, SR, SH)    { fe->samplingRate = SR; fe->shift = SH;}

/* --- misc --- */
#define streqn(s1,s2,num)    (strncmp (s1,s2,num) == 0)   /* num characters */
#define SQUARE(X) ((X)*(X))
#define ROUND(a)        (floor((a) + 0.5))
#define SLIMIT(x) \
  ((((x) < SLOW)) ? (SLOW) : (((x) < (SHIGH)) ? (x) : (SHIGH)))

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   SWITCH statement for Strings (Hermann Hild)
   Switch(name)
     Case("Herm") {printf("Hello Hermann");}
     Default      {printf("not in Case");}  
   End_switch;                                
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#define Switch(str) { char* _strP_ = str;  int _cont_ = 1;           
#define Case(str)   if (_cont_) if (!(_cont_ = strcmp(str, _strP_))) 
#define Default     if (_cont_)                                      
#define End_switch  }                                                
/*---------------------------------------------------------------------------*/



/* ========================================================================
    prototypes of functions to be used in 'FeatureSet' methods
   ======================================================================== */
extern int    feSVector(   FeatureSet* FS, Feature* F, SVector* V, int runonok);
extern int    feFMatrix(   FeatureSet* FS, Feature* F, FMatrix* M, int runonok);

/* ---- to use within itfParseArgv() -------------------------------------- */
extern int    createFe(    ClientData cd, char* key, ClientData result, 
			   int *argcP, char *argv[]);
extern int    getFe(       ClientData cd, char* key, ClientData result, 
			   int *argcP, char *argv[]);
extern int    createFeI(   ClientData cd, char* key, ClientData result, 
			   int *argcP, char *argv[]);
extern int    getFeI(      ClientData cd, char* key, ClientData result,
			   int *argcP, char *argv[]);

extern FMatrix* fmatrixFeature( int m, int n, FeatureSet* FS, Feature* F, int* frameA);
extern SVector* svectorFeature( int n, FeatureSet* FS, Feature* F, int* frameA);

extern int getSampleX(FeatureSet *FS, Feature *fe, char *time, int *sampleP);

extern int fesAppendAESVMItf (ClientData cd, int argc, char *argv[]);


/* ========================================================================
    prototypes of methodItf functions
   ======================================================================== */
/* ------------------------------------------------------------------------
    featureAudioItf.c
   ------------------------------------------------------------------------ */
extern int fesPlayItf( ClientData cd, int argc, char *argv[]);
extern int fesGetRecordItf( ClientData cd, int argc, char *argv[]);
extern int fesRecordStartItf( ClientData cd, int argc, char *argv[]);
extern int fesAudioInitItf( ClientData cd, int argc, char *argv[]);

#ifdef PORTAUDIO
extern int fesPAInfoItf(ClientData cd, int argc, char *argv[]);
extern int fesPAStartRecordingItf(ClientData cd, int argc, char *argv[]);
extern int fesPAPlayItf(ClientData cd, int argc, char *argv[]);
extern int fesPAGetRecordedItf(ClientData cd, int argc, char *argv[]);
extern int fesPAStopItf(ClientData cd, int argc, char *argv[]);
#endif

/* ------------------------------------------------------------------------
    featurePLPItf.c
   ------------------------------------------------------------------------ */
extern int fesPLPItf(  ClientData cd, int argc, char *argv[]);
extern int fesAudItf(  ClientData cd, int argc, char *argv[]);
extern int fesPaudItf( ClientData cd, int argc, char *argv[]);
/* ------------------------------------------------------------------------
    featureIOItf.c
   ------------------------------------------------------------------------ */
extern int fesWriteItf( ClientData cd, int argc, char *argv[]);
extern int fesReadItf(  ClientData cd, int argc, char *argv[]);
extern int fesHtkReadItf( ClientData cd, int argc, char *argv[]);
/* ------------------------------------------------------------------------
    featureJanus.c  (should be featureJanusItf.c)
   ------------------------------------------------------------------------ */
extern int fesFrameItf( ClientData cd, int argc, char *argv[]);
extern int fesFrameNItf( ClientData cd, int argc, char *argv[]);
/* ------------------------------------------------------------------------
    featureFFTItf.c
   ------------------------------------------------------------------------ */
extern int fesADC2SPECItf( ClientData cd, int argc, char *argv[]);
extern int fesSPEC2ADCItf( ClientData cd, int argc, char *argv[]);
/* ------------------------------------------------------------------------
    featureModalities.c
   ------------------------------------------------------------------------ */
extern int fesSNRItf      (ClientData cd, int argc, char *argv[]);
extern int fesSNRKItf     (ClientData cd, int argc, char *argv[]);
extern int fesAutocorrItf (ClientData cd, int argc, char *argv[]);
extern int fesAmdfItf     (ClientData cd, int argc, char *argv[]);
extern int fesPitchItf    (ClientData cd, int argc, char *argv[]);
extern int fesSpkRateItf  (ClientData cd, int argc, char *argv[]);
extern int fesProsodyItf  (ClientData cd, int argc, char *argv[]);
extern int fesSilenceDetectionItf (ClientData cd, int argc, char *argv[]);
extern int fesAvMagnitudeItf (ClientData cd, int argc, char *argv[]);
extern int fesZeroXItf    (ClientData cd, int argc, char *argv[]);
extern int fesPowerItf    (ClientData cd, int argc, char *argv[]);
extern int fesReorderItf  (ClientData cd, int argc, char *argv[]);
extern int fesGradientItf (ClientData cd, int argc, char *argv[]);


/* ------------------------------------------------------------------------
    featureNRItf.c
   ------------------------------------------------------------------------ */
extern int fesNoiseEstimationItf( ClientData cd, int argc, char *argv[]);
extern int fesNoiseReductionItf( ClientData cd, int argc, char *argv[]);
extern int fesSpecSubItf( ClientData cd, int argc, char *argv[]);
/* ------------------------------------------------------------------------
    featureLPCItf.c
   ------------------------------------------------------------------------ */
  /* extern int fesAutoCorItf( ClientData cd, int argc, char *argv[]);
     extern int fesLPCItf( ClientData cd, int argc, char *argv[]);
     extern int fesFormantsItf( ClientData cd, int argc, char *argv[]); */

/* ------------------------------------------------------------------------
    featureMapItf.c
   ------------------------------------------------------------------------ */
extern int fesMapItf( ClientData cd, int argc, char *argv[]);
extern int fesVTSItf( ClientData cd, int argc, char *argv[]);

#ifdef __cplusplus
}
#endif
