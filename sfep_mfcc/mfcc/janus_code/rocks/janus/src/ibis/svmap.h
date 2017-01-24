/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Search Vocabulary Mapper
               ------------------------------------------------------------

    Author  :  Florian Metze
    Module  :  svmap.h
    Date    :  $Id: svmap.h 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 4.1  2003/08/14 11:20:10  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.22  2003/03/08 17:56:52  soltau
    Revert to previuos version

    Revision 1.1.2.20  2003/01/17 10:04:09  soltau
    Added Reverse Mapping

    Revision 1.1.2.19  2002/11/13 09:56:37  soltau
    Start and End words rely on LVX now
    Changes for optional words

    Revision 1.1.2.18  2002/06/26 11:57:55  fuegen
    changes for new directory structure support and code cleaning

    Revision 1.1.2.17  2002/06/13 08:07:28  metze
    Added dirty as a function in LMs

    Revision 1.1.2.16  2002/06/10 13:25:00  metze
    Added dirty tag

    Revision 1.1.2.15  2002/04/29 13:29:38  metze
    Redesign of LM interface (LingKS)

    Revision 1.1.2.14  2002/02/14 18:32:04  soltau
    *** empty log message ***

    Revision 1.1.2.13  2002/02/14 12:02:59  soltau
    Added svmapClearCache

    Revision 1.1.2.12  2002/02/13 13:48:11  soltau
    Added svmapGetLMScore

    Revision 1.1.2.11  2002/02/01 14:17:20  fuegen
    moved stypes.h into slimits.h

    Revision 1.1.2.10  2002/01/18 11:41:08  soltau
    Cleared container for separate map info

    Revision 1.1.2.9  2002/01/17 16:30:04  soltau
    Added svN, the current number of known swords ion svmap

    Revision 1.1.2.8  2001/07/12 18:39:29  soltau
    Added svmapInitLMWords

    Revision 1.1.2.7  2001/06/28 10:48:23  soltau
    Added Load/save routines for binary files to support fast decoder startup

    Revision 1.1.2.6  2001/06/18 07:59:34  metze
    svmapGet only returns the index, not the score

    Revision 1.1.2.5  2001/06/07 19:39:43  soltau
    Added svmapGet

    Revision 1.1.2.4  2001/05/23 08:17:23  metze
    Removed ruins of attempts to include multi-words here

    Revision 1.1.2.3  2001/05/11 16:41:01  metze
    Cleaned up LCT interface

    Revision 1.1.2.2  2001/05/11 09:46:19  metze
    Moved lz/lp to svMap

    Revision 1.1.2.1  2001/04/27 07:38:02  metze
    Separated svocab and svmap


   ======================================================================== */

#ifndef _svmap
#define _svmap

#include <limits.h>
#include "slimits.h"
#include "sglobal.h"
#include "list.h"
#include "dictionary.h"
#include "ngramLM.h"
#include "svocab.h"


/* ========================================================================
    Vocabulary to LanguageModel Mapper
   ======================================================================== */
 
struct SVMap_S {
  char*        name;
  int          useN;  
  SVocab*      svocabP;
  LingKS*      lksP;
  int          svN;        /* number of sv words                   */
  int          lnN;        /* number of lm words                   */
  LVX*         lvxA;       /* map : svX -> lvX                     */
  lmScore*     lvxSA;      /* additional score for each map entry  */
  int          dirty;      /* something-has-changed flag           */

  float        lz;         /* LM weighting factor                  */
  float        lalz;       /* LM weighting factor for LM-LookAhead */

  struct {                 /* insertion penalties                  */
    float      phone;      /* phone transition                     */
    float      word;       /* word  transition                     */
    float      fil;        /* (optional) filler  transition        */
  } penalty;

  struct {                 /* container for separate map info      */
    int        svN;        /* number of entries                    */
    SVX*       svXA;       /* list of svX indices                  */
  } XTbl;

  struct {                 /* reverse mapping                      */
    int*       N;          /* N[lvX] = offset in X for this lvX    */
    SVX*       X;          /* list of svX's beloging to lvX        */
  } rev;

  struct {                 /* mandatory language-model words       */
    char*      start;      /* start of sentence                    */
    char*      end;        /* end of sentence                      */
    char*      unk;        /* unknown word class                   */
  } string;

  struct {                 /* mandatory lm word indices            */
    LVX        start;
    LVX        end;
    LVX        unk;
  } idx;

  struct {                 /* lm score cache                       */
    int        N;          /* number of cache items per svX        */
    lmScore*   sA;         /* array of scores                      */
    LCT*       lA;         /* array of corresponding lct's         */
    int*       cA;         /* active cache entries                 */
    int*       xA;         /* ring-buffer index                    */
    int     hits;
    int     calls;
  } cache;

};


/* ========================================================================
    Module Interface
   ======================================================================== */

extern int     SVMap_Init       (void);
extern int     svmapLoad        (SVMap* svmapP, FILE* fp);
extern int     svmapSave        (SVMap* svmapP, FILE* fp);
extern int     svmapDoMapping   (SVMap* svmapP, lmScore *lnA);

extern LVX     svmapGet         (SVMap* svmapP, SVX svX);
extern int     svmapAdd         (SVMap* svmapP, SVX svX, LVX lvX, lmScore score);
extern int     svmapDelete      (SVMap* svmapP, SVX svX);
extern int     svmapCheck       (SVMap* svmapP);

extern lmScore svmapGetLMScore  (SVMap* svmapP, LCT lct, SVX svX);
extern int     svmapClearCache  (SVMap* svmapP);
extern int     svmapInitLMWords (SVMap* svmapP);
extern int     svmapIsDirty     (SVMap* svmapP);

#endif
