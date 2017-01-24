/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Feature Module
               ------------------------------------------------------------

    Author  :  Martin Westphal
    Module  :  featureJanus.c
    Date    :  $Id: featureJanus.c 2779 2007-03-02 13:06:36Z fuegen $
    Remarks :  The functions defined here are used by other Janus objects.

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
    Revision 3.4  2007/03/02 13:06:36  fuegen
    added code for PORTAUDIO support (Florian Metze, Friedrich Faubel)

    Revision 3.3  1997/08/11 15:29:30  westphal
    itf methods frame frameN, removed bugs in c-interface

    Revision 3.2  1997/07/09 17:42:22  westphal
    *** empty log message ***


   ======================================================================== */
#include "featureMethodItf.h"
/* ------------------------------------------------------------------------
    fesIndex

    This routine is called to lock a feature (increase 'useN') and to make
    sure it has and will have the desired 'type' and number of coefficients
    'coeffN'.
    
    Returns the index of the feature given in 'name' or a negative value
    if feature couldn't be found (only for create=0 !).
    The return value is also negativ if the feature exists and doesn't match
    the conditions or the conditions or already set differently.

    If the calling function doesn't need the feature any more 'useN' should
    be decreased.
    ------------------------------------------------------------------------ */

int fesIndex( FeatureSet* FS, char* name, FeatureType type,
	      int coeffN, int create)
{
  int i = fesGetIndex( FS, name, create);
  if (i < 0) return -1;

  /* ----- feature has data: matching conditions? ----- */
  if (FS->featA[i].type != FE_UNDEF) {
    if (FS->featA[i].type != type) {
      SERROR("Feature '%s' is of type %d, not %d.\n",
	     name, FS->featA[i].type, type);
      return -1;
    }
    /* check for all types if dimension is ok */ 
    if (FS->featA[i].type == FE_FMATRIX) {
      if (FS->featA[i].data.fmat->n != coeffN) {
	SERROR("Feature '%s' has %d coefs, not %d.\n",
	       name, FS->featA[i].data.fmat->n, coeffN);
	return -1;
      }
    }
    /* ... */
  }

  /* ----- feature has no data but is already used: equal settings? ----- */
  else if (FS->featA[i].useN > 0  &&  FS->featA[i].type == FE_UNDEF) {
    if (FS->featA[i].dtype != FE_UNDEF  &&  FS->featA[i].dtype != type) {
      SERROR("Feature '%s' must be of type %d, not %d.\n",
	      name, FS->featA[i].dtype, type);
      return -1;
    }
    if (FS->featA[i].dcoeffN > 0  && FS->featA[i].dcoeffN != coeffN) {
      SERROR("Feature '%s' must have %d coefs, not %d.\n",
	      name, FS->featA[i].dcoeffN, coeffN);
      return -1;
    }
  }

  /* ----- everything is ok ----- */ 
  FS->featA[i].dcoeffN = coeffN;
  FS->featA[i].dtype   = type;
  FS->featA[i].useN++;

  return i;
}

/* ------------------------------------------------------------------------
    fesFrame

    Returns a pointer to the 'frame' of a feature with the given 'index'.
    The frame shift of the featureSET determines the 'index'. If the 
    feature frame shift differs frames might be skipped or returned more 
    than once. Example:

    from = 2            |------>
    featureSet (10ms):          0   1   2   
    feature 1  (10ms):  0   1   2   3   4   .
    feature 2  (20ms):  0       1       2       .
    feature 3  ( 5ms):  0 1 2 3 4 5 6 7 8   .

    index 1 will return frame #3 of feature 1, frame #1 of feature 2
    and frame #6 of feature 3
   ------------------------------------------------------------------------ */

float *fesFrame( FeatureSet* FS, int index, int frame)
{
  Feature *F = FS->featA + index;

  if ( (F->type == FE_FMATRIX) &&  F->data.fmat) {

    if (F->shift) frame  = (FS->sfshift * (frame + FS->from)) / F->shift;
    else          frame +=  FS->from;

    assert(FS->from >= 0);
    if (frame >= 0 && frame < F->data.fmat->m)
      return F->data.fmat->matPA[frame];

  }
  return NULL;
}

/* ------------------------------------------------------------------------
   Method 'frame'
   ------------------------------------------------------------------------ */
int fesFrameItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet* FS     = (FeatureSet*)cd;
  Feature*    source = NULL;
  int         src    = 0;
  int         frame  = 0;
  char*       format = "% 2.6e";
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<source_feature>", ARGV_CUSTOM, getFeI, &src,   cd, SRC_FEAT,
      "<frame>",          ARGV_INT,    NULL,   &frame, cd, "featureSet frame index",
      "-format",          ARGV_STRING, NULL,   &format,cd, "format string",
      NULL) != TCL_OK) return TCL_ERROR;

  if (frame<0) {
    ERROR("frame index must be > 0 but is %d\n", frame);  
    return TCL_ERROR;
  }
  /* -------------------
     FLOAT features
     ------------------- */
  source = FS->featA + src;
  if (IS_FMATRIX(source)) {
     int    coeffN = source->data.fmat->n;
     float* frameP = fesFrame(FS, src, frame);
     int    coeffX;

     if (frameP) {
       for (coeffX=0; coeffX<coeffN; coeffX++) {
	 if (coeffX) itfAppendResult(" ");
	 itfAppendResult(format, frameP[coeffX]);
       }
     } else {
       ERROR("No frame %d for feature %s\n", frame, source->name);  
       return TCL_ERROR;
     }
     return TCL_OK;
  }	 
  MUST_FMATRIX(source); return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    fesFrameN

    Returns the number of frames for recognition. Frame requests from
    0 .. frameN-1 with fesFrame() should return a valid float pointer.
   ------------------------------------------------------------------------ */

int fesFrameN( FeatureSet* FS, int* list, int  listN, 
                               int* from, int* frameshift, int* ready)
{
  assert(FS);
  if ( from)       *from       = FS->from;
  if ( frameshift) *frameshift = FS->sfshift;
  if ( ready)      *ready      = FS->runon ? FS->ready : 1;
  if ( list && listN) {
    int frames,   i;
    int frameN = -1;

    for( i = 0; i < listN; i++) {
      Feature* F = FS->featA + list[i];
      FMatrix* m;

      if (!F || (F->type != FE_FMATRIX) || !(m = F->data.fmat)) 
	return 0;
      if (F->shift && FS->sfshift) {
	float dur = F->shift * m->m;
	frames = ceil(dur / FS->sfshift) - FS->from;
      } else 
	frames = m->m - FS->from;

      if ((frameN < 0) || (frames < frameN))
	frameN = frames;
    }
    return (frameN < 0) ? 0 : frameN;
  }
  return 0;
}

/* ------------------------------------------------------------------------
   Method 'frameN'
   ------------------------------------------------------------------------ */
int fesFrameNItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet* FS = (FeatureSet*)cd;
  int*        source;
  int         sourceN;
  int         i0;
  int         from, shift, ready;
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
      "<feature*>",    ARGV_REST,   NULL,    &i0,     NULL,"list of features",
      NULL) != TCL_OK) return TCL_ERROR;
  sourceN = argc - i0;

  source = malloc(sourceN*sizeof(int));
  if (source) {
    int i;
    for ( i=0; i<sourceN; i++) {
      i0++;
      source[i] = fesGetIndex(FS, argv[i0], 0);
    }
    itfAppendResult("%d", fesFrameN(FS, source, sourceN, &from, &shift, &ready));
    free(source);
    return TCL_OK;
  }
  return TCL_ERROR;
}


