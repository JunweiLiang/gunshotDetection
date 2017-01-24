/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Codebook
               ------------------------------------------------------------

    Author  :  Michael Finke & Ivica Rogina
    Module  :  codebook.h
    Date    :  $Id: codebook.h 3262 2010-06-02 17:04:01Z metze $
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
    Revision 3.15  2007/02/22 16:24:24  fuegen
    minor changes in Gaussian Selection code by Thilo Koehler

    Revision 3.14  2007/02/21 12:58:18  fuegen
    added Gaussian Selection code from Thilo Koehler

    Revision 3.13  2004/10/27 11:43:41  metze
    Fixed '-bmem' issues with cbs/dss

    Revision 3.12  2003/08/14 11:20:13  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.11.4.8  2003/02/05 09:00:46  soltau
    Added MMIE training

    Revision 3.11.4.7  2002/11/20 13:24:50  soltau
    added declarations for CBX

    Revision 3.11.4.6  2002/03/18 17:44:19  soltau
    cleaned pre-declarations

    Revision 3.11.4.5  2002/02/01 14:17:20  fuegen
    moved stypes.h into slimits.h

    Revision 3.11.4.4  2001/07/06 13:11:58  soltau
    Changed compress and scoreFct handling

    Revision 3.11.4.3  2001/07/04 18:08:24  soltau
    Changed compression routines

    Revision 3.11.4.2  2001/07/03 09:30:41  soltau
    support for compressed codebooks

    Revision 3.11.4.1  2001/06/26 19:29:32  soltau
    beautified/fixed a couple of ugly hacker routines

    Revision 3.6  1999/08/11 15:52:49  soltau
    SCQ and some other things

    Revision 3.5  1999/03/02 10:24:01  soltau
    *** empty log message ***

    Revision 3.4  1998/08/12 10:40:58  kemp
    added declaration for cbKullbackLeibler

 * Revision 3.3  1997/07/31  15:25:44  rogina
 * made code gcc -DFKIclean
 *
    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.27  96/11/14  09:30:53  fritsch
    Added flag to cb's, to prune exp() calls based on the size of the argument
    
    Revision 1.26  1996/09/27  08:32:35  fritsch
    added flag in Codebook to enable/disable BBI scoring.

    Revision 1.25  1996/09/24  15:19:27  fritsch
    Added the rdimN and defaultRdimN items to Codebook and CodebookSet
    in order to be able to evaluate scores in a dimensionality reduced space

    Revision 1.24  1996/07/08  14:54:01  kemp
    Added the cbfact item to the Cbfcfg cstrcucture

    Revision 1.23  1996/04/23  19:54:11  maier
    RUNON, ready

    Revision 1.22  1996/02/27  00:19:05  rogina
    added tying / typing of covars

    Revision 1.21  1996/01/16  21:14:48  rogina
    made reference vectors use matrices

    Revision 1.20  96/01/10  13:25:24  13:25:24  rogina (Ivica Rogina)
    removed sbuf stuff, will appear in extra module
    
    Revision 1.19  96/01/03  17:52:08  17:52:08  rogina (Ivica Rogina)
    added Cbcfg
    
    Revision 1.18  1995/12/27  16:03:16  rogina
    split off the map stuff into extra file cbmap.h

    Revision 1.17  95/12/06  19:39:08  19:39:08  rogina (Ivica Rogina)
    added datastructures and function predeclarions for
    feature extraction (what was called xs in Janus2)
    
    Revision 1.16  95/11/30  15:06:11  15:06:11  finkem (Michael Finke)
    CodebookMap revised and made working
    
    Revision 1.15  1995/11/25  20:26:40  finkem
    removed a couple of problems with the data structures of the
    accumulator (pointing back to codebooks)
    Removed a BUG in the accumulator read function, which did
    not add values but just read it !!!!!

    Revision 1.14  1995/11/23  08:00:00  finkem
    CodebookMap, set function and neural gas clustering added
    and a couple of functions cleaned up.

    Revision 1.13  1995/11/15  04:08:37  finkem
    Added BBI score computation

    Revision 1.12  1995/11/14  06:05:14  finkem
    movec covar function to covr.c

    Revision 1.11  1995/10/19  16:53:33  rogina
    *** empty log message ***

    Revision 1.10  1995/10/18  23:21:07  finkem
    cleaning up

    Revision 1.9  1995/09/23  18:17:23  finkem
    init and call of changed cache scoring routines

    Revision 1.8  1995/09/21  21:39:43  finkem
    Init?Deinit and default argument handling/initialization corrected

    Revision 1.7  1995/09/12  18:01:52  rogina
    *** empty log message ***

    Revision 1.6  1995/08/31  08:28:50  kemp
    *** empty log message ***

    Revision 1.5  1995/08/29  13:14:56  kemp
    *** empty log message ***

    Revision 1.4  1995/08/28  16:10:35  rogina
    *** empty log message ***

    Revision 1.3  1995/08/10  08:11:26  rogina
    *** empty log message ***

    Revision 1.2  1995/08/04  14:18:40  rogina
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#ifndef _codebook
#define _codebook

