/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: extended codebooks for semi tied covariances
               ------------------------------------------------------------

    Author  :  Matthias Seeger
    Module  :  cbnew.c
    Date    :  $Id: cbnew.c 2453 2004-05-27 11:22:42Z metze $
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
    Revision 1.9  2004/05/27 11:22:42  metze
    Removed typo

    Revision 1.8  2003/08/14 11:20:25  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.7.20.4  2003/04/02 16:48:22  soltau
    Added reset function for active status

    Revision 1.7.20.3  2002/08/27 08:38:30  metze
    Now initializing string pointers to NULL in Itf

    Revision 1.7.20.2  2002/03/22 10:48:46  soltau
    cbnewsetConvertItf: enabled check for empty models

    Revision 1.7.20.1  2001/06/26 19:29:30  soltau
    beautified/fixed a couple of ugly hacker routines

    Revision 1.7  2000/11/14 12:01:10  janus
    Merged branch jtk-00-10-27-jmcd.

    Revision 1.6.6.1  2000/11/06 10:50:24  janus
    Made changes to enable compilation under -Wall.

    Revision 1.6  2000/10/24 14:01:19  janus
    Merged changes on branch jtk-00-10-12-tschaaf

    Revision 1.5.24.1  2000/10/24 13:24:12  janus
    moved NDEBUG definition to optimizing definition.
    if NDEBUG is defined assert statements are removed during compilation

    Revision 1.5  2000/04/12 09:59:14  soltau
    Fixed bug in transform_sample

    Revision 1.4  2000/04/12 09:05:02  soltau
    Changed   some float into doubles,
    Corrected convert function
    Added compare function

    Revision 1.3  1999/10/18 12:28:52  soltau
    Fixed bugs in cbnewUpdate

    Revision 1.2  1999/10/15 16:15:04  soltau
    Fixed some bugs in cbsetAccu

    Revision 1.1  1999/08/11 15:50:32  soltau
    Initial revision


   ======================================================================== */


/* Janus standards */
#include "common.h"
#include "error.h"
#include "mach_ind_io.h"

#include "cbnew.h"
#include "cbn_assert.h"
#include "cbn_common.h"

#include "codebook.h"
#include "distrib.h"
#include "modelSet.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define PI        3.14159265358979323846264338327950288419716939937510582097494459230
#define COV_FLOOR 1.0e-19



extern TypeInfo cbnewInfo; 


/* Available protocol criterions */
static struct {
  char* name;
  char* desc;
  crit_func_ptr code;
} name2code[] = {{"logLH","logLH: log likelihood of data matrix\n",
		  &cbnewCritLogLH}};


/* -------------------------------------------------------------------
   cbnewInitEmpty
   ------------------------------------------------------------------- */

