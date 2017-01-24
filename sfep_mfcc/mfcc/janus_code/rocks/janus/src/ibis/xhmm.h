/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Crossword Phone Hidden Markov Model
               ------------------------------------------------------------

    Author  :  Hagen Soltau
    Module  :  xhmm.h
    Date    :  $Id: xhmm.h 2794 2007-03-19 15:09:44Z fuegen $
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
    Revision 1.5  2007/03/19 15:09:44  fuegen
    fixed some comments

    Revision 1.4  2003/08/14 11:20:11  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.3.4.6  2002/02/01 14:17:21  fuegen
    moved stypes.h into slimits.h

    Revision 1.3.4.5  2002/01/27 15:09:48  soltau
    added ignoreRCM option in xcmSet

    Revision 1.3.4.4  2001/07/12 18:42:09  soltau
    Added xcm

    Revision 1.3.4.3  2001/06/28 10:48:24  soltau
    Added Load/save routines for binary files to support fast decoder startup

    Revision 1.3.4.2  2001/02/01 18:41:53  soltau
    moved typedefs to sglobal

    Revision 1.3.4.1  2001/01/29 18:01:21  soltau
    CONTEXT -> CTX

    Revision 1.3  2001/01/25 11:01:36  soltau
    temporary checkin to make Christian happy

    Revision 1.2  2001/01/18 13:49:06  soltau
    Chnaged types, e.g. (UCHAR -> CONTEXT)

    Revision 1.1  2000/12/29 06:19:08  soltau
    Initial revision

 
   ======================================================================== */

#ifndef _xhmm
#define _xhmm

#include <limits.h>
#include "slimits.h"
#include "sglobal.h"
#include "list.h"
#include "phmm.h"

/* ========================================================================
    Left context HMM
   ======================================================================== */

/* For dynamic mapped left context models, we have to consider a PHMM,
   indexed by hmmXA[leftContextX] for each left context, e.g. phone.
   hmmXA holds indices for the PHMMSet array.
 */

struct LCM_S {
  CTX     hmmN;
  PHMMX*  hmmXA;
};

typedef struct LIST(LCM) LCMList;

struct LCMSet_S {
  char*     name;
  int       useN;
  LCMList   list;
  PHMMSet*  hmmsP;
};

/* ========================================================================
    Right context HMM
   ======================================================================== */

/* Since we have to consider all different right context models, (and not only
   the single best model as for left contexts) we store only the uniqe models 
   here in hmmXA and hmmN and provide a right-context access by the mapA array,
   mapA[rightContextX] map to a index uniqX for the hmmXA array. 
   mapN is the number of different right contexts, e.g. nr. of phones.
 */

struct RCM_S {
  CTX      hmmN;
  PHMMX*   hmmXA;
  CTX      mapN;
  CTX*     mapXA;
};

typedef struct LIST(RCM) RCMList;

struct RCMSet_S {
  char*     name;
  int       useN;
  RCMList   list;
  PHMMSet*  hmmsP;
};

/* ========================================================================
    Full context HMM
   ======================================================================== */

/* We need here a full context map to model left and right context dependencies 
   for single phone words. For each left context, mapXA define a map from
   right contexts to indices in the hmmXA array, which refer to the PHMM indices.
   hmmNA[leftX] contain the number of different PHMM indices for all possible
   right contexts, given a left context.
   hmmXA[leftX][mapXA[leftX][rightX]] = PHMM index for that particluar context
*/

struct XCM_S {
  CTX      mapL;         /* number of different left  phonetic context */
  CTX      mapR;         /* number of different right phonetic context */
  CTX      hmmMax;       /* maximum number of different hmm's indices  */
  CTX**    mapXA;
  CTX*     hmmNA;
  PHMMX**  hmmXA;
};

typedef struct LIST(XCM) XCMList;

struct XCMSet_S {
  char*     name;
  int       useN;
  int       ignoreRCM;
  XCMList   list;
  PHMMSet*  hmmsP;
};

/* ========================================================================
    Module Interface
   ======================================================================== */
extern int      XHMM_Init();
extern LCMSet*  lcmSetCreate (char* name,PHMMSet* hmmSP);
extern int      lcmSetFree   (LCMSet* lcmsP);
extern int      lcmSetAdd    (LCMSet* lcmsP,CTX hmmN,PHMMX* hmmXA);
extern int      lcmSetLoad   (LCMSet* lcmsP,FILE* fp);
extern int      lcmSetSave   (LCMSet* lcmsP,FILE* fp);

extern RCMSet*  rcmSetCreate (char* name,PHMMSet* hmmSP);
extern int      rcmSetFree   (RCMSet* rcmsP);
extern int      rcmSetAdd    (RCMSet* rcmsP,CTX hmmN,PHMMX* hmmXA);
extern int      rcmSetLoad   (RCMSet* rcmsP,FILE* fp);
extern int      rcmSetSave   (RCMSet* rcmsP,FILE* fp);

extern XCMSet*  xcmSetCreate (char* name,PHMMSet* hmmSP, int ignoreRCM);
extern int      xcmSetFree   (XCMSet* xcmsP);
extern int      xcmSetAdd    (XCMSet* xcmsP,CTX hmmL,CTX hmmR,PHMMX** hmmXA);
extern int      xcmSetLoad   (XCMSet* xcmsP,FILE* fp);
extern int      xcmSetSave   (XCMSet* xcmsP,FILE* fp);

#endif