#include "list.h"
#include "rewrite.h"
#include "covar.h"
#include "feature.h"
#include "matrix.h"
#include "cbmap.h"
#include "bmem.h"

typedef struct CodebookSet_S CodebookSet;
#include "gs.h"

typedef struct CbWindowCache_S {
  int            frX; /* recent frame                   */
  FMatrix*      matP; /* score array                    */
  IMatrix*     baseP; /* base frame                     */
  unsigned char* mdA; /* minimum distance distribution  */
  int*           cbL; /* list of codebooks to compute   */
#ifdef SSE_OPT
  __m128**       ptA; /* pattern                        */
#else
  float**        ptA;
#endif
} CbWindowCache;

/* ========================================================================
    Forward declarations for BBI
   ======================================================================== */

/* These defines restrict the number of gaussias per codebook to UCHAR_MAX.
   If you need larger values, use the following setup.

typedef USHORT   CBX;
#define CBX_MAX  USHRT_MAX
*/
typedef UCHAR    CBX;
#define CBX_MAX  UCHAR_MAX


typedef struct BBITree_S     BBITree;
typedef struct LIST(BBITree) BBITreeList;

/* ========================================================================
    Codebook Configuration Structure
   ======================================================================== */

typedef struct {
  char    *name;         /* name of the configuration object    */
  int      useN;         /* number of references to this object */

  float    minRvCount;   /* minimal count for updating means    */
  float    minCvCount;   /* minimal count for covariance update */

  float    beta;         /* temperature for accumulation                   */
  float    momentum;     /* how much of the old parameters to be preserved */
  float    momentumCv;   /* same for covs, if < 0 => momentum is taken     */

  char     doAccumulate; /* 'y' = do accumulate, 'n' = don't accumulate            */
  char     doUpdate;     /* 'y' = do update, 'n' = don't update                    */
  char     method;       /* 'm' = maximum likelihood, 'l' = LVQ, 'L' = LVQ2        */
  float    weight;       /* multiplicative factor to mahalanobis distance          */
                         /* during scoring, allows to scale vs distribution weight */

  int      topN;         /* score computation using top how many vectors?    */
  int      rdimN;        /* score computation using top how many dimensions? */
  int      bbiOn;        /* flag to enable/disable BBI scoring               */
  float    expT;         /* threshold to toggle exp() function evaluation    */

  float   mergeThresh;   /* sample threshold for gaussian merging            */
  float   splitStep;     /* mean shift for spliting                          */
  double  rhoGlob;       /* The 'temperature'  Multivar Training             */

  float  E;              /* MMIE momentum term                               */
  float  H;              /* mixture weight between MLE and MMIE              */
  float  I;              /* I-smoothing                                      */


} Cbcfg;

/* ========================================================================
    Codebook Accumulator
   ======================================================================== */

typedef struct Codebook_S     Codebook;
typedef struct CodebookAccu_S CodebookAccu;

struct CodebookAccu_S {
  Codebook     *cb;         /* for which codebook are we accumulating       */
  int           subN;       /* how many sub-accumulators are we using       */

  double      **count;      /* training counts for each sub-accu/compoment  */
  DMatrix     **rv;         /* training means  for each sub-accu/compoment  */
  DCovMatrix  **sumOsq;     /* training sum of squares                      */

  FMatrix      *priorRV;    /* mean priors for MAP training                 */
  FMatrix      *priorCV;    /* covariance priors for MAP training           */

                            /* Multivar Training control parameters         */
  float         countSum;   /* The last normalization factor from cbsAccu   */
  double        score;      /* The last score computed in cbsAccu           */
  double        distortion; /* fmatrixDistortion of cb and data             */
  double        mindist;    /* minimum distance between 2 cb-vectors        */
  double        maxdist;    /* maximum distance between 2 cb-vectors        */

};


/* ========================================================================
    Codebook
   ======================================================================== */

