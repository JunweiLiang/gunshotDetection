/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: WordGraphs, PhoneGraphs, StateGraphs
               ------------------------------------------------------------

    Author  :  Ivica Rogina & Michael Finke
    Module  :  hmm.h
    Date    :  $Id: hmm.h 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 4.3  2003/08/14 11:19:57  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 4.2.56.1  2002/02/04 21:00:13  soltau
    Support for full context dependent single phone words

    Revision 4.2  2000/08/27 15:31:17  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 4.1.4.1  2000/08/25 22:19:44  jmcd
    Just checking.

    Revision 4.1  2000/01/12 10:02:02  fuegen
    make clustering faster
    integrate tags into hmm structure

    Revision 4.0  1998/11/30 18:07:14  fuegen
    *** empty log message ***

    Revision 3.4  1997/07/31 17:16:06  rogina
    fixed typo

    Revision 3.3  1997/07/31 17:08:08  rogina
    made code gcc -DFKIclean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.10  96/11/27  13:18:06  monika
    patch for training without cross word triphones added by Michael Finke
    
    Revision 1.9  96/01/31  05:05:39  finkem
    new senoneSet setup and ptree moved to tree (finally?)
    
    Revision 1.8  1995/11/30  15:06:11  finkem
    handling pronunciation variants corrected.

    Revision 1.7  1995/11/17  22:25:42  finkem
    *** empty log message ***

    Revision 1.6  1995/10/16  15:53:37  finkem
    create context dependend phone/state graphs

    Revision 1.5  1995/10/04  23:42:02  torsten
    *** empty log message ***

    Revision 1.4  1995/09/27  17:49:58  rogina
    *** empty log message ***

    Revision 1.3  1995/08/10  08:11:54  rogina
    *** empty log message ***

    Revision 1.2  1995/08/04  14:18:13  rogina
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _hmm
#define _hmm

#include "list.h"
#include "array.h"
#include "dictionary.h"
#include "amodel.h"

/* ========================================================================
    StateGraph
   ======================================================================== */

typedef struct
{
  int        stateN;   /* number of states in the graph */
  int*       senoneX;  /* array of senone indices       */
  int*       transX;   /* array of transition indices   */
  int*       phoneX;   /* array of phone indices        */
  int*       wordX;    /* array of word indices         */

  SenoneSet* snsP;

  int        initN;    /* number of initial states      */
  int*       init;     /* indices of initial states     */

  int*       fwdTrN;   /* number of forward transitions for every word */
  int**      fwdIdx;   /* a vertex-index for every words successors    */
  float**    fwdPen;   /* a penalty for every forward transition       */ 

} StateGraph;

extern int         sgraphInit(   StateGraph* SG, SenoneSet* snsP, AModel* amP);
extern StateGraph* sgraphCreate( SenoneSet* snsP, AModel* amP);
extern int         sgraphReset(  StateGraph* SG);
extern int         sgraphDeinit( StateGraph* SG);
extern int         sgraphFree(   StateGraph* SG);

extern float**     sgraphLogPen2Prob( StateGraph* SG);

/* ========================================================================
    PhoneGraph
   ======================================================================== */

/* ------------------------------------------------------------------------
    PhoneGraphContext    describes the context (either left or right) to
                         a given phoneGraph. The context consists of the 
                         index of a phone, a tag and a list of entry points
                         to or exit points from the graph.
   ------------------------------------------------------------------------ */

typedef struct
{
  int   phone;
  int   tag;

  int   stateN;
  int*  stateXA;

} PhoneGraphContext;

typedef struct PhoneGraph_S
{
  int          phoneN;     /* number of phones (vertices) in the graph      */
  int*         phone;      /* one monophone index for every vertex          */
  int*         tag;        /* one tag (bitset) for every vertex             */
  int*         word;       /* relative word index                           */

  StateGraph** stateGraph; /* one stateGraph pointer for every phone        */

  int*         amodel;     /* one acoustic model index for every phone      */
  AModelSet*   amsP;       /* pointer to an acoustic model set              */

  int          initN;      /* number of initial phones                      */
  int*         init;       /* indices of initial phones                     */

  int*         fwdTrN;     /* number of forward transitions for every phone */
  int**        fwdIdx;     /* a vertex-index for every phones successors    */
  float**      fwdPen;     /* a penalty for every forward transition        */ 

  int          modTags;    /* modality tags for all phones in phoneGraph
			      (tags for one word) */
  int          modMask;    /* mask to unset all modality tags
			      (inverse of modTags) */

} PhoneGraph;

