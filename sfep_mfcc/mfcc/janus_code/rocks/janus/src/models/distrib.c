/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Distributions
               ------------------------------------------------------------

    Author  :  Michael Finke & Ivica Rogina
    Module  :  distrib.c
    Date    :  $Id: distrib.c 3416 2011-03-23 03:02:18Z metze $
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
    Revision 4.16  2007/02/21 15:28:37  stueker
    Implemented MMIE distrib set update according to dissertation of Dan Povey, formulae (4.34) and (4.35), see dsAccuMMIE

    Revision 4.15  2006/11/10 10:16:06  fuegen
    merged changes from branch jtk-05-02-02-shajith
    - all modification related to MMIE training
    - first unverified functions for MPE training
    - modifications made by Shajith, Roger, Wilson, and Sebastian

    Revision 4.14.8.1  2006/11/03 12:48:01  stueker
    Initial check-in of MMIE from Shajith, Wilson, and Roger.

    Revision 4.14  2004/10/27 11:58:00  metze
    '-bmem' changes

    Revision 4.13  2004/10/22 13:06:01  metze
    Fixed '-bmem' option

    Revision 4.12  2004/09/16 14:36:21  metze
    Made code 'x86_64'-clean

    Revision 4.11  2003/08/14 11:20:14  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 4.10.4.24  2003/08/07 12:42:15  metze
    Fixed list overflow

    Revision 4.10.4.23  2003/06/24 06:57:55  metze
    Check for existing accus fixed

    Revision 4.10.4.22  2003/02/05 09:34:59  soltau
    typo

    Revision 4.10.4.21  2003/02/05 08:59:58  soltau
    Added MMIE training

    Revision 4.10.4.20  2003/01/27 14:44:00  metze
    Changed behaviour when setting distributions

    Revision 4.10.4.19  2002/12/16 13:06:25  soltau
    dsCopy : check for allocated 'val'

    Revision 4.10.4.18  2002/12/13 08:44:16  soltau
    dsSplit : support for radial gaussians, mupltiple mixtures

    Revision 4.10.4.16  2002/11/25 17:37:44  metze
    Made 256 Gaussians per Codebook possible

    Revision 4.10.4.15  2002/11/20 09:38:52  soltau
    dsSplit: check for CBX_MAX

    Revision 4.10.4.14  2002/11/19 13:10:52  soltau
    dssAccuFree: added dsP->accu = NULL

    Revision 4.10.4.13  2002/11/19 09:31:45  metze
    Added interface to Hagen's changes

    Revision 4.10.4.12  2002/11/19 08:43:51  soltau
    Added dssAccusFree

    Revision 4.10.4.11  2002/11/13 08:34:34  soltau
    dssCreateLh : fixed accu allocation

    Revision 4.10.4.10  2002/08/27 08:44:04  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 4.10.4.9  2002/08/01 13:42:33  metze
    Fixes for clean documentation.

    Revision 4.10.4.8  2002/04/03 11:06:07  soltau
    Removed useless threads

    Revision 4.10.4.7  2002/03/18 17:43:59  soltau
    re-vived splitting & merging od distributions
    cleaned  pre-declarations

    Revision 4.10.4.6  2002/03/14 10:02:11  soltau
    Save only real acumulators

    Revision 4.10.4.5  2002/03/13 11:53:01  soltau
    allocate distrib accus on demand to allow efficient polyphone training

    Revision 4.10.4.4  2002/03/13 10:06:36  soltau
    Added bmem support

    Revision 4.10.4.3  2002/02/28 11:45:28  soltau
    dsDeinit: fixed delink

    Revision 4.10.4.2  2001/10/05 11:27:08  metze
    *** empty log message ***

    Revision 4.10.4.1  2001/10/02 17:59:29  metze
    Added scoreFeature method to score a feature-detector during pre-processing

    Revision 4.5  2000/08/16  08:41:43  soltau
    added distrib splitting stuff
   
    Revision 4.4  2000/02/08 15:31:31  soltau
    debug info

    Revision 4.3  2000/01/12 10:02:02  fuegen
    add normalisation for distances
    add verbosity for clustering
    faster distance calculation for entropy

    Revision 4.2  1999/07/06 09:51:12  fuegen
    added QUESTION_VERBOSE flag

    Revision 4.1  1998/12/10 21:04:37  fuegen
    *** empty log message ***

    Revision 4.0  1998/11/30 18:07:14  fuegen
    *** empty log message ***

    Revision 3.7  1998/08/12 10:41:55  kemp
    added method kldist

 * Revision 3.6  1998/02/02  18:03:01  rogina
 * added missing initialization of dsa->lh in dsAccuInit
 *
    Revision 3.5  1997/08/27 15:53:42  rogina
    removed predeclaration of gethostname (wouldn't compile on HPs/PCs)

    Revision 3.4  1997/07/31 16:24:54  rogina
    made code gcc -DFKIclean

    Revision 3.3  1997/07/16 11:37:35  rogina
    added likelihood-distance stuff

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 2.6  96/05/24  09:27:32  rogina
    changed default values for minCount to 1.0 and for shift to 0.0
    
    Revision 2.5  1996/03/25  22:47:54  finkem
    added scoreNBest to print out the n-best distribution for
    a given frame

    Revision 2.4  1996/03/04  13:43:40  rogina
    fixed turning-off of distrib-training

    Revision 2.3  1996/02/14  03:00:15  finkem
    fixed problem in dssDist and made it faster

    Revision 2.2  1996/02/13  05:12:09  finkem
    added configure option

    Revision 2.1  1996/02/11  06:17:13  finkem
    changed distance function to take 3 model arrays

    Revision 2.0  1996/01/31  04:49:17  finkem
    New senoneSet setup

    Revision 1.26  1996/01/12  18:38:33  rogina
    added configuration of object name

    Revision 1.25  1996/01/12  16:08:56  finkem
    changed dssRead to freadAdd

    Revision 1.24  1996/01/05  16:59:40  finkem
    remove ref to old ERR macro

    Revision 1.23  1996/01/02  18:11:04  rogina
    introduced Dscfg class, modified the usage of shift and
    mincount and floor value stuff, added training selection
    mechanism

    Revision 1.22  1995/12/27  15:31:50  rogina
    eliminated vscore

    Revision 1.21  1995/12/15  22:38:08  finkem
    removed splitting alloc bug wrt to dsAccuInit

    Revision 1.20  1995/12/04  04:43:09  finkem
    added accu configure and add.
    added map function.

    Revision 1.19  1995/11/30  15:06:11  finkem
    CodebookMap revised and made working

    Revision 1.18  1995/11/25  20:26:40  finkem
    removed a couple of problems with the data structures of the
    accumulator (pointing back to distributions)

    Revision 1.17  1995/11/23  08:00:00  finkem
    minor changes

    Revision 1.16  1995/11/15  04:07:32  finkem
    Added BBI score computation

    Revision 1.15  1995/11/14  06:05:14  finkem
    splitting eighting matrix instead of a vector

    Revision 1.14  1995/10/25  16:54:20  rogina
    *** empty log message ***

    Revision 1.13  1995/10/18  23:21:07  finkem
    cleaning up / introducing shift & mincount

    Revision 1.12  1995/10/16  22:59:39  torsten
    *** empty log message ***

    Revision 1.11  1995/10/06  01:09:22  torsten
    added dss accu functions

    Revision 1.10  1995/09/21  21:39:43  finkem
    Init?Deinit and default arguments changed

    Revision 1.9  1995/09/12  18:01:22  rogina
    *** empty log message ***

    Revision 1.8  1995/09/05  16:05:02  kemp
    *** empty log message ***

    Revision 1.7  1995/08/31  08:29:00  kemp
    y

    Revision 1.6  1995/08/28  16:10:40  rogina
    y

    Revision 1.5  1995/08/17  17:41:43  rogina
    *** empty log message ***

    Revision 1.4  1995/08/14  12:21:43  finkem
    *** empty log message ***

    Revision 1.3  1995/08/10  08:11:05  rogina
    *** empty log message ***

    Revision 1.2  1995/08/04  14:16:18  rogina
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#include <stdarg.h>
#include <time.h>
#include <sys/types.h>

#include <limits.h>
#include "common.h"
#include "itf.h"
#include "distrib.h"
#include "mach_ind_io.h"
#include "array.h"
#include "modelSet.h"
#include "rewrite.h"
#include "codebook.h"
#include "bbi.h"
#include "modalities.h"
#include "path.h"
#include "score.h"

char distribRCSVersion[]= 
           "@(#)1$Id: distrib.c 3416 2011-03-23 03:02:18Z metze $";

/* ========================================================================
    Global Declarations
   ======================================================================== */

extern TypeInfo distribInfo;
extern TypeInfo distribSetInfo;

/* ========================================================================
    Accumulation Count Arrays (used by all accumulators)
   ======================================================================== */

static float*  dssAddCountA = NULL;
static int     dssAddCountN = 0;

static float** dssSubA      = NULL;
static int     dssSubM      = 0;
static int     dssSubN      = 0;

static BMem*   _bmemDSSP    = NULL;
static int     _bmemDSSN    = 0;

/* ========================================================================
    Distrib Configuration
   ======================================================================== */

Dscfg defaultDscfg = 
{
  "default",/* name         name of the configuration object */
  1,        /* useN         1 to prevent its destruction */
  0.0,      /* momentum     how much of the old parameters to be preserved */
  0.0,      /* shift        will be added to every accumulator before update */
  1.0,      /* minCount     don't update if not enough training counts */
  0.000001, /* floor        forced floor value for all accumulator counts */
  'y',      /* doAccumulate 'y' = do accumulate, 'n' = don't accumulate */
  'y',      /* doUpdate     'y' = do update, 'n' = don't update */
  'm',      /* method;      'm' = maximum likelihood, '...' other */
};

static int dscfgInit (Dscfg *dscfg, char *name)
{
  *dscfg       = defaultDscfg;
   dscfg->name = strdup(name);

  return TCL_OK;
}

static Dscfg *dscfgCreate (char *name)
{
  Dscfg *dscfg = (Dscfg*)malloc(sizeof(Dscfg));
  if (dscfgInit(dscfg,name)!=TCL_OK) { free(dscfg); return NULL; }
  return dscfg;
}

static ClientData dscfgCreateItf (ClientData cd, int argc, char *argv[])
{
  return (ClientData)dscfgCreate(argv[0]);
}

static int dscfgFreeItf (ClientData cd)
{
  return TCL_OK;
}

static int dscfgConfigureItf (ClientData cd, char *var, char *val)
{
  Dscfg *cfg = (Dscfg*)cd;
  if (! cfg) cfg = &defaultDscfg;
  if (! var) {
    ITFCFG(dscfgConfigureItf,cd,"name");
    ITFCFG(dscfgConfigureItf,cd,"useN");
    ITFCFG(dscfgConfigureItf,cd,"momentum");
    ITFCFG(dscfgConfigureItf,cd,"shift");
    ITFCFG(dscfgConfigureItf,cd,"minCount");
    ITFCFG(dscfgConfigureItf,cd,"floor");
    ITFCFG(dscfgConfigureItf,cd,"accu");
    ITFCFG(dscfgConfigureItf,cd,"update");
    ITFCFG(dscfgConfigureItf,cd,"method");
    return TCL_OK;
  }
  ITFCFG_CharPtr(var,val,"name",     cfg->name,         1);
  ITFCFG_Int    (var,val,"useN",     cfg->useN,         1);
  ITFCFG_Float  (var,val,"momentum", cfg->momentum,     0);
  ITFCFG_Float  (var,val,"shift",    cfg->shift,        0);
  ITFCFG_Float  (var,val,"minCount", cfg->minCount,     0);
  ITFCFG_Float  (var,val,"floor",    cfg->floor,        0);
  ITFCFG_Char   (var,val,"accu",     cfg->doAccumulate, 0);
  ITFCFG_Char   (var,val,"update",   cfg->doUpdate,     0);
  ITFCFG_Char   (var,val,"method",   cfg->method,       0);
  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;   
}

static int dscfgPutsItf (ClientData cd, int argc, char *argv[])
{
  return dscfgConfigureItf(cd,NULL,NULL);
}

/* ========================================================================
    DistribAccu
   ======================================================================== */
/* ------------------------------------------------------------------------
    Create DistribAccu Structure:
   ------------------------------------------------------------------------ */

/* forward delcaration */
static int dsAccuClear( DistribAccu *dsa, int subX);

static int dsAccuAllocate(DistribAccu* dsa)
{
  int subX;
  if (!dsa)       return TCL_OK;
  if (dsa->count) return TCL_OK;

  if (_bmemDSSP) {
    dsa->count    = bmemAlloc(_bmemDSSP, dsa->subN * sizeof(double*));
    dsa->count[0] = bmemAlloc(_bmemDSSP, dsa->subN * dsa->ds->valN * sizeof(double));
  } else {
    if (NULL == (dsa->count = malloc(dsa->subN * sizeof(double*)))) {
      ERROR("dsAccuAllocate: allocation failure\n");
      return TCL_ERROR;
    }
    if (NULL == (dsa->count[0] = malloc(dsa->subN * dsa->ds->valN * sizeof(double)))) {
      ERROR("dsAccuAllocate: allocation failure\n");
      return TCL_ERROR;
    }
  }
  for ( subX = 1; subX < dsa->subN; subX++) 
    dsa->count[subX] = dsa->count[subX-1] + dsa->ds->valN;

  dsAccuClear(dsa,-1);
  return TCL_OK;
}


static int dsAccuInit( DistribAccu* dsa,  Distrib* ds, int subN)
{
  int subX;

  dsa->ds       = ds;   link( ds, "Distrib");
  dsa->subN     = subN;
  dsa->lh       = NULL;
  dsa->count    = NULL;

  if (! dssAddCountA || dssAddCountN < ds->valN) {
    dssAddCountN =  ds->valN;
    dssAddCountA = (float*)realloc( dssAddCountA, dssAddCountN * sizeof(float));
  }
  if (! dssSubA || dssSubM < ds->valN || dssSubN < subN) {
    if ( dssSubA) { 
      if (  dssSubA[0]) free( dssSubA[0]);
      free( dssSubA);
    }
    dssSubM    =  ds->valN;
    dssSubN    =  subN;
    dssSubA    = (float**)malloc( dssSubM *           sizeof(float*));
    dssSubA[0] = (float *)malloc( dssSubM * dssSubN * sizeof(float));

    for ( subX = 1; subX < dssSubM; subX++) 
          dssSubA[subX] = dssSubA[subX-1] + dssSubN;
  }

  return TCL_OK;
}

static DistribAccu* dsAccuCreate( Distrib *ds, int subN)
{
  DistribAccu* dsa = (DistribAccu*)malloc(sizeof(DistribAccu));

  assert(ds);
  if (! dsa || dsAccuInit(dsa, ds, subN) != TCL_OK) {
    if ( dsa) free( dsa);
    ERROR("Can't allocate distribution accumulator.\n"); 
    return NULL;
  }
  dsAccuClear(dsa,-1);
  return dsa;
}

/* ------------------------------------------------------------------------
    dsAccuDeinit   deinitialize and free the distributions
   ------------------------------------------------------------------------ */

static int dsAccuDeinit( DistribAccu* dsa)
{
  if (! dsa) return TCL_OK;
  if ( dsa->ds) unlink( dsa->ds, "Distrib");
  if (_bmemDSSP) {
    dsa->count = NULL;
  } else {
    if ( dsa->count) {
      if (  dsa->count[0]) free( dsa->count[0]);
      free( dsa->count);   dsa->count = NULL;
    }
  }
  return TCL_OK;
}

static int dsAccuFree( DistribAccu* dsa)
{
  if ( dsAccuDeinit( dsa) != TCL_OK) return TCL_ERROR;
  if ( dsa) free( dsa);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dsAccuConfigureItf    configure variables within a distribution
                          accumulator
   ------------------------------------------------------------------------ */

static int dsAccuConfigureItf (ClientData cd, char *var, char *val)
{
  DistribAccu* dsaP = (DistribAccu*)cd;
  if (! dsaP)  return TCL_ERROR;

  if (! var) {
    ITFCFG(dsAccuConfigureItf,cd,"subN");
    ITFCFG(dsAccuConfigureItf,cd,"count");
    return TCL_OK;
  }
  ITFCFG_Int  (var,val,"subN",     dsaP->subN,     1);

  if (! strcmp(var,"count")) {
    if (! val) {
      int   subX, refX;
      float sum  = 0;
      dsAccuAllocate(dsaP);
      for ( subX = 0; subX < dsaP->subN; subX++)
        for ( refX = 0; refX < dsaP->ds->valN; refX++)
          sum += dsaP->count[subX][refX]; 
      itfAppendResult("% 2.4e",sum); 
      return TCL_OK;
    }
    return TCL_OK;
  }
  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    dsAccuAccessItf    allow access to Janus typed objects within a 
                       distribution accumulator via TCL
   ------------------------------------------------------------------------ */

static ClientData dsAccuAccessItf (ClientData cd, char *name, TypeInfo **ti)
{
  if ( *name == '.') {
    if ( name[1] =='\0') {
      *ti=NULL;
      return NULL; 
    }
    else {
      *ti=NULL; return NULL;
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    dsAccuClear    clear the distribution accumulator to a preconfigured
                   floor value
   ------------------------------------------------------------------------ */

static int dsAccuClear( DistribAccu *dsa, int subIndex)
{
  int subA, subE, subX, valX;

  if (! dsa || !dsa->count) return TCL_OK;

  subA = (subIndex >= 0) ? subIndex   : 0; 
  subE = (subIndex >= 0) ? subIndex+1 : dsa->subN;

  for ( subX=subA; subX < subE; subX++)
    for ( valX=0; valX < dsa->ds->valN; valX++)
      dsa->count[subX][valX] = dsa->ds->cfg->floor;

  return TCL_OK;
}

static int dsAccuClearItf( ClientData cd, int argc, char *argv[]) 
{ 
  DistribAccu *dsa  = (DistribAccu*)cd;
  int          ac   =  argc-1;
  int          subX = -1;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "-subX", ARGV_INT,  NULL, &subX,  NULL, "sub-accumulator, -1 to clear all",
        NULL) != TCL_OK) return TCL_ERROR;

  return dsAccuClear(dsa,subX); 
}

/* ------------------------------------------------------------------------
    dsAccuPuts   print accumulator values
   ------------------------------------------------------------------------ */

static int dsAccuPutsItf (ClientData cd, int argc, char *argv[])
{
  int subX, valX;
  DistribAccu *dsa=(DistribAccu*)cd;

  dsAccuAllocate(dsa);
  for (subX=0; subX<dsa->subN; subX++) {
    itfAppendResult("{");
    for (valX=0; valX<dsa->ds->valN; valX++)
      itfAppendResult(" % 2.4e",dsa->count[subX][valX]);
    itfAppendResult("}%c", (subX!=dsa->subN-1) ? '\n' : 0);
  }

  if (dsa->lh)
  {  itfAppendResult("\n{");
     for (subX=0; subX<dsa->subN; subX++) lhPuts(dsa->lh[subX]);
     itfAppendResult("}");
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dsAccuMatch   check, whether two distributions match such that we can
                  add, interpolate etc. them.
   ------------------------------------------------------------------------ */

static int dsAccuMatch( DistribAccu* dst, DistribAccu* src)
{
  if ( dst->ds->valN != src->ds->valN) {
    SERROR("Inconsistent valN: %d %d\n", dst->ds->valN, src->ds->valN);
    return TCL_ERROR;
  }
  if ( dst->subN != src->subN) {
    SERROR("Inconsistent subN: %d %d\n", dst->subN, src->subN);
    return TCL_ERROR;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dsAccuCopy  copy one accumulator to another. The only precondition is
                that the size of the distribution underlying currently the
                destination is of the same size as the source distribution.
   ------------------------------------------------------------------------ */

static int dsAccuCopy( DistribAccu* dst, DistribAccu* src)
{
  Distrib* ds = dst->ds;
  int      subX, valX;

  if ( dst == src)  return TCL_OK;
  if (! src->count) return TCL_OK;

  if ( ds->valN != src->ds->valN) {
    ERROR("Inconsistent dimensions of the distributions %s and %s.\n",
           ds->name, src->ds->name);
    return TCL_ERROR;
  }
  if ( dsAccuDeinit( dst) != TCL_OK ||
       dsAccuInit( dst, ds, src->subN) != TCL_OK) {
    ERROR("Can't initialize new accumulator for Distrib %s.\n", ds->name);
    return TCL_ERROR;
  }
  dsAccuAllocate(dst);
  for (subX=0; subX<dst->subN; subX++)
    for (valX=0; valX<dst->ds->valN; valX++)
      dst->count[subX][valX] = src->count[subX][valX];
  return TCL_OK;
}

static int dsAccuCopyItf (ClientData cd, int argc, char *argv[])
{
  DistribAccu* dst = (DistribAccu*)cd;
  DistribAccu* src =  NULL;
  int          ac  =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<source>", ARGV_OBJECT, NULL, &src, "DistribAccu", "source accumulator",
       NULL) != TCL_OK) return TCL_ERROR;
  return dsAccuCopy( dst, src);
}

/* ------------------------------------------------------------------------
    dsAccuPlus  add two accumulators weighted by a factor. It is also
                possible to add all values to the same distribution
                component, by defining val.
   ------------------------------------------------------------------------ */

static int dsAccuPlus( DistribAccu* dst, DistribAccu* src, float factor, int val)
{
  int subX, valX;

  if (!src->count) return TCL_OK;
  assert(dst->count);
  if ( val < 0) {
    if ( dsAccuMatch( dst, src) != TCL_OK) return TCL_ERROR;
    for (subX=0; subX<dst->subN; subX++)
      for (valX=0; valX<dst->ds->valN; valX++)
        dst->count[subX][valX] += factor * src->count[subX][valX];
  }
  else {
    if ( val >= dst->ds->valN) {
       SERROR("dsAccu reference %d out of range %d.\n",val, dst->ds->valN);
       return TCL_ERROR;
    }
    if (src->subN != dst->subN) {
       SERROR("cdsAccu inconsistent subN: %d %d\n",dst->subN,src->subN);
       return TCL_ERROR;
    }
    for ( subX = 0; subX < src->subN; subX++)
      for (valX=0; valX<dst->ds->valN; valX++)
        dst->count[subX][val] += factor * src->count[subX][valX];
  }
  return TCL_OK;
}

static int dsAccuPlusItf (ClientData cd, int argc, char *argv[])
{
  DistribAccu* dst    = (DistribAccu*)cd;
  DistribAccu* src    =  NULL;
  float        factor =  1.0;
  int          valX   = -1;
  int          ac     =  argc-1;

  if ( itfParseArgv(argv[0], &ac, argv+1, 0,
      "<source>", ARGV_OBJECT, NULL, &src, "DistribAccu", "source accumulator",
      "-factor",  ARGV_FLOAT,  NULL, &factor, NULL, "scaling factor",
      "-valX",    ARGV_INT,    NULL, &valX,   NULL, "add accus to valX component",
       NULL) != TCL_OK) return TCL_ERROR;
      
  return dsAccuPlus( dst, src, factor, valX);
}

/* ------------------------------------------------------------------------
    dsAccuTimes  scale a distribution accumulator by a factor
   ------------------------------------------------------------------------ */

static int dsAccuTimes( DistribAccu* dst, float factor)
{
  int subX, valX;

  if (! dst->count) return TCL_OK;
  for (subX=0; subX<dst->subN; subX++)
    for (valX=0; valX<dst->ds->valN; valX++)
      dst->count[subX][valX] *= factor;
  return TCL_OK;
}


static int dsAccuTimesItf (ClientData cd, int argc, char *argv[])
{
  DistribAccu* dst    = (DistribAccu*)cd;
  float        factor =  1.0;
  int          ac     =  argc-1;

  if ( itfParseArgv(argv[0], &ac, argv+1, 0,
      "<factor>", ARGV_FLOAT, NULL, &factor, NULL, "multiplication factor",
       NULL) != TCL_OK) return TCL_ERROR;
      
  return dsAccuTimes( dst, factor);
}

/* ------------------------------------------------------------------------
    dsAccuIncr    shift the counts in a distribution accumulator by a
                  constant amount.
   ------------------------------------------------------------------------ */

static int dsAccuIncr( DistribAccu* dst, float shift)
{
  int subX, valX;

  if (! dst->count) return TCL_OK;
  for (subX=0; subX<dst->subN; subX++)
    for (valX=0; valX<dst->ds->valN; valX++)
      dst->count[subX][valX] += shift;
  return TCL_OK;
}

static int dsAccuIncrItf (ClientData cd, int argc, char *argv[])
{
  DistribAccu* dst    = (DistribAccu*)cd;
  float        shift  =  0.0;
  int          ac     =  argc-1;

  if ( itfParseArgv(argv[0], &ac, argv+1, 0,
      "<shift>", ARGV_FLOAT, NULL, &shift, NULL, 
      "value by which to increment every count", NULL) != TCL_OK) 
       return TCL_ERROR;
      
  return dsAccuIncr( dst, shift);
}

/* ------------------------------------------------------------------------
    dsAccuMLE
   ------------------------------------------------------------------------ */

static double*  dsAccuCountA = NULL;
static int      dsAccuCountN = 0;

static int dsAccuMLE( DistribAccu* dsa, int subX)
{
  Distrib* ds   = dsa->ds;
  int      subN = dsa->subN;
  int      valN = ds->valN, valX, i;
  double   sum  = 0.0;

  /* no accus, no update */
  if (!dsa->count) return TCL_OK;
  
  if (! dsAccuCountA || dsAccuCountN < valN) {
    dsAccuCountN =  valN;
    dsAccuCountA = (double*)realloc( dsAccuCountA, dsAccuCountN * sizeof(double));
  }

  if ( subX < 0) {
    for ( sum = 0, valX = 0; valX < valN; valX++)
      for ( i = 0; i < subN; i++) sum += dsa->count[i][valX];

    if ((ds->count = sum) < ds->cfg->minCount) return TCL_OK;

    for (  sum = 0, valX = 0; valX < valN; valX++) {
      double count = 0.0;

      for ( i = 0; i < subN; i++) count += dsa->count[i][valX];
      if (count < ds->cfg->floor) count  = ds->cfg->floor;

      sum += ( dsAccuCountA[valX] = count + ds->cfg->shift);
    }

    ds->count = sum;
    if ( ds->cfg->momentum > 0.0) {
      for ( valX=0; valX < valN; valX++) {
        ds->val[valX] =  -1.0 * log( ds->cfg->momentum  *  exp((-1.0)*ds->val[valX]) + 
                        ( 1.0 -      ds->cfg->momentum) * (dsAccuCountA[valX] / ds->count));
      }
    }
    else {
      for ( valX=0; valX < valN; valX++)
        ds->val[valX] = -log(dsAccuCountA[valX] / ds->count);
    }
    return TCL_OK;
  }

  for ( sum  = 0.0, valX = 0; valX < valN; valX++) 
        sum += dsa->count[subX][valX];

  if  ((ds->count = sum) < ds->cfg->minCount) return TCL_OK;

  for (  sum = 0.0, valX = 0; valX < valN; valX++) {
    double count = ds->accu->count[subX][valX];
    if (   count < ds->cfg->floor) count = ds->cfg->floor;
    sum += ( dsAccuCountA[valX] = count + ds->cfg->shift);
  }

  ds->count = sum;

  for ( valX=0; valX<ds->valN; valX++) {
    ds->val[valX] = (-1.0) * log( ds->cfg->momentum  *  exp((-1.0)*ds->val[valX]) + 
                    ( 1.0  -      ds->cfg->momentum) * (dsAccuCountA[valX] / 
                                                    ds->count));
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dsAccuMMIE
    --------------------------
    Update according to formulae (4.34) and (4.35) in the dissertaion of
    Dan Povey
   ------------------------------------------------------------------------ */

static double* k    = NULL;    /* the k_j[m] */
static int     kN   = 0;       /* number of values that the static arrays have been allocated for */

static int dsAccuMMIE( DistribAccu* dsaP, int subX) {
  Distrib* dsP       = dsaP->ds;
  int      nIt       = 100;
  int      i         = 0;
  int      valX      = 0;
  double   max       = 0.0;
  double   maxTmp    = 0.0;
  double   normTerm  = 0.0; /* Normalization term, is the denominator in (4.34) */
  double   normCnts0 = 0.0; /* Normalize the counts                             */
  double   normCnts1 = 0.0; /* for both sub accus                               */


  /* no accus, no update */
  if (!dsaP || !dsaP->count) return TCL_OK;

  if (dsaP->subN != 2) {
    ERROR("dsAccuMMIE : '%s' has %d subaccus, but should be 2\n",dsP->name,dsaP->subN);
    return TCL_ERROR;
  }
  
  /* Try out the new distribSet Update instead */
  /* do MLE update based on 'nominator' statistics */
  /* return dsAccuMLE(dsaP,0);*/

  /* assumes numerator statistics in sub-accu '0' and denominator statistics in sub-accu '1' */
  /* Update according to Dan Povey, formulae (4.34) and (4.35) in his dissertation */

  /* k_j,m, and the whole denominator of the update formulae are constant over the local iterations
     so calculate first k and then the denominator */

  /* Transform the mixture weights from the log domain */
  for (valX=0; valX < dsP->valN; valX++) dsP->val[valX] = exp((double)-1.0*dsP->val[valX]);

  /* Norm the counts for both sub-accumulators */
  for (valX=0; valX < dsP->valN; valX++) {
    normCnts0 += dsP->accu->count[0][valX];
    normCnts1 += dsP->accu->count[1][valX];
  }

  for (valX=0; valX < dsP->valN; valX++) {
    dsP->accu->count[0][valX] = dsP->accu->count[0][valX] / normCnts0;
    dsP->accu->count[1][valX] = dsP->accu->count[1][valX] / normCnts1;
  }

  /* static array does not exist or is too small */
  if (!k || kN < dsP->valN) {
    kN = dsP->valN;
    k  = (double*) realloc(k, kN*sizeof(double)); 
  }
  
  /* find max over all m of count(m)/val(m) */
  for (valX=0; valX < dsP->valN; valX++) {
    maxTmp = dsP->accu->count[1][valX]/dsP->val[valX];
    if (maxTmp > max) max=maxTmp;
  }

  /* now calculate k_j[m] */
  for (valX=0; valX < dsP->valN; valX++) k[valX] = max - (dsP->accu->count[1][valX]/dsP->val[valX]);

  /* Calculate the new mixture weights according to (4.34) */
  for (nIt=0; i < nIt; nIt++) {
    /* calculate the denominator in (4.34) */
    normTerm = 0.0;
    for (valX=0; valX < dsP->valN; valX++) normTerm = normTerm + dsP->accu->count[0][valX] + k[valX]*dsP->val[valX];
    /* update the weights */
    for (valX=0; valX < dsP->valN; valX++) dsP->val[valX] = (dsP->accu->count[0][valX] + k[valX]*dsP->val[valX]) / normTerm ;
  }

  /* Transform back to the log domain */
  for (valX=0; valX < dsP->valN; valX++) dsP->val[valX] = -log(dsP->val[valX]); 

  return TCL_OK;
}


/* ========================================================================
    Distrib
   ======================================================================== */

/* ------------------------------------------------------------------------
    Create Distrib Structure:
   ------------------------------------------------------------------------ */

static int dsInit (Distrib *dsP, ClientData cd) 
{
  assert(cd);

  if (! _bmemDSSP) {
     dsP->name   = strdup((char*)cd);
  } else {
    int n        = strlen ((char*)cd);
    dsP->name    = bmemAlloc (_bmemDSSP, n+1);    
    memcpy (dsP->name, cd, n);
    dsP->name[n] = 0;
  }
  dsP->useN  =  0;
  dsP->val   =  NULL;
  dsP->valN  =  0;
  dsP->count =  0.0;
  dsP->cbX   = -1;
  dsP->accu  =  NULL;
  dsP->cfg   =  &defaultDscfg;

  link(&defaultDscfg,"Dscfg");

  return TCL_OK;
}

static Distrib* dsCreate( char* name)
{
  Distrib* dsP = (Distrib*)malloc(sizeof(Distrib));

  if ((!dsP) || (dsInit(dsP, (ClientData)name) != TCL_OK)) {
    if (dsP) free (dsP);
    ERROR("Cannot create Distrib object.\n");
    return NULL;
  }
  return dsP;
}

/* ------------------------------------------------------------------------
    dsAlloc/Dealloc   (de)allocate data structures for distributions
   ------------------------------------------------------------------------ */

static int dsAlloc(Distrib* dsP, int cbX, int valN)
{
  int i;

  dsP->cbX    =  cbX;
  dsP->valN   =  valN;
  if (_bmemDSSP) {
    dsP->val  = bmemAlloc(_bmemDSSP,sizeof(float) * valN);
  } else {
    if (NULL == (dsP->val = malloc(sizeof(float) * valN))) {
      ERROR("dsAlloc: allocation failure\n");
      return TCL_ERROR;
    }
  }
  dsP->count  =  0.0;
  dsP->accu   =  NULL;

  for ( i = 0; i < valN; i++) dsP->val[i] = log((double)valN);
  return TCL_OK;
}

static int dsDealloc( Distrib* dsP)
{
  if (dsP->val && ! _bmemDSSP) free(dsP->val);  
  if ( dsP->accu) dsAccuFree( dsP->accu); 
  dsP->val  = NULL; 
  dsP->accu = NULL; 
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dsDeinit  deinitialize distribution structure
   ------------------------------------------------------------------------ */

static int dsDeinit (Distrib* dsP)
{
  assert(dsP);
  if (! _bmemDSSP && dsP->name) free( dsP->name);
  dsP->name = NULL;
  unlink(dsP->cfg,"Dscfg");
  return dsDealloc( dsP);
}

static int dsFree( Distrib* ds)
{
  if ( dsDeinit( ds) != TCL_OK) return TCL_ERROR;
  if ( ds) free( ds);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dsPutsItf
   ------------------------------------------------------------------------ */

static int dsPutsItf (ClientData cd, int argc, char *argv[])
{
  Distrib* dsP = (Distrib*)cd;
  int i;

  if (dsP->val) {
    itfAppendResult(" {");
    for (i=0; i<dsP->valN; i++) 
      itfAppendResult( " % 2.4e", exp((double)-1.0*dsP->val[i]));
    itfAppendResult("}");
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dsConfigure     configure a distribution type object
   ------------------------------------------------------------------------ */

static int dsConfigureItf (ClientData cd, char *var, char *val)
{
  Distrib* dsP = (Distrib*)cd;

  if (! var) {
    ITFCFG(dsConfigureItf,cd,"name");
    ITFCFG(dsConfigureItf,cd,"val");
    ITFCFG(dsConfigureItf,cd,"valN");
    ITFCFG(dsConfigureItf,cd,"count");
    ITFCFG(dsConfigureItf,cd,"cbX");
    ITFCFG(dsConfigureItf,cd,"cfg");
    return TCL_OK;
  }
  ITFCFG_CharPtr(var,val,"name",  dsP->name,       1);
  ITFCFG_Int(    var,val,"valN",  dsP->valN,       1);
  ITFCFG_Float(  var,val,"count", dsP->count,      0);
  ITFCFG_Int(    var,val,"cbX",   dsP->cbX,        0);
  ITFCFG_Object( var,val,"cfg",   dsP->cfg, name, Dscfg, 0);

  if (! strcmp(var,"val")) {
    if (! val) { 
      itfAppendResult(" {");
      if (dsP->val) {
        int i;
        for (i=0; i<dsP->valN; i++) 
         itfAppendResult( " % 2.4e", exp((double)-1.0*dsP->val[i]));
      }
      itfAppendResult("}");
      return TCL_OK;
    }
    else { 
      FArray fa;
      fa.itemA = dsP->val;
      fa.itemN = dsP->valN;
      if ( farrayGetItf( &fa, val) == TCL_OK) {
        float f = 0;
        int   i;
        for (i = 0; i < dsP->valN; i++) f += (dsP->val[i] < 0) ? 0 : dsP->val[i];
	if      (f  < 0) for (i = 0; i < dsP->valN; i++) dsP->val[i] = -log (1.0 / (float) dsP->valN);
	else if (f == 0) for (i = 0; i < dsP->valN; i++) dsP->val[i] = -log (1e-19);
	else             for (i = 0; i < dsP->valN; i++) dsP->val[i] = -log (dsP->val[i] / f);
        return TCL_OK;
      }
      else return TCL_ERROR;
    }
  }

  ERROR("Unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    dsAccess
   ------------------------------------------------------------------------ */

static ClientData dsAccessItf (ClientData cd, char *name, TypeInfo **ti)
{
  Distrib *ds=(Distrib*)cd;

  if (*name=='.') {
    if (name[1]=='\0') {
      if (ds->accu) itfAppendElement("accu");
      if (ds->cfg)  itfAppendElement("cfg");
    }
    else { 
      if (!strcmp(name+1,"accu")) {
	*ti=itfGetType("DistribAccu");
	return (ClientData)ds->accu;
      }
      else if (ds->cfg && strcmp(name+1,"cfg")==0) { 
        *ti = itfGetType("Dscfg"); 
        return (ClientData)(ds->cfg);
      }
    }
  }
  *ti=NULL;
  return NULL;
}

/* ------------------------------------------------------------------------
    dsCreateAccu
   ------------------------------------------------------------------------ */

static int dsCreateAccu( Distrib* ds, int subN)
{
  DistribAccu* dsa =  NULL;

  if ((dsa = dsAccuCreate(ds, subN))) {
    if ( ds->accu) dsAccuFree( ds->accu);
    ds->accu = dsa;
    return TCL_OK;
  }
  return TCL_ERROR;
}

static int dsCreateAccuItf( ClientData cd, int argc, char *argv[])
{
  int   ac    = argc-1;
  int   subN  = 1;

  if ( itfParseArgv(argv[0], &ac, argv+1, 1,
      "-subN",     ARGV_INT,   NULL, &subN,     NULL, "number of subaccumulators",
       NULL) != TCL_OK) return TCL_ERROR;

  return dsCreateAccu((Distrib*)cd,subN);
}

/* ------------------------------------------------------------------------
    dsFreeAccu
   ------------------------------------------------------------------------ */

static int dsFreeAccuItf( ClientData cd, int argc, char *argv[])
{
  Distrib* ds  = (Distrib*)cd;
  if ( ds->accu) dsAccuFree( ds->accu);
  ds->accu = NULL;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dsKullbackLeibler    compute (symmetrized) KL-distance between two distributions
   ----------------------------------------------------------------------- */

static double dsKullbackLeibler(Distrib *dst1, Distrib *dst2, DistribSet *dss) {
  Codebook *cb1P, *cb2P;
  double ret;
  cb1P = &(dss->cbsP->list.itemA[dst1->cbX]);
  cb2P = &(dss->cbsP->list.itemA[dst2->cbX]);
  ret = cbKullbackLeibler(cb1P, dst1->val, cb2P, dst2->val);
  return(ret);
}

static int dssKullbackLeiblerItf(ClientData cd, int argc, char *argv[])
{
  Distrib* dst1, *dst2;
  int      ac  = argc-1;
  DistribSet     *dssP = (DistribSet *)cd;
  double   dist;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<distribution 1>",  ARGV_OBJECT, NULL, &dst1, "Distrib", "name of first distribution",
      "<distribution 2>",  ARGV_OBJECT, NULL, &dst2, "Distrib", "name of second distribution",
		    NULL) != TCL_OK) return TCL_ERROR;
  dist = dsKullbackLeibler(dst1,dst2,dssP);
  itfAppendResult("%g", dist);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dsCopy    copy one distribution onto the other
   ------------------------------------------------------------------------ */

static int dsCopy( Distrib* dst, Distrib* src)
{
  int cbX  = dst->cbX;
  int valN = src->valN;
  int v;

  if ( src->val == NULL) return TCL_OK;
  if ( src == dst)       return TCL_OK;

  if ( dsDealloc( dst) != TCL_OK ||
       dsAlloc(   dst, cbX, valN) != TCL_OK) return TCL_ERROR;

  for( v = 0; v < valN; v++) dst->val[v] = src->val[v];

  if ( src->accu) {
    dst->accu = dsAccuCreate( dst, src->accu->subN);
    dsAccuCopy( dst->accu, src->accu);
  }
  return TCL_OK;
}

static int dsCopyItf (ClientData cd, int argc, char *argv[])
{
  Distrib* src = NULL;
  int      ac  = argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<source>",  ARGV_OBJECT, NULL, &src, "Distrib", 
      "name of the source distribution", NULL) != TCL_OK) return TCL_ERROR;

  return dsCopy((Distrib*)cd, src);
}

/* ------------------------------------------------------------------------
    dsSave   save distribution in machine independent format to a file
   ------------------------------------------------------------------------ */

static int dsSave( Distrib* dsP, char* cbName, FILE *fp)
{
   int refX;

   /* ---------------------------------- */
   /* write the name of the distribution */
   /* ---------------------------------- */
   write_string(fp,dsP->name);

   /* ----------------------------------- */
   /* write the name of the used codebook */
   /* ----------------------------------- */
   write_string(fp, cbName);

   /* ----------------------------------------------------- */
   /* write the size of the distribution (and the codebook) */
   /* The negative value indicates that there is also a     */
   /* count field present.                                  */
   /* ----------------------------------------------------- */
   write_int(fp,(-1)*dsP->valN);

   /* ----------------------------------------------------- */
   /* write the counts the distribution got during training */
   /* ----------------------------------------------------- */
   write_float(fp,dsP->count);

   /* ------------------------------------ */
   /* write the values of the distribution */
   /* ------------------------------------ */
   for (refX=0; refX<dsP->valN; refX++) write_float(fp,dsP->val[refX]);

   return TCL_OK;
}

/* ------------------------------------------------------------------------
    dsMap    create a new mixture from an existing one by using a 
             CodebookMap object which defines which reference vector maps
             to which new vector and where the sub accumulators are mapped
             to. This function is used as part of the codebook splitting
             routines.
   ------------------------------------------------------------------------ */

static Distrib* dsMap( Distrib* dsP, CodebookMap* mapP)
{
  Distrib*     ds      = NULL;
  DistribAccu* dsa     = NULL;
  DistribAccu* dsaP    = dsP->accu;
  int          i, refN = 0, refX;

  if (! mapP) return NULL;

  /* determine the size of the new distribution */

  for ( refX = -1, i = 0; i < mapP->itemN; i++)
    if ( refX < mapP->itemA[i].to) refX = mapP->itemA[i].to;

  refN = refX + 1;

  /* allocate and initialize the new distribution */

  if (! (ds = dsCreate( dsP->name)) ||
         dsAlloc( ds, dsP->cbX, refN) != TCL_OK) {
    dsFree( ds);
    ERROR("Can't allocate new distribution %s.\n", dsP->name);
    return NULL;
  }

  /* map the old distribution entries to the new distribution */

  for (i=0; i<mapP->itemN; i++) {
    int   from  = mapP->itemA[i].from;
    int   to    = mapP->itemA[i].to;
    float beta0 = mapP->itemA[i].beta0;
    float beta  = mapP->itemA[i].beta;

    if ( to >= 0) { 
      if ( from < 0 || from >= dsP->valN) {
        ds->val[to] = -log(beta0 * exp(- ds->val[to]));
      }
      else {
        ds->val[to] = -log(beta0 * exp(- ds->val[to]) +
                           beta  * exp(- dsP->val[from]));
      }
    }
  }

  /* if there are accumulators allocated for the given distribution, we
     have to map them according to the subspace specification in the
     codebook map */

  if (! dsaP) return ds;

  dsa = ds->accu = dsAccuCreate( ds,dsaP->subN);

  for (i=0; i<mapP->itemN; i++) {
    int   from  = mapP->itemA[i].from;
    int   to    = mapP->itemA[i].to;
    int   subX  = mapP->itemA[i].subX;

    if ( to >= 0 && from >= 0 && from < dsP->valN) {
      if ( subX < 0) {
        int   s;
        for ( s = 0; s < dsa->subN; s++) {
          dsa->count[s][to] += dsaP->count[s][from];
        }
      }
      else if ( subX > -1 && subX < dsa->subN) {
        dsa->count[subX][to] += dsaP->count[subX][from];
      }
    }
  }
  return ds;
}

/* ------------------------------------------------------------------------
   dsMerge 
   ------------------------------------------------------------------------ */

/* ASSERTIONS:
   1. fully continuous system
   2. existing accu's
   3. already updated codebooks and distrib's
*/

static int dsMerge(Codebook* cbP, DistribSet* dssP)
{
  int       refN = cbP->refN; 
  int       dimN = cbP->dimN;
  int        cbX = listIndex((List*) &(dssP->cbsP->list),cbP->name,0);
  int       subX = 0;
  double   *cntA = NULL;
  float   minCnt = FLT_MAX;    
  int refX,refY,dimX,dsX;

  if (cbP->accu && cbP->accu->count) {
    cntA = cbP->accu->count[subX];
  }
  if (! cntA) return TCL_OK;

  if (cbP->accu->subN > 1) {
    ERROR("dsMerge: %s has more than one sub accu (not supported) subN= %d\n",\
	  cbP->name,cbP->accu->subN);
    return TCL_ERROR;
  }

  for (refX=0;refX<refN;refX++)
    if (cntA[refX] < minCnt && cntA[refX] > 0.0) minCnt= cntA[refX];
  
  while ( minCnt < cbP->cfg->mergeThresh) {
    float minDelta = FLT_MAX;
    int mergeX=0,mergeY=0;

    for (refX=0;refX<refN;refX++) {
      int      c1 = cntA[refX];
      float  det1 = cbP->cv[refX]->det;

      if (c1 < 1.0) continue;

      for (refY=refX+1;refY<refN;refY++) {
	int       c2 = cntA[refY];
	float   det2 = cbP->cv[refY]->det;
	float   det3 = 0.0;
	float  delta = 0.0;

	if (c2 < 1.0) continue;

	for (dimX=0;dimX<dimN;dimX++) {
	  float mean =  cbP->accu->rv[subX]->matPA[refX][dimX] +\
	                cbP->accu->rv[subX]->matPA[refY][dimX];
	  float sq   =  cbP->accu->sumOsq[subX][refX].m.d[dimX] +\
            	        cbP->accu->sumOsq[subX][refY].m.d[dimX];
	  float cnt  =  cbP->accu->count[subX][refX] + cbP->accu->count[subX][refY];
	  float val  =  sq/cnt - mean*mean/(cnt*cnt);
	  det3 += log(val);
	}

	delta=(c1+c2)*det3/2.0 - c1*det1/2.0 - c2*det2/2.0;
	if (delta < minDelta) {
	  minDelta = delta;
	  mergeX = refX;
	  mergeY = refY;
	}
      }
    }

    if (minDelta == FLT_MAX) break;

    INFO("dsMerge: %s merge gaussian components %d (cnt= %f) with %d (cnt= %f)\n",\
	 cbP->name,\
	 mergeX,cbP->accu->count[subX][mergeX],\
	 mergeY,cbP->accu->count[subX][mergeY]);

    /* update codebook means for mergeX */
    for (dimX=0;dimX<dimN;dimX++) {
      cbP->rv->matPA[mergeX][dimX]  = cbP->accu->rv[subX]->matPA[mergeX][dimX];
      cbP->rv->matPA[mergeX][dimX] += cbP->accu->rv[subX]->matPA[mergeY][dimX];
      cbP->rv->matPA[mergeX][dimX] /= (cbP->accu->count[subX][mergeX] + cbP->accu->count[subX][mergeY]);
    }

    /* update codebook covariances for mergeX */
    cbP->cv[mergeX]->det = 0.0;
    for (dimX=0;dimX<dimN;dimX++) {
      float mean =  cbP->accu->rv[subX]->matPA[mergeX][dimX] +\
                    cbP->accu->rv[subX]->matPA[mergeY][dimX];
      float sq   =  cbP->accu->sumOsq[subX][mergeX].m.d[dimX] +\
            	    cbP->accu->sumOsq[subX][mergeY].m.d[dimX];
      float cnt  =  cbP->accu->count[subX][mergeX] + cbP->accu->count[subX][mergeY];
      float val  =  sq/cnt - mean*mean/(cnt*cnt);

      if ( val <= COV_FLOOR) val = COV_FLOOR;
      cbP->cv[mergeX]->m.d[dimX]  = 1.0/ val;
      cbP->cv[mergeX]->det += log(val);
    }

    /* update counts for mergeX */
    cbP->accu->count[subX][mergeX] += cbP->accu->count[subX][mergeY];
    cntA[mergeX]                   += cntA[mergeY];
    cbP->count[mergeX]             += cbP->count[mergeY];

    /* deactivate means, covars, counts for mergeY */
    for (dimX=0;dimX<dimN;dimX++) {
      cbP->rv->matPA[mergeY][dimX] = 0.0;
      cbP->cv[mergeY]->m.d[dimX]   = 0.0;
    }
    cbP->count[mergeY]   = 0.0;
    cntA[mergeY]         = 0.0;
    cbP->cv[mergeY]->det = 0.0;
    cbP->accu->count[subX][mergeY] = 0.0;

    /* go to all distrib' that point to this codebook*/
    for (dsX=0;dsX<dssP->list.itemN;dsX++) {
      Distrib* dsP = dssP->list.itemA+dsX;

      if (dsP->cbX  != cbX) continue;
      if (dsP->valN != refN) {
	ERROR("distrib/codebook mismatch : ds= %s cb= %s valN= %d  refN= %d\n",\
	      dsP->name,cbP->name,dsP->valN,refN);
	return TCL_ERROR;
      }

      dsP->val[mergeX] = -1.0* log (\
				    exp((double)-1.0*dsP->val[mergeX]) +\
				    exp((double)-1.0*dsP->val[mergeY]));

      
      dsP->val[mergeY]     = FLT_MAX;
      dsP->accu->count[subX][mergeX] += dsP->accu->count[subX][mergeY];
      dsP->accu->count[subX][mergeY]  = 0.0;
    }

    minCnt= FLT_MAX;
    for (refX=0;refX<refN;refX++)
      if (cntA[refX] < minCnt && cntA[refX] > 0.0) minCnt= cntA[refX];

  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
   dsSplit
   ------------------------------------------------------------------------ */

/* ASSERTIONS:
   1. existing accu's and subN= 1)
   2. already updated codebooks and distrib's
*/

typedef struct {
  int   refX;
  float score;
} SortElement;

static int compareSortElement(const void * x, const void *y) {
  SortElement* xs = (SortElement*) x;
  SortElement* ys = (SortElement*) y;
  if      ( xs->score > ys->score) return -1;
  else if ( xs->score < ys->score) return  1;
  else                             return  0;
}


static int dsSplit( Codebook* cbP, DistribSet* dssP)
{
  int        subX = 0;
  int        subN = 1;
  int         cbX = listIndex((List*) &(dssP->cbsP->list),cbP->name,0);
  int        refN = cbP->refN; 
  int        dimN = cbP->dimN;
  double    *cntA = NULL;
  int     usedCnt = 0;
  int        refM = 0;
  int    *usedIdx = NULL;
  FMatrix     *rv = NULL; 
  FCovMatrix **cv = NULL; 
  float      *cnt = NULL; 
  float      *val = NULL; 
  SortElement *cL = NULL;

  int dsX,refX,refY,dimX;

  if (cbP->accu && cbP->accu->count)
    cntA = cbP->accu->count[subX];

  if (! cntA || refN == 0) return TCL_OK;

  if (_bmemDSSP) {
    ERROR("dsSplit: bmem operations not supported\n"); 
    return TCL_ERROR;
  }

  if (NULL == (usedIdx= malloc(refN*sizeof(int)))) {
    ERROR("allocation error\n"); 
    return TCL_ERROR;
  }

  /* get number of used components */  
  for (refX=0;refX<refN;refX++) {
    if (cntA[refX] >  2 * cbP->cfg->mergeThresh) {
      /*enough to split*/
      usedCnt++;
      refM += 2;
      usedIdx[refX]=2;
      continue;
    } 
    if (cntA[refX] >  1 * cbP->cfg->mergeThresh) {
      /*enough to train*/
      usedCnt++;
      refM += 1;
      usedIdx[refX]=1;
      continue;
    } 
    usedIdx[refX]=0;
    INFO("dsSplit cb= %s refX= %d cnt= %f deactivated\n",cbP->name,refX,cntA[refX]);  
  }

  if (usedCnt == 0) {
    /* WARN("dsSplit %s no active components\n",cbP->name); */
    free(usedIdx);
    return TCL_OK;
  }

  /* alloc list of sortElements */
  if (NULL == (cL = ( SortElement*) malloc (usedCnt * sizeof(SortElement)))) {
    ERROR("allocation error\n");
    return TCL_ERROR;
  }

  /* compute relevance (=count) of each component and sort list */
  refY=0;
  for (refX=0;refX<refN;refX++) {
    if (usedIdx[refX] > 0) {
      cL[refY].refX  = refY;
      cL[refY].score = cntA[refX]; 
      refY++;
    }
  }
  qsort(cL,usedCnt, sizeof(SortElement), compareSortElement);

  /* split all used components into two new components */
  if (refM > cbP->refMax) refM= cbP->refMax;
  if (usedCnt > refM) {
    ERROR("dsSplit %s cannot store old model old= %d new= %d\n",cbP->name,usedCnt,refM);
    return TCL_ERROR;
  }
  if (refM > CBX_MAX+1) {
    ERROR("dsSplit: maximum nr. of gaussians (CBX_MAX= %d) exceeded in %s\n",
	  CBX_MAX,cbP->name);
    return TCL_ERROR;
  }

  /* codebook mean's*/
  rv= fmatrixCreate(refM,dimN);
  refY=0;
  for (refX=0;refX<refN;refX++) {
    if (usedIdx[refX] > 0) {
      memcpy(rv->matPA[refY],cbP->rv->matPA[refX],dimN*sizeof(float));
      refY++;
    }
  }
  fmatrixFree(cbP->rv);
  cbP->rv= rv;

  /* codebook covar's*/
  if (NULL == (cv = ( FCovMatrix**) malloc (refM * sizeof(FCovMatrix*)))) {
    ERROR("dsSplit: allocation error\n");
    return TCL_ERROR;
  }
  for (refX=0;refX<refM;refX++)
    cv[refX]=fcvCreate(dimN,cbP->type);

  refY=0;
  for (refX=0;refX<refN;refX++) {
    if (usedIdx[refX] > 0) {
      fcvCopy(cv[refY],cbP->cv[refX]);
      refY++;
    }
    fcvFree(cbP->cv[refX]);      
  }
  free(cbP->cv);
  cbP->cv= cv;

  /*codebook count's*/
  if (NULL == (cnt = ( (float*) malloc (refM * sizeof(float))))) {
    ERROR("allocation error\n");
    return TCL_ERROR;
  }
  for (refX=0;refX<refM;refX++) cnt[refX]=0.0;
  refY=0;
  for (refX=0;refX<refN;refX++) {
    if (usedIdx[refX] > 0) {
      cnt[refY]=cbP->count[refX];
      refY++;
    }
  }
  free(cbP->count);
  cbP->count= cnt;
        
  /*codebook accu's*/
  cbP->refN = refM;
  if (cbP->accu) { 
    cbP->accu->count[subX] = realloc(cbP->accu->count[subX],refM*sizeof(double));
    if ( cbP->accu->count[subX] == NULL) {
      ERROR("allocation failure\n");
      return TCL_ERROR;
    }
    for (refX=0; refX<refM; refX++) {
      cbP->accu->count[subX][refX]=0.0;
    }
    dmatrixResize(cbP->accu->rv[subX],refM,dimN);
    dmatrixClear(cbP->accu->rv[subX]);

    for (refX=0; refX<refN; refX++) {
      dcvDeinit(cbP->accu->sumOsq[subX]+refX);
    }
    cbP->accu->sumOsq[subX] = realloc(cbP->accu->sumOsq[subX],refM*sizeof(DCovMatrix));
    if ( cbP->accu->sumOsq[subX] == NULL) {
      ERROR("allocation failure\n");
      return TCL_ERROR;
    }
    for (refX=0; refX<refM; refX++) {
      dcvInit(cbP->accu->sumOsq[subX]+refX,dimN,cbP->type);
    }
  } 

  /* split all (used) codebook components */
  for (refX = 0; refX < refM - usedCnt;refX++) {
    int refA = cL[refX].refX; /* component to split */
    int refB = usedCnt +refX; /* next free component for spliting */

    if (refA >= refM || refB >= refM) {
      ERROR("dsSplit cb %s index overflow refMax= %d split: %d -> %d\n",cbP->name,refM,refA,refB);
      return TCL_ERROR;
    }

    if (cbP->cv[refA]->type == COV_RADIAL) {
      for (dimX=0;dimX<dimN;dimX++) {
	cbP->rv->matPA[refB][dimX]  = cbP->rv->matPA[refA][dimX];
	cbP->rv->matPA[refB][dimX] -= cbP->cfg->splitStep / cbP->cv[refA]->m.r;
	cbP->rv->matPA[refA][dimX] += cbP->cfg->splitStep / cbP->cv[refA]->m.r;
      }
    } else if (cbP->cv[refA]->type == COV_DIAGONAL) {
      for (dimX=0;dimX<dimN;dimX++) {
	cbP->rv->matPA[refB][dimX]  = cbP->rv->matPA[refA][dimX];
	cbP->rv->matPA[refB][dimX] -= cbP->cfg->splitStep / cbP->cv[refA]->m.d[dimX];
	cbP->rv->matPA[refA][dimX] += cbP->cfg->splitStep / cbP->cv[refA]->m.d[dimX];
      }
    } else {
      ERROR("dsSplit: %s has unsupported covariance type.\n",cbP->name);
      return TCL_ERROR;
    }
    fcvCopy(cbP->cv[refB],cbP->cv[refA]);
    cbP->count[refA] /= 2.0;
    cbP->count[refB]  = cbP->count[refA];    
  }
 
  /* go to all distrib' that point to this codebook*/
  for (dsX=0;dsX<dssP->list.itemN;dsX++) {
    Distrib* dsP = dssP->list.itemA+dsX;
    int valN = dsP->valN;
    int valX,valY,valM;

    if (dsP->cbX != cbX) continue;
    if (valN != refN) {
      ERROR("distrib/codebook mismatch : ds= %s cb= %s valN= %d  refN= %d\n",\
	    dsP->name,cbP->name,valN,refN);
      return TCL_ERROR;
    }

    valM= refM;

    /*distrib weights*/
    if (NULL == (val = ( (float*) malloc (valM * sizeof(float))))) {
      ERROR("allocation error\n");
      return TCL_ERROR;
    }
    for (valX=0;valX<valM;valX++) val[valX]=FLT_MAX;
    valY=0;
    for (valX=0;valX<valN;valX++) {
      if (usedIdx[valX] > 0) {
	val[valY]=dsP->val[valX];
	valY++;
      }
    }
    free(dsP->val);
    dsP->val= val;
 
    /* distrib accu's */
    if (dsP->accu) {  
      dsAccuFree(dsP->accu); 
      dsP->valN = valM;
      dsP->accu = dsAccuCreate(dsP,subN);
    } else {
      dsP->valN = valM;
    }

    /* split all distrib (used) components */
    for (valX = 0; valX < refM - usedCnt;valX++) {
      int valA = cL[valX].refX; /* component to split */
      int valB = usedCnt +valX; /* next free component for spliting */

      if (valA >= valM || valB >= valM) {
	ERROR("dsSplit %s ds index overflow valMax= %d split: %d -> %d\n",dsP->name,valM,valA,valB);
	return TCL_ERROR;
      }

      dsP->val[valA]  = -1.0* log ( 0.5* exp((double)-1.0*dsP->val[valA]));
      dsP->val[valB]  = dsP->val[valA];
    }
  }

  free(usedIdx);
  free(cL);

  return TCL_OK;
}

/* ========================================================================
    DistribSet
   ======================================================================== */

/* check if index is a normal ds-index or a dummy index 
   returns 1 if it is a dummy index otherwise 0         
*/
#define DUMMYINDEX(dssP,idx) (idx >= dssP->dummyStart)

static DistribSet dssDefaults;

/* ------------------------------------------------------------------------
    Create Distribution Set Structure
   ------------------------------------------------------------------------ */

static int dssInit( DistribSet* dssP, char* name, CodebookSet* cbsP, int bmem)
{
  dssP->name         = strdup(name);
  dssP->useN         = 0;

  listInit((List*)&(dssP->list), &distribInfo, sizeof(Distrib),
                                  dssDefaults.list.blkSize);

  dssP->list.init    = (ListItemInit  *)dsInit;
  dssP->list.deinit  = (ListItemDeinit*)dsDeinit;

  dssP->minCount     =  0;
  dssP->cbsP         =  cbsP; link(dssP->cbsP,"CodebookSet");
  dssP->rwsP         =  NULL;
  dssP->distance     =  'e'; /* 'e' = entropy, 'l' = likelihood */
  dssP->commentChar  =  dssDefaults.commentChar;

  dssP->dummyStart   =  dssDefaults.dummyStart;
  dssP->dummyName    =  strdup(dssDefaults.dummyName);
  dssP->stP          =  dssDefaults.stP;

  /* use all sub-accus */
  dssP->subIndex     = -1;

  dssP->normDistance = dssDefaults.normDistance;

  /* Block memory management */
  if (bmem) {
    if (!_bmemDSSP) _bmemDSSP = bmemCreate (65536, 0);
    _bmemDSSN++;
  } 

  return TCL_OK;
}

static DistribSet* dssCreate (char* name, CodebookSet* cbsP, int bmem)
{
  DistribSet* dssP = (DistribSet*)malloc(sizeof(DistribSet));

  if (! dssP || dssInit( dssP, name, cbsP, bmem) != TCL_OK) {
    if ( dssP) free( dssP); 
    ERROR("Failed to allocate distribution set '%s'.\n", name); 
    return NULL;
  }
  return dssP;
}

static ClientData dssCreateItf (ClientData cd, int argc, char *argv[])
{
  CodebookSet*  cbsP  = NULL;
  int           ac    = argc /* (argc > 1) ? 1 : 0 */ ;
  char          *name = NULL; 
  int           bmem  = 0;

  if ( itfParseArgv( argv[0], &ac, argv, 1,
      "<name>",        ARGV_STRING, NULL, &name, NULL,          "name of the distrib set",
      "<CodebookSet>", ARGV_OBJECT, NULL, &cbsP, "CodebookSet", "set of codebooks",
      "-bmem",         ARGV_INT,    NULL, &bmem, NULL,          "use block memory management",
       NULL) != TCL_OK) return NULL;

  return (ClientData) dssCreate (name, cbsP, bmem);
}

/* ------------------------------------------------------------------------
    Free DistribSet Structure
   ------------------------------------------------------------------------ */

static int dssLinkN( DistribSet* dssP)
{
  int  useN = listLinkN((List*)&(dssP->list)) - 1;
  if ( useN < dssP->useN) return dssP->useN;
  else                    return useN;
}

static int dssDeinit (DistribSet* dssP)
{
  int rc;
  if (dssP==NULL) { ERROR("null argument"); return TCL_ERROR; }

  if ( dssLinkN(dssP)) { 
    SERROR("DistribSet '%s' still in use by other objects.\n", dssP->name);
    return TCL_ERROR;
  }  

  if (dssP->name)  free( dssP->name);
  if (dssP->cbsP)  unlink(dssP->cbsP, "CodebookSet");
  if (dssP->stP)   unlink(dssP->stP,  "StateTable");

  rc = listDeinit( (List*)&(dssP->list));

  if (_bmemDSSP && !(--_bmemDSSN)) {
    bmemFree (_bmemDSSP);
    _bmemDSSP = NULL;
  }

  return rc;
}

static void dssFree (DistribSet* dssP)
{
  dssDeinit( dssP);
  if ( dssP) free( dssP);
}

static int dssFreeItf (ClientData cd )
{
  dssFree((DistribSet*)cd);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dssConfigureItf
   ------------------------------------------------------------------------ */

static int dssConfigureItf( ClientData cd, char *var, char *val)
{
  DistribSet* dssP = (DistribSet*)cd;
  if (! dssP) dssP = &dssDefaults;

  if (! var) {
    ITFCFG(dssConfigureItf,cd,"name");
    ITFCFG(dssConfigureItf,cd,"useN");
    ITFCFG(dssConfigureItf,cd,"minCount");
    ITFCFG(dssConfigureItf,cd,"codebookSet");
    ITFCFG(dssConfigureItf,cd,"rewriteSet");
    ITFCFG(dssConfigureItf,cd,"distance"); 
    ITFCFG(dssConfigureItf,cd,"normDistance"); 
    ITFCFG(dssConfigureItf,cd,"stateTable");
    ITFCFG(dssConfigureItf,cd,"dummyName");
    ITFCFG(dssConfigureItf,cd,"dummyStart");
    ITFCFG(dssConfigureItf,cd,"subX");
    return listConfigureItf((ClientData)&(dssP->list), var, val);
  }
  ITFCFG_CharPtr (var,val,"name",             dssP->name,             1);
  ITFCFG_Int     (var,val,"useN",             dssP->useN,             1);
  ITFCFG_Float   (var,val,"minCount",         dssP->minCount,         0);
  ITFCFG_Char    (var,val,"distance",         dssP->distance,         0);
  ITFCFG_Int     (var,val,"normDistance",     dssP->normDistance,     0);
  ITFCFG_CharPtr (var,val,"dummyName",        dssP->dummyName,        0);
  ITFCFG_Int     (var,val,"dummyStart",       dssP->dummyStart,       1);
  ITFCFG_Object  (var,val,"codebookSet",      dssP->cbsP,name,  CodebookSet, 1);
  ITFCFG_Object  (var,val,"rewriteSet",       dssP->rwsP,name,  RewriteSet,  0);
  ITFCFG_Object  (var,val,"stateTable",       dssP->stP,name,   StateTable,  0);
  ITFCFG_Int     (var,val,"subX",             dssP->subIndex,0);
  return listConfigureItf((ClientData)&(dssP->list), var, val);
}

/* ------------------------------------------------------------------------
    dssAccessItf
   ------------------------------------------------------------------------ */

static ClientData dssAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  DistribSet* dssP  = (DistribSet*)cd;
  int dsX;

  if (*name == '.') { 
    if (name[1]=='\0') {
      if ( dssP->list.itemN)
           itfAppendElement("distrib(0..%d)", dssP->list.itemN-1);
      itfAppendElement("codebookSet");
      itfAppendElement("rewriteSet");
      if ( dssP->stP )
	  itfAppendElement("stateTable");
    }
    else {
      if (! strcmp(name+1,"codebookSet")) {
	 *ti = itfGetType("CodebookSet");
	 return (ClientData)dssP->cbsP;
      }
      else if (!strcmp(name+1,"rewriteSet")) {
	 *ti = itfGetType("RewriteSet");
	 return (ClientData)dssP->rwsP;
      }
      else if (!strcmp(name+1,"stateTable")) {
	 *ti = itfGetType("StateTable");
	 return (ClientData)dssP->stP;
      }
      else if ( sscanf(name+1,"distrib(%d)",&dsX)==1) {
	*ti = itfGetType("Distrib");
	if (dsX >= 0 && dsX < dssP->list.itemN) 
            return (ClientData)&(dssP->list.itemA[dsX]); else return NULL;
      }
    }
  }
  return (ClientData)listAccessItf((ClientData)&(dssP->list),name,ti);
}

/* ------------------------------------------------------------------------
    dssPutsItf
   ------------------------------------------------------------------------ */

static int dssPutsItf( ClientData cd, int argc, char *argv[])
{
  DistribSet* dssP  = (DistribSet*)cd;
  return listPutsItf( (ClientData)&(dssP->list), argc, argv);
}

/* ------------------------------------------------------------------------
    dssIndex
   ------------------------------------------------------------------------ */

int dssIndex( DistribSet* dss,  char* name) {
  return listIndex((List*)&(dss->list), name, 0);
}

static int dssIndexItf( ClientData clientData, int argc, char *argv[] )
{
  DistribSet* dss = (DistribSet*)clientData;
  return listName2IndexItf((ClientData)&(dss->list), argc, argv);
}

/* ------------------------------------------------------------------------
    dssName
   ------------------------------------------------------------------------ */

char* dssName( DistribSet* dss, int i) {
  return (i < 0 || i >= dss->list.itemN) ? "(null)" : dss->list.itemA[i].name;
}

static int dssNameItf( ClientData clientData, int argc, char *argv[] )
{
  DistribSet* dss = (DistribSet*)clientData;
  return listIndex2NameItf((ClientData)&(dss->list), argc, argv);
}

/* ------------------------------------------------------------------------
    dssNumber
   ------------------------------------------------------------------------ */

int dssNumber( DistribSet* dss) {
  return dss->list.itemN;
}

/* ------------------------------------------------------------------------
    dssScoreArray   compute the scores of a set of mixture of gaussians
   ------------------------------------------------------------------------ */

float dssScoreArray( DistribSet* dssP, int* dsA, float* scoreA,
                                       int  dsN, int frameX)
{
  if ( dsN == 1) {
    int idx = dsA[0];
    Distrib *ds;

    if ( dssP->stP && DUMMYINDEX(dssP, dsA[0]) ) {
      idx = stateTableLookup(dssP->stP, dsA[0], frameX);
    }

    ds = dssP->list.itemA + idx;

    /* -------------------------- */
    /* compute distribution score */
    /* -------------------------- */

    scoreA[0] = dssP->cbsP->score( dssP->cbsP, ds->cbX, ds->val, frameX,
                                   NULL);
  } else {
    int i;

    for ( i = 0; i < dsN; i++) {
      int idx = dsA[i];
      Distrib *ds;

      if ( dssP->stP && DUMMYINDEX(dssP, dsA[i]) ) {
	idx = stateTableLookup(dssP->stP, dsA[i], frameX);
      }

      ds = dssP->list.itemA + idx;

      /* -------------------------- */
      /* compute distribution score */
      /* -------------------------- */

      scoreA[i] = dssP->cbsP->score( dssP->cbsP, ds->cbX, ds->val, frameX,
                                     NULL);
    }
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dssScore    compute the score of a given mixture of gaussians
   ------------------------------------------------------------------------ */

float dssScore( DistribSet* dssP, int dsX, int frameX)
{
  int idx = dsX;
  Distrib *ds;

  if ( dssP->stP && DUMMYINDEX(dssP, dsX) ) {
    idx = stateTableLookup(dssP->stP, dsX, frameX);
  }

  ds  = dssP->list.itemA + idx;

  return dssP->cbsP->score(dssP->cbsP,ds->cbX,ds->val,frameX,NULL);
}

int dssScoreItf( ClientData cd, int argc, char *argv[])
{
  DistribSet* dssP   = (DistribSet*)cd;
  int         frameX =  0, dsX, ac = argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<distrib>", ARGV_LINDEX, NULL, &dsX,&(dssP->list), "name of the distribution",
      "<frame>",   ARGV_INT,    NULL, &frameX, 0, "index of the requested frame",
       NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult("%f", dssScore( dssP, dsX, frameX));
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dssScoreNBest  find the n-bestscoring mixture of gaussians
   ------------------------------------------------------------------------ */

typedef struct {
  int   dsX;
  float score;               
} ScoreList;

static int compareScoreListItem(const void * a, const void *b)
{
  ScoreList* bpa = (ScoreList *)a;
  ScoreList* bpb = (ScoreList *)b;

  if      ( bpa->score < bpb->score) return -1;
  else if ( bpa->score > bpb->score) return  1;
  else                               return  0;
}

static int dssScoreNBestItf( ClientData cd, int argc, char *argv[])
{
  DistribSet* dssP   = (DistribSet*)cd;
  int         frameX =  0, n = 1, ac = argc-1;
  

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<n>",     ARGV_INT,    NULL, &n,      0, "length of list",
      "<frame>", ARGV_INT,    NULL, &frameX, 0, "index of the requested frame",
       NULL) != TCL_OK) return TCL_ERROR;

  if (n) {

    int dsX;
    int*   dsXA   = (int  *)malloc( dssP->list.itemN * sizeof(int));
    float* scoreA = (float*)malloc( dssP->list.itemN * sizeof(float));
    ScoreList* sc = (ScoreList*)malloc( dssP->list.itemN * sizeof(ScoreList));

    for (dsX=0; dsX<dssP->list.itemN; dsX++) dsXA[dsX] = dsX;

    dssScoreArray( dssP, dsXA, scoreA, dssP->list.itemN, frameX);

    for (dsX=0; dsX<dssP->list.itemN; dsX++) {
      sc[dsX].dsX   = dsX;
      sc[dsX].score = scoreA[dsX];
    }

    qsort( sc, dssP->list.itemN, sizeof(ScoreList), compareScoreListItem);

    for (dsX=0; dsX<n; dsX++) {
      itfAppendResult(" {%s %e}", dssP->list.itemA[sc[dsX].dsX].name,
                                  sc[dsX].score);
    }
    if ( sc)     free( sc);
    if ( dsXA)   free( dsXA);
    if ( scoreA) free( scoreA);
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dssDeleteItf
   ------------------------------------------------------------------------ */

static int dssDeleteItf( ClientData cd, int argc, char *argv[])
{
  DistribSet* dssP  = (DistribSet*)cd;
  return listDeleteItf( (ClientData)&(dssP->list), argc, argv);
}

/* ------------------------------------------------------------------------
    dssAdd   add a new distribution to the distribution set
   ------------------------------------------------------------------------ */

int dssAdd (DistribSet* dssP, char* name, int cbX)
{
  Distrib* dsP;
  int idx = listIndex((List*)&(dssP->list),(ClientData)name,0);

  if (idx >= 0) { 
    dsP = dssP->list.itemA+idx;
    dsDealloc(dsP);
  } else {
    idx = listNewItem((List*)&(dssP->list),(ClientData)name);
  }

  if ( cbX >= 0) 
    dsAlloc( dssP->list.itemA+idx, cbX,dssP->cbsP->list.itemA[cbX].refN);

  /* check if it is a dummy distribution and index not set */
  if ( !strncmp(name,dssP->dummyName,strlen(dssP->dummyName)) &&
       dssP->dummyStart < 0 ) {

    dssP->dummyStart = idx;

    /*
    if ( !dssP->stP )
	WARN("Dummy name '%s' used for distrib '%s' and no state table linked "
	     "to '%s'.\n", dssP->dummyName, name, dssP->name);
    */
  }

  return idx;
}

static int dssAddItf( ClientData cd, int argc, char *argv[])
{
  DistribSet* dssP   = (DistribSet*)cd;
  int         ac     =  argc - 1;
  char*       name   =  NULL;
  int         cbX    =  -1;
  //int         idx;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<name>",     ARGV_STRING, NULL, &name, cd, "name of the distribution",
      "<codebook>", ARGV_LINDEX, NULL, &cbX,&(dssP->cbsP->list), "name of the codebook",
       NULL) != TCL_OK) return TCL_ERROR;

  if (dssAdd( dssP, name, cbX) < 0) 
       return TCL_ERROR;
  else return TCL_OK;
}

/* ------------------------------------------------------------------------
    dssRead   read a distribution set description file
   ------------------------------------------------------------------------ */

static int dssReadItf (ClientData cd, int argc, char *argv[])
{
  DistribSet* dssP  = (DistribSet*)cd;
  int         ac    =  argc - 1;
  char*       fname =  NULL;
  int         n;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &fname, cd, "file to read from",
       NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult( "%d", n = freadAdd( cd, fname, dssP->commentChar,
                                                  dssAddItf));
  return (n >= 0) ? TCL_OK : TCL_ERROR;
}

/* ------------------------------------------------------------------------
    dssWrite   write distribution set description file
   ------------------------------------------------------------------------ */

static int dssWrite (DistribSet* dssP, char* fileName)
{
  FILE*      fp;
  int        dsX;

  assert( dssP && fileName);

  if (! (fp = fileOpen( fileName,"w"))) return TCL_ERROR;

  fprintf( fp, "%c -------------------------------------------------------\n", dssP->commentChar);
  fprintf( fp, "%c  Name            : %s\n",                                   dssP->commentChar, dssP->name);
  fprintf( fp, "%c  Type            : DistribSet\n",                           dssP->commentChar);
  fprintf( fp, "%c  Number of Items : %d\n",                                   dssP->commentChar, dssP->list.itemN);
  fprintf( fp, "%c  Date            : %s",                                     dssP->commentChar, dateString());
  fprintf( fp, "%c -------------------------------------------------------\n", dssP->commentChar);

  for (dsX=0; dsX<dssP->list.itemN; dsX++) {
    int cbX=dssP->list.itemA[dsX].cbX;
    fprintf( fp,"%-16s %-16s\n",dssP->list.itemA[dsX].name,
                           cbsName(dssP->cbsP, cbX));
  }
  fileClose( fileName, fp);
  return TCL_OK;
}

static int dssWriteItf (ClientData cd, int argc, char *argv[])
{
  DistribSet* dssP  = (DistribSet*)cd;
  int         ac    =  argc - 1;
  char*       fname =  NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &fname, cd, "file to read from",
       NULL) != TCL_OK) return TCL_ERROR;

  return dssWrite(dssP,fname);
}

/* ------------------------------------------------------------------------
    dssLoad    load distributions from a file written in machine 
               independent I/O format
   ------------------------------------------------------------------------ */

#define DS_MAGIC 22223333

static int dsLoad (DistribSet *dssP, FILE *fp)
{
  char name[MAX_NAME];		      /* will hold distribution and codebook */
  int  dsX, cbX, refX, refN;
  
  Distrib	*dsP;	      /* local pointer to the distribution struct */
  CodebookSet	*cbsP;	      /* for not having to dereferenciate twice */

  cbsP=dssP->cbsP;

  /* ---------------------------------- */
  /* check the name of the distribution */
  /* ---------------------------------- */

  read_string (fp, name);
  if (name != rwsRewrite (dssP->rwsP, name))
    strcpy (name, rwsRewrite(dssP->rwsP, name));

  if ((dsX=listIndex((List*)&(dssP->list),(ClientData)name,0))<0)
  { 
/*
     WARN("Undefined distribution: %s in weightsfile.\n",name);
*/
     /* ---------------------------------------------------------- */
     /* undefined? ... ignore the rest of the distribution weights */
     /* ---------------------------------------------------------- */

     read_string(fp,name);   refN=read_int(fp);

     /* ------------------------------------------- */
     /* is a 'count' field present? if yes, read it */
     /* ------------------------------------------- */
     if ( refN<0) { (void)read_float(fp); refN = (-refN); }

     for (refX=0; refX<refN; refX++) (void)read_float(fp);

     /* ---------------------------------------------------------------- */
     /* now the filepointer is at the beginning of the next distribution */
     /* ---------------------------------------------------------------- */

     return TCL_OK;
  }
  dsP = &(dssP->list.itemA[dsX]);

  /* ----------------------------------- */
  /* check the name of the used codebook */
  /* ----------------------------------- */

  read_string (fp, name);
  if (name != rwsRewrite (dssP->cbsP->rwsP, name))
    strcpy (name, rwsRewrite (dssP->cbsP->rwsP, name));

  if ((cbX=listIndex((List*)&(dssP->cbsP->list),(ClientData)name,0))<0)
    WARN("Distribution %s uses undefined codebook %s in weightsfile",
          dsP->name,name);

  /* ------------------------------------ */
  /* check the index of the used codebook */
  /* ------------------------------------ */

  if (cbX>=0 && cbX!=dsP->cbX) { 
    WARN("Codebook mismatch: %s %s: %s %d / %s %d\n",dsP->name, name,
          cbsP->list.itemA[dsP->cbX].name,dsP->cbX,
          cbsP->list.itemA[     cbX].name,     cbX);
    cbX = dsP->cbX;
  }

  /* ------------------------------------------------------------------- */
  /* Read the size of the distribution.                                  */
  /* If the size is negative, another file format has been used. In this */
  /* case, immediately after refN one float is in the file which gives   */
  /* the number of counts this distribution got during training, and refN*/
  /* is just (-1.0 * refN). This is to ensure harmony between the old    */
  /* file format without the count field and the newer one with count.   */
  /* ------------------------------------------------------------------- */

  refN=read_int(fp);

  /* ------------------------------------------------------ */
  /* is a count field present? Indicator is a negative refN */
  /* ------------------------------------------------------ */
  if (refN<0) { refN = (-refN); dsP->count = read_float(fp); }

  /* ----------------------------------- */
  /* check the size of the used codebook */
  /* ----------------------------------- */

  if (refN!=cbsP->list.itemA[cbX].refN) { 
      WARN( "Distribution/codebook size mismatch: %s/%s %d/%d (not loaded!)\n",
             dsP->name,cbsP->list.itemA[cbX].name,refN,
             cbsP->list.itemA[cbX].refN);
      for (refX=0; refX<refN; refX++) read_float(fp);
  }
  else {
    int warn = 0;

    /* ----------------------------------------------- */
    /* now, everything is checked, so read the weights */
    /* ----------------------------------------------- */

    if (dsP->val==NULL) { 
       for (refX=0; refX<refN; refX++) (void)read_float(fp); 
       ERROR("Value array of %s is NULL",dsP->name);
       return TCL_ERROR;
    }
    for (refX=0; refX<refN; refX++) dsP->val[refX]=read_float(fp);
    for (refX = 0; refX < refN; refX++) {
      if (isnan (dsP->val[refX]) || isinf (dsP->val[refX])) {
	if (!warn) {
	  WARN ("dsLoad: '%s' count=%f looks weird\n", dsP->name, dsP->count);
	  warn = 1;
	}
	dsP->val[refX] = 0;
      }
    }
  }
  if (feof(fp)) ERROR("premature end of file\n");
  return TCL_OK;
}

static int dssLoad (DistribSet *dssP, char *filename)
{
  FILE* fp;
  int   l_dsX, l_dsN, ret = TCL_OK;

	if (! (fp=fileOpen(filename,"r"))) return -2;

  l_dsN = read_int(fp);	
  if (l_dsN == DS_MAGIC) {  /* ... read header ... */ ; /* ... verbose header, if desired ... */ }
  if (l_dsN != dssP->list.itemN)
    WARN("Found %d distributions, expected %d\n",l_dsN,dssP->list.itemN);

  for (l_dsX=0; l_dsX<l_dsN; l_dsX++) 
     if ((ret=dsLoad(dssP,fp)) != TCL_OK) break;

  fileClose( filename, fp);
  if (ret != TCL_OK) ERROR("couldn't load %d. distribution\n",l_dsX);
  return ret;
}

static int dssLoadItf (ClientData cd, int argc, char *argv[])
{
  DistribSet* dssP  = (DistribSet*)cd;
  int         ac    =  argc - 1;
  char*       fname =  NULL;
  //int         idx;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &fname, cd, "file to read from",
       NULL) != TCL_OK) return TCL_ERROR;

  if (dssLoad(dssP,fname)<0) return TCL_ERROR; else return TCL_OK;
}

/* ------------------------------------------------------------------------
    dssSave   save all distribution in machine independent format to a file
   ------------------------------------------------------------------------ */

static int dssSave (DistribSet *dssP, char *filename)
{
  int   ret = TCL_OK, dsX;
  FILE  *fp;
  
  /* ------------------------ */
  /* check sanity of the file */  
  /* ------------------------ */
  if (filename==NULL) { ERROR("null filename\n"); return TCL_ERROR; }

#ifdef WINDOWS
  if (! (fp=fileOpen(filename,"wb"))) return TCL_ERROR;
#else
	if (! (fp=fileOpen(filename,"w"))) return TCL_ERROR;
#endif


  /* ----------------------------- */
  /* write number of distributions */
  /* ----------------------------- */
  write_int(fp,dssP->list.itemN);

  /* ------------------------------------ */
  /* Now, write every single distribution */
  /* ------------------------------------ */
  for (dsX=0; dsX<dssP->list.itemN; dsX++) {
    if (( ret = dsSave( dssP->list.itemA+dsX, cbsName(dssP->cbsP, 
                        dssP->list.itemA[dsX].cbX), fp)) != TCL_OK) break;
  }

  fileClose( filename, fp);
  if (ret != TCL_OK) ERROR("Couldn't save distrib %d",dsX);
  return ret;
}

static int dssSaveItf (ClientData cd, int argc, char *argv[])
{
  DistribSet* dssP  = (DistribSet*)cd;
  int         ac    =  argc - 1;
  char*       fname =  NULL;
  //int         idx;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &fname, cd, "file to read from",
       NULL) != TCL_OK) return TCL_ERROR;

  if (dssSave(dssP,fname)<0) return TCL_ERROR; else return TCL_OK;
}


/* ======================================================================== */
/* Training stuff                                                           */
/* ======================================================================== */

/* ------------------------------------------------------------------------
    dsMultiVar   does some MultiVar annealing
                 on a Distribution (alas its Codebook)
   ------------------------------------------------------------------------ */

/* forward declaration */
static int minmaxdistance ( FMatrix *cb, double *min, double *max );

static int dsMultiVar( DistribSet *dss, int dsX, int verbosity, int mvmode, int samples, 
		       int firstsamp, int lastsamp ) {
  CodebookSet  *cbs = dss->cbsP;
  Distrib      *ds  = dss->list.itemA + dsX;
  Codebook     *cb  = cbs->list.itemA + ds->cbX;
  CodebookAccu *cba = cb->accu;
  FMatrix     *data = cbs->feat->featA[cb->featX].data.fmat;
  int                 max, frameX;
  int                 i, j;
  float      factor = 1.0;
  float    scoreSum = 0.0;

  if (verbosity) 
    printf("Multivar: Doing mode %d update on codebook '%s', sample size is %dx%d.\n", 
	 mvmode, cb->name, data->m, data->n);

  /* Clear Accus: */
  dsAccuClear (ds->accu,-1);
  cbAccuClear (cba,-1);

  switch (mvmode) {
  case 0:
    /* UNIVAR mode */
    for (j = 0; j < cb->refN; j++)
      fcvConst(cb->cv[j], pow(cb->cfg->rhoGlob, -2.0));
    if (verbosity) printf("  Univar mode with rhoGlob = %2.4e.\n", cb->cfg->rhoGlob);
    cbUpdateConst ( cb );
    break;
  case 1:
    /* Multivar Mode */
    if (verbosity) printf("  Multivar mode.\n");  
    break;
  case 2:
    /* Radialize the cov */
    cbStepDiag(cb, cb->refN, 0);
    if (verbosity) printf("  Radializing covariances before update.\n");    
    break;
  case 3:
    /* Step-Diagonalize the cov */
    cbStepDiag(cb, 4, 0);
    if (verbosity) printf("  Step-diagonalizing covariances before update.\n");    
    break;
  case 4:
    /* Step-diagonalize with sorted axes */
    cbStepDiag(cb, 4, 1);
    if (verbosity) printf("  Step-diagonalizing sorted covariances before update.\n");    
    break;
  case 5:
    /* Step-diagonalize with commonly sorted axes */
    cbStepDiag(cb, 4, 2);
    if (verbosity) printf("  Step-diagonalizing common sorted covariances before update.\n");    
    break;
  }

  /* Loop: no update of Cov-Mat */
  if (firstsamp < 0)                        firstsamp = 0;
  if (lastsamp == -1 || lastsamp > data->m) lastsamp  = data->m;
  if (samples == lastsamp-firstsamp)        samples   = -1;
  max = (samples == -1) ? lastsamp : firstsamp + samples;
  if (verbosity) 
    printf("  Entering loop with samples=%d, firstsamp=%d, lastsamp=%d, max=%d\n", 
	   samples, firstsamp, lastsamp, max);
  for ( j = firstsamp; j < max ; j++ ) {

    /* Accumulate frame 'j' or 'random' (dssAccu also calls cbsAccu) */
    frameX = (samples == -1) ? j : (int)(chaosRandom((double)firstsamp, (double)lastsamp));
    dssAccu ( dss, dsX, frameX, factor );
    /* So, in dssAddCountA haben wir jetzt die probabilities, diese koennen wir
       noch in Log-Likelihood umrechnen (analog zu sampleSetAccuCblh): */
    scoreSum += cba->score;

    if (verbosity > 2) {
      printf("    %d: %d -> %2.4e *", j-firstsamp, frameX, cba->score);
      if (verbosity > 3) { for (i = 0; i < cb->refN; i++) printf(" %2.4e", dssAddCountA[i]); }
      printf("\n");
    }
  }
  
  /* Do the updates, if wanted */
  if ( ds->cfg->doUpdate == 'y' ) {
    dsAccuMLE     ( ds->accu, -1 );
    if (verbosity) printf("  Updated distribution %s, %f counts.\n", ds->name, ds->count);
  }
  if ( cb->cfg->doUpdate == 'y' ) {
    cbMLE         (cb,-1);
    cbUpdateConst (cb);
    if (verbosity) printf("  Updated codebook %s, %f counts(0).\n", cb->name, cb->count[0]);
  }
    
  if (samples != 0) {
    /* Calculate control parameters: */
    cba->distortion = fmatrixDistortion(cb->rv, NULL, data);
    cba->score      = scoreSum/(max-firstsamp);
    minmaxdistance    (cb->rv, &cba->mindist, &cba->maxdist);
  }  

  /* Output: is this useful ? */
  if (verbosity > 1) {
    printf("  AvgScore   = %2.4e\n", cba->score);
    printf("  Distortion = %2.4e, MinDist = %2.4e, MaxDist = %2.4e\n", 
	   cba->distortion, cba->mindist, cba->maxdist);
  }
  if (verbosity) fflush (stdout);

  return TCL_OK;
}


static int dssMultiVarItf (ClientData cd, int argc, char *argv[]) {
  DistribSet   *dss = (DistribSet *)cd;
  CodebookSet  *cbs = dss->cbsP;
  Distrib             *ds;
  Codebook            *cb;
  int            ac = argc-1;
  int     verbosity = 0;
  int       samples = -1;
  int     firstsamp = 0;
  int      lastsamp = -1;
  int        mvmode = 1;
  int                 dsX;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<Distrib>",         ARGV_LINDEX, NULL, &dsX,      &(dss->list), "Distribution",
      "-samples",          ARGV_INT,    NULL, &samples,   NULL,        "Number of random samples drawn (-1 for linear mode)",
      "-first",            ARGV_INT,    NULL, &firstsamp, NULL,        "First sample",
      "-last",             ARGV_INT,    NULL, &lastsamp,  NULL,        "Last sample",
      "-mode",             ARGV_INT,    NULL, &mvmode,    NULL,        "Univar, Multivar, TiedRho, TiedSelf mode?",
      "-verbosity",        ARGV_INT,    NULL, &verbosity, NULL,        "Verbosity of output",
      NULL) != TCL_OK) return TCL_ERROR;

  ds  = dss->list.itemA + dsX;
  cb  = cbs->list.itemA + ds->cbX;

  if (!cb->accu || !ds->accu) {
    SERROR("Accus don't exist for codebookSet %s or distribSet %s!\n", cbs->name, dss->name);
    return TCL_ERROR;
  }

  return dsMultiVar( dss, dsX, verbosity, mvmode, samples, firstsamp, lastsamp );
}


/* ------------------------------------------------------------------------
   Multivar support functions
   ------------------------------------------------------------------------ */

static int dssMultiVarInitItf (ClientData cd, int argc, char *argv[]) {
  DistribSet  *dss = (DistribSet *)cd;
  CodebookSet *cbs = dss->cbsP;
  Distrib     *ds;
  Codebook    *cb;
  int           ac = argc-1;
  float       saat = 0.0, rhoRel = 0.0;
  int                dsX;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<Distrib>",         ARGV_LINDEX, NULL, &dsX,      &(dss->list), "Distribution",
      "-seed",             ARGV_FLOAT,  NULL, &saat,      NULL,        "seed for random number generator",
      "-rhoRel",           ARGV_FLOAT,  NULL, &rhoRel,    NULL,        "initialize univariate distribution",
      NULL) != TCL_OK) return TCL_ERROR;

  ds  = dss->list.itemA + dsX;
  cb  = cbs->list.itemA + ds->cbX;

  /* Initialize random number generator */
  if (saat != 0.0) chaosRandomInit(saat);

  /* Initialization with univariate distribution? */
  if (rhoRel != 0.0) {
    FMatrix *data = cbs->feat->featA[cb->featX].data.fmat;
    FVector *rmean, *smean, *dev;
    double maxRho = 0.0;
    int    i, j;

    if (data->n != cb->dimN) {
      SERROR("Dimensions of data (%d) and codebook (%d) don't match.\n", data->n, cb->dimN);
      return TCL_ERROR;
    }

    /* Calculate the mean and variance of the data */
    rmean  = fvectorCreate(data->n);
    smean  = fvectorCreate(data->n);
    dev    = fvectorCreate(data->n);
    fmatrixMeanvar(data, NULL, rmean, smean, dev);

    /* Set the mean, calculate the maximum variance */
    for (i = 0; i < data->n; i++) {
      if (dev->vecA[i] > maxRho)
	maxRho = dev->vecA[i];
      for (j = 0; j < cb->refN; j++)
	cb->rv->matPA[j][i] = rmean->vecA[i] * (1 + j * 1e-4);
    }
    cb->cfg->rhoGlob = maxRho*rhoRel;
    fvectorFree(rmean); fvectorFree(smean); fvectorFree(dev);

    /* Set the univariate Covariance-Matrices */
    for (j = 0; j < cb->refN; j++)
      fcvConst(cb->cv[j], pow(cb->cfg->rhoGlob,-2.0));
    cbUpdateConst ( cb );

    /* Distribution: */
    for (i = 0; i < cb->refN; i++) ds->val[i] = log((double)cb->refN);
  }

  return TCL_OK;
}


static int minmaxdistance ( FMatrix *cb, double *min, double *max ) {
  int xvalX, refX;

  *min = 1E10, *max = 0.0;
  for (xvalX=0; xvalX < cb->m; xvalX++){
    for (refX=xvalX+1; refX < cb->m; refX++) { 
      int dimX; double dist=0.0;
      for (dimX=0; dimX < cb->n; dimX++)
	dist += (cb->matPA[refX][dimX] - cb->matPA[xvalX][dimX]) *
	        (cb->matPA[refX][dimX] - cb->matPA[xvalX][dimX]);
      if (dist < *min) *min = dist;
      if (dist > *max) *max = dist;
    }
  }

  *min = sqrt ( *min );
  *max = sqrt ( *max );

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    dssAccu  
   ------------------------------------------------------------------------ */

int dssAccu( DistribSet* dss, int classX, int frameX, float factor)
{
  Distrib* ds  =  dss->list.itemA + classX;

  if (! ds->accu)
    (void)dss->cbsP->accu(dss->cbsP,ds->cbX,ds->val,frameX,factor,
                                    NULL,NULL);
  else  {
    dsAccuAllocate(ds->accu);
    
    (void)dss->cbsP->accu(dss->cbsP,ds->cbX,ds->val,frameX,factor,
                                    dssAddCountA,dssSubA);

    if (ds->accu && ds->cfg->doAccumulate=='y') {
      int subA = (dss->subIndex >=0) ? dss->subIndex   : 0;
      int subE = (dss->subIndex >=0) ? dss->subIndex+1 : ds->accu->subN ;
      int   valX, subX;
      for ( valX = 0; valX < ds->valN; valX++) {
        for ( subX = subA; subX < subE; subX++) {
	  switch (ds->cfg->method) { 
	  case 'm':  ds->accu->count[subX][valX] += factor * dssSubA[valX][subX] * dssAddCountA[valX]; break;
	  case 'd':  ds->accu->count[subX][valX] += factor * dssSubA[valX][subX] * dssAddCountA[valX]; break;
	  default :  ERROR("Undefined training method: %c\n",ds->cfg->method);
          }
        }
      }
    }
  }
  return TCL_OK;
}


int dssAccuItf( ClientData cd, int argc, char *argv[])
{
  DistribSet* dssP   = (DistribSet*)cd;
  int         frameX =  0, dsX, ac = argc-1;
  float       factor =  1.0;
  int         frameY =  -1;
  int              i;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<distrib>", ARGV_LINDEX, NULL, &dsX,&(dssP->list), "name of the distribution",
      "<frame>",   ARGV_INT,    NULL, &frameX, 0, "index of the requested frame",
      "-factor",   ARGV_FLOAT,  NULL, &factor, 0, "training factor",
      "-toframe",  ARGV_INT,    NULL, &frameY, 0, "first frame not to train",
      NULL) != TCL_OK) return TCL_ERROR;

  for (i = frameY-1; i > frameX; i--)
    dssAccu (dssP, dsX, i, factor);

  return dssAccu( dssP, dsX, frameX, factor);
}

/* ------------------------------------------------------------------------
    dssScoreMatrix 
   ------------------------------------------------------------------------ */
int dssScoreMatrix( DistribSet* dss, int classX, FMatrix* mat, int frameB, int frameE)
{
  Distrib* ds  =  dss->list.itemA + classX;
  int frameX = frameB;
  /* int frameN = 0; should be added */
  int valX, valN = ds->valN;

  assert(frameE >= frameB);
  assert(frameB >= 0);
  /* assert(frameE < frameN); */
  fmatrixResize(mat,frameE-frameB+1,valN); 
  for ( ; frameX < frameE; frameX++) {
    float sum = 0.0;
    dss->cbsP->score(dss->cbsP,ds->cbX,ds->val,frameX,mat->matPA[frameX]);
    for (valX = 0; valX < valN; valX++) sum += mat->matPA[frameX][valX];
    for (valX = 0; valX < valN; valX++) mat->matPA[frameX][valX] /= sum;
  }
  return TCL_OK;
}

int dssScoreMatrixItf( ClientData cd, int argc, char *argv[])
{
  DistribSet* dssP   = (DistribSet*)cd;
  int         frameB =  0, frameE = -1, dsX, ac = argc-1;
  FMatrix*    matrix = NULL;
  /* float       factor =  1.0; */

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<distrib>", ARGV_LINDEX, NULL, &dsX,&(dssP->list), "name of the distribution",
      "<matrix>",  ARGV_CUSTOM, getFMatrixP,&matrix, cd, "FMatrix",
      "-from",     ARGV_INT,    NULL, &frameB,  NULL, "start frame",
      "-to",       ARGV_INT,    NULL, &frameE,  NULL, "end frame (needed!)",
       NULL) != TCL_OK) return TCL_ERROR;

  return dssScoreMatrix( dssP, dsX, matrix, frameB, frameE);
}


/* ------------------------------------------------------------------------
    dssScoreFeature
   -----------------
    Scores a feature from this distribution into a row of a matrix
   ------------------------------------------------------------------------ */

int dssScoreFeature (DistribSet* dssP, int dsX, int dsY, FMatrix* mat, int row, 
		     int fB, int fE, float offset, float factor)
{
  int fX;

  if (dsY == -1) for (fX = fB; fX < fE; fX++)
      mat->matPA[fX][row] = offset+factor*dssScore (dssP, dsX, fX);
  else           for (fX = fB; fX < fE; fX++)
      mat->matPA[fX][row] = offset+factor*(dssScore (dssP, dsX, fX) - dssScore (dssP, dsY, fX));    

  return TCL_OK;
}

int dssScoreFeatureItf (ClientData cd, int argc, char *argv[])
{
  DistribSet* dssP   = (DistribSet*) cd;
  FMatrix*    matrix = NULL;
  int         dsX    = 0,   dsY    = -1, ac  = argc-1, fX = 0;
  int         frameB = 0,   frameE = -1, row = 0;
  float       offset = 0.0, factor = 1.0;
  FeatureSet  *fs    = dssP->cbsP->feat;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "<distrib>", ARGV_LINDEX, NULL,        &dsX,    &(dssP->list), "distribution name",
      "<matrix>",  ARGV_CUSTOM, getFMatrixP, &matrix, cd,            "FMatrix",
      "-row",      ARGV_INT,    NULL,        &row,    NULL,          "row",
      "-from",     ARGV_INT,    NULL,        &frameB, NULL,          "start frame",
      "-to",       ARGV_INT,    NULL,        &frameE, NULL,          "end frame",
      "-offset",   ARGV_FLOAT,  NULL,        &offset, NULL,          "offset",
      "-factor",   ARGV_FLOAT,  NULL,        &factor, NULL,          "factor for this contribution",
      "-anti",     ARGV_LINDEX, NULL,        &dsY,    &(dssP->list), "name of the anti-distribution",
		   NULL) != TCL_OK) return TCL_ERROR;

  fX = dssP->cbsP->list.itemA[dssP->list.itemA[dsX].cbX].featX;
  if (frameB < 0) frameB = 0;
  if (frameE < 0) frameE = fs->featA[fX].data.fmat->m;

  return dssScoreFeature (dssP, dsX, dsY, matrix, row, frameB, frameE, offset, factor);
}