struct Codebook_S {
  char         *name;   /* the name of the codebook                         */
  int           useN;

  int           refN;   /* number of vectors in the codebook                */
  int           refMax; /* max. number of components for spliting           */
  int           dimN;   /* dimensionality of the underlying feature space   */
  int           bbiX;   /* bbi tree index */
  int           bbiY;   /* bbi local codebook index */
  FMatrix      *rv;     /* rv->matPA[j][k] = k-th coeff of j-th vector      */
  FCovMatrix  **cv;     /* *cv[j] = covariance matrix of j-th vector        */
  CovType       type;   /* the default type of the covariance matrices      */
  float        *count;  /* the training counts for each reference vector    */
  int           featX;  /* feature space index used by the feature set      */

  float         pi;     /* the precalculated value of log((2 PI)^dimN)      */
  Cbcfg        *cfg;    /* structure containing configuration variables     */

  UCHAR         **rvC;   /* compressed mean vectors                         */ 
  UCHAR         **cvC;   /* compressed semi-tied radial covariance vectors  */ 
  float         *detC;   /* direct access to determinants                   */   
  int           featY;   /* compressed feature index                        */
 
  CodebookAccu* accu;    /* accumulator structure for training data accumulation */
  CodebookMap*  map;
};

/* ========================================================================
    CodebookSet
   ======================================================================== */

typedef struct LIST(Codebook) CodebookList;

struct CodebookSet_S {
  char*         name;     /* name of the codebook list                     */
  int           useN;     /* used by another object                        */

  CodebookList  list;     /* list of codebooks                             */
  BBITreeList   bbi;      /* list of BBI trees                             */
  GSClusterSet* gscsP;    /* Gaussian Selection Cluster Set                */
  RewriteSet*   rwsP;     /* pointer to a rewrite set                      */
  FeatureSet*   feat;     /* pointer to the current feature object         */
  int*          featXA;   /* array of used feature indices                 */
  int           featN;    /* number of used features (i.e. size of featXA) */

  int           comMode;  /* compress mode                                 */
  float         scaleRV;  /* scaling factor for compressed means's         */
  float         scaleCV;  /* scaling factor for compressed covar's         */
  float         offset;   /* offset  for compressed models                 */
  int*          x2y;      /* feature dimension mapper                      */

  int   defaultTopN;      /* score computation method                      */
  int   defaultRdimN;     /* reduced dimensionality                        */
  int   defaultBbiOn;     /* use bbi tree                                  */
  float defaultExpT;      /* triggering exp() evaluation                   */
  
  int   subIndex;         /* global sub-accumulator index                  */

  struct CbCache_S* cache;        

  struct CbWindowCache_S* swcP; /* sliding window cache */

  float       (*score)  (CodebookSet *cbsP,int cbX,float *dsVal, 
                         int frameX, float *mixVal);
  int          (*accu)  (CodebookSet *cbsP, int cbX,float *dsVal,
                         int    frameX,    float   factor, 
                         float* addCounts, float** subX);
  int         (*update) (CodebookSet *cbsP);

  char         commentChar;

  int        swc_queries; /* number of queries              */
  int           swc_hits; /* number of hits                 */  
  int          swc_width; /* width of cache                 */

  float*       gdft_offset;
  int*         gdft_frame2class;

};

typedef float CbsScoreFct  (CodebookSet *cbsP, int cbX, float *dsVal, int frameX);
typedef int   CbsAccuFct   (CodebookSet *cbsP, int cbX, float *dsVal, int frameX, 
                            float factor, float* addCounts, float** subA);
typedef int   CbsUpdateFct (CodebookSet *cbsP);


/* ========================================================================
    Module Interface
   ======================================================================== */

extern int    Codebook_Init    (void);

extern int    cbsAdd           (CodebookSet* cbsP, char* name, int refN, 
				int dimN, CovType type, int featX);
extern int    cbsFrameN        (CodebookSet* cbs, int* from, int* shift, int* ready);
extern char*  cbsName          (CodebookSet* cbs, int i);
extern double cbKullbackLeibler(Codebook *cb1P, float *distribution1, 
				Codebook *cb2P, float *distribution2);

extern int    cbMLE            (Codebook *cb, int subX);
extern int    cbUpdateConst    (Codebook *cb);
extern int    cbStepDiag       (Codebook *cb, int n, int m);
extern float  cbPlotFunc       (float x, float y);
extern int    cbAccuClear      (CodebookAccu* cba, int subX);

#endif
