/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Senones
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  senones.c
    Date    :  $Id: senones.c 3428 2011-04-15 18:00:28Z metze $
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
    Revision 3.22  2007/03/02 13:14:37  fuegen
    support for opt_str score function in snsClearStreamCacheItf (Florian Metze)

    Revision 3.21  2005/12/20 15:14:49  metze
    Code for AF training (snStreamWeights.c|h)

    Revision 3.20  2005/03/07 19:31:28  metze
    Added MICV support and MTHREAD define check

    Revision 3.19  2004/09/10 15:38:28  metze
    Added base_semCont

    Revision 3.18  2004/08/27 14:26:30  metze
    Updated message

    Revision 3.17  2004/08/27 13:16:00  metze
    Added opt_semCount

    Revision 3.16  2003/08/14 11:20:18  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.15.4.40  2003/07/02 17:46:29  fuegen
    free -> TclFree

    Revision 3.15.4.39  2003/07/02 08:04:49  metze
    Fixed bug in streamCache allocation

    Revision 3.15.4.38  2003/06/12 15:56:17  metze
    snsName now works properly

    Revision 3.15.4.37  2003/06/03 15:46:06  metze
    Checked for NULL pointer

    Revision 3.15.4.36  2003/06/03 15:41:03  metze
    Added snsIndexItf

    Revision 3.15.4.35  2002/11/25 17:37:45  metze
    Made 256 Gaussians per Codebook possible

    Revision 3.15.4.34  2002/11/21 17:10:34  fuegen
    windows code cleaning

    Revision 3.15.4.33  2002/11/20 13:26:43  soltau
    Removed score cache

    Revision 3.15.4.32  2002/09/11 17:22:33  metze
    Removed warning

    Revision 3.15.4.31  2002/09/09 06:47:13  metze
    MLE stream estimation

    Revision 3.15.4.30  2002/09/06 14:59:31  metze
    Bugfixes in snsWrite, senone2phoneX

    Revision 3.15.4.29  2002/09/02 15:17:11  metze
    Fixed the bugs Hagen left for me to find

    Revision 3.15.4.28  2002/09/02 08:30:57  soltau
    snsUpdateWeightsMLE : ensure positive scores

    Revision 3.15.4.27  2002/08/30 12:32:14  soltau
    Added MLE estimation of stream weights

    Revision 3.15.4.26  2002/08/27 08:41:34  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 3.15.4.25  2002/08/01 13:42:34  metze
    Fixes for clean documentation.

    Revision 3.15.4.24  2002/06/28 16:07:15  soltau
    Added snsIndex

    Revision 3.15.4.23  2002/06/26 15:52:17  metze
    Improved handling of stream-training

    Revision 3.15.4.22  2002/04/22 09:06:31  soltau
    Changed training and smoothing of stream weights, added option for 'probability mass'

    Revision 3.15.4.21  2002/04/12 16:02:21  soltau
    Cleaned interface to stream weights
    Added training and smoothing of stream weights

    Revision 3.15.4.20  2002/04/03 11:07:56  soltau
    setScoreFct : Added thread support

    Revision 3.15.4.19  2002/03/18 17:46:54  soltau
    Added bbi header

    Revision 3.15.4.18  2002/03/13 15:57:15  metze
    More sensible interface to "stream"

    Revision 3.15.4.17  2002/02/27 09:28:51  metze
    ssa_opt_feat allocates smaller cache, uses streamCache->count

    Revision 3.15.4.16  2002/02/26 19:14:16  metze
    streamCache->count initialized

    Revision 3.15.4.15  2002/02/20 10:50:34  soltau
    snsAccu: Added from/to options

    Revision 3.15.4.14  2002/02/15 15:31:13  metze
    Added info for setScoreFct

    Revision 3.15.4.13  2002/02/04 14:24:37  metze
    Changed interface to wordPuts

    Revision 3.15.4.12  2002/01/30 09:28:27  metze
    Added opt_feat score function

    Revision 3.15.4.11  2002/01/18 13:38:20  metze
    Added SenoneSet.streamCache for opt_str score function and dumps

    Revision 3.15.4.10  2001/12/07 16:22:00  metze
    Initial Fix for problems with stream weights
    Can now be set globally, too

    Revision 3.15.4.9  2001/11/06 15:43:52  metze
    scoreScale is now a float

    Revision 3.15.4.8  2001/10/24 15:21:23  soltau
    Added ssa_opt_str

    Revision 3.15.4.7  2001/10/16 11:54:00  soltau
    Introduced CBX for bbi

    Revision 3.15.4.6  2001/07/09 14:40:51  soltau
    Fixed default value in setScoreFct

    Revision 3.15.4.5  2001/07/06 13:12:00  soltau
    Changed compress and scoreFct handling

    Revision 3.15.4.4  2001/07/04 18:11:38  soltau
    Removed ssa_mmx

    Revision 3.15.4.3  2001/07/03 09:44:01  soltau
    Changed score function interface

    Revision 3.15.4.2  2001/06/28 10:48:22  soltau
    Added Load/save routines for binary files to support fast decoder startup

    Revision 3.15.4.1  2001/06/26 19:29:34  soltau
    beautified/fixed a couple of ugly hacker routines

 * Revision 3.9  2000/08/16  11:41:37  soltau
 * score cache checking
 * Free -> TclFree
 * 
    Revision 3.8  2000/07/20 12:42:55  soltau
    Fixed bug in score computation (pattern access)

    Revision 3.7  2000/05/01 21:23:42  soltau
    Fixed bug in score computation (bbi access)

    Revision 3.6  2000/04/28 16:46:52  soltau
    *** empty log message ***

    Revision 3.5  2000/04/14 07:20:12  soltau
    optimized score computation:
    assert global bbi, same feature space over all codebooks

    Revision 3.4  2000/03/03 14:34:32  soltau
    *** empty log message ***

    Revision 3.3  1999/08/11 15:53:24  soltau
    fast score computation

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***


   ======================================================================== */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include "common.h"
#include "itf.h"
#include "array.h"
#include "matrix.h"
#include "senones.h"
#include "codebook.h"
#include "distrib.h"
#include "distribStream.h"
#include "mach_ind_io.h"
#include "bbi.h"
#include "scoreA.h"

#include "snStreamWeights.h"

#ifndef IBIS
#include "micvStream.h"
#endif

char senonesRcsVersion[] = 
           "@(#)1$Id: senones.c 3428 2011-04-15 18:00:28Z metze $";

/* ========================================================================
    Global Declarations
   ======================================================================== */

extern TypeInfo senoneInfo;
extern TypeInfo senoneTagInfo;
extern TypeInfo senoneSetInfo;

/* ========================================================================
    SenoneKey
   ======================================================================== */
/* ------------------------------------------------------------------------
    senoneKeyInit
   ------------------------------------------------------------------------ */

