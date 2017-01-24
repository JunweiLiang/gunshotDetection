/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Language model that can do phrases (multi-words)
               ------------------------------------------------------------

    Author  :  Florian Metze
    Module  :  phraseLM.h
    Date    :  $Id: phraseLM.h 2468 2004-07-23 17:13:08Z fuegen $

    Remarks :  This module implements a phrase language model, it needs
               a standard language model below

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
    Revision 4.2  2004/07/23 17:13:08  fuegen
    Moved Un/CompressScore macro definitions to lks.h to remove
    redefinition warnings during compilation.

    Revision 4.1  2003/08/14 11:20:05  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.8  2002/05/03 14:01:46  metze
    Freeing of LingKS now works properly

    Revision 1.1.2.7  2002/04/29 13:29:38  metze
    Redesign of LM interface (LingKS)

    Revision 1.1.2.6  2001/10/22 08:41:31  metze
    Changed interface to score functions

    Revision 1.1.2.5  2001/10/15 16:09:46  soltau
    Remove bitmask

    Revision 1.1.2.4  2001/06/18 07:56:13  metze
    Compilation and LCT handling under SUN works

    Revision 1.1.2.3  2001/06/01 10:15:37  metze
    Cosmetic changes

    Revision 1.1.2.2  2001/06/01 10:09:02  metze
    Made changes necessary for decoding along lattices

    Revision 1.1.2.1  2001/05/23 08:20:28  metze
    First revision of multi-word language model (might eventually become included in lmodelNJD)


   ======================================================================== */


#ifdef __cplusplus
extern "C" {
#endif


#ifndef _lmphrases
#define _lmphrases


#include "list.h"
#include "svmap.h"
#include "lmla.h"
#include "ngramLM.h"


/* -------------------------------------------------------------------------
    Definitions
   ------------------------------------------------------------------------- */
#define MAXLMLINE 1024

/* ========================================================================
    PhraseLMItem
   ---------------
    One language model word
   ======================================================================== */

typedef struct PhraseLMItem_S {

  char              *name;
  struct PhraseLM_S *lmP;
  lmScore            score;
  int                lnxN;
  LVX               *lnxA;

} PhraseLMItem;

typedef struct LIST(PhraseLMItem) PhraseLMList;


/* ========================================================================
    Phrase language model (multi-words)
   ======================================================================== */

struct PhraseLM_S {

  /*  char*        name; */   /* The name of the object                   */
  /* int          useN;  */  /* useN                                     */
  LingKS*      lksP;    /* The lingustic info container             */
  LingKS*      baseLM;  /* The pointer to the base LM               */
  int          baseN;   /* The number of items in the base LM       */
  float        bias;    /* A scaling/ biasing parameter := lp/lz    */

  int          order;   /* The order (max number of words per multiw) */
  int          history; /* The max history */
  
  PhraseLMList list;
};


extern int        PhraseLM_Init        ();
extern int        PhraseLMInit         (LingKS* lksP, PhraseLM* lmP, char* name);
extern int        PhraseLMDeinit                     (PhraseLM *lmP);
extern int        PhraseLMConfigureItf (ClientData cd, char *var, char *val);
extern ClientData PhraseLMAccessItf    (ClientData cd, char *name, TypeInfo **ti);


#endif

#ifdef __cplusplus
}
#endif
