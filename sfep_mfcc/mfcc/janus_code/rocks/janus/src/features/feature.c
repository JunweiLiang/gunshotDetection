/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Feature Module
               ------------------------------------------------------------

    Author  :  Martin Westphal
    Module  :  feature.c
    Date    :  $Id: feature.c 3418 2011-03-23 15:07:34Z metze $
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
    Revision 3.31  2007/03/21 16:49:51  fuegen
    added adaptation factor in fesMeansubItf (Thilo)

    Revision 3.30  2007/03/21 15:49:38  fuegen
    added Qin's feature warpping based on CDF matching for robust speaker recognition

    Revision 3.29  2007/03/19 15:05:18  fuegen
    added W2MVDR from Matthias W.

    Revision 3.28  2007/03/02 13:06:36  fuegen
    added code for PORTAUDIO support (Florian Metze, Friedrich Faubel)

    Revision 3.27  2007/02/22 16:41:13  fuegen
    removed bug in fesReplace

    Revision 3.26  2007/02/21 12:33:38  fuegen
    added Early Feature Vector Reduction (EFVR) code from Thilo Koehler
    added fesAppendItf to append data to another features
    some minor changes in fesConcatItf

    Revision 3.25  2007/02/08 09:47:55  stueker
    Added discrete wavelet transformation code from Michael Wand

    Revision 3.24  2006/11/02 15:14:57  fuegen
    ADC feature will be deinitialized before reading the new ADC

    Revision 3.23  2005/12/14 14:44:49  fuegen
    added fesReplaceItf

    Revision 3.22  2005/03/04 09:06:42  metze
    Reactivated Christian's silDetCF

    Revision 3.21  2004/09/16 14:35:59  metze
    Made code 'x86_64'-clean

    Revision 3.20  2004/09/10 15:42:48  metze
    Added fesMixN (preliminary)

    Revision 3.19  2004/05/27 11:26:57  metze
    Fixed output

    Revision 3.18  2004/05/10 14:16:29  metze
    Yue's changes

    Revision 3.17  2003/08/14 11:19:52  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.16.4.10  2003/07/02 17:15:31  fuegen
    fixes for file handling under Windows
    using general functions for file open/close

    Revision 3.16.4.9  2003/06/06 07:56:54  fuegen
    made gcc 3.3 clean

    Revision 3.16.4.8  2003/04/16 12:59:35  metze
    MVDR added (Matthias Woelfel)

    Revision 3.16.4.7  2002/08/01 13:42:30  metze
    Fixes for clean documentation.

    Revision 3.16.4.6  2002/02/15 07:19:34  metze
    Bugfix in fefree (freed too much)

    Revision 3.16.4.5  2001/08/08 15:16:02  fuegen
    fixed problem in file_exists in runon mode

    Revision 3.16.4.4  2001/07/09 14:39:15  soltau
    Added trainMode in featureCompress

    Revision 3.16.4.3  2001/07/06 13:11:58  soltau
    Changed compress and scoreFct handling

    Revision 3.16.4.2  2001/07/04 18:10:22  soltau
    Changed compression interface

    Revision 3.16.4.1  2001/07/03 09:36:39  soltau
    Support for compressed features

    Revision 3.16  2001/01/15 09:49:56  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 3.15.4.1  2001/01/12 15:16:52  janus
    necessary changes for running janus under WINDOWS

    Revision 3.15  2000/11/14 12:01:12  janus
    Merged branch jtk-00-10-27-jmcd.

    Revision 3.14.12.1  2000/11/06 10:50:27  janus
    Made changes to enable compilation under -Wall.

    Revision 3.14  2000/10/12 12:57:12  janus
    Merged branch jtk-00-10-09-metze.

    Revision 3.13.16.1  2000/10/12 08:02:15  janus
    This compiles, but does not work on SUN for decoding without SIMD

    Revision 3.13.10.1  2000/10/11 09:24:40  janus
    Added SIMD instructions and preliminary MultiVar version

    Revision 3.13  2000/09/14 11:36:57  janus
    merging branch jtk-00-09-11-fuegen-2

 * Revision 3.12.20.1  2000/09/11  16:04:32  janus
 * Merging outdated branch jtk-00-09-08-fuegen.
 * 
    Revision 3.12.14.1  2000/09/08 15:00:07  janus
    added M. Westphal signal adaption (MAM)

 * Revision 3.12  2000/08/16  09:19:53  soltau
 * removed memory leak: free filter
 * 
    Revision 3.11  2000/02/07 17:05:09  soltau
    Added formants

    Revision 3.10  1998/09/03 13:43:31  westphal
    some SERROR -> ERROR

    Revision 3.9  1997/08/22 10:42:05  westphal
    runon for 'offset' with delta, mean calculation only with new data

    Revision 3.8  1997/08/22 09:57:45  westphal
    flag -count for offset

    Revision 3.7  1997/08/14 17:42:26  westphal
    'offset': new flag -delta, default mean

    Revision 3.6  1997/08/11 15:29:03  westphal
    methods frame frameN

    Revision 3.5  1997/08/06 07:38:16  westphal
    removed memory leak in fesReadADCItf

    Revision 3.4  1997/07/30 13:41:47  westphal
    bug fix for runon 'delta' 'shift' adjacent'

    Revision 3.3  1997/07/30 11:58:46  westphal
    -f for method readADC

    Revision 3.2  1997/07/09 17:42:22  westphal
    *** empty log message ***

  
   ======================================================================== */

/* ========================================================================
    includes
   ======================================================================== */

#include <limits.h>
#include <errno.h>
#include <math.h>
#include "featureMethodItf.h"           /* includes: Audio, PLP, IO         */

                                        /* Methods defined here:            */
                                        /* ---------------------            */
#include "featureADC.h"                 /* read/write audio data            */
#include "featureFFT.h"                 /* FFT and melscale calculataion    */
#include "featureDWT.h"                 /* fast wavelet transformation      */
#include "featureFilter.h"              /* filter object and methods        */
#include "featureDownsample.h"          /* down sampling to phone quality   */

#include "formants.h"
#include "featureModalities.h"          /* methods for modalities           */
#include "common.h"
#include "codebook.h"
#include "distrib.h"

#ifdef LIBSNDFILE
#include <sndfile.h>
#endif

/* --------------------------------------------------------------------------*/
/* PARTICLE FILTER */
/* --------------------------------------------------------------------------*/
/* #include "RNG.h" */
#include "NCPF.h"
#include "ParticleFilter.h"
#include "GaussianMixture.h"
#include "NoiseCompensation.h"
#include "NoiseModel.h"
#include "SpeechModel.h"
#include "Transcription.h"

char featureRcsVersion[]= 
  "@(#)1$Id: feature.c 3418 2011-03-23 15:07:34Z metze $";

/* ========================================================================
    Feature
    ~~~~~~~
      feInit              used by FeatureSet to create and free features
      feDeinit             "
      feFree               "

      feConfigureItf      configure function
      feAccessItf         access function
      fePutsItf           method 'puts'

      feSVector           used by FeatureSet methods
      feFMatrix            "
   ======================================================================== */

extern TypeInfo featureInfo;

/* ------------------------------------------------------------------------
    Create Feature Structure
   ------------------------------------------------------------------------ */
static int feInit( FeatureSet* FS, Feature* F, char* name, FeatureType type)
{
  F->name         = (name) ? (char*)strdup(name) : NULL;
  F->useN         = -1;
  F->type         =  type;
  F->dcoeffN      =  0;
  F->dtype        =  FE_UNDEF;
  F->samplingRate =  FS->samplingRate;
  F->shift        =  FS->sfshift;
  F->data.fmat    =  NULL;
  F->trans        =  -1;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Free
   ------------------------------------------------------------------------ */
static int feDeinit( Feature* F)
{
  switch ( F->type) {
  case FE_SVECTOR:
    if ( F->data.svec) svectorFree(F->data.svec);
    break;
  case FE_FMATRIX:
    if ( F->data.fmat) fmatrixFree(F->data.fmat);
    break;
  case FE_UNDEF:
    break;
  default:
    break;
  }
  F->data.fmat = NULL;
  F->type      = FE_UNDEF;
  return TCL_OK;
}

static int feFree( Feature* F)
{
  feDeinit( F);
  if ( F->name) free( F->name);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Configure
   ------------------------------------------------------------------------ */
static int feConfigureItf( ClientData cd, char *variable, char *value )
{
  Feature* F = (Feature*)cd;
  if (! F) return TCL_ERROR;

  if (! variable) {
    ITFCFG( feConfigureItf, cd, "samplingRate");
    ITFCFG( feConfigureItf, cd, "shift");
    if IS_FMATRIX( F) {
       ITFCFG( feConfigureItf, cd, "frameN")
       ITFCFG( feConfigureItf, cd, "coeffN")
    }
    else if IS_SVECTOR( F) {
       ITFCFG( feConfigureItf, cd, "sampleN");
    }
    ITFCFG( feConfigureItf, cd, "dcoeffN")
    ITFCFG( feConfigureItf, cd, "trans");
    return TCL_OK;
  }
  ITFCFG_Float( variable,value,"samplingRate",F->samplingRate,0);
  ITFCFG_Float( variable,value,"shift"       ,F->shift,       0);
  if IS_FMATRIX( F) {
     ITFCFG_Int( variable,value,"frameN",(F->data.fmat->m),1);
     ITFCFG_Int( variable,value,"coeffN",(F->data.fmat->n),1);
  }
  else if IS_SVECTOR( F) {
     ITFCFG_Int( variable,value,"sampleN",(F->data.svec->n),1);
  }
  ITFCFG_Int( variable,value,"dcoeffN",F->dcoeffN, (F->useN>0));
  ITFCFG_Int( variable,value,"trans",  F->trans,        0);
	ERROR("unknown option '-%s %s'.\n", variable,  value ? value : "?");
  return TCL_ERROR;
}  

/* ------------------------------------------------------------------------
    Access
   ------------------------------------------------------------------------ */
static ClientData feAccessItf( ClientData cd, char *name, TypeInfo **ti )
{
  Feature* F = (Feature*)cd;

  if (*name=='.') { 
    if (name[1]=='\0') {
      itfAppendResult( "data");
      *ti = NULL; return NULL;
    }
    else {
      name++;
      if (! strcmp( name, "data")) {
        switch ( F->type) {
        case ( FE_SVECTOR) :
          *ti = itfGetType("SVector");
          return (ClientData)F->data.svec;

        case ( FE_FMATRIX) :
          *ti = itfGetType("FMatrix");
          return (ClientData)F->data.fmat;
	case ( FE_CMATRIX) :
          *ti = itfGetType("CMatrix");
          return (ClientData)F->data.cmat;

        default :
          ERROR("undefined type of sub-object 'data'.\n");
          return NULL;
        }
      }
      *ti=NULL; return NULL;
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    Puts
   ------------------------------------------------------------------------ */
static int fePutsItf( ClientData cd, int argc, char *argv[])
{
  Feature* feP = (Feature*)cd;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0, 
		   NULL) != TCL_OK) return TCL_ERROR;
  itfAppendResult( "%s {useN %d} ", (feP->name) ? feP->name : "<undef>", 
                                     feP->useN);
  if      (feP->type == FE_FMATRIX)
     itfAppendResult( "{type FMatrix} {frameN %d} {coefN %d}", 
                        feP->data.fmat->m, feP->data.fmat->n);
  else if (feP->type == FE_CMATRIX)
     itfAppendResult( "{type CMatrix} {frameN %d} {coefN %d}", 
                        feP->data.cmat->m,feP->data.cmat->n);
  else if (feP->type == FE_SVECTOR)
     itfAppendResult( "{type SVector} {frameN %d} {coefN 1}", 
                        feP->data.svec->n);
  else
     itfAppendResult( "{type <undef>} {frameN 0} {coefN 0}");

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    feSVector
    Attach 'SVector V' to 'Feature F'.
    runonok should be 1 if the new feature won't change due to
    additional frames of the source feature(s), otherwise 0
   ------------------------------------------------------------------------ */
int feSVector( FeatureSet* FS, Feature* F, SVector* V, int runonok)
{
  if ( F->useN > 0 && F->dtype != FE_UNDEF && F->dtype != FE_SVECTOR) {
    ERROR("Feature '%s' is in use and mustn't be of type SVector!\n",F->name);
    return -1;
  }
  if (FS->runon && !runonok)
    WARN("RUNON: Data of feature %s might change!\n", F->name);

  feDeinit( F);
  F->type         = FE_SVECTOR;
  F->data.svec    = V;
  F->trans        = FS->trans;
  return 0;
}

/* ------------------------------------------------------------------------
    feFMatrix
    Attach 'FMatrix M' to 'Feature F'.
    see also feSVector
   ------------------------------------------------------------------------ */
int feFMatrix( FeatureSet* FS, Feature* F, FMatrix* M, int runonok)
{
  if ( F->useN > 0  &&  F->dcoeffN > 0  &&  F->dcoeffN != M->n) {
    ERROR("Feature '%s' is in use and must have %d coefficients not %d.\n",
	   F->name,F->dcoeffN,M->n);
    return -1;
  }
  if ( F->useN > 0  &&  F->dtype != FE_UNDEF  &&  F->dtype != FE_FMATRIX) {
    ERROR("Feature '%s' is in use and mustn't be of type FMatrix!\n",F->name);
    return -1;
  }
  if (FS->runon  &&  F->trans >= FS->trans) {
    ERROR("RUNON: Feature %s already used in this transaction (%d,%d)\n"
	  "       Please use other name\n",
	  F->name, F->trans, FS->trans);
    return -1;
  }
  if (FS->runon && !runonok)
    WARN("RUNON: Data of feature %s might change!\n", F->name);
  
  feDeinit( F);
  F->type         = FE_FMATRIX;
  F->data.fmat    = M;
  F->trans        = FS->trans;
  return 0;
}

/* ========================================================================
    FeatureSet
    ~~~~~~~~~~
        fesInit
        fesCreate
      fesCreateItf      creator function
        fesFree
      fesFreeItf        destructor function
      fesConfigureItf   configure function 
      fesAccessItf      access function

        fesGetName
      fesGetNameItf     method 'name'
        fesGetIndex
      fesGetIndexItf    method 'index'

      featureGet        used by FeatureSet methods
      createFe          used by FeatureSet methods (itfParseArgv)
      getFe               "
      createFeI           "
      getFeI              "
      fmatrixFeature    used by FeatureSet methods
      svectorFeature      "
      getSampleX          "
      getSample2X         "

      ... other methods
   ======================================================================== */
static FeatureSet fesTypeDefaults = 
{ 
   NULL,        /* name           */
   0,           /*                */
   NULL,        /* feature array  */
   0,           /* number         */

   10,          /* standard shift */
   0,           /* from           */ 
   -1,          /* to, -1 = end   */ 
   0,           /* verbosity      */

   16,          /* samplingRate   */
   "auto",      /* adcByteOrder   */
   "auto",      /* adcHeader      */
   "0",         /* offset         */
   "0",         /* fade_in        */

   1,           /* writeHeader    */
   1,           /* byteModeOut    */
   1,           /* byteModeIn     */
      
   0,           /* runon          */
   0,           /* transaction    */
   1            /* ready          */
#ifdef PORTAUDIO
   ,NULL
#endif
};

/* ------------------------------------------------------------------------
    Create Feature Set Structure containing a list of Feature objects
   ------------------------------------------------------------------------ */
static int fesInit( FeatureSet* fesP, char* name)
{
  Feature     *feA;
  int          j;

  if (! (feA = (Feature*)malloc( 10 * sizeof(Feature)))) {
    SERROR( "Can't allocate feature set list '%s'.\n", name);
    return TCL_ERROR;
  }
  fesP->name         = strdup(name);
  fesP->useN         = fesTypeDefaults.useN;
  fesP->featA        = feA;
  fesP->featN        = 10;

  fesP->sfshift      = fesTypeDefaults.sfshift;
  fesP->from         = fesTypeDefaults.from;
  fesP->to           = fesTypeDefaults.to;
  fesP->verbosity    = fesTypeDefaults.verbosity;

  fesP->samplingRate =         fesTypeDefaults.samplingRate  ;
  fesP->adcByteOrder = strdup( fesTypeDefaults.adcByteOrder );
  fesP->adcHeader    = strdup( fesTypeDefaults.adcHeader    );
  fesP->offset       = strdup( fesTypeDefaults.offset       );
  fesP->fadeIn       = strdup( fesTypeDefaults.fadeIn       );

  fesP->writeHeader  =         fesTypeDefaults.writeHeader   ;
  fesP->byteModeOut  =         fesTypeDefaults.byteModeOut   ;
  fesP->byteModeIn   =         fesTypeDefaults.byteModeIn    ;

  fesP->runon        =         fesTypeDefaults.runon         ;
  fesP->trans        =         fesTypeDefaults.trans         ;
  fesP->ready        =         fesTypeDefaults.ready         ;

#ifdef PORTAUDIO
  fesP->portAudioInstance = NULL;
#endif

  for ( j = 0; j < fesP->featN; j++)
    feInit( fesP, feA + j, NULL, FE_UNDEF);

  return TCL_OK;
}

static FeatureSet *fesCreate( char *name)
{
  FeatureSet  *fesP = (FeatureSet*)malloc( sizeof(FeatureSet));
  
  if (! fesP || fesInit( fesP, name) != TCL_OK) {
    if ( fesP) free( fesP);
    ERROR( "Cannot allocate FeatureSet %s.\n", name);
    return NULL;
  }
  return fesP;
}

static ClientData fesCreateItf( ClientData cd, int argc, char *argv[])
{
  char* name = NULL;
  if (itfParseArgv("FeatureSet", &argc, argv, 0,
		   "<name>", ARGV_STRING, NULL, &name, cd, "featureSet name",
		   NULL) != TCL_OK) return NULL;

  return (ClientData)fesCreate(name);
}

/* ------------------------------------------------------------------------
    fesFree
   ------------------------------------------------------------------------ */
extern int paStop (FeatureSet*);

static void fesFree( FeatureSet* F)
{
  int i;

  assert( F);
  for ( i = 0; i < F->featN; i++)
    feFree( F->featA + i);
  if (F->featA)        free( F->featA);
  if (F->adcByteOrder) free( F->adcByteOrder); 
  if (F->adcHeader)    free( F->adcHeader); 
  if (F->offset)       free( F->offset);
  if (F->fadeIn)       free( F->fadeIn);
  if (F->name)         free( F->name);

#ifdef PORTAUDIO
  if (F->portAudioInstance)
    paStop(F);    
#endif

  free( F);
}

static int fesFreeItf( ClientData cd )
{
  fesFree( (FeatureSet*)cd);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Configure
   ------------------------------------------------------------------------ */
static int fesConfigureItf( ClientData cd, char *variable, char *value )
{
  FeatureSet* F = (FeatureSet*)cd;
  F = (F) ?   F : &fesTypeDefaults;

  if (! variable) {
    ITFCFG(fesConfigureItf,cd,"name");
    ITFCFG(fesConfigureItf,cd,"useN");

    ITFCFG(fesConfigureItf,cd,"verbosity");
    
    ITFCFG(fesConfigureItf,cd,"frameShift");
    ITFCFG(fesConfigureItf,cd,"from");
    ITFCFG(fesConfigureItf,cd,"to");

    ITFCFG(fesConfigureItf,cd,"samplingRate");
    ITFCFG(fesConfigureItf,cd,"adcByteOrder");
    ITFCFG(fesConfigureItf,cd,"adcHeader");
    ITFCFG(fesConfigureItf,cd,"offset");
    ITFCFG(fesConfigureItf,cd,"fadeIn");

    ITFCFG(fesConfigureItf,cd,"writeHeader");
    ITFCFG(fesConfigureItf,cd,"byteModeOut");
    ITFCFG(fesConfigureItf,cd,"byteModeIn");

    ITFCFG(fesConfigureItf,cd,"runon")
    ITFCFG(fesConfigureItf,cd,"trans")
    ITFCFG(fesConfigureItf,cd,"ready")
    return TCL_OK;
  }
  ITFCFG_CharPtr(variable,value,"name",         F->name,         1);
  ITFCFG_Int(    variable,value,"useN",         F->useN,         1);

  ITFCFG_Int(    variable,value,"verbosity",    F->verbosity,    0);
					       
  ITFCFG_Float(  variable,value,"frameShift",   F->sfshift,      0);
  ITFCFG_Int(    variable,value,"from",         F->from,         0);
  ITFCFG_Int(    variable,value,"to",           F->to,           0);

  ITFCFG_Float(  variable,value,"samplingRate", F->samplingRate, 0);
  ITFCFG_CharPtr(variable,value,"adcByteOrder", F->adcByteOrder, 0);
  ITFCFG_CharPtr(variable,value,"adcHeader",    F->adcHeader,    0);
  ITFCFG_CharPtr(variable,value,"offset",       F->offset,       0);
  ITFCFG_CharPtr(variable,value,"fadeIn",       F->fadeIn,       0);

  ITFCFG_Int(    variable,value,"writeHeader",  F->writeHeader,  0);
  ITFCFG_Int(    variable,value,"byteModeOut",  F->byteModeOut,  0);
  ITFCFG_Int(    variable,value,"byteModeIn",   F->byteModeIn,   0);

  ITFCFG_Int(    variable,value,"runon",        F->runon,        0);
  ITFCFG_Int(    variable,value,"trans",        F->trans,        0);
  ITFCFG_Int(    variable,value,"ready",        F->ready,        0);
  ERROR("unknown option '-%s %s'.\n", variable, value ? value : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    Access
   ------------------------------------------------------------------------ */
static ClientData fesAccessItf( ClientData cd, char *name, TypeInfo **ti )
{
  FeatureSet* F = (FeatureSet*)cd;

  if (*name==':') { 
    if (name[1]=='\0') {
      int i;
      for ( i = 0; i < F->featN; i++) {
        if ( F->featA[i].useN >= 0 && F->featA[i].name) 
             itfAppendResult( "%s ",  F->featA[i].name);
      }
      *ti = NULL; return NULL;
    }
    else {
      int i;
      name++;
      for ( i = 0; i < F->featN; i++) {
        if ( F->featA[i].useN >= 0 && F->featA[i].name &&
           ! strcmp( F->featA[i].name, name)) {

          *ti = &featureInfo;
          return (ClientData)&(F->featA[i]);
        }
      }
    }
  }
  *ti = NULL;
  return NULL;
}

/* ------------------------------------------------------------------------
    fesGetName

    Returns a pointer to the name corresponding to the given index
    or NULL if no such feature exists.
   ------------------------------------------------------------------------ */
char* fesGetName( FeatureSet* FS, int index)
{
  if (index < 0 || index >= FS->featN) return NULL;
  else return FS->featA[index].name;
}

static int fesGetNameItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  int i;
  int error = 0;
  
  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
      "<idx*>",ARGV_REST,NULL,&i,NULL,"list of feature indices",
      NULL) != TCL_OK) return TCL_ERROR;

  while ( i++ < argc) {
    int     tokc;
    char  **tokv;
    int     j = 0;

    if ( Tcl_SplitList (itf,argv[i],&tokc,&tokv) != TCL_OK) return TCL_ERROR;
    while ( j < tokc) {
      int  i = atoi(tokv[j]);
      char *name = fesGetName( FS, i);

      if (name) itfAppendElement("%s", name);
      else    {
	itfAppendElement("<none>");
	ERROR("no index %d in FeatureSet %s\n",i,FS->name);
	error = 1;
      }
      j++;
    }
    Tcl_Free((char*)tokv);
  }
  if (error) return TCL_ERROR;
  else return TCL_OK;
}

/* ------------------------------------------------------------------------
    fesGetIndex

    Returns the index corresponding to the given feature name 
    or < 0 if the feature does not exist or could not be created.

    'create' = 1 means that a feature with the given name is created and
                 the index is returned.
    'create' = 0 returns the feature index or -1 if the desired feature
                 couldn't be found.
   ------------------------------------------------------------------------ */
int fesGetIndex( FeatureSet* F, char* name, int create)
{
  int freeX = -1;
  int i;
  
  for ( i = 0; i < F->featN; i++) {
    if (F->featA[i].name && ! strcmp( F->featA[i].name, name)) return i;
    if (freeX < 0 && ! F->featA[i].name) freeX = i;
    /* printf("%d %d %s\n",i,freeX,F->featA[i].name?F->featA[i].name:"<undef>"); */
  }
  if (!create) {
    SERROR("Can't find feature %s in %s\n",name,F->name);
    return -1;
  }

  if ( freeX < 0) {  /* if there no space left to insert the new feature
                        double the size of the featA */
    int j;
    freeX     =  F->featN;
    F->featA  = (Feature*)realloc( F->featA, 2 * F->featN * sizeof(Feature));

    for ( j = F->featN; j < 2*F->featN; j++) 
          feInit( F, F->featA + j, NULL, FE_UNDEF);
    F->featN *=  2;
  }
  F->featA[freeX].name = strdup(name);
  F->featA[freeX].useN = 0;
  return freeX;
}

static int fesGetIndexItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  int i;
  
  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
      "<names*>",ARGV_REST,NULL,&i,NULL,"list of feature names",
      NULL) != TCL_OK) return TCL_ERROR;

  while ( i++ < argc) {
    int     tokc;
    char  **tokv;
    int     j = 0;

    if ( Tcl_SplitList (itf,argv[i],&tokc,&tokv) != TCL_OK) return TCL_ERROR;

    while ( j < tokc) {
      int index = fesGetIndex(FS,tokv[j],0);
      itfAppendElement("%d",index);
      j++;
    }
    Tcl_Free((char*)tokv);
  }
  MSGCLEAR(NULL);
  return TCL_OK;

}


/* ------------------------------------------------------------------------
    featureGet

    Returns a pointer to the feature corresponding to the given name.

    'create' = 1 means that a feature with the given name is created and
                 the pointer is returned. NULL means something is wrong.
    'create' = 0 returns the feature pointer or NULL if the desired feature
                 couldn't be found.
   ------------------------------------------------------------------------ */
static Feature *featureGet( FeatureSet* F, char* name, int create)
{
  int index = fesGetIndex( F, name , create);

  if (index >= 0)  return (F->featA + index);
  else             return NULL;
}

/* ------------------------------------------------------------------------
    createFe, getFe
    createFeI, getFeI

    Use these functions together with the function itfParseArgv():

    ATTENTION: If you CREATE features after you already created or got other
           features you must use 'createFeI' and 'getFeI' because a pointer
	   could change while new features are created.
  ------------------------------------------------------------------------ */
int createFe( ClientData cd, char* key, ClientData result, 
	     int *argcP, char *argv[])
{
   FeatureSet* FS = (FeatureSet*)cd;
   Feature* F;
   
   if ((*argcP < 1) || ((F=featureGet(FS,argv[0],1)) == NULL)) {
     SERROR("Can't create feature '%s' in featureSet '%s' as %s\n", argv[0], FS->name, key);
     return -1;
   }
   *((Feature**)result) = F;
   (*argcP)--; return 0;
}

int getFe( ClientData cd, char* key, ClientData result,
	  int *argcP, char *argv[]
)
{
   FeatureSet* FS = (FeatureSet*)cd;
   Feature* F;
   
   if ((*argcP <1) || ((F=featureGet(FS,argv[0],0)) == NULL)) {
     SERROR("Can't get feature '%s' in featureSet '%s' as %s\n", argv[0], FS->name, key);
     return -1;
   }
   *((Feature**)result) = F;
   (*argcP)--; return 0;
}

int createFeI( ClientData cd, char* key, ClientData result, 
	      int *argcP, char *argv[])
{
   FeatureSet* FS = (FeatureSet*)cd;
   int index;
   
   if ((*argcP < 1) || ((index=fesGetIndex(FS,argv[0],1)) < 0)) {
     SERROR("Can't create feature '%s' in featureSet '%s' as %s\n", argv[0], FS->name, key);
     return -1;
   }
   *((int*)result) = index;
   (*argcP)--; return 0;
}

int getFeI( ClientData cd, char* key, ClientData result,
	   int *argcP, char *argv[])
{
   FeatureSet* FS = (FeatureSet*)cd;
   int index;
   
   if ((*argcP <1) || ((index=fesGetIndex(FS,argv[0],0)) < 0)) {
     SERROR("Can't get feature '%s' in featureSet '%s' as %s\n", argv[0], FS->name, key);
     return -1;
   }
   *((int*)result) = index;
   (*argcP)--; return 0;
}

/* ------------------------------------------------------------------------
     fmatrixFeature, svectorFeature

     Returns a pointer to a float matrix or NULL if an error occured.
     For runon mode the existing part of the feature is copied and the
     next frame number to come passed back in frameA.

     Similar for SVector Feature.
   ------------------------------------------------------------------------ */
FMatrix* fmatrixFeature( int m, int n, FeatureSet* FS, Feature* F, int* frameA)
{
  if (FS->runon) {
    FMatrix* old = F->data.fmat;
    FMatrix* new;
    
    if (F->trans >= FS->trans) {
      ERROR("RUNON: Feature %s already used in this transaction (%d,%d)\n",
	    F->name, F->trans, FS->trans); return NULL;
    }

    if (FS->trans == 0) {
      *frameA  = 0;
      return fmatrixCreate(m,n);
    }

    if (!IS_FMATRIX(F)) {
      ERROR("RUNON: Feature %s is not FMatrix\n", F->name); return NULL;
    }
    if (old->n != n) {
      ERROR("RUNON:  Feature %s mustn't change dimension in runon mode\n",
	    F->name); return NULL;
    }
    if (old->m > m) {
      ERROR("RUNON:  Feature %s has already more frames (%d) then demanded(%d)\n",
	    F->name, F->data.fmat->m, m); return NULL;
    }
    *frameA = F->data.fmat->m;
    if ((new = fmatrixCreate(m,n)) == NULL) return NULL;
    if (old->m * old->n) memcpy( new->matPA[0], old->matPA[0], sizeof(float) * old->n * old->m);
    return new;
  }
  else {
    *frameA  = 0;
    return fmatrixCreate(m,n);
  }
}

SVector* svectorFeature( int n, FeatureSet* FS, Feature* F, int* frameA)
{
  if (FS->runon) {
    SVector* old = F->data.svec;
    SVector* new;
    
    if (F->trans >= FS->trans) {
      ERROR("RUNON: Feature %s already used in this transaction (%d,%d)\n",
	    F->name, F->trans, FS->trans); return NULL;
    }

    if (FS->trans == 0) {
      *frameA  = 0;
      return svectorCreate(n);
    }

    if (!IS_SVECTOR(F)) {
      ERROR("RUNON: Feature %s is not SVECTOR\n", F->name); return NULL;
    }
    if (old->n > n) {
      ERROR("RUNON:  Feature %s has already more samples (%d) then demanded(%d)\n",
	    F->name, F->data.svec->n, n); return NULL;
    }
    *frameA = F->data.svec->n;
    if ((new = svectorCreate(n)) == NULL) return NULL;
    if (old->n) memcpy( new->vecA, old->vecA, sizeof(short) * old->n);
    return new;
  }
  else {
    *frameA = 0;
    return svectorCreate(n);
  }
}

/* ------------------------------------------------------------------------
     getSampleX

     return 0   and a frame or sample number in 'sampleX'
                allowing different formats
     or    -1   when error occured
     note: negative values are allowed!
   ------------------------------------------------------------------------ */
int getSampleX(FeatureSet *FS, Feature *fe, char *time, int *sampleP)
{
   if (fe && time) {
      int sampleN = 0; 
      float rate  = fe->samplingRate > 0.0 ? fe->samplingRate : FS->samplingRate;
      float shift = fe->shift        > 0.0 ? fe->shift        : FS->sfshift;
      float val;
      int n; char unit;

      if IS_FMATRIX(fe) sampleN = fe->data.fmat->m;
      if IS_SVECTOR(fe) sampleN = fe->data.svec->n;
      if (streqn("last",time,4) || streqn("end",time,3)) {
	 *sampleP = sampleN; return 0;
      }
      
      if ((n=sscanf(time,"%f%c",&val,&unit)) == 1) {
	 *sampleP = val;
	 return 0;
      }
      else if (n==2) {
	 if IS_FMATRIX(fe) switch (unit) {
	  case 'm': if (shift>0.0) {
	               *sampleP = ROUND(val / shift);
		       return 0;
		    } break;
	  case 's': if (shift>0.0) {
	               *sampleP = ROUND(1000.0 * val / shift);
		       return 0;
		    } break;
	  case 'f': *sampleP = ROUND(val); return 0;
	  case 'p': if (shift>0.0 && rate>0.0) {			   
	               *sampleP = ROUND(val / (shift * rate));
		       return 0;
		    } break;
	 }							   
	 if IS_SVECTOR(fe) switch (unit) {			   
	  case 'm': if (rate>0.0)  {
	               *sampleP = ROUND(val * rate);
		       return 0;
		    } break;
	  case 's': if (rate>0.0)  {
	               *sampleP = ROUND(1000.0 * val * rate);
		       return 0;
		    } break;
	  case 'f': if (rate>0.0 && shift>0.0) {			   
	               *sampleP = ROUND(val * rate * shift);
		       return 0;
		    } break;
	  case 'p': *sampleP = ROUND(val); return 0;
	 }
      }
   }
   if (fe) SERROR("No valid time value '%s' for feature %s\n"
	  "(shift or rate might be <= 0).\n", time,fe->name);
   return -1;
}
static int getSample2X(char *time, float rate, int *sampleP)
{
   float val;
   int n; char unit;

   if (streqn("last",time,4) || streqn("end",time,3)) {
      *sampleP = -1; return 0;
   }
   if ((n=sscanf(time,"%f%c",&val,&unit)) == 1) {
      *sampleP = val;
      return 0;
   }
   else if (n==2) {
      switch (unit) {			   
       case 'm': if (rate>0.0) { *sampleP = ROUND(val * rate);          return 0;} break;
       case 's': if (rate>0.0) { *sampleP = ROUND(1000.0 * val * rate); return 0;} break;
      }
   }
   if (rate > 0.0)
      SERROR("No valid time value '%s' (must be <number>, <number>s.. or <number>m..).\n", time);
   else
      SERROR("No valid time value '%s' (sampling rate must be > 0).\n", time);
   return -1;
}


/* ========================================================================

   ... more featureSet methods

   ======================================================================== */
/* ------------------------------------------------------------------------
    Method 'puts'
    Give all features.
   ------------------------------------------------------------------------ */

static int fesPutsItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet* F = (FeatureSet*)cd;
  int         i;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0, 
		   NULL) != TCL_OK) return TCL_ERROR;
  for ( i = 0; i < F->featN; i++) {
    if ( F->featA[i].useN >= 0 && F->featA[i].name) 
      itfAppendResult( "%s ",  F->featA[i].name);
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Method 'delete'
   ------------------------------------------------------------------------ */

static int fesDeleteItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet* F = (FeatureSet*)cd;
  int         i, c;
  int         retCode = TCL_OK;

  for ( c = 1; c < argc; c++) {
    for ( i = 0; i < F->featN; i++) {

      if ( F->featA[i].name && ! strcmp( F->featA[i].name, argv[c])) {
        if ( F->featA[i].useN > 0) {

           ERROR("feature '%s' in use %d.\n",argv[c],F->featA[i].useN);
           retCode = TCL_ERROR;
        }
        else { 
          feDeinit( F->featA + i);
          free( F->featA[i].name); F->featA[i].name = NULL;
          F->featA[i].useN = -1;
        }
      }
    }
  }
  return retCode;
}

/* ------------------------------------------------------------------------
    Method 'readDes'
    Read a 'Feature Description' from file.
   ------------------------------------------------------------------------ */
#define STRING_MAX 1000

static int fesSetDesc( FeatureSet* FS, char* proc) 
{
  DString str;
  int     rc  = TCL_OK;

  if (proc && proc[0]=='@') {
    ERROR("Couldn't find feature description file %s\n",proc + 1);
    return TCL_ERROR;
  }
  dstringInit(   &str);
  dstringAppend( &str, "proc featureSetEval<%s> {fes {sampleList {}}} {\n", 
                        FS->name);
  dstringAppend( &str, "set sampleList [$fes access $sampleList]\n");
  dstringAppend( &str, "makeArray arg $sampleList\n");
  dstringAppend( &str, "%s\n", proc);
  dstringAppend( &str, "}\n");

  rc = Tcl_GlobalEval( itf, dstringValue( &str));

  dstringFree(   &str);
  return rc;
}

static int fesSetDescItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet* FS = (FeatureSet*)cd;
  int         ac =  argc - 1;
  char*       body = (char *) NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "@<filename>|<command>", ARGV_STRING,  NULL, &body, cd,
      "@ and name of 'Feature Description' or Tcl command", NULL) != TCL_OK) 
       return TCL_ERROR;
  else return fesSetDesc( FS, body);
}

/* ------------------------------------------------------------------------
    fesSetAccItf    Read a 'File access description' from file.
   ------------------------------------------------------------------------ */

static int fesSetAcc( FeatureSet* FS, char* proc)
{
  DString str;
  int     rc  = TCL_OK;

  if (proc && proc[0]=='@') {
    ERROR("Couldn't find feature access file %s\n",proc + 1);
    return TCL_ERROR;
  }
  dstringInit(   &str);
  dstringAppend( &str, "proc featureSetAccess<%s> { fes {sampleList {}}} {\n",
                        FS->name);
  dstringAppend( &str, "set accessList \"\"\n");
  dstringAppend( &str, "makeArray arg $sampleList\n");
  dstringAppend( &str, "%s\n", proc);
  dstringAppend( &str, "return $accessList\n");
  dstringAppend( &str, "}\n");

  rc = Tcl_GlobalEval( itf, dstringValue( &str));

  dstringFree(   &str);
  return rc;
}

static int fesSetAccItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet* FS   = (FeatureSet*)cd;
  int         ac   =  argc-1;
  char*       body =  NULL;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
		   "@<filename>|<command>", ARGV_STRING,  NULL, &body, cd,
		   "@ and name of 'Feature Description File' or Tcl command",
		   NULL) != TCL_OK) return TCL_ERROR;

  return fesSetAcc( FS, body);
}

/* ------------------------------------------------------------------------
    Method 'eval'
    Create features by calling the 'eval procedure'.
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
    fesEval   evaluate feature description script given a list of sample
              references
   ------------------------------------------------------------------------ */

int fesEval( FeatureSet* FS, char* sampleList)
{
  DString str;
  int     rc = TCL_OK;

  dstringInit(   &str);
  dstringAppend( &str, "%s eval {%s}\n", FS->name, sampleList);
  rc = Tcl_Eval( itf, dstringValue( &str));
  dstringFree(   &str);
  return rc;
}

/* ------------------------------------------------------------------------
    Method 'SVector'
   ------------------------------------------------------------------------ */

static int fesSVectorItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet* FS = (FeatureSet*)cd;
  ClientData  vec;

  if ( argc < 3) {
    ERROR("Usage: <FeatureSet> SVector <name> <vector>.\n");
    return TCL_ERROR;
  }

  if ( (vec = svectorCreateItf( NULL, argc-1, argv+1)) != NULL ) {
    int i = fesGetIndex( FS, argv[1], 1);

    if (! feSVector( FS, FS->featA+i, (SVector*)vec, 1)) {
       CONFIG_FE( (FS->featA+i), FS->samplingRate, 0);
       return TCL_OK;
    }
  }
  ERROR("could not create 'SVector %s'.\n",argv[1]);
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    Method 'FMatrix'
   ------------------------------------------------------------------------ */

static int fesFMatrixItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet *FS = (FeatureSet*)cd;
  ClientData  mat;
  

  if ( argc < 2) {
    ERROR("give name of the new feature.\n");
    return TCL_ERROR;
  }

  if ( (mat = fmatrixCreateItf( NULL, argc-1, argv+1)) != NULL ) {
     int i = fesGetIndex( FS, argv[1], 1);
     
     if (! feFMatrix( FS, FS->featA+i, (FMatrix*)mat, 1)) {
			 CONFIG_FE( (FS->featA+i), FS->samplingRate, 0);
			 return TCL_OK;
     }
  }
  ERROR("could not create 'FMatrix %s'.\n", argv[1]);
  return TCL_ERROR;
}


/* ------------------------------------------------------------------------
    Method 'readADC'
   ------------------------------------------------------------------------ */

static int feOffsetCorrection(SVector* vec, int offcor, int offset)
{
   if (offcor) {
      if (offcor > 0) svectorAddScalar( vec, - offset);
      else {
	 double mean;
	 if (svectorMean(vec,&mean) > 0) svectorAddScalar( vec, - (int)mean);
      }
   }
   return 0;
}

static int feFadeIn(SVector* vec, int sampleN)
{
   int i; float samples = (float)sampleN;
   
   if (sampleN) {
      if (sampleN > vec->n) sampleN = vec->n;
      for (i=0;i<sampleN;i++)
	 vec->vecA[i] = (short)((float)i * (float)vec->vecA[i] / samples);
   }
   return sampleN;
}

static int file_exist(char *filename)
{
  FILE *fp;
  int ret;
  fp=fileOpen(filename,"r"); 
  ret = ( (fp==(FILE*)0) ? 0 : 1);
  if (ret) fileClose(filename, fp);
  return(ret);
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
static int fesReadADCItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet *FS = (FeatureSet*)cd;
  Feature    *destin       = NULL;
  char       *filename     = NULL;
  float      samplingRate  = 0;            /* user sampling rate, undefined */
  int        hSRHz;                        /* header sampling rate in Hz    */ 
  float      headSamplRate = 0;            /* header sampling rate          */ 
  char       *adcByteOrder = FS->adcByteOrder;
  char       *adcHeader    = FS->adcHeader;
  int        force         = 0;
  int        chX  = 1, chN = 1;
  char       *cfrom = "0", *cto  = "last";
  int        from, to;
  int        verbosity = 1;
  char       *offset       = FS->offset;
  char       *fadeIn       = FS->fadeIn;
  char       *startFile    = "adc.start";
  char       *readyFile    = "adc.ready";
  float      sleepTime     = 0.1;
  int        rmFiles       = 1;
#ifdef LIBSNDFILE
  char           *fformat  = "RAW";    /* The input audiofile format*/
  char           *subtype  = "PCM_U8"; /* The subtype of the input audio file. Default value:
					  Unsigned 8 bit data (WAV and RAW only)*/
  char         *endianness = "LITTLE"; /* The endianess of the file*/
#endif

  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
	       "<feature>",  ARGV_CUSTOM, createFe, &destin,       cd, NEW_FEAT,
	       "<filename>", ARGV_STRING, NULL,     &filename,     cd, "name of ADC file",
	       "-hm",        ARGV_STRING, NULL,     &adcHeader,    cd, "header mode, kind or size in byte",
	       "-bm",        ARGV_STRING, NULL,     &adcByteOrder, cd, "byte mode",
	       "-f",         ARGV_INT,    NULL,     &force,        cd, "1 => skip unnecessary bytes when reading",
	       "-chX",       ARGV_INT,    NULL,     &chX,          cd, "selected channel: 1..chN",
	       "-chN",       ARGV_INT,    NULL,     &chN,          cd, "number of channels",
	       "-from",      ARGV_STRING, NULL,     &cfrom,        cd, "from",
	       "-to",        ARGV_STRING, NULL,     &cto,          cd, "to",
	       "-sr",        ARGV_FLOAT,  NULL,     &samplingRate, cd, "sampling rate in kHz",
	       "-offset",    ARGV_STRING, NULL,     &offset,       cd, "subtract offset",
	       "-fadeIn",    ARGV_STRING, NULL,     &fadeIn,       cd, "fade in",
	       "-v",         ARGV_INT,    NULL,     &verbosity,    cd, "verbosity",
	       "-startFile", ARGV_STRING, NULL,     &startFile,    cd, "runon: name of start file",
	       "-readyFile", ARGV_STRING, NULL,     &readyFile,    cd, "runon: name of ready file",
	       "-sleep",     ARGV_FLOAT,  NULL,     &sleepTime,    cd, "runon: time to wait before next try",
	       "-rmFiles",   ARGV_INT,    NULL,     &rmFiles,      cd, "runon: remove files",
#ifdef LIBSNDFILE
               "-ff",        ARGV_STRING, NULL,     &fformat,      cd, "The input audiofile format",
               "-subtp",     ARGV_STRING, NULL,     &subtype,      cd, "The subtype of the input audiofile",
               "-endns",     ARGV_STRING, NULL,     &endianness,   cd, "The endian-ness of the input audiofile",
#endif
	       NULL) != TCL_OK) return TCL_ERROR;

  if (getSample2X(cfrom, samplingRate, &from) || getSample2X(cto, samplingRate, &to)) {
     if (samplingRate == 0) ERROR("Use '-sr <rate in kHz>' to set sampling rate\n");
     return TCL_ERROR;
  }
  if (FS->runon && (from!=0 || to!=-1 || fadeIn!=FS->fadeIn)) {
     WARN("Sorry, -from,-to and -fadeIn will be ignored in RUNON mode\n");
  }
  
  destin->samplingRate = samplingRate > 0 ? samplingRate : FS->samplingRate;
  destin->shift        = 0;
     
  {
     int fadeTo;
     SVector* vec    = svectorCreate(0);
     int oldN        = 0;                    /* how many old samples */
     if (!vec) return TCL_ERROR;

     adc_verbosity( verbosity );
     /* ---------------------------
	Read from file, runon mode
	--------------------------- */
     if (FS->runon) {
       if (FS->ready) { /* the last file was ready, now we start again */
	 int i;
	 FS->ready = 0;
	 FS->trans = 0;
	 for (i=0; i<FS->featN; i++) FS->featA[i].trans = -1;
	 if (strneq(startFile,"") && strneq(startFile,filename)) {
	   if (!file_exist(startFile)) {
	     INFO("Waiting for start file %s\n",startFile);
	     while(!file_exist(startFile)) usleep((long)(1000000.0 * sleepTime));
	   }
	   if (rmFiles) remove(startFile);
	 }
	 if (!file_exist(filename)) {
	   INFO("Waiting for audio file %s\n",filename);
	   while(!file_exist(filename)) usleep((long)(1000000.0 * sleepTime));
	 }
       } else {
	 (FS->trans)++;
	 /* check if destin is SVector !!!!!! */
	 oldN = destin->data.svec->n;
       }
       while (vec->n==0 && FS->ready==0) {
	 if (file_exist(readyFile)) FS->ready = 1;
#ifdef LIBSNDFILE
	 vec->n = readADCrunon( filename, &(vec->vecA), adcHeader,
			    adcByteOrder, chX, chN, (FS->trans==0), FS->ready, fformat, subtype, endianness);
#else
	 vec->n = readADCrunon( filename, &(vec->vecA), adcHeader,
			    adcByteOrder, chX, chN, (FS->trans==0), FS->ready);
#endif
	 if (FS->ready && rmFiles) {
	   if (streq(startFile,"") || streq(startFile,filename)) {
	     remove(filename);
	   }
	   remove(readyFile);
	 }
	 else if (vec->n==0) usleep((long)(1000000.0 * sleepTime));
       }
     }
     /* ---------------------------
	Read from file, normal mode
	--------------------------- */
     else {
       feDeinit (destin);
#ifdef LIBSNDFILE
       vec->n = readADC( filename, &(vec->vecA),
			adcHeader, adcByteOrder, chX, chN, from, to, force, fformat, subtype, endianness);
#else
       vec->n = readADC( filename, &(vec->vecA),
			adcHeader, adcByteOrder, chX, chN, from, to, force);
#endif
     }
     if (vec->n < 0) {
	ERROR("Can't read ADC file \"%s\".\n", filename);
	perror(NULL);
	FS->ready = 1;
	vec->n = 0; svectorFree(vec); return TCL_ERROR;
     }
     
     /* --------------------------
	Header sampling rate ?
	-------------------------- */
     if (getADCsamplRate(&hSRHz) && hSRHz != 0) { 
	headSamplRate = hSRHz / 1000.0;           /* kHz not Hz */
	if (samplingRate == 0)                    /* nothing defined by user */
	   destin->samplingRate  =  headSamplRate;
	else if ( samplingRate != headSamplRate)
	   WARN("User defined sampling rate %f kHz and header sampling rate %f kHz differ\n",samplingRate,headSamplRate);
     }
     
     /* ---------------------------
	Offset correction, fade-in
	--------------------------- */
     if (FS->runon) {
       int start;
       SVector* new = svectorFeature(oldN+vec->n,FS,destin,&start);
       if (!new) return TCL_ERROR;
       assert(oldN==start);

       if (vec->n) memcpy( new->vecA + oldN, vec->vecA, sizeof(short) * vec->n);
       svectorFree(vec);
       if (streqc(offset,"mean")) 
	 WARN("readADC: cannot remove offset 'mean' in runon mode!\n");
       else {
	 int m = atoi(offset), i = oldN;
	 for ( ; i < new->n; i++) new->vecA[i] = SLIMIT(new->vecA[i] - m);
       }
       if (feSVector( FS, destin, new, 1)) {
	 svectorFree(new); return TCL_ERROR;
       }
     }
     else {
       if (streqc(offset,"mean")) feOffsetCorrection( vec,-1, 0);
       else                       feOffsetCorrection( vec, 1, atoi(offset));
       if (feSVector( FS, destin, vec, 1)) {
	 svectorFree(vec); return TCL_ERROR;
       }
       if (getSampleX(FS,destin,fadeIn,&fadeTo)==0) feFadeIn(vec,fadeTo);
     }
   }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
   Method 'writeADC'
   ------------------------------------------------------------------------ */
static int fesWriteADCItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*) cd;
  Feature     *source = NULL;
  char        *filename = NULL;
  char        *cfrom = "0", *cto = "last";
  char        *adcHeader = "";
  char        *adcByteMode = "auto";
  int         from, to;
  int         verbosity = 1;
  int         append = 0;

  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
	"<source_feature>", ARGV_CUSTOM, getFe, &source,   cd, SRC_FEAT,
	"<filename>",       ARGV_STRING, NULL,  &filename, cd, "file to write",
	"-hm",              ARGV_STRING, NULL,  &adcHeader,cd, "header kind or \"\" for no header",
        "-bm",              ARGV_STRING, NULL,  &adcByteMode, cd, "byte mode",
	"-from",            ARGV_STRING, NULL,  &cfrom,    cd, "from",
	"-to",              ARGV_STRING, NULL,  &cto,      cd, "to",
	"-append",          ARGV_INT,    NULL,  &append,   cd, "append to file",
	"-v",               ARGV_INT,    NULL,  &verbosity,cd, "verbosity",
	NULL) != TCL_OK) return TCL_ERROR;
  
  if (getSampleX(FS,source,cfrom,&from) || getSampleX(FS,source,cto,&to))
      return TCL_ERROR;

  /* ------------------
     SHORT features
     ------------------ */
  if (IS_SVECTOR( source)) {
     SVector  *vec = source->data.svec;
     
     IF_EMPTY_RANGE(from, to, vec->n) {
       /* empty range for run-on mode means: close the file in writeADC */
       if (!streq(adcHeader, "RUNON")) {
	 ERROR("Given range is empty! samples 0 .. %d available.\n",vec->n - 1);
	 return TCL_ERROR;
       }
     }
     adc_verbosity( verbosity );
     if (writeADC(filename, vec->vecA + from, to - from + 1, adcHeader,
		  adcByteMode, (int) 1000*source->samplingRate, append) < 0) {
       ERROR("can't write ADC file: %s\n", filename);
       return TCL_ERROR;
     }
     return TCL_OK;
  }
  MUST_SVECTOR(source);
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
   Method 'resample'
   ------------------------------------------------------------------------ */
#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

/* do si interpolation */
static short fesIPsi(SVector *vec, float index, int order)
{
   int prev = floor((double)index);
   int next = ceil((double)index);
   float val= 0.0, x;
   int i;
   
   if (next >= vec->n || prev < 0) return 0;
   if (prev == next) return vec->vecA[prev];
   for ( i=prev>=order?prev-order:0; i<vec->n && i<=next+order; i++) {
     x    =  (index-i) * M_PI;
     val +=  (vec->vecA[i]*sin(x)/x);
   }
   return SLIMIT(ROUND(val));
}

/* Given a "float index" for a short vector do linear interpolation
   and return value                                                         */ 
static short fesIPlin(SVector *vec, float index)
{
   int prev = floor((double)index);
   int next = ceil((double)index);
   float di = index - (float)prev;

   if (next >= vec->n || prev < 0) return 0;
   if (prev == next) return vec->vecA[prev];
   return ROUND((1.0 - di) * vec->vecA[prev] + di * vec->vecA[next]);

}

static void fesMIPlin(FMatrix *des, int i, FMatrix *src, float index)
{
   int prev = floor((double)index);
   int next = ceil((double)index);
   int j;
   float di = index - (float)prev;
   assert(des->n == src->n);

   if (prev >= 0 && next < src->m) {
      if (prev == next) for (j=0; j<des->n; j++)
	 des->matPA[i][j] = src->matPA[prev][j];
      else for (j=0; j<des->n; j++)
	 des->matPA[i][j] = (1.0 - di) * src->matPA[prev][j] + di * src->matPA[next][j];
   }
}

static int fesResADCItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet *FS = (FeatureSet*)cd;
  Feature    *destin = NULL;
  Feature    *source = NULL;
  float      rate    = 16.0;
  int        order   = 0;
  char       *style  = "lin";
     
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<feature>",        ARGV_CUSTOM, createFe,  &destin, cd, NEW_FEAT,
    "<source_feature>", ARGV_CUSTOM, getFe,     &source, cd, SRC_FEAT,
    "<rate/shift>",     ARGV_FLOAT,  NULL,      &rate,   cd,
		                                "new sampling rate in kHz for SVector "
		                                "or new shift in ms for FMatrix",
    "-style",           ARGV_STRING, NULL,      &style,  cd, "'lin' or 'si' (short only!)",
    "-order",           ARGV_INT,    NULL,      &order,  cd, "order for 'si'",
    NULL) != TCL_OK) return TCL_ERROR;
  
  if (source->samplingRate <= 0) {
     ERROR("%s: Sampling rate of source is %.0f kHz.\n",
	   argv[0], source->samplingRate); return TCL_ERROR;
  }
  if (rate <= 0) {
     ERROR("%s: Please give samplingrate (or frame shift) > 0 kHz (0 ms).\n", argv[0]);
     return TCL_ERROR;
  }
  
  if (IS_SVECTOR( source)) {
     SVector *vec1 = source->data.svec;
     float      di = source->samplingRate / rate;
     int i,sampleN = 1 + floor((double)(vec1->n - 1) / di);
     int   runonok = 0;
     SVector  *vec = svectorFeature(sampleN,FS,destin,&i);
     if (!vec) return TCL_ERROR;
     
     Switch( style)
	Case( "lin") {
          for (; i<sampleN;i++)
	    vec->vecA[i] = fesIPlin(vec1, (float)(i * di));
	  runonok = 1;
	}
        Case( "si") {
	  if (order<0) order=0;
	  for (; i<sampleN;i++)
	    vec->vecA[i] = fesIPsi(vec1, (float)(i * di), order);
	}
        Default {
	   ERROR("Don't know style '%s', use 'lin' or 'si'\n",style);
	   return TCL_ERROR;
	}
     End_switch;

     if(!feSVector( FS, destin, vec, runonok)) {
	destin->samplingRate = rate;
	destin->shift        = 0;
	return TCL_OK;
     }
     svectorFree(vec); return TCL_ERROR;
  }
  else if (IS_FMATRIX( source) && source->shift <= 0) {
     ERROR("Frame shift of feature %s mustn't be %.0f for 'resample'.\n",
	   argv[2], source->shift);  return TCL_ERROR;
  }
  else if (IS_FMATRIX( source)) {
     FMatrix *mat1 = source->data.fmat;
     float      di = rate / source->shift;
     int i,frameN = 1 + floor((double)(mat1->m - 1) / di);
     FMatrix  *mat = fmatrixFeature(frameN,mat1->n,FS,destin,&i);

     for ( ; i<frameN; i++)
        fesMIPlin(mat, i, mat1, (float)(i * di));
     
     if(!feFMatrix( FS, destin, mat, 1)) {
	destin->samplingRate = source->samplingRate;
	destin->shift        = rate;
	return TCL_OK;
     }
     fmatrixFree(mat); return TCL_ERROR;
  }
  ERROR("Don't know feature type of %s.\n", argv[2]);  return TCL_ERROR;
}


/* ------------------------------------------------------------------------
   Method 'downsample'
   ------------------------------------------------------------------------ */
static int fesDownADCItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet *FS = (FeatureSet*)cd;
  Feature    *destin = NULL;
  Feature    *source = NULL;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<feature>",        ARGV_CUSTOM, createFe,  &destin, cd, NEW_FEAT,
    "<source_feature>", ARGV_CUSTOM, getFe,     &source, cd, SRC_FEAT,
    NULL) != TCL_OK) return TCL_ERROR;
  
  if (source->samplingRate != 16) {
     ERROR("%s: Sampling rate of source must be 16 kHz.\n", argv[0]);
     return TCL_ERROR;
  }
  
  if (IS_SVECTOR( source)) {
     SVector *vec1 = source->data.svec;
     int i,sampleN = vec1->n;
     short x;
     SVector  *vec = svectorFeature((sampleN+1)/2,FS,destin,&i);

     for (i = 0; i < sampleN; i++) {
	x = vec1->vecA[i];
	x = ds_filter1(x,i);
	x = ds_filter2(x,i);
	if (i%2 == 0) vec->vecA[i/2] = ds_compand(x);
     }
     
     if(!feSVector( FS, destin, vec, 1)) {
	destin->samplingRate = 8;
	destin->shift        = 0;
	return TCL_OK;
     }
     svectorFree(vec); return TCL_ERROR;
  }
  MUST_SVECTOR(source);
  return TCL_ERROR;

}
     

/* ------------------------------------------------------------------------
   Display
   ------------------------------------------------------------------------ */
static int fesDisplayItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *feature;
  /* ------------------
     get arguments
     ------------------ */
  if ( (argc<3)  ||  ((feature = featureGet( FS,argv[1],0))  ==  NULL)) {
     ERROR("Usage: <FeatureSet> %s <feature> <canvas>.\n",argv[0]);
     return TCL_ERROR;
  }
  
  /* -------------------
     FLOAT features
     ------------------- */
  if (IS_FMATRIX( feature)) {
     return fmatrixDisplayItf( (ClientData)feature->data.fmat, argc-1, argv + 1);
     
  }	 
  /* ------------------
     SHORT features
     ------------------ */
  else if (IS_SVECTOR( feature)) {
     return fmatrixDisplayItf( (ClientData)feature->data.svec, argc-1, argv + 1);
  }
  ERROR("Don't know feature type of %s.\n", argv[2]);  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
   Peak
   racking peaks
   ------------------------------------------------------------------------ */
static int fesPeakItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin  = NULL;
  Feature     *source = NULL;
  float       hz_min  = 300;
  float       hz_max  = 5000;
  float       sr      = 16000;

  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
	"<new_feature>",  ARGV_CUSTOM, createFe,&destin, cd,
		          "peaks of <feature>",
	"<feature>", ARGV_CUSTOM, getFe,   &source,    cd, SRC_FEAT,
	"-hz_min" ,  ARGV_FLOAT,  NULL,    &hz_min,    cd, "",
	"-hz_max",   ARGV_FLOAT,  NULL,    &hz_max,    cd, "",
	"-sr",       ARGV_FLOAT,  NULL,    &sr,        cd, "",
	NULL) != TCL_OK) return TCL_ERROR;


  if (IS_FMATRIX( source )) {
    int frN    = source->data.fmat->m;
    int dim    = source->data.fmat->n;
    int   imin = floor (dim * hz_min / (sr / 2.0) );
    int   imax = floor (dim * hz_max / (sr / 2.0) );
    int i,frX; 
    FMatrix *C = fmatrixCreate(frN,dim);

    for (frX=0;frX<frN;frX++) {
      int peakN  =  0;
      float vmax =  source->data.fmat->matPA[frX][imin] -10.0;
      float vmin =  source->data.fmat->matPA[frX][imin] +10.0;
      float v0   =  source->data.fmat->matPA[frX][imin];
      float v1   =  source->data.fmat->matPA[frX][imin+1];
      float v;
      int steigung= (v1 > v0) ? 1 : 0;
      
      for (i=imin;i< imax; i++) {
	v=  source->data.fmat->matPA[frX][i];
	if (steigung) {
	  if (v < vmax) {
	    C->matPA[frX][peakN]= (sr / 2.0) * i / dim;
	    peakN++;
	    steigung= 0;
	    vmin = v;
	  } else {
	    vmax= v;
	  }
	} else {
	  if (v > vmin) {
	    steigung = 1;
	    vmax = v;
	  } else {
	    vmin= v;
	  }
	}
      }
    }
    feFMatrix( FS,  destin, C, 1); return TCL_OK;
  }

  return TCL_ERROR;
}


/* ------------------------------------------------------------------------
   Corr
   correlation of two features
   ------------------------------------------------------------------------ */
static int fesCorrItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin  = NULL;
  Feature     *source1 = NULL;
  Feature     *source2 = NULL;
  int         pad = 1;
  int         from = 0, to = 0;
  int	      step = 1;
  int	      sstep = 1;
  int         dim  = 1;
    
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
	"<new_feature>",  ARGV_CUSTOM, createFe,&destin, cd,
		          "correlation of <featureA> and <featureB>",
	"<featureA>", ARGV_CUSTOM, getFe,   &source1,    cd, SRC_FEAT1,
	"<featureB>", ARGV_CUSTOM, getFe,   &source2,    cd, SRC_FEAT2,
	"-from",      ARGV_INT,    NULL,    &from,       cd, "", 
	"-to",        ARGV_INT,    NULL,    &to,         cd, "",
	"-step",      ARGV_INT,    NULL,    &step,       cd, "", 
	"-samplestep",ARGV_INT,    NULL,    &sstep,      cd, "", 
	"-pad",       ARGV_INT,    NULL,    &pad,        cd, "pad with 0", 
	NULL) != TCL_OK) return TCL_ERROR;

  if (step <= 0 || sstep <= 0) {
    ERROR("step must be > 0\n"); return TCL_ERROR;
  }
  if (to < from) {
    ERROR("from=%d must be <= to=%d\n",from,to);
    return TCL_ERROR;
  }
  dim = 1 + (to - from) / step;
  
  if (BOTH_SVECTOR( source1, source2)) {
    SVector* v1 = source1->data.svec;
    SVector* v2 = source2->data.svec;
    int j, i, start, end;
    int N, maxN = MIN( v1->n, v2->n);
    FMatrix* C = fmatrixCreate(1,dim); if (!C) return TCL_ERROR;

    for (j=0; j<dim; j++) {
      int k      = from + j * step;
      double sum = 0.0;
      start      = k<0 ? -k : 0;
      if (k<0) N = MIN( v1->n + k, v2->n    );
      else     N = MIN( v1->n    , v2->n - k);
      end        = start + N;

      if (N>0) {
	for (i=start; i<end; i += sstep)
	  sum += ((double)v1->vecA[i] * (double)v2->vecA[i+k]);
	if (pad) C->matPA[0][j] = sum / (1 + (maxN-1)/sstep);
	else     C->matPA[0][j] = sum / (1 + (N-1)/sstep);
      }
      else       C->matPA[0][j] = sum;
    }
    feFMatrix( FS,  destin, C, 1); return TCL_OK;
  }
  MUST_SVECTOR2( source1, source2);
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
   Add
   Adds two features
   ------------------------------------------------------------------------ */
static int fesAddItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin  = NULL;
  Feature     *source1 = NULL;
  Feature     *source2 = NULL;
  float       a = 1.0, b = 1.0;
  int         mode = 0;
    
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
	"<new_feature>",  ARGV_CUSTOM, createFe,&destin,  cd,
		          "<a> * <featureA> + <b> * <featureB>",
	"<a>",        ARGV_FLOAT,  NULL,    &a,        cd, "",
	"<featureA>", ARGV_CUSTOM, getFe,   &source1,     cd, SRC_FEAT1,
	"<b>",        ARGV_FLOAT,  NULL,    &b,        cd, "",
	"<featureB>", ARGV_CUSTOM, getFe,   &source2,     cd, SRC_FEAT2,
	"-mode",      ARGV_INT,    NULL,    &mode,     cd, 
 	"mode 0, 1 or -1 for dimension(result) =, max or min of input",
	NULL) != TCL_OK) return TCL_ERROR;

  if (FS->runon && !FS->ready) mode = -1;
  /* -------------------
     Add FLOAT features
     ------------------- */
  if (BOTH_FMATRIX( source1, source2)) {
     FMatrix *C = fmatrixCreate(0,0);
          
     if (C && fmatrixAdd( C, a, source1->data.fmat, b, source2->data.fmat)
	 &&  !feFMatrix( FS, destin, C, 1)) {
	COPY_CONFIG( destin, source1);
	return TCL_OK;
     }

     if (C) fmatrixFree(C);
     return TCL_ERROR;
  }
  /* ------------------
     Add SHORT features
     ------------------ */
  else if (BOTH_SVECTOR( source1, source2)) {
     SVector *C = svectorCreate(0);
     
     if (C && svectorAdd( C, a, source1->data.svec, b, source2->data.svec, mode)) {
	feSVector( FS,  destin, C, 1);
	COPY_CONFIG( destin, source1);
	return TCL_OK;
     }

     if (C) svectorFree(C);
     return TCL_ERROR;
  }
  MUST_SAME(source1,source2);
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
   Mul
   Multiply two features
   ------------------------------------------------------------------------ */
static int fesMulItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin  = NULL;
  Feature     *source1 = NULL;
  Feature     *source2 = NULL;
  float       a = 1.0;
  int         mode = 0;
  int         div  = 0;
    
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
	"<new_feature>",  ARGV_CUSTOM, createFe,&destin, cd,
		          "<a> * <featureA> * <featureB>",
	"<featureA>", ARGV_CUSTOM, getFe,   &source1, cd, SRC_FEAT1,
	"<featureB>", ARGV_CUSTOM, getFe,   &source2, cd, SRC_FEAT2,
	"-a",         ARGV_FLOAT,  NULL,    &a,       cd, "factor a",
	"-div",       ARGV_INT,    NULL,    &div,     cd, "division instead multiplication",
	"-mode",      ARGV_INT,    NULL,    &mode,    cd, 
 	"mode 0, 1 or -1 for dimesion(result) =, max or min of input",
	NULL) != TCL_OK) return TCL_ERROR;

  if (FS->runon && !FS->ready) mode = -1;
  /* -------------------
     Mul FLOAT features
     ------------------- */
  if (BOTH_FMATRIX( source1, source2)) {
     FMatrix *C = fmatrixCreate(0,0);
     
     if (C && fmatrixMulcoef( C, a, source1->data.fmat, source2->data.fmat, mode, div)) {
	feFMatrix( FS,  destin, C, 1);
	COPY_CONFIG( destin, source1);
	return TCL_OK;
     }

     if (C) fmatrixFree(C);
     return TCL_ERROR;
  }
  /* ------------------
     Mul SHORT features
     ------------------ */
  else if (BOTH_SVECTOR( source1, source2)) {
     SVector *C = svectorCreate(0);
     
     if (C && svectorMul( C, a, source1->data.svec, source2->data.svec, mode, div)) {
	feSVector( FS,  destin, C, 1);
	COPY_CONFIG( destin, source1);
	return TCL_OK;
     }

     if (C) svectorFree(C);
     return TCL_ERROR;
  }
  MUST_SAME( source1, source2);
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
   Matmul
   multiply matrix with feature
   ------------------------------------------------------------------------ */
static int fesMatmulItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin = NULL;
  Feature     *source = NULL;
  FMatrix     *matrix = NULL;
  int cut = -1;
    
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>",        ARGV_CUSTOM, createFe,  &destin, cd, NEW_FEAT,
      "<source_feature>", ARGV_CUSTOM, getFe,     &source, cd, SRC_FEAT,
      "<matrix>",         ARGV_CUSTOM, getFMatrix,&matrix, cd, "FMatrix",
      "-cut",             ARGV_INT,    NULL,      &cut,    cd,
      "take first n coefficients",
      NULL) != TCL_OK) return TCL_ERROR;

  /* -------------------
     Add FLOAT features
     ------------------- */
  if (IS_FMATRIX( source)) {
    FMatrix* C;
    
    if (cut <= 0) cut = matrix->m;
    if (cut > matrix->m) {
      WARN("MATMUL: -cut too big, setting to %d\n",matrix->m);
      cut = matrix->m;
    }
    
    C = fmatrixCreate(source->data.fmat->m,cut);
    fmatrixResize(matrix,cut,matrix->n); 
    if ( C && fmatrixMulot( C, source->data.fmat, matrix)
	   && !feFMatrix(FS,destin,C,1)) {
      COPY_CONFIG( destin, source);
      if ( matrix) fmatrixFree( matrix);
      return TCL_OK;
    }
    if ( matrix) fmatrixFree( matrix);
    if ( C)      fmatrixFree(C);
    return TCL_ERROR;
  }
  if ( matrix) fmatrixFree( matrix);
  MUST_FMATRIX( source);
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
   EFVR (Early Feature Vector Reduction)
   Reduce amount of feature vectors
   ------------------------------------------------------------------------ */
static int fesEFVRItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS       = (FeatureSet*)cd;
  Feature     *destin   = NULL;
  Feature     *source   = NULL;
  Feature     *FeWeight = NULL;
  Feature     *FeThres  = NULL;
  float threshold = 0.5;
  float boost     = 1.0;
  float decrease  = 1.0;
  float maxboost  = 5.0;
  int   shrink    = 1;

  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
	    "<feature>",        ARGV_CUSTOM, createFe, &destin,    cd, NEW_FEAT,
	    "<source_feature>", ARGV_CUSTOM, getFe,    &source,    cd, SRC_FEAT,
	    "<threshold>",      ARGV_FLOAT,  NULL,     &threshold, cd,
	    "threshold level for feature reduction",
	    "-weight",          ARGV_CUSTOM, createFe, &FeWeight,  cd, "weight feature to be written",
	    "-boost",           ARGV_FLOAT,  NULL,     &boost,     cd, "boost factor for the weights",
	    "-decrease",        ARGV_FLOAT,  NULL,     &decrease,  cd, "decrease of influence of each dimension of feature vector (0=all equal,1= 1/n)",
	    "-shrink",          ARGV_INT,    NULL,     &shrink,    cd, "!=0 means no shrinking of the feature vector, merged fv are duplicated",
	    "-maxboost",        ARGV_FLOAT,  NULL,     &maxboost,  cd, "maximum boost factor", 
	    "-thresweight",     ARGV_CUSTOM, getFe,    &FeThres,   cd, "vector of factors for dynamic threshold level",
		    NULL) != TCL_OK) return TCL_ERROR;

  if (IS_FMATRIX( source)) {
    FMatrix *C, *W, *T;
    float*   weight = NULL;
    float*   thres  = NULL;

    if (FeWeight) W=fmatrixCreate(1,source->data.fmat->m); else W = NULL;

    if (W) {
      weight = &W->matPA[0][0];
      INFO("Created weight vector with %dx%d dimensions.\n",source->data.fmat->m,source->data.fmat->n);
    } 

    if (FeThres) {
      T     = (FMatrix*)FeThres->data.fmat;
      thres = &T->matPA[0][0];
      INFO("Using dynamic threshold.\n");
    }

    C = fmatrixEFVR(source->data.fmat,weight,threshold,thres,boost,decrease,maxboost,shrink);

    if (W) feFMatrix(FS,FeWeight,W,1);

    if ( C && !feFMatrix(FS,destin,C,1)) {
      COPY_CONFIG( destin, source);
      return TCL_OK;
    }

    if (C) fmatrixFree(C);
    return TCL_ERROR;
  }

  MUST_FMATRIX( source);
  return TCL_ERROR;
}


/* ------------------------------------------------------------------------
   Filterbank
   multiply band matrix with feature
   ------------------------------------------------------------------------ */
static int fesFilterbankItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin = NULL;
  Feature     *source = NULL;
  FBMatrix    *bmatrix = NULL;
    
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>",        ARGV_CUSTOM, createFe,   &destin, cd, NEW_FEAT,
      "<source_feature>", ARGV_CUSTOM, getFe,      &source, cd, SRC_FEAT,
      "<bmatrix>",        ARGV_CUSTOM, getFBMatrix,&bmatrix, cd,
      "float band matrix",
      NULL) != TCL_OK) return TCL_ERROR;

  /* -------------------
     Add FLOAT features
     ------------------- */
  if (IS_FMATRIX( source)) {
     FMatrix *C = fmatrixCreate(source->data.fmat->m,bmatrix->m);

     if ( C && fmatrixBMulot( C, source->data.fmat, bmatrix)
	 && !feFMatrix(FS,destin,C,1)) {
	COPY_CONFIG( destin, source);
        if ( bmatrix) fbmatrixFree( bmatrix);
	return TCL_OK;
     }
     if ( bmatrix) fbmatrixFree( bmatrix);
     if ( C)       fmatrixFree(C);
     return TCL_ERROR;
  }
  if ( bmatrix) fbmatrixFree( bmatrix);
  MUST_FMATRIX( source);
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
   Methods 'log','exp','lin','alog'
   ------------------------------------------------------------------------ */
static int fesFunItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin = NULL;
  Feature     *source = NULL;
  float       m,a;
  int         runonok = 1;
  
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
     "<new_feature>",    ARGV_CUSTOM, createFe,&destin,   cd, NEW_FEAT,
     "<source_feature>", ARGV_CUSTOM, getFe,   &source,   cd, SRC_FEAT,
     "<m>",              ARGV_FLOAT,  NULL,    &m,        cd, "",
     "<a>",              ARGV_FLOAT,  NULL,    &a,        cd, "",
     NULL) != TCL_OK) return TCL_ERROR;

  /* -------------------
     FLOAT features
     ------------------- */
  if (IS_FMATRIX( source)) {
     FMatrix      *mat1 = source->data.fmat;
     int         frameA = 0;
     int frameX, frameN = mat1->m;
     int coeffX, coeffN = mat1->n;
     FMatrix      *mat  = fmatrixCreate(frameN,coeffN);
     if (!mat) return TCL_ERROR;
      
     Switch( argv[0])
	Case( "log")  {
	   double val;
	   int err=0;
	   foreach_coeff {
	      if ((val=mat1->matPA[frameX][coeffX] + a) <= 0) { val = 1.0; err++; }
	      mat->matPA[frameX][coeffX] = m * log10(val);
	   }
	   if (err) itfAppendResult(
	     "Warning: %d times the value for log() was <= 0, set result to 0.0\n",err);
	}
	Case( "alog")  {
	   double val;
	   float min, max, b;
	   int err=0;
	   runonok = 0;
	   fmatrixMinmax( mat1, &min, &max);
	   b = max / pow((double)10.0, (double)a);
	   foreach_coeff {
	      if ((val=mat1->matPA[frameX][coeffX] + b) <= 0) { val = 1.0; err++; }
	      mat->matPA[frameX][coeffX] = m * log10(val);
	   }
	   if (err) itfAppendResult(
	     "Warning: %d times the value for log() was <= 0, set result to 0.0\n",err);
	}
     	Case( "exp")  {
	   foreach_coeff
	      mat->matPA[frameX][coeffX] = m * exp((double)(mat1->matPA[frameX][coeffX] * a));
	}
        Case( "lin")  {
	   foreach_coeff
	      mat->matPA[frameX][coeffX] = m * mat1->matPA[frameX][coeffX] + a;
	}
        Case( "pow")  {
	   foreach_coeff
	      mat->matPA[frameX][coeffX] = m * pow((double)(mat1->matPA[frameX][coeffX]),(double)a);
	}
        Default {
	   ERROR("Don't know function %s.",argv[0]);
	   return TCL_ERROR;
	}
     End_switch;

     if (!feFMatrix( FS, destin, mat, runonok)) {
	COPY_CONFIG( destin, source); return TCL_OK;
     }
     fmatrixFree(mat); return TCL_ERROR;
  }
  else if (IS_SVECTOR( source)) {
     double         val;
     SVector      *vec1 = source->data.svec;
     int         frameA = 0;
     int frameX, frameN = vec1->n;
     SVector       *vec = svectorCreate(frameN);
     if (!vec) return TCL_ERROR;

     Switch( argv[0])
	Case( "log")  {
	   int err=0;
	   foreach_frame {
	      if ((val=vec1->vecA[frameX] + a) <= 0) { val = 1.0; err++; }
	      vec->vecA[frameX] = SLIMIT(m * log10(val));
	   }
	   if (err) itfAppendResult(
	     "Warning: %d times the value for log() was <= 0, set result to 0.0\n",err);
	}
	Case( "alog")  {
	   int err=0;
	   short min, max; float b;
	   runonok = 0;
	   svectorMinmax( vec1, &min, &max);
	   b = max / pow((double)10.0, (double)a);
	   foreach_frame {
	      if ((val=vec1->vecA[frameX] + b) <= 0) { val = 1.0; err++; }
	      vec->vecA[frameX] = SLIMIT(m * log10(val));
	   }
	   if (err) itfAppendResult(
	     "Warning: %d times the value for log() was <= 0, set result to 0.0\n",err);
	}
     	Case( "exp")  {
	   foreach_frame {
	      val = m * exp((double)(vec1->vecA[frameX] * a));
	      vec->vecA[frameX] = SLIMIT(val);
	   }
	}
        Case( "lin")  {
	   foreach_frame {
	      val = m * vec1->vecA[frameX] + a;
	      vec->vecA[frameX] = SLIMIT(val);
	   }
	}
        Case( "pow")  {
	   foreach_frame {
	      val = m * pow((double)vec1->vecA[frameX],(double)a);
	      vec->vecA[frameX] = SLIMIT(val);
	   }
	}
        Default {
	   ERROR("Don't know function %s.",argv[0]);
	   return TCL_ERROR;
	}
     End_switch;

     if (!feSVector( FS, destin, vec, runonok)) {
	COPY_CONFIG( destin, source); return TCL_OK;
     }
     svectorFree(vec); return TCL_ERROR;
  }
  DONT_KNOW( source);  return TCL_ERROR;
}


/* ------------------------------------------------------------------------
   Method 'maxarg'
   ------------------------------------------------------------------------ */
static int fesMaxargItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin = NULL;
  Feature     *source = NULL;
  int         abs = 1;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>",        ARGV_CUSTOM, createFe,&destin, cd, NEW_FEAT,
      "<source_feature>", ARGV_CUSTOM, getFe,   &source, cd, SRC_FEAT,
      "-abs",             ARGV_INT,    NULL,    &abs,    cd,
      "1 for absolute value or 0 for signed values",
      NULL) != TCL_OK) return TCL_ERROR;

  if (! IS_FMATRIX(source))  {
    MUST_FMATRIX(source);
    free(source); return TCL_ERROR;
  }
  else {
     FMatrix      *mat1 = source->data.fmat;
     int         frameA = 0;
     int frameX, frameN = mat1->m;
     int coeffX, coeffN = mat1->n;
     FMatrix      *mat  = fmatrixFeature(frameN,1,FS,destin,&frameA);
     if (!mat) return TCL_ERROR;

     if (abs) {
	foreach_frame {
	   int    maxarg = 0;
	   double val;
	   double max    = coeffN ? ABS(mat1->matPA[frameX][0]) : 0.0;
	   for ( coeffX=1; coeffX<coeffN; coeffX++)
	      if ((val = ABS(mat1->matPA[frameX][coeffX])) > max) {
		 maxarg = coeffX;
		 max    = val;
	      }
	   mat->matPA[frameX][0] = maxarg;
	}
     }
     else {
	foreach_frame {
	   int    maxarg = 0;
	   double max    = coeffN ? mat1->matPA[frameX][0] : 0.0;
	   for ( coeffX=1; coeffX<coeffN; coeffX++)
	      if (mat1->matPA[frameX][coeffX] > max) {
		 maxarg = coeffX;
		 max    = mat1->matPA[frameX][coeffX];
	      }
	   mat->matPA[frameX][0] = maxarg;
	}
     }
     if(!feFMatrix( FS, destin, mat, 1)) {
	COPY_CONFIG( destin, source); return TCL_OK;
     }
     fmatrixFree(mat); return TCL_ERROR;
  }	 
}



/* ------------------------------------------------------------------------
   Method 'meanarg'
   ------------------------------------------------------------------------ */
static int fesMeanargItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin = NULL;
  Feature     *source = NULL;

  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>",        ARGV_CUSTOM, createFe,&destin, cd, NEW_FEAT,
      "<source_feature>", ARGV_CUSTOM, getFe,   &source, cd, SRC_FEAT,
      NULL) != TCL_OK) return TCL_ERROR;

  if (! IS_FMATRIX(source))  {
    MUST_FMATRIX(source);
    free(source); return TCL_ERROR;
  }
  else {
     FMatrix      *mat1 = source->data.fmat;
     int         frameA = 0;
     int frameX, frameN = mat1->m;
     int coeffX, coeffN = mat1->n;
     FMatrix       *mat = fmatrixFeature(frameN,1,FS,destin,&frameA);
     if (!mat) return TCL_ERROR;

     foreach_frame {
	double val;
	double sum  = 0.0;
	double wsum = 0.0;
	for ( coeffX=0; coeffX<coeffN; coeffX++) {
	   val  = ABS(mat1->matPA[frameX][coeffX]);
	   sum  += val;
	   wsum += (coeffX * val);
	}
	mat->matPA[frameX][0] = (sum>0.0) ? (wsum/sum) : (coeffN-1)/2.0;
     }
     if(!feFMatrix( FS, destin, mat, 1)) {
	COPY_CONFIG( destin, source); return TCL_OK;
     }
     fmatrixFree(mat); return TCL_ERROR;
  }	 
}



/* ------------------------------------------------------------------------
   xtalk filter:
                                                 +
   signal A ------------------------------------->O-------*------> e = Aout
                                                  ^ -     |
   correlated interference  +-----------+         |       |
   B -------------*-------->| Filter  w |---------+       |
                  |         | - - - - - |                 |
		  +-------->| Adaption  |<----------------+
		            +-----------+

  Parameters of the filter itself are:
    L             size of the input window or number of filter coefficients
    shift         shift in samples of the input relative to output
    u             filter convergence factor
  Since the filter should only remove xtalk produced by the speaker on
  channel B it mustn't be adapted if the original speaker talks and his xtalk
  might be found on the other side. Therefor the adaptation time can be
  controlled with another SVector feature 'featA' or with the parameters:
    alpha         power estimate factor
    thr1          powerB > thr1 * powerA => adaptation starts
    thr2          powerB < thr2 * powerA => adaptation ends
    thrP          powerB <= thrP         => don't do adaptation
  In both cases the adaptation can be delayed with a counter ac or better
  the adaptation condition has to be true for 'ac' times before it really
  starts.

  If the filter should only work as echo cancellation give channel A as
  'featA'.
  ------------------------------------------------------------------------ */
#define XCOEF double
static int fesXTalkItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS  = (FeatureSet*)cd;
  int     dst = 0, src1 = 0, src2 = 0;
  Feature *destin;        char *hdestin  = "name of filtered channel A";
  Feature *source1;       char *hsource1 = "channel with xtalk";
  Feature *source2;       char *hsource2 = "channel causing xtalk";
  char    *cL     ="100"; char *hL       = "number of filter weights";
  char    *cshift = "50"; char *hshift   = "shift of the input samples";
  char    *cpshift=  "0"; char *hpshift  = "shift of the power window";
  int L, shift, pshift;
  int dshift = 0;
  float u     = 0.1; char *hu     = "filter convergence factor";
  float sf    = 1;   char *hsf    = "adaptiv shift factor";
  float alpha = 0.02;char *halpha = "power estimate factor";
  float thr1  = 2.0; char *hthr1  = "power ratio activating the adaptation";
  float thr2  = 1.5; char *hthr2  = "power ratio deactivating the adaptation";
  float xpow1 =   0; char *hxpow1 =
                             "xtalk power threshold activating the adaptation";
  float xpow2 =   0; char *hxpow2 =
                           "xtalk power threshold deactivating the adaptation";
  float forget=   0; char *hforget=
                         "forget weights with (1.0 - forget) when not adapted";
  float min   =   0; char *hmin=
                         "take minimum(original,filter) as output, boolean";
  int   acN   =   0; char *hacN   = "adaption counter";
  int     ifeatA = -1, iinfA = -1, iinfF = -1;
  Feature *featA = NULL; char *hfeatA ="feature telling when to do adaptation";
  Feature *infA  = NULL; char *hinfA  ="feature showing when was adapted";
  Feature *infF  = NULL; char *hinfF  ="feature showing filter coefficients";
  
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
	"<new_feature>",ARGV_CUSTOM, createFeI,&dst,    cd, hdestin,
	"<channelA>",   ARGV_CUSTOM, getFeI,   &src1,   cd, hsource1,
	"<channelB>",   ARGV_CUSTOM, getFeI,   &src2,   cd, hsource2,
	"-L",           ARGV_STRING, NULL,     &cL,     cd, hL,
	"-shift",       ARGV_STRING, NULL,     &cshift, cd, hshift,
	"-u",           ARGV_FLOAT,  NULL,     &u,      cd, hu,
	"-sf",          ARGV_FLOAT,  NULL,     &sf,     cd, hsf,
	"-alpha",       ARGV_FLOAT,  NULL,     &alpha,  cd, halpha,
	"-thr1",        ARGV_FLOAT,  NULL,     &thr1,   cd, hthr1,
	"-thr2",        ARGV_FLOAT,  NULL,     &thr2,   cd, hthr2,
	"-xpow1",       ARGV_FLOAT,  NULL,     &xpow1,  cd, hxpow1,
	"-xpow2",       ARGV_FLOAT,  NULL,     &xpow2,  cd, hxpow2,
	"-pshift",      ARGV_STRING, NULL,     &cpshift,cd, hpshift,	   
	"-forget",      ARGV_FLOAT,  NULL,     &forget, cd, hforget,
	"-min",         ARGV_INT,    NULL,     &min,    cd, hmin,
	"-ac",          ARGV_INT,    NULL,     &acN,    cd, hacN,
	"-adap",        ARGV_CUSTOM, getFeI,   &ifeatA, cd, hfeatA,
	"-infA",        ARGV_CUSTOM, createFeI,&iinfA,  cd, hinfA,
	"-infF",        ARGV_CUSTOM, createFeI,&iinfF,  cd, hinfF,
	NULL) != TCL_OK) return TCL_ERROR;
  destin  = FS->featA + dst; 
  source1 = FS->featA + src1; 
  source2 = FS->featA + src2;
  if (ifeatA > 0) featA = FS->featA + ifeatA;
  if (iinfA  > 0) infA  = FS->featA + iinfA;
  if (iinfF  > 0) infF  = FS->featA + iinfF;

  if (getSampleX(FS,source1,cL,&L) || getSampleX(FS,source1,cshift,&shift) ||
      getSampleX(FS,source1,cpshift,&pshift))
     return TCL_ERROR;
  if (L <= 0) {
     ERROR("%d filter coefficients is not ok!\n", L);
     return TCL_ERROR;
  }
  if (featA && !IS_SVECTOR(featA)) {
    ERROR("feature given with '-adap' must be of type SVector!\n");
    return TCL_ERROR;
  }
  
  /* -----------------------------------------------------------------------
     remove xtalk
                                k -->                                sampleN
				|                                        |
     Ain:.......................y........................................|
                                |                                        |
                                 --- + shift --> j                       |
				                 |                       |
     Bin:........................................x.......................|
     Filter:                      |<----- L ---->|
     
                                                 |<= xP     |<= bP
     buffer:              |<------ L + bN -------|--------->|
                                                          bmax
     ----------------------------------------------------------------------- */
  if (BOTH_SVECTOR( source1, source2)) {
     int bN = 1000;                                           /* buffer size */
     int i;
     SVector *Ain    = source1->data.svec;             /* in and out signals */
     SVector *Bin    = source2->data.svec;
     int     k;
     int     sampleN = Ain->n;          /* int sampleN = MIN(Ain->n,Bin->n); */
     SVector *Aout   = svectorCreate(sampleN);
     XCOEF   sums = 0.0;   

     SVector *infAv  = NULL;                                  /* filter data */
     int     winN    = source1->samplingRate * FS->sfshift;
     int     frameN  = (winN > 0) ? ceil(sampleN / (double)winN) : 0;
     FMatrix *infFm  = NULL;
     
     SVector *adapP  = featA ? featA->data.svec : NULL;
     int    adapt    = FALSE;               /* filter adaption active or not */
     int    acX      = 0;                   /* counter for adaptation states */
     
     XCOEF  pA, pB, pAold = 0.0, pBold = 0.0;              /* power estimate */
     XCOEF  aa = alpha, ab = 1.0 - aa;

     XCOEF *wP = (XCOEF*)malloc((unsigned)(L*sizeof(XCOEF)));/*filter weight */
     XCOEF *bP = (XCOEF*)malloc((unsigned)((L + bN)*sizeof(XCOEF)));/* input */
     int   bmax  = 0;
     int   error = 0;
     int   reset = 0;
     
     if ( !(Aout && wP && bP) ) error = 1;
     if ( adapP && adapP->n < sampleN ) {
       ERROR("Number of samples in '-adap' feature is to small\n");
       error = 1;
     }
     if (error) {
       if (Aout) svectorFree(Aout);
       if (wP) free(wP); if (bP) free(bP);
       return TCL_ERROR;
     }
     
     if (infA) 	infAv = svectorCreate(sampleN);
     if (infF) 	infFm = fmatrixCreate(frameN,L);
     for (i=0; i<L; i++) wP[i] = 0.0;

     /* ====================================
	 Main loop over all samples.
	------------------------------------ */
     for (k=0; k<sampleN; k++) {
	int i, j = k+shift;
	XCOEF wx = 0.0, y, yout;
	XCOEF *xP, *xn, *xi, *wi = wP; 

	/* ----------------------- */
	/* get signals             */
	/* ----------------------- */
	if (k  &&  j <= bmax  &&  j > (bmax-bN)) {       /* input in buffer? */
	   xP = bP + bmax - j;
	   if (dshift) sums += (xP[0]*xP[0] - xP[L]*xP[L]);
	   else {
	      sums = 0.0;
	      for (i=0; i<L; i++) sums += (xP[i]*xP[i]);
	   }
	}
	else {            /* put the L current  + bN future values in buffer */
	   bmax = j + bN;
	   for (i= -bN; i<L ; i++)
	      bP[i+bN] = (i<=j && (j-i)<sampleN) ? (XCOEF)Bin->vecA[j-i] : 0.0;
	   xP = bP + bN; sums = 0.0;
	   for (i=0; i<L; i++) sums += (xP[i]*xP[i]);
	}
	y = Ain->vecA[k];
	//x = xP[0];

	/* ----------------------- */
	/* filter                  */
	/* ----------------------- */
	xi = xP;  xn = xP + L-3;
	while (xi<xn) {
	   wx += wi[0]*xi[0] + wi[1]*xi[1] + wi[2]*xi[2] + wi[3]*xi[3];
	   xi += 4; wi += 4;
	}
	xn += 3;
	while (xi<xn)  wx += *(wi++) * *(xi++);
	yout = y - wx;
	if (min) Aout->vecA[k] = yout*yout > y*y ? y : SLIMIT(yout);
	else     Aout->vecA[k] = SLIMIT(yout);

	/* ------------------------------------------------------- */
	/* Adapt filter or not                                     */
	/* Always adapt if adaptation feature is > 0 or is         */
	/* channel A. Otherwise compare power values on both       */
	/* input channels with each other and thresholds.          */     
	/* ------------------------------------------------------- */
	if (adapP) {
	  adapt = (adapP == Ain) ? 1 : (adapP->vecA[k] > 0);
	}
	else {
	  int sampleX  = j + pshift;
	  int sampleY  = k + pshift;
	  XCOEF xshift = sampleX >= 0 && sampleX < sampleN ?
	                 Bin->vecA[sampleX] : 0.0;
	  XCOEF yshift = sampleY >= 0 && sampleY < sampleN ?
	                 Ain->vecA[sampleY] : 0.0;
	  /* --- Update power values --- */
	  pA = aa*yshift*yshift + ab*pAold;    pAold = pA;
	  pB = aa*xshift*xshift + ab*pBold;    pBold = pB;

	  /* --- test xtalk condition --- */ 
	  if (adapt) {
	    if (pB < (thr2*pA) || pA < xpow2) { 
	      adapt = FALSE;
	      acX   = 0;
	    }
	    else acX++;
	  }
	  else if (pB > (thr1*pA) &&  pA > xpow1)
	    adapt = TRUE;
	}
	
	/* ---------------------- */
	/* store user information */
	/* ---------------------- */
	if (infFm && !(k%winN))
	   for (i=0;i<L;i++) infFm->matPA[k/winN][i] = wP[i];
	if (infAv) infAv->vecA[k] = (adapt && acX>=acN)?10000:-acX;

	/* --------------------- */
	/* Adapt filter          */
	/* --------------------- */
	if (adapt && (acX >= acN) && (sums>0.0)) {
	   double fac = 2.0*u*yout/sums;
/*	   double fac = 2.0*u*yout/(L*pB);*/
	   for (i=0;i<L;i++) {
	     wP[i] = wP[i] + fac * xP[i];
	     if (wP[i] > 1.0e6 || wP[i] < -1.0e6) {
	       int i;
	       WARN("xtalk filter getting unstable at sample %d\n",k);
	       WARN("resetting filter coefficients.\n");
	       for (i=0;i<L;i++) wP[i] = 0.0;
	       adapt = FALSE;
	       reset = 1; break;
	     }
	   }
	   if (reset) {reset = 0; continue;}

	   /* ----------------------- */
	   /* Adaptive shift          */
	   /* ----------------------- */
	   if (sf > 1.0) {
	      double val, sp = 0.0, weight = 0.0;
	      int i;
	      /* calculate 'mean index' */
	      for (i=0; i<L; i++) {
		 val = ABS(wP[i]); /* or wP[i]*wP[i] ? */
		 sp     += i*val;
		 weight += val;
	      }
	      sp = (weight>0) ? sp/weight : (L-1)/2;
	      dshift = (int)(2.0 * sf * (sp/(L-1.0) - 0.5));
/* if (dshift)
   fprintf(stderr,"sp= %f, delta shift %d, new shift %d at sample k= %d\n",
		    sp, dshift, shift, k);                                   */

	      if (dshift > 0) {
		 for (i=0; i<(L-dshift); i++) wP[i] = wP[i+dshift];
		 for (i=(L-dshift); i<L; i++) wP[i] = 0.0;
	      }
	      else if (dshift < 0) {
		 for (i=0; i< -dshift; i++) wP[i] = 0.0;
		 for (i= -dshift; i<L; i++) wP[i] = wP[i+dshift];
	      }
	      shift -= dshift;
	   }

	}
	else if (!adapt && forget > 0) {
	   for (i=0;i<L;i++) wP[i] = (1-forget) * wP[i];
	}
     }
     free(bP); free(wP);
     /* ------------------------------------
	 End of loop over all samples.
	==================================== */

     if (infAv) {
	feSVector( FS, infA, infAv, 0);
	COPY_CONFIG( infA, source1);
     }
     if (infFm) {
	feFMatrix( FS, infF, infFm, 0);
	infF->samplingRate = source1->samplingRate;
	infF->shift        = FS->sfshift;
     }
     if (!feSVector( FS, destin, Aout, 0)) {
	COPY_CONFIG( destin, source1); return TCL_OK;
     }
     svectorFree(Aout); return TCL_ERROR;
  }
  MUST_SVECTOR2( source1, source2);
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
   Method 'filter'
   ------------------------------------------------------------------------ */
static int fesFilterItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin = NULL;
  Feature     *source = NULL;
  Filter      *filter = NULL;
  int         pad = 0;
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<feature>",        ARGV_CUSTOM, createFe,  &destin, cd, NEW_FEAT,
    "<source_feature>", ARGV_CUSTOM, getFe,     &source, cd, SRC_FEAT,
    "<filter>",         ARGV_CUSTOM, getFilter, &filter, cd,
		        "@filename, name or definition of a filter",
    "-pad",             ARGV_INT,    NULL,      &pad,    cd,
		        "=0 pad with 0.0, !=0 pad with first & last value",
    NULL) != TCL_OK) return TCL_ERROR;


  /* -------------------
     FLOAT features
     ------------------- */
  if (IS_FMATRIX( source)) {
     FMatrix      *mat1 = source->data.fmat;
     int      i, frameY;
     int         frameA = 0;
     int frameX, frameN = mat1->m;
     int coeffX, coeffN = mat1->n;
     FMatrix       *mat = fmatrixCreate(frameN,coeffN);
     
     for (coeffX=0; coeffX<coeffN; coeffX++) {
	foreach_frame {
	   double sum = 0.0;
	   for (i=0; i< filter->n; i++) {
	      frameY = frameX - i - filter->offset;
	      if (pad)	sum +=
		 (frameY < 0)       ?
		    filter->a[i] * mat1->matPA[0][coeffX] :
		 (frameY >= frameN) ?
		    filter->a[i] * mat1->matPA[frameN-1][coeffX] :
		    filter->a[i] * mat1->matPA[frameY][coeffX];
	      else sum += (frameY < 0)       ? 0.0 :
	                  (frameY >= frameN) ? 0.0 :
		          filter->a[i] * mat1->matPA[frameY][coeffX];
	   }
	   for (i=0; i< filter->m; i++)
	      if ((frameY = frameX - i - 1) >= 0)
		 sum -= filter->b[i] * mat->matPA[frameY][coeffX];
	   mat->matPA[frameX][coeffX] = sum;
	}
     }
     filterFree(filter);

     if(!feFMatrix( FS, destin, mat, 0)) {
	COPY_CONFIG( destin, source); return TCL_OK;
     }
     fmatrixFree(mat); return TCL_ERROR;
  }
  
  else if (IS_SVECTOR( source)) {
     SVector      *vec1 = source->data.svec;
     int i, frameY, frameA = 0;
     int frameX, frameN = vec1->n;
     SVector      *vec  = svectorCreate(frameN);  if (!vec) return TCL_ERROR;
     
     foreach_frame {
	double sum = 0.0;
	for (i=0; i< filter->n; i++) {
	   frameY = frameX - i - filter->offset;
	   if (pad) sum +=
	      (frameY < 0)       ? filter->a[i] * vec1->vecA[0] :
	      (frameY >= frameN) ? filter->a[i] * vec1->vecA[frameN-1] :
		                   filter->a[i] * vec1->vecA[frameY];
	   else sum += (frameY < 0)       ? 0.0 :
	               (frameY >= frameN) ? 0.0 :
		       filter->a[i] * vec1->vecA[frameY];
	}
	for (i=0; i< filter->m; i++)
	   if ((frameY = frameX - i - 1) >= 0)
	      sum -= filter->b[i] * vec->vecA[frameY];
	vec->vecA[frameX] = SLIMIT(ROUND(sum));
     }
     filterFree(filter);
     if (!feSVector( FS, destin, vec, 0)) {
	COPY_CONFIG( destin, source); return TCL_OK;
     }
     svectorFree(vec); return TCL_ERROR;
  }
  DONT_KNOW(source); return TCL_ERROR;
}


/* ------------------------------------------------------------------------
   Method 'normframe'
   ------------------------------------------------------------------------ */
static int fesNormFrameItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  int         dst = 0, src = 0, nrm = -1;
  Feature     *destin, *source;
  Feature     *norm = NULL;
  int         p = 2;
  int         add = 0;
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<feature>",        ARGV_CUSTOM, createFeI,  &dst, cd, NEW_FEAT,
    "<source_feature>", ARGV_CUSTOM, getFeI,     &src, cd, SRC_FEAT,
    "-L",               ARGV_INT,    NULL,       &p,   cd,
		        "Lp norm = (SUM |x[i]|^p)^1/p",
    "-n",               ARGV_CUSTOM, createFeI,  &nrm, cd,
		        "feature to hold norm",
    "-add",             ARGV_INT,    NULL,       &add, cd,
		       "1: take norm as additional coefficient to new feature",
    NULL) != TCL_OK) return TCL_ERROR;
  destin = FS->featA + dst; 
  source = FS->featA + src; 
  if (nrm >= 0) norm = FS->featA + nrm;
  
  if (add != 0 ) add = 1;
  if (p < 1 || p > 2)  {
     ERROR("Please use only '1' or '2' for flag -L\n");
     return TCL_ERROR;
  }
  /* -------------------
     FLOAT features
     ------------------- */
  if (IS_FMATRIX( source)) {
     FMatrix      *mat1 = source->data.fmat;
     int         frameA = 0;
     int frameX, frameN = mat1->m;
     int coeffX, coeffN = mat1->n;
     FMatrix     *mat   = fmatrixCreate(frameN,coeffN + add);
     FMatrix     *mat2  = norm == NULL ? NULL : fmatrixCreate(frameN,1);
     double sum;
     if (!mat || (norm && !mat2)) {
	if (mat) fmatrixFree(mat); if (mat2) fmatrixFree(mat2);  
	ERROR("allocation problem\n"); return TCL_ERROR;
     }
     
     if (p==1) {
	foreach_frame {
	   for (sum=0, coeffX=0; coeffX<coeffN; coeffX++)
	      sum += ABS(mat1->matPA[frameX][coeffX]);
	   if (sum > 0) for (coeffX=0; coeffX<coeffN; coeffX++)
	      mat->matPA[frameX][coeffX] = mat1->matPA[frameX][coeffX]/sum;
	   if (add)  mat->matPA[frameX][coeffN] = sum;
	   if (norm) mat2->matPA[frameX][0] = sum;
	}
     }
     else { /* p==2 */
	foreach_frame {
	   for (sum=0, coeffX=0; coeffX<coeffN; coeffX++)
	      sum += SQUARE(mat1->matPA[frameX][coeffX]);
	   if (sum > 0) {
	      sum = sqrt(sum);
	      for (coeffX=0; coeffX<coeffN; coeffX++)
		 mat->matPA[frameX][coeffX] = mat1->matPA[frameX][coeffX]/sum;
	   }
	   if (add)  mat->matPA[frameX][coeffN] = sum;
	   if (norm) mat2->matPA[frameX][0] = sum;
	}
     }

     if(!feFMatrix( FS, destin, mat, 1)) {
	COPY_CONFIG( destin, source);
	if (norm && feFMatrix( FS, norm, mat2, 1)) {
	   fmatrixFree(mat2);  return TCL_ERROR;
	}
	return TCL_OK;
     }
     fmatrixFree(mat);  if (mat2) fmatrixFree(mat2);
     return TCL_ERROR;
  }
  MUST_FMATRIX(source);
  return TCL_ERROR;
}

/* ========================================================================
   Method 'Vocal Tract Length Normalization(VTLN)'

   linear warping:
   --------------
   Ratio = VTL(speaker) / VTL(all_speakers)
   ==>  norm_frequency = frequency(speaker) / Ratio

   If 'Ratio' is bigger than 1.0 then there is no good
   estimate for part of the norm_frequency.
   We can define an edge for a piecewise linear transformation
   so that the whole range is transformed when 'Ratio' is bigger
   than 'edge':
        norm_frequency = frequency(speaker) / Ratio       frequency < edge
	norm_frequency = b' * frequency(speaker) + c'     frequency >= edge
	     so that  edge / Ratio = b' * edge + c'
             and            rate/2 = b' * rate/2 + c'

   The 'edge' we use below is normalized (divided by rate/2) so its
   range is 0 .. 1.0.
   ======================================================================== */
static int fesVTLnItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin = NULL, *source = NULL;
  float       MinWarp = 0.1, MaxWarp = 10.0;
  char        *mod = "lin";
  float       Ratio = 1.0;
  float       edge  = 1.0;

  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<feature>", ARGV_CUSTOM, createFe,   &destin,  cd, NEW_FEAT,
    "<source>",  ARGV_CUSTOM, getFe,      &source,  cd, SRC_FEAT,
    "<ratio>",   ARGV_FLOAT,  NULL,       &Ratio,   cd, "warping factor",
    "-min",      ARGV_FLOAT,  NULL,       &MinWarp, cd, "max warping factor",
    "-max",      ARGV_FLOAT,  NULL,       &MaxWarp, cd, "min warping factor",
    "-edge",     ARGV_FLOAT,  NULL,       &edge,    cd, "edge point for piecewise warping",
    "-mod",      ARGV_STRING, NULL,       &mod,     cd,
		 "warping modus: lin, nonlin",
    NULL) != TCL_OK) return TCL_ERROR;

  if (MinWarp <= 0.0) {
    ERROR("MinWarp must be > 0 but is %f\n",MinWarp);
    return TCL_ERROR;
  }
  Ratio = Ratio < MinWarp ? MinWarp : Ratio > MaxWarp ? MaxWarp : Ratio;
  if (edge > 1.0) edge = 1.0;
  if (edge < 0.0) edge = 0.0;
  if (FE_VER) INFO("VTLN Mode: %s, warp factor %f, (edge %f)\n",mod,Ratio,edge);

  /* -------------------
     FLOAT features
     ------------------- */
  if (IS_FMATRIX( source)) {
     FMatrix      *mat1 = source->data.fmat;
     int         frameA = 0;
     int frameX, frameN = mat1->m;
     int coeffX, coeffN = mat1->n;

     FMatrix     *mat = fmatrixCreate(frameN,coeffN);  if (!mat) return TCL_ERROR;

     /* ------------------------------------------- */
     /* Piecewise linear: Y = X/Ratio  for X < edge */
     /* Includes linear warping                     */
     /* ------------------------------------------- */
     if (streqn(mod,"lin",3)) {
       double yedge  = edge < Ratio ? edge / Ratio                 : 1.0;
       double b      = yedge < 1.0  ? (1.0 - edge) / (1.0 - yedge) : 0;

       for ( frameX=frameA; frameX<frameN; frameX++) {
          for (coeffX=0; coeffX<coeffN; coeffX++) {

            double Y0 = (double)(coeffX)   / (double)coeffN;
            double Y1 = (double)(coeffX+1) / (double)coeffN;

	    double X0 = ((Y0 < yedge) ? (Ratio * Y0) :
                                        (b     * Y0 +  1.0 - b)) * coeffN;
	    double X1 = ((Y1 < yedge) ? (Ratio * Y1) :
                                        (b     * Y1 +  1.0 - b)) * coeffN;

	    int    Lower_coeffY1 = (int)(X1);
            double alpha1        = X1 - Lower_coeffY1;

	    int    Lower_coeffY0 = (int)(X0); 
            double alpha0        = (int)(X0) + 1 - X0;

            double z             =  0.0;
            int    i;
         
            if ( Lower_coeffY0 == Lower_coeffY1) {
              z += (X1-X0) * mat1->matPA[frameX][Lower_coeffY0];
            } else {
              z += alpha0 * mat1->matPA[frameX][Lower_coeffY0];

              for ( i = Lower_coeffY0+1; i < Lower_coeffY1; i++) {
                z += mat1->matPA[frameX][i];
              }
              if ( Lower_coeffY1 < coeffN) {
                z += alpha1 * mat1->matPA[frameX][Lower_coeffY1];
              }
            }
            mat->matPA[frameX][coeffX] = z;
          }
	}
     }

     /* ------------------------------------------ */
     /*            Nonlinear Warping               */
     /* ------------------------------------------ */
     else if (streqn(mod,"non",3)) {
       double factor = 3.0/(2.0*(float)coeffN);     /* BBN's nonlinear factor */

       foreach_coeff {
	 float X = (float)pow((double)Ratio,(factor*coeffX))*coeffX;
	 int   Upper_coeffY = ceil((double)X);
	 int   Lower_coeffY = floor((double)X);
	 float alpha = X - Lower_coeffY;
         float beta = 1-alpha;

	 if (Upper_coeffY <coeffN) {
	   mat->matPA[frameX][coeffX]  
	     = alpha*mat1->matPA[frameX][Upper_coeffY]
	       + beta*mat1->matPA[frameX][Lower_coeffY];
	 }
	 else{
	   mat->matPA[frameX][coeffX] = mat1->matPA[frameX][coeffN-1];
	 }
       }
     }
     /* ----------------------- */
     /* No method was specified */
     /* ----------------------- */
     else {
       ERROR ("VTLN: Don't know warping method\n"
	      "Use method with flag -help to get more information\n");
       return TCL_ERROR;
     }

     if(feFMatrix( FS, destin, mat, 1)) {
	fmatrixFree(mat); return TCL_ERROR;
     }
     destin->samplingRate = source->samplingRate;
     destin->shift = source->shift;
     return TCL_OK;
  }

  MUST_FMATRIX(source);
  return TCL_ERROR;

}

/* ------------------------------------------------------------------------
   Method 'mean'
   ------------------------------------------------------------------------ */
static int fesMeanItf( ClientData cd, int argc, char *argv[])
{
  Feature     *weight = NULL, *source = NULL;
  FMatrix     *meanO  = NULL;
  FMatrix     *devO   = NULL;
  FMatrix     *smeanO = NULL;
  FVector     *countO = NULL;
  int         update  = 0;

  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<matrix>",  ARGV_CUSTOM, getFMatrixP,&meanO,  cd, "mean vector(s) of type FMatrix",
    "<source>",  ARGV_CUSTOM, getFe,      &source, cd, SRC_FEAT,
    "-weight",   ARGV_CUSTOM, getFe,      &weight, cd, "weight frames when calculate mean vector",
    "-dev",      ARGV_CUSTOM, getFMatrixP,&devO,   cd, "deviation vector(s) of type FMatrix",
    "-smean",    ARGV_CUSTOM, getFMatrixP,&smeanO, cd, "mean of squares vector(s) of type FMatrix",
    "-count",    ARGV_CUSTOM, getFVectorP,&countO, cd, "counts",
    "-update",   ARGV_INT,    NULL,       &update, cd, "update mean and smean with using counts",

  NULL) != TCL_OK) return TCL_ERROR;

  /* -------------------
     FLOAT features
     ------------------- */
  if (IS_FMATRIX( source)) {
     FMatrix *mat   = source->data.fmat;
     FMatrix *w     = NULL;
     int i,  frameN = mat->m;
     int j,  coeffN = mat->n;
     int c,  classN = 1;
     FMatrix *mean, *dev, *smean;
     FVector *count;
     
     if (weight) {
	if (!IS_FMATRIX(weight)) { MUST_FMATRIX(source); return TCL_ERROR; }
	w = weight->data.fmat;
	if (frameN != w->m) {
	   ERROR("Frames of source feature %d and weight feature %d differ\n",
		 frameN, w->m);
	   return TCL_ERROR;
	}
	classN = w->n;
     }
     mean  = fmatrixCreate(classN,coeffN);
     dev   = fmatrixCreate(classN,coeffN);
     smean = fmatrixCreate(classN,coeffN);
     count = fvectorCreate(classN);
     for (c=0; c<classN; c++) {
	double m,sm,f,cc=0.0;
	for (i=0; i<frameN; i++) {
	   f = w ? w->matPA[i][c] : 1.0;
	   if (f!=0.0) {
	      cc += f;
	      for (j=0; j<coeffN; j++) {
		 m               = mat->matPA[i][j];
		 mean->matPA[c][j]  += (f * m);
		 smean->matPA[c][j] += (f * m * m);
	      }
	   }
	}
	if (cc!=0)  for (j=0; j<coeffN; j++) {
	   m  = mean->matPA[c][j] / cc;
	   sm = smean->matPA[c][j] / cc;
	   mean->matPA[c][j]  = m;
	   smean->matPA[c][j] = sm;          
	   dev->matPA[c][j]   = sqrt(sm - m*m);
	   count->vecA[c] = cc;
	}
     }

     if (update) {
	if (!countO) {
	   ERROR("Need counts to update mean!\n");
	   return TCL_ERROR;
	}
	if (countO->n != classN) {
	   WARN("Counts with wrong dimensions %d instead %d,\n"
		"        will resize and set coefficients to zero!\n",
		 countO->n, classN);
	   fvectorResize(countO,classN);
	   for( j=0; j<classN; j++) countO->vecA[j] = 0.0;
	}
	if (meanO->m != classN || meanO->n != coeffN) {
	   WARN("Mean with wrong dimensions %dx%d instead %dx%d,\n"
		"        will resize and set coefficients to zero!\n",
		 meanO->m, meanO->n, classN, coeffN);
	   fmatrixResize(meanO,classN,coeffN);
	   fmatrixClear(meanO);
	}
	if (smeanO && (smeanO->m != classN || smeanO->n != coeffN)) {
	   WARN("SMean with wrong dimensions %dx%d instead %dx%d,\n"
		"        will resize and set coefficients to zero!\n",
		 smeanO->m, smeanO->n, classN, coeffN);
	   fmatrixResize(smeanO,classN,coeffN);
	   fmatrixClear(smeanO);
	}
	for (c=0; c<classN; c++) {
	   float sum = countO->vecA[c] + count->vecA[c];
	   float a   = sum!=0.0 ?  countO->vecA[c] / sum : 0.0;
	   float b   = sum!=0.0 ?  count->vecA[c] / sum : 0.0;
	   countO->vecA[c] = sum;
	   if (sum!=0.0) for (j=0; j<coeffN; j++)
	      meanO->matPA[c][j] = a * meanO->matPA[c][j]
		                 + b * mean->matPA[c][j];
	   if (smeanO && sum!=0.0) for (j=0; j<coeffN; j++)
	      smeanO->matPA[c][j] = a * smeanO->matPA[c][j]
		                  + b * smean->matPA[c][j];
	}
     }
     else {
	fmatrixCopy(meanO,mean);
	if (smeanO) fmatrixCopy(smeanO,smean);
	if (devO)   fmatrixCopy(devO,dev);
	if (countO) fvectorCopy(countO,count);
     }
     return TCL_OK;
  }	 
  MUST_FMATRIX(source);
  return TCL_ERROR;
}


/* ------------------------------------------------------------------------
   Method 'meansub'
   ------------------------------------------------------------------------ */
static int fesMeansubItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin = NULL, *source = NULL;
  Feature     *wfeat  = NULL;
  Feature     *mfeat  = NULL;
  float             a = 1.0;
  float         alpha = 1.0;
  FVector       *mean = NULL;
  FVector       *dev  = NULL;
  FVector       *smean= NULL;
  char          *upMc = NULL;
  char          *upSc = NULL;
  int           useup = 1;
 
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<feature>", ARGV_CUSTOM, createFe,   &destin, cd, NEW_FEAT,
    "<source>",  ARGV_CUSTOM, getFe,      &source, cd, SRC_FEAT,
    "-a",        ARGV_FLOAT,  NULL,       &a,      cd, "if (a > 0) a * standard deviation is normalised to 1.0",
    "-mean",     ARGV_CUSTOM, getFVector, &mean,   cd, "mean vector of type FVector",
    "-dev",      ARGV_CUSTOM, getFVector, &dev,    cd, "deviation vector of type FVector",
    "-smean",    ARGV_CUSTOM, getFVector, &smean,  cd, "mean of squares vector of type FVector",
    "-upMean",   ARGV_STRING, NULL,       &upMc,   cd, "update mean in FVector object",
    "-upSMean",  ARGV_STRING, NULL,       &upSc,   cd, "update mean of squares  in FVector object",
    "-useup",    ARGV_INT,    NULL,       &useup,  cd, "1 for: \"use updated vectors\" or 0 for: \"current\"",
    "-weight",   ARGV_CUSTOM, getFe,      &wfeat,  cd, "feature that weights each frame when mean is calculated",
    "-factor",   ARGV_CUSTOM, getFe,      &mfeat,  cd, "feature that weights each frame when mean is subtracted, a:=0!",
    "-alpha",    ARGV_FLOAT,  NULL,       &alpha,  cd, "adaptation factor",
     NULL) != TCL_OK) return TCL_ERROR;

# define FREE_MSVECS {if(mean) fvectorFree(mean); if(smean) fvectorFree(smean); if(dev)  fvectorFree(dev);}
  /* -------------------
     FLOAT features
     ------------------- */
  if (IS_FMATRIX( source)) {
     FMatrix      *mat1 = source->data.fmat;
     int         frameA = 0;
     int frameX, frameN = mat1->m;
     int coeffX, coeffN = mat1->n;
     FMatrix     *mat;
     FMatrix     *weight = NULL, *factor = NULL;

     if (wfeat != NULL) {
	   if (IS_FMATRIX( wfeat))
           weight = wfeat->data.fmat;
	   else {
           ERROR("weight-feature %s must be of type FMatrix!\n", wfeat->name);
           return TCL_ERROR;
       }
     }
     if (mfeat != NULL) {
	   if (IS_FMATRIX( mfeat))
         factor = mfeat->data.fmat;
	   else {
           ERROR("mean-feature %s must be of type FMatrix!\n", mfeat->name);
           return TCL_ERROR;
       }
	   if (factor->m != frameN) {
	     ERROR("frame number of mean-feature %s is not %d.\n", mfeat->name, frameN);
         return TCL_ERROR;
       }
     }
     /* --------------------------------------------------
	check mean and dev if given, then alloc feature
	-------------------------------------------------- */
     if (dev && smean) { fvectorFree(smean); smean = NULL; }
     if (mean && mean->n != coeffN) {
	ERROR("dimension of mean vector is %d not %d.\n", mean->n, coeffN);
	FREE_MSVECS;	return TCL_OK;
     }
     else if (dev && dev->n != coeffN) {
	ERROR("dimension of deviation vector is %d not %d.\n", dev->n, coeffN);
	FREE_MSVECS;	return TCL_OK;
     }
     else if (smean && smean->n != coeffN) {
	ERROR("dimension of smean vector is %d not %d.\n", smean->n, coeffN);
	FREE_MSVECS;	return TCL_OK;
     }
     if (smean) {
	int i; double val;
	if (!mean) {
	   ERROR("missing mean vector while using -smean.\n");
	   FREE_MSVECS;	return TCL_OK;
	}
	dev = smean;
	for (i=0;i<coeffN;i++) {
	   val = smean->vecA[i] - mean->vecA[i]*mean->vecA[i];
	   dev->vecA[i] = (val>0) ? sqrt(val) : 0; 
	}
	smean = NULL;
     }

     mat = fmatrixCreate(frameN,coeffN); if (!mat) return TCL_ERROR;


     /* ----------------------------------------------
	if no mean and dev is given create both
	---------------------------------------------- */
     if (!mean && !dev) {
        smean = fvectorCreate(coeffN);
        mean  = fvectorCreate(coeffN);
        dev   = fvectorCreate(coeffN);
	if (!mean || !dev || !smean) {
	   fmatrixFree(mat);
	   FREE_MSVECS;	return TCL_ERROR;
	}
	fmatrixMeanvar( mat1, weight, mean, smean, dev);
	/* -----------------------------------------------
	   update
	   ----------------------------------------------- */
	if (upMc) {
	   FVector *upMean = (FVector*)itfGetObject( upMc, "FVector");
	   if (upMean) {
	     int i;
	     double ab, a, b;

	     if (upMean->n!=coeffN) {
	       WARN("meansub: Will resize 'upMean' vector\n");
	       fvectorResize(upMean,coeffN);
	       upMean->count = 0.0;
	     }
	     /* normalization over the number of frames/second */
	     if (source->shift) {
	       float rate = 1000 / source->shift;
	       alpha = pow(alpha,mean->count/rate);
	     }
	     ab    = upMean->count*alpha + mean->count;
	     a     = ab==0.0 ? 0.0 : upMean->count*alpha / ab;
	     b     = ab==0.0 ? 0.0 : mean->count / ab;
	     for (i=0;i<coeffN;i++) {
	       upMean->vecA[i] = a * upMean->vecA[i] + b * mean->vecA[i]; 
	       upMean->count   = ab;
	       if (useup) mean->vecA[i]   = upMean->vecA[i];
	     }
	     if (upSc) {
	       FVector *upSMean = (FVector*)itfGetObject( upSc, "FVector");
	       
	       if (upSMean) {
		 if (upSMean->n!=coeffN) {
		   WARN("meansub: Will resize 'upSMean' vector\n");
		   fvectorResize(upSMean,coeffN);
		   upSMean->count = 0.0;
		 }
		 for (i=0;i<coeffN;i++) {
		   upSMean->vecA[i] = a * upSMean->vecA[i] + b * smean->vecA[i];
		   upSMean->count   = ab;
		   if (useup) {
		     dev->vecA[i] = upSMean->vecA[i] - upMean->vecA[i] * upMean->vecA[i];
		     dev->vecA[i] = (dev->vecA[i] > 0) ? sqrt((double) dev->vecA[i]) : 0.0; 
		   }
		 }
	       }
	       else WARN("meansub: no smean, won't update FVector %s\n",upSc);
	     }
	   }
	}
	fvectorFree(smean);
     }

     /* -----------------------------------------------
	do mean calculation
	----------------------------------------------- */
     
     if (factor == NULL) { /* no weighting for each frame */
       if (a <= 0 || !dev ) {
          if (mean) foreach_coeff
             mat->matPA[frameX][coeffX] = mat1->matPA[frameX][coeffX] - mean->vecA[coeffX];
          else foreach_coeff
             mat->matPA[frameX][coeffX] = mat1->matPA[frameX][coeffX];
       }
       else {
          for (coeffX=0; coeffX<coeffN; coeffX++) dev->vecA[coeffX] *= a;
          if (mean) foreach_coeff {
             if (dev->vecA[coeffX] != 0.0)
                mat->matPA[frameX][coeffX] = (mat1->matPA[frameX][coeffX] - mean->vecA[coeffX])/dev->vecA[coeffX];
             else
                mat->matPA[frameX][coeffX] = mat1->matPA[frameX][coeffX] - mean->vecA[coeffX];
          }
          else foreach_coeff {
             if (dev->vecA[coeffX] != 0.0)
                mat->matPA[frameX][coeffX] = mat1->matPA[frameX][coeffX] / dev->vecA[coeffX];
             else
                mat->matPA[frameX][coeffX] = mat1->matPA[frameX][coeffX];
          }
       }
     } else { /* subtract mean weighted with factor */
       if (mean) foreach_coeff
          mat->matPA[frameX][coeffX] = mat1->matPA[frameX][coeffX] - factor->matPA[frameX][0] * mean->vecA[coeffX];
       else foreach_coeff
          mat->matPA[frameX][coeffX] = mat1->matPA[frameX][coeffX];
     }
     if (mean) fvectorFree(mean); if (dev) fvectorFree(dev);
     if(!feFMatrix( FS, destin, mat, 0)) {
	COPY_CONFIG( destin, source); return TCL_OK;
     }
     fmatrixFree(mat); return TCL_ERROR;
  }	 
  MUST_FMATRIX(source);
  return TCL_ERROR;
}


/* ------------------------------------------------------------------------
   Method 'MVN' --- mean and variance normalization by FF
   ------------------------------------------------------------------------ */
static int fesMVNItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin = NULL, *source = NULL;
  Feature     *wfeat  = NULL;
  float       a = 1.0;
  char        *upMc = NULL;
  char        *upSc = NULL;
  FVector     *upMean;
  FVector     *upSMean;
  float       alpha;
  float       alphaI;

  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<feature>", ARGV_CUSTOM, createFe,   &destin, cd, NEW_FEAT,
    "<source>",  ARGV_CUSTOM, getFe,      &source, cd, SRC_FEAT,
    "<mean>",    ARGV_STRING, NULL,       &upMc,   cd, "update mean in FVector object",
    "<smean>",   ARGV_STRING, NULL,       &upSc,   cd, "update mean of squares in FVector object",
    "<alpha>",   ARGV_FLOAT,  NULL,       &alpha, cd,  "exponential weighting factor",
    "-a",        ARGV_FLOAT,  NULL,       &a,      cd, "if (a > 0) a * standard deviation is normalised to 1.0",
    "-weight",   ARGV_CUSTOM, getFe,      &wfeat,  cd, "feature that weights each frame when mean is calculated",
  NULL) != TCL_OK) return TCL_ERROR;

  alphaI = 1 - alpha;
	upMean = (FVector*)itfGetObject( upMc, "FVector");
  upSMean = (FVector*)itfGetObject( upSc, "FVector");

  # define FREE_MSVECS {if(mean) fvectorFree(mean); if(smean) fvectorFree(smean); if(dev)  fvectorFree(dev);}
  /* -------------------
     FLOAT features
     ------------------- */
  if (IS_FMATRIX( source)) {
    FMatrix     *mat1 = source->data.fmat;
    int frameX, frameN = mat1->m;
    int coeffX, coeffN = mat1->n;
    FMatrix     *mat;
    FMatrix     *weight = NULL;
    float       mean, smean, dev, alphaW, alphaWI, v;

    if (wfeat != NULL) {
	    if (IS_FMATRIX( wfeat))
        weight = wfeat->data.fmat;
	    else
      {
        ERROR("weight-feature %s must be of type FMatrix!\n", wfeat->name);
        return TCL_ERROR;
      }
    }

    mat = fmatrixCreate(frameN,coeffN); if (!mat) return TCL_ERROR;

    for (frameX=0; frameX<frameN; frameX++)
    {
      alphaW  = alpha;
      if (weight!=NULL)
        alphaW  += (1 - weight->matPA[frameX][0]) * (1 - alpha);
      alphaWI = (1-alphaW);
      for (coeffX=0; coeffX<coeffN; coeffX++)
      {
        v = mat1->matPA[frameX][coeffX];
        /* compute exponential weighted mean */
        mean  = (upMean->vecA[coeffX] * alphaW) + (v * alphaWI); 
        /* compute exponential weighted varaince */
        /*d = (mat1->matPA[frameX][coeffX] - mean);
        var = (upSMean->vecA[coeffX] * alphaW) + (d*d * alphaWI);*/
        /* compute exponential weighted smean */        
        smean = (upSMean->vecA[coeffX] * alphaW) + (v*v * alphaWI);
        /* compute a * standard deviation */
        /* dev = sqrt(var) * a;*/
        if (a>0)
        {
          dev = sqrt(smean - mean*mean) * a;
          if (dev==0) { dev = 0.00001;  } /* WARN("Need some help!"); */
        }
        else
          dev = 1;
        /* mean & variance normalization */
        mat->matPA[frameX][coeffX] = (v - mean) / dev;
        /* update */
        upMean->vecA[coeffX]  = mean;
        /* upSMean->vecA[coeffX] = var; */
        upSMean->vecA[coeffX] = smean;
      }
    }
      
    if(!feFMatrix( FS, destin, mat, 0)) {
	    COPY_CONFIG( destin, source); return TCL_OK;
    }
    fmatrixFree(mat); return TCL_ERROR;
  }

  MUST_FMATRIX(source);
  return TCL_ERROR;
}


/* ------------------------------------------------------------------------
   Method 'delta'
   ------------------------------------------------------------------------ */
static int fesDeltaItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin = NULL, *source = NULL;
  char        *cdelta = "1";
  int         delta;
    
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>",        ARGV_CUSTOM, createFe,&destin, cd, NEW_FEAT,
      "<source_feature>", ARGV_CUSTOM, getFe,   &source, cd, SRC_FEAT,
      "-delta",           ARGV_STRING, NULL,    &cdelta, cd,
      "delta (in time format)",
      NULL) != TCL_OK) return TCL_ERROR;

  if (getSampleX(FS,source,cdelta,&delta)) return TCL_ERROR;
  /* -------------------
     FLOAT features
     ------------------- */
  if (IS_FMATRIX( source)) {
     FMatrix      *mat1 = source->data.fmat;
     FMatrix      *mat  = NULL;
     int         frameA = 0;
     int frameX, frameN = mat1->m;
     int coeffX, coeffN = mat1->n;
      
     Switch( argv[0])
	Case( "delta")  {
	  int frame1, frame2, frameMax = frameN - 1;
	   if (FS->ready==0) {
	     frameN -= ABS(delta); /* runon mode */
	     if (frameN<0) frameN = 0;
	   }
	   mat = fmatrixFeature(frameN,coeffN,FS,destin,&frameA);
	   if (!mat) return TCL_ERROR;

	   foreach_frame {
	      frame1 = IN_RANGE( frameX + delta, 0, frameMax);
	      frame2 = IN_RANGE( frameX - delta, 0, frameMax);
	      for (coeffX=0; coeffX<coeffN; coeffX++)
		 mat->matPA[frameX][coeffX] =  mat1->matPA[frame1][coeffX] - mat1->matPA[frame2][coeffX];
	   }
	}
	Case( "shift")  {
	   int frameMax = frameN - 1;
	   if (FS->ready==0 && delta<0) {
	     frameN += delta; /* runon mode */
	     if (frameN<0) frameN = 0;
	   }
	   mat = fmatrixFeature(frameN,coeffN,FS,destin,&frameA);
	   if (!mat) return TCL_ERROR;

	   foreach_frame {
	      frameA = IN_RANGE( frameX - delta, 0 , frameMax);
	      for (coeffX=0; coeffX<coeffN; coeffX++)
		 mat->matPA[frameX][coeffX] = mat1->matPA[ frameA][ coeffX];
	   }
	}
	Case( "adjacent")  {
	   int frameI, coeffI, frameMax = frameN - 1;
	   if (FS->ready==0) {
	     frameN -= ABS(delta); /* runon mode */
	     if (frameN<0) frameN = 0;
	   }
	   mat = fmatrixFeature(frameN,(2*delta+1)*coeffN,FS,destin,&frameA);
	   if (!mat) return TCL_ERROR;

	   foreach_frame {
	      coeffI = 0;
	      for ( frameI=frameX-delta; frameI<=frameX+delta; frameI++) {
		 frameA = IN_RANGE( frameI, 0 , frameMax);
		 for (coeffX=0; coeffX<coeffN; coeffX++)
		    mat->matPA[frameX][coeffI++] = mat1->matPA[frameA][coeffX];
	      }
	   }
	}
	Default {
	   ERROR("Don't know function %s.",argv[0]);
	   return TCL_ERROR;
	}
     End_switch;
     
     if (!feFMatrix( FS, destin, mat, 1)) {
	COPY_CONFIG( destin, source); return TCL_OK;
     }
     fmatrixFree(mat); return TCL_ERROR;
  }	 
  /* ------------------
     SHORT features
     ------------------ */
  else if (IS_SVECTOR( source)) {
     SVector*    vec1   = source->data.svec;
     SVector*    vec    = NULL;
     int         frameA = 0;
     int frameX, frameN = vec1->n;
      
     Switch( argv[0])
	Case( "delta")  {
           int frameMax = frameN - 1;
	   if (FS->ready==0) {
	     frameN -= ABS(delta); /* runon mode */
	     if (frameN<0) frameN = 0;
	   }
	   vec = svectorFeature(frameN,FS,destin,&frameA);
	   if (!vec) return TCL_ERROR;
	   foreach_frame
	      vec->vecA[frameX] = vec1->vecA[ IN_RANGE(frameX+delta,0,frameMax)] 
	                          - vec1->vecA[ IN_RANGE(frameX-delta,0,frameMax)];
	}
	Case( "shift")  {
           int frameMax = frameN - 1;
	   if (FS->ready==0 && delta<0) {
	     frameN += delta; /* runon mode */
	     if (frameN<0) frameN = 0;
	   }
	   vec = svectorFeature(frameN,FS,destin,&frameA);
	   foreach_frame
	      vec->vecA[frameX] = vec1->vecA[ IN_RANGE(frameX-delta,0,frameMax)];
	}
        Default {
	   ERROR("For command %s type of source feature %s must be FMatrix.",argv[0]);
	   svectorFree(vec); return TCL_ERROR;
	}
     End_switch;
          
     feSVector( FS, destin, vec, 1);
     COPY_CONFIG( destin, source); return TCL_OK;
  }
  DONT_KNOW(source); return TCL_ERROR;
}



/* ------------------------------------------------------------------------
   Method 'thresh'
   ------------------------------------------------------------------------ */
static int fesThreshItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin = NULL, *source = NULL;
  float       thresh,  value;
  int         compare;
  char        *mode = NULL;
  
  
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>",        ARGV_CUSTOM, createFe,&destin,   cd, NEW_FEAT,
      "<source_feature>", ARGV_CUSTOM, getFe,   &source,   cd, SRC_FEAT,
      "<value>",          ARGV_FLOAT,  NULL,    &value,    cd, "",
      "<thresh>",         ARGV_FLOAT,  NULL,    &thresh,   cd, "",
      "<mode>",           ARGV_STRING, NULL,    &mode,     cd, "",
	NULL) != TCL_OK) return TCL_ERROR;

  if      (streq(mode,"upper")) compare =  1;
  else if (streq(mode,"lower")) compare = -1;
  else if (streq(mode,"both" )) compare =  0;
  else {
       ERROR("Mode for command %s must be 'upper', 'lower' or 'both'.\n", argv[0]);
       return TCL_ERROR;
  }
	   
  /* -------------------
     FLOAT features
     ------------------- */
  if (IS_FMATRIX( source)) {
     FMatrix      *mat1 = source->data.fmat;
     int         frameA = 0;
     int frameX, frameN = mat1->m;
     int coeffX, coeffN = mat1->n;
     FMatrix       *mat = fmatrixCreate(frameN,coeffN);  if (!mat) return TCL_ERROR;
      
     if (compare > 0)
	foreach_coeff
	   mat->matPA[frameX][coeffX] = (mat1->matPA[frameX][coeffX] >= thresh) ? value : mat1->matPA[frameX][coeffX];
     
     else if (compare == 0)
	foreach_coeff
	   mat->matPA[frameX][coeffX] = (mat1->matPA[frameX][coeffX] >= thresh) ?
		 value :
		    ((mat1->matPA[frameX][coeffX] <= -thresh) ? -value : mat1->matPA[frameX][coeffX]);
     
     else if (compare < 0)
	foreach_coeff
	   mat->matPA[frameX][coeffX] = (mat1->matPA[frameX][coeffX] <= thresh) ? value : mat1->matPA[frameX][coeffX];
     
     if (!feFMatrix( FS, destin, mat, 1)) {
	COPY_CONFIG( destin, source); return TCL_OK;
     }
     fmatrixFree(mat); return TCL_ERROR;
  }	 
  /* ------------------
     SHORT features
     ------------------ */
  else if (IS_SVECTOR( source)) {
     if ((thresh > SHIGH) || (value > SHIGH) || (thresh < SLOW) || (value < SLOW)) {
	ERROR("Range of 'thresh' and 'value' is %d ... %d.\n", SLOW, SHIGH);
	return TCL_ERROR;
     }
     else {
	SVector  *vec1 = source->data.svec;
        int     frameA = 0;
	int     frameX, frameN = vec1->n;
	short  threshS = (short)thresh;
	short   valueS = (short)value;
	SVector  *vec  = svectorCreate(frameN);  if (!vec) return TCL_ERROR;
	
	if (compare > 0)
	   foreach_frame
	      vec->vecA[frameX] = (vec1->vecA[frameX] >= threshS) ? valueS : vec1->vecA[frameX];
	
	else if (compare == 0)
	   foreach_frame
	      vec->vecA[frameX] = (vec1->vecA[frameX] >= threshS) ?
		 valueS :
		    ((vec1->vecA[frameX] <= -threshS) ? -valueS : vec1->vecA[frameX]);
	
	else if (compare < 0)
	   foreach_frame
	      vec->vecA[frameX] = (vec1->vecA[frameX] <= threshS) ? valueS : vec1->vecA[frameX];
	
	feSVector( FS, destin, vec, 1);
	COPY_CONFIG( destin, source); return TCL_OK;
     }
  }
  DONT_KNOW(source); return TCL_ERROR;
}



/* ------------------------------------------------------------------------
   Method 'split'
   ------------------------------------------------------------------------ */
static int fesSplitItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin = NULL, *source = NULL;
  int         from = 0, to = -1;  
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>",        ARGV_CUSTOM, createFe,&destin,   cd, NEW_FEAT,
      "<source_feature>", ARGV_CUSTOM, getFe,   &source,   cd, SRC_FEAT,
      "<from>",           ARGV_INT,    NULL,    &from,     cd, "",
      "<to>",             ARGV_INT,    NULL,    &to,       cd, "",
      NULL) != TCL_OK) return TCL_ERROR;

  /* -------------------
     FLOAT features !
     ------------------- */
  if (IS_FMATRIX(source)) {
    FMatrix      *mat1 = source->data.fmat;

    if (to >= mat1->n)
       WARN("split: reduce 'to' to max. coef. index %d\n",mat1->n - 1);
    IF_EMPTY_RANGE(from,to,mat1->n) {
      ERROR("Given range is empty! Coefficients 0 .. %d available.\n",
	    mat1->n - 1);
      return TCL_ERROR;
    }
    else {
      int         frameA = 0;
      int frameX, frameN = mat1->m;
      int coeffX, coeffN = to + 1 - from;
      FMatrix *mat = fmatrixCreate(frameN,coeffN); if (!mat) return TCL_ERROR;
      
      foreach_coeff
	mat->matPA[frameX][coeffX] = mat1->matPA[frameX][coeffX + from];
      
      if(!feFMatrix( FS, destin, mat, 1)) {
	COPY_CONFIG( destin, source); return TCL_OK;
      }
      fmatrixFree(mat); return TCL_ERROR;
    }	 
  }
  MUST_FMATRIX(source);
  return TCL_ERROR;
}


/* ------------------------------------------------------------------------
   Method 'cut'
   ------------------------------------------------------------------------ */
static int fesCutItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin = NULL, *source = NULL, *select = NULL;
  char        *cfrom = "0", *cto = "last";
  int         from, to;
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>",        ARGV_CUSTOM, createFe,&destin,   cd, NEW_FEAT,
      "<source_feature>", ARGV_CUSTOM, getFe,   &source,   cd, SRC_FEAT,
      "<from>",           ARGV_STRING, NULL,    &cfrom,    cd, "start",
      "<to>",             ARGV_STRING, NULL,    &cto,      cd, "end",
      "-select",          ARGV_CUSTOM, getFe,   &select,   cd, "1-dimensional FMatrix feature that selects the parts to be taken",
      NULL) != TCL_OK) return TCL_ERROR;

  if (getSampleX(FS,source,cfrom,&from) || getSampleX(FS,source,cto,&to))
      return TCL_ERROR;

  /* -------------------
     FLOAT features !
     ------------------- */
  if (IS_FMATRIX(source)) {
    FMatrix     *mat1 = source->data.fmat;

    IF_EMPTY_RANGE(from, to, mat1->m) {
      ERROR("Given range is empty! Frames 0 .. %d available.\n", mat1->m - 1);
      return TCL_ERROR;
    }
    else {
      int         frameA = 0;
      int frameX, frameN = to + 1 - from;
      int coeffX, coeffN = mat1->n;
      FMatrix *mat = fmatrixCreate(frameN,coeffN); if (!mat) return TCL_ERROR;

      foreach_coeff
	mat->matPA[frameX][coeffX] = mat1->matPA[frameX + from][coeffX];

      if (select) {
	if (!IS_FMATRIX(select)) {
	  MUST_FMATRIX(select);
	  fmatrixFree(mat);
	  return TCL_ERROR;
	}
	else {
	  FMatrix *mat2 = select->data.fmat;
	  int frameY = 0;
	  if (mat2->n < 1 || mat2->m < frameN) {
	    ERROR("'-select' feature with wrong dimensions: %d (must be > 0) x %d (must be >= %d)\n", mat2->n, mat2->m, frameN);
	    fmatrixFree(mat);
	    return TCL_ERROR;
	  }
	  foreach_frame {
	    if (mat2->matPA[frameX][0] > 0.0) {
	      for (coeffX=0; coeffX<coeffN; coeffX++)
		mat->matPA[frameY][coeffX] =  mat1->matPA[frameX][coeffX];
	      frameY++;
	    }
	  }
	  fmatrixResize(mat,frameY,coeffN); 
	}
      }

      if(!feFMatrix( FS, destin, mat, 0)) {
	COPY_CONFIG( destin, source); return TCL_OK;
      }
      fmatrixFree(mat); return TCL_ERROR;
    }	 
  }
  /* ------------------
     SHORT features
     ------------------ */
  else if (IS_SVECTOR( source)) {
    SVector      *vec1 = source->data.svec;

    IF_EMPTY_RANGE(from, to, vec1->n) {
      ERROR("Given range is empty! samples 0 .. %d available.\n", vec1->n - 1);
      return TCL_ERROR;
    }
    else {
      int         frameA = 0;
      int frameX, frameN = to + 1 - from;
      SVector      *vec  = svectorCreate(frameN);  if (!vec) return TCL_ERROR;

      foreach_frame
	vec->vecA[frameX] = vec1->vecA[frameX+from];
      if(!feSVector( FS, destin, vec, 0)) {
	COPY_CONFIG( destin, source); return TCL_OK;
      }
      svectorFree(vec); return TCL_ERROR;
    }
  }
  DONT_KNOW(source); return TCL_ERROR;
}


/* ------------------------------------------------------------------------
   Method 'replace'
   ------------------------------------------------------------------------ */
static int fesReplaceItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin = NULL, *source = NULL;
  char        *cfrom = "0";
  int         from;
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>",        ARGV_CUSTOM, getFe,   &destin,   cd, "name of feature to replace",
      "<feature>",        ARGV_CUSTOM, getFe,   &source,   cd, "name of replacing feature",
      "<from>",           ARGV_STRING, NULL,    &cfrom,    cd, "start",
      NULL) != TCL_OK) return TCL_ERROR;

  if ( getSampleX(FS,source,cfrom,&from) )
      return TCL_ERROR;

  if ( destin->type != source->type ) {
    MUST_SAME(destin,source);
    return TCL_ERROR;
  }

  /* -------------------
     FLOAT features !
     ------------------- */
  if (IS_FMATRIX(source)) {
    FMatrix     *mat1   = source->data.fmat;
    FMatrix     *mat2   = destin->data.fmat;
    int          frameA = 0;
    int          frameN = mat1->m;
    int          frameX;
    int          coeffN = mat1->n;
    int          coeffX;

    if ( from + mat1->m > mat2->m ) {
      ERROR ("unable to replace %s with %s starting at %d.\n",
	     destin->name, source->name, from);
      return TCL_ERROR;
    }

    if ( mat1->n != mat2->n ) {
      ERROR ("features of different dimensions.\n");
      return TCL_ERROR;
    }

    foreach_coeff
      mat2->matPA[from+frameX][coeffX] = mat1->matPA[frameX][coeffX];

    return TCL_OK;
  }
  /* ------------------
     SHORT features
     ------------------ */
  else if (IS_SVECTOR( source)) {
    SVector *vec1 = source->data.svec;
    SVector *vec2 = destin->data.svec;
    int      frameA = 0;
    int      frameN = vec1->n;
    int      frameX;

    if ( from + vec1->n > vec2->n ) {
      ERROR ("unable to replace %s with %s starting at %d.\n",
	     destin->name, source->name, from);
      return TCL_ERROR;
    }

    foreach_frame
      vec2->vecA[from+frameX] = vec1->vecA[frameX];

    return TCL_OK;
  }
  DONT_KNOW(source); return TCL_ERROR;
}


/* ------------------------------------------------------------------------
   Method 'append'
   ------------------------------------------------------------------------ */
static int fesAppendItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS     = (FeatureSet*)cd;
  Feature     *destin = NULL,
              *source = NULL;
  char        *mode   = "frames";

  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>",        ARGV_CUSTOM, createFe, &destin,   cd, "name of feature to which the new data is appended",
      "<feature>",        ARGV_CUSTOM, getFe,    &source,   cd, "name of appending feature",
      "<mode>",           ARGV_STRING, NULL,     &mode,     cd, "append frames/ coeffs (i.e. rows/ columns)",
      NULL) != TCL_OK) return TCL_ERROR;

  /* -------------------
     FLOAT features !
     ------------------- */
  if (IS_FMATRIX(source)) {
    FMatrix     *mat1   = source->data.fmat;
    FMatrix     *mat2   = destin->data.fmat;
    int          frameN = mat1->m;
    int          frameX, frameA;
    int          coeffN = mat1->n;
    int          coeffX, coeffI;

    if ( destin->type == FE_UNDEF ) {
      mat2 = fmatrixCreate (0,0);
      feFMatrix (FS, destin, mat2, 0);
      COPY_CONFIG (destin, source);
    } else if ( !IS_FMATRIX(destin) ) {
      MUST_SAME(destin, source);
      return TCL_ERROR;
    }

    if ( !strcmp(mode, "frames") || !strcmp(mode, "rows") ) {
      if ( mat2->n && mat1->n != mat2->n ) {
	ERROR ("feature '%s' has different number of coefficients %d (!= %d)\n", destin->name, mat2->n, mat1->n);
	return TCL_ERROR;
      }

      frameN += mat2->m;
      frameA  = mat2->m;
      fmatrixResize (mat2, frameN, coeffN);

      memcpy (mat2->matPA[frameA], mat1->matPA[0], mat1->m * mat1->n * sizeof(float));

    } else if ( !strcmp(mode, "coeffs") || !strcmp(mode, "columns") ) {
      if ( mat2->m && mat1->m != mat2->m ) {
	ERROR ("feature '%s' has different number of frames %d (!= %d)\n", destin->name, mat2->m, mat1->m);
	return TCL_ERROR;
      }

      coeffI  = mat2->n;
      frameA  = 0;
      fmatrixResize (mat2, frameN, coeffN+mat2->n);

      foreach_coeff
	mat2->matPA[frameX][coeffI+coeffX] = mat1->matPA[frameX][coeffX];

    } else {
      ERROR ("unknown mode '%s'\n", mode);
      return TCL_ERROR;
    }

    return TCL_OK;
  }
  /* ------------------
     SHORT features
     ------------------ */
  else if (IS_SVECTOR( source)) {
    SVector *vec1 = source->data.svec;
    SVector *vec2 = destin->data.svec;
    int      frameA;
    int      frameN = vec1->n;

    if ( destin->type == FE_UNDEF ) {
      vec2 = svectorCreate (0);
      feSVector (FS, destin, vec2, 0);
      COPY_CONFIG (destin, source);
    } else if ( !IS_SVECTOR(destin) ) {
      MUST_SAME(destin,source);
      return TCL_ERROR;
    }

    if ( !strcmp(mode, "frames") || !strcmp(mode, "rows") ) {
      frameN += vec2->n;
      frameA  = vec2->n;

      svectorResize (vec2, frameN);
      memcpy (&vec2->vecA[frameA], vec1->vecA, vec1->n * sizeof(short));

    } else if ( !strcmp(mode, "coeffs") || !strcmp(mode, "columns") ) {
      ERROR ("unable to append coeffs with SVectors\n");
      return TCL_ERROR;
    } else {
      ERROR ("unknown mode '%s'\n", mode);
      return TCL_ERROR;
    }

    return TCL_OK;
  }
  DONT_KNOW(source); return TCL_ERROR;
}

/* ------------------------------------------------------------------------
   Method 'flip'
   ------------------------------------------------------------------------ */
static int fesFlipItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin = NULL, *source = NULL;
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>",        ARGV_CUSTOM, createFe,&destin, cd, NEW_FEAT,
      "<source_feature>", ARGV_CUSTOM, getFe,   &source, cd, SRC_FEAT,
      NULL) != TCL_OK) return TCL_ERROR;

  /* -------------------
     FLOAT features !
     ------------------- */
  if (IS_FMATRIX(source)) {
    FMatrix      *mat1 = source->data.fmat;
    int         frameA = 0;
    int frameX, frameN = mat1->m, frameL = frameN - 1;
    int coeffX, coeffN = mat1->n;
    FMatrix *mat = fmatrixCreate(frameN,coeffN); if (!mat) return TCL_ERROR;

    foreach_coeff
      mat->matPA[frameX][coeffX] = mat1->matPA[frameL - frameX][coeffX];
	
    if(!feFMatrix( FS, destin, mat, 0)) {
      COPY_CONFIG( destin, source); return TCL_OK;
    }
    fmatrixFree(mat); return TCL_ERROR;
  }	 
  /* ------------------
     SHORT features
     ------------------ */
  else if (IS_SVECTOR( source)) {
    SVector      *vec1 = source->data.svec;
    int         frameA = 0;
    int frameX, frameN = vec1->n, frameL = frameN - 1;
    SVector      *vec  = svectorCreate(frameN);  if (!vec) return TCL_ERROR;

    foreach_frame
      vec->vecA[frameX] = vec1->vecA[frameL - frameX];
    if(!feSVector( FS, destin, vec, 0)) {
      COPY_CONFIG( destin, source); return TCL_OK;
    }
    svectorFree(vec); return TCL_ERROR;
  }
  DONT_KNOW(source); return TCL_ERROR;
}



/*--------------------------------------------------------------------------------------------------------------
; int zerocrossing(short adcA[], int adcN, float mean)
; This function returns the zero crossing rate within the ADC data sample beginning at adcA[0] and ending
; at adcA[adcN-1]. If the input parameter 'mean' is nonzero, it calculates the rate the ADC data crosses not
; zero, but the given value of 'mean' (the 'mean crossing rate')
; IN:   short adcA[ ]       the ADC data array
;       int   adcN          the number of ADC data points
;       float mean          this is the border between positive and negative (see comment above)
; OUT:  function's return value - # of times, the ADC data crosses the value 'mean' 
; History:       30 Nov 93       tk     created
;                12 Aug 94       mm     corrected
--------------------------------------------------------------------------------------------------------------*/
static int zerocrossing(short *adcA, int adcN, float mean)
{
  int i,crosscount=0;
  short current, previous = adcA[0]-(short)mean;
  
  for (i=1; i<adcN; i++) {
     if ((current = adcA[i] - (short)mean)!= 0){
	if ((previous*current) < 0) crosscount++;
	previous = current;
     }
  }
  return(crosscount);
}

/*--------------------------------------------------------------------------------------------------------------
; int peak_to_peak(short adcA[], int adcN)
; This function returns the maximum valley-to-next-peak value in the given ADC data sample. It always scans from one
; valley to the next peak and calculates the difference between the two. The maximum difference over the sample is
; returned.
; IN:  short adcA[ ]         the ADC data array
;      int   adcN            the number of ADC data points
; OUT: function's return value - maximum valley to next peak value (or peak-to-next-valley value), always positive
; History:       30 Nov 93       tk     created
----------------------------------------------------------------------------------------------------------------*/
static int peak_to_peak(short *adcA, int adcN)
{
  int i,min, max, tangent, maxdiff;
  
  tangent=SGN(adcA[1]-adcA[0]);
  min=max=adcA[0];
  maxdiff=(adcA[1]-adcA[0])*tangent;
  for (i=1; i<adcN; i++){
    if (adcA[i] == adcA[i-1]) continue;
    if (SGN(adcA[i] - adcA[i-1]) != tangent){
      if (tangent > 0){
	max=adcA[i-1];
      }
      else{
	min=adcA[i-1];
      }
      if (max - min > maxdiff) maxdiff=max-min;
      tangent = SGN(adcA[i] - adcA[i-1]);
    } /* if SGN changed */
  }
  return(maxdiff);
}

/*--------------------------------------------------------------------------------------------------------------
; int maxpeak_to_peak(short adcA[], int adcN)
; This function returns the maximum valley-to-peak value in the given ADC data sample. It searches the deepest valley
; and the highest peak and returns the difference.
;
; IN:  short adcA[ ]         the ADC data array
;      int   adcN            the number of ADC data points
; OUT: function's return value - peak to peak value
; History:       01 Dec 93       tk     created
----------------------------------------------------------------------------------------------------------------*/
static int maxpeak_to_peak(short *adcA,int adcN)
{
  int i,min, max;
  
  min=max=adcA[0];
  for (i=1; i<adcN; i++){
    if (adcA[i]>max) max=adcA[i];
    if (adcA[i]<min) min=adcA[i];
  }
  return(max-min);
}

/*--------------------------------------------------------------------------------------------------------------
; float calc_power(short adcA[ ], int adcN)
; This routines calculates the total power in the ADC sample handed over, normalized by the number of data points
; in the sample.
; IN:  short adcA[ ]         the ADC data array
;      int   adcN            the number of ADC data points
  OUT: function's return value - power
; History:       30 Nov 93       tk     created
----------------------------------------------------------------------------------------------------------------*/
static float calc_power(short *adcA,int adcN)
{
  int i;
  float power=0.0;
  
  for (i=0; i<adcN; i++){
    power += (float)adcA[i] * (float)adcA[i];
  }
  return(power / (float)adcN );
}



/* ------------------------------------------------------------- */
/* spikeFilter() filters an array of shorts by means of a median */
/* filter to remove short spikes from the adc waveform           */
/*    adc      : waveform array                                  */
/*    adcN     : length of adc                                   */
/*    tapN     : length of filter (must be >= 3)                 */
/* ------------------------------------------------------------- */
static int  spikeFilter (short *adc, int adcN, int tapN) {

  int              adcX,queueN,windowN,queuePnt;
  int              queueX,windowX,j,i;
  short            *queue,*window,swappy;

  /* ---------------------------- */
  /* check for correct parameters */
  /* ---------------------------- */
  if (tapN < 3)    return TCL_OK;
  if (adcN < tapN) return TCL_ERROR;

  /* -------------------------------------- */
  /* allocate filter delay queue and window */
  /* -------------------------------------- */
  queueN  = (tapN-1)>>1;
  queue   = (short *) malloc (sizeof(short)*queueN);
  windowN = tapN;
  window  = (short *) malloc (sizeof(short)*windowN);

  /* ------------------------------ */
  /* fill queue with initial values */
  /* ------------------------------ */
  for (queueX=0; queueX<queueN; queueX++)
    queue[queueX] = adc[queueX];
  queuePnt = 0;

  /* ------------------------------------------ */
  /* move filter window over the waveform array */
  /* ------------------------------------------ */
  for (adcX=queueN; adcX<adcN-queueN; adcX++) {

    /* --------------------------------------------- */
    /* copy samples into filter window and sort them */
    /* --------------------------------------------- */
    for (windowX=0; windowX<windowN; windowX++) {
      window[windowX] = adc[adcX+windowX-queueN];
      i = windowX;
      j = windowX-1;
      while ((j >= 0) && (window[j] > window[i])) {
        swappy      = window[i];
        window[i] = window[j];
        window[j]   = swappy;
        i = j--;
      }
    }

    /* ----------------------------------- */
    /* take oldest sample out of the queue */
    /* ----------------------------------- */
    adc[adcX-queueN] = queue[queuePnt]; 

    /* -------------------------------------- */
    /* pick median and fill it into the queue */
    /* -------------------------------------- */
    queue[queuePnt++] = window[queueN];
    queuePnt %= queueN;
  }
  
  /* --------------------- */
  /* free allocated memory */
  /* --------------------- */
  free(queue);
  free(window);

  return TCL_OK;
}



/* ------------------------------------------------------------- */
/* spikeFilter2() filters an array of shorts by means of a slope */
/* detector to remove short spikes from the adc waveform         */
/*    adc       : waveform array                                 */
/*    adcN      : length of adc                                  */
/*    width     : maximum expected width of spikes               */
/*    maxslope  : (start) value for max slope                    */
/*    thresh    : slope threshold                                */
/*    alpha     : slope adaption factor                          */
/* ------------------------------------------------------------- */
static int  spikeFilter2 (short *adc, int adcN, int width, int maxslope, int startslope,
		   float thresh, float alpha, int verbose)
{
  int              adcP,adcQ;
  int              spikeB,spikeE,spikeN,spikeX;
  float            lambda;
  int              slope;
  float            meanslope = startslope;
  int              max, signB, signE;
  int              count = 0;
  float            beta = 1.0 - alpha;
  
  if (width<=0) return TCL_OK;
  /* ----------------- */
  /* filter the signal */
  /* ----------------- */
  adcP=0;
  adcQ=1;
  while (adcQ<adcN) {

    /* ---------------------------------------- */
    /* check for very high slopes in the signal */
    /* ---------------------------------------- */
    slope = adc[adcQ]-adc[adcP];
    if (slope < 0) {
      slope = -1*slope;
      signB = -1;
    }
    else signB = 1;
    adcP = adcQ++;
    max  = thresh * meanslope;
    if (slope > max && slope > maxslope) {
      int oslope = slope;
      /* ------------------------------- */
      /* determine width of actual spike */
      /* ------------------------------- */
      spikeB = adcP-1;
      spikeN = 0;
      while ((adcQ<adcN) && (spikeN < width)) {
        slope = adc[adcQ]-adc[adcP];
        if (slope < 0) {
	  slope = -1*slope;
	  signE = -1;
	}
	else signE = 1;
        adcP = adcQ++;
        spikeN++;
        if (signB!=signE && slope > max && slope > maxslope) break;
      }
      spikeE = adcP;

      /* ---------------------------------------- */
      /* filter spike out by linear interpolation */
      /* ---------------------------------------- */
      for (spikeX=spikeB+1; spikeX<spikeE; spikeX++) {
        lambda = ((float) (spikeX-spikeB))/(spikeE-spikeB);
        adc[spikeX] = (short) ((1-lambda)*adc[spikeB] + lambda*adc[spikeE]);
      }
      count++;
      if (verbose>1) printf("spike %d at %d..%d, slope = %d, max = %d\n",
			    count,spikeB+1,spikeE-1,oslope,max);

    }
    else {
      meanslope = beta * meanslope + alpha * slope;
    }
  }
  if (verbose>0) printf("%d spikes removed\n",count);
  return TCL_OK;
}



/* ----------------------------------------------------------- */
/* Method AntiSpike : variable length filter for spike removal */
/* ----------------------------------------------------------- */
static int fesAntiSpikeItf (ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS     = (FeatureSet *) cd;
  Feature     *destin = NULL;
  Feature     *source = NULL;
  int         window  = 0;     /* default window width of median filter */  
  int         width   = 3;     /* default max spike width of slope filter */  
  int         maxslope = 7000; /* default max slope of slope filter */  
  int         meanslope= 100;  /* default start mean slope of slope filter */  
  float       thresh  =  0.0;  /* 15  is a good value */
  float       alpha   =  0.0;  /* 0.2 is a good value */
  int         verbose = 0;

  /* --------------- */
  /* parse arguments */
  /* --------------- */
  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
      "<destin>", ARGV_CUSTOM, createFe, &destin, cd, NEW_FEAT,
      "<source>", ARGV_CUSTOM, getFe   , &source, cd, SRC_FEAT,
      "-window" , ARGV_INT   , NULL    , &window, cd, "window width of median filter (<3 = off)",
      "-width"  , ARGV_INT   , NULL    , &width , cd, "max spike width of slope filter (<1 = off)",
      "-maxslope",ARGV_INT   , NULL    , &maxslope,cd,"max slope of slope filter",
      "-meanslope",ARGV_INT  , NULL    , &meanslope,cd,"start mean value of slope filter",
      "-thresh" , ARGV_FLOAT , NULL    , &thresh, cd, "thresh of slope filter",
      "-alpha"  , ARGV_FLOAT , NULL    , &alpha , cd, "adaption factor of slope filter",
      "-v"      , ARGV_INT   , NULL    , &verbose,cd, "verbosity",
      NULL) != TCL_OK) return TCL_ERROR;

  /* ----------------------------- */
  /* check source feature validity */
  /* ----------------------------- */
  if (! IS_SVECTOR( source)) {
    MUST_SVECTOR( source);
    return TCL_ERROR;
  }
  else {
    SVector      *vec1 = source->data.svec;
    int         frameA = 0;
    int frameX, frameN = vec1->n;
    SVector       *vec = svectorCreate(frameN);  if (!vec) return TCL_ERROR;

    /* --------------- */
    /* copy samples    */
    /* --------------- */
    foreach_frame vec->vecA[frameX] = vec1->vecA[frameX];

    /* --------------------- */
    /* apply AntiSpikeFilter */
    /* --------------------- */
    if ((spikeFilter (vec->vecA,frameN,window) != TCL_OK) || 
	(spikeFilter2(vec->vecA,frameN,width,maxslope,meanslope,thresh,alpha,verbose) != TCL_OK) ||
	feSVector( FS, destin, vec, 0)) {
      svectorFree(vec);
      return TCL_ERROR; 
    }
    COPY_CONFIG( destin, source);
    return TCL_OK;
  }
}

/* ----------------------------------------------------------- */
/* Method Offset : adaptive offset removal                     */
/* ----------------------------------------------------------- */
static int fesOffsetItf (ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS     = (FeatureSet *) cd;
  Feature     *destin = NULL;
  Feature     *source = NULL;
  char        *offsets= NULL;
  char        *smeans = NULL;
  float       alpha   =  0.0;  /* update factor */
  float       count   =  0;
  float       mindev  = 0.00001;
  float       smindev;
  char*       cdelta  = "0";
  int         delta;
  float       a       = 1.0;
  char        *upMc   = NULL;
  char        *upSc   = NULL;

  /* --------------- */
  /* parse arguments */
  /* --------------- */
  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
    "<destin>", ARGV_CUSTOM, createFe, &destin, cd, NEW_FEAT,
    "<source>", ARGV_CUSTOM, getFe   , &source, cd, SRC_FEAT,
    "-alpha"  , ARGV_FLOAT , NULL    , &alpha ,  cd, "adaption factor of offset filter",
    "-count"  , ARGV_INT ,   NULL    , &count ,  cd, "if not 0 then calculate alpha using count of the mean vectors",
    "-offset" , ARGV_STRING, NULL    , &offsets, cd, "see -mean (old flag!)",
    "-mean"   , ARGV_STRING, NULL    , &offsets, cd, "start value for mean",
    "-smean"  , ARGV_STRING, NULL    , &smeans,  cd, "start value for smean",
    "-a"      , ARGV_FLOAT,  NULL    , &a,       cd, "a * standard deviation is normalised to 1.0 (if a > 0 and smean given)",
    "-mindev" , ARGV_FLOAT,  NULL    , &mindev,  cd, "minimal deviation",
    "-delta"  , ARGV_STRING, NULL    , &cdelta,  cd, "calculate mean <delta> frames/samples ahead",
    "-upMean" , ARGV_STRING, NULL    , &upMc,    cd, "update mean in FVector object",
    "-upSMean", ARGV_STRING, NULL    , &upSc,    cd, "update mean of squares  in FVector object",
      NULL) != TCL_OK) return TCL_ERROR;

  if (getSampleX(FS,source,cdelta,&delta)) return TCL_ERROR;
  if (mindev<=0) return TCL_ERROR;
  if (a<=0) {
    ERROR("a must be > 0\n"); return TCL_ERROR;
  }
  /* -------------------
     FLOAT feature
     ------------------- */
  if (IS_FMATRIX(source)) {
     FMatrix      *mat1 = source->data.fmat;
     int         frameA = 0;
     int frameX, frameN = (FS->ready==0 && delta>0) ? mat1->m - delta : mat1->m; /* runon */
     int coeffX, coeffN = mat1->n;
     float         beta = 1.0 - alpha;
     FVector      *mean = offsets == NULL ? fvectorCreate(coeffN) : fvectorGetItf( offsets);
     FMatrix       *mat = fmatrixFeature(frameN,coeffN,FS,destin,&frameA);

     if (!mean || !mat) return TCL_ERROR;
     if (mean->n != coeffN) {
       ERROR("mean vector's dimension is %d not %d\n",mean->n,coeffN);
       fvectorFree(mean); fmatrixFree(mat);
       return TCL_ERROR;
     }

     /* -----------------------------
	do also normalization
	----------------------------- */
     if (smeans) {
       float dev = 0.0;
       FVector     *smean = fvectorGetItf( smeans);
       if (!smean) {
	 fvectorFree(mean); fmatrixFree(mat); return TCL_ERROR;
       }
       if (smean->n != coeffN) {
	 ERROR("smean vector's dimension is %d not %d\n",smean->n,coeffN);
	 fvectorFree(mean); fmatrixFree(mat); return TCL_ERROR;
       }
       smindev = mindev*mindev;
       foreach_coeff {
	 float val  = mat1->matPA[frameX][coeffX];
	 /* 1: calculate mean vectors if we have data */
	 int frameS = frameX + delta;
	 if (frameS >= 0  &&  frameS < frameN) {
	   float valS = mat1->matPA[frameS][coeffX];
	   if (count) {
	     mean->vecA[coeffX] = (mean->count * mean->vecA[coeffX] + valS) / (mean->count + 1.0);
	     mean->count += 1.0;
	     smean->vecA[coeffX] = (smean->count * smean->vecA[coeffX] + valS * valS) / (smean->count + 1.0);
	     smean->count += 1.0;
	   } else {
	     mean->vecA[coeffX]   = beta * mean->vecA[coeffX] + alpha * valS;
	     smean->vecA[coeffX]  = beta * smean->vecA[coeffX] + alpha * valS * valS;
	   }
	   dev = smean->vecA[coeffX] - mean->vecA[coeffX] * mean->vecA[coeffX];
	   if (dev<=smindev) dev = mindev;  else dev = sqrt(dev);
	 }
	 /* 2: subtract mean */
	 mat->matPA[frameX][coeffX] = (val - mean->vecA[coeffX]) / (a * dev); 
       }
       /* --- update smean --- */
       if (upSc) {
	 FVector *upSMean = (FVector*)itfGetObject( upSc, "FVector");
	 if (upSMean) fvectorCopy(upSMean,smean);
	 else WARN("Couldn't find FVector %s for updating\n",upSc);
       }
       fvectorFree(smean);
     }
     /* -----------------------------
	only mean subtraction
	----------------------------- */
     else {
       foreach_coeff {
	 float val  = mat1->matPA[frameX][coeffX];
	 /* 1: calculate mean if we have data */
	 int frameS = frameX + delta;
	 if (frameS >= 0  &&  frameS < frameN) {
	   float valS = mat1->matPA[frameS][coeffX];
	   if (count) {
	     mean->vecA[coeffX] = (mean->count * mean->vecA[coeffX] + valS) / (mean->count + 1.0);
	     mean->count += 1.0;
	   } else {
	     mean->vecA[coeffX] = beta * mean->vecA[coeffX] + alpha * valS;
	   }
	 }
	 /* 2: subtract mean */
	 mat->matPA[frameX][coeffX] = val - mean->vecA[coeffX]; 
       }
     }

     /* --- update mean --- */
     if (upMc) {
       FVector *upMean = (FVector*)itfGetObject( upMc, "FVector");
       if (upMean) fvectorCopy(upMean,mean);
       else WARN("Couldn't find FVector %s for updating\n",upMc);
     }
     fvectorFree(mean);

     if(!feFMatrix( FS, destin, mat, 1)) {
       COPY_CONFIG( destin, source); return TCL_OK;
     }
     fmatrixFree(mat); return TCL_ERROR;
  }
  /* ------------------
     SHORT features
     ------------------ */
  else if (IS_SVECTOR( source)) {
     SVector      *vec1 = source->data.svec;
     int         frameA = 0;
     int frameX, frameN = (FS->ready==0 && delta>0) ? vec1->n - delta : vec1->n;
     float         beta = 1.0 - alpha;
     FVector      *mean = offsets == NULL ? fvectorCreate(1) : fvectorGetItf( offsets);
     SVector       *vec = svectorCreate(frameN);

     if (!vec || !mean) return TCL_ERROR;
     if (a!=1.0) {
       /* Might add this in the future */
       WARN("No normalization is done for SVector features (offset only)!\n");
     }
     if (mean->n != 1) {
       ERROR("Given mean vector %s must be one dimensional!\n",offsets);
     }
     else {
       foreach_frame {
	 float val  = vec1->vecA[frameX];
	 /* 1: calculate mean if we have data */
	 int frameS = frameX + delta;
	 if (frameS >= 0  &&  frameS < frameN) {
	   float valS = vec1->vecA[frameS];
	   if (count) {
	     mean->vecA[0] = (mean->count * mean->vecA[0] + valS) / (mean->count + 1.0);
	     mean->count += 1.0;
	   } else {
	     mean->vecA[0] = beta * mean->vecA[0] + alpha * valS;
	   }
	 }
	 /* 2: subtract mean */
	 vec->vecA[frameX] = SLIMIT(val - mean->vecA[0]);
       }
       
       /* --- update mean --- */
       if (upMc) {
	 FVector *upMean = (FVector*)itfGetObject( upMc, "FVector");
	 if (upMean) fvectorCopy(upMean,mean);
	 else WARN("Couldn't find FVector %s for updating\n",upMc);
       }
       if(!feSVector( FS, destin, vec, 1)) {
	 fvectorFree(mean);
	 COPY_CONFIG( destin, source); return TCL_OK;
       }
     }       
     svectorFree(vec); fvectorFree(mean); return TCL_ERROR;
  }
  DONT_KNOW(source); return TCL_ERROR;
}


/* ------------------------------------------------------------------------
   Methods ADC:  'spectrum', 'adc2mel', 'zero', 'peak', 'maxpeak', 'adc2pow'
   ------------------------------------------------------------------------ */

static int fesADCItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin = NULL, *source = NULL;
  char        string[20];
  char        *cwin = "16msec", *cshift = string;
  int         winPoints, shiftPoints;
  float       samplingRate = 0;
  int         lagN = 512;
  sprintf(string,"%fmsec",FS->sfshift);
    
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>",        ARGV_CUSTOM, createFe, &destin, cd, NEW_FEAT,
      "<source_feature>", ARGV_CUSTOM, getFe,    &source, cd, SRC_FEAT,
      "<win>",            ARGV_STRING, NULL,     &cwin,   cd, "window size",
      "-shift",           ARGV_STRING, NULL,     &cshift, cd, "shift",
      NULL) != TCL_OK) return TCL_ERROR;

  /* -----------------------
     check source feature
     ----------------------- */
  samplingRate = source->samplingRate;

  if (! IS_SVECTOR( source)) {
     MUST_SVECTOR( source);
     return TCL_ERROR;
  }
  else if (samplingRate <= 0) {
     ERROR("Sampling rate of source feature %s must be > 0.\n", argv[2]);
     return TCL_ERROR;
  }
  else if (getSampleX(FS,source,cwin,  &winPoints) || 
           getSampleX(FS,source,cshift,&shiftPoints)) {
     return TCL_ERROR;
  }
/*  else if (shiftPoints % samplingRate) {
     ERROR("Shift must be exactly some msec but it's %f msec.\n",
	   (float)shiftPoints/(float)samplingRate);
     return TCL_ERROR;
  } */
  /* -----------------------------------------------
     everything is ok
     ----------------------------------------------- */
  else {
     FMatrix *mat = NULL;
     SVector *vec    = source->data.svec;
     int frameN = (int)((vec->n - (winPoints - shiftPoints)) / shiftPoints);
     int frameA = 0, frameX;

     if (FE_VER) fprintf(STDERR,"winPoints = %d, shiftPoints = %d, frameN = %d\n", winPoints, shiftPoints, frameN);
     /* -----------------------------------------------------------
	Calculate
	new features depending on function
	----------------------------------------------------------- */
     Switch( argv[0])
	Case( "spectrum")  {
	   int log2N = ceil(  log((double)winPoints) / log((double)2.0) );
	   int fftN  = ROUND( pow(2.0,(double)log2N) );
	   float *inA  = (float*)malloc((unsigned)(winPoints*sizeof(float)));

	   if (!inA) return TCL_ERROR;
	   if (FE_DEB) fprintf(STDERR,"log2N = %d, fftN = %d, winPoints = %d\n", log2N, fftN, winPoints);
	   mat  = fmatrixCreate(frameN, fftN/2 + 1);
	   if (!mat) {
	      free(inA);   return TCL_ERROR;
	   }

	   foreach_frame {
	      /* do_hamm_window( vec->vecA + (frameX * shiftPoints), winPoints, inA); */
	      do_hamm_window( vec->vecA + (frameX * shiftPoints), winPoints, inA);
	      fft_pow       ( inA, mat->matPA[frameX], winPoints, log2N);
	   }
	   free(inA);
	}
     	Case( "adc2mel")  {
	   int err;
	   
	   mat  = fmatrixCreate(frameN, FE_MELSCALES_N);  if(!mat) return TCL_ERROR;
	   /* --- Initialize lookup tables --- */
	   if ((err = init_fht(winPoints)) != 0) {
	      if (err > 0) { ERROR("FFT: Window size too big, maximum is %d",err);     } 
	      else         { ERROR("FFT: %d is not allowed as window size",winPoints); };
	      return TCL_ERROR;
	   }
	   
	   foreach_frame
	      adc_to_melscale_frame( vec->vecA  + (frameX * shiftPoints),
				     mat->matPA[frameX],
				     (int)samplingRate, winPoints );
	}
     	Case( "zero")  {
	   float per_sec = samplingRate/(float)winPoints; 
	   mat  = fmatrixCreate(frameN,1);  if(!mat) return TCL_ERROR;
	   
	   foreach_frame
	      mat->matPA[frameX][0] = zerocrossing( vec->vecA + (frameX * shiftPoints), (int)winPoints, (float)0.0) * per_sec;
	}
     	Case( "peak")  {
	   mat  = fmatrixCreate(frameN,1);  if(!mat) return TCL_ERROR;
	   
	   foreach_frame
	      mat->matPA[frameX][0] = peak_to_peak( vec->vecA + (frameX * shiftPoints), winPoints)/(float)ADCRANGE;
	}
     	Case( "maxpeak")  {
	   mat  = fmatrixCreate(frameN,1);  if(!mat) return TCL_ERROR;
	   
	   foreach_frame
	      mat->matPA[frameX][0] = maxpeak_to_peak( vec->vecA + (frameX * shiftPoints), winPoints)/(float)ADCRANGE;
	}
     	Case( "adc2pow")  {
	   float range= (float)ADCRANGE * (float)ADCRANGE / 4.0;
	   mat  = fmatrixCreate(frameN,1);  if(!mat) return TCL_ERROR;

	   foreach_frame
	      mat->matPA[frameX][0] = calc_power( vec->vecA + (frameX * shiftPoints), winPoints)/range;
	}

	/* Methods conceived by Yue Pan (?)
	   Introduced by Florian Metze
	   Useful for D&S Beamforming */

        Case( "fft2")  {
           int log2N = ceil(  log((double)winPoints) / log((double)2.0) );
           int fftN  = ROUND( pow(2.0,(double)log2N) );
           float *inA  = (float*)malloc((unsigned)(winPoints*sizeof(float)));

           if (!inA) return TCL_ERROR;
           if (FE_DEB) 
             fprintf(STDERR,"log2N = %d, fftN = %d, winPoints = %d\n",
                             log2N, fftN, winPoints);
           mat  = fmatrixCreate(frameN, fftN);
           if (!mat) {
              free(inA);   return TCL_ERROR;
           }

           foreach_frame {
              do_hamm_window( vec->vecA + (frameX * shiftPoints), 
                              winPoints, inA);
              fft_pow2      ( inA, mat->matPA[frameX], winPoints, log2N);
           }
           free(inA);
        }
        Case( "impulse")  {
           int i, startX;

           mat  = fmatrixCreate(frameN, lagN+winPoints);
           if (!mat) return TCL_ERROR;

           foreach_frame {
              short* inA = vec->vecA + (frameX * shiftPoints);
              for (i = 0; i < winPoints; i++)
                mat->matPA[frameX][lagN+i] = inA[i];
              /* fill lag */
              startX = frameX * shiftPoints - lagN;
              for (i = 0; startX < 0; startX++, i++) {
                mat->matPA[frameX][i] = 0;
              }
              inA = vec->vecA + startX;
              for (;i < lagN; i++) {
                mat->matPA[frameX][i] = inA[i];
              }
           }
        }

        Default {
	   ERROR("FE_ADC: Don't know function %s.",argv[0]);
	   return TCL_ERROR;
	}
     End_switch;
     
     if(feFMatrix( FS, destin, mat, 1)) {
	fmatrixFree(mat); return TCL_ERROR;
     }
     destin->samplingRate = samplingRate;
     destin->shift        = shiftPoints / samplingRate;
  }
  return TCL_OK;
}	 

/* ------------------------------------------------------------------------
     Methods conceived by Yue Pan (?)
     Introduced by Florian Metze
     Useful for D&S Beamforming
   ------------------------------------------------------------------------ */

static int fesMixItf( ClientData cd, int argc, char *argv[])
{
/*  FeatureSet  *FS = (FeatureSet*)cd; */
  Feature     *source  = NULL;
  Feature     *source1 = NULL;
  int            shift = 0;
  float           gain = 1.0;

  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<source_feature>", ARGV_CUSTOM, getFe,    &source,  cd, SRC_FEAT,
      "<source_feature>", ARGV_CUSTOM, getFe,    &source1, cd, SRC_FEAT,
      "-shift",           ARGV_INT,    NULL,     &shift,   cd, "shift between features",
      "-gain",            ARGV_FLOAT,  NULL,     &gain,    cd, "gain",
      NULL) != TCL_OK) return TCL_ERROR;

  if (! IS_SVECTOR( source)) {
     MUST_SVECTOR( source);
     return TCL_ERROR;
  } else if (! IS_SVECTOR( source1)) {
     MUST_SVECTOR( source1);
     return TCL_ERROR;
  } else {
     SVector *vec    = source->data.svec;
     SVector *vec1   = source1->data.svec;
     int samplesN = (int)(vec->n);
     int samplesX;
     float sum;

     if (samplesN != (int)(vec1->n)) {
       ERROR("length of sources must be equal\n");
       return TCL_ERROR;
     }

     for (samplesX = samplesN-1; samplesX >= 0; samplesX--) {
       if (samplesX+shift < 0 || samplesX+shift >= vec1->n)
	 sum = gain *  vec->vecA[samplesX];
       else
	 sum = gain * (vec->vecA[samplesX] + vec1->vecA[samplesX+shift]) * 0.5;

       if      (sum >  32765) sum =  32765;
       else if (sum < -32765) sum = -32765;
       vec->vecA[samplesX] = (short) sum;
     }
  }
  return TCL_OK;
}

static int fesMixNItf (ClientData cd, int argc, char *argv[])
{
  FeatureSet       *FS = (FeatureSet*) cd;
  Feature      *destin = NULL;
  Feature      *source = NULL;
  SVector         *vec = NULL;
  char*        infeats = NULL;
  char*        filters = NULL;
  int          ignoreN = 0;
  int          sampleN = -1;
  int           normal = 1;
  float           gain = 1.0;

  int    refN, fltN, i;
  char  **refv, **fltv;
  short          **svA;

  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>",         ARGV_CUSTOM, createFe, &destin,  cd, NEW_FEAT,
      "<source_features>", ARGV_STRING, NULL,     &infeats, cd, "list of source features",
      "-filters",          ARGV_STRING, NULL,     &filters, cd, "filters (objects) to apply to signals",
      "-ignoreN",          ARGV_INT,    NULL,     &ignoreN, cd, "samples to ignore left/ right",
      "-normalize",        ARGV_INT,    NULL,     &normal,  cd, "normalize with number of features",
      "-gain",             ARGV_FLOAT,  NULL,     &gain,    cd, "gain",
      NULL) != TCL_OK) return TCL_ERROR;

  /* Get input vectors (required) */
  if (Tcl_SplitList (itf, infeats, &refN, &refv) != TCL_OK || refN < 1) {
    ERROR ("Couldn't split list '%s'.\n", infeats);
    Tcl_Free ((char*) refv);
    return TCL_ERROR;
  }
  
  /* Get filters (optional) */
  if (filters && (Tcl_SplitList (itf, filters, &fltN, &fltv) != TCL_OK || fltN < 1)) {
    ERROR ("Couldn't split list '%s'.\n", filters);
    Tcl_Free ((char*) refv);
    Tcl_Free ((char*) fltv);
    return TCL_ERROR;
  }

  /* Sanity checks */
  if (filters && fltN != refN) {
    ERROR ("Number of inputs (%d) != number of filters (%d).\n", refN, fltN);
    Tcl_Free ((char*) refv);
    Tcl_Free ((char*) fltv);
    return TCL_ERROR;
  }

  for (i = 0; i < refN; i++) {
    Feature* f = featureGet (FS, refv[i], 0);

    /* Feature type */
    if (!f || f->type != FE_SVECTOR) {
      ERROR ("Cannot find feature '%s' or it's not an SVector.\n", refv[i]);
      Tcl_Free ((char*) refv);
      if (filters) Tcl_Free ((char*) fltv);
      return TCL_ERROR;
    }

    /* Length of SVector */
    if (!source)
      source = f, sampleN = f->data.svec->n;
    if (sampleN != f->data.svec->n) {
      ERROR ("Feature '%s' has %d samples, not %d.\n", refv[i], f->data.svec->n, sampleN);
      Tcl_Free ((char*) refv);
      if (filters) Tcl_Free ((char*) fltv);
      return TCL_ERROR;
    }

    /* Filter */
    if (filters && !itfGetObject (fltv[i], "Filter")) {
      ERROR ("Cannot find filter '%s'.\n", fltv[i]);
      Tcl_Free ((char*) refv);
      if (filters) Tcl_Free ((char*) fltv);
      return TCL_ERROR;
    }
  }

  /* Create output vector */
  if (sampleN-2*ignoreN <= 0 || !(vec = svectorCreate (sampleN))) {
    ERROR ("Problem when creating output feature of size '%d - 2*%d'.\n", sampleN, ignoreN);
    Tcl_Free ((char*) refv);
    if (filters) Tcl_Free ((char*) fltv);
    return TCL_ERROR;
  }

  /* Do actual work */
  gain = (normal) ? gain / refN : gain;
  svA = (short**) malloc (refN * sizeof (SVector*));
  for (i = 0; i < refN; i++)
    svA[i] = (featureGet (FS, refv[i], 0))->data.svec->vecA;

  for (i = sampleN-1; i >= 0; i--) {
    double s = 0;
    int    j;

    for (j = 0; j < refN; j++)
      s += svA[j][i];
    s *= gain;

    if      (s >  32765) s =  32765;
    else if (s < -32765) s = -32765;
    vec->vecA[i] = (short) s;
  }
  free (svA);

  /* Create output feature */
  if (feSVector (FS, destin, vec, 1)) {
    ERROR ("Problem when creating feature '%s'.\n", destin);
    svectorFree (vec);
    Tcl_Free ((char*) refv);
    if (filters) Tcl_Free ((char*) fltv);
    return TCL_ERROR;
  }

  destin->samplingRate = source->samplingRate;
  destin->shift        = source->shift;

  Tcl_Free ((char*) refv);
  if (filters) Tcl_Free ((char*) fltv);
  return TCL_OK;
}

static int fesConvItf( ClientData cd, int argc, char *argv[])
{
/*  FeatureSet  *FS = (FeatureSet*)cd; */
  Feature     *source = NULL;
  FMatrix     *H  = NULL;
  float       gain = 1.0;
   
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<source_feature>", ARGV_CUSTOM, getFe,    &source, cd, SRC_FEAT,
      "<fmatrix>",  ARGV_CUSTOM, getFMatrixP,    &H,      cd, "impulse resp",
      "-gain",      ARGV_FLOAT,  NULL,           &gain,   cd, "gain",
      NULL) != TCL_OK) return TCL_ERROR;

  if (! IS_SVECTOR( source)) {
     MUST_SVECTOR( source);
     return TCL_ERROR;
  } else {
     SVector *vec    = source->data.svec;
     int samplesN = (int)(vec->n);
     int impulseN = H->n - 256;
     int samplesX, i;

     for (samplesX = samplesN-1; samplesX >= 0; samplesX--) {
        float sum = 0;
        for (i = 0; i < impulseN; i++) {
           if (samplesX-i < 0) break;

           sum += vec->vecA[samplesX-i] * H->matPA[0][i];
        }
        sum *= gain;
        if (sum > 32765) sum = 32765;
        else if (sum < -32765) sum = -32765;
        vec->vecA[samplesX] = (short) sum;
     }
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    auto correlation: see  Rabiner pp. 162
   ------------------------------------------------------------------------ */
	   
static int fesAutoCorItf( ClientData cd, int argc, char *argv[] )
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin  = NULL;
  Feature     *source = NULL;
  char        string[20];
  char        *cwin = "16msec", *cshift = string;
  int         coeffN, frameX, frameN, frameA, winPoints, shiftPoints;
  float       samplingRate = 0;
  FMatrix     *mat =NULL;
  SVector     *vec =NULL;
  float       *inA =NULL, *tmpA=NULL, *tmpB=NULL;
  sprintf(string,"%fmsec",FS->sfshift);

  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>",        ARGV_CUSTOM, createFe, &destin, cd, NEW_FEAT,
      "<source_feature>", ARGV_CUSTOM, getFe,    &source, cd, SRC_FEAT,
      "<coeffN>",         ARGV_INT,    NULL,     &coeffN, cd, "coeffN",
      "<win>",            ARGV_STRING, NULL,     &cwin,   cd, "window size",
      "-shift",           ARGV_STRING, NULL,     &cshift, cd, "shift",    
      NULL) != TCL_OK) return TCL_ERROR;

  samplingRate = source->samplingRate;

  if (! IS_SVECTOR( source)) {
     MUST_SVECTOR( source);
     return TCL_ERROR;
  }

  /*including R[0]*/
  coeffN++;

  getSampleX(FS,source,cwin,  &winPoints);
  getSampleX(FS,source,cshift,&shiftPoints);
  vec    = source->data.svec;
  frameN = (int)((vec->n - (winPoints - shiftPoints)) / shiftPoints);
  frameA = 0;

  assert (coeffN > 0 && coeffN< winPoints);

  inA  = (float*)malloc((unsigned)(winPoints*sizeof(float)));
  tmpA = (float*)malloc((unsigned)(winPoints*sizeof(float)));
  tmpB = (float*)malloc((unsigned)(winPoints*sizeof(float)));
  mat  = fmatrixCreate(frameN, coeffN);

  assert( inA != NULL && mat !=NULL && tmpA != NULL && tmpB != NULL);
  
  foreach_frame {
    do_hamm_window( vec->vecA + (frameX * shiftPoints), winPoints, inA);
    fesAutoCor( inA, mat->matPA[frameX],tmpA,tmpB, winPoints, coeffN);
  }
  
  free(inA);
  free(tmpA);
  free(tmpB);

  if(feFMatrix( FS, destin, mat, 1)) {
    fmatrixFree(mat); return TCL_ERROR;
  }
  destin->samplingRate = samplingRate;
  destin->shift        = shiftPoints / samplingRate;

  return TCL_OK;
}        

/* ------------------------------------------------------------------------
    lps using durbin: see  Rabiner pp. 162
   ------------------------------------------------------------------------ */
	   
static int fesLPCItf( ClientData cd, int argc, char *argv[] )
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin  = NULL;
  Feature     *source = NULL;
  int         a0 =1;
  int         i,order, dim, frameX, frameN, frameA;
  FMatrix     *matS =NULL, *matD= NULL, *tmpA=NULL;
  float       *tmpE =NULL, *tmpK=NULL;

  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>",        ARGV_CUSTOM, createFe, &destin, cd, NEW_FEAT,
      "<source_feature>", ARGV_CUSTOM, getFe,    &source, cd, SRC_FEAT,
      "<order>",          ARGV_INT,    NULL,     &order,  cd, "order",
      "-a0",              ARGV_INT,    NULL,     &a0,     cd, "include a0",
      NULL) != TCL_OK) return TCL_ERROR;

  if (! IS_FMATRIX( source)) {
     MUST_FMATRIX( source);
     return TCL_ERROR;
  }
  
  matS   = source->data.fmat;
  frameN = matS->m;
  dim    = matS->n;
  frameA = 0;

  assert (order > 0 && order < dim);
  
  if (a0) {
    matD  = fmatrixCreate(frameN, order+1);
  } else {
    matD  = fmatrixCreate(frameN, order);
  }

  tmpA = fmatrixCreate(order+1,order+1);
  tmpE = (float*)malloc((unsigned)((order+1)*sizeof(float)));
  tmpK = (float*)malloc((unsigned)((order+1)*sizeof(float)));
  assert (tmpA != NULL && tmpE != NULL && tmpK != NULL);

  foreach_frame {
    fesLPC(matS->matPA[frameX],tmpA->matPA,tmpE,tmpK,order+1);

    if (a0) {
      matD->matPA[frameX][0]=1.0;
      for (i=1;i<=order;i++)
	matD->matPA[frameX][i]= -1.0*tmpA->matPA[i][order];
    } else {
      for (i=0;i<order;i++)
	matD->matPA[frameX][i]= -1.0*tmpA->matPA[i+1][order];
    }
  }

  free(tmpE);
  free(tmpK);
  fmatrixFree(tmpA);

  if(feFMatrix( FS, destin, matD, 1)) {
    fmatrixFree(matD); return TCL_ERROR;
  }

  destin->samplingRate = source->samplingRate;
  destin->shift        = source->shift;

  return TCL_OK;
}        

/*----------------------------------------------------------------
   Feature Warping based on CDF matching; implemented by Qin Jin
------------------------------------------------------------------*/
static void QWarp (FMatrix *dest, FMatrix *source, int frameN, int dimN, int WinSize)
{
  int         i, j, dimX, frameX, index, rank;
  float       val, MIN, PhiX;
  int         WW = (int) (WinSize / 2);

  float Phi[61] = {0.0013, 0.0019, 0.0026, 0.0035, 0.0047, 0.0062, 0.0082, 0.0107, 0.0139, 0.0179,
		   0.0228, 0.0287, 0.0359, 0.0446, 0.0548, 0.0668, 0.0808, 0.0968, 0.1151, 0.1357,
		   0.1587, 0.1841, 0.2119, 0.2420, 0.2743, 0.3085, 0.3446, 0.3821, 0.4207, 0.4602,
		   0.5000, 0.5398, 0.5793, 0.6179, 0.6554, 0.6915, 0.7257, 0.7580, 0.7881, 0.8159,
		   0.8413, 0.8643, 0.8849, 0.9032, 0.9192, 0.9332, 0.9452, 0.9554, 0.9641, 0.9713,
		   0.9772, 0.9821, 0.9861, 0.9893, 0.9918, 0.9938, 0.9953, 0.9965, 0.9974, 0.9981, 0.9987};

  float XX[61] = {-3.0, -2.9, -2.8, -2.7, -2.6, -2.5, -2.4, -2.3, -2.2, -2.1, -2.0,
		  -1.9, -1.8, -1.7, -1.6, -1.5, -1.4, -1.3, -1.2, -1.1, -1.0,
		  -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1, 0.0,
		  0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0,
		  1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0,
		  2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9, 3.0};

  for (frameX=0; frameX<frameN; frameX++) {
    for (dimX=0; dimX<dimN; dimX++) {
      rank = 1;
      for (j=1; j<WW; j++) {
	if (frameX - j >=0 ) {
	  if (source->matPA[frameX][dimX] >= source->matPA[frameX-j][dimX])
	    rank ++;
	} else {
	  if (source->matPA[frameX][dimX] >=0 )
	    rank ++;
	}
	if (frameX + j < frameN ) {
	  if (source->matPA[frameX][dimX] >= source->matPA[frameX+j][dimX])
	    rank ++;
	} else {
	  if (source->matPA[frameX][dimX] >=0 )
	    rank ++;
	}
      }
      PhiX = (rank - 0.5) / WinSize;
      MIN = 1e20;
      index = 0;
      for (i=0; i<61; i++) {
	if ( PhiX > Phi[i]) 
	  val = PhiX - Phi[i];
	else 
	  val = Phi[i] - PhiX;
	if (val < MIN) {
	  MIN = val;
	  index = i;
	}
      }
      dest->matPA[frameX][dimX] = XX[index];
    }
  }
}

static int fesQWarpItf( ClientData cd, int argc, char *argv[] )
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin  = NULL;
  Feature     *source = NULL;
  int         WinSize = 300;
  int         DIM, frameN;
  FMatrix     *matS =NULL, *matD= NULL;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>",        ARGV_CUSTOM, createFe, &destin, cd, NEW_FEAT,
      "<source_feature>", ARGV_CUSTOM, getFe,    &source, cd, SRC_FEAT,
      "-WinSize",         ARGV_INT,    NULL,     &WinSize,  cd, "window size",
      NULL) != TCL_OK) return TCL_ERROR;

  if (! IS_FMATRIX( source)) {
     MUST_FMATRIX( source);
     return TCL_ERROR;
  }

  matS   = source->data.fmat;
  frameN = matS->m;
  DIM    = matS->n;
  matD  = fmatrixCreate(frameN, DIM);

  QWarp(matD, matS, frameN, DIM, WinSize);

  if(feFMatrix( FS, destin, matD, 1)) {
    fmatrixFree(matD); return TCL_ERROR;
  }

  destin->samplingRate = source->samplingRate;
  destin->shift        = source->shift;

  return TCL_OK;
}        


/* ------------------------------------------------------------------------
    formants
   ------------------------------------------------------------------------ */
	   
static int fesFormantsItf( ClientData cd, int argc, char *argv[] )
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin  = NULL;
  Feature     *source = NULL;
  float fMin = 200;
  float fMax = 7500;
  float bMax = 3500;
  int   fN   = 5;
  int         dim, frameX, frameN, frameA;
  FMatrix     *matS =NULL, *matD= NULL;

  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>",        ARGV_CUSTOM, createFe, &destin, cd, NEW_FEAT,
      "<source_feature>", ARGV_CUSTOM, getFe,    &source, cd, SRC_FEAT,
      "-N",               ARGV_INT,    NULL,     &fN,     cd, "max. number of formants",
      "-fMin",            ARGV_FLOAT,  NULL,     &fMin,   cd, "min. formant frequency",
      "-fMax",            ARGV_FLOAT,  NULL,     &fMax,   cd, "max. formant frequency",
      "-bMax",            ARGV_FLOAT,  NULL,     &bMax,   cd, "max. formant bandwidth",
      NULL) != TCL_OK) return TCL_ERROR;

  if (! IS_FMATRIX( source)) {
     MUST_FMATRIX( source);
     return TCL_ERROR;
  }
  
  matS   = source->data.fmat;
  frameN = matS->m;
  dim    = matS->n;
  frameA = 0;

  matD  = fmatrixCreate(frameN, fN);

  foreach_frame {
    fesFormants(matS->matPA[frameX],matD->matPA[frameX],dim,source->samplingRate*1000,fN,fMin,fMax,bMax);
  }
    
  if(feFMatrix( FS, destin, matD, 1)) {
    fmatrixFree(matD); return TCL_ERROR;
  }
  destin->samplingRate = source->samplingRate;
  destin->shift        = source->shift;

  return TCL_OK;
}        

/* ------------------------------------------------------------------------
   Method 'delta'
   ------------------------------------------------------------------------ */
static int fesDistanceItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin = NULL, *source = NULL;
  char        *cdelta = "1";
  int         delta;
    
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>",        ARGV_CUSTOM, createFe,&destin, cd, NEW_FEAT,
      "<source_feature>", ARGV_CUSTOM, getFe,   &source, cd, SRC_FEAT,
      NULL) != TCL_OK) return TCL_ERROR;

  if (getSampleX(FS,source,cdelta,&delta)) return TCL_ERROR;

  /* -------------------
     FLOAT features
     ------------------- */
  if (IS_FMATRIX( source)) {
     FMatrix      *mat1 = source->data.fmat;
     FMatrix      *mat;

     int frameX, frameN = mat1->m;
     int coeffX, coeffN = mat1->n;

     mat = fmatrixCreate(frameN,1);
     if (!mat) return TCL_ERROR;

     for (frameX=1;frameX<frameN;frameX++) {
       float sum=0;
       for (coeffX=0; coeffX<coeffN; coeffX++) {
	 float delta =  mat1->matPA[frameX][coeffX] - mat1->matPA[frameX-1][coeffX];
	 sum += delta*delta;
       }
       mat->matPA[frameX][0]=sum/coeffN;
     }
     mat->matPA[0][0]= 10.0;

     if (!feFMatrix( FS, destin, mat, 1)) {
       COPY_CONFIG( destin, source); return TCL_OK;
     }
     fmatrixFree(mat); return TCL_ERROR;
  }	 
  
  return TCL_ERROR;
}


/* ------------------------------------------------------------------------
   Method 'silTK'
   Thomas Kemp's silence feature
   ------------------------------------------------------------------------ */
/*=========================================================================
; sort_array;
;  IN:
;	FA		: array with floats
;	n		: number of frames in FA
;
;  RETURN:		1, if successful, 0 else
;-------------------------------------------------------------------------- */
static int compare_floats(const void* x, const void* y)
{
  if (*(float *)x > *(float *)y)  return(1);
  if (*(float *)x == *(float *)y) return(0);
  return(-1);
}

static float sort_Farray(float* FA, int n)
{
  int    (*f)(const void *,const void*) = compare_floats;
  qsort(FA, n, sizeof(float), f);		/* sort the array		*/

  return 1;
}

static int fesSilTKItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin = NULL, *power = NULL, *ptp = NULL;
  float PEAK_THRESHOLD = 1100/(float)ADCRANGE;
  float LOW_TH  = 3.;
  int   NUMFRAM = 30;
  int   NUMSIL2 = 3;
  int   NUMSIL3 = 9;
  char  *minPow = NULL, *maxPow = NULL;
   
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>", ARGV_CUSTOM, createFe,&destin, cd, NEW_FEAT,
      "<power>",   ARGV_CUSTOM, getFe,   &power,  cd, "name of power feature",
      "<ptp>",     ARGV_CUSTOM, getFe,   &ptp,    cd, "name of ptp feature",
      "-minPower", ARGV_STRING,    NULL, &minPow, cd, "mean of the most silent frames",
      "-maxPower", ARGV_STRING, NULL,    &maxPow, cd, "mean of loudest frames",
      NULL) != TCL_OK) return TCL_ERROR;

  /* -------------------
     FLOAT features
     ------------------- */
  if (BOTH_FMATRIX( power, ptp)) {
     FMatrix      *powMat = power->data.fmat;
     FMatrix      *ptpMat = ptp->data.fmat;
     int           frameA = 0;
     int frameX,  frameN  = powMat->m;
     int len = 0, index, i;
     float *sort;
     float lowmean=0., highmean=0.;

     /* ------------------------------------------ */
     /* The result matrix is initialized with zero */
     /* ------------------------------------------ */
     FMatrix      *mat    = fmatrixCreate(frameN,1);
     if (!mat) return TCL_ERROR;

     /* ------------------------------------
	 calculate the low energy threshold
	------------------------------------ */
     if (frameN <= NUMFRAM){
       WARN("SILENCE: only %d frames available, silence feature padded with zero (=no silence)\n", frameN);
     }
     else {
       sort=(float *) malloc(sizeof(float) * frameN);
       foreach_frame sort[frameX] = powMat->matPA[frameX][0];
       sort_Farray(sort, frameN);
       for (i=0; i<NUMFRAM; i++) lowmean  += sort[i];
       for (i=0; i<NUMFRAM; i++) highmean += sort[frameN-1-i];
       lowmean  /= (float)NUMFRAM;
       highmean /= (float)NUMFRAM;

       if (minPow != 0) { char pf[100]; sprintf(pf,"%e",lowmean); Tcl_SetVar( itf, minPow, pf, 0); }
       if (maxPow != 0) { char pf[100]; sprintf(pf,"%e",highmean); Tcl_SetVar( itf, maxPow, pf, 0); }
	 
       /* Message2(2,"\nLow energy mean %f, high energy mean %f. ",lowmean,highmean);  */
       free(sort);
       
       /* if low energy more than 1/15 of high energy, no silence: */
       if (lowmean*15.0 > highmean) {
	 WARN("SILENCE: No silence found (lowmean too big)\n");
       }
       else {
	 lowmean *= LOW_TH;
	 /* ----------------------------
	    main loop over all frames
	    ---------------------------- */
	 foreach_frame {
	   if ( ptpMat->matPA[frameX][0] < PEAK_THRESHOLD &&
	       powMat->matPA[frameX][0] < lowmean) { /* silence candidate */
	     mat->matPA[frameX][0] = 1;
	     index = frameX - (NUMSIL2-1)/2;
	     if (++len > NUMSIL2) mat->matPA[index][0] = 2;
	     if (len > NUMSIL3) {
	       index = frameX-(NUMSIL3-1)/2;
	       mat->matPA[index][0]=3;
	       if ( ptpMat->matPA[index][0] < PEAK_THRESHOLD/2 &&
		   powMat->matPA[index][0] < lowmean/2)
		 mat->matPA[index][0]=4;
	     }
	   }
	   else   len=0;
	 }
       }
     } /* if there are enough frames to compute the silence feature */
     
     if(!feFMatrix( FS, destin, mat, 0)) {
	COPY_CONFIG( destin, power);
	return TCL_OK;
     }
     fmatrixFree(mat); return TCL_ERROR;
  }	 
  ERROR("Feature type of power and ptp must be FMatrix for command '%s'.\n",argv[0]);  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
   Puls
   create Puls in Feature destin
   ------------------------------------------------------------------------ */
static int fesPulsItf( ClientData cd, int argc, char *argv[])
{
   FeatureSet  *FS     = (FeatureSet*)cd;
   Feature     *destin = NULL;
   char        *cfrom  = "0"; int from;
   char        *cto    = "9"; int to;
   int         val     = 1;

   /* ------------------
      get arguments
      ------------------ */
   argc--;
   if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>", ARGV_CUSTOM, getFe,&destin,  cd, NEW_FEAT,
      "<from>",    ARGV_STRING, NULL,   &cfrom, cd, "start in time format",
      "<to>",      ARGV_STRING, NULL,   &cto,   cd, "length in time format",
      "-value",    ARGV_INT,    NULL,   &val,   cd, "value of puls",
      NULL) != TCL_OK) return TCL_ERROR;

   if (getSampleX(FS,destin,cfrom,&from) || getSampleX(FS,destin,cto,&to))
      return TCL_ERROR;
   /* -------------------
      SHORT features      
      ------------------- */
   if (IS_SVECTOR( destin)) {
     SVector *vec    = destin->data.svec;
     int     sampleN = vec->n;
     int     sampleX;

     if (from>to || from<0) return TCL_ERROR;
     val = SLIMIT(val);
     for( sampleX=from; sampleX<sampleN && sampleX<=to; sampleX++)
       vec->vecA[sampleX] = val;
     return TCL_OK;
   }
   MUST_SVECTOR( destin);
   return TCL_ERROR;
}


/* ------------------------------------------------------------------------
   Tone
   ------------------------------------------------------------------------ */
#ifndef M_2PI
#define M_2PI   6.28318530717958647692
#endif
static int fesToneItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin = NULL;
  SVector     *inf = NULL, *sound = NULL;
  int         soundN = 0;
  float       sr   = FS->samplingRate;
  int         N, i, j, total = 0;
  int         attack = 0, decay = 0, release = 0;
  int         amF = 0, amA = 0;
  int         fmF = 0, fmA = 0;
  float       peak = 1.0;
  float       gain = 10000;
  
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>",        ARGV_CUSTOM, createFe,     &destin, cd, NEW_FEAT,
      "<vector>",         ARGV_CUSTOM, getSVector,   &inf,    cd, "vector with \"<length_ms> <pitch_Hz> ...\"",
      "-g",               ARGV_FLOAT,  NULL,         &gain,   cd, "gain",
      "-sr",              ARGV_FLOAT,  NULL,         &sr,     cd, "sampling rate in kHz",
      "-attack",          ARGV_INT,    NULL,         &attack, cd, "attack time in ms",
      "-peak",            ARGV_FLOAT,  NULL,         &peak,   cd, "relative peak",
      "-decay",           ARGV_INT,    NULL,         &decay,  cd, "decay time in ms",
      "-release",         ARGV_INT,    NULL,         &release,cd, "release time in ms",
      "-amA",             ARGV_INT,    NULL,         &amA,    cd, "AM amplitude in %%",
      "-amF",             ARGV_INT,    NULL,         &amF,    cd, "AM frequency in Hz",
      "-fmA",             ARGV_INT,    NULL,         &fmA,    cd, "FM frequency shift in 0.01%%",
      "-fmF",             ARGV_INT,    NULL,         &fmF,    cd, "FM frequency in Hz",
      "-sound",           ARGV_CUSTOM, getSVector,   &sound,  cd, "sound",
      NULL) != TCL_OK) return TCL_ERROR;

  if (sound) soundN = sound->n;
  N       = inf->n / 2;
  attack  *= sr; 
  release *= sr;
  decay   *= sr;
  for (i=0;i<N;i++) total += inf->vecA[2*i];

  /* -------------------
     
     ------------------- */
  {
     int sampleX = 0, sampleN = sr * total;
     int dur, pitch, k;
     double t, twopif, twopifmF, twopiamF, dt = 0.001/sr;
     double sum;
     SVector     *vec = svectorCreate(sampleN);  if (!vec) return TCL_ERROR;

     for(i=0;i<N;i++) {
	dur     = inf->vecA[2*i];
	pitch   = inf->vecA[2*i+1];
	twopif  = M_2PI * pitch;
	twopiamF= M_2PI * amF;
	twopifmF= M_2PI * fmF;
	sampleN = sr * dur;
	t       = 0.0;
	for(j=0;j<sampleN;j++) {
	   if (fmA) twopif  = M_2PI * pitch * (1.0 + 0.0001 * fmA * sin(twopifmF * t));;
	   if (soundN) {
	      sum = 0;
	      for(k=1;k<=soundN;k++)
		 sum += 0.01 * sound->vecA[k-1] * sin(k * twopif * t);
	   }
	   else sum = sin(twopif * t);
	   sum *= gain * (1.0 + 0.01 * amA * sin(twopiamF * t));

	   if      (j<attack)             sum *= (peak*j/(double)attack);
	   else
	      if ((k = j-attack) < decay) sum *= (peak + (1.0-peak)*k/(double)decay);
	   if      (j>(sampleN-release))  sum *= ((sampleN-j)/(double)release);
	   vec->vecA[sampleX++] = SLIMIT(sum);
	   t += dt;
	}
     }
     
     if(!feSVector( FS, destin, vec, 1)) {
	destin->samplingRate = sr;
	return TCL_OK;
     }
     svectorFree(vec);
     return TCL_ERROR;
  }
}



/* ============================================================================
   Noise
;  ========================================================================= */
/* ----------------------------------------------------------------------------
;  ran1
;
;  "Minimal" random number generator of Park and Miller with Bays-Durham
;  shuffle and added safeguards. Returns a uniform random deviate between 0.0
;  and 1.0 (exclusive of the endpoint values). Call with idum a negative
;  integer to initialize; thereafter, do not alter idum between succesive
;  deviates in a sequence. RNMX should be approximate the largest floating
;  value that is less than 1.
;  ------------------------------------------------------------------------- */
#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

static float ran1(long *idum)
{
   int j;
   long k;
   static long iy = 0;
   static long iv[NTAB];
   float temp;

   if (*idum <= 0  ||  !iy) {
      if (-(*idum) < 1) *idum = 1;
      else *idum = -(*idum);
      for (j=NTAB+7; j>=0; j--) {
	 k     = (*idum) / IQ;
	 *idum = IA * (*idum - k * IQ) - IR * k;
	 if (*idum < 0) *idum += IM;
	 if (j < NTAB)  iv[j]  = *idum;
      }
      iy = iv[0];
   }
   k = (*idum) / IQ;
   *idum = IA * (*idum - k * IQ) - IR * k;
   if (*idum < 0) *idum += IM;
   j = iy / NDIV;
   iy = iv[j];
   iv[j] = *idum;
   if ((temp = AM * iy) > RNMX) return RNMX;
   else return temp;
   
}

/* ----------------------------------------------------------------------------
;  uniformnoise()
;
;  Returns a uniform distributed deviate with zero mean and unit variance
;  ------------------------------------------------------------------------- */
static float uniformnoise(long *idum)
{
   static double fac = 3.4641016151377544;  /* sqrt(12) */
   return (ran1(idum) - 0.5) * fac;
}

/* ----------------------------------------------------------------------------
;  gaussnoise()
;
;  Returns a normally distributed deviate with zero mean and unit variance
;  ------------------------------------------------------------------------- */
static float gaussnoise(long *idum)
{
   static int iset = 0;
   static float gset;
   float fac, rsq, v1, v2;

   if (iset == 0) {
      do {
	 v1  = 2.0 * ran1(idum) - 1.0;
	 v2  = 2.0 * ran1(idum) - 1.0;
	 rsq = v1 * v1  +  v2 * v2;
      } while (rsq >= 1.0  ||  rsq == 0.0);
      fac = sqrt(-2.0 * log((double)rsq) / rsq);
      gset = v1 * fac;
      iset = 1;
      return v2 * fac;
   } else {
      iset = 0;
      return gset;
   }
}

static int fesNoiseItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS     = (FeatureSet*)cd;
  Feature     *destin = NULL;
  char        *ctype  = "normal";
  char        *cN     = NULL;  int sampleN;
  float       samplRate = FS->samplingRate;
  float       mean = 0, dev = 1000;
  int         dim  = 0;
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>",ARGV_CUSTOM, createFe,&destin, cd, NEW_FEAT,
      "<length>", ARGV_STRING, NULL, &cN,        cd, "length in time format",
      "-type",    ARGV_STRING, NULL, &ctype,     cd,
		                       "\"uniform\" or \"normal\" distribution",
      "-sr",      ARGV_FLOAT,  NULL, &samplRate, cd, "sampling rate in kHz",
      "-mean",    ARGV_FLOAT,  NULL, &mean,      cd, "mean value",
      "-dev",     ARGV_FLOAT,  NULL, &dev,       cd, "deviation value",
      "-dim",     ARGV_INT,    NULL, &dim,       cd, "0 to create a SVector noise feature, >0 to create a FMatrix noise feature with <dim> channels, (in this case length must be in frames!)",
      NULL) != TCL_OK) return TCL_ERROR;

  if (dim <= 0) {
    if (getSample2X(cN,samplRate,&sampleN)) return TCL_ERROR;
  }
  else if ( sscanf(cN,"%d",&sampleN) != 1) {
    ERROR("Give <length> in frames not: %s\n",cN);
    return TCL_ERROR;
  }
  
  /* -------------------
     SHORT features
     ------------------- */
  if (dim <= 0) {
    int     sampleX;
    long    idum = -1;
    SVector *vec = svectorCreate(sampleN);  if (!vec) return TCL_ERROR;
    
    Switch( ctype)
      Case("uniform") {
	for( sampleX=0; sampleX<sampleN; sampleX++) {
	  float val = dev * uniformnoise(&idum) + mean;
	  vec->vecA[sampleX] = SLIMIT(val);
	}
      }
      Case("normal") {
	for( sampleX=0; sampleX<sampleN; sampleX++) {
	  float val = dev * gaussnoise(&idum) + mean;
	  vec->vecA[sampleX] = SLIMIT(val);
	}
      }
      Default {
	ERROR("Don't know noise type %s.\n", ctype);
	svectorFree(vec); return TCL_ERROR;
      }
    End_switch;
    if(!feSVector( FS, destin, vec, 1)) {
      return TCL_OK;
    }
    svectorFree(vec); return TCL_ERROR;
  }
  /* -------------------
     FLOAT features
     ------------------- */
  else {
    int          frameA = 0;
    int  frameX, frameN = sampleN;
    int  coeffX, coeffN = dim;
    long idum = -1;
    FMatrix  *mat = fmatrixCreate(frameN,coeffN);  if (!mat) return TCL_ERROR;

    Switch( ctype)
      Case("uniform") {
	foreach_coeff
	  mat->matPA[frameX][coeffX] = dev * uniformnoise(&idum) + mean;
      }
      Case("normal") {
	foreach_coeff
	  mat->matPA[frameX][coeffX] = dev * gaussnoise(&idum) + mean;
      }
      Default {
	ERROR("Don't know noise type %s.\n", ctype);
	fmatrixFree(mat); return TCL_ERROR;
      }
    End_switch;
    if(!feFMatrix( FS, destin, mat, 1)) {
      return TCL_OK;
    }
    fmatrixFree(mat); return TCL_ERROR;
  }
}


/* ------------------------------------------------------------------------
   Method 'melscale'
   ------------------------------------------------------------------------ */
static int fesMelItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin = NULL, *source = NULL;
   
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>",        ARGV_CUSTOM, createFe,&destin,   cd, NEW_FEAT,
      "<source_feature>", ARGV_CUSTOM, getFe,   &source,   cd, SRC_FEAT,
      NULL) != TCL_OK) return TCL_ERROR;

  if ((source->samplingRate!=8) && (source->samplingRate!=16)) {
     ERROR("Sampling rate must be 8 or 16 kHz for command '%s'.\n",argv[0]);
     return TCL_ERROR;
  }
  /* -------------------
     FLOAT features
     ------------------- */
  if (IS_FMATRIX( source)) {
     FMatrix      *mat1 = source->data.fmat;
     int         frameA = 0;
     int frameX, frameN = mat1->m;
     int         coeffN = 16;
     int         scale8 = (source->samplingRate==8);
     FMatrix *mat = fmatrixCreate(frameN,coeffN); if (!mat) return TCL_ERROR;
      
     foreach_frame {      
	if(! compute_melscale_from_power(mat->matPA[frameX], mat->n, mat1->matPA[frameX], mat1->n, scale8)) {
	   fmatrixFree(mat); 
	   ERROR("Can't calculate melscales.\n",argv[0]);  return TCL_ERROR;
	}
     }
     
     if(!feFMatrix( FS, destin, mat, 1)) {
	COPY_CONFIG( destin, source);
	return TCL_OK;
     }
     fmatrixFree(mat); return TCL_ERROR;
  }	 
  MUST_FMATRIX(source); return TCL_ERROR;
}


/* ------------------------------------------------------------------------
   feMinmax
   give minimum and maximum of a feature
   ------------------------------------------------------------------------ */
static int feMinmax( Feature* F, float *min, float *max, int from, int to)
{
   int ret = 0;
   
   if (IS_FMATRIX( F)) {
     ret = fmatrixMinmaxft( F->data.fmat, min, max, from, to);
   }
   else if (IS_SVECTOR( F)) {
     short smin, smax;
      
     if ((ret = svectorMinmaxft( F->data.svec, &smin, &smax, from, to))  > 0)
         *min = (float)smin; *max = (float)smax;
   }
   return ret;
}


/* ------------------------------------------------------------------------
   Method 'normalize'
   normalize coefficients of feature so that new range is <min> ... <max>
   ------------------------------------------------------------------------ */
static int fesNormItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin = NULL, *source = NULL;
  float       min = 0.0, max = 1.0;
  double      factor, add, range;
  float       xmin, xmax, xrange;
  
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<feature>",        ARGV_CUSTOM, createFe,&destin,   cd, NEW_FEAT,
      "<source_feature>", ARGV_CUSTOM, getFe,   &source,   cd, SRC_FEAT,
      "-min",             ARGV_FLOAT,  NULL,    &min,      cd, "",
      "-max",             ARGV_FLOAT,  NULL,    &max,      cd, "",
      NULL) != TCL_OK) return TCL_ERROR;
  /* --- target range and min --- */
  range = max - min;
  if (range <= 0) {
     ERROR("Given range is not ok: min = %f, max = %f.\n", min, max);
     return TCL_ERROR;
  }
  
  /* --- old range and min --- */
  if (feMinmax( source, &xmin, &xmax, 0, -1) <= 0) return TCL_ERROR;
  xrange = xmax - xmin;
  if (xrange <= 0.0) {
    /* ERROR("values all = %f",xmax); return TCL_ERROR; */
    factor = 1;
  } else {
  /*------------------------------------------------------------
  ;  y = ((x - xmin)/xrange) * yrange + ymin
  ;    =   x * (yrange/xrange)  - xmin*yrange/xrange + ymin
  ;    =   x * factor           - xmin*factor + ymin
  ;-----------------------------------------------------------*/
    factor = range/xrange;
  }
  add    = min - xmin*factor;

  if (FE_DEB) fprintf(STDERR,"Wanted: min= %f, range= %f, Old: min= %f, range= %f\n", min, range, xmin, xrange);
  /* -------------------
     FLOAT features
     ------------------- */
  if (IS_FMATRIX( source)) {
     FMatrix      *mat1 = source->data.fmat;
     int         frameA = 0;
     int frameX, frameN = mat1->m;
     int coeffX, coeffN = mat1->n;
     FMatrix       *mat = fmatrixCreate(frameN,coeffN);  if (!mat) return TCL_ERROR;
      
     foreach_coeff
	mat->matPA[frameX][coeffX] = mat1->matPA[frameX][coeffX] * factor + add;

     if(!feFMatrix( FS, destin, mat, 0)) {
	COPY_CONFIG( destin, source); return TCL_OK;
     }
     fmatrixFree(mat); return TCL_ERROR;
  }	 
  /* ------------------
     SHORT features
     ------------------ */
  else if (IS_SVECTOR( source)) {
     SVector      *vec1 = source->data.svec;
     int         frameA = 0;
     int frameX, frameN = vec1->n;
     SVector      *vec  = svectorCreate(frameN);  if (!vec) return TCL_ERROR;
      
     foreach_frame
	vec->vecA[frameX] = SLIMIT(ROUND(vec1->vecA[frameX] * factor + add));

     feSVector( FS, destin, vec, 0);
     COPY_CONFIG( destin, source); return TCL_OK;
  }
  DONT_KNOW(source); return TCL_ERROR;
}



/* ------------------------------------------------------------------------
   Method 'concat'
   concat frames (samples) of features together
   ------------------------------------------------------------------------ */
static int fesConcatItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet* FS = (FeatureSet*)cd;
  Feature     *destin, **source;
  FeatureType type = FE_UNDEF;
  int         sourceN  = argc - 2;
  int         frameN = 0;
  int         coeffN = -1;
  int         i;
    
  /* ------------------
     get arguments
     ------------------ */
  if (argc-- < 3) {
     ERROR("Usage: %s <new_feature> <feature1> [<feature2> ...].\n", *argv);
     return TCL_ERROR;
  }

  if (createFe( cd, "",(ClientData)&destin, &argc, ++argv)  <  0) return TCL_ERROR;

  source = (Feature**)malloc((unsigned)(sourceN*sizeof(Feature*)));
  for ( i=0; i<sourceN; i++) {

     /* --- get feature --- */
     if (FE_VER) fprintf(STDERR,"%d: source %s\n",argc,argv[1]);
     if ( getFe( cd, "", (ClientData)(source + i), &argc, ++argv) < 0) {
       free(source); return TCL_ERROR;
     }

     /* --- get/ set properties, skipping empty features --- */
     if ( source[i]->data.fmat == NULL ) continue;

     if ( coeffN == -1 ) {
	type   = source[i]->type;
	coeffN = source[i]->data.fmat->n;
     }
     else if (type != source[i]->type) {
	ERROR("feature '%s' has different type.\n", *argv);
	free(source); return TCL_ERROR;
     }
     switch ( type) {
       case ( FE_SVECTOR) :
	  frameN += source[i]->data.svec->n; break;
       case ( FE_FMATRIX) :
	  if (coeffN != source[i]->data.fmat->n) {
	     ERROR("feature '%s' has different coefficient number %d (!= %d).\n",
		   *argv, source[i]->data.fmat->n,coeffN);
	     free(source); return TCL_ERROR;
	  }
	  frameN += source[i]->data.fmat->m; break;
       default :
          ERROR("not a method for type of feature '%s'.\n",*argv);
          free(source); return TCL_ERROR;
    }
  }

  /* -------------------
     concat features
     ------------------- */
  switch ( type) {
    case ( FE_SVECTOR) : {
       SVector *vec = svectorCreate(frameN);
       int frameI = 0;
     
       for ( i=0; i<sourceN; i++) {
	  SVector *vec1 = source[i]->data.svec;
	  if (vec1 && vec1->n) {
	    memcpy(vec->vecA + frameI, vec1->vecA, vec1->n * sizeof(short));
	    frameI += vec1->n;
	  }
       }
       if(feSVector( FS,  destin, vec, 0)) {
	  svectorFree(vec); return TCL_ERROR;
       }
    } break;
    case ( FE_FMATRIX) : {
      if ( source[0] == source[1] ) {
	/* append other features (> 1) to source[0] */
	FMatrix *mat    = source[0]->data.fmat;
	int      frameI = mat->m;

	fmatrixResize (mat, frameN, coeffN);

	for (i = 1; i < sourceN; i++) {
	  FMatrix *mat1 = source[i]->data.fmat;
	  if (coeffN * mat1->m) {
	    memcpy(mat->matPA[frameI], mat1->matPA[0], coeffN * mat1->m * sizeof(float));
	    frameI += mat1->m;
	  }
	}	  
      } else {
	/* copy features into new FMatrix */
	FMatrix *mat = fmatrixCreate(frameN,coeffN);
	int frameI = 0;
       
	for (i=0; i<sourceN; i++) {
	  FMatrix *mat1 = source[i]->data.fmat;
	  if (mat1 && coeffN * mat1->m) {
	    memcpy(mat->matPA[frameI], mat1->matPA[0], coeffN * mat1->m * sizeof(float));
	    frameI += mat1->m;
	  }
	}
	if(feFMatrix( FS,  destin, mat, 0)) {
	  fmatrixFree(mat); return TCL_ERROR;
	}
      }
    } break;
    default: break;
  }
  
  COPY_CONFIG( destin, source[0]);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
   Method 'merge'
   merge features together
   ------------------------------------------------------------------------ */
static int fesMergeItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet* FS = (FeatureSet*)cd;
  Feature     *destin, **source;
  int         sourceN;
  int         frameA = 0;
  int frameX, frameN = -1;
  int coeffX, coeffN = 0;
  int         i, i0;
  int         mode = 0;
    
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
      "<new_feature>", ARGV_CUSTOM, createFe,&destin, cd,  NEW_FEAT,
      "<names*>",      ARGV_REST,   NULL,    &i0,     NULL,"list of source features",
      /* "-mode",         ARGV_INT,    NULL,    &mode,   cd,  "mode",*/
      NULL) != TCL_OK) return TCL_ERROR;

  if (FS->runon && !FS->ready) mode = -1;
  sourceN = argc - i0;
  source = (Feature**)malloc((unsigned)(sourceN*sizeof(Feature*)));
  for ( i=0; i<sourceN; i++) {
     i0++;
     if (FE_VER) fprintf(STDERR,"%d: source %s\n",i,argv[i0]);
     if ((source[i] = featureGet(FS,argv[i0],0)) == NULL) { 
       free(source); return TCL_ERROR;
     }
     if (source[i]->type != source[0]->type)  {
       ERROR("Feature type of %s differs!\n",source[i]->name);
       free(source); return TCL_ERROR;
     }

     if (IS_FMATRIX(source[0])) {
       frameX = source[i]->data.fmat->m;
       coeffN += source[i]->data.fmat->n; 
     } else if (IS_SVECTOR(source[0])) {
       frameX = source[i]->data.svec->n;
     } else {
       ERROR("Feature %s has wrong type!\n",source[i]->name);
       free(source); return TCL_ERROR;
     }
     
     if (frameN == -1) frameN = frameX;
     else if (frameN != frameX) {
       if (mode<0)  frameN = MIN( frameN, frameX); 
       else {
	 ERROR("feature '%s' has different frame/sample number %d (!= %d).\n",
	       *argv, frameX, frameN);
	 free(source); return TCL_ERROR;
       }
     }
  }

  /* -------------------
     merge features
     ------------------- */
  if (IS_FMATRIX(source[0])) {
     FMatrix *mat = fmatrixCreate(frameN,coeffN);
     int coeffI = 0;
     
     for ( i=0; i<sourceN; i++) {
	FMatrix *mat1 = source[i]->data.fmat;
	
	coeffN = mat1->n;
	foreach_coeff
	   mat->matPA[frameX][coeffI+coeffX] = mat1->matPA[frameX][coeffX];
	coeffI += coeffN;
     }
     
     if(feFMatrix( FS,  destin, mat, 1)) {
	fmatrixFree(mat); return TCL_ERROR;
     }
  } else {
     SVector *vec = svectorCreate(frameN*sourceN);
     
     for ( i=0; i<sourceN; i++) {
	SVector *vec1 = source[i]->data.svec;
	
	foreach_frame
	   vec->vecA[frameX*sourceN + i] = vec1->vecA[frameX];
     }
     if(feSVector( FS, destin, vec, 1)) {
	svectorFree(vec); return TCL_ERROR;
     }
  }
  COPY_CONFIG( destin, source[0]);

  if (source) free(source);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
   Method 'compress'
   compress FMatrix into catrix
   ------------------------------------------------------------------------ */
static int fesCompressItf( ClientData cd, int argc, char *argv[])
{
  Feature     *dest, *source;
  CodebookSet* cbsP = NULL;
  float     errorRV = 0.0;
  int  cntUnderflow = 0.0;
  int   cntOverflow = 0.0;
  int         cntRV = 0;
  int       verbose = 0;
  int     trainMode = 0;
  float         **fmat = NULL;
  unsigned char **cmat = NULL;

  
  int i,j;
    
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
      "<new_feature>",    ARGV_CUSTOM, createFe,&dest, cd,  NEW_FEAT,
      "<source_feature>", ARGV_CUSTOM, getFe,   &source,   cd, SRC_FEAT,
      "codebookSet",      ARGV_OBJECT,  NULL,   &cbsP,   "CodebookSet", "will need a cbs after a couple of beers",
      "-verbose",         ARGV_INT,    NULL,    &verbose,   NULL, "verbose",
      "-trainMode",       ARGV_INT,    NULL,    &trainMode, NULL, "store compressed values in orginal feature",

      NULL) != TCL_OK) return TCL_ERROR;

  if (! IS_FMATRIX( source)) {
    MUST_FMATRIX( source);
    return TCL_ERROR;
  }

  dest->samplingRate = source->samplingRate;
  dest->shift        = source->shift;
  dest->type         = FE_FMATRIX;

  if (! trainMode) {
    dest->type         = FE_CMATRIX;
    if (dest->data.cmat) cmatrixFree(dest->data.cmat);
    dest->data.cmat    = cmatrixCreate(source->data.fmat->m,source->data.fmat->n);
    cmat= dest->data.cmat->matPA;
  }

  fmat= source->data.fmat->matPA;

  for (i=0;i<source->data.fmat->m;i++) {
    for (j=0;j<source->data.fmat->n;j++) {
      int  k = cbsP->x2y[j];
      long l = floor(cbsP->scaleRV*fmat[i][k]+cbsP->offset);
      float f2;
      if (l < 0)         {cntUnderflow++; l=0.0;}
      if (l > UCHAR_MAX) {cntOverflow++;  l=UCHAR_MAX;}

      f2= (1.0*l-cbsP->offset)/cbsP->scaleRV ;
      if (l > 0 && l < UCHAR_MAX) {
	errorRV += fabs(fmat[i][k] - f2 );
	cntRV++;
      }

      if (trainMode) 
	fmat[i][j] = (float) l;
      else 
	cmat[i][j]   = (unsigned char) l;
    
    }
  }
  if (cntRV >0) errorRV /= cntRV;

  if (verbose) {
    INFO("%d underflows after quantization\n",cntUnderflow);
    INFO("%d overflows  after quantization\n",cntOverflow);
    INFO("quantization error: %2.2f\n",errorRV);
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
   Method 'beepSeg' and 'silSeg'
   segment (spectral) feature at beeper positions
   (i.e. return time markers for beeper positions)
   example:
   score     ____11_____1_1111__1______1_1_______
   maxInt 1  ____11_____111111__1______111_______
   minDur 3  ___________111111_________111_______
   return              {11   16}      {26 28}
   ------------------------------------------------------------------------ */
static int fesSegItf( ClientData cd, int argc, char *argv[])
{
  /* FeatureSet* FS = (FeatureSet*)cd; */
  Feature     *feature;
  int         from   = 0;
  int         to     = -1;
  int         band   = 0;
  int         minDur = 10;
  int         maxInt = 0;
  float       thresh = 0.5;

  float       **mat;
  float       *score;
  float       s1 = 1.0, s2 = 0.0;
  int         frameX, state, start;
    
  if (streq( argv[0], "silSeg")) { s1 = 0.0; s2 = 1.0; }

  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
     "<feature>", ARGV_CUSTOM, getFe, &feature,cd, "(spectral) source feature",
     "-from",     ARGV_INT,    NULL,  &from,   cd, "starting frame", 
     "-to",       ARGV_INT,    NULL,  &to,     cd, "final frame",
     "-band",     ARGV_INT,    NULL,  &band,   cd, "index of frequency band", 
     "-thresh",   ARGV_FLOAT,  NULL,  &thresh, cd, "energy threshold value", 
     "-minDur",   ARGV_INT,    NULL,  &minDur, cd, "minimum duration", 
     "-maxInt",   ARGV_INT,    NULL,  &maxInt, cd, "maximum interruption",
     NULL) != TCL_OK) return TCL_ERROR;

  /* -------------------------------
     check sanity of given arguments
     ------------------------------- */

  if (feature->type != FE_FMATRIX) 
  { MUST_FMATRIX(feature); 
    return TCL_ERROR; }
  if (from<0 || (to<from && to!=(-1)) || to>=feature->data.fmat->m) 
  {  ERROR("illegal from/to value (%d,%d), must be within (%d,%d)\n",
           from,to,0,feature->data.fmat->m-1);
     return TCL_ERROR;
  }
  if (to==(-1)) to = feature->data.fmat->m-1;

  /* -----------------------------------------------------------
     run through feature and look for beeps or silence (beep=0)
     ----------------------------------------------------------- */
  /* 1: score */
  mat = feature->data.fmat->matPA;
  score  = (float*)malloc(feature->data.fmat->m*sizeof(float));
  for (frameX=from; frameX<=to; frameX++)
      score[frameX] = (mat[frameX][band] >= thresh) ? s1 : s2 ;

  /* 2: maxInt */
  state = 0; start = -1 - maxInt;
  for (frameX=from; frameX<=to; frameX++)
      if (score[frameX] != 0.0) {
	if (state == 0 && (frameX - start) <= maxInt) {
	  int i;
	  for (i=start; i<frameX; i++) score[i] = 1.0;
	}
	state = 1;
      } else {
	if (state == 1) start = frameX;
	state = 0; 
      }

  /* 3: minDur */
  state = 0; start = 0;
  for (frameX=from; frameX<=to; frameX++)
      if (score[frameX] != 0.0) {
	if (state == 0) start = frameX;
	state = 1;
      } else {
	if (state == 1 && (frameX - start) >= minDur) {
	  itfAppendElement("%d %d",start,frameX-1);
	}
	state = 0; 
      }
  if (state == 1 && (frameX - start) >= minDur)
    itfAppendElement("%d %d",start,to);

  free(score);

  return TCL_OK;
}

/* Calculate the variance of the speech spectrum */

static int fesVarSSItf( ClientData cd, int argc, char *argv[] )
{
  Feature     *source = NULL;
  int         i, dim, frameX, frameN, frameA;
  FMatrix     *matS = NULL;
  double *mean = NULL, *var = NULL;
  double varsum;

  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<source_feature>", ARGV_CUSTOM, getFe,    &source, cd, SRC_FEAT,
      NULL) != TCL_OK) return TCL_ERROR;

  if (! IS_FMATRIX( source)) {
     MUST_FMATRIX( source);
     return TCL_ERROR;
  }
  
  matS   = source->data.fmat;
  frameN = matS->m;
  dim = matS->n;
  frameA = 0;

  mean = (double*) malloc((unsigned)((dim)*sizeof(double)));
  var = (double*) malloc((unsigned)((dim)*sizeof(double)));
  for (i=1;i<dim;i++) {
	mean[i]=0.0;
	var[i]=0.0;
  }
  foreach_frame {
	float *X = matS->matPA[frameX];
    for (i=1;i<dim;i++) {
	  mean[i]+=X[i];
	}
  }
  for (i=1;i<dim;i++) {
	mean[i]/=frameX;
	/* fprintf(stderr," mean %i %f \n", i, mean[i]); */
  }
  foreach_frame {
	float *X = matS->matPA[frameX];
    for (i=1;i<dim;i++) {
	  var[i]+=(X[i]-mean[i])*(X[i]-mean[i]);
	}
  }
  for (i=1;i<dim;i++) {
	var[i]/=frameX;
	/* fprintf(stderr," var %i %f \n", i, var[i]); */
  }
  varsum=0;
  for (i=1;i<dim;i++) varsum+=var[i];
  fprintf(stderr," variance %f \n", varsum/dim);
  free(mean);
  free(var);
  return TCL_OK;
}



/* ========================================================================
   Functions needed by fesSpecEstItf
   ======================================================================== */

double rc2lp (const float *rc, float *pc, int Np) {
/*
Convert reflection coefficients to linear predictor coefficients
*/
	int i, j, k;
	int minphase;
	float tp;
	double perr;
	
	minphase = 1;
	perr = 1.0;
	for (k = 0; k < Np; ++k) {
		perr = perr * (1.0 - rc[k] * rc[k]);
		if (perr <= 0.0)
			minphase = 0;
		pc[k] = -rc[k];
		for (i = 0, j = k - 1; i < j; ++i, --j) {
			tp = pc[i] + rc[k] * pc[j];
			pc[j] = pc[j] + rc[k] * pc[i];
			pc[i] = tp;
		}
		if (i == j)
			pc[i] = pc[i] + rc[k] * pc[i];
	}
	if (! minphase) fprintf(stderr,"Not minimum phase! \n");
	
	return perr;
}

void LTSolve (float **L, float *b, float *x, int N, int frame) {
/*
Solve a lower triangular set of equations
*/
	int i, k;
	double sum;

	for (i = 0; i <= N; i++) {
		sum = b[i];
		for (k = 0; k < i; k++) sum -= L[i][k] * x[k];
		x[i] = (float) sum / L[i][i];
	}
	return;
}

void Covariance(float *X, int dim, float **Cov, int order) {
/*
Calculate a correlation (covariance) matrix for a data sequence
*/
	int i, j, k;

	for (i = 0; i <= order; i++) {
		for (j = 0; j <= order; j++) {
			Cov[i][j] = 0.0;
			for (k=order; k<dim; k++) 
				Cov[i][j] += X[k-i]*X[k-j]+X[k-order+i]*X[k-order+j];
		}
	}
}

int ToeplitzMatrix(float *A, float **L, int N) {
/*
This routine calculated the Toeplitz matrix
*/
	int i, j;
	for (i = 0; i <= N; i++) {
		for (j = 0; j <= N; j++) {
			L[i][j] = A[abs(i-j)];
		}
	}

	return 0;
}

int ToeplitzDMatrix(double *A, double **L, int N) {
/*
This routine calculated the Toeplitz matrix
*/
	int i, j;

	for (i = 0; i <= N; i++) {
		for (j = 0; j <= N; j++) {
			L[i][j] = A[abs(i-j)];
		}
	}

	return 0;
}

int CholeskyDecomp(float **A, float **L, int N) {
/*
This routine uses the Cholesky decomposition algorithm to produce the
triangular decomposition of the given positive definite matrix.

A = L U,  where U = L' and L and U are triangular matrices.
*/
	int i, j, k;
	int ier;
	double sum;
	for (i = 0; i <= N; i++) {
		
		/* Off-diagonal elements */
		for (j = 0; j < i; j++) {
			sum = A[i][j];
			for (k = 0; k < j; k++) sum -= L[i][k] * L[j][k];
			L[i][j] = (float) sum / L[j][j];
			L[j][i] = 0.0;
		}

		/* Diagonal elements */
		sum = A[i][i];
		for (k = 0; k < i; k++) sum -= L[i][k] * L[i][k];
		if (sum <= 0.0) {
			fprintf(stderr,"Below zero, break \n");
			break;
		}
		L[i][i] = (float) sqrt (sum);
	}
	
	/* Normal return */
	if (i >= N)	return 0;
	
	/* Incomplete factorization, zero the remainder of L */
	fprintf(stderr,"Incomplete factorization, zero the remainder of L \n");
	ier = i + 1;
	for (; i <= N; i++) {
		for (j = 0; j < i; j++) {
			L[i][j] = 0.0;
			L[j][i] = 0.0;
		}
		L[i][i] = 0.0;
	}
	
	return ier;
}

void VRfZero (float y[], int N) {
  int i;
  for (i = 0; i < N; ++i) y[i] = 0.0;
  return;
}

int LPmodcovar(float *X, float *LP, float *E, int order, int dim, int frame) {
/*  
This procedure finds the predictor coefficients for a linear predictor using
the modified covariance method which guarantees a minimum phase prediction
error filter. 
	
Results in LP[1..order]
*/
	double perr, y2;
	int i, ier;
	int Na;
	int burg = 0;
	float *r, *rc = NULL, *y, *LPshift = NULL;
	float **L, **Cov;
	FMatrix *tmpCov = NULL;
	FMatrix *tmpL = NULL;
	
	/* Allocate space */
	tmpCov = fmatrixCreate(order+2,order+2);
	tmpL = fmatrixCreate(order+1,order+1);
	r = (float*) malloc(((order+1)*sizeof(float)));
	rc = (float*) malloc((unsigned)((order+1)*sizeof(float)));
	LPshift = (float*) malloc((unsigned)((order+1)*sizeof(float)));
	y = (float*) malloc((unsigned)((order+1)*sizeof(float)));	

	Cov=tmpCov->matPA;
	L=tmpL->matPA;
	
	/* ModCovariance(X, dim, Cov, order, int frame); */
	Covariance(X, dim, Cov, order);

	/* Solve the covariance equations using Cholesky decompostion */
	ier = CholeskyDecomp (Cov, L, order);
	Na = order;
	if (ier != 0) {
		Na = ier - 1;
		fprintf(stderr,"Incomplete factorization: Cholesky Decomposition, %d \n", Na);
		VRfZero (&rc[Na], order-Na);
	}

	/* Copy the first column of Cov to pc (vector r).  This copying is necessary to
	restrict access to the lower triangular part of Cov.
	*/
		
	for (i = 0; i < Na; ++i) r[i] = Cov[i+1][0];
	
	/* Solve L y = r; the solution appears in y */
	LTSolve (L, r, y, Na, frame);

	E[0] = Cov[0][0];
	perr = Cov[0][0];
	/* if (perr < 0.0) fprintf(stderr, "negative energy \n"); */
	for (i = 0; i < Na; ++i) {
		y2 = (double) y[i] * y[i];
		if (y2 > perr) {
			burg = 1;
			/* fprintf(stderr,"negative energy, frame %i \n", frame); */
			VRfZero (&rc[i], Na-i);
			break;
		}
		else if (perr == 0.0) rc[i] = 0.0;
		else {
			rc[i] = (float) (-y[i] / sqrt(perr));
			perr = perr - y2;
		}
	}
		
	/* Convert to predictor coefficients */
	rc2lp(rc, LPshift, order);
		
	/* important for second step */
	for (i=0;i<order;i++) LP[i+1]=-LPshift[i];
	LP[0]=1;
	
	/* Deallocate storage */
	free(rc);
	free(r);
	free(y);
	free(LPshift);
	fmatrixFree(tmpCov);
	fmatrixFree(tmpL);
	
	return TCL_OK;
}

int LPautocorrelation(float *X, float *LP, float *E, int order, int dim) {
/*
Calculate LP through autocorrelation
Durbin's Recursion for LPC: see  Rabiner pp. 411
(or Applications of DSP editor Oppenheimer pp. 152)

Results in LP[1..order]=-A[1..order][order]
*/
	int i,ii;
	float temp;
	float *K = NULL, *R = NULL;
	FMatrix *tmpA = NULL;
	K = (float*) malloc((unsigned)((order+1)*sizeof(float)));
	R = (float*) malloc((unsigned)((order+1)*sizeof(float)));
	tmpA = fmatrixCreate(order+1,order+1);
	
	for (i=0;i<=order;i++) {
		temp=0.0;
		for (ii=0;ii<dim-i;ii++)
			temp += X[ii]*X[ii+i];
		R[i]=temp;
	}

	E[0]=R[0];
	tmpA->matPA[1][0]=1;

	for (i=1;i<=order;i++) {
		float **A=tmpA->matPA;
		/* eq. 8.68 (eq. 3.51a) */
		K[i]=R[i];
		for (ii=1;ii<i;ii++) K[i]-= A[ii][i-1] * R[i-ii];
		
		if (E[i-1] != 0) {
			K[i] /= E[i-1];
		} else {
			K[i] = 1000000000;
			/* fprintf(stderr,"WARNING: in LPautocorrelation division by zero\n"); */
		}
		
		/* eq. 8.69 (eq. 3.51b) */
		A[i][i]=K[i];
		
		/* eq. 8.70 (eq. 3.51c) */
		for (ii=1;ii<=i-1;ii++) 
			A[ii][i]= A[ii][i-1] - K[i] * A[i-ii][i-1];
		
		/* eq. 8.71 (eq. 3,51d) */
		E[i]=(1-K[i]*K[i]) * E[i-1];
		
	}
	
	/* important for second step */
 	LP[0]=1;
	for (i=1;i<=order;i++) {
		float **A=tmpA->matPA;
		LP[i]=-A[i][order];
	}

	free(K);
	free(R);
	fmatrixFree(tmpA);
	return TCL_OK;
}

int LPwarpautocorrelation(float *X, float *LP, float *E, int order, int dim, float warp, float rewarp, int compensate) {
/*
Compensate for warp like Nakamoto et al. INTERSPEECH 2004
Own warped-LPC implementation
Calculate LP through autocorrelation
Durbin's Recursion for LPC: see  Rabiner pp. 411
(or Applications of DSP editor Oppenheimer pp. 152)

Results in LP[1..order]=-A[1..order][order]
*/
	int i,ii;
	float temp;
	float *WX = NULL, *WXTEMP = NULL, *K = NULL, *R = NULL;
	float g1, gj, sum;
    	float a0, a1;
	FMatrix *tmpA = NULL;
	K = (float*) malloc((unsigned)((order+1)*sizeof(float)));
	R = (float*) malloc((unsigned)((order+2)*sizeof(float)));
	WX = (float*) malloc((unsigned)((dim+1)*sizeof(float)));
	WXTEMP = (float*) malloc((unsigned)((dim+1)*sizeof(float)));
	tmpA = fmatrixCreate(order+1,order+1);
	
	/* calculate warped LP */
	temp=0.0;
	for (ii=0;ii<dim;ii++) temp += X[ii]*X[ii];
	R[0]=temp;
	
	for (ii=0;ii<dim;ii++) WX[ii] = X[ii];

	/*  for compensation we need R[order+1], otherwise not needed */
	for (i=1;i<=order+1;i++) {
		/* filter */
		for (ii=0;ii<dim;ii++) WXTEMP[ii] = WX[ii];

		WX[0]=-warp*WXTEMP[0];
		for (ii=1;ii<dim;ii++) WX[ii]=warp*(WX[ii-1]-WXTEMP[ii])+WXTEMP[ii-1];

		temp=0.0;
		for (ii=0;ii<dim;ii++)
		 	temp += X[ii]*WX[ii];
		R[i]=temp;
	}

	/* compensate for the warp value */
	if (compensate > 0) {
		a0 = (warp+rewarp)/(1+warp*rewarp);
		gj = 1. - a0*a0;
    		a1 = a0/gj;
		a0 = (1. + a0*a0)/gj;

		g1 = R[0];
    		R[0]= a0*R[0] + 2.* a1 * R[1];

		for (i = 1; i <= order; ++i) {
		    sum = a0*R[i] + a1 * (g1 + R[i+1]);
		    g1 = R[i];
	    	R[i] = sum;
    		}
	}

	E[0]=R[0];
	tmpA->matPA[1][0]=1;

	for (i=1;i<=order;i++) {
		float **A=tmpA->matPA;
		/* eq. 8.68 (eq. 3.51a) */
		K[i]=R[i];
		for (ii=1;ii<i;ii++) K[i]-= A[ii][i-1] * R[i-ii];
		
		if (E[i-1] != 0) {
			K[i] /= E[i-1];
		} else {
			K[i] = 1000000000;
			/* fprintf(stderr,"WARNING: in LPautocorrelation division by zero\n"); */
		}
		
		/* eq. 8.69 (eq. 3.51b) */
		A[i][i]=K[i];
		
		/* eq. 8.70 (eq. 3.51c) */
		for (ii=1;ii<=i-1;ii++) 
			A[ii][i]= A[ii][i-1] - K[i] * A[i-ii][i-1];
		
		/* eq. 8.71 (eq. 3,51d) */
		E[i]=(1-K[i]*K[i]) * E[i-1];
		
	}
	
	/* important for second step */
 	LP[0]=1;
	for (i=1;i<=order;i++) {
		float **A=tmpA->matPA;
		LP[i]=-A[i][order];
	}

	free(WXTEMP);
	free(WX);
	free(K);
	free(R);
	fmatrixFree(tmpA);
	return TCL_OK;
}

int LPburg(float *X, float *A, float *E, int order, int dim) {
/*
Calculate LP through Burg
see Digital Processing of Speech Signals, L.R. Rabiner, R.W. Schafer. pp. 413

Results in LP[1..order]=-A[1..order][order]
*/
	int i,ii;
	double num, den;
	float *EF = NULL, *EB = NULL;
	float *EFP = NULL, *EBP = NULL;
	float *K, *A_flip = NULL;
	EF = (float*) malloc((unsigned)((dim)*sizeof(float)));
	EB = (float*) malloc((unsigned)((dim)*sizeof(float)));
	EFP = (float*) malloc((unsigned)((dim)*sizeof(float)));
	EBP = (float*) malloc((unsigned)((dim)*sizeof(float)));
	A_flip = (float*) malloc((unsigned)((order+1)*sizeof(float)));
	K = (float*) malloc((unsigned)((order+1)*sizeof(float)));

	E[0]=0.0;
	for (i=0;i<dim;i++) E[0] += X[i]*X[i];
	
	for (i=0;i<=order;i++) {
		A_flip[i]=0.0;
		A[i]=0.0;
	}

	for (i=0;i<dim;i++) {
		EF[i] = X[i];
		EB[i] = X[i];
	}

	for (i=0;i<order;i++) {

		/* Calculate the next order reflection (parcor) coefficient */

		for (ii=0;ii<dim-i-1;ii++) {
			EFP[ii] = EF[ii+1];
			EBP[ii] = EB[ii];
		}

		num = 0.0;
		den = 0.0;	
		for (ii=0;ii<dim-i-1;ii++) {
			num -= 2*EBP[ii]*EFP[ii];
			den += EFP[ii]*EFP[ii]+EBP[ii]*EBP[ii];
		}

		K[i] = (float) num/den;

		/* Update the forward and backward prediction errors */
		
		for (ii=0;ii<dim-i-1;ii++) {
			EF[ii] = EFP[ii] + K[i]*EBP[ii];
			EB[ii] = EBP[ii] + K[i]*EFP[ii];
		}
				
		/* Update the AR coeff. */

		A[0] = 1.0;
		for (ii=0;ii<=i+1;ii++) A_flip[ii] = A[i-ii+1];
		for (ii=1;ii<=i+1;ii++) A[ii] += K[i]*A_flip[ii];
	}   

	free(EF);
	free(EB);
	free(EFP);
	free(EBP);
	free(A_flip);
	free(K);
	return TCL_OK;
}

float R1R0(float *X, int dim) {
/*
returns R[1]/R[0]
*/
	int i,ii;
	float temp;
	float *R = NULL;
	R = (float*) malloc((unsigned)((2)*sizeof(float)));
	
	for (i=0;i<=1;i++) {
		temp=0.0;
		for (ii=0;ii<dim-i;ii++) { temp += X[ii]*X[ii+i]; }
		R[i]=temp;
	}
	
	temp = fabs( R[1]/R[0] );
	free(R);
	return temp;
}

/***********************************************************************/
/* This function uses a long chain of first order                      */
/* allpass elements to warp a signal                                   */
/* USAGE: trans_longchain(signal,len,lam, tim, xm)                     */
/* where signal is the input of the filter with tim values             */
/* where xm is the ouput of the filter                                 */
/* len<length(signal) is the length of the produced warped signal and lam is  */
/* the warping parameter */
/***********************************************************************/
void trans_longchain(float *signal, int len, float lam, float *xm, int tim)
{
	float x,tmpr;
	int w,e, ind = 0;
	
	for(w=0;w<tim;w++) {
		x=signal[ind++];
		for(e=0; e <len; e++) {
			/* The difference equation */
			tmpr=xm[e]+lam*xm[e+1]-lam*x; 
			xm[e]=x;
			x=tmpr;
		}
	}
	return;
}


/* ------------------------------------------------------------------------
     Spectral Estimation using LP, WLP, MVDR or WMVDR
     For detail see diploma thesis by Matthias Woelfel
   ------------------------------------------------------------------------ */

static int fesSpecEstItf( ClientData cd, int argc, char *argv[] )
{
	FeatureSet  *FS = (FeatureSet*)cd;
	Feature     *destin = NULL;
	Feature     *source = NULL;
	int         i, ii, dim, frameX, frameN, frameA;
	FMatrix     *matS = NULL,   *matD = NULL;
	float       *tmpR = NULL,   *E = NULL, *tmpA = NULL, *tmpPC = NULL, *tmpPA = NULL;
	int         log2N;
	char        *type  = "MVDR";
	char        *lpmethod  = "burg";
	int         order = 60;
	int         correlate = 0;
	int         compensate = -1;
	int         temp_order;
	float       temp;
	float       warp = 0.0, warp_var = 0.0, warp_store = 0.4595, rewarp = 0.0;
	float       sensibility = 0.1;
	
	/* ------------------
	get arguments
	------------------ */
	
	argc--;
	if (itfParseArgv( argv[0], &argc, argv+1, 0,
		"<feature>",        ARGV_CUSTOM, createFe, &destin, cd, NEW_FEAT,
		"<source_feature>", ARGV_CUSTOM, getFe,    &source, cd, SRC_FEAT,
		"<order>",          ARGV_INT,    NULL,     &order,  cd, "order",
		"-type",            ARGV_STRING, NULL,     &type,   cd,
			"\"LP\" or \"MVDR\" or \"MVDR.rewarp\" ",
		"-warp",            ARGV_FLOAT,  NULL,     &warp,   cd, "warp",
		"-sensibility",     ARGV_FLOAT,  NULL,     &sensibility,   cd, "sensibility",
		"-lpmethod",		ARGV_STRING, NULL,     &lpmethod, cd,
			"\"autocorrelation\" or \"modcovarianz\" or \"burg\" or \"warp\" ",
		"-correlate",          ARGV_INT,    NULL,  &correlate, cd, 
			"needed for burg and modcovariance",
		"-compensate",            ARGV_INT,  NULL,     &compensate,   cd, 
			"compensate for the amplitute change in rewarp",
		NULL) != TCL_OK) return TCL_ERROR;
	
	if (! IS_FMATRIX( source)) {
		MUST_FMATRIX( source);
		return TCL_ERROR;
	}

	warp_store = warp;
	
	matS   = source->data.fmat;
	frameN = matS->m;
	temp_order = 2*order+1;
	dim = matS->n;
	frameA = 0;
	if (correlate < 10) correlate = dim;

	assert (order > 0 && order < dim/2+1);
	
	matD = fmatrixCreate(frameN, dim/2+1);
	
	tmpR = (float*) malloc((unsigned)((order+1)*sizeof(float)));
	tmpA = (float*) malloc((unsigned)((order+1)*sizeof(float)));
	tmpPC = (float*) malloc((unsigned)((temp_order)*sizeof(float)));
	tmpPA = (float*) malloc((unsigned)((dim+1)*sizeof(float)));
	E = (float*) malloc((unsigned)((order+1)*sizeof(float)));
	assert (tmpR != NULL);

	if (compensate < 0) {
		Switch (type)
			Case("LP") { compensate = 0; }
    			Case("MVDR") { compensate = 0; }
			Case("MVDR.rewarp") { compensate = 1; }
    			Default { fprintf(stderr,"default, "); }
		End_switch;
	}
	if (compensate < 0) { compensate = 0; }
	if (compensate > 1) { compensate = 1; }

	fprintf(stderr,"Spectral estimation: ");
	
	Switch (type)
		Case("LP") { fprintf(stderr,"LP, ");  }
    		Case("MVDR") { fprintf(stderr,"MVDR, "); }
		Case("MVDR.rewarp") { fprintf(stderr,"MVDR.rewarp, "); }
    		Default { fprintf(stderr,"default, "); }
	End_switch;
    
	fprintf(stderr,"order %i, ", order);

	Switch (lpmethod)
	    Case("autocorrelation") { fprintf(stderr,"autocorrelation \n"); }
	    Case("burg") { fprintf(stderr,"Burg \n"); }
	    Case("modcovarianz") { fprintf(stderr,"modified covarianz \n"); }
	    Case("warp") {
		if (compensate==0) { fprintf(stderr,"warp %f, compensation off \n", warp); }
		if (compensate!=0) { fprintf(stderr,"warp %f, compensation on \n", warp); }
	    }
	    Case("varwarp") {
    		if (compensate==0) fprintf(stderr,"warp %f, varwarp %f, compensation off \n", warp, sensibility);
		if (compensate!=0) fprintf(stderr,"warp %f, varwarp %f, compensation on\n", warp, sensibility);
	    }
	    Case("fixwarp") {
    		if (compensate==0) fprintf(stderr,"warp %f, fixwarp %f, compensation off \n", warp, sensibility);
		if (compensate!=0) fprintf(stderr,"warp %f, fixwarp %f, compensation on\n", warp, sensibility);
	    }
	    Default { fprintf(stderr,"default \n"); }
	End_switch;
	
	foreach_frame {
		
		float *X = matS->matPA[frameX];
		float *A = tmpA;
		float *PC = tmpPC;
		float *PA = tmpPA;
		
		Switch (lpmethod)
		Case("autocorrelation") {
			LPautocorrelation(X,tmpA, E, order, dim);
		}
		Case("burg") {
			LPburg(X, tmpA, E, order, correlate);
		}
		Case("warp") {
			LPwarpautocorrelation(X, tmpA, E, order, correlate, warp, 0.0, compensate);
		}
		Case("varwarp") {
			warp_var = sensibility*(R1R0(X, correlate)-0.5) + warp;
			rewarp = (warp_var-warp)/(1-warp_var*warp);
			LPwarpautocorrelation(X, tmpA, E, order, correlate, warp_var, rewarp, compensate);
		}
		Case("fixwarp") {
			warp_var = sensibility + warp;
			rewarp = (warp_var-warp)/(1-warp_var*warp);
			LPwarpautocorrelation(X, tmpA, E, order, correlate, warp_var, rewarp, compensate);
		}
		Case("modcovarianz") {
			LPmodcovar(X, tmpA, E, order, correlate, frameX);
			/* LPautocorrelation is not always stable */
		}

		Default {
			LPautocorrelation(X, tmpA, E, order, dim);
		}
		End_switch;
		
		/* ------------------------------------------------------------------------
			Correlate the Predictor Coefficients PC
			see B.Musicus Fast MLM Power Spectrum Estimation from Uniformly Spaced
			Correlations
		*/

		Switch (type)
		Case("MVDR") {
			for (i=0;i<=order;i++) {
				temp = 0;
				for (ii=0;ii<=(order-i);ii++)
					temp += (float)(order+1-i-2*ii)*A[ii]*A[ii+i];
				if (E[0] > 0) {
					PA[order+i+1] = -temp;
					/* instead of -temp/E[order] in the paper by B. Musicus IEEE 1985 */
				} else {
					PA[order+i+1] = 10000000;
				}
			}
			for (i=1;i<=order;i++) PA[order-i+1] = PA[order+i+1];
			PA[0] = 0;
			for (i=temp_order+1;i<=dim;i++) PA[i] = 0;

		/* ------------------------------------------------------------------------
			Calculate MVDR Power
		    (also called Capon Max. Likelihood Method)
		*/

			log2N = ceil( log((double)dim) / log((double)2.0) );
		
			fft_pow( PA, PA, dim, log2N);
		
			for (i=0;i<=dim/2;i++) {
				temp = sqrt(PA[i]);
				if (temp > 0) {
					temp = E[0]/temp;
				} else {
					temp = 10000000;
				}
				matD->matPA[frameX][i]= temp;
			}
		}
		Case("LP") {
			PA[0] = 0;
			for (i=1;i<=order+1;i++) PA[i] = A[i-1];  /* [..-1] because of fft */
			for (i=order+2;i<=dim;i++) PA[i] = 0;

			log2N = ceil( log((double)dim) / log((double)2.0) );
		
			fft_pow( PA, PA, dim, log2N);
		
			for (i=0;i<=dim/2;i++) {
				temp = PA[i];
				if (temp > 0) {
					temp = (2*E[0])/(temp*dim);
				} else {
					temp = 10000000;
				}
				matD->matPA[frameX][i]= temp;
			}
		}
		Case("MVDR.rewarp") {	
			for (i=0;i<=order;i++) {
				temp = 0;
				for (ii=0;ii<=(order-i);ii++)
					temp += (float)(order+1-i-2*ii)*A[ii]*A[ii+i];
				if (E[0] > 0) {
					PC[order+i] = -temp;
					/* instead of -temp/E[order] in the paper by B. Musicus IEEE 1985 */
				} else {
					PC[order+i] = 10000000;
				}
			}
			
			for (i=1;i<=order;i++) PC[order-i] = PC[order+i];

			for (i=0;i<=dim;i++) PA[i] = 0.0;
			trans_longchain(PC,dim,-rewarp,PA,temp_order);
			
			/* ------------------------------------------------------------------------
			Calculate MVDR Power (also called Capon Max. Likelihood Method)*/

			log2N = ceil( log((double)dim) / log((double)2.0) );
									
			fft_pow( PA, PA, dim, log2N);
			
			for (i=0;i<=dim/2;i++) {
				temp = sqrt(PA[i]);
				if (temp > 0) {
					temp = E[0]/temp;
				} else {
					temp = 10000000;
				}
				matD->matPA[frameX][i]= temp;
			}
		}
		End_switch;
		
	}
	free(tmpR);
	free(tmpA);
	free(tmpPC);
	free(tmpPA);
	free(E);

	if(feFMatrix( FS, destin, matD, 1)) {
		fmatrixFree(matD); return TCL_ERROR;
	}
	destin->samplingRate = source->samplingRate;
	destin->shift        = source->shift;
	
	return TCL_OK;
}

static int fesSpecAdjustItf( ClientData cd, int argc, char *argv[] )
{
	FeatureSet  *FS = (FeatureSet*)cd;
	Feature     *destin = NULL;
	Feature     *maxfft = NULL;
	Feature     *maxspec = NULL;
	int         i, dim, frameX, frameN, frameA;
	FMatrix     *matD = NULL;
	FMatrix     *matMaxFFT = NULL,   *matMaxSpec = NULL;
	float       *tmpR = NULL;
	char        *show  = "off";
	int blur = 2;
	
	/* ------------------
	get arguments
	------------------ */
	
	argc--;
	if (itfParseArgv( argv[0], &argc, argv+1, 0,
		"<feature>",        ARGV_CUSTOM, createFe, &destin, cd, NEW_FEAT,
		"<adjustto>",       ARGV_CUSTOM, getFe,    &maxspec, cd, "adjust this feature",
		"<adjustfrom>",     ARGV_CUSTOM, getFe,    &maxfft, cd, "adjust from this feature",
		"-smooth",          ARGV_INT, NULL,        &blur,   cd, "smooth the adjust from feature (0,1,2,3,4)",
		"-show",            ARGV_STRING, NULL,     &show,   cd, "\"on\" or \"off\" ",
		NULL) != TCL_OK) return TCL_ERROR;
	
	matMaxFFT = maxfft->data.fmat;
	matMaxSpec = maxspec->data.fmat;
	frameN = matMaxFFT->m;
	dim = matMaxFFT->n;
	frameA = 0;

	matD = fmatrixCreate(frameN, dim);
	tmpR = (float*) malloc((unsigned)((dim+1)*sizeof(float)));

	fprintf(stderr,"Spectral adjusted. (blur %i) \n", blur);
	
	foreach_frame {
		float *R = tmpR;		
		float *MaxFFT = matMaxFFT->matPA[frameX];
		float *MaxSPEC = matMaxSpec->matPA[frameX];
		float mult;
		float maxFFT = 0.0;
		float maxSPEC = 0.0;

		if (blur==0) {
			for (i=0;i<(dim);i++) R[i] =  MaxFFT[i]; 
		}

		/* bluring */
		if (blur==1) {
			R[0] = 0.0;
			for (i=1;i<(dim-1);i++) {
				R[i] =  MaxFFT[i-1]/4 + MaxFFT[i]/2 + MaxFFT[i+1]/4; 
			}
			R[dim-1] = 0.0;
		}

		if (blur==2) {
			R[0] = 0.0;
			R[1] = 0.0;
			for (i=2;i<(dim-2);i++) {
				R[i] =  MaxFFT[i-2]/9 + 2*MaxFFT[i-1]/9 + MaxFFT[i]/3 
					+ 2*MaxFFT[i+1]/9 + MaxFFT[i+2]/9;
			}
			R[dim-2] = 0.0;
			R[dim-1] = 0.0;
		}

		if (blur==3) {
			R[0] = 0.0;
			R[1] = 0.0;
			R[2] = 0.0;
			for (i=3;i<(dim-3);i++) {
				R[i] =  (MaxFFT[i-3] + 2*MaxFFT[i-2] + 3*MaxFFT[i-1] + 4*MaxFFT[i] 
					+ 3*MaxFFT[i+1] + 2*MaxFFT[i+2] + MaxFFT[i+3])/16;
			}
			R[dim-3] = 0.0;
			R[dim-2] = 0.0;
			R[dim-1] = 0.0;
		}

		if (blur==4) {
			R[0] = 0.0;
			R[1] = 0.0;
			R[2] = 0.0;
			R[3] = 0.0;
			for (i=4;i<(dim-4);i++) {
				R[i] =  (MaxFFT[i-4] + 2*MaxFFT[i-3] + 3*MaxFFT[i-2] + 4*MaxFFT[i-1] 
					+ 5*MaxFFT[i] + 4*MaxFFT[i+1] 
					+ 3*MaxFFT[i+2] + 2*MaxFFT[i+3] + MaxFFT[i+4])/25;
			}
			R[dim-4] = 0.0;
			R[dim-3] = 0.0;
			R[dim-2] = 0.0;
			R[dim-1] = 0.0;
		}

		for (i=0;i<dim;i++) if (maxFFT<R[i]) maxFFT=R[i];
		for (i=0;i<dim;i++) if (maxSPEC<MaxSPEC[i]) maxSPEC=MaxSPEC[i];
		
		if (maxSPEC < 0.01) {mult = 100 * maxFFT;}
		else {mult = maxFFT/maxSPEC;}
		
		for (i=0;i<dim;i++) {
			matD->matPA[frameX][i]= mult*MaxSPEC[i];
		}

		Switch (show)
			Case("on") { fprintf(stderr,"%f \n" ,mult);}
		End_switch;
	}

	if(feFMatrix( FS, destin, matD, 1)) {
		fmatrixFree(matD); return TCL_ERROR;
	}
	destin->samplingRate = maxfft->samplingRate;
	destin->shift        = maxfft->shift;

	free(tmpR);

	return TCL_OK;
}


static int fesSpecSubLogItf( ClientData cd, int argc, char *argv[] )
{
	FeatureSet  *FS = (FeatureSet*)cd;
	Feature     *destin = NULL;
	Feature     *maxfft = NULL;
	Feature     *maxspec = NULL;
	int         i, dim, frameX, frameN, frameA;
	FMatrix     *matD = NULL;
	FMatrix     *matMaxFFT = NULL,   *matMaxSpec = NULL;
	float       *tmpR = NULL;
	
	/* ------------------
	get arguments
	------------------ */
	
	argc--;
	if (itfParseArgv( argv[0], &argc, argv+1, 0,
		"<feature>",        ARGV_CUSTOM, createFe, &destin, cd, NEW_FEAT,
		"<adjustto>",       ARGV_CUSTOM, getFe,    &maxspec, cd, "adjust this feature",
		"<adjustfrom>",     ARGV_CUSTOM, getFe,    &maxfft, cd, "adjust from this feature",
		NULL) != TCL_OK) return TCL_ERROR;
	
	matMaxFFT = maxfft->data.fmat;
	matMaxSpec = maxspec->data.fmat;
	frameN = matMaxFFT->m;
	dim = matMaxFFT->n;
	frameA = 0;

	matD = fmatrixCreate(frameN, dim);
	tmpR = (float*) malloc((unsigned)((dim+1)*sizeof(float)));
	
	foreach_frame {
		float *MaxFFT = matMaxFFT->matPA[frameX];
		float *MaxSPEC = matMaxSpec->matPA[frameX];
		float temp;

		for (i=0;i<dim;i++) {
			temp = exp10F(0.1*MaxSPEC[i]) - exp10F(0.1*MaxFFT[i]);
			if (temp < 10.0) temp = 10.0;
			temp = 10.0*log10F(temp);
			matD->matPA[frameX][i]= temp;
		}
	}

	if(feFMatrix( FS, destin, matD, 1)) {
		fmatrixFree(matD); return TCL_ERROR;
	}
	destin->samplingRate = maxfft->samplingRate;
	destin->shift        = maxfft->shift;

	free(tmpR);

	return TCL_OK;
}

static int fesSpecSubLog1Itf( ClientData cd, int argc, char *argv[] )
{
	FeatureSet  *FS = (FeatureSet*)cd;
	Feature     *destin = NULL;
	Feature     *maxfft = NULL;
	Feature     *maxspec = NULL;
	int         i, dim, frameX, frameN, frameA;
	FMatrix     *matD = NULL;
	FMatrix     *matMaxFFT = NULL,   *matMaxSpec = NULL;
	float       *tmpR = NULL;
	
	/* ------------------
	get arguments
	------------------ */
	
	argc--;
	if (itfParseArgv( argv[0], &argc, argv+1, 0,
		"<feature>",        ARGV_CUSTOM, createFe, &destin, cd, NEW_FEAT,
		"<adjustto>",       ARGV_CUSTOM, getFe,    &maxspec, cd, "adjust this feature",
		"<adjustfrom>",     ARGV_CUSTOM, getFe,    &maxfft, cd, "adjust from this feature",
		NULL) != TCL_OK) return TCL_ERROR;
	
	matMaxFFT = maxfft->data.fmat;
	matMaxSpec = maxspec->data.fmat;
	frameN = matMaxFFT->m;
	dim = matMaxFFT->n;
	frameA = 0;

	matD = fmatrixCreate(frameN, dim);
	tmpR = (float*) malloc((unsigned)((dim+1)*sizeof(float)));
	
	foreach_frame {
		float *MaxFFT = matMaxFFT->matPA[frameX];
		float *MaxSPEC = matMaxSpec->matPA[frameX];
		double temp;

		for (i=0;i<dim;i++) {
			temp = exp10D((double) MaxSPEC[i]) - exp10D((double) MaxFFT[i]);
			if (temp < 1.0) temp = 1.0;
			temp = log10D(temp+1.0);
			matD->matPA[frameX][i]= (float) temp;
		}
	}

	if(feFMatrix( FS, destin, matD, 1)) {
		fmatrixFree(matD); return TCL_ERROR;
	}
	destin->samplingRate = maxfft->samplingRate;
	destin->shift        = maxfft->shift;

	free(tmpR);

	return TCL_OK;
}


static int fesChangeSpecSubItf( ClientData cd, int argc, char *argv[] )
{
	FeatureSet  *FS = (FeatureSet*)cd;
	Feature     *destin = NULL;
	Feature     *maxfft = NULL;
	Feature     *maxspec = NULL;
	int         i, dim, frameX, frameN, frameA;
	FMatrix     *matD = NULL;
	FMatrix     *matMaxFFT = NULL,   *matMaxSpec = NULL;
	float       *tmpR = NULL;
	double      sum_sq_x = 0.0;
	double      sum_sq_y = 0.0;
	double      sum_coproduct = 0.0;
	double      mean_x = 0.0;
	double      mean_y = 0.0;
	double      sweep, delta_x, delta_y, pop_sd_x, pop_sd_y, cov_x_y, correlation, scale;
	
	/* ------------------
	get arguments
	------------------ */
	
	argc--;
	if (itfParseArgv( argv[0], &argc, argv+1, 0,
		"<feature>",        ARGV_CUSTOM, createFe, &destin, cd, NEW_FEAT,
		"<adjustto>",       ARGV_CUSTOM, getFe,    &maxspec, cd, "adjust this feature",
		"<adjustfrom>",     ARGV_CUSTOM, getFe,    &maxfft, cd, "adjust from this feature",
		NULL) != TCL_OK) return TCL_ERROR;
	
	matMaxFFT = maxfft->data.fmat;
	matMaxSpec = maxspec->data.fmat;
	frameN = matMaxFFT->m;
	dim = matMaxFFT->n;
	frameA = 0;

	matD = fmatrixCreate(frameN, dim);
	tmpR = (float*) malloc((unsigned)((dim+1)*sizeof(float)));
	
	foreach_frame {
		float *MaxFFT = matMaxFFT->matPA[frameX];
		float *MaxSPEC = matMaxSpec->matPA[frameX];

		mean_x = MaxFFT[0];
		mean_y = MaxSPEC[0];
		for (i=1;i<dim;i++) {
		    sweep = (i - 1.0) / i;
		    delta_x = MaxFFT[i] - mean_x;
		    delta_y = MaxSPEC[i] - mean_y;
		    sum_sq_x += delta_x * delta_x * sweep;
		    sum_sq_y += delta_y * delta_y * sweep;
		    sum_coproduct += delta_x * delta_y * sweep;
		    mean_x += delta_x / i;
		    mean_y += delta_y / i;
		}
		pop_sd_x = sqrt(sum_sq_x / (dim+1));
		pop_sd_y = sqrt(sum_sq_y / (dim+1));
		cov_x_y = sum_coproduct / (dim+1);
		correlation = cov_x_y / (pop_sd_x * pop_sd_y);
		scale = 1.3 * correlation;
		/* scale = 4.0*(0.6 - correlation); */
		if (scale < 0.0) scale=0.0;
		if (scale > 1.0) scale=1.0;
		fprintf(stderr,"scale[%i] %f %f \n", frameX, correlation, scale);

		for (i=0;i<dim;i++) {
			matD->matPA[frameX][i]= scale * MaxSPEC[i];
		}
	}

	if(feFMatrix( FS, destin, matD, 1)) {
		fmatrixFree(matD); return TCL_ERROR;
	}
	destin->samplingRate = maxfft->samplingRate;
	destin->shift        = maxfft->shift;

	free(tmpR);

	return TCL_OK;
}


static int fesTraceItf( ClientData cd, int argc, char *argv[] )
{
        #define FULL 0
	#define DIAGONAL 1
	FeatureSet  *FS = (FeatureSet*)cd;
	Feature     *destin = NULL;
	Feature     *class = NULL;
	Feature     *source = NULL;
	int         i, d, d1, d2, dim, frameX, frameN, frameA;
	FMatrix     *matMean = NULL;
	DMatrix     *matSw = NULL;
	DMatrix     *matSt = NULL;
	DMatrix     *matD = NULL;
	FMatrix     *matclass = NULL,   *matsource = NULL;
	FMatrix     *matDf = NULL;
	int         *count = NULL;
	int         number = 0;
	int         which = 0;
	int	    type = FULL;
	int     iter   = 50000;
	float   thresh = 1.0e-7;
	
	/* ------------------
	get arguments
	------------------ */
	
	argc--;
	if (itfParseArgv( argv[0], &argc, argv+1, 0,
		"<feature>",        ARGV_CUSTOM, createFe, &destin, cd, NEW_FEAT,
		"<source_feature>", ARGV_CUSTOM, getFe,    &source, cd, SRC_FEAT,
		"<class>",          ARGV_CUSTOM, getFe,    &class,  cd, "frame belongs to class",
		"-numberofclasses", ARGV_INT, NULL,        &number, cd, "define number of classes",
		NULL) != TCL_OK) return TCL_ERROR;
	
	matclass = class->data.fmat;
	matsource = source->data.fmat;
	frameN = matsource->m;
	dim = matsource->n;
	frameA = 0;

	if (number == 0) {
	    for (frameX=0; frameX<frameN; frameX++) {
	        which = matclass->matPA[frameX][0]+1;
		if (which > number) number=which;
	    }
	    fprintf(stderr,"found %i classes\n", number);
	}

	matSw = dmatrixCreate(dim, dim);
	matSt = dmatrixCreate(dim, dim);
	matD = dmatrixCreate(dim, dim);
	matDf = fmatrixCreate(1, dim);

	matMean = fmatrixCreate(number+1, dim);
	count = (int*) malloc((unsigned)((number+1)*sizeof(int)));

	for (i=1; i<=number; i++) count[i] = 0;

	for (frameX=0; frameX<frameN; frameX++) {
	    which = matclass->matPA[frameX][0]+1;
	    if (which > 0) count[which]++;
	}

	for (i=1; i<=number; i++) {
	    if (count[i] < 5) count[i] = 0;
	}

	if (number>1) {

	    /* calculate counts, mean of classes and overall mean */
	    for (frameX=0; frameX<frameN; frameX++) {
	        which = matclass->matPA[frameX][0]+1;
	        if (which > 0) if (count[which] > 0)
	            for (d=0; d<dim; d++) matMean->matPA[which][d] += matsource->matPA[frameX][d];
	    }

	    for (i=1; i<=number; i++) {
	        if (count[i] > 0) {
	            count[0] += count[i];
	            for (d=0; d<dim; d++) matMean->matPA[0][d] += matMean->matPA[i][d];
	        }
	    }

	    for (i=0; i<=number; i++)
	        for (d=0; d<dim; d++)
		    if (count[i] > 0) matMean->matPA[i][d] /= (float) count[i];

	    /* build within-class scatter Sw and total scatter St */
	    for (frameX=0; frameX<frameN; frameX++) {
	        which = matclass->matPA[frameX][0]+1;
	        if (which > 0) if (count[which] > 0) {
		    if (type == FULL) {
		        for (d1=0; d1<dim; d1++) {
			    for (d2=0; d2<dim; d2++) {
			        matSw->matPA[d1][d2] += (matsource->matPA[frameX][d1]-matMean->matPA[which][d1])
				    *(matsource->matPA[frameX][d2]-matMean->matPA[which][d2]);
			        matSt->matPA[d1][d2] += (matsource->matPA[frameX][d1]-matMean->matPA[0][d1])
				    *(matsource->matPA[frameX][d2]-matMean->matPA[0][d2]);
			    }
		        }
		    } else {
		        for (d=0; d<dim; d++) {
			    matSw->matPA[d][d] += (matsource->matPA[frameX][d]-matMean->matPA[which][d])
			        *(matsource->matPA[frameX][d]-matMean->matPA[which][d]);
			    matSt->matPA[d][d] += (matsource->matPA[frameX][d]-matMean->matPA[0][d])
			        *(matsource->matPA[frameX][d]-matMean->matPA[0][d]);
		        }
		    }
	        }
	    }

	    if (type == FULL) {
	        dmatrixWhiten(matSw);
	        if (dmatrixEigenQR(matD,matSt,(double)thresh,iter) < 0) return 0;
	        if (!dmatrixClean(matD,thresh)) {
	            ERROR("Simdiag #2: not enough iterations %d\n",iter);
	            for (d=0; d<dim; d++) matDf->matPA[0][d] = -1.0;
	        }
	        dmatrixMul   (matD,matSt,matSw);
	        for (d=0; d<dim; d++) matDf->matPA[0][d] = matD->matPA[d][d];
	    } else {
	        for (d=0; d<dim; d++) {
	            if (matSw->matPA[d][d] > 0) {
		        matDf->matPA[0][d] = matSt->matPA[d][d]/matSw->matPA[d][d];
	            } else {
		        fprintf(stderr,"Sw[%i][%i] = %e -> -1.0\n", d,d,matSw->matPA[d][d]);
		        matDf->matPA[0][d] = -1.0;
	           }
	        }
	    }


	} else {
	    fprintf(stderr,"skip processing\n");
	    for (d=0; d<dim; d++) matDf->matPA[0][d] = -1.0;
	}

	if(feFMatrix( FS, destin, matDf, 1)) {
		fmatrixFree(matDf); return TCL_ERROR;
	}
	destin->samplingRate = 0.0;
	destin->shift        = 0.0;

	dmatrixFree(matSw);
	dmatrixFree(matSt);
	dmatrixFree(matD);
	fmatrixFree(matMean);
	free(count);

	return TCL_OK;
}

static int extractGaussianMixturesFromDistribSet(DistribSet* dsP, GaussianMixture*** gmmsP)
{
    int                 nDistribs = dsP->list.itemN;
    CodebookSet*        cbsP = dsP->cbsP;

    GaussianMixture**   gmms;
    Gaussian**          gaussians;
    Distrib*            dP;
    Codebook*           cbP;
    int                 nMixtures, dim;
    int                 i,j,k;
    float*              mean;
    float*              covariance;
    float*              weights;
    float               weight_delay_mean = 0.3;
    float               weight_delay_var = 0.2;


    /* -------------------------------------------------------------------------
    /  check that nDistribs is smooth
      ---------------------------------------------------------------------- */
    if (nDistribs!=cbsP->list.itemN) {
        fprintf(stderr,"PF: #codebook in CodebookSet != #distrib in DistribSet\n");
        exit(1);
    }

    /* -------------------------------------------------------------------------
    /  allocate memory for gaussian mixtures
      ---------------------------------------------------------------------- */
    gmms = (GaussianMixture**) malloc(nDistribs*sizeof(GaussianMixture*));
    memset(gmms, 0, nDistribs*sizeof(GaussianMixture*));

    /* -------------------------------------------------------------------------
    /  get dimension
      ---------------------------------------------------------------------- */
    cbP = &(cbsP->list.itemA[0]);
    if (cbP!=0) {
        dim = cbP->dimN;
    } else {
        fprintf(stderr,"PF: ERROR empty codebookset\n");
        exit(1);
    }

    /* -------------------------------------------------------------------------
    /  extract codebooks
      ---------------------------------------------------------------------- */
    for (i=0; i<nDistribs; i++) {
        /* -------------------------------------------------------------------------
        /  get distribution & codebook + output info
          ---------------------------------------------------------------------- */
        dP = &(dsP->list.itemA[i]);
        nMixtures = dP->valN;
        cbP = &(cbsP->list.itemA[dP->cbX]);
	dim = cbP->dimN;
        fprintf(stderr,"PF: codebook %s, mixtures %i, dim %i\n",dP->name,nMixtures,dim);

        /* -------------------------------------------------------------------------
          allocate memory for gaussians
          ---------------------------------------------------------------------- */
        gaussians = (Gaussian**) malloc(nMixtures*sizeof(Gaussian*));

        /* -------------------------------------------------------------------------
          walk through nMixtures
          ---------------------------------------------------------------------- */
        for (j=0; j<nMixtures; j++) {
            if (cbP->type!=COV_DIAGONAL) {
                fprintf(stderr,"[ERROR] COVARIANCE MATRIX MUST BE DIAGONAL\n");
                exit(1);
            }
            if (cbP->dimN!=dim) {
                fprintf(stderr,"[WARN] Codebooks have different dimensions!\n");
            }

            /* -------------------------------------------------------------------------
             allocate memory for mean and covariance and copy
             ---------------------------------------------------------------------- */
	    if (strcmp(dP->name,"noise")==0) {
                mean = (float*) malloc((2*dim)*sizeof(float));
                covariance = (float*) malloc((2*dim)*sizeof(float));
                for (k=0; k<dim; k++) {
		    mean[k] = cbP->rv->matPA[j][k];
                    covariance[k] = 1.0/(cbP->cv[j]->m.d[k]);
		}
		for (k=dim; k<2*dim; k++) {
		    mean[k] = weight_delay_mean;
		    covariance[k] = weight_delay_var;
		}
            } else {
                mean = (float*) malloc(dim*sizeof(float));
                covariance = (float*) malloc(dim*sizeof(float));
                for (k=0; k<dim; k++) mean[k] = cbP->rv->matPA[j][k];
                for (k=0; k<dim; k++) covariance[k] = 1/(cbP->cv[j]->m.d[k]);
	    }

            /* -------------------------------------------------------------------------
             create Gaussian
             ---------------------------------------------------------------------- */
            gaussians[j] = createGaussian(dim, mean, covariance, DIAGONAL_COVARIANCE);
        }

        /* -------------------------------------------------------------------------
          allocate memory for weights
          ---------------------------------------------------------------------- */
        weights = malloc(nMixtures*sizeof(float));

        /* -------------------------------------------------------------------------
          get weights
         ---------------------------------------------------------------------- */
        for (j=0; j<nMixtures; j++) weights[j] = exp((double)-1.0*dP->val[j]);

        /* -------------------------------------------------------------------------
          create Gaussian mixture
          ---------------------------------------------------------------------- */
        gmms[i] = createGaussianMixture(dim, nMixtures, gaussians, weights);
    }

    (*gmmsP) = gmms;
 
    return(nDistribs);
}

static Gaussian* createStaticNPEGaussian(int dim, float *variance)
{

    float*      mean;
    float*      covariance;
    Gaussian*   gaussian;
    int         i;

    /* -------------------------------------------------------------------------
      allocate memory for mean and covariance and fill
      ---------------------------------------------------------------------- */
    mean = (float*) malloc(dim*sizeof(float));
    covariance = (float*) malloc(dim*sizeof(float));
    for (i=0; i<dim; i++) mean[i] = 0.0;
    for (i=0; i<dim; i++) covariance[i] = variance[i];
    fprintf(stderr,"use variance (%i): ", dim);
    for (i=0; i<dim; i++) fprintf(stderr," %e,",covariance[i]);
    fprintf(stderr,"\n");

    /* -------------------------------------------------------------------------
      create Gaussian
      ---------------------------------------------------------------------- */
    gaussian = createGaussian(dim, mean, covariance, DIAGONAL_COVARIANCE);

    return(gaussian);
}

static int extractModels(DistribSet* dsP, SpeechModel** speechModelA, NoiseModel** noiseModelA, int dim_noise, float *variance)
{
    int                 nDistribs = dsP->list.itemN;
    GaussianMixture**   gmms = 0;
    GaussianMixture**   specificSpeechGmms = 0;
    GaussianMixture*    generalSpeechGmm = 0;
    GaussianMixture*    noiseGmm = 0;
    Distrib*            dP;
    int                 i;
    int                 nSpecificDistributions;
    ARModel*            arModel;
    Gaussian*           npErrorG;
    int                 foundNoise = 0, foundSpeech = 0;

    /* -------------------------------------------------------------------------
      early exit
      ------------------------------------------------------------------------ */
    if (!nDistribs) {
        fprintf(stderr,"PF: zero distributions in distribution set\n");
        exit(1);
    }

    /* -------------------------------------------------------------------------
      extract distributions
      ------------------------------------------------------------------------ */
    extractGaussianMixturesFromDistribSet(dsP, &gmms);

    /* -------------------------------------------------------------------------
      find "noise", "speech" distribution
      ------------------------------------------------------------------------ */
    nSpecificDistributions = 0;
    for (i=0; i<nDistribs; i++) {
        dP = &(dsP->list.itemA[i]);
        if (strcmp(dP->name,"noise")==0) {
            foundNoise = 1;
            noiseGmm = gmms[i];
	    noiseGmm->dim = dim_noise;
        } else {
            if (strcmp(dP->name,"speech")==0) {
                foundSpeech = 1;
                generalSpeechGmm = gmms[i];
            } else nSpecificDistributions++;
        }
    }
    if (!foundSpeech) fprintf(stderr,"PF: can't find speech CB\n");
    if (!foundNoise)  fprintf(stderr,"PF: can't find noise CB\n");

    specificSpeechGmms = gmms;

    /* -------------------------------------------------------------------------
     check that there is a noise-gmm and extract dim
     ------------------------------------------------------------------------ */
    if (!noiseGmm) {
        fprintf(stderr,"PF: noise distribution not found in distribution set\n");
        exit(1);
    }

    /* -------------------------------------------------------------------------
      create noise prediction error distribution with static variance
      ---------------------------------------------------------------------- */
    npErrorG =  createStaticNPEGaussian(dim_noise, variance);

    /* -------------------------------------------------------------------------
      create models
      ------------------------------------------------------------------------ */
    (*speechModelA) = createSpeechModel(generalSpeechGmm, specificSpeechGmms, nDistribs);

    arModel = createARModel(dim_noise, 1, npErrorG);

    (*noiseModelA) = createNoiseModelAR(arModel, noiseGmm);
    return 0;
}

int updateNoiseModel(DistribSet* dsP, NoiseModel* noiseModel, int dim_noise)
{
    int                 nDistribs = dsP->list.itemN;
    CodebookSet*        cbsP = dsP->cbsP;
    Distrib*            dP;
    Codebook*           cbP;
    int                 i, j, k;
    int                 nSpecificDistributions;
    int                 dim;
    int                 nMixtures;
    int                 foundNoise = 0;

    /* -------------------------------------------------------------------------
      early exit
      ------------------------------------------------------------------------ */
    if (!nDistribs) {
        fprintf(stderr,"PF: zero distributions in distribution set\n");
        exit(1);
    }

    /* -------------------------------------------------------------------------
      find "noise" distribution
      ------------------------------------------------------------------------ */

    nSpecificDistributions = 0;
    for (i=0; i<nDistribs; i++) {
        dP = &(dsP->list.itemA[i]);
	cbP = &(cbsP->list.itemA[dP->cbX]);
	nMixtures = dP->valN;
	dim = cbP->dimN;
        if (strcmp(dP->name,"noise")==0) {
            foundNoise = 1;

	    noiseModel->gmm->nMixtures = nMixtures;
	    noiseModel->gmm->dim = dim;

	    for (j=0; j<nMixtures; j++) {
                for (k=0; k<dim; k++) noiseModel->gmm->gaussians[j]->mean[k] = cbP->rv->matPA[j][k];
                for (k=0; k<dim; k++) noiseModel->gmm->gaussians[j]->covariance[k] = 1/(cbP->cv[j]->m.d[k]);
	        noiseModel->gmm->weightDistribution->probabilities[j] = exp((double)-1.0*dP->val[j]);
	    }

	    break;
	}
    }
    if (!foundNoise)  fprintf(stderr,"PF: can't find noise CB -> no update\n");
    return 0;
}

static int extractAUSet(DistribSet* dsP, char*** auNames)
{
    int                 nDistribs = dsP->list.itemN;
    Distrib*            dP;
    int                 i;
    char**              acousticalUnitNames;

    /* -------------------------------------------------------------------------
    /  early exit
      ---------------------------------------------------------------------- */
    if (!nDistribs) {
        fprintf(stderr,"PF: Zero distributions in distribution set\n");
        exit(1);
    }

    /* -------------------------------------------------------------------------
    /  allocate memory for gaussians
      ---------------------------------------------------------------------- */
    acousticalUnitNames = (char**) malloc(nDistribs*sizeof(char*));
    (*auNames) = acousticalUnitNames;

    /* -------------------------------------------------------------------------
    /  find "noise", "speech" distribution
      ---------------------------------------------------------------------- */
    for (i=0; i<nDistribs; i++) {
        dP = &(dsP->list.itemA[i]);
        acousticalUnitNames[i] = dP->name;
    }

    return(nDistribs);
}

int getAUModelForFrame(Utterance* utterance, int frame)
{
    SimpleList*         phonemeList;
    SimpleNode*         node;
    AcousticalUnit*     au;

    if (utterance==0)
        return(-1);
    phonemeList = (*utterance).phonemeList;
    if (phonemeList==0) return(-1);

    node = (*phonemeList).search(phonemeList, &frame, searchAcousticalUnitFrame);
    if (node==0) return(-1);

    au = (AcousticalUnit*) (*node).cargo;
    if (au==0) return(-1);

    return((*au).number);
}

/* ------------------------------------------------------------------------------------------------/
/ STATIC VARIABLES
/------------------------------------------------------------------------------------------------*/
static NCPF        *ncpf = 0;
static ConfigNCPF  configNCPF;
static int rngInitialized = 0;

static int fesParticleFilterItf( ClientData cd, int argc, char *argv[] )
{
    FeatureSet  *FS = (FeatureSet*)cd;
    Feature     *destin = NULL;
    Feature     **source;
    Feature     *spec = NULL;
    DistribSet  *dsP;
    int         dim, frameX, frameN, frameA, i,j;
    FMatrix     *matD = NULL;
    FMatrix     *matSpec = NULL;
    FMatrix     *matV = NULL;
    ConfigPF    configPF;
    ConfigNC    configNC;
    /* static necessary for online mode */
    static NoiseModel  *noiseModel;
    static SpeechModel *speechModel;
    char*       transStr = 0;
    Utterance*  utterance;
    char**      auNames;
    int         nau;
    int         auN;
    int         number = 100;
    Feature     *variance = NULL;
    float       refresh  = 1E-40;
    float       nio = 0.0;
    float       smoothing = 1.0;
    char        *type  = "sia";
    Feature     *wfeat  = NULL;
    FMatrix     *matSPEECH = NULL;
    int         i0, use_delay;
    int         sourceN;
    int         fast = 0;
    int         coeffN = 0;
    int         init = 1;

    /* ------------------
    get arguments
    ------------------ */
    argc--;
    if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<feature_out>",    ARGV_CUSTOM,    createFe,   &destin,    cd, "cleaned feature",
    "<feature_in>",     ARGV_CUSTOM,    getFe,      &spec,      cd, "feature to be cleaned",
    "distribSet",       ARGV_OBJECT,    NULL,       &dsP,       "DistribSet", "enter the DistribSet here (CodebookSet is also loaded)",
    "-number",          ARGV_INT,       NULL,       &number,    cd, "number of particles",
    "-fast",            ARGV_INT,       NULL,       &fast,      cd, "fast version (skipping every second frame)",
    "-variance",        ARGV_CUSTOM,    getFe,      &variance,  cd, "variance of the noise propagation",
    "-refresh",         ARGV_FLOAT,     NULL,       &refresh,   cd, "cut of the likelihood",
    "-nio",             ARGV_FLOAT,     NULL,       &nio,       cd, "noise intensity offset",
    "-ARsmoothing",     ARGV_FLOAT,     NULL,       &smoothing, cd, "determines smoothing over frames for the dynamic AR matrix",
    "-transcription",   ARGV_STRING,    NULL,       &transStr,  cd, "acoustic model trancription",
    "-type",            ARGV_STRING,    NULL,       &type,      cd, "\"sia\" or \"vts\" ",
    "-speech",          ARGV_CUSTOM,    getFe,      &wfeat,     cd, "speech frames marked with 1, otherwise 0",
    "-init",            ARGV_INT,       NULL,       &init,      cd, "initialize particle filter",
    "-delayspec",       ARGV_REST,      NULL,       &i0,        cd, "list of delay spectra",
	NULL) != TCL_OK) return TCL_ERROR;
	
    matSpec = spec->data.fmat;
    matV   = variance->data.fmat;
    frameN = matSpec->m;
    dim = matSpec->n;
    frameA = 0;
    matD = fmatrixCreate(frameN, dim);
    float *var = matV->matPA[0];
    if (wfeat > 0) matSPEECH = wfeat->data.fmat;
    sourceN = argc - i0;
    source = (Feature**)malloc((unsigned)(sourceN*sizeof(Feature*)));

    for (use_delay=0; use_delay<sourceN; use_delay++) {
        i0++;
        fprintf(stderr,"PF: using delayed spectra %s for joint compensation\n",argv[i0]);

        if ((source[use_delay] = featureGet(FS,argv[i0],0)) == NULL) { 
            free(source); return TCL_ERROR;
        }
        if (source[use_delay]->type != source[0]->type)  {
            ERROR("PF: feature type of %s differs!\n",source[use_delay]->name);
           free(source); return TCL_ERROR;
        }

        if (IS_FMATRIX(source[0])) {
            frameX = source[use_delay]->data.fmat->m;
            coeffN += source[use_delay]->data.fmat->n; 
        } else if (IS_SVECTOR(source[0])) {
            frameX = source[use_delay]->data.svec->n;
        } else {
            ERROR("PF: feature %s has wrong type!\n",source[use_delay]->name);
            free(source); return TCL_ERROR;
        }

        if (frameN == -1) frameN = frameX;
        else if (frameN != frameX) {
	    ERROR("PF: feature '%s' has different frame/sample number %d (!= %d).\n", *argv, frameX, frameN);
	    free(source); return TCL_ERROR;
        }
    }

    if (!use_delay) fprintf(stderr,"PF: found no delayed spectra using additive compensation only\n");

    /* --------------------------------------------------------------------------------------------/
    /  INTITIALIZE RNG
    /--------------------------------------------------------------------------------------------*/
    if (!rngInitialized) {
	RNG_Init(0);
	rngInitialized = 1;
	init = 1;
    }

    /* -------------------------------------------------------------------------
      extract models
      ------------------------------------------------------------------------ */
    if (init) {
        fprintf(stderr,"PF: initializing\n");
        if (use_delay) {
            extractModels(dsP, &speechModel, &noiseModel, dim+2, var);
        } else {
            extractModels(dsP, &speechModel, &noiseModel, dim, var);
        }
    } else if (!speechModel) {
        fprintf(stderr,"PF: speechModel was NULL\n");
        exit(1);
    }

    /* -------------------------------------------------------------------------
      obtain acousticalUnitNames, acousticalUnitNamesSize
      ------------------------------------------------------------------------ */
    nau = extractAUSet(dsP, &auNames);

    /* -------------------------------------------------------------------------
      get utterance
      ------------------------------------------------------------------------ */
    initTranscriptionJANUS(auNames, nau);

    /* -------------------------------------------------------------------------------------------
    /  check dimensionality
    /------------------------------------------------------------------------------------------*/
    GaussianMixture* largestGMM = speechModel->determineLargestGMM(speechModel);
    if (dim!=largestGMM->dim) {
	fprintf(stderr,"PF: dimensional mismatch  %i<->%i !\n", dim,speechModel->generalGMM->dim);
	exit(1);
    }

    /* -------------------------------------------------------------------------------------------
    /  PF CONFIGURATION
    /------------------------------------------------------------------------------------------*/
    configPF.flags         = PF_CONFIG_RESAMPLING_RSR | PF_CONFIG_USE_REFRESH;

    /* defines dimensions of the particle filter */
    if (use_delay) {
        configPF.dim           = dim+2;
    } else {
        configPF.dim           = dim;
    }

    configPF.historyLevel  = 1;
    configNC.phaseAccuracy = 8;
    configNC.compensationAccuracy = 0.9;

    /* --------------------------------------------------------------------------------------------/
      PF CONFIGURATION
    /--------------------------------------------------------------------------------------------*/
    configPF.nParticles    = number;
    configPF.refresh_ollb  = refresh;
    configPF.refresh_sllt  = 10;

    Switch (type)
	Case("sia")    { configNC.flags = NC_CONFIG_METHOD_SIA; }
	Case("vts")    { configNC.flags = NC_CONFIG_METHOD_VTS; }
	Default        {
	    configNC.flags = NC_CONFIG_METHOD_SIA;
	    fprintf(stderr,"PF: compensation method unknown, use 'sia'\n");
	}
    End_switch;

    configNCPF.flags = NCPF_CONFIG_USE_ACCEPTANCE_TEST;
    configNCPF.FAT_nRetries = 100;

    /* --------------------------------------------------------------------------------------------/
    /  MAKE PF CONFIG
    /--------------------------------------------------------------------------------------------*/
    configNCPF.configPF = configPF;
    configNCPF.configNC = configNC;

    /* --------------------------------------------------------------------------------------------/
    /  Initialize
    /--------------------------------------------------------------------------------------------*/

    if (init) {
        ncpf = createNCPF(noiseModel, speechModel, configNCPF);
        /* init with first frame */
        ncpf->initialize(ncpf, matSpec->matPA[0]);

        /* set diagonal AR matrix */
        for (j=0; j<ncpf->pf.config.dim; j++) {
            for (i=0; i<ncpf->pf.config.dim; i++) {
                ncpf->noiseModel->arm->coeffMatrix[j][i] = 0.0;
            }
        }
        for (i=0; i<ncpf->pf.config.dim; i++) ncpf->noiseModel->arm->coeffMatrix[i][i] = 1.0;
    } else {
	if (use_delay) {
            updateNoiseModel(dsP, ncpf->noiseModel, dim+2);
	} else {
	    updateNoiseModel(dsP, ncpf->noiseModel, dim);
	}
    }

    ncpf->setNIO(ncpf, nio);

    /*----------------------------------------------------------
    /  get transcription
    /---------------------------------------------------------*/
    utterance = 0;
    if (transStr!=0) utterance = parseUtteranceJANUS(transStr);

    /* set AR smoothing parameter */
    ncpf->noiseModel->arm->smoothing = smoothing;

    /*-----------------------------------------------------------------
    /  run PF
    /----------------------------------------------------------------*/
    foreach_frame {
        auN = getAUModelForFrame(utterance, frameX);
        /* fprintf(stderr,"    Frame[%i] use: %i \n",frameX,auN); */
	float *SPEC_noise = matSpec->matPA[frameX];
        float *SPEC_cleaned = matD->matPA[frameX];
	float *SPEC_delay = NULL;
	if (use_delay) {
	    SPEC_delay = source[0]->data.fmat->matPA[frameX];
	}

	/* skipping silence frames */
	if (wfeat > 0) {
	    float *SPEECH = matSPEECH->matPA[frameX];
	    if (SPEECH[0]<0.5) {
		for (i=0;i<dim;i++) SPEC_cleaned[i]=SPEC_noise[i];
		continue;
	    }
	}

	/* fast version - skipping every second frame */
	if ((fast == 1) && (frameX % 2 == 0)) {
	    for (i=0;i<dim;i++) SPEC_cleaned[i]=SPEC_noise[i];
	    continue;
	}

	/* change speech model (phoneme dependent PF) */
	ncpf->speechModel->set(ncpf->speechModel, &auN);

	/* run the PF for a single frame */
	ncpf->run(ncpf, SPEC_noise, SPEC_cleaned, SPEC_delay, frameX);
    }

    if(feFMatrix( FS, destin, matD, 1)) {
	fmatrixFree(matD); return TCL_ERROR;
    }
    destin->samplingRate = spec->samplingRate;
    destin->shift        = spec->shift;

    return TCL_OK;
}

static int fesCalculateVarianceItf( ClientData cd, int argc, char *argv[] )
{
	FeatureSet  *FS = (FeatureSet*)cd;
	Feature     *destin = NULL;
	Feature     *source = NULL;
	Feature     *wfeat  = NULL;
	int         i, dim, frameX, frameN;
	FMatrix     *matD = NULL;
	FMatrix     *matM = NULL;
	FMatrix     *matSPEECH = NULL; 
	FMatrix     *matSOURCE = NULL; 
	int count = 0;
        float       temp;
	
	/* ------------------
	get arguments
	------------------ */
	
	argc--;
	if (itfParseArgv( argv[0], &argc, argv+1, 0,
		"<feature>",        ARGV_CUSTOM, createFe, &destin, cd, NEW_FEAT,
		"<source>",         ARGV_CUSTOM, getFe,    &source, cd, "adjust this feature",
    		"-silence",         ARGV_CUSTOM, getFe,    &wfeat,  cd, "silence frames marked with 1, otherwise 0",
		NULL) != TCL_OK) return TCL_ERROR;
	
        matSPEECH = wfeat->data.fmat;
        matSOURCE = source->data.fmat;
	frameN = matSOURCE->m;
	dim = matSOURCE->n;
        matD = fmatrixCreate(1, dim);
	matM = fmatrixCreate(1, dim);
	for (i=0; i<dim; i++) matM->matPA[0][i] = 0.0;
	for (i=0; i<dim; i++) matD->matPA[0][i] = 0.0;
	for ( frameX=0; frameX<(frameN-1); frameX++ ) {
	    float *SILENCE = matSPEECH->matPA[frameX];
            float *SILENCENEXT = matSPEECH->matPA[frameX+1];
            float *SOURCE = matSOURCE->matPA[frameX];
            float *SOURCENEXT = matSOURCE->matPA[frameX+1];
	    if ( (SILENCE[0]+SILENCENEXT[0])>1.9 ) {
 	        count++;
                for (i=0; i<dim; i++) {
                    matM->matPA[0][i] += SOURCENEXT[i] - SOURCE[i];
                }
 	    }	
	}
	for (i=0; i<dim; i++) matM->matPA[0][i] /= count;
/*
      fprintf(stderr,"prediction error mean ");
      for (i=0; i<dim; i++) {
	        matD->matPA[0][i] /= count;
	        fprintf(stderr," %e,",matM->matPA[0][i]);
      }
*/
	count = 0;
	for ( frameX=0; frameX<(frameN-1); frameX++ ) {
	     float *SILENCE = matSPEECH->matPA[frameX];
             float *SILENCENEXT = matSPEECH->matPA[frameX+1];
             float *SOURCE = matSOURCE->matPA[frameX];
             float *SOURCENEXT = matSOURCE->matPA[frameX+1];
	     if ( (SILENCE[0]+SILENCENEXT[0])>1.9 ) {
 	         count++;
                 for (i=0; i<dim; i++) {
		   /* temp = SOURCENEXT[i] - SOURCE[i] - matM->matPA[0][i]; */
		     temp = SOURCENEXT[i] - SOURCE[i];
                     matD->matPA[0][i] += temp*temp;
                 }
 	     }
	}
	fprintf(stderr,"prediction error variance: ");
	for (i=0; i<dim; i++) {
	     matD->matPA[0][i] /= count;
	     fprintf(stderr," %e,",matD->matPA[0][i]);
	}
	fprintf(stderr,"\n");

	if(feFMatrix( FS, destin, matD, 1)) {
		fmatrixFree(matD); return TCL_ERROR;
	}
	fmatrixFree(matM);
	destin->samplingRate = source->samplingRate;
	destin->shift        = source->shift;

	return TCL_OK;
}

static int fesGetDeltaSamplesItf( ClientData cd, int argc, char *argv[] )
{
	FeatureSet  *FS = (FeatureSet*)cd;
	Feature     *destin = NULL;
	Feature     *source = NULL;
	Feature     *wfeat  = NULL;
	int         i, dim, frameX, frameN;
	FMatrix     *matD = NULL;
	FMatrix     *matP = NULL;
	FMatrix     *matM = NULL;
	FMatrix     *matSPEECH = NULL; 
	FMatrix     *matSOURCE = NULL; 
	FMatrix     *lpCoeffs = NULL;
	int count = 0;
	
	/* ------------------
	get arguments
	------------------ */
	
	argc--;
	if (itfParseArgv( argv[0], &argc, argv+1, 0,
		"<feature>",        ARGV_CUSTOM, createFe, &destin, cd, NEW_FEAT,
		"<source>",         ARGV_CUSTOM, getFe,    &source, cd, "adjust this feature",
        	"<matrix>",         ARGV_CUSTOM, getFMatrix, &lpCoeffs,  cd, "LP-COEFF-Matrix",
		NULL) != TCL_OK) return TCL_ERROR;
	
    assert (wfeat); // can be NULL
        matSPEECH = wfeat->data.fmat;
        matSOURCE = source->data.fmat;
	frameN = matSOURCE->m;
	dim = matSOURCE->n;

	matM = fmatrixCreate(frameN, dim);
	count = 0;

	matP = fmatrixCreate(frameN, dim);
	for ( frameX=0; frameX<count; frameX++ ) {
            for (i=0; i<dim; i++) matP->matPA[frameX][i] = matM->matPA[frameX][i];
	}

	FMatrix *C = fmatrixCreate(source->data.fmat->m,dim);
	fmatrixMulot( C, matP, lpCoeffs);

	for ( frameX=0; frameX<(frameN-1); frameX++ ) {
            float *SOURCE = matSOURCE->matPA[frameX];
	    float *SOURCENEXT = matP->matPA[frameX];
		for (i=0; i<dim; i++) matM->matPA[count][i] = SOURCENEXT[i] - SOURCE[i];
		count++;
	}
	matD = fmatrixCreate(count, dim);
	for ( frameX=0; frameX<count; frameX++ ) {
            for (i=0; i<dim; i++) matD->matPA[frameX][i] = matM->matPA[frameX][i];
	}

	if(feFMatrix( FS, destin, matD, 1)) {
	    fmatrixFree(matD); return TCL_ERROR;
	}
	fmatrixFree(matM);
	destin->samplingRate = source->samplingRate;
	destin->shift        = source->shift;

	return TCL_OK;
}

static int fesAccumulateMatrixItf( ClientData cd, int argc, char *argv[] ) {
    FeatureSet  *FS = (FeatureSet*)cd;
    int dst1 = 0, dst2 = 0, src = 0, wf = 0;
    Feature     *destin1 = NULL;
    Feature     *destin2 = NULL;
    Feature     *source = NULL;
    Feature     *wfeat  = NULL;
    int         i, j, dim, frameX, frameN;
    FMatrix     *matD1 = NULL;
    FMatrix     *matD2 = NULL;
    FMatrix     *matSPEECH = NULL;
    FMatrix     *matSOURCE = NULL;
   
    /* ------------------
       get arguments
       ------------------ */
    argc--;
    if (itfParseArgv( argv[0], &argc, argv+1, 0,
        "<feature>",        ARGV_CUSTOM, createFeI, &dst1, cd, NEW_FEAT,
        "<feature>",        ARGV_CUSTOM, createFeI, &dst2, cd, NEW_FEAT,
        "<source>",         ARGV_CUSTOM, getFeI,    &src, cd, "adjust this feature",
        "-silence",         ARGV_CUSTOM, getFeI,    &wf,  cd, "silence frames marked with 1, otherwise 0",
    NULL) != TCL_OK) return TCL_ERROR;
       
    destin1 = FS->featA + dst1;
    destin2 = FS->featA + dst2;
    source  = FS->featA + src;
    if (wf > 0) wfeat = FS->featA + wf;
   
    if (wfeat > 0) matSPEECH = wfeat->data.fmat;
    matSOURCE = source->data.fmat;
    frameN = matSOURCE->m;
    dim = matSOURCE->n;

    matD1 = fmatrixCreate(dim, dim);
    matD2 = fmatrixCreate(dim, dim);

    for (j=0; j<dim; j++) for (i=0; i<dim; i++) {
        matD1->matPA[j][i] = 0.0;
        matD2->matPA[j][i] = 0.0;
    }

    for ( frameX=0; frameX<(frameN-1); frameX++ ) {
        if (wfeat > 0) {
            float *SILENCE = matSPEECH->matPA[frameX];
            float *SILENCENEXT = matSPEECH->matPA[frameX+1];
            float *SOURCE = matSOURCE->matPA[frameX];
            float *SOURCENEXT = matSOURCE->matPA[frameX+1];
            if ( (SILENCE[0]+SILENCENEXT[0])>1.9 ) {
                for (j=0; j<dim; j++)
                    for (i=0; i<dim; i++) {
                        matD1->matPA[j][i] += SOURCENEXT[i] * SOURCE[j];
                        matD2->matPA[j][i] += SOURCENEXT[i] * SOURCENEXT[j];
                    }
            }   
        } else {
            float *SOURCE = matSOURCE->matPA[frameX];
            float *SOURCENEXT = matSOURCE->matPA[frameX+1];
            for (j=0; j<dim; j++)
            for (i=0; i<dim; i++) {
                matD1->matPA[j][i] += SOURCENEXT[i] * SOURCE[j];
                matD2->matPA[j][i] += SOURCENEXT[i] * SOURCENEXT[j];
            }
        }
    }


    if(feFMatrix( FS, destin2, matD2, 1)) {
        fmatrixFree(matD2); return TCL_ERROR;
    }
    destin2->samplingRate = source->samplingRate;
    destin2->shift        = source->shift;
   
    if(feFMatrix( FS, destin1, matD1, 1)) {
        fmatrixFree(matD1); return TCL_ERROR;
    }

    destin1->samplingRate = source->samplingRate;
    destin1->shift        = source->shift;

    return TCL_OK;
}

/* ------------------------------------------------------------------------
   Method 'linear prediction matrix'
   ------------------------------------------------------------------------ */
static int fesPredictionMatrixItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin = NULL, *source = NULL;
  Feature     *wfeat  = NULL;
 
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<feature>", ARGV_CUSTOM, createFe,   &destin, cd, NEW_FEAT,
    "<source>",  ARGV_CUSTOM, getFe,      &source, cd, SRC_FEAT,
    "-weight",   ARGV_CUSTOM, getFe,      &wfeat,  cd, "feature that weights each frame when mean is calculated",
     NULL) != TCL_OK) return TCL_ERROR;

  /* -------------------
     FLOAT features
     ------------------- */
  if (IS_FMATRIX( source)) {
     FMatrix      *mat1 = source->data.fmat;
     int frameN = mat1->m;
     int coeffN = mat1->n;
     FMatrix     *mat;

     mat = fmatrixCreate(frameN,coeffN); if (!mat) return TCL_ERROR;

     if(!feFMatrix( FS, destin, mat, 0)) {
	COPY_CONFIG( destin, source); return TCL_OK;
     }
     fmatrixFree(mat); return TCL_ERROR;
  }	 
  MUST_FMATRIX(source);
  return TCL_ERROR;
}

static int fesGetGaussianItf( ClientData cd, int argc, char *argv[] )
{
    FeatureSet  *FS = (FeatureSet*)cd;
    Feature     *destin = NULL;
    Feature     *spec = NULL;
    DistribSet  *dsP;
    int         dim, frameX, frameN, frameA, i;
    FMatrix     *matD = NULL;
    FMatrix     *matSpec = NULL;
    FMatrix     *matV = NULL;
    Feature     *variance = NULL;
    Feature     *wfeat  = NULL;
    FMatrix     *matSPEECH = NULL;
    GaussianMixture**   gmms = 0;
    GaussianMixture*    generalSpeechGmm = 0;
    float*      mean;
    float*      covariance;
    float       score[64];
    float       scoremin;
    int         wheremin;
    float       temp;
    int         k;
    Distrib*    dP;
    int         foundSpeech = 0;
    int         nDistribs;
    char        *model  = "classes";

    /* ------------------
    get arguments
    ------------------ */
    argc--;
    if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<feature_out>",    ARGV_CUSTOM,    createFe,   &destin,    cd, NEW_FEAT,
    "<feature_in>",     ARGV_CUSTOM,    getFe,      &spec,      cd, "feature to be cleaned",
    "distribSet",       ARGV_OBJECT,    NULL,       &dsP,       "DistribSet", "enter the DistribSet here (CodebookSet is also loaded)",
    "-gmm",             ARGV_STRING,    NULL,       &model,     cd, "name of gmm to be used",
	NULL) != TCL_OK) return TCL_ERROR;
    assert (variance); // XCode says could be NULL
    matSpec = spec->data.fmat;
    matV   = variance->data.fmat;
    frameN = matSpec->m;
    dim = matSpec->n;
    frameA = 0;
    if (wfeat > 0) matSPEECH = wfeat->data.fmat;
    matD = fmatrixCreate(frameN, 1);

    extractGaussianMixturesFromDistribSet(dsP, &gmms);

    nDistribs = dsP->list.itemN;
    for (i=0; i<nDistribs; i++) {
        dP = &(dsP->list.itemA[i]);
            if (strcmp(dP->name,model)==0) {
                foundSpeech = 1;
                generalSpeechGmm = gmms[i];
            }
    }
    if (foundSpeech == 0) fprintf(stderr,"[ERROR] can't find classes CB\n");

    foreach_frame {
	float *SPEC = matSpec->matPA[frameX];
	scoremin = 10000.0;
	wheremin = 0;
    	for (i=0; i<generalSpeechGmm->nMixtures; i++) {
            mean =generalSpeechGmm->gaussians[i]->mean;
            covariance =generalSpeechGmm->gaussians[i]->covariance;
	    score[i] = 0.0;
	    for (k=0; k<dim; k++) {
		temp = mean[k] - SPEC[k];
		score[i] += temp*temp/covariance[k];
	    }
	    if (score[i] < scoremin) {
		scoremin = score[i];
		wheremin = i;
	    }
	}
	matD->matPA[0][frameX] = wheremin;
	for (i=0; i<generalSpeechGmm->nMixtures; i++) {
	}
    }

    if(feFMatrix( FS, destin, matD, 1)) {
	fmatrixFree(matD); return TCL_ERROR;
    }
    destin->samplingRate = spec->samplingRate;
    destin->shift        = spec->shift;

    return TCL_OK;
}

static int fesCorrelationMatrixItf( ClientData cd, int argc, char *argv[] )
{
	FeatureSet  *FS = (FeatureSet*)cd;
	Feature     *destin = NULL;
	Feature     *source = NULL;
	int         i, j, dim, frameX, frameN;
	FMatrix     *matM = NULL;
	FMatrix     *matCORR   = NULL; 
	FMatrix     *matSOURCE = NULL; 
	int         norm = 1;

	/* ------------------
	get arguments
	------------------ */
	
	argc--;
	if (itfParseArgv( argv[0], &argc, argv+1, 0,
		"<feature>",        ARGV_CUSTOM, createFe, &destin, cd, NEW_FEAT,
		"<source>",         ARGV_CUSTOM, getFe,    &source, cd, "adjust this feature",
		"-normalize",       ARGV_CUSTOM, NULL,     &norm,   cd, "normalize correlation",
		NULL) != TCL_OK) return TCL_ERROR;
	
        matSOURCE = source->data.fmat;
	frameN = matSOURCE->m;
	dim = matSOURCE->n;
        matCORR = fmatrixCreate(dim, dim);
	matM = fmatrixCreate(2, dim);

        for (i=0; i<dim; i++) for (j=0; j<2; j++) matM->matPA[j][i] = 0.0;
        for (i=0; i<dim; i++) for (j=0; j<dim; j++) matCORR->matPA[j][i] = 0.0;
        for ( frameX=0; frameX<frameN; frameX++ ) {
            float *SOURCE = matSOURCE->matPA[frameX];
            for (i=0; i<dim; i++) matM->matPA[0][i] += SOURCE[i]; 
        }
        for (i=0; i<dim; i++) matM->matPA[0][i] /= frameN;
        for ( frameX=0; frameX<frameN; frameX++ ) {
            float *SOURCE = matSOURCE->matPA[frameX];
            for (i=0; i<dim; i++) matM->matPA[1][i] += (SOURCE[i]-matM->matPA[0][i])*(SOURCE[i]-matM->matPA[0][i]);
        }
        /* for (i=0; i<dim; i++) matM->matPA[1][i] /= frameN; */

	for ( frameX=0; frameX<frameN; frameX++ ) {
	    for (i=0; i<dim; i++) {
		for (j=0; j<dim; j++) {
		    float *SOURCE = matSOURCE->matPA[frameX];
		    matCORR->matPA[j][i] += (SOURCE[i]-matM->matPA[0][i])*(SOURCE[j]-matM->matPA[0][j]);
		}
	    }
	}
	if (norm > 0.5) {
            for (i=0; i<dim; i++) {
	        for (j=0; j<dim; j++) {
		    matCORR->matPA[j][i] /= (sqrt(matM->matPA[1][i]*matM->matPA[1][j]));
	        }
	    }
	}

	if(feFMatrix( FS, destin, matCORR, 1)) {
		fmatrixFree(matCORR); return TCL_ERROR;
	}
	fmatrixFree(matM);
	destin->samplingRate = source->samplingRate;
	destin->shift        = source->shift;

	return TCL_OK;
}

static int fesCholeskyDecompItf( ClientData cd, int argc, char *argv[] )
{
	FeatureSet  *FS = (FeatureSet*)cd;
	Feature     *destin = NULL;
	Feature     *source = NULL;
	int         i, j, dim, frameN;
	FMatrix     *matM = NULL;
	FMatrix     *matCHOL   = NULL; 
	FMatrix     *matSOURCE = NULL; 
	
	/* ------------------
	get arguments
	------------------ */
	
	argc--;
	if (itfParseArgv( argv[0], &argc, argv+1, 0,
		"<feature>",        ARGV_CUSTOM, createFe, &destin, cd, NEW_FEAT,
		"<source>",         ARGV_CUSTOM, getFe,    &source, cd, "adjust this feature",
		NULL) != TCL_OK) return TCL_ERROR;
	
        matSOURCE = source->data.fmat;
	frameN = matSOURCE->m;
	dim = matSOURCE->n;
        matCHOL = fmatrixCreate(dim, dim);

        for (i=0; i<dim; i++) for (j=0; j<dim; j++) matCHOL->matPA[j][i] = 0.0;

	CholeskyDecomp(matSOURCE->matPA , matCHOL->matPA, dim-1);

	if(feFMatrix( FS, destin, matCHOL, 1)) {
		fmatrixFree(matCHOL); return TCL_ERROR;
	}
	fmatrixFree(matM);
	destin->samplingRate = source->samplingRate;
	destin->shift        = source->shift;

	return TCL_OK;
}

static int toepsolve(double *hinv, double *r, double *q, int order){

    DMatrix *matA = NULL;
    double alpha,beta;
    int c,d,temp,j,k;

    matA = dmatrixCreate(order+1,order+1);
    double **a = matA->matPA;

    a[0][0] = 1.0;

    hinv[0] = q[0]/r[0];
    
    alpha=r[0];
    c=0;
    d=1;
    
    for (k = 1; k < order; k++) {
        a[k][c] = 0.0;
        a[0][d] = 1.0;
        beta = 0.0;
        for (j = 1; j <= k; j++) beta += r[k+1-j]*a[j-1][c];
        beta /= alpha;
        for (j = 1; j <= k; j++) a[j][d] = a[j][c] - beta*a[k-j][c];
        alpha *= (1 - beta*beta);
        hinv[k] = q[k];
        for (j = 1; j <= k; j++) hinv[k] -= r[k+1-j]*hinv[j-1];
        hinv[k] /= alpha;
        for (j = 1; j <= k; j++) hinv[j-1] += a[k+1-j][d]*hinv[k];
        temp=c;
        c=d;
        d=temp;
    }

    dmatrixFree(matA);

    return TCL_OK;
}

static int fesMultiSpepLPItf( ClientData cd, int argc, char *argv[] )
{
	FeatureSet  *FS = (FeatureSet*)cd;
	Feature     *destin = NULL;
	Feature     *source = NULL;
	FMatrix     *matM = NULL;
	SVector    *X = NULL;
	double     *R = NULL;
	double     *Q = NULL;
	double     *filtercoeff = NULL;
	double     temp;
	int        delay = 500;
	int        frameX;
	int        order = 500;
	int        i,ii;

	/* ------------------
	get arguments
	------------------ */
	
	argc--;
	if (itfParseArgv( argv[0], &argc, argv+1, 0,
		"<feature>",        ARGV_CUSTOM, createFe,  &destin, cd, NEW_FEAT,
		"<source>",         ARGV_CUSTOM, getFe,     &source, cd, "adjust this feature",
    		"-delay",           ARGV_INT,  NULL,        &delay,  cd, "delay of linear prediction",
    		"-order",           ARGV_INT,  NULL,        &order,  cd, "model order",
		NULL) != TCL_OK) return TCL_ERROR;

	R = (double*) malloc((unsigned)((order+1)*sizeof(double)));
	Q = (double*) malloc((unsigned)((order+1)*sizeof(double)));
	filtercoeff = (double*) malloc((unsigned)((order+1)*sizeof(double)));

        X = source->data.svec;
	frameX = X->n;

	FMatrix *filter = fmatrixCreate(1,order);

	fprintf(stderr,"samples: %i \n", frameX);

  	for (i=0;i<=order;i++) {
	    temp=0.0;
 	    for (ii=0;ii<frameX-i-delay;ii++) { temp += (double) (X->vecA[ii]*X->vecA[ii+i]); }
	    R[i] = temp;
	}

  	for (i=0;i<=order;i++) {
	    temp=0.0;
	    for (ii=0;ii<frameX-i-delay;ii++) { temp += (double) X->vecA[ii]*X->vecA[ii+i+delay]; }
	    Q[i] = temp;
	}

	toepsolve(filtercoeff, R, Q, order);

	for (i=0;i<order;i++) {
	    filter->matPA[0][i] = - (float) filtercoeff[i];
	}

	if(feFMatrix( FS, destin, filter, 1)) {
		fmatrixFree(filter); return TCL_ERROR;
	}
	fmatrixFree(matM);
	destin->samplingRate = source->samplingRate;
	destin->shift        = source->shift;

	free(R);
	free(Q);
	free(filtercoeff);

	return TCL_OK;
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method featureMethod[] = 
{ 
  { "puts", fePutsItf, NULL },
  {  NULL,     NULL, NULL } 
};

TypeInfo featureInfo = { "Feature", 0, 0, featureMethod, 
                          NULL, NULL,
                          feConfigureItf, feAccessItf, 
                          itfTypeCntlDefaultNoLink,
	                 "Feature\n" } ;

Method featureSetMethod[] = 
{
  /* basic methods */
  { "puts",    fesPutsItf, NULL },
  { "delete",  fesDeleteItf,  "delete a feature" },
  { "name",    fesGetNameItf, "get feature name for a given index" },
  { "index",   fesGetIndexItf,"get feature index for a given name" },
  { "display", fesDisplayItf, "displays a feature" },
  { "SVector", fesSVectorItf, "insert SVector type object into feature set" },
  { "FMatrix", fesFMatrixItf, "insert FMatrix type object into feature set" },

  /* JANUS interface methods */
  { "setDesc", fesSetDescItf,"read a 'Feature Description'" },
  { "setAccess",fesSetAccItf,"read a 'File Access Description'" },
  /* the next 2 methods are defined only to test the Janus c-interface */
  { "frame",   fesFrameItf,"return frame of a feature given a featureSet frame index" },
  { "frameN",  fesFrameNItf,"return featureSet frame number given a list of features" },
   
  /* added by Friedrich Faubel (FF ADD) */ 
  { "appendAESVM", fesAppendAESVMItf, "append an ASCII encoded short vector message to an SVector feature" },

  /* ADC or ADC related functions */
  { "readADC", fesReadADCItf, "read ADC file" },
  { "writeADC",fesWriteADCItf,"write ADC file" },

#ifdef PORTAUDIO
  { "paInfo",            fesPAInfoItf,           "info on status, devices, ..." },
  { "paStartRecording",  fesPAStartRecordingItf, "start PortAudio recording" },
  { "paGetRecorded",     fesPAGetRecordedItf,    "get recorded PortAudio data" },
  { "paPlay",            fesPAPlayItf,           "start PortAudio playing" },
  { "paStop",            fesPAStopItf,           "stop PortAudio playing" },
#endif

  { "audioInit",  fesAudioInitItf,"init audio device" },
  { "recordStart",fesRecordStartItf,"start audio recording (see also 'recordGet')" },
  { "recordGet",  fesGetRecordItf,"get new audio data after starting with 'recordStart'" },
  { "play",       fesPlayItf,"play audio" },

  { "adc2pow", fesADCItf,     "framebased power" },
  { "peak",    fesADCItf,     "framebased peak distance" },
  { "maxpeak", fesADCItf,     "framebased maximum of peak to peak" },
  { "zero",    fesADCItf,     "framebased zero crossing rate / sec" },
  { "silTK",   fesSilTKItf,   "T.Kemp's silence feature" },
  { "adc2spec",fesADC2SPECItf,"framebased spectral analysis" },
  { "spec2adc",fesSPEC2ADCItf,"audio signal reconstruction from spectrum" },
  { "spectrum",fesADCItf,     "framebased power spectrum" },
  { "adc2mel", fesADCItf,     "16 framebased melscale coefficients, 8 and 16 kHz only" },

  { "findpeaks", fesPeakItf, "framebased peak tracker" },
  { "corr",    fesCorrItf,    "correlation of two features" },
  { "puls",    fesPulsItf,    "create puls in signals" },
  { "tone",    fesToneItf,    "create audio signals" },
  { "noise",   fesNoiseItf,   "create noise signal" },
  { "xtalk",   fesXTalkItf,   "remove crosstalk with an adaptive filter" },
  { "resample",fesResADCItf,  "resample audiosignal changing sampling rate" },
  { "downsample",fesDownADCItf, "downsample from 16kHz to 8kHz telephone quality" },

  /* Methods for wavelet transformation - Michael Wand */
  { "fwt",      fesFWTItf,      "perform the fast wavelet tranformation, real decimated case" },
  { "dtcwt",    fesDTCWTItf,    "perform the dual-tree complex wavelet transform according to Kingsbury"},
  { "rdwt",     fesRDWTItf,     "perform the redundant discrete wavelet transform "},

  /* Methods originally from by Yue Pan (?) */
  { "fft2",    fesADCItf,     "framebased complex spectrum" },
  { "conv",    fesConvItf,    "convolution with an impulse response" },
  { "mix",     fesMixItf,     "mix with a new signal" },
  { "mixn",    fesMixNItf,    "mix n signals" },
  { "impulse", fesADCItf,     "framebased impulse reponse" },

  /* spectrum, filterbank methods */
  { "melscale",fesMelItf,     "melscale from power spectrum" },
  { "plp",     fesPLPItf,     "perceptual linear prediction" },
  { "auditory",fesAudItf,     "auditory filterbank" },
  { "postaud", fesPaudItf,    "post processing for auditory filterbank" },

  /* stuff for Matthias Woelfel diploma thesis */
  { "varss",             fesVarSSItf,               "variance of the speech signal" },
  { "specest",           fesSpecEstItf,             "spectral estimation: lp wlp, mvdr or wmvdr" },
  { "specadj",           fesSpecAdjustItf,          "adjust first spectrum to max of second spectrum" },
  { "specsublog",        fesSpecSubLogItf,          "logarithmic spectral subtraction (log10)" },
  { "specsublog1",       fesSpecSubLog1Itf,         "logarithmic spectral subtraction (log1)" },
  { "traceScatter",      fesTraceItf,               "return the trace of the scatter matrix" },
  { "particlefilter",    fesParticleFilterItf,      "partilce filter spectral enhancement" },
  { "predictionmatrix",  fesPredictionMatrixItf,    "linear prediction matrix" },
  { "predictionvariance",  fesCalculateVarianceItf, "calculate variance" },
  { "getdeltasamples",   fesGetDeltaSamplesItf,     "get delta of samples 4 correlated random variables" },
  { "accumulatematrix",  fesAccumulateMatrixItf,    "accumulate matrix" },
  { "getgaussian",       fesGetGaussianItf,         "return the gaussian with the highest likelihood" },
  { "corrMatrix",        fesCorrelationMatrixItf,   "correlation matrix of features" },
  { "CholeskyDecomp",    fesCholeskyDecompItf,      "calculates the Cholesky Decomposition" },
  { "multisteplp",       fesMultiSpepLPItf,        "calculate multi step linear prediction coefficients" },
  { "changesub",         fesChangeSpecSubItf,       "change scale of noise for spectral subtraction" },
  /* { "vts",               fesVTSItf,                 "acoustic mapping using the VTS approximation" }, */

  /* formants methods*/
  { "autocorr",fesAutoCorItf, "auto correlation" },
  { "lpc",     fesLPCItf,     "linear predictive coding" },
  { "formants",fesFormantsItf,"extract fromants from lpc" },
  { "distance",fesDistanceItf,"frame based distance" },

  /* feature warpping based on CDF matching for robust speaker recognition */
  { "QWarp",   fesQWarpItf,   "feature warping based on CDF matching"},

  /* feature file I/O */  
  { "write",   fesWriteItf,   "write feature file" },
  { "read",    fesReadItf,    "read feature file" },
  { "readhtk", fesHtkReadItf, "read HTK feature file"},

  /* methods using one source */ 
  { "split",   fesSplitItf,   "take coefficients <from> .. <to> of source feature" },
  { "cut",     fesCutItf,     "take frames <from> .. <to> of source feature" },
  { "replace", fesReplaceItf, "replace frames starting at <from> of source feature" },
  { "append",  fesAppendItf,  "append frames/ coefficients to the source feature" },

  { "flip",    fesFlipItf,    "take last frames first" },
  { "delta",   fesDeltaItf,   "symmetrical delta coefficients: x(t+delta) - x(t-delta)" },
  { "shift",   fesDeltaItf,   "shift frames: x(t+delta)" },
  { "adjacent",fesDeltaItf,   "put adjacent frames together: x(t-delta), x(t+1-delta), ..., x(t+delta)" },

  { "lin",     fesFunItf,     "m * source_feature + a" },
  { "log",     fesFunItf,     "m * log(source_feature + a)" },
  { "alog",    fesFunItf,     "m * log(source_feature + b) with b=max/10^a" },
  { "exp",     fesFunItf,     "m * exp(a * source_feature)" },
  { "pow",     fesFunItf,     "m * (source_feature ^ a)" },
  { "maxarg",  fesMaxargItf,  "index of maximum value per frame" },
  { "meanarg", fesMeanargItf, "mean index per frame" },

  { "aspike" , fesAntiSpikeItf, "remove spikes from signal"},
  { "offset" , fesOffsetItf,    "remove offset adaptively from signal"},

  { "filter",  fesFilterItf,  "filter a feature" },
  { "VTLN",    fesVTLnItf,    "Vocal Tract Length Normalization (VTLN)" },
  { "normframe",fesNormFrameItf,"normalize each frame" },
  { "mean",    fesMeanItf,    "calculate mean and variance" },
  { "meansub", fesMeansubItf, "meansubtraction and variance normalisation" },
  { "MVN",     fesMVNItf,     "mean and variance normalisation with exponential weighting (by FF)" },
  { "normalize",fesNormItf,   "normalize coefficients to range <min> .. <max>" },
  { "thresh",  fesThreshItf,  "set coefficients to a specified value if they exceed a threshold" },

  { "beepSeg", fesSegItf,     "segment (spectral) feature at beeper positions" },
  { "silSeg",  fesSegItf,     "segment (spectral) feature at silence positions" },

  { "EFVR",    fesEFVRItf,    "Early Feature Vector Reduction"}, 

  /* functions using more than one source */
  { "add",     fesAddItf,     "add two features: a * featureA + b * featureB" },
  { "mul",     fesMulItf,     "multiply two features: a * featureA * featureB" },
  { "matmul",  fesMatmulItf,  "multiply matrix A with each frame x of feature: A * x"},
  { "filterbank",fesFilterbankItf,"multiply band matrix A with each frame x of feature: A * x"},
  { "concat",  fesConcatItf,  "concat frames (or samples) of features" },
  { "merge",   fesMergeItf,  "merge coefficients (interleave samples) of features" },

  /* pitch detection (Kjell Schubert) */
  /*
  { "pitch",    fesPitchItf,     "calculate pitch"},
  { "crossCorr",fesCrossCorrItf, "calculate cross correlation"},
  */

  /* functions for modalities */
  { "snr",      fesSNRItf,      "signal to noise ratio of feature" },
  { "snrK",     fesSNRKItf,     "signal to noise ratio of feature (kmeans)" },
  { "avMagnitude", fesAvMagnitudeItf, "frame based average magnitude" },
  { "zeroX",    fesZeroXItf,    "frame based zero crossing" },
  { "power",    fesPowerItf,    "frame based power" },
  { "reorder",  fesReorderItf,  "reorder entries in feature" },
  { "gradient", fesGradientItf,  "compute gradients for a given window length" },

  { "silDetCF",  fesSilenceDetectionItf,  "Christian Fuegen's Silence Detection" },

  /* functions for signal adaption (MAM) */
  { "noiseest", fesNoiseEstimationItf, "estimate the noise in a given signal" },
  { "noisered", fesNoiseReductionItf, "Ephraim and Malah Noise Reduction (additive noise reduction)" },
  { "specsub",  fesSpecSubItf, "Spectral Subtraction after Boll (additive noise reduction)" },
  { "map",      fesMapItf,    "acoustic mapping" },

  { "compress", fesCompressItf, "compress float features to 8bit values" },


  { NULL,      NULL, NULL } 
};

TypeInfo featureSetInfo = { "FeatureSet", 0, 0, featureSetMethod, 
                             fesCreateItf, fesFreeItf, 
                             fesConfigureItf, fesAccessItf, NULL,
		            "set of features\n" } ;

char *featureTcl = "\n\
proc featureSetAccess { fes {sampleLst {}}} {\n\
 set accessLst \"\"\n\
 makeArray arg $sampleLst\n\
 set accessLst $sampleLst\n\
 return $accessLst\n\
}\n\
\n\
proc featureSetEval { fes {sampleLst {}}} {\n\
 set sampleLst [$fes access $sampleLst]\n\
 makeArray arg $sampleLst\n\
 $fes read FEAT $arg(FEATFILE)\n\
}\n\
\n\
FeatureSet method access featureSetAccess -text \"preprocess feature evaluation parameters\"\n\
FeatureSet method eval featureSetEval -text \"run feature description script\"\n\
FeatureSet method show featshow -text \"show feature set\"";

int Feature_Init ( void)
{
   static int featureInitialized = 0;

   if (! featureInitialized) {

     itfNewType ( &featureInfo );
     itfNewType ( &featureSetInfo );
     Filter_Init ( );

     Tcl_Eval( itf, featureTcl);

     featureInitialized = 1;
   }
   return TCL_OK;
}