/* ------------------------------------------------------------------------
    dssAccuPath    add all items in a a path to the accumulator weighted by
                   factor times gamma.
   ------------------------------------------------------------------------ */

static int dssAccuPathItf( ClientData cd, int argc, char *argv[]) 
{ 
  DistribSet* dssP   = (DistribSet*)cd; 
  Path*       path   =  NULL;
  float       factor =  1.0;
  int         ac     =  argc-1;
  int         frX,i;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<path>",  ARGV_OBJECT, NULL, &path, "Path", "name of the path object",
      "-factor", ARGV_FLOAT,  NULL, &factor, 0,    "training factor",
       NULL) != TCL_OK) return TCL_ERROR;


  for ( frX = 0; frX < path->lastFrame-path->firstFrame+1; frX++) {
    for ( i = 0; i < path->pitemListA[frX].itemN; i++)
      dssAccu( dssP, path->pitemListA[frX].itemA[i].senoneX,
                    frX+path->firstFrame,
                    factor * path->pitemListA[frX].itemA[i].gamma);
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dssUpdate
   ------------------------------------------------------------------------ */

int dssUpdate( DistribSet* dss)
{ 
  int         dsX;

  (void)dss->cbsP->update(dss->cbsP);

  for (dsX=0; dsX<dss->list.itemN; dsX++) {
    Distrib* ds = &(dss->list.itemA[dsX]);
    if (ds->accu && ds->cfg->doUpdate == 'y') {  
      switch (ds->cfg->method) {  
      case 'm': dsAccuMLE(ds->accu, dss->subIndex); break;
      case 'd': dsAccuMMIE(ds->accu, dss->subIndex); break;
      default : ERROR("Undefined training method: %c\n",ds->cfg->method);
      }
    }
  }
  return TCL_OK;
}

int dssUpdateItf( ClientData cd, int argc, char *argv[])
{
  DistribSet* dssP   = (DistribSet*)cd;
  int         ac     = argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_ERROR;

  return dssUpdate( dssP);
}

/* ------------------------------------------------------------------------
    dssDist    computes the distance between two sets of distributions
               which are supposed to be defined over the same codebook.
   ------------------------------------------------------------------------ */

static float** dssDistA = NULL;
static int     dssDistN = 0;

static int dssDistAlloc( int N)
{
  int j;

  if (! dssDistA || dssDistN < N) {
        dssDistN    =  N;
        dssDistA    = (float**)realloc( dssDistA, 4 * sizeof(float*) +
                                        4 *dssDistN * sizeof(float));
        dssDistA[0] = (float *)(dssDistA + 4);
        dssDistA[1] =  dssDistA[0] + dssDistN;
        dssDistA[2] =  dssDistA[1] + dssDistN;
        dssDistA[3] =  dssDistA[2] + dssDistN;
  }
  for ( j = 0; j < 4*dssDistN; j++) dssDistA[0][j] = 0.0;
  return TCL_OK;
}

static int dssDistEntropy( DistribSet* dssP, ModelArray* maP, 
                                             ModelArray* maQ,
                                             ModelArray* maR, float* distP)
{
  double count[4] = { 0.0, 0.0, 0.0, 0.0};
  double h[4]     = { 0.0, 0.0, 0.0, 0.0};
  int    cbX      = -1;
  int    i, j, m, N=0;

  /* because of the calling profile of this function, the model array
     maP represents the 'no'-models and maQ represents the 'yes'-models.
     Normally in the 'no'-array are more models than in the 'yes'-array.
     This means, that if you want to make the calculation faster, you
     must first check whether the counts of the no part are greater
     than minCount (ma[1]=maQ and ma[2]=maP) and than check the others.
     maR is normally empty, so do it at last. */
  ModelArray *ma[4]; ma[1] = maQ; ma[2] = maP; ma[3] = maR;

  if ( maP->mdsP.cd != (ClientData)dssP || 
       maQ->mdsP.cd != (ClientData)dssP || 
       maR->mdsP.cd != (ClientData)dssP) {
    ERROR("ModelArrays are not defined over DistribSet %s.\n", dssP->name);
    return TCL_ERROR;
  }

  /* check, whether all distributions are defined over the same codebook,
     allocate sufficient space and compute the distributions */
  
  for (m = 1; m < 4; m++) {
#ifdef QUESTION_VERBOSE
      INFO ("%i modelN %i\n", m, ma[m]->modelN);
#endif
      for (i = 0; i < ma[m]->modelN; i++) {
	  Distrib* dsP = dssP->list.itemA + ma[m]->modelA[i];
#ifdef QUESTION_VERBOSE
	  INFO("  %s  %f\n", dsP->name, dsP->count);
#endif
	  if      ( cbX <  0) { cbX = dsP->cbX; N = dsP->valN; dssDistAlloc( N); }
	  else if ( cbX != dsP->cbX) {
	      ERROR("Distributions are defined over different codebooks %d %d.\n",
		    cbX, dsP->cbX);
	      return TCL_ERROR;
	  }

	  for ( j = 0; j < N; j++) {
	      dssDistA[m][j] += dsP->count * exp(-dsP->val[j]);
	  }

	  count[m] += dsP->count;
      }

      if ( count[m] > 0.0 && count[m] < dssP->minCount ) {
#ifdef QUESTION_VERBOSE
	    WARN("ENTROPY count %e of set %i smaller than mincount %e\n",
		 count[m], m, dssP->minCount); fflush(stdout);
	    for (i = 0; i < ma[m]->modelN; i++) {
	      Distrib* dsP = dssP->list.itemA + ma[m]->modelA[i];
	      INFO(  "distrib=%s  count=%f\n",dsP->name,dsP->count);
	    }
#endif
	    return TCL_ERROR;
      }

      count[0] += count[m];
  }

  if ( count[0] > 0.0 && count[0] < dssP->minCount ) {
#ifdef QUESTION_VERBOSE
       WARN ("ENTROPY count %e of set %i smaller than mincount %e\n",
	     count[0], 0, dssP->minCount); fflush(stdout);
#endif
      return TCL_ERROR;
  }

  if ( count[1]+count[2] <= 0.0 || count[1]+count[3] <= 0.0 ||
       count[2]+count[3] <= 0.0 || count[0]          <= 0.0 ) {
#ifdef QUESTION_VERBOSE
	WARN ("ENTROPY sets are unseparable [%e, %e, %e, %e]\n",
	      count[0], count[1], count[2], count[3]); fflush(stdout);
#endif
      return TCL_ERROR;
  }

  for (m = 0; m < 4; m++)
      for (j = 0; j < N; j++) dssDistA[0][j] += dssDistA[m][j];

  for (m = 0; m < 4; m++) {
      if ( count[m] > 0.0 ) {
	  for (j = 0; j < N; j++) {
	      double x        = (dssDistA[m][j] /= count[m]);
	      h[m]           -= (x < 1e-19) ? 0.0 : (x * log(x));
#ifdef QUESTION_VERBOSE
	      INFO("x=%f %f %f, hm=%f\n", dssDistA[m][j], count[m], x, h[m]);
#endif
	  }
      }
  }
#ifdef QUESTION_VERBOSE
    WARN ("ENTROPY count [%e, %e, %e, %e] of set GREATER than mincount %e\n",
	  count[0], count[1], count[2], count[3], dssP->minCount);
#endif

  if ( distP ) {
      *distP = count[0] * h[0] - count[1] * h[1] - count[2] * h[2] - count[3] * h[3];

      if ( dssP->normDistance ) *distP /= count[0];
  }
#ifdef QUESTION_VERBOSE
  INFO("%f %f %f %f %f\n",count[0],count[1],count[2],count[3],*distP);
#endif
  return TCL_OK;
}


static int dssDistLikelihood (DistribSet* dssP, ModelArray* maP, 
		       			 ModelArray* maQ, 
					 ModelArray* maR, float* distP)
{
  int    subX, subN = -1;
  int    i, j, m, xvalX, N=0;
  int    mN = 4;

  double lh = 0.0;

  ModelArray *ma[4]; ma[1] = maP; ma[2] = maQ; ma[3] = maR;

  /* ----------------------------------- */
  /* check compatibility of model arrays */
  /* ----------------------------------- */

  if ( maP->mdsP.cd != (ClientData)dssP || 
       maQ->mdsP.cd != (ClientData)dssP || 
       maR->mdsP.cd != (ClientData)dssP)
  {   ERROR("ModelArray(s) not defined over DistribSet %s.\n", dssP->name); 
      return TCL_ERROR; }

  /* ------------------------------------ */
  /* check compatibility of distributions */
  /* ------------------------------------ */

  for (m=1; m<mN; m++)
  {   for (i=0; i<ma[m]->modelN; i++) 
      {   Distrib  *dsP = dssP->list.itemA + ma[m]->modelA[i];
          N = dsP->valN;  dssDistAlloc(N);
          if (!dsP->accu) { ERROR("Distrib has no accu.\n"); return TCL_ERROR;}
          if (subN < 0) { subN = dsP->accu->subN; }
          else if (subN != dsP->accu->subN) 
               { ERROR("Distribs have different subN.\n"); return TCL_ERROR; }
      }
  }
  if (N==0) { *distP = 0.0; return TCL_OK; }

  /* ------------------------------------------------------------------- */
  /* for each of the cross-validation sets do the likelihood computation */
  /* ------------------------------------------------------------------- */

  for (xvalX=0; xvalX<subN; xvalX++)
  {
      double locLh = 0.0, count[4] = { 0.0, 0.0, 0.0, 0.0};

      for (j=0; j<N; j++) 
	dssDistA[0][j] = 
        dssDistA[1][j] = 
        dssDistA[2][j] = 
        dssDistA[3][j] = 0.0;

      /* ------------------------------------------------------ */
      /* accumulate all the subaccumulators                     */
      /* that do not belong to the current cross-validation set */
      /* ------------------------------------------------------ */

      for (subX=0; subX<subN; subX++)
          if (subX != xvalX) 
             for (m=1; m<mN; m++) 
                 for (i=0; i<ma[m]->modelN; i++) 
                 {   Distrib  *dsP = dssP->list.itemA + ma[m]->modelA[i];
                     for (j=0; j<N; j++) 
                     {   
                         dssDistA[m][j] += dsP->accu->count[subX][j]; 
                         count[m]       += dsP->accu->count[subX][j]; 
			 dssDistA[0][j] += dsP->accu->count[subX][j]; 
                         count[0]       += dsP->accu->count[subX][j]; 
                     }
                 }

      for (m=0; m<mN; m++) 
          for (j=0; j<N; j++) 
              if (count[m]) dssDistA[m][j] /= count[m];

      if (count[1] > 0 && count[1] < dssP->minCount) {
	/* WARN("dssDistLikelihood: COUNT 1 %e below mincount\n", count[1]); */
	  return TCL_ERROR;
      }
      if (count[2] > 0 && count[2] < dssP->minCount) {
	/*  WARN("dssDistLikelihood: COUNT 2 %e below mincount\n", count[2]); */
	  return TCL_ERROR;
      }
      if (count[3] > 0 && count[3] < dssP->minCount) {
	/*  WARN("dssDistLikelihood: COUNT 3 %e below mincount\n", count[3]); */
	  return TCL_ERROR;
      }

      /* -------------------------------------------- */
      /* multiply the likelihood of the current       */
      /* cross-validation set to the total likelihood */
      /* -------------------------------------------- */

      for (m=1; m<mN; m++) 
          for (i=0; i<ma[m]->modelN; i++) 
          {   Lh **buf = (dssP->list.itemA + ma[m]->modelA[i])->accu->lh;
	      if (buf) 
	         locLh += lhLikelihood(dssDistA[m],buf[xvalX])
                        - lhLikelihood(dssDistA[0],buf[xvalX]);
          }

      /* ------------------------------------------------------------------- */
      /* NOTE: adding the log-likelohoods of the successor-nodes and sub-    */
      /*       tracting the log-likelihood of their sum-node means dividing  */
      /*       the resulting split likelihood by the unsplit likelihood,     */
      /*       which is what we want.                                        */
      /* ------------------------------------------------------------------- */

      lh += locLh;
  }

  *distP = -1.0 * lh;

  /* ----------------------------------------------------------------------- */
  /* NOTE: The above statements for combining the likelihoods of different   */
  /*       xval sets could also be additive instead of multiplicative, such  */
  /*       that in the loop we would have lh += exp(-1.0*locLh); instead of  */
  /*       lh += locLh; and outside the loop we would have *distP = log(lh); */
  /*       instead of *distP = -1.0 * lh;                                    */
  /* ----------------------------------------------------------------------- */

  if (*distP <= 0.0) {
      fprintf (stderr, "DIST too small\n");
      return TCL_ERROR;
  }
  else               return TCL_OK;
}

static int dssDist( DistribSet* dssP, ModelArray* maP, 
		                      ModelArray* maQ,
                                      ModelArray* maR, float* distP)
{
  if ( maP->mdsP.cd != (ClientData)dssP || 
       maQ->mdsP.cd != (ClientData)dssP ||
       maQ->mdsP.cd != (ClientData)dssP) {

    ERROR("ModelArrays are not defined over DistribSet %s.\n", dssP->name);
    return TCL_ERROR;
  }

  if (dssP->distance=='e') return dssDistEntropy    (dssP, maP, maQ, maR, distP);
  if (dssP->distance=='l') return dssDistLikelihood (dssP, maP, maQ, maR, distP);

  return 0;
}

static int dssDistItf( ClientData cd, int argc, char *argv[])
{
  DistribSet* dssP   = (DistribSet*)cd;
  ModelArray* maP    = NULL;
  ModelArray* maQ    = NULL;
  ModelArray* maR    = NULL;
  int         ac     = argc-1;
  float       d      = 0;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<ModelArray P>",  ARGV_OBJECT, NULL, &maP, "ModelArray","model array",
      "<ModelArray Q>",  ARGV_OBJECT, NULL, &maQ, "ModelArray","model array",
      "<ModelArray R>",  ARGV_OBJECT, NULL, &maR, "ModelArray","model array",
        NULL) != TCL_OK) return TCL_ERROR;

  if ( dssDist( dssP, maP, maQ, maR, &d) == TCL_OK) {
    itfAppendResult( "%e", d);
    return TCL_OK;
  }
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    dssMerge, dssSplit
   ------------------------------------------------------------------------ */