int senoneKeyInit( SenoneKey* skeyP, int streamN, SenoneSet* snsP)
{
  if ( streamN) {
    skeyP->classXA = (int  *)malloc(sizeof(int)   * streamN);
    skeyP->stwA    = (float*)malloc(sizeof(float) * streamN);
  }
  else {
    skeyP->classXA = NULL;
    skeyP->stwA    = NULL;
  }
  skeyP->streamN = streamN;
  skeyP->snsP    = snsP;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    senoneKeyDeinit
   ------------------------------------------------------------------------ */

int senoneKeyDeinit( SenoneKey* skeyP)
{
  if ( skeyP->classXA) { free( skeyP->classXA); skeyP->classXA = NULL; }
  if ( skeyP->stwA)    { free( skeyP->stwA);    skeyP->stwA    = NULL; }

  skeyP->streamN = 0;
  skeyP->snsP    = NULL;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    senoneKeyGetItf
   ------------------------------------------------------------------------ */

int senoneKeyGetItf( SenoneKey* skeyP, char* value)
{
  int     argc;
  char**  argv;

  if ( Tcl_SplitList( itf, value, &argc, &argv) == TCL_OK) {
    int i;

    if ( skeyP->streamN > 0) {
      if ( skeyP->streamN != argc) {
        ERROR("Expected SenoneKey of size %d but got size %d.\n", 
               skeyP->streamN, argc);
        return TCL_ERROR;
      }
    }
    else {
      skeyP->classXA = (int  *)calloc( argc, sizeof(int));
      skeyP->stwA    = (float*)calloc( argc, sizeof(float));
      skeyP->streamN =  argc;
    }
    for ( i = 0; i < argc; i++) {
      int     tokc;
      char**  tokv;

      if ( Tcl_SplitList( itf, argv[i], &tokc, &tokv) == TCL_OK) {
        int    ac = tokc;
        char** av = tokv;
        char*  sn = NULL;
        float  w  = skeyP->snsP->strA.stwA[i];
        int    m  = -1;

        if ( itfParseArgv( argv[0], &ac, av, 0,
            "<model>", ARGV_STRING, NULL, &sn, NULL,"stream object",
            "-weight", ARGV_FLOAT,  NULL, &w,  NULL,"stream weight",
             NULL) != TCL_OK) { 
          Tcl_Free((char*)argv); Tcl_Free((char*)tokv);
          return TCL_ERROR;
        }

        if ( strlen(sn) < 1 || strcmp( sn, "-")) {
          if (     skeyP->snsP->strA.strPA[i]->indexFct)
               m = skeyP->snsP->strA.strPA[i]->indexFct(
                   skeyP->snsP->strA.cdA[i], sn);
          else m = atoi(sn);

          if ( m < 0) {
            ERROR("Model %s for stream %d is not defined.\n",sn,i);
            free((char*)argv); free((char*)tokv);
            return TCL_ERROR;
          }
        }
        skeyP->classXA[i] = m;
        skeyP->stwA[i]    = w;
        Tcl_Free((char*)tokv);
      }
    }
    Tcl_Free((char*)argv);
    return TCL_OK;
  }
  return TCL_ERROR;
}

int getSenoneKey( ClientData cd, char* key, ClientData result,
	          int *argcP, char *argv[])
{
   SenoneKey* skeyP = (SenoneKey*)result;
   
   if ((*argcP < 1) || 
       ( senoneKeyGetItf(skeyP, argv[0]) != TCL_OK)) return -1;
   (*argcP)--; return 0;
}

/* ========================================================================
    Senone
   ======================================================================== */

/* ------------------------------------------------------------------------
    Create Senone Structure:
   ------------------------------------------------------------------------ */

int snInit( Senone* SN, SenoneKey* skeyP)
{
  assert(SN);

  if (skeyP->streamN) {
    SN->classXA = (int  *)malloc(sizeof(int)   * skeyP->streamN);
    SN->stwA    = (float*)malloc(sizeof(float) * skeyP->streamN);

    memcpy( SN->classXA, skeyP->classXA, sizeof(int)   * skeyP->streamN);
    memcpy( SN->stwA,    skeyP->stwA,    sizeof(float) * skeyP->streamN);
  } else {
    SN->classXA  = NULL;
    SN->stwA     = NULL;
  }
  SN->streamN    =  skeyP->streamN;
  SN->name       =  NULL;
  SN->snX        = -1;
  SN->cnt        = 0.0;
  SN->accu       = NULL;
  SN->dccu       = NULL;

  if ( skeyP->snsP) {
    StreamArray* staP = &(skeyP->snsP->strA);
    int          i;
    for ( i = 0; i < skeyP->streamN; i++) {
      if ( SN->classXA[i] > -1 && staP->strPA[i]->nameFct) {
        SN->name = strdup(staP->strPA[i]->nameFct(staP->cdA[i],
                                                  SN->classXA[i]));
        break;
      }
    }
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    snDeinit  deinitialize senone structure
   ------------------------------------------------------------------------ */

int snDeinit (Senone* SN)
{
  assert(SN);

  if (SN->name)    { free( SN->name);    SN->name    = NULL; }
  if (SN->classXA) { free( SN->classXA); SN->classXA = NULL; }
  if (SN->stwA)    { free( SN->stwA);    SN->stwA    = NULL; }
  if (SN->accu)    { free( SN->accu);    SN->accu    = NULL; }
  if (SN->dccu)    { free( SN->dccu);    SN->dccu    = NULL; }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    snHashKey & snHashCmp
   ------------------------------------------------------------------------ */

static long snHashKey( const SenoneKey *key)
{
  unsigned long ret = 0; 
  int           i;

  for (i=0; i < key->streamN; i++) {
    ret = ((ret<<5) | (ret >> (8*sizeof(long)-5))) ^  key->classXA[i];
  }
  return ret;
}

static int snHashCmp( const SenoneKey *key, const Senone *snP)
{
  int i;

  if (snP->classXA[0] != key->classXA[0]) return 0;
  if (snP->streamN    != key->streamN)    return 0;

  for ( i = 1; i < key->streamN; i++) {
    if (snP->classXA[i] != key->classXA[i]) return 0;
  }
  return 1;
}

/* ------------------------------------------------------------------------
    snPutsItf
   ------------------------------------------------------------------------ */

static int snPutsItf (ClientData cd, int argc, char *argv[])
{
  Senone* snP = (Senone*)cd;
  int     streamX;

  itfAppendResult("%s ",snP->name);
  itfAppendResult("{");
  for ( streamX = 0; streamX < snP->streamN; streamX++) 
    itfAppendResult(" %d", snP->classXA[streamX]);
  itfAppendResult("} {");
  for ( streamX = 0; streamX < snP->streamN; streamX++) 
    itfAppendResult(" % 2.4e", snP->stwA[streamX]);
  itfAppendResult("}");
  itfAppendResult(" %2.4f",snP->cnt);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    snConfigureItf     configure a Senone type object
   ------------------------------------------------------------------------ */

static int snConfigureItf (ClientData cd, char *var, char *val)
{
  Senone* snP = (Senone*)cd;
  if (! snP) return TCL_ERROR;
 
  if (! var) {
    ITFCFG(snConfigureItf,cd,"name");
    ITFCFG(snConfigureItf,cd,"streamN");
    ITFCFG(snConfigureItf,cd,"snX");
    return TCL_OK;
  }
  ITFCFG_CharPtr(var,val,"name",    snP->name,    1);
  ITFCFG_Int(    var,val,"streamN", snP->streamN, 1);
  ITFCFG_Int(    var,val,"snX",     snP->snX,     0);

  ERROR( "Unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    snAccessItf
   ------------------------------------------------------------------------ */

static ClientData snAccessItf (ClientData cd, char *name, TypeInfo **ti)
{
  return NULL;
}

/* ------------------------------------------------------------------------
    snAccu   accumulate sufficient statistics for training the senone
   ------------------------------------------------------------------------ */

static int sniAccu (StreamNormItem *item, float score);

int snAccu( SenoneSet* snsP, int snX, int frameX, float factor)
{
  StreamArray* staP = &(snsP->strA);
  Senone*      snP  =   snsP->list.itemA + snX;
  StreamNormItem        *item;
  int          stX,ok,k;

  if ( snX < 0 || snX > snsP->list.itemN-1) {
    WARN("Cannot accumulate senone %d.\n",snX); return TCL_ERROR;
  }

  if (snsP->normalize) {

    /* ------------------------- */
    /*  accu stream normalizers  */
    /* ------------------------- */
    for ( stX = 0; stX < snP->streamN; stX++) {
      int  idx;
      if ((idx = snP->classXA[stX]) >= 0) {
        float  f;

        staP->strPA[stX]->scoreFct( staP->cdA[stX], &idx, &f, 1, frameX);
  
        ok = 0;
        for (k=0; k<snsP->streamNormList.itemN; k++) {
          item = snsP->streamNormList.itemA + k;
          if (item->streamX == stX) {
            ok = 1;
            sniAccu(item,f);
            break;
          }
        }
        if (!ok) { ERROR("Couldn't accu normalizer for stream %d\n",stX); }

      }
    }
    

  } else {

    /* -------------------------------------- */
    /*  accu training pattern in all streams  */
    /* -------------------------------------- */
    for ( stX = 0; stX < snP->streamN; stX++) {
      int  idx;
      if ((idx = snP->classXA[stX]) >= 0) {

        if ( staP->strPA[stX]->accuFct( staP->cdA[stX], idx, frameX, 
                                       factor) != TCL_OK)
  	     SERROR("Can't accumulate stream %d\n",stX);
      }
    }

  }

  return TCL_OK;
}

/* ========================================================================
    SenoneTag
   ======================================================================== */

/* ------------------------------------------------------------------------
    senoneTagInit
   ------------------------------------------------------------------------ */

int senoneTagInit( SenoneTag* ST, char* name)
{
  assert(ST);
  ST->name       =  strdup(name);
  ST->tagA       =  NULL;
  ST->tagN       =  0;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    senoneTagDefine    fills the senone tag array with the stream specific
                       tag ids by calling the tagFct() of each stream
   ------------------------------------------------------------------------ */

int senoneTagDefine( SenoneTag* ST, SenoneSet* snsP)
{
  StreamArray* staP = &(snsP->strA);
  int          N    =   staP->streamN;
  int          n    =   0, i;

  ST->tagA = (int*)malloc( N * sizeof(int));
  ST->tagN =  N;

  for ( i = 0; i < N; i++) {
    int  idx = -1;
    if ( staP->strPA[i]->tagFct) {
      idx = staP->strPA[i]->tagFct ( staP->cdA[i], ST->name);
    }
    if ((ST->tagA[i] = idx) < 0 && N > 1) {
      WARN("SenoneTag '%s' is not defined for stream %d.\n", ST->name, i);
    }
    if ( idx > -1) n++;
  }
  if (! n) {
    ERROR("SenoneTag '%s' is invalid in all streams 0..%d.\n", ST->name, N-1);
    free( ST->tagA);
    return TCL_ERROR;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    senoneTagDeinit  deinitialize senone tag
   ------------------------------------------------------------------------ */

int senoneTagDeinit( SenoneTag* ST)
{
  assert(ST);

  if (ST->name) { free( ST->name); ST->name = NULL; }
  if (ST->tagA) { free( ST->tagA); ST->tagA = NULL; ST->tagN = 0; }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    senoneTagPutsItf
   ------------------------------------------------------------------------ */

static int senoneTagPutsItf (ClientData cd, int argc, char *argv[])
{
  SenoneTag* ST = (SenoneTag*)cd;
  int        i;

  for ( i = 0; i < ST->tagN; i++) itfAppendResult(" %d", ST->tagA[i]);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    senoneTagArrayGetItf
   ------------------------------------------------------------------------ */

int senoneTagArrayGetItf( SenoneTagArray* staP, char* value)
{
  SenoneTagList* stlP;
  int            argc;
  char**         argv;

  assert( staP && staP->snsP);

  stlP = &(staP->snsP->tagList);

  if ( Tcl_SplitList( itf, value, &argc, &argv) == TCL_OK) {
    int i;

    if ( staP->tagN > 0) {
      if ( staP->tagN != argc) {
        ERROR("Expected SenoneTagArray of size %d but got size %d.\n", 
               staP->tagN, argc);
        return TCL_ERROR;
      }
    }
    else {
      staP->tagA = (int*)calloc( argc, sizeof(int));
      staP->tagN =  argc;
    }

    for ( i = 0; i < argc; i++) {
      char* name = argv[i];
      int   idx  = listIndex((List*)stlP,(ClientData)name,0);

      if (idx < 0) {
        MSGCLEAR(NULL);

        idx = listNewItem((List*)stlP, (ClientData)name);

        if ( senoneTagDefine( stlP->itemA + idx, staP->snsP) != TCL_OK) {
          Tcl_Free((char*)argv);
          return TCL_ERROR;
        }
      }
      staP->tagA[i] = idx;
    }
    Tcl_Free((char*)argv);
    return TCL_OK;
  }
  return TCL_ERROR;
}

int getSenoneTagArray( ClientData cd, char* key, ClientData result,
	               int *argcP, char *argv[])
{
   SenoneTagArray* staP = (SenoneTagArray*)result;
   
   if ((*argcP < 1) || 
       ( senoneTagArrayGetItf(staP, argv[0]) != TCL_OK)) return -1;
   (*argcP)--; return 0;
}


/* ========================================================================
    StreamNormItem
   ======================================================================== */

/* ------------------------------------------------------------------------ */
/*  sniInitItem                                                             */
/* ------------------------------------------------------------------------ */

int  sniInitItem (StreamNormItem *item, ClientData cd) {
  char         *name = (char *) cd;

  if (name) item->name = strdup(name); else item->name = NULL;
  item->hist      = NULL;
  item->pmf       = NULL;
  return TCL_OK;
}

/* ------------------------------------------------------------------------ */
/*  sniClear                                                                */
/* ------------------------------------------------------------------------ */

int  sniClear (StreamNormItem *sni) {
  int    histX;

  assert (sni);
  sni->samplesN  = 0;
  sni->minScore  = +1E20;
  sni->maxScore  = -1E20;
  if (sni->hist) {
    for (histX=0; histX<sni->histN; histX++)
      sni->hist[histX] = 0.0;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------ */
/*  sniInit                                                                 */
/* ------------------------------------------------------------------------ */

int  sniInit (StreamNormItem *sni, int streamX, int histN, int minmaxN) {
  int    histX;

  sni->streamX   = streamX;
  sni->histN     = histN;
  sni->minmaxN   = minmaxN;
  sni->hist      = (double *) malloc (sizeof(double) * histN);
  sni->pmf       = (double *) malloc (sizeof(double) * histN);
  sni->from = 0.0;
  sni->to   = 0.0;
  for (histX=0; histX<histN; histX++)
    sni->pmf[histX] = 0.0;
  return sniClear(sni);
}

/* ----------------------------------------------------------------------- */
/*  sniDeinitItem                                                          */
/* ----------------------------------------------------------------------- */

int  sniDeinitItem (StreamNormItem  *item) {
  if (item->name) {
    free(item->name);
    item->name = NULL;
  }
  if (item->hist) { free(item->hist); item->hist = NULL; }
  if (item->pmf)  { free(item->pmf);  item->pmf  = NULL; }
  return TCL_OK;
}

/* ----------------------------------------------------------------------- */
/*  sniScore                                                               */
/* ----------------------------------------------------------------------- */

#define  THRESHOLD    1E-10

float  sniScore (StreamNormItem  *item, float score) {
  float        normScore = 0.0;
  double       range = item->to - item->from;
  double       alpha,beta,gamma;
  int          idx;

  if (range <= 0.0) return normScore;

  if ((score >= item->from) && (score <= item->to)) {
    alpha = (score - item->from) / range;
    beta  = (alpha*(item->histN-1)+0.5);
    idx   = (int) beta;
    gamma = beta - idx;

    if (idx < item->histN-1)
      normScore = (1.0-gamma)*item->pmf[idx] + gamma*item->pmf[idx+1];
    else
      normScore = item->pmf[idx];
  } else {
    if (score < item->from) normScore = 1.0;
    if (score > item->to)   normScore = item->pmf[item->histN-1];
  }
  if (normScore <= THRESHOLD) normScore = THRESHOLD;
  return normScore;
}

/* ----------------------------------------------------------------------- */
/*  sniAccu                                                                */
/* ----------------------------------------------------------------------- */

int  sniAccu (StreamNormItem  *item, float score) {
  double       range,alpha,beta;
  int          idx=0;
  if (item->samplesN < item->minmaxN) {
    if (score < item->minScore) item->minScore = score;
    if (score > item->maxScore) item->maxScore = score;
  } else {
    range = item->maxScore - item->minScore;
    if ((score >= item->minScore) && (score <= item->maxScore)) {
      alpha = (score - item->minScore) / range;
      beta  = (alpha*(item->histN-1)+0.5);
      idx   = (int) beta;
    } else {
      if (score < item->minScore) idx = 0;
      if (score > item->maxScore) idx = item->histN-1;
    }
    item->hist[idx] += 1.0;
  }
  item->samplesN++;
  return TCL_OK;
}

/* ----------------------------------------------------------------------- */
/*  sniUpdate                                                              */
/* ----------------------------------------------------------------------- */

int  sniUpdate (StreamNormItem  *item) {
  double            sum;
  int               histX;

  if (item->samplesN < item->minmaxN) {
    ERROR("Not enough training patterns to adequately update normalizer\n");
    return TCL_ERROR;
  }
  item->from = item->minScore;
  item->to   = item->maxScore;
  sum = 0.0;
  for (histX=item->histN-1; histX>=0; histX--) {
    sum += item->hist[histX];
    item->pmf[histX] = sum;
  }
  for (histX=0; histX<item->histN; histX++) {
    item->pmf[histX] /= sum;
  }
  return sniClear(item);
}

/* ----------------------------------------------------------------------- */
/*  sniSave                                                                */
/* ----------------------------------------------------------------------- */

int  sniSave (StreamNormItem  *item, FILE  *fp) {
  int      histX;

  write_string(fp,item->name);
  write_int(fp,item->streamX);
  write_int(fp,item->histN);

  write_int(fp,item->samplesN);

  write_float(fp,item->minScore);
  write_float(fp,item->maxScore);

  write_float(fp,item->from);
  write_float(fp,item->to);

  for (histX=0; histX<item->histN; histX++)
    write_float(fp,item->hist[histX]);

  for (histX=0; histX<item->histN; histX++)
    write_float(fp,item->pmf[histX]);

  return TCL_OK;
}

/* ----------------------------------------------------------------------- */
/*  sniLoad                                                                */
/* ----------------------------------------------------------------------- */

int  sniLoad (StreamNormItem  *item, FILE  *fp) {
  char      name[255];
  int       histX,histN;

  if (read_string(fp,name) == 0) return TCL_ERROR;

  if (strcmp(item->name,name) != 0) {
    ERROR("Attempt to load stream normalizer '%s' into '%s'\n",
         name,item->name);
    return TCL_ERROR;
  }

  if (item->streamX != read_int(fp))
    WARN("Stream normalizer in file doesn't point to stream %d\n",
          item->streamX);

  histN = read_int(fp);
  if (histN != item->histN) {
    if (item->hist) free(item->hist);
    if (item->pmf)  free(item->pmf);
    item->hist  = (double *) malloc (sizeof(double) * histN);
    item->pmf   = (double *) malloc (sizeof(double) * histN);
    item->histN = histN;
  }

  item->samplesN = read_int(fp);

  item->minScore = read_float(fp);
  item->maxScore = read_float(fp);

  item->from = read_float(fp);
  item->to   = read_float(fp);

  for (histX=0; histX<item->histN; histX++)
    item->hist[histX] = read_float(fp);

  for (histX=0; histX<item->histN; histX++)
    item->pmf[histX] = read_float(fp);

  return TCL_OK;
}

/* ------------------------------------------------------------------------ */
/*  sniConfigureItf                                                         */
/* ------------------------------------------------------------------------ */

int  sniConfigureItf (ClientData cd, char *var, char *val) {
  StreamNormItem  *sniP = (StreamNormItem *) cd;

  if (! var) {
    ITFCFG(sniConfigureItf,cd,"name");
    ITFCFG(sniConfigureItf,cd,"streamX");
    ITFCFG(sniConfigureItf,cd,"histN");
    ITFCFG(sniConfigureItf,cd,"samplesN");
    ITFCFG(sniConfigureItf,cd,"minmaxN");
    ITFCFG(sniConfigureItf,cd,"minScore");
    ITFCFG(sniConfigureItf,cd,"maxScore");
    ITFCFG(sniConfigureItf,cd,"from");
    ITFCFG(sniConfigureItf,cd,"to");
    return TCL_OK;
  }
  ITFCFG_CharPtr (var,val,"name",      sniP->name,        1);
  ITFCFG_Int     (var,val,"streamX",   sniP->streamX,     0);
  ITFCFG_Int     (var,val,"histN",     sniP->histN,       1);
  ITFCFG_Int     (var,val,"samplesN",  sniP->samplesN,    1);
  ITFCFG_Int     (var,val,"minmaxN",   sniP->minmaxN,     1);
  ITFCFG_Float   (var,val,"minScore",  sniP->minScore,    1);
  ITFCFG_Float   (var,val,"maxScore",  sniP->maxScore,    1);
  ITFCFG_Float   (var,val,"from",      sniP->from,        1);
  ITFCFG_Float   (var,val,"to",        sniP->to,          1);

  return TCL_OK;
}

/* ------------------------------------------------------------------------ */
/*  sniPutsItf                                                              */
/* ------------------------------------------------------------------------ */

int  sniPutsItf (ClientData cd, int argc, char **argv) {
  StreamNormItem  *sniP = (StreamNormItem *) cd;
  int             histX;

  itfAppendResult("{name %s} ",     sniP->name);
  itfAppendResult("{streamX %d} ",  sniP->streamX);
  itfAppendResult("{histN %d} ",    sniP->histN);
  itfAppendResult("{samplesN %d} ", sniP->samplesN);
  itfAppendResult("{minmaxN %d} ",  sniP->minmaxN);
  itfAppendResult("{minScore %f} ", sniP->minScore);
  itfAppendResult("{maxScore %f} ", sniP->maxScore);
  itfAppendResult("{from %f} ",     sniP->from);
  itfAppendResult("{to %f} ",       sniP->to);

  if (sniP->hist) {
    itfAppendResult("{ hist { ");
    for (histX=0; histX<sniP->histN; histX++)
      itfAppendResult("%f ",sniP->hist[histX]);
    itfAppendResult("} }");
  }

  if (sniP->pmf) {
    itfAppendResult("{ pmf {");
    for (histX=0; histX<sniP->histN; histX++)
      itfAppendResult("%f ",sniP->pmf[histX]);
    itfAppendResult("} }");
  }

  return TCL_OK;
}

/* ========================================================================
    SenoneSet
   ======================================================================== */

static SenoneSet  snsDefaults;
TypeInfo streamNormItemInfo;

/* ------------------------------------------------------------------------
    Create Senone Set Structure
   ------------------------------------------------------------------------ */

int snsInit( SenoneSet* snsP, char* name, StreamArray* strA)
{
  int i;

  snsP->name          = strdup(name);
  snsP->useN          = 0;

  /* initialize senone list */

  listInit((List*)&(snsP->list), &senoneInfo, sizeof(Senone), 
                                  snsDefaults.list.blkSize);

  snsP->list.init    = (ListItemInit  *)snInit;
  snsP->list.deinit  = (ListItemDeinit*)snDeinit;
  snsP->list.hashKey = (HashKeyFn     *)snHashKey;
  snsP->list.hashCmp = (HashCmpFn     *)snHashCmp;

  /* initialize senone tags */

  listInit((List*)&(snsP->tagList), &senoneTagInfo, sizeof(SenoneTag), 5);

  snsP->tagList.init    = (ListItemInit  *)senoneTagInit;
  snsP->tagList.deinit  = (ListItemDeinit*)senoneTagDeinit;

  /* initialize stream normalization item list */

  listInit((List*)&(snsP->streamNormList), &streamNormItemInfo, 
           sizeof(StreamNormItem), 2);

  snsP->streamNormList.init   = (ListItemInit   *) sniInitItem;
  snsP->streamNormList.deinit = (ListItemDeinit *) sniDeinitItem;

  /* initialize stream array */

  snsP->strA.streamN =  strA->streamN;
  snsP->strA.cdA     =  strA->cdA;
  snsP->strA.stwA    =  strA->stwA;
  snsP->strA.strPA   =  strA->strPA;
  snsP->strA.cBaseA  =  strA->cBaseA;

  snsP->phones       =  NULL;
  snsP->tags         =  NULL;

  snsP->fesPA        = NULL;
  snsP->fesN         = 0;
  snsP->commentChar  = snsDefaults.commentChar;
  snsP->verbosity    = snsDefaults.verbosity;

  snsP->normalize    = 0;
  snsP->mixMode      = 0;
  snsP->scoreScale   = 1.0;
  snsP->streamWght   = NULL;
  snsP->streamCache  = NULL;
  snsP->streamCacheX = -1;
  snsP->streamWghtAccuMode = 0;
  snsP->toX          = -1;

  snsP->scoreFctP    = &ssa_one;
  snsP->scoreAFctP   = &ssa_opt;

  /* initialize index array */
  if (!(snsP->refXA = (CBX*) malloc ((1+CBX_MAX)*sizeof(CBX)))) {
    ERROR("snsInit : allocation failure\n");
    return TCL_ERROR;
  }
  for (i = 0; i <= CBX_MAX; i++) snsP->refXA[i] = i;

  return TCL_OK;
}

SenoneSet* snsCreate( char* name, StreamArray* strA)
{
  SenoneSet* snsP = (SenoneSet*)malloc(sizeof(SenoneSet));

  if (! snsP || snsInit( snsP, name, strA) != TCL_OK) {
    if ( snsP) free( snsP); 
    ERROR("Failed to allocate senone set '%s'.\n", name); 
    return NULL;
  }
  return snsP;
}

static ClientData snsCreateItf (ClientData cd, int argc, char *argv[])
{
  char          *name   = NULL;
  Phones*        phones = NULL;
  Tags*          tags   = NULL;
  SenoneSet*     snsP   = NULL;
  StreamArray    strA;

  streamArrayInit( &strA, 0);

  if ( itfParseArgv( argv[0], &argc, argv, 1,
      "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the senones set",
      "<streamArray>",ARGV_CUSTOM,getStreamArray,&strA,cd,
      "list of {stream [-streamType ST] [-weight W]}", 
      "-phones",   ARGV_OBJECT,NULL,&phones,   "Phones",   "set of phones",
      "-tags",     ARGV_OBJECT,NULL,&tags,     "Tags",     "set of tags",
       NULL) != TCL_OK) {

    streamArrayDeinit( &strA);
    return NULL;
  }
  if ((snsP = snsCreate(name, &strA))) {
    snsP->phones = phones;
    snsP->tags   = tags;
  }
  return (ClientData)snsP;
}

/* ------------------------------------------------------------------------
    snsReset
   ------------------------------------------------------------------------ */

int snsReset( SenoneSet* snsP)
{
  listDeinit((List*)&(snsP->list));
  listInit(  (List*)&(snsP->list), &senoneInfo, sizeof(Senone), 
                                    snsP->list.blkSize);

  snsP->list.init    = (ListItemInit  *)snInit;
  snsP->list.deinit  = (ListItemDeinit*)snDeinit;
  snsP->list.hashKey = (HashKeyFn     *)snHashKey;
  snsP->list.hashCmp = (HashCmpFn     *)snHashCmp;

  return TCL_OK;
}

static int snsResetItf(ClientData cd, int argc, char *argv[])
{
  int         ac    =  argc - 1;

  if (  itfParseArgv( argv[0], &ac, argv+1, 0, NULL) != TCL_OK) 
        return TCL_ERROR;
  else  return snsReset((SenoneSet*)cd);
}

/* ------------------------------------------------------------------------
    Free SenoneSet Structure
   ------------------------------------------------------------------------ */

int snsLinkN( SenoneSet* snsP)
{
  int  useN = listLinkN((List*)&(snsP->list)) - 1;
  if ( useN < snsP->useN) return snsP->useN;
  else                    return useN;
}

int snsDeinit (SenoneSet* snsP)
{
  if ( snsLinkN(snsP)) { 
    SERROR("SenoneSet '%s' still in use by other objects.\n", snsP->name);
    return TCL_ERROR;
  }

  if ( snsP->name)       { free(snsP->name);       snsP->name       = NULL; }
  if ( snsP->strA.cdA)   { free(snsP->strA.cdA);   snsP->strA.cdA   = NULL; }
  if ( snsP->strA.stwA)  { free(snsP->strA.stwA);  snsP->strA.stwA  = NULL; }
  if ( snsP->strA.strPA) { free(snsP->strA.strPA); snsP->strA.strPA = NULL; }
  if ( snsP->fesPA)      { free(snsP->fesPA);      snsP->fesPA      = NULL; }
  if ( snsP->refXA)      { free(snsP->refXA);      snsP->refXA      = NULL; }

  if ( listDeinit((List *) &(snsP->streamNormList)) == TCL_ERROR)
    return TCL_ERROR;

  return listDeinit((List*)&(snsP->list));
}

int snsFree (SenoneSet* snsP)
{
  if ( snsDeinit(snsP) != TCL_OK) return TCL_ERROR;
  if ( snsP) free(snsP);
  return TCL_OK;
}

static int snsFreeItf( ClientData cd )
{
  return snsFree((SenoneSet*)cd);
}

/* ------------------------------------------------------------------------
    snsName
   ------------------------------------------------------------------------ */

char* snsName( SenoneSet* sns, int i) 
{
  return (i < 0 || !sns->list.itemA[i].name) ? "(null)" : 
          sns->list.itemA[i].name;
}

static int snsNameItf (ClientData clientData, int argc, char *argv[] )
{
  SenoneSet* snsP = (SenoneSet*) clientData;
  int         idx = 0;
  int          ac = argc-1;
  char**       av = argv+1;

  if (itfParseArgv (argv[0], &ac, av, 0,
    "<index>", ARGV_INT, NULL, &idx, NULL, "index to look up",
    NULL) != TCL_OK) { 
    return TCL_ERROR;
  }

  itfAppendResult ("%s", snsName (snsP, idx));
  return TCL_OK;
  /*
  return listIndex2NameItf((ClientData)&(snsP->list), argc, argv);
  */
}

/* ------------------------------------------------------------------------
    snsIndex
   ------------------------------------------------------------------------ */

int snsIndex( SenoneSet* snsP, char* name) 
{
  int i = snsP->list.itemN-1;
  while (i>=0 && ! streq(snsP->list.itemA[i].name,name)) i--;
  return i;
}

static int snsIndexItf (ClientData clientData, int argc, char *argv[])
{
  SenoneSet* snsP = (SenoneSet*) clientData;
  if (argv[1]) itfAppendResult ("%d", snsIndex (snsP, argv[1]));
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    snsNumber
   ------------------------------------------------------------------------ */

int snsNumber( SenoneSet* sns) { return sns->list.itemN; }

/* ------------------------------------------------------------------------
    snsPutsItf
   ------------------------------------------------------------------------ */

static int snsPutsItf (ClientData cd, int argc, char *argv[])
{
  SenoneSet* snsP  = (SenoneSet*)cd;
  return listPutsItf( (ClientData)&(snsP->list), argc, argv);
}

/* ------------------------------------------------------------------------
    snsConfigureItf    configure a SenoneSet type object
   ------------------------------------------------------------------------ */

static int snsConfigureItf (ClientData cd, char *var, char *val)
{
  SenoneSet*  snsP = (SenoneSet*)cd;
  if (! snsP) snsP = &snsDefaults;

  if (! var) {
    ITFCFG(snsConfigureItf,cd,"useN");
    ITFCFG(snsConfigureItf,cd,"featSetN");
    ITFCFG(snsConfigureItf,cd,"commentChar");
    ITFCFG(snsConfigureItf,cd,"normalize");
    ITFCFG(snsConfigureItf,cd,"mixMode");
    ITFCFG(snsConfigureItf,cd,"scoreScale");
    ITFCFG(snsConfigureItf,cd,"likelihood");

    return listConfigureItf((ClientData)&(snsP->list), var, val);
  }
  ITFCFG_Int(    var,val,"useN",        snsP->useN,         1);
  ITFCFG_Int(    var,val,"featSetN",    snsP->fesN,         1);
  ITFCFG_Char(   var,val,"commentChar", snsP->commentChar,  1);
  ITFCFG_Int(    var,val,"normalize",   snsP->normalize,    0);
  ITFCFG_Int(    var,val,"mixMode",     snsP->mixMode,      0);
  ITFCFG_Float(  var,val,"scoreScale",  snsP->scoreScale,   0);
  ITFCFG_Float(  var,val,"likelihood",  snsP->likelihood,   1);

  return listConfigureItf((ClientData)&(snsP->list), var, val);
}

/* ------------------------------------------------------------------------
    snsAccessItf
   ------------------------------------------------------------------------ */

static ClientData snsAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  SenoneSet* snsP  = (SenoneSet*)cd;
  int i;

  if (*name == '.') { 
    if (name[1]=='\0') {
      if (snsP->list.itemN)
          itfAppendElement("item(0..%d)",(snsP->list.itemN)-1); 
      if (snsP->streamNormList.itemN)
          itfAppendElement("streamNorm(0..%d)",(snsP->streamNormList.itemN)-1);
      if (snsP->streamWght) 
	itfAppendElement("streamWght");
      if (snsP->streamCache)
	itfAppendElement ("streamCache");
      if (snsP->fesPA) 
	itfAppendElement("featureSet(0..%d)",snsP->fesN-1);

      itfAppendElement("stream(0..%d)",(snsP->strA.streamN)-1);
      itfAppendElement("tagList");
      *ti = NULL;
      return NULL;
    } else {
      name++;
      if (snsP->list.itemN && sscanf(name,"item(%d)",&i)==1) {
	*ti = itfGetType("Senone");
	 if (i >= 0 && i < snsP->list.itemN)
              return (ClientData)(snsP->list.itemA + i);
	 else return  NULL;
      } else if (snsP->streamNormList.itemN && 
                 sscanf(name,"streamNorm(%d)",&i)==1) {
	*ti = itfGetType("StreamNormItem");
	 if (i >= 0 && i < snsP->streamNormList.itemN)
              return (ClientData)(snsP->streamNormList.itemA + i);
	 else return  NULL;
      } else if (snsP->streamWght  && !strcmp (name, "streamWght")) {
	*ti = itfGetType ("FMatrix");
        return (ClientData) snsP->streamWght;
      } else if (snsP->streamCache && !strcmp (name, "streamCache")) {
	*ti = itfGetType ("FMatrix");
        return (ClientData) snsP->streamCache;
      } else if (snsP->fesPA && sscanf(name,"featureSet(%d)",&i)==1) {
	*ti = itfGetType("FeatureSet");
	 if (i >= 0 && i < snsP->fesN)
              return (ClientData) snsP->fesPA[i];
	 else return  NULL;
      } else if (sscanf(name,"stream(%d)",&i)==1) {
	 if (i >= 0 && i < snsP->strA.streamN) {
           *ti = snsP->strA.strPA[i]->tiP;
           return  snsP->strA.cdA[i];
	 } else
	   return  NULL;
      } else if (! strcmp( name, "tagList")) {
        *ti = itfGetType("List");
        return (ClientData)&(snsP->tagList);
      }
      return NULL;
    }
  } else if (*name == ':') { 
    if (name[1]=='\0') { 
      itfAppendElement ( "0 %d", snsP->list.itemN-1);
      *ti = NULL;
      return NULL;
    } else { 
      int i;
      for (i = 0; i < snsP->list.itemN; i++)
	if (!strcmp (snsName (snsP, i), name+1))
	  break;
      /* atoi (name+1); */
      *ti = &senoneInfo;
      if (i >= 0 && i < snsP->list.itemN) 
           return (ClientData) &(snsP->list.itemA[i]);
      else return NULL;
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    snsAdd    add a new senone to the senone set and initialize it
   ------------------------------------------------------------------------ */

int snsAdd (SenoneSet* snsP, SenoneKey* skeyP, char* name)
{
  Senone*   snP;
  int       idX;

  idX = listIndex((List*)&(snsP->list), (ClientData)skeyP, 1);
  snP = snsP->list.itemA + idX;

  if ( name && strcmp( name, "-")) {
    if ( snP->name) free( snP->name);
    snP->name = strdup( name);
  }

  return idX;
}

static int snsAddItf (ClientData cd, int argc, char *argv[])
{
  SenoneSet* snsP    = (SenoneSet*)cd;
  char*      name    =  NULL;
  int        ac      =  argc-1;
  char**     av      =  argv+1;
  int        idX;
  SenoneKey  skey;

  senoneKeyInit( &skey, snsP->strA.streamN, snsP);

  if (itfParseArgv(argv[0], &ac, av, 0,
      "<senone>", ARGV_CUSTOM, getSenoneKey,&skey, NULL,"list of score names",
      "-name",    ARGV_STRING, NULL,        &name, NULL,"name of the senone",
       NULL) != TCL_OK) {
    senoneKeyDeinit( &skey);
    return TCL_ERROR;
  }
  itfAppendResult(" %d", idX = snsAdd( snsP, &skey, name));
  senoneKeyDeinit( &skey);
  return ( idX< 0) ? TCL_ERROR : TCL_OK;
}

/* ------------------------------------------------------------------------
    snsRead
   ------------------------------------------------------------------------ */

static int snsReadItf( ClientData cd, int argc, char *argv[])
{
  SenoneSet* snsP  = (SenoneSet*)cd;
  int        ac    =  argc - 1;
  char*      fname =  NULL;
  int        n;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &fname, cd, "file to read from",
       NULL) != TCL_OK) return TCL_ERROR;

  if ((n = freadAdd( cd, fname, snsP->commentChar, snsAddItf)) >= 0) {
    itfResetResult();
    itfAppendResult("%d",n);
    return TCL_OK;
  }
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    snsWrite
   ------------------------------------------------------------------------ */

int snsWrite( SenoneSet* snsP, char* fileName)
{
  StreamArray* staP = &(snsP->strA);
  FILE*        fp;
  int          snX;

  assert( fileName);

  if (! ( fp = fileOpen( fileName,"w"))) return TCL_ERROR;

  fprintf( fp, "%c -------------------------------------------------------\n", snsP->commentChar);
  fprintf( fp, "%c  Name            : %s\n",                                   snsP->commentChar, snsP->name);
  fprintf( fp, "%c  Type            : SenoneSet\n" ,                           snsP->commentChar);
  fprintf( fp, "%c  Number of Items : %d\n",                                   snsP->commentChar, snsP->list.itemN);
  fprintf( fp, "%c  Date            : %s",                                     snsP->commentChar, dateString());
  fprintf( fp, "%c -------------------------------------------------------\n", snsP->commentChar);

  for ( snX = 0; snX < snsP->list.itemN; snX++) {
    int i, streamN = snsP->list.itemA[snX].streamN;

    fprintf( fp, "{");
    for ( i = 0; i < streamN; i++) {
      int  idX = snsP->list.itemA[snX].classXA[i];
      fprintf( fp, " {");
      if ( idX < 0) 
             fprintf( fp, " {%s}", "-");
      else {
        if ( staP->strPA[i]->nameFct)
             fprintf( fp, " {%s}", staP->strPA[i]->nameFct(staP->cdA[i],idX));
        else fprintf( fp, " %d",   idX);
      }
      fprintf( fp, " -weight %f}", snsP->list.itemA[snX].stwA[i]);
    }
    fprintf( fp, "}");
    if ( snsP->list.itemA[snX].name) 
         fprintf( fp, " -name %-20s", snsP->list.itemA[snX].name);      
    fprintf( fp, "\n");
  }
  fileClose( fileName, fp);
  return TCL_OK;
}

static int snsWriteItf (ClientData cd, int argc, char *argv[])
{
  SenoneSet* snsP  = (SenoneSet*)cd;
  int        ac    =  argc - 1;
  char*      fname =  NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &fname, cd, "file to read from",
       NULL) != TCL_OK) return TCL_ERROR;

  return snsWrite( snsP, fname);
}

/* ------------------------------------------------------------------------
    snsLoad
   ------------------------------------------------------------------------ */

int snsLoad (SenoneSet* snsP, FILE* fp)
{
  int snX,snN;
  SenoneKey skey;
  char str[1024];

  senoneKeyInit( &skey, snsP->strA.streamN, snsP);

  if (! fp) {
    ERROR("snsLoad: Invalid file pointer\n");
    return TCL_ERROR;
  }
  if (snsP->list.itemN > 0) {
    ERROR("snsLoad: expected empty SenoneSet\n");
    return TCL_ERROR;
  }

  read_string(fp,str);
  if (0 != strcmp(str,"SENONESET-BEGIN")) {
    ERROR("snsLoad: inconsistent file\n");
    return TCL_ERROR;
  }

  snN= read_int(fp);
  for (snX=0;snX<snN;snX++) {
    read_string(fp,str);
    skey.streamN = read_int(fp);
    read_ints  (fp,skey.classXA,skey.streamN);
    read_floats(fp,skey.stwA,   skey.streamN);
    snsAdd(snsP,&skey,str);
  }
  senoneKeyDeinit(&skey);

  read_string(fp,str);
  if (0 != strcmp(str,"SENONESET-END")) {
    ERROR("snsLoad: inconsistent file\n");
    return TCL_ERROR;
  }

  return TCL_OK;
}

static int snsLoadItf( ClientData cd, int argc, char *argv[])
{
  SenoneSet* snsP  = (SenoneSet*)cd;
  int        ac    =  argc - 1;
  char*      fname =  NULL;
  FILE*      fp    = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &fname, cd, "file to load from",
       NULL) != TCL_OK) return TCL_ERROR;

  fp = fileOpen(fname,"r");
  snsLoad(snsP,fp);
  fileClose(fname,fp);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    snsSave
   ------------------------------------------------------------------------ */

int snsSave (SenoneSet* snsP, FILE* fp)
{
  int snX;

  if (! fp) {
    ERROR("snsSave: Invalid file pointer\n");
    return TCL_ERROR;
  }

  write_string(fp,"SENONESET-BEGIN");
  write_int   (fp,snsP->list.itemN); 

  for (snX=0;snX<snsP->list.itemN;snX++) {
    Senone* snP = snsP->list.itemA+snX;
    write_string(fp,snP->name);
    write_int   (fp,snP->streamN);
    write_ints  (fp,snP->classXA,snP->streamN);
    write_floats(fp,snP->stwA,   snP->streamN);
  }
  write_string(fp,"SENONESET-END");
  fflush(fp);
  return TCL_OK;
}

static int snsSaveItf( ClientData cd, int argc, char *argv[])
{
  SenoneSet* snsP  = (SenoneSet*)cd;
  int        ac    =  argc - 1;
  char*      fname =  NULL;
  FILE*      fp    = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &fname, cd, "file to save to",
       NULL) != TCL_OK) return TCL_ERROR;

  fp = fileOpen(fname,"w");
  snsSave(snsP,fp);
  fileClose(fname,fp);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    snsFesEval
   ------------------------------------------------------------------------ */

int snsFesEval( SenoneSet *snsP, char *sampleList)
{
  StreamArray* staP  = &(snsP->strA);
  int          fesX;

  /* -------------------------------------------------------------------- */
  /* if getting here the first time, build up a list of used feature sets */
  /* -------------------------------------------------------------------- */

  if (! snsP->fesPA) {
    int sX;

    for ( sX = 0; sX < staP->streamN; sX++) {
      FeatureSet *fsP = staP->strPA[sX]->fesUsedFct(staP->cdA[sX]);

      assert (snsP->fesPA || !snsP->fesN); // XCode says could be NULL
      for ( fesX = 0; fesX < snsP->fesN; fesX++) 
        if ( snsP->fesPA[fesX] == fsP) break;

      if ( fesX == snsP->fesN) {
        snsP->fesPA               = (FeatureSet**)realloc( snsP->fesPA,
                                    (snsP->fesN+1) * sizeof(FeatureSet*)); 
        snsP->fesPA[snsP->fesN++] =  fsP;
      }
    }
  }

  /* ----------------------------------------------------------------- */
  /* know already what feature sets are needed, let them do their jobs */
  /* ----------------------------------------------------------------- */
  assert (snsP->fesPA); // XCode says could be NULL
  for ( fesX=0; fesX < snsP->fesN; fesX++) { 
    if ( fesEval( snsP->fesPA[fesX],sampleList) != TCL_OK) return TCL_ERROR;
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    snsFrameN
   ------------------------------------------------------------------------ */

int snsFrameN (SenoneSet *snsP, int *from, int *shift, int *ready) {
  StreamArray* staP  = &(snsP->strA);
  int          sX;
  int frameN = (staP->streamN > 0) ? 
               staP->strPA[0]->frameNFct( staP->cdA[0], from, shift, ready) :
               -1;

  for ( sX = 1; sX < staP->streamN; sX++) {
    int frN = staP->strPA[sX]->frameNFct( staP->cdA[sX], from, shift, ready);
    if ( frN < frameN) frameN = frN;
  }
  return frameN;
}

/* ------------------------------------------------------------------------
    snsGetModel  given a phone/tag context the streams are asked for their
                 model. Based on the models a new senone will be generated
                 if it does not already exist.
   ------------------------------------------------------------------------ */

int snsGetModel( SenoneSet* snsP, int tag, Word* wP, int left, int right) {
  SenoneKey skeyP;
  int       idX = -1, i;
  int       n   =  0;

  senoneKeyInit( &skeyP, snsP->strA.streamN, snsP);

  for ( i = 0; i < snsP->strA.streamN; i++) {
    int   t =  snsP->tagList.itemA[tag].tagA[i];
    float w =  1.0;
    int   m = -1;

    if ( snsP->strA.strPA[i]->getFct) {
      w = snsP->strA.stwA[i];
      m = snsP->strA.strPA[i]->getFct(snsP->strA.cdA[i],t,wP,left,right);
    }
    skeyP.classXA[i] = m; if ( m > -1) n++;
    skeyP.stwA[i]    = w;
  }

  if (! n) {
    ERROR("No valid model %s %d %d in stream 0..%d.\n", wordPuts(wP, 1), 
           left, right, snsP->strA.streamN-1);
    senoneKeyDeinit( &skeyP);
    return -1;
  }

  idX = snsAdd( snsP, &skeyP, NULL);

  senoneKeyDeinit( &skeyP);
  return idX;
}

int snsGetModelItf( ClientData clientData, int argc, char *argv[]) {
  SenoneSet*     snsP  = (SenoneSet*)clientData;
  int            left  = -1, right = 0, tag;
  int            ac    = argc - 1, idX;
  SenoneTagArray sta   = {NULL, 0};
  Word           word;

  if (! snsP->phones || ! snsP->tags) {
    ERROR("Phones/Tags definition missing in SenoneSet %s.\n", snsP->name);
    return TCL_ERROR;
  }
  wordInit( &word, snsP->phones, snsP->tags);

  sta.tagA = &tag;
  sta.tagN =  1;
  sta.snsP =  snsP;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<senone tag>",   ARGV_CUSTOM, getSenoneTagArray,&sta,NULL,"tag",
      "<tagged phones>",ARGV_CUSTOM, getWord, &word,NULL,
      "list of tagged phones",
      "<leftContext>",  ARGV_INT, NULL, &left,  NULL, "left  context",
      "<rightContext>", ARGV_INT, NULL, &right, NULL, "right context",
       NULL) != TCL_OK) {
    wordDeinit( &word);
    return TCL_ERROR;
  }

  idX = snsGetModel (snsP, tag, &word, left, right);
  itfResetResult();
  itfAppendResult(" %d", idX);

  wordDeinit( &word);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    snsScore   compute the score of a single senone for a given frame
   ------------------------------------------------------------------------ */

float snsScore( SenoneSet* snsP, int senoneX, int frameX) {
  StreamArray* staP  = &(snsP->strA);
  Senone*      snP   =   NULL;
  StreamNormItem        *item;
  double       score =   0.0;
  int          stX   =   0;
  int          ok,k;

  if ( senoneX < 0 || senoneX > snsP->list.itemN-1) {
    ERROR("Asking for score of undefined senone %d.\n",senoneX); 
    return 0.0;
  }

  snP = snsP->list.itemA + senoneX;

  if (snsP->normalize) {

    /* ---------------------- */
    /*  stream normalization  */
    /* ---------------------- */
    for ( stX = 0; stX < snP->streamN; stX++) {
      int     idx;
      float   p = 1.0;
      if ((idx = snP->classXA[stX]) >= 0) {
        float  f;
        staP->strPA[stX]->scoreFct( staP->cdA[stX], &idx, &f, 1, frameX);
  
        ok = 0;
        for (k=0; k<snsP->streamNormList.itemN; k++) {
          item = snsP->streamNormList.itemA + k;
          if (item->streamX == stX) {
            ok = 1;
            p = sniScore(item,f);
            break;
          }
        }

        if (!ok) { ERROR("Couldn't normalize stream %d\n",stX); }

        score -= snsP->scoreScale * snP->stwA[stX] * log(p);
      }
    }

  } else {

    /* ------------------------- */
    /*  no stream normalization  */
    /* ------------------------- */
    for ( stX = 0; stX < snP->streamN; stX++) {
      int  idx;
      if ((idx = snP->classXA[stX]) >= 0) {
        float  f;
        staP->strPA[stX]->scoreFct( staP->cdA[stX], &idx, &f, 1, frameX);
        score += snsP->scoreScale * snP->stwA[stX] * f;
      }
    }
  }

  return score;
}


static int snsScoreItf (ClientData cd, int argc, char *argv[]) {
  SenoneSet*  snsP  = (SenoneSet*)cd;
  int        frameX = 0, snX = -1, ac=argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<senone>", ARGV_INT, NULL, &snX,   NULL, "senone index",
      "<frame>",  ARGV_INT, NULL, &frameX,NULL, "index of the requested frame",
       NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult("%f", snsScore( snsP, snX, frameX));
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    snsScoreArray    compute all scores marked in the score array array
                     as being active scores
   ------------------------------------------------------------------------ */

int snsScoreArray( SenoneSet* snsP, int* idxA, float* scoreA, 
                                    int  idxN, int    frameX)
{
  StreamArray*    staP = &(snsP->strA);
  StreamNormItem* item = NULL;

  int*         refP   =  (int  *)malloc( idxN * sizeof(int));
  int*         idxP   =  (int  *)malloc( idxN * sizeof(int));
  float*       scoreP =  (float*)malloc( idxN * sizeof(float));

  int          ok, k, i, stX, stN = 0;

  /* --------------------------------------------------- */
  /* set score array to 0 and find max number of streams */
  /* --------------------------------------------------- */

  for ( i   = 0; i   < idxN; i++) {
    if ( snsP->list.itemA[idxA[i]].streamN > stN) 
         stN  = snsP->list.itemA[idxA[i]].streamN;
    scoreA[i] = 0.0;
  }

  if (snsP->normalize) {

    /* ------------------------- */
    /*  normalize stream scores  */
    /* ------------------------- */
    for ( stX = 0; stX < stN; stX++) {
      Senone* snP = snsP->list.itemA;
      int     n   = 0;

      for ( i = 0; i < idxN; i++) {
        if ( snP[idxA[i]].streamN > stX && snP[idxA[i]].classXA[stX] >= 0) {
          idxP[n] = snP[idxA[i]].classXA[stX];
          refP[n] = i;
          n++;
        }
      }

      staP->strPA[stX]->scoreFct( staP->cdA[stX], idxP, scoreP, n, frameX);

      /* search for stream normalizer */
      ok = 0;
      for (k=0; k<snsP->streamNormList.itemN; k++) {
        item = snsP->streamNormList.itemA + k;
        if (item->streamX == stX) {
          ok = 1;
          break;
        }
      }

      if (ok)
        for (i=0; i<n; i++) scoreP[i] = sniScore(item,scoreP[i]);
      else {
        ERROR("Couldn't normalize stream %d\n",stX);
        return TCL_ERROR;
      }

      /* -------------------------------------------------- */
      /*  check if a stream confidence update is necessary  */
      /* -------------------------------------------------- */
      if (snsP->mixMode == 1) {
        double   mass,entropy,p;
        entropy = 0.0;
        mass = 0.0;
        for (i=0; i<n; i++) mass += scoreP[i];
        for (i=0; i<n; i++) {
          p = scoreP[i] / mass;
          if (p>1E-10) entropy -= p*log(p);
        }

        if (!snsP->streamWght) {
          ERROR("No stream weight matrix allocated\n");
          return TCL_ERROR;
        }
        if ((stX >= snsP->streamWght->m) || (frameX >= snsP->streamWght->n)) {
          ERROR("Invalid size of weight matrix (forgot to clear?)\n");
          return TCL_ERROR;
        }
        snsP->streamWght->matPA[stX][frameX] = entropy;
      }

      /* ------------------- */
      /*  mixing of streams  */
      /* ------------------- */
      if (snsP->mixMode != 2) {

        /* ------------------------------------------- */
        /*  static mixing of normalized stream scores  */
        /* ------------------------------------------- */
        for ( i = 0; i < n; i++) 
          scoreA[refP[i]] -= snsP->scoreScale * 
                             snP[idxA[refP[i]]].stwA[stX] *   
                             log(scoreP[i]);

      } else {

        /* -------------------------------------------------------- */
        /*  stream confidence measure based adaptive stream mixing  */
        /* -------------------------------------------------------- */
        float    weight;

        if (!snsP->streamWght) {
          ERROR("No stream weight matrix allocated\n");
          return TCL_ERROR;
        }
        if ((stX >= snsP->streamWght->m) || (frameX >= snsP->streamWght->n)) {
          ERROR("Invalid size of weight matrix (forgot to clear?)\n");
          return TCL_ERROR;
        }

        weight = snsP->streamWght->matPA[stX][frameX];

        for ( i = 0; i < n; i++) 
          scoreA[refP[i]] -= snsP->scoreScale *
                             weight * 
                             log(scoreP[i]);
      }
    }

  } else {

    /* -------------------------------- */
    /*  do not normalize stream scores  */
    /* -------------------------------- */
    for ( stX = 0; stX < stN; stX++) {
      Senone* snP = snsP->list.itemA;
      int     n   = 0;

      for ( i = 0; i < idxN; i++) {
        if ( snP[idxA[i]].streamN > stX && snP[idxA[i]].classXA[stX] >= 0) {
          idxP[n] = snP[idxA[i]].classXA[stX];
          refP[n] = i;
          n++;
        }
      }

      staP->strPA[stX]->scoreFct( staP->cdA[stX], idxP, scoreP, n, frameX);

      /* ------------------- */
      /*  mixing of streams  */
      /* ------------------- */
      if (snsP->mixMode != 2) {
        for ( i = 0; i < n; i++) 
          scoreA[refP[i]] += snsP->scoreScale * 
                             snP[idxA[refP[i]]].stwA[stX] * 
                             scoreP[i];
      } else {

        /* -------------------------------------------------------- */
        /*  stream confidence measure based adaptive stream mixing  */
        /* -------------------------------------------------------- */
        float    weight;

        if (!snsP->streamWght) {
          ERROR("No stream weight matrix allocated\n");
          return TCL_ERROR;
        }
        if ((stX >= snsP->streamWght->m) || (frameX >= snsP->streamWght->n)) {
          ERROR("Invalid size of weight matrix (forgot to clear?)\n");
          return TCL_ERROR;
        }

        weight = snsP->streamWght->matPA[stX][frameX];
        for ( i = 0; i < n; i++) 
          scoreA[refP[i]] += snsP->scoreScale *
                             weight * 
                             scoreP[i];

      }
    }
  }

  if ( refP)   free( refP);
  if ( idxP)   free( idxP);
  if ( scoreP) free( scoreP);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    snsAccu    add all items in a a path to the accumulator weighted by
               factor times gamma.
   ------------------------------------------------------------------------ */

#define   iswap(X,Y)   { register int d; d = X; X = Y; Y = d; }

static int snsAccu( SenoneSet* snsP, Path* path, float factor, int do_rand, int from, int to)
{
  int          frX, i, j, frN, idx, permX;
  static int   *rand_table = NULL;
  static int   table_size = 0;
  
  assert( snsP && path && path->pitemListN);

  if (!do_rand) {
    if (from < 0) from = 0;
    if (to   < 0) to   = path->lastFrame-path->firstFrame+1;

    for ( frX = from; frX < to; frX++) {
      for ( i = 0; i < path->pitemListA[frX].itemN; i++)
        snAccu( snsP, path->pitemListA[frX].itemA[i].senoneX,
                      frX+path->firstFrame,
                      factor * path->pitemListA[frX].itemA[i].gamma);

    }
    
  } else {

    frN = path->lastFrame-path->firstFrame+1;
    if (frN > table_size) {
      if (rand_table != NULL) free(rand_table);
      if (NULL == (rand_table = (int *) malloc (sizeof(int)*frN))) {
        ERROR ("Could not allocate memory.\n");
        return TCL_ERROR;
      }
      table_size = frN;
    }
 
    for ( frX = 0; frX < frN; frX++) rand_table[frX] = frX;
    for ( permX = 0; permX < 5*frN; permX++) {
      i = rand() % frN;
      j = rand() % frN;
      iswap(rand_table[i],rand_table[j]);
    }
    
    for ( frX = 0; frX < frN; frX++) {
      idx = rand_table[frX];
      for ( i = 0; i < path->pitemListA[idx].itemN; i++)
        snAccu( snsP, path->pitemListA[idx].itemA[i].senoneX,
                      idx+path->firstFrame,
                      factor * path->pitemListA[idx].itemA[i].gamma);

    }
    
  }

  return TCL_OK;
}

static int snsAccuItf (ClientData cd, int argc, char *argv[]) 
{ 
  SenoneSet* sns     = (SenoneSet*)cd; 
  Path*      path    =  NULL;
  float      factor  =  1.0;
  int        do_rand =  0;
  int        ac      =  argc-1;
  int        from    = -1;
  int        to      = -1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<path>",  ARGV_OBJECT, NULL, &path, "Path", "name of the path object",
      "-factor", ARGV_FLOAT,  NULL, &factor, 0,    "training factor",
      "-random", ARGV_INT,    NULL, &do_rand,NULL, "random frame presentation",
      "-from",   ARGV_INT,    NULL, &from,   NULL, "start frame",
      "-to",     ARGV_INT,    NULL, &to,     NULL, "end   frame",
       NULL) != TCL_OK) return TCL_ERROR;

  return snsAccu(sns,path,factor,do_rand,from,to);
}

/* ------------------------------------------------------------------------
    snsUpdate
   ------------------------------------------------------------------------ */

static int snsUpdate( SenoneSet *snsP)
{
  StreamArray* staP  = &(snsP->strA);
  int          sX, sY, itemX;

  assert( snsP);

  if (snsP->normalize) {

    for (itemX=0; itemX<snsP->streamNormList.itemN; itemX++)
      sniUpdate(snsP->streamNormList.itemA+itemX);

  } else {

    for ( sX = 0; sX < staP->streamN; sX++) {
      int alreadyUpdated = 0;

      for ( sY = 0; sY < sX; sY++)
        if ( staP->cdA[sX]   == staP->cdA  [sY] &&
             staP->strPA[sX] == staP->strPA[sY]) { alreadyUpdated=1; break; }

      if (! alreadyUpdated) {
        if ( snsP->verbosity & SNS_UPDATE)
          INFO("Updating stream %d in score computer\n", sX);

        if ( staP->strPA[sX]->updateFct( staP->cdA[sX]) != TCL_OK)
          ERROR("Can't update stream %d\n", sX);
      }
    }
  }
  return TCL_OK;
}

static int snsUpdateItf (ClientData cd, int argc, char *argv[])
{
  SenoneSet* sns    = (SenoneSet*)cd; 
  int        ac     =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0, NULL) != TCL_OK) 
       return TCL_ERROR;

  return snsUpdate(sns);
}

/* ------------------------------------------------------------------------ */
/*  snsAddNorm                                                              */
/* ------------------------------------------------------------------------ */

int snsAddNorm (SenoneSet *snsP, char *name, int streamX, 
                int histN, int minmaxN) {
  int      sniX;

  sniX = listIndex((List *) &(snsP->streamNormList),(ClientData) name,0);

  if (sniX >= 0) {
    ERROR("Stream normalizer %s already exists\n",name);
    return TCL_ERROR;
  }

  sniX  = listNewItem((List*) &(snsP->streamNormList),(ClientData) name);

  if ((streamX < 0) || (streamX > snsP->strA.streamN)) {
    ERROR("Invalid stream index %d\n",streamX);
    return TCL_ERROR;
  }

  if (histN <= 0) {
    ERROR("Invalid resolution for histogram (%d)\n",histN);
    return TCL_ERROR;
  }

  if (minmaxN < 0) {
    ERROR("Invalid minmaxN value %d\n",minmaxN);
    return TCL_ERROR;
  }

  return sniInit(snsP->streamNormList.itemA + sniX,streamX,histN,minmaxN);
}

/* ------------------------------------------------------------------------ */
/*  snsAddNormItf                                                           */
/* ------------------------------------------------------------------------ */

static int snsAddNormItf (ClientData cd, int argc, char *argv[]) {
  SenoneSet* sns     = (SenoneSet*)cd; 
  int        ac      =  argc-1;
  char       *name   = NULL;
  int        streamX = -1;
  int        histN   = 1000;
  int        minmaxN = 500000;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<name>",    ARGV_STRING, NULL, &name,  NULL, "name of stream normalizer",
      "<streamX>", ARGV_INT,  NULL, &streamX, NULL, "stream index",
      "-histN",    ARGV_INT,  NULL, &histN,   NULL, "resolution of histogram",
      "-minmaxN",  ARGV_INT,  NULL, &minmaxN, NULL, "number of samples for min/max computation",
    NULL) != TCL_OK)  return TCL_ERROR;

  return snsAddNorm(sns,name,streamX,histN,minmaxN);
}

/* ------------------------------------------------------------------------ */
/*  snsClearNormItf                                                         */
/* ------------------------------------------------------------------------ */

static int snsClearNormItf (ClientData cd, int argc, char *argv[]) {
  SenoneSet      *sns     = (SenoneSet*) cd; 
  char           *name    = NULL;
  int            ac       = argc-1;
  int            sniX;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-name",    ARGV_STRING,  NULL, &name,    NULL, "name of stream normalizer",
    NULL) != TCL_OK)  return TCL_ERROR;

  if (name != NULL) {
    sniX = listIndex((List *) &(sns->streamNormList),(ClientData) name,0);
    if (sniX < 0) {
      ERROR("Stream normalizer %s doesn't exist\n",name);
      return TCL_ERROR;
    }
    sniClear(sns->streamNormList.itemA+sniX);
  } else {
    for (sniX=0; sniX<sns->streamNormList.itemN; sniX++)
      sniClear(sns->streamNormList.itemA+sniX);
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------ */
/*  snsSaveNormItf                                                          */
/* ------------------------------------------------------------------------ */

static int snsSaveNormItf (ClientData cd, int argc, char *argv[]) {
  SenoneSet     *sns   = (SenoneSet*) cd; 
  int           ac     = argc-1;
  char       *filename = NULL;
  char          *name  = NULL;
  int           sniX;
  FILE          *fp;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING,  NULL, &filename, NULL, "name of file",
      "-name",      ARGV_INT,  NULL, &name,     NULL, "name of stream normalizer",
    NULL) != TCL_OK)  return TCL_ERROR;

  if ((fp = fileOpen(filename,"w")) == NULL) {
    ERROR("Couldn't open file '%s'\n",filename);
    return TCL_ERROR;
  }

  if (name != NULL) {
    sniX = listIndex((List *) &(sns->streamNormList),(ClientData) name,0);
    if (sniX < 0) {
      ERROR("Stream normalizer %s doesn't exist\n",name);
      return TCL_ERROR;
    }
    sniSave(sns->streamNormList.itemA+sniX,fp);
  } else {
    for (sniX=0; sniX<sns->streamNormList.itemN; sniX++)
      sniSave(sns->streamNormList.itemA+sniX,fp);
  }
  fileClose(filename,fp);
  return TCL_OK;
}

/* ------------------------------------------------------------------------ */
/*  snsLoadNormItf                                                          */
/* ------------------------------------------------------------------------ */

static int snsLoadNormItf (ClientData cd, int argc, char *argv[]) {
  SenoneSet     *sns   = (SenoneSet*) cd; 
  int           ac     = argc-1;
  char       *filename = NULL;
  char          *name  = NULL;
  int           sniX;
  FILE          *fp;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
    "<filename>", ARGV_STRING,  NULL, &filename, NULL, "name of file",
    "-name",      ARGV_STRING,  NULL, &name,     NULL, "name of stream normalizer",
    NULL) != TCL_OK)  return TCL_ERROR;

  if ((fp = fileOpen(filename,"r")) == NULL) {
    ERROR("Couldn't open file '%s'\n",filename);
    return TCL_ERROR;
  }
  if (name != NULL) {
    sniX = listIndex((List *) &(sns->streamNormList),(ClientData) name,0);
    if (sniX < 0) {
      ERROR("Stream normalizer %s doesn't exist\n",name);
      return TCL_ERROR;
    }
    sniLoad(sns->streamNormList.itemA+sniX,fp);
  } else {
    for (sniX=0; sniX<sns->streamNormList.itemN; sniX++)
      sniLoad(sns->streamNormList.itemA+sniX,fp);
  }
  fileClose(filename,fp);
  return TCL_OK;
}

/* ------------------------------------------------------------------------ */
/*  snsClearMixItf                                                          */
/* ------------------------------------------------------------------------ */

static int snsClearMixItf (ClientData cd, int argc, char *argv[]) {
  SenoneSet     *sns   = (SenoneSet*) cd; 
  int           ac     = argc-1;
  int           streamN = 0;
  int           frameN = 0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
    "<streamN>", ARGV_INT,  NULL, &streamN, NULL, "number of streams",
    "<frameN>",  ARGV_INT,  NULL, &frameN,  NULL, "number of frames",
    NULL) != TCL_OK)  return TCL_ERROR;

  if (sns->streamWght) fmatrixFree(sns->streamWght);
  sns->streamWght = fmatrixCreate(streamN,frameN);
  sns->mixMode = 1;

  if (!sns->normalize) WARN("Stream mixing requires normalized streams\n");

  return TCL_OK;
}

/* ------------------------------------------------------------------------ */
/*  snsUpdateMixItf                                                         */
/* ------------------------------------------------------------------------ */

static int snsUpdateMixItf (ClientData cd, int argc, char *argv[]) {
  SenoneSet     *sns   = (SenoneSet*) cd; 
  int           ac     = argc-1;
  int           winN   = 7;
  int           streamX,frameX,idx,k,streamN,frameN,n;
  double        sum;
  FMatrix       *cf;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
    "-smooth", ARGV_INT,  NULL, &winN, NULL, "size of smoothing window",
    NULL) != TCL_OK)  return TCL_ERROR;

  if (winN < 0) {
    ERROR("invalid size of smoothing window\n");
    return TCL_ERROR;
  }

  if ((sns->mixMode != 1) || (!sns->streamWght)){
    ERROR("stream mixer update requires search pass with mixMode=1\n");
    return TCL_ERROR;
  }

  /* ----------------------------- */
  /*  smooth confidence estimates  */
  /* ----------------------------- */
  streamN = sns->streamWght->m;
  frameN  = sns->streamWght->n;
  cf = fmatrixCreate(streamN,frameN);
  for (streamX=0; streamX<streamN; streamX++) {
    for (frameX=0; frameX<frameN; frameX++) {
      n = 0;
      sum = 0.0;
      for (k=-winN; k<=+winN; k++) {
        idx = frameX+k;
        if ((idx >= 0) && (idx < frameN)) {
          sum += sns->streamWght->matPA[streamX][idx];
          n++;
        }
      }
      cf->matPA[streamX][frameX] = sum/((double) n);
    }
  }

  /* ---------------------------------- */
  /*  compute posterior stream weights  */
  /* ---------------------------------- */
  for (frameX=0; frameX<frameN; frameX++) {
    sum = 0.0;
    for (streamX=0; streamX<streamN; streamX++)
      sum += cf->matPA[streamX][frameX];

    for (streamX=0; streamX<streamN; streamX++)
      sns->streamWght->matPA[streamX][frameX] = 
                      1.0 - (cf->matPA[streamX][frameX]/sum);
  }

  fmatrixFree(cf);

  sns->mixMode = 2;
  return TCL_OK;
}

/* ------------------------------------------------------------------------ */
/*  snsLabelMixItf                                                          */
/* ------------------------------------------------------------------------ */

static int snsLabelMixItf (ClientData cd, int argc, char *argv[]) {
  SenoneSet         *sns   = (SenoneSet*) cd; 
  int               ac     = argc-1;
  Path              *path  = NULL;
  int               soft   = 0;
  int               winN   = 5;
  int               frameX,frameN,senoneX,streamX,streamN,idx,ok,k,n;
  float             sum,max,p;
  Senone            *snP;
  StreamNormItem    *item;
  StreamArray* staP  = &(sns->strA);
  float             *locScore;
  FMatrix           *cf;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
    "<path>",  ARGV_OBJECT,  NULL, &path,   "Path", "path object",
    "-soft",   ARGV_INT,     NULL, &soft,   NULL,   "soft targets?",
    "-smooth", ARGV_INT,     NULL, &winN,   NULL,   "size of smoothing window",
    NULL) != TCL_OK)  return TCL_ERROR;

  frameN  = path->lastFrame - path->firstFrame + 1;
  streamN = 1;
  for (frameX=0; frameX<frameN; frameX++) {
    senoneX = path->pitemListA[frameX].itemA[0].senoneX;
    snP = sns->list.itemA + senoneX;
    if (snP->streamN > streamN) streamN = snP->streamN;
  }

  if ((streamN <= 1) || (!sns->normalize)) {
    ERROR("not enough streams or normalization turned off\n");
    return TCL_ERROR;
  }

  locScore = (float *) malloc (sizeof(float) * streamN);
  if (sns->streamWght) fmatrixFree(sns->streamWght);
  sns->streamWght = fmatrixCreate(streamN,frameN);

  for (frameX=0; frameX<frameN; frameX++) {
    senoneX = path->pitemListA[frameX].itemA[0].senoneX;
    snP = sns->list.itemA + senoneX;
    for (streamX=0; streamX<snP->streamN; streamX++) {
      sns->streamWght->matPA[streamX][frameX] = 1.0 / ((float) streamN);
      p = 0.0;
      if ((idx = snP->classXA[streamX]) >= 0) {
        float  f;
        staP->strPA[streamX]->scoreFct( staP->cdA[streamX], &idx, &f, 1, frameX);
        ok = 0;
        for (k=0; k<sns->streamNormList.itemN; k++) {
          item = sns->streamNormList.itemA + k;
          if (item->streamX == streamX) {
            ok = 1;
            p = sniScore(item,f);
            break;
          }
        }

        if (!ok) { ERROR("Couldn't normalize stream %d\n",streamX); }
      }
      locScore[streamX] = p;
      if (streamX == 1) locScore[streamX] *= 0.6;
    }

    sum = 0.0;
    max = 0.0;
    idx = 0;
    for (streamX=0; streamX<snP->streamN; streamX++) {
      sum += locScore[streamX];
      if (locScore[streamX] > max) {
        max = locScore[streamX];
        idx = streamX;
      }
    }

    if (sum <= 0.0) {
      ERROR("invalid scores in frameX=%d\n",frameX);
      return TCL_ERROR;
    }

    for (streamX=0; streamX<snP->streamN; streamX++) {
      if (soft) {
        sns->streamWght->matPA[streamX][frameX] = locScore[streamX] / sum;
      } else {
        if (streamX == idx)
          sns->streamWght->matPA[streamX][frameX] = 1.0;
        else
          sns->streamWght->matPA[streamX][frameX] = 0.0;
      }
    }
  }

  /* ---------------------------- */
  /*  smooth weighting estimates  */
  /* ---------------------------- */
  cf = fmatrixCreate(streamN,frameN);
  for (streamX=0; streamX<streamN; streamX++) {
    for (frameX=0; frameX<frameN; frameX++) {
      n = 0;
      sum = 0.0;
      for (k=-winN; k<=+winN; k++) {
        idx = frameX+k;
        if ((idx >= 0) && (idx < frameN)) {
          sum += sns->streamWght->matPA[streamX][idx];
          n++;
        }
      }
      cf->matPA[streamX][frameX] = sum/((double) n);
    }
  }
  for (streamX=0; streamX<streamN; streamX++) {
    for (frameX=0; frameX<frameN; frameX++) {
      sns->streamWght->matPA[streamX][frameX] = cf->matPA[streamX][frameX];
    }
  }

  fmatrixFree(cf);
  free(locScore);
  sns->mixMode = 2;

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    snSetStreamWeights 
   ------------------------------------------------------------------------ */

static int snSetStreamWeights (Senone *snP, float* wght)
{
  int streamX;
  if (wght)
    memcpy(snP->stwA,wght,snP->streamN*sizeof(float));  

  for (streamX = 0; streamX < snP->streamN; streamX++)
    itfAppendResult ("%g ", snP->stwA[streamX]);

  return TCL_OK;
}

static int snSetStreamWeightsItf (ClientData cd, int argc, char **argv)
{
  int            ac = argc-1;
  FArray       wght = {0, 0};
  Senone*       snP = (Senone*)cd;
  int      retValue = TCL_OK;

 if ( itfParseArgv( argv[0], &ac, argv+1, 0,
    "-weight", ARGV_FARRAY, NULL, &wght,        NULL, "array of stream weights",
    NULL) != TCL_OK)  return TCL_ERROR;

 if (wght.itemN > 0 && wght.itemN != snP->streamN) {
   ERROR("snSetStreamWeightsItf: Invalid nr. of stream weights\n");
   return TCL_ERROR;
 }
 retValue= snSetStreamWeights (snP,wght.itemA);
 if (wght.itemA) free(wght.itemA);
 return retValue;
}


/* ------------------------------------------------------------------------
    snSetStreamAccu 
   ------------------------------------------------------------------------ */

static int snSetStreamAccu (Senone *snP, float* wght) {
  int streamN = (snP->dccu) ? 2*snP->streamN : snP->streamN;
  int streamX;

  if (wght) for (streamX = 0; streamX < streamN; streamX++)
    snP->accu[streamX] = wght[streamX];

  for (streamX = 0; streamX < streamN; streamX++)
    itfAppendResult ("%g ", snP->accu[streamX]);

  return TCL_OK;
}

static int snSetStreamAccuItf (ClientData cd, int argc, char **argv)
{
  int            ac = argc-1;
  FArray       wght = {0, 0};
  Senone*       snP = (Senone*) cd;
  int      retValue = TCL_OK;

 if (itfParseArgv (argv[0], &ac, argv+1, 0,
    "-accu", ARGV_FARRAY, NULL, &wght,        NULL, "array of stream accu values",
    NULL) != TCL_OK)  return TCL_ERROR;

 if (wght.itemN > 0 && wght.itemN != ((snP->dccu) ? 2*snP->streamN : snP->streamN)) {
   ERROR ("snSetStreamAccuItf: invalid nr. of stream accus (%d)\n", wght.itemN);
   return TCL_ERROR;
 }

 retValue = snSetStreamAccu (snP, wght.itemA);
 if (wght.itemA) free (wght.itemA);

 return retValue;
}


/* ------------------------------------------------------------------------
    snsSetStreamWeights 
   ------------------------------------------------------------------------ */

static int snsSetStreamWeights (SenoneSet *snsP, int globalFlag, int localFlag, float* wght)
{
  StreamArray* staP = &(snsP->strA); 
  int           snN =  snsP->list.itemN;
  int snX,streamX;

  if (globalFlag && wght)
    memcpy(staP->stwA,wght,staP->streamN*sizeof(float));

  if (localFlag && wght) {
    for (snX = 0; snX < snN; snX++) {
      Senone* snP = snsP->list.itemA+snX;
      if (snP->streamN != staP->streamN) {
	ERROR("snsSetStreamWeights: inconsistent nr. of streams in '%s'\n",snP->name);
	return TCL_ERROR;
      }
      memcpy(snP->stwA,wght,staP->streamN*sizeof(float));
    }
  }
  for (streamX = 0; streamX < staP->streamN; streamX++)
    itfAppendResult ("%g ", staP->stwA[streamX]);

  if (wght) snsWeightsClear (snsP);

  return TCL_OK;
}

static int snsSetStreamWeightsItf (ClientData cd, int argc, char **argv)
{ 
  int            ac = argc-1;
  int    globalFlag = 1;
  int     localFlag = 1;
  FArray       wght = {0, 0};
  SenoneSet*   snsP = (SenoneSet*)cd;
  int      retValue = TCL_OK;

 if ( itfParseArgv( argv[0], &ac, argv+1, 0,
    "-global", ARGV_INT,    NULL, &globalFlag,  NULL, "set weight global",
    "-local",  ARGV_INT,    NULL, &localFlag,   NULL, "set weights for each senone",
    "-weight", ARGV_FARRAY, NULL, &wght,        NULL, "array of stream weights",
    NULL) != TCL_OK)  return TCL_ERROR;

 if (wght.itemN > 0 && wght.itemN != snsP->strA.streamN) {
   ERROR("snsSetStreamWeightsItf: invalid nr. of stream weights\n");
   return TCL_ERROR;
 }
 retValue= snsSetStreamWeights (snsP,globalFlag,localFlag,wght.itemA);
 if (wght.itemA) free(wght.itemA);

 return retValue;
}


/* ------------------------------------------------------------------------ */
/*  sns Weights Training 
   ----------------------
    functionality in snStreamWeights.c                                      */
/* ------------------------------------------------------------------------ */

static int snsWeightsClearItf (ClientData cd, int argc, char *argv[]) {
  SenoneSet* snsP = (SenoneSet*) cd; 
  int          ac = argc-1;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
    NULL) != TCL_OK)  return TCL_ERROR;

  return snsWeightsClear (snsP);
}

