/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Linguistic Knowledge Source
               ------------------------------------------------------------

    Author  :  Florian Metze
    Module  :  lks.h
    Date    :  $Id: lks.h 3275 2010-07-01 20:36:51Z metze $
    Remarks : 

   ========================================================================

    This software was developed by

    the  University of Karlsruhe       and  Carnegie Mellon University
         Dept. of Informatics               Dept. of Computer Science
         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
         Am Fasanengarten 5                 5000 Forbes Ave
         D-76131 Karlsruhe                  Pittsburgh, PA 15213
         - Germany -                        - USA -

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
     Revision 4.2  2004/07/23 17:13:07  fuegen
     Moved Un/CompressScore macro definitions to lks.h to remove
     redefinition warnings during compilation.

     Revision 4.1  2003/08/14 11:20:03  fuegen
     Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

     Revision 1.1.2.6  2003/07/15 16:37:52  fuegen
     changed LingKS load/save interface
     added lingKSPutsFct

     Revision 1.1.2.5  2003/06/23 13:03:17  fuegen
     made it compatible to new CFG implementation
     added LingKS type CFG

     Revision 1.1.2.4  2003/02/02 12:22:36  soltau
     *** empty log message ***

     Revision 1.1.2.3  2002/06/13 08:06:54  metze
     Added dirty as a function in LMs

     Revision 1.1.2.2  2002/06/10 16:30:54  metze
     Added 'dirty' tag

     Revision 1.1.2.1  2002/04/29 12:07:41  metze
     Added lks.{c|h} to repository


   ======================================================================== */


#ifdef __cplusplus
extern "C" {
#endif

#ifndef _lks
#define _lks

#include "slimits.h"
#include "sglobal.h"
#include "bmem.h"
#include "lmla.h"
#include "ngramLM.h"
#include "phraseLM.h"
#include "metaLM.h"
#include "cfg.h"

/* -------------------------------------------------------------------------
    Definitions
   ------------------------------------------------------------------------- */

/* COMPRESSEDLM is defined in slimits.h and defines whether to use
   the compressed LM (scores are short int or signed char) or the full float-type LM */
#ifdef COMPRESSEDLM
  #define   CompressScore(score) (lmScore)((score)*LMSCORE_SCALE_FACTOR-0.5)
  #define UnCompressScore(score) (float)(score)/LMSCORE_SCALE_FACTOR
#else
  #define   CompressScore(score) (score)
  #define UnCompressScore(score) (score)
#endif


/* =======================================================================
     Forward declarations
   ======================================================================= */

enum LingKSType { LingKS_NULL, LingKS_LatticeLM, LingKS_NGramLM, LingKS_PhraseLM, LingKS_MetaLM, LingKS_CFG, LingKS_CFGSet };

typedef int      LingKSSaveFct       (LingKS* lksP, FILE* fp);
typedef int      LingKSLoadFct       (LingKS* lksP, FILE* fp, char* txt);
typedef int      LingKSSaveItfFct    (ClientData cd, int argc, char *argv[]);
typedef int      LingKSLoadItfFct    (ClientData cd, int argc, char *argv[]);
typedef int      LingKSPutsFct       (ClientData cd, int argc, char *argv[]);
typedef int      LingKSScoreArrayFct (LingKS* lksP, LCT lct, lmScore *arr, int n);
typedef lmScore  LingKSScoreFct      (LingKS* lksP, LCT lct, LVX  w,       int n);
typedef LCT      LingKSCreateLCT     (LingKS* lksP,          LVX  w);
typedef LCT      LingKSReduceLCT     (LingKS* lksP, LCT lct,               int n);
typedef LCT      LingKSExtendLCT     (LingKS* lksP, LCT lct, LVX  w);
typedef int      LingKSDecodeLCT     (LingKS* lksP, LCT lct, LVX* w,       int n);
typedef int      LingKSFreeLCT       (LingKS* lksP, LCT lct);
typedef int      LingKSitemN         (LingKS* lksP);
typedef LVX      LingKSitemX         (LingKS* lksP, char *name);
typedef char*    LingKSitemA         (LingKS* lksP, int i);
typedef int      LingKSIsDirty       (LingKS* lksP);


/* -----------------------------------------------------------------------
     Generic structure for Linguistic Knowledge Sources like
     LModels, Grammars, ...
   ----------------------------------------------------------------------- */

struct LINGKS_S {
  char*                name;
  int                  useN;
  lmScore*             cacheA;
  int                  cacheN;
  int                  type;          /* The type information (LingKSs)    */
  int                  dirty;         /* did-something-change flag         */

  LingKSSaveFct*       lingKSSaveFct; /* dumps a LingKS to file            */
  LingKSLoadFct*       lingKSLoadFct; /* reads a LingKS dump from file     */
  LingKSSaveItfFct*    lingKSSaveItfFct; /* saves a LingKS to file         */
  LingKSLoadItfFct*    lingKSLoadItfFct; /* loads a LingKS from file       */
  LingKSPutsFct*       lingKSPutsFct; /* prints LingKS content             */
  LingKSScoreArrayFct* scoreArrayFct; /* Fills an array with scores        */
  LingKSScoreFct*      scoreFct;      /* Returns a single score            */
  LingKSCreateLCT*     createLCT;     /* Creates an LCT                    */
  LingKSReduceLCT*     reduceLCT;     /* Reduces an LCT (Mgram -> M-1gram) */
  LingKSExtendLCT*     extendLCT;     /* Adds an LVX word to an LCT        */
  LingKSDecodeLCT*     decodeLCT;     /* Convert an LCT to a series of LVX */
  LingKSFreeLCT*       freeLCT;       /* Frees this LCT                    */
  LingKSitemN*         itemN;         /* Returns the # of items of the LM  */
  LingKSitemA*         itemA;         /* Returns item #i (an lvx)          */
  LingKSitemX*         itemX;         /* Returns the index of an lvx       */
  LingKSIsDirty*       isDirty;       /* Oops, I did it again              */

  union ks_p {                        /* Union for the data struct         */
    ClientData  cd;        /* <---- Sort of a generic pointer in the union */
    NGramLM*    ngramLM;
    PhraseLM*   phraseLM;
    MetaLM*     metaLM;
    CFG*        cfgP;
    CFGSet*     cfgSP;
  } data;

  float  interpol_wgt;  /* global interpolation weight */
  int    global_interpol; /* introduce a flag indicating global interpolation */
  

};


/* =======================================================================
     Functions Prototypes
   ======================================================================= */

extern int LKS_Init();
extern LingKS* LingKSCreate (char* name, int type);

#endif

#ifdef __cplusplus
}
#endif