int dssMerge( DistribSet* dss)
{ 
  int cbX;
  for (cbX=0; cbX<dss->cbsP->list.itemN; cbX++) {
    Codebook* cbP = dss->cbsP->list.itemA + cbX;
    if ( dsMerge (cbP,dss) == TCL_ERROR) {
      return TCL_ERROR;
    }
  } 

  return TCL_OK;
}


int dssSplit( DistribSet* dss)
{ 
  int cbX;

  for (cbX=0; cbX<dss->cbsP->list.itemN; cbX++) {
    Codebook* cbP = dss->cbsP->list.itemA + cbX;
    if ( dsSplit (cbP,dss) == TCL_ERROR) {
      return TCL_ERROR;
    }
  } 
  if (dss->cbsP->cache) cbcDealloc(dss->cbsP->cache);
  cbcAlloc(dss->cbsP->cache);
  return TCL_OK;
}

int dssMergeItf( ClientData cd, int argc, char *argv[])
{
  DistribSet* dssP   = (DistribSet*)cd;
  int         ac     = argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_ERROR;

  return dssMerge( dssP);
}

int dssSplitItf( ClientData cd, int argc, char *argv[])
{
  DistribSet* dssP   = (DistribSet*)cd;
  int         ac     = argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_ERROR;

  return dssSplit( dssP);
}