static int snsWeightsAccuItf (ClientData cd, int argc, char *argv[]) {
  SenoneSet* snsP = (SenoneSet*) cd; 
  int          ac = argc-1;
  Path*     pathP = NULL;
  int           v = 0;
  int          zm = 1;
  int        from = 0;
  int          to = -1;
  int         idx = 0;
  char*       acc = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
    "<path>",    ARGV_OBJECT,  NULL, &pathP,    "Path", "path object",
    "-from",     ARGV_INT,     NULL, &from,NULL,        "first frame",
    "-to",       ARGV_INT,     NULL, &to,  NULL,        "last frame",
    "-accu",     ARGV_STRING,  NULL, &acc, NULL,        "for MMI: accu into 'num' (ref) or 'den' (hyp)",
    "-v",        ARGV_INT,     NULL, &v,   NULL,        "verbose information",
    "-zeroMode", ARGV_INT,     NULL, &zm,  NULL,        "don't train streams with weight 0",
    NULL) != TCL_OK)  return TCL_ERROR;

  if      (!acc)               idx = 0;
  else if (streq (acc, "num")) idx = 0;
  else if (streq (acc, "den")) idx = 1;

  return snsWeightsAccu (snsP, pathP, idx, from, to, v, zm);
}

static int snsWeightsUpdateItf (ClientData cd, int argc, char *argv[]) {
  SenoneSet* snsP = (SenoneSet*) cd; 
  int          ac = argc-1;
  float      mass = 1.0;
  float  contrast = 0.0;
  char*       txt = "dmc";
  char*       tie = "global";
  int      startX = 0;
  int          rc = TCL_OK;
  int         cla = 1;
  int           v = 0;
  int          cm = 1;
  float      minw = 1;
  float  minCount = 1;
  float         q;
  int           c;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
    "-mode", ARGV_STRING, NULL, &txt,  NULL, "dmc or mmi",
    "-tie",  ARGV_STRING, NULL, &tie,  NULL, "global, phone, state, or senone-based smoothing",
    "-mass", ARGV_FLOAT,  NULL, &mass, NULL, "probability mass to assign",
    "-contrast", ARGV_FLOAT,  NULL, &contrast, NULL, "contrast",
    "-minCount",ARGV_FLOAT,NULL,&minCount,NULL,"min count for update",
    "-start",ARGV_INT,    NULL, &startX,NULL,"first stream index to touch (0 or 1)",
    "-clear",ARGV_INT,    NULL, &cla,  NULL, "clear accus after update",
    "-iter", ARGV_INT,    NULL, &cm,   NULL, "loops",
    "-min",  ARGV_FLOAT,  NULL, &minw, NULL, "min weight, >=1 no control, <0 auto to -1*val",
    "-v",    ARGV_INT,    NULL, &v,    NULL, "verbosity",
    NULL) != TCL_OK)  return TCL_ERROR;

  /* checks for mode and tying ... */
  if        (!txt || (!streq (txt, "mmi")   && !streq (txt, "dmc"))) {
    ERROR ("snsWeightsUpdate: specify mode, one of dmc or mmi");
    return TCL_ERROR;
  } else if (!tie || (!streq (tie, "global") && !streq (tie, "Global") && !streq (tie, "senone") && !streq (tie, "tree"))) {
    ERROR ("snsWeightsUpdate: specify tying, one of {G|g}lobal, tree or senone\n");
    return TCL_ERROR;
  } else if (mass <= 0)
    mass = 1.0;

  /* q should be an indicator of quality */
  for (c = 0; c < cm; c++)
    rc = snsWeightsUpdate (snsP, txt, tie, minw, mass, contrast, startX, minCount, &q, v);

  if (cla) snsWeightsClear (snsP);

  return rc;
}

