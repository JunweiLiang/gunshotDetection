/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Feature Module
               ------------------------------------------------------------

    Author  :  Michael Finke & Martin Westphal
    Module  :  feature.h
    Date    :  $Id: feature.h 2779 2007-03-02 13:06:36Z fuegen $

    Remarks :  This file should be included by other Janus modules 
               using the 'FeatureSet' object.

   ========================================================================

    $Log$
    Revision 3.6  2007/03/02 13:06:36  fuegen
    added code for PORTAUDIO support (Florian Metze, Friedrich Faubel)

    Revision 3.5  2000/10/12 12:57:13  janus
    Merged branch jtk-00-10-09-metze.

    Revision 3.4.24.1  2000/10/12 08:02:15  janus
    This compiles, but does not work on SUN for decoding without SIMD

    Revision 3.4.14.1  2000/10/11 09:24:41  janus
    Added SIMD instructions and preliminary MultiVar version

    Revision 3.4  2000/08/27 15:31:12  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.3.2.1  2000/08/25 22:19:43  jmcd
    Just checking.

    Revision 3.3  2000/08/24 13:32:26  jmcd
    Merging changes from branch jtk-00-08-23-jmcd.

    Revision 3.2.2.1  2000/08/24 09:32:42  jmcd
    More revision fixes.

    Revision 3.2  1997/07/09 17:42:22  westphal
    *** empty log message ***

  
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _feature
#define _feature

#include "matrix.h"             /* includes: itf.h(tcl.h), common.h         */

#ifdef PORTAUDIO
#include <portaudio.h>

  typedef struct PaInstance_S PaInstance;
#endif

/* ========================================================================
    Feature
    If you add something here change also:
     *   feInit()
     *   feCreate()
     *   feFree()
     *   feConfigureItf()
   ======================================================================== */

typedef enum { FE_UNDEF, FE_SVECTOR, FE_FMATRIX, FE_CMATRIX } FeatureType;

typedef struct {

  char*       name;             /* name of the feature                      */
  int         useN;             /* used by some other module                */
  FeatureType type;             /* feature type                             */

  int         dcoeffN;          /* demanded coefficients or 0               */
  FeatureType dtype;            /* demanded feature type                    */

  float       samplingRate;     /* sample rate in kHz                       */
  float       shift;            /* shift in ms= 1/frame_rate in kHz         */

  union {
    SVector*  svec;             /* vector of shorts                         */
    FMatrix*  fmat;             /* matrix of floats                         */
    CMatrix*  cmat;             /* matrix of char                           */
  }           data;

  /* --- runon mode ------------------------------------------------------- */
  int         trans;            /* transaction number                       */
                                /* 0 first runon block, then increasing     */
} Feature;

/* ========================================================================
    FeatureSet
    If you add something here change also:
     *   fesTypeDefaults
     *   fesCreate()
     *   fesFree()
     *   fesConfigureItf()
   ======================================================================== */

typedef struct {

  char*    name;                 /* name of the feature set                 */
  int      useN;                 /* number of modules using feature set     */
  Feature* featA;                /* array of features                       */
  int      featN;                /* size of feature array                   */ 

  float    sfshift;              /* standard frame shift in ms              */
  int      from;                 /* from in ms                              */
  int      to;                   /* to in ms                                */

  int      verbosity;            /* verbosity                               */
  
  float    samplingRate;         /* read_adc                                */
  char*    adcByteOrder;
  char*    adcHeader;
  char*    offset;
  char*    fadeIn;

  int      writeHeader;          /* Feature I/O                             */
  int      byteModeOut;
  int      byteModeIn;

  /* --- runon mode ------------------------------------------------------- */
  int      runon;                /* runon mode 0 or 1                       */
  int      trans;                /* transaction number, starting with 0     */
  int      ready;                /* runon ready, 1 for no runon             */

#ifdef PORTAUDIO
  /* --- PortAudio recording ---------------------------------------------- */
  PaInstance* portAudioInstance;
#endif

} FeatureSet;

/* ========================================================================
    prototypes
   ======================================================================== */

/* ---- init function ----------------------------------------------------- */
extern int    Feature_Init(void);

/* ---- Janus interface --------------------------------------------------- */
extern char*  fesGetName  ( FeatureSet* FS, int index);
extern int    fesGetIndex ( FeatureSet* F, char* name, int create );
extern int    fesIndex    ( FeatureSet* FS, char* name, FeatureType type, 
			    int coeffN, int create );
extern float* fesFrame    ( FeatureSet* FS, int index, int frame );
extern int    fesFrameN   ( FeatureSet* FS, int* list, int listN, int* from,
			    int* frameshift, int* ready );
extern int    fesEval     ( FeatureSet* FS, char* sampleList );


extern int    feSVector   ( FeatureSet* FS, Feature* F, SVector* V, int runonok);

#endif  /* _feature */




#ifdef __cplusplus
}
#endif
