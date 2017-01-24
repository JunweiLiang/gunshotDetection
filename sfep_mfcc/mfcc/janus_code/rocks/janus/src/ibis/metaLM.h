/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Language model that can connect or mix LMs
               ------------------------------------------------------------

    Author  :  Florian Metze
    Module  :  metaLM.h
    Date    :  $Id: metaLM.h 3108 2010-01-30 20:41:26Z metze $

    Remarks :  This module is a meta language model, it needs
               base language models underneath and can interpolate ...
               them

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
    Revision 4.3  2004/11/12 16:28:33  fuegen
    Deactivated use of MetaLMElem.weight. Use MetaLMCoverItf instead.

    Revision 4.2  2004/07/23 17:13:07  fuegen
    Moved Un/CompressScore macro definitions to lks.h to remove
    redefinition warnings during compilation.

    Revision 4.1  2003/08/14 11:20:04  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.13  2003/03/11 16:37:23  metze
    Cacheing for MetaLM, Map for GLat computeLCT

    Revision 1.1.2.12  2002/06/12 10:09:51  metze
    Moved declarations

    Revision 1.1.2.11  2002/06/11 11:39:58  metze
    Code reorganization

    Revision 1.1.2.10  2002/05/03 14:01:57  metze
    Freeing of LingKS now works properly

    Revision 1.1.2.9  2002/04/29 13:29:37  metze
    Redesign of LM interface (LingKS)

    Revision 1.1.2.8  2002/02/07 20:52:58  metze
    Removed history for metaLM

    Revision 1.1.2.7  2002/02/07 13:36:55  soltau
    Added hct handling by hashing

    Revision 1.1.2.6  2002/02/06 14:18:40  soltau
    Access correct interpolation weights

    Revision 1.1.2.5  2002/02/05 19:27:15  soltau
    Added tmp score arrays to make interpolation working

    Revision 1.1.2.4  2002/02/05 11:30:35  soltau
    n -> order

    Revision 1.1.2.3  2001/10/22 08:41:02  metze
    Changed interface to score functions

    Revision 1.1.2.2  2001/10/03 15:53:49  metze
    Basic implementation

    Revision 1.1.2.1  2001/09/26 08:58:08  metze
    Renamed interpolLM to metaLM

   ======================================================================== */


#ifdef __cplusplus
extern "C" {
#endif


#ifndef _metalm
#define _metalm


#include "list.h"
#include "svmap.h"
#include "lmla.h"


/* -------------------------------------------------------------------------
    Definitions
   ------------------------------------------------------------------------- */
#define MAXLMLINE 1024


/* -------------------------------------------------------------------------
   HashLVX
   ------------------------------------------------------------------------- */

typedef struct HashLVX_S     HashLVX;
typedef struct LIST(HashLVX) HashLVXList;

struct HashLVX_S {
  LCT    lct;   /* LCT to to expand    */
  LCT    _new;   /* LCT expanded lb lvX */ /* renamed from "new" */
  LVX    lvx;   /* lvx                 */
};


/* -------------------------------------------------------------------------
    MetaLMPathItem
   ------------------------------------------------------------------------- */

typedef struct METALMPATHITEM_S MetaLMPathItem;

struct METALMPATHITEM_S {
  MetaLMPathItem* fromP;
  MetaLMPathItem* toP;
  MetaLMPathItem* nextP;
  LCT*            lctA;
  float           prob;  /* interpolation weight */
};

/* -------------------------------------------------------------------------
   MetaLCT
   ------------------------------------------------------------------------- */

typedef struct MetaLCT_S     MetaLCT;
typedef struct LIST(MetaLCT) MetaLCTList;

struct MetaLCT_S {
  LCT    lctA;  /* lct A                */
  LCT    lctB;  /* lct B                */
  float  prob;  /* interpolation weight */
};


/* ========================================================================
    MetaLMItem
   ------------
    One language model word
    
    Currently, the score funtion will interpolate the LMs A and B with a
    given weigth, however, it should theoretically be possible to add
    something like a 'mode' here, which would make it possible to use
    different LMs or different score functions for different words.
   ======================================================================== */

typedef struct MetaLMItem_S {

  char             *name; /* The name                               */

  unsigned char      lmA; /* The list index of the first LM         */
  unsigned char      lmB; /* The list index of the second LM        */

  LVX               idxA; /* The index in the first LM              */
  LVX               idxB; /* The index in the second LM             */

  MetaLMPathItem*  pathP; /* A pointer to MetaLM paths for this lvX */

  double            prob; /* A probability                          */

  char             isStopWord; /* mark if the word is a stopword    */


} MetaLMItem;

typedef struct LIST(MetaLMItem) MetaLMItemList;


/* ========================================================================
    MetaLMElem
   ------------
    One underlying language model
   ======================================================================== */

typedef struct MetaLMElem_S {

  char    *name; /* Name                       */
  LingKS  *lksP; /* Pointer to container       */
  double weight; /* A weight
		    fuegen, 12.11.2004: This is not in use. Use
		    MetaLMCoverItf instead. It is left here, because of
		    compatibility reasons to older dump files. */

} MetaLMElem;

typedef struct LIST(MetaLMElem) MetaLMElemList;


/* ========================================================================
    Meta Language Model
   ---------------------
    e.g. for language model interpolation
   ======================================================================== */

struct MetaLM_S {

  LingKS*        lksP;     /* The linguistic info container            */
  int            order;    /* The maximal history                      */
  LCT            bitmask;  /* Needed for LCT handling                  */
  lmScore*       tmpA;     /* temp. score array for interpolation      */
  lmScore*       tmpB;     /* temp. score array for interpolation      */

                           /* additional hashtable to cache lct,lvx -> lct */
  HashLVXList    lvxList;  /* A list of hashed LVXs                        */

  MetaLCTList    mlctList;

  MetaLMItemList list;     /* The list of LM-words                     */
  MetaLMElemList subl;     /* The list of sub-LMs                      */

  struct {                 /* Structure for the Path structure         */
    BMem*        node;
    BMem*        lct;
    int          subN;
  } mem;

};


extern int        MetaLM_Init        ();
extern int        MetaLMInit         (LingKS* lksP, MetaLM* lmP, char* name);
extern int        MetaLMDeinit                     (MetaLM *lmP);
extern int        MetaLMConfigureItf (ClientData cd, char *var, char *val);
extern ClientData MetaLMAccessItf    (ClientData cd, char *name, TypeInfo **ti);


#endif

#ifdef __cplusplus
}
#endif