static int snsWeightsAccuMLEItf (ClientData cd, int argc, char *argv[]) {
  SenoneSet* snsP = (SenoneSet*) cd; 
  int          ac = argc-1;
  Path*     pathP = NULL;
  int           v = 0;
  int          zm = 0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
    "<path>",    ARGV_OBJECT,  NULL, &pathP, "Path", "path object",
    "-zeroMode", ARGV_INT,     NULL, &zm,     NULL,  "update streams with weight=0",
    "-v",        ARGV_INT,     NULL, &v,      NULL,  "verbose information",
    NULL) != TCL_OK)  return TCL_ERROR;

  return snsWeightsAccuMLE (snsP, pathP, v, zm);
}

static int snsWeightsUpdateMLEItf (ClientData cd, int argc, char *argv[]) {
  SenoneSet* snsP = (SenoneSet*) cd; 
  int          ac = argc-1;
  float         K = 2.0;
  float         M = 2.0;  
  float    minCnt = 15.0; 
  char*       txt = "senone";
  int        mode = 3;
  int          zm = 0;
  int          nm = 0;
  int        sIdx = 0;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
    "-minCnt",    ARGV_FLOAT,  NULL, &minCnt, NULL, "min. count to update",
    "-M",         ARGV_FLOAT,  NULL, &M,      NULL, "M-norm, M > 1",
    "-K",         ARGV_FLOAT,  NULL, &K,      NULL, "normalizer constant",
    "-zeroMode",  ARGV_INT,    NULL, &zm,     NULL, "update streams with weight=0",
    "-noiseMode", ARGV_INT,    NULL, &nm,     NULL, "zero noises before update?",
    "-mode",      ARGV_STRING, NULL, &txt,    NULL, "global, phone, state, or senone-based smoothing",
    "-startIdx",  ARGV_INT,    NULL, &sIdx,   NULL, "start index (0 or 1 is useful)",
    NULL) != TCL_OK)  return TCL_ERROR;

  /* Check the mode */
  if      (streq (txt, "global")) mode = SMOOTH_GLOBAL;
  else if (streq (txt, "phone"))  mode = SMOOTH_PHONE;
  else if (streq (txt, "state"))  mode = SMOOTH_STATE;
  else if (streq (txt, "senone")) mode = SMOOTH_SENONE;

  return snsWeightsUpdateMLE (snsP,minCnt,M,K,mode,zm,nm,sIdx);
}