/* ========================================================================
    Accumulator stuff
   ======================================================================== */
/* ------------------------------------------------------------------------
    dssAccusClear
   ------------------------------------------------------------------------ */

static int dssAccusClear (DistribSet *dss, int subX)
{
  int i,ret;

  for(i=0; i< (dss->list).itemN;i++) {
    ret = dsAccuClear ((DistribAccu *)((dss->list).itemA[i]).accu,subX);
    if(ret != TCL_OK) return TCL_ERROR;
  }
  return TCL_OK;
}

static int dssAccusClearItf( ClientData cd, int argc, char *argv[])
{
  int           ac = argc-1;
  DistribSet* dssP = (DistribSet*)cd;
  int         subX = dssP->subIndex;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "-subX", ARGV_INT,  NULL, &subX,  NULL, "sub-accumulator, -1 to clear all",
        NULL) != TCL_OK) return TCL_ERROR;

  return dssAccusClear(dssP,subX);
}

/* ------------------------------------------------------------------------
    dssAccusCreate
   ------------------------------------------------------------------------ */

static int dssAccusCreate (DistribSet *dss, int subN)
{
  int i;

  for(i=0; i< (dss->list).itemN;i++) {
    if ( dsCreateAccu( (dss->list).itemA + i, subN) != TCL_OK)
         return TCL_ERROR;
  }
  return TCL_OK;
}

