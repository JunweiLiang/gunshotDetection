/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Bigram and Trigram Language Models
               ------------------------------------------------------------
               
    Author  :  Monika Woszczyna
    Date    :  $Id: lm_3g.h 700 2000-11-14 12:35:27Z janus $
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
    Revision 3.7  2000/11/14 12:29:35  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 3.6.30.1  2000/11/06 10:50:34  janus
    Made changes to enable compilation under -Wall.

    Revision 3.6  2000/08/27 15:31:21  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.5.4.1  2000/08/25 22:19:44  jmcd
    Just checking.

    Revision 3.5  1998/06/11 14:31:40  kries
    small adaptations

 * Revision 3.4  1998/05/26  18:27:23  kries
 * adaptation to new language model interface
 *
 * Revision 3.3  1997/07/18  17:57:51  monika
 * gcc-clean
 *
    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.10  1996/11/30  20:24:31  kries
    Segment Size can be swithout recompilation
    Class based adaptation

    Revision 1.9  1996/08/15  07:14:51  kries
    linear interpolation enhanced, cache model, stubs for trigger pairs,
    class model enhanced

    Revision 1.8  1996/07/03  13:19:26  monika
    changes to work with 64K vocabulary

    Revision 1.7  96/03/24  05:04:23  05:04:23  kries (Klaus Ries)
    added stub for lattice module resetting the long distance context
    
    Revision 1.6  1996/01/28  18:27:59  kries
    fix possible errors at begin of sentence
    simple interpolation scheme

    Revision 1.5  1996/01/25  21:41:47  kries
    extension of file-formats (including binary) and functionality
    to support a separate unigram distribution and class based model
    all old files should work as usual

    Revision 1.4  1996/01/14  14:19:30  monika
    always give lm-pointer when calling lm routines

    Revision 1.3  1995/08/10  08:00:44  finkem
    *** empty log message ***

    Revision 1.2  1995/08/03  14:42:23  finkem
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

  
   ========================================================================= */