static int snsWeightsLoadItf (ClientData cd, int argc, char *argv[]) {
  SenoneSet* snsP = (SenoneSet*) cd; 
  int          ac = argc-1;
  char*      name = NULL;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
    "<filename>", ARGV_STRING, NULL, &name, NULL, "file to load from",
    NULL) != TCL_OK)  return TCL_ERROR;

  return snsWeightsLoad (snsP,name);
}

static int snsWeightsSaveItf (ClientData cd, int argc, char *argv[]) {
  SenoneSet* snsP = (SenoneSet*) cd;
  int          ac = argc-1;
  char*      name = NULL;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
    "<filename>", ARGV_STRING, NULL, &name, NULL, "file to save to",
    NULL) != TCL_OK)  return TCL_ERROR;

  return snsWeightsSave (snsP, name);
}


/* ------------------------------------------------------------------------ */
/*  sns Cache Routines                                                      */
/* ------------------------------------------------------------------------ */

static int snsClearStreamCacheItf (ClientData cd, int argc, char *argv[]) {
  SenoneSet       *snsP = (SenoneSet*) cd; 
  StreamArray*     staP = &(snsP->strA);                                           
  DistribSet*      dssP = ((DistribStream*) staP->cdA[0])->dssP;
  CodebookSet*     cbsP = dssP->cbsP;
  int               dsN = 2*cbsP->list.itemN;

  int                ac = argc-1;
  int      i, j, frameN = 0;

  if (snsP->scoreAFctP == ssa_opt_str)
    dsN = staP->cBaseA[staP->streamN] - staP->cBaseA[1];
  if (staP->streamN == 1)
    dsN = dssP->list.itemN;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
    "-frameN", ARGV_INT, NULL, &frameN, NULL, "number of frames to clear",
    NULL) != TCL_OK) return TCL_ERROR;

  if (snsP->scoreAFctP != &ssa_opt_str && snsP->scoreAFctP != &ssa_opt_semCont)
    INFO ("scoreAFctP != ssa_opt_str or ssa_opt_semCont, clear has no effect!\n");

  /* free cache */
  if (!frameN || !dsN) {
    if (snsP->streamCache) fmatrixFree (snsP->streamCache);
    snsP->streamCacheX = -1;
    snsP->streamCache  = NULL;
    return TCL_OK;
  }

  if (!snsP->streamCache) 
    snsP->streamCache = fmatrixCreate (frameN, dsN);
  else
    fmatrixResize (snsP->streamCache, frameN, dsN);

  /* Reset the lines */
  snsP->streamCache->count = 0;
  for (i = 0; i < frameN; i++)
    for (j = 0; j < dsN; j++)
      snsP->streamCache->matPA[i][j] = -FLT_MAX;

  snsP->streamCacheX = -1;

  for (i = 0; i <= staP->streamN; i++)
    itfAppendResult ("%d ", staP->cBaseA[i]);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    snsInlineScoreArray    compute all scores marked in the score array array
                     as being active scores
   ------------------------------------------------------------------------ */

