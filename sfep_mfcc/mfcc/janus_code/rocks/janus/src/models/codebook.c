/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Codebook
               ------------------------------------------------------------

    Author  :  Michael Finke & Ivica Rogina
    Module  :  codebook.c
    Date    :  $Id: codebook.c 3416 2011-03-23 03:02:18Z metze $
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
    Revision 3.31  2007/04/03 12:38:18  fuegen
    reactivated access to ref of a codebook

    Revision 3.30  2007/02/22 16:24:24  fuegen
    minor changes in Gaussian Selection code by Thilo Koehler

    Revision 3.29  2007/02/21 16:21:52  fuegen
    some changes for the Gaussian Selection

    Revision 3.28  2007/02/21 12:58:19  fuegen
    added Gaussian Selection code from Thilo Koehler

    Revision 3.27  2006/11/10 10:16:06  fuegen
    merged changes from branch jtk-05-02-02-shajith
    - all modification related to MMIE training
    - first unverified functions for MPE training
    - modifications made by Shajith, Roger, Wilson, and Sebastian

    Revision 3.26  2005/10/12 13:36:17  metze
    Consistency

    Revision 3.25.8.1  2006/11/03 12:46:01  stueker
    Initial check in of MMIE from Shajith, Wilson, and Roger.

    Revision 3.25  2004/10/27 11:58:00  metze
    '-bmem' changes

    Revision 3.24  2004/09/02 17:14:50  metze
    Beautification

    Revision 3.23  2004/03/25 10:01:00  fuegen
    removed problematic characters out of cvs log
    messages, which produce code page problems

    Revision 3.22  2003/08/14 11:20:13  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.21.4.31  2003/08/07 12:42:14  metze
    Fixed list overflow

    Revision 3.21.4.30  2003/02/21 10:41:44  soltau
    minor fixes for MMIE

    Revision 3.21.4.29  2003/02/05 09:04:01  soltau
    Added MMIE training

    Revision 3.21.4.28  2002/12/13 08:45:08  soltau
    cbAccuMLE: fixed update for radial covariances

    Revision 3.21.4.27  2002/11/25 17:37:44  metze
    Made 256 Gaussians per Codebook possible

    Revision 3.21.4.26  2002/11/20 09:39:20  soltau
    cbSetup: check for CBX_MAX

    Revision 3.21.4.25  2002/11/04 14:18:56  metze
    BMEM argument typeinfo corrected

    Revision 3.21.4.24  2002/08/27 08:44:50  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 3.21.4.23  2002/08/01 13:42:33  metze
    Fixes for clean documentation.

    Revision 3.21.4.22  2002/07/18 12:21:05  soltau
    Removed malloc.h

    Revision 3.21.4.21  2002/06/26 11:57:56  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.21.4.20  2002/06/10 13:22:43  metze
    Checks for existing accus in update funcions

    Revision 3.21.4.19  2002/06/05 12:18:40  metze
    Faster accumulation

    Revision 3.21.4.18  2002/06/04 17:16:33  metze
    Bugfix in cbsAccu

    Revision 3.21.4.17  2002/06/04 12:43:05  metze
    Catch error in cbsAccu if accus don't exist

    Revision 3.21.4.16  2002/03/19 13:45:07  soltau
    cbsAccu: call cbAccuAllocate on demand

    Revision 3.21.4.15  2002/03/18 17:44:51  soltau
    allocate accumulators on demand, bmem support

    Revision 3.21.4.14  2002/02/28 11:46:04  soltau
    cbCopy: checking for rv and cv

    Revision 3.21.4.13  2002/02/27 15:15:51  soltau
    cleaned cbDealloc

    Revision 3.21.4.12  2002/02/27 08:54:25  metze
    Another error message

    Revision 3.21.4.11  2002/02/26 19:13:10  metze
    MOre informative output for CbCfg

    Revision 3.21.4.10  2001/11/23 08:24:53  metze
    Added Warning in cbsWrite

    Revision 3.21.4.9  2001/11/21 11:02:26  soltau
    allocate cb structures from cbAccess to provide a compatibility mode

    Revision 3.21.4.8  2001/10/16 11:52:58  soltau
    Introduced CBX for bbi

    Revision 3.21.4.7  2001/09/24 17:03:05  metze
    Added cbAllocItf to allocate a codebook (Fmatrix, ...)
    (Needed for neuralGas)

    Revision 3.21.4.6  2001/07/12 18:40:46  soltau
    Changed cbsCompress to support training with compressed features

    Revision 3.21.4.5  2001/07/09 14:38:26  soltau
    Added options resortFeat, trainMode in cbsCompress

    Revision 3.21.4.4  2001/07/06 13:10:39  soltau
    Changed compress and scoreFct handling

    Revision 3.21.4.3  2001/07/04 18:07:43  soltau
    Changed compression routines

    Revision 3.21.4.2  2001/07/03 09:31:08  soltau
    support for compressed codebooks

    Revision 3.21.4.1  2001/06/26 19:29:31  soltau
    beautified/fixed a couple of ugly hacker routines

    Revision 3.14  1999/08/11 15:52:25  soltau
    SVQ and some other things

    Revision 3.13  1999/03/01 17:34:08  soltau
    map update covariance

    Revision 3.12  1999/03/01 15:54:28  soltau
    MPA update mean, ML update covariances

    Revision 3.11  1999/02/20 11:07:03  soltau
    made map working

    Revision 3.10  1998/10/21 14:20:12  soltau
    map: first try

    Revision 3.9  1998/08/12 09:24:39  kemp
    added method 'add' to add two one-dimensional codebooks
    added cbKullbackLeibler
    made full covariance matrix estimation work

 * Revision 3.8  1998/03/10  09:58:48  kemp
 * Added codebook method update to update a single codebook
 * Added codebook method accuMatrix to fill the codebook accumulator from
 * a FMatrix. This allows covariance initialization.
 *
 * Revision 3.7  1997/10/01  16:59:45  koll
 * fixed bug: covar-type not set while loading description file
 *
    Revision 3.6  1997/08/27 15:53:42  rogina
    removed predeclaration of gethostname (wouldn't compile on HPs/PCs)

    Revision 3.5  1997/08/18 10:49:39  rogina
    made "case COV_NO: { }" out of "case COV_NO:" because cc doesn't like empty cases while gcc does

    Revision 3.4  1997/07/31 15:24:42  rogina
    made code gcc -DFKIclean

    Revision 3.3  1997/07/23 07:12:51  fritsch
    minor changes in BBI flag handling + extensions for BBI on radial covariance codebooks

 * Revision 3.2  1997/06/17  17:41:43  westphal
 * *** empty log message ***
 *
    Revision 1.59  96/11/14  09:31:51  fritsch
    Added flag to cb's, to prune exp() calls based on the size of the argument
    
    Revision 1.58  1996/10/08  09:15:20  fritsch
    added top-N (N > 1) scoring capability

    Revision 1.57  1996/09/27  08:33:01  fritsch
    added flag in Codebook to enable/disable BBI scoring.

    Revision 1.56  1996/09/26  17:21:46  fritsch
    changed default scoring function, added switching of scoring functions in cbsSet()

    Revision 1.55  1996/09/24  17:14:52  fritsch
    changed method propagate to set (for CodebookSet)

    Revision 1.54  1996/09/24  15:20:36  fritsch
    Added the rdimN and defaultRdimN items to Codebook and CodebookSet
    in order to be able to evaluate scores in a dimensionality reduced space

    Revision 1.53  1996/09/05  14:13:50  rogina
    the last bugfix (when freeing Accumulators) wasn't thorough enough

    Revision 1.52  1996/09/04  15:34:42  rogina
    fixed some bugs, added variance shifting command

    Revision 1.51  96/07/08  14:53:28  14:53:28  kemp (Thomas Kemp)
    Added a cb-weight to the Cbcfg object to allow weighting of
    codebooks vs distributions
    
    Revision 1.50  1996/05/07  19:27:32  rogina
    cbMap will just return NULL if it gets an empty map

    Revision 1.49  1996/04/23  19:54:11  maier
    RUNON, ready

    Revision 1.48  1996/02/27  00:18:25  rogina
    added tying / typing of covars

    Revision 1.47  1996/02/13  05:10:54  finkem
    made add function prevent double entries to the list

    Revision 1.46  1996/02/10  16:44:37  finkem
    checked if accu exists before clearing it.

    Revision 1.45  1996/01/25  10:16:54  kemp
    Removed bug in loading codebook accus.

    Revision 1.44  1996/01/21  03:12:26  finkem
    removed problem with access of mat in codebook

    Revision 1.43  1996/01/18  21:37:04  finkem
    moved neuralGas to FMatrix

    Revision 1.42  1996/01/18  21:32:56  rogina
    added diff of two codebooks display and floor

    Revision 1.41  96/01/18  14:41:32  14:41:32  rogina (Ivica Rogina)
    fixed score computation, added ascii map output
    for plot3d stuff
    
    Revision 1.40  96/01/17  17:19:29  17:19:29  rogina (Ivica Rogina)
    changed parameter handling of plot3d, added couple of additional arguments
    
    Revision 1.39  96/01/16  21:14:29  21:14:29  rogina (Ivica Rogina)
    made reference vectors use matrices
    
    Revision 1.38  96/01/12  15:39:53  15:39:53  finkem (Michael Finke)
    removed cbsRead function
    
    Revision 1.37  1996/01/10  13:25:06  rogina
    removed sbuf stuff, will appear in extra module

    Revision 1.36  96/01/02  18:07:49  18:07:49  rogina (Ivica Rogina)
    fixed some minor bugs (fp in cbFlushSbuf, n++ in cbsRead),
    introduced Cbcfg class, modified the usage of momentum and
    the training selection mechanism
    
    Revision 1.35  1995/12/27  16:02:52  rogina
    split off the map stuff into extra file cbmap.c

    Revision 1.34  95/12/17  16:58:44  16:58:44  finkem (Michael Finke)
    fixed problem with sub space accumulation if refFactor == 0
    
    Revision 1.33  1995/12/12  18:37:59  rogina
    changed name of fmatrixDump function call to fmatrixDAppend

    Revision 1.32  1995/12/06  19:32:04  rogina
    added datastructures and functions for doing
    feature extraction (what was called xs in Janus2)

    Revision 1.31  95/12/06  02:27:31  02:27:31  finkem (Michael Finke)
    init LDA
    
    Revision 1.30  1995/12/04  04:43:09  finkem
    removed bug in split criterion and added split itf functions
    revised use of momentum for covariances matrices

    Revision 1.29  1995/11/30  15:06:11  finkem
    CodebookMap revised and made working

    Revision 1.28  1995/11/25  20:26:40  finkem
    removed a couple of problems with the data structures of the
    accumulator (pointing back to codebooks)
    Removed a BUG in the accumulator read function, which did
    not add values but just read it !!!!!

    Revision 1.27  1995/11/23  08:00:00  finkem
    CodebookMap, set function and neural gas clustering added
    and a couple of functions cleaned up.

    Revision 1.26  1995/11/17  22:25:42  finkem
    removed realloc bug in cbsAccu

    Revision 1.25  1995/11/15  04:08:37  finkem
    Added BBI score computation

    Revision 1.24  1995/11/14  06:05:14  finkem
    movec covar function to covr.c

    Revision 1.23  1995/11/01  01:17:39  finkem
    another bug in AddItf

    Revision 1.22  1995/11/01  01:12:25  finkem
    fixed error in addItf

    Revision 1.21  1995/10/18  23:21:07  finkem
    cleaning up / removing BUGSSSSSS...

    Revision 1.20  1995/10/16  17:50:22  torsten
    Removed annoying UPDATING message

    Revision 1.19  1995/10/06  01:09:22  torsten
    added cbs accu functions

    Revision 1.18  1995/10/04  23:38:47  torsten
    *** empty log message ***

    Revision 1.17  1995/09/23  18:17:23  finkem
    init and call of changed cache scoring routines

    Revision 1.16  1995/09/21  21:39:43  finkem
    Init?Deinit and default argument handling/initialization corrected

    Revision 1.15  1995/09/12  18:01:16  rogina
    *** empty log message ***

    Revision 1.14  1995/09/05  16:03:25  kemp
    *** empty log message ***

    Revision 1.13  1995/09/05  15:13:13  kemp
    *** empty log message ***

    Revision 1.12  1995/09/05  15:11:33  kemp
    *** empty log message ***

    Revision 1.11  1995/08/31  08:28:54  kemp
    *** empty log message ***

    Revision 1.10  1995/08/29  15:35:05  finkem
    *** empty log message ***

    Revision 1.9  1995/08/29  13:15:01  kemp
    *** empty log message ***

    Revision 1.8  1995/08/28  16:10:32  rogina
    *** empty log message ***

    Revision 1.7  1995/08/21  09:58:47  rogina
    *** empty log message ***

    Revision 1.6  1995/08/17  17:38:52  rogina
    *** empty log message ***

    Revision 1.5  1995/08/14  12:21:35  finkem
    *** empty log message ***

    Revision 1.4  1995/08/10  08:11:22  rogina
    *** empty log message ***

    Revision 1.3  1995/08/04  14:16:38  rogina
    *** empty log message ***

    Revision 1.2  1995/07/26  12:29:35  rogina
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#include <sys/types.h>

#include <limits.h>
#include "common.h"
#include "itf.h"
#include "codebook.h"
#include "mach_ind_io.h"
#include "score.h"

#include "array.h"
#include "bbi.h"

char codebookRCSVersion[]= 
           "@(#)1$Id: codebook.c 3416 2011-03-23 03:02:18Z metze $";

/* ------------------------------------------------------------------------
    Static objects
   ------------------------------------------------------------------------ */

static CodebookSet cbsDefault;
static Cbcfg       defaultCbcfg;

static BMem*       _bmemCBSP = NULL;
int                _bmemCBSN = 0;

/* ------------------------------------------------------------------------
    Forward Declarations
   ------------------------------------------------------------------------ */

extern TypeInfo codebookInfo;
extern TypeInfo codebookSetInfo;

#define square(X) ((X)*(X))

#define PI 3.14159265358979323846264338327950288419716939937510582097494459230

#define DEFAULT_EXPT_VALUE  -100.0

static int cbCovarUntieAll (Codebook *cb, IArray *tieList);
static int cbCovarRetieAll (Codebook *cb, IArray *tieList);

/* ========================================================================
    codebook configuration
   ======================================================================== */



static int cbcfgInit (Cbcfg *cbcfg, char *name)
{
  *cbcfg       = defaultCbcfg;
   cbcfg->name = strdup(name);

  return TCL_OK;
}

static Cbcfg *cbcfgCreate (char *name)
{
  Cbcfg *cbcfg = (Cbcfg*)malloc(sizeof(Cbcfg));
  if (cbcfgInit(cbcfg,name)!=TCL_OK) { free(cbcfg); return NULL; }
  return cbcfg;
}

static ClientData cbcfgCreateItf (ClientData cd, int argc, char *argv[])
{
  return (ClientData)cbcfgCreate(argv[0]);
}

static int cbcfgFreeItf (ClientData cd)
{
  return TCL_OK;
}

static int cbcfgConfigureItf (ClientData cd, char *var, char *val)
{
  Cbcfg *cfg = (Cbcfg*)cd;
  if (! cfg) cfg = &defaultCbcfg;
  if (! var) {
    ITFCFG(cbcfgConfigureItf,cd,"name");
    ITFCFG(cbcfgConfigureItf,cd,"useN");
    ITFCFG(cbcfgConfigureItf,cd,"minRv");
    ITFCFG(cbcfgConfigureItf,cd,"minCv");
    ITFCFG(cbcfgConfigureItf,cd,"beta");
    ITFCFG(cbcfgConfigureItf,cd,"momentum");
    ITFCFG(cbcfgConfigureItf,cd,"momentumCv");
    ITFCFG(cbcfgConfigureItf,cd,"accu");
    ITFCFG(cbcfgConfigureItf,cd,"update");
    ITFCFG(cbcfgConfigureItf,cd,"method");
    ITFCFG(cbcfgConfigureItf,cd,"weight");
    ITFCFG(cbcfgConfigureItf,cd,"topN");
    ITFCFG(cbcfgConfigureItf,cd,"rdimN");
    ITFCFG(cbcfgConfigureItf,cd,"bbiOn");
    ITFCFG(cbcfgConfigureItf,cd,"expT");
    ITFCFG(cbcfgConfigureItf,cd,"mergeThresh");
    ITFCFG(cbcfgConfigureItf,cd,"splitStep");
    ITFCFG(cbcfgConfigureItf,cd,"rhoGlob");
    ITFCFG(cbcfgConfigureItf,cd,"E");
    ITFCFG(cbcfgConfigureItf,cd,"H");
    ITFCFG(cbcfgConfigureItf,cd,"I");

    return TCL_OK;
  }
  ITFCFG_CharPtr(var,val,"name",       cfg->name,         1);
  ITFCFG_Int    (var,val,"useN",       cfg->useN,         1);
  ITFCFG_Float  (var,val,"minRv",      cfg->minRvCount,   0);
  ITFCFG_Float  (var,val,"minCv",      cfg->minCvCount,   0);
  ITFCFG_Float  (var,val,"beta",       cfg->beta,         0);
  ITFCFG_Float  (var,val,"momentum",   cfg->momentum,     0);
  ITFCFG_Float  (var,val,"momentumCv", cfg->momentumCv,   0);
  ITFCFG_Char   (var,val,"accu",       cfg->doAccumulate, 0);
  ITFCFG_Char   (var,val,"update",     cfg->doUpdate,     0);
  ITFCFG_Char   (var,val,"method",     cfg->method,       0);
  ITFCFG_Float  (var,val,"weight",     cfg->weight,       0);
  ITFCFG_Int(    var,val,"topN",       cfg->topN,         0);
  ITFCFG_Int(    var,val,"rdimN",      cfg->rdimN,        1);
  ITFCFG_Int(    var,val,"bbiOn",      cfg->bbiOn,        0);
  ITFCFG_Float(  var,val,"expT",       cfg->expT,         0);
  ITFCFG_Float(  var,val,"mergeThresh",cfg->mergeThresh,  0);
  ITFCFG_Float(  var,val,"splitStep",  cfg->splitStep,    0);
  ITFCFG_Float(  var,val,"rhoGlob",    cfg->rhoGlob,      0);
  ITFCFG_Float(  var,val,"E",          cfg->E,            0);
  ITFCFG_Float(  var,val,"H",          cfg->H,            0);
  ITFCFG_Float(  var,val,"I",          cfg->I,            0);

  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;   
}

static int cbcfgPutsItf (ClientData cd, int argc, char *argv[])
{
  return cbcfgConfigureItf(cd,NULL,NULL);
}

/* ========================================================================
    CodebookRvRank
   ======================================================================== */
/* ------------------------------------------------------------------------
    Codebook Vector Ranking      used for the neural gas ranking
   ------------------------------------------------------------------------ */

typedef struct {
  int   i;                           /* index of the reference vector       */
  float d;                           /* probability of the reference vector */

} CodebookRvRank;

static CodebookRvRank* cbRankA = NULL;
static int             cbRankN = 0;

static CodebookRvRank* cbRankAlloc( int refN)
{
  if ( cbRankN < refN) {
    cbRankN =  refN;
    cbRankA = (CodebookRvRank*)realloc( cbRankA, cbRankN * 
                                        sizeof(CodebookRvRank));
  }
  return cbRankA;
}

#if USEFUL
static int compareUpCbRvRank(const void * a, const void *b) 
{
  CodebookRvRank* bpa = (CodebookRvRank *)a;
  CodebookRvRank* bpb = (CodebookRvRank *)b;

  if      ( bpa->d < bpb->d) return -1;
  else if ( bpa->d > bpb->d) return  1;
  else                       return  0;
}
#endif 

static int compareDownCbRvRank(const void * a, const void *b)
{
  CodebookRvRank* bpa = (CodebookRvRank *)a;
  CodebookRvRank* bpb = (CodebookRvRank *)b;

  if      ( bpa->d > bpb->d) return -1;
  else if ( bpa->d < bpb->d) return  1;
  else                       return  0;
}

/* ========================================================================
    CodebookAccu
   ======================================================================== */

/* ------------------------------------------------------------------------
    cbAccuInit   initialize the codebook accumulator and allocate the 
                 data structures. Also initialize the covariance matrix
                 accumulators and then clear the accu to zero.
   ------------------------------------------------------------------------ */

static int cbAccuAllocate( CodebookAccu* accu)
{
  Codebook* cb = accu->cb;
  int     subN = accu->subN;
  int     subX = 0;

  if (accu->count) {
    assert (accu->rv);
    assert (accu->sumOsq);
    return TCL_OK;
  }

  if (_bmemCBSP) {
    accu->count    = bmemAlloc(_bmemCBSP,subN * sizeof(double*));
    accu->rv       = bmemAlloc(_bmemCBSP,subN * sizeof(DMatrix*));
    accu->sumOsq   = bmemAlloc(_bmemCBSP,subN * sizeof(DCovMatrix*));
  } else {
    if (NULL == (accu->count = malloc(subN * sizeof(double*)))) {
      ERROR("cbAccuAllocate: allocation failure\n");
      return TCL_ERROR;
    }
    if (NULL == (accu->rv = malloc(subN * sizeof(DMatrix*)))) {
      ERROR("cbAccuAllocate: allocation failure\n");
      return TCL_ERROR;
    }
    if (NULL == (accu->sumOsq   = malloc(subN * sizeof(DCovMatrix*)))) {
      ERROR("cbAccuAllocate: allocation failure\n");
      return TCL_ERROR;
    }
  }

  /* bayes training : create prior accus */
  if (cb->cfg->method  =='b') {
    if (_bmemCBSP) {
      accu->priorRV  = fmatrixCreate_BMEM(cb->refN, cb->dimN, _bmemCBSP);
      accu->priorCV  = fmatrixCreate_BMEM(cb->refN, cb->dimN, _bmemCBSP);
    } else {
      accu->priorRV  = fmatrixCreate(cb->refN, cb->dimN);
      accu->priorCV  = fmatrixCreate(cb->refN, cb->dimN);
    } 
  } else {
    accu->priorRV  = NULL;
    accu->priorCV  = NULL;
  }

  for ( subX = 0; subX < subN; subX++) {
    int refX;

    if (_bmemCBSP) {
      accu->count[subX]  = bmemAlloc(_bmemCBSP,(cb->refN) * sizeof(double));
      accu->rv[subX]     = dmatrixCreate_BMEM(cb->refN, cb->dimN,_bmemCBSP);
      accu->sumOsq[subX] = bmemAlloc(_bmemCBSP,(cb->refN) * sizeof(DCovMatrix));
    } else {
       if (NULL == (accu->count[subX] = malloc((cb->refN) * sizeof(double)))) {
	 ERROR("cbAccuAllocate: allocation failure\n");
	 return TCL_ERROR;
       }
       if (NULL == (accu->rv[subX] = dmatrixCreate(cb->refN, cb->dimN))) {
	 ERROR("cbAccuAllocate: allocation failure\n");
	 return TCL_ERROR;
       }
       if (NULL == (accu->sumOsq[subX] = malloc((cb->refN) * sizeof(DCovMatrix)))) {
	 ERROR("cbAccuAllocate: allocation failure\n");
	 return TCL_ERROR;
       }
    }

    for (refX=0; refX<cb->refN; refX++) {
      dcvInit(accu->sumOsq[subX]+refX, cb->dimN, cb->type);
    }
  }
  return cbAccuClear(accu,-1);
}

static int cbAccuInit( CodebookAccu* accu, Codebook *cb, int subN)
{
  assert(cb);
  assert(subN>0);
  accu->cb         = cb; 
  accu->subN       = subN;
  accu->countSum   = 0.0;
  accu->score      = 0.0;
  accu->distortion = 0.0;
  accu->mindist    = 0.0;
  accu->maxdist    = 0.0;
  accu->count      = NULL;
  accu->rv         = NULL;
  accu->sumOsq     = NULL;
  accu->priorRV    = NULL;
  accu->priorCV    = NULL;
  link( cb, "Codebook");

  /* cbAccuAllocate(accu); */

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbAccuCreate    allocate a new codebook accumulator and initialize
                    it using the cbAccuInit function
   ------------------------------------------------------------------------ */

static CodebookAccu* cbAccuCreate(Codebook *cb, int subN)
{
  CodebookAccu* cba = (CodebookAccu*)malloc(sizeof(CodebookAccu));

  if (! cba || cbAccuInit( cba, cb, subN) != TCL_OK) {
    if ( cba) free( cba);
    ERROR("Can't allocate codebook accumulator.\n"); 
    return NULL;
  }
  return cba;
}

/* ------------------------------------------------------------------------
    cbAccuDeinit    free the accumulator vectors and covariance matrices
   ------------------------------------------------------------------------ */

static int cbAccuDeinit( CodebookAccu* cba)
{
  int subX;

  if (! cba || ! cba->count) return TCL_OK;

  if (! cba->cb) {
    ERROR("no valid codebook for cbAccuDeinit\n");
    return TCL_ERROR;
  }

  for (subX=0; subX<cba->subN; subX++) {
    int refX;
    int refA= cba->cb->refN;
    int refB= cba->rv[subX]->m;
    if (refA != refB) {
      ERROR("%s refN mismatch cb: %d  accu: %d\n",cba->cb,refA,refB);
      return TCL_ERROR;
    }

    if (!_bmemCBSP && cba->count[subX]) 
      free( cba->count[subX]);
    if (!_bmemCBSP)             
      dmatrixFree(cba->rv[subX]);
    for (refX=0; refX<cba->cb->refN; refX++) {
      if (cba->sumOsq[subX]+refX) 
	dcvDeinit(cba->sumOsq[subX]+refX);
    }   
    if (!_bmemCBSP && cba->sumOsq [subX])  
      free( cba->sumOsq[subX]);
  }

  if (! _bmemCBSP) {
    if ( cba->count)  free( cba->count);         
    if ( cba->rv)     free( cba->rv);            
    if ( cba->sumOsq) free( cba->sumOsq);        
    if (cba->priorRV) fmatrixFree(cba->priorRV); 
    if (cba->priorCV) fmatrixFree(cba->priorCV); 
  }

  cba->count   = NULL;
  cba->rv      = NULL;
  cba->sumOsq  = NULL; 
  cba->priorRV = NULL;
  cba->priorCV = NULL;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbAccuFree    deinit the accumulator data structure and remove the
                  the accumulator afterwards
   ------------------------------------------------------------------------ */

static int cbAccuFree( CodebookAccu* cba)
{
  if ( cbAccuDeinit( cba) != TCL_OK) return TCL_ERROR;
  if ( cba && cba->cb) { unlink( cba->cb, "Codebook"); cba->cb = NULL; }
  if ( cba) free( cba);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbAccuConfigureItf    configure variables within a codebook accumulator
   ------------------------------------------------------------------------ */

static int cbAccuConfigureItf (ClientData cd, char *var, char *val)
{
  CodebookAccu* cbaP = (CodebookAccu*)cd;
  if (! cbaP)   return TCL_ERROR;

  if (! var) {
    ITFCFG(cbAccuConfigureItf,cd,"subN");
    ITFCFG(cbAccuConfigureItf,cd,"count");
    ITFCFG(cbAccuConfigureItf,cd,"score");
    ITFCFG(cbAccuConfigureItf,cd,"minDistance");
    ITFCFG(cbAccuConfigureItf,cd,"maxDistance");
    ITFCFG(cbAccuConfigureItf,cd,"distortion");
    return TCL_OK;
  }
  ITFCFG_Int    (var,val,"subN",        cbaP->subN,       1);
  ITFCFG_Float  (var,val,"score",       cbaP->score,      0);
  ITFCFG_Float  (var,val,"minDistance", cbaP->mindist,    0);
  ITFCFG_Float  (var,val,"maxDistance", cbaP->maxdist,    0);
  ITFCFG_Float  (var,val,"distortion",  cbaP->distortion, 0);

  if (! strcmp(var,"count")) {
    if (! val) {
      int   subX, refX;
      float sum  = 0;
      
      if (cbaP->count) {
	for ( subX = 0; subX < cbaP->subN; subX++)
	  for ( refX = 0; refX < cbaP->cb->refN; refX++)
	    sum += cbaP->count[subX][refX]; 
      }
      itfAppendResult("% 2.4e",sum); 
      return TCL_OK;
    }
    return TCL_OK;
  }
  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    cbAccuAccessItf    allow access to Janus typed objects within a 
                       codebook accumulator via TCL
   ------------------------------------------------------------------------ */

static ClientData cbAccuAccessItf (ClientData cd, char *name, TypeInfo **ti)
{
  CodebookAccu* cba = (CodebookAccu*)cd;
  int           refX, subX;

  if ( *name == '.') {
    if ( name[1] =='\0') {
      if ( cba->cb->refN > 0) {
        itfAppendElement("cov(0..%d,0..%d)", cba->subN-1, cba->cb->refN-1);
        itfAppendElement("mat(0..%d)", cba->subN-1);
	itfAppendElement("priorRV");
	itfAppendElement("priorCV");
      }
      *ti=NULL;
      return NULL; 
    }
    else {
      if (sscanf(name+1,"cov(%d,%d)", &subX, &refX) == 2) {
        *ti = itfGetType("DCovMatrix"); 
        if ( subX >= 0 && subX < cba->subN &&
             refX >= 0 && refX < cba->cb->refN && cba->sumOsq)
             return (ClientData)(cba->sumOsq[subX]+refX);
        else return  NULL;
      }
      if (sscanf(name+1,"mat(%d)", &subX) == 1) {
        *ti = itfGetType("DMatrix"); 
        if ( subX >= 0 && subX < cba->subN && cba->rv)
             return (ClientData)(cba->rv[subX]);
        else return  NULL;
      }
      if (strcmp(name+1,"priorRV") == 0) {
        *ti = itfGetType("FMatrix"); 
	return (ClientData)(cba->priorRV);
      }
      if (strcmp(name+1,"priorCV") == 0) {
        *ti = itfGetType("FMatrix"); 
	return (ClientData)(cba->priorCV);
      }

    }
    *ti=NULL; return NULL;
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    cbAccuPuts     prints the accumulator information to a TCL string
   ------------------------------------------------------------------------ */

static int cbAccuPutsItf (ClientData cd, int argc, char *argv[])
{
  CodebookAccu* cba  = (CodebookAccu*)cd;
  int           ac   =  argc-1;
  int           subX, refX, dimX;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0, NULL) != TCL_OK) 
       return TCL_ERROR;

  for (subX=0; subX<cba->subN; subX++) {    
    itfAppendResult("{{");
    for (refX=0; refX<cba->cb->refN; refX++)
      itfAppendResult(" % 2.4e",cba->count ? cba->count[subX][refX] : 0.0);
    itfAppendResult("}\n{");

    for (refX=0; refX<cba->cb->refN; refX++) {
      itfAppendResult("{ ");
      for (dimX=0; dimX<cba->cb->dimN; dimX++) 
        itfAppendResult(" % 2.4e",cba->rv ? cba->rv[subX]->matPA[refX][dimX] : 0.0);
      itfAppendResult("}\n");
    }
    itfAppendResult("}}\n");
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbAccuSubspace  defines the accumulator subspace partitioning according
                    to the direction of maximum variance of the gaussians.
                    In order to designate the side of the hyperplane we use
                    plus/minus the dimension index (and thus have to shift
                    the dimension index itself so that we start at -1,+1..)
   ------------------------------------------------------------------------ */

static int cbAccuSubspace( CodebookAccu* cba)
{
  Codebook* cb   = cba->cb;
  int       refN = cb->refN, refX;

  if (!cba->count) return TCL_OK;

  int    cb_fcvVarN =  cb->dimN;
  float* cb_fcvVarA = (float*) malloc( cb_fcvVarN * sizeof(float));
  int*   cb_fcvVarX = (int  *) malloc( cb_fcvVarN * sizeof(int));
  
  for ( refX = 0; refX < refN; refX++) {
    fcvVariances (cb->cv[refX], cb_fcvVarA, cb_fcvVarX);

    if ( cba->subN > 1) {
      int    i;
      for ( i = 0; i < cba->subN; i++) {
        cba->sumOsq[i][refX].maxVarDimX = (i%2) ? -(cb_fcvVarX[i/2]+1) : (cb_fcvVarX[i/2]+1);
        cba->sumOsq[i][refX].maxVar     =  cb_fcvVarA[i/2];
      }
    } 
    else {
      cba->sumOsq[0][refX].maxVarDimX = 0;
      cba->sumOsq[0][refX].maxVar     = cb_fcvVarA[0];    
      /* ------------------------------------------------------------------- */
      /* in case of full covariances, we cannot really give the dimension... */
      /* set it to an 'impossible' value here. Hopefully this crashes if     */
      /* someone uses it, reminding him/her to look closer to this thing     */
      /* ------------------------------------------------------------------- */
      if (cb->type == COV_FULL) { 
	cba->sumOsq[0][refX].maxVarDimX = -1;
      }
    }
  }
  
  free( cb_fcvVarA);
  free( cb_fcvVarX);

  return TCL_OK;
}

static int cbAccuSubspaceItf (ClientData cd, int argc, char *argv[])
{
  CodebookAccu *cba  = (CodebookAccu*)cd;
  int           ac   =  argc-1;

  if (   itfParseArgv( argv[0], &ac, argv+1, 0, NULL) != TCL_OK) 
         return TCL_ERROR;
  return cbAccuSubspace(cba);
}

/* ------------------------------------------------------------------------
    cbAccuClear   clear the contents of the accumulator to zero and
                  determine the resulting new subspace.
   ------------------------------------------------------------------------ */

int cbAccuClear( CodebookAccu* cba, int subIndex)
{
  int subX, subA, subE, refX, dimX;

  if (! cba || ! cba->count) return TCL_OK;

  if (subIndex >= 0) {
    subA = subIndex;
    subE = subIndex+1;
  } else {
    subA = 0;
    subE = cba->subN;
  }
  for (subX=subA; subX<subE; subX++) {
    for (refX=0; refX<cba->cb->refN; refX++) {
      cba->count[subX][refX] = 0.0;
      for ( dimX=0; dimX < cba->cb->dimN; dimX++) 
        cba->rv[subX]->matPA[refX][dimX] = 0.0;
      dcvClear( cba->sumOsq[subX] + refX);
    }
  }

  if (cba->priorRV) fmatrixClear(cba->priorRV);
  if (cba->priorCV) fmatrixClear(cba->priorCV);

  if (subIndex >= 0)
    return cbAccuSubspace( cba);
  else
    return TCL_OK;
}

static int cbAccuClearItf( ClientData cd, int argc, char *argv[])
{
  CodebookAccu *cba  = (CodebookAccu*)cd;
  int           ac   =  argc-1;
  int           subX = -1;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "-subX", ARGV_INT,  NULL, &subX,  NULL, "sub-accumulator, -1 to clear all",
        NULL) != TCL_OK) return TCL_ERROR;

  return cbAccuClear(cba,subX);
}

static float*  cbsAddCountA = NULL;   /* those will be dynamical allocated  */
static int     cbsAddCountN = 0;      /* and realloced by codebook routines */
static float** cbsSubA      = NULL;   /* below.                             */
static int     cbsSubN      = 0;

/* ------------------------------------------------------------------------
    cbAccuSubShare   computes each subaccumulator's share of the training 
   ------------------------------------------------------------------------ */

static int cbAccuSubShare( CodebookAccu* cba, float*  pattern, float** subA)
{
  int refX, subX, subN = cba->subN;

  /* no accus, no subspace */
  if (!cba->sumOsq) return TCL_OK;

  if (subN == 1) {
    for (refX = 0; refX < cba->cb->refN; refX++)
      subA[refX][0] = 1.0;
  } else {
    for (refX = 0; refX < cba->cb->refN; refX++) {
      float sum = 0.0;
      for (subX = 0; subX < subN; subX++)
	sum += subA[refX][subX]
	     = dcvSubspace (cba->sumOsq[subX]+refX, 
			    cba->cb->rv->matPA[refX], pattern, 10.0);
      for (subX = 0; subX < subN; subX++)
	subA[refX][subX] /= sum;
    }
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbAccuAdd
   -----------
    adds a pattern to the codebook accumulator

    addCount[refX] is the a-priori-probability per reference vector
    subA[refX][subX] is the share of each ref vector among the sub-accus
    (sum over all subX of subA[refX][subX] is one for each refX, or in
    case of no sub-accus, all subA[refX][0] are 1.0)
   ------------------------------------------------------------------------ */

static double* dpattern = NULL;
static double* spattern = NULL;
static float*  opattern = NULL;
static int     ddimN    = 0;

int cbAccuAdd( CodebookAccu* cba, float* pattern,  float   factor, 
                                  float* addCount, float** subA, int subIndex)
{
  int  refN = cba->cb->refN, refX;
  int  dimN = cba->cb->dimN;
  int  subS = (subIndex >=0) ? subIndex   : 0;
  int  subE = (subIndex >=0) ? subIndex+1 : cba->subN;
  

  /* allocate accu on demand */
  if (! cba->count) 
    cbAccuAllocate(cba);

  if (!addCount) addCount = cbsAddCountA;

  /* copy pattern to 'double' and create 'squared pattern'
     useful unless full COVs are used */
  if (opattern != pattern) {
    if (ddimN < dimN) {
      dpattern = (double*) malloc (sizeof(double) * dimN);
      spattern = (double*) malloc (sizeof(double) * dimN);
      ddimN = dimN;
    }
    for (refX = 0; refX < dimN; refX++) {
      dpattern[refX] =  pattern[refX];
      spattern[refX] = dpattern[refX] * dpattern[refX];
    }
    opattern = pattern;
  }

  for (refX = 0; refX < refN; refX++) {
    float  refFactor = addCount[refX] * factor;
    int    subX;
    
    if (refFactor > 0.0) {
      for (subX = subS; subX < subE; subX++) {
	double gamma = subA[refX][subX] * refFactor;
	if (gamma > 0.0) {
	  double     *rv = cba->rv[subX]->matPA[refX];
	  int       dimX;
	  for (dimX = 0; dimX < dimN; dimX++)
	    rv[dimX] += gamma * dpattern[dimX];
	  dcvAccuAdd (cba->sumOsq[subX] + refX, dpattern, spattern, gamma);
	  cba->count[subX][refX] += gamma;
	}
      }
      
    } else {
      double w = 1.0 / (double) (subE-subS);
      for (subX = subS; subX < subE; subX++)
	subA[refX][subX] = w;
    }
  }

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    cbAccuMLE    given accumulator cba[subX][refX] find the MLE solution
                 for the new mean vector and covariance matrix. If subX < 0
                 then the sum of all sub-accus will be used to estimate
                 the new model.
   ------------------------------------------------------------------------ */

static int cbAccuMLE( CodebookAccu* cba, int refX, int subX,
		      float* muP, FCovMatrix* cvP, float* gamma)
{
  Codebook* cb       = cba->cb;
  int       subN     = cba->subN;
  int       dimN     = cb->dimN;
  double    count    = 0.0;
  int       dimX, dimY, i;
  float     momentumCv;
  double    mean, sq, val;

  /* no accus, no update */
  if (!cba->count) {
    if (muP != cb->rv->matPA[refX])
      memcpy(muP,cb->rv->matPA[refX],dimN*sizeof(float));
    if (cvP != cb->cv[refX]) 
      fcvCopy( cvP, cb->cv[refX]);
    return TCL_OK;
  }

  /* If no sub-accu is specified, sum all accumulators together and update
     the gaussian model based on all samples in the accumulator */

  /* Allow for different momenta for Means and Covariances */
  momentumCv = (cb->cfg->momentumCv < 0.0) ? cb->cfg->momentum : cb->cfg->momentumCv;

  if ( subX < 0) {
    for ( i = 0; i < subN; i++) count += cba->count[i][refX];
    if ( gamma) *gamma = count;

    /* update the means into muP[dimX] */
    if ( count > cb->cfg->minRvCount) {
      for ( dimX = 0; dimX < dimN; dimX++) {
        mean = 0.0;
        for (i = 0; i < subN; i++) mean += cba->rv[i]->matPA[refX][dimX];
        muP[dimX] = (    cb->cfg->momentum) * cb->rv->matPA[refX][dimX] +
                    (1.0-cb->cfg->momentum) * mean / count;
      }
    } else if ( muP != cb->rv->matPA[refX]) 
      for ( dimX = 0; dimX < dimN; dimX++) muP[dimX] = cb->rv->matPA[refX][dimX];
    
    cb->count[refX] = count;

    /* covariance estimation. First: do we have sufficient counts? */
    if ( count < cb->cfg->minCvCount) {
      /* no, copy existing covariance matrix */
      if ( cvP != cb->cv[refX]) fcvCopy( cvP, cb->cv[refX]);
      return TCL_OK;
    }

    switch ( cba->cb->type) {
    case COV_RADIAL:

      /* Florian */
      mean = sq = 0.0;
      for ( i = 0; i < subN; i++) {
	for ( dimX = 0; dimX < dimN; dimX++ ) 	
          mean += cba->rv[i]->matPA[refX][dimX];
	sq += cba->sumOsq[i][refX].m.r;
      }
      mean /= dimN;
      sq   /= dimN;

      if ( momentumCv > 0.0)
	/* the old covariance vectors are stored as 1/cov, account for that */
	/* by adding [1/oldvalue] plus [newvalue]                           */
      	val = (      momentumCv) /  cb->cv[refX]->m.r +
	      (1.0 - momentumCv) * (sq/count - square(mean/count));
      else val =  sq/count - square(mean/count);
      
      if ( val <= COV_FLOOR) val = COV_FLOOR;
      
      cvP->m.r = 1.0 / val;
      cvP->det = log(val);
      break;

    case COV_DIAGONAL:

      cvP->det = 0.0;
      for ( dimX = 0; dimX < dimN; dimX++) { 
        mean = sq = 0.0;

        for ( i = 0; i < subN; i++) {
          mean += cba->rv[i]->matPA[refX][dimX];
          sq   += cba->sumOsq[i][refX].m.d[dimX];
        }

        if ( momentumCv > 0.0)
	  /* the old covariance vectors are stored as 1/cov, account for that */
	  /* by adding [1/oldvalue] plus [newvalue]                           */
             val = (      momentumCv) /  cb->cv[refX]->m.d[dimX] +
                   (1.0 - momentumCv) * (sq/count - square(mean/count));
        else val =  sq/count - square(mean/count);

        if ( val <= COV_FLOOR) val = COV_FLOOR;

        cvP->m.d[dimX]  =  1.0 / val;
        cvP->det       +=  log(val);
      }
      break;

    case COV_FULL:
      {
	double dummy;
	DMatrix *dmat;
	dmat = dmatrixCreate(dimN,dimN);
	if (momentumCv > 0.0) {
	  /* ----------------------------------------- */
	  /* we need to have the old covariance matrix */
	  /* but we have it only inverted. Re-invert   */
	  /* it to allow later linear interpolation    */
	  /* ----------------------------------------- */
	  FATAL("TODO: momentum training for full covariances\n");
	}
	for (dimX = 0; dimX < dimN; dimX++) {
	  for (dimY = 0; dimY < dimN; dimY++) {
	    double meanX= 0.0;
	    double meanY= 0.0;
	    double xy   = 0.0;
	    double val  = 0.0;
	    /* compute mean value of dimensions X, Y and of */
	    /* the product of x * y (E(xy), E(x), E(y))     */
	    for ( i = 0; i < subN; i++) {
	      meanX+= cba->rv[i]->matPA[refX][dimX];
	      meanY+= cba->rv[i]->matPA[refX][dimY];
	      xy   += cba->sumOsq[i][refX].m.f[dimX][dimY];
	    }
	    meanX = meanX / count; meanY = meanY / count; xy = xy / count;
	    if (cb->cfg->momentum > 0.0) {
	      FATAL("TODO: momentum training for full covariances\n");
	    } else {
	      val = xy - meanX * meanY;
	    }
	    if ( (dimX == dimY) && (val <= COV_FLOOR) ) val = COV_FLOOR;
	    dmat->matPA[dimX][dimY] = val;
	  }
	}
	/* compute determinant */
	cvP->det = dmatrixLogDet(dmat, &dummy);
	/* now we have the covariance matrix in dmat */
	/* but we need the inverse of it -> do that */
	dmatrixInv(dmat,dmat);
	/* now copy inverse of covariance matrix into final target */
	for (dimX = 0; dimX < dimN; dimX++) {
	  for (dimY = 0; dimY < dimN; dimY++) {
	    cvP->m.f[dimX][dimY] = dmat->matPA[dimX][dimY];
	  }
	}
	dmatrixFree(dmat);
      }
      break;

    case COV_ERR: { }

    case COV_NO: { }

    }
    return TCL_OK;
  }

  if ( subX >= subN) {
    ERROR( "Illegal subX %d (%d) used in cbAccuMLE.\n", subX, subN);
    return  TCL_ERROR;
  }

  /* Do the MLE update only based on the sub-accumulator subX */

  if ((count = cba->count[subX][refX]) > cb->cfg->minRvCount) {
    for ( dimX = 0; dimX < dimN; dimX++) {
      muP[dimX] = (    cb->cfg->momentum) * cb->rv->matPA[refX][dimX] +
                  (1.0-cb->cfg->momentum) * cba->rv[subX]->matPA[refX][dimX] / count;
    }
  }
  else if ( muP != cb->rv->matPA[refX]) {
    for ( dimX = 0; dimX < cb->dimN; dimX++) muP[dimX] = cb->rv->matPA[refX][dimX];
  }
  if ( gamma) *gamma = count;
  if ( count < cb->cfg->minCvCount) {
    if ( cvP != cb->cv[refX]) fcvCopy( cvP, cb->cv[refX]);
    return TCL_OK;
  }
  cb->count[refX] = count;

  switch ( cba->sumOsq[subX][refX].type) {
  case COV_RADIAL:

    FATAL("TODO: MLE of radial matrix");
    break;

  case COV_DIAGONAL:

    cvP->det = 0.0;
    for ( dimX = 0; dimX < dimN; dimX++) {
      double val  = cba->sumOsq[subX][refX].m.d[dimX] / count - 
                    square( cba->rv[subX]->matPA[refX][dimX] / count);

      if ( momentumCv > 0.0)
        val = (    momentumCv) / cb->cv[refX]->m.d[dimX] +
              (1.0-momentumCv) * val;

      if (   val <= COV_FLOOR) val = COV_FLOOR;

      cvP->m.d[dimX]  = 1.0 / val;
      cvP->det       += log(val);
    }
    break;

  case COV_FULL:

    for ( dimX = 0; dimX < dimN; dimX++) {
      for ( dimY = 0; dimY < dimN; dimY++) {
        double val  = cba->sumOsq[subX][refX].m.f[dimX][dimY] / count - 
                      cba->rv[subX]->matPA[refX][dimX] * cba->rv[subX]->matPA[refX][dimY] /
                      count;

        if (   val <= COV_FLOOR) val = COV_FLOOR;
        cvP->m.f[dimX][dimY]  = (    momentumCv) * cb->cv[refX]->m.f[dimX][dimY] +
                                (1.0-momentumCv) / val;
      }
    }
    FATAL("TODO: computeDeterminant & invertCovMatrix");
    break;

  case COV_ERR: { }

  case COV_NO: { }

  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbAccuMAP    given accumulator cba[subX][refX] and codebook cb 
                 find the MAP solution assuming diagonal covariances
   ------------------------------------------------------------------------ */

static int cbAccuMAP( CodebookAccu* cba, int refX, int subX,
		      float* muP, FCovMatrix* cvP, float* gamma)
{
  Codebook* cb       = cba->cb;
  int       subN     = cba->subN;
  int       dimN     = cb->dimN;
  double    count    = 0.0;
  int       dimX, i;

  double meanOld,mean;
  double sigmaOld,precisionOld,sigma,sigmaMAP;
  double alpha,beta,lambda;
  double 
    tmpLambda=0.0,   tmpPrior=0.0, tmpSigma=0.0, 
    tmpSigmaMAP=0.0, tmpSigmaOld=0.0 ;

  /* no accus, no update */
  if (!cba->count) {
    if (muP != cb->rv->matPA[refX])
      memcpy(muP,cb->rv->matPA[refX],dimN*sizeof(float));
    if (cvP != cb->cv[refX]) 
      fcvCopy( cvP, cb->cv[refX]);
    return TCL_OK;
  }

  assert(subX<0 && cba->sumOsq[0][refX].type == COV_DIAGONAL);

  for ( i = 0; i < subN; i++) count += cba->count[i][refX];
  if ( gamma) *gamma = count;
   
  cvP->det=0.0;

  for ( dimX = 0; dimX < dimN; dimX++) {
    /*old mean and variance*/  
    meanOld      = cb->rv->matPA[refX][dimX];
    sigmaOld     = 1.0 / cb->cv[refX]->m.d[dimX];
    precisionOld = cb->cv[refX]->m.d[dimX];

    /*new mean*/
    mean = 0.0;
    for (i = 0; i < subN; i++) mean += cba->rv[i]->matPA[refX][dimX];
    mean /= count;

    /*new variance*/
    sigma =0.0;
    for (i = 0; i < subN; i++) sigma +=  cba->sumOsq[i][refX].m.d[dimX];
    sigma/=count;
    sigma-= square(mean);
    if (sigma <= COV_FLOOR) sigma = COV_FLOOR;

    /*update MAP mean*/
    if (count >= cb->cfg->minRvCount) {
      cb->count[refX] = count;
      lambda = count * cba->priorRV->matPA[refX][dimX] / (sigmaOld + count * cba->priorRV->matPA[refX][dimX]);
      muP[dimX] =  lambda * mean  + (1.0 - lambda) * meanOld ;
    } else {
      lambda=0.0;
      muP[dimX] = meanOld;
    }

    /*update MAP covariance*/
    if ( count >= cb->cfg->minCvCount && cba->priorCV->matPA[refX][dimX] > 1e-8) {
      alpha = square(precisionOld) / cba->priorCV->matPA[refX][dimX];
      beta  = precisionOld         / cba->priorCV->matPA[refX][dimX];
      sigmaMAP = (beta +count/2.0 * sigma) / (alpha + count/2.0);
      if (sigmaMAP <= COV_FLOOR) sigmaMAP = COV_FLOOR;

      cvP->m.d[dimX]  =  1.0 / sigmaMAP;
      cvP->det       +=  log(sigmaMAP);
    } else {
      sigmaMAP=0.0;
      cvP->m.d[dimX]  =  1.0 / sigmaOld;
      cvP->det       +=  log(sigmaOld);
    }

    tmpSigma    +=sigma;
    tmpSigmaMAP +=sigmaMAP;
    tmpSigmaOld +=sigmaOld;
    tmpPrior    +=cba->priorRV->matPA[refX][dimX];
    tmpLambda   +=lambda;

  }
  
  tmpSigmaOld /=dimN;
  tmpSigmaMAP /=dimN;
  tmpSigma    /=dimN;
  tmpPrior    /=dimN;
  tmpLambda   /=dimN;

  INFO("cbAccuMAP for %s: refX=%d, count=%f: sigmaOld=%f:  sigma=%f:  sigmaMAP=%f:  prior=%f:  lambda=%f\n",
       cb->name,refX,count,tmpSigmaOld,tmpSigma,tmpSigmaMAP,tmpPrior,tmpLambda);

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    cbAccuSplitLR     computes a split likelihood ratio score for each of
                      the reference vectors within the codebook accu.
   ------------------------------------------------------------------------ */

static CodebookRvRank* cbAccuSplitLR( CodebookAccu* cba)
{
  float*          mean[3];
  FCovMatrix*     cov[3];
  float           gamma[3];
  Codebook*       cb    = cba->cb;
  int             subN  = cba->subN;
  int             dimN  = cb->dimN, dimX;
  int             refN  = cb->refN, refX;
  CodebookRvRank* rankA = cbRankAlloc( refN);

  if (!cba->count) return NULL;
  if ( subN < 2)   return NULL;

  /* allocate the mean vectors and the covariance matrices for the split
     and the non-split case */

  mean[0] = (float*)malloc( 3 * dimN * sizeof(float));
  mean[1] =  mean[0]  + dimN;
  mean[2] =  mean[1]  + dimN;
  cov[0]  =  fcvCreate( dimN, cb->type);
  cov[1]  =  fcvCreate( dimN, cb->type);
  cov[2]  =  fcvCreate( dimN, cb->type);

  for ( refX = 0; refX < refN; refX++) {
    float lmean = 0.0;
    float lcov  = 0.0;

    cbAccuMLE( cba, refX, -1, mean[0], cov[0], &(gamma[0]));

    if ( gamma[0] > 0.000001) {
      cbAccuMLE( cba, refX,  0, mean[1], cov[1], &(gamma[1]));
      cbAccuMLE( cba, refX,  1, mean[2], cov[2], &(gamma[2]));

      for ( dimX = 0; dimX < dimN; dimX++)
        mean[0][dimX] = mean[2][dimX] - mean[1][dimX];

      /* TODO: think about splitting score for lmean again */

      lmean =  1 + (gamma[1]*gamma[2])/(gamma[0]*gamma[0]) *
               fcvMetric( cov[0], mean[0]);
      lmean =  gamma[0]/2.0 * log(lmean);

      lcov  =  gamma[1]/gamma[0] * cov[1]->det +
               gamma[2]/gamma[0] * cov[2]->det - cov[0]->det;
      lcov  = -gamma[0]/2.0      * lcov;

      rankA[refX].i = refX;
      rankA[refX].d = lmean + lcov;
    }
    else {
      rankA[refX].i = refX;
      rankA[refX].d = lmean;
    }
  }

  qsort( rankA, refN, sizeof(CodebookRvRank), compareDownCbRvRank);

  if ( mean[0]) free( mean[0]);
  if ( cov[0])  fcvFree( cov[0]);
  if ( cov[1])  fcvFree( cov[1]);
  if ( cov[2])  fcvFree( cov[2]);
  return rankA;
}

/* ------------------------------------------------------------------------
    cbAccuMatch   check if two accumulator setups match such that copy,
                  plus, times etc. should work
   ------------------------------------------------------------------------ */

static int cbAccuMatch( CodebookAccu* dst, CodebookAccu* src)
{
  if (src == dst) return TCL_OK;

  if (src->cb->refN != dst->cb->refN) {
     SERROR("cbAccu inconsistent refN: %d %d\n",dst->cb->refN,src->cb->refN); 
     return TCL_ERROR;
  }
  if (src->cb->dimN != dst->cb->dimN) {
     SERROR("cbAccu inconsistent dimN: %d %d\n",dst->cb->dimN,src->cb->dimN);
     return TCL_ERROR;
  }
  if (src->cb->type != dst->cb->type) {
     SERROR("cbAccu inconsistent type: %d %d\n",dst->cb->type,src->cb->type);
     return TCL_ERROR;
  }
  if (src->subN != dst->subN) {
     SERROR("cbAccu inconsistent subN: %d %d\n",dst->subN,src->subN);
     return TCL_ERROR;
  }
  if (src->cb->featX != dst->cb->featX) {
     SERROR("cbAccu inconsistent featX: %d %d\n",dst->cb->featX,src->cb->featX);
     return TCL_ERROR;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbAccuCopy    copy one accumulator to another. The destination codebook
                  must thereby be of the same dimensions (refN,dimN) as
                  the source codebook.
   ------------------------------------------------------------------------ */

static int cbAccuCopy( CodebookAccu* dst, CodebookAccu* src)
{
  Codebook* cb   = dst->cb;
  int       dimN = src->cb->dimN;
  int       refN = src->cb->refN;
  int       rc   = TCL_OK;
  int subX, refX, dimX;

  if ( dst == src) return TCL_OK;

  if ( cb->refN != refN || cb->dimN != dimN) {
    ERROR("Inconsistent dimensions of the codebooks %s and %s.\n",
           cb->name, src->cb->name);
    return TCL_ERROR;
  }
  if ( cbAccuDeinit( dst) != TCL_OK ||
       cbAccuInit( dst, cb, src->subN) != TCL_OK) {
    ERROR("Can't initialize new accumulator for Codebook %s.\n", cb->name);
    return TCL_ERROR;
  }
  if (!src->count) {
    return TCL_OK;
  }
  if (!dst->count) {
    cbAccuAllocate(dst);
  }
  for (subX=0; subX<dst->subN; subX++) {
    for (refX=0; refX< refN; refX++) {
      dst->count[subX][refX] = src->count[subX][refX];
      for (dimX=0; dimX < dimN; dimX++) 
	dst->rv[subX]->matPA[refX][dimX] = src->rv[subX]->matPA[refX][dimX];
      if ( dcvCopy( dst->sumOsq[subX]+refX, src->sumOsq[subX]+refX) != TCL_OK)
        rc = TCL_ERROR;
    }
  }
  return rc;
}

static int cbAccuCopyItf (ClientData cd, int argc, char *argv[])
{
  CodebookAccu *dst  = (CodebookAccu*)cd;
  CodebookAccu *src  =  NULL;
  int           ac   =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<source>", ARGV_OBJECT, NULL, &src, "CodebookAccu", "source accumulator",
       NULL) != TCL_OK) return TCL_ERROR;

  return cbAccuCopy( dst, src);
}

/* ------------------------------------------------------------------------
    cbAccuPlus   add the contents of another accumulator to the current 
                 one. It is also possible to specify a single reference
                 vector in the destination codebook such that all accus
                 are added to this single vector/cov.
   ------------------------------------------------------------------------ */

static int cbAccuPlus( CodebookAccu* dst, CodebookAccu* src, float factor, int ref)
{
  int rc = TCL_OK;
  int subX, refX, dimX;

  if (!src->count) {
    return TCL_OK;
  }
  if (!dst->count) {
    cbAccuAllocate(dst);
  }

  if ( ref < 0) {
    if ( cbAccuMatch( dst, src) != TCL_OK) return TCL_ERROR;
    for (subX=0; subX<dst->subN; subX++) {
      for (refX=0; refX<dst->cb->refN; refX++) {
        dst->count[subX][refX] += factor * src->count[subX][refX];
        for (dimX=0; dimX<dst->cb->dimN; dimX++)
          dst->rv[subX]->matPA[refX][dimX] += factor * src->rv[subX]->matPA[refX][dimX];
        if ( dcvAdd( dst->sumOsq[subX]+refX, src->sumOsq[subX]+refX,
                     factor) != TCL_OK) rc = TCL_ERROR;
      }
    }
  }
  else {
    if ( ref >= dst->cb->refN) {
       SERROR("cbAccu reference %d out of range %d.\n",ref, dst->cb->refN);
       return TCL_ERROR;
    }
    if (src->cb->dimN != dst->cb->dimN) {
       SERROR("cbAccu inconsistent dimN: %d %d\n",dst->cb->dimN,src->cb->dimN);
       return TCL_ERROR;
    }
    if (src->cb->type  != dst->cb->type  || 
        src->subN      != dst->subN      ||
        src->cb->featX != dst->cb->featX) {
       SERROR("cbAccu inconsistent accumulators\n");
       return TCL_ERROR;
    }
    for (subX=0; subX<dst->subN; subX++) {
      for (refX=0; refX<dst->cb->refN; refX++) {
        dst->count[subX][ref] += factor * src->count[subX][refX];
        for (dimX=0; dimX<dst->cb->dimN; dimX++) 
  	  dst->rv[subX]->matPA[ref][dimX] += factor * src->rv[subX]->matPA[refX][dimX]; 
        if ( dcvAdd( dst->sumOsq[subX]+ref, src->sumOsq[subX]+refX,
                     factor) != TCL_OK) rc = TCL_ERROR;
      }
    }
  }
  return rc;
}

static int cbAccuPlusItf (ClientData cd, int argc, char *argv[])
{
  CodebookAccu *dst    = (CodebookAccu*)cd;
  CodebookAccu *src    =  NULL;
  int           ref    = -1;
  float         factor =  1.0;
  int           ac     =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<source>", ARGV_OBJECT, NULL, &src,    "CodebookAccu", "source accumulator",
      "-factor",  ARGV_FLOAT,  NULL, &factor, NULL,           "scaling factor",
      "-refX",    ARGV_INT,    NULL, &ref,    NULL,           "add accus to reference refX",
       NULL) != TCL_OK) return TCL_ERROR;

  return cbAccuPlus( dst, src, factor, ref);
}

/* ------------------------------------------------------------------------
    cbAccuTimes    scale the contents of the accumulator by a floating 
                   point constant
   ------------------------------------------------------------------------ */

static int cbAccuTimes( CodebookAccu* cba, float scale)
{
  int subX, refX, dimX;

  if (!cba->count) return TCL_OK;

  for (subX=0; subX< cba->subN; subX++) {
    for (refX=0; refX<cba->cb->refN; refX++) {
      cba->count[subX][refX] *= scale;
      for (dimX=0; dimX<cba->cb->dimN; dimX++) 
        cba->rv[subX]->matPA[refX][dimX] *= scale;
      dcvTimes( cba->sumOsq[subX]+refX, scale);
    }
  }
  return TCL_OK;
}

static int cbAccuTimesItf (ClientData cd, int argc, char *argv[])
{
  CodebookAccu *cba    = (CodebookAccu*)cd;
  int           ac     =  argc-1;
  float         factor =  1.0;

  if ( itfParseArgv(argv[0], &ac, argv+1, 0,
      "<factor>", ARGV_FLOAT, NULL, &factor, NULL, "multiplication factor",
       NULL) != TCL_OK) return TCL_ERROR;

  return cbAccuTimes( cba, factor);
}
/* ------------------------------------------------------------------------
    cbAccuSet   set reference vectors in the CodebookAccu
   ------------------------------------------------------------------------ */

static int cbAccuSet( CodebookAccu* cba, FMatrix* fmP, int subX, int refX, int dimX)
{
  int dimN = cba->cb->dimN;
  int refN = cba->cb->refN, i, j;

  if ( subX < 0 || subX >= cba->subN) {
    ERROR("Subaccumulator index is out of range.\n");
    return TCL_ERROR;
  }
  if ( refX < 0 || dimX < 0) {
    ERROR("Reference/dimension index out of range.\n");
    return TCL_ERROR;
  }
  if (!cba->count)
    cbAccuAllocate(cba);
  for ( i = refX; i < refN && i < fmP->m+refX; i++) {
    for ( j = dimX; j < dimN && j < fmP->n+dimX; j++)
      cba->rv[subX]->matPA[i][j] = fmP->matPA[i-refX][j-dimX];
  }
  return TCL_OK;
}

static int cbAccuSetItf (ClientData cd, int argc, char *argv[])
{
  FMatrix*  fmP  = NULL;
  int       refX = 0;
  int       dimX = 0;
  int       subX = 0;
  int       ac   = argc-1;
  int       rc   = TCL_OK;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<matrix>", ARGV_CUSTOM, getFMatrix, &fmP,  cd,   "matrix of reference vectors", 
      "-subX",    ARGV_INT,    NULL,       &subX, NULL, "index of the subaccu",
      "-refX",    ARGV_INT,    NULL,       &refX, NULL, "index of the reference vector",
      "-dimX",    ARGV_INT,    NULL,       &dimX, NULL, "index of the dimension",
      NULL) != TCL_OK) {
    if ( fmP) fmatrixFree( fmP);
    return TCL_ERROR;
  }
  rc = cbAccuSet((CodebookAccu*)cd, fmP, subX, refX, dimX);
  if ( fmP) fmatrixFree( fmP);
  return rc;
}

/* ------------------------------------------------------------------------
    cbAccuSkip  skip codebook parameters in a given codebook accu file
                This is just a subroutine to cbsLoadAccus, which is called
                if there is a problem with a given accu in the file (e.g.,
		if this accu has not been created yet).
		subN gives the number of sub-accus for the codebook, refN
		is the number of ref vectors of the codebook, CovType may
		be one of COV_NO, COV_RADIAL, COV_DIAGONAL, COV_FULL.
   ------------------------------------------------------------------------ */

static int cbAccuSkip( int subN, int refN, int dimN, CovType type, FILE* fp)
{
  int refX,dimX;

  /* ------------------------- */
  /* read all sub accumulators */
  /* ------------------------- */
  for ( ; subN > 0; subN--) {
    for (refX=0; refX < refN; refX++){
      /* ---------- */
      /* read count */
      /* ---------- */
      read_float(fp);
      /* --------------------------- */
      /* read the vector accu itself */
      /* --------------------------- */
      for (dimX=0; dimX < dimN; dimX++) read_float(fp);
      /* ------------------------------------ */
      /* read (skip) the covariance accu info */
      /* ------------------------------------ */
      dcvSkip(dimN, type, fp);
    } /* for refX */
  } /* for subX */
  return TCL_OK;
}

/* ========================================================================
    Codebook
   ======================================================================== */

/* ------------------------------------------------------------------------
    cbInit  initialize codebook structure
   ------------------------------------------------------------------------ */

static int cbInit (Codebook* cb, ClientData CD)
{ 
  assert(cb);
  if (! _bmemCBSP) {
     cb->name   = strdup((char*)CD);
  } else {
    int n       = strlen((char*)CD);
    cb->name    = bmemAlloc(_bmemCBSP, n+1);    
    memcpy (cb->name, CD, n);
    cb->name[n] = 0;
  }
  cb->useN  = 0;
  cb->refN  = 0;        /* number of vectors in the codebook                */
  cb->refMax  = 0;      /* max. mumber of components                        */
  cb->dimN  = 0;        /* dimensionality of the underlying feature space   */
  cb->bbiX  = -1;       /* index of BBI tree (default -1 = no BBI tree)     */
  cb->bbiY  = 0;        /* index of codebook in BBI tree (default 0)        */
  cb->rv    = NULL;     /* rv[j][k] = k-th coeff of j-th vector             */
  cb->cv    = NULL;     /* *cv[j] = covariance matrix of j-th vector        */
  cb->type  = COV_DIAGONAL; 
  cb->featX = -1;       /* feature space index used by the feature set      */
  cb->pi    = 0;        /* the precalculated value of log((2 PI)^dimN)      */
  cb->cfg   = &defaultCbcfg;
  cb->count = NULL;

  cb->rvC   = NULL;      /* compressed means                                */
  cb->cvC   = NULL;      /* compressed semi-tied radial covariances         */
  cb->detC  = NULL;      /* direct access to determinats                    */
  cb->featY = -1;        /* compressed feature                              */

  cb->accu  = NULL;
  cb->map   = NULL;
 
  link(&defaultCbcfg,"Cbcfg");

  return TCL_OK;
}

static Codebook* cbCreate( char* name)
{
  Codebook* cbP = (Codebook*)malloc(sizeof(Codebook));

  if ((!cbP) || (cbInit(cbP, (ClientData)name) != TCL_OK)) {
    if (cbP) free (cbP);
    ERROR("Cannot create Codebook object.\n");
    return NULL;
  }
  return cbP;
}

/* ------------------------------------------------------------------------
    cbSetup
   ------------------------------------------------------------------------ */

static int cbSetup(Codebook* cb, int refN, int dimN, CovType type, int featX)
{
  assert(cb);
  assert(cb->name);

  if (cb->rv || cb->cv || cb->count) {
    WARN("cbSetup: inconsistent allocation state\n");
  }
  if (refN > CBX_MAX+1) {
    ERROR("cbSetup: maximum nr. of gaussians (CBX_MAX= %d) exceeded in %s\n",
	  CBX_MAX,cb->name);
    return TCL_ERROR;
  }

  cb->dimN   = dimN;
  cb->refN   = refN;
  cb->type   = type;
  cb->pi     = log(2.0 * PI) * (float)dimN;
  cb->featX  = featX;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbAlloc    allocate reference vectors and covariance matrices
   ------------------------------------------------------------------------ */

static int cbAlloc(Codebook* cb)
{
  int i;
  
  assert(cb);
  assert(cb->name);

  if (cb->rv && cb->cv && cb->count) 
    return TCL_OK;

  if (cb->rv || cb->cv || cb->count) {
    WARN("cbAlloc: inconsistent allocation state\n");
  }

  cb->rv = fmatrixCreate(cb->refN,cb->dimN);
  if (NULL == cb->rv) {
    ERROR("allocation failure in cbAlloc %s refN= %d  dimN= %d\n",\
	  cb->name,cb->refN,cb->dimN);
    return TCL_ERROR;
  }

  cb->count = (float*)malloc(sizeof(float)*cb->refN);
  if (NULL == cb->count) {
    ERROR("allocation failure in cbAlloc %s refN= %d  dimN= %d\n",\
	  cb->name,cb->refN,cb->dimN);
    return TCL_ERROR;
  }

  cb->cv = (FCovMatrix**)malloc(sizeof(FCovMatrix*)*cb->refN);
  if (NULL == cb->cv) {
    ERROR("allocation failure in cbAlloc %s refN= %d  dimN= %d\n",\
	  cb->name,cb->refN,cb->dimN);
    return TCL_ERROR;
  }

  for (i=0; i<cb->refN; i++) {
    cb->cv[i]=  fcvCreate(cb->dimN, cb->type);
    if (NULL == cb->cv[i]) {
      ERROR("allocation failure in cbAlloc %s refN= %d  dimN= %d\n",\
	    cb->name,cb->refN,cb->dimN);
      return TCL_ERROR;
    }
  }

  for (i=0; i<cb->refN; i++) cb->count[i] = 0.0;
 
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbAllocC    allocate compressed codebook structure
   ------------------------------------------------------------------------ */

static int cbAllocC(Codebook* cbP, int mode)
{
  int  refX;
  int  refN = cbP->refN;
  int  dimN = cbP->dimN;

  if (cbP->rvC) {
    if (cbP->rvC[0]) free(cbP->rvC[0]);
    free(cbP->rvC);
  }
  if (cbP->cvC) {
    if (cbP->cvC[0]) free(cbP->cvC[0]);
    free(cbP->cvC);
  }
  if (cbP->detC) {
    free(cbP->detC);
  }
  cbP->rvC  = NULL;
  cbP->cvC  = NULL;
  cbP->detC = NULL;

  if (NULL == (cbP->detC = (float*) memalign(8,refN*sizeof(float)))) {
    ERROR("cbAllocC: allocation failure");
    return TCL_ERROR;
  } 
  if (NULL == (cbP->rvC = (UCHAR**) memalign(8,refN*sizeof(UCHAR*)))) {
    ERROR("cbAllocC: allocation failure");
    return TCL_ERROR;
  }  
  if (NULL == (cbP->rvC[0] = (UCHAR*) memalign(8,refN*dimN*sizeof(UCHAR)))) {
    ERROR("cbAllocC: allocation failure");
    return TCL_ERROR;
  }
  for (refX=1;refX<refN;refX++) 
    cbP->rvC[refX] = cbP->rvC[refX-1]+dimN;
      
  if (NULL == (cbP->cvC = (UCHAR**) memalign(8,refN*sizeof(UCHAR*)))) {
    ERROR("cbAllocC: allocation failure");
    return TCL_ERROR;
  }
  if (mode == 1) {
    if (NULL == (cbP->cvC[0] = (UCHAR*) memalign(8,refN*dimN*sizeof(UCHAR)))) {
      ERROR("cbAllocC: allocation failure");
      return TCL_ERROR;
    }
    for (refX=1;refX<refN;refX++) 
      cbP->cvC[refX] = cbP->cvC[refX-1]+dimN;
  } else {
    if (NULL == (cbP->cvC[0] = (UCHAR*) memalign(8,refN*dimN/4*sizeof(UCHAR)))) {
      ERROR("cbAllocC: allocation failure");
      return TCL_ERROR;
    }
    for (refX=1;refX<refN;refX++) 
      cbP->cvC[refX] = cbP->cvC[refX-1]+dimN/4;
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbAllocItf
   ------------
    Ensures that a codebook gets allocated properly
    (Needed for example to run neuralGas on a codebook)
    ----------------------------------------------------------------------- */

static int cbAllocItf (ClientData cd, int argc, char *argv[])
{
  Codebook* cbP = (Codebook*) cd;
  int         c = 0;
  int         m = 0;
 
  if ( itfParseArgv( argv[0], &argc, argv+1, 0,
      "-compress", ARGV_INT, NULL, &c, NULL, "compressed codebook",
      "-mode",     ARGV_INT, NULL, &m, NULL, "mode for compressed codebooks (ask Hagen at soltau@ira.uka.de)",
      NULL) != TCL_OK) return TCL_ERROR;
 
  if (c) return cbAllocC (cbP, m);
  else   return cbAlloc  (cbP);
}


/* ------------------------------------------------------------------------
    cbDealloc    deallocates reference vectors and covariance matrices
   ------------------------------------------------------------------------ */

static int cbDealloc(Codebook* cb)
{
  assert(cb);

  if (cb->rv && cb->cv) {
    int refA= cb->refN;
    int refB= cb->rv->m;
    int dimA= cb->dimN;
    int dimB= cb->rv->n;
    int i;
    if (refA != refB) {
      ERROR("%s refN mismatch refN: %d  rv: %d\n",cb->name,refA,refB);
      return TCL_ERROR;
    }
    if (dimA != dimB) {
      ERROR("%s dimN mismatch dimN: %d  rv: %d\n",cb->name,dimA,dimB);
      return TCL_ERROR;
    }
    for (i=0;i<cb->refN;i++) 
      assert(dimA == cb->cv[i]->dimN);
  }

  if ( cb->rv)    { fmatrixFree(cb->rv);   cb->rv    = NULL; }
  if ( cb->count) { free(cb->count);       cb->count = NULL; }    
  if ( cb->accu)  { cbAccuFree( cb->accu); cb->accu  = NULL; }
  if ( cb->map)   { cbMapFree(  cb->map);  cb->map   = NULL; }
  if ( cb->cv) {
    int  i;
    for (i=0; i<cb->refN; i++) if (cb->cv[i]) fcvFree(cb->cv[i]);     
    free( cb->cv); 
    cb->cv = NULL;
  } 
    
  return TCL_OK;
}
/* ------------------------------------------------------------------------
    cbDeallocC    free compressed codebook structure
   ------------------------------------------------------------------------ */

static int cbDeallocC(Codebook* cbP)
{
  if (cbP->rvC) {
    if (cbP->rvC[0]) free(cbP->rvC[0]);
    free(cbP->rvC);
  }
  if (cbP->cvC) {
    if (cbP->cvC[0]) free(cbP->cvC[0]);
    free(cbP->cvC);
  }
  if (cbP->detC) {
    free(cbP->detC);
  }
  cbP->rvC  = NULL;
  cbP->cvC  = NULL;
  cbP->detC = NULL;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbDeinit     deinitialize codebook structure
   ------------------------------------------------------------------------ */

static int cbDeinit (Codebook* cb)
{
  assert(cb);

  if (!_bmemCBSP && cb->name) 
    free(cb->name); 
  cb->name = NULL; 

  unlink(cb->cfg,"Cbcfg");
  cbDeallocC( cb);
  return cbDealloc( cb);
}

static int cbFree( Codebook* cb)
{
  if ( cbDeinit( cb) != TCL_OK) return TCL_ERROR;
  if ( cb) free( cb);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbConfigureItf   configure members of a Codebook object 
   ------------------------------------------------------------------------ */

static int cbConfigureItf(ClientData cd, char *var, char *val)
{
  Codebook *cb = (Codebook*)cd;
  char countRange[40];
  int refX=0;

  if (! cb) return TCL_ERROR;
  if (! var) {
    ITFCFG(cbConfigureItf,cd,"name");
    ITFCFG(cbConfigureItf,cd,"useN");
    ITFCFG(cbConfigureItf,cd,"dimN");
    ITFCFG(cbConfigureItf,cd,"refN");
    ITFCFG(cbConfigureItf,cd,"refMax");
    ITFCFG(cbConfigureItf,cd,"featX");
    ITFCFG(cbConfigureItf,cd,"featY");
    ITFCFG(cbConfigureItf,cd,"bbiX");
    ITFCFG(cbConfigureItf,cd,"bbiY");
    ITFCFG(cbConfigureItf,cd,"type");
    ITFCFG(cbConfigureItf,cd,"cfg");
    if (cb->count) {
      sprintf(countRange,"count(0..%d)",cb->refN-1);
      ITFCFG(cbConfigureItf,cd,countRange);
    }
    return TCL_OK;
  }
  ITFCFG_CharPtr(var,val,"name",  cb->name,  1);
  ITFCFG_Int(    var,val,"useN",  cb->useN,  1);
  ITFCFG_Int(    var,val,"dimN",  cb->dimN,  1);
  ITFCFG_Int(    var,val,"refN",  cb->refN,  1); 
  ITFCFG_Int(    var,val,"refMax",cb->refMax,  0);
  ITFCFG_Int(    var,val,"featX", cb->featX, 1);
  ITFCFG_Int(    var,val,"featY", cb->featY, 0);
  ITFCFG_Int(    var,val,"bbiX",  cb->bbiX,  1);
  ITFCFG_Int(    var,val,"bbiY",  cb->bbiY,  1);
  ITFCFG_Object( var,val,"cfg",   cb->cfg, name, Cbcfg, 0);

  if (! strcmp(var,"type")) {
    if (! val) { itfAppendElement("%s",cvTypeToStr(cb->type)); 
                 return TCL_OK; }
    return TCL_OK;
  }

  if (sscanf(var,"count(%d)",&refX)==1)
  { 
     float dummy;
     if (!cb->count) return TCL_OK;
     if (! val) { itfAppendElement("%f",cb->count[refX]); return TCL_OK; }
     if (sscanf(val,"%f",&dummy)!=1) { ERROR("Expected float, got %s\n",val); return TCL_ERROR; }
     cb->count[refX]=dummy;
     return TCL_OK;
  }
  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;   
}

/* ------------------------------------------------------------------------
    cbAccessItf    access typed subitems in a Codebook
   ------------------------------------------------------------------------ */

static FMatrix refVector;

static ClientData cbAccessItf (ClientData cd, char *name, TypeInfo **ti)
{
  Codebook* cb = (Codebook*)cd;
  int       refX;

  if ( *name == '.') {
    if ( name[1] =='\0') { 
      if ( cb->accu) itfAppendElement("accu");
      if ( cb->map)  itfAppendElement("map");
      if ( cb->cfg)  itfAppendElement("cfg");
      if ( cb->refN > 0 && cb->rv && cb->cv) {
        itfAppendElement("cov(0..%d)", cb->refN-1);
        itfAppendElement("ref(0..%d)", cb->refN-1);
        itfAppendElement("mat");
      }
      *ti=NULL;
      return NULL; 
    }
    else {
      if (sscanf(name+1,"cov(%d)",&refX)==1) {
	if (!cb->cv) cbAlloc(cb);
        *ti = itfGetType("FCovMatrix"); 
        if ( refX >= 0 && refX < cb->refN)
             return (ClientData)(cb->cv[refX]);
        else return  NULL;
      }
      else if (sscanf(name+1,"ref(%d)",&refX)==1) { 
	if (!cb->rv) cbAlloc(cb);
	refVector.n     = cb->dimN;
	refVector.m     = 1;
	refVector.count = 0;
	refVector.matPA = &(cb->rv->matPA[refX]);
        *ti = itfGetType("FMatrix");
        if ( refX >= 0 && refX < cb->refN)
	     return (ClientData)(&refVector);
        else return  NULL;
      }
      else if (strcmp(name+1,"mat")==0) { 
	if (!cb->rv) cbAlloc(cb);
        *ti = itfGetType("FMatrix"); 
        return (ClientData)(cb->rv);
      }
      else if (cb->accu && strcmp(name+1,"accu")==0) { 
        *ti = itfGetType("CodebookAccu"); 
        return (ClientData)(cb->accu);
      }
      else if (cb->map && strcmp(name+1,"map")==0) { 
        *ti = itfGetType("CodebookMap"); 
        return (ClientData)(cb->map);
      }
      else if (cb->cfg && strcmp(name+1,"cfg")==0) { 
        *ti = itfGetType("Cbcfg"); 
        return (ClientData)(cb->cfg);
      }
      *ti=NULL; return NULL;
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    cbPutsItf    print codebook matrix to TCL string
   ------------------------------------------------------------------------ */

static int cbPutsItf (ClientData cd, int argc, char *argv[])
{
  Codebook* cb   = (Codebook*)cd;
  int       refN =  cb->refN;
  int       dimN =  cb->dimN;
  int       i,j;

  if (! cb->rv) return TCL_OK;

  for (i=0; i<refN; i++) {
    itfAppendResult("{");
    for (j=0; j<dimN; j++) itfAppendResult(" % 2.4e", cb->rv->matPA[i][j]);
    itfAppendResult(" }%c", i<refN-1?'\n':'\0');
  }
  return TCL_OK;
}
/* ------------------------------------------------------------------------
    cbCreateAccu   allocate a Codebook accumulator
   ------------------------------------------------------------------------ */

static int cbCreateAccu( Codebook* cb, int subN)
{
  CodebookAccu* cba =  NULL;

  cbAlloc(cb);
  if ((cba = cbAccuCreate(cb,subN))) {
    if ( cb->accu) cbAccuFree( cb->accu);
    cb->accu = cba;
    return TCL_OK;
  }
  return TCL_ERROR;
}

static int cbCreateAccuItf (ClientData cd, int argc, char *argv[])
{
  int   ac       = argc-1;
  int   subN     = 1;
  Codebook*  cb  = (Codebook*)cd;

  if ( itfParseArgv(argv[0], &ac, argv+1, 1,
      "-subN",     ARGV_INT,   NULL, &subN,      NULL, "number of subaccumulators",
       NULL) != TCL_OK) return TCL_ERROR;

  return cbCreateAccu( cb, subN);
}

/* ------------------------------------------------------------------------
    cbFreeAccu     remove the Codebook accumulator
   ------------------------------------------------------------------------ */

int cbFreeAccu( Codebook* cb)
{
  if ( cb->accu) cbAccuFree( cb->accu);
  cb->accu = NULL;
  return TCL_OK;
}

static int cbFreeAccuItf( ClientData cd, int argc, char *argv[])
{
  Codebook* cb = (Codebook*)cd;
  int       ac =  argc-1;

  if ( itfParseArgv(argv[0], &ac, argv+1, 1, NULL) != TCL_OK) return TCL_ERROR;
  return cbFreeAccu( cb);
}

/* ------------------------------------------------------------------------
    cbMatch       check if two codebooks are matching such that copy, 
                  add etc. should work
   ------------------------------------------------------------------------ */

#if USEFUL
static int cbMatch( Codebook* dst, Codebook* src)
{
  if ( dst->dimN != src->dimN || dst->refN != src->refN) {
    SERROR( "Codebook dimensions do not match %s:%dx%d vs %s:%dx%d.\n",
             dst->name, dst->refN, dst->dimN, src->name, src->refN, src->dimN);
    return TCL_ERROR;
  }
  if ( dst->type != src->type) {
    SERROR( "Codebook covariance types do not match.\n");
    return TCL_ERROR;
  }
  if ( dst->featX != src->featX) {
    SERROR( "Codebooks %s and %s are based on different features!\n",
             dst->name, src->name);
    return TCL_ERROR;
  }
  return TCL_OK;
}
#endif

/* ------------------------------------------------------------------------
    cbCopy      copy one codebook into another
   ------------------------------------------------------------------------ */

static int cbCopy( Codebook* dst, Codebook* src)
{
  int featX = dst->featX;
  int dimN  = src->dimN, dimX;
  int refN  = src->refN, refX;

  if ( src == dst) return TCL_OK;
  if (!src->rv)    return TCL_OK;

  if ( cbDealloc(dst) != TCL_OK ||
       cbSetup(dst, refN, dimN, src->type, featX) != TCL_OK ||
       cbAlloc(dst) != TCL_OK)
    return TCL_ERROR;

  for ( refX = 0; refX < refN; refX++) {
    for ( dimX = 0; dimX < dimN; dimX++)
      dst->rv->matPA[refX][dimX] = src->rv->matPA[refX][dimX];
    fcvCopy( dst->cv[refX], src->cv[refX]);
  }  
  if ( src->accu) {
    dst->accu = cbAccuCreate( dst, src->accu->subN);
    cbAccuCopy( dst->accu, src->accu);
  }
  return TCL_OK;
}

static int cbCopyItf (ClientData cd, int argc, char *argv[])
{
  Codebook* src = NULL;
  int       ac  = argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<source>",  ARGV_OBJECT, NULL, &src, "Codebook", "name of the source codebook",
       NULL) != TCL_OK) return TCL_ERROR;

  return cbCopy((Codebook*)cd, src);
}

/* -------------------------------------------------- */
/* add two codebooks (with one reference vector each) */
/* into one (with one ref vector)                     */
/* Full covariance matrices are handled properly      */
/* -------------------------------------------------- */

static int cb1Add (Codebook *cb1P, Codebook *cb2P, float n1, float n2, Codebook *cb0P) {
  float n0 = n1+n2;
  int dimX, dimN;
  int refX = 0;

  if (cb1P->refN != 1 || cb2P->refN != 1 || cb0P->refN != 1) { 
    ERROR("add method only defined for codebooks with one reference vector!\n");
    return TCL_ERROR;
  }
  if (cb1P->dimN != cb2P->dimN || cb1P->dimN != cb0P->dimN) {
    ERROR("add: dimensions do not match\n");
    return TCL_ERROR;
  }
  dimN = cb1P->dimN;
  if (n0 == 0) return TCL_ERROR;

  /* the new mean vector is just a weighted mean of the old one */
  for (dimX=0; dimX < dimN; dimX++) {
    cb0P->rv->matPA[refX][dimX] = n1/n0 * cb1P->rv->matPA[refX][dimX] + n2/n0 * cb2P->rv->matPA[refX][dimX] ;
  }

  /* the new covariance matrix */
  switch (cb0P->type) {
   case COV_NO: break;
   case COV_DIAGONAL: ERROR("codebook add: not implemented for diagonal covariance matrices!"); return TCL_ERROR;
   case COV_RADIAL: ERROR("codebook add: not implemented for radial covariance matrices!"); return TCL_ERROR;
   case COV_FULL: {
     /* ------------------------------------------------------- */
     /* we store the inverted covariance matrix -> re-invert it */
     /* ------------------------------------------------------- */
     DMatrix *dmat1, *dmat2, *dmat0;
     FCovMatrix *cv1P, *cv2P, *cv0P;
     int dimX, dimY;
     double dummy;
     dmat1 = dmatrixCreate(dimN, dimN);
     dmat2 = dmatrixCreate(dimN, dimN);
     dmat0 = dmatrixCreate(dimN, dimN);
     cv1P = cb1P->cv[refX];
     cv2P = cb2P->cv[refX];
     cv0P = cb0P->cv[refX];
     for (dimX = 0; dimX < dimN; dimX++) {
       for (dimY = 0; dimY < dimN; dimY++) {
	 dmat1->matPA[dimX][dimY] = (double)cv1P->m.f[dimX][dimY];
	 dmat2->matPA[dimX][dimY] = (double)cv2P->m.f[dimX][dimY];
       }
     }
     dmatrixInv(dmat1,dmat1); dmatrixInv(dmat2,dmat2);
     /* compute the covariance matrix */
     for (dimX = 0; dimX < dimN; dimX++) {
       for (dimY = 0; dimY < dimN; dimY++) {
	 dmat0->matPA[dimX][dimY] = n1 * dmat1->matPA[dimX][dimY] + n2 * dmat2->matPA[dimX][dimY];
	 dmat0->matPA[dimX][dimY] += n1 * cb1P->rv->matPA[refX][dimX] * cb1P->rv->matPA[refX][dimY];
	 dmat0->matPA[dimX][dimY] += n2 * cb2P->rv->matPA[refX][dimX] * cb2P->rv->matPA[refX][dimY];
	 dmat0->matPA[dimX][dimY] -= n0 * cb0P->rv->matPA[refX][dimX] * cb0P->rv->matPA[refX][dimY];
	 dmat0->matPA[dimX][dimY] /= n0;
       }
     }
     /* compute its determinant */
     cv0P->det = dmatrixLogDet(dmat0, &dummy);
     /* invert it to store it into new codebook */
     dmatrixInv(dmat0,dmat0);
     /* store it */
     for (dimX = 0; dimX < dimN; dimX++) {
       for (dimY = 0; dimY < dimN; dimY++) {
	 cv0P->m.f[dimX][dimY] = dmat0->matPA[dimX][dimY];
       }
     }
     /* de-allocate used DMatrix objects */
     dmatrixFree(dmat1); dmatrixFree(dmat0); dmatrixFree(dmat2);
   }
    break;
   default: ERROR("codebook add: unknown covariance type"); return TCL_ERROR;
  }

  return TCL_OK;
}

static int cb1AddItf(ClientData cd, int argc, char *argv[]) {
  Codebook *cb0P, *cb1P, *cb2P;
  float n1,n2;
  int ac = argc-1;
  cb0P = (Codebook *)cd;

  if (itfParseArgv(argv[0], &ac, argv+1, 1,
		   "<codebook>", ARGV_OBJECT, NULL, &cb1P, "Codebook", "first codebook",
		   "<count>",    ARGV_FLOAT,  NULL, &n1,   NULL,       "count for first codebook",
		   "<codebook>", ARGV_OBJECT, NULL, &cb2P, "Codebook", "second codebook",
		   "<count>",    ARGV_FLOAT,  NULL, &n2,   NULL,       "count for second codebook",
		   NULL) != TCL_OK) return TCL_ERROR;

  return cb1Add(cb1P, cb2P, n1, n2, cb0P);
}

/* ------------------------------------------------------------------------
    cbKullbackLeibler
   ------------------------------------------------------------------------ */

double cbKullbackLeibler(Codebook *cb1P, float *distribution1, Codebook *cb2P, 
			 float *distribution2) {
  int dimX, ref1X, ref2X;
  double dist, kl2dist = 0.0;

  if (cb1P->type != COV_DIAGONAL) {
    ERROR("Kullback-Leibler distance only implemented for DIAGONAL covariances!\n");
    return TCL_ERROR;
  }
  if (! cb1P->rv) {
    WARN("cbKullbackLeibler: didn't found codebooks\n");
    return TCL_ERROR;
  }
  if (! cb2P->rv) {
    WARN("cbKullbackLeibler: didn't found codebooks\n");
    return TCL_ERROR;
  }
  for (ref1X=0; ref1X < cb1P->refN; ref1X++) {
    float p1 = exp(-distribution1[ref1X]); 
    for (ref2X=0; ref2X < cb2P->refN; ref2X++) {
      float p2 = exp(-distribution2[ref2X]);
      for (dimX=0; dimX < cb1P->dimN; dimX++) {
	float s1square, s2square, mean1, mean2, s1, s2;
	s1square = 1.0 / cb1P->cv[ref1X]->m.d[dimX]; s2square = 1.0 / cb2P->cv[ref2X]->m.d[dimX];
	s1 = sqrt(s1square);
	s2 = sqrt(s2square);

	mean1 = cb1P->rv->matPA[ref1X][dimX];
	mean2 = cb2P->rv->matPA[ref2X][dimX];
	
	if (p1 == 0.0 || p2 == 0.0 || s1square == 0.0 || s2square == 0.0 ) {
	  WARN("Kullback-Leibler distance: strange distribution/codebook in dimension %d! (distribution values: %f, %f; variance values: %f,%f\n",
	       dimX,p1,p2,s1,s2);
	  continue;
	}
	dist = (p1 - p2) * log(p1 * s2 / (p2 * s1) );
	dist += p1/2.0 * s1square/s2square;
	dist += p2/2.0 * s2square/s1square;
	dist += (mean1-mean2) * (mean1-mean2) / 2.0 * (p2/s1square + p1/s2square);
	dist -= (p1+p2)/2.0;

	kl2dist += dist; 
      } /* for each dimension */
    }
  }
  return(kl2dist);
}
/* ------------------------------------------------------------------------
    method 'invert', 'lin2log', 'log2lin', 'noise'
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   if updateDet is true:
      update the log(det(covariance)) before inversion
   ------------------------------------------------------------------------ */
static int cbInvert (Codebook *cbP, int updateDet) {
  int dimN = cbP->dimN;
  int refN = cbP->refN;
  
  if (! cbP->rv) {
    WARN("cbInvert: didn't found codebooks\n");
    return TCL_ERROR;
  }
  switch (cbP->type) {
   case COV_NO: break;
   case COV_RADIAL: ERROR("codebook invert: not implemented for radial covariance matrices!"); return TCL_ERROR;
   case COV_DIAGONAL: {
     int i, refX;
     for (refX=0; refX < refN; refX++) {
       if (updateDet) cbP->cv[refX]->det = 0.0;
       for (i=0; i < dimN; i++) {
	 assert(cbP->cv[refX]->m.d[i]!=0.0);
	 if (updateDet) cbP->cv[refX]->det += log(cbP->cv[refX]->m.d[i]);
	 cbP->cv[refX]->m.d[i] = 1.0 / cbP->cv[refX]->m.d[i];
       }
     }
     break;
   }
   case COV_FULL: {
     int i, j, refX;
     DMatrix* buf =  dmatrixCreate( dimN, dimN);
     for (refX=0; refX < refN; refX++) {
       for (i=0; i < dimN; i++)
	 for (j=0; j < dimN; j++) 
	   buf->matPA[i][j] = cbP->cv[refX]->m.f[i][j];
       if (updateDet) {
	 double dummy;
	 cbP->cv[refX]->det = dmatrixLogDet(buf, &dummy);
       }
       dmatrixInv(buf,buf);
       for (i=0; i < dimN; i++)
	 for (j=0; j < dimN; j++) 
	   cbP->cv[refX]->m.f[i][j] = buf->matPA[i][j];
     }
     dmatrixFree(buf);
     break;
   }
   default: ERROR("codebook lin2log: unknown covariance type"); return TCL_ERROR;
  }
  return TCL_OK;
}
int cbLin2Log (Codebook *cbP) {
  int dimN = cbP->dimN;
  int refN = cbP->refN;
  switch (cbP->type) {
   case COV_NO: break;
   case COV_RADIAL: ERROR("codebook lin2log: not implemented for radial covariance matrices!"); return TCL_ERROR;
   case COV_DIAGONAL: {
     int i, refX;
     float* buf = (float*)malloc(dimN * sizeof(float));
     for (refX=0; refX < refN; refX++) {
       for (i=0; i < dimN; i++) buf[i] = cbP->rv->matPA[refX][i] * cbP->rv->matPA[refX][i];
       for (i=0; i < dimN; i++) {
	 cbP->rv->matPA[refX][i] = log(buf[i] / sqrt(buf[i] + cbP->cv[refX]->m.d[i]));
	 assert(buf[i]!=0.0);	 /* buf[i] could be 0! */
	 cbP->cv[refX]->m.d[i] = log(cbP->cv[refX]->m.d[i]/buf[i] + 1);
       }
     }
     free(buf);
     break;
   }
   case COV_FULL: {
     int i, j, refX;
     float* buf = (float*)malloc(dimN * sizeof(float));
     for (refX=0; refX < refN; refX++) {
       for (i=0; i < dimN; i++) buf[i] = cbP->rv->matPA[refX][i];
       for (i=0; i < dimN; i++) {
	 cbP->rv->matPA[refX][i] = log(buf[i]*buf[i]/sqrt(buf[i]*buf[i] + cbP->cv[refX]->m.f[i][i]));
	 for (j=0; j < dimN; j++) cbP->cv[refX]->m.f[i][j] = log(cbP->cv[refX]->m.f[i][j]/(buf[i]*buf[j]) + 1);
       }
     }
     free(buf);
     break;
   }
   default: ERROR("codebook lin2log: unknown covariance type"); return TCL_ERROR;
  }
  return TCL_OK;
}
int cbLog2Lin (Codebook *cbP) {
  int dimN = cbP->dimN;
  int refN = cbP->refN;

  if (! cbP->rv) {
    WARN("cbLog2Lin: didn't found codebooks\n");
    return TCL_ERROR;
  }
  switch (cbP->type) {
   case COV_NO: break;
   case COV_RADIAL: ERROR("codebook log2lin: not implemented for radial covariance matrices!"); return TCL_ERROR;
   case COV_DIAGONAL: {
     int i, refX;
     float buf;
     for (refX=0; refX < refN; refX++) {
       for (i=0; i < dimN; i++) {
	 buf = exp(cbP->rv->matPA[refX][i] + 0.5 * cbP->cv[refX]->m.d[i]);
	 cbP->rv->matPA[refX][i] = buf;
	 cbP->cv[refX]->m.d[i] = buf*buf * (exp(cbP->cv[refX]->m.d[i]) - 1);
       }
     }
     break;
   }
   case COV_FULL: {
     int i, j, refX;
     float* buf = (float*)malloc(dimN * sizeof(float));
     for (refX=0; refX < refN; refX++) {
       for (i=0; i < dimN; i++) buf[i] = exp(cbP->rv->matPA[refX][i] + 0.5 * cbP->cv[refX]->m.f[i][i]);
       for (i=0; i < dimN; i++) {
	 cbP->rv->matPA[refX][i] = buf[i];
	 for (j=0; j < dimN; j++) cbP->cv[refX]->m.f[i][j] = buf[i]*buf[j] * (exp(cbP->cv[refX]->m.f[i][j]) - 1);
       }
     }
     free(buf);
     break;
   }
   default: ERROR("codebook log2lin: unknown covariance type"); return TCL_ERROR;
  }
  return TCL_OK;
}

static int cbNoise (Codebook *cb0P, Codebook *cb1P, float n1, float n2) {
  int dimN = cb0P->dimN;
  int refN = cb0P->refN;

  assert(cb1P->refN == 1);
  assert(cb1P->dimN == dimN);
  if (! cb0P->rv || !cb1P) {
    WARN("cbNoise: didn't found codebooks\n");
    return TCL_ERROR;
  }
  switch (cb0P->type) {
   case COV_NO: break;
   case COV_RADIAL: ERROR("codebook noise: not implemented for radial covariance matrices!"); return TCL_ERROR;
   case COV_DIAGONAL: {
     int i, refX;
     for (refX=0; refX < refN; refX++) {
       for (i=0; i < dimN; i++) {
	 cb0P->rv->matPA[refX][i] = n1 * cb0P->rv->matPA[refX][i] + n2 * cb1P->rv->matPA[0][i];
	 cb0P->cv[refX]->m.d[i]   = n1 * n1 * cb0P->cv[refX]->m.d[i] + n2 * n2 * cb1P->cv[0]->m.d[i];
       }
     }
     break;
   }
   case COV_FULL: {
     int i, j, refX;
     for (refX=0; refX < refN; refX++) {
       for (i=0; i < dimN; i++) {
	 cb0P->rv->matPA[refX][i] = n1 * cb0P->rv->matPA[refX][i] + n2 * cb1P->rv->matPA[0][i];
	 for (j=0; j < dimN; j++) 
	   cb0P->cv[refX]->m.f[i][j] = n1 * n1 * cb0P->cv[refX]->m.f[i][j] + n2 * n2 * cb1P->cv[0]->m.f[i][j];
       }
     }
     break;
   }
   default: ERROR("codebook log2lin: unknown covariance type"); return TCL_ERROR;
  }
  return TCL_OK;
}

static int cbInvertItf(ClientData cd, int argc, char *argv[]) {
  Codebook *cb0P;
  int ac = argc-1;
  int updateDet = 0;

  cb0P = (Codebook *)cd;
  if (itfParseArgv(argv[0], &ac, argv+1, 1,
	"-updateDet", ARGV_INT, NULL, &updateDet, NULL, "update log(det(covar)) before inversion",
	NULL) != TCL_OK) return TCL_ERROR;
  return cbInvert(cb0P,updateDet);
}

static int cbLin2LogItf(ClientData cd, int argc, char *argv[]) {
  Codebook *cb0P;
  int ac = argc-1;
  cb0P = (Codebook *)cd;

  if (itfParseArgv(argv[0], &ac, argv+1, 1,
		   NULL) != TCL_OK) return TCL_ERROR;
  return cbLin2Log(cb0P);
}

static int cbLog2LinItf(ClientData cd, int argc, char *argv[]) {
  Codebook *cb0P;
  int ac = argc-1;
  cb0P = (Codebook *)cd;

  if (itfParseArgv(argv[0], &ac, argv+1, 1,
		   NULL) != TCL_OK) return TCL_ERROR;
  return cbLog2Lin(cb0P);
}

static int cbNoiseItf(ClientData cd, int argc, char *argv[]) {
  Codebook *cb0P, *cb1P;
  float n1 = 1.0;
  float n2 = 1.0;
  int ac = argc-1;
  cb0P = (Codebook *)cd;

  if (itfParseArgv(argv[0], &ac, argv+1, 1,
		   "<codebook>", ARGV_OBJECT, NULL, &cb1P, "Codebook", "noise codebook",
		   "-s",         ARGV_FLOAT,  NULL, &n1,   NULL,       "weight for speech",
		   "-n",         ARGV_FLOAT,  NULL, &n2,   NULL,       "weight for noise",
		   NULL) != TCL_OK) return TCL_ERROR;

  return cbNoise(cb0P, cb1P, n1, n2);
}
/* ------------------------------------------------------------------------
    cbSet      set reference vectors in the Codebook
   ------------------------------------------------------------------------ */

static int cbSet( Codebook* cbP, FMatrix* fmP, int refX, int dimX)
{
  int dimN = cbP->dimN;
  int refN = cbP->refN, i, j;

  if ( refX < 0 || dimX < 0) {
    ERROR("Reference/dimension index out of range.\n");
    return TCL_ERROR;
  }
  cbAlloc(cbP);
  for ( i = refX; i < refN && i < fmP->m+refX; i++) {
    for ( j = dimX; j < dimN && j < fmP->n+dimX; j++)
      cbP->rv->matPA[i][j] = fmP->matPA[i-refX][j-dimX];
  }
  return TCL_OK;
}

static int cbSetItf (ClientData cd, int argc, char *argv[])
{
  FMatrix*  fmP  = NULL;
  int       refX = 0;
  int       dimX = 0;
  int       ac   = argc-1;
  int       rc   = TCL_OK;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<matrix>", ARGV_CUSTOM, getFMatrix, &fmP,  cd,   "matrix of reference vectors", 
      "-refX",    ARGV_INT,    NULL,       &refX, NULL, "index of the reference vector",
      "-dimX",    ARGV_INT,    NULL,       &dimX, NULL, "index of the dimension",
      NULL) != TCL_OK) {
    if ( fmP) fmatrixFree( fmP);
    return TCL_ERROR;
  }
  rc = cbSet((Codebook*)cd, fmP, refX, dimX);
  if ( fmP) fmatrixFree( fmP);
  return rc;
}

/* ------------------------------------------------------------------------
    cbLoad     load a single codebook from a file written in machine
               independent format
   ------------------------------------------------------------------------ */

#define MAXCPN 512

static int cbLoad (CodebookSet *cbs, FILE *fp)
{
  int       l_refN, l_dimN, l_dimX, cbX, l_refX;
  CovType   l_cov, thisCov;
  char      l_name[MAXCPN];
  Codebook* cb = NULL;

  if (fp==NULL) return TCL_ERROR;

  /* --------------------------------- */
  /* read and compare name of codebook */
  /* --------------------------------- */

  l_name[0]='\0';

  read_string (fp, l_name);
  if (rwsRewrite (cbs->rwsP, l_name) != l_name)
    strcpy (l_name, rwsRewrite (cbs->rwsP, l_name));

  cbX=listIndex((List*)&(cbs->list),(ClientData)l_name,0);

  if (cbX>=0) 
  {
    cb = &(cbs->list.itemA[cbX]);
    cbAlloc(cb);

    /* --------------------------------- */
    /* read and compare size of codebook */
    /* --------------------------------- */
    l_refN = read_int(fp);

    if (l_refN != cb->refN) 
    {
      WARN("Size of %s is %d, expected %d (not loaded!)\n",l_name,l_refN,cb->refN);
      cbX = (-1); /* mark it as 'not to read' */
    }

    /* ----------------------------------------------------------- */
    /* read and compare size of reference vectors (dimensionality) */
    /* ----------------------------------------------------------- */
    l_dimN = read_int(fp);
    if (l_dimN!=cb->dimN) 
    {
      WARN("Dimension of %s is %d, expected %d (not loaded!)\n",l_name,l_dimN,cb->dimN);
      cbX = (-1); /* mark it as 'not to read' */
    }

    /* -------------------------------- */
    /* read and compare covariance type */
    /* -------------------------------- */
    l_cov = (CovType) read_int(fp);
  }
  else 
  {
    /* -------------------------------------------------------------- */
    /* listIndex returned error, we don't know this codebook. Read in */
    /* #of vectors, dimensionality and covariance type of the unknown */
    /* codebook, so we can overread it correctly:                     */
    /* -------------------------------------------------------------- */
    l_refN = read_int(fp);
    l_dimN = read_int(fp);
    l_cov  = (CovType) read_int(fp);     
  }

  if (cbX < 0) {
    for (l_refX=0; l_refX<l_refN; l_refX++)
    {
      if (l_cov==(COV_ERR)) (void)read_float(fp);
      for (l_dimX=0; l_dimX<l_dimN; l_dimX++) (void)read_float(fp);
      fcvSkip (l_dimN, l_cov==(COV_ERR) ? (CovType) read_int(fp) : l_cov, fp);
    }
    if (feof(fp)) { ERROR("Premature EOF (1).\n"); return TCL_ERROR; }
    return TCL_OK;
  }

  /* ------------------------- */
  /* read all codebook entries */
  /* ------------------------- */

  for (l_refX=0; l_refX<l_refN; l_refX++) 
  {
      if (l_cov==(COV_ERR)) cb->count[l_refX] = read_float(fp);
	          else cb->count[l_refX] = 0;
      /* --------------------- */
      /* read reference vector */
      /* --------------------- */
      for (l_dimX=0; l_dimX<l_dimN; l_dimX++) 
           cb->rv->matPA[l_refX][l_dimX] = read_float(fp);

      /* ---------------------- */
      /* read covariance matrix */
      /* ---------------------- */

      /* no uniform codebook covar type, then read the type from file */
      thisCov = l_cov== (COV_ERR) ? (CovType) read_int(fp) : l_cov;	

      /* check whether the read covariance type fits to the type given in cb */
      if (cb->type != -1 && cb->type != thisCov) {
	ERROR("Covariance Matrix types do not match for covariance with index %d in codebook %s\n", l_refX, cb->name);
	return TCL_ERROR;
      }

      /* not the same type as in memory, then get rid of current matrix */
      if (thisCov != (int)cb->cv[l_refX]->type)        
      {  
	/* int *backOffPtr yet to be defined */
         int dontTie = 0; 				
         fcvFree(cb->cv[l_refX]);
	 /* weightsfile says: tied, then tie */
	 if (thisCov<0 && !dontTie) cb->cv[l_refX] = cb->cv[l_refX+thisCov];     
	 if (thisCov<0 &&  dontTie) FATAL("not yet implemented\n");              
	 /* create matrix, back off and reread */
	 /* otherwise create a new matrix of correct type */
	 /* and fill it with data from the file */
	 if (thisCov>=0) 
         {  cb->cv[l_refX] = fcvCreate(cb->dimN,thisCov);            
	    fcvLoad (cb->cv[l_refX], fp);			       
         }
      }
      /* same type as in memory, then just load it in from file */
      else fcvLoad (cb->cv[l_refX], fp);               

      cb->pi=(float)log(2.0*PI)*(float)l_dimN;
  }
  for (l_refX=1; l_refX<cb->refN; l_refX++) 
      if (cb->cv[l_refX]->type != cb->cv[0]->type) { cb->type = COV_ERR; break; }
  if (l_refX==cb->refN) cb->type = cb->cv[0]->type;

  if (feof(fp)) { ERROR("Premature EOF (2).\n"); return TCL_ERROR; }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbSave    write a codebook to a file in machine independent format
   ------------------------------------------------------------------------ */

static int cbSave( Codebook* cb, FILE *fp)
{
  int refX, refY, dimX;

  cbAlloc(cb);

  write_string(fp,cb->name);
  write_int(fp,cb->refN);
  write_int(fp,cb->dimN);
  write_int(fp,(int)(-1));

  for (refX=0; refX<cb->refN; refX++) {  
    write_float(fp,cb->count[refX]);
    for (dimX=0; dimX<cb->dimN; dimX++) write_float(fp,cb->rv->matPA[refX][dimX]);
    for (refY=0; refY<refX; refY++) if (cb->cv[refY] == cb->cv[refX]) break;
    if (refY==refX)
    {  write_int(fp,(cb->cv[refX])->type);
       fcvSave(cb->cv[refX], fp);
    }
    else write_int(fp,refY-refX);
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbLoadC     load compressed codebooks
   ------------------------------------------------------------------------ */

static int cbLoadC (CodebookSet *cbs, FILE *fp, int mode)
{
  char      name[MAXCPN];
  Codebook* cbP = NULL;
  int cbX=-1;

  if (fp==NULL) return TCL_ERROR;
  if (mode != 1 && mode != 2) {
    ERROR("cbLoadC: wrong covariance compression mode\n");
    return TCL_ERROR;
  }

  read_string(fp,name);
  
  cbX=listIndex((List*)&(cbs->list),(ClientData)name,0);

  if (cbX>=0) 
  {
    cbP = &(cbs->list.itemA[cbX]);
    cbAllocC(cbP,mode);

    read_floats(fp,cbP->detC,cbP->refN);

    fread(cbP->rvC[0],sizeof(UCHAR),cbP->refN*cbP->dimN,fp); 
    if (mode ==1) 
      fread(cbP->cvC[0],sizeof(UCHAR),cbP->refN*cbP->dimN,fp); 
    else
      fread(cbP->cvC[0],sizeof(UCHAR),cbP->refN*cbP->dimN/4,fp); 
  }
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    cbSaveC   write a compressed codebook to a file 
   ------------------------------------------------------------------------ */

static int cbSaveC( Codebook* cbP, FILE *fp,int mode)
{

  if (!cbP->rvC) {
    ERROR("cbSaveC: couldn't find compressed codebooks\n");
    return TCL_ERROR;
  }
  if (mode != 1 && mode != 2) {
    ERROR("cbSaveC: wrong covariance compression mode\n");
    return TCL_ERROR;
  }
  write_string(fp,cbP->name);
  write_floats(fp,cbP->detC,cbP->refN);

  fwrite(cbP->rvC[0],sizeof(UCHAR),cbP->refN*cbP->dimN,fp); 
  if (mode == 1)
    fwrite(cbP->cvC[0],sizeof(UCHAR),cbP->refN*cbP->dimN,fp); 
  else
   fwrite(cbP->cvC[0],sizeof(UCHAR),cbP->refN*cbP->dimN/4,fp); 

  fflush(fp);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbCreateMap   allocate a CodebookMap
   ------------------------------------------------------------------------ */

static int cbCreateMap( Codebook* cb, int n)
{
  CodebookMap* map =  NULL;
  if ((map = cbMapCreate(n))) {
    if ( cb->map) cbMapFree( cb->map);
    cb->map = map;
    return TCL_OK;
  }
  return TCL_ERROR;
}

static int cbCreateMapItf (ClientData cd, int argc, char *argv[])
{
  Codebook* cb = (Codebook*)cd;
  int       ac =  argc-1;
  int       n  =  cb->refN;

  if ( itfParseArgv(argv[0], &ac, argv+1, 1,
      "<n>", ARGV_INT,  NULL, &n, NULL, "length of map",
       NULL) != TCL_OK) return TCL_ERROR;

  return cbCreateMap( cb, n);
}

/* ------------------------------------------------------------------------
    cbFreeMap     remove the Codebook map
   ------------------------------------------------------------------------ */

static int cbFreeMap( Codebook* cb)
{
  if ( cb->map) cbMapFree( cb->map);
  cb->map = NULL;
  return TCL_OK;
}

static int cbFreeMapItf( ClientData cd, int argc, char *argv[])
{
  Codebook* cb = (Codebook*)cd;
  int       ac =  argc-1;

  if ( itfParseArgv(argv[0], &ac, argv+1, 1, NULL) != TCL_OK) 
       return TCL_ERROR;
  return cbFreeMap( cb);
}

/* ------------------------------------------------------------------------
    cbMap    create a new codebook from an existing one by using a 
             CodebookMap object which defines which reference vector maps
             to which new vector and where the sub accumulators are mapped
             to. This function is used as part of the codebook splitting
             routines.
   ------------------------------------------------------------------------ */

static Codebook* cbMap( Codebook* cbP)
{
  Codebook*     cb      = NULL;
  CodebookAccu* cba     = NULL;
  CodebookMap*  mapP    = cbP->map;
  CodebookAccu* cbaP    = cbP->accu;
  int           i, refN = 0, refX, j;
  int           dimN    = cbP->dimN;

  if (! mapP) return NULL;
  cbAlloc(cbP);

  /* determine the size of the new codebook */
  for ( refX = -1, i = 0; i < mapP->itemN; i++)
    if ( refX < mapP->itemA[i].to) refX = mapP->itemA[i].to;

  refN = refX + 1;

  if (refN == 0) return NULL;

  /* allocate and initialize the new codebook */

  if (! (cb = cbCreate( cbP->name)) ||
      cbSetup(cb, refN, cbP->dimN, cbP->type, cbP->featX) != TCL_OK ||
      cbAlloc(cb) != TCL_OK) {
    cbFree( cb);
    ERROR("Can't allocate new codebook %s.\n", cbP->name);
    return NULL;
  }

  /* map the old codebook entries to the new codebook */

  for (i=0; i<mapP->itemN; i++) {
    int   from   = mapP->itemA[i].from;
    int   to     = mapP->itemA[i].to;
    float alpha0 = mapP->itemA[i].alpha0;
    float alpha  = mapP->itemA[i].alpha;

    if ( to >= 0) { 
      if ( from < 0 || from >= cbP->refN) {
        for ( j = 0; j < dimN; j++) cb->rv->matPA[to][j] *= alpha0;
        fcvAdd( cb->cv[to], cb->cv[to], alpha0, 0);
      }
      else {
        for ( j = 0; j < dimN; j++) 
          cb->rv->matPA[to][j] = alpha0 * cb->rv->matPA[to][j] + alpha  * cbP->rv->matPA[from][j];
        fcvAdd(cb->cv[to], cbP->cv[from], alpha0, alpha);
      }
    }
  }

  /* if there are accumulators allocated for the given codebook, we
     have to map them according to the subspace specification in the
     codebook map */

  if (! cbaP) return cb;

  cba = cb->accu = cbAccuCreate( cb,cbaP->subN);

  for (i=0; i<mapP->itemN; i++) {
    int   from  = mapP->itemA[i].from;
    int   to    = mapP->itemA[i].to;
    int   subX  = mapP->itemA[i].subX;

    if ( to >= 0 && from >= 0 && from < cbP->refN) {
      if ( subX < 0) {
        int   s;
        for ( s = 0; s < cba->subN; s++) {
          cba->count[s][to] += cbaP->count[s][from];
          for ( j = 0; j < dimN; j++)
            cba->rv[s]->matPA[to][j] += cbaP->rv[s]->matPA[from][j];
          dcvAdd( cba->sumOsq[s]+to, cbaP->sumOsq[s]+from, 1.0);
        }
      }
      else if ( subX > -1 && subX < cba->subN) {
        cba->count[subX][to] += cbaP->count[subX][from];
        for ( j = 0; j < dimN; j++)
          cba->rv[subX]->matPA[to][j] += cbaP->rv[subX]->matPA[from][j];
        dcvAdd( cba->sumOsq[subX]+to, cbaP->sumOsq[subX]+from, 1.0);
      }
    }
  }
  return cb;
}

/* ------------------------------------------------------------------------
    cbSplit    calls the splitting criterion evaluation function which
               returns a sorted list of potential list candidates of type
               CodebookRvRank. Based on this list a codebook map is filled
               out which defines how to split the codebook and 
               distributions.
   ------------------------------------------------------------------------ */

static int cbSplit( Codebook* cb, float beam, int n)
{
  CodebookMap*    map   =  NULL;
  CodebookRvRank* rankA =  NULL;
  int             refN  =  cb->refN, i;

  if (!cb->accu) return TCL_OK;

  if ((rankA = cbAccuSplitLR(cb->accu))) {
    int splitN = 0;

    /* return a list of the likely candidates for splitting and count
       the number of actual splits */

    for ( i = 0; i < refN; i++)
      if ( rankA[i].d > beam && (splitN < n || n < 0)) splitN++;

    if (!splitN) return TCL_OK;

    if ((map = cbMapCreate(refN + splitN))) {
      int  j = 0;

      if ( cb->map) cbMapFree( cb->map);
      cb->map = map;

      for ( splitN = 0, i = 0; i < refN; i++) {
        if ( rankA[i].d > beam && (splitN < n || n < 0)) {
          map->itemA[j  ].from  =  rankA[i].i; 
          map->itemA[j  ].to    =  j;
          map->itemA[j  ].subX  =  0;
          map->itemA[j  ].beta  =  0.5;
          map->itemA[j+1].from  =  rankA[i].i; 
          map->itemA[j+1].to    =  j+1;
          map->itemA[j+1].subX  =  1;
          map->itemA[j+1].beta  =  0.5;
          j                    +=  2;
        }
        else {
          map->itemA[j  ].from  =  rankA[i].i; 
          map->itemA[j  ].to    =  j;
          map->itemA[j  ].subX  = -1;
          j                    +=  1;
        }
      }
      return TCL_OK;
    }
    ERROR("Failed to create the CodebookMap for splitting.\n"); 
  }
  return TCL_ERROR;
}

static int cbSplitItf (ClientData cd, int argc, char *argv[])
{
  Codebook* cb   = (Codebook*)cd;
  int       ac   =  argc-1;
  int       n    = -1;
  float     beam =  0.0;

  if ( itfParseArgv(argv[0], &ac, argv+1, 1,
      "-max",  ARGV_FLOAT,NULL, &beam, NULL, "splitting beam",
      "-beam", ARGV_INT,  NULL, &n,    NULL, "max. number of splits",
       NULL) != TCL_OK) return TCL_ERROR;

  return cbSplit( cb, beam, n);
}

static int cbSplitListItf( ClientData cd, int argc, char *argv[])
{
  Codebook*       cb    = (Codebook*)cd;
  int             ac    =  argc-1;
  CodebookRvRank* rankA =  NULL;
  int             refN  =  cb->refN, i;

  if ( itfParseArgv(argv[0], &ac, argv+1, 1, NULL) != TCL_OK) 
       return TCL_ERROR;

  if (! cb->accu) return TCL_OK;

  if ((rankA = cbAccuSplitLR(cb->accu))) {

    /* return a list of the likely candidates for splitting and count
       the number of actual splits */

    for ( i = 0; i < refN; i++) {
      itfAppendResult( " {%d % 2.4e}", rankA[i].i, rankA[i].d);
    }
    return TCL_OK;
  }
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
   cbStepDiag Converts a diagonal covariance matrix into a StepDiagonal
              one, leaves the determinant invariant
   ------------------------------------------------------------------------ */

int cbStepDiag(Codebook *cb, int n, int m) {
  register int     cbvX, dimX, dimY, dimN;
  register float   r;

  float* cb_fcvVarA = NULL;
  int*   cb_fcvVarX = NULL;
  int    cb_fcvVarN = 0;

  cbAlloc(cb);

  /* Only useful for diagonal CV */
  if (cb->type != COV_DIAGONAL) {
    INFO("StepDiag only works for diagonal cvs\n");
    return TCL_OK;
  }

  /* Check reduced CVs */
  dimN = cb->dimN;
  if ((cb->cfg->rdimN > 0) && (cb->cfg->rdimN < cb->dimN)) 
    dimN = cb->cfg->rdimN;

  /* Dimensions still ok? */
  if (dimN % n != 0) {
    INFO("No integer number of steps (%d) for dimensions (%d)!\n",n,dimN); 
    return TCL_OK;
  }

  /* Preparations:
     cb_fcvVarX holds the order of the dimensions, by default this is set to
     their number, so that dimensions 0-3, 4-7, ... are grouped */
  cb_fcvVarN =  dimN;
  cb_fcvVarA = (float*)realloc( cb_fcvVarA, cb_fcvVarN * sizeof(float));
  cb_fcvVarX = (int  *)realloc( cb_fcvVarX, cb_fcvVarN * sizeof(int));
  for (dimY = 0; dimY < dimN; dimY++) cb_fcvVarX[dimY] = dimY;
  cb->pi = log(2.0 * PI) * (float)dimN;

  /* Do we average according to the average variance? (m=2) */
  if (m == 2) {
    FCovMatrix *CV = fcvCreate( dimN, COV_DIAGONAL);
    for (dimY = 0; dimY < dimN; dimY++) {
      CV->m.d[dimY] = 0.0;
      for( cbvX=0; cbvX<cb->refN; cbvX++)
	CV->m.d[dimY] += log(cb->cv[cbvX]->m.d[dimY]);
      CV->m.d[dimY] = exp(CV->m.d[dimY]/cb->refN);
    }
    fcvVariances( CV, cb_fcvVarA, cb_fcvVarX );
    free( CV);
    /*
    for (dimY = 0; dimY < dimN; dimY++)
      printf("{%d %2.4e} ", cb_fcvVarX[dimY], cb_fcvVarA[dimY]);
    printf("\n");
    fflush(stdout);
    */
  }

  /* Do it, calculate the mean covariance */
  for( cbvX=0; cbvX<cb->refN; cbvX++) {

    /* Do we average according to the widths of every specific vector? */
    if(m == 1) fcvVariances( cb->cv[cbvX], cb_fcvVarA, cb_fcvVarX );

    for( dimX=0; dimX<dimN; dimX+=n) {
      r = 0.0;
      /* geom. mean: */
      for (dimY = dimX; dimY < dimX+n; dimY++)
	r += log(cb->cv[cbvX]->m.d[cb_fcvVarX[dimY]]);
      r = exp(r/n);

      /* arithm. mean:
      for (dimY = dimX; dimY < dimX+n; dimY++)
	r += cb->cv[cbvX]->m.d[dimY];
      r = r/n;
      */

      for (dimY = dimX; dimY < dimX+n; dimY++)
	cb->cv[cbvX]->m.d[cb_fcvVarX[dimY]] = r;

    }
  }
  
  free (cb_fcvVarA);
  free (cb_fcvVarX);

  return TCL_OK;
}

static int cbStepDiagItf (ClientData cd, int argc, char *argv[]) {
  Codebook *cb = (Codebook *)cd;
  int       ac = argc - 1;
  int        n = 8;
  int        m = 2;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<modulo>",   ARGV_INT,    NULL, &n,  NULL,       "Modulo",
      "-mode",      ARGV_INT,    NULL, &m,  NULL,       "0=dimensions, 1=sorted individually, 2=sorted by average cov",
     NULL) != TCL_OK) return TCL_ERROR;

  return cbStepDiag( cb, n, m );
}

/* ------------------------------------------------------------------------
    cbMLE    computes a maximum likelihood update of a codebook and 
             recomputes the accumulator subspaces afterwards
   ------------------------------------------------------------------------ */

int cbMLE( Codebook *cb, int subX)
{
  CodebookAccu* cba = cb->accu;
  int           refX;
  float         f;

  for (refX=0; refX<cb->refN; refX++) {
    cbAccuMLE(cba, refX, subX, cb->rv->matPA[refX], cb->cv[refX], &f);
    if (subX < 0) {
      cbAccuSubspace( cba);
    }
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbMAP    computes a maximum a-posteriori update of a codebook and 
             recomputes the accumulator subspaces afterwards
   ------------------------------------------------------------------------ */

static int cbMAP( Codebook *cb, int subX)
{
  CodebookAccu* cba = cb->accu;
  int           refX;
  float         f;

  for (refX=0; refX<cb->refN; refX++) {
    cbAccuMAP(cba, refX, subX, cb->rv->matPA[refX], cb->cv[refX], &f);
    if (subX<0) {
      cbAccuSubspace( cba);
    }
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbMMIE    computes a maximum mutual information update of a
              codebook
   ------------------------------------------------------------------------ */

static int _negativeVariances = 0;

static int cbMMIE( Codebook *cbP, int subX) {
  CodebookAccu* cbaP = cbP->accu;
  int           refN = cbP->refN;
  int           dimN = cbP->dimN;
  int refX,dimX;

  /* no accus, no update */
  if (! cbaP || ! cbaP->count) return TCL_OK;

  if (cbaP->subN != 2) {
    ERROR("cbMMIE : '%s' has %d subaccus, but should be 2\n",cbP->name,cbaP->subN);
    return TCL_ERROR;
  }
  if (cbP->type != COV_DIAGONAL) {
    ERROR("cbMMIE : '%s' has wrong covariance type, only DIAGONAL supported\n",cbP->name);
    return TCL_ERROR;
  }

  for (refX=0;refX<refN;refX++) {
    double     E = cbP->cfg->E;
    double     H = cbP->cfg->H;

    /* current estimates */
    float*   rvP = cbP->rv->matPA[refX];
    float*   cvP = cbP->cv[refX]->m.d;
    double   det = 0.0;

    /* numerator statistics */
    double* rvRefP = cbaP->rv[0]->matPA[refX];
    double* cvRefP = cbaP->sumOsq[0][refX].m.d;
    double  cntRef = cbaP->count[0][refX];
 
    /* denominator statistics */
    double* rvHypP = cbaP->rv[1]->matPA[refX];
    double* cvHypP = cbaP->sumOsq[1][refX].m.d;
    double  cntHyp = cbaP->count[1][refX];

    /*cntHyp*=5;
    for(dimX=0;dimX<dimN;dimX++) {
      rvHypP[dimX]*=5;
      cvHypP[dimX]*=5;
      }*/

    /* I-smoothing */
    double     tau = (cntRef > 0) ? 1.0 + cbP->cfg->I /cntRef : 1.0;

    /* check counts */
    if (cntRef < cbP->cfg->minCvCount ) continue;
    if (cntHyp < cbP->cfg->minCvCount ) continue;

    /*while (1) {
      for (dimX=0;dimX<dimN;dimX++) {
	double mmiRV = 0.0;
	double mmiCV = 0.0;
	
	// estimate mean 
	mmiRV  = tau*rvRefP[dimX] - rvHypP[dimX] + E*cntHyp*rvP[dimX];
	mmiRV /= tau*cntRef-cntHyp+E*cntHyp;
	
	// estimate diagonal covariance 
	mmiCV  = tau*cvRefP[dimX] - cvHypP[dimX] + E*cntHyp*( 1.0/cvP[dimX] + square(rvP[dimX]));
	mmiCV /= tau*cntRef-cntHyp+E*cntHyp;
	mmiCV -= square(mmiRV);
	
	if (mmiCV < 0) break;
      }
      if (dimX < dimN) E += 0.2; else break;
      }*/
    /* INFO ("cbMMIE: '%s' used E= %f for update\n",cbP->name,E); */

    for (dimX=0;dimX<dimN;dimX++) {
      double mlRV  = 0.0;
      double mlCV  = 0.0;
      double mmiRV = 0.0;
      double mmiCV = 0.0;
      
      /* estimate mean */
      mlRV   = rvRefP[dimX]/cntRef;
      mmiRV  = tau*rvRefP[dimX] - rvHypP[dimX] + E*cntHyp*rvP[dimX];
      mmiRV /= tau*cntRef-cntHyp+E*cntHyp;
      
      /* estimate diagonal covariance */
      mlCV   = cvRefP[dimX]/cntRef - square(rvRefP[dimX]/cntRef);
      mmiCV  = tau*cvRefP[dimX] - cvHypP[dimX] + E*cntHyp*( 1.0/cvP[dimX] + square(rvP[dimX]));
      mmiCV /= tau*cntRef-cntHyp+E*cntHyp;
      mmiCV -= square(mmiRV);
	
      /* update & smoothing */
      rvP[dimX] = H*mmiRV+(1.0-H)*mlRV;
      cvP[dimX] = H*mmiCV+(1.0-H)*mlCV;
	
      /* polish covariance */
      if (cvP[dimX] <= COV_FLOOR) cvP[dimX] = COV_FLOOR;
      det += log(cvP[dimX]);
      cvP[dimX] = 1.0/cvP[dimX];
    }

    cbP->cv[refX]->det = det;

  }


  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbMPE     computes a minimum phone error update of a
              codebook
   ------------------------------------------------------------------------ */

static int cbMPE( Codebook *cbP, int subX) {
  CodebookAccu* cbaP = cbP->accu;
  int           refN = cbP->refN;
  int           dimN = cbP->dimN;
  int refX,dimX;

  /* no accus, no update */
  if (! cbaP || ! cbaP->count) return TCL_OK;

  if (cbaP->subN != 3) {
    ERROR("cbMPE : '%s' has %d subaccus, but should be 3\n",cbP->name,cbaP->subN);
    return TCL_ERROR;
  }
  if (cbP->type != COV_DIAGONAL) {
    ERROR("cbMPE : '%s' has wrong covariance type, only DIAGONAL supported\n",cbP->name);
    return TCL_ERROR;
  }

  int num_gau = 0;
  int num_good_numer = 0;
  int num_good_denom = 0;
  
  for (refX=0;refX<refN;refX++) {
    double     E = cbP->cfg->E;
    double     H = cbP->cfg->H;

    /* current estimates */
    float*   rvP = cbP->rv->matPA[refX];
    float*   cvP = cbP->cv[refX]->m.d;
    double   det = 0.0;

    /* numerator statistics */
    double* rvNumP = cbaP->rv[0]->matPA[refX];
    double* cvNumP = cbaP->sumOsq[0][refX].m.d;
    double  cntNum = cbaP->count[0][refX];
 
    /* denominator statistics */
    double* rvDenP = cbaP->rv[1]->matPA[refX];
    double* cvDenP = cbaP->sumOsq[1][refX].m.d;
    double  cntDen = cbaP->count[1][refX];

    /* ML statistics */
    double* rvMLP = cbaP->rv[2]->matPA[refX];
    double* cvMLP = cbaP->sumOsq[2][refX].m.d;
    double  cntML = cbaP->count[2][refX];

    /* I-smoothing */
    //double     tau = (cntNum > 0) ? 1.0 + cbP->cfg->I /cntML : 1.0;
    double     tau = cbP->cfg->I;
    double D = 0.0;

    if (cntML < cbP->cfg->minCvCount ) continue;
    if(cntNum >= tau)
      num_good_numer++;
    if(cntDen >= tau)
      num_good_denom++;
    num_gau++;
    /* check counts */
    fprintf(stderr, "count: %e %e %e\n", cntNum, cntDen, cntML);
    if (cntNum < cbP->cfg->minCvCount ) continue;
    if (cntDen < cbP->cfg->minCvCount ) continue;
/*
    for (dimX=0;dimX<dimN;dimX++) {
      rvNumP[dimX] += (tau/cntML)*rvMLP[dimX];
      cvNumP[dimX] += (tau/cntML)*cvMLP[dimX];
    }
    cntNum += cntML;
*/    
    for (dimX=0;dimX<dimN;dimX++) {
      double cur_D = 0.0;
      double a, b, c;
      double old_sigmasq = 1.0/cvP[dimX];
      double old_mu = rvP[dimX];
      double i_val = (tau/cntML)*rvMLP[dimX];
      double i_valsq = (tau/cntML)*cvMLP[dimX];

      a = old_sigmasq;
      b = (old_sigmasq+old_mu*old_mu)*(cntNum-cntDen+tau) + cvNumP[dimX] - cvDenP[dimX] + i_valsq - 2*old_mu*(rvNumP[dimX] - rvDenP[dimX] + i_val);
      //b = (old_sigmasq+old_mu*old_mu)*(cntNum-cntDen) + cvNumP[dimX] - cvDenP[dimX] - 2*old_mu*(rvNumP[dimX] - rvDenP[dimX]);
      c = (cvNumP[dimX]-cvDenP[dimX]+i_valsq)*(cntNum-cntDen+tau) - (rvNumP[dimX] - rvDenP[dimX] + i_val)*(rvNumP[dimX] - rvDenP[dimX] + i_val);
      //c = (cvNumP[dimX]-cvDenP[dimX])*(cntNum-cntDen) - (rvNumP[dimX] - rvDenP[dimX])*(rvNumP[dimX] - rvDenP[dimX]);

      double tmp = b*b - 4*a*c;
      if(a==0.0){
        cur_D = -c/b;
      }
      else if(tmp<0.0){
        if(b!=0.0 && fabs(tmp/(b*b)) < 0.0001){
          cur_D = -b/(2*a);
        }
        else{
          cur_D = 0.0;
        }
      }
      else{
        tmp = sqrt(tmp);
        cur_D = (tmp - b)/(2*a);
        if(cur_D < (-tmp - b)/(2*a)){
          cur_D = (-tmp - b)/(2*a);
        }
      }
      cur_D *= 2.0;

      cur_D = (cur_D < E*cntDen) ? E*cntDen : cur_D;
      D = (D < cur_D) ? cur_D : D;
    }

    for (dimX=0;dimX<dimN;dimX++) {
      double mpeRV = 0.0;
      double mpeCV = 0.0;
      double mlRV  = 0.0;
      double mlCV  = 0.0;
      double Snum=0.0, Sden=0.0, Sml=0.0;
     
      /* estimate mean */
      mlRV   = rvMLP[dimX]/cntML;
      mpeRV  = rvNumP[dimX] - rvDenP[dimX] + D*rvP[dimX] + (tau/cntML)*rvMLP[dimX];
      mpeRV /= cntNum-cntDen+D+tau;

      Snum = cvNumP[dimX] - 2*rvNumP[dimX]*mpeRV + cntNum*mpeRV*mpeRV;
      Sden = cvDenP[dimX] - 2*rvDenP[dimX]*mpeRV + cntDen*mpeRV*mpeRV;
      Sml  = cvMLP[dimX]  - 2*rvMLP[dimX]*mlRV  + cntML*mlRV*mlRV;
      
      /* estimate diagonal covariance */
      mlCV   = cvMLP[dimX]/cntML - square(rvMLP[dimX]/cntML);
      mpeCV  = Snum - Sden + D*( 1.0/cvP[dimX] + square(rvP[dimX] - mpeRV)) + (tau/cntML)*Sml;
      mpeCV /= cntNum-cntDen+D+tau;

      //fprintf(stderr, "rv: %e %e, cv: %e %e \n", rvMLP[dimX], rvNumP[dimX], cvMLP[dimX], cvNumP[dimX]);
      //fprintf(stderr, "mean: %e %e %e\n", mlRV, mpeRV, rvP[dimX]);
      //fprintf(stderr, "cov: %e %e %e %e\n", mlCV, mpeCV, 1.0/cvP[dimX], COV_FLOOR);
      
      /* update & smoothing */
      rvP[dimX] = H*mpeRV+(1.0-H)*mlRV;
      cvP[dimX] = H*mpeCV+(1.0-H)*mlCV;
//      cvP[dimX] = mlCV;
	
      /* polish covariance */
      if (cvP[dimX] <= COV_FLOOR) cvP[dimX] = COV_FLOOR;
      det += log(cvP[dimX]);
      cvP[dimX] = 1.0/cvP[dimX];
    }

    cbP->cv[refX]->det = det;

  }

  fprintf(stderr, "report: %d %d %d\n", num_good_numer, num_good_denom, num_gau);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbBMMIEc     computes a boosted MMIE-c update of a
              codebook
   ------------------------------------------------------------------------ */

static int cbBMMIEc( Codebook *cbP, int subX) {
  CodebookAccu* cbaP = cbP->accu;
  int           refN = cbP->refN;
  int           dimN = cbP->dimN;
  int refX,dimX;

  /* no accus, no update */
  if (! cbaP || ! cbaP->count) return TCL_OK;

  if (cbaP->subN != 3) {
    ERROR("cbBMMIEc : '%s' has %d subaccus, but should be 3\n",cbP->name,cbaP->subN);
    return TCL_ERROR;
  }
  if (cbP->type != COV_DIAGONAL) {
    ERROR("cbBMMIEc : '%s' has wrong covariance type, only DIAGONAL supported\n",cbP->name);
    return TCL_ERROR;
  }

  int num_gau = 0;
  int num_good_numer = 0;
  int num_good_denom = 0;
  
  for (refX=0;refX<refN;refX++) {
    double     E = cbP->cfg->E;
    double     H = cbP->cfg->H;

    /* current estimates */
    float*   rvP = cbP->rv->matPA[refX];
    float*   cvP = cbP->cv[refX]->m.d;
    double   det = 0.0;

    /* numerator statistics */
    double* rvNumP = cbaP->rv[1]->matPA[refX];
    double* cvNumP = cbaP->sumOsq[1][refX].m.d;
    double  cntNum = cbaP->count[1][refX];
 
    /* denominator statistics */
    double* rvDenP = cbaP->rv[2]->matPA[refX];
    double* cvDenP = cbaP->sumOsq[2][refX].m.d;
    double  cntDen = cbaP->count[2][refX];

    /* numerator statistics */
    double* rvNumCP = cbaP->rv[1]->matPA[refX];
    double* cvNumCP = cbaP->sumOsq[1][refX].m.d;
    double  cntNumC = cbaP->count[1][refX];
 
    /* denominator statistics */
    double* rvDenCP = cbaP->rv[2]->matPA[refX];
    double* cvDenCP = cbaP->sumOsq[2][refX].m.d;
    double  cntDenC = cbaP->count[2][refX];

    /* ML statistics */
    double* rvMLP = cbaP->rv[0]->matPA[refX];
    double* cvMLP = cbaP->sumOsq[0][refX].m.d;
    double  cntML = cbaP->count[0][refX];

    /* I-smoothing */
    //double     tau = (cntNum > 0) ? 1.0 + cbP->cfg->I /cntML : 1.0;
    double     tau = cbP->cfg->I;
    double D = 0.0;

    if (cntML < cbP->cfg->minCvCount ) continue;
    if(cntNum >= tau)
      num_good_numer++;
    if(cntDen >= tau)
      num_good_denom++;
    num_gau++;
    /* check counts */
    if (cntNum < cbP->cfg->minCvCount ) continue;
    if (cntDen < cbP->cfg->minCvCount ) continue;

    for (dimX=0;dimX<dimN;dimX++) {
      double cur_D = 0.0;
      double a, b, c;
      double old_sigmasq = 1.0/cvP[dimX];
      double old_mu = rvP[dimX];
      double i_val = (tau/cntML)*rvMLP[dimX];
      double i_valsq = (tau/cntML)*cvMLP[dimX];

      a = old_sigmasq;
      b = (old_sigmasq+old_mu*old_mu)*(cntNum-cntDen+tau) + cvNumP[dimX] - cvDenP[dimX] + i_valsq - 2*old_mu*(rvNumP[dimX] - rvDenP[dimX] + i_val);
      c = (cvNumP[dimX]-cvDenP[dimX]+i_valsq)*(cntNum-cntDen+tau) - (rvNumP[dimX] - rvDenP[dimX] + i_val)*(rvNumP[dimX] - rvDenP[dimX] + i_val);

      double tmp = b*b - 4*a*c;
      if(a==0.0){
        cur_D = -c/b;
      }
      else if(tmp<0.0){
        if(b!=0.0 && fabs(tmp/(b*b)) < 0.0001){
          cur_D = -b/(2*a);
        }
        else{
          cur_D = 0.0;
        }
      }
      else{
        tmp = sqrt(tmp);
        cur_D = (tmp - b)/(2*a);
        if(cur_D < (-tmp - b)/(2*a)){
          cur_D = (-tmp - b)/(2*a);
        }
      }
      cur_D *= 2.0;

      cur_D = (cur_D < E*cntDen) ? E*cntDen : cur_D;
      D = (D < cur_D) ? cur_D : D;
    }

    for (dimX=0;dimX<dimN;dimX++) {
      double bmmiRV = 0.0;
      double bmmiCV = 0.0;
      double mlRV  = 0.0;
      double mlCV  = 0.0;
      double Snum=0.0, Sden=0.0, Sml=0.0;
     
      /* estimate mean */
      mlRV   = rvMLP[dimX]/cntML;
      bmmiRV  = rvNumCP[dimX] - rvDenCP[dimX] + D*rvP[dimX] + (tau/cntML)*rvMLP[dimX];
      bmmiRV /= cntNumC-cntDenC+D+tau;

      Snum = cvNumCP[dimX] - 2*rvNumCP[dimX]*bmmiRV + cntNumC*bmmiRV*bmmiRV;
      Sden = cvDenCP[dimX] - 2*rvDenCP[dimX]*bmmiRV + cntDenC*bmmiRV*bmmiRV;
      Sml  = cvMLP[dimX]  - 2*rvMLP[dimX]*mlRV  + cntML*mlRV*mlRV;

      /* estimate diagonal covariance */
      mlCV   = cvMLP[dimX]/cntML - square(rvMLP[dimX]/cntML);
      bmmiCV  = Snum - Sden + D*( 1.0/cvP[dimX] + square(rvP[dimX] - bmmiRV)) + (tau/cntML)*Sml;
      bmmiCV /= cntNumC-cntDenC+D+tau;
     
      /* update & smoothing */
      rvP[dimX] = H*bmmiRV+(1.0-H)*mlRV;
      cvP[dimX] = H*bmmiCV+(1.0-H)*mlCV;
	
      /* polish covariance */
      if (cvP[dimX] <= COV_FLOOR) cvP[dimX] = COV_FLOOR;
      det += log(cvP[dimX]);
      cvP[dimX] = 1.0/cvP[dimX];
    }

    cbP->cv[refX]->det = det;

  }

  return TCL_OK;
}



/* ------------------------------------------------------------------------
    cbUpdate
   ------------------------------------------------------------------------ */

void cbUpdate(Codebook *cb, int subX)
{
  CodebookAccu *cba = cb->accu;

  if (cba==NULL) return;
  if (cb->cfg->doUpdate=='n') return;
  if (cb->cfg->method  =='m') { 
    cbMLE(cb,subX);
    return;
  }
  if (cb->cfg->method  =='b') { 
    cbMAP(cb,subX);
    return;
  }
  if (cb->cfg->method  =='d') { 
    cbMMIE(cb,subX);
    return;
  }
  if (cb->cfg->method  =='p') { 
    cbMPE(cb,subX);
    return;
  }
  if (cb->cfg->method  =='c') { 
    cbBMMIEc(cb,subX);
    return;
  }

  ERROR("Training method '%s' not known. (m= MLE, b= MAP, d= MMIE)\n",cb->cfg->method);
  return;
}

/* ------------------------------------------------------------------------
    cbUpdateWithTying
     Does the same as cbUpdate; but if covariance tying is active for the
     codebook, this will be properly handled
   ------------------------------------------------------------------------ */
static int cbUpdateWithTying (Codebook *cb) {
  IArray tying;
  iarrayInit(&tying,cb->refN);
  cbCovarUntieAll(cb, &tying);
  cbUpdate(cb,-1);
  cbCovarRetieAll(cb, &tying);
  iarrayDeinit(&tying);
  return TCL_OK;
}

static int cbUpdateWithTyingItf (ClientData cd, int argc, char *argv[]) {
  Codebook *cb = (Codebook *)cd;
  int       ac = argc - 1;
  if (!cb) return TCL_ERROR;

  if ( itfParseArgv(argv[0], &ac, argv+1, 1, NULL) != TCL_OK) return TCL_ERROR;
  return cbUpdateWithTying(cb);
}
  


/* ------------------------------------------------------------------------
    cbUpdateConst    update constant pre-factors for all Gaussians
   ------------------------------------------------------------------------ */

int cbUpdateConst (Codebook *cb) {

  register int     cbvX,dimX,dimY,dimN;
  register float   det;
  DMatrix          *cvmat;

  if (! cb->rv) {
    return TCL_OK;
  }

  dimN = cb->dimN;
  if ((cb->cfg->rdimN > 0) && (cb->cfg->rdimN < cb->dimN)) 
    dimN = cb->cfg->rdimN;
  cb->pi = log(2.0 * PI) * (float)dimN;

  switch (cb->type) {
  case COV_RADIAL:    
    for (cbvX=0; cbvX<cb->refN; cbvX++) {
      cb->cv[cbvX]->det = dimN*log(1.0/cb->cv[cbvX]->m.r);
    }
    break;

  case COV_DIAGONAL:
    for (cbvX=0; cbvX<cb->refN; cbvX++) {
      det = 0.0;
      for (dimX=0; dimX<dimN; dimX++)
        det += log (1.0 / cb->cv[cbvX]->m.d[dimX]);
      cb->cv[cbvX]->det = det;
    }
    break;

  case COV_FULL:
    cvmat = dmatrixCreate(dimN,dimN);
    for (cbvX=0; cbvX<cb->refN; cbvX++) {
      double dummy;
      /* Copy the matrix: */
      for (dimX = 0; dimX < dimN; dimX++) {
	for (dimY = 0; dimY < dimN; dimY++)
	  cvmat->matPA[dimX][dimY] = cb->cv[cbvX]->m.f[dimX][dimY];
      }
      /* Invert it */
      dmatrixInv(cvmat,cvmat);	  
      /* Compute determinant */
      cb->cv[cbvX]->det = dmatrixLogDet(cvmat, &dummy);
    }
    dmatrixFree(cvmat);
    break;

  default:
    break;
  }
  
  return TCL_OK;
}

/* ========================================================================
    CodebookSet
   ======================================================================== */

/* ------------------------------------------------------------------------
    Create CodebookSet Structure
   ------------------------------------------------------------------------ */

extern TypeInfo bbiInfo;

static int cbsInit( CodebookSet* cbsP, char* name, FeatureSet* fs, int bmem)
{
  cbsP->name        =  strdup(name);
  cbsP->useN        =  0;

  listInit((List*)&(cbsP->list), &codebookInfo, sizeof(Codebook), 
                                  cbsDefault.list.blkSize);

  cbsP->list.init    = (ListItemInit  *)cbInit;
  cbsP->list.deinit  = (ListItemDeinit*)cbDeinit;

  listInit((List *) &(cbsP->bbi), &bbiInfo, sizeof(BBITree),
                                   cbsDefault.list.blkSize);

  cbsP->bbi.init     = (ListItemInit   *) bbiInit;
  cbsP->bbi.deinit   = (ListItemDeinit *) bbiDeinit;

  cbsP->commentChar  = cbsDefault.commentChar;
  cbsP->rwsP         = cbsDefault.rwsP;
  cbsP->feat         = fs;
  cbsP->featN        = 0;
  cbsP->featXA       = NULL;
  cbsP->score        = cbsDefault.score;
  cbsP->accu         = cbsDefault.accu;
  cbsP->update       = cbsDefault.update;
  cbsP->cache        = cbcCreate( name, cbsP, 1);
  cbsP->defaultTopN  = cbsDefault.defaultTopN;
  cbsP->defaultRdimN = cbsDefault.defaultRdimN;
  cbsP->defaultBbiOn = cbsDefault.defaultBbiOn;
  cbsP->defaultExpT  = cbsDefault.defaultExpT;

  /* sliding window cache */
  cbsP->swcP         = (CbWindowCache*) malloc (sizeof (CbWindowCache));
  cbsP->swcP->mdA    = NULL, cbsP->swcP->cbL  = NULL, cbsP->swcP->ptA = NULL;
  cbsP->swcP->frX    = -1,   cbsP->swcP->matP = fmatrixCreate (0, 0), cbsP->swcP->baseP = imatrixCreate (0, 0);
  cbsP->swc_queries  = cbsP->swc_hits = 0;
  cbsP->swc_width    = 8;

  /* use all sub-accus */
  cbsP->subIndex     = -1; 

  cbsP->offset       = 0.0;
  cbsP->scaleRV      = 1.0;
  cbsP->scaleCV      = 1.0;
  cbsP->comMode      = 0;
  cbsP->x2y          = NULL;
  cbsP->gscsP        = NULL;

  cbsP->gdft_offset       = NULL;
  cbsP->gdft_frame2class  = NULL;

  /* Block memory management */
  if (bmem) {
    if (!_bmemCBSP) _bmemCBSP = bmemCreate (65536, 0);
    _bmemCBSN++;
  } 

  link(cbsP->feat, "FeatureSet");
  return TCL_OK;
}

static CodebookSet* cbsCreate (char* name, FeatureSet *fs, int bmem)
{
  CodebookSet* cbsP = (CodebookSet*)malloc( sizeof(CodebookSet));

  if (! cbsP || cbsInit( cbsP,name,fs,bmem) != TCL_OK) {
    if ( cbsP) free( cbsP);
    ERROR("Failed to allocate CodebookSet '%s'.\n", name); 
    return NULL;
  }
  return cbsP;
}

static ClientData cbsCreateItf (ClientData cd, int argc, char *argv[])
{
  FeatureSet *feat = NULL; 
  char       *name = NULL; 
  int           ac = argc;
  int         bmem = 0;

  if (itfParseArgv (argv[0], &ac, argv, 1,
      "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the codebook set",
      "<featureset>", ARGV_OBJECT, NULL, &feat, "FeatureSet", "name of the feature set",
      "-bmem",   ARGV_INT,    NULL, &bmem, NULL,         "bmem option",
       NULL) != TCL_OK) return NULL;

  return (ClientData) cbsCreate (name, feat, bmem);
}

/* ------------------------------------------------------------------------
    Free CodebookSet Structure
   ------------------------------------------------------------------------ */

static int cbsLinkN( CodebookSet* cbsP)
{
  int  useN = listLinkN((List*)&(cbsP->list)) - 1;
  if ( useN < cbsP->useN) return cbsP->useN;
  else                    return useN;
}

static int cbsDeinit (CodebookSet* cbsP)
{
  int rc;
  if (! cbsP) { ERROR("null argument"); return TCL_ERROR; }

  if ( cbsLinkN(cbsP)) { 
    SERROR("CodebookSet '%s' still in use by other objects.\n", cbsP->name);
    return TCL_ERROR;
  }
  if (cbsP->name)  { free(   cbsP->name);  cbsP->name  = NULL; }
  if (cbsP->cache) { cbcFree(cbsP->cache); cbsP->cache = NULL; }
  if (cbsP->swcP)  { /* sliding window score cache */
    fmatrixFree (cbsP->swcP->matP);
    imatrixFree (cbsP->swcP->baseP);
#ifdef SSE_OPT
    _mm_free (cbsP->swcP->mdA);
    _mm_free (cbsP->swcP->ptA);
    _mm_free (cbsP->swcP->cbL);
#else
    free (cbsP->swcP->mdA);
    free (cbsP->swcP->ptA);
    free (cbsP->swcP->cbL);
#endif
    free (cbsP->swcP);
    cbsP->swcP = NULL;
  }

  /* TODO: possibly also unlock single feature index locks, here */

  unlink( cbsP->feat, "FeatureSet");

  if (cbsP->x2y) free(cbsP->x2y);
  cbsP->x2y = NULL;

  rc = listDeinit((List*)&(cbsP->list));

  if (_bmemCBSP && !(--_bmemCBSN)) {
    bmemFree (_bmemCBSP);
    _bmemCBSP = NULL;
  }

  return rc;
}

static int cbsFree (CodebookSet* cbsP)
{
  if ( cbsDeinit( cbsP)!=TCL_OK) return TCL_ERROR;
  if ( cbsP) free(cbsP);
  return TCL_OK;
}

static int cbsFreeItf (ClientData cd)
{
  return cbsFree((CodebookSet*)cd);
}

/* ------------------------------------------------------------------------
    cbsConfigureItf   configure members of a CodebookSet object
   ------------------------------------------------------------------------ */

static int cbsConfigureItf (ClientData cd, char *var, char *val)
{
  CodebookSet* cbsP = (CodebookSet*)cd;
  if (! cbsP)  cbsP = &cbsDefault;

  if (! var) {
    ITFCFG(cbsConfigureItf,cd,"name");
    ITFCFG(cbsConfigureItf,cd,"useN");
    ITFCFG(cbsConfigureItf,cd,"defaultTopN");
    ITFCFG(cbsConfigureItf,cd,"defaultRdimN");
    ITFCFG(cbsConfigureItf,cd,"defaultBbiOn");
    ITFCFG(cbsConfigureItf,cd,"defaultExpT");
    ITFCFG(cbsConfigureItf,cd,"rewriteSet");
    ITFCFG(cbsConfigureItf,cd,"featureSet");
    ITFCFG(cbsConfigureItf,cd,"commentChar");
    ITFCFG(cbsConfigureItf,cd,"scaleRV");
    ITFCFG(cbsConfigureItf,cd,"scaleCV");
    ITFCFG(cbsConfigureItf,cd,"offset");
    ITFCFG(cbsConfigureItf,cd,"subX");
    ITFCFG(cbsConfigureItf,cd,"swc-queries");
    ITFCFG(cbsConfigureItf,cd,"swc-hits");
    ITFCFG(cbsConfigureItf,cd,"swc-width");

    return listConfigureItf( (ClientData)&(cbsP->list), var, val);
  }
  ITFCFG_CharPtr(var,val,"name",         cbsP->name,                 1);
  ITFCFG_Int    (var,val,"useN",         cbsP->useN,                 1);
  ITFCFG_Int    (var,val,"defaultTopN",  cbsP->defaultTopN,          1);
  ITFCFG_Int    (var,val,"defaultRdimN", cbsP->defaultRdimN,         1);
  ITFCFG_Int    (var,val,"defaultBbiOn", cbsP->defaultBbiOn,         1);
  ITFCFG_Int    (var,val,"defaultExpT",  cbsP->defaultExpT,          1);
  ITFCFG_Float  (var,val,"scaleRV",      cbsP->scaleRV,              0);
  ITFCFG_Float  (var,val,"scaleCV",      cbsP->scaleCV,              0);
  ITFCFG_Float  (var,val,"offset",       cbsP->offset,               0);
  ITFCFG_Object (var,val,"rewriteSet",   cbsP->rwsP,name,RewriteSet, 0);
  ITFCFG_Object (var,val,"featureSet",   cbsP->feat,name,FeatureSet, 1);
  ITFCFG_Char   (var,val,"commentChar",  cbsP->commentChar,          1);
  ITFCFG_Int    (var,val,"subX",         cbsP->subIndex,             0);
  ITFCFG_Int    (var,val,"swc-queries",  cbsP->swc_queries,          0);
  ITFCFG_Int    (var,val,"swc-hits",     cbsP->swc_hits,             0);
  ITFCFG_Int    (var,val,"swc-width",    cbsP->swc_width,            0);
  return listConfigureItf( (ClientData)&(cbsP->list), var, val);
}

/* ------------------------------------------------------------------------
    cbsAccessItf   access type subobjects of the codebook set
   ------------------------------------------------------------------------ */

static ClientData cbsAccessItf( ClientData cd, char *name, TypeInfo **ti)
{ 
  CodebookSet* cbsP  = (CodebookSet*)cd;
  int          bbiX;

  if (*name == '.') { 
    if (name[1] == '\0') {
      if ( cbsP->feat) itfAppendElement( "featureSet"); 
      if ( cbsP->rwsP) itfAppendElement( "rewriteSet"); 
      if ( cbsP->swcP && cbsP->swcP->matP)  itfAppendElement( "swCache"); 
      if ( cbsP->swcP && cbsP->swcP->baseP) itfAppendElement( "swIndex"); 
      if ( cbsP->bbi.itemN > 0) itfAppendElement( "bbi(0..%d)", cbsP->bbi.itemN-1);
    }
    else {
      if (cbsP->feat && !strcmp( name+1, "featureSet")) { 
        *ti = itfGetType("FeatureSet"); 
        return (ClientData)cbsP->feat;
      }
      else if (cbsP->rwsP && !strcmp( name+1, "rewriteSet")) { 
        *ti = itfGetType("RewriteSet"); 
        return (ClientData)cbsP->rwsP;
      }
      else if (cbsP->swcP && cbsP->swcP->matP && !strcmp( name+1, "swCache")) { 
        *ti = itfGetType("FMatrix"); 
        return (ClientData)cbsP->swcP->matP;
      }
      else if (cbsP->swcP && cbsP->swcP->baseP && !strcmp( name+1, "swIndex")) { 
        *ti = itfGetType("IMatrix"); 
        return (ClientData)cbsP->swcP->baseP;
      }
      else if (sscanf(name+1,"bbi(%d)",&bbiX) == 1) {
	*ti = itfGetType("BBITree");
	if (bbiX >= 0 && bbiX < cbsP->bbi.itemN) {
            return (ClientData) &(cbsP->bbi.itemA[bbiX]); 
        }
        else return NULL;
      }                
    }
  }
  return listAccessItf((ClientData)&(cbsP->list),name,ti);
}

/* ------------------------------------------------------------------------
    cbsPutsItf
   ------------------------------------------------------------------------ */

static int cbsPutsItf( ClientData cd, int argc, char *argv[])
{
  CodebookSet* cbsP  = (CodebookSet*)cd;
  return listPutsItf( (ClientData)&(cbsP->list), argc, argv);
}

/* ------------------------------------------------------------------------
    cbsSet
   ------------------------------------------------------------------------ */

static int cbsSet (CodebookSet *cbsP, int topN, int rdimN, int bbiOn, float expT)
{
  register int               cbX;
  
  /* -------------------------------------------------------- */
  /*  propagate topN, bbiOn, expT and rdimN to all codebooks  */
  /* -------------------------------------------------------- */
  WARN ("Changing flags in Cbcfg object '%s', this might affect other Codebooks as well!\n",
	cbsP->list.itemA[0].cfg->name);

  for (cbX = 0; cbX < cbsP->list.itemN; cbX++) {
    Codebook* cb = cbsP->list.itemA + cbX;
    if (topN >= 0)   cb->cfg->topN  = topN;
    if (rdimN >= 0)  cb->cfg->rdimN = rdimN;
    if (bbiOn >= 0)  cb->cfg->bbiOn = bbiOn;
    if (expT <= 0.0) cb->cfg->expT  = expT;
    cbUpdateConst(cb);
  }

  /* ------------------------------------ */
  /*  change pointer to scoring function  */
  /* ------------------------------------ */
  if (topN == 0) cbsP->score = scScoreTopAllBbi;
  if (topN == 1) cbsP->score = scScoreTopOneBbi;
  if (topN >  1) cbsP->score = scScoreTopNBbi;
  
  return TCL_OK;
}



/* ------------------------------------------------------------------------
    cbsSetItf
   ------------------------------------------------------------------------ */

static int cbsSetItf (ClientData cd, int argc, char *argv[])
{
  CodebookSet        *cbsP = (CodebookSet *) cd;
  int                ac    = argc-1;
  int                topN  = -1;
  int                rdimN = -1;
  int                bbiOn = -1;
  float              expT  = DEFAULT_EXPT_VALUE;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-topN",  ARGV_INT,    NULL, &topN,   NULL, "set topN scoring",
      "-rdimN", ARGV_INT,    NULL, &rdimN,  NULL, "reduce dimensionality",
      "-bbiOn", ARGV_INT,    NULL, &bbiOn,  NULL, "enable/disable BBI scoring",
      "-expT",  ARGV_FLOAT,  NULL, &expT,   NULL, "threshold for evaluating exp()",
    NULL) != TCL_OK) return TCL_ERROR;  

  return cbsSet(cbsP,topN,rdimN,bbiOn,expT);
}

/* ------------------------------------------------------------------------
    cbsAdd  add a new codebook to the CodebookSet
   ------------------------------------------------------------------------ */

static int cbsIndex(CodebookSet* cbs,  char* name);

int cbsAdd (CodebookSet* cbsP, char* name, int refN, int dimN, CovType type, 
                               int featX)
{
  Codebook* cb  = NULL;
  int       idx = cbsIndex( cbsP, name);
  int       featAX;

  if ( idx < 0) {
    idx = listNewItem( (List*)&(cbsP->list), (ClientData)name);
    cb  = cbsP->list.itemA + idx;
  } else {
    cb  = cbsP->list.itemA + idx;
    cbDealloc(cb);
  }

  for ( featAX =  0; featAX<cbsP->featN; featAX++) 
    if (cbsP->featXA[featAX]==featX) break;

  if (  featAX == cbsP->featN) { 
    cbsP->featXA                = (int*) realloc(cbsP->featXA,(cbsP->featN+1)*sizeof(int)); 
    cbsP->featXA[cbsP->featN++] = featX;
  }
  cbSetup( cb, refN, dimN, type, featX);

  return idx;
}

static int cbsAddItf( ClientData cd, int argc, char *argv[])
{
  CodebookSet* cbsP = (CodebookSet*)cd;
  int          ac   =  argc - 1;
  char*        name =  NULL;
  char*        feat =  NULL;
  int          dimN =  0;
  int          refN =  0;
  CovType      type =  COV_DIAGONAL;
  //int          idx;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<name>", ARGV_STRING, NULL, &name,  NULL, "name of the codebook",
      "<feat>", ARGV_STRING, NULL, &feat,  NULL, "name of the feature space",
      "<refN>", ARGV_INT,    NULL, &refN,  NULL, "number of reference vectors",
      "<dimN>", ARGV_INT,    NULL, &dimN,  NULL, "dimension of feature space",
      "<type>", ARGV_CUSTOM, cvGetTypeItf, &type, NULL, "type of covariance matrix {NO,RADIAL,DIAGONAL,FULL}",
      NULL) != TCL_OK) return TCL_ERROR;

  if (cbsAdd( cbsP,name,refN,dimN,type,
                   fesIndex(cbsP->feat,feat,FE_FMATRIX,dimN,1))<0)
       return TCL_ERROR; 
  else return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbsDeleteItf
   ------------------------------------------------------------------------ */

static int cbsDeleteItf( ClientData cd, int argc, char *argv[])
{
  CodebookSet* cbsP  = (CodebookSet*)cd;
  return listDeleteItf((ClientData)&(cbsP->list), argc, argv);
}
/* ------------------------------------------------------------------------
    cbsIndex
   ------------------------------------------------------------------------ */

static int cbsIndex( CodebookSet* cbs,  char* name) 
{
  return listIndex((List*)&(cbs->list), name, 0);
}

static int cbsIndexItf( ClientData clientData, int argc, char *argv[] )
{
  CodebookSet* cbs = (CodebookSet*)clientData;
  return listName2IndexItf((ClientData)&(cbs->list), argc, argv);
}

/* ------------------------------------------------------------------------
    cbsName
   ------------------------------------------------------------------------ */

char* cbsName( CodebookSet* cbs, int i) 
{
  return (i < 0 || i >= cbs->list.itemN) ? "(null)" : cbs->list.itemA[i].name;
}

static int cbsNameItf( ClientData clientData, int argc, char *argv[] )
{
  CodebookSet* cbs = (CodebookSet*)clientData;
  return listIndex2NameItf((ClientData)&(cbs->list), argc, argv);
}

/* ------------------------------------------------------------------------
    cbsReadItf  read a Codebook description file using the cbsAddItf 
                parsing function
   ------------------------------------------------------------------------ */

static int cbsReadItf (ClientData cd, int argc, char *argv[])
{
  CodebookSet* cbsP  = (CodebookSet*)cd;
  int          ac    =  argc - 1;
  char*        fname =  NULL;
  int          n;

  if ( itfParseArgv(argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &fname, NULL, "file to read from",
       NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult( "%d", n = freadAdd( cd, fname, cbsP->commentChar,
                                                  cbsAddItf));

  if (cbsP->cache) cbcDealloc(cbsP->cache);
  cbcAlloc(cbsP->cache);
  return (n >= 0) ? TCL_OK : TCL_ERROR;
}

/* ------------------------------------------------------------------------
    cbsWrite     write codebook set description to file
   ------------------------------------------------------------------------ */

static int cbsWrite (CodebookSet* cbsP, char* fileName)
{
  FILE*      fp;
  int        cbX;

  assert( cbsP && fileName);

  if ((fp=fileOpen( fileName,"w"))==NULL) {
    ERROR( "Can't write codebook set file \"%s\".\n", fileName);
    return TCL_ERROR;
  }

  fprintf( fp, "%c -------------------------------------------------------\n", cbsP->commentChar);
  fprintf( fp, "%c  Name            : %s\n",                                   cbsP->commentChar,  cbsP->name);
  fprintf( fp, "%c  Type            : CodebookSet\n",                          cbsP->commentChar);
  fprintf( fp, "%c  Number of Items : %d\n",                                   cbsP->commentChar,  cbsP->list.itemN);
  fprintf( fp, "%c  Date            : %s",                                     cbsP->commentChar,  dateString());
  fprintf( fp, "%c -------------------------------------------------------\n", cbsP->commentChar);

  for (cbX = 0; cbX < cbsP->list.itemN; cbX++) {
    Codebook* cb = cbsP->list.itemA + cbX;
      
    fprintf(fp, "%-15s ",   cb->name);
    fprintf(fp, "%-15s ",   cbsP->feat->featA[cb->featX].name);
    fprintf(fp, "%6d %6d ", cb->refN, cb->dimN);
    if (cb->cv && cb->cv[0]->type)
      fprintf(fp, "%-10s\n",  cvTypeToStr(cb->cv[0]->type));
    else {
      fprintf(fp, "%-10s\n",  cvTypeToStr(cb->type));
    }
  }
  fileClose(fileName, fp);
  return 0;
}

static int cbsWriteItf( ClientData cd, int argc, char *argv[])
{
  CodebookSet* cbsP  = (CodebookSet*)cd;
  int          ac    =  argc - 1;
  char*        fname =  NULL;
  //int          idx;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &fname, NULL, "file to write to",
       NULL) != TCL_OK) return TCL_ERROR;

  if (cbsWrite( cbsP, fname) < 0) return TCL_ERROR;
  else                                    return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbsLoad   load a codebook parameter file written in machine independent
              format using cbsSave
   ------------------------------------------------------------------------ */

#define CB_MAGIC 64207531

int cbsLoad (CodebookSet *cbs, char *fileName)
{
  FILE* fp;
  int   mode = 1;
  int   l_cbX = -1, l_cbN = 0, ret = TCL_OK;

  if ((fp=fileOpen( fileName,"r"))==NULL) { 
     ERROR( "Can't load codebook set file \"%s\".\n", fileName); 
     return TCL_ERROR;
  }

  l_cbN     =  read_int(fp);	

  /* compression mode */
  if (l_cbN < 0) {
    l_cbN *= -1;
    mode = -1;
  }

  if (l_cbN != cbs->list.itemN) { 
     WARN("Found %d codebooks, expected %d\n", l_cbN, cbs->list.itemN);
  }
  if (mode == 1) {
    for (l_cbX=0; l_cbX<l_cbN; l_cbX++) 
      if ((ret=cbLoad(cbs,fp)) != TCL_OK) break;
  }
  if (mode == -1) {
    int dimN = cbs->list.itemA[0].dimN;
    cbs->scaleRV  = read_float(fp);
    cbs->scaleCV  = read_float(fp);
    cbs->offset   = read_float(fp);
    cbs->comMode = read_int(fp);
    
    if (cbs->x2y) free(cbs->x2y);
    if (NULL == (cbs->x2y = malloc(dimN*sizeof(int)))) {
      ERROR("cbsLoad: allocation failure\n");
      return TCL_ERROR;
    }
    read_ints(fp,cbs->x2y,dimN);
    for (l_cbX=0; l_cbX<l_cbN; l_cbX++)
      if ((ret=cbLoadC(cbs,fp,cbs->comMode)) != TCL_OK) break;
  }

  fileClose(fileName, fp);
  if (ret != TCL_OK) ERROR("Couldn't load %d. codebook\n",l_cbX);
  return ret;
}

static int cbsLoadItf (ClientData cd, int argc, char *argv[])
{
  CodebookSet* cbsP  = (CodebookSet*)cd;
  int          ac    =  argc - 1;
  char*        fname =  NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &fname, NULL, "file to read from",
       NULL) != TCL_OK) return TCL_ERROR;

  return cbsLoad(cbsP, fname);
}

/* ------------------------------------------------------------------------
    cbsSave   save all codebooks in the codebook set to a file in machine
              independent format.
   ------------------------------------------------------------------------ */

/* Compression mode:
   mode ==  1 : write uncompressed codebooks
   mode == -1 : write compresssed  codebooks
*/

static int cbsSave (CodebookSet *cbs, char *fileName, int mode)
{
  FILE* fp;
  int   cbX= -1, ret = TCL_OK;

#ifdef WINDOWS
  if ((fp=fileOpen( fileName,"wb"))==NULL) { 
#else
  if ((fp=fileOpen( fileName,"w"))==NULL) { 
#endif
    ERROR( "Can't save codebook set file \"%s\".\n", fileName); 
    return TCL_ERROR;
  }
  write_int(fp,mode*cbs->list.itemN);

  if (mode == 1) {
    for (cbX=0; cbX<cbs->list.itemN; cbX++) 
      if ((ret=cbSave(&(cbs->list.itemA[cbX]),fp))<0) break; 
  }

  if (mode == -1) {
    write_float(fp,cbs->scaleRV);
    write_float(fp,cbs->scaleCV);
    write_float(fp,cbs->offset);
    write_int(fp,cbs->comMode);
    write_ints(fp,cbs->x2y,cbs->list.itemA[0].dimN);

    for (cbX=0; cbX<cbs->list.itemN; cbX++) 
      if ((ret=cbSaveC(&(cbs->list.itemA[cbX]),fp,cbs->comMode))<0) break; 
  }

  fileClose(fileName, fp);
  if (ret != TCL_OK) 
    ERROR("Couldn't save %d. codebook: %s\n",cbX,cbs->list.itemA[cbX].name);
  return ret;
}

static int cbsSaveItf (ClientData cd, int argc, char *argv[])
{
  CodebookSet* cbsP  = (CodebookSet*)cd;
  int          ac    =  argc - 1;
  char*        fname =  NULL;
  int          mode  = 1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &fname, NULL, "file to write to",
       "-mode",     ARGV_INT,    NULL, &mode, NULL,  "compression mode (-1,1)",
       NULL) != TCL_OK) return TCL_ERROR;

  if (mode != 1 && mode != -1) {
    ERROR("cbsSave: Invalid compression mode : must be 1 or -1\n");
    return TCL_ERROR;
  }
  return cbsSave(cbsP, fname, mode);
}

/* ------------------------------------------------------------------------
    cbsFrameN  get the number of frames loaded in the featureSet. The
               minimum over all different features used will be returned
   ------------------------------------------------------------------------ */

int cbsFrameN( CodebookSet* cbs, int* from, int* shift, int* ready) 
{
  return fesFrameN(cbs->feat, cbs->featXA, cbs->featN, from, shift, ready);
}


static float **realloc2dimFloat (float **ptr, int n0, int n1) 
{
  float **array0;

  if ((array0 = (float**)realloc(ptr, n0*sizeof(float*) + n0*n1*sizeof(float)))!=NULL)
  {
    int i; float *array1 = (float*)(array0+n0);
    for (i=0; i<n0; i++) array0[i]=array1+i*n1;
  }
  return array0;
}

/* ------------------------------------------------------------------------
    cbAccuFmatrix  accumulate the contents of a data matrix into a given 
                   codebook. The selection of the corresponding reference
		   vector is done solely on the euclidean distance, that is
		   without covariance or distribution value. This function
		   is used to compute initial covariances after doing kmeans.
   ------------------------------------------------------------------------ */
static int cbAccuFmatrix(Codebook *cb, FMatrix *data) {
  CodebookAccu *cba;
  float **subA, *addCount;
  int refX, subX, dimX, dataX;

  /* ------------- */
  /* sanity checks */
  /* ------------- */
  assert(cb);
  cba = cb->accu;

  if (cba && ! cba->count)
    cbAccuAllocate(cba);

  if (NULL == cba) { ERROR("cbAccuFmatrix: codebook %s has no accumulator",cb->name); return TCL_ERROR; }
  if (0 == cba->subN) { ERROR("cbCovarFromFmatrix: subN is zero"); return TCL_ERROR; }
  if (cb->dimN != data->n) { ERROR("cbCovarFromFmatrix: codebook dim (%d) doesn't match FMatrix dim (%d)",
				   cb->dimN, data->n); return TCL_ERROR; }

  /* ------------------------------------------------------- */
  /* alloc addCount array (probabilities of each ref-vector) */
  /* ------------------------------------------------------- */
  addCount = (float*)malloc(sizeof(float) * cb->refN);
  if (NULL == addCount) { ERROR("cbCovarFromFmatrix: out of memory allocating %d floats", cb->refN); return TCL_ERROR; }
  for (refX=0; refX < cb->refN; refX++) addCount[refX] = 0.0;

  /* ------------------------------------- */
  /* alloc and fill subA[refX][subX] array */
  /* ------------------------------------- */
  subA = (float **)malloc(sizeof(float*) * cb->refN + sizeof(float) * cba->subN * cb->refN);
  if (NULL == subA) { free(addCount); 
		      ERROR("cbCovarFromFmatrix: out of memory allocating %d * %d floats", cb->refN, cba->subN);
		      return TCL_ERROR; } 
  { 
    float *tmp = (float *)(subA + cb->refN);
    for (refX=0; refX < cb->refN; refX++) subA[refX] = tmp + refX * cba->subN;
  }
  for (refX=0; refX < cb->refN; refX++) 
    for (subX=0; subX < cba->subN; subX++) subA[refX][subX] = 1.0 / cba->subN;


  /* accumulate each data pattern into the accu corresponding to 
     the codebook ref vector that has the lowest euclidean distance
     to the data pattern. Do not look at distributions or covariances. */

  for (dataX=0; dataX < data->m; dataX++) {
    int bestRefX = -1; float minDist = 1e30;
    for (refX=0; refX < cb->refN; refX++) {
      float dist=0.0;
      for (dimX=0; dimX < cb->dimN; dimX++) {
	dist += (cb->rv->matPA[refX][dimX] - data->matPA[dataX][dimX]) *
         	(cb->rv->matPA[refX][dimX] - data->matPA[dataX][dimX]);
      }
      if (dist < minDist) { minDist = dist; bestRefX = refX; }
    }
    addCount[bestRefX] = 1.0;
    cbAccuAdd(cba, data->matPA[dataX], 1.0, addCount, subA,-1);
    addCount[bestRefX] = 0.0;
  }

  /* ------------------ */
  /* finished, clean up */
  /* ------------------ */
  if (addCount) free(addCount);
  if (subA) free(subA);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbAccuFmatrixItf
   ------------------------------------------------------------------------ */
 
static int cbAccuFmatrixItf (ClientData cd, int argc, char *argv[])
{
  Codebook*    cb   = (Codebook*)cd;
  int          ac   =  argc-1;
  FMatrix*     M    = 0;

  if ( itfParseArgv(argv[0], &ac, argv+1, 1,
      "<fmatrix>", ARGV_CUSTOM, getFMatrix, &M, cd, "",
       NULL) != TCL_OK) return TCL_ERROR;
  return cbAccuFmatrix( cb, M);
}

/* ------------------------------------------------------------------------
    cbsAccu    add the pattern of a given frame to the accumulator of the
               rsp. codebook.
   ------------------------------------------------------------------------ */

static int cbsAccu( CodebookSet* cbsP, int cbX, float* dsVal, int frameX, 
             float factor, float* addCount, float** subA)
{
  Codebook*      cb = cbsP->list.itemA+cbX; /* pointer to the current codebook               */
  CodebookAccu* cba = cb->accu;             /* pointer to the current codebook's accumulator */
  int          refN = cb->refN;
  float*    pattern = fesFrame (cbsP->feat, cb->featX, frameX); /* current speech frame      */
  int          refX;

  if (!pattern) { 
    ERROR("Can't get frame %d of feature %d\n", frameX, cb->featX); 
    return TCL_ERROR;
  }

  /* allocate cba on demand */
  if (cba) cbAccuAllocate(cba);

  /* First make sure, that the temporary arrays for communicating the 
     the mixture values to the distribution object */

  /* allocate addCount array (1 value per reference vector) */
  /* contains how many counts each ref vector got           */
  if (! addCount) {
    if (! cbsAddCountA || cbsAddCountN < cbsP->list.itemA[cbX].refN) {
       cbsAddCountN =  cbsP->list.itemA[cbX].refN;
       cbsAddCountA = (float*)realloc( cbsAddCountA, cbsAddCountN * sizeof(float));
    }
    addCount = cbsAddCountA;
  }

  /* allocate cbsSubA array (cba->subN vectors of 1 value per reference vector) */
  /* this is done in case the subA array has been passed as NULL pointer        */
  if (cba && !subA) {
    if (!cbsSubA || cbsSubN < cba->subN) 
      cbsSubA = realloc2dimFloat (cbsSubA, cb->refN, cba->subN);
  }

  /* Compute the scores for the frame (see e.g. sampleSetAccuCblh) */
  if (cba) {
    cba->score    = cbsP->score( cbsP, cbX, dsVal, frameX, addCount);
    cba->countSum = 0.0;
  }

  /* ------------------------------------------------------------- */
  /* compute countSum, the sum of all probs of all ref vectors of  */
  /* this codebook and therefore the total prob of the codebook    */
  /* This is (should be) one if no temperature is defined, no?     */
  /* ------------------------------------------------------------- */
  if  (cb->cfg->beta >= 0.0) {

    /* If there is a temperature defined for that codebook than do
       a rank weighted update, i.e. instead of using the a posteriori
       probabilities returned by the scoring module directly we determine
       the rank of each reference vector and update the vector weighted
       by exp(-beta * rank_of_rv[i]). If beta is large this amounts to
       a standard KMeans update. The idea of the whole approach is to 
       make sure that even vectors being far away in the sense of the
       metric defined by covariance matrix are being updated. */

    CodebookRvRank* rank = cbRankAlloc( refN);

    for ( refX = 0; refX < refN; refX++) {
      rank[refX].d = addCount[refX];
      rank[refX].i = refX;
    }
    qsort( rank, refN, sizeof(CodebookRvRank), compareDownCbRvRank);

    if (cba) { /* 'f' is introduced for vectorisation with ICC */
      float f = 0;
      for (refX = 0; refX < refN; refX++)
	f += (addCount[rank[refX].i] = exp(-(cb->cfg->beta) * (double)refX));
      cba->countSum = f;
    }

  } else { 

    if (cba) { /* 'f' is introduced for vectorisation with ICC */
      float f = 0;
      for (refX = 0; refX < refN; refX++)
	f += addCount[refX]; 
      cba->countSum = f;
    }

  }

  /* compute the probabilities for all ref vecs into addCount[refX] */
  /* now sum(addCount[refX]) is one                                 */
  if (cba) for (refX = 0; refX < refN; refX++) addCount[refX] /= cba->countSum;

  /* there are four different cba and subA combinations:                     */
  /* cba == 0, subA == 0:  don't do anything                                 */
  /* cba == 0, subA != 0:  fill subA (distrib.c needs it) assuming subN==1   */
  /* cba != 0, subA == 0:  use codebook.c's own cbsSubA (distrib don't care) */
  /* cba != 0, subA != 0:  use distrib.c's subA (called dssSubA, there)      */

  if (!cba) { 
    if (subA) for (refX = 0; refX < cb->refN; refX++) subA[refX][0] = 1.0;

  } else {
    /* ----------------------------------------------- */
    /* set sub[refX][subX] to the global array cbsSubA */
    /* if it has been passed as NULL pointer           */
    /* ----------------------------------------------- */
    float **sub = subA ? subA : cbsSubA;

    /* compute sub[refX][subX] for all sub accumulators: */
    if (cbsP->subIndex <0) 
      (void) cbAccuSubShare (cba, pattern, sub);
    else 
      for (refX = 0; refX < cb->refN; refX++) subA[refX][cbsP->subIndex] = 1.0;

    /* do the real accumulation: */
    if (cb->cfg->doAccumulate!='n')
      return cbAccuAdd (cba, pattern, factor, addCount, sub, cbsP->subIndex);
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbsCreateAccus   create an accumulator for each codebook 
   ------------------------------------------------------------------------ */

int cbsCreateAccus (CodebookSet *cbs,int subN)
{
  int i, ret = TCL_OK;

  for( i = 0; i < (cbs->list).itemN; i++)
    if ( cbCreateAccu ( cbs->list.itemA+i, subN) != TCL_OK)
         ret = TCL_ERROR;
  return ret;
}

static int cbsCreateAccusItf (ClientData cd, int argc, char *argv[])
{
  int   ac       = argc-1;
  int   subN     = 1;

  if ( itfParseArgv(argv[0], &ac, argv+1, 1,
      "-subN",     ARGV_INT,   NULL, &subN,      NULL, "number of subaccumulators",
       NULL) != TCL_OK) return TCL_ERROR;

  return cbsCreateAccus((CodebookSet *)cd,subN);
}

/* ------------------------------------------------------------------------
    cbsFreeAccus   remove all accumulators
   ------------------------------------------------------------------------ */

static int cbsFreeAccus( CodebookSet *cbs)
{
  int i,ret = TCL_OK;

  for ( i=0; i < (cbs->list).itemN; i++)
  {
    if ( cbAccuFree( cbs->list.itemA[i].accu) != TCL_OK) ret = TCL_ERROR;
    cbs->list.itemA[i].accu = NULL;
  }
  return ret;
}

static int cbsFreeAccusItf (ClientData cd, int argc, char *argv[])
{
  int ac = argc-1;

  if (   itfParseArgv( argv[0], &ac, argv+1, 0, NULL) != TCL_OK) 
         return TCL_ERROR;
  return cbsFreeAccus((CodebookSet*)cd);
}

/* ------------------------------------------------------------------------
    cbsClearAccus   clear all accumulators to zero
   ------------------------------------------------------------------------ */

int cbsClearAccus( CodebookSet *cbs, int subX)
{
  int i,ret = TCL_OK;

  for ( i=0; i < (cbs->list).itemN; i++)
    if ( cbAccuClear( cbs->list.itemA[i].accu,subX) != TCL_OK) ret = TCL_ERROR;
  return ret;
}

static int cbsClearAccusItf (ClientData cd, int argc, char *argv[])
{
  int            ac = argc-1;
  CodebookSet* cbsP = (CodebookSet*)cd;
  int          subX = cbsP->subIndex;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "-subX", ARGV_INT,  NULL, &subX,  NULL, "sub-accumulator, -1 to clear all",
        NULL) != TCL_OK) return TCL_ERROR;

  return cbsClearAccus((CodebookSet*)cd,subX);
}

/* ------------------------------------------------------------------------
    cbsLoadAccus    load all accumulators from a file written in machine
                    independent format and ADD them to the existing ones
   ------------------------------------------------------------------------ */

#define MAXHEAD 1000
#define MAXNAME 256

int cbsLoadAccus ( CodebookSet *cbs, char *filename, float addFactor)
{
  FILE *fp;  char str[MAXHEAD], name[MAXNAME], *p; 
  int inFile=0, defined=0, undefined=0, loaded=0, refNMismatch=0, 
      dimNMismatch=0, subNMismatch=0, typeMismatch=0, noAccu=0;
  int cbX, subX, refX, dimX, didRead, subN, refN, dimN;
  CovType type;
  Codebook *cb;

  if (filename==NULL) { ERROR("NULL filename\n");    return TCL_ERROR; }
  if (     cbs==NULL) { ERROR("NULL CodebookSet\n"); return TCL_ERROR; }
  if ((fp=fileOpen(filename,"r"))==NULL) { ERROR("Can't open file %s for reading.\n",filename); return TCL_ERROR; }

  /* --------------- */
  /* read the header */
  /* --------------- */

  (void)read_string(fp,str);    /* if verbosity printO("%s\n",str); */

  p=strtok(str," \t");  if (p==NULL) goto errorInHeader;
  if (strcmp(p,"VERSION")!=0)        goto errorInHeader;
  p=strtok(NULL," \t"); if (p==NULL) goto errorInHeader;
  if (strcmp(p,"3.0"))               goto errorInVersion;
  p=strtok(NULL," \t"); if (p==NULL) goto errorInHeader;
  if (strcmp(p,"CODEBOOKS")!=0)      goto errorInHeader;
  p=strtok(NULL," \t"); if (p==NULL) goto errorInHeader;
  if (strcmp(p,"ACCUMULATOR")!=0)    goto errorInHeader;

  while (!feof(fp)) {
    /* ------------- */
    /* read the data */
    /* ------------- */
    
    (void)read_string(fp,name); strcpy(name,rwsRewrite(cbs->rwsP,name));

    refN = read_int(fp); 
    dimN = read_int(fp); 
    subN = read_int(fp); 
    type = (CovType) read_int(fp);

    didRead = 0;
    if (feof(fp)) break;
    inFile++;
    
    /* ----------------------------------- */
    /* known codebook? then try to load it */
    /* ----------------------------------- */
    
    if ((cbX=listIndex((List*)&(cbs->list),(ClientData)name,0))>=0) {
      defined++;
      cb   = cbs->list.itemA + cbX;
      
      if (cb->accu == NULL)           { noAccu++;       goto dontRead; }
      if (refN     != cb->refN)       { refNMismatch++; goto dontRead; }
      if (dimN     != cb->dimN)       { dimNMismatch++; goto dontRead; }
      if (subN     != cb->accu->subN) { subNMismatch++; goto dontRead; }
      if (type     != (int)cb->type)  { typeMismatch++; goto dontRead; }

      if (!cb->accu->count)
	cbAccuAllocate(cb->accu);

      for (subX=0; subX<cb->accu->subN; subX++) {
	for (refX=0; refX<cb->refN; refX++) {
          float count =  read_float(fp);
          cb->accu->count[subX][refX] += addFactor * count;
          for (dimX=0; dimX<cb->dimN; dimX++) {
            float x = read_float(fp);
            cb->accu->rv[subX]->matPA[refX][dimX] += addFactor * x;
          }
          dcvLoad( cb->accu->sumOsq[subX]+refX, addFactor, fp);
	}
      }
      didRead=1; loaded++;
    }
    else undefined++;

    dontRead: if (didRead==0) { cbAccuSkip( subN, refN, dimN, type, fp); }
  }

  if (inFile)       INFO("%6d accumulators were found in the file\n",inFile);
  if (loaded)       INFO("%6d accumulators were loaded\n",loaded);
  if (defined)      INFO("%6d codebooks were defined\n",defined);
  if (undefined)    INFO("%6d codebooks were undefined\n",undefined);
  if (noAccu)       INFO("%6d codebooks had no accumulator\n",noAccu);
  if (refNMismatch) INFO("%6d refN mismatches occurred\n",refNMismatch);
  if (dimNMismatch) INFO("%6d dimN mismatches occurred\n",dimNMismatch);
  if (subNMismatch) INFO("%6d subN mismatches occurred\n",subNMismatch);

  itfAppendResult("%d", loaded);
  
  fileClose(filename, fp);
  return TCL_OK;
	
  errorInVersion: fileClose(filename, fp); ERROR("unsupported version %s\n",p); return TCL_ERROR;
  errorInHeader:  fileClose(filename, fp); ERROR("corrupt file header\n");      return TCL_ERROR;
}

static int cbsLoadAccusItf (ClientData cd, int argc, char *argv[])
{
  char  *filename = NULL;
  float addFactor = 1.0;
  int          ac = argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "file to read from",
      "-factor",    ARGV_FLOAT,  NULL, &addFactor,NULL, "multiplicator before adding",
       NULL) != TCL_OK) return TCL_ERROR;

  return cbsLoadAccus((CodebookSet*)cd,filename,addFactor); 
}

/* ------------------------------------------------------------------------
    cbsSaveAccus   save all codebook accumulators to a file in machine
                   independent format
   ------------------------------------------------------------------------ */

#define MAXCAT   250

int cbsSaveAccus(CodebookSet *cbs, char *filename)
{
  char  head[MAXHEAD], cat[MAXCAT];
  int   cbX, subX, refX, dimX, counter=0;
  FILE* fp;

  if (filename==NULL) { ERROR("NULL filename\n");    return TCL_ERROR; }
  if (     cbs==NULL) { ERROR("NULL CodebookSet\n"); return TCL_ERROR; }

  if ((fp=fileOpen(filename,"w"))==NULL) { 
    ERROR("Can't open file %s for writing.\n",filename); 
    return TCL_ERROR;
  }

  /* ------------------------------------- */
  /* write header (same style as Janus 2)  */
  /* ------------------------------------- */

  sprintf(head,"VERSION 3.0 CODEBOOKS ACCUMULATOR FILE\n");
  sprintf(cat,"CREATED AT TIME: %d\n",(int)time(0));	        strcat(head,cat);
  sprintf(cat,"CREATED ON HOST: ");			        strcat(head,cat);
  if (gethostname(cat,60)<0) strcpy(cat,"unknown");	        strcat(head,cat);
  sprintf(cat,"\nCREATED BY USER: %s ","?");		        strcat(head,cat);
  sprintf(cat,"(id=%d:%d)\n\n",(int)getuid(),(int)getgid());	strcat(head,cat);
  write_string(fp,head);

  /* ------------------------------------------- */
  /* now write every single codebook accumulator */
  /* ------------------------------------------- */

  for (cbX=0; cbX<cbs->list.itemN; cbX++) {   
    Codebook* cb = cbs->list.itemA + cbX; 
    if (cb==NULL || cb->accu==NULL || cb->accu->count == NULL) continue;

    write_string(fp,cb->name);		/* name of this codebook */
    write_int(   fp,cb->refN);
    write_int(   fp,cb->dimN);
    write_int(   fp,cb->accu->subN);
    write_int(   fp,cb->type);

    for (subX=0; subX<cb->accu->subN; subX++) {
      for (refX=0; refX<cb->refN; refX++) {
	write_float(fp,(float)cb->accu->count[subX][refX]);
	for (dimX=0; dimX<cb->dimN; dimX++) 
             write_float(fp,(float)cb->accu->rv[subX]->matPA[refX][dimX]);
        dcvSave( cb->accu->sumOsq[subX]+refX, fp); 
      }
    }
    counter++;
  }
  fileClose(filename, fp);

  itfAppendResult("%d", counter);
  INFO("Saved %d codebook accumulators\n", counter);
  return TCL_OK;
}

static int cbsSaveAccusItf (ClientData cd, int argc, char *argv[])
{
  char *filename = NULL;
  int         ac = argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, cd, "file to write",
       NULL) != TCL_OK) return TCL_ERROR;

  return cbsSaveAccus((CodebookSet*)cd,filename); 
}

/* ------------------------------------------------------------------------
    cbsUpdate   update all codebooks in the codebook set
   ------------------------------------------------------------------------ */

static int cbsUpdate( CodebookSet *cbs)
{
  int cbX;
  int cbN = cbs->list.itemN;

  _negativeVariances = 0;

#pragma omp parallel for if(cbN>8)
  for (cbX = 0; cbX < cbN; cbX++) { 
    Codebook *cb = cbs->list.itemA + cbX;
    IArray tying;

    /* no accus, no update */
    if (!cb->accu || !cb->accu->count)
      continue;

    iarrayInit(&tying,cb->refN);
    cbCovarUntieAll(cb, &tying);
    cbUpdate(cb,cbs->subIndex);
    cbCovarRetieAll(cb, &tying);
    iarrayDeinit(&tying);
  }
  if (_negativeVariances > 0) {
    INFO("cbsUpdate: found %d negative variances\n",_negativeVariances);
  }
  return TCL_OK;
}

static int cbsUpdateItf( ClientData cd, int argc, char *argv[])
{
  int   ac       = argc-1;
  if ( itfParseArgv(argv[0],&ac,argv+1,1,NULL) != TCL_OK) return TCL_ERROR;
  return cbsUpdate((CodebookSet *)cd);
}

/* ------------------------------------------------------------------------
    cbsSplit   split all codebooks
   ------------------------------------------------------------------------ */

static int cbsSplit( CodebookSet *cbs, float beam, int n)
{
  int i, ret = TCL_OK;

  for( i = 0; i < (cbs->list).itemN; i++)
    if ( cbSplit( cbs->list.itemA+i, beam, n) != TCL_OK) ret = TCL_ERROR;
  return ret;
}

static int cbsSplitItf (ClientData cd, int argc, char *argv[])
{
  CodebookSet* cbs  = (CodebookSet*)cd;
  int          ac   =  argc-1;
  int          n    = -1;
  float        beam =  0.0;

  if ( itfParseArgv(argv[0], &ac, argv+1, 1,
      "-beam",ARGV_FLOAT, NULL, &beam, NULL, "splitting beam",
      "-max", ARGV_INT,   NULL, &n,    NULL, "max. number of splits",
       NULL) != TCL_OK) return TCL_ERROR;

  return cbsSplit( cbs, beam, n);
}

/* ------------------------------------------------------------------------
    cbsCreateMaps   create a map for each codebook 
   ------------------------------------------------------------------------ */

static int cbsCreateMaps( CodebookSet *cbs, int n)
{
  int i, ret = TCL_OK;

  for( i = 0; i < (cbs->list).itemN; i++)
    if ( cbCreateMap( cbs->list.itemA+i, n) != TCL_OK) ret = TCL_ERROR;
  return ret;
}

static int cbsCreateMapsItf( ClientData cd, int argc, char *argv[])
{
  int   ac       = argc-1;
  int   n        = 0;

  if ( itfParseArgv(argv[0], &ac, argv+1, 1,
      "<n>", ARGV_INT, NULL, &n, NULL, "size of maps",
       NULL) != TCL_OK) return TCL_ERROR;

  return cbsCreateMaps((CodebookSet *)cd, n);
}

/* ------------------------------------------------------------------------
    cbsMap   maps all codebooks which have a map defined
   ------------------------------------------------------------------------ */

static int cbsMap( CodebookSet* cbs)
{
  int i, ret = TCL_OK;

  for( i = 0; i < (cbs->list).itemN; i++) {
    Codebook* cbP;
    if ((cbP = cbMap( cbs->list.itemA+i))) {
      cbCopy( cbs->list.itemA+i, cbP);
      cbFree( cbP);
    }
  }
  cbcDealloc( cbs->cache);
  cbcAlloc(   cbs->cache);
  return ret;
}

static int cbsMapItf( ClientData cd, int argc, char *argv[])
{
  int   ac       = argc-1;
  if ( itfParseArgv(argv[0],&ac,argv+1,1,NULL) != TCL_OK) return TCL_ERROR;
  return cbsMap((CodebookSet *)cd);
}

/* ------------------------------------------------------------------------
    cbsFreeMaps   free the maps for all codebooks 
   ------------------------------------------------------------------------ */

static int cbsFreeMaps( CodebookSet* cbs)
{
  int i, ret = TCL_OK;

  for( i = 0; i < (cbs->list).itemN; i++)
    if ( cbFreeMap( cbs->list.itemA+i) != TCL_OK) ret = TCL_ERROR;
  return ret;
}

static int cbsFreeMapsItf( ClientData cd, int argc, char *argv[])
{
  int   ac       = argc-1;
  if ( itfParseArgv(argv[0],&ac,argv+1,1,NULL) != TCL_OK) return TCL_ERROR;
  return cbsFreeMaps((CodebookSet *)cd);
}

/* ------------------------------------------------------------------------
   cbsCompress   compress means/covariances to 8bit values
   ------------------------------------------------------------------------ */

typedef struct {
  float cv;
  int   dimX;
  int   cnt;
} cvItem;

static int cmpCV(const void* a, const void* b)
{
  float x = ((cvItem*) a)->cv;
  float y = ((cvItem*) b)->cv;
  if (x > y) return -1;
  if (x < y) return  1;
  return 0;
}

/* ------------------------------------------------------------------------
    cbsCompress   compress means/covariances to 8bit values
   ------------------------------------------------------------------------ */

static int cbsCompressItf( ClientData cd, int argc, char *argv[])
{
  int              ac =  argc-1;
  CodebookSet*   cbsP = (CodebookSet *)cd;
  int             cbN = cbsP->list.itemN;
  float     cov_floor = 1.0 / COV_FLOOR;
  float       scaleRV = 1.0;
  float       scaleCV = 1.0;
  float        offset = 0.0;
  float    minFloatRV =  1e+36;
  float    maxFloatRV = -1e+36;
  float    minFloatCV =  1e+36;
  float    maxFloatCV = -1e+36;
  float       errorRV = 0.0;
  float       errorCV = 0.0;
  int  cntUnderflowRV = 0;
  int   cntOverflowRV = 0;
  int  cntUnderflowCV = 0;
  int   cntOverflowCV = 0;  
  int           cntRV = 0;
  int           cntCV = 0; 
  cvItem*         cvA = NULL;
  int cbX,refX,dimX,dimN;

  float    underflowRV = -10.0;   /* clipping */
  float     overflowRV =  10.0;   /* clipping */
  float     overflowCV =  50.0;   /* clipping */
  int          classRV =  200;    /* number of quantization classes */
  int          classCV =  200;    /* number of quantization classes */
  int            mode = 1;        /* covariance compression         */
  int      resortFeat = 0;        /* resort dimensions according to their covariances */
  int       deallocCB = 0;
  int       trainMode = 0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-underflowRV",    ARGV_FLOAT, NULL, &underflowRV,NULL, "underflow threshold",
      "-overflowRV",     ARGV_FLOAT, NULL, &overflowRV, NULL, "overflow  threshold",
      "-overflowCV",     ARGV_FLOAT, NULL, &overflowCV, NULL, "overflow  threshold", 
      "-classRV",        ARGV_INT,   NULL, &classRV,    NULL, "number of quantization classes (max 255)",
      "-classCV",        ARGV_INT,   NULL, &classCV,    NULL, "number of quantization classes (max 255)",
      "-compressCV",     ARGV_INT,   NULL, &mode,       NULL, "covariance compression mode  1,2", 
      "-resortFeat",     ARGV_INT,   NULL, &resortFeat, NULL, "resort feature dimensions", 
      "-deallocCB",      ARGV_INT,   NULL, &deallocCB,  NULL, "deallocate orginal codebooks", 
      "-trainMode",      ARGV_INT,   NULL, &trainMode,  NULL, "store compressed values in orginal codebooks",
		     NULL) != TCL_OK) return TCL_ERROR;

  if (cbsP->offset > 0 && ! trainMode && cbsP->list.itemA[0].rvC) {
    WARN("cbsCompress: Assume scaled orgininal codebooks, but doesn't seem so\n");
  }
  
  if (deallocCB + trainMode > 1) {
    ERROR("cbsCompress: cannot use trainMode and deallocate orginal cb's at same time\n");
    return TCL_ERROR;
  }

  /* same dimension for each codebook and divdeable by 4 */
  dimN = cbsP->list.itemA[0].dimN;
  if (dimN != 4 * floor(0.5+dimN/4)) {
    ERROR("cbsCompress: Codebooks cannot be compressed\n");
    return TCL_ERROR;
  }

  /* covariance sorter */
  if (NULL == (cvA = malloc(dimN*sizeof(cvItem)))) {
    ERROR("cbsCompress: allocation failure\n");
    return TCL_ERROR;
  }
  if (cbsP->x2y) free(cbsP->x2y);
  if (NULL == (cbsP->x2y = malloc(dimN*sizeof(int)))) {
    ERROR("cbsCompress: allocation failure\n");
    return TCL_ERROR;
  }

  for (dimX = 0;dimX<dimN;dimX++) {
    cvA[dimX].dimX = dimX;
    cvA[dimX].cv   = 0.0;
    cvA[dimX].cnt  = 0;
  }

  /* go to all codebooks, and extract min/max */
  for (cbX=0;cbX<cbN;cbX++) {
    Codebook *cbP = cbsP->list.itemA + cbX;
    int      refN = cbP->refN;

    if (dimN != cbP->dimN) {
      ERROR("cbsCompress: dimension mismatch at Codebook %s\n",cbP->name);
      return TCL_ERROR;
    }
    if (! cbP->rv) {
      ERROR("cbsCompress: couldn't find params for codebooks %s\n",cbP->name);
      return TCL_ERROR;
    }

    for (refX=0;refX<refN;refX++) {
      float *rv = cbP->rv->matPA[refX];
      float *cv = cbP->cv[refX]->m.d; 
      if (cbP->count[refX] < 1) continue;
      for (dimX=0;dimX<dimN;dimX++) {
	float r = (rv[dimX]-cbsP->offset)/cbsP->scaleRV;
	float c = cv[dimX]*cbsP->scaleRV*cbsP->scaleRV;
	if (r < underflowRV) {cntUnderflowRV++; continue; }
	if (r > overflowRV)  {cntOverflowRV++;  continue; }
	if (c > overflowCV)  {cntOverflowCV++;  continue; }
	if (r < minFloatRV) minFloatRV= r;
	if (c < minFloatCV) minFloatCV= c;
	if (r > maxFloatRV) maxFloatRV= r;
	if (c > maxFloatCV) maxFloatCV= c;
	if (c < cov_floor) {
	  cvA[dimX].cv  += c;
	  cvA[dimX].cnt += 1;
	}
      }    
    }
  }

  INFO("Detected %d / %d under/over flows for means (threshold was %2.2f/%2.2f)\n",\
       cntUnderflowRV,cntOverflowRV,underflowRV,overflowRV);
  INFO("Detected %d overflows for covars (threshold was %2.2f)\n",\
       cntOverflowCV,overflowCV);
  INFO("Quantization range for mean:  [%2.2f %2.2f]\n",minFloatRV,maxFloatRV);
  INFO("Quantization range for covar: [%2.2f %2.2f]\n",minFloatCV,maxFloatCV);

  /* reestimate scale, offset */
  scaleRV = classRV / (maxFloatRV - minFloatRV);
  scaleCV = classCV / maxFloatCV;  
  offset  = -1.0*minFloatRV * scaleRV;

  cntUnderflowRV = 0.0;
  cntOverflowRV  = 0.0;
  cntUnderflowCV = 0.0;
  cntOverflowCV  = 0.0;

  /* resort dimensions according to their covariances */  
  for (dimX=0;dimX<dimN;dimX++) {
    cvA[dimX].cv /=  cvA[dimX].cnt;
  }
  if (resortFeat)
    qsort(cvA,dimN,sizeof(cvItem),cmpCV); 
  for (dimX=0;dimX<dimN;dimX++) {
    cbsP->x2y[dimX] = cvA[dimX].dimX;
  }

  /* go to all codebooks, allocate memory, and dump compressed values*/
  for (cbX=0;cbX<cbN;cbX++) {
    Codebook *cbP = cbsP->list.itemA + cbX;
    int      refN = cbP->refN;
    int      dimN = cbP->dimN;

    if (! trainMode)
      cbAllocC(cbP,mode);

    for (refX=0;refX<refN;refX++) {
      float *rv = cbP->rv->matPA[refX];
      float *cv = cbP->cv[refX]->m.d;     
      UCHAR *rvC = trainMode ? NULL : cbP->rvC[refX];
      UCHAR *cvC = trainMode ? NULL : cbP->cvC[refX];
      float  det = 0.0;

      /* semi-tied radials */
      if (mode == 2) {
	for (dimX=0;dimX<dimN;dimX+=4) {
	  int   i = 0;
	  float c = 0.0;
	  int dimY0 = cbsP->x2y[dimX+0];
	  int dimY1 = cbsP->x2y[dimX+0];
	  int dimY2 = cbsP->x2y[dimX+0];
	  int dimY3 = cbsP->x2y[dimX+0];
	  float c0 = cbsP->scaleRV*cbsP->scaleRV*cv[dimY0];
	  float c1 = cbsP->scaleRV*cbsP->scaleRV*cv[dimY1];
	  float c2 = cbsP->scaleRV*cbsP->scaleRV*cv[dimY2];
	  float c3 = cbsP->scaleRV*cbsP->scaleRV*cv[dimY3];
	  if (c0 < cov_floor) {c += scaleCV*c0; i++;}
	  if (c1 < cov_floor) {c += scaleCV*c1; i++;}
	  if (c2 < cov_floor) {c += scaleCV*c2; i++;}
	  if (c3 < cov_floor) {c += scaleCV*c3; i++;}
	  if (i == 0) 
	    c= UCHAR_MAX;
	  else {
	    c = floor(0.5+c/i);
	    if (c < 0)         {cntUnderflowCV++; c=1.0;}
	    if (c > UCHAR_MAX) {cntOverflowCV++;  c=UCHAR_MAX;}
	  }
	  det += 4 * log (scaleCV / c);

	  if (trainMode) {
	    cv[dimY0] = cv[dimY1] = cv[dimY2] = cv[dimY3] = c;	    
	  } else {
	    cvC[dimX/4] = (UCHAR) c;
	  }
	}

	/* update determinat */
	if (! trainMode) {
	  if (cbP->count[refX] < 1) 
	    cbP->detC[refX] = cov_floor;
	  else
	    cbP->detC[refX] = det;
	} else {
	  if (cbP->count[refX] < 1) 
	    cbP->cv[refX]->det = cov_floor;
	  else
	    cbP->cv[refX]->det = det;
	}
      }
     
      for (dimX=0;dimX<dimN;dimX++) {
	int dimY= cbsP->x2y[dimX];
	float r0 = (rv[dimY]-cbsP->offset)/cbsP->scaleRV; 
	float c0 = cv[dimY]*cbsP->scaleRV*cbsP->scaleRV;
	long   r = 0;
	long   c = 0;
	float r2,c2;

	r = (long) floor(scaleRV*r0 + offset +0.5);
	c = (long) floor(scaleCV*c0 + 0.5);
	if (c0 >= cov_floor) c = UCHAR_MAX+1;

	if (cbP->count[refX] >= 1) {
	  if (r < 0)         cntUnderflowRV++; 
	  if (r > UCHAR_MAX) cntOverflowRV++;  
	  if (mode == 1) {
	    if (c < 0) 
	      cntUnderflowCV++; 
	    if (c > UCHAR_MAX && c0 < cov_floor) 
	      cntOverflowCV++;  
	  }
	}
	if (r < 0)         r=0.0;
	if (r > UCHAR_MAX) r=UCHAR_MAX;
	if (c < 0)         c=1.0;
	if (c > UCHAR_MAX) c=UCHAR_MAX;

	r2= (1.0*r-offset)/scaleRV ;
	c2= (1.0*c)       /scaleCV ;

	det += log(scaleCV/c);

	/* count quantization error */
	if (r > 0 && r < UCHAR_MAX && r0 != 0.0 && cbP->count[refX]>0) {
	  errorRV += fabs(r0 - r2 );
	  cntRV++;
	}
	if (mode == 1) {
	  if (c > 0 && c < UCHAR_MAX && c0 != 0.0 && cbP->count[refX]>0) {
	    errorCV += fabs(c0 - c2 );
	    cntCV++;
	  }
	}

	/* store compressed values */
	if (trainMode) {
	  rv[dimX] = r;
	  if (mode == 1)
	    cv[dimX] = c;
	} else {
	  rvC[dimX] = (UCHAR) r;
	  if (mode == 1)
	    cvC[dimX] = (UCHAR) c;
	}
      }

      /* update determinat */
      if (! trainMode) {
	if (cbP->count[refX] < 1) 
	  cbP->detC[refX] = cov_floor;
	else
	  cbP->detC[refX] = det;
      } else {
	if (cbP->count[refX] < 1) 
	  cbP->cv[refX]->det = cov_floor;
	else
	  cbP->cv[refX]->det = det;
      }
    }
  }

  if (cntRV >0) errorRV /= cntRV;
  if (cntCV >0) errorCV /= cntCV;

  INFO("%d / %d underflows after quantization (mean/covar)\n",\
       cntUnderflowRV,cntUnderflowCV);
  INFO("%d / %d overflows  after quantization (mean/covar)\n",\
       cntOverflowRV,cntOverflowCV);
  INFO("quantization error: mean= %2.4f  covar= %2.4f\n",\
       errorRV,errorCV);

  cbsP->scaleRV = scaleRV;
  cbsP->scaleCV = scaleCV;
  cbsP->offset  = offset;
  cbsP->comMode = mode;

  INFO("scaling factor for mean : %2.2f\n",cbsP->scaleRV);
  INFO("scaling factor for covar: %2.2f\n",cbsP->scaleCV);
  INFO("offset for mean         : %2.2f\n",cbsP->offset);

  if (deallocCB) {
    for (cbX=0;cbX<cbN;cbX++) {
      Codebook *cbP = cbsP->list.itemA + cbX;
      cbDealloc(cbP);
    }
  }
  if (cvA) free(cvA);

  return TCL_OK;
}

/* ========================================================================
    Covariance Stuff
   ======================================================================== */

/* ------------------------------------------------------------------------
    cbsCovarTie		tying of covariance matrices
   ------------------------------------------------------------------------ */

static int cbCovarTieHull(Codebook *cb, IArray *toBeTied, IArray *toBeTiedArg)
{
  char *hull = (char*)calloc(cb->refN,sizeof(char));
  int i,j,n=0;

  for (i=0; i<toBeTiedArg->itemN; i++)
      for (j=0; j<cb->refN; j++)
	  if (cb->cv[j]==cb->cv[toBeTiedArg->itemA[i]]) hull[j]=1;

  for (j=0; j<cb->refN; j++) if (hull[j]) n++;
  iarrayInit(toBeTied,n);
  n=0;
  for (j=0; j<cb->refN; j++) if (hull[j]) toBeTied->itemA[n++]=j;

  return TCL_OK;
}

static int cbCovarTie (Codebook *cb, IArray *toBeTiedArg)
{
  int i, j;
  FArray counts;
  float  countSum = 0.0;
  FCovMatrix **toBeFreed = NULL;
  IArray toBeTied;

  cbCovarTieHull(cb,&toBeTied,toBeTiedArg);

  if (toBeTied.itemN<2) return TCL_OK;

  /* iarraySort(toBeTied); */

  /* 1. make sure that all matrix indices have acceptable values */

  for (i=0; i<toBeTied.itemN; i++)
  {   if (toBeTied.itemA[i] < 0 || toBeTied.itemA[i] > cb->refN)
      {  ERROR("Sick index: %d of reference vector.\n",toBeTied.itemA[i]);
	 return TCL_ERROR;
      }
      if (i>0 && toBeTied.itemA[i]==toBeTied.itemA[i-1])
      {  ERROR("Double index: %d of reference vector.\n",toBeTied.itemA[i]);
	 return TCL_ERROR;
      }
  }

  /* 2. make sure that all matrices are of the same type */
  
  for (i=1; i<toBeTied.itemN; i++)
    if (cb->cv[toBeTied.itemA[i]]->type != cb->cv[toBeTied.itemA[0]]->type)
    {  ERROR("Matrix %d is of type %s, while %d is of type %s, won't work.\n",
		toBeTied.itemA[0],cvTypeToStr(cb->cv[toBeTied.itemA[0]]->type),
		toBeTied.itemA[i],cvTypeToStr(cb->cv[toBeTied.itemA[0]]->type));
       return TCL_ERROR;
    }

  /* 3. get the counts for every matrix to be tied */

  farrayInit(&counts,toBeTied.itemN);

  for (i=0; i<cb->refN; i++)
      for (j=0; j<toBeTied.itemN; j++)
          if (cb->cv[i] == cb->cv[toBeTied.itemA[j]]) counts.itemA[j] += cb->count[i];

  for (j=0; j<toBeTied.itemN; j++) countSum += counts.itemA[j];
  if (countSum==0.0) 
  {  ERROR("Sum of counts is 0.0, won't do.\n"); 
     farrayDeinit(&counts); iarrayDeinit(&toBeTied); 
     return TCL_ERROR; 
  }
  for (j=0; j<toBeTied.itemN; j++) counts.itemA[j] /= countSum;

  toBeFreed = (FCovMatrix**)malloc(sizeof(FCovMatrix*)*toBeTied.itemN);
  for (j=0; j<toBeTied.itemN; j++) toBeFreed[j] = NULL;

  fcvAdd(cb->cv[toBeTied.itemA[0]],cb->cv[toBeTied.itemA[0]],counts.itemA[0],0.0);
  for (j=1; j<toBeTied.itemN; j++) 
  {
      fcvAdd(cb->cv[toBeTied.itemA[0]],cb->cv[toBeTied.itemA[j]],1.0,counts.itemA[j]);
      for (i=0; toBeFreed[i]; i++) if (toBeFreed[i]==cb->cv[toBeTied.itemA[j]]) break;
      if (!toBeFreed[i] && cb->cv[toBeTied.itemA[j]]!=cb->cv[toBeTied.itemA[0]]) toBeFreed[i] = cb->cv[toBeTied.itemA[j]];
      cb->cv[toBeTied.itemA[j]] = cb->cv[toBeTied.itemA[0]];
  }
  for (i=0; toBeFreed[i]; i++) fcvFree(toBeFreed[i]);

  farrayDeinit(&counts);  iarrayDeinit(&toBeTied);
  free(toBeFreed);

  return TCL_OK;
}

static int cbCovarTieItf (ClientData cd, int argc, char *argv[])
{
  int ret, ac = argc-1;
  IArray toBeTied = { NULL, 0 };

  if ( itfParseArgv(argv[0], &ac, argv+1, 1,
      "<indexList>", ARGV_IARRAY, NULL, &toBeTied, NULL, "indices of matrices to be tied",
       NULL) != TCL_OK) return TCL_ERROR;

  ret = cbCovarTie((Codebook*)cd, &toBeTied);
  iarrayDeinit(&toBeTied);
  return ret;
}

static int cbCovarUntie (Codebook *cb, IArray *toBeUntied);

static int cbCovarUntieAll (Codebook *cb, IArray *tieList)
{
  int i,j;
  IArray all;

  for (i=0; i<cb->refN; i++)
    for (j=0; j<=i; j++) 
      if (cb->cv[i] == cb->cv[j]) {
	tieList->itemA[i]=j;
	break;
      }

  iarrayInit (&all, cb->refN);
  for (i=0; i < cb->refN; i++)
    all.itemA[i]=i;
  cbCovarUntie (cb, &all);
  iarrayDeinit (&all);

  return TCL_OK;
}

static int cbCovarRetieAll (Codebook *cb, IArray *tieList)
{
  int i;
  IArray tie;

  iarrayInit(&tie,2);
  for (i=0; i<cb->refN; i++)
      if (tieList->itemA[i] != i)
      {
         tie.itemA[0] = tieList->itemA[i];
	 tie.itemA[1] = i;
	 cbCovarTie(cb,&tie);
      }
  iarrayDeinit(&tie);
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    cbsCovarUntie		untying of covariance matrices
   ------------------------------------------------------------------------ */

static int cbCovarUntie (Codebook *cb, IArray *toBeUntied)
{
  int i, j;

  if (toBeUntied->itemN<1) return TCL_OK;

  for (i=0; i<toBeUntied->itemN; i++)
      if (toBeUntied->itemA[i] < 0 || toBeUntied->itemA[i] > cb->refN)
      {  ERROR("Sick index: %d of reference vector.\n",toBeUntied->itemA[i]);
	 return TCL_ERROR;
      }

  for (i=0; i<toBeUntied->itemN; i++)
  {
      int useN = 0;
      int refX = toBeUntied->itemA[i];

      for (j=0; j<cb->refN; j++) if (cb->cv[j]==cb->cv[refX]) useN++;
      if (useN>1)
      {
	 FCovMatrix *cv = fcvCreate(cb->dimN, cb->cv[refX]->type);
	 if (cv==NULL) { ERROR("Can't create %d dimensional FCovMatrix of type %s.",cb->dimN,cvTypeToStr(cb->cv[refX]->type)); return TCL_ERROR; }
	 fcvCopy(cv,cb->cv[refX]);
	 cb->cv[refX] = cv;
      }
  }
  return TCL_OK;
}

static int cbCovarUntieItf (ClientData cd, int argc, char *argv[])
{
  int ret, ac = argc-1;
  IArray toBeUntied = { NULL, 0 };

  if ( itfParseArgv(argv[0], &ac, argv+1, 1,
      "<indexList>", ARGV_IARRAY, NULL, &toBeUntied, NULL, "indices of matrices to get their own copy",
       NULL) != TCL_OK) return TCL_ERROR;

  ret = cbCovarUntie((Codebook*)cd, &toBeUntied);
  iarrayDeinit(&toBeUntied);
  return ret;
}


/* ------------------------------------------------------------------------
    cbCovarTieQ		show which covariance matrices are tied
   ------------------------------------------------------------------------ */

static int cbCovarTieQItf (ClientData cd, int argc, char *argv[])
{
  Codebook *cb = (Codebook*)cd;
  int ac = argc-1;
  int i,j;
  char *printed=(char*)calloc(cb->refN,sizeof(char));

  if ( itfParseArgv(argv[0], &ac, argv+1, 1, NULL) != TCL_OK) return TCL_ERROR;

  for (i=0; i<cb->refN; i++)
  {   
      if (printed[i]) continue;
      itfAppendResult("{ ");
      for (j=i; j<cb->refN; j++)
	if (cb->cv[i]==cb->cv[j]) { itfAppendResult("%d ",j); printed[j]=1; }
      itfAppendResult("} ");
  }
  free(printed);
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    cbCovarType		change the type of a covariance matix
   ------------------------------------------------------------------------ */

extern int cbCovarType (Codebook *cb, int n, CovType type)
{
  FCovMatrix *CV;
  FCovMatrix *cv = cb->cv[n];
  int i,j;
  double dummy;

  if (cv->type == type) return TCL_OK;

  if ((CV=fcvCreate(cb->dimN,type))==NULL)
  {  ERROR("Can't create %d-dimensional %d matrix.\n",cb->dimN,cvTypeToStr(type));
     return TCL_ERROR;
  }

  CV->det = cv->det;

  switch (cv->type)
  {
    case COV_NO:

	switch (type)
        {
	  case COV_RADIAL  : CV->m.r = 1; break;
	  case COV_DIAGONAL: CV->det = 0; for (i=0; i<cb->dimN; i++) CV->m.d[i] = 1; break;
          case COV_FULL    : CV->det = 0; for (i=0; i<cb->dimN; i++) 
			                      for (j=0; j<cb->dimN; j++) CV->m.f[i][j] = (i==j); break;
	  case COV_NO: { }
          case COV_ERR: { }
	}
	break;

    case COV_RADIAL:

	if (cv->m.r==0.0) goto cbCovarTypeSingular;
	switch (type)
        {
	  case COV_DIAGONAL: for (i=0; i<cb->dimN; i++) CV->m.d[i] = cv->m.r; break;
          case COV_FULL    : for (i=0; i<cb->dimN; i++) 
				 for (j=0; j<cb->dimN; j++) CV->m.f[i][j] = (i==j)? cv->m.r : 0.0; break;
	  case COV_RADIAL:
  	  case COV_NO: { } 
          case COV_ERR: { }
	}
	break;

    case COV_DIAGONAL:

	for (i=0; i<cb->dimN; i++) if (cv->m.d[i]==0.0) goto cbCovarTypeSingular;
	switch (type)
        {
	  case COV_RADIAL  : dummy = 1.0; 
			     for (i=0; i<cb->dimN; i++) 
				if (cv->m.d[i]!=0.0) dummy /= cv->m.d[i]; 
				else goto cbCovarTypeSingular;
			     if (dummy>0.0) CV->m.r = pow(dummy,-1.0/cv->dimN); 
			     else goto cbCovarTypeSingular;
			     break;
          case COV_FULL    : for (i=0; i<cb->dimN; i++) 
				 for (j=0; j<cb->dimN; j++) CV->m.f[i][j] = (i==j)? cv->m.d[i] : 0.0; break;
	  case COV_DIAGONAL:
 	  case COV_NO: { }
          case COV_ERR: { }
	}
	break;

    case COV_FULL:

	for (i=0; i<cb->dimN; i++) if (cv->m.f[i][i]==0.0) goto cbCovarTypeSingular;
	switch (type)
        {
	  case COV_DIAGONAL: CV->det = 0.0; 
			     for (i=0; i<cb->dimN; i++) 
			     {  if (cv->m.f[i][i]>0.0) CV->det -= log(CV->m.d[i] = cv->m.f[i][i]); 
				else goto cbCovarTypeSingular;
                             }
			     break;
	  case COV_RADIAL  : dummy   = 1.0; 
			     for (i=0; i<cb->dimN; i++) 
			     {   if (cv->m.f[i][i] > 0.0) dummy /= cv->m.f[i][i]; 
				 else goto cbCovarTypeSingular;
			     }
			     if (dummy>0.0) { CV->det = log(dummy); CV->m.r = pow(dummy,-1.0/cv->dimN); }
			     else goto cbCovarTypeSingular;
		             break;
	  case COV_FULL:
	  case COV_NO: { }
 	  case COV_ERR: { }
	}
	break;

  case COV_ERR:
	break;
  }
  for (i=0; i<cb->refN; i++) if (cb->cv[i] == cv) cb->cv[i] = CV; /* make all sharing vectors use the new matrix */
  fcvFree(cv); 

  for (i=1; i<cb->refN; i++) if (cb->cv[i]->type != cb->cv[0]->type) { cb->type = COV_ERR; break; }
  if (i==cb->refN) cb->type = cb->cv[0]->type;

  /* If the codebook has an accumulator, it's type must be modifyed, too. Here, we lose the trainng information. */

  if (cb->accu) { int subN = cb->accu->subN; cbAccuFree(cb->accu); cb->accu = cbAccuCreate(cb,subN); }
  
  return TCL_OK;

  cbCovarTypeSingular:
    ERROR("Opps, the original matrix was (too close to) singular.\n");
    ERROR("Can't convert %d-dimensional %s matrix to %s matrix\n",cb->dimN,cvTypeToStr(cb->cv[n]->type),cvTypeToStr(type));
    fcvFree(CV);
    return TCL_ERROR;
}

static int cbCovarTypeItf (ClientData cd, int argc, char *argv[])
{
  int   ac        = argc-1;
  int   n         = 0;
  char *type      = NULL;

  if ( itfParseArgv(argv[0], &ac, argv+1, 1,
      "<n>",    ARGV_INT,    NULL, &n,    NULL, "index of the reference vector",
      "<type>", ARGV_STRING, NULL, &type, NULL, "desired type of the covariance matix",
       NULL) != TCL_OK) return TCL_ERROR;

  return cbCovarType((Codebook*)cd, n, cvStrToType(type));
}


/* ------------------------------------------------------------------------
    cbCovarShift		add a constant value to all variances
   ------------------------------------------------------------------------ */

static int cbCovarShiftItf (ClientData cd, int argc, char *argv[])
{
  int   ac        = argc-1;
  float shift     = 0.0;
  Codebook *cb    = (Codebook*)cd;
  int   refX;

  if ( itfParseArgv(argv[0], &ac, argv+1, 1,
      "<shift>", ARGV_FLOAT, NULL, &shift, NULL, "shift value to be added",
       NULL) != TCL_OK) return TCL_ERROR;

  for (refX=0; refX<cb->refN; refX++)
      fcvShift(cb->cv[refX],shift);

  return TCL_OK;
}



/* =================================================================================== */
/*  Distance Functions between codebooks:                                              */
/*  The distances are computed pairwise betweem the components of the codebooks, the   */
/*  resulting distances are saved in an FMatrix                                        */
/*  ----------------------------                                                       */
/*  - Kullback Leibler Distances                                                       */
/*  - Bhattacharya Distance                                                            */
/*  - Extended Mahanalobis Distance*/
/* =================================================================================== */

/* ------------------------------------------------------------------------ */
/*  pairwise Kullback-Leibler distance, aka Kullback-Leibler divergence,    */
/*  aka relative entropy                                                    */
/*  Formula see:                                                            */
/*  Beth Logan and Arel Salomon, "A Music similarity function based signal  */
/*  analysis", IEEE International Conference ond Multimedia and Expo, 2001  */
/*  Formula (1)                                                             */
/* ------------------------------------------------------------------------ */
static int cbKlMatrix(Codebook* cb1, Codebook* cb2, FMatrix* fm) {

  assert (cb1);
  assert (cb2);


  if (cb1->type != COV_DIAGONAL || cb2->type != COV_DIAGONAL) {
    ERROR("Kullback-Leibler distance only implemented for DIAGONAL covariances!\n");
    return TCL_ERROR;
  }

  int ref1N = cb1->refN;
  int dim1N = cb1->dimN;
  int ref2N = cb2->refN;
  int dim2N = cb2->dimN;
  if (cb1->dimN != cb2->dimN) {
    ERROR("dimension of %s does not match dimension of %s, %d != %d\n", cb1->name, cb2->name, dim1N, dim2N);
    return TCL_ERROR;
  }

  fmatrixResize(fm,ref1N,ref2N);

  int i,j,dim;
  float s1squareInv, s2squareInv, mean1, mean2;
  float result;

  for ( i = 0; i < ref1N; i++) {
    for ( j = 0; j < ref2N; j++) {
      result = 0;

      for ( dim = 0; dim < dim1N; dim++) {
	s1squareInv = cb1->cv[i]->m.d[dim]; 
	s2squareInv = cb2->cv[j]->m.d[dim];
	
        mean1 = cb1->rv->matPA[i][dim];
        mean2 = cb2->rv->matPA[j][dim];

	result += (s2squareInv/s1squareInv) + (s1squareInv/s2squareInv) + ((s1squareInv + s2squareInv) * (mean1 - mean2) * (mean1 - mean2));
      }

      fm->matPA[i][j] = result;
    }
  }

  return TCL_OK;
}


static int cbKlMatrixItf ( ClientData cd, int argc, char *argv[]) {
  int          ac    =  argc - 1;

  Codebook*    cb    = NULL;  
  FMatrix      *fm   = NULL;
  /*  int tcounter = 0;*/

  if ( itfParseArgv(argv[0], &ac, argv+1, 0,
      "<codebook>", ARGV_OBJECT, NULL, &cb    , "Codebook", "Codebook to calculate the distance to",
      "<fmatrix>" , ARGV_OBJECT, NULL, &fm    , "FMatrix" , "FMatrix to store the distances in",
       NULL) != TCL_OK) return TCL_ERROR;

  if (cbKlMatrix((Codebook*)cd, cb, fm) != TCL_OK) return TCL_ERROR;
  return TCL_OK;
}



/* ------------------------------------------------------------------------ */
/*  pairwise Bhattacharya distance                                          */
/*  Formula see:                                                            */
/*  Spyros Liapis, Georgios Tziritas, "Image retrieval by colour and        */
/*  texture using chromaticity histograms and wavelet frames                */
/*  Formula (7)                                                             */
/* ------------------------------------------------------------------------ */
static int cbBhattacharyaMatrix(Codebook* cb1, Codebook* cb2, FMatrix* fm) {

  assert (cb1);
  assert (cb2);


  if (cb1->type != COV_DIAGONAL || cb2->type != COV_DIAGONAL) {
    ERROR("Bhattacharya distance only implemented for DIAGONAL covariances!\n");
    return TCL_ERROR;
  }

  int ref1N = cb1->refN;
  int dim1N = cb1->dimN;
  int ref2N = cb2->refN;
  int dim2N = cb2->dimN;

  if (cb1->dimN != cb2->dimN) {
    ERROR("dimension of %s does not match dimension of %s, %d != %d\n", cb1->name, cb2->name, dim1N, dim2N);
    return TCL_ERROR;
  }

  fmatrixResize(fm,ref1N,ref2N);

  int i,j,dim;
  float s1squareInv, s2squareInv, s1square, s2square, mean1, mean2;
  float result;

  for ( i = 0; i < ref1N; i++) {
    for ( j = 0; j < ref2N; j++) {
      result = 0;

      for ( dim = 0; dim < dim1N; dim++) {
        mean1 = cb1->rv->matPA[i][dim];
        mean2 = cb2->rv->matPA[j][dim];

	s1squareInv = cb1->cv[i]->m.d[dim]; 
	s2squareInv = cb2->cv[j]->m.d[dim];
	s1square    = 1.0 / s1squareInv;
	s2square    = 1.0 / s2squareInv;

	result += log((s1square + s2square) / (2.0 * sqrt(s1square * s2square)));
	result += 0.5 * ((fabs(mean1 - mean2) * fabs(mean1 - mean2)) / (s1square + s2square));
      }

      fm->matPA[i][j] = 0.5 * result;
    }
  }

  return TCL_OK;
}


static int cbBhattacharyaMatrixItf ( ClientData cd, int argc, char *argv[]) {
  int          ac    =  argc - 1;

  Codebook*    cb    = NULL;  
  FMatrix      *fm   = NULL;
  /*  int tcounter = 0;*/

  if ( itfParseArgv(argv[0], &ac, argv+1, 0,
      "<codebook>", ARGV_OBJECT, NULL, &cb    , "Codebook", "Codebook to calculate the distance to",
      "<fmatrix>" , ARGV_OBJECT, NULL, &fm    , "FMatrix" , "FMatrix to store the distances in",
       NULL) != TCL_OK) return TCL_ERROR;

  if (cbBhattacharyaMatrix((Codebook*)cd, cb, fm) != TCL_OK) return TCL_ERROR;
  return TCL_OK;
}


/* ------------------------------------------------------------------------ */
/*  pairwise Extended Mahanalobis distance                                  */
/*  Formula see:                                                            */
/*  Dominic Heger, "Speech Feature Enhancement using Particle Filters with  */
/*  class-based phoneme models", Studienarbeit 2007                         */
/*  Formula (4.1) and following formula                                     */
/* ------------------------------------------------------------------------ */
static int cbExtMahanalobisMatrix(Codebook* cb1, Codebook* cb2, FMatrix* fm) {

  assert (cb1);
  assert (cb2);


  if (cb1->type != COV_DIAGONAL || cb2->type != COV_DIAGONAL) {
    ERROR("Extended Mahanalobis distance only implemented for DIAGONAL covariances!\n");
    return TCL_ERROR;
  }

  int ref1N = cb1->refN;
  int dim1N = cb1->dimN;
  int ref2N = cb2->refN;
  int dim2N = cb2->dimN;

  if (cb1->dimN != cb2->dimN) {
    ERROR("dimension of %s does not match dimension of %s, %d != %d\n", cb1->name, cb2->name, dim1N, dim2N);
    return TCL_ERROR;
  }

  fmatrixResize(fm,ref1N,ref2N);

  int i,j,dim;
  float s1squareInv, s2squareInv, s1square, s2square, mean1, mean2, invCovSum;
  float result;

  for ( i = 0; i < ref1N; i++) {
    for ( j = 0; j < ref2N; j++) {
      result = 0;

      for ( dim = 0; dim < dim1N; dim++) {
        mean1 = cb1->rv->matPA[i][dim];
        mean2 = cb2->rv->matPA[j][dim];

	s1squareInv = cb1->cv[i]->m.d[dim]; 
	s2squareInv = cb2->cv[j]->m.d[dim];
	s1square    = 1.0 / s1squareInv;
	s2square    = 1.0 / s2squareInv;

        invCovSum = 1/(s1square + s2square);

        result += (mean1 - mean2) * invCovSum * (mean1 - mean2);
      }

      fm->matPA[i][j] = result;
    }
  }

  return TCL_OK;
}


static int cbExtMahanalobisMatrixItf ( ClientData cd, int argc, char *argv[]) {
  int          ac    =  argc - 1;

  Codebook*    cb    = NULL;  
  FMatrix      *fm   = NULL;
  /*  int tcounter = 0;*/

  if ( itfParseArgv(argv[0], &ac, argv+1, 0,
      "<codebook>", ARGV_OBJECT, NULL, &cb    , "Codebook", "Codebook to calculate the distance to",
      "<fmatrix>" , ARGV_OBJECT, NULL, &fm    , "FMatrix" , "FMatrix to store the distances in",
       NULL) != TCL_OK) return TCL_ERROR;

  if (cbExtMahanalobisMatrix((Codebook*)cd, cb, fm) != TCL_OK) return TCL_ERROR;
  return TCL_OK;
}



/* ------------------------------------------------------------------------ */
/*  cbsAddBbi                                                               */
/* ------------------------------------------------------------------------ */

static int  cbsAddBbi (CodebookSet *cbs, char *cbName, char *bbiName) {

  int           cbX,bbiX,cbvX;
  Codebook      *cb;
  BBITree       *bbi;

  assert (cbs);

  if ((cbX = listIndex((List *) &(cbs->list),(ClientData) cbName,0)) < 0) {
    ERROR("Codebook '%s' doesn't exist\n",cbName);
    return TCL_ERROR;
  }
  cb = cbs->list.itemA + cbX;

  for (cbvX=0; cbvX<cb->refN; cbvX++) {
    if ((cb->cv[cbvX]->type != COV_DIAGONAL) && 
        (cb->cv[cbvX]->type != COV_RADIAL)) {
      ERROR("BBI trees require codebooks with radial or diagonal covariances\n");
      return TCL_ERROR;
    }
  }

  bbiX = listIndex((List *) &(cbs->bbi),(ClientData) bbiName,1);
  bbi = cbs->bbi.itemA + bbiX;

  cb->bbiX = bbiX;
  cb->bbiY = bbi->cbN++;

  return TCL_OK;
}

static int  cbsAddBbiItf (ClientData cd, int argc, char *argv[]) {

  CodebookSet  *cbs = (CodebookSet *) cd;
  char         *cb  = NULL;
  char         *bbi = NULL;
  int          ac   = argc-1;

  if ( itfParseArgv(argv[0], &ac, argv+1, 0,
      "<codebook>", ARGV_STRING, NULL, &cb,  NULL, "name of codebook",
      "<bbiTree>",  ARGV_STRING, NULL, &bbi, NULL, "name of BBI tree",
       NULL) != TCL_OK) return TCL_ERROR;

  return cbsAddBbi(cbs,cb,bbi);
}

/* ------------------------------------------------------------------------ */
/*  cbsReadBbiItf                                                           */
/* ------------------------------------------------------------------------ */

static int  cbsReadBbiItf (ClientData cd, int argc, char *argv[]) {

  char         *filename = NULL;
  int          ac   = argc-1;
  //int          n;

  if ( itfParseArgv(argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename,  NULL, "name of desc file",
       NULL) != TCL_OK) return TCL_ERROR;

  if (freadAdd(cd,filename,';',cbsAddBbiItf) < 0) {
    ERROR("freadAdd() failure trying to read description file\n");
    return TCL_ERROR;
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------ */
/*  cbsSaveBbi                                                              */
/* ------------------------------------------------------------------------ */

static int  cbsSaveBbi (CodebookSet *cbs, FILE *fp) {

  BBITree      *bbi;
  int          bbiX;
  write_int(fp,cbs->bbi.itemN);
  for (bbiX=0; bbiX<cbs->bbi.itemN; bbiX++) {
    bbi = cbs->bbi.itemA + bbiX;
    if (bbiSave(bbi,fp) == TCL_ERROR) return TCL_ERROR;
  }
  return TCL_OK;
}

static int  cbsSaveBbiItf (ClientData cd, int argc, char *argv[]) {

  CodebookSet  *cbs = (CodebookSet *) cd;
  char         *filename = NULL;
  int          ac   = argc-1;
  FILE         *fp;

  if ( itfParseArgv(argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename,  NULL, "name of param file",
       NULL) != TCL_OK) return TCL_ERROR;

  if ((fp = fileOpen(filename,"w")) == NULL) {
    ERROR("Couldn't open file '%s'\n",filename);
    return TCL_ERROR;
  }
  if (cbsSaveBbi(cbs,fp) == TCL_ERROR) return TCL_ERROR;
  fileClose(filename,fp);
  return TCL_OK;
}

/* ------------------------------------------------------------------------ */
/*  cbsLoadBbi                                                              */
/* ------------------------------------------------------------------------ */

static int  cbsLoadBbi (CodebookSet *cbs, FILE *fp) {

  BBITree      *bbi;
  int          bbiX,bbiN;
  int          cbX= 0;

  while (cbX < cbs->list.itemN) {
    if (CBX_MAX < cbs->list.itemA[cbX].refN) {
      ERROR("cbsLoadBBi: CBX_MAX exceeded\n");
      return TCL_ERROR;
    }
    cbX++;
  }
  bbiN = read_int(fp);
  if (bbiN != cbs->bbi.itemN) {
    ERROR("mismatch in number of BBI trees (%d instead of %d)\n",
            bbiN,cbs->bbi.itemN);
    return TCL_ERROR;
  }
  for (bbiX=0; bbiX<bbiN; bbiX++) {
    bbi = cbs->bbi.itemA + bbiX;
    if (bbiLoad(bbi,fp) == TCL_ERROR) return TCL_ERROR;
  }
  return TCL_OK;
}

static int  cbsLoadBbiItf (ClientData cd, int argc, char *argv[]) {

  CodebookSet  *cbs = (CodebookSet *) cd;
  char         *filename = NULL;
  int          ac   = argc-1;
  FILE         *fp;

  if ( itfParseArgv(argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename,  NULL, "name of param file",
       NULL) != TCL_OK) return TCL_ERROR;

  if ((fp = fileOpen(filename,"r")) == NULL) {
    ERROR("Couldn't open file '%s'\n",filename);
    return TCL_ERROR;
  }
  if (cbsLoadBbi(cbs,fp) == TCL_ERROR) return TCL_ERROR;
  fileClose(filename,fp);
  return TCL_OK;
}

/* ------------------------------------------------------------------------ */
/*  cbsMakeBbi                                                              */
/* ------------------------------------------------------------------------ */

static int  cbsMakeBbi (CodebookSet *cbs, int depth, float gamma, int verbose) {
  Codebook     *cb;
  CbsBox       *cbsBox;
  BBITree      *bbi;
  int          bbiX,cbX,idx,dimN = -1;

  cbX = 0;
  while (cbX < cbs->list.itemN) {
    if (CBX_MAX < cbs->list.itemA[cbX].refN) {
      ERROR("cbsMakeBBi: CBX_MAX exceeded\n");
      return TCL_ERROR;
    }
    cbX++;
  }

  cbsBox = (CbsBox *) malloc (sizeof(CbsBox));
  for (bbiX=0; bbiX<cbs->bbi.itemN; bbiX++) {
    bbi = cbs->bbi.itemA + bbiX;
    if (bbi->active) {

      /* --------------------------------------------------------- */
      /*  create Gaussian boxes for all codebooks refering to bbiX */
      /* --------------------------------------------------------- */
      cbsBox->cbN = bbi->cbN;
      cbsBox->box = (CbBox **) malloc (sizeof(CbBox *) * bbi->cbN);

      if (verbose > 0)
        fprintf(stderr,"    Computing Gaussian boxes for codebooks refering to BBI tree %s using gamma=%f\n",bbi->name,gamma);

      for (cbX=0; cbX<cbs->list.itemN; cbX++) {
        cb = cbs->list.itemA + cbX;
        if (dimN < 0) dimN = cb->dimN;
        if ((dimN > 0) && (dimN != cb->dimN)) {
          ERROR("Codebooks dot not have same dimensionality\n");
          return TCL_ERROR;
        }
        if (cb->bbiX == bbiX) {
          idx = cb->bbiY;
          if ((idx >= 0) && (idx < bbi->cbN)) {
            cbsBox->box[idx] = bbiCreateCbBox(cb,gamma);
          } else {
            ERROR("invalid bbiY index in codebook %s\n",cb->name);
            return TCL_ERROR;
          }
        }
      }

      /* ------------------------------------------------------ */
      /*  compute new BBI tree using the Gaussian boxes struct  */
      /* ------------------------------------------------------ */
      bbiMake(bbi,cbsBox,depth,verbose);

      /* --------------------------------------------- */
      /*  clean up, i.e. free memory for all cbsBoxes  */
      /* --------------------------------------------- */
      for (cbX=0; cbX<cbsBox->cbN; cbX++)
        bbiFreeCbBox(cbsBox->box[cbX]);
      free(cbsBox->box);

    }
  }

  free(cbsBox);
  return TCL_OK;
}

static int  cbsMakeBbiItf (ClientData cd, int argc, char *argv[]) {

  CodebookSet  *cbs     = (CodebookSet *) cd;
  int          ac       = argc-1;
  int          depth    = 8;
  float        gamma    = 0.6;
  int          verbose  = 0;

  if ( itfParseArgv(argv[0], &ac, argv+1, 0,
      "-depth",   ARGV_INT,    NULL, &depth,   NULL, "depth of trees",
      "-gamma",   ARGV_FLOAT,  NULL, &gamma,   NULL, "Gaussian box threshold",
      "-verbose", ARGV_INT,    NULL, &verbose, NULL, "verbose level",
       NULL) != TCL_OK) return TCL_ERROR;

  return cbsMakeBbi(cbs,depth,gamma,verbose);
}

/* ------------------------------------------------------------------------ */
/*  cbsPruneBbi                                                             */
/* ------------------------------------------------------------------------ */
static int  cbsPruneBbi (CodebookSet *cbs, int levelN) {
  BBITree          *bbi;
  int              bbiX;
  assert (cbs);
  for (bbiX=0; bbiX<cbs->bbi.itemN; bbiX++) {
    bbi = cbs->bbi.itemA + bbiX;
    if (bbi->active) {
      bbiPrune(bbi,levelN);
    }
  }
  return TCL_OK;
}

static int  cbsPruneBbiItf (ClientData cd, int argc, char *argv[]) {
  CodebookSet  *cbs     = (CodebookSet *) cd;
  int          ac       = argc-1;
  int          levelN   = 0;

  if ( itfParseArgv(argv[0], &ac, argv+1, 0,
      "<levelN>",   ARGV_INT,    NULL, &levelN,   NULL, "number of levels",
       NULL) != TCL_OK) return TCL_ERROR;

  if (levelN <= 0) {
    ERROR("Invalid value for argument levelN\n");
    return TCL_ERROR;
  }
  return cbsPruneBbi(cbs,levelN);
}

/* ------------------------------------------------------------------------ */
/*  cbsFreeBbi                                                              */
/* ------------------------------------------------------------------------ */

static int  cbsFreeBbi (CodebookSet *cbs) {
  int          cbX;
  Codebook     *cb;
  BBITree      *bbi;
  for (cbX=0; cbX<cbs->list.itemN; cbX++) {
    cb = cbs->list.itemA + cbX;
    cb->bbiX = -1;
    cb->bbiY = 0;
  }
  while (cbs->bbi.itemN > 0) {
    bbi = cbs->bbi.itemA;
    listDelete((List *) &(cbs->bbi),bbi->name);
  }
  return TCL_OK;
}

static int  cbsFreeBbiItf (ClientData cd, int argc, char *argv[]) {
  CodebookSet  *cbs = (CodebookSet *) cd;
  return cbsFreeBbi(cbs);
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method cbcfgMethod[] =
{
  { "puts", cbcfgPutsItf, NULL },
  { NULL,   NULL,         NULL }
} ;

TypeInfo cbcfgInfo = { "Cbcfg", 0, 0, cbcfgMethod,
                       cbcfgCreateItf, cbcfgFreeItf,
		       cbcfgConfigureItf, NULL, NULL,
                       "configuration of a codebook" } ;

static Method accuMethod[] =
{
  { "puts",    cbAccuPutsItf,     NULL },
  { ":=",      cbAccuCopyItf,    "copies one accumulator into another" },
  { "+=",      cbAccuPlusItf,    "adds one accumulator to another" },
  { "*=",      cbAccuTimesItf,   "multiplies an accumulator with a factor" },
  { "set",     cbAccuSetItf,     "set reference vectors in the accumulator" },
  { "clear",   cbAccuClearItf,   "reset a single codebook's accumulator to zero" },
  { "subspace",cbAccuSubspaceItf,"define the accumulator subspacing" },
  { NULL,      NULL,              NULL }
} ;

TypeInfo codebookAccuInfo = { "CodebookAccu", 0, 0, accuMethod, 
                               NULL, NULL,
                               cbAccuConfigureItf, cbAccuAccessItf, 
                               itfTypeCntlDefaultNoLink,
	                      "a single codebook's accumulator\n" } ;

static Method codebookMethod[] = 
{ 
  { "puts",       cbPutsItf,        NULL },
  { ":=",         cbCopyItf,       "copy the parameters of one codebook into another" },
  { "set",        cbSetItf,        "set reference vectors in the codebook" },
  { "alloc",      cbAllocItf,      "allocate the codebook" },
  { "createAccu", cbCreateAccuItf, "create an accumulator" },
  { "freeAccu",   cbFreeAccuItf,   "remove an accumulator" },
  { "accuMatrix", cbAccuFmatrixItf,"accumulate data from fmatrix" },
  { "add",        cb1AddItf,       "add two one-dimensional codebooks" },
  { "invert",     cbInvertItf,     "invert covariance matrix to get original one" },
  { "log2lin",    cbLog2LinItf,    "transformation into linear domain" },
  { "lin2log",    cbLin2LogItf,    "transformation into log domain" },
  { "noise",      cbNoiseItf,      "adding of a noise cb (lin domain!)" },
  { "createMap",  cbCreateMapItf,  "create a codebook map" },
  { "freeMap",    cbFreeMapItf,    "remove a codebook map" },
  { "splitList",  cbSplitListItf,  "codebook split candidates" },
  { "stepdiag",   cbStepDiagItf,   "create step-diagonal covariances" },  
  { "update",     cbUpdateWithTyingItf, "update one codebook" },
  { "split",      cbSplitItf,      "split codebook (create map)" },
  { "covarTie",   cbCovarTieItf,   "tie covariance matrices together" },
  { "covarTie?",  cbCovarTieQItf,  "show which covariance matrices are tied together" },
  { "covarUntie", cbCovarUntieItf, "untie covariance matrices" },
  { "covarType",  cbCovarTypeItf,  "modify the type of covariance matrix" },
  { "covarShift", cbCovarShiftItf, "add a constant value to all variances" },

  { "klmatrix",           cbKlMatrixItf,             "compute pairwise KL-distances of codebook components; store result in FMatrix" },
  { "bhattacharyaMatrix", cbBhattacharyaMatrixItf,   "compute pairwise bhattacharya distances of codebook components; store result in FMatrix" },
  { "extMhnMatrix"      , cbExtMahanalobisMatrixItf, "compute pairwise extended Mahanalobis distances of codebook components; store result in FMatrix" },


  {  NULL, NULL, NULL } 
};

TypeInfo codebookInfo = { "Codebook", 0, 0, codebookMethod, 
                           NULL, NULL,
                           cbConfigureItf, cbAccessItf, NULL,
	                  "Codebook" } ;

 static Method codebookSetMethod[] = 
{ 
  { "puts",   cbsPutsItf,    NULL },
  { "index",  cbsIndexItf,  "returns indices of named codebooks" },
  { "name",   cbsNameItf,   "returns names of indexed codebooks" },
  { "add",    cbsAddItf,    "add a new codebook to the set" },
  { "delete", cbsDeleteItf, "remove codebook from the set"  },
  { "read",   cbsReadItf,   "read codebook definitions from file" },
  { "write",  cbsWriteItf,  "write codebook definitions to file" },
  { "load",   cbsLoadItf,   "load codebook weights" },
  { "save",   cbsSaveItf,   "save codebook weights" },
  { "set",    cbsSetItf,    "set and propagate defaultTopN or defaultRdimN" },
  
  { "createAccus",  cbsCreateAccusItf, "creates accumulators for all codebooks" },
  { "freeAccus",    cbsFreeAccusItf,   "removes accumulators of  all codebooks" },
  { "clearAccus",   cbsClearAccusItf,  "clear accumulators for all codebooks" },

  { "loadAccus",    cbsLoadAccusItf,   "loads codebook accumulators from a file" },
  { "saveAccus",    cbsSaveAccusItf,   "saves codebook accumulators into a file" },

  { "createMaps",   cbsCreateMapsItf,  "creates maps for all codebooks" },
  { "freeMaps",     cbsFreeMapsItf,    "removes maps of  all codebooks" },

  { "split",        cbsSplitItf,       "split all codebooks" },
  { "map",          cbsMapItf,         "map all codebooks to new codebooks" },
  { "update",       cbsUpdateItf,      "update all codebooks" },

  { "addBBI",       cbsAddBbiItf,   "add new (or link to existing) BBI tree" },
  { "readBBI",      cbsReadBbiItf,  "read BBI description file" },
  { "saveBBI",      cbsSaveBbiItf,  "save BBI tree parameters" },
  { "loadBBI",      cbsLoadBbiItf,  "load BBI tree parameters" },
  { "makeBBI",      cbsMakeBbiItf,  "make new BBI trees" },
  { "pruneBBI",     cbsPruneBbiItf, "prune BBI trees"    },
  { "freeBBI",      cbsFreeBbiItf,  "free all BBI trees" },
  { "compress",     cbsCompressItf, "compress means/covars to 8bit values" },
  {  NULL,    NULL,          NULL } 
};

TypeInfo codebookSetInfo = { "CodebookSet", 0, -1, codebookSetMethod, 
                              cbsCreateItf,     cbsFreeItf,
                              cbsConfigureItf,  cbsAccessItf, NULL,
	                     "Set of codebooks\n" } ;

static int codebookInitialized = 0;

extern int Score_Init(void);
extern int Covar_Init(void);
extern int Rewrite_Init(void);
extern int LDA_Init(void);
extern int MLAdapt_Init(void);
extern int Cbmap_Init(void);
extern int SignalAdapt_Init(void);
 
int Codebook_Init(void)
{
  if ( codebookInitialized) return TCL_OK;

  if ( Score_Init()   != TCL_OK) return TCL_ERROR;
  if ( Covar_Init()   != TCL_OK) return TCL_ERROR;
  if ( Rewrite_Init() != TCL_OK) return TCL_ERROR;
  if ( LDA_Init()     != TCL_OK) return TCL_ERROR;
  if ( MLAdapt_Init() != TCL_OK) return TCL_ERROR;
  if ( Cbmap_Init()   != TCL_OK) return TCL_ERROR;
  if (SignalAdapt_Init() != TCL_OK) return TCL_ERROR;
  if (GSClusterSet_Init() != TCL_OK) return TCL_ERROR;

  defaultCbcfg.name          = "default";
  defaultCbcfg.useN          =  1;
  defaultCbcfg.minRvCount    =  1.0;
  defaultCbcfg.minCvCount    =  6.0;
  defaultCbcfg.beta          = -1.0;
  defaultCbcfg.momentum      =  0.0;
  defaultCbcfg.momentumCv    = -1.0;
  defaultCbcfg.doAccumulate  =  'y';
  defaultCbcfg.doUpdate      =  'y';
  defaultCbcfg.method        =  'm';
  defaultCbcfg.weight        =  1.0;
  defaultCbcfg.topN          =  0;
  defaultCbcfg.rdimN         =  0;
  defaultCbcfg.bbiOn         =  1;
  defaultCbcfg.expT          =  DEFAULT_EXPT_VALUE;
  defaultCbcfg.mergeThresh   =  10.0;
  defaultCbcfg.splitStep     =  0.01;
  defaultCbcfg.rhoGlob       =  1.0;
  defaultCbcfg.E             =  1.0;
  defaultCbcfg.H             =  0.8;
  defaultCbcfg.I             =  0.0;

  cbsDefault.name           = NULL;
  cbsDefault.useN           = 0;
  cbsDefault.list.blkSize   = 1000;
  cbsDefault.rwsP           = NULL;
  cbsDefault.feat           = NULL;
  cbsDefault.featXA         = NULL;
  cbsDefault.featN          = 0;
  cbsDefault.cache          = NULL;
  cbsDefault.score          = scScoreTopAllBbi;
  cbsDefault.accu           = cbsAccu;
  cbsDefault.update         = cbsUpdate;
  cbsDefault.defaultTopN    = 0;
  cbsDefault.defaultRdimN   = 0;
  cbsDefault.defaultBbiOn   = 1;
  cbsDefault.scaleRV        = 1.0;
  cbsDefault.scaleCV        = 1.0;
  cbsDefault.offset         = 0.0;
  cbsDefault.defaultExpT    = DEFAULT_EXPT_VALUE;
  cbsDefault.commentChar    = ';';

  itfNewType( &codebookAccuInfo);
  itfNewType( &codebookInfo);
  itfNewType( &codebookSetInfo);
  itfNewType( &cbcfgInfo);

  codebookInitialized = 1;
  return TCL_OK;
}