#ifdef __cplusplus
extern "C" {
#endif


#ifndef _lm_3g
#define _lm_3g

#include <assert.h>
#include "common.h"
#include "hash_lm_3g.h"
#include "vocab.h"
#include "array.h"

typedef struct unigram_s {
    int   wid;		/* vocab word-id */
    float prob1;
    float bo_wt1;
    int   bigrams;	/* index of 1st entry in LM_STRUCT.bigrams[] */
} UnigramItem;

/*
 * The idea here is, that a FLOAT has 4 bytes and a short only two.
 * This is why probabilities are given as a short that is an index into a
 * table of FLOATS. As the probabilities are computed from discrete counts,
 * the number of different probabilities is *much* smaller than the number of
 * bigrams.
 * Also a POINTER to a trigram-structure would be 4 bytes (8 on the alphas)
 * while we really only have space for 2 bytes. But there are more than
 * 64K trigrams so they're not easily indexed by a 16Bit value.
 * This is why we keep many trigram-tables one for all
 * BG_SEG_SIZ bigrams in a way that each table is smaller than 64K.
 *********************************************************************/

/* chosen so that #trigram/segment <= 2**16 */


#define BG_SEG_SZ	512
#define LOG_BG_SEG_SZ	9


/*       For larger models this setting might be useful 
         This was standard for SWB

#define BG_SEG_SZ	(128)	
#define LOG_BG_SEG_SZ	(7)

*/



/* ------------------------------------------------------------------------
    CacheLm
   ------------------------------------------------------------------------ */



typedef struct cache_s {
  float mono_discount, bi_discount;
  int   window,context;
  int   vocab_size;
  int   bos,eos;
  int   effective_vocab_size;
  int   selection;
  int * monogram;
  int * selective;
  int	mono_sum, selective_sum;
  int   mono_incache, selective_incache;
  int * nocache;
#if defined(TRIGGER)
  int   trigger;
#endif
} CacheLm;



/* ------------------------------------------------------------------------
    M-gram backoff model Lm
   ------------------------------------------------------------------------ */

typedef struct bigram_s {
    unsigned short wid;	        /* vocab word-id */
    unsigned short prob2;	/* index into array of actual bigram probs */
    unsigned short bo_wt2;	/* index into array of actual bigram backoff wts */
    unsigned short trigrams;	/* index of 1st entry in LM_STRUCT.trigrams[],
			   RELATIVE TO its segment base (see above) */
} BigramItem3g;

/*
 * As with bigrams, trigram prob info kept in a separate table for conserving
 * memory space.
 */
typedef struct trigram_s {
    unsigned short wid;	/* vocab word-id */
    unsigned short prob3;	/* index into array of actual trigram probs */
} TrigramItem3g;

typedef struct TgCacheInfo_s {
    int   w1;       /* w2 of the bigram is known from other sources */
    int   first_tg; /* index of first trigram entry */
    int   last_tg;  /* index of last trigram entry; < first_tg if no 3g exists*/
    float bo_wt;    /* 3g->2g backoff wt for <w1,w2> */
    int   line;     /* trigram cache line, if any */
    struct TgCacheInfo_s *next;  /* with same w2 as this entry */
} TgCacheInfo;

typedef struct BgCacheInfo_s {
    int   line;             /* bigram cache line, if any */
    TgCacheInfo *tgInfo;    /* trigram info for all bg whose w2 = this word */
} BgCacheInfo;

typedef struct CacheInfo_s {
    int   w1;                /* line inactive if -1 */
    int   w2;                /* bigram probs line if -1 (and w1 > -1) */
    int   fr_used;           /* frame in which last used */
    int   size;              /* #valid bg or tg entries in cache line */
    float *prob;             /* probs cache line */
    TgCacheInfo *tgInfo;     /* trigrams info for a trigrams cache line */
} CacheInfo;

typedef struct ClassInfo_s {
    int   class;             /* class of the current word */
    float penalty;           /* -log_10(p(word|class)) */
} ClassInfo;

/*
 * The language model.
 * Bigrams for each unigram are contiguous.  Bigrams for unigram i+1 come
 * immediately after bigrams for unigram i.  So, no need for a separate count
 * of bigrams/unigram; it is enough to know the 1st bigram for each unigram.
 * But an extra dummy unigram entry needed at the end to terminate the last
 * real entry.
 * Similarly, trigrams for each bigram are contiguous and trigrams for bigram
 * i+1 come immediately after trigrams for bigram i, and an extra dummy bigram
 * entry is required at the end.
 ***************************************************************************/
typedef struct lm_s {
    char * name;
    int    useN;

    UnigramItem *unigrams;
    BigramItem3g  *bigrams;	/* for entire LM */
    TrigramItem3g *trigrams;/* for entire LM */
    float *prob2;	/* table of actual bigram probs */
    int   n_prob2;	/* prob2 size */
    float *bo_wt2;	/* table of actual bigram backoff weights */
    int   n_bo_wt2;	/* bo_wt2 size */
    float *prob3;	/* table of actual trigram probs */
    int   n_prob3;	/* prob3 size */
    int   *tseg_base;	/* tseg_base[i>>LOG_BG_SEG_SZ] = index of 1st
			   trigram for bigram segment (i>>LOG_BG_SEG_SZ) */
    int   *vocabwid_map; /* lexicon word-id to ILM word-id map */
    int   max_ucount;	/* To which ucount can grow with dynamic addition of words */
    int   ucount;	/* #unigrams in LM */
    int   bcount;	/* #bigrams in entire LM */
    int   tcount;	/* #trigrams in entire LM */
    int   vocab_size;	/* #words in lexicon */

    float lw;          /* language weight */
    float zerogram;    /* zerogram logprob */
    float log_wip;     /* word insertion penalty */
    float lz, lp;

    Vocab * vocabP;
    
    HashtableP hashtableP; /* hash table defines which words are in lm and their wid */
    char ** wordSA;

    /*----------------------------*/
    BgCacheInfo    *bg_info;
    CacheInfo      *cache_info;
    int cacheLineN;
    int currentFrame;


    /*---------------------------*/

    ClassInfo * classes; /* Implementation of class-based lm */
    float     * separate_unigrams; /* If backoff distribution is skewed as in Kneser/Ney */
    struct lm_s * follower; /* LM chain for interpolation */
    struct lm_s * adapt; /* Minor corpus for adaptation */

    CacheLm   * cachelm;
    int         cacheWindow;
    float       (*lm_cache_score) (struct lm_s * mP, int w);

    IArray      utterance_context; /* the utterance previous to the current utterance */
    IArray      interpol_names;    /* names of the interpolated models in the NIST file */


    /*---------------------------*/

    int bg_seg_sz, log_bg_seg_sz;  /* flexible partitioning */

    /*---------------------------*/

    /* OPTIONAL: words to be used as context in trigrams with "(" :
       trigram { ??? (  v } -> { pre_context2X pre_context1X v }
       trigram { ( v w  } -> { pre_context1X v w }        
       NOTE: Setting of pre_context1X!=regular startWordX will 
       activate this feature */

    int  pre_context1X, pre_context2X;   

    /* OPTIONAL: words to be used as context in trigrams with ")" :
       trigram { v w ) } -> { v w post_context1X } & 
       { v post_context1X post_context2X}
       NOTE: Setting of pre_context1X!=regular finishWordX will
       activate this feature */

    int  post_context1X, post_context2X;


    /* Pointers to functions used by this LM to calculate the actual scores */
    /* Macros are used to call them                                         */

    float    (*lm_UgScoreProc) ( struct lm_s * mP, int w1);     /* Macro: lm_UgScore(mP,w1) */
    float    (*lm_BUgScoreProc) ( struct lm_s * mP, int w1);    /* Macro: lm_BUgScore(p,w1) */
    float    (*lm_BgScoreProc) ( struct lm_s * mP, int w1, int w2 );
    float    (*lm_TgScoreProc) ( struct lm_s * mP, int w1, int w2, int w3 );

        
} LmStruct;

#define UG_WID(m,u)		((m)->unigrams[u].wid)
#define UG_PROB_F(m,u)		((m)->unigrams[u].prob1)
#define UG_BO_WT_F(m,u)		((m)->unigrams[u].bo_wt1)
#define FIRST_BG(m,u)		((m)->unigrams[u].bigrams)
#define LAST_BG(m,u)		(FIRST_BG((m),(u)+1)-1)
#define VOCAB2LM_WID(m,d)	((m)->vocabwid_map[d])

#define BG_WID(m,b)		((m)->bigrams[b].wid)
#define BG_PROB_F(m,b)		((m)->prob2[(m)->bigrams[b].prob2])
#define BG_BO_WT_F(m,b)		((m)->bo_wt2[(m)->bigrams[b].bo_wt2])
#define TSEG_BASE(m,b)		((m)->tseg_base[(b)>>LOG_BG_SEG_SZ])
#define FIRST_TG(m,b)		(TSEG_BASE((m),(b))+((m)->bigrams[b].trigrams))
#define LAST_TG(m,b)		(FIRST_TG((m),(b)+1)-1)

#define TG_WID(m,t)		((m)->trigrams[t].wid)
#define TG_PROB_F(m,t)		((m)->prob3[(m)->trigrams[t].prob3])

#define NO_WORD_3G              65535 /* not a word defined in the vocabulary (vocabX) */
#define UNKNOWN_WORD            65533 /* not a word defined in the lm (lm-wid) */

/* ----Interface--------------------------------------- */
extern void	        lm_SetStartSym ( char *sym );
extern void	        lm_SetEndSym ( char *sym );
extern LmStruct *	lm_LangModelRead ( char *file, char *name,
					   float lw, float uw, float wip );
extern int  	        lm_DeleteByName ( char *name );
extern int  	        lm_SetCurrentByName ( char *name );
extern LmStruct *	lmGetCurrent ( void );
extern float		lm_GetCurrentWip ( void );
extern float		lm_GetCurrentLw ( void );
extern LmStruct *	lm_GetByName ( char *name );
extern char *		lm_GetCurrentName ( void );
extern int		lm_GetNumberOfLms ( void );
extern int		lm_Init( char mode );
extern int		lm_Flag( char *flag, char *val );   
extern int		lm_assign( char *var, char* val);
extern int		lm_print( char *var);
extern int		lm_command( char *line);
extern int		lm_VocabwdInCurrent ( CHAR* wstr );
extern int		lm_VocabwdInCurrent2 ( LmStruct *mP, CHAR* wstr );

extern void		lm_ResetContext(LmStruct* modelP, void * bestP);

extern int		flag_lm( char *flag, char *val ); /* do use lm_Flag instead */
extern int		init_lm( char mode );             /* do use lm_Init instead */

extern			int Lm_Init ( );

#endif

#ifdef __cplusplus
}
#endif