int snsInlineScoreArray( SenoneSet* snsP, int* idxA, float* scoreA, 
			 int  idxN, int frameX)
{
  int       streamX  = 0;
  StreamArray* staP  = &(snsP->strA); 
  DistribSet*  dssP  = ((DistribStream*) staP->cdA[streamX])->dssP;
  CodebookSet* cbsP  = dssP->cbsP;
  Senone*       snP  = snsP->list.itemA;
  Distrib*      dsP  = NULL;
  Codebook*     cbP  = cbsP->list.itemA + 0;
  float*    pattern  = NULL;
  CBX*         tmpI  = snsP->refXA;
  char*     activeA  = NULL;

  /*assume that all codebooks have the same dimN as the first one*/
  int          refN  = 0;
  int          dimN  = (cbP->cfg)->rdimN > 0 ? (cbP->cfg)->rdimN : cbP->dimN;
  int        dimN_4  = dimN / 4;
    
  /*bbi stuff*/
  BBITree                 *bbi = NULL;
  int                     pos,depth,depthX,adr=0,bbiN;
  BBINode                 *actNode;
  BBILeaf                 *bucket;
  CBX                     *bbilist;
  
  /*fast access*/
  register int            minDistIdx, dimX,i, bestI;
  register float          minDistSum;
 
  /* indices for senone, distrib, codebook*/
  int idx, snX, dsX, cbX;

  /* allocate active index array and init it (in trust we got it) */
  activeA =   malloc ((1+CBX_MAX) * sizeof(char));
  memset (activeA, 1, (1+CBX_MAX) * sizeof(char));
 
  /* get pattern (same feature space for all codebooks) */
  idx = 0;
  snX = idxA[idx];
  dsX = snP[snX].classXA[streamX];
  dsP = dssP->list.itemA + dsX;
  cbX = dsP->cbX;
  cbP = cbsP->list.itemA + cbX;
  pattern  = fesFrame(cbsP->feat,cbP->featX,frameX);

  /* get bbi tree if available */ 
  if (cbsP->bbi.itemA) {
    bbi = cbsP->bbi.itemA + cbP->bbiX;
    pos = 1; 
    depth = bbi->depthN;
    for (depthX=0; depthX<depth; depthX++) {
      actNode = (bbi->node)+pos;
      pos = (pos<<1) + (pattern[(int)actNode->k] > actNode->h);            
    }
    adr = pos-(1<<depth);            
  } 

  /* ------------------------------------------------------------------ */
  /* compute scores for all necessary distributions indexed in idxA     */
  /* ------------------------------------------------------------------ */
 
  for ( idx   = 0; idx < idxN; idx++) {
    
    /*find corresponding codebook from senone index idxA[i] */
    snX = idxA[idx];
    
    dsX  = snP[snX].classXA[streamX];
    dsP  = dssP->list.itemA + dsX;
    cbX  = dsP->cbX;
    cbP  = cbsP->list.itemA + cbX;
    refN = cbP->refN;

    /*bbi tree*/
    if (cbsP->bbi.itemA && cbP->bbiY >= 0) {
      bucket   = &bbi->leaf[adr][cbP->bbiY];
      bbiN     = bucket->cbvN;              
      bbilist  = bucket->cbX;               
    } else {
      bbiN    = refN;
      bbilist = tmpI;
    }

    /* ------------------------------------------------------------------*/
    /* Compute all the Mahalanobis distances for the subset of Gaussians */  
    /* ------------------------------------------------------------------*/

    minDistSum = 1E20;
    minDistIdx = 0;  
    bestI      = 0;

    for (i=0; i<bbiN; i++) {     
      register int     refX     =  bbilist[i];
      register float   *pt      =  pattern;
      register float   *rv      =  cbP->rv->matPA[refX];
      register float   *cv      =  cbP->cv[refX]->m.d;
      register float   distSum  =  cbP->pi + cbP->cv[refX]->det;

      if (activeA[i]) {
	for ( dimX = 0; dimX < dimN_4; dimX++) {
	  register float diff0;
	  if (distSum > minDistSum) break;
	  diff0 = *rv++ - *pt++;
	  distSum += diff0*diff0*(*cv++);
	  diff0 = *rv++ - *pt++;
	  distSum += diff0*diff0*(*cv++);
	  diff0 = *rv++ - *pt++;
	  distSum += diff0*diff0*(*cv++);
	  diff0 = *rv++ - *pt++;
	  distSum += diff0*diff0*(*cv++);
	}
	if (dimX == dimN_4) {
	  for (dimX = 4*dimN_4; dimX < dimN; dimX++) {
	    register float diff0 = *rv++ - *pt++;
	    distSum += diff0*diff0*(*cv++);
	  }
	}
	if (distSum < minDistSum) {
	  minDistSum = distSum;
	  minDistIdx = refX;
	  bestI      = i;
	}
      }
    }

    /*assume fully continuous system*/
    scoreA[idx]=  0.5 * (minDistSum + 2 * dsP->val[minDistIdx]);

    /* reorder gaussian in the bbi leaf */
    if (bestI != 0 && bbilist != tmpI) {
      bbilist[bestI] = bbilist[0];
      bbilist[0]     = minDistIdx;
    }
  }    

  free(activeA);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    snsSetScoreFct
   ------------------------------------------------------------------------ */

static int snsSetScoreFctItf( ClientData clientData, 
		      int argc, char *argv[])
{
  int         ac  = argc - 1;
  SenoneSet* snsP = (SenoneSet*)clientData;
  char*      name = "base";

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
     "<name>",ARGV_STRING, NULL, &name, NULL, "one of (base, opt, opt_thread, opt_semCont, opt_str, compress, old_base, old_opt)",
       NULL) != TCL_OK) return TCL_ERROR;

  snsP->scoreFctP  = NULL;
  snsP->scoreAFctP = NULL;

  if (streq (name, "base")) {
    snsP->scoreFctP  = &ssa_one;
    snsP->scoreAFctP = &ssa_base;

  } else if (streq (name, "opt")) {
    snsP->scoreFctP  = &ssa_one;
    snsP->scoreAFctP = &ssa_opt;
#ifdef MTHREAD
  } else if (streq (name, "opt_thread")) {
    snsP->scoreFctP  = &ssa_one;
    snsP->scoreAFctP = &ssa_opt_thread;
#endif

  } else if (streq (name, "base_semCont")) {
    snsP->scoreFctP  = &ssa_one;
    snsP->scoreAFctP = &ssa_base_semCont;

  } else if (streq (name, "opt_semCont")) {
    snsP->scoreFctP  = &ssa_one;
    snsP->scoreAFctP = &ssa_opt_semCont;

  } else if (streq (name, "opt_str")  || streq (name, "str_opt") || 
	     streq (name, "feat_opt") || streq (name, "opt_feat")) {
    snsP->scoreFctP  = &ssa_one;
    snsP->scoreAFctP = &ssa_opt_str;
  
  } else if (streq (name, "base_str")) {
    snsP->scoreFctP  = &ssa_one;
    snsP->scoreAFctP = &ssa_base_str;
  
  } else if (streq (name, "compress")) {
    snsP->scoreFctP  = &ssa_one;
    snsP->scoreAFctP = &ssa_compress;

  } else if (streq (name, "old_base") || streq (name, "base_old")) {
    snsP->scoreFctP  = &snsScore;
    snsP->scoreAFctP = &snsScoreArray;

  } else if (streq (name, "old_opt") || streq (name, "opt_old")) {
    snsP->scoreFctP  = &snsScore;
    snsP->scoreAFctP = &snsInlineScoreArray;

  } else if (snsP->scoreFctP == NULL) {
    WARN("snsSetScoreFct: invalid score function (use default)\n");
    snsP->scoreFctP  = &ssa_one;
    snsP->scoreAFctP = &ssa_opt;
  }

  return TCL_OK;
}