static int dssAccusCreateItf( ClientData cd, int argc, char *argv[])
{
  int   ac    = argc-1;
  int   subN  = 1;

  if ( itfParseArgv(argv[0], &ac, argv+1, 1,
      "-subN",     ARGV_INT,   NULL, &subN,     NULL, "number of subaccumulators",
       NULL) != TCL_OK) return TCL_ERROR;

  return dssAccusCreate((DistribSet *)cd,subN);
}

/* ------------------------------------------------------------------------
    dssAccusFree
   ------------------------------------------------------------------------ */

static int dssAccusFree (DistribSet *dssP)
{
  int i;

  for(i=0; i< dssP->list.itemN;i++) {
    Distrib* dsP = dssP->list.itemA + i;
    if (dsP->accu &&  dsAccuFree(dsP->accu) != TCL_OK)
      return TCL_ERROR;
    dsP->accu = NULL;
  }
  return TCL_OK;
}

static int dssAccusFreeItf( ClientData cd, int argc, char *argv[])
{
  int ac = argc-1;
  if (itfParseArgv (argv[0], &ac, argv+1, 1,
       NULL) != TCL_OK) return TCL_ERROR;
  return dssAccusFree((DistribSet *)cd);
}

/* ------------------------------------------------------------------------
    dssCreateLh
   ------------------------------------------------------------------------ */