int cbnewInitEmpty (CBNew* cd,ClientData name)
{
  assert (cd);
  assert (name);

  if (!(cd->name = strdup ((char*) name))) {
    ERROR("Out of memory!\n");
    return TCL_ERROR;
  }
  cd->useN            = 0;
  cd->num_dens        = 0;
  cd->accu_flag       = ACCUS_EMPTY;
  cd->accu_num        = 0;
  cd->cbnew_set       = NULL;
  cd->feat            = -1;
  cd->active          = NULL;
  cd->distrib         = NULL;
  cd->mean = cd->diag = NULL;
  cd->parmat          = NULL;
  cd->accu_gamma_sum  = NULL;
  cd->accu_mean       = NULL;
  cd->accu_cov        = NULL;
  cd->slot            = NULL;
  cd->protlist_size   = 0;
  cd->internal_flag   = 0;
  cd->lh_factor       = NULL;
  cd->trans_mean      = NULL;
  cd->temp_gamma      = NULL;
  cd->lh_buff         = NULL;
  cd->temp_samp       = NULL;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewInit
   ------------------------------------------------------------------- */

int cbnewInit (CBNew* cd,char* name,int num_dens,CBNewSet* cbnew_set,int feat)
{
  int i;
  float temp;
  char* fesGetNameRet;

  assert (name);
  assert (num_dens > 0);
  assert (cbnew_set->phase == CONSTRUCTION_PHASE);
  fesGetNameRet = fesGetName (cbnew_set->feat_set,feat);
  assert (fesGetNameRet);
  assert ((cbnew_set->feat_set->featA[feat].type == FE_FMATRIX &&
	   cbnew_set->feat_set->featA[feat].data.fmat->n
	   == cbnew_set->num_dim) ||
	  (cbnew_set->feat_set->featA[feat].type == FE_UNDEF &&
	   cbnew_set->feat_set->featA[feat].dtype == FE_FMATRIX &&
	   cbnew_set->feat_set->featA[feat].dcoeffN == cbnew_set->num_dim));

  FREE(cd->name);

  /* don't allocate memory for accumulators here */
  if (cbnewInitEmpty (cd,(ClientData) name) == TCL_ERROR)
    return TCL_ERROR;

  /* Simple components */
  cd->num_dens = num_dens;
  cd->cbnew_set = cbnew_set;
  cd->feat = feat;

  /* Allocation of simple arrays / matrices */
  GETMEMX(cd->active,char,num_dens,cbnewDeinit (cd));
  for (i = 0; i < num_dens; i++) cd->active[i] = 1;
  GETMEMX(cd->distrib,float,num_dens,cbnewDeinit (cd));

  /* create means */
  if (!(cd->mean = (FMatrix*) fmatrixCreate (num_dens,cbnew_set->num_dim))) {
    ERROR("couldn't allocate mean %d x %d for %s\n",num_dens,cbnew_set->num_dim,name);
    cbnewDeinit (cd);
    return TCL_ERROR;
  }

  /* create diag covariances */
  if (!(cd->diag = (FMatrix*) fmatrixCreate (num_dens,cbnew_set->num_dim))) {
    ERROR("couldn't allocate diag %d x %d for %s\n",num_dens,cbnew_set->num_dim,name);
    cbnewDeinit (cd);
    return TCL_ERROR;
  }

  /* create links to paramter matrices */
  GETMEMX(cd->parmat,CBNew_pmlink,num_dens,cbnewDeinit (cd));
  for (i = 0; i < num_dens; i++) {
    cd->parmat[i].mat   = NULL;
    cd->parmat[i].index = -1;
  }

  GETMEMX(cd->lh_factor,double,num_dens,cbnewDeinit (cd));
  GETMEMX(cd->temp_gamma,double,num_dens,cbnewDeinit (cd));
  GETMEMX(cd->lh_buff,float,cbnew_set->num_dim,cbnewDeinit (cd));
  GETMEMX(cd->temp_samp,double,cbnew_set->num_dim,cbnewDeinit (cd));

  /* init means and diag covariances */
  fmatrixInitConst (cd->mean,0.0);
  fmatrixInitConst (cd->diag,1.0);
  temp = (float) log ((double) num_dens);
  for (i = 0; i < num_dens; i++) cd->distrib[i] = temp;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewCreate
   ------------------------------------------------------------------- */
CBNew* cbnewCreate (char* name,int num_dens,CBNewSet* cbnew_set,int feat)
{
  CBNew* cd;

  if (!(cd = (CBNew*) malloc (sizeof(CBNew)))) {
    ERROR("Out of memory!\n");
    return 0;
  }
  cd->name = 0;
  if (cbnewInit (cd,name,num_dens,cbnew_set,feat) == TCL_ERROR) return 0;

  return cd;
}

/* -------------------------------------------------------------------
   cbnewDeinit
   ------------------------------------------------------------------- */

int cbnewDeinit (CBNew* cd)
{
  int i;

  assert (cd);

  if (cd->useN > 0) {
    ERROR("There exist %d references to codebook '%s'!\n",cd->useN,cd->name);
    return TCL_ERROR;
  }

  if (cd->parmat) {
    for (i = 0; i < cd->num_dens; i++)
      if (cd->parmat[i].mat)
	cbnewparmatUnlink (cd->parmat[i].mat,cd->parmat[i].index);
    FREE(cd->parmat);
  }
  FREE(cd->name);
  FREE(cd->active);
  FREE(cd->distrib);
  FREEFMAT(cd->mean);
  FREEFMAT(cd->diag);
  free_accus (cd);  /* deallocate accus */
  FREE(cd->lh_factor);
  FREEFMAT(cd->trans_mean);
  FREE(cd->lh_buff);
  FREE(cd->temp_samp);
  FREE(cd->temp_gamma);
  if (cd->slot) {
    for (i = 0; i < cd->protlist_size; i++)
      if (cd->slot[i].crit && cd->slot[i].epoch)
	fmatrixxDestroy (cd->slot[i].epoch);
    FREE(cd->slot);
  }

  /* Decrease 'useN' of feature object */
  if (cd->cbnew_set) {
    if (fesGetName (cd->cbnew_set->feat_set,cd->feat))
      cd->cbnew_set->feat_set->featA[cd->feat].useN--;
    cd->cbnew_set = 0;
  }
  cd->feat = -1;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewDestroy
   ------------------------------------------------------------------- */

int cbnewDestroy (CBNew* cd)
{
  assert (cd);
  if (cbnewDeinit (cd) == TCL_ERROR) return TCL_ERROR;
  free (cd);
  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewPuts
   ------------------------------------------------------------------- */
int cbnewPuts (CBNew* cd)
{
  int l,flag = 0;

  itfAppendResult("{name %s} {useN %d} {refN %d} {accuFlag %d} {accuNum %d} {cbnewSet %s} {feature %s} ",
		  cd->name,cd->useN,cd->num_dens,cd->accu_flag,cd->accu_num,
                  cd->cbnew_set->name,
		  fesGetName (cd->cbnew_set->feat_set,cd->feat));
  for (l = 0; l < cd->protlist_size; l++)
    if (cd->slot[l].crit) {
      if (!flag) {
	flag = 1;
	itfAppendResult ("\nActive protocol slots are:\n");
      }
      itfAppendResult ("{%d %s} ",l,
		       cbnewCritfuncName (cd->slot[l].crit));
    }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewLink
   ------------------------------------------------------------------- */

int cbnewLink (CBNew* cd,CBNewParMatrix* pmat,int dens_index)
{
  int index;

  assert (dens_index >= 0 && dens_index < cd->num_dens);
  assert (pmat->parmat_set == cd->cbnew_set->parmat_set);
  assert (cd->active[dens_index]);

  if (cd->cbnew_set->phase != CONSTRUCTION_PHASE) {
    ERROR("%s not in construction phase\n",cd->cbnew_set->name);
    return TCL_ERROR;
  }

  if (cd->parmat[dens_index].mat) {
    if (cbnewparmatUnlink (cd->parmat[dens_index].mat,
			   cd->parmat[dens_index].index)
	== TCL_ERROR) {

      ERROR("cbnewparmatUnlink failed\n");
      return TCL_ERROR;
    }
  }
  if (cbnewparmatLink (pmat,cd,dens_index,&index) == TCL_ERROR) {
    ERROR("cbnewparmatLink failed\n");
    cd->parmat[dens_index].mat = 0;
    cd->parmat[dens_index].index = -1;

    return TCL_ERROR;
  }
  cd->parmat[dens_index].mat = pmat;
  cd->parmat[dens_index].index = index;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewUnlink
   ------------------------------------------------------------------- */

int cbnewUnlink (CBNew* cd,int dens_index)
{

  assert (dens_index >= 0 && dens_index < cd->num_dens);

  if (cd->cbnew_set->phase != CONSTRUCTION_PHASE) {
    ERROR("%s not in construction phase\n",cd->cbnew_set->name);
    return TCL_ERROR;
  }
 
  if (cd->parmat[dens_index].mat) {
    if (cbnewparmatUnlink (cd->parmat[dens_index].mat,
			   cd->parmat[dens_index].index) == TCL_ERROR) {
      ERROR("cbnewparmatUnlink failed\n");
      return TCL_ERROR;
    }
    cd->parmat[dens_index].mat = 0;
    cd->parmat[dens_index].index = -1;
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewClearAccus
   ------------------------------------------------------------------- */
int cbnewClearAccus (CBNew* cd)
{
  register int i,i2,j,d,bound;

  if (cd->cbnew_set->phase != WORKING_PHASE) {
    ERROR("%s not in working phase\n",cd->cbnew_set->name);
    return TCL_ERROR;
  }

  if (cd->accu_flag != ACCUS_EMPTY) {
    dmatrixInitConst (cd->accu_mean,0.00);
    for (i = 0; i < cd->num_dens; i++)
      if (cd->active[i]) {
	cd->accu_gamma_sum[i] = 0.00;
	if ((cd->cbnew_set->train_parmats != 0) && cd->parmat[i].mat) {
	  for (i2 = 0; i2 < cd->parmat[i].mat->num_block; i2++) {
	    d = cd->parmat[i].mat->list[i2]->m;
	    bound = (d*(d+1))/2;
	    for (j = 0; j < bound; j++)
	      cd->accu_cov[i].block[i2][j] = 0.00;
	  }
	  for (j = 0; j < cd->cbnew_set->num_dim - cd->parmat[i].mat->num_rel;
	       j++)
	    cd->accu_cov[i].diag[j] = 0.00;
	} else
	  for (j = 0; j < cd->cbnew_set->num_dim; j++)
	    cd->accu_cov[i].diag[j] = 0.00;
      }
    cd->accu_flag = ACCUS_EMPTY;
    cd->accu_num = 0;
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   calc_internals : transform means and calc lh_factor
   ------------------------------------------------------------------- */

static int calc_internals (CBNew* cd)
{

  int cb_num_dim;
  register int i,j,k,l;

  cb_num_dim = cd->cbnew_set->num_dim;

  if (!cd->trans_mean) 
    GETFMAT(cd->trans_mean,cd->num_dens,cb_num_dim);
 
  /* Transform means */
  for (k = 0; k < cd->num_dens; k++) {
    int    icum, *dim_vect;
    float *mean, *tr_mean;

    if (! cd->active[k]) continue;

    mean    = cd->mean->matPA[k];
    tr_mean = cd->trans_mean->matPA[k];

    if (! cd->parmat[k].mat) {
      for (j = 0; j < cb_num_dim; j++) tr_mean[j] = mean[j];
      continue;
    }

    icum     = 0;
    dim_vect = cd->parmat[k].mat->dim_vect;

    for (i= 0; i < cd->parmat[k].mat->num_block; i++) {	  
      int d = cd->parmat[k].mat->list[i]->m;	  
      for (j = 0; j < d; j++) {
	float *pmat_row = cd->parmat[k].mat->list[i]->matPA[j];
	float  ftemp    = 0.0;
	for (l= 0; l < d; l++) 
	  ftemp += (pmat_row[l] * mean[dim_vect[icum+l]]);
	tr_mean[dim_vect[j+icum]] = ftemp;
      }	  
      icum += d;
    }

    for (j = icum; j < cb_num_dim; j++) {
      int index = dim_vect[j];
      tr_mean[index] = mean[index];
    }
  }


  /* Calculate 'lh_factor' array */
  for (k = 0; k < cd->num_dens; k++) {
    if (cd->active[k]) {
      float* diag = cd->diag->matPA[k];
      double temp = ((double) cb_num_dim) * LOG_TWO_PI;
      for (j = 0; j < cb_num_dim; j++)
	temp += log ((double) diag[j]);
      cd->lh_factor[k] = (double) (- cd->distrib[k]) - 0.50*temp;
      /* INFO("  %d  pi=%4f  temp=%4f  lh=%4f\n",k,(double) cb_num_dim * LOG_TWO_PI,temp, cd->lh_factor[k]); */
    }
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   transform_sample : transform sample with parmat of Gaussian k
   ------------------------------------------------------------------- */

static int transform_sample (CBNew* cd,float* sample,int k,float** tr_sampP)
{
  register int i,j,l,icum,d,cb_num_dim;
  float sum;
  float* pmat_row,*tr_samp;
  int* dim_vect;
  CBNewParMatrix* pmat;

  assert (cd->cbnew_set->phase != CONSTRUCTION_PHASE);
  assert (sample);
  assert (tr_sampP);
  assert (k >= 0 && k < cd->num_dens);
  assert (cd->active[k]);

  cb_num_dim = cd->cbnew_set->num_dim;
  pmat = cd->parmat[k].mat;

  /* nothing to do, if we don't have a parameter matrix or is def. to I*/
  if (!pmat || pmat->def) {
    *tr_sampP = sample;
   return TCL_OK;
  }

  /* nothing to do, if the sample is already transformed */
  if (pmat->ts_valid == 1) {
    *tr_sampP = pmat->trans_sample;
    return TCL_OK;
  }

  /* transform the sample and cache the result */
  tr_samp  = pmat->trans_sample;
  dim_vect = pmat->dim_vect;
  for (i = icum = 0; i < pmat->num_block; i++,icum += d) {
    d = pmat->list[i]->m;
    for (j = 0; j < d; j++) {
      pmat_row = pmat->list[i]->matPA[j];
      for (l = 0,sum = 0.0; l < d; l++)
	sum += (pmat_row[l] * sample[dim_vect[icum+l]]);
      tr_samp[dim_vect[icum+j]] = sum;
    }
  }
  *tr_sampP = tr_samp;
  pmat->ts_valid = 1;
 
  return TCL_OK;
}


/* -------------------------------------------------------------------
   lh_single : calculate likelihood  p(sample| gaussian k)
   ------------------------------------------------------------------- */

static double lh_single (CBNew* cd,float* sample,int k)
{
  register int j;
  float sum = 0.0,temp;
  float* tr_mean,*diag,*tr_samp;

  assert (cd->cbnew_set->phase == WORKING_PHASE);
  
  /* asssert that we working on an active gaussian*/
  assert (k >= 0 && k < cd->num_dens);
  assert (cd->active[k]);

  /* assert that we have transformed the means already */
  assert (cd->internal_flag);
  tr_mean = cd->trans_mean->matPA[k];

  /* transform sample using parmat internal cache*/
  if (transform_sample (cd,sample,k,&tr_samp) == TCL_ERROR) {
    return TCL_ERROR;
  }

  /* compute mahlanobis distance */
  diag = cd->diag->matPA[k];
  for (j = 0; j < cd->cbnew_set->num_dim; j++) {
    temp = tr_samp[j] - tr_mean[j];
    sum += (temp * temp / diag[j]);
  }

  /* where is the prefactor? HAGEN */
  return (double) (-0.5 * sum);
}

/* -------------------------------------------------------------------
   CBNewAccu
   ------------------------------------------------------------------- */

int cbnewAccu (CBNew* cd,float* sample,double factor,double* loglh)
{
  CBNewSet *cbs = (CBNewSet*) cd->cbnew_set;  
  int refN      = cd->num_dens;
  int dimN      = cbs->num_dim;

  double scale, scoreSum;
  register int dimX, dimY, dimCov, refX, blockX, dimBlock, dimTotal;
  
  if (cbs->phase != WORKING_PHASE) {
    ERROR("%s not in working phase\n",cbs->name);
    return TCL_ERROR;
  }
  if (cd->accu_flag == ACCUS_LOCKED) {
    ERROR("%s Accus locked\n",cd->name);
    return TCL_ERROR;
  }
  for (refX = 0; refX < refN; refX++) {
    if (cd->parmat[refX].mat == NULL) {
      ERROR("%s refX=%d has no parmat\n",cd->name,refX);
      return TCL_ERROR;
    }
    if (cd->parmat[refX].mat->ts_valid != 0) {
      ERROR("param %s has active sample cache\n",cd->parmat[refX].mat->name);
      return TCL_ERROR;
    }
  }

  if (!cd->accu_mean)
    /* Allocate accumulator subobjects */
    if (allocate_accus (cd) == TCL_ERROR) {
      return TCL_ERROR;
    }
  if (cd->internal_flag == 0) {
    /* Calc. internal temporary fields (including the transformed means) */
    calc_internals (cd);
    cd->internal_flag = 1;
  }

  /* --------------------------------------------------*/
  /* special case:  mixture contains only one Gaussian */
  /* --------------------------------------------------*/

  /* The gamma factors don't have to be calculated. 
     Full covariance stats are gathered in the original domain. 
  */

  if (refN == 1 && cbs->train_parmats != 0) {   
    refX=0;
    if ( cd->active[refX] ) {     
      CBNewParMatrix *pm  = (cd->parmat[refX]).mat;
      double        *mean = cd->accu_mean->matPA[refX];
      double        *diag = cd->accu_cov[refX].diag;
      int          blockN = pm->num_block;
      double gam = factor;
      cd->accu_gamma_sum[refX] += gam;

      if (pm) {
	int* dim_vect = pm->dim_vect;
	for (dimX = 0; dimX < dimN; dimX++) {
	  cd->temp_samp[dimX] = (double) sample[dim_vect[dimX]];
	}

	dimTotal=0;
	for (blockX = 0; blockX < blockN; blockX++) {	  
	  double *cov = cd->accu_cov[refX].block[blockX];
	  dimBlock    = cd->parmat[refX].mat->list[blockX]->m;	  
	  dimCov      = 0;
	  for (dimX= 0;dimX < dimBlock; dimX++) {
	    double tmp = gam * cd->temp_samp[dimTotal+dimX];
	    mean[dim_vect[dimTotal+dimX]] += tmp;
	    for (dimY = dimX; dimY < dimBlock; dimY++, dimCov++) {
	      cov[dimCov] += tmp * cd->temp_samp[dimTotal+dimY];
	    }
	  }
	  dimTotal += dimBlock;
	}

	for (dimX = dimTotal; dimX < dimN; dimX++) {
	  double tmp = gam * cd->temp_samp[dimX];
	  mean[dim_vect[dimX]] += tmp;
	  diag[dimX-dimTotal]  += (tmp * cd->temp_samp[dimX]);
	}
      } else {
	for (dimX = 0; dimX < dimN; dimX++) {
	  mean[dimX] +=  gam * sample[dimX];
	  diag[dimX] +=  gam * sample[dimX] * sample[dimX];
	}
      }
    }
    cd->accu_num++;   
  }  else {
    
    /* -------------------------------------------------------*/
    /* general case:  mixture contains more than one Gaussian */
    /* -------------------------------------------------------*/

    /* note: lh_single calls cause sample transforms */
    /* calc. posteriories, log likelihoods and scale */   
    scale    = -1e37;
    scoreSum = 0.0;
    for (refX=0 ; refX < refN; refX++) {
      if (cd->active[refX]) {
	cd->temp_gamma[refX] = cd->lh_factor[refX] + lh_single (cd,sample,refX);
	if (cd->temp_gamma[refX] > scale) scale = cd->temp_gamma[refX];
      }
    }
    for (refX = 0; refX < refN; refX++) {
      if (cd->active[refX]) scoreSum += (cd->temp_gamma[refX] = exp (cd->temp_gamma[refX] - scale));
    }
    if (scoreSum <= 0.00) {
       ERROR("Too low likelihood! Score will be corrupt!\n");
       *loglh = 1000.00;
    } else {
      *loglh =  -(log (scoreSum) + scale); 

      /* Loop over the Gaussians */  
      for (refX = 0; refX < refN; refX++) {
	CBNewParMatrix *pm  = (cd->parmat[refX]).mat;
	double        *mean = cd->accu_mean->matPA[refX];
	double        *diag = cd->accu_cov[refX].diag;
	int          blockN = pm->num_block;
	int       *dim_vect = pm->dim_vect; /* added. HAGEN */
	double          gam = factor * cd->temp_gamma[refX] / scoreSum;
	float      *tr_samp = NULL;

	if (! cd->active[refX]) { continue; }

	cd->accu_gamma_sum[refX] += gam;
	
	transform_sample (cd,sample,refX,&tr_samp);

	if (cbs->train_parmats == 0 || ! pm) {
	  /* Why don't we use the dimension index vector dim_vect here? Hagen */
	  /* mean  accus in the original    domain */
	  /* covar accus in the transformed domain */
	  for (dimX = 0; dimX < dimN; dimX++) {
	    mean[dimX] += (gam * sample[dimX]); 
	    diag[dimX] += (gam * tr_samp[dimX] * tr_samp[dimX]);
	  }
	} else {
	  /* mean and covar accus in the original domain */
	  for (dimX = 0; dimX < dimN; dimX++) {
	    cd->temp_samp[dimX] = (double) sample[dim_vect[dimX]];
	  }

	  dimTotal=0;
	  for (blockX = 0; blockX < blockN; blockX++) {     
	    double *cov = cd->accu_cov[refX].block[blockX];
	    dimBlock    = cd->parmat[refX].mat->list[blockX]->m;    
	    dimCov      = 0;
	    for (dimX= 0;dimX < dimBlock; dimX++) {
	      double tmp = gam * cd->temp_samp[dimTotal+dimX];
	      mean[dim_vect[dimTotal+dimX]] += tmp;
	      for (dimY = dimX; dimY < dimBlock; dimY++, dimCov++) {
		cov[dimCov] += (tmp * cd->temp_samp[dimTotal+dimY]);
	      }
	    }
	    dimTotal += dimBlock;
	  }
	  for (dimX = dimTotal; dimX < dimN; dimX++) {
	    double tmp = gam * cd->temp_samp[dimX];
	    mean[dim_vect[dimX]] += tmp;
	    diag[dimX-dimTotal]  += (tmp * cd->temp_samp[dimX]);
	  }
	}
      }
      cd->accu_num++;  
    }
  }

  /* mark transformed sample cache as invalid */
  for (refX = 0; refX < refN; refX++) {
    if (cd->parmat[refX].mat) {
      cd->parmat[refX].mat->ts_valid =0;   
    }
  }

  if (cd->accu_num > 0) cd->accu_flag = ACCUS_BUILDING;
  return TCL_OK;
}


/* -------------------------------------------------------------------
   cbnewUpdate
   ------------------------------------------------------------------- */

int cbnewUpdate (CBNew* cd,int* not_mean,int* not_diag)
{
  
  int refN      = cd->num_dens;
  int dimN      = cd->cbnew_set->num_dim;  
  double sumDistrib = 0.0;
  float  logDistrib = 1.0;
  register int dimX, dimY, refX;

  int x=0, y=0;

  if (cd->accu_flag == ACCUS_LOCKED) {
    ERROR("accus in %s locked\n",cd->name);
    return TCL_ERROR;
  }

  if (cd->cbnew_set->phase != WORKING_PHASE) {
    ERROR("%s not in working phase\n",cd->cbnew_set->name);
    return TCL_ERROR;
  }

  if (cd->accu_flag == ACCUS_EMPTY) { 
    return TCL_OK;  /* nothing to do! */
  }

  *not_mean  = 0;
  *not_diag  = 0; 

  for (refX = 0; refX < refN; refX++) {    
    double beta = cd->accu_gamma_sum[refX];
    float* mean = cd->mean->matPA[refX];
    float* diag = cd->diag->matPA[refX];
    double* adiag  = NULL;
    double* amean  = NULL; 

    if (beta < GAMMA_SUM_THRES && cd->active[refX]) {
      cd->active[refX] = 0;  
      WARN("Gaussian %d of codebook '%s' deactivated!\n",refX,cd->name);
      cd->distrib[refX] = (float) -log (GAMMA_SUM_THRES);
    } else {
      cd->distrib[refX] = (float) - log (beta);
    }

    /* skip deactivated gaussians */
    if (! cd->active[refX]) { continue; }

    if (beta <= MEAN_UPDATE_THRES) x++; 
    if (beta > MEAN_UPDATE_THRES && beta <  DIAG_UPDATE_THRES) y++; 

    sumDistrib += beta;

    adiag = cd->accu_cov[refX].diag;
    amean = cd->accu_mean->matPA[refX];

    /* -------------------------------------- */
    /* case: PM trained                       */
    /* -------------------------------------- */

    if ( cd->cbnew_set->train_parmats != 0  && cd->parmat[refX].mat) { 
      register int blockX;

      int  dimTotal = 0;
      int    blockN = cd->parmat[refX].mat->num_block;
      int* dim_vect = cd->parmat[refX].mat->dim_vect;

      /* convert correlation stats to covariance stats */

      /* do it for block covered dimensions */      

      for (blockX = 0; blockX < blockN; blockX++) {
	int dimCov     = 0;
	int dimBlock   = cd->parmat[refX].mat->list[blockX]->m;
	double* ablock = cd->accu_cov[refX].block[blockX];
	double temp;
	for (dimX= 0; dimX < dimBlock; dimX++) {
	  temp = amean[dim_vect[dimTotal+dimX]] / beta;
	  for (dimY = dimX; dimY < dimBlock; dimY++) {
	    ablock[dimCov] -= amean[dim_vect[dimTotal+dimY]] * temp;
	    dimCov++;
	  }
	}
	dimTotal += dimBlock;
      }
      /* do it for diag covered dimensions */
      for (dimX = dimTotal; dimX < dimN; dimX++) {
	register double temp  = amean[dim_vect[dimX]];
	adiag[dimX-dimTotal] -= (temp*temp/beta);
      }
      /* Update mean */
      if (beta <= MEAN_UPDATE_THRES) {
	(*not_mean)++;
	continue;
      }
      for (dimX = 0; dimX < dimN; dimX++) {
	register int index = dim_vect[dimX];
	mean[index] = (float) (amean[index] / beta);
      }

      /* Update diagonal variance matrix */
      if (beta < DIAG_UPDATE_THRES) {
	(*not_diag)++;
	continue;
      }
      for (dimX = dimTotal; dimX < dimN; dimX++) {
	register double temp = adiag[dimX-dimTotal] / beta;
	if ( temp < DIAG_COMP_FLOOR)
	  temp = DIAG_COMP_FLOOR;
	diag[dim_vect[dimX]] = (float) temp;
      }
      reestimate_diag (cd,diag,refX);

    } /*endif PM trained */

    /* -------------------------------------- */
    /* case: PM not trained                   */
    /* -------------------------------------- */

    else {

      /* case: PM exists */
      if (cd->parmat[refX].mat) {
	register int blockX;

	int  dimTotal = 0;
	int    blockN = cd->parmat[refX].mat->num_block;
	int* dim_vect = cd->parmat[refX].mat->dim_vect;

	for (blockX = 0; blockX < blockN; blockX++ ) {
	  int dimBlock = cd->parmat[refX].mat->list[blockX]->m;

	  for (dimX = 0; dimX < dimBlock; dimX++) {
	    float* pmat_row = cd->parmat[refX].mat->list[blockX]->matPA[dimX];
	    double sum = 0.0;
	    for (dimY = 0; dimY < dimBlock; dimY++) {
	      sum += (((double) pmat_row[dimY]) * amean[dim_vect[dimTotal+dimY]]);
	    }
	    adiag[dim_vect[dimX+dimTotal]] -= (sum*sum/beta);
	  }
	  dimTotal += dimBlock;
	}
	for (dimX = dimTotal; dimX < dimN; dimX++) {
	  register int    index = dim_vect[dimX];
	  register double temp  = amean[index];
	  adiag[index] -= (temp*temp/beta);
	}
      } 
      /* case: PM doesn't exists*/
      else {
	for (dimX = 0; dimX < dimN; dimX++) {
	  register double temp = amean[dimX];
	  adiag[dimX] -= (temp*temp/beta);
	}
      }

      /* Update mean and covariance */
      if (beta < MEAN_UPDATE_THRES) {
	(*not_mean)++;
	continue;
      }	 
      for (dimX = 0; dimX < dimN; dimX++) {
	mean[dimX] = (float) (amean[dimX] / beta);
	if (beta >= DIAG_UPDATE_THRES) {
	  double temp = adiag[dimX] / beta;
	  if ( temp < DIAG_COMP_FLOOR)
	    temp = DIAG_COMP_FLOOR;
	  diag[dimX] = (float) temp;
	}
      }
      if (beta < DIAG_UPDATE_THRES) (*not_diag)++;

    } /* endelse PM no trained */

  } /* loopend for each gaussian refX */
   
  
  /* Normalize distrib */
  logDistrib = (float) log (sumDistrib);
  for (refX = 0; refX < refN; refX++)
    if (cd->active[refX]) cd->distrib[refX] += logDistrib;

  cd->accu_flag = ACCUS_LOCKED;
  cd->internal_flag = 0;  

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewEvalKL
   ------------------------------------------------------------------- */

int cbnewEvalKL (CBNew* cd,double* val)
{
  register int k;
  double sum = 0.00,denum = 0.00;

  assert (val);

  if (cd->accu_flag != ACCUS_LOCKED) {
    ERROR("accus not %s locked\n",cd->name);
    return TCL_ERROR;
  }

  if (cd->cbnew_set->phase != WORKING_PHASE) {
    ERROR("%s not in working phase\n",cd->cbnew_set->name);
    return TCL_ERROR;
  }

  for (k = 0; k < cd->num_dens; k++)
    if (cd->active[k]) {
      sum += eval_kl_gaussian (cd,k);
      denum += cd->accu_gamma_sum[k];
    }
  *val = (denum != 0.00) ? sum / denum : 0.00;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewCritfuncPtr
   ------------------------------------------------------------------- */
crit_func_ptr cbnewCritfuncPtr (char* name)
{
  int i;
  char errormsg[500];

  assert (name);

  sprintf (errormsg,"There's no criterion '%s'! The available functions are:\n",
	   name);
  for (i = 0; i < NUM_CRIT_FUNCS; i++) {
    strcat (errormsg,name2code[i].desc);
    if (!strcmp (name,name2code[i].name))
      return name2code[i].code;
  }

  ERROR(errormsg);
  return 0;  /* no such criterion */
}

/* -------------------------------------------------------------------
   cbnewCritfuncName
   ------------------------------------------------------------------- */

char* cbnewCritfuncName (crit_func_ptr code)
{
  int i;

  assert (code);

  for (i = 0; i < NUM_CRIT_FUNCS; i++) {
    if (name2code[i].code == code)
      return name2code[i].name;
  }

  return 0;  /* no such criterion */
}

/* -------------------------------------------------------------------
   cbnewCritLogLH
   Phase: working.
   The parameter matrices must have determinant 1!
   We use scaling to avoid underflow. If p('sample' | 'cd') = 0, we
   set the log odds to the lowest possible value.
   Note: We use sample caching (see 'transform_sample'). The method
   assumes that the 'ts_valid' flags of all parmats are cleared!
   ------------------------------------------------------------------- */
int cbnewCritLogLH (struct CBNew* cd,FMatrixX* data,double* val)
{
  register int i,k;
  double sum,llh = 0.00,scale;
  float* sample;

#ifdef DEBUG
  ASSERT_CB(cd,0);
  assert (cd->cbnew_set->phase == WORKING_PHASE);
  ASSERT_MATRIX(data,data->m,cd->cbnew_set->num_dim);
  assert (val);
  {
    int i;
    for (i = 0; i < cd->cbnew_set->parmat_set->list.itemN; i++)
      assert (cd->cbnew_set->parmat_set->list.itemA[i].ts_valid == 0);
  }
#endif

  /* Calc. internal temporary fields if necessary */
  if (cd->internal_flag == 0) {
    calc_internals (cd);
    cd->internal_flag = 1;
  }

  for (i = 0; i < data->m; i++) {
    sample = data->matPA[i];
    for (k = 0; !cd->active[k]; k++);
    cd->temp_gamma[k] = scale = cd->lh_factor[k] + lh_single (cd,sample,k);
    for (k++; k < cd->num_dens; k++)
      if (cd->active[k]) {
	cd->temp_gamma[k] = sum = cd->lh_factor[k] + lh_single (cd,sample,k);
	if (sum > scale) scale = sum;
      }
    sum = 0.00;
    for (k = 0; k < cd->num_dens; k++)
      if (cd->active[k])
	sum += exp (cd->temp_gamma[k] - scale);
    /* sum == 0 ABFANGEN !!!!!!! */
    llh += (log (sum) + scale);

    /* Clear 'ts_valid' flags of assoc. parmats */
    for (k = 0; k < cd->num_dens; k++)
      if (cd->parmat[k].mat)
	cd->parmat[k].mat->ts_valid = 0;
  }
  *val = llh / (double) data->m;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewOpenProt
   Phase: all.
   ------------------------------------------------------------------- */
int cbnewOpenProt (CBNew* cd,FMatrix* datamat,crit_func_ptr crit,int begin,
		   int end,int* protnum)
{
  int firstfree,i;
  CBNew_protslot* copy;

#ifdef DEBUG
  ASSERT_CB(cd,1);
  ASSERT_MATRIX(datamat,datamat->m,cd->cbnew_set->num_dim);
  assert (crit);
  assert (protnum);
  assert (begin >= 0 && begin <= end && end < datamat->m);
#endif

  for (firstfree = 0; firstfree < cd->protlist_size; firstfree++)
    if (!cd->slot[firstfree].crit) break;
  if (firstfree == cd->protlist_size) {
    /* Increase protocol slot list */
    cd->protlist_size += PROT_LIST_INCR_SIZE;
    if (!(copy = (CBNew_protslot*)
	  realloc (cd->slot,cd->protlist_size *
		   sizeof(CBNew_protslot)))) {
      ERROR("Out of memory!\n");
      cd->protlist_size -= PROT_LIST_INCR_SIZE;
      return TCL_ERROR;
    }
    cd->slot = copy;
    for (i = firstfree; i < cd->protlist_size; i++) {
      cd->slot[i].crit = 0;
      cd->slot[i].epoch = 0;
    }
  }
  cd->slot[firstfree].epoch = fmatrixxCreate (0,0);
  if (fmatrixxAlias2 (datamat,cd->slot[firstfree].epoch,begin,0,end,
		       cd->cbnew_set->num_dim-1) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }
  cd->slot[firstfree].crit = crit;
  cd->slot[firstfree].counter = 0;

  *protnum = firstfree;
  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewCloseProt
   Phase: all.
   ------------------------------------------------------------------- */
int cbnewCloseProt (CBNew* cd,int protnum)
{
#ifdef DEBUG
  ASSERT_CB(cd,1);
#endif

  if (protnum < 0 || protnum >= cd->protlist_size ||
      cd->slot[protnum].crit == 0) {
    ERROR("There's no protocol with number %d!\n",protnum);
    return TCL_ERROR;
  }
  if (cd->slot[protnum].epoch) fmatrixxDestroy (cd->slot[protnum].epoch);
  cd->slot[protnum].crit = 0;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewEvalProt
   Phase: working.
   ------------------------------------------------------------------- */
int cbnewEvalProt (CBNew* cd,int protnum)
{
  int i,begin = protnum,end = protnum;
  double value;
  int ret = TCL_OK;

#ifdef DEBUG
  ASSERT_CB(cd,1);
  assert (cd->cbnew_set->phase == WORKING_PHASE);
#endif

  if (protnum == -1) {
    begin = 0; end = cd->protlist_size - 1;
  } else if (protnum < 0 || protnum >= cd->protlist_size ||
	     !cd->slot[protnum].crit) {
    ERROR("There's no protocol with number %d!\n",protnum);
    return TCL_ERROR;
  }

  for (i = begin; i <= end; i++)
    if (cd->slot[i].crit) {
      if (cd->slot[i].crit (cd,cd->slot[i].epoch,&value) ==
	  TCL_ERROR) {
#ifdef DEBUG
	ERROR("<= called by\n");
#endif
	ret = TCL_ERROR;
      } else
	itfAppendResult ("{%d %d %f} ",i,cd->slot[i].counter++,(float) value);
    }

  return ret;
}

/* -------------------------------------------------------------------
   cbnewDataPlot
   Phase: != construction,!= testing.
   Generates a plot (GNUPlot format) that shows the effect of the
   parmat transformations. We assume the samples are independently
   drawn from the distribution modelled by the codebook and proceed
   as follows (for each sample):
   - classify sample using Bayes rule (that is label sample with class
     having the largest posterior prob. given that sample)
   - now if i is the label of sample x, W_i is the parmat of the i-th
     Gaussian and m_i is its mean, we calc.
     y = W_i (x - m_i) + m_i
   - store coordinates 'ind_vect' of y into plot file. If you focus
     on one of the Gaussian, y is stored if and only if it's labelled
     with that Gaussian
   The resulting plot should nicely show the effect of the parmat
   "rotations". Note that this is just for means of visualization and
   has no strict mathematical foundation.
   Note: We use sample caching (see `transform_sample'). The method
   assumes the 'ts_valid' flags of all parmats being cleared!
   ------------------------------------------------------------------- */
int cbnewDataPlot (CBNew* cd,FMatrix* data,int begin,int end,
                   SVector* ind_vect,FILE* fp_plot,int ref_ind)
{
  register int i,k,index,ind,cb_num_dim;
  float* sample,*mean,*tr_samp,*tr_mean;
  double scale,tempd;

#ifdef DEBUG
  ASSERT_CB(cd,1);
  assert (cd->cbnew_set->phase != CONSTRUCTION_PHASE &&
	  cd->cbnew_set->phase != TESTING_PHASE);
  ASSERT_MATRIX(data,data->m,cd->cbnew_set->num_dim);
  assert (begin >= 0 && begin <= end && end < data->m);
  assert (fp_plot);
  assert (ref_ind >= -1 && ref_ind < cd->num_dens);
  assert (ind_vect);
  assert (ind_vect->n > 0);
  {
    int i;
    for (i = 0; i < ind_vect->n; i++)
      assert (ind_vect->vecA[i] >= 0 &&
	      ind_vect->vecA[i] < cd->cbnew_set->num_dim);
  }
  {
    int i;
    for (i = 0; i < cd->cbnew_set->parmat_set->list.itemN; i++)
      assert (cd->cbnew_set->parmat_set->list.itemA[i].ts_valid == 0);
  }
#endif

  cb_num_dim = cd->cbnew_set->num_dim;
  if (cd->internal_flag == 0) {
    calc_internals (cd);  /* Calc. internal vars */
    cd->internal_flag = 1;
  }

  /* main loop */
  for (i = begin; i <= end; i++) {
    /* label sample using Bayes rule */
    sample = data->matPA[i];
    for (k = 0; !cd->active[k]; k++);
    scale = cd->lh_factor[k] + lh_single (cd,sample,k);
    index = k;
    for (k++; k < cd->num_dens; k++)
      if (cd->active[k]) {
	tempd = cd->lh_factor[k] + lh_single (cd,sample,k);
	if (tempd > scale) {
	  scale = tempd;
	  index = k;
	}
      }

    /* "rotate" sample around mean of its class */
    if (ref_ind == -1 || ref_ind == index) {
      /* The sample has been transformed and stored during the calc. of
	 the likelihood scores, so 'transform_sample' has not much to do */
      transform_sample (cd,sample,index,&tr_samp);
      mean = cd->mean->matPA[index];
      tr_mean = cd->trans_mean->matPA[index];
      for (k = 0; k < ind_vect->n; k++) {
	ind = ind_vect->vecA[k];
	fprintf (fp_plot,"%f ",tr_samp[ind] - tr_mean[ind] + mean[ind]);
      }
      fprintf (fp_plot,"\n");
    }

    /* clear 'ts_valid' flags of assoc. parmats */
    for (k = 0; k < cd->num_dens; k++)
      if (cd->parmat[k].mat)
	cd->parmat[k].mat->ts_valid = 0;
  }

  return TCL_OK;
}


/* -------------------------------------------------------------------
   normal_random_vector
   Generates sample of random vector of 'num_dim' independant comp.
   of Gaussian distribution. We use the polar method (see Knuth II,
   page 117) to transform two uniform variates into two normal ones.
   ------------------------------------------------------------------- */

int normal_random_vector (int num_dim,float* mean,float* var,float* sample)
{
  int j;
  float v1,v2,s,x1,x2,temp;

  for (j = 0; j < num_dim; j += 2) {
    do {
      v1 = chaosRandom (-1.0,1.0);
      v2 = chaosRandom (-1.0,1.0);
    } while ((s = v1*v1 + v2*v2) >= 1.0);
    temp = sqrt (-2.0*log (s) / s);
    x1 = v1 * temp;
    x2 = v2 * temp;
    sample[j] = sqrt (var[j]) * x1 + mean[j];
    if (j+1 < num_dim) sample[j+1] = sqrt (var[j+1]) * x2 + mean[j+1];
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewGenSamples_cleanup
   ------------------------------------------------------------------- */

void cbnewGenSamples_cleanup (CBNew* cd,DMatrix*** pm_inv,float** tr_mean,
			      float* sample,DMatrix* copymat)
{
  register int i,j;

  if (pm_inv) {
    for (i = 0; i < cd->num_dens; i++)
      if (pm_inv[i]) {
	for (j = 0; j < cd->parmat[i].mat->num_block; j++)
	  FREEDMAT(pm_inv[i][j]);
	free (pm_inv[i]);
      }
    FREE(pm_inv);
  }
  if (tr_mean) {
    for (i = 0; i < cd->num_dens; i++)
      FREE(tr_mean[i]);
    FREE(tr_mean);
  }
  FREE(sample);
  FREEDMAT(copymat);
}

#define GETMEMGS(var,type,size)\
{\
  if (!((var) = (type*) malloc ((size) * sizeof(type)))) {\
    ERROR("Out of memory!\n");\
    cbnewGenSamples_cleanup (cd,pm_inv,tr_mean,sample,copymat);\
    return TCL_ERROR;\
  }\
}

/* -------------------------------------------------------------------
   cbnewGenSamples
   Phase: != testing.
   Generates set of independant samples drawn from the distribution
   modelled by the codebook. If x is N(mu,Sigma)-distributed, A*x is
   N(A*mu,A*Sigma*A^t)-distributed, so to get a RV which is
   N(mu,(W*Diag*W^t)^(-1))-distributed, we generate a
   N(W^t*mu,Diag)-distributed sample x (which is easy) and use
   W^(-t)*x.
   Helper variables:
   - pm_inv: 2-dim. field of double matrices which stores the inverses
     of the parmat blocks assoc. with the Gaussians of the codebook
   - tr_mean: Stores the transformed means (W^t*mu) of the codebook
     (note that the 'trans_mean' internal field of the codebook stores
     W*mu and cannot be used here)
   ------------------------------------------------------------------- */
int cbnewGenSamples (CBNew* cd,int num_samples,FMatrix* smat,double* seed,
		     int choose_seed)
{
  register int i,j,k,l,m,jcum,label,cb_num_dim;
  DMatrix*** pm_inv = 0;
  DMatrix* copymat = 0;
  float** tr_mean = 0;
  float* sample = 0;
  double temp,sum;
  CBNewParMatrix* pmat;

#ifdef DEBUG
  ASSERT_CB(cd,0);
  assert (cd->cbnew_set->phase != TESTING_PHASE);
  assert (num_samples > 0);
  assert (smat);
  assert (seed);
#endif

  cb_num_dim = cd->cbnew_set->num_dim;
  if (fmatrixResize (smat,num_samples,cb_num_dim) == TCL_ERROR) {
    ERROR("Out of memory!\n");
    return TCL_ERROR;
  }

  /* Allocate temporary fields */
  copymat = dmatrixCreate (0,0);
  GETMEMC(pm_inv,DMatrix**,cd->num_dens);
  GETMEMGS(tr_mean,float*,cd->num_dens);
  GETMEMGS(sample,float,cb_num_dim);
  for (i = 0; i< cd->num_dens; i++) tr_mean[i] = 0;
  for (i = 0; i < cd->num_dens; i++) {
    GETMEMGS(tr_mean[i],float,cb_num_dim);
    pmat = cd->parmat[i].mat;
    if (cd->active[i] && pmat) {
      GETMEMGS(pm_inv[i],DMatrix*,pmat->num_block);
      for (j = 0; j < pmat->num_block; j++)
	pm_inv[i][j] = 0;
      for (j = 0; j < pmat->num_block; j++) {
	m = pmat->list[j]->m;
	if (!(pm_inv[i][j] = dmatrixCreate (m,m))) {
	  ERROR("Out of memory!\n");
	  cbnewGenSamples_cleanup (cd,pm_inv,tr_mean,sample,copymat);
	  return TCL_ERROR;
	}
      }
    }
  }

  /* Calculates inverses of parmat blocks and transformed means */
  for (i = 0; i < cd->num_dens; i++)
    if (cd->active[i]) {
      pmat = cd->parmat[i].mat;
      if (pmat) {
	for (k = jcum = 0; k < pmat->num_block; k++,jcum += m) {
	  m = pmat->list[k]->m;
	  dmatrixFMatrix (copymat,pmat->list[k]);
	  dmatrixInv (pm_inv[i][k],copymat);
	  for (j = 0; j < m; j++) {
	    sum = 0.0;
	    for (l = 0; l < m; l++)
	      sum += (pmat->list[k]->matPA[l][j] *
		      cd->mean->matPA[i][pmat->dim_vect[jcum+l]]);
	    tr_mean[i][pmat->dim_vect[jcum+j]] = sum;
	  }
	}
	for (j = jcum; j < cb_num_dim; j++) {
	  l = pmat->dim_vect[j];
	  tr_mean[i][l] = cd->mean->matPA[i][l];
	}
      } else {
	for (j = 0; j < cb_num_dim; j++)
	  tr_mean[i][j] = cd->mean->matPA[i][j];
      }
    }
  if (choose_seed) {
    temp = log ((double) (time (0) + 1));
    *seed = exp (temp - log (10.00) * ceil (temp / log (10.00)));
  }
  chaosRandomInit (*seed);

  for (i = 0; i < num_samples; i++) {
    /* Draw class label */
    temp = chaosRandom (0.0,1.0);
    for (k = 0; !cd->active[k]; k++);
    sum = exp (- cd->distrib[k]); label = k++;
    while (sum < temp && k < cd->num_dens) {
      if (cd->active[k]) {
	sum += exp (- cd->distrib[k]);
	label = k;
      }
      k++;
    }
    normal_random_vector (cb_num_dim,tr_mean[label],cd->diag->matPA[label],
			  sample);
    pmat = cd->parmat[label].mat;
    if (pmat) {
      for (k = jcum = 0; k < pmat->num_block; k++,jcum += m) {
	m = pmat->list[k]->m;
	for (j = 0; j < m; j++) {
	  sum = 0.0;
	  for (l = 0; l < m; l++)
	    sum += (((float) pm_inv[label][k]->matPA[l][j]) *
		    sample[pmat->dim_vect[jcum+l]]);
	  smat->matPA[i][pmat->dim_vect[jcum+j]] = sum;
	}
      }
      for (j = jcum; j < cb_num_dim; j++) {
	l = pmat->dim_vect[j];
	smat->matPA[i][l] = sample[l];
      }
    } else {
      for (j = 0; j < cb_num_dim; j++)
	smat->matPA[i][j] = sample[j];
    }
  }

  cbnewGenSamples_cleanup (cd,pm_inv,tr_mean,sample,copymat);
  return TCL_OK;
}

#undef GETMEMGS


/* --- Janus itf functions --- */

/* -------------------------------------------------------------------
   cbnewPutsItf
   Phase: all.
   ------------------------------------------------------------------- */
int cbnewPutsItf (ClientData cd,int argc,char *argv[])
{
  return cbnewPuts ((CBNew*) cd);
}

/* -------------------------------------------------------------------
   cbnewConfigureItf
   Phase: all.
   ------------------------------------------------------------------- */
int cbnewConfigureItf (ClientData cd,char *var,char *val)
{
  CBNew* obj = (CBNew*) cd;

  if (!var) {
    ITFCFG(cbnewConfigureItf,cd,"name");
    ITFCFG(cbnewConfigureItf,cd,"useN");
    ITFCFG(cbnewConfigureItf,cd,"refN");
    ITFCFG(cbnewConfigureItf,cd,"cbnewSet");
    ITFCFG(cbnewConfigureItf,cd,"featIndex");
    ITFCFG(cbnewConfigureItf,cd,"accuFlag");
    ITFCFG(cbnewConfigureItf,cd,"accuN");
    return TCL_OK;
  }
  ITFCFG_CharPtr (var,val,"name",     obj->name,          1);
  ITFCFG_Int     (var,val,"useN",     obj->useN,          1);
  ITFCFG_Int     (var,val,"refN",     obj->num_dens,      1);
  ITFCFG_Object  (var,val,"cbnewSet", obj->cbnew_set,
		  name,CBNewSet,                          1);
  ITFCFG_Int     (var,val,"featIndex",obj->feat,          1);
  ITFCFG_Int     (var,val,"accuFlag",obj->accu_flag,      1);
  ITFCFG_Int     (var,val,"accuN",   obj->accu_num,       1);

  return TCL_ERROR;
}

/* -------------------------------------------------------------------
   cbnewAccessItf
   Phase: all.
   ------------------------------------------------------------------- */
ClientData cbnewAccessItf (ClientData cd,char* name,TypeInfo** ti)
{
  CBNew* obj = (CBNew*) cd;
  int l,m,i,d;

  if (*name == '.') {
    if (name[1] == 0) {
      itfAppendResult ("active distrib mean diag parmat(0..%d) ",
		       obj->num_dens-1);
      if (obj->cbnew_set->phase == WORKING_PHASE && obj->accu_mean) {
	itfAppendResult ("accuGammaS accuMean accuCovBlock(0..%d,0..) accuCovDiag(0..%d) ",
			 obj->num_dens-1,obj->num_dens-1);
      }
      *ti = 0;
      return 0;
    } else {
      if (!strcmp (name+1,"active")) {
        print_char_vector (0,obj->active,obj->num_dens);
	*ti = 0;
	return 0;
      }
      if (!strcmp (name+1,"distrib")) {
	for (i = 0; i < obj->num_dens; i++)
	  obj->temp_gamma[i] = exp (- obj->distrib[i]);
        print_double_vector (0,obj->temp_gamma,obj->num_dens);
	*ti = 0;
	return 0;
      }
      if (!strcmp (name+1,"mean")) {
	print_float_matrix (0,obj->mean);
	*ti = 0;
	return 0;
      }
      if (!strcmp (name+1,"diag")) {
	print_float_matrix (0,obj->diag);
	*ti = 0;
	return 0;
      }
      if (sscanf (name+1,"parmat(%d)",&l) == 1) {
	if (l >= 0 && l < obj->num_dens && obj->parmat[l].mat) {
	  *ti = itfGetType ("CBNewParMatrix");
	  return (ClientData) obj->parmat[l].mat;
	}
      }
      if (obj->cbnew_set->phase == WORKING_PHASE && obj->accu_mean) {
	if (!strcmp (name+1,"accuGammaS")) {
          print_double_vector (0,obj->accu_gamma_sum,obj->num_dens);
	  *ti = 0;
	  return 0;
	}
	if (!strcmp (name+1,"accuMean")) {
	  print_double_matrix (0,obj->accu_mean);
	  *ti = 0;
	  return 0;
	}
	if (sscanf (name+1,"accuCovBlock(%d,%d)",&l,&m) == 2 && l >= 0 &&
	    l < obj->num_dens && obj->cbnew_set->train_parmats != 0) {
	  if (obj->parmat[l].mat && m >= 0 &&
		     m < obj->parmat[l].mat->num_block) {
	    print_symm_double_matrix (obj->accu_cov[l].block[m],
				      obj->parmat[l].mat->list[m]->m);
	  *ti = 0;
	  return 0;
	  }
	}
	if (sscanf (name+1,"accuCovDiag(%d)",&l) == 1 && l >= 0 &&
	    l < obj->num_dens && obj->accu_cov[l].diag) {
	  if (obj->cbnew_set->train_parmats == 0)
	    d = obj->cbnew_set->num_dim;
	  else
	    d = obj->cbnew_set->num_dim - obj->parmat[l].mat->num_rel;
	  print_double_vector (0,obj->accu_cov[l].diag,d);
	  *ti = 0;
	  return 0;
	}
      }
    }
  }
  ERROR("No such subobject!\n");
  *ti = 0;

  return 0;
}

/* -------------------------------------------------------------------
   cbnewSetItf
   Phase: construction (checked).
   ------------------------------------------------------------------- */
int cbnewSetItf (ClientData cd,int argc,char *argv[])
{
  CBNew* obj = (CBNew*) cd;
  int ac = argc-1;
  char* so_name = 0;
  FMatrix* mat = 0;
  int i,j;

  if (obj->cbnew_set->phase != CONSTRUCTION_PHASE) {
    ERROR("Codebook set is not in construction phase!\n");
    return TCL_ERROR;
  }
  if (itfParseArgv (argv[0],&ac,argv+1,1,
		    "<subObj>",ARGV_STRING,NULL,&so_name,NULL,
		    "subobject",
		    "<matrix>",ARGV_OBJECT,NULL,&mat,    "FMatrix",
		    "source matrix",
		    NULL) != TCL_OK) return TCL_ERROR;

  if (!strcmp (so_name,"distrib")) {
    if (mat->m == 1 && mat->n == obj->num_dens) {
      for (i = 0; i < obj->num_dens; i++)
	obj->distrib[i] = (float) -log ((double) mat->matPA[0][i]);
      return TCL_OK;
    } else
      ERROR("Matrix must be 1x%d!\n",obj->num_dens);
  }
  if (!strcmp (so_name,"mean")) {
    if (mat->m == obj->num_dens && mat->n == obj->cbnew_set->num_dim) {
      fmatrixCopy (obj->mean,mat);
      return TCL_OK;
    } else
      ERROR("Matrix must be %dx%d!\n",obj->num_dens,obj->cbnew_set->num_dim);
  }
  if (!strcmp (so_name,"diag")) {
    if (mat->m == obj->num_dens && mat->n == obj->cbnew_set->num_dim) {
      for (i = 0; i < mat->m; i++)
	for (j = 0; j < mat->n; j++)
	  if (mat->matPA[i][j] <= 0.0) {
	    ERROR("All entries must be positive!\n");
	    return TCL_ERROR;
	  }
      fmatrixCopy (obj->diag,mat);
      return TCL_OK;
    } else
      ERROR("Matrix must be %dx%d!\n",obj->num_dens,obj->cbnew_set->num_dim);
  }

  ERROR("'%s' couldn't be set!\n",so_name);
  return TCL_ERROR;
}


/* -------------------------------------------------------------------
   Interface CBNewSet
   ------------------------------------------------------------------- */

/* --- Internal functions --- */

/* -------------------------------------------------------------------
   cbnewsetInit
   ------------------------------------------------------------------- */
int cbnewsetInit (CBNewSet* cd,char* name,int num_dim,
		  CBNewParMatrixSet* parmat_set,FeatureSet* feat_set)
{
#ifdef DEBUG
  assert (name);
  ASSERT_PMSET(parmat_set,1);
  assert (parmat_set->cbnew_set == 0);
  assert (num_dim == parmat_set->num_dim);
  assert (feat_set);
#endif

  /* Simple components. Set pointers to zero. */
  cd->useN = 0;  /* no references to this object */
  if (!(cd->name = strdup (name))) {
    ERROR("Out of memory!\n");
    return TCL_ERROR;
  }
  cd->num_dim = num_dim;
  cd->phase = CONSTRUCTION_PHASE;
  cd->train_parmats = 1;
  cd->parmat_set = parmat_set;
  cd->feat_set = feat_set;
  link (parmat_set,"CBNewParMatrixSet");
  link (feat_set,"FeatureSet");

  /* Initialize list */
  listInit ((List*) &(cd->list),&cbnewInfo,sizeof(CBNew),CBS_LIST_BLKSIZE);
  cd->list.init = (ListItemInit*) cbnewInitEmpty;
  cd->list.deinit = (ListItemDeinit*) cbnewDeinit;

  /* Link parmat set to codebook set and increment 'useN' counters of assoc.
     parmat set and feat. set */
  parmat_set->cbnew_set = cd;
  parmat_set->useN++;
  feat_set->useN++;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewsetCreate
   ------------------------------------------------------------------- */
CBNewSet* cbnewsetCreate (char* name,int num_dim,
			  CBNewParMatrixSet* parmat_set,FeatureSet* feat_set)
{
  CBNewSet* cd;

  if (!(cd = (CBNewSet*) malloc (sizeof(CBNewSet)))) {
    ERROR("Out of memory!\n");
    return 0;
  }
  if (cbnewsetInit (cd,name,num_dim,parmat_set,feat_set) == TCL_ERROR)
    return 0;

  return cd;
}

/* -------------------------------------------------------------------
   cbnewsetLoad
   Understands structure file format version 0 (initial version)
   ------------------------------------------------------------------- */
CBNewSet* cbnewsetLoad (char* name,char* fname,CBNewParMatrixSet* parmat_set,
			FeatureSet* feat_set,int* wrongFeatN,int* noPMN)
{
  CBNewSet* cd;
  register int i,j;
  int num_dim,num_cb,num_dens,dummy,feat,index,pm_ind;
  char temp_str[256],temp_str2[256];
  FILE* fp;
  CBNewParMatrix* pmat;
  CBNew* cb;

#ifdef DEBUG
  assert (name);
  assert (fname);
  ASSERT_PMSET(parmat_set,0);
  assert (parmat_set->cbnew_set == 0);
  assert (feat_set);
#endif

  *wrongFeatN = *noPMN = 0;
  if (!(fp = fileOpen (fname,"r"))) {
    ERROR("Cannot open file '%s'!\n",fname);
    return 0;
  }
  if ((scan_line (fp,temp_str,256) == TCL_ERROR) ||
      strcmp (temp_str,"CBSTRUCT")) {
    ERROR("'%s' is not a cbset structure file!\n",fname);
    return 0;
  }
  if (fscanf (fp,"%d\n%d\n%d\n",&dummy,&num_dim,&num_cb) != 3) {
    ERROR("Error while reading file!\n");
    return 0;
  }
  if (dummy != 0) {
    ERROR("The file format version no. %d cannot be handeled by this revision. Please get an upgrade!\n",dummy);
    return 0;
  }

  if (!(cd = (CBNewSet*) malloc (sizeof(CBNewSet)))) {
    ERROR("Out of memory!\n");
    return 0;
  }
  if (cbnewsetInit (cd,name,num_dim,parmat_set,feat_set) == TCL_ERROR) {
    free (cd);
    return 0;
  }

  /* Create codebook list */
  if (num_cb > 0) {
    for (i = 0; i < num_cb; i++) {
      if (scan_line (fp,temp_str,256) == TCL_ERROR ||
	  scan_line (fp,temp_str2,256) == TCL_ERROR) {
	ERROR("Error while reading structure file '%s'!\n",fname);
	cbnewsetDestroy (cd);
	return 0;
      }
      if (listIndex ((List*) &(cd->list),(ClientData) temp_str,0) >= 0) {
	ERROR("Object '%s' already exists!\n",temp_str);
	cbnewsetDestroy (cd);
	return 0;
      }
      if ((feat = fesIndex (feat_set,temp_str2,
			    FE_FMATRIX,cd->num_dim,1)) < 0) {
	(*wrongFeatN)++;
	continue;
      }
      if (scan_line (fp,temp_str2,256) == TCL_ERROR ||
	  sscanf (temp_str2,"%d",&num_dens) != 1) {
	ERROR("Error while reading structure file '%s'!\n",fname);
	cbnewsetDestroy (cd);
	return 0;
      }
      MSGCLEAR(NULL);
      index = listNewItem ((List*) &(cd->list),(ClientData) temp_str);
      cb = &(cd->list.itemA[index]);
      if (cbnewInit (&(cd->list.itemA[index]),
		     temp_str,num_dens,cd,feat) == TCL_ERROR) {
#ifdef DEBUG
	ERROR("<= called by\n");
#endif
	feat_set->featA[feat].useN--; /* Decrease 'useN' of feature */
	cbnewsetDestroy (cd);
	return 0;
      }
      /* loop over gaussians of codebook */
      for (j = 0; j < num_dens; j++) {
	if (scan_line (fp,temp_str,256) == TCL_ERROR) {
	  ERROR("Error while reading structure file '%s'!\n",fname);
	  cbnewsetDestroy (cd);
	  return 0;
	}
	/* If the parameter matrix of the stored gaussian doesn't exist in
	   the system, the gaussian is modelled without parameter matrix. */
	if (strlen (temp_str) > 0) {
	  if ((index = listIndex ((List*) &(parmat_set->list),
				temp_str,0)) >= 0) {
	    /* link parameter matrix to gaussian */
	    pmat = &(parmat_set->list.itemA[index]);
	    if (cbnewparmatLink (pmat,cb,j,&pm_ind) == TCL_ERROR) {
#ifdef DEBUG
	      ERROR("<= called by\n");
#endif
	      cbnewsetDestroy (cd);
	      return 0;
	    }
	    cb->parmat[j].mat = pmat;
	    cb->parmat[j].index = pm_ind;
	  }
	} else
	  (*noPMN)++;
      }
    }
  }

  return cd;
}

/* -------------------------------------------------------------------
   cbnewsetDeinit
   Phase: all.
   ------------------------------------------------------------------- */
int cbnewsetDeinit (CBNewSet* cd)
{
#ifdef DEBUG
  assert (cd);
  assert (cd->useN == 0);
#endif

  if (listDeinit ((List*) &(cd->list)) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }
  if (cd->name) {
    free (cd->name);
    cd->name = 0;
  }
  if (cd->parmat_set) {
    cd->parmat_set->cbnew_set = 0;  /* Delete link */
    cd->parmat_set->useN--;  /* Decrement useN counter */
    cd->parmat_set = 0;
  }
  if (cd->feat_set) {
    cd->feat_set->useN--;  /* Decrement useN counter */
    cd->feat_set = 0;
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewsetDestroy
   Phase: all.
   ------------------------------------------------------------------- */
int cbnewsetDestroy (CBNewSet* cd)
{
#ifdef DEBUG
  assert (cd);
#endif

  if (cbnewsetDeinit (cd) == TCL_ERROR) return TCL_ERROR;
  free (cd);  /* free object itself */

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewsetSave
   Phase: all.
   Uses structure file format version 0 (initial version)
   ------------------------------------------------------------------- */
int cbnewsetSave (CBNewSet* cd,char* fname)
{
  register int i,j;
  FILE* fp;
  char* name;

#ifdef DEBUG
  ASSERT_CBSET(cd,1);
  assert (fname);
#endif

  if (!(fp = fileOpen (fname,"w"))) {
    ERROR("Cannot create file '%s'!\n",fname);
    return TCL_ERROR;
  }
  fprintf (fp,"CBSTRUCT\n0\n%d\n%d\n",cd->num_dim,cd->list.itemN);
  for (i = 0; i < cd->list.itemN; i++) {
    print_line (fp,cd->list.itemA[i].name);
    print_line (fp,fesGetName (cd->feat_set,cd->list.itemA[i].feat));
    fprintf (fp,"%d\n",cd->list.itemA[i].num_dens);
    for (j = 0; j < cd->list.itemA[i].num_dens; j++) {
      name = cd->list.itemA[i].parmat[j].mat ?
	cd->list.itemA[i].parmat[j].mat->name : "";
      print_line (fp,name);
    }
  }
  fileClose (fname,fp);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewsetLoadWeights
   Phase: != testing.
   Understands weights file format version 0 (initial version)
   ------------------------------------------------------------------- */
int cbnewsetLoadWeights (CBNewSet* cd,char* fname,int* okN,int* nexistN,
			 int* nemptyN,int* wrongRefNN)
{
  register int i;
  int num_cbs,dummy,index;
  char temp_str[256];
  FILE* fp;
  CBNew* cb = NULL;
  char disc_flag;

#ifdef DEBUG
  ASSERT_CBSET(cd,1);
  assert (cd->phase != TESTING_PHASE);
  assert (fname);
  assert (okN);
  assert (nexistN);
  assert (nemptyN);
  assert (wrongRefNN);
#endif

  *okN = *nexistN = *nemptyN = *wrongRefNN = 0;
  if (!(fp = fileOpen (fname,"r"))) {
    ERROR("Cannot open file '%s'!\n",fname);
    return TCL_ERROR;
  }
  if ((read_string (fp,temp_str) == 0) || strcmp (temp_str,"CBWEIGHT")) {
    ERROR("'%s' is not a cbset weight file!\n",fname);
    return TCL_ERROR;
  }
  dummy = read_int (fp);
  if (dummy != 0) {
    ERROR("The file format version no. %d cannot be handeled by this revision. Please get an upgrade!\n",dummy);
    return 0;
  }
  dummy = read_int (fp);
  if (dummy != cd->num_dim) {
    ERROR("Stored system has wrong number of feature space dimensions!\n");
    return TCL_ERROR;
  }
  num_cbs = read_int (fp);
  for (i = 0; i < num_cbs; i++) {
    if (read_string (fp,temp_str) == 0) {
      ERROR("Error while reading file!\n");
      return TCL_ERROR;
    }
    disc_flag = 0;
    if ((index = listIndex ((List*) &(cd->list),
			    (ClientData) temp_str,0)) < 0) {
      (*nexistN)++;
      disc_flag = 1;
    } else {
      cb = &(cd->list.itemA[index]);
      if (cb->accu_flag != ACCUS_EMPTY) {
	disc_flag = 1;
	(*nemptyN)++;
      }
    }
    dummy = read_int (fp);
    if (!disc_flag && (dummy != cb->num_dens)) {
      (*wrongRefNN)++;
      disc_flag = 1;
    }
    if (disc_flag) {
      /* Discard entry */
      if (discard_char_vector (fp,dummy) == TCL_ERROR) {
        ERROR("Error while reading file!\n");
        return TCL_ERROR;
      }
      if (discard_float_vector (fp,dummy) == TCL_ERROR) {
        ERROR("Error while reading file!\n");
        return TCL_ERROR;
      }
      if (discard_float_matrix (fp,dummy,cd->num_dim) == TCL_ERROR) {
        ERROR("Error while reading file!\n");
        return TCL_ERROR;
      }
      if (discard_float_matrix (fp,dummy,cd->num_dim) == TCL_ERROR) {
        ERROR("Error while reading file!\n");
        return TCL_ERROR;
      }
    } else {
      /* Reset internal flag. The internal variables won't be
	 valid any more. */
      cb->internal_flag = 0;
      if (read_char_vector (fp,cb->active,dummy) == TCL_ERROR) {
        ERROR("Error while reading file!\n");
        return TCL_ERROR;
      }
      if (read_float_vector (fp,cb->distrib,dummy) == TCL_ERROR) {
        ERROR("Error while reading file!\n");
        return TCL_ERROR;
      }
      if (read_float_matrix (fp,cb->mean) == TCL_ERROR) {
        ERROR("Error while reading file!\n");
        return TCL_ERROR;
      }
      if (read_float_matrix (fp,cb->diag) == TCL_ERROR) {
        ERROR("Error while reading file!\n");
        return TCL_ERROR;
      }
      (*okN)++;
    }
  }
  fileClose (fname,fp);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewsetSaveWeights
   Phase: != testing.
   Uses weights file format version 0 (initial version)
   ------------------------------------------------------------------- */
int cbnewsetSaveWeights (CBNewSet* cd,char* fname)
{
  register int i;
  int num_dens;
  FILE* fp;

#ifdef DEBUG
  ASSERT_CBSET(cd,1);
  assert (cd->phase != TESTING_PHASE);
  assert (fname);
#endif

  if (!(fp = fileOpen (fname,"w"))) {
    ERROR("Cannot create file '%s'!\n",fname);
    return TCL_ERROR;
  }
  write_string (fp,"CBWEIGHT");
  write_int (fp,0);  /* file format version */
  write_int (fp,cd->num_dim);
  write_int (fp,cd->list.itemN);
  for (i = 0; i < cd->list.itemN; i++) {
    write_string (fp,cd->list.itemA[i].name);
    num_dens = cd->list.itemA[i].num_dens;
    write_int (fp,num_dens);
    write_char_vector (fp,cd->list.itemA[i].active,num_dens);
    write_float_vector (fp,cd->list.itemA[i].distrib,num_dens);
    write_float_matrix (fp,cd->list.itemA[i].mean);
    write_float_matrix (fp,cd->list.itemA[i].diag);
  }
  fileClose (fname,fp);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewsetLoadAccus
   Phase: working.
   Understands accu file format version 0 (initial version)
   ------------------------------------------------------------------- */
int cbnewsetLoadAccus (CBNewSet* cd,char* fname,int* emptyN,int* lockedN,
		       int* buildN,int* nexistN,int* wrongRefNN,int* wrongAS,
		       int* structN,int* inactN)
{
  register int i,j,k;
  int num_cbs,num_dens,dummy,num_block,d,index,size;
  char phase,accu_flag,train_parmats;
  char temp_str[256];
  FILE* fp;
  CBNew* cb = NULL;
  char disc_flag,temp_disc;

#ifdef DEBUG
  ASSERT_CBSET(cd,1);
  assert (cd->phase == WORKING_PHASE);
  assert (fname);
  assert (emptyN);
  assert (lockedN);
  assert (buildN);
  assert (nexistN);
  assert (wrongRefNN);
  assert (structN);
  assert (inactN);
#endif

  /* File handling and initialization */
  *emptyN = *lockedN = *buildN = *nexistN = *wrongRefNN = *structN =
    *inactN = *wrongAS = 0;
  if (!(fp = fileOpen (fname,"r"))) {
    ERROR("Cannot open file '%s'!\n",fname);
    return TCL_ERROR;
  }

  if ((read_string (fp,temp_str) == 0) || strcmp (temp_str,"CBACCUMU")) {
    ERROR("'%s' is not a cbset accumulator file!\n",fname);
    return TCL_ERROR;
  }
  dummy = read_int (fp);  /* version number */
  if (dummy != 0) {
    ERROR("The file format version no. %d cannot be handeled by this revision. Please get an upgrade!\n",dummy);
    return 0;
  }
  dummy = read_int (fp);
  if (dummy != cd->num_dim) {
    ERROR("Stored system has wrong number of feature space dimensions!\n");
    return TCL_ERROR;
  }
  phase = (char) read_int (fp);
  if (phase != cd->phase) {
    ERROR("Stored system is in wrong phase!\n");
    return TCL_ERROR;
  }
  train_parmats = (char) read_int (fp);
  if (train_parmats != cd->train_parmats) {
    ERROR("Stored system has wrong -trainParmats value! Please use 'setTrainParmats' or 'clearTrainParmats' to get into the correct subphase.\n");
    return TCL_ERROR;
  }
  num_cbs = read_int (fp);  /* number of codebooks */

  /* main loop (over the stored codebooks) */
  for (i = 0; i < num_cbs; i++) {
    /* read codebook name */
    if (read_string (fp,temp_str) == 0) {
      ERROR("Error while reading file!\n");
      return TCL_ERROR;
    }
    /* 'disc_flag' is 1 if and only if the whole codebook is discarded */
    disc_flag = 0;
    if ((index = listIndex ((List*) &(cd->list),
			    (ClientData) temp_str,0)) < 0)
      disc_flag = 1;
    else
      cb = &(cd->list.itemA[index]);
    accu_flag = (char) read_int (fp);
    num_dens = read_int (fp);
    if (disc_flag)
      (*nexistN) += num_dens;
    else if (num_dens != cb->num_dens) {
      (*wrongRefNN) += num_dens;
      disc_flag = 1;
    }
    /* We treat the stored accus depending on their accumulation states */
    switch (accu_flag) {
    case ACCUS_EMPTY: {
      if (!disc_flag) (*emptyN) += num_dens;
      break;
    }
    case ACCUS_LOCKED: {
      if (!disc_flag && cb->accu_flag == ACCUS_BUILDING) {
	(*wrongAS) += num_dens;
	disc_flag = 1;
      }
      if (!disc_flag) {
	if (!cb->accu_mean)
	  /* Allocate accus */
	  if (allocate_accus (cb) == TCL_ERROR) {
#ifdef DEBUG
	    ERROR("<= called by\n");
#endif
	    return TCL_ERROR;
	  }
	cb->accu_num += read_int (fp);
        if (read_double_vector_incr (fp,cb->accu_gamma_sum,num_dens)
	    == TCL_ERROR) {
          ERROR("Error while reading file!\n");
          return TCL_ERROR;
        }
      } else {
	read_int (fp);  /* overread 'accu_num' */
        if (discard_double_vector (fp,num_dens) == TCL_ERROR) {
          ERROR("Error while reading file!\n");
          return TCL_ERROR;
        }
      }
      /* loop over the Gaussians of the codebook */
      for (j = 0; j < num_dens; j++) {
	/* 'temp_disc' = 1: Overread the Gaussian entry */
	temp_disc = 1 - cb->active[j];
        num_block = read_int (fp);  /* number of blocks */
	if (num_block >= 0) {
	  /* Stored Gaussian is active */
	  if (!cb->active[j]) (*inactN)++;
	  else {
	    if (cb->parmat[j].mat && train_parmats != 0)
	      dummy = cb->parmat[j].mat->num_block;
	    else
	      dummy = 0;
	    if (!disc_flag && !temp_disc) {
	      if (dummy != num_block) {
		(*structN)++;  /* structural conflict */
		temp_disc = 1;
	      } else
		(*lockedN)++;
	    }
	  }
	  /* loop over the blocks */
	  for (k = 0,size = cd->num_dim; k < num_block; k++) {
	    d = read_int (fp);  /* block size */
	    if (!disc_flag && !temp_disc) {
	      dummy = cb->parmat[j].mat->list[k]->m;
	      if (d != dummy) {
		ERROR("Stored 'accuCovBlock' of codebook '%s' has wrong structure!\n",cb->name);
		return TCL_ERROR;
	      }
	      if (read_double_vector_incr (fp,cb->accu_cov[j].block[k],
					   (d*(d+1))/2) == TCL_ERROR) {
		ERROR("Error while reading file!\n");
		return TCL_ERROR;
	      }
	    } else {
	      if (discard_double_vector (fp,(d*(d+1))/2) == TCL_ERROR) {
		ERROR("Error while reading file!\n");
		return TCL_ERROR;
	      }
	    }
	    size -= d;
	  }
	  if (size > 0) {
	    if (!disc_flag && !temp_disc) {
	      if (read_double_vector_incr (fp,cb->accu_cov[j].diag,size)
		  == TCL_ERROR) {
		ERROR("Error while reading file!\n");
		return TCL_ERROR;
	      }
	    } else if (discard_double_vector (fp,size) == TCL_ERROR) {
	      ERROR("Error while reading file!\n");
	      return TCL_ERROR;
	    }
	  }
	} else if (cb->active[j])
	  (*inactN)++;
      }
      break;
    }
    case ACCUS_BUILDING: {
      if (!disc_flag && cb->accu_flag == ACCUS_LOCKED) {
	(*wrongAS) += num_dens;
	disc_flag = 1;
      }
      if (!disc_flag) {
	if (!cb->accu_mean)
	  /* Allocate accumulators */
	  if (allocate_accus (cb) == TCL_ERROR) {
#ifdef DEBUG
	    ERROR("<= called by\n");
#endif
	    return TCL_ERROR;
	  }
	cb->accu_num += read_int (fp);
        if (read_double_vector_incr (fp,cb->accu_gamma_sum,num_dens)
	    == TCL_ERROR) {
          ERROR("Error while reading file!\n");
          return TCL_ERROR;
        }
        if (read_double_matrix_incr (fp,cb->accu_mean) == TCL_ERROR) {
          ERROR("Error while reading file!\n");
          return TCL_ERROR;
        }
      } else {
	read_int (fp);  /* overread 'accu_num' */
        if (discard_double_vector (fp,num_dens) == TCL_ERROR) {
          ERROR("Error while reading file!\n");
          return TCL_ERROR;
        }
        if (discard_double_matrix (fp,num_dens,cd->num_dim)
            == TCL_ERROR) {
          ERROR("Error while reading file!\n");
          return TCL_ERROR;
        }
      }
      /* loop over the Gaussians of the codebook */
      for (j = 0; j < num_dens; j++) {
	temp_disc = 1 - cb->active[j];
        num_block = read_int (fp);  /* number of blocks */
	if (num_block >= 0) {
	  if (!cb->active[j]) (*inactN)++;
	  else {
	    if (cb->parmat[j].mat && train_parmats != 0)
	      dummy = cb->parmat[j].mat->num_block;
	    else
	      dummy = 0;
	    if (!disc_flag && !temp_disc) {
	      if (dummy != num_block) {
		(*structN)++;
		temp_disc = 1;
	      } else
		(*buildN)++;
	    }
	  }
	  /* loop over the blocks */
	  for (k = 0,size = cd->num_dim; k < num_block; k++) {
	    d = read_int (fp);  /* block size */
	    if (!disc_flag && !temp_disc) {
	      dummy = cb->parmat[j].mat->list[k]->m;
	      if (d != dummy) {
		ERROR("Stored 'accuCovBlock' of codebook '%s' has wrong structure!\n",
		      cb->name);
		return TCL_ERROR;
	      }
	      if (read_double_vector_incr (fp,cb->accu_cov[j].block[k],
					   (d*(d+1))/2) == TCL_ERROR) {
		ERROR("Error while reading file!\n");
		return TCL_ERROR;
	      }
	    } else {
	      if (discard_double_vector (fp,(d*(d+1))/2) == TCL_ERROR) {
		ERROR("Error while reading file!\n");
		return TCL_ERROR;
	      }
	    }
	    size -= d;
	  }
	  if (size > 0) {
	    if (!disc_flag && !temp_disc) {
	      if (read_double_vector_incr (fp,cb->accu_cov[j].diag,size)
		  == TCL_ERROR) {
		ERROR("Error while reading file!\n");
		return TCL_ERROR;
	      }
	    } else if (discard_double_vector (fp,size) == TCL_ERROR) {
	      ERROR("Error while reading file!\n");
	      return TCL_ERROR;
	    }
	  }
	} else if (cb->active[j])
	  (*inactN)++;
      }
      break;
    }
    }
    if (!disc_flag && cb->accu_flag == ACCUS_EMPTY)
      cb->accu_flag = accu_flag;
  }
  fileClose (fname,fp);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewsetLoadAccus_dep (DEBUG)
   Phase: working.
   Understands accu file format version 0 (initial version)
   CAUTION: System-dependant format for doubles!!
   ------------------------------------------------------------------- */
int cbnewsetLoadAccus_dep (CBNewSet* cd,char* fname,int* emptyN,int* lockedN,
			   int* buildN,int* nexistN,int* wrongRefNN,
			   int* wrongAS,int* structN,int* inactN)
{
  register int i,j,k;
  int num_cbs,num_dens,dummy,num_block,d,index,size;
  char phase,accu_flag,train_parmats;
  char temp_str[256];
  FILE* fp;
  CBNew* cb = NULL;
  char disc_flag,temp_disc;

#ifdef DEBUG
  ASSERT_CBSET(cd,1);
  assert (cd->phase == WORKING_PHASE);
  assert (fname);
  assert (emptyN);
  assert (lockedN);
  assert (buildN);
  assert (nexistN);
  assert (wrongRefNN);
  assert (structN);
  assert (inactN);
#endif

  /* File handling and initialization */
  *emptyN = *lockedN = *buildN = *nexistN = *wrongRefNN = *structN =
    *inactN = *wrongAS = 0;
  if (!(fp = fileOpen (fname,"r"))) {
    ERROR("Cannot open file '%s'!\n",fname);
    return TCL_ERROR;
  }

  if ((read_string (fp,temp_str) == 0) || strcmp (temp_str,"CBACCUMU")) {
    ERROR("'%s' is not a cbset accumulator file!\n",fname);
    return TCL_ERROR;
  }
  dummy = read_int (fp);  /* version number */
  if (dummy != 0) {
    ERROR("The file format version no. %d cannot be handeled by this revision. Please get an upgrade!\n",dummy);
    return 0;
  }
  dummy = read_int (fp);
  if (dummy != cd->num_dim) {
    ERROR("Stored system has wrong number of feature space dimensions!\n");
    return TCL_ERROR;
  }
  phase = (char) read_int (fp);
  if (phase != cd->phase) {
    ERROR("Stored system is in wrong phase!\n");
    return TCL_ERROR;
  }
  train_parmats = (char) read_int (fp);
  if (train_parmats != cd->train_parmats) {
    ERROR("Stored system has wrong -trainParmats value! Please use 'setTrainParmats' or 'clearTrainParmats' to get into the correct subphase.\n");
    return TCL_ERROR;
  }
  num_cbs = read_int (fp);  /* number of codebooks */

  /* main loop (over the stored codebooks) */
  for (i = 0; i < num_cbs; i++) {
    /* read codebook name */
    if (read_string (fp,temp_str) == 0) {
      ERROR("Error while reading file!\n");
      return TCL_ERROR;
    }
    /* 'disc_flag' is 1 if and only if the whole codebook is discarded */
    disc_flag = 0;
    if ((index = listIndex ((List*) &(cd->list),
			    (ClientData) temp_str,0)) < 0)
      disc_flag = 1;
    else
      cb = &(cd->list.itemA[index]);
    accu_flag = (char) read_int (fp);
    num_dens = read_int (fp);
    if (disc_flag)
      (*nexistN) += num_dens;
    else if (num_dens != cb->num_dens) {
      (*wrongRefNN) += num_dens;
      disc_flag = 1;
    }
    /* We treat the stored accus depending to their accumulation states */
    switch (accu_flag) {
    case ACCUS_EMPTY: {
      if (!disc_flag) (*emptyN) += num_dens;
      break;
    }
    case ACCUS_LOCKED: {
      if (!disc_flag && cb->accu_flag == ACCUS_BUILDING) {
	(*wrongAS) += num_dens;
	disc_flag = 1;
      }
      if (!disc_flag) {
	if (!cb->accu_mean)
	  /* Allocate accus */
	  if (allocate_accus (cb) == TCL_ERROR) {
#ifdef DEBUG
	    ERROR("<= called by\n");
#endif
	    return TCL_ERROR;
	  }
	cb->accu_num += read_int (fp);
        if (read_double_vector_incr_dep (fp,cb->accu_gamma_sum,num_dens)
	    == TCL_ERROR) {
          ERROR("Error while reading file!\n");
          return TCL_ERROR;
        }
      } else {
	read_int (fp);  /* overread 'accu_num' */
        if (discard_double_vector_dep (fp,num_dens) == TCL_ERROR) {
          ERROR("Error while reading file!\n");
          return TCL_ERROR;
        }
      }
      /* loop over the Gaussians of the codebook */
      for (j = 0; j < num_dens; j++) {
	/* 'temp_disc' = 1: Overread the Gaussian entry */
	temp_disc = 1 - cb->active[j];
        num_block = read_int (fp);  /* number of blocks */
	if (num_block >= 0) {
	  /* Stored Gaussian is active */
	  if (!cb->active[j]) (*inactN)++;
	  else {
	    if (cb->parmat[j].mat && train_parmats != 0)
	      dummy = cb->parmat[j].mat->num_block;
	    else
	      dummy = 0;
	    if (!disc_flag && !temp_disc) {
	      if (dummy != num_block) {
		(*structN)++;  /* structural conflict */
		temp_disc = 1;
	      } else
		(*lockedN)++;
	    }
	  }
	  /* loop over the blocks */
	  for (k = 0,size = cd->num_dim; k < num_block; k++) {
	    d = read_int (fp);  /* block size */
	    if (!disc_flag && !temp_disc) {
	      dummy = cb->parmat[j].mat->list[k]->m;
	      if (d != dummy) {
		ERROR("Stored 'accuCovBlock' of codebook '%s' has wrong structure!\n",
		      cb->name);
		return TCL_ERROR;
	      }
	      if (read_double_vector_incr_dep (fp,cb->accu_cov[j].block[k],
					   (d*(d+1))/2) == TCL_ERROR) {
		ERROR("Error while reading file!\n");
		return TCL_ERROR;
	      }
	    } else {
	      if (discard_double_vector_dep (fp,(d*(d+1))/2) == TCL_ERROR) {
		ERROR("Error while reading file!\n");
		return TCL_ERROR;
	      }
	    }
	    size -= d;
	  }
	  if (size > 0) {
	    if (!disc_flag && !temp_disc) {
	      if (read_double_vector_incr_dep (fp,cb->accu_cov[j].diag,size)
		  == TCL_ERROR) {
		ERROR("Error while reading file!\n");
		return TCL_ERROR;
	      }
	    } else if (discard_double_vector_dep (fp,size) == TCL_ERROR) {
	      ERROR("Error while reading file!\n");
	      return TCL_ERROR;
	    }
	  }
	} else if (cb->active[j])
	  (*inactN)++;
      }
      break;
    }
    case ACCUS_BUILDING: {
      if (!disc_flag && cb->accu_flag == ACCUS_LOCKED) {
	(*wrongAS) += num_dens;
	disc_flag = 1;
      }
      if (!disc_flag) {
	if (!cb->accu_mean)
	  /* Allocate accumulators */
	  if (allocate_accus (cb) == TCL_ERROR) {
#ifdef DEBUG
	    ERROR("<= called by\n");
#endif
	    return TCL_ERROR;
	  }
	cb->accu_num += read_int (fp);
        if (read_double_vector_incr_dep (fp,cb->accu_gamma_sum,num_dens)
	    == TCL_ERROR) {
          ERROR("Error while reading file!\n");
          return TCL_ERROR;
        }
        if (read_double_matrix_incr_dep (fp,cb->accu_mean) == TCL_ERROR) {
          ERROR("Error while reading file!\n");
          return TCL_ERROR;
        }
      } else {
	read_int (fp);  /* overread 'accu_num' */
        if (discard_double_vector_dep (fp,num_dens) == TCL_ERROR) {
          ERROR("Error while reading file!\n");
          return TCL_ERROR;
        }
        if (discard_double_matrix_dep (fp,num_dens,cd->num_dim)
            == TCL_ERROR) {
          ERROR("Error while reading file!\n");
          return TCL_ERROR;
        }
      }
      /* loop over the Gaussians of the codebook */
      for (j = 0; j < num_dens; j++) {
	temp_disc = 1 - cb->active[j];
        num_block = read_int (fp);  /* number of blocks */
	if (num_block >= 0) {
	  if (!cb->active[j]) (*inactN)++;
	  else {
	    if (cb->parmat[j].mat && train_parmats != 0)
	      dummy = cb->parmat[j].mat->num_block;
	    else
	      dummy = 0;
	    if (!disc_flag && !temp_disc) {
	      if (dummy != num_block) {
		(*structN)++;
		temp_disc = 1;
	      } else
		(*buildN)++;
	    }
	  }
	  /* loop over the blocks */
	  for (k = 0,size = cd->num_dim; k < num_block; k++) {
	    d = read_int (fp);  /* block size */
	    if (!disc_flag && !temp_disc) {
	      dummy = cb->parmat[j].mat->list[k]->m;
	      if (d != dummy) {
		ERROR("Stored 'accuCovBlock' of codebook '%s' has wrong structure!\n",
		      cb->name);
		return TCL_ERROR;
	      }
	      if (read_double_vector_incr_dep (fp,cb->accu_cov[j].block[k],
					   (d*(d+1))/2) == TCL_ERROR) {
		ERROR("Error while reading file!\n");
		return TCL_ERROR;
	      }
	    } else {
	      if (discard_double_vector_dep (fp,(d*(d+1))/2) == TCL_ERROR) {
		ERROR("Error while reading file!\n");
		return TCL_ERROR;
	      }
	    }
	    size -= d;
	  }
	  if (size > 0) {
	    if (!disc_flag && !temp_disc) {
	      if (read_double_vector_incr_dep (fp,cb->accu_cov[j].diag,size)
		  == TCL_ERROR) {
		ERROR("Error while reading file!\n");
		return TCL_ERROR;
	      }
	    } else if (discard_double_vector_dep (fp,size) == TCL_ERROR) {
	      ERROR("Error while reading file!\n");
	      return TCL_ERROR;
	    }
	  }
	} else if (cb->active[j])
	  (*inactN)++;
      }
      break;
    }
    }
    if (!disc_flag && cb->accu_flag == ACCUS_EMPTY)
      cb->accu_flag = accu_flag;
  }
  fileClose (fname,fp);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewsetSaveAccus
   Phase: working.
   Uses accu format file version 0 (initial version)
   ------------------------------------------------------------------- */
int cbnewsetSaveAccus (CBNewSet* cd,char* fname,int* emptyN,int* lockedN,
		       int* buildN)
{
  register int i,j,k,d;
  FILE* fp;
  CBNew* cb;

#ifdef DEBUG
  ASSERT_CBSET(cd,1);
  assert (cd->phase == WORKING_PHASE);
  assert (fname);
#endif

  *emptyN = *lockedN = *buildN = 0;
  if (!(fp = fileOpen (fname,"w"))) {
    ERROR("Cannot create file '%s'!\n",fname);
    return TCL_ERROR;
  }
  write_string (fp,"CBACCUMU");
  write_int (fp,0);  /* version number */
  write_int (fp,cd->num_dim);
  write_int (fp,(int) cd->phase);
  write_int (fp,(int) cd->train_parmats);
  write_int (fp,cd->list.itemN);
  for (i = 0; i < cd->list.itemN; i++) {
    cb = &(cd->list.itemA[i]);
    write_string (fp,cb->name);
    write_int (fp,(int) cb->accu_flag);
    write_int (fp,cb->num_dens);
    if (cb->accu_flag != ACCUS_EMPTY) {
      write_int (fp,cb->accu_num);
      write_double_vector (fp,cb->accu_gamma_sum,cb->num_dens);
      if (cb->accu_flag == ACCUS_BUILDING) {
        write_double_matrix (fp,cb->accu_mean);
	(*buildN)++;
      } else
	(*lockedN)++;
      for (j = 0; j < cb->num_dens; j++)
	if (cb->active[j]) {
	  if (cb->parmat[j].mat && cd->train_parmats != 0) {
	    write_int (fp,cb->parmat[j].mat->num_block);
	    for (k = 0; k < cb->parmat[j].mat->num_block; k++) {
	      d = cb->parmat[j].mat->list[k]->m;
	      write_int (fp,d);
	      write_double_vector (fp,cb->accu_cov[j].block[k],
				   (d*(d+1))/2);
	    }
	    d = cd->num_dim - cb->parmat[j].mat->num_rel;
	  } else {
	    write_int (fp,0); /* no blocks */
	    d = cd->num_dim;
	  }
	  if (d > 0)
	    write_double_vector (fp,cb->accu_cov[j].diag,d);
	} else
	  write_int (fp,-1);  /* inactive Gaussian */
    } else
      (*emptyN)++;
  }
  fileClose (fname,fp);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewsetSaveAccus_dep (DEBUG)
   Phase: working.
   Uses accu format file version 0 (initial version)
   CAUTION: System-dependant format for doubles!!
   ------------------------------------------------------------------- */
int cbnewsetSaveAccus_dep (CBNewSet* cd,char* fname,int* emptyN,int* lockedN,
			   int* buildN)
{
  register int i,j,k,d;
  FILE* fp;
  CBNew* cb;

#ifdef DEBUG
  ASSERT_CBSET(cd,1);
  assert (cd->phase == WORKING_PHASE);
  assert (fname);
#endif

  *emptyN = *lockedN = *buildN = 0;
  if (!(fp = fileOpen (fname,"w"))) {
    ERROR("Cannot create file '%s'!\n",fname);
    return TCL_ERROR;
  }
  write_string (fp,"CBACCUMU");
  write_int (fp,0);  /* version number */
  write_int (fp,cd->num_dim);
  write_int (fp,(int) cd->phase);
  write_int (fp,(int) cd->train_parmats);
  write_int (fp,cd->list.itemN);
  for (i = 0; i < cd->list.itemN; i++) {
    cb = &(cd->list.itemA[i]);
    write_string (fp,cb->name);
    write_int (fp,(int) cb->accu_flag);
    write_int (fp,cb->num_dens);
    if (cb->accu_flag != ACCUS_EMPTY) {
      write_int (fp,cb->accu_num);
      write_double_vector_dep (fp,cb->accu_gamma_sum,cb->num_dens);
      if (cb->accu_flag == ACCUS_BUILDING) {
        write_double_matrix_dep (fp,cb->accu_mean);
	(*buildN)++;
      } else
	(*lockedN)++;
      for (j = 0; j < cb->num_dens; j++)
	if (cb->active[j]) {
	  if (cb->parmat[j].mat && cd->train_parmats != 0) {
	    write_int (fp,cb->parmat[j].mat->num_block);
	    for (k = 0; k < cb->parmat[j].mat->num_block; k++) {
	      d = cb->parmat[j].mat->list[k]->m;
	      write_int (fp,d);
	      write_double_vector_dep (fp,cb->accu_cov[j].block[k],
				   (d*(d+1))/2);
	    }
	    d = cd->num_dim - cb->parmat[j].mat->num_rel;
	  } else {
	    write_int (fp,0); /* no blocks */
	    d = cd->num_dim;
	  }
	  if (d > 0)
	    write_double_vector_dep (fp,cb->accu_cov[j].diag,d);
	} else
	  write_int (fp,-1);  /* inactive Gaussian */
    } else
      (*emptyN)++;
  }
  fileClose (fname,fp);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewsetScoreArray
   The score of a codebook is approx. by the lowest Gaussian score.
   Phase: testing.
   CAUTION: We assume that all codebooks use the same feature space!
   ------------------------------------------------------------------- */
int cbnewsetScoreArray (CBNewSet* cd,int* index,float* score,int num_ind,
			int frame_ind)
{
  register int i,j,k;
  float* sample,*tr_samp;
  double scale,sum,tempd;
  float temp,tsum;
  CBNew* cb;

#ifdef DEBUG
  ASSERT_CBSET(cd,0);
  assert (index);
  assert (score);
  assert (num_ind > 0);
  assert (frame_ind >= 0);
#endif

  if (cd->phase != TESTING_PHASE) {
    ERROR("Codebook set must be in testing phase!\n");
    return TCL_ERROR;
  }
  for (i = 0; i < num_ind; i++)
    if (index[i] < 0 || index[i] >= cd->list.itemN) {
      ERROR("Codebook indices out of range!\n");
      return TCL_ERROR;
    }

  /* Clear the 'ts_valid' flag for all parmat's */
  for (i = 0; i < cd->parmat_set->list.itemN; i++)
    cd->parmat_set->list.itemA[i].ts_valid = 0;

  /* Locate sample */
  cb = &(cd->list.itemA[index[0]]);
  if (!(sample = fesFrame (cd->feat_set,cb->feat,frame_ind))) {
    ERROR("Feature '%s' has no frame %d!\n",
	  fesGetName (cd->feat_set,cb->feat),frame_ind);
    return TCL_ERROR;
  }

  /* Loop over models */
  for (i = 0; i < num_ind; i++) {
    cb = &(cd->list.itemA[index[i]]);
    /* Calc. internal vars if necessary */
    if (cb->internal_flag == 0) {
      calc_internals (cb);
      cb->internal_flag = 1;
    }  
    /* Loop over the Gaussians */
    for (k = 0; !cb->active[k]; k++);
    if (cb->parmat[k].mat && cb->parmat[k].mat->ts_valid == 1)
      tr_samp = cb->parmat[k].mat->trans_sample;
    else
      transform_sample (cb,sample,k,&tr_samp);
    for (j = 0,tsum = 0.0; j < cd->num_dim; j++) {
      temp = tr_samp[j] - cb->trans_mean->matPA[k][j];
      tsum += (temp * temp / cb->diag->matPA[k][j]);
    }
    scale = -2.00 * cb->lh_factor[k] + (double) tsum;
    for (k++; k < cb->num_dens; k++)
      if (cb->active[k]) {
	if (cb->parmat[k].mat && cb->parmat[k].mat->ts_valid == 1)
	  tr_samp = cb->parmat[k].mat->trans_sample;
	else
	  transform_sample (cb,sample,k,&tr_samp);
	sum = -2.00 * cb->lh_factor[k];
	for (j = 0; sum < scale && j < cd->num_dim; j++) {
	  tempd = (double) (tr_samp[j] - cb->trans_mean->matPA[k][j]);
	  sum += (tempd * tempd / (double) cb->diag->matPA[k][j]);
	}
	if (sum < scale) scale = sum;
      }
    score[i] = (float) (0.50 * scale);

    /*
    INFO("%s %d %4.4f\n",cb->name,frame_ind,score[i]);
    for (k = 0; k < cb->num_dens; k++)
      INFO("   %d %4.4f\n",k, -2.0 * cb->lh_factor[k]);
    */

  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewsetScoreArrayExact
   Same as 'cbnewsetScoreArray', but the full scores are calc.
   Phase: testing.
   ------------------------------------------------------------------- */
int cbnewsetScoreArrayExact (CBNewSet* cd,int* index,float* score,int num_ind,
			     int frame_ind)
{
  register int i,j,k;
  float* sample,*tr_samp;
  double scale,sum;
  float temp,tsum;
  CBNew* cb;

#ifdef DEBUG
  ASSERT_CBSET(cd,0);
  assert (index);
  assert (score);
  assert (num_ind > 0);
  assert (frame_ind >= 0);
#endif

  if (cd->phase != TESTING_PHASE) {
    ERROR("Codebook set must be in testing phase!\n");
    return TCL_ERROR;
  }
  for (i = 0; i < num_ind; i++)
    if (index[i] < 0 || index[i] >= cd->list.itemN) {
      ERROR("Codebook indices out of range!\n");
      return TCL_ERROR;
    }

  /* Clear the 'ts_valid' flag for all parmat's */
  for (i = 0; i < cd->parmat_set->list.itemN; i++)
    cd->parmat_set->list.itemA[i].ts_valid = 0;

  /* Loop over models */
  for (i = 0; i < num_ind; i++) {
    cb = &(cd->list.itemA[index[i]]);
    if (!(sample = fesFrame (cd->feat_set,cb->feat,frame_ind))) {
      ERROR("Feature '%s' has no frame %d!\n",
	    fesGetName (cd->feat_set,cb->feat),frame_ind);
      return TCL_ERROR;
    }
    /* Calc. internal vars if necessary */
    if (cb->internal_flag == 0) {
      calc_internals (cb);
      cb->internal_flag = 1;
    }
    /* Loop over the gaussians */
    for (k = 0; !cb->active[k]; k++);
    transform_sample (cb,sample,k,&tr_samp);
    for (j = 0,tsum = 0.0; j < cd->num_dim; j++) {
      temp = tr_samp[j] - cb->trans_mean->matPA[k][j];
      tsum += (temp * temp / cb->diag->matPA[k][j]);
    }
    cb->temp_gamma[k] = scale = cb->lh_factor[k] - 0.50 * (double) tsum;
    for (k++; k < cb->num_dens; k++)
      if (cb->active[k]) {
	transform_sample (cb,sample,k,&tr_samp);
	for (j = 0,tsum = 0.0; j < cd->num_dim; j++) {
	  temp = tr_samp[j] - cb->trans_mean->matPA[k][j];
	  tsum += (temp * temp / cb->diag->matPA[k][j]);
	}
	cb->temp_gamma[k] = sum = cb->lh_factor[k] - 0.50 * (double) tsum;
	if (sum > scale) scale = sum;
      }
    sum = 0.00;
    for (k = 0; k < cb->num_dens; k++)
      if (cb->active[k])
	sum += exp (cb->temp_gamma[k] - scale);
    /* sum == 0 ABFANGEN !!!!!!! */
    score[i] = (float) -(log (sum) + scale);
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewsetAccu
   ------------------------------------------------------------------- */
int cbnewsetAccu (CBNewSet* cd,int cbX,int frameX,float factor)
{
  CBNew* cb;
  float* sample;
  double loglh;

  if (cd->phase != WORKING_PHASE) {
    ERROR("Codebook set must be in working phase!\n");
    return 0.00;
  }
  if (cbX < 0 || cbX >= cd->list.itemN) {
    ERROR("Codebook %d doesn't exist!\n",cbX);
    return 0.00;
  }
  cb = &(cd->list.itemA[cbX]);
  if (cb->accu_flag == ACCUS_LOCKED) {
    ERROR("Codebook has locked accus!\n");
    return 0.00;
  }
  if (!(sample = fesFrame (cd->feat_set,cb->feat,frameX))) {
    ERROR("Feature '%s' has no frame %d!\n",
	  fesGetName (cd->feat_set,cb->feat),frameX);
    return 0.00;
  }
  return cbnewAccu (cb,sample,(double) factor,&loglh);
}

/* -------------------------------------------------------------------
   cbnewsetUpdate
   Phase: working (checked).
   ------------------------------------------------------------------- */
int cbnewsetUpdate (CBNewSet* cd)
{
  register int i;
  int okN = 0,errorN = 0,wrongN = 0,not_mean,not_diag,nm_sum = 0,nd_sum = 0;

  if (cd->phase != WORKING_PHASE) {
    ERROR("Codebook set must be in working phase!\n");
    return TCL_ERROR;
  }
  for (i = 0; i < cd->list.itemN; i++)
    if (cd->list.itemA[i].accu_flag == ACCUS_BUILDING) {
      if (cbnewUpdate (&(cd->list.itemA[i]),&not_mean,&not_diag) == TCL_ERROR)
	errorN++;
      else {
	okN++;
	nm_sum += not_mean;
	nd_sum += not_diag;
      }
    } else wrongN++;
  INFO("%d codebooks updated\n",okN);
  if (wrongN > 0)
    INFO("%d codebooks discarded because of wrong accu state\n",wrongN);
  if (errorN > 0)
    INFO("%d codebooks discarded because of errors\n",errorN);
  if (nm_sum > 0)
    INFO("%d Gaussians had too few samples to update mean/diag\n",nm_sum);
  if (nd_sum > 0)
    INFO("%d Gaussians had enough samples to update mean, but too few samples to update diag\n",nd_sum);

  return (errorN > 0) ? TCL_ERROR : TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewsetIndex
   Phase: all.
   ------------------------------------------------------------------- */
int cbnewsetIndex (CBNewSet* cd,char *name)
{
  return listIndex ((List*) &(cd->list),(ClientData) name,0);
}

/* -------------------------------------------------------------------
   cbnewsetName
   Phase: all.
   ------------------------------------------------------------------- */
char* cbnewsetName (CBNewSet* cd,int index)
{
  if (index < 0 || index >= cd->list.itemN) return "(null)";
  else return cd->list.itemA[index].name;
}

/* -------------------------------------------------------------------
   cbnewsetDist
   Phase: all.
   Just a dummy for ModelSetDistFnc. Will always return 0!
   ------------------------------------------------------------------- */
int cbnewsetDist (CBNewSet* cd,ModelArray* maP,ModelArray* maQ,
		  ModelArray* maR,float* distP)
{
  *distP = 0.0;
  return TCL_OK;
}

/* --- Janus itf functions --- */

/* -------------------------------------------------------------------
   cbnewsetCreateItf
   ------------------------------------------------------------------- */
ClientData cbnewsetCreateItf (ClientData cd,int argc,char *argv[])
{
  char* name = NULL;
  CBNewParMatrixSet* parmat_set = 0;
  FeatureSet* feat_set = 0;
  int num_dim = 1,wrongFeatN,noPMN;
  char* str = NULL;
  CBNewSet* ret;

  if (itfParseArgv (argv[0],&argc,argv,1,
      "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the set",
		    "<parmatSet>", ARGV_OBJECT,NULL,     &parmat_set,
		    "CBNewParMatrixSet","parameter matrix set",
		    "<featureSet>",ARGV_OBJECT,NULL,     &feat_set,
		    "FeatureSet","feature set",
		    "<par2>",      ARGV_CUSTOM,getString,&str,cd,
                    "feature space dimensions <dimN> OR @<fname>: name of structure file to load",
		    NULL) != TCL_OK) return 0;

  if (parmat_set->cbnew_set) {
    ERROR("Parameter matrix set '%s' already assoc. with codebook set '%s'!\n",
	  parmat_set->name,parmat_set->cbnew_set->name);
    return 0;
  }
  if (str[0] == '@') {
    if ( (ret = cbnewsetLoad (name,str+1,parmat_set,feat_set,&wrongFeatN,
			    &noPMN)) ) {
      INFO("%d codebooks loaded\n",ret->list.itemN - wrongFeatN);
      if (wrongFeatN > 0)
	INFO("%d codebooks discarded because of wrong features\n",wrongFeatN);
      if (noPMN > 0)
	INFO("%d of stored Gaussians were linked to non-exist. parmats\n",
	     noPMN);
    }
  } else if (sscanf (str,"%d",&num_dim) == 1) {
    if (num_dim != parmat_set->num_dim) {
      ERROR("<dimN> of codebook set and parameter matrix set must be the same!\n");
      ret = 0;
    } else
      ret = cbnewsetCreate (name,num_dim,parmat_set,feat_set);
  } else {
    ERROR("Wrong parameters!\n");
    ret = 0;
  }

  return (ClientData) ret;
}

/* -------------------------------------------------------------------
   cbnewsetDestroyItf
   Phase: all.
   ------------------------------------------------------------------- */
int cbnewsetDestroyItf (ClientData cd)
{
  CBNewSet* obj = (CBNewSet*) cd;
  if (obj->useN > 0) {
    ERROR("There exist %d references to this object!\n",obj->useN);
    return TCL_ERROR;
  }

  return cbnewsetDestroy (obj);
}

/* -------------------------------------------------------------------
   cbnewsetPutsItf
   Phase: all.
   ------------------------------------------------------------------- */
int cbnewsetPutsItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;

  return listPutsItf ((ClientData) &(obj->list),argc,argv);
}

/* -------------------------------------------------------------------
   cbnewsetConfigureItf
   Phase: all.
   ------------------------------------------------------------------- */
int cbnewsetConfigureItf (ClientData cd,char *var,char *val)
{
  CBNewSet* obj = (CBNewSet*) cd;
  char dummy[5];
  char* phaseStr = dummy;

  if (!var) {
    ITFCFG(cbnewsetConfigureItf,cd,"name");
    ITFCFG(cbnewsetConfigureItf,cd,"useN");
    ITFCFG(cbnewsetConfigureItf,cd,"dimN");
    ITFCFG(cbnewsetConfigureItf,cd,"parmatSet");
    ITFCFG(cbnewsetConfigureItf,cd,"featureSet");
    ITFCFG(cbnewsetConfigureItf,cd,"phase");
    ITFCFG(cbnewsetConfigureItf,cd,"trainParmats");
    return listConfigureItf ((ClientData) &(obj->list),var,val);
  }
  ITFCFG_CharPtr (var,val,"name",        obj->name,            1);
  ITFCFG_Int     (var,val,"useN",        obj->useN,            1);
  ITFCFG_Int     (var,val,"dimN",        obj->num_dim,         1);
  ITFCFG_Object  (var,val,"parmatSet",   obj->parmat_set,
		  name,CBNewParMatrixSet,                      1);
  ITFCFG_Object  (var,val,"featureSet",  obj->feat_set,
		  name,FeatureSet,                             1);
  if (obj->phase == CONSTRUCTION_PHASE)
    strcpy (phaseStr,"cons");
  else if (obj->phase == WORKING_PHASE)
    strcpy (phaseStr,"work");
  else
    strcpy (phaseStr,"test");
  ITFCFG_CharPtr (var,val,"phase",       phaseStr,             1);
  ITFCFG_Int     (var,val,"trainParmats",obj->train_parmats,   1);

  return listConfigureItf ((ClientData) &(obj->list),var,val);
}

/* -------------------------------------------------------------------
   cbnewsetAccessItf
   Phase: all.
   ------------------------------------------------------------------- */
ClientData cbnewsetAccessItf (ClientData cd,char *name,TypeInfo **ti)
{
  CBNewSet* obj = (CBNewSet*) cd;
  int l;

  if (*name == '.') {
    if (name[1] == 0) {
      if (obj->list.itemN)
	itfAppendResult ("item(0..%d) ",obj->list.itemN-1);
      *ti = 0;
      return 0;
    } else if (sscanf (name+1,"item(%d)",&l) == 1) {
      if (l >= 0 && l < obj->list.itemN) {
	*ti = itfGetType ("CBNew");
	return (ClientData) &(obj->list.itemA[l]);
      } else {
 	*ti = 0;
	return 0;
      }
    }
  }

  return (ClientData) listAccessItf ((ClientData) &(obj->list),name,ti);
}

/* -------------------------------------------------------------------
   cbnewsetIndexItf
   Phase: all.
   ------------------------------------------------------------------- */
int cbnewsetIndexItf (ClientData cd,int argc,char *argv[])
{
   CBNewSet* obj = (CBNewSet*) cd;

  return listName2IndexItf ((ClientData) &(obj->list),argc,argv);
}

/* -------------------------------------------------------------------
   cbnewsetNameItf
   Phase: all.
   ------------------------------------------------------------------- */
int cbnewsetNameItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;

  return listIndex2NameItf ((ClientData) &(obj->list),argc,argv);
}

/* -------------------------------------------------------------------
   cbnewsetAddItf
   Phase: construction (checked).
   ------------------------------------------------------------------- */
int cbnewsetAddItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;
  int ac = argc-1;
  char* name = NULL, *feat = NULL;
  int num_dens = 1,feat_ind,index;
  CBNew* hp;

  if (obj->phase != CONSTRUCTION_PHASE) {
    ERROR("Object must be in construction phase!\n");
    return TCL_ERROR;
  }
  if (itfParseArgv (argv[0],&ac,argv+1,1,
		    "<cbName>",  ARGV_STRING,NULL,&name,    NULL,
		    "name for codebook",
		    "<featName>",ARGV_STRING,NULL,&feat,    NULL,
		    "name of feature to use",
		    "<refN>",    ARGV_INT,   NULL,&num_dens,NULL,
		    "number of densities",
		    NULL) != TCL_OK) return TCL_ERROR;

  if (num_dens <= 0) {
    ERROR("<refN> must be positive!\n");
    return TCL_ERROR;
  }
  if (listIndex ((List*) &(obj->list),(ClientData) name,0) >= 0) {
    ERROR("'%s' already exists!\n",name);
    return TCL_ERROR;
  }
  if ((feat_ind = fesIndex (obj->feat_set,feat,FE_FMATRIX,
			    obj->num_dim,1)) < 0) {
    ERROR("Feature '%s' has wrong configuration or cannot be created!\n",
	  feat);
    return TCL_ERROR;
  }
  MSGCLEAR(NULL);
  index = listNewItem ((List*) &(obj->list),(ClientData) name);
  hp = &(obj->list.itemA[index]);
  if (cbnewInit (hp,name,num_dens,obj,feat_ind) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    listDelete ((List*) &(obj->list),(ClientData) name);
    obj->feat_set->featA[feat_ind].useN--;  /* Decrease 'useN' of feature */
    return TCL_ERROR;
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewsetLinkItf
   Phase: construction (checked).
   ------------------------------------------------------------------- */
int cbnewsetLinkItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;
  int ac = argc-1,cbi;
  char* pm_name = NULL, *str = NULL;
  CBNewParMatrix* pmat;
  CBNew* cb;
  int index,ret = TCL_OK;
  char all_flag = 0;

  if (obj->phase != CONSTRUCTION_PHASE) {
    ERROR("Object must be in construction phase!\n");
    return TCL_ERROR;
  }
  if (itfParseArgv (argv[0],&ac,argv+1,1,
		    "<parmatName>",ARGV_STRING,NULL,&pm_name,NULL,
		    "name of parameter matrix",
		    "<cbIndex>",    ARGV_INT,  NULL,&cbi,     NULL,
		    "index of codebook",
		    "<refIndex>",  ARGV_STRING,NULL,&str,    NULL,
		    "reference index (or 'allFree' / 'all')",
		    NULL) != TCL_OK) return TCL_ERROR;

  if ((index = listIndex ((List*) &(obj->parmat_set->list),pm_name,0)) < 0) {
    ERROR("Parameter matrix '%s' doesn't exist!\n",pm_name);
    return TCL_ERROR;
  }
  pmat = &(obj->parmat_set->list.itemA[index]);
  if (cbi < 0 || cbi >= obj->list.itemN) {
    ERROR("Codebook %d doesn't exist!\n",cbi);
    return TCL_ERROR;
  }
  cb = &(obj->list.itemA[cbi]);
  if (!strcmp (str,"allFree")) all_flag = 1;
  else if (!strcmp (str,"all")) all_flag = 2;
  if (all_flag) {
    for (index = 0; index < cb->num_dens; index++)
      if (cb->active[index] && (all_flag == 2 || !cb->parmat[index].mat))
	if (cbnewLink (cb,pmat,index) == TCL_ERROR)
	  ret = TCL_ERROR;
    if (ret == TCL_ERROR)
      ERROR("Error while linking!\n");
  } else if (sscanf (str,"%d",&index) == 1 && index >= 0 &&
	     index < cb->num_dens) {
    if (!cb->active[index]) {
      ERROR("Gaussian %d of codebook '%s' has been deactivated!\n",index,
	    cb->name);
      return TCL_ERROR;
    }
    ret = cbnewLink (cb,pmat,index);
  } else {
    ERROR("<refIndex> invalid!\n");
    ret = TCL_ERROR;
  }

  return ret;
}

/* -------------------------------------------------------------------
   cbnewsetUnlinkItf
   Phase: construction (checked).
   ------------------------------------------------------------------- */
int cbnewsetUnlinkItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;
  int ac = argc-1,index = -1,ret = TCL_OK,cbi = -1;
  CBNew* cb;

  if (obj->phase != CONSTRUCTION_PHASE) {
    ERROR("Object must be in construction phase!\n");
    return TCL_ERROR;
  }
  if (itfParseArgv (argv[0],&ac,argv+1,1,
		    "<cbIndex>", ARGV_INT,NULL,&cbi,  NULL,
		    "index of codebook",
		    "-refIndex",ARGV_INT,NULL,&index,NULL,
		    "index of gaussian to unlink (def: all)",
		    NULL) != TCL_OK) return TCL_ERROR;

  if (cbi < 0 || cbi >= obj->list.itemN) {
    ERROR("Invalid codebook index!\n");
    return TCL_ERROR;
  }
  cb = &(obj->list.itemA[cbi]);
  if (index < -1 || index >= cb->num_dens) {
    ERROR("Invalid reference index!\n");
    return TCL_ERROR;
  }
  if (index == -1) {
    /* Unlink all gaussians of codebook */
    for (index = 0; index < cb->num_dens; index++)
      if (cbnewUnlink (cb,index) == TCL_ERROR) ret = TCL_ERROR;
    if (ret == TCL_ERROR)
      ERROR("Error recorded while unlinking!\n");
  } else
    ret = cbnewUnlink (cb,index);

  return ret;
}

/* -------------------------------------------------------------------
   cbnewsetPhaseItf
   ------------------------------------------------------------------- */
int cbnewsetPhaseItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;
  char* ph_name = NULL;
  int ac = argc-1,phase,i;

  if (itfParseArgv (argv[0],&ac,argv+1,1,
		    "<phaseName>",ARGV_STRING,NULL,&ph_name,NULL,
		    "new phase ('work','test')",
		    NULL) != TCL_OK) return TCL_ERROR;

  if (!strcmp (ph_name,"work")) phase = WORKING_PHASE;
  else if (!strcmp (ph_name,"test")) phase = TESTING_PHASE;
  else {
    ERROR("Possible phases: working ('work'), testing ('test').\n");
    return TCL_ERROR;
  }

  if (phase == obj->phase) return TCL_OK;  /* nothing to do */
  if (obj->phase == CONSTRUCTION_PHASE) {
    obj->phase = phase;
    return TCL_OK;
  } else if (obj->phase == TESTING_PHASE) {
    if (phase != WORKING_PHASE) {
      ERROR("You can go to working phase only!\n");
      return TCL_ERROR;
    }
    obj->phase = phase;
    return TCL_OK;
  }
  /* system is in working phase */
  if (phase != TESTING_PHASE) {
    ERROR("You can go to testing phase only!\n");
    return TCL_ERROR;
  }
  for (i = 0; i < obj->list.itemN; i++)
    if (obj->list.itemA[i].accu_flag != ACCUS_EMPTY) {
      ERROR("All codebooks must have empty accus!\n");
      return TCL_ERROR;
    }
  /* Free all accus */
  for (i = 0; i < obj->list.itemN; i++)
    free_accus (&(obj->list.itemA[i]));
  obj->phase = phase;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewsetSetTrainParmatsItf
   Phase: all. In working phase, all accus must be empty.
   All accus are freed.
   ------------------------------------------------------------------- */
int cbnewsetSetTrainParmatsItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;
  int i;

  if (argc != 1) {
    ERROR("Method has no arguments!\n");
    return TCL_ERROR;
  }
  if (obj->train_parmats != 0)
    return TCL_OK;  /* nothing to do */

  if (obj->phase == WORKING_PHASE) {
    /* Check if all accus are empty */
    for (i = 0; i < obj->list.itemN; i++)
      if (obj->list.itemA[i].accu_flag != ACCUS_EMPTY) {
	ERROR("All codebooks must have empty accus!\n");
	return TCL_ERROR;
      }
    /* Free all accus */
    for (i = 0; i < obj->list.itemN; i++)
      free_accus (&(obj->list.itemA[i]));
  }
  obj->train_parmats = 1;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewsetClearTrainParmatsItf
   Phase: all. In working phase, all accus must be empty.
   All accus are freed.
   ------------------------------------------------------------------- */
int cbnewsetClearTrainParmatsItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;
  int i;

  if (argc != 1) {
    ERROR("Method has no arguments!\n");
    return TCL_ERROR;
  }
  if (obj->train_parmats == 0)
    return TCL_OK;  /* nothing to do */

  if (obj->phase == WORKING_PHASE) {
    /* Check if all accus are empty */
    for (i = 0; i < obj->list.itemN; i++)
      if (obj->list.itemA[i].accu_flag != ACCUS_EMPTY) {
	ERROR("All codebooks must have empty accus!\n");
	return TCL_ERROR;
      }
    /* Free all accus */
    for (i = 0; i < obj->list.itemN; i++)
      free_accus (&(obj->list.itemA[i]));
  }
  obj->train_parmats = 0;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewsetClearAccusItf
   Phase: working (checked).
   ------------------------------------------------------------------- */
int cbnewsetClearAccusItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;
  int ac = argc-1,ret = TCL_OK,cbi = -1;

  if (obj->phase != WORKING_PHASE) {
    ERROR("Object must be in working phase!\n");
    return TCL_ERROR;
  }
  if (itfParseArgv (argv[0],&ac,argv+1,1,
		    "-cbIndex",ARGV_INT,NULL,&cbi,NULL,
		    "codebook index (def: all)",
		    NULL) != TCL_OK) return TCL_ERROR;

  if (cbi < -1 || cbi >= obj->list.itemN) {
    ERROR("Invalid codebook index!\n");
    return TCL_ERROR;
  }
  if (cbi != -1)
    ret = cbnewClearAccus (&(obj->list.itemA[cbi]));
  else {
    for (cbi = 0; cbi < obj->list.itemN; cbi++)
      if (cbnewClearAccus (&(obj->list.itemA[cbi])) == TCL_ERROR)
	ret = TCL_ERROR;
    if (ret == TCL_ERROR)
      ERROR("Error recorded while clearing accus!\n");
  }

  return ret;
}

/* -------------------------------------------------------------------
   cbnewsetAccuItf
   Returns the neg. log likelihood of the samples.
   Phase: working (checked).
   ------------------------------------------------------------------- */
int cbnewsetAccuItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;
  int ac = argc-1,cbi,i;
  int okN = 0,errorN = 0,wrongN = 0;
  SVector* feat_iv;
  FVector* scale_v = 0;
  CBNew* cb;
  double scale,loglh,sum;
  float* sample;

  if (obj->phase != WORKING_PHASE) {
    ERROR("Object must be in working phase!\n");
    return TCL_ERROR;
  }
  if (itfParseArgv (argv[0],&ac,argv+1,1,
		    "<cbIndex>",    ARGV_INT,   NULL,&cbi,    NULL,
		    "index of codebook",
		    "<featIndVect>",ARGV_OBJECT,NULL,&feat_iv,"SVector",
		    "vector of sample indices in feature",
		    "-scaleVect",   ARGV_OBJECT,NULL,&scale_v,"FVector",
		    "vector of scaling factors (def.: (1 1 ... 1)^t)",
		    NULL) != TCL_OK) return TCL_ERROR;

  if (cbi < 0 || cbi >= obj->list.itemN) {
    ERROR("Codebook %d doesn't exist!\n",cbi);
    return TCL_ERROR;
  }
  cb = &(obj->list.itemA[cbi]);
  if (cb->accu_flag == ACCUS_LOCKED) {
    ERROR("Codebook has locked accus!\n");
    return TCL_ERROR;
  }
  for (i = 0,sum = 0.00; i < feat_iv->n; i++) {
    scale = scale_v ? (double) scale_v->vecA[i] : 1.00;
    if (!(sample = fesFrame (obj->feat_set,cb->feat,(int) feat_iv->vecA[i])))
      wrongN++;
    else if (cbnewAccu (cb,sample,scale,&loglh) == TCL_ERROR) errorN++;
    else {
      okN++;
      sum += loglh;
    }
  }
  INFO("%d samples accumulated.\n",okN);
  itfAppendResult ("%lf",sum);
  if (wrongN > 0)
    INFO("%d samples discarded because of wrong feature indices\n",wrongN);
  if (errorN > 0)
    INFO("%d samples discarded because of errors\n",errorN);

  return (errorN > 0) ? TCL_ERROR : TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewsetAccuMatrixItf
   Returns neg. log likelihood of the samples.
   Phase: working (checked).
   ------------------------------------------------------------------- */
int cbnewsetAccuMatrixItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;
  int ac = argc-1,cbi,i,begin = 0,end = -1;
  int okN = 0,errorN = 0;
  FMatrix* fmat = 0;
  FVector* scale_v = 0;
  CBNew* cb;
  double scale,loglh,sum;

  if (obj->phase != WORKING_PHASE) {
    ERROR("Object must be in working phase!\n");
    return TCL_ERROR;
  }
  if (itfParseArgv (argv[0],&ac,argv+1,1,
		    "<cbIndex>", ARGV_INT,   NULL,&cbi,    NULL,
		    "index of codebook",
		    "<sampMat>", ARGV_OBJECT,NULL,&fmat,   "FMatrix",
		    "matrix of samples to accumulate",
		    "-begin",    ARGV_INT,   NULL,&begin,  NULL,
		    "first row of epoch in sample matrix",
		    "-end",      ARGV_INT,   NULL,&end,    NULL,
		    "last row of epoch in sample matrix",
		    "-scaleVect",ARGV_OBJECT,NULL,&scale_v,"FVector",
		    "vector of scaling factors (def.: (1 1 ... 1)^t)",
		    NULL) != TCL_OK) return TCL_ERROR;

  if (end == -1) end = fmat->m - 1;
  if (cbi < 0 || cbi >= obj->list.itemN) {
    ERROR("Codebook %d doesn't exist!\n",cbi);
    return TCL_ERROR;
  }
  cb = &(obj->list.itemA[cbi]);
  if (cb->accu_flag == ACCUS_LOCKED) {
    ERROR("Codebook has locked accus!\n");
    return TCL_ERROR;
  }
  if (begin < 0 || end < begin || end >= fmat->m) {
    ERROR("-begin and/or -end out of range!\n");
    return TCL_ERROR;
  }
  if (fmat->n != obj->num_dim) {
    ERROR("Sample matrix must have %d columns!\n",obj->num_dim);
    return TCL_ERROR;
  }
  if (scale_v && scale_v->n < end-begin+1) {
    ERROR("-scaleVect must have %d components!\n",end-begin+1);
    return TCL_ERROR;
  }
  for (i = begin,sum = 0.00; i <= end; i++) {
    scale = scale_v ? (double) scale_v->vecA[i-begin] : 1.00;
    if (cbnewAccu (cb,fmat->matPA[i],scale,&loglh) == TCL_ERROR) errorN++;
    else {
      okN++;
      sum += loglh;
    }
  }
  INFO("%d samples accumulated.\n",okN);
  itfAppendResult ("%lf",sum);
  if (errorN > 0)
    INFO("%d samples discarded because of errors\n",errorN);

  return (errorN > 0) ? TCL_ERROR : TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewsetUpdateItf
   Phase: working (checked).
   ------------------------------------------------------------------- */
int cbnewsetUpdateItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;
  int ac = argc-1,cbi = -1,ret = TCL_OK,not_mean,not_diag;

  if (obj->phase != WORKING_PHASE) {
    ERROR("Object must be in working phase!\n");
    return TCL_ERROR;
  }
  if (itfParseArgv (argv[0],&ac,argv+1,1,
		    "-cbIndex",ARGV_INT,NULL,&cbi,NULL,
		    "codebook index (def: all)",
		    NULL) != TCL_OK) return TCL_ERROR;

  if (cbi < -1 || cbi >= obj->list.itemN) {
    ERROR("Invalid codebook index!\n");
    return TCL_ERROR;
  }
  if (cbi != -1) {
    if (obj->list.itemA[cbi].accu_flag == ACCUS_LOCKED) {
      ERROR("Codebook has locked accus!\n");
      return TCL_ERROR;
    }
    ret = cbnewUpdate (&(obj->list.itemA[cbi]),&not_mean,&not_diag);
    if (ret == TCL_OK) {
      if (not_mean > 0)
	INFO("%d Gaussians had too few samples to update mean/diag\n",not_mean);
      if (not_diag > 0)
	INFO("%d Gaussians had enough samples to update mean, but too few samples to update diag\n",not_diag);
    }
  } else
    ret = cbnewsetUpdate (obj);

  return ret;
}

/* -------------------------------------------------------------------
   cbnewsetEvalKLItf
   Phase: working (checked).
   ------------------------------------------------------------------- */
int cbnewsetEvalKLItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;
  double val;
  int ac = argc-1,ret,cbi = -1;

  if (obj->phase != WORKING_PHASE) {
    ERROR("Object must be in working phase!\n");
    return TCL_ERROR;
  }
  if (itfParseArgv (argv[0],&ac,argv+1,1,
		    "<cbIndex>",ARGV_INT,NULL,&cbi,NULL,
		    "codebook index",
		    NULL) != TCL_OK) return TCL_ERROR;

  if (cbi < 0 || cbi >= obj->list.itemN) {
    ERROR("Invalid codebook index!\n");
    return TCL_ERROR;
  }
  if (obj->list.itemA[cbi].accu_flag != ACCUS_LOCKED) {
    ERROR("Accumulators must be locked! Use 'update'.\n");
    return TCL_ERROR;
  }

  if ((ret = cbnewEvalKL (&(obj->list.itemA[cbi]),&val)) == TCL_OK)
    itfAppendResult ("%lf",val);

  return ret;
}

/* -------------------------------------------------------------------
   cbnewsetOpenProtItf
   Phase: all.
   ------------------------------------------------------------------- */
int cbnewsetOpenProtItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;
  int ac = argc-1,cbi;
  FMatrix* datamat = 0;
  char* crit_name = NULL;
  crit_func_ptr crit = DEF_CRIT;
  int begin = 0;
  int end = -1;
  int protnum;

  if (itfParseArgv (argv[0],&ac,argv+1,1,
		    "<cbIndex>", ARGV_INT,  NULL,&cbi,      NULL,
		    "codebook index",
		    "<dataMat>",ARGV_OBJECT,NULL,&datamat,  "FMatrix",
		    "evaluation data matrix",
		    "-critFunc",ARGV_STRING,NULL,&crit_name,NULL,
		    "criterion function",
		    "-begin",   ARGV_INT,   NULL,&begin,    NULL,
		    "first row of eval. epoch in data matrix",
		    "-end",     ARGV_INT,   NULL,&end,      NULL,
		    "last row of eval. epoch in data matrix",
		    NULL) != TCL_OK) return TCL_ERROR;

  if (end == -1)
    end = datamat->m - 1;  /* Default value for 'end' */
  if (cbi < 0 || cbi >= obj->list.itemN) {
    ERROR("Codebook %d doesn't exist!\n",cbi);
    return TCL_ERROR;
  }
  if (crit_name && !(crit = cbnewCritfuncPtr (crit_name))) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }
  if (begin < 0 || end < begin || end >= datamat->m) {
    ERROR("-begin and/or -end out of range!\n");
    return TCL_ERROR;
  }
  if (datamat->n != obj->num_dim) {
    ERROR("Data matrix must have %d columns!\n",obj->num_dim);
    return TCL_ERROR;
  }
  if (cbnewOpenProt (&(obj->list.itemA[cbi]),datamat,crit,begin,end,&protnum)
      == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }
  itfAppendResult ("%d",protnum); /* Return protocol number */

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewsetCloseProtItf
   Phase: all.
   ------------------------------------------------------------------- */
int cbnewsetCloseProtItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;
  int ac = argc-1,cbi;
  int protnum;

  if (itfParseArgv (argv[0],&ac,argv+1,1,
		    "<cbIndex>",ARGV_INT,NULL,&cbi,    NULL,
		    "codebook index",
		    "<protNum>",ARGV_INT,NULL,&protnum,NULL,
		    "protocol number",
		    NULL) != TCL_OK) return TCL_ERROR;

  if (cbi < 0 || cbi >= obj->list.itemN) {
    ERROR("Codebook %d doesn't exist!\n",cbi);
    return TCL_ERROR;
  }
  if (cbnewCloseProt (&(obj->list.itemA[cbi]),protnum) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewsetEvalProtItf
   Phase: working (checked).
   ------------------------------------------------------------------- */
int cbnewsetEvalProtItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;
  int ac = argc-1,cbi,protnum = -1;

  if (obj->phase != WORKING_PHASE) {
    ERROR("Object must be in working phase!\n");
    return TCL_ERROR;
  }
  if (itfParseArgv (argv[0],&ac,argv+1,1,
		    "<cbIndex>",ARGV_INT,NULL,&cbi,    NULL,
		    "codebook index",
		    "-protNum", ARGV_INT,NULL,&protnum,NULL,
		    "protocol number (none => eval. all prot.)",
		    NULL) != TCL_OK) return TCL_ERROR;

  if (cbi < 0 || cbi >= obj->list.itemN) {
    ERROR("Codebook %d doesn't exist!\n",cbi);
    return TCL_ERROR;
  }

  return cbnewEvalProt (&(obj->list.itemA[cbi]),protnum);
}

/* -------------------------------------------------------------------
   cbnewsetDataPlotItf
   Phase: != construction,!= testing (checked).
   ------------------------------------------------------------------- */
int cbnewsetDataPlotItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;
  int ac = argc-1,ret = TCL_OK,i,cbi;
  CBNew* cb;
  FMatrix* datamat = 0;
  int ref_ind = -1,begin = 0,end = -1;
  char* fname = NULL;
  FILE* fp_plot;
  SVector* ind_vect = 0;

  if (obj->phase == CONSTRUCTION_PHASE || obj->phase == TESTING_PHASE) {
    ERROR("Object must not be in construction or testing phase!\n");
    return TCL_ERROR;
  }
  if (itfParseArgv (argv[0],&ac,argv+1,1,
                    "<cbIndex>",ARGV_INT,   NULL,&cbi,     NULL,
		    "codebook index",
                    "<dataMat>",ARGV_OBJECT,NULL,&datamat, "FMatrix",
                    "data matrix",
                    "<indVect>",ARGV_OBJECT,NULL,&ind_vect,"SVector",
                    "vector of dimensions to project upon",
                    "<fName>",  ARGV_STRING,NULL,&fname,   NULL,
                    "name of result file",
                    "-begin",   ARGV_INT,   NULL,&begin,   NULL,
		    "first row of eval. epoch in data matrix",
                    "-end",     ARGV_INT,   NULL,&end,     NULL,
		    "last row of eval. epoch in data matrix",
                    "-refIndex",ARGV_INT,   NULL,&ref_ind, NULL,
                    "focus on gaussian ... (def: show all)",
		    NULL) != TCL_OK) return TCL_ERROR;

  if (end == -1)
    end = datamat->m - 1;  /* Default value for 'end' */
  if (begin < 0 || end < begin || end >= datamat->m) {
    ERROR("-begin and/or -end out of bounds!\n");
    return TCL_ERROR;
  }
  if (!datamat->m || datamat->n != obj->num_dim) {
    ERROR("Data matrix must have %d columns!\n",obj->num_dim);
    return TCL_ERROR;
  }
  if (ind_vect->n == 0) {
    ERROR("<indVect> must have entries!\n");
    return TCL_ERROR;
  }
  if (cbi < 0 || cbi >= obj->list.itemN) {
    ERROR("Codebook %d doesn't exist!\n",cbi);
    return TCL_ERROR;
  }
  cb = &(obj->list.itemA[cbi]);
  if (ref_ind < -1 || ref_ind >= cb->num_dens) {
    ERROR("-refIndex not a valid density index!\n");
    return TCL_ERROR;
  }
  for (i = 0; i < ind_vect->n; i++)
    if (ind_vect->vecA[i] < 0 || ind_vect->vecA[i] >= obj->num_dim) {
      ERROR("<indVect> entries out of bounds!\n");
      return TCL_ERROR;
    }
  if (!(fp_plot = fileOpen (fname,"w"))) {
    ERROR("Can't create file '%s'!\n",fname);
    return TCL_ERROR;
  }
  if (cbnewDataPlot (cb,datamat,begin,end,ind_vect,fp_plot,ref_ind)
      == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    ret = TCL_ERROR;
  }
  fileClose (fname,fp_plot);

  return ret;
}

/* -------------------------------------------------------------------
   cbnewsetGenSamplesItf
   Generates samples given codebook model.
   Phase: != testing (checked).
   ------------------------------------------------------------------- */

int cbnewsetGenSamplesItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;
  int ac = argc-1,cbi,num_samples = 0,ret;
  double seed = 0.00;
  FMatrix* smat = 0;
  CBNew* cb;

  if (obj->phase == TESTING_PHASE) {
    ERROR("Object must not be in testing phase!\n");
    return TCL_ERROR;
  }
  if (itfParseArgv (argv[0],&ac,argv+1,1,
		    "<cbIndex>",ARGV_INT,   NULL,&cbi,        NULL,
		    "index of codebook",
		    "<sampN>",  ARGV_INT,   NULL,&num_samples,NULL,
		    "number of samples to create",
		    "<sampMat>",ARGV_OBJECT,NULL,&smat,       "FMatrix",
		    "sample matrix",
		    "-seed",    ARGV_DOUBLE,NULL,&seed,       NULL,
		    "seed to use for PRNG",
		    NULL) != TCL_OK) return TCL_ERROR;

  if (cbi < 0 || cbi >= obj->list.itemN) {
    ERROR("Codebook %d doesn't exist!\n",cbi);
    return TCL_ERROR;
  }
  cb = &(obj->list.itemA[cbi]);
  if (num_samples <= 0) {
    ERROR("<sampN> must be positive!\n");
    return TCL_ERROR;
  }
  if (seed == 0.00)
    ret = cbnewGenSamples (cb,num_samples,smat,&seed,1);
  else
    ret = cbnewGenSamples (cb,num_samples,smat,&seed,0);
  itfAppendResult ("%lf",seed);

  return ret;
}

/* -------------------------------------------------------------------
   cbnewsetSaveItf
   Phase: all.
   ------------------------------------------------------------------- */
int cbnewsetSaveItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;
  int ac = argc-1,ret;
  char* fname = NULL;

  if (itfParseArgv (argv[0],&ac,argv+1,1,
                    "<fName>",ARGV_STRING,NULL,&fname,NULL,
                    "name of structure file to create",
		    NULL) != TCL_OK) return TCL_ERROR;

  if ((ret = cbnewsetSave (obj,fname)) == TCL_OK)
    INFO("%d codebooks stored\n",obj->list.itemN);

  return ret;
}

/* -------------------------------------------------------------------
   cbnewsetLoadWeightsItf
   Phase: != testing (checked).
   ------------------------------------------------------------------- */
int cbnewsetLoadWeightsItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;
  int ac = argc-1,ret,okN,nexistN,nemptyN,wrongRefNN;
  char* fname = NULL;

  if (obj->phase == TESTING_PHASE) {
    ERROR("Object must not be in testing phase!\n");
    return TCL_ERROR;
  }
  if (itfParseArgv (argv[0],&ac,argv+1,1,
                    "<fName>",ARGV_STRING,NULL,&fname,NULL,
                    "name of weight file to load",
		    NULL) != TCL_OK) return TCL_ERROR;

  if ((ret = cbnewsetLoadWeights (obj,fname,&okN,&nexistN,&nemptyN,
				  &wrongRefNN)) == TCL_OK) {
    INFO("%d codebook weights loaded\n",okN);
    if (nexistN > 0)
      INFO("%d entries discarded because codebooks don't exist\n",nexistN);
    if (nemptyN > 0)
      INFO("%d entries discarded because codebooks don't have empty accus\n",
	   nemptyN);
    if (wrongRefNN > 0)
      INFO("%d entries discarded because of 'refN' mismatch\n",wrongRefNN);
  }

  return ret;
}

/* -------------------------------------------------------------------
   cbnewsetSaveWeightsItf
   Phase: != testing (checked).
   ------------------------------------------------------------------- */
int cbnewsetSaveWeightsItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;
  int ac = argc-1,ret;
  char* fname = NULL;

  if (obj->phase == TESTING_PHASE) {
    ERROR("Object must not be in testing phase!\n");
    return TCL_ERROR;
  }
  if (itfParseArgv (argv[0],&ac,argv+1,1,
                    "<fName>",ARGV_STRING,NULL,&fname,NULL,
                    "name of weight file to create",
		    NULL) != TCL_OK) return TCL_ERROR;

  ret = cbnewsetSaveWeights (obj,fname);
  if (ret == TCL_OK)
    INFO("Weights of %d codebooks stored\n",obj->list.itemN);

  return ret;
}

/* -------------------------------------------------------------------
   cbnewsetLoadAccusItf
   Phase: working.
   ------------------------------------------------------------------- */
int cbnewsetLoadAccusItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;
  int ac = argc-1,ret,emptyN,lockedN,buildN,nexistN,wrongRefNN,structN;
  int wrongAS,inactN;
  char* fname = NULL;

  if (obj->phase != WORKING_PHASE) {
    ERROR("Object must be in working phase!\n");
    return TCL_ERROR;
  }
  if (itfParseArgv (argv[0],&ac,argv+1,1,
                    "<fName>",ARGV_STRING,NULL,&fname,NULL,
                    "name of accumulator file to load",
		    NULL) != TCL_OK) return TCL_ERROR;

  if ((ret = cbnewsetLoadAccus (obj,fname,&emptyN,&lockedN,&buildN,&nexistN,
				&wrongRefNN,&wrongAS,&structN,
				&inactN)) == TCL_OK) {
    INFO("%d empty Gaussian accus loaded\n",emptyN);
    INFO("%d locked Gaussian accus loaded\n",lockedN);
    INFO("%d Gaussian accus in construction loaded\n",buildN);
    if (nexistN > 0)
      INFO("%d Gaussian accus discarded because codebooks don't exist\n",
	   nexistN);
    if (wrongRefNN > 0)
      INFO("%d Gaussian accus discarded because of 'refN' mismatch\n",
	   wrongRefNN);
    if (wrongAS > 0)
      INFO("%d Gaussian accus discarded because of 'accuFlag' mismatch\n",
	   wrongAS);
    if (structN > 0)
      INFO("%d Gaussian accus discarded because of structural mismatch\n",
	   structN);
    if (inactN > 0)
      INFO("%d Gaussian accus discarded because of different 'active' states\n",inactN);
  }

  return ret;
}

/* -------------------------------------------------------------------
   cbnewsetLoadAccusItf_dep
   Phase: working.
   ------------------------------------------------------------------- */
int cbnewsetLoadAccusItf_dep (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;
  int ac = argc-1,ret,emptyN,lockedN,buildN,nexistN,wrongRefNN,structN;
  int wrongAS,inactN;
  char* fname = NULL;

  if (obj->phase != WORKING_PHASE) {
    ERROR("Object must be in working phase!\n");
    return TCL_ERROR;
  }
  if (itfParseArgv (argv[0],&ac,argv+1,1,
                    "<fName>",ARGV_STRING,NULL,&fname,NULL,
                    "name of accumulator file to load",
		    NULL) != TCL_OK) return TCL_ERROR;

  if ((ret = cbnewsetLoadAccus_dep (obj,fname,&emptyN,&lockedN,&buildN,
				    &nexistN,&wrongRefNN,&wrongAS,&structN,
				    &inactN)) == TCL_OK) {
    INFO("%d empty Gaussian accus loaded\n",emptyN);
    INFO("%d locked Gaussian accus loaded\n",lockedN);
    INFO("%d Gaussian accus in construction loaded\n",buildN);
    if (nexistN > 0)
      INFO("%d Gaussian accus discarded because codebooks don't exist\n",
	   nexistN);
    if (wrongRefNN > 0)
      INFO("%d Gaussian accus discarded because of 'refN' mismatch\n",
	   wrongRefNN);
    if (wrongAS > 0)
      INFO("%d Gaussian accus discarded because of 'accuFlag' mismatch\n",
	   wrongAS);
    if (structN > 0)
      INFO("%d Gaussian accus discarded because of structural mismatch\n",
	   structN);
    if (inactN > 0)
      INFO("%d Gaussian accus discarded because of different 'active' states\n",inactN);
  }

  return ret;
}

/* -------------------------------------------------------------------
   cbnewsetSaveAccusItf
   Phase: working.
   ------------------------------------------------------------------- */
int cbnewsetSaveAccusItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;
  int ac = argc-1,ret,emptyN,lockedN,buildN;
  char* fname = NULL;

  if (obj->phase != WORKING_PHASE) {
    ERROR("Object must be in working phase!\n");
    return TCL_ERROR;
  }
  if (itfParseArgv (argv[0],&ac,argv+1,1,
                    "<fName>",ARGV_STRING,NULL,&fname,NULL,
                    "name of accumulator file to create",
		    NULL) != TCL_OK) return TCL_ERROR;

  if ((ret = cbnewsetSaveAccus (obj,fname,&emptyN,&lockedN,
				&buildN)) == TCL_OK) {
    INFO("%d empty accus stored\n",emptyN);
    INFO("%d locked accus stored\n",lockedN);
    INFO("%d accus in construction stored\n",buildN);
  }

  return ret;
}

/* -------------------------------------------------------------------
   cbnewsetSaveAccusItf_dep (DEBUG)
   Phase: working.
   CAUTION: System-dep. format for doubles!!
   ------------------------------------------------------------------- */
int cbnewsetSaveAccusItf_dep (ClientData cd,int argc,char *argv[])
{
  CBNewSet* obj = (CBNewSet*) cd;
  int ac = argc-1,ret,emptyN,lockedN,buildN;
  char* fname = NULL;

  if (obj->phase != WORKING_PHASE) {
    ERROR("Object must be in working phase!\n");
    return TCL_ERROR;
  }
  if (itfParseArgv (argv[0],&ac,argv+1,1,
                    "<fName>",ARGV_STRING,NULL,&fname,NULL,
                    "name of accumulator file to create",
		    NULL) != TCL_OK) return TCL_ERROR;

  if ((ret = cbnewsetSaveAccus_dep (obj,fname,&emptyN,&lockedN,
				&buildN)) == TCL_OK) {
    INFO("%d empty accus stored\n",emptyN);
    INFO("%d locked accus stored\n",lockedN);
    INFO("%d accus in construction stored\n",buildN);
  }

  return ret;
}

/* -------------------------------------------------------------------
   cbnewsetConvertItf
   ------------------------------------------------------------------- */
int cbnewsetConvertItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet          *cbn = (CBNewSet*) cd;
  CodebookSet       *cbs = NULL;
  DistribSet        *dss = NULL;
  FeatureSet        *fes = cbn->feat_set;
  int cbX = 0, dsN=0;
  int cbN = cbn->list.itemN;
  int ac  = argc-1;
  
  if (itfParseArgv (argv[0],&ac,argv+1,1,
		    "<CodebookSet>",   ARGV_OBJECT, NULL, &cbs, "CodebookSet", "name of the codebook set", 
		    "<DistribSet>" ,   ARGV_OBJECT, NULL, &dss, "DistribSet",  "name of the distrib  set", 
		    NULL) != TCL_OK) return TCL_ERROR;


  dsN = dss->list.itemN;
  if (cbN != dsN) {
    WARN("cbnewSetConvert: expect same number of distributions and codebooks but got cbN= %d and dsN= %d\n",
	 cbN,dsN);
  }

  for (cbX=0;cbX<cbN;cbX++) {
    CBNew          *cn = cbn->list.itemA + cbX;
    CBNewParMatrix *pm = (cn->parmat)->mat;
    Codebook       *cb = NULL;
    Distrib        *ds = NULL;
    int refN =  cn->num_dens;
    int dimN = cbn->num_dim;
    int refX, featX, cbX, dsX;
    char buf[256];

    strcpy(buf,"OFS-");
    strcat(buf,pm->name);

    /* create feature */
    featX= fesGetIndex(fes,buf,1);
    if (featX < 0) {
      ERROR("cbnewSetConvert:failed to create feature %s\n",buf);
      return TCL_ERROR;
    }

    /* get distribution and codebook */
    dsX= listIndex((List*)&(dss->list), cn->name, 0);
    if (dsX < 0) {
      WARN("cbnewSetConvert:couldn't find codebook %s\n",cn->name);
      continue;
    }
    ds  = dss->list.itemA+dsX;
    cbX = ds->cbX;
    cb  = cbs->list.itemA+cbX;

    /* replace featX */
    cb->featX= featX;

    /* heuristics to detect unseen models */
    if (refN) {
      float mean = log((double)refN);
      int   refM = refN -1;
      while ( refM >=0 && fabs(cn->distrib[refM] - mean) < 1e-5) refM--;
      if (refM < 0) {
	WARN("cbnewSetConvert: detected empty model '%s'\n",cn->name);
	continue;
      }
    }

    /* convert distrib */
    memcpy(ds->val,cn->distrib,refN*sizeof(float));

    /* convert means from semi-tied codebook to old style codebooks */
    for (refX=0;refX<refN;refX++) {
      float *mean    = cn->mean->matPA[refX];
      float *tr_mean = cb->rv->matPA[refX];      
      int dimX, dimY, blockX, dimTotal, *dimV;

      if (! pm) {
	memcpy(tr_mean,mean,dimN*sizeof(float));
	continue;
      }

      dimV    = pm->dim_vect;
      dimTotal= 0;
    
      for (blockX=0; blockX< pm->num_block; blockX++) {	
	float **block = pm->list[blockX]->matPA;
	int dimBlock  = pm->list[blockX]->m;

	for (dimX=0; dimX< dimBlock; dimX++) {
	  float  sum = 0.0;
	  for (dimY=0; dimY< dimBlock; dimY++) {	    
	    sum += block[dimX][dimY] * mean[dimV[dimTotal+dimY]];
	  }
	  tr_mean[dimV[dimTotal+dimX]] = sum;
	}   
	dimTotal += dimBlock;
      }
    }

    /* convert covariances from semi-tied codebook to old style codebooks */
    for (refX=0;refX<refN;refX++) {
      float *diag    = cn->diag->matPA[refX];
      float *tr_diag = cb->cv[refX]->m.d;
      int dimX;

      for (dimX=0; dimX< dimN; dimX++) {
	if (diag[dimX] <= COV_FLOOR)
	  tr_diag[dimX]= 1.0 / COV_FLOOR;
	else
	  tr_diag[dimX]= 1.0 / diag[dimX];
      }
    }

    /* convert determinant from semi-tied codebook to old style codebooks */
    cb->pi= log(2.0 * PI) * (float)dimN;
    for (refX=0;refX<refN;refX++) {
      float *diag       = cn->diag->matPA[refX];
      float lh_factor   = -1.0 * dimN * LOG_TWO_PI;
      int dimX;
      for (dimX=0; dimX< dimN; dimX++) {
	lh_factor -= log ((double) diag[dimX]);
      }
      lh_factor /= 2.0;
      cb->cv[refX]->det= -2.0*lh_factor - cb->pi;
    }

  }
  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewsetCompareItf
   ------------------------------------------------------------------- */

int cbnewsetCompareItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet          *cbs1 = (CBNewSet*) cd;
  CBNewSet          *cbs2 = NULL;
  int cbX,cb1N, cb2N;
  
  int ac  = argc-1;
  
  if (itfParseArgv (argv[0],&ac,argv+1,1,
		    "<CBNewSet>",   ARGV_OBJECT, NULL, &cbs2, "CBNewSet", "CBNewSet to compare", 
		    NULL) != TCL_OK) return TCL_ERROR;


  if (cbs1->num_dim != cbs2->num_dim) {
    ERROR("num_dim: %d %d\n",cbs1->num_dim,cbs2->num_dim);
    return TCL_ERROR;
  }
  
  cb1N= cbs1->list.itemN;
  cb2N= cbs2->list.itemN;
  if (cb1N != cb2N) {
    ERROR("itemN: %d %d\n",cb1N,cb2N);
    return TCL_ERROR;
  }
  
  for (cbX=0;cbX<cb1N;cbX++) {
    int refX, dimX;
    CBNew* cb1 = cbs1->list.itemA+cbX;
    CBNew* cb2 = cbs2->list.itemA+cbX;

    if (strcmp(cb1->name,cb2->name) !=0) {
      ERROR("cb name: %s %s\n",cb1->name,cb2->name);
      return TCL_ERROR;
    }
    if (cb1->num_dens != cb2->num_dens) {
      ERROR("%s num_dens: %d %d\n",cb1->name,cb1->num_dens,cb2->num_dens);
      return TCL_ERROR;
    }
    if (cb1->feat != cb2->feat) {
      ERROR("%s feat: %d %d\n",cb1->name,cb1->feat,cb2->feat);
    }
    for (refX=0;refX<cb1->num_dens;refX++) {
      if (cb1->active[refX] != cb2->active[refX]) {
	ERROR("%s active %d: %8f %8f\n",cb1->name,refX,cb1->active[refX],cb2->active[refX]);
      }
      if (cb1->distrib[refX] != cb2->distrib[refX]) {
	ERROR("%s distrib %d: %8f %8f\n",cb1->name,refX,cb1->distrib[refX],cb2->distrib[refX]);
      }
      for (dimX=0;dimX<cbs1->num_dim;dimX++) {
	if (cb1->mean->matPA[refX][dimX] != cb2->mean->matPA[refX][dimX]) {
	  ERROR("%s mean %d %d: %8f %8f\n",cb1->name,refX,dimX,cb1->mean->matPA[refX][dimX],
		cb2->mean->matPA[refX][dimX]);
	}
	if (cb1->diag->matPA[refX][dimX] != cb2->diag->matPA[refX][dimX]) {
	  ERROR("%s diag %d %d: %30.30f %30.30f\n",cb1->name,refX,dimX,cb1->diag->matPA[refX][dimX],
		cb2->diag->matPA[refX][dimX]);
	}
      }
    }
  }
  INFO("everything fine\n");
  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewsetResetItf
   ------------------------------------------------------------------- */

int cbnewsetResetItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet  *cbs = (CBNewSet*) cd;
  int        cbN = cbs->list.itemN;
  int cbX;

  for (cbX=0;cbX<cbN;cbX++) {
    CBNew* cb = cbs->list.itemA+cbX;
    int  refX = cb->num_dens-1;
    if (cb->active)
      for (;refX>0; refX--) cb->active[refX] = 1;
  }
  return TCL_OK;
}

/* --- Class descriptions --- */

static Method cbnewMethod[] = {
  { "puts",      cbnewPutsItf,      NULL},
  { "set",       cbnewSetItf,       "set subobject" },
  {  NULL,       NULL,              NULL }
};

TypeInfo cbnewInfo = {
  "CBNew", 0, -1, cbnewMethod,
  0, 0,
  cbnewConfigureItf, cbnewAccessItf,
  itfTypeCntlDefaultNoLink,
  "Codebook using additional parameter matrices\n" };

/* -------------------------------------------------------------------
   CBNew_Init
   Type constructor for CBNew. 
   ------------------------------------------------------------------- */
static int cbnewInitialized = 0;

int CBNew_Init ()
{
  if (cbnewInitialized) return TCL_OK;

  if ( CBNewParMatrixSet_Init() != TCL_OK) return TCL_ERROR;

  itfNewType (&cbnewInfo);
  cbnewInitialized = 1;

  return TCL_OK;
}

static Method cbnewsetMethod[] = {
  { "puts",      cbnewsetPutsItf,      NULL},
  { "index",     cbnewsetIndexItf,     "get index of list element" },
  { "name",      cbnewsetNameItf,      "get name of list element" },
  { "add",       cbnewsetAddItf,       "add new codebook" },
  { "link",      cbnewsetLinkItf,      "link parameter matrix to gaussian(s)" },
  { "unlink",    cbnewsetUnlinkItf,    "unlink gaussian(s)" },
  { "phase",     cbnewsetPhaseItf,     "change object phase" },
  { "setTrainParmats",cbnewsetSetTrainParmatsItf,"switch on parmat training" },
  { "clearTrainParmats",cbnewsetClearTrainParmatsItf,
                                       "switch off parmat training" },
  { "clearAccus",cbnewsetClearAccusItf,"clear accumulators" },
  { "accu",      cbnewsetAccuItf,      "accumulate data" },
  { "accuMatrix",cbnewsetAccuMatrixItf,"accumulate sample matrix" },
  { "update",    cbnewsetUpdateItf,    "update parameters based on accus" },
  { "evalKL",    cbnewsetEvalKLItf,    "evaluate KL criterion" },
  { "openProt",  cbnewsetOpenProtItf,  "INTERNAL! Use CBNewSetOpenProt" },
  { "closeProt", cbnewsetCloseProtItf, "INTERNAL! Use CBNewSetCloseProt" },
  { "evalProt",  cbnewsetEvalProtItf,  "INTERNAL! Use CBNewSetEvalProt" },
  { "dataPlot",  cbnewsetDataPlotItf,  "scatter plot of most relev. dimensions" },
  { "genSamples",cbnewsetGenSamplesItf,"generate samples using codebook model" },
  { "save",      cbnewsetSaveItf,      "save object structure" },
  { "loadWeights",cbnewsetLoadWeightsItf,"load codebook weights" },
  { "saveWeights",cbnewsetSaveWeightsItf,"save codebook weights" },
  { "loadAccus", cbnewsetLoadAccusItf, "load accumulators incrementally" },
  { "saveAccus", cbnewsetSaveAccusItf, "save accumulators" },
  { "loadAccusDep", cbnewsetLoadAccusItf_dep,"load accumulators incrementally" },
  { "saveAccusDep", cbnewsetSaveAccusItf_dep,"save accumulators" },
  { "convert",      cbnewsetConvertItf,      "convert new codebook style to old style" },
  { "compare",      cbnewsetCompareItf,      "compare two codebooks" },
  { "reset",       cbnewsetResetItf,         "reset active flags" },
  {  NULL,       NULL,                 NULL }
};

TypeInfo cbnewsetInfo = {
  "CBNewSet", 0, -1, cbnewsetMethod,
  cbnewsetCreateItf, cbnewsetDestroyItf,
  cbnewsetConfigureItf, cbnewsetAccessItf,
  itfTypeCntlDefaultNoLink,
  "Set of CBNew codebooks\n" };

static ModelSet cbnewsetModel = 
             { "CBNewSet", &cbnewsetInfo, &cbnewInfo,
               (ModelSetIndexFct*) cbnewsetIndex,
               (ModelSetNameFct *) cbnewsetName,
               (ModelSetDistFct *) cbnewsetDist };

/* -------------------------------------------------------------------
   CBNewSet_Init
   Type constructor for CBNewSet. Since we want to generate random
   matrices using internal type FMatrix, we have to initialize the
   random number generator.
   ------------------------------------------------------------------- */
static int cbnewsetInitialized = 0;

int CBNewSet_Init ()
{
  if (cbnewsetInitialized) return TCL_OK;

  if ( ModelSet_Init() != TCL_OK) return TCL_ERROR;
  if ( CBNewParMatrixSet_Init() != TCL_OK) return TCL_ERROR;
  if ( CBNew_Init() != TCL_OK) return TCL_ERROR;

  /* Init. of pseudo random number generator of FMatrix (ffmat module).
     CHANGE THIS! */
  chaosRandomInit (0.123456789);
  srand (31415927);

  itfNewType (&cbnewsetInfo);
  modelSetNewType( &cbnewsetModel);
  cbnewsetInitialized = 1;

  return TCL_OK;
}