static int snsScoreMatrixItf (ClientData cd, int argc, char *argv[])
{
  int           ac  = argc - 1;
#ifdef SSE_OPT
  SenoneSet*   snsP = (SenoneSet*)cd;
  StreamArray* staP = &(snsP->strA); 
  DistribSet*  dssP = ((DistribStream*) staP->cdA[0])->dssP;
  CodebookSet* cbsP = dssP->cbsP;
  int         featX = cbsP->list.itemA[0].featX;
  Feature*        F = cbsP->feat->featA+featX;
#endif
  FMatrix*        f = NULL;
  int          topN = 1;
  char*        mode = "ds";

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
     "<matrix>", ARGV_CUSTOM, getFMatrixP, &f,    cd,   "matrix to score results in",
     "-topN",    ARGV_INT,    NULL,        &topN, NULL, "topN value for score function",
     "-mode",    ARGV_STRING, NULL,        &mode, NULL, "cb or ds",
       NULL) != TCL_OK) return TCL_ERROR;

#ifdef SSE_OPT
  ssa_matrix (dssP, f, F->data.fmat, topN, mode);
#else
  INFO ("Not implemented.\n");
#endif

  return TCL_OK;
}


/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method senoneMethod[] = 
{ 
  { "puts",       snPutsItf, NULL },
  { "setWeights", snSetStreamWeightsItf, "set stream weights" },
  { "setAccu",    snSetStreamAccuItf,    "set/get stream weights accu" },
  {  NULL,  NULL, NULL } 
};

