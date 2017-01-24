/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Bigram and Trigram Language Models
               ------------------------------------------------------------
               
    Author  :  Monika Woszczyna
    Date    :  $Id: lm_3g.c 2390 2003-08-14 11:20:32Z fuegen $
    Remarks :  further documentation below copyright notice.

   ========================================================================
               
    This software was developed by
    The Interactive Systems Laboratories at

    the  University of Karlsruhe       and  Carnegie Mellon University
         Dept. of Informatics               Dept. of Computer Science
         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
         Am Fasanengarten 5                 5000 Forbes Ave
         D-76131 Karlsruhe                  Pittsburgh, PA 15213
         - West Germany -                   - USA -

    Copyright (C) 1990-1995.   All rights reserved.

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

   =========================================================================

    $Log$
    Revision 3.13  2003/08/14 11:20:23  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.12.20.5  2003/07/03 07:58:39  fuegen
    b flag under Linux not supported for popen

    Revision 3.12.20.4  2003/07/02 17:59:23  fuegen
    fixes for file handling under Windows
    using general functions for file open/close

    Revision 3.12.20.3  2002/11/20 13:21:12  soltau
    lmScore -> lmScoreLocal

    Revision 3.12.20.2  2002/06/26 11:57:58  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.12.20.1  2002/04/29 11:57:48  metze
    Cleaner compiling on gcc

    Revision 3.12  2000/11/14 12:29:35  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 3.11.34.2  2000/11/08 17:21:28  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 3.11.34.1  2000/11/06 10:50:33  janus
    Made changes to enable compilation under -Wall.

    Revision 3.11  1998/07/03 08:52:21  kries
    oops

 * Revision 3.9  1998/07/01  14:45:59  kries
 * oops
 *
    Revision 3.6  1998/07/01 12:13:26  kries
    added global configuration of segment sizes

 * Revision 3.5  1998/06/11  14:31:40  kries
 * small adaptations
 *
 * Revision 3.4  1998/05/26  18:27:23  kries
 * adaptation to new language model interface
 *
 * Revision 3.3  1997/07/18  17:57:51  monika
 * gcc-clean
 *
    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.23  1997/01/12  01:18:49  kries
    Corrected serious error in 1.22

    Revision 1.22  1996/11/30  20:24:31  kries
    Segment Size can be swithout recompilation
    Class based adaptation

    Revision 1.21  1996/11/22  16:29:21  hhild
    modified lm_reset_context (Detlef), added warning in save_log

    Revision 1.20  96/08/15  07:14:51  07:14:51  kries (Klaus Ries)
    linear interpolation enhanced, cache model, stubs for trigger pairs,
    class model enhanced
    
    Revision 1.19  1996/07/03  13:18:42  monika
    changes to work with 64K vocabulary

    Revision 1.18  96/03/24  05:04:23  05:04:23  kries (Klaus Ries)
    added stub for lattice module resetting the long distance context
    
    Revision 1.17  1996/02/07  21:37:43  kries
    Reintroduced behaviour of 1.12 for begin of sentence

    Revision 1.16  1996/02/04  16:36:40  kries
    Removed bug in cacheline introduced in 1.14
    Don't use 1.14 and 1.15

    Revision 1.15  1996/01/29  20:10:04  kries
    __lm_UgScore --> local_lm_UgScore ( etc. )
    made static declaration of these functions consistent

    Revision 1.14  1996/01/28  18:29:59  kries
    fix possible errors at begin of sentence
    simple interpolation scheme
    .

    Revision 1.13  1996/01/25  21:37:52  kries
    extension of file-formats (including binary) and functionality
    to support a separate unigram distribution and class based model
    all old files should work as usual

    Revision 1.12  1996/01/17  02:40:52  finkem
    added ClientData typecast to most lmSetCurrent calls

    Revision 1.11  1996/01/14  14:19:14  monika
    always give lm-pointer when calling lm routines

    Revision 1.10  1995/10/26  20:44:57  finkem
    scoreItf changed so that it handles pron. variants and lz lp rescoring

    Revision 1.9  1995/10/04  23:40:28  torsten
    *** empty log message ***

    Revision 1.8  1995/09/08  11:01:45  monika
    removed bug with NON-LM words (NO_WORD <0) which really is an int/ushort problem.
    also set default for zerogram to -9999 instead of 9999

    Revision 1.7  1995/09/06  07:32:47  kemp
    *** empty log message ***

    Revision 1.6  1995/08/29  02:49:50  finkem
    *** empty log message ***

    Revision 1.5  1995/08/17  17:07:40  rogina
    *** empty log message ***

    Revision 1.4  1995/08/10  08:00:40  finkem
    *** empty log message ***

    Revision 1.3  1995/08/06  21:03:42  finkem
    *** empty log message ***

    Revision 1.2  1995/08/03  14:42:23  finkem
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

  
   ========================================================================= */

char lm_3gRCSVersion[] = "$Id: lm_3g.c 2390 2003-08-14 11:20:32Z fuegen $";


static int lmInitialized = 0;


/*
 * general comments and explanations:
 * ----------------------------------
 * the lm module can handle an arbitray amount of language models loaded
 * at the same time, but only ONE is active at a time. The reason for that
 * is that the rather big cache structur should only be allocated once.
 *
 * Bigram probs and bo-wts, and trigram probs are kept in separate tables
 * rather than within the BIGRAM_ITEM and TRIGRAM_ITEM structures. These tables
 * hold unique prob and bo-wt values, and can be < 64K long (see lm_3g.h).
 * The following tree structure is used to construct these tables of unique
 * values.  Whenever a new value is read from the LM file, the sorted tree
 * structure is searched to see if the value already exists, and inserted
 * if not found.
 ****************************************************************************/
 
#define  SMALLPROB              -9999.0
#define  ILLEGAL_PROB           99999.0
#define  MAX_SORTED_ENTRIES       65534

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <limits.h>
#include "itf.h"
#include "hash.h"
#include "lm_3g.h"
#include "search.h"
#include "treefwd.h"
#include "lattice.h"
#include "array.h"
#include "hypos.h"
#include "mach_ind_io.h"

#if defined(TRIGGER)
#include "/afs/cs/project/link/include/k005.api.h"
#endif

static char        *start_sym = "<s>"; /* HIER clean this  ( ) <s> </s> mess */
static char        *end_sym = "</s>";
static LmStruct    *currentLmP = NULL; /* The CURRENTLY active LM           */

static int   find_free_cacheline  ( LmStruct *modelP );
static int   find_bigram  (LmStruct* mP, INT   w1, int   w2);
static void  lm_set_param (LmStruct *modelP, 
                           float     lw,
                           float     uw,
                           float     wip,
                           int       word_pair);
static char *salloc (char *str);
static char *listelem_alloc  (INT elem_size);
static void listelem_free (CHAR *elem, int elem_size);
static int  wstr2lmwid (LmStruct * modelP, char *w);
static int  map_lmwid_to_vocabwid (LmStruct *modelP);
static int  remove_nonvocab_entries (LmStruct * model);
static int  lm_WriteDump(LmStruct *modelP, char *dumpfilename);
static int  lmRead (LmStruct * modelP,char* filename);
static int  lmReadDump(LmStruct *modelP, FILE *fp);
static int  lmWriteEmpty (char * myfile);
static int  lmWriteFromHypoList (char * myfile,HypoList * hypoListP, Vocab* vocabP);
static int cacheCreate(LmStruct* modelP,int cacheLineN);
static LmStruct *lmCreate(char * name, Vocab * vocabP, int bg_seg_sz, int log_bg_seg_sz, float lw, float zero,
                          float lpen, int cacheLineN);

static LmStruct lmDefault;

static float           local_lm_UgScore ( LmStruct* mP, int w1);     /* in case of a real call */
static float  	       local_lm_BgScore ( LmStruct* mP, int w1, int w2 );         /* in case of a real call */
static float  	       local_lm_TgScore ( LmStruct* mP, int w1, int w2, int w3 ); /* in case of a real call */


static float save_log(double t) {
  if(t>0) {
    return log(t);
  } else {
    fprintf(stderr, "Negative LOG! t = %f\n", t); abort();
  }
}

/* float save_log(double t) { if(t>0) return log(t); else return(-999.9); } */


#if defined(TRIGGER)
#include "/afs/cs/project/link/include/k005.api.h"

static float lm_triggerUgScore(LmStruct* p, int w1) {
  return -save_log(get_trig_prob(w1))/log(10.0)*(p->lw)+(p->log_wip);
}

static float lm_triggerBgScore(LmStruct* p, int w1, int w2) {
  return -save_log(get_trig_prob(w2))/log(10.0)*(p->lw)+(p->log_wip);
}

static float lm_triggerTgScore(LmStruct* p, int w1, int w2, int w3) {
  return -save_log(get_trig_prob(w3))/log(10.0)*(p->lw)+(p->log_wip);
}
#endif


static float           lm_UgScore1 ( LmStruct* mP, int w1 ) {

  if(!mP->vocabP->vwordList.itemA[w1].active)
    return ILLEGAL_PROB;
  else
    return ((mP)->lm_UgScoreProc)(mP,w1);
}
  
static float           lm_BUgScore1 ( LmStruct* mP, int w1 ) {

  if(!mP->vocabP->vwordList.itemA[w1].active)
    return ILLEGAL_PROB;
  else
    return ((mP)->lm_BUgScoreProc)(mP,w1);
}
  
float           lm_BgScore1 ( LmStruct* mP, int w1, int w2 ) {

  if(!mP->vocabP->vwordList.itemA[w2].active)
    return ILLEGAL_PROB;
  if(w1==mP->vocabP->startWordX)
    w1=mP->pre_context1X;
  if(w2==mP->vocabP->finishWordX)
    w2=mP->post_context1X;
  return ((mP)->lm_BgScoreProc)(mP,w1,w2);

}
  


static float           lm_TgScore1 ( LmStruct* mP, int w1, int w2, int w3 ) {



  if(!mP->vocabP->vwordList.itemA[w3].active)
    return ILLEGAL_PROB;
  if(w2==mP->vocabP->startWordX) {
    w1=mP->pre_context2X;
    w2=mP->pre_context1X;
  }
  else if(w1==mP->vocabP->startWordX) {
    w1=mP->pre_context1X;
  }

  if(w3==mP->vocabP->finishWordX) {
    if(mP->post_context2X==NO_WORD)
      w3=mP->post_context1X;
    else
      return ((mP)->lm_TgScoreProc)(mP,w2,mP->post_context1X,mP->post_context2X)
	+ ((mP)->lm_TgScoreProc)(mP,w1,w2,mP->post_context1X);
  }
  return ((mP)->lm_TgScoreProc)(mP,w1,w2,w3);
}



static float lm_UgSeparatedScore( LmStruct* p, int w1) {
  
  return -p->separate_unigrams[w1];
}



static float lprobAdd(float p1, float p2) {
  
  if(p1<p2) return lprobAdd(p2,p1);

  assert(p1>=p2);

  return p1 + log(1+exp(log(10.0)*(p2-p1)))/log(10.0);
}

#define add(LPROB) score = lprobAdd(LPROB,score)

static float lm_UgInterpolScore( LmStruct* p, int w1)
{
  float lw=p->lw;
  float log_wip=p->log_wip;
  float score=0;

  for(p=p->follower;p;p=p->follower)
    add(-lm_UgScore1( p,w1 ));
  return score*lw+log_wip;
}

static float lm_UgInterpol2Score( LmStruct* p, int w1)
{
  float lw=p->lw;
  float log_wip=p->log_wip;
  LmStruct* this=p;
  int i=0;
  float score=0;

  for(p=p->follower,i=0;p;p=p->follower)
    add(-lm_UgScore1( p,w1 ) + local_lm_UgScore(this,this->interpol_names.itemA[i++]));
  return score*lw+log_wip;
}
/*
static float lm_UgInterpolAddScore( LmStruct* p, int w1) {
  
  float ret=0;
  float lw=p->lw;
  float log_wip=p->log_wip;

  for(p=p->follower;p;p=p->follower)
    ret += lm_UgScore1( p,w1 );
  return ret*lw+log_wip;
}
*/
static float lm_BUgInterpolScore( LmStruct* p, int w1)
{
  float lw=p->lw;
  float log_wip=p->log_wip;
  float score=0;

  for(p=p->follower;p;p=p->follower)
    add(-lm_BUgScore1( p,w1 ));
  return score*lw+log_wip;
}
/*
static float lm_BUgInterpolAddScore( LmStruct* p, int w1) {
  
  float ret=0;
  float lw=p->lw;
  float log_wip=p->log_wip;

  for(p=p->follower;p;p=p->follower)
    ret += lm_BUgScore1( p,w1 );
  return ret*lw+log_wip;
}
*/
static float lm_UgAdaptScore( LmStruct* p, int w1) {
  
  return -local_lm_UgScore(p,p->classes[w1].class)
    + lm_UgScore1(p->adapt,w1);
}

static float lm_BUgInterpol2Score( LmStruct* p, int w1)
{
  float lw=p->lw;
  float log_wip=p->log_wip;
  LmStruct* this=p;
  int i=0;
  float score=0;

  for(p=p->follower,i=0;p;p=p->follower)
    add(-lm_BUgScore1( p,w1 ) + local_lm_UgScore(this,this->interpol_names.itemA[i++]));
  return score*lw+log_wip;
}

static float lm_UgClassesScore( LmStruct* p, int w1) {
  return - (local_lm_UgScore(p,p->classes[w1].class) + p->classes[w1].penalty);
}

static float lm_UgCacheScore( LmStruct* p, int w1) {
  return - p->lm_cache_score( p, w1 );
}

static float lm_UgStandardScore( LmStruct* p, int w1) {
  return - local_lm_UgScore(p,w1);
}


static float lm_BgInterpolScore( LmStruct* p, int w1, int w2)
{
  float lw=p->lw;
  float log_wip=p->log_wip;
  float score=0;

  for(p=p->follower;p;p=p->follower)
    add(-lm_BgScore1( p,w1,w2 ));
  return score*lw+log_wip;
}

static float lm_BgAdaptScore( LmStruct* p, int w1, int w2) {
  
  return -local_lm_BgScore(p,p->classes[w1].class,p->classes[w2].class)
    + lm_BgScore1(p->adapt,w1,w2);
}


static float lm_BgInterpol2Score( LmStruct* p, int w1, int w2)
{
  float lw=p->lw;
  float log_wip=p->log_wip;
  LmStruct* this=p;
  int i=0;
  float score=0;

  for(p=p->follower;p;p=p->follower) 
    add(-lm_BgScore1( p,w1,w2 ) + local_lm_BgScore(this,w1,this->interpol_names.itemA[i++]));
  return score*lw+log_wip;
}
/*
static float lm_BgInterpolAddScore( LmStruct* p, int w1, int w2) {
  
  float ret=0;
  float lw=p->lw;
  float log_wip=p->log_wip;

  for(p=p->follower;p;p=p->follower)
    ret += lm_BgScore1( p,w1,w2 );
  return ret*lw+log_wip;
}
*/
static float lm_BgClassesScore( LmStruct* p, int w1, int w2) {

  return - (local_lm_BgScore(p,p->classes[w1].class,p->classes[w2].class) + p->classes[w2].penalty);
}

static float lm_BgCacheScore( LmStruct* p, int w1, int w2) {
  return - p->lm_cache_score( p, w2 );
}

static float lm_BgStandardScore( LmStruct* p, int w1, int w2) {
  return - local_lm_BgScore(p,w1,w2);
}


static float lm_TgInterpolScore( LmStruct* p, int w1, int w2, int w3)
{
  float lw=p->lw;
  float log_wip=p->log_wip;
  float score=0;

  for(p=p->follower;p;p=p->follower)
    add(-lm_TgScore1( p,w1,w2,w3 ));
  return score*lw+log_wip;
}

static float lm_TgAdaptScore( LmStruct* p, int w1, int w2, int w3) {
  
  return -local_lm_TgScore(p,p->classes[w1].class,p->classes[w2].class,p->classes[w3].class)
    + lm_TgScore1(p->adapt,w1,w2,w3);
}
/*
static float lm_TgInterpolAddScore( LmStruct* p, int w1, int w2, int w3) {
  
  float ret=0;
  float lw=p->lw;
  float log_wip=p->log_wip;

  for(p=p->follower;p;p=p->follower)
    ret += lm_TgScore1( p,w1,w2,w3 );
  return ret*lw+log_wip;
}
*/
static float lm_TgInterpol2Score( LmStruct* p, int w1, int w2, int w3)
{
  float lw=p->lw;
  float log_wip=p->log_wip;
  LmStruct* this=p;
  int i=0;
  float score=0;

  for(p=p->follower;p;p=p->follower) 
    add(-lm_TgScore1( p,w1,w2,w3 ) + local_lm_TgScore(this,w1,w2,this->interpol_names.itemA[i++]));
  return score*lw+log_wip;
}