static int dssCreateLh (DistribSet *dss, Lh *lh)
{
  int **sizes, *submod, dsX, subX, frameX, valX, gaussX;
  Distrib *ds;

  /* --------------------------------------------- */
  /* first we should read the index mapping file,  */
  /* but this is not yet implemented. Maybe later. */
  /* --------------------------------------------- */

  /* ---------------------------------------- */
  /* now find out the size for each lh object */
  /* ---------------------------------------- */

  sizes  = malloc(sizeof(int*)*dss->list.itemN);
  submod = malloc(sizeof(int*)*dss->list.itemN);
  for (dsX=0; dsX<dss->list.itemN; dsX++) {
    ds = dss->list.itemA+dsX;
    submod[dsX] = 0;
    sizes[dsX]  = NULL;
    if (ds->accu) {
      dsAccuAllocate(ds->accu);
      sizes[dsX] = malloc(sizeof(int)*ds->accu->subN);
      for (subX=0; subX<ds->accu->subN; subX++) sizes[dsX][subX] = 0; 
    }
    else sizes[dsX] = NULL;
  }

  for (frameX=0; frameX<lh->frameN; frameX++) {   
    dsX = lh->modX[frameX];
    ds  = dss->list.itemA+dsX;
    if (ds->accu == NULL)  {  
      ERROR("%s (%d) does not have accu",ds->name,dsX); 
      return TCL_ERROR; 
    }    
    sizes[dsX][(submod[dsX]++)%ds->accu->subN]+=lh->valN[frameX];
  }

  /* ---------------------------------- */
  /* now allocate the needed lh-objects */
  /* ---------------------------------- */

  for (dsX=0; dsX<dss->list.itemN; dsX++) {
    ds = dss->list.itemA+dsX;

    if (sizes[dsX] && sizes[dsX][0]) {
      ds->accu->lh = malloc(sizeof(Lh*)*ds->accu->subN);
      for (subX=0; subX<ds->accu->subN; subX++) {
	ds->accu->lh[subX] = lhCreate();
	lhAlloc(ds->accu->lh[subX],sizes[dsX][subX]);
      }
    }
  }

  /* ---------------------------------------------------------------------- */
  /* fill the created lh-objects, and the counts fields of the accumulators */
  /* ---------------------------------------------------------------------- */
 
  for (dsX=0; dsX<dss->list.itemN; dsX++) { 
    submod[dsX] = 0;
    ds = dss->list.itemA+dsX;
    if (ds->accu) 
      for (subX=0; subX<ds->accu->subN; subX++) sizes[dsX][subX] = 0; 
  }
  gaussX = 0;

  for (frameX=0; frameX<lh->frameN; frameX++) {   
    Lh*       curLh = NULL; 
    double countSum = 0.0;
    
    dsX   = lh->modX[frameX];
    ds    = dss->list.itemA+dsX;
    subX  = (submod[dsX]++)%ds->accu->subN;    
    curLh = ds->accu->lh[subX];

    curLh->valN[curLh->frameN]  = lh->valN[frameX];
    curLh->modX[curLh->frameN]  = dsX;

    for (valX=0; valX<lh->valN[frameX]; valX++) 
      countSum += exp(-1.0*lh->valG[gaussX+valX]);

    for (valX=0; valX<lh->valN[frameX]; valX++) {
      curLh->valG[curLh->gaussX] = lh->valG[gaussX];
      curLh->valX[curLh->gaussX] = lh->valX[gaussX];
      ds->accu->count[subX][lh->valX[gaussX]] 
	+= exp(-1.0*lh->valG[gaussX]) / countSum;
      curLh->gaussX++;
      gaussX++;
    }
    curLh->frameN++;
  }
  
  return TCL_OK;
}