TypeInfo senoneInfo = { 
        "Senone", 0, -1, senoneMethod, 
         NULL,           NULL,
         snConfigureItf, snAccessItf, itfTypeCntlDefaultNoLink,
	"Senone\n" } ;

static Method senoneTagMethod[] = 
{ 
  { "puts",   senoneTagPutsItf, NULL },
  {  NULL,    NULL, NULL } 
};

TypeInfo senoneTagInfo = { 
        "SenoneTag", 0, -1, senoneTagMethod, 
         NULL,           NULL,
         NULL,           NULL, itfTypeCntlDefaultNoLink,
	"SenoneTag\n" } ;

static Method streamNormItemMethod[] = 
{ 
  { "puts",   sniPutsItf,     NULL },
  { NULL,     NULL,           NULL }
};


TypeInfo streamNormItemInfo = { 
        "StreamNormItem", 0, -1, streamNormItemMethod, 
         NULL,      NULL,
         sniConfigureItf, NULL, NULL,
	"A stream normalizer\n" } ;


static Method senoneSetMethod[] = 
{ 
  { "puts",              snsPutsItf,              NULL },
  { "name",              snsNameItf,              "returns names of indexed senones" },
  { "index",             snsIndexItf,             "returns indices of named senones"},
  { "add",               snsAddItf,               "add a new senone to the set" },
  { "get",               snsGetModelItf,          "find a senone given phonetic context" },
  { "read",              snsReadItf,              "reads a senone description file" },
  { "write",             snsWriteItf,             "writes a senone description file" },
  { "load",              snsLoadItf,              "load a senone binary file" },
  { "save",              snsSaveItf,              "save a senone binary file" }, 
  { "accu",              snsAccuItf,              "accumulate training data for the given path" },
  { "update",            snsUpdateItf,            "update the underlying acoustic parameters" },
  { "reset",             snsResetItf,             "reset senoneSet" },
  { "score",             snsScoreItf,             "compute the score for a senone and a frame" },
  { "scoreMatrix",       snsScoreMatrixItf,       "compute the scores for senones in a matrix" },
  { "setScoreFct",       snsSetScoreFctItf,       "set score function (interface to Ibis)" },
  { "clearStreamCache",  snsClearStreamCacheItf,  "clear stream cache (opt_str score fct)" },

  { "addNorm",           snsAddNormItf,           "add a stream normalizer item" },
  { "clearNorm",         snsClearNormItf,         "clear stream normalizer" },
  { "saveNorm",          snsSaveNormItf,          "save stream normalizer" },
  { "loadNorm",          snsLoadNormItf,          "load stream normalizer" },

  { "clearMix",          snsClearMixItf,          "clear dynamic stream mixer" },
  { "updateMix",         snsUpdateMixItf,         "update dynamic stream mixer" },
  { "labelMix",          snsLabelMixItf,          "compute mixing weights based on labels" },

  { "accuWeights",       snsWeightsAccuItf,       "accu statistics to train stream weights (MMIE)" },
  { "updateWeights",     snsWeightsUpdateItf,     "update stream weights (MMIE)" },
  { "accuWeightsMLE",    snsWeightsAccuMLEItf,    "accumulate MLE statistics to train stream weights " },
  { "updateWeightsMLE",  snsWeightsUpdateMLEItf,  "MLE-update stream weights" },

  { "loadAccuWeights",   snsWeightsLoadItf,       "load MLE- or MMIE-update stream weights accu" },
  { "saveAccuWeights",   snsWeightsSaveItf,       "save MLE- or MMIE-update stream weights accu" },
  { "clearAccuWeights",  snsWeightsClearItf,      "clear update stream weights accu" },

  { "setWeights",        snsSetStreamWeightsItf,  "set stream weights" },

  {  NULL,    NULL,           NULL } 
};

TypeInfo senoneSetInfo = {
        "SenoneSet", 0, -1, senoneSetMethod, 
         snsCreateItf,      snsFreeItf,
         snsConfigureItf,   snsAccessItf, NULL,
	"Set of senones\n" } ;

extern int Distrib_Init();
extern int Stream_Init();
extern int DistribStream_Init();

#ifndef IBIS
extern int MicvStream_Init();
#endif

int Senones_Init ( )
{
  static int senonesInitialized = 0;

  if (senonesInitialized) return TCL_OK;

  if (Distrib_Init()        != TCL_OK) return TCL_ERROR;
  if (Stream_Init()         != TCL_OK) return TCL_ERROR;
  if (DistribStream_Init()  != TCL_OK) return TCL_ERROR;

#ifndef IBIS
  if (MicvStream_Init()     != TCL_OK) return TCL_ERROR;
#endif

  itfNewType ( &streamNormItemInfo );
  itfNewType ( &senoneInfo );
  itfNewType ( &senoneSetInfo );
  itfNewType ( &senoneTagInfo );

  snsDefaults.useN         = 0;
  snsDefaults.list.blkSize = 500;
  snsDefaults.commentChar  = ';';

  senonesInitialized = 1;
  return TCL_OK;
}