#undef add

static float lm_TgClassesScore( LmStruct* p, int w1, int w2, int w3) {
  
  if(p->vocabP->startWordX==w2)  /* Kneser-Ney patch */
    w1=w2;

  return - (local_lm_TgScore(p,p->classes[w1].class,p->classes[w2].class,p->classes[w3].class)
	    + p->classes[w3].penalty);
}

static float lm_TgCacheScore( LmStruct* p, int w1, int w2, int w3) {
  return - p->lm_cache_score( p, w3 );
}

static float lm_TgStandardScore( LmStruct* p, int w1, int w2, int w3) {
  if(p->vocabP->startWordX==w2)  /* Kneser-Ney patch */
    w1=w2;

  return - local_lm_TgScore(p,w1,w2,w3);
}





static struct cache_s * cache_create(int vocab_size, int effective_vocab_size, int window, IArray * nocache, int bos, int eos
#if defined(TRIGGER)
, int trigger
#endif
  ) {
  struct cache_s * this=0;
  int i;


  
  if(0==(this=malloc(sizeof(struct cache_s))))
    return 0;
#if defined(TRIGGER)
if(trigger) {
  if(0==(this->monogram=calloc(window,sizeof(int)))) {
    free(this); return 0;
  }
  this->selective=0;
}
else {
#endif
  if(0==(this->monogram=calloc(vocab_size,sizeof(int)))) {
    free(this); return 0;
  }
  if(0==(this->selective=calloc(vocab_size,sizeof(int)))) {
    free(this->monogram); free(this); return 0;
  }
#if defined(TRIGGER)
}
#endif
  if(nocache->itemN==0) {
    this->nocache=0;
  }
  else {

    if(0==(this->nocache=calloc(vocab_size,sizeof(int)))) {
      free(this->monogram); free(this->selective); free(this); return 0;      
    }
    for(i=0;i<vocab_size;i++)
      this->nocache[i]=0;
    for(i=0;i<nocache->itemN;i++)
      this->nocache[nocache->itemA[i]]++;
  }

  this->vocab_size=vocab_size; this->window=window;
  this->mono_discount=0.1;
  this->bi_discount=0.1;
  this->effective_vocab_size=effective_vocab_size;

  this->bos=bos; this->eos=eos;

  return this;
}

static void cache_delete(struct cache_s * this) {
  if(this==0)
    return;
  free(this->monogram); free(this->selective); free(this->nocache); free(this);
}


static float cache_mono_score(struct cache_s * cache, int w) {



  if(cache->monogram[w]>0) {	/* non-selective cache hit */
    return (cache->monogram[w]-cache->mono_discount)/
             (cache->mono_sum-cache->mono_discount*cache->mono_incache);
  }

  /* word not in cache */  

  return 0;

  /* Old smooth estimates */

  if(cache->monogram[w]>0) {	/* non-selective cache hit */
    return (cache->monogram[w]-cache->mono_discount)/
             cache->mono_sum;
  }

  /* word not in cache */  

  if(cache->mono_sum==0)
    return 1.0/cache->vocab_size;

  return cache->mono_incache*
    cache->mono_discount/
    cache->mono_sum/
    (cache->effective_vocab_size - cache->mono_incache);
}

static float cache_score(struct cache_s * cache, int w) {
  float prob;

  if(cache->selective[w]>0)   /* selective monogram-cache hit */
    return (cache->selective[w]-cache->bi_discount)/cache->selective_sum;

  /* Back off to non-selective */
  if(cache->selective_sum>0) {	/* Context was seen */
    prob  = cache->selective_incache * cache->bi_discount;
    prob /= cache->selective_sum;
    prob /= cache->effective_vocab_size - cache->selective_incache;
  }
  else	/* Context wasn't seen */
    prob=1;
  return prob * cache_mono_score(cache, w);
}

static float   lm_mono_cache_score(LmStruct* mP, int w) {
  float prob;

  if(mP->cachelm==0)
    FATAL("No cache model defined");

  prob=cache_mono_score(mP->cachelm,w);

  if(prob==0)
    return -9999;
  else
    return save_log(prob)/log(10.0)*(mP->lw)-(mP->log_wip);
}

static float   lm_selective_cache_score(LmStruct* mP, int w) {
  float prob;

  if(mP->cachelm==0)
    FATAL("No cache model defined");

  prob=cache_score(mP->cachelm,w);

  if(prob==0)
    return -9999;
  else
    return save_log(prob)/log(10.0)*(mP->lw)-(mP->log_wip);
}

static int cache_add(struct cache_s * cache, int last_word, int current_word) {

  if(cache->context>=cache->window)
    return 0;

  if(cache->nocache && cache->nocache[current_word])
    return 1;

  if(last_word==cache->eos)
    last_word=cache->bos;
  if(current_word==cache->bos)
    current_word=cache->eos;

  if(current_word == NO_WORD) {	/* Initialization: Set the selection */
    cache->selection=last_word;
    current_word=cache->eos;
    return 1;
  }
  cache->context++;

#if defined(TRIGGER)
  if(cache->trigger) {
    cache->monogram[cache->context-1]=current_word;
    return 1;
  }
#endif

  if( 0 == cache->monogram[current_word]++ )
    cache->mono_incache++;
  cache->mono_sum++;
  if(last_word != cache->selection)
    return 1;

  if( 0 == cache->selective[current_word]++ )
    cache->selective_incache++;
  cache->selective_sum++;
  return 1;
}
    

static void reset_context_cache(struct cache_s * cache, Vocab * vocabP, IArray * warray, struct LatLink_S* bestP) {

  VWord*    vocabA = vocabP->vwordList.itemA;
  int last_word,current_word;
  int i;

  if(cache==0)
    return;

  last_word=NO_WORD; current_word=cache->eos;


#if defined(TRIGGER)
if(cache->trigger)
  for(i=0;i<cache->context;i++)
    cache->monogram[i]=0;
else
#endif

  /* Reset all entries to zero */

  for(i=0;i<cache->vocab_size;i++)
    cache->monogram[i]=cache->selective[i]=0;
  cache->mono_sum = cache->selective_sum = 0;
  cache->mono_incache = cache->selective_incache = 0;  
  cache->context = 0;
  cache->selection=NO_WORD;

  /* Adding all entries to the cache */

  if(bestP) {
    for(;bestP;bestP=bestP->bestP) {
      current_word=last_word;	/* it's goin' backwards, buddy */
      if(bestP->toP)
	last_word=vocabA[bestP->toP->wordX].realX;
      else {
	WARN("The bestP->toP==0 problem occured");
	continue;
      }
      if(!cache_add(cache,last_word,current_word))
	goto end;
    }
    /* Add the first word of the lattice */
    if(last_word!=cache->bos && last_word!=cache->eos) {
      current_word=last_word;	/* it's goin' backwards, buddy */
      last_word=cache->bos;
      if(!cache_add(cache,last_word,current_word))
	goto end;
      last_word=cache->eos;
    }
  }
  if(warray && warray->itemN>0) {


    for(i=warray->itemN-1;i>=0;i--) {
      current_word=last_word;	/* it's goin' backwards, buddy */
      last_word=vocabA[warray->itemA[i]].realX;      
      if(!cache_add(cache,last_word,current_word))
	goto end;
    }	
  }
  
  end:
  if(last_word==NO_WORD || (last_word!=cache->eos && last_word!=cache->bos)) {
    current_word=last_word;
    last_word=cache->bos;
    cache_add(cache,last_word,current_word);
  }
    
  cache_add(cache,NO_WORD,last_word);


#if defined(TRIGGER)
  if(cache->trigger) {
    if(cache->trigger>1)
      destroy_context();
    else
      cache->trigger++;
    for(i=0;i<cache->context && i<cache->context-i-1;i++) { /* Change ordering of array */
      int t;
      t=cache->monogram[i];
      cache->monogram[i]=cache->monogram[cache->context-i-1];
      cache->monogram[cache->context-i-1]=t;
    }
    for(i=0;i<cache->context;i++) /* Change ordering of array */
      if(cache->monogram[i]==cache->bos)
	cache->monogram[i]=cache->eos;
    set_context(cache->context,cache->monogram);
  }
#endif
  return;
}


void lm_ResetContext(LmStruct* modelP, void * bestP) {

  struct LatLink_S * bestL = (struct LatLink_S*) bestP;

  for(;modelP;modelP=modelP->follower) {
    if(modelP->cachelm)
      reset_context_cache(modelP->cachelm, modelP->vocabP, &(modelP->utterance_context),bestL);
  }
}




/*===========================================================================*/
/*===========================================================================*/
/*===========================================================================*/

static void lm_CacheReset1 (LmStruct * modelP);