extern int         pgraphInit(   PhoneGraph* PG,  AModelSet* amsP,
                                                  Dictionary* dict, int idx,
                                                  int variants, int modTags);
extern PhoneGraph* pgraphCreate( AModelSet* amsP, Dictionary* dict, int idx,
                                                  int variants, int modTags);
extern int         pgraphReset(  PhoneGraph* PG);
extern int         pgraphDeinit( PhoneGraph* PG);
extern int         pgraphFree(   PhoneGraph* PG);

extern int pgraphMake( PhoneGraph* pgraphP, IArray* parray, char* transS,
                                            IArray* iarray);

extern int pgraphMakeSGraph( PhoneGraph* pgraphP);

/* ========================================================================
    WordGraph
   ======================================================================== */

typedef struct
{
  int          wordN;       /* number of words (vertices) in the graph      */
  int*         word;        /* one word index for every vertex              */
  PhoneGraph** phoneGraph;  /* one pointer to a phoneGraph for every word   */

  Dictionary*  dict;        /* dictionary the word graph is based on        */
  AModelSet*   amsP;       /* pointer to an acoustic model set              */

  int          initN;       /* number of initial words                      */
  int*         init;        /* indices of initial words                     */

  int*         fwdTrN;      /* number of forward transitions for every word */
  int**        fwdIdx;      /* a vertex-index for every words successors    */
  float**      fwdPen;      /* a penalty for every forward transition       */

  int*         modTagsA;    /* array of modTags (set by hmmSetModalityTagsItf,
			       read by hmmGetModalityTags (linked to wgraph)) */
} WordGraph;

extern int        wgraphInit(   WordGraph* WG,   Dictionary* dict,
                                AModelSet* amsP, IArray* waP);
extern WordGraph* wgraphCreate( Dictionary* dict,AModelSet* amsP,IArray* waP);
extern int        wgraphReset(  WordGraph* WG);
extern int        wgraphDeinit( WordGraph* WG);
extern int        wgraphFree(   WordGraph* WG);

extern int        wgraphMake(   WordGraph* WG, IArray* warray, char* transS,
                                               IArray* iarray, int optWord,
                                               int variants);

extern int wgraphBuildContext( WordGraph*           wgraphP,
                        PhoneGraphContext*** ppleft,  int** ppleftN,
                        PhoneGraphContext*** ppright, int** pprightN);

/* ========================================================================
    HMM
   ======================================================================== */

typedef struct
{
  char*         name;
  int           useN;

  Dictionary*   dict;
  AModelSet*    amodels;

  WordGraph*    wordGraph;
  PhoneGraph*   phoneGraph;
  StateGraph*   stateGraph;

  int           logPen;
  int           full;
  int           xwmodels;      /* use cross word models                           */
  int           rcmSdp;        /* use right context models for single phone words */

} HMM;

extern int  hmmInit(   HMM* hmm, char* name, Dictionary* dict, 
                                             AModelSet*  amodelSet);
extern HMM *hmmCreate (char *name, Dictionary *dict, AModelSet *amodelSet);
extern int  hmmDeinit( HMM *hmm);
extern int  hmmFree(   HMM *hmm);

/* ========================================================================
    Module Interface
   ======================================================================== */

extern int sgraphBuild( StateGraph* sgraphP, PhoneGraph* pgraphP, int logPen);
extern int pgraphBuild( PhoneGraph* pgraphP, WordGraph*  wgraphP, int logPen,
			int full, int xwmodels, int rcmSdp);

extern int HMM_Init (void);


#endif



#ifdef __cplusplus
}
#endif