static int dssCreateLhItf (ClientData cd, int argc, char *argv[])
{
  int   ac    = argc-1;
  Lh   *lh    = NULL;

  if ( itfParseArgv(argv[0], &ac, argv+1, 1,
      "<lh>",   ARGV_OBJECT, NULL, &lh, "Lh", "source likelihood accumulator",
       NULL) != TCL_OK) return TCL_ERROR;

  return dssCreateLh ((DistribSet*)cd,lh);
}


/* ------------------------------------------------------------------------
    dssAccusLoad     read distribution accumulators from a machine 
                     independent I/O format written file
   ------------------------------------------------------------------------ */

#define MAXNAME  256
#define MAXHEAD 1000

static int dssAccusLoad (DistribSet* dss, char *filename, float addFactor)
{
  FILE *fp;  char str[MAXHEAD], name[MAXNAME], *p; 
  int inFile=0, defined=0, undefined=0, loaded=0, valNMismatch=0, subNMismatch=0, noAccu=0, notWanted=0;
  int dsX, subX, valX, didRead, subN, valN;
  Distrib *ds;

  if (filename==NULL) { ERROR("NULL filename\n");   return TCL_ERROR; }
  if (     dss==NULL) { ERROR("NULL DistribSet\n"); return TCL_ERROR; }

  if (! (fp=fileOpen(filename,"r"))) return TCL_ERROR;

  /* --------------- */
  /* read the header */
  /* --------------- */

  (void)read_string(fp,str);    /* if verbosity printO("%s\n",str); */

  p=strtok(str," \t");  if (p==NULL) goto errorInHeader;
  if (strcmp(p,"VERSION")!=0)        goto errorInHeader;
  p=strtok(NULL," \t"); if (p==NULL) goto errorInHeader;
  if (strcmp(p,"3.0"))               goto errorInVersion;
  p=strtok(NULL," \t"); if (p==NULL) goto errorInHeader;
  if (strcmp(p,"DISTRIBUTIONS")!=0)  goto errorInHeader;
  p=strtok(NULL," \t"); if (p==NULL) goto errorInHeader;
  if (strcmp(p,"ACCUMULATOR")!=0)    goto errorInHeader;

  while (!feof(fp)) {

    /* ------------- */
    /* read the data */
    /* ------------- */
    
    (void)read_string(fp,name); strcpy(name,rwsRewrite(dss->rwsP,name));
    didRead = 0;
    if (feof(fp)) break;

    valN = read_int(fp); 
    subN = read_int(fp);

    inFile++;
    
    /* --------------------------------------- */
    /* known distribution? then try to load it */
    /* --------------------------------------- */
    
    if ((dsX=dssIndex(dss,name))>=0) {
      defined++;
      ds = &(dss->list.itemA[dsX]);

      if (ds->accu == NULL)           { noAccu++;       goto dontRead; }
      if (valN     != ds->valN)       { valNMismatch++; goto dontRead; }
      if (subN     != ds->accu->subN) { subNMismatch++; goto dontRead; }

      dsAccuAllocate(ds->accu);
      for (subX=0; subX<ds->accu->subN; subX++)
	for (valX=0; valX<ds->valN; valX++)
	  ds->accu->count[subX][valX] += addFactor * read_float(fp);

      didRead=1; loaded++;
    }
    else undefined++;

    dontRead: if (didRead==0)
    {
      for ( subX = 0; subX < subN; subX++) 
        for ( valX = 0; valX < valN; valX++) (void)read_float(fp);
    }
  }
  if (inFile)       INFO("%6d accumulators were found in the file\n",inFile);
  if (notWanted)    INFO("%6d accumulators were not wanted\n",notWanted);
  if (loaded)       INFO("%6d accumulators were loaded\n",loaded);
  if (defined)      INFO("%6d distributions were defined\n",defined);
  if (undefined)    INFO("%6d distributions were undefined\n",undefined);
  if (noAccu)       INFO("%6d distributions had no accumulator\n",noAccu);
  if (valNMismatch) INFO("%6d valN mismatches occurred\n",valNMismatch);
  if (subNMismatch) INFO("%6d subN mismatches occurred\n",subNMismatch);
  
  fileClose( filename, fp);
  itfAppendResult("%d", loaded);
  return TCL_OK;
	
  errorInVersion: fclose(fp); ERROR("unsupported version %s\n",p); return TCL_ERROR;
  errorInHeader:  fclose(fp); ERROR("corrupt file header\n");      return TCL_ERROR;
}
    