static int cacheCreate(LmStruct* modelP,int cacheLineN)
{

  float   *prob;
  int   i;
  modelP->cacheLineN = cacheLineN;
  
   /* do the equivalent of lm_cache_init (); */
  modelP->bg_info    = 
       (BgCacheInfo *) calloc (modelP->vocab_size,sizeof (BgCacheInfo));
  modelP->cache_info = 
       (CacheInfo *) calloc (modelP->cacheLineN, sizeof (CacheInfo));
  prob          = 
       (float   *)  calloc (modelP->vocab_size * cacheLineN, sizeof(float  ));

  /* check for memory allocation errors */
  assert(modelP->bg_info && modelP->cache_info && prob);

  for (i = 0; i < cacheLineN; i++)  
    modelP->cache_info[i].prob = prob + i*modelP->vocab_size;

  lm_CacheReset1 (modelP);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmCreate
   ------------------------------------------------------------------------ */

static LmStruct* lmCreate( char* name, Vocab* vocabP, int bg_seg_sz, int log_bg_seg_sz, float lw, 
                           float zero, float lpen, int cacheLineN)
{
  LmStruct * modelP  = (LmStruct *) calloc (1,sizeof (LmStruct));
  modelP->name       = strdup(name);
  modelP->useN       = 0;

  modelP->bg_seg_sz  =bg_seg_sz;
  modelP->log_bg_seg_sz  =log_bg_seg_sz;
  modelP->lw         = lw;
  modelP->lz         = 1; modelP->lp = 0;
  modelP->zerogram   = zero;
  modelP->log_wip    = lpen;
  modelP->vocabP     = vocabP; link( vocabP, "Vocab");

  modelP->separate_unigrams = 0;
  modelP->classes    = 0;
  modelP->follower   = 0;
  modelP->cachelm    = 0;

  modelP->vocab_size = vocabNumber(vocabP);

  modelP->utterance_context.itemA = 0;
  modelP->utterance_context.itemN = 0;
  
  modelP->interpol_names.itemA = 0;
  modelP->interpol_names.itemN = 0;
  
  modelP->lm_UgScoreProc = lm_UgStandardScore;
  modelP->lm_BUgScoreProc= lm_UgStandardScore;
  modelP->lm_BgScoreProc = lm_BgStandardScore;
  modelP->lm_TgScoreProc = lm_TgStandardScore;

  /* CHANGE by bernhard */
  /* initialize alternative begin/end-of-sentence trigram context */
  
  modelP->pre_context1X = vocabP->startWordX;
  modelP->pre_context2X = vocabP->startWordX;
  modelP->post_context1X = vocabP->finishWordX;
  modelP->post_context2X = NO_WORD;

  cacheCreate(modelP,cacheLineN);  

  return (modelP);
}


/* ------------------------------------------------------------------------
    lmDestroy
   ------------------------------------------------------------------------ */

static int cacheDestroy(LmStruct * modelP)
{
  int i;
  
  for (i = 0; i< modelP->cacheLineN; i++) {
     TgCacheInfo* nextP;
     while (modelP->cache_info[i].tgInfo) {
       nextP = modelP->cache_info[i].tgInfo->next;
       free (modelP->cache_info[i].tgInfo);
       modelP->cache_info[i].tgInfo= nextP;
    }
  }
  
  free (modelP->bg_info);
  free (modelP->cache_info[0].prob);
  free (modelP->cache_info);
  return TCL_OK;
}

/*===========================================================================*/
static int lmDestroy( LmStruct * modelP)
{
  int   i;
  if (modelP) {
    if (modelP->useN !=0)   
      ERROR("destroying Lm %s that has %d links\n",
                                          modelP->name,modelP->useN);

    cacheDestroy(modelP);
    if (modelP->unigrams)    free (modelP->unigrams);
    if (modelP->bigrams)     free (modelP->bigrams);
    if (modelP->prob2)       free (modelP->prob2);
    if (modelP->vocabwid_map) free (modelP->vocabwid_map);
    if (modelP->tcount > 0) {
      if (modelP->trigrams)  free (modelP->trigrams);
      if (modelP->tseg_base) free (modelP->tseg_base);
      if (modelP->bo_wt2)    free (modelP->bo_wt2);
      if (modelP->prob3)     free (modelP->prob3);
    }
    if (modelP->hashtableP) hash_free_table(modelP->hashtableP);
    for (i = 0; i< modelP->ucount; i++) {
      if (modelP->wordSA[i]) free(modelP->wordSA[i]);
    }
    if (modelP->wordSA) free(modelP->wordSA);
    if (modelP->follower) modelP->follower->useN--;
    free(modelP);
  }
  modelP->vocabP->useN --;
  if (modelP->name) free (modelP->name);
  

  if(modelP->separate_unigrams) free (modelP->separate_unigrams);
  if(modelP->classes) free (modelP->classes);
  if(modelP->cachelm) cache_delete(modelP->cachelm);
  if(modelP->interpol_names.itemA) free(modelP->interpol_names.itemA);

  return (0);
}


static int lmDestroyItf(ClientData cd)
{
  if (cd) {
    lmDestroy ((LmStruct *) cd);
    return TCL_OK;
  }
  else return TCL_ERROR;
}
/*===========================================================================*/
static int lmSetCurrent(ClientData cd)
{
 LmStruct *  modelP = (LmStruct *) cd;

 currentLmP    = modelP;

 return 1;
}

/*===========================================================================*/
static ClientData lmCreateItf(ClientData cd, int argc, char* argv[])
{
  /* from the interface, you can create 3 kinds of language models:
      a) by giving a filename for reading a nist file
      b) by giving a hypolist including ( and ) marker
      c) by asking for a nolm hypo given the name "empty"
  */
  
  char * returnstring;

  char * emptyfilename = "empty.nist";
  char * customfilename = "custom.nist";

  char * cache_exclude = 0;
  char * interpol_names = 0;
  float lweight =1.0, lzero = 9999.0, lpen =0.0;
  int cacheLineN = 100, cacheWindow=-1, monocacheWindow=-1;
  int  bg_seg_sz=lmDefault.bg_seg_sz, log_bg_seg_sz=lmDefault.log_bg_seg_sz;
  Vocab* vocabP;
  int vocab_size;
  HypoList * hypoListP;
  LmStruct * modelP;
  LmStruct * p;
  LmStruct * follower = 0;
  LmStruct * adapt = 0;
  IArray     nocache = {0,0};
  int ac = argc-1;
  int i;

  if (itfParseArgv( "show", &ac,  argv+1,  1,
    "vocab",           ARGV_OBJECT, NULL, &vocabP, "Vocab", 
                       "vocabulary this lm is defined over",
    "method",          ARGV_STRING, NULL, &returnstring, NULL,
                       "<nistfilename> | <hypolist> | empty",
    "-bg_seg_sz",      ARGV_INT,    NULL, &bg_seg_sz, NULL,
                       "bigram segment size (useful to modify for very large LMs)",
    "-log_bg_seg_sz",  ARGV_INT,    NULL, &log_bg_seg_sz, NULL,
                       "log bigram size (useful to modify for very large LMs)",
    "-weight",         ARGV_FLOAT,  NULL, &lweight, NULL,
                       "language model weight (multiplicative)",
    "-penalty",        ARGV_FLOAT,  NULL, &lpen, NULL,
                       "language model offset (additive)",
    "-zerogram",       ARGV_FLOAT,  NULL, &lzero, NULL,
                       "language model penalty for unexpected words",
    "-customfilename", ARGV_STRING, NULL, &customfilename, NULL,
                       "filename if building form hypos",
    "-emptyfilename",  ARGV_STRING, NULL, &emptyfilename, NULL,
                       "filename  [empty.nist]",
    "-cachelines",     ARGV_INT,    NULL, &cacheLineN, NULL,
                       "number of cache lines for this Lm",
    "-cache",     	ARGV_INT,    NULL, &cacheWindow, NULL,
                       "use a selective cache Lm with the windowsize given",
    "-monocache",     	ARGV_INT,    NULL, &monocacheWindow, NULL,
                       "use a monogram cache Lm with the windowsize given",
#if defined(TRIGGER)
    "-trigger",     	ARGV_STRING,    NULL, &trigger, NULL,
                       "use the triggermodel",
#endif
    "-nocache",        ARGV_STRING, NULL, &(cache_exclude), NULL,
		       "exclude a list of words from cacheing",
    "-linear",         ARGV_STRING, NULL, &(interpol_names), NULL,
		       "names of the linear interpolated model",
     "-follower",    	ARGV_OBJECT, NULL, &follower, "Lm", 
                       "additive language model",
     "-adapt",    	ARGV_OBJECT, NULL, &adapt, "Lm", 
                       "adapt this class based model by the small domain model",
     NULL)  != TCL_OK) return NULL;

  modelP = lmCreate(argv[0],vocabP, bg_seg_sz, log_bg_seg_sz, lweight,-lzero,lpen,cacheLineN);
  modelP->follower = follower;
  modelP->adapt = adapt;
  if(follower)
    modelP->follower->useN++;
  if(adapt)
    modelP->adapt->useN++;
  modelP->utterance_context.itemA = 0;
  modelP->utterance_context.itemN = 0;

  if((cacheWindow>1) || (monocacheWindow>1)) {
    if(monocacheWindow>1) {
      cacheWindow=monocacheWindow;
      modelP->lm_cache_score = lm_mono_cache_score;
    }
    else
      modelP->lm_cache_score = lm_selective_cache_score;
    modelP->cacheWindow=cacheWindow;
    if ( cache_exclude && (iarrayGetListItf(&nocache, cache_exclude, (List*)&(vocabP->vwordList)) != TCL_OK))
      return NULL;

    for(i=vocab_size=0;i<vocabP->vwordList.itemN;i++)
      if(vocabP->vwordList.itemA[i].realX == i)
	vocab_size++;
    if(cache_exclude)
      vocab_size-=nocache.itemN;

#if defined(TRIGGER)
    modelP->cachelm = cache_create(modelP->vocab_size, vocab_size, cacheWindow, &nocache,
				   vocabIndex(vocabP,"("),vocabIndex(vocabP,")"),trigger!=0); 
#else
    modelP->cachelm = cache_create(modelP->vocab_size, vocab_size, cacheWindow, &nocache,
				   vocabIndex(vocabP,"("),vocabIndex(vocabP,")")); 
#endif

    modelP->lm_UgScoreProc = lm_UgCacheScore;
    modelP->lm_BUgScoreProc= lm_UgCacheScore;
    modelP->lm_BgScoreProc = lm_BgCacheScore;
    modelP->lm_TgScoreProc = lm_TgCacheScore;
#if defined(TRIGGER)
    modelP->cachelm->trigger=0;

    if(trigger) {
      char ** words;
      int * ids;

      modelP->lm_UgScoreProc = lm_triggerUgScore;
      modelP->lm_BUgScoreProc= lm_triggerUgScore;
      modelP->lm_BgScoreProc = lm_triggerBgScore;
      modelP->lm_TgScoreProc = lm_triggerTgScore;
      modelP->cachelm->trigger=1;
      if(!init_trig_model(trigger,"WALL-WORD", "UNK",2,200,200)) {
	WARN("Failure in init_trig_model %s\n",error_message);
      }

      
      if(0==(ids=calloc(vocabP->vwordList.itemN,sizeof(int)))) {
	ERROR("Not allocated trigger transform");
      }
      if(0==(words=calloc(vocabP->vwordList.itemN,sizeof(char *))))
	ERROR("Not allocated trigger transform words");

      for(i=0;i<vocabP->vwordList.itemN;i++) {
	ids[i]=i;
	words[i]=vocabP->vwordList.itemA[i].spellS;
      }
      if(!set_vocabulary(modelP->cachelm->eos,0,vocabP->vwordList.itemN,vocabP->vwordList.itemN,words,ids))
	WARN("Failure in set_vocabulary %s\n",error_message);
    }
#endif

    reset_context_cache(modelP->cachelm, vocabP, &(modelP->utterance_context), 0);

    free(nocache.itemA);

  }
  else  {
    free(nocache.itemA);
    if(modelP->adapt) {
      modelP->lm_UgScoreProc = lm_UgAdaptScore;
      modelP->lm_BUgScoreProc= lm_UgAdaptScore;
      modelP->lm_BgScoreProc = lm_BgAdaptScore;
      modelP->lm_TgScoreProc = lm_TgAdaptScore;
      if (lmRead(modelP, customfilename) == TCL_ERROR) {
	lmDestroy(modelP);
	return NULL;
      }
    }
    else if ((strcmp(returnstring,"empty") == 0) || (interpol_names!=0) ){
      if(modelP->follower) {	
	if(interpol_names) {
	  modelP->lm_UgScoreProc = lm_UgInterpol2Score;
	  modelP->lm_BUgScoreProc= lm_BUgInterpol2Score;
	  modelP->lm_BgScoreProc = lm_BgInterpol2Score;
	  modelP->lm_TgScoreProc = lm_TgInterpol2Score;

	  MSGCLEAR(NULL); /* reset the error caused by itfGetObject */
	  if (lmRead(modelP, returnstring) == TCL_ERROR) {
	    lmDestroy(modelP);
	    return NULL;
	  }
	  if(iarrayGetListItf(&(modelP->interpol_names), interpol_names, (List*)&(vocabP->vwordList)) != TCL_OK)
	    return NULL;

	  for(i=0,p=modelP->follower;p;p=p->follower)
	    i++;

	  if(i!=modelP->interpol_names.itemN) {
	    WARN("#models (%d) has to be equal to the number of names (%d) in -linear \"%s\"\n",
		 i,modelP->interpol_names.itemN,interpol_names);
	    lmDestroy(modelP);
	    return NULL;
	  }

	}
	else {
/*
	  modelP->lm_UgScoreProc = lm_UgInterpolAddScore;
	  modelP->lm_BUgScoreProc= lm_BUgInterpolAddScore;
	  modelP->lm_BgScoreProc = lm_BgInterpolAddScore;
	  modelP->lm_TgScoreProc = lm_TgInterpolAddScore;
*/
	  modelP->lm_UgScoreProc = lm_UgInterpolScore;
	  modelP->lm_BUgScoreProc= lm_BUgInterpolScore;
	  modelP->lm_BgScoreProc = lm_BgInterpolScore;
	  modelP->lm_TgScoreProc = lm_TgInterpolScore;
	}
      }      
      else {
	if( lmWriteEmpty (emptyfilename) == TCL_ERROR) {
	  lmDestroy(modelP);
	  return NULL;
	}
	if (lmRead(modelP, emptyfilename) == TCL_ERROR) {
	  lmDestroy(modelP);
	  return NULL;
	}
      }
    }
    else if ((hypoListP = (HypoList*)itfGetObject (returnstring,"HypoList")) != NULL) {
      lmWriteFromHypoList (customfilename, hypoListP, vocabP);
      if (lmRead( modelP, customfilename) == TCL_ERROR) {
	lmDestroy(modelP);
	return NULL;
      }
    }
    else {
      MSGCLEAR(NULL); /* reset the error caused by itfGetObject */
      if (lmRead(modelP, returnstring) == TCL_ERROR) {
	lmDestroy(modelP);
	return NULL;
      }
    
      if(modelP->classes) {      
	modelP->lm_UgScoreProc = lm_UgClassesScore;
	modelP->lm_BUgScoreProc= lm_UgClassesScore;
	modelP->lm_BgScoreProc = lm_BgClassesScore;
	modelP->lm_TgScoreProc = lm_TgClassesScore;
      }
      if(modelP->separate_unigrams) {
	modelP->lm_UgScoreProc = lm_UgSeparatedScore;
      }      
    }
  }
  if (currentLmP == NULL) lmSetCurrent((ClientData) modelP);
  return( (ClientData) modelP );
}


LmStruct* lmGetCurrent() { return currentLmP; }

/*===========================================================================*/
static int lmSetCurrentItf(ClientData cd, int argc, char * argv[])
{
  lmSetCurrent(cd);
  return TCL_OK;
} 

/*===========================================================================*/
static int lmSetCacheLinesItf(ClientData cd, int argc, char * argv[])
{
  LmStruct *  modelP = (LmStruct *) cd;
  int cacheLineN;
  int ac = argc-1;
  
  if (itfParseArgv( "cachelines", &ac,  argv+1,  2,
              "cachelines" ,   ARGV_INT   , NULL, &cacheLineN, NULL, "new number of cachelines", NULL)
    != TCL_OK) return TCL_ERROR;
  
  cacheDestroy(modelP);
  cacheCreate(modelP,cacheLineN);
  
  itfAppendResult("%d",cacheLineN);
  return TCL_OK;
} 

/*===========================================================================*/
static int lmSetUtteranceContextItf(ClientData cd, int argc, char * argv[])
{
  LmStruct *  modelP = (LmStruct *) cd;
  LmStruct *  firstP = modelP;
  int i;
  int ac = argc-1;

  free(modelP->utterance_context.itemA);
  modelP->utterance_context.itemA =  0;
  modelP->utterance_context.itemN = 0;
  

  if (itfParseArgv( "utterance_context", &ac,  argv+1,  0,
              "utterance_context" ,   ARGV_LIARRAY   , NULL, &(modelP->utterance_context),
		    &(modelP->vocabP->vwordList), "set utterance_context", NULL)
    != TCL_OK) return TCL_ERROR;
  
  reset_context_cache(modelP->cachelm, modelP->vocabP, &(modelP->utterance_context), 0);

  itfAppendResult("\"");
  for(i=0;i<modelP->utterance_context.itemN-1;i++)
    itfAppendResult("%s ",vocabName(modelP->vocabP,modelP->utterance_context.itemA[i]));
  if(modelP->utterance_context.itemN>0)
    itfAppendResult("%s\"",vocabName(modelP->vocabP,modelP->utterance_context.itemA[i]));
  else
    itfAppendResult("\"");
    

  for(modelP=modelP->follower;modelP;modelP=modelP->follower) {
    modelP->utterance_context.itemN=firstP->utterance_context.itemN;
    free(modelP->utterance_context.itemA);
    if(modelP->utterance_context.itemN==0)
      modelP->utterance_context.itemA=0;
    else
      modelP->utterance_context.itemA=calloc(modelP->utterance_context.itemN,sizeof(int));
    for(i=0;i<modelP->utterance_context.itemN;i++) {
      modelP->utterance_context.itemA[i]=firstP->utterance_context.itemA[i];
    }
    reset_context_cache(modelP->cachelm, modelP->vocabP, &(modelP->utterance_context), 0);
  }
  return TCL_OK;
}

/*===========================================================================*/
static int lmPutsItf (ClientData cd, int argc, char * argv[])
{
 LmStruct * mP = (LmStruct*) cd;
 int i;
 int w1,w2,w3;
 
 if (argc >1) { w1= vocabIndex(mP->vocabP,argv[1]);
   if (w1 != NO_WORD) { 
     itfAppendResult("{ %s %f }\n",argv[1], lm_UgScore1(mP,w1));
     if (argc >2) { 
       w2= vocabIndex(mP->vocabP,argv[2]);
       if (w2 != NO_WORD) { 
         itfAppendResult("{ %s %s %f }\n",argv[1], argv[2],lm_BgScore1(mP,w1,w2));
         if (argc >3) {
            w3= vocabIndex(mP->vocabP,argv[3]);
            if (w3 != NO_WORD) { 
              itfAppendResult("{ %s %s %s %f }\n",argv[1],argv[2],argv[3],lm_TgScore1(mP,w1,w2,w3));
            } else return TCL_ERROR;
         } 
       } else return TCL_ERROR; 
     }
   } else return TCL_ERROR;
 }
else {
     itfAppendResult("{\n { Unigrams: %4d }\n { Bigrams:  %4d }\n { Trigrams: %4d }\n}\n",
                      mP->ucount,  mP->bcount, mP->tcount);
   itfAppendResult("{ ");
   for (i = 0; i < mP->ucount; i ++)
     itfAppendResult("%s ", mP->wordSA[i]);
   itfAppendResult("}\n");
}
return TCL_OK;
}
/*===========================================================================*/
static int lmDebugItf (ClientData cd, int argc, char * argv[]) 
{
 LmStruct * mP = (LmStruct*) cd;
 int i;
   itfAppendResult("{\n { Unigrams: %4d }\n { Bigrams:  %4d }\n { Trigrams: %4d }\n}\n",
                      mP->ucount,  mP->bcount, mP->tcount);
  
   itfAppendResult("{ ");
   for (i = 0; i < mP->ucount+1; i ++)
     itfAppendResult("%s ", mP->wordSA[i]);
   itfAppendResult("}\n");
   
   itfAppendResult("{\n");
   for (i = 0; i < mP->vocab_size; i ++)
     itfAppendResult(" { %d %d %s }\n", 
       i, VOCAB2LM_WID(mP,i),mP->wordSA[VOCAB2LM_WID(mP,i)]);
   itfAppendResult("}\n");

   itfAppendResult("{\n");
     for (i = 0; i < mP->ucount; i ++) 
       itfAppendResult(" { %d \t%d \t%6.2f \t%6.2f \t%d}\n",
                         i,
                         mP->unigrams[i].wid,
                         mP->unigrams[i].prob1,
                         mP->unigrams[i].bo_wt1,
                         mP->unigrams[i].bigrams );
   itfAppendResult("}\n");

   itfAppendResult("{\n");
     for (i = 0; i < mP->bcount; i ++) 
       itfAppendResult(" { %d \t%d \t%6.2f \t%6.2f \t%d}\n",
                         i,
                         mP->bigrams[i].wid,
                         mP->prob2[mP->bigrams[i].prob2],
                         mP->prob2[mP->bigrams[i].bo_wt2],
                         mP->bigrams[i].trigrams
                      );
   itfAppendResult("}\n");
   
     for (i = 0; i < mP->tcount; i ++) 
       itfAppendResult(" { %d \t%d \t%6.2f}\n",i,mP->trigrams[i].wid,
                                           mP->prob3[mP->trigrams[i].prob3]
                      );
   itfAppendResult("}\n");

   
 return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmScoreOldItf
   ------------------------------------------------------------------------ */

static int lmScoreOldItf(ClientData cd, int argc, char * argv[])
{
  LmStruct* mP = (LmStruct *) cd;
  IArray    warray = {NULL, 0};
  int       ac     =  argc-1;
  int       i      =  0;
  float     zn     =  mP->lw,      z = zn;
  float     pn     =  mP->log_wip, p = pn;

  if (itfParseArgv( argv[0], &ac,  argv+1,  0,
     "<word sequence>", ARGV_LIARRAY, NULL, &warray, 
     &(mP->vocabP->vwordList), "sequence of words",
     "-lz" , ARGV_FLOAT, NULL, &zn, NULL, "language model weight", 
     "-lp" , ARGV_FLOAT, NULL, &pn, NULL, "insertion penalty", 
       NULL) != TCL_OK) return TCL_ERROR;

  z = zn / z;


  WARN("This function will be discontinued soon\n");

  for ( i = 0; i < warray.itemN; i++)
    warray.itemA[i] = mP->vocabP->vwordList.itemA[warray.itemA[i]].realX;

  if ( warray.itemN == 1) {
    itfAppendResult( "%f", (lm_UgScore1(mP, warray.itemA[0])-p) * z + pn);
  }
  else if ( warray.itemN == 2) {
    itfAppendResult( "%f", (lm_BgScore1(mP, warray.itemA[0], warray.itemA[1])-p)*z+pn);
  }
  else if ( warray.itemN > 2) {
    itfAppendResult( "%f", (lm_TgScore1(mP, warray.itemA[warray.itemN-3],
                                       warray.itemA[warray.itemN-2], 
                                       warray.itemA[warray.itemN-1])-p)*z+pn);
  }
  if ( warray.itemA) free( warray.itemA);
  return TCL_OK;
} 

/*===========================================================================*/
/*===========================================================================*/
/*===========================================================================*/
/*===========================================================================*/
 
typedef struct sorted_entry_s {
  float val;                  /* value being kept in this node */
  unsigned short lower;       /* index of another entry.  All descendants down
                              this path have their val < this node's val.
                              0 => no son exists (0 is root index) */
  unsigned short higher;      /* index of another entry.  All descendants down
                              this path have their val > this node's val
                              0 => no son exists (0 is root index) */
} sorted_entry_t;

/*
 * The sorted list.  list is a (64K long) array.  The first entry is the
 * root of the tree and is created during initialization.
 */

typedef struct {
    sorted_entry_t *list;
    int  free;         /* first free element in list */
} sorted_list_t;

/* Arrays of unique bigram probs and bo-wts, and trigram probs */
static sorted_list_t sorted_prob2;
static sorted_list_t sorted_bo_wt2;
static sorted_list_t sorted_prob3;

/*
 * Initialize sorted list with the 0-th entry = SMALLPROB, which may be needed
 * to replace spurious values in the Darpa LM file.
 */
 
/*===========================================================================*/
static void init_sorted_list ( sorted_list_t *l)
{
  l->list =
     (sorted_entry_t *) calloc (MAX_SORTED_ENTRIES,sizeof (sorted_entry_t));
  l->list[0].val = SMALLPROB;
  l->list[0].lower = 0;
  l->list[0].higher = 0;
  l->free = 1;
}
/*===========================================================================*/
static void free_sorted_list (sorted_list_t *l)
{
   free (l->list);
}
/*===========================================================================*/
static float *vals_in_sorted_list (sorted_list_t *l)
{
    float *vals;
    int   i;
    
    vals = (float *) calloc (l->free, sizeof (float));
    for (i = 0; i < l->free; i++)
        vals[i] = l->list[i].val;
    return (vals);
}
/*===========================================================================*/
static int   sorted_id ( sorted_list_t *l,  float *val)
{
    int   i = 0;
    
    for (;;) {
        if (*val == l->list[i].val)
            return (i);
        if (*val < l->list[i].val) {
            if (l->list[i].lower == 0) {
                if (l->free >= MAX_SORTED_ENTRIES) 
                     FATAL("sorted list overflow\n");
                l->list[i].lower = l->free;
                (l->free)++;
                i = l->list[i].lower;
                l->list[i].val = *val;
                return (i);
            } else
                i = l->list[i].lower;
        } else {
            if (l->list[i].higher == 0) {
                if (l->free >= MAX_SORTED_ENTRIES)
                    FATAL("sorted list overflow\n") ;
                l->list[i].higher = l->free;
                (l->free)++;
                i = l->list[i].higher;
                l->list[i].val = *val;
                return (i);
            } else
                i = l->list[i].higher;
        }
    }
}
/*===========================================================================*/
/*===========================================================================*/
/*===========================================================================*/
/*
 * Read and return #unigrams, #bigrams, #trigrams as stated in input file.
 */
static VOID ReadNgramCounts (
    FILE *fp,
    int   *n_ug, int *n_bg, int *n_tg)          /* return the info here */
{
    char string[256];
    int   ngram, ngram_cnt;
    
    /* skip file until past the '\data\' marker */
    do
        fgets (string, sizeof (string), fp);
    while ( (strcmp (string, "\\data\\\n") != 0) && (! feof (fp)) );

    if (strcmp (string, "\\data\\\n") != 0)
      FATAL("No \\data\\ mark in LM file\n");

    *n_ug = *n_bg = *n_tg = 0;
    while (fgets (string, sizeof (string), fp) != NULL) {
        if (sscanf (string, "ngram %d=%d", &ngram, &ngram_cnt) != 2)
            break;
        switch (ngram) {
        case 1: *n_ug = ngram_cnt;
            break;
        case 2: *n_bg = ngram_cnt;
            break;
        case 3: *n_tg = ngram_cnt;
            break;
        default:
          FATAL("Unknown ngram (%d)\n",ngram);
            break;
        }
    }
    
    /* Position file to just after the unigrams header '\1-grams:\' */
    while ( (strcmp (string, "\\1-grams:\n") != 0) && (! feof (fp)) )
        fgets (string, sizeof (string), fp);
    
    /* Check counts;  NOTE: #trigrams *CAN* be 0 */
    if ((*n_ug <= 0) || (*n_bg <= 0) || (*n_tg < 0))
        FATAL("Bad or missing ngram count\n");
}

/*===========================================================================*/
/*
 * Read in the unigrams from given file into the LM structure model.  On
 * entry to this procedure, the file pointer is positioned just after the
 * header line '\1-grams:'.
 */
static void ReadUnigrams (
    FILE *fp,           /* input file */
    LmStruct *modelP   /* to be filled in */)
{
    char string[256];
    char name[128];
    int   wcnt = 0;
    int   vocabwid; /* as opposed to lm_wid */
    float p1, bo_wt;

    
     INFO("Reading unigrams\n");

    while ((fgets (string, sizeof(string), fp) != NULL) &&
           (strcmp (string, "\\2-grams:\n") != 0))
    {
        if (sscanf (string, "%f %s %f", &p1, name, &bo_wt) != 3) {
            if (string[0] != '\n') 
                          INFO("Format error; unigram ignored:%s", string);
            continue;
        }
        
        if (wcnt >= modelP->ucount)
            FATAL("Too many unigrams\n");

        /* Associate name with word id */
        vocabwid = wcnt;

        modelP->wordSA[wcnt] = (CHAR *) salloc(name);
        hash_insert(modelP->hashtableP,
                    (VOID *) modelP->wordSA[wcnt],
                    (VOID *) (wcnt+1)); 
         modelP->unigrams[wcnt].prob1 = p1;
         modelP->unigrams[wcnt].bo_wt1 = bo_wt;
         modelP->unigrams[wcnt].wid = wcnt; /* for now */
         wcnt++;
    }

    if (modelP->ucount != wcnt) {
      INFO("LmStruct.ucount(%d) != #unigrams read(%d)\n",modelP->ucount,wcnt);
      modelP->ucount = wcnt;
    }
}



static int VocabIndex(
    char * name,
    LmStruct *modelP)  /* to be filled in */
{
    int ret;

    ret = vocabIndex (modelP->vocabP, name);
    if(ret==NO_WORD) {
      if(strcmp(name,"<UNK>") == 0)
	return 0;
    }
    return ret;
}



/*===========================================================================*/
/*
 * In case of separate unigrams ( because the backoff distribution doesn't
 * reflect the real unigram distribution anymore ) a separate unigramtable is
 * constructed
 */
static void ReadSeparateUnigrams (
    FILE *fp,   
    LmStruct *modelP)
{
    char string[256*16];
    char name[256*16];
    int   wcnt = 0;
    int   lm_wid;
    float p1;
    int i;
    
    INFO("Reading separated unigrams\n");
    INFO("Extension of NIST language model format\n");

    if(modelP->separate_unigrams) {
      INFO("Already found an old table, overwriting values");
    }
    else {
      modelP->separate_unigrams = (float *) calloc (vocabNumber(modelP->vocabP), sizeof (float));
      for (i = 0; i < vocabNumber(modelP->vocabP); i++)
        modelP->separate_unigrams[i] = -99.0;
    }	

    while ((fgets (string, sizeof(string), fp) != NULL) &&
           (strncmp (string, "\\end\\",5) != 0))
    {
        if (sscanf (string, "%f %s", &p1, name) != 2) {
            if (string[0] != '\n') 
                          INFO("Format error; separated unigram ignored:%s", string);
            continue;
        }

	if((lm_wid =VocabIndex(name,modelP))==NO_WORD) {
/*	  WARN("Non-vocabulary %s in separated unigrams\n",name); */
	  continue;
	}
	modelP->separate_unigrams[lm_wid] = p1;
	wcnt++;
    }

    if (modelP->ucount != wcnt) {
      INFO("LmStruct.ucount(%d) != #separated_unigrams read(%d)\n",modelP->ucount,wcnt);
    }
}


/*===========================================================================*/
/*
 * In case of separate unigrams ( because the backoff distribution doesn't
 * reflect the real unigram distribution anymore ) a separate unigramtable is
 * constructed
 */
static void ReadClasses (
    FILE *fp,           /* input file */
    LmStruct *modelP)   /* to be filled in */
{
    char string[256];
    char name1[128], name2[128]; 
    int   wcnt = 0;
    int   wid1, wid2;
    float penalty;

    float* min_class_penalty = NULL;

    int i;
    
    INFO("Reading class based mappings and penalties\n");
    INFO("Extension of NIST language model format\n");

    if(modelP->classes) {
      FATAL("Already found an old table");
    }
    else {
      modelP->classes = (ClassInfo *) calloc (vocabNumber(modelP->vocabP), sizeof (ClassInfo));
      min_class_penalty = (float *) calloc (vocabNumber(modelP->vocabP), sizeof (float));
      for (i = 0; i < vocabNumber(modelP->vocabP); i++) {
        modelP->classes[i].penalty = 0.0;
	min_class_penalty[i] = -99999;
	modelP->classes[i].class = i;
      }
    }	

    while ((fgets (string, sizeof(string), fp) != NULL) &&
           (strncmp (string, "\\end\\",5) != 0))
    {
        if (sscanf (string, "%f %s %s", &penalty, name1, name2) != 3) {
            if (string[0] != '\n') 
                          INFO("Format error; classes ignored:%s", string);
            continue;
        }
        

	if((wid1 = VocabIndex (name1, modelP )) == NO_WORD) {
/*	  WARN("Non-vocabulary %s in classes\n",name1);	*/
	  continue;
	}
	if((wid2 = VocabIndex (name2, modelP )) == NO_WORD)
	  FATAL("Non-vocabulary class %s\n",name2);	

	modelP->classes[wid1].class = wid2;
	modelP->classes[wid1].penalty = penalty;
	if(min_class_penalty[wid2]<penalty)
	  min_class_penalty[wid2]=penalty;

	if((wid1 != wid2) || (penalty!=0))
	  wcnt++;

	if(!hash_lookup(modelP->hashtableP,(VOID *)vocabName(modelP->vocabP,wid1))) {
	  hash_insert(modelP->hashtableP,
		      (VOID *) vocabName(modelP->vocabP,wid1),
		      (VOID *) (wcnt+1)); 
	  wcnt++;
	}

    }

    for (i = 0; i < vocabNumber(modelP->vocabP); i++) {
      if(0 && min_class_penalty[modelP->classes[i].class] != -99999)
	modelP->classes[i].penalty -= min_class_penalty[modelP->classes[i].class];
    }

    INFO("#mappings read(%d)\n",wcnt);
    free(min_class_penalty);
}

/*===========================================================================*/
/*
 * If (! final) LmStruct.unigrams[word1].bigrams = bgcount.
 * else copy bigrams info into LmStruct.bigrams, starting at
 * [LmStruct.unigrams[word1].bigrams].
 * (See ReadBigrams for more on the two-pass scheme.)
 */
static VOID PutBigramsIn (
    LmStruct *modelP,
    char *word1,        /* unigram for which bigrams being entered */
    BigramItem3g *bigrams,
    int   bgcount,
    float missing_prob,
    int   final)
{
    int   i, k, lm_wid1;
    int   first_bg, last_bg;

    if (bgcount == 0)
        return;
    if ((lm_wid1 = wstr2lmwid (modelP, word1)) == NO_WORD) 
      FATAL("Unknown word %s\n",word1); 
    
    if (! final) {
      if (modelP->unigrams[lm_wid1].bigrams != 0)
        FATAL("Bigrams for %s not contiguous\n", word1);
      modelP->unigrams[lm_wid1].bigrams = bgcount;
    } else {
        first_bg = FIRST_BG(modelP,lm_wid1);
        last_bg  = LAST_BG (modelP,lm_wid1);
        if ((bgcount != (last_bg-first_bg+1)) ||
                                 (first_bg+bgcount > modelP->bcount))
          FATAL("Error in bigram count\n");
        for (k = first_bg, i = 0; i < bgcount; i++, k++) {
            modelP->bigrams[k].wid    = bigrams[i].wid;
            modelP->bigrams[k].prob2  = bigrams[i].prob2;
            modelP->bigrams[k].bo_wt2 = bigrams[i].bo_wt2;
        }
    }
}

/*===========================================================================*/
/*
 * Read bigrams from given file.  Called twice, first with final=FALSE, then
 * with final=TRUE.  In the first pass, obtain only the bigram counts for each
 * unigram.  (The bigrams attribute is used temporarily for these counts.
 * Before the second pass, the counts are used to allocate sufficient space
 * for bigrams for each unigram.)  In the second pass enter bigram info into
 * model.  On entry to this procedure the file pointer is positioned just after
 * the header line '\2-grams:'.
 */
static VOID ReadBigrams(
    FILE *fp,
    LmStruct *modelP,
    int   final)
{
    char string[256], word1[128], word2[128], current[128];
    int   i;
    char  firstletter = '\0';
    int   lm_wid;
    int   bgcount;
    float p2, bo_wt;
    BigramItem3g *bigrams;
    char *word2_valid;
    float missing_prob;

    INFO("Reading bigrams (pass %d):  ",final);
    
    bigrams = (BigramItem3g *) calloc (modelP->ucount, sizeof (BigramItem3g));
    word2_valid = (char *) calloc (modelP->ucount, sizeof (char));
    current[0] = '\0';
    bgcount = 0;
    missing_prob = 0.0;
    
    while ((fgets (string, sizeof(string), fp) != NULL) &&
           (strcmp (string, "\\end\\\n") != 0) &&
           (strcmp (string, "\\3-grams:\n") != 0))
    {
       if (modelP->tcount > 0) {
         if (sscanf (string, "%f %s %s %f", &p2, word1, word2, &bo_wt) != 4) {
           if (string[0] !='\n') WARN("Format error; bigram ignored:%s",string);
           continue;
         }
       } else {
           if (sscanf (string, "%f %s %s", &p2, word1, word2) != 3) {
             if (string[0] != '\n')
               WARN("Format error; bigram ignored:%s",string);
               continue;
           }
        }
        if (strcmp (word1, current) != 0) {
            if (firstletter!= word1[0]) {
                  fprintf(STDERR,"%c",word1[0]);
                  firstletter = word1[0];
            }      
            /* Starting bigrams for another unigram; wind up previous one */
            PutBigramsIn (modelP, current, bigrams, bgcount, missing_prob, final);
            for (i = 0; i < bgcount; i++) word2_valid[bigrams[i].wid] = 0;
            bgcount = 0;
            missing_prob = 0.0;
            strcpy (current, word1);
        }
        if ((lm_wid = wstr2lmwid (modelP, word2)) != NO_WORD) {
            if (word2_valid[lm_wid])
                FATAL("Multiple occurrences of bigram %s,%s\n", word1, word2);
            word2_valid[lm_wid] = 1;
            bigrams[bgcount].wid = lm_wid;
            
            if (final) {
                bigrams[bgcount].prob2 = sorted_id (&sorted_prob2, &p2);
                if (modelP->tcount > 0)
                  bigrams[bgcount].bo_wt2 = sorted_id (&sorted_bo_wt2, &bo_wt);
            }
            bgcount++;
        } else FATAL("Unknown word %s\n", word2);
    }

    /* No more bigrams; wind up the last set */
    PutBigramsIn (modelP, current, bigrams, bgcount, missing_prob, final);
    fprintf(STDERR,"\n");
    free (word2_valid);
    free (bigrams);
}

/*===========================================================================*/
/*
 * Very similar to PutBigramsIn
 */
static VOID PutTrigramsIn (
    LmStruct *modelP,
    char *word1,
    char *word2,
    TrigramItem3g *trigrams,
    int   tgcount,
    float missing_prob,
    int   final)
{
    int   i, k;
    int   lm_wid1, lm_wid2;
    int   bg_idx;
    int   first_bg, last_bg;
    int   first_tg, last_tg;
    
    if (tgcount == 0)
        return;

    if ((lm_wid1 = wstr2lmwid(modelP, word1)) == NO_WORD)
        FATAL("Unknown word %s\n", word1);
    if ((lm_wid2 = wstr2lmwid(modelP, word2)) == NO_WORD)
        FATAL("Unknown word %s\n", word2);
    
    /* find bigram corresponding to word1,word2 */
    first_bg = FIRST_BG(modelP,lm_wid1);
    last_bg  = LAST_BG (modelP,lm_wid1);
    for (bg_idx = first_bg; bg_idx <= last_bg; bg_idx++) {
        if (modelP->bigrams[bg_idx].wid == lm_wid2)
            break;
    }
    if (bg_idx > last_bg)
      FATAL("Missing bigram %s,%s\n", word1, word2);
    /* else bg_idx = index of parent bigram word1,word2 */

    if (! final) {
        if (modelP->bigrams[bg_idx].trigrams != 0)
            FATAL("Trigrams for %s,%s not contiguous\n",  word1, word2);
        modelP->bigrams[bg_idx].trigrams = tgcount;
    } else {
        first_tg = FIRST_TG(modelP,bg_idx);
        last_tg  = LAST_TG (modelP,bg_idx);
        if ((tgcount != (last_tg-first_tg+1)) ||
            (first_tg+tgcount > modelP->tcount))
            FATAL("Too many trigrams\n");
        for (k = first_tg, i = 0; i < tgcount; i++, k++) {
            modelP->trigrams[k].wid   = trigrams[i].wid;
            modelP->trigrams[k].prob3 = trigrams[i].prob3;
        }
    }
}

/*===========================================================================*/
/*
 * Very similar to ReadBigrams.
 */
static VOID ReadTrigrams(
    FILE *fp,
    LmStruct *modelP,
    int   final)
{
    char string[256], word1[128], word2[128], word3[128];
    char current1[128], current2[128];
    int   i;
    int   lm_wid;
    int   tgcount;
    float p3;
    TrigramItem3g *trigrams;
    char *word3_valid;
    float missing_prob;
    
     INFO("Reading trigrams (pass %d)\n",final);
    
    trigrams = (TrigramItem3g *) calloc (modelP->ucount, sizeof (TrigramItem3g));
    word3_valid =   (CHAR *) calloc (modelP->ucount, sizeof (char));
    current1[0] = '\0';
    current2[0] = '\0';
    tgcount = 0;
    missing_prob = 0.0;
    
    while ((fgets (string, sizeof(string), fp) != NULL) &&
           (strcmp (string, "\\end\\\n") != 0))
    {
        if (sscanf (string, "%f %s %s %s", &p3, word1, word2, word3) != 4) {
            if (string[0] != '\n')
                WARN("Format error; trigram ignored:%s", string);
            continue;
        }
        if ((strcmp (word1, current1) != 0) || (strcmp(word2, current2) != 0)) {
            /* Starting trigrams for another bigram; wind up previous one */
            PutTrigramsIn (modelP, current1, current2, trigrams, tgcount,
                           missing_prob, final);
            for (i = 0; i < tgcount; i++)
                word3_valid[trigrams[i].wid] = 0;
            tgcount = 0;
            missing_prob = 0.0;
            strcpy (current1, word1);
            strcpy (current2, word2);
        }
        if ((lm_wid = wstr2lmwid (modelP, word3)) != NO_WORD) {
          if (word3_valid[lm_wid])
            FATAL("Multiple occurrences of trigram %s,%s,%s\n",
                                                            word1, word2, word3);
          word3_valid[lm_wid] = 1;
          trigrams[tgcount].wid = lm_wid;
          if (final)
             trigrams[tgcount].prob3 = sorted_id (&sorted_prob3, &p3);
          tgcount++;
        } else {
            FATAL("Unknown word %s\n", word3);
        }
    }
    /* No more trigrams; wind up the last set */
    PutTrigramsIn (modelP, current1, current2, trigrams, tgcount,
                   missing_prob, final);
    
    free (word3_valid);
    free (trigrams);
}

/*===========================================================================*/
static FILE *lm_file_open (char * filename, int *usingPipeP)
{
  FILE *fp = NULL;
  char command[1024];
  char compressed[1024];
  struct stat statbuf;
  
  fp = fileOpen (filename, "r");
  if (fp != NULL) {
    *usingPipeP = FALSE;
    return (fp);
  }
  sprintf(compressed,"%s.Z",filename);
  if (stat(compressed, &statbuf) == 0) {
    sprintf(command, "zcat %s", compressed);
    *usingPipeP = TRUE;
#ifndef WINDOWS
    fp = popen (command, "r");
#else
    fp = popen (command, "rb");
#endif
    return (fp);
  }
  sprintf(compressed,"%s.gz",filename);  
  if (stat(compressed, &statbuf) == 0) {
    sprintf(command, "gunzip -c %s", compressed);
#ifndef WINDOWS
    fp = popen (command, "r");
#else
    fp = popen (command, "rb");
#endif
    *usingPipeP = TRUE;
    return (fp);
  }
  return NULL;
}
/*===========================================================================*/
static UnigramItem *NewUnigramTable (int unigramN)
{
    UnigramItem *table;
    int   i;

    table = (UnigramItem *) calloc (unigramN, sizeof (UnigramItem));
    for (i = 0; i < unigramN; i++) {
        table[i].wid = NO_WORD;
        table[i].prob1  = -99.0;
        table[i].bo_wt1 = -99.0;
    }
    return table;
}


/*===========================================================================*/
static void allocModelStuff(LmStruct* modelP,int unigramN, int bigramN, int trigramN, int vocabWordsN)
{

  /*
   * Allocate one extra unigram and bigram entry: semaphors to terminate
   * followers (bigrams and trigrams, respectively) of previous entry.
   * There is one aditional unigram-entry for all words that are in the vocabulary
   * but have no lm information.
   */
     
  modelP->unigrams    = NewUnigramTable (unigramN+2);
  modelP->bigrams     = (BigramItem3g *) calloc ((bigramN+1), sizeof (BigramItem3g));

  if (trigramN > 0) {
    modelP->trigrams  = (TrigramItem3g *) calloc (trigramN, sizeof (TrigramItem3g));
    modelP->tseg_base = (int *) calloc (((bigramN+1)/modelP->bg_seg_sz+1), sizeof (int));
  }

  modelP->max_ucount  = unigramN;
  modelP->ucount      = unigramN;
  modelP->bcount      = bigramN;
  modelP->tcount      = trigramN;
  modelP->vocab_size   = vocabWordsN;
  modelP->vocabwid_map = (int *) calloc (vocabWordsN, sizeof (int));
  modelP->wordSA      = (char **) calloc(modelP->ucount+1, sizeof (CHAR *));
  modelP->hashtableP  = hash_new_string_table();

/*  for (i = 0; i < vocabWordsN; i++)
    modelP->vocabwid_map[i] = UNKNOWN_WORD; *//* an illegal index into unigrams */
    
}

/*===========================================================================*/
/*
 * Reads in a trigram language model from the given file.
 */

static int lmRead (LmStruct * modelP,char* filename)
{
    FILE *fp = NULL;
    int   usingPipe = FALSE;
    int   n_unigram;
    int   n_bigram;
    int   n_trigram;
    int   vocab_size;
    int   file_pos = 0;
    int   i, j, k, last_bg, last_tg;
    char  dumpfilename[1024]; char dumpstring[1024];
    
    INFO("Reading LM file %s\n", filename);
            
    sprintf(dumpfilename,"%s.BINDUMP",filename);
    
    fp = lm_file_open (dumpfilename, &usingPipe);
    if (fp != NULL) {
      INFO("reading from dump %s\n",dumpfilename);
      if (lmReadDump(modelP,fp) < 0) {
        ERROR("illegal dumpfile: %s\n",dumpfilename);
        return (TCL_ERROR);
      }
      if (usingPipe) pclose(fp);
      else fclose (fp);
    }
    else {
      fp = lm_file_open (filename, &usingPipe);
      if (fp == NULL) {
        ERROR("Cannot open %s -- no related file\n",filename);
        return(TCL_ERROR);
      }
      /* Read #unigrams, #bigrams, #trigrams from file */
      ReadNgramCounts (fp, &n_unigram, &n_bigram, &n_trigram);
       INFO("ngrams 1=%d, 2=%d, 3=%d\n", n_unigram, n_bigram, n_trigram);
    
      /* Determine vocabulary size (for vocab-wid -> LM-wid map) */
      vocab_size = vocabNumber(modelP->vocabP);
      INFO("%d words in vocabulary\n", vocab_size);
      if (vocab_size >= 65535) FATAL("#vocab-words(%d) > 65534\n", vocab_size);

      /* Allocate space for LM, including initial OOVs and placeholders; 
         initialize it */
      allocModelStuff(modelP,n_unigram, n_bigram, n_trigram, vocab_size);
        
    /* read Darpa LM file */
    if (1) {
        ReadUnigrams (fp, modelP);
/*      INFO("%d = #unigrams created\n",modelP->ucount);
*/        
        /* Read on to find #bigrams/unigram */
        if (! usingPipe)
            file_pos = ftell (fp);
        ReadBigrams (fp, modelP, FALSE);
        /* Turn the bigram counts into LmStruct.bigrams indices */
        {
            int   i, sum, prev_sum;
            
            sum = 0;
            for (i = 0; i <= modelP->ucount; i++) {
                prev_sum = sum;
                sum += modelP->unigrams[i].bigrams;
                modelP->unigrams[i].bigrams = prev_sum;
            }
        }
        /* Now re-read bigrams and fill in LmStruct */
        if (! usingPipe)
            fseek(fp, file_pos, 0);
        else {
            char string[1024];
            
            pclose (fp);
            fp = lm_file_open (filename, &usingPipe);
            /* skip file until past the \data\ marker */
            do {
                fgets (string, sizeof (string), fp);
            } while ( (strcmp (string, "\\data\\\n") != 0) && (! feof (fp)) );
            /* skip past the \2-grams\ marker */
            do {
                fgets (string, sizeof(string), fp);
            } while ( (strcmp (string, "\\2-grams:\n") != 0)&&(! feof (fp)) );
        }

        init_sorted_list (&sorted_prob2);
        if (modelP->tcount > 0)
            init_sorted_list (&sorted_bo_wt2);
        ReadBigrams (fp, modelP, TRUE);

        modelP->bcount = FIRST_BG(modelP,modelP->ucount);
        modelP->n_prob2 = sorted_prob2.free;
        modelP->prob2  = vals_in_sorted_list (&sorted_prob2);
        free_sorted_list (&sorted_prob2);
/*      INFO("%8d = #bigrams created\n", modelP->bcount);
        INFO("%8d = #prob2 entries\n", modelP->n_prob2);
*/
        if (modelP->tcount > 0) {
            /* Create trigram bo-wts array */
            modelP->n_bo_wt2 = sorted_bo_wt2.free;
            modelP->bo_wt2 = vals_in_sorted_list (&sorted_bo_wt2);
            free_sorted_list (&sorted_bo_wt2);
/*          INFO("%8d = #bo_wt2 entries\n", modelP->n_bo_wt2);
*/
            /* Read rest of file to find #trigrams/bigram */
            if (! usingPipe)
                file_pos = ftell (fp);
            ReadTrigrams (fp, modelP, FALSE);
            /* Turn the trigram counts into LmStruct.trigrams indices */
            {
                int   i, sum, prev_sum;
                
                sum = 0;
                for (i = 0; i <= modelP->bcount; i++) {
                  if ((i & (modelP->bg_seg_sz-1)) == 0)
                      modelP->tseg_base[i >> modelP->log_bg_seg_sz] = sum;
                  prev_sum = sum;
                  sum += modelP->bigrams[i].trigrams;
                  if ((prev_sum - modelP->tseg_base[i >> modelP->log_bg_seg_sz]) > 65535)
                      FATAL("trigram offset > 65535\n");
                  modelP->bigrams[i].trigrams =
                      prev_sum - modelP->tseg_base[i >> modelP->log_bg_seg_sz];
              }
            }
            /* Now re-read trigrams and fill in LmStruct */
            if (! usingPipe)
                fseek(fp, file_pos, 0);
            else {
                char string[1024];
                
                pclose (fp);
                fp = lm_file_open (filename, &usingPipe);
                /* skip file until past the \data\ marker */
                do {
                    fgets (string, sizeof (string), fp);
                } 
                while ( (strcmp (string, "\\data\\\n") != 0) && (! feof (fp)) );
                /* skip past the \3-grams\ marker */
                do {
                    fgets (string, sizeof(string), fp);
                } 
                while ( (strcmp (string, "\\3-grams:\n") != 0)&&(!feof (fp)) );
            }
            init_sorted_list (&sorted_prob3);
            ReadTrigrams (fp, modelP, TRUE);
            modelP->tcount = FIRST_TG(modelP,modelP->bcount);
            modelP->n_prob3 = sorted_prob3.free;
            modelP->prob3  = vals_in_sorted_list (&sorted_prob3);
/*          INFO("%d = #trigrams created\n", modelP->tcount);
            INFO("%d = #prob3 entries\n",modelP->n_prob3);
*/
            free_sorted_list (&sorted_prob3);
        }
    }

    /* Start to read extended file-format */
    /* Possible further entries are subsmodels and separate unigrams */

    while((fgets(dumpstring,sizeof(dumpstring),fp))) {
      if(dumpstring[0]=='\\') {
	if(0 == strncmp(dumpstring,"\\unigram:",9))
	  ReadSeparateUnigrams(fp,modelP);
	else
	  if(0 == strncmp(dumpstring,"\\subsmodel:",11))
	    ReadClasses(fp,modelP);
	  else	
	    INFO("Invalid in extended arpabo format: %s\n",dumpstring);
      }
    }

    if (usingPipe)
        pclose (fp);
    else
        fclose (fp);
    if ((modelP->bcount) > 100) {
      (VOID) lm_WriteDump(modelP,dumpfilename);
    }  
  }    /* end if reading from normal file */
  
   /*
    * Discourage expansion of end_sym and transition to start_sym.  (The given
    * Darpa LM may contain some spurious values that don't reflect these
    * requirements.)
    */
   /* bo_wt(</s>) = SMALLPROB */
   for (i=0;(i<modelP->ucount) && (strcmp(modelP->wordSA[i],end_sym) != 0);i++);
   INFO("bo_wt(%s) changed from %.4f to %.4f\n",
                    end_sym, modelP->unigrams[i].bo_wt1, SMALLPROB);
   modelP->unigrams[i].bo_wt1 = SMALLPROB;
    /* unigram prob(<s>) = SMALLPROB */
   for (i=0;(i<modelP->ucount)&&(strcmp(modelP->wordSA[i],start_sym)!=0); i++);
   INFO("prob(%s) changed from %.4f to %.4f\n",
                   start_sym, modelP->unigrams[i].prob1, SMALLPROB);
   modelP->unigrams[i].prob1 = SMALLPROB;
    /* bigram prob(<s>,<s>) = SMALLPROB (if bigram exists) */
   j = FIRST_BG(modelP,i);
   last_bg = LAST_BG(modelP,i);
   for (;(j<=last_bg)&&(strcmp(modelP->wordSA[BG_WID(modelP,j)],start_sym)!=0);j++);
   if (j <= last_bg) {
       INFO("prob(%s,%s) changed from %.4f to %.4f\n",start_sym, start_sym,
              modelP->prob2[modelP->bigrams[j].prob2],
              modelP->prob2[0]);
       modelP->bigrams[j].prob2 = 0;
        if (modelP->tcount > 0) {
           /* trigram prob(<s>,<s>,<s>) = SMALLPROB (if trigram exists) */
           k = FIRST_TG(modelP,j);
           last_tg = LAST_TG(modelP,j);
           for (; k <= last_tg; k++) {
               if (strcmp (modelP->wordSA[TG_WID(modelP,k)], start_sym) == 0)
                   break;
           }
           if (k <= last_tg) {
             INFO("prob(%s,%s,%s) changed from %.4f to %.4f\n",
                   start_sym, start_sym, start_sym,
                   modelP->prob3[modelP->trigrams[k].prob3],
                   modelP->prob3[0]);
               modelP->trigrams[k].prob3 = 0;
           }
       }
   }

#ifndef STANDALONE
    /* Map LM wids to vocab wid and remove LM entries for words not in vocab */
    if (map_lmwid_to_vocabwid (modelP) > 0)
        remove_nonvocab_entries (modelP);
        
    /* Create an special unigram entry without bigrams for fallback
       to zerograms and move the final dummy unigram entry one down*/
    modelP->unigrams[modelP->ucount].bigrams = modelP->bcount;
    modelP->unigrams[modelP->ucount].prob1   = modelP->zerogram;
    modelP->unigrams[modelP->ucount].bo_wt1  = 0.0;
    modelP->unigrams[modelP->ucount].wid     = NO_WORD; /* not ONE vocabulary word */
    modelP->wordSA[modelP->ucount]           = "word_not_known_to_language_model";
    
    modelP->unigrams[1+modelP->ucount].bigrams = modelP->bcount;
        
        
    for (i = 0; i < modelP->vocab_size; i++)
        modelP->vocabwid_map[i] = modelP->ucount; /* this unigram is reserved to unigrams */
    for (i = 0; i < modelP->ucount; i++) {
        if (modelP->unigrams[i].wid < modelP->vocab_size)
          modelP->vocabwid_map[modelP->unigrams[i].wid] = i;
    }

    lm_set_param (modelP, modelP->lw, modelP->zerogram, modelP->log_wip, FALSE);
    
     INFO("%d = #unigrams, %d = #bigrams, %d = #trigrams\n",
           modelP->ucount, modelP->bcount, modelP->tcount);
#else
    for (i = 0; i < modelP->vocab_size; i++)
        modelP->vocabwid_map[i] = i;
#endif
   return (TCL_OK);
}

/*===========================================================================*/

#ifndef STANDALONE
/*
 * Map LM wid entries to vocab wid, while eliminating non-vocabulary entries
 * from the LM.  Until this point, lm_t is built independent of the vocab.
 * In particular, vocab wid = LM wid, and the LM is a direct, 1-to-1
 * representation of the input Darpa Trigram LM file.  Return the number of
 * non-vocabulary LM words.
 */
static int map_lmwid_to_vocabwid (LmStruct *modelP)
{
    int u, b, t, vocab_wid;
    int n_nonvocab_ug, n_nonvocab_bg, n_nonvocab_tg;
    int first_bg, last_bg;
    int first_tg, last_tg;
    int * classes_used=0;
    
    INFO("Mapping LM wids to vocab wid\n");

    /*
     * Map unigram wid entries to vocabulary wid.  If any word not found
     * in vocabulary, mark it and its descendant bigram and trigrams to
     * be illegal.
     */
    n_nonvocab_ug = 0;
    n_nonvocab_bg = 0;
    n_nonvocab_tg = 0;

    
    for (u = 0; u < modelP->ucount; u++) {
        vocab_wid = vocabIndex(modelP->vocabP,modelP->wordSA[UG_WID(modelP,u)]);
        if ((vocab_wid == NO_WORD)) { 
          modelP->unigrams[u].wid = NO_WORD;
          first_bg = FIRST_BG(modelP,u);
          last_bg  = LAST_BG (modelP,u);
          for (b = first_bg; b <= last_bg; b++) {
            modelP->bigrams[b].wid = NO_WORD;
              if (modelP->tcount > 0) {
                first_tg = FIRST_TG(modelP,b);
                last_tg  = LAST_TG (modelP,b);
                for (t = first_tg; t <= last_tg; t++)
                  modelP->trigrams[t].wid = NO_WORD;
              }
          }
          n_nonvocab_ug++;
        } else {
          modelP->unigrams[u].wid = vocab_wid;
        }  
    }

    /*
     * Map bigram wid entries to vocabulary wid, marking illegal ones
     * and their descendant trigrams.
     */
    for (b = 0; b < modelP->bcount; b++) {
        if ((vocab_wid = BG_WID(modelP,b)) != NO_WORD) {
            vocab_wid = UG_WID(modelP,vocab_wid);
            if ((vocab_wid == NO_WORD) && (modelP->tcount > 0)) {
                first_tg = FIRST_TG(modelP,b);
                last_tg  = LAST_TG (modelP,b);
                for (t = first_tg; t <= last_tg; t++)
                    modelP->trigrams[t].wid = NO_WORD;
            }
            modelP->bigrams[b].wid = vocab_wid;
        }
        if (modelP->bigrams[b].wid == NO_WORD) {
            n_nonvocab_bg++;
        }    
    }

    /* Map trigram wid entries to vocabulary wid, marking illegal ones */
    for (t = 0; t < modelP->tcount; t++) {
        if ((vocab_wid = TG_WID(modelP,t)) != NO_WORD)
            modelP->trigrams[t].wid = UG_WID(modelP,vocab_wid);
        if (modelP->trigrams[t].wid == NO_WORD)
            n_nonvocab_tg++;
    }

    free(classes_used);
    
    INFO("%d = #LM words not in vocab; used in %d bigrams, %d trigrams\n",
          n_nonvocab_ug, n_nonvocab_bg, n_nonvocab_tg);
    
    return (n_nonvocab_ug);
}

/*===========================================================================*/
static int remove_nonvocab_entries (LmStruct * modelP)
{
    int  u, b, t;
    int first_bg, last_bg;
    int first_tg = 0, last_tg = 0;
    int n_ug, n_tg, n_bg;
    int *t_tseg_base = NULL, *tt;
    
    INFO("Removing non-vocabulary entries from LM\n");

    /*
     * Compress trigrams (if they exist), eliminating those with NO_WORD.
     * Need to recompute tseg_base too!
     */
    if (modelP->tcount > 0) {
        /* Temporary tseg_base */
        t_tseg_base = 
            (INT *) calloc (((modelP->bcount+1)/modelP->bg_seg_sz+1), sizeof (INT));
        n_tg = 0;
        for (b = 0; b < modelP->bcount; b++) {
            first_tg = FIRST_TG(modelP,b);
            last_tg  = LAST_TG (modelP,b);
            
            /* Recompute tseg_base and trigrams offset for b */
            if ((b & (modelP->bg_seg_sz-1)) == 0)
                t_tseg_base[b >> modelP->log_bg_seg_sz] = n_tg;
            modelP->bigrams[b].trigrams =
                n_tg - t_tseg_base[b >> modelP->log_bg_seg_sz];
            
            /* Compress trigrams for b, retaining only legal ones */
            if (BG_WID(modelP,b) != NO_WORD) {
                for (t = first_tg; t <= last_tg; t++) {
                    if (TG_WID(modelP,t) != NO_WORD) {
                        modelP->trigrams[n_tg] = modelP->trigrams[t];
                        n_tg++;
                    }
                }
            }
        }
        /* Complete the job for the final dummy bigram entry */
        if ((b & (modelP->bg_seg_sz-1)) == 0)
            t_tseg_base[b >> modelP->log_bg_seg_sz] = n_tg;
        modelP->bigrams[b].trigrams = n_tg - t_tseg_base[b >> modelP->log_bg_seg_sz];
    
        /* Update lm_t.tcount and lm_t.tseg_base */
        modelP->tcount = n_tg;
        tt = modelP->tseg_base;
        modelP->tseg_base = t_tseg_base;
        t_tseg_base = tt;
    }
    
    /* Compress bigrams */
    n_bg = 0;
    n_tg = 0;
    for (u = 0; u < modelP->ucount; u++) {
        first_bg = FIRST_BG(modelP,u);
        last_bg  = LAST_BG (modelP,u);
        modelP->unigrams[u].bigrams = n_bg;
        
        if (UG_WID(modelP,u) != NO_WORD) {
            for (b = first_bg; b <= last_bg; b++) {
                if (modelP->tcount > 0) {
                    first_tg = FIRST_TG(modelP,b);
                    last_tg = LAST_TG(modelP,b);
                }
                if (BG_WID(modelP,b) != NO_WORD) {
                    modelP->bigrams[n_bg] = modelP->bigrams[b];
                    if (modelP->tcount > 0) {
                        if ((n_bg & (modelP->bg_seg_sz-1)) == 0)
                            t_tseg_base[n_bg >> modelP->log_bg_seg_sz] = n_tg;
                        if ((n_tg - t_tseg_base[n_bg >> modelP->log_bg_seg_sz]) > 65535)
                            FATAL("trigram offset > 65535\n");
                        modelP->bigrams[n_bg].trigrams =
                            n_tg - t_tseg_base[n_bg >> modelP->log_bg_seg_sz];
                        n_tg += (last_tg - first_tg + 1);
                    }
                    n_bg++;
                }
            }
        }
    }
    if (modelP->tcount > 0) {
      /* Create a final dummy bigram entry */
      if ((n_bg & (modelP->bg_seg_sz-1)) == 0)
          t_tseg_base[n_bg >> modelP->log_bg_seg_sz] = n_tg;
      if ((n_tg - t_tseg_base[n_bg >> modelP->log_bg_seg_sz]) > 65535)
          FATAL("trigram offset > 65535\n");
      modelP->bigrams[n_bg].trigrams = n_tg - t_tseg_base[n_bg >> modelP->log_bg_seg_sz];
      if (n_tg != modelP->tcount) {
          ERROR("%s(%d): n_tg (%d) != modelP->tcount (%d)\n",
                                 __FILE__,__LINE__,n_tg,modelP->tcount);
          exit (-1);
      }
        
      tt = modelP->tseg_base;
      modelP->tseg_base = t_tseg_base;
      t_tseg_base = tt;
      free (t_tseg_base);
    }
    modelP->bcount = n_bg;
    
    /* Compress unigrams */
    n_ug = 0;
    for (u = 0; u < modelP->ucount; u++) {
        if (UG_WID(modelP,u) != NO_WORD) {
            modelP->unigrams[n_ug] = modelP->unigrams[u];
            n_ug++;
        }
    }
    
    /* add final dummy unigram entry */
    
    /* Create an special unigram entry for fallback to zerograms and
       a final dummy unigram entry */
    modelP->unigrams[n_ug].bigrams = modelP->bcount;
    modelP->unigrams[n_ug].prob1   = modelP->zerogram;
    modelP->unigrams[n_ug].bo_wt1  = 0.0;
    modelP->unigrams[n_ug].wid     = NO_WORD; /* not ONE vocabulary word */
    modelP->wordSA[n_ug]           = "UNKNOWN_WORD";
    
    modelP->unigrams[n_ug].bigrams = modelP->bcount;
    modelP->ucount = n_ug;
    return 1;
}
#endif

/*===========================================================================*/

/*
 * add language weight
 * and insertion penalty.
 */
static VOID lm_set_param (LmStruct *modelP, 
                          float     lw,
                          float     zero,
                          float     wip,
                          int       word_pair)
{
 /* HERE WE NEED TO ADD THE LANGUAGE WEIGHT TO THE LM-VALUES !! */
  int probX, probN;

/*  int   unkwid = wstr2lmwid (modelP, "<UNK>"); */

  
  probN = modelP->ucount; /* number of UNIGRAMS */
  
  for (probX = 0; probX < probN; probX ++) {
     modelP->unigrams[probX].prob1  *= (lw);
     modelP->unigrams[probX].prob1  -= wip;
     modelP->unigrams[probX].bo_wt1 *= (lw);
  }
   
  probN = modelP->n_prob2; /* number of DIFFERING probabilities for bigrams */ 
  for (probX = 0; probX < probN; probX ++) modelP->prob2[probX] *= (lw);
  for (probX = 0; probX < probN; probX ++) modelP->prob2[probX] -= wip;

  if (modelP->tcount > 0) /* if we have trigrams */ {
    probN = modelP->n_bo_wt2;
    for (probX = 0; probX < probN; probX ++) modelP->bo_wt2[probX] *= (lw);
    probN = modelP->n_prob3;
    for (probX = 0; probX < probN; probX ++) modelP->prob3[probX] *= (lw);
    for (probX = 0; probX < probN; probX ++) modelP->prob3[probX] -= wip;
  }  

  if(modelP->separate_unigrams) {
    for (probX = 0; probX < vocabNumber(modelP->vocabP); probX ++) {
      modelP->separate_unigrams[probX]  *= (lw);
      modelP->separate_unigrams[probX] -= wip;
    }
  }
  if(modelP->classes) {
    for (probX = 0; probX < vocabNumber(modelP->vocabP); probX ++) {
      modelP->classes[probX].penalty  *= (lw);
    }
  }
  



}

/*===========================================================================*/
int lm_VocabwdInCurrent2 ( LmStruct *mP, CHAR* wstr )
{
  

  if(mP->follower) {
    for(mP=mP->follower;mP;mP=mP->follower) /* Make an "AND" */
      if(wstr2lmwid (mP, wstr) == NO_WORD)
	return 0;
    return 1;
  }
  else
    return ( wstr2lmwid (mP, wstr) != NO_WORD); 
}

int lm_VocabwdInCurrent ( CHAR* wstr )
{
  return lm_VocabwdInCurrent2 ( currentLmP, wstr ); /* HIER */
}


/*===========================================================================*/

static int   wstr2lmwid (modelP, w)
    LmStruct *modelP;
    char *w;
{
   int val;
   int * bla;
   bla = (int *) hash_lookup(modelP->hashtableP,w);
   val = (INT) bla; 
   if (val == 0 ) return NO_WORD;
   return ((INT  ) val-1);
}

/*===========================================================================*/

/*
 * LM cache structure:
 * 
 * TgCacheInfo caches trigram information for bigrams <w1,w2>.  Entries for
 * all bigrams with same w2 are linked together.  The first of this list is
 * pointed to by lm_bg_info[w2].  This gives fast access to any cached entry,
 * or to recognize that no cached entry exists.
 * TgCacheInfo entries are allocated and linked up on demand, very few are
 * active in any utterance.
 * TgCacheInfo points to cached trigram <w1,w2,*> scores; however, no cache
 * line is allocated for any bigram for which no trigram exists.
 * 
 * BgCacheInfo[w2] points to cached trigram info entries for all <w1,w2>, and
 * to cached bigram <w2,*> scores.
 * 
 * CacheInfo[l] maintains info for cache line l.
 */


#if defined(LM_CACHE_PROFILE)
static int   n_tg_score_calls;
static int   n_bg_score_calls;
static int   n_cache_line_repl;
static int   n_lines_active;
static int   n_lines_active_max;
#endif
/*===========================================================================*/
static int   find_free_cacheline ( LmStruct *modelP )
{
    int   i;
    int   sel = 0; /* initalization not necessary but reasons are quite involved ! */
    int   sel_fr, sel_sz;
    int   lmw1, bg, last_bg;
    float *prob_ptr;
    int  tg;
    TgCacheInfo *tgInfo;

    
    BgCacheInfo *lm_bg_info=modelP->bg_info;
    CacheInfo   *lm_cache_info=modelP->cache_info;
    int          cacheLineN=modelP->cacheLineN;
    int          currentFrame=modelP->currentFrame;

    /* Find free line, or oldest and smallest line to replace */
    sel_fr = currentFrame;
    sel_sz = 0x7fffffff;
    for (i = 0; i < cacheLineN; i++) {
        if (lm_cache_info[i].w1 < 0)
            return (i);
        
        if (lm_cache_info[i].fr_used < sel_fr) {
            sel_fr = lm_cache_info[i].fr_used;
            sel_sz = lm_cache_info[i].size;
            sel = i;
        } else if (lm_cache_info[i].fr_used == sel_fr) {
            if (lm_cache_info[i].size < sel_sz) { 
                sel_sz = lm_cache_info[i].size;
                sel = i;
            }
        }
    }
    assert ((sel >= 0) && (sel < cacheLineN));
    
#if defined(LM_CACHE_PROFILE)
    n_cache_line_repl++;
#endif

    /* Reclaim cache line[sel]; sel = line selected to be replaced */
    if (lm_cache_info[sel].w2 < 0) {
        /* Currently a bigrams cache line */
        lm_bg_info[lm_cache_info[sel].w1].line = -1;
        
        lmw1 = VOCAB2LM_WID (modelP,lm_cache_info[sel].w1);
        assert (lmw1 >= 0);
        last_bg = LAST_BG (modelP, lmw1);
        
        prob_ptr = lm_cache_info[sel].prob;
        for (bg = FIRST_BG (modelP, lmw1); bg <= last_bg; bg++)
            prob_ptr[modelP->bigrams[bg].wid] = ILLEGAL_PROB+1;  /* an illegal logprob value */
    } else {
        /* Currently a trigrams cache line */
        tgInfo = lm_cache_info[sel].tgInfo;
        tgInfo->line = -1;
        
        prob_ptr = lm_cache_info[sel].prob;
        for (tg = tgInfo->first_tg; tg <= tgInfo->last_tg; tg++)
            prob_ptr[modelP->trigrams[tg].wid] = ILLEGAL_PROB+1;
    }

    return (sel);
}
/*===========================================================================*/
void lm_CacheReset1 (LmStruct * modelP)
{
    int   i, j;
    TgCacheInfo * local_tgInfo, *tmp;
    BgCacheInfo * local_bginfo = modelP->bg_info;
    CacheInfo * local_cacheinfo = modelP->cache_info;
    int           local_cacheLineN = modelP->cacheLineN;
    
    for (i = 0; i < modelP->vocab_size; i++) {
        local_bginfo[i].line = -1;
        for (local_tgInfo = local_bginfo[i].tgInfo; local_tgInfo; local_tgInfo = tmp) {
            tmp = local_tgInfo->next;
            listelem_free ((char *) local_tgInfo, sizeof (TgCacheInfo));
        }
        local_bginfo[i].tgInfo = NULL;
    }
    for (i = 0; i < local_cacheLineN; i++) {
        local_cacheinfo[i].w1 = -1;
        local_cacheinfo[i].tgInfo = NULL;
        local_cacheinfo[i].fr_used = -1;
        
        /* Set all cached logprob values to some illegal (+ve) flag */
        for (j = 0; j < modelP->vocab_size; j++)
            local_cacheinfo[i].prob[j] = ILLEGAL_PROB+1;
    }
    
    modelP->currentFrame = 0;
}
/*===========================================================================*/
static void lm_NextFrame2 (LmStruct* mP)
{
#if defined(LM_CACHE_PROFILE)
    int   i, k;
    
    k = 0;
    for (i = 0; i < cacheLineN; i++)
        if (lm_cache_info[i].fr_used == mP->currentFrame)
            k++;
    if (n_lines_active_max < k)
        n_lines_active_max = k;
    n_lines_active += k;
#endif
    if (mP->currentFrame < INT_MAX-1) {
      mP->currentFrame++;
    } else {
      lm_CacheReset1(mP);
    }
      
    if(mP->follower)
      lm_NextFrame2(mP->follower);
}


/*===========================================================================*/

static float  local_lm_UgScore (LmStruct* mP, INT   w)
{   int   lmwid;

    lmwid = VOCAB2LM_WID(mP,w);
    assert (lmwid >= 0);
    return (UG_PROB_F (mP, lmwid));
}
/*===========================================================================*/
static float   local_lm_BgScore (LmStruct* mP, INT   w1, int   w2)
{
    float   *prob_ptr;
    int     bg, last_bg;
    float   prob;
    int     lmw1, lmw2, line;

    BgCacheInfo *lm_bg_info = mP->bg_info;
    CacheInfo   *lm_cache_info = mP->cache_info;
    
    lmw1 = VOCAB2LM_WID(mP,w1);
    assert (lmw1 >= 0);
    


    if ((line = lm_bg_info[w1].line) < 0) {
        /* Bigrams for <w1> not cached; get free cache line and fill it first */
        line = lm_bg_info[w1].line = find_free_cacheline ( mP );
        
        lm_cache_info[line].w1 = w1;
        lm_cache_info[line].w2 = -1;  /* mark as bigram-cacheline */
        
        bg = FIRST_BG (mP, lmw1);
        last_bg = LAST_BG (mP, lmw1);
        lm_cache_info[line].size = last_bg - bg + 1;
        
        prob_ptr = lm_cache_info[line].prob;
        
        /* the bigrams[].wid are vocabwids.
         * therefore always index prob_ptr with vocabwid */
        
        for (; bg <= last_bg; bg++) {
            prob_ptr[mP->bigrams[bg].wid] = BG_PROB_F(mP, bg);
        }    
    } else
        prob_ptr = lm_cache_info[line].prob;
    
    lm_cache_info[line].fr_used = mP->currentFrame;
    
    if ((prob = prob_ptr[w2]) >= ILLEGAL_PROB) { 
        lmw2 =  VOCAB2LM_WID(mP,w2);
        assert (lmw2 >= 0);
        
        prob = UG_BO_WT_F (mP, lmw1) + UG_PROB_F (mP, lmw2);
/*	prob_ptr[w2]=prob; */
    }
    
    return (prob);
}
/*===========================================================================*/
/*
 * Find bigram entry for <w1,w2> and return index.  Return -1 if none available.
 */
static int   find_bigram (LmStruct* mP, INT   w1, int   w2)
{
    int   bg, last_bg, lmw1;
    int   temp;

    
    lmw1 = VOCAB2LM_WID(mP,w1);
    assert (lmw1 >= 0);
    

    bg = FIRST_BG (mP, lmw1);
    last_bg = LAST_BG (mP, lmw1);

    /* Do binary search to narrow down the area */
    /* For termination, have const>0 in "while(bg<last_bg - const)" */

    while(bg<last_bg - 8) {   
      temp = (bg+last_bg)/2;
      if( w2 < mP->bigrams[temp].wid)
	last_bg = temp-1;
      else
	bg = temp;
    }

    /* Do linear search when array is small enough */

    for (; bg <= last_bg; bg++) {
      if (mP->bigrams[bg].wid == w2)
	break;
    }	
    
    return (((bg <= LAST_BG (mP, lmw1)) && (mP->bigrams[bg].wid==w2))? bg : -1);

}

/*===========================================================================*/
static float   local_lm_TgScore (LmStruct* mP, INT   w1, int   w2, int   w3)
{
    TgCacheInfo *tgInfo, *prev_tgInfo;
    float   *prob_ptr;
    int bg, tg;
    float prob;

    BgCacheInfo *lm_bg_info = mP->bg_info;
    CacheInfo   *lm_cache_info = mP->cache_info;

#if defined(LM_CACHE_PROFILE)
    n_tg_score_calls++;
#endif

    if ((mP->tcount <= 0) || (w1 == NO_WORD) || ((mP->vocabP->startWordX==w2) && (mP->separate_unigrams==0 && (mP->interpol_names.itemA != 0))))
        return (local_lm_BgScore (mP, w2, w3));

    /* Find cached trigrams info for <w1,w2>, if any */
    prev_tgInfo = NULL;
    for (tgInfo = lm_bg_info[w2].tgInfo; tgInfo; tgInfo = tgInfo->next) {
        if (tgInfo->w1 == w1)
            break;
        prev_tgInfo = tgInfo;
    }
    
    if (tgInfo == NULL) {
        /* Trigrams info for <w1,w2> not known; find it */
        bg = find_bigram (mP, w1, w2);
        tgInfo = (TgCacheInfo *) listelem_alloc (sizeof (TgCacheInfo));
        tgInfo->w1 = w1;
        if (bg >= 0) {
            tgInfo->first_tg = FIRST_TG(mP, bg);
            tgInfo->last_tg = LAST_TG(mP, bg);
            tgInfo->bo_wt = BG_BO_WT_F(mP, bg);
        } else {
            tgInfo->first_tg = -1;
            tgInfo->last_tg = -2;
            tgInfo->bo_wt = 0;
        }
        tgInfo->line = -1;
        
        /* Insert new entry at head of list */
        tgInfo->next = lm_bg_info[w2].tgInfo;
        lm_bg_info[w2].tgInfo = tgInfo;
    } else if (prev_tgInfo != NULL) {
        /* Trigrams info for <w1,w2> exists; 
           move it to head of cached entries. */
        prev_tgInfo->next = tgInfo->next;
        tgInfo->next = lm_bg_info[w2].tgInfo;
        lm_bg_info[w2].tgInfo = tgInfo;
    }
    
    /* Now tgInfo contains trigrams info for <w1,w2>; look for cached prob */
    if (tgInfo->last_tg >= tgInfo->first_tg) {
        /*
         * Trigrams for <w1,w2> exist; see if they have been cached.
         * (Later, perhaps, cache only if #trigrams for <w1,w2> > some minimum.
         */
        if (tgInfo->line < 0) {
            /* Trigrams for <w1,w2> not cached;
               get free cache line and fill it first */
               
            tgInfo->line = find_free_cacheline ( mP );
            
            lm_cache_info[tgInfo->line].w1 = w1;
            lm_cache_info[tgInfo->line].w2 = w2;
            lm_cache_info[tgInfo->line].size = 
                            tgInfo->last_tg - tgInfo->first_tg + 1;
            lm_cache_info[tgInfo->line].tgInfo = tgInfo;

            prob_ptr = lm_cache_info[tgInfo->line].prob;
            for (tg = tgInfo->first_tg; tg <= tgInfo->last_tg; tg++)
                prob_ptr[mP->trigrams[tg].wid] = TG_PROB_F(mP, tg);
        } else
            prob_ptr = lm_cache_info[tgInfo->line].prob;

        lm_cache_info[tgInfo->line].fr_used = mP->currentFrame;
        
        if ((prob = prob_ptr[w3]) >= ILLEGAL_PROB) {
            prob = tgInfo->bo_wt + local_lm_BgScore (mP,w2, w3);
/*	    prob_ptr[w3]=prob; */
	}
    } else {
        /* No trigrams for <w1,w2> exist; revert to bigrams */
        prob = tgInfo->bo_wt + local_lm_BgScore (mP,w2, w3);
    }

    return (prob);
}
/*===========================================================================*/

/*-------------------- list stuff for caches---------------------------*/

#define MAX_LIST        16
#define MAX_ALLOC       40944

/*
 * Elements are seen as structures with an array of void pointers.
   So element size
 * must be integral muliple of (void *).
 */
typedef struct list_s {
    char **freelist;    /* ptr to first element in freelist */
    int elem_size;    /* #(CHAR *) in element */
    int n_malloc;     /* #elements to malloc if run out of free elments */
} list_t;
list_t list[MAX_LIST];
INT n_list = 0;


static char *listelem_alloc (INT elem_size)
{
    int i, j;
    char **cpp, *cp;
    
    for (i = 0; i < n_list; i++) {
        if (list[i].elem_size == elem_size)
            break;
    }
    if (i >= n_list) {
        /* New list element size encountered, create new list entry */
        if (n_list >= MAX_LIST)
            FATAL("Increase MAX_LIST\n");
        if (elem_size > MAX_ALLOC)
            FATAL("Increase MAX_ALLOC to %d\n", elem_size);
        if ((elem_size % sizeof(CHAR *)))
            FATAL("Element size (%d) not multiple of (CHAR *)\n", elem_size);

        list[n_list].freelist = NULL;
        list[n_list].elem_size = elem_size;
        list[n_list].n_malloc = MAX_ALLOC / elem_size;
        i = n_list++;
    }
    
    if (list[i].freelist == NULL) {
        cpp = list[i].freelist = 
                  (CHAR **) calloc (list[i].n_malloc, elem_size);
        cp = (CHAR *) cpp;
        for (j = list[i].n_malloc-1; j > 0; --j) {
            cp += elem_size;
            *cpp = cp;
            cpp = (CHAR **)cp;
        }
        *cpp = NULL;
    }
    
    cp = (CHAR *)(list[i].freelist);
    list[i].freelist = (CHAR **)(*(list[i].freelist));
    return (cp);
}

static VOID listelem_free (CHAR *elem, int elem_size)
{
    int i;
    char **cpp;
    
    for (i = 0; i < n_list; i++) {
        if (list[i].elem_size == elem_size)
            break;
    }
    if (i >= n_list)
        FATAL("elem_size (%d) not in known list\n", elem_size);
    
    cpp = (CHAR **) elem;
    *cpp = (CHAR *) list[i].freelist;
    list[i].freelist = cpp;
}

static char *salloc (char *str)
{
    int len = strlen(str)+1;
    char *buf;
    
    buf = (char *) malloc(len);
    strcpy (buf, str);
    return (buf);
}

VOID            lm_SetStartSym ( char *sym )
{
start_sym = (CHAR*) salloc(sym);
}
VOID            lm_SetEndSym ( char *sym )
{
end_sym = (CHAR*) salloc(sym);
}
/*===========================================================================*/
static int lm_WriteDump(LmStruct *modelP, char *dumpfilename)
{
  int i;

  FILE *fp=fileOpen(dumpfilename,"w");
  if (fp == 0) {
   ERROR("Could not open %s for writing",dumpfilename);
   return (-1);
  }
  init_mach_ind_io();             /* just in case this had not been done */
  fprintf(fp,"binary lms\n"); 
  write_int(fp,modelP->ucount);
  write_int(fp,modelP->bcount);
  write_int(fp,modelP->tcount);
  write_int(fp,modelP->n_prob2);
  if (modelP->tcount > 0) {
    write_int(fp,modelP->bg_seg_sz);
    write_int(fp,modelP->log_bg_seg_sz);
    write_int(fp,modelP->n_bo_wt2);
    write_int(fp,modelP->n_prob3);
  }

  write_int(fp,11111);            /*sanity check */
  for (i = 0; i< modelP->ucount+1; i++) {
    write_int(fp,modelP->unigrams[i].wid);
    write_float(fp,modelP->unigrams[i].prob1);
    write_float(fp,modelP->unigrams[i].bo_wt1);
    write_int(fp,modelP->unigrams[i].bigrams);
  }
  write_int(fp,22222);            /*sanity check */

  for (i = 0; i< modelP->ucount; i++) {
    fprintf(fp,"%s\n",modelP->wordSA[i]);
  }

  write_int(fp,33333);            /*sanity check */
  for (i = 0; i < modelP->bcount+1; i++) {
    write_short(fp,modelP->bigrams[i].wid);
    write_short(fp,modelP->bigrams[i].prob2);
    write_short(fp,modelP->bigrams[i].bo_wt2);
    write_short(fp,modelP->bigrams[i].trigrams);
  }

  write_int(fp,44444);            /*sanity check */
  write_floats(fp,modelP->prob2,modelP->n_prob2);

  write_int(fp,55555);            /*sanity check */
  if(modelP->tcount > 0) {
    for (i = 0; i < modelP->tcount; i++) {
      write_short(fp,modelP->trigrams[i].wid);
      write_short(fp,modelP->trigrams[i].prob3);
    }
    write_int(fp,66666);            /*sanity check */
    write_floats(fp,modelP->bo_wt2,modelP->n_bo_wt2);
    write_int(fp,77777);            /*sanity check */
    write_floats(fp,modelP->prob3,modelP->n_prob3);
    write_int(fp,88888);            /*sanity check */
    write_ints(fp,modelP->tseg_base, 1+ ((modelP->bcount +1)/modelP->bg_seg_sz));
  }


  if(modelP->separate_unigrams) {
    INFO("Using extended .BINDUMP format to store separate unigrams\n");
    write_int(fp,99990);
    write_int(fp,vocabNumber(modelP->vocabP));
    for (i = 0; i< vocabNumber(modelP->vocabP); i++) {
      /* Have to write the string to achieve vocabulary independence */
      write_string(fp,vocabName(modelP->vocabP,i)); write_float(fp,modelP->separate_unigrams[i]);
    }
  }    
  if(modelP->classes) {
    INFO("Using extended .BINDUMP format to store class based n-gram information\n");
    write_int(fp,99991);
    write_int(fp,vocabNumber(modelP->vocabP));
    for (i = 0; i< vocabNumber(modelP->vocabP); i++) {
      /* Have to write the string to achieve vocabulary independence */
      write_string(fp,vocabName(modelP->vocabP,i));
      write_string(fp,vocabName(modelP->vocabP,modelP->classes[i].class));
      write_float(fp,modelP->classes[i].penalty);
    }
  }    


  write_int(fp,99999);            /*sanity check */
  fileClose (dumpfilename, fp);
	return(0);
}
/*===========================================================================*/
static int lmReadDump(LmStruct * modelP, FILE *fp)
{
INT i, erg, test, n_ug, n_bg, n_tg, n_vocab;
CHAR teststring[200];

init_mach_ind_io();             /* just in case this had not been done */
fgets(teststring,100,fp);
if (strcmp(teststring,"binary lms\n")!= 0) {
  ERROR("Not a dumpfile\n"); return (-1);
}
n_ug = read_int(fp);
if ((n_ug <=0) || (n_ug >= 65535)) { 
  ERROR("Number of unigrams %d\n",n_ug); return (-1);
}  
else INFO("Read #unigrams: %5d\n",n_ug);

n_bg =  read_int(fp);
if (n_bg <=0) { ERROR("Number of bigrams %d",n_bg);  return (-1); }
else            INFO("Read #bigrams:  %5d\n",n_bg);

n_tg = read_int(fp);
if (n_tg < 0) { ERROR("Number of trigrams %d",n_tg); return (-1); }
else            INFO("Read #trigrams: %5d\n",n_tg);

n_vocab = vocabNumber(modelP->vocabP);
INFO("%d words in vocabulary\n", n_vocab);
  
modelP->n_prob2 = read_int(fp);
if (modelP->n_prob2 <=0) {
  ERROR("Nr of bigram-floats %d\n", modelP->n_prob2); return (-1);
}

if (n_tg > 0) {
  test = read_int(fp);
  if (test != modelP->bg_seg_sz) {
    INFO("Changed bg_seg_sz %d !=%d\n",test,modelP->bg_seg_sz);
    modelP->bg_seg_sz=test;
  }
  test = read_int(fp);
  if (test != modelP->log_bg_seg_sz) {
    INFO("Changed log_bg_seg_sz %d !=%d\n",test,modelP->log_bg_seg_sz);
    modelP->log_bg_seg_sz=test;
  }
  
  modelP->n_bo_wt2 = read_int(fp);
  if (modelP->n_bo_wt2 <=0)
    ERROR("Nr of bo_wt2s %d\n", modelP->n_bo_wt2);
  
  modelP->n_prob3 = read_int(fp);
  if (modelP->n_prob3 <=0)
    ERROR("Nr of trigram-floats %d\n",modelP->n_prob3);
}
allocModelStuff(modelP,n_ug,n_bg,n_tg,n_vocab);  


test = read_int(fp);
if (test != 11111)
    ERROR("Checkval 11111 != %d\n",test);

for (i = 0; i< modelP->ucount+1; i++) {
  modelP->unigrams[i].wid = read_int(fp);
  modelP->unigrams[i].prob1 = read_float(fp);
  modelP->unigrams[i].bo_wt1 = read_float(fp);
  modelP->unigrams[i].bigrams = read_int(fp);
}

test = read_int(fp);
if (test != 22222)
    ERROR("Checkval 22222 != %d\n",test);
            

for (i = 0; i< modelP->ucount; i++) {
  char string[255];
  fgets (string,255,fp);
  string[strlen(string)-1] = '\0'; /* chop \n */
  modelP->wordSA[i] = salloc(string);
  hash_insert(modelP->hashtableP,
               (VOID *) modelP->wordSA[i],
               (VOID *) (i+1)); 
}

INFO("file contains the word-strings \"%s\" to \"%s\"\n",
             modelP->wordSA[0],modelP->wordSA[n_ug-1]);  

test = read_int(fp);
if (test != 33333)
    ERROR("Checkval 33333 != %d",test);

for (i = 0; i < modelP->bcount+1; i++) {
  modelP->bigrams[i].wid      = read_short(fp);
  modelP->bigrams[i].prob2    = read_short(fp);
  modelP->bigrams[i].bo_wt2   = read_short(fp);
  modelP->bigrams[i].trigrams = read_short(fp);
}

test = read_int(fp);
if (test != 44444) {
  int moretest;
  if (feof(fp)) {
    ERROR("Unexpected end of file\n");
    return (-1);
  }
  moretest = read_int(fp);
  ERROR("checkval 44444 != %d",test);
  return (-1);
}    

modelP->prob2 =
       (float *) calloc((modelP->n_prob2), sizeof (float));
    
read_floats(fp,modelP->prob2,modelP->n_prob2);

test = read_int(fp);
if (test != 55555) {
    ERROR("checkval 55555 != %d",test);
    return (-1);
}

if(modelP->tcount > 0) {

  for (i = 0; i < modelP->tcount; i++) {
    modelP->trigrams[i].wid   = read_short(fp);
    modelP->trigrams[i].prob3 = read_short(fp);
  }

  test = read_int(fp);
  if (test != 66666) {
    ERROR("checkval 66666 != %d",test);
    return (-1);
  }
  modelP->bo_wt2 = (float *) calloc(modelP->n_bo_wt2, sizeof (float));
    
  read_floats(fp,modelP->bo_wt2,modelP->n_bo_wt2);
  
  INFO("done reading bigram bo-probs\n");  
  test = read_int(fp);
  if (test != 77777) {
    ERROR("checkval 77777 != %d",test);
    return (-1);
  }
  modelP->prob3 = (float *) calloc(modelP->n_prob3, sizeof (float));
  
  read_floats(fp,modelP->prob3,modelP->n_prob3);
  test = read_int(fp);
  if (test != 88888) {
    ERROR("checkval 88888 != %d",test);
    return (-1);
  }
  read_ints(fp,modelP->tseg_base, ((modelP->bcount +1 )/modelP->bg_seg_sz+1));
}

  INFO("done reading trigram\n");  
  test = read_int(fp);

  if(test == 99990) {
    test = read_int(fp);
    modelP->separate_unigrams = (float *) calloc (vocabNumber(modelP->vocabP), sizeof (float));    
    for(i=0;i<vocabNumber(modelP->vocabP);i++) {
      modelP->separate_unigrams[i] = -99.9;
    }
    for (i = 0; i< test; i++) {
      read_string(fp,teststring);
      erg = VocabIndex(teststring,modelP);
      if(erg==NO_WORD) {
/*	WARN("Non-vocabulary %s in separated unigrams\n",teststring); */
	read_float(fp);
      }
      else
	modelP->separate_unigrams[erg] = read_float(fp);
    }
    test = read_int(fp);
  }    
  if(test == 99991) {
    test = read_int(fp);
    modelP->classes = (ClassInfo *) calloc (vocabNumber(modelP->vocabP), sizeof (ClassInfo));
    for(i=0;i<vocabNumber(modelP->vocabP);i++) {
      modelP->classes[i].class=i;
      modelP->classes[i].penalty = 0;
    }
    for (i = 0; i< test; i++) {
      read_string(fp,teststring);
      erg = VocabIndex(teststring,modelP);
      if(erg==NO_WORD) {
/*	WARN("Non-vocabulary %s in classes\n",teststring); */
	read_string(fp,teststring);
	read_float(fp);
      }
      else {
	if(!hash_lookup(modelP->hashtableP,(VOID *)vocabName(modelP->vocabP,erg))) {
	  hash_insert(modelP->hashtableP,
		      (VOID *) vocabName(modelP->vocabP,erg),
		      (VOID *) 1); 
	}

	read_string(fp,teststring);
	modelP->classes[erg].class = VocabIndex(teststring,modelP);
	if(modelP->classes[erg].class==NO_WORD) {
	  FATAL("Non-vocabulary class %s\n",teststring);
	}
	modelP->classes[erg].penalty = read_float(fp);
      }
    }
    test = read_int(fp);
  }    


  if (test != 99999) {
    ERROR("checkval 99999 != %d",test);
    return (-1);
  }

  return (1);  
}
/*===========================================================================*/

static int lmWriteEmpty (char * myfile)
{
/* create a file with an empty nist-lm  */

  FILE * fp;
  
  if ((fp = fileOpen (myfile,"w")) == NULL) {
    ERROR("Cannot open file %s to build empty lm.\n", myfile);
    return (TCL_ERROR);
  }

  fprintf(fp,"\\data\\\n"
             "ngram 1=2\n"
             "ngram 2=2\n"
             "\n"
             "\\1-grams:\n"
             "-99.0 <s>   0.0\n"
             "-00.0 </s> -99.0\n"
             "\n"
             "\\2-grams:\n"
             "-00.0 <s> </s>\n"
             "-99.0 </s> <s>\n\n");

  fileClose(myfile, fp);
  return (TCL_OK);
}

/*===========================================================================*/
static int lmCompare (const void * a, const void * b)
{
  int * lm1 = *(int **) a;   int * lm2 = *(int **) b;

       if (lm1[0] < lm2[0]) return (-1); else if (lm1[0] > lm2[0]) return (1);
  else if (lm1[1] < lm2[1]) return (-1); else if (lm1[1] > lm2[1]) return (1);
  else if (lm1[2] < lm2[2]) return (-1); else if (lm1[2] > lm2[2]) return (1);
  else return (0);
}

/*===========================================================================*/

static int lmWriteFromHypoList (char * myfile, HypoList * hypoListP, 
                                Vocab * vocabP)
{
/* create a file called custom.nist to be read by lmRead */

  FILE * fp;
  int hypoX, wordX, spaceN =0;
  
  int unigramN, bigramN, trigramN;
  int ** lmM;
  int (*lmCompareF)(const void *a, const void *b) = lmCompare;
  /* we will need a temporary file to store the built language model,
     try to open it before allocating any space for the new lm;
   */
  
  if ((fp = fileOpen (myfile,"w")) == NULL) {
    ERROR("Cannot open file %s to build custom made lm.\n", myfile);
    return TCL_ERROR;
  }

  /* -- first count them, then calloc space for them, then find them. -----  */
  
  for (hypoX = 0; hypoX < hypoListP->hypoN; hypoX ++) 
              spaceN += hypoListP->hypoPA[hypoX]->hwordN;

  if (spaceN <= 0) {
    ERROR("Cannot build language model from empty hypoListP\n", myfile);
    return TCL_ERROR;
  }
              
  lmM    =  (int **) calloc (spaceN, sizeof (int *));
  for (wordX = 0; wordX < spaceN; wordX ++) 
    lmM[wordX] = (int *) calloc (3, sizeof (int));
  
  spaceN = 0;                                                            
  for (hypoX = 0; hypoX < hypoListP->hypoN; hypoX ++) {
    Hypo * hypoP = hypoListP->hypoPA[hypoX];
    int  wordN = hypoP->hwordN;
    for (wordX = 0; wordX < wordN; wordX ++) {
       lmM[spaceN][0]  = hypoP->hwordA[wordX].vocabX;
       if (wordX < (wordN -1)) lmM[spaceN][1]   = hypoP->hwordA[wordX+1].vocabX;
                       else lmM[spaceN][1] = lmM[spaceN][0];
       if (wordX < (wordN -2)) lmM[spaceN][2]  = hypoP->hwordA[wordX+2].vocabX;
                       else lmM[spaceN][2] = lmM[spaceN][1];
       spaceN ++;
    }
  }

  /* -- now sort those tripples and print uni-, bi- and trigrams to file. -- */
  
  qsort ( (void *) lmM, spaceN, sizeof (int*), lmCompareF);
  
  unigramN = 1; bigramN = 1; trigramN =1;
  for (wordX = 1; wordX < spaceN; wordX ++) {
    if (lmM[wordX][0] != lmM[wordX-1][0]) { unigramN++;bigramN++;trigramN ++;}
    else if (lmM[wordX][1] != lmM[wordX-1][1]) { bigramN ++; trigramN ++;}
    else if (lmM[wordX][2] != lmM[wordX-1][2]) { trigramN ++;}
  }
  
  /* -- header and unigrams -------------------------------------------------*/
    
  fprintf(fp,"\\data\\\nngram 1=%d\nngram 2=%d\nngram 3=%d\n\n\\1-grams:\n",
                                unigramN,   bigramN,    trigramN);

  fprintf (fp,"-99.9 %s -99.9\n",
              vocabName( vocabP,lmM[0][0]));       
  for (wordX = 1; wordX < spaceN; wordX ++) {
    if (lmM[wordX][0] != lmM[wordX-1][0]) 
      fprintf (fp,"-99.9 %s -99.9\n", 
                   vocabName(vocabP,lmM[wordX][0]));
  }

  /* -- bigrams -------------------------------------------------------------*/
  
  fprintf (fp,"\n\\2-grams:\n-99.9 %s %s -99.9\n",
               vocabName( vocabP,lmM[0][0]),
               vocabName( vocabP,lmM[0][1]));       
  for (wordX = 1; wordX < spaceN; wordX ++) 
    if ((lmM[wordX][0] != lmM[wordX-1][0]) ||
        (lmM[wordX][1] != lmM[wordX-1][1]))
       fprintf (fp,"-99.9 %s %s -99.9\n", 
                    vocabName( vocabP,lmM[wordX][0]),
                    vocabName( vocabP,lmM[wordX][1]));  
                                          
  /* -- trigrams ------------------------------------------------------------*/
  
  fprintf(fp,"\n\\3-grams:\n");
  
  fprintf (fp,"-0.0 %s %s %s\n", 
               vocabName( vocabP,lmM[0][0]),
               vocabName( vocabP,lmM[0][1]),      
               vocabName( vocabP,lmM[0][2]));
  for (wordX = 1; wordX < spaceN; wordX ++) 
    if ((lmM[wordX][0] != lmM[wordX-1][0]) ||
        (lmM[wordX][1] != lmM[wordX-1][1]) ||
        (lmM[wordX][2] != lmM[wordX-1][2]))
        fprintf (fp,"-0.0 %s %s %s\n", 
                     vocabName( vocabP,lmM[wordX][0]),
                     vocabName( vocabP,lmM[wordX][1]),      
                     vocabName( vocabP,lmM[wordX][2]));    
                 
  fprintf(fp,"\n\\end\\\n");

  fileClose(myfile, fp);
  for (wordX = 0; wordX < spaceN; wordX ++)   free(lmM[wordX]);
  free(lmM);
  return TCL_OK;
}
/*===========================================================================*/

static int lmConfigureItf( ClientData cd, char *var, char *val)
{
  LmStruct* modelP = (LmStruct*)cd;
  if (! modelP)  modelP = &lmDefault;

  if (! var) {

    ITFCFG(lmConfigureItf,cd,"name");
    ITFCFG(lmConfigureItf,cd,"useN");
    ITFCFG(lmConfigureItf,cd,"vocab");
    ITFCFG(lmConfigureItf,cd,"weight");
    ITFCFG(lmConfigureItf,cd,"penalty");
    ITFCFG(lmConfigureItf,cd,"lp");
    ITFCFG(lmConfigureItf,cd,"lz");
    ITFCFG(lmConfigureItf,cd,"zerogram");
    ITFCFG(lmConfigureItf,cd,"utterance_context");
    ITFCFG(lmConfigureItf,cd,"bg_seg_sz");
    ITFCFG(lmConfigureItf,cd,"log_bg_seg_sz");
    if(modelP->cachelm) {
      ITFCFG(lmConfigureItf,cd,"absolut_discount");
      ITFCFG(lmConfigureItf,cd,"selective_absolut_discount");
    }

    /* CHANGE by bernhard */
    ITFCFG(lmConfigureItf,cd,"pre_context1X");
    ITFCFG(lmConfigureItf,cd,"pre_context2X");
    ITFCFG(lmConfigureItf,cd,"post_context1X");
    ITFCFG(lmConfigureItf,cd,"post_context2X");

    return TCL_OK;
  }
  ITFCFG_CharPtr( var,val,"name",    modelP->name,             0);
  ITFCFG_Int    ( var,val,"useN",    modelP->useN,             1);
  ITFCFG_Object ( var,val,"vocab",   modelP->vocabP,name,Vocab,1);
  ITFCFG_Float  ( var,val,"lp",      modelP->lp,               0);
  ITFCFG_Float  ( var,val,"lz",      modelP->lz,               0);
  ITFCFG_Float  ( var,val,"weight",  modelP->lw,               1);
  ITFCFG_Float  ( var,val,"penalty", modelP->log_wip,          1);
  ITFCFG_Float  ( var,val,"zerogram",modelP->zerogram,         1);
  ITFCFG_IArray ( var,val,"utterance_context", (&(modelP->utterance_context)), modelP->vocabP, 1);
  ITFCFG_Int(    var,val,"bg_seg_sz", modelP->bg_seg_sz,        (modelP!=&lmDefault));
  ITFCFG_Int(    var,val,"log_bg_seg_sz", modelP->log_bg_seg_sz,    (modelP!=&lmDefault));
  if(modelP->cachelm) {
    ITFCFG_Float    ( var,val,"absolut_discount",   modelP->cachelm->mono_discount,             0);
    ITFCFG_Float    ( var,val,"selective_absolut_discount",    modelP->cachelm->bi_discount,             0);
  }
  /* CHANGE by bernhard */
  ITFCFG_Int    ( var,val,"pre_context1X", modelP->pre_context1X, 0);
  ITFCFG_Int    ( var,val,"pre_context2X", modelP->pre_context2X, 0);
  ITFCFG_Int    ( var,val,"post_context1X", modelP->post_context1X, 0);
  ITFCFG_Int    ( var,val,"post_context2X", modelP->post_context2X, 0);

  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}

/*===========================================================================*/
int lmScoreItf( ClientData cd, int argc, char * argv[]);

Method lmMethod[] = 
{ 
  { "puts",      lmPutsItf,          "write out info about lm in tcl-format"},
  { "score",     lmScoreItf,         "get language model scores"}, 
  { "scoreOld",  lmScoreOldItf,      "get language model scores (outdated version -- 3.2)"}, 
  { "debug",     lmDebugItf,         "show internal data structures "},
  { "activate",  lmSetCurrentItf,    "make this lm the current one"},
  { "cachelines",lmSetCacheLinesItf, "change number of cachelines"},
  { "utterance_context",lmSetUtteranceContextItf, "change current utterance_context"},
  {  NULL,       NULL,                NULL} 
};


TypeInfo LmInfo =  { "Lm",        /* name of type                            */
		   0,             /* size of type                            */
		   -1,            /* number of parameters                    */
		   lmMethod,      /* set of functions that work on this type */
		   lmCreateItf,   /* creator function                        */
		   lmDestroyItf,  /* destructor function                     */
                   lmConfigureItf,/* configure function                      */
		   NULL,          /* subtype accessing function              */
		   NULL,

	       "The object Lm contains a language model.\n" };

#include "lm.h"

int    lmIndex( struct lm_s *lm, char* name) {
  int ret = vocabIndex( lm->vocabP, name);
  if(ret<0) {
    if(strcmp("<s>",name)==0)
      return vocabIndex( lm->vocabP, "(");
    if(strcmp("</s>",name)==0)
      return vocabIndex( lm->vocabP, ")");
  }
  return ret;
}

int    lmNameN( struct lm_s *lm) {
  return vocabNumber( lm->vocabP);
}

char*  lmName( struct lm_s *lm , int i) {
  if(lm->vocabP->startWordX==i) return "<s>";
  if(lm->vocabP->finishWordX==i) return "</s>";

  return vocabName( lm->vocabP, i);
}

float lmScoreLocal(struct lm_s *lmP, int* w, int n, int i)
{
  float prob = 0.0;
  int   j;

  if  ( i < 0) i = 0;
  for ( j = i; j < n; j++) {
    if (      j > 1) 
      prob += lm_TgScore1 (lmP, w[j-2], w[j-1], w[j]);
    else if ( j > 0)
      prob += lm_BgScore1(lmP, w[j-1], w[j]);
    else
      prob += lm_UgScore1(lmP, w[j]);
  }
  return lmP->lz * prob + (n-i) * lmP->lp;
}


int lmScoreItf( ClientData cd, int argc, char * argv[])
{
  struct lm_s *   lmP    = (struct lm_s *) cd;
  IArray    warray = {NULL, 0};
  int       ac     =  argc-1;
  int       i      =  0;

  if (itfParseArgv( argv[0], &ac,  argv+1,  0,
     "<word sequence>", ARGV_LIARRAY, NULL, &warray, 
     &(lmP->vocabP->vwordList), "sequence of words", 
     "-idx", ARGV_INT, NULL, &i, NULL, "index of cond. prob.",
     NULL) != TCL_OK) {
    if ( warray.itemA) free( warray.itemA);
    return TCL_ERROR;
  }

  itfAppendElement( "%f", lmScoreLocal(lmP, warray.itemA, warray.itemN, i));

  if ( warray.itemA) free( warray.itemA);
  return TCL_OK;
} 



char * lmNextFrame(struct lm_s * modelP) { lm_NextFrame2 (modelP); return ""; }
char * lmReset(struct lm_s * modelP) { lm_CacheReset1 (modelP); return ""; }


static LM LmLM = 
       { "Lm", &LmInfo, &LmInfo,
         (LMIndexFct*)    lmIndex,
         (LMNameFct *)    lmName,
	 (LMNameNFct*)    lmNameN,
         (LMScoreFct*)    lmScoreLocal,
         (LMNextFrameFct*)lmNextFrame,
         (LMResetFct*)    lmReset,
       NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };

int Lm_Init ( )
{
  if (lmInitialized) return (TCL_OK);
  itfNewType ( &LmInfo );
  lmNewType(  &LmLM);
  lmInitialized = 1;
  lmDefault.bg_seg_sz = BG_SEG_SZ;
  lmDefault.log_bg_seg_sz = LOG_BG_SEG_SZ;
  return (TCL_OK);
}