static int dssAccusLoadItf (ClientData cd, int argc, char *argv[]) 
{ 
  char  *filename = NULL;
  float addFactor = 1.0;
  int          ac = argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "file to read from",
      "-factor",    ARGV_FLOAT,  NULL, &addFactor,NULL, "multiplicator before adding",
       NULL) != TCL_OK) return TCL_ERROR;

  return dssAccusLoad((DistribSet*)cd,filename,addFactor); 
}

/* ------------------------------------------------------------------------
    dssAccusSave   save all accumulators to a file in machine independent
                   I/O format
   ------------------------------------------------------------------------ */

#define MAXCAT   250

static int dssAccusSave( DistribSet* dss, char *filename)
{
  char head[MAXHEAD], cat[MAXCAT];
  int dsX, subX, valX, counter=0;
  FILE *fp;

  if (filename==NULL) { ERROR("NULL filename\n");   return TCL_ERROR; }
  if (     dss==NULL) { ERROR("NULL DistribSet\n"); return TCL_ERROR; }
  if (! (fp=fileOpen(filename,"w")))  return TCL_ERROR;

  /* ------------------------------------- */
  /* write header (samye style as Janus 2) */
  /* ------------------------------------- */

  sprintf(head,"VERSION 3.0 DISTRIBUTIONS ACCUMULATOR FILE\n");
  sprintf(cat,"CREATED AT TIME: %d\n",(int)time(0));	        strcat(head,cat);
  sprintf(cat,"CREATED ON HOST: ");			        strcat(head,cat);
  if (gethostname(cat,60)<0) strcpy(cat,"unknown");	        strcat(head,cat);
  sprintf(cat,"\nCREATED BY USER: %s ","?");		        strcat(head,cat);
  sprintf(cat,"(id=%d:%d)\n\n",(int)getuid(),(int)getgid());	strcat(head,cat);
  write_string(fp,head);

  /* ----------------------------------------------- */
  /* now write every single distribution accumulator */
  /* ----------------------------------------------- */

  for (dsX=0; dsX<dss->list.itemN; dsX++) { 
    
    Distrib *ds = &(dss->list.itemA[dsX]); 
    
    if (ds==NULL || ds->accu==NULL || ds->accu->count == NULL) continue;

    write_string(fp,ds->name);		/* name of this distribution */
    write_int(fp,ds->valN);	        /* size of the distribution  */
    write_int(fp,ds->accu->subN);	/* number of subaccumulators */

    for (subX=0; subX<ds->accu->subN; subX++)
      for (valX=0; valX<ds->valN; valX++)
	write_float(fp,(float)ds->accu->count[subX][valX]);

    counter++; 
  }
  fileClose( filename,fp);

  INFO("Saved %d distribution accumulators\n",counter);
  return TCL_OK;
}

static int dssAccusSaveItf (ClientData cd, int argc, char *argv[]) 
{ 
  char *filename = NULL;
  int         ac = argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, cd, "file to write",
       NULL) != TCL_OK) return TCL_ERROR;

  return dssAccusSave((DistribSet*)cd,filename); 
}

/* ------------------------------------------------------------------------
    dssMap   maps all distributions whose Codebooks have a map defined
   ------------------------------------------------------------------------ */

static int dssMap( DistribSet* dss)
{
  int i, ret = TCL_OK;

  for( i = 0; i < (dss->list).itemN; i++) {
    Distrib* ds = dss->list.itemA+i;
    Distrib* dsP;
    if ((dsP = dsMap( ds, dss->cbsP->list.itemA[ds->cbX].map))) {
      dsCopy( ds, dsP);
      dsFree( dsP);
    }
  }
  return ret;
}

static int dssMapItf( ClientData cd, int argc, char *argv[])
{
  int   ac       = argc-1;
  if ( itfParseArgv(argv[0],&ac,argv+1,1,NULL) != TCL_OK) return TCL_ERROR;
  return dssMap((DistribSet *)cd);
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method dscfgMethod[] =
{
  { "puts", dscfgPutsItf, NULL },
  {  NULL,  NULL,         NULL }
} ;

TypeInfo dscfgInfo = { 
        "Dscfg", 0, -1, dscfgMethod,
         dscfgCreateItf, dscfgFreeItf,
	 dscfgConfigureItf, NULL, NULL,
        "configuration of a distribution" } ;

static Method accuMethod[] =
{
  { "puts",  dsAccuPutsItf,    NULL },
  { ":=",    dsAccuCopyItf,    "copies one accumulator into another" },
  { "+=",    dsAccuPlusItf,    "adds one accumulator to another" },
  { "*=",    dsAccuTimesItf,   "multiplies an accumulator with a factor" },
  { ">=",    dsAccuIncrItf,    "increase an accumulator's counts by a number" },
  { "clear", dsAccuClearItf,   "reset a single distribution's accumulator to zero" },
  { NULL,    NULL,             NULL }
} ;

TypeInfo distribAccuInfo = { "DistribAccu", 0, -1, accuMethod, 
                              NULL, NULL,
                              dsAccuConfigureItf, dsAccuAccessItf, 
                              itfTypeCntlDefaultNoLink,
	                     "a single distribution's accumulator\n" } ;

static Method distribMethod[] = 
{ 
  { "puts",    dsPutsItf,    NULL },
  { ":=",      dsCopyItf,   "copies distribution weights" },

  { "createAccu", dsCreateAccuItf, "create a single distribution's accumulator" },
  { "freeAccu",   dsFreeAccuItf,   "remove a single distribution's accumulator" },
  {  NULL,  NULL, NULL } 
} ;

TypeInfo distribInfo = { 
        "Distrib", 0, -1, distribMethod, 
         NULL,           NULL,
         dsConfigureItf, dsAccessItf, NULL,
	"A single distribution\n" } ;

static Method distribSetMethod[] = 
{ 
  { "puts",   dssPutsItf,   NULL },
  { "index",  dssIndexItf,  "returns indices of named distributions" },
  { "name",   dssNameItf,   "returns names of indexed distributions" },
  { "add",    dssAddItf,    "add a new distribution to the set" },
  { "delete", dssDeleteItf, "remove distribution from the set"  },
  { "score",  dssScoreItf,  "computes the score of a mixture distribution" },
  { "kldist", dssKullbackLeiblerItf,  "computes the symmetrized Kullback-Leibler distance of two distribs" },
  { "scoreNBest",   dssScoreNBestItf,   "computes the n-best mixtures mixtures" },
  { "accuPath",     dssAccuPathItf,     "accumulates sufficient statistic from path" },
  { "accuFrame",    dssAccuItf,         "accumulates sufficient statistic from frame" },
  { "scoreMatrix",  dssScoreMatrixItf,  "store contribution of distrib in a matrix" },
  { "scoreFeature", dssScoreFeatureItf, "store contribution of a feature in a matrix" },
  { "update", dssUpdateItf, "update distributions and codebooks" },
  { "merge",  dssMergeItf,  "merge distributions and codebooks" },
  { "split",  dssSplitItf,  "split distributions and codebooks" },
  { "read",   dssReadItf,   "reads a distribution description file" },
  { "write",  dssWriteItf,  "writes a distribution description file" },
  { "load",   dssLoadItf,   "loads distribution weights from a file" },
  { "save",   dssSaveItf,   "saves distribution weights into a file" },

  { "createAccus", dssAccusCreateItf, "creates accumulators for all distributions" },
  { "freeAccus",   dssAccusFreeItf,   "frees  accumulators for all distributions" },
  { "clearAccus",  dssAccusClearItf,  "clears accumulators for all distributions" },

  { "loadAccus",   dssAccusLoadItf,   "loads distribution accumulators from a file" },
  { "saveAccus",   dssAccusSaveItf,   "saves distribution accumulators into a file" },

  { "map",         dssMapItf,   "map all distributions" },
  { "createLh",    dssCreateLhItf,    "fill the lh fields of the accumulators" },
  { "dist",        dssDistItf,  "measure distance between distributions" },

  { "multiVar",    dssMultiVarItf, "perform Multivar algorithm on codebook/ distribution given samples" },
  { "multiVarInit", dssMultiVarInitItf, "initialize Multivar algorithm" },

  {  NULL,    NULL,          NULL } 
} ;

TypeInfo distribSetInfo = { 
        "DistribSet", 0,-1, distribSetMethod, 
         dssCreateItf,     dssFreeItf,
         dssConfigureItf,  dssAccessItf, NULL,
	"Set of distributions\n" } ;

static ModelSet distribSetModel = 
             { "DistribSet", &distribSetInfo, &distribInfo,
               (ModelSetIndexFct*)dssIndex,
               (ModelSetNameFct *)dssName,
               (ModelSetDistFct *)dssDist };

static int distribInitialized = 0;

int Distrib_Init (void)
{
  if ( distribInitialized) return TCL_OK;

  if ( ModelSet_Init()   != TCL_OK) return TCL_ERROR;
  if ( Rewrite_Init()    != TCL_OK) return TCL_ERROR;
  if ( Codebook_Init()   != TCL_OK) return TCL_ERROR;
  if ( Modalities_Init() != TCL_OK) return TCL_ERROR;

  dssDefaults.useN             = 0;
  dssDefaults.cbsP             = NULL;
  dssDefaults.rwsP             = NULL;
  dssDefaults.list.blkSize     = 5000;
  dssDefaults.commentChar      = ';';
  dssDefaults.dummyStart       = -1;
  dssDefaults.dummyName        = strdup("dummyDs");
  dssDefaults.stP              = NULL;
  dssDefaults.normDistance     = 0;

  itfNewType ( &distribAccuInfo );
  itfNewType ( &distribInfo );
  itfNewType ( &distribSetInfo );
  itfNewType ( &dscfgInfo );

  modelSetNewType( &distribSetModel);

  distribInitialized = 1;
  return TCL_OK;
}
