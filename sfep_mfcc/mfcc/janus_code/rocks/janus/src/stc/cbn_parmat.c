/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: semi tied covariances
               ------------------------------------------------------------

    Author  :  Matthias Seeger
    Module  :  cbn_param.c
    Date    :  $Id: cbn_parmat.c 3253 2010-05-26 15:30:48Z metze $
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
    Revision 1.6  2003/08/14 11:20:25  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.5.4.4  2002/08/27 08:38:12  metze
    Now initializing string pointers to NULL in Itf

    Revision 1.5.4.3  2002/03/15 16:49:28  soltau
    cbnewparmatUpdate: fixed initialization problems

    Revision 1.5.4.2  2002/03/03 18:18:39  soltau
    cleaned update

    Revision 1.5.4.1  2001/06/26 19:29:30  soltau
    beautified/fixed a couple of ugly hacker routines

    Revision 1.5  2001/01/15 09:49:55  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 1.4.4.1  2001/01/12 15:16:49  janus
    necessary changes for running janus under WINDOWS

    Revision 1.4  2000/11/14 12:01:10  janus
    Merged branch jtk-00-10-27-jmcd.

    Revision 1.3.30.1  2000/11/06 10:50:24  janus
    Made changes to enable compilation under -Wall.

    Revision 1.3  2000/09/08 13:22:44  janus
    Merging branch 'jtk-00-08-27-hyu'.

    Revision 1.2.12.1  2000/09/07 20:06:24  janus
    Just checking.

    Revision 1.2  2000/08/16 09:11:14  soltau
    changed cbnewparmatAccess
    replaced free by Tcl_Free if memory was allocated by Tcl_SplitList

    Revision 1.1  1999/08/11 15:51:22  soltau
    Initial revision


   ======================================================================== */

/* Janus standards */
#include "common.h"
#include "error.h"
#include "mach_ind_io.h"

#include "cbn_parmat.h"
#include "cbnew.h"
#include "matrix_ext.h"
#include "cbn_assert.h" 
#include "cbn_common.h"
#include "featureMethodItf.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

extern TypeInfo cbnewparmatInfo;  /* predecl. */

/* --- Module local datatypes and variables --- */

/* -------------------------------------------------------------------
   cluster_gaussian
   cbnewpmsetCluster uses a temporary list with one entry of this
   type for each Gaussian.
   ------------------------------------------------------------------- */
typedef struct {
  CBNew* cb;
  int cb_ind;       /* density index within codebook */
  int index;        /* class index */
} cluster_gaussian;

static struct {     /* list of curr. available clustering procedures */
  char* name;
  char* desc;
  int code;
} name2code[] = {{"ml","ml: Maximum likelihood criterion\n",CLUSFUNC_ML}};


/* --- Module local functions --- */

/* -------------------------------------------------------------------
   normalize_fmatrix
   Normalizes each block of block diagonal FMatrix 'blockmat' to unit
   determinant. 'num_block' is the number of blocks.
   ------------------------------------------------------------------- */
static int normalize_fmatrix (FMatrix** blockmat,int num_block)
{
  int i;
  double a,b;

#ifdef DEBUG
  assert (num_block > 0);
  assert (blockmat);
  {
    int i;
    for (i = 0; i < num_block; i++) {
      assert (blockmat[i]);
      assert (blockmat[i]->m == blockmat[i]->n);
      assert (blockmat[i]->m > 0);
    }
  }
#endif

  for (i = 0; i < num_block; i++) {
    if (fmatrixExtDet (blockmat[i],&a,&b) == TCL_ERROR) {
#ifdef DEBUG
      ERROR("<= called by\n");
#endif
      return TCL_ERROR;
    }
    fmatrixScale (blockmat[i],blockmat[i],
		  (float) exp (-a / (double) blockmat[i]->m));
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   normalize_dmatrix
   Same as 'normalize_fmatrix', but using DMatrix block diagonal
   matrices.
   ------------------------------------------------------------------- */
static int normalize_dmatrix (DMatrix** blockmat,int num_block)
{
  int i;
  double a,b;

#ifdef DEBUG
  assert (num_block > 0);
  assert (blockmat);
  {
    int i;
    for (i = 0; i < num_block; i++) {
      assert (blockmat[i]);
      assert (blockmat[i]->m == blockmat[i]->n);
      assert (blockmat[i]->m > 0);
    }
  }
#endif

  for (i = 0; i < num_block; i++) {
    if (dmatrixExtDet (blockmat[i],&a,&b) == TCL_ERROR) {
#ifdef DEBUG
      ERROR("<= called by\n");
#endif
      return TCL_ERROR;
    }
    dmatrixScale (blockmat[i],blockmat[i],
		  exp (-a / (double) blockmat[i]->m));
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   eval_kl_gaussian_optimized
   The Q crit. summand corr. to the Gaussian is calc. and
   returned where terms not depending on the parmat of the Gaussian
   are discarded.
   If the Gaussian has enough data (i.e. >= DIAG_UPDATE_THRES samples)
   we assume that the diagonal variance matrix (the relevant comp.
   coming from 'diag', the irrelevant ones coming from 'cb->diag, see
   below) is the ML estimate based on the parmat value given by
   'pm_block' or 'pc_buff' (see below) (this need NOT be the value of
   the parmat linked to the Gaussian). In this case, the criterion
   formula is particularly simple.
   Note that you don't have to use the Gaussian's actual parmat in
   the calculations (see 'pm_block'). However the Gaussian must have
   a parmat with the same structure as the parmat used in the calc.
   IMPORTANT:
   - instead of 'cb->parmat[k].mat->list', we use 'pm_block' for the
     parmat blocks ('pm_block' has double coeff.!) except when
     'pc_buff' is used
   - instead of the relevant comp. of 'cb->diag->matPA[k]', we use
     'diag' for the Gaussian's variance matrix. Note that 'diag[j]'
     corr. to 'cb->diag->matPA[k][dim_vect[j]]' where 'dim_vect' is
     the dim. index vector of the parmat. The irrelevant comp. of
     the variance matrix are taken from 'cb->diag'. Note that
     'diag' has double coeff.
   - if 'pc_buff' != 0, it holds an array with precomputed values
     depending on the parmat to use. (see 'cbnewparmatUpdate' or
     'cluster_ml'. This array is then used instead of 'pm_block'.
     In this case, 'pc_size' must contain size information about
     the parmat blocks (see 'cbnewparmatUpdate').
   ------------------------------------------------------------------- */
static double eval_kl_gaussian_optimized (CBNew* cb,int k,DMatrix** pm_block,
					  double* diag,double* pc_buff,
					  int* pc_size)
{
  register int i,i2,j,l,dcnt,dcnt2,d,icum,cb_num_dim,size;
  double* cov,*pmat_row;
  double sum,factor,temp,beta,klsum;
  CBNewParMatrix* pmat;

#ifdef DEBUG
  ASSERT_CB(cb,0);
  assert (k >= 0 && k < cb->num_dens);
  assert (cb->cbnew_set->phase == WORKING_PHASE);
  assert (cb->cbnew_set->train_parmats != 0);
  assert (cb->accu_flag == ACCUS_LOCKED);
  assert (cb->active[k]);
  if (!pc_buff) {
    int i;
    for (i = 0; i < cb->parmat[k].mat->num_block; i++)
      assert (cb->parmat[k].mat->list[i]->m == pm_block[i]->m);
  } else
    assert (pc_size);
  assert (cb->accu_gamma_sum[k] > MEAN_UPDATE_THRES);
  assert (diag);
#endif

  cb_num_dim = cb->cbnew_set->num_dim;
  pmat = cb->parmat[k].mat;
  beta = cb->accu_gamma_sum[k];
  for (j = 0,temp = 0.00; j < pmat->num_rel; j++)
    temp += log (diag[j]);
  for (j = pmat->num_rel; j < cb_num_dim; j++)
    temp += log ((double) cb->diag->matPA[k][pmat->dim_vect[j]]);
  klsum = temp * beta;
  if (beta >= DIAG_UPDATE_THRES)
    klsum += (beta * (double) cb_num_dim);
  else {
    if (pc_buff) {
      /* We use the precomp. arrays 'pc_buff' and 'pc_size' (see
	 cbnewparmatUpdate) */
      for (i = icum = dcnt2 = 0; i < pmat->num_block; i++,icum += d) {
	d = pmat->list[i]->m;
	cov = cb->accu_cov[k].block[i];
	size = pc_size[i];
	for (i2 = 0; i2 < d; i2++) {
	  sum = 0.00;
	  for (dcnt = 0; dcnt < size; dcnt++)
	    sum += (pc_buff[dcnt2++] * cov[dcnt]);
	  klsum += (sum / diag[icum+i2]);
	}
      }
    } else {
      for (i = icum = 0; i < pmat->num_block; i++,icum += d) {
	d = pmat->list[i]->m;
	cov = cb->accu_cov[k].block[i];
	for (i2 = 0; i2 < d; i2++) {
	  pmat_row = pm_block[i]->matPA[i2];
	  sum = 0.00;
	  for (j = dcnt = 0; j < d; j++) {
	    factor = pmat_row[j];
	    sum += (factor * factor * cov[dcnt++]);
	    factor *= 2.00;
	    for (l = j+1; l < d; l++,dcnt++)
	      sum += (factor * pmat_row[l] * cov[dcnt]);
	  }
	  klsum += (sum / diag[icum+i2]);
	}
      }
    }
    for (j = icum; j < cb_num_dim; j++)
      klsum += (cb->accu_cov[k].diag[j-icum] /
		(double) cb->diag->matPA[k][pmat->dim_vect[j]]);
  }

  return -0.50 * klsum;
}

/* -------------------------------------------------------------------
   reestimate_diag_optimized
   Reestimates the diagonal variance matrix of Gaussian. The result
   is returned via 'diag' (i.e. overwrites the old values).
   You should use these values only if the exp. number of samples
   of the Gaussian exceeds DIAG_UPDATE_THRES.
   Note that you don't have to use the Gaussian's actual parmat in
   the calculations. However the Gaussian must have
   a parmat with the same structure as the parmat used in the calc.
   IMPORTANT:
   - the parmat is (implicitely) given by the precomputed arrays
     'pc_buff' and 'pc_size' (see 'cbnewparmatUpdate'). The parmat
     value to use here need not be the value of the parmat curr.
     linked to the Gaussian
   - instead of the relevant comp. of 'cb->diag->matPA[k]', we use
     'diag' for the Gaussian's variance matrix. Note that 'diag[j]'
     corr. to 'cb->diag->matPA[k][dim_vect[j]]' where 'dim_vect' is
     the dim. index vector of the parmat. The irrelevant comp. of
     the variance matrix are taken from 'cb->diag'. Note that
     'diag' has double coeff. 'diag' is used as input/output, the
     entries are replaced by the new ML estimates given the parmat.
     The irrelevant components are left unchanged (assuming them to
     already be ML estimates).
   ------------------------------------------------------------------- */
static void reestimate_diag_optimized (CBNew* cb,int k,double* diag,
				       double* pc_buff,int* pc_size)
{
  register int i,i2,icum,dcnt,dcnt2,d,size;
  double* cov;
  double sum,beta;
  CBNewParMatrix* pmat;

#ifdef DEBUG
  ASSERT_CB(cb,0);
  assert (k >= 0 && k < cb->num_dens);
  assert (cb->cbnew_set->phase == WORKING_PHASE);
  assert (cb->cbnew_set->train_parmats != 0);
  assert (cb->accu_flag == ACCUS_LOCKED);
  assert (cb->active[k]);
  assert (pc_buff);
  assert (pc_size);
  assert (cb->accu_gamma_sum[k] > MEAN_UPDATE_THRES);
  assert (diag);
#endif

  pmat = cb->parmat[k].mat;
  beta = cb->accu_gamma_sum[k];
  for (i = icum = dcnt2 = 0; i < pmat->num_block; i++) {
    d = pmat->list[i]->m;
    cov = cb->accu_cov[k].block[i];
    size = pc_size[i];
    for (i2 = 0; i2 < d; i2++,icum++) {
      sum = 0.00;
      for (dcnt = 0; dcnt < size; dcnt++)
	sum += (pc_buff[dcnt2++] * cov[dcnt]);
      sum /= beta;
      if (sum < DIAG_COMP_FLOOR)
	sum = DIAG_COMP_FLOOR;  /* fix to floor value */
      diag[icum] = sum;
    }
  }
}

/* -------------------------------------------------------------------
   cluster_ml
   Phase: working.
   Plug-in clustering function using ML distance measure. For each
   Gaussian, we compute the distance to all reference matrices and
   classify the Gaussian to the class of the nearest reference.
   Each Gaussian in the list 'gauss' must be active, have locked
   accus and more than MEAN_UPDATE_THRES samples.
   The result is returned via 'iarray' and 'first_ind' (see
   'cbnewpmsetCluster'). The number of Gaussians NOT classified to
   their actual parameter matrix is returned via 'changesN'.
   All parameter matrices must have the same block structure and the
   same dimension index vector. This is not checked!
   'hfield' contains a (quite large!) helper field for each class
   and is used to speed up the distance calculations. 'hfield' would
   normally be indexed 'hfield[class][block]', but we use linear
   storage. Note that 'hfield[class]' contains the same precomp.
   information about the parmat repr. 'class' as 'pc_buff' in
   'cbnewparmatUpdate'.
   ------------------------------------------------------------------- */
static int cluster_ml (CBNewParMatrixSet* cd,cluster_gaussian* gauss,
		       int* iarray,int* first_ind,int num_gauss,
		       int* changesN)
{
  register int i,i2,j,k,l,gind,d,dcnt,num_block,index=0;
  register int cb_ind,num_rel,total_size;
  double* hfield,*diag;
  float* pmat_row;
  double temp,max=0.0,beta;
  int* block_size,*dim_vect;
  CBNew* cb;
  char robust_flag;

#ifdef DEBUG
  ASSERT_PMSET(cd,1);
  assert (cd->list.itemN > 0);
  assert (cd->cbnew_set && cd->cbnew_set->phase == WORKING_PHASE);
  assert (cd->cbnew_set->train_parmats != 0);
  assert (gauss);
  assert (iarray);
  assert (first_ind);
  assert (num_gauss > 0);
  assert (changesN);
#endif

  /* Allocation of temp. arrays */
  num_block = cd->list.itemA[0].num_block;
  dim_vect = cd->list.itemA[0].dim_vect;
  num_rel = cd->list.itemA[0].num_rel;
  GETMEM(block_size,int,num_block);
  for (i = total_size = 0; i < num_block; i++) {
    d = cd->list.itemA[0].list[i]->m;
    block_size[i] = j = (d*(d+1))/2;
    total_size += (d*j);
  }
  GETMEMX(hfield,double,total_size * cd->list.itemN,free (block_size));
  if (!(diag = (double*) malloc (num_rel * sizeof(double)))) {
    ERROR("Not enough memory!\n");
    free (hfield);
    free (block_size);
    return TCL_ERROR;
  }

  /* Precomputation => 'hfield'. This involves a run over all parameter
     matrices */
#ifdef DEBUG2
  WARN("Precomputation...\n");
#endif
  for (k = dcnt = 0; k < cd->list.itemN; k++) {
    for (i = 0; i < num_block; i++) {
      d = cd->list.itemA[0].list[i]->m;
      for (i2 = 0; i2 < d; i2++) {
	pmat_row = cd->list.itemA[k].list[i]->matPA[i2];
	for (j = 0; j < d; j++) {
	  temp = (double) pmat_row[j];
	  hfield[dcnt++] = temp * temp;
	  temp *= 2.00;
	  for (l = j+1; l < d; l++)
	    hfield[dcnt++] = temp * (double) pmat_row[l];
	}
      }
    }
  }

  /* Classification of each Gaussian */
  *changesN = 0;
  for (k = 0; k < cd->list.itemN; k++) first_ind[k] = -1;

  for (gind = 0; gind < num_gauss; gind++) {
    cb = gauss[gind].cb;
    cb_ind = gauss[gind].cb_ind;
    beta = cb->accu_gamma_sum[cb_ind];
    robust_flag = (beta >= DIAG_UPDATE_THRES) ? 1 : 0;
    if (robust_flag == 0)
      /* Initialize 'diag' with relevant comp.'s values */
      for (i = 0; i < num_rel; i++)
	diag[i] = (double) cb->diag->matPA[cb_ind][dim_vect[i]];
    /* Run over all parmats */
    for (k = dcnt = 0; k < cd->list.itemN; k++,dcnt += total_size) {
      if (robust_flag == 1)
	reestimate_diag_optimized (cb,cb_ind,diag,&hfield[dcnt],block_size);
      temp = eval_kl_gaussian_optimized (cb,cb_ind,0,diag,&hfield[dcnt],
					 block_size);
      if (!k || temp > max) {
	max = temp;
	index = k;
      }
    }
    gauss[gind].index = index;
    /* insert in front of list */
    iarray[gind] = first_ind[index];
    first_ind[index] = gind;
    if (&(cd->list.itemA[index]) != cb->parmat[cb_ind].mat) (*changesN)++;
  }

  /* Cleanup */
  free (hfield);
  free (block_size);
  free (diag);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   init_class_representatives
   Phase: working.
   Initialize parmats (as class representatives) before a "first"
   clustering step takes place. For each parmat one of the Gaussians
   of 'gauss' is choosen at random. The Gaussian is rejected if it
   has less than 'samplesThres' samples (all Gaussians in the list
   have locked accus). We perform SVD on the covariance matrix of
   the Gaussian and initialize the parmat value with the transpose
   of the rotation matrix.
   Note that we perform sampling with replacement on 'gauss' such that
   it's theoretically possible to initialize two different parmats
   with the same value. To avoid this the cardinality of 'gauss' should
   always be much larger than the number of parmats.
   ------------------------------------------------------------------- */
static int init_class_representatives (CBNewParMatrixSet* cd,
                                       cluster_gaussian* gauss,int num_gauss,
                                       double samplesThres)
{
  register int i,j,k,l,dcnt,index,timeout,cb_ind,d;
  CBNew* cb;
  CBNewParMatrix* pmat;
  DMatrix* covar,*diag,*dummy;
  double* cov;
  double beta;

#ifdef DEBUG
  ASSERT_PMSET(cd,0);
  assert (gauss);
  assert (num_gauss > 0);
  assert (samplesThres >= 0.00);
#endif

  /* Initialization */
  covar = dmatrixCreate (0,0);
  diag = dmatrixCreate (0,0);
  dummy = dmatrixCreate (0,0);

  /* Run over all parmats */
#ifdef DEBUG2
  WARN("Initialization of representatives\n");
#endif
  for (k = 0; k < cd->list.itemN; k++) {
    /* Draw one of the Gaussians at random */
    timeout = 0;
    do {
      if (++timeout > 1000) {
	ERROR("Error while sampling. 'samplesThres' too large.\n");
	return TCL_ERROR;
      }
      index = floor (chaosRandom (0.0,(float) num_gauss));
      if (index == num_gauss) index--;
      if (gauss[index].cb->accu_gamma_sum[gauss[index].cb_ind] <
	  samplesThres) index = -1;
    } while (index == -1);

    /* perform SVD on covariance matrix */
    pmat = &(cd->list.itemA[k]);
    cb = gauss[index].cb;
    cb_ind = gauss[index].cb_ind;
    beta = cb->accu_gamma_sum[cb_ind];
    for (i = 0; i < pmat->num_block; i++) {
      d = pmat->list[i]->m;
      dmatrixResize (covar,d,d);
      dmatrixResize (diag,1,d);
      dmatrixResize (dummy,d,d);
      cov = cb->accu_cov[cb_ind].block[i];
      for (j = dcnt = 0; j < d; j++)
        for (l = j; l < d; l++)
          covar->matPA[j][l] = covar->matPA[l][j] = cov[dcnt++] / beta;
      dmatrixSVD (covar,diag,dummy);
      for (j = 0; j < d; j++)
	for (l = 0; l < d; l++)
	  pmat->list[i]->matPA[j][l] = (float) covar->matPA[l][j];
    }
    pmat->def = 0; /* clear default flag */
  }

  return TCL_OK;
}


/* -------------------------------------------------------------------
   Interface CBNewParMatrix
   ------------------------------------------------------------------- */

/* --- Internal functions --- */

/* -------------------------------------------------------------------
   cbnewparmatInitEmpty
   ------------------------------------------------------------------- */
int cbnewparmatInitEmpty (CBNewParMatrix* cd,ClientData name)
{
#ifdef DEBUG
  assert (cd);
  assert (name);
#endif

  if (!(cd->name = strdup ((char*) name))) {
    ERROR("Out of memory!\n");
    return TCL_ERROR;
  }
  cd->useN       = 0; 
  cd->active     = 1;
  cd->num_block  = 0;
  cd->num_rel    = 0;
  cd->learn_rate = 0.0;
  cd->parmat_set = 0;
  cd->def        = 0;
  cd->list       = 0;
  cd->dim_vect   = 0;
  cd->link       = 0;
  cd->linklist_size = cd->linklist_num = 0;
  cd->trans_sample = 0;
  cd->ts_valid     = 0;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewparmatInit
   ------------------------------------------------------------------- */
int cbnewparmatInit (CBNewParMatrix* cd,char* name,int num_block,
		     CBNewParMatrixSet* parmat_set,int* size_vect,
		     int* dim_vect,float learn_rate)
{
  int i,num_rel;

#ifdef DEBUG
  assert (name);
  ASSERT_PMSET(parmat_set,0);
  assert (num_block > 0 && num_block <= parmat_set->num_dim);
  assert (learn_rate > 0.0);
  assert (size_vect);
  {
    int i,j;
    for (i = j = 0; i < num_block; i++) {
      assert (size_vect[i] > 0);
      j += (int) size_vect[i];
    }
    assert (j <= parmat_set->num_dim);
    if (dim_vect) {
      for (i = j = 0; i < parmat_set->num_dim; i++) {
        assert (dim_vect[i] >= 0 &&
	        dim_vect[i] < parmat_set->num_dim);
	j += (int) dim_vect[i];
      }
      i = parmat_set->num_dim;
      assert (j == (i*(i-1))/2);
    }
  }
#endif

  FREE(cd->name);
  if (cbnewparmatInitEmpty (cd,(ClientData) name) == TCL_ERROR)
    return TCL_ERROR;

  /* Simple components. Allocate block list. */
  cd->num_block = num_block;
  cd->learn_rate = learn_rate;
  cd->parmat_set = parmat_set;
  GETMEM(cd->list,FMatrix*,num_block);
  for (i = 0; i < num_block; i++) cd->list[i] = 0;

  /* Allocate and init. parameter matrix blocks and dimension vector. Set
     'num_rel' component. */
  for (i = num_rel = 0; i < num_block; i++) {
    if (!(cd->list[i] = (FMatrix*) fmatrixCreate (size_vect[i],
						  size_vect[i]))) {
#ifdef DEBUG
      ERROR("<= called by\n");
#endif
      cbnewparmatDeinit (cd);
      return TCL_ERROR;
    }
    num_rel += size_vect[i];
  }
  cd->num_rel = num_rel;
  GETMEMX(cd->dim_vect,int,parmat_set->num_dim,cbnewparmatDeinit (cd));
  if (dim_vect) {
    for (i = 0; i < parmat_set->num_dim; i++)
      cd->dim_vect[i] = dim_vect[i];
  } else {
    for (i = 0; i < parmat_set->num_dim; i++)
      cd->dim_vect[i] = i;
  }
  cbnewparmatDefault (cd);

  /* Tenmporary fields */
  GETMEMX(cd->trans_sample,float,parmat_set->num_dim,cbnewparmatDeinit (cd));
  cd->ts_valid = 0;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewparmatCreate
   ------------------------------------------------------------------- */
CBNewParMatrix* cbnewparmatCreate (char* name,int num_block,
				   CBNewParMatrixSet* parmat_set,
				   int* size_vect,int* dim_vect,
				   float learn_rate)
{
  CBNewParMatrix* cd;

#ifdef DEBUG
  assert (name);
#endif

  if (!(cd = (CBNewParMatrix*) malloc (sizeof(CBNewParMatrix)))) {
    ERROR("Out of memory!\n");
    return 0;
  }
  cd->name = 0;
  if (cbnewparmatInit (cd,name,num_block,parmat_set,size_vect,dim_vect,
		       learn_rate) == TCL_ERROR) {
    free (cd);
    return 0;
  }

  return cd;
}

/* -------------------------------------------------------------------
   cbnewparmatDeinit
   ------------------------------------------------------------------- */
int cbnewparmatDeinit (CBNewParMatrix* cd)
{
  int i;

#ifdef DEBUG
  assert (cd);
#endif

  if (cd->useN) {
    ERROR("There exist %d references to this object!\n",cd->useN);
    return TCL_ERROR;
  }
  if (cd->linklist_num) {
    ERROR("Parameter matrix is linked to %d gaussians!\n",cd->linklist_num);
    return TCL_ERROR;
  }

  if (cd->parmat_set) cd->parmat_set = 0;
  FREE(cd->name);
  FREE(cd->link);
  FREE(cd->dim_vect);
  if (cd->list) {
    for (i = 0; i < cd->num_block; i++)
      FREEFMAT(cd->list[i]);
    FREE(cd->list);
  }
  FREE(cd->trans_sample);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewparmatDestroy
   ------------------------------------------------------------------- */
int cbnewparmatDestroy (CBNewParMatrix* cd)
{
#ifdef DEBUG
  assert (cd);
#endif

  if (cbnewparmatDeinit (cd) == TCL_ERROR) return TCL_ERROR;
  free (cd);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewparmatDefault
   We initialize all blocks of the parameter matrix with I.
   The constructor uses this method with temp. inconsistent objects,
   so we don't check assertions here!
   ------------------------------------------------------------------- */
int cbnewparmatDefault (CBNewParMatrix* cd)
{
  int i;

#ifdef DEBUG
  assert (cd);
#endif

  for (i = 0; i < cd->num_block; i++)
    fmatrixInitDiag (cd->list[i],1.0);
  cd->def = 1;  /* set default flag */

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewparmatPuts
   ------------------------------------------------------------------- */
int cbnewparmatPuts (CBNewParMatrix* cd)
{

#ifdef DEBUG
  ASSERT_PARMAT(cd,1);
#endif

  itfAppendResult("{name %s} {useN %d} {blockN %d} {relCompN %d} {learnRate %f} {parmatSet %s}",
		  cd->name,cd->useN,cd->num_block,cd->num_rel,cd->learn_rate,
		  cd->parmat_set->name);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewparmatLink
   We allow temporary inconsistencies in the link structure of the
   CBNew object 'cb'. Therefore we don't use assertions.
   ------------------------------------------------------------------- */
int cbnewparmatLink (CBNewParMatrix* cd,CBNew* cb,int cb_ind,int* pm_ind)
{
  CBNewParMatrixLink* copy;

#ifdef DEBUG
  ASSERT_PARMAT(cd,0);
  assert (cb);
  assert (cd->parmat_set->cbnew_set == cb->cbnew_set);
  assert (cb_ind >= 0 && cb_ind < cb->num_dens);
#endif

  if (cd->linklist_num >= cd->linklist_size) {
    /* Increase link list size */
    cd->linklist_size += LINK_LIST_INCR_SIZE;
    if (!(copy = (CBNewParMatrixLink*)
	  realloc (cd->link,cd->linklist_size *
		   sizeof(CBNewParMatrixLink)))) {
      ERROR("Out of memory!\n");
      cd->linklist_size -= LINK_LIST_INCR_SIZE;
      return TCL_ERROR;
    }
    cd->link = copy;
  }
  cd->link[cd->linklist_num].cb = cb;
  cd->link[cd->linklist_num].index = cb_ind;
  *pm_ind = cd->linklist_num++;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewparmatUnlink
   We allow temporary inconsistencies in the link structure of the
   CBNew object. Therefore we don't use assertions.
   ------------------------------------------------------------------- */
int cbnewparmatUnlink (CBNewParMatrix* cd,int index)
{
#ifdef DEBUG
  ASSERT_PARMAT(cd,0);
#endif

  if (index < 0 || index >= cd->linklist_num) {
    ERROR("Parameter matrix link list has no entry %d!\n",index);
    return TCL_ERROR;
  }
  cd->link[index] = cd->link[--(cd->linklist_num)];
  cd->link[index].cb->parmat[cd->link[index].index].index = index;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewparmatUpdate_cleanup
   Frees all data structures allocated for 'cbnewparmatUpdate'.
   ------------------------------------------------------------------- */
static void cbnewparmatUpdate_cleanup (CBNewParMatrix* cd,DMatrix*** hmat,
				       DMatrix*** hvect,int*** hindex,
				       int** hind_size,DMatrix** pm_old,
				       DMatrix** pm_new,double* pc_buff,
				       int* pc_size,
				       CBNewParMatrixLink* gauss,
				       DMatrixX* pm_inv,DMatrix* dummy_mat,
				       DMatrixX* pm_trans,double* evect,
				       DMatrixGen* mask1,DMatrixGen* mask2,
				       double* epsvect,double* copyvect,
				       double* diag_copy)
{
  register int i,j,d;

  if (hmat) {
    for (i = 0; i < cd->num_block; i++) {
      if (hmat[i]) {
	d = cd->list[i]->m;
	for (j = 0; j < d; j++)
	  FREEDMAT(hmat[i][j]);
	free (hmat[i]);
      }
    }
    free (hmat);
  }
  if (hvect) {
    for (i = 0; i < cd->num_block; i++) {
      if (hvect[i]) {
	d = cd->list[i]->m;
	for (j = 0; j < d; j++)
	  FREEDMAT(hvect[i][j]);
	free (hvect[i]);
      }
    }
    free (hvect);
  }
  if (hindex) {
    for (i = 0; i < cd->num_block; i++) {
      if (hindex[i]) {
	d = cd->list[i]->m;
	for (j = 0; j < d; j++)
	  FREE(hindex[i][j]);
	free (hindex[i]);
      }
    }
    free (hindex);
  }
  if (hind_size) {
    for (i = 0; i < cd->num_block; i++)
      FREE(hind_size[i]);
    free (hind_size);
  }
  if (pm_old) {
    for (i = 0; i < cd->num_block; i++)
      FREEDMAT(pm_old[i]);
    free (pm_old);
  }
  if (pm_new) {
    for (i = 0; i < cd->num_block; i++)
      FREEDMAT(pm_new[i]);
    free (pm_new);
  }
  FREE(pc_buff);
  FREE(pc_size);
  FREE(gauss);
  if (pm_inv)   dmatrixxDestroy (pm_inv);
  if (pm_trans) dmatrixxDestroy (pm_trans);
  FREEDMAT(dummy_mat);
  FREE(evect);
  FREE(epsvect);
  FREE(copyvect);
  FREE(diag_copy);
  if (mask1) dmatrixgenDestroy (mask1);
  if (mask2) dmatrixgenDestroy (mask2);
}

#define GETMEMUP(var,type,size)\
{\
  if (!((var) = (type*) malloc ((size) * sizeof(type)))) {\
    ERROR("Out of memory!\n");\
    cbnewparmatUpdate_cleanup (cd,hmat,hvect,hindex,hind_size,pm_old,pm_new,\
			       pc_buff,pc_size,gauss,pm_inv,dummy_mat,\
			       pm_trans,evect,mask1,mask2,epsvect,copyvect,\
			       diag_copy);\
    return TCL_ERROR;\
  }\
}

/* -------------------------------------------------------------------
   cbnewparmatUpdate
   Phase: working. The 'trainParmats' flag of the assoc. codebook
   set must be set.
   Updates the parmat using an iterative scheme. An iteration step
   can either be "small" or "large". A large step is a small step
   followed by a ML reestimation of the diagonal variance matrices
   of the assoc. Gaussians and a recalculation of the H^(i) helper
   matrices (whose values depend on the variance matrices).
   'small_steps', 'first_small_steps', 'upperb' and 'delta_thres'
   control the total number of iterations and the number of small
   steps done between two large steps.

   The method assumes the diags being consistent (as precondition)
   and reestimates them before returning. The scheme of one loop:
   - Calc. the helper matrices H^(i)
   - Perform n small steps (n is 'first_small_steps' in the first
     loop, 'small_steps' else). The old PM value is not
     overwritten.
   - Normalize the PM blocks to unit determinant
   - Reestimate the diags and calculate the new EM criterion
     value. The old diags are not overwritten.
     This step is very costly and is speeded up by using a
     large precomputed array.
   - If the new criterion value is worse than the old one, we
     leave the loop and use the old values of the diags and the
     PM.
   - Replace old by new values
   - If less than 'upperb' small steps have done so far and the
     difference between the new crit. value and the old one is
     bigger than 'delta_thres', we perform another loop, else
     leave.

   Almost singular helper matrices H^(i) can lead to numerical
   inaccuracies. We avoid this by performing SVD on H^(i),
   transform the PM's row i into the base spanned by the eigenvectors
   and then update only those components which corr. to large enough
   eigenvalues.

   There are several ways to update the PM during a small step.
   We've coded three of them, more or less based on the Gales update
   scheme:
   - Version 1: Update the PM row by row. After each row update,
     the inverse of the PM is reestimated (using a simple O(n^2)
     method). Row update uses the Gales scheme.
   - Version 2: Like Version 1, but row update is simplified and
     somehow more reasonable.
   - Version 3: We don't reestimate the PM's inverse after each
     row update. This is mathematically incorrect, but may be a
     reasonable approximation.
   Version x is enabled by setting constant PM_UPDATE_VERSION_x.
   You must set exactly one of these constants.
   scheme is referred to as "small step". A "large step" involves
   reestimating the diagonal variance matrices of the assoc.
   Gaussians and the recalculation of the helper matrices 'hmat'
   used during small steps.

   Temp. arrays:
   - pm_old: Values of old parmat blocks
   - pm_new: Values of new parmat blocks
   - diag_copy: Copy of the diagonal variance matrices of the
     Gaussians. Flat organisation, the vectors are stored here
     in the same order as their Gaussians are stored in 'gauss'.
   - gauss: List of Gaussians curr. involved in update of parmat
   ------------------------------------------------------------------- */

/* Florian: otherwise the code produces bogus results on icc */
#pragma optimization_level 2

int cbnewparmatUpdate (CBNewParMatrix* cd,int upperb,int small_steps,
		       int first_small_steps,double delta_thres,
		       int* steps_done)
{
  register int d=0,j=0,l=0,dcnt=0,dcnt2=0,i=0,i2=0,icum=0,k=0;

  int index=0,size=0,dv_size=0,dc_ind=0;
  int num_gauss=0,ss_bound=0,num_robust_gauss=0;

  /* counts total number of small steps */
  int       counter = 0;

  double      alpha = 0.0;
  double       beta = 0.0;
  double      gamma = 0.0;
  double      denom = 0.0;
  double     factor = 0.0;

  double        sum = 0.0;
  double       sum2 = 0.0;
  double     q_crit = 0.0;
  double q_crit_old = 0.0;
 
  double*       cov = NULL;
  double*        hm = NULL;
  double*      pmat = NULL;
  double*   pc_buff = NULL;

  int*      pc_size = NULL;
  int*     dim_vect = NULL;
  float*       diag = NULL;

  CBNew*         cb = NULL;
  DMatrix**  pm_new = NULL;
  DMatrix**  pm_old = NULL;
  DMatrix***   hmat = NULL;
  DMatrix***  hvect = NULL;

  int***      hindex = NULL;
  int**    hind_size = NULL;
  DMatrix* dummy_mat = NULL;

  double*      evect = NULL;
  double*    epsvect = NULL;
  double*   copyvect = NULL;
  double*  diag_copy = NULL;

  DMatrixX*   pm_inv = NULL;
  DMatrixX* pm_trans = NULL;
  DMatrixGen*  mask1 = NULL;
  DMatrixGen*  mask2 = NULL;
   
  CBNewParMatrixLink* gauss = 0; /* Gaussians involved */


#ifdef DEBUG2
  double det_dummy=0.0, min_dummy=0.0, max_dummy=0.0, sign_dummy=0.0;
  DMatrixGen* testmask;
#endif

#ifdef DEBUG
  ASSERT_PARMAT(cd,1);
  assert (upperb >= 0);
  assert (small_steps > 0);
  assert (first_small_steps > 0);
  assert (cd->parmat_set->cbnew_set &&
	  (cd->parmat_set->cbnew_set->phase == WORKING_PHASE) &&
	  (cd->parmat_set->cbnew_set->train_parmats != 0));
  {
    int i;
    for (i = 0; i < cd->linklist_num; i++)
      assert (cd->link[i].cb->accu_flag != ACCUS_BUILDING);
  }
#endif

  /* -------------------------
     Allocate temporary arrays
     ------------------------- */
  d = cd->parmat_set->num_dim;
  if (!(pm_inv = dmatrixxCreate (d,d)) ||
      !(pm_trans = dmatrixxCreate (d,d)) ||
      !(dummy_mat = dmatrixCreate (d,d))) {
    ERROR("Out of memory!\n");
    cbnewparmatUpdate_cleanup (cd,hmat,hvect,hindex,hind_size,pm_old,pm_new,
			       pc_buff,pc_size,gauss,pm_inv,dummy_mat,
			       pm_trans,evect,mask1,mask2,epsvect,copyvect,
			       diag_copy);
    return TCL_ERROR;
  }
  if (!(mask1 = dmatrixgenCreateX (pm_inv)) ||
      !(mask2 = dmatrixgenCreate (dummy_mat))) {
    ERROR("Out of memory!\n");
    cbnewparmatUpdate_cleanup (cd,hmat,hvect,hindex,hind_size,pm_old,pm_new,
			       pc_buff,pc_size,gauss,pm_inv,dummy_mat,
			       pm_trans,evect,mask1,mask2,epsvect,copyvect,
			       diag_copy);
    return TCL_ERROR;
  }
  GETMEMUP(evect,double,d);
  GETMEMUP(epsvect,double,d);
  GETMEMUP(copyvect,double,d);
  GETMEMUP(hmat,DMatrix**,cd->num_block);
  GETMEMUP(hvect,DMatrix**,cd->num_block);
  GETMEMUP(hindex,int**,cd->num_block);
  GETMEMUP(hind_size,int*,cd->num_block);
  GETMEMUP(pm_old,DMatrix*,cd->num_block);
  GETMEMUP(pm_new,DMatrix*,cd->num_block);
  for (i = 0; i < cd->num_block; i++) {
    hmat[i] = 0;
    hvect[i] = 0;
    hindex[i] = 0;
    hind_size[i] = 0;
    pm_old[i] = 0;
    pm_new[i] = 0;
  }
  GETMEMUP(pc_size,int,cd->num_block);
  for (i = size = 0; i < cd->num_block; i++) {
    d = cd->list[i]->m;
    pc_size[i] = j = (d*(d+1))/2;
    size += (d*j);
    if (!(pm_old[i] = dmatrixCreate (d,d)) ||
	!(pm_new[i] = dmatrixCreate (d,d))) {
      ERROR("Out of memory!\n");
      cbnewparmatUpdate_cleanup (cd,hmat,hvect,hindex,hind_size,pm_old,pm_new,
				 pc_buff,pc_size,gauss,pm_inv,dummy_mat,
				 pm_trans,evect,mask1,mask2,epsvect,copyvect,
				 diag_copy);
      return TCL_ERROR;
    }
    GETMEMUP(hmat[i],DMatrix*,d);
    GETMEMUP(hvect[i],DMatrix*,d);
    GETMEMUP(hindex[i],int*,d);
    GETMEMUP(hind_size[i],int,d);
    for (i2 = 0; i2 < d; i2++) {
      hmat[i][i2] = 0;
      hvect[i][i2] = 0;
      hindex[i][i2] = 0;
    }
    for (i2 = 0; i2 < d; i2++) {
      if (!(hmat[i][i2] = dmatrixCreate (d,d)) ||
	  !(hvect[i][i2] = dmatrixCreate (1,d))) {
	ERROR("Not enough memory!\n");
	cbnewparmatUpdate_cleanup (cd,hmat,hvect,hindex,hind_size,pm_old,pm_new,
				   pc_buff,pc_size,gauss,pm_inv,dummy_mat,
				   pm_trans,evect,mask1,mask2,epsvect,copyvect,
				   diag_copy);
	return TCL_ERROR;
      }
      GETMEMUP(hindex[i][i2],int,d);
    }
  }
  GETMEMUP(pc_buff,double,size);
  GETMEMUP(gauss,CBNewParMatrixLink,cd->linklist_num);

  /* ---------------------------------------------------------
     Collect all Gaussians who take part in the parmat update.
     These must:
     - be active (active[x] = 1)
     - have locked accus
     - have accuGammaS > MEAN_UPDATE_THRES
     'num_gauss' is the number of such Gaussians,
     'num_robust_gauss' the size of the subset of Gaussians
     with >= DIAG_UPDATE_THRES samples (DEBUG only).
     --------------------------------------------------------- */
  alpha = 0.00;  /* number of samples of all Gaussians */
  num_gauss = num_robust_gauss = 0;
  for (k = 0; k < cd->linklist_num; k++) {
    cb = cd->link[k].cb;
    index = cd->link[k].index;
    if (cb->active[index] && (cb->accu_flag == ACCUS_LOCKED) &&
	((beta = cb->accu_gamma_sum[index]) > MEAN_UPDATE_THRES)) {
      alpha += beta;
      gauss[num_gauss].cb = cb;
      gauss[num_gauss++].index = index;
      if (beta >= DIAG_UPDATE_THRES) num_robust_gauss++;
    }
  }
  if (num_gauss == 0) {
    /* Nothing to do: All codebooks linked to the parmat have empty accus
       or too few samples to allow robust update of the parmat! */
    cbnewparmatUpdate_cleanup (cd,hmat,hvect,hindex,hind_size,pm_old,pm_new,
			       pc_buff,pc_size,gauss,pm_inv,dummy_mat,
			       pm_trans,evect,mask1,mask2,epsvect,copyvect,
			       diag_copy);
    return TCL_OK;
  }
#ifdef DEBUG2
  WARN("alpha=%lf, num_gauss=%d, num_robust_gauss=%d\n",alpha,num_gauss,
       num_robust_gauss);
#endif
  GETMEMUP(diag_copy,double,num_gauss * cd->num_rel);

  /* -----------------------------------------------------
     Initialization of temp. arrays:
     - 'pm_new', 'pm_old' are init. with the parmat blocks
     - 'diag_copy' is init. with the diagonal variance
       matrices
     ----------------------------------------------------- */
  for (i = 0; i < cd->num_block; i++) {
    d = cd->list[i]->m;
    for (j = 0; j < d; j++)
      for (l = 0; l < d; l++) {
	pm_old[i]->matPA[j][l] = pm_new[i]->matPA[j][l] =
	  (double) cd->list[i]->matPA[j][l];
      }
  }
  for (k = dc_ind = 0; k < num_gauss; k++)
    for (j = 0; j < cd->num_rel; j++)
      diag_copy[dc_ind++] = (double)
	gauss[k].cb->diag->matPA[gauss[k].index][cd->dim_vect[j]];

#ifdef DEBUG2
  for (i = 0; i < cd->num_block; i++) {
    dmatrixExtDet (pm_old[i],&det_dummy,&sign_dummy);
    WARN("Parmat block %d: log det=%lf\n",i,det_dummy);
  }
#endif

  ss_bound = first_small_steps;


  /* -------------------------------------------
     Step loop (see schematic description above)
     ------------------------------------------- */
  do {
    /* --------------------------------------------------------------
       Calculate the 'hmat' helper matrices (2nd part of large step).
       In case of 'counter' = 0 (i.e. no small steps so far) we also
       calc. the Q crit. and store it in 'q_crit_old'.
       We perform SVD on the H^(i) helper matrix, H^(i) = U D U^t,
       where U is orthonormal and D contains the eigenvalues. We
       store U in hmat[i] and D in hvect[i]. hindex[i] contains the
       numbers of the dimensions of not too low eigenvalues in D,
       hind_size[i] the number of such dimensions.
       Update of row i during small steps will only affect the
       dimensions of hindex[i] (the row is transformed by hmat[i]
       first).
       -------------------------------------------------------------- */
    if (!counter)
      q_crit_old = 0.00;
    else
      q_crit_old = q_crit; /* Copy old value */

    for (i = 0; i < cd->num_block; i++) {
      d = cd->list[i]->m;
      for (i2 = 0; i2 < d; i2++)
	for (j = 0; j < d; j++)
	  for (l = j; l < d; l++)
	    hmat[i][i2]->matPA[j][l] = 0.00;
    }
    for (k = dc_ind = 0; k < num_gauss; k++) {
      cb = gauss[k].cb;
      index = gauss[k].index;
      if (!counter)
	q_crit_old += eval_kl_gaussian_optimized (cb,index,pm_old,
						  &diag_copy[dc_ind],0,0);
      for (i = icum = 0; i < cd->num_block; i++) {
	d = cd->list[i]->m;
	cov = cb->accu_cov[index].block[i];
	for (i2 = 0; i2 < d; i2++,icum++) {
	  factor = 1.00 / diag_copy[dc_ind++];
	  for (j = dcnt = 0; j < d; j++) {
	    hm = hmat[i][i2]->matPA[j];
	    for (l = j; l < d; l++,dcnt++)
	      hm[l] += (factor * cov[dcnt]);
	  }
	}
      }
    }
    if (!counter) {
      q_crit_old /= alpha;
#ifdef DEBUG2
      WARN("First run Q crit. value = %lf\n",q_crit_old);
#endif
    }
    for (i = 0; i < cd->num_block; i++) {
      d = cd->list[i]->m;
      for (i2 = 0; i2 < d; i2++)
	for (j = 0; j < d; j++)
	  for (l = j; l < d; l++)
	    hmat[i][i2]->matPA[l][j] = (hmat[i][i2]->matPA[j][l] /= alpha);
    }

    /* ------------------------------------------
       Perform SVD and select "robust" dimensions
       ------------------------------------------ */
    for (i = 0; i < cd->num_block; i++) {
      d = cd->list[i]->m;
      for (i2 = 0; i2 < d; i2++) {
	dmatrixSVD (hmat[i][i2],hvect[i][i2],dummy_mat);
#ifdef DEBUG2
	max_dummy = 0.00;
	for (j = 0; j < d; j++)
	  for (l = 0; l < d; l++) {
	    min_dummy = dummy_mat->matPA[j][l] - hmat[i][i2]->matPA[j][l];
	    if (min_dummy > max_dummy) max_dummy = min_dummy;
	    else if (-min_dummy > max_dummy) max_dummy = -min_dummy;
	  }
	if (min_dummy > 0.000001)
	  WARN("Maximum comp. diff. between U,V (SVD): %lf\n",max_dummy);
#endif
	hind_size[i][i2] = 0;
	for (j = 0; j < d; j++) {
	  if (hvect[i][i2]->matPA[0][j] >= HMAT_SVAL_THRES)
	    hindex[i][i2][hind_size[i][i2]++] = j;
#ifdef DEBUG2
	  else
	    WARN("hmat[%d][%d], sval %d = %lf\n",i,i2,j,
		 hvect[i][i2]->matPA[0][j]);
#endif
	}
      }
    }

    /* ----------------------------------------------------------------
       Small steps loop
       This is the part of code which is largely different for the
       three versions.
       We use the following temp. arrays:
       - pm_inv: Inverse of actual parmat block. In version 1,2
         'pm_inv' is reestimated after update of each parmat block
	 row, in version 3 we recalc. 'pm_inv' after each block
	 update.
       - pm_trans: Rows of 'pm_trans' are the rows of the actual
         parmat block transformed by the corr. 'hmat[.]' matrix
       - evect: If the k-th PM block row is updated, 'evect' contains
         the k-th column of the inverse transformed by the transpose
	 (= inverse) of 'hmat[k]'.
       - copyvect: Copy of PM block row before it's updated (versions
         1,2 only)
       - epsvect: The difference between the updated row and
         'copyvect' is transformed back (by transpose of 'hmat[.]'
	 to give 'epsvect' (versions 1,2 only)
       ---------------------------------------------------------------- */
    for (i = 0; i < cd->num_block; i++) {
      d = cd->list[i]->m;
      dmatrixxResize (pm_trans,d,d);
      dmatrixxResize (pm_inv,d,d);
      dmatrixgenMask (mask2,pm_new[i]);

#ifndef PM_UPDATE_VERSION_3
      /* Compute inverse ('mask1' is mask for 'pm_inv') */
      dmatrixgenInv (mask1,mask2);
#endif

      /* ----------------------
	 Transform parmat block
	 ---------------------- */
      for (k = 0; k < d; k++) {
	pmat = pm_new[i]->matPA[k];
	for (j = 0; j < d; j++) {
	  sum = 0.00;
	  for (l = 0; l < d; l++)
	    sum += (pmat[l] * hmat[i][k]->matPA[l][j]);
	  pm_trans->matPA[k][j] = sum;
	}
      }

      /* -------------------------
	 Small steps for one block
         ------------------------- */

#ifdef PM_UPDATE_VERSION_1
      for (k = 0; k < ss_bound; k++) {
	/* Increase global small steps counter */
	if (i == 0) counter++;
#ifdef DEBUG2
	WARN("Block %d, small step %d\n",i,k);
#endif
	/* loop over the rows of the block */
	for (i2 = 0; i2 < d; i2++) {
	  dim_vect = hindex[i][i2];
	  dv_size = hind_size[i][i2];
	  pmat = pm_trans->matPA[i2];
	  /* Compute 'evect' (by transforming the inverse's column) */
	  for (j = 0; j < d; j++) {
	    sum = 0.00;
	    for (l = 0; l < d; l++)
	      sum += (hmat[i][i2]->matPA[l][j] * pm_inv->matPA[l][i2]);
	    evect[j] = sum;
	  }
	  /* parmat block row update loop. We update only row components
	     whose index appears in 'dim_vect' */
	  for (j = 0,sum = 0.00; j < d; j++) {
	    sum += (pmat[j] * evect[j]);
	    copyvect[j] = pmat[j];  /* copy of block row */
	  }
	  /* Note that 'sum' is just the det. of the parmat block
	     (apart from inaccuracies in the inverse) */
	  for (j = 0; j < dv_size; j++) {
	    index = dim_vect[j];
	    temp = hvect[i][i2]->matPA[0][index];
	    c3 = evect[index];
	    c1 = temp * c3;
	    c2 = temp * (pmat[index] * c3 - sum);
	    pmat[index] = (sqrt (c2*c2 + 4.00*c1*c3) + c2) / (2.00*c1);
	  }
	  /* Transform back the difference between new and old row of the
	     transformed parmat block => 'epsvect' */
	  for (j = 0; j < d; j++) {
	    sum = 0.00;
	    for (l = 0; l < d; l++)
	      sum += ((pmat[l] - copyvect[l]) * hmat[i][i2]->matPA[j][l]);
	    epsvect[j] = sum;
	  }
	  /* ------------------------------------------------------
	     Recalc. of the inverse. Since we've changed only one
	     row, there's a simple and fast "correction" algorithm
	     for the inverse.
	     ------------------------------------------------------ */
	  for (l = 0,sum2 = 1.00; l < d; l++)
	    sum2 += (epsvect[l] * pm_inv->matPA[l][i2]);
	  for (j = 0; j < d; j++)
	    if (j == i2) {
	      for (l = 0,sum = 1.00; l < d; l++)
		sum += (epsvect[l] * pm_inv->matPA[l][j]);
	      for (l = 0; l < d; l++)
		pm_inv->matPA[l][j] /= sum;
	    } else {
	      for (l = 0,sum = 0.00,sum2 = 1.00; l < d; l++)
		sum += (epsvect[l] * pm_inv->matPA[l][j]);
	      sum /= sum2;
	      for (l = 0; l < d; l++)
		pm_inv->matPA[l][j] -= (sum * pm_inv->matPA[l][i2]);
	    }
	}
      }
#endif

#ifdef PM_UPDATE_VERSION_2
      for (k = 0; k < ss_bound; k++) {
	/* Increase global small steps counter */
	if (i == 0) counter++;
#ifdef DEBUG2
	WARN("Block %d, small step %d\n",i,k);
#endif
	/* loop over the rows of the block */
	for (i2 = 0; i2 < d; i2++)
	  if ((dv_size = hind_size[i][i2]) > 0) {
	    dim_vect = hindex[i][i2];
	    pmat = pm_trans->matPA[i2];
	    /* - compute 'evect' (by transforming the inverse)
	       - copy actual block row into 'copyvect'
	       - calc. the 'gamma' value.
	       Note: We assume that the indices in 'dim_vect' are
	       increasing. */
	    gamma = 1.00; denom = 0.00;
	    for (j = index = 0; j < d; j++) {
	      sum = 0.00;
	      for (l = 0; l < d; l++)
		sum += (hmat[i][i2]->matPA[l][j] * pm_inv->matPA[l][i2]);
	      evect[j] = sum;
	      copyvect[j] = pmat[j];
	      if (index < dv_size && j == dim_vect[index]) {
		index++;
		denom += (sum * sum / hvect[i][i2]->matPA[0][j]);
	      } else
		gamma -= (pmat[j] * sum);
	    }
	    gamma /= denom;
	    /* parmat block row update loop. We update only row components
	       whose index appears in 'dim_vect' */
	    for (j = 0; j < dv_size; j++) {
	      index = dim_vect[j];
	      pmat[index] = gamma * evect[index] /
		hvect[i][i2]->matPA[0][index];
	    }
	    /* Transform back the difference between new and old row of the
	       transformed parmat block => 'epsvect' */
	    for (j = 0; j < d; j++) {
	      sum = 0.00;
	      for (l = 0; l < d; l++)
		sum += ((pmat[l] - copyvect[l]) * hmat[i][i2]->matPA[j][l]);
	      epsvect[j] = sum;
	    }
  	    /* ------------------------------------------------------
	       Recalc. of the inverse. Since we've changed only one
	       row, there's a simple and fast "correction" algorithm
	       for the inverse.
	       ------------------------------------------------------ */
	    for (l = 0,sum2 = 1.00; l < d; l++)
	      sum2 += (epsvect[l] * pm_inv->matPA[l][i2]);
	    for (j = 0; j < d; j++)
	      if (j == i2) {
		for (l = 0,sum = 1.00; l < d; l++)
		  sum += (epsvect[l] * pm_inv->matPA[l][j]);
		for (l = 0; l < d; l++)
		  pm_inv->matPA[l][j] /= sum;
	      } else {
		for (l = 0,sum = 0.00,sum2 = 1.00; l < d; l++)
		  sum += (epsvect[l] * pm_inv->matPA[l][j]);
		sum /= sum2;
		for (l = 0; l < d; l++)
		  pm_inv->matPA[l][j] -= (sum * pm_inv->matPA[l][i2]);
	      }
	  }
      }
#endif

#ifdef PM_UPDATE_VERSION_3
      for (k = 0; k < ss_bound; k++) {
	/* We have to (re)compute the inverse before each block
	   update step ('mask1' is mask of 'pm_inv') */
	dmatrixgenInv (mask1,mask2);
	/* Increase global small steps counter */
	if (i == 0) counter++;
#ifdef DEBUG2
	WARN("Block %d, small step %d\n",i,k);
#endif
	/* loop over the rows of the block */
	for (i2 = 0; i2 < d; i2++) {
	  dim_vect = hindex[i][i2];
	  dv_size = hind_size[i][i2];
	  pmat = pm_trans->matPA[i2];
	  /* Compute 'evect' (by transforming the inverse) */
	  for (j = 0; j < d; j++) {
	    sum = 0.00;
	    for (l = 0; l < d; l++)
	      sum += (pm_inv->matPA[l][i2] * hmat[i][i2]->matPA[l][j]);
	    evect[j] = sum;
	  }
	  /* parmat block row update loop. We update only row components
	     whose index appears in 'dim_vect' */
	  for (j = 0,sum = 0.00; j < d; j++) {
	    sum += (pmat[j] * evect[j]);
	  }
	  /* Note that 'sum' is just the det. of the parmat block
	     (apart from inaccuracies) */
	  for (j = 0; j < dv_size; j++) {
	    index = dim_vect[j];
	    temp = hvect[i][i2]->matPA[0][index];
	    c3 = evect[index];
	    c1 = temp * c3;
	    c2 = temp * (pmat[index] * c3 - sum);
	    pmat[index] = (sqrt (c2*c2 + 4.00*c1*c3) + c2) / (2.00*c1);
	  }
	}
      }
#endif

      /* --------------------
	 Transform block back
	 -------------------- */
      for (k = 0; k < d; k++) {
	pmat = pm_trans->matPA[k];
	for (j = 0; j < d; j++) {
	  sum = 0.00;
	  for (l = 0; l < d; l++)
	    sum += (hmat[i][k]->matPA[j][l] * pmat[l]);
	  pm_new[i]->matPA[k][j] = sum;
	}
      }
#ifdef DEBUG2
      testmask = dmatrixgenCreate (dummy_mat);
      dmatrixgenMul (testmask,mask1,mask2);
      max_dummy = 0.00;
      for (j = 0; j < d; j++)
	for (l = 0; l < d; l++) {
	  min_dummy = dummy_mat->matPA[j][l];
	  if (j == l) min_dummy -= 1.00;
	  if (min_dummy > max_dummy) max_dummy = min_dummy;
	  else if (-min_dummy > max_dummy) max_dummy = -min_dummy;
	}
      dmatrixgenDestroy (testmask);
      if (max_dummy > 0.001)
	WARN("Maximum comp. diff. from I: %lf\n",max_dummy);

      for (j = 0; j < cd->num_block; j++) {
	dmatrixExtDet (pm_new[j],&det_dummy,&sign_dummy);
	WARN("Parmat block %d: log det=%lf\n",j,det_dummy);
      }
#endif
    }

    /* -------------------------------------------------------
       Normalize parmat blocks to unit determinant
       In theory this wouldn't be necessary when using version
       2, but the calculations aren't very accurate.
       ------------------------------------------------------- */
#ifdef DEBUG2
    WARN("Normalizing parmat\n");
#endif
    normalize_dmatrix (pm_new,cd->num_block);
#ifdef DEBUG2
    for (i = 0; i < cd->num_block; i++) {
      dmatrixExtDet (pm_new[i],&det_dummy,&sign_dummy);
      WARN("Parmat block %d: log det=%lf\n",i,det_dummy);
    }
#endif

    /* ----------------------------------------------------------------
       Reestimate 'diag' vectors (results are written into 'diag_copy')
       and calc. Q criterion (1st part of large step). We use the
       precomputed array 'pc_buff' to speedup the calculations.
       ---------------------------------------------------------------- */
    /* Precomputation => 'pc_buff' */
#ifdef DEBUG2
    WARN("Precomp. for large step\n");
#endif
    for (i = dcnt2 = 0; i < cd->num_block; i++) {
      d = cd->list[i]->m;
      for (i2 = 0; i2 < d; i2++) {
	pmat = pm_new[i]->matPA[i2];
	for (j = 0; j < d; j++) {
	  factor = pmat[j];
	  pc_buff[dcnt2++] = factor * factor;
	  factor *= 2.00;
	  for (l = j+1; l < d; l++)
	    pc_buff[dcnt2++] = factor * pmat[l];
	}
      }
    }
    /* Loop over all Gaussians in 'gauss' */
#ifdef DEBUG2
    WARN("Run over Gaussians\n");
#endif
    q_crit = 0.00;
    for (k = dc_ind = 0; k < num_gauss; k++) {
      cb = gauss[k].cb;
      index = gauss[k].index;
      beta = cb->accu_gamma_sum[index];
      /* We don't reestimate diags whose Gaussians have accum. too
	 few samples. */
      if (cb->accu_gamma_sum[index] >= DIAG_UPDATE_THRES)
	reestimate_diag_optimized (cb,index,&diag_copy[dc_ind],pc_buff,
				   pc_size);
      q_crit += eval_kl_gaussian_optimized (cb,index,pm_new,&diag_copy[dc_ind],
					    pc_buff,pc_size);
      dc_ind += cd->num_rel;
    }
    q_crit /= alpha;
#ifdef DEBUG2
    WARN("Q crit. after reest.: %lf\n",q_crit);
    WARN("alpha=%lf,num_gauss=%d,num_robust_gauss=%d\n",alpha,num_gauss,
	 num_robust_gauss);
#endif

    /* --------------------------------------------------------------
       If the Q crit. has increased, we copy 'pm_new' to 'pm_old' and
       'diag_copy' into the 'diag' matrices of the codebooks. If not,
       we leave the loop.
       -------------------------------------------------------------- */
    if (q_crit <= q_crit_old) {
#ifdef DEBUG2
      WARN("Q crit. getting worse!\n");
#endif
      break;  /* abort, return 'pm_old' values */
    }

    for (k = dc_ind = 0; k < num_gauss; k++) {
      diag = gauss[k].cb->diag->matPA[gauss[k].index];
      for (i = 0; i < cd->num_rel; i++)
	diag[cd->dim_vect[i]] = (float) diag_copy[dc_ind++];
    }
    for (i = 0; i < cd->num_block; i++) {
      d = cd->list[i]->m;
      for (j = 0; j < d; j++)
	for (l = 0; l < d; l++)
	  pm_old[i]->matPA[j][l] = pm_new[i]->matPA[j][l];
    }

    ss_bound = small_steps;
  } while ((counter < upperb) && (q_crit - q_crit_old >= delta_thres));

  /* ----------------
     Copy parmat back
     ---------------- */
  for (i = 0; i < cd->num_block; i++) {
    d = cd->list[i]->m;
    for (j = 0; j < d; j++)
      for (l = 0; l < d; l++)
	cd->list[i]->matPA[j][l] = (float) pm_old[i]->matPA[j][l];
  }
  cd->def = 0;  /* clear default flag (the PM shouldn't be I any more) */

  /* ---------------------------------------
     Clear 'internal_flag' of all Gaussians.
     Cleanup
     --------------------------------------- */
  for (k = 0; k < num_gauss; k++)
    gauss[k].cb->internal_flag = 0;

  cbnewparmatUpdate_cleanup (cd,hmat,hvect,hindex,hind_size,pm_old,pm_new,
			     pc_buff,pc_size,gauss,pm_inv,dummy_mat,
			     pm_trans,evect,mask1,mask2,epsvect,copyvect,
			     diag_copy);
  *steps_done = counter; /* return total number of small steps */
  return TCL_OK;
}

#undef GETMEMUP

/* --- Janus itf functions --- */

/* -------------------------------------------------------------------
   pmblocksGetItf_cleanup (helper function for 'pmblocksGetItf' and
   'cbnewparmatSetItf')
   Frees the FMatrix array 'lmat' with 'num' entries.
   ------------------------------------------------------------------- */
static void pmblocksGetItf_cleanup (FMatrix** lmat,int num)
{
  int i;

  if (lmat) {
    for (i = 0; i < num; i++)
      if (lmat[i]) fmatrixDestroy (lmat[i]);
    free (lmat);
  }
}

/* -------------------------------------------------------------------
   pmblocksGetItf (helper function for 'getPMBlocks')
   ------------------------------------------------------------------- */
static FMatrix** pmblocksGetItf (CBNewParMatrix* cd,char* value)
{
  FMatrix** lmat = 0;
  FMatrix* mat;
  int argc,i;
  char** argv;

  if (!(lmat = (FMatrix**) malloc (cd->num_block * sizeof(FMatrix*)))) {
    ERROR("Out of memory!\n");
    return 0;
  }
  for (i = 0; i < cd->num_block; i++) lmat[i] = 0;
  MSGCLEAR(NULL);
  if (Tcl_SplitList (itf,value,&argc,&argv) == TCL_ERROR) {
    pmblocksGetItf_cleanup (lmat,cd->num_block);
    return 0;
  }
  if (argc != cd->num_block) {
    pmblocksGetItf_cleanup (lmat,cd->num_block);
    Tcl_Free ((char*) argv);
    return 0;
  }
  for (i = 0; i < argc; i++) {
    if (!(mat = (FMatrix*) itfGetObject (argv[i],"FMatrix")) ||
	mat->m != cd->list[i]->m || mat->n != cd->list[i]->n) {
      ERROR("List of FMatrix objects size-compatible with parmat blocks expected!\n");
      pmblocksGetItf_cleanup (lmat,cd->num_block);
      free ((char*) argv);
      return 0;
    }
    if (!(lmat[i] = fmatrixCreate (mat->m,mat->n))) {
      ERROR("Out of memory!\n");
      pmblocksGetItf_cleanup (lmat,cd->num_block);
      free ((char*) argv);
      return 0;
    }
    fmatrixCopy (lmat[i],mat);
  }
  Tcl_Free ((char*) argv);

  return lmat;
}

/* -------------------------------------------------------------------
   getPMBlocks (custom function for 'cbnewparmatSetItf')
   Expects a Tcl-style list containing exactly 'num_block' names of
   FMatrix objects. The i-th of these objects must have the same size
   as the i-th block of the actual parameter matrix.
   The return value is a list of 'num_block' FMatrix objects.
   ------------------------------------------------------------------- */
static int getPMBlocks (ClientData cd,char* key,ClientData result,int *argcP,
			char *argv[])
{
  FMatrix** lmat;
  
  if (*argcP < 1 || (lmat = pmblocksGetItf ((CBNewParMatrix*) cd,
					    argv[0])) == 0) return -1;
  *((FMatrix***) result) = lmat;
  (*argcP)--;
  
  return 0;
}

/* -------------------------------------------------------------------
   cbnewparmatPutsItf
   ------------------------------------------------------------------- */
int cbnewparmatPutsItf (ClientData cd,int argc,char *argv[])
{
  return cbnewparmatPuts ((CBNewParMatrix*) cd);
}

/* -------------------------------------------------------------------
   cbnewparmatConfigureItf
   ------------------------------------------------------------------- */
int cbnewparmatConfigureItf (ClientData cd,char *var,char *val)
{
  CBNewParMatrix* obj = (CBNewParMatrix*) cd;
  int def;

  if (!var) {
    ITFCFG(cbnewparmatConfigureItf,cd,"name");
    ITFCFG(cbnewparmatConfigureItf,cd,"useN");
    ITFCFG(cbnewparmatConfigureItf,cd,"active");
    ITFCFG(cbnewparmatConfigureItf,cd,"blockN");
    ITFCFG(cbnewparmatConfigureItf,cd,"relCompN");
    ITFCFG(cbnewparmatConfigureItf,cd,"parmatSet");
    ITFCFG(cbnewparmatConfigureItf,cd,"learnRate");
    ITFCFG(cbnewparmatConfigureItf,cd,"defaultFlag");
    ITFCFG(cbnewparmatConfigureItf,cd,"linkN");

    return TCL_OK;
  }
  ITFCFG_CharPtr (var,val,"name",      obj->name,           1);
  ITFCFG_Int     (var,val,"useN",      obj->useN,           1);
  ITFCFG_Int     (var,val,"active",    obj->active,         0);
  ITFCFG_Int     (var,val,"blockN",    obj->num_block,      1);
  ITFCFG_Int     (var,val,"relCompN",  obj->num_rel,        1);
  ITFCFG_Object  (var,val,"parmatSet", obj->parmat_set,
		  name,CBNewParMatrixSet,                   1);
  ITFCFG_Float   (var,val,"learnRate", obj->learn_rate,     0);
  def = (int) obj->def;
  ITFCFG_Int     (var,val,"defaultFlag",def,                1);
  ITFCFG_Int     (var,val,"linkN",     obj->linklist_num,   1);

  return TCL_ERROR;
}

/* -------------------------------------------------------------------
   cbnewparmatAccessItf
   ------------------------------------------------------------------- */
ClientData cbnewparmatAccessItf (ClientData cd,char *name,TypeInfo **ti)
{
  CBNewParMatrix* obj = (CBNewParMatrix*) cd;
  int l;

  if (*name == '.') {
    if (name[1] == 0) {
      itfAppendResult ("item(0..%d) dimVect ",obj->num_block-1);
      if (obj->linklist_num)
	itfAppendResult ("link(0..%d) ",obj->linklist_num-1);
      *ti = 0;
      return 0;
    } else {
      if (sscanf (name+1,"item(%d)",&l) == 1) {
	if (l >= 0 && l < obj->num_block) {
	  *ti = itfGetType("FMatrix"); 
	  return (ClientData) obj->list[l];
	}
      }
      if (!strcmp (name+1,"dimVect")) {
	print_int_vector (0,obj->dim_vect,obj->parmat_set->num_dim);
	*ti = 0;
	return 0;
      }
      if (sscanf (name+1,"link(%d)",&l) == 1) {
	if (l >= 0 && l < obj->linklist_num) {
	  itfAppendResult ("{ %s %d } ",obj->link[l].cb->name,
			   obj->link[l].index);
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
   cbnewparmatSetItf
   ------------------------------------------------------------------- */
int cbnewparmatSetItf (ClientData cd,int argc,char *argv[])
{
  CBNewParMatrix* obj = (CBNewParMatrix*) cd;
  int ac = argc-1,i;
  FMatrix** lmat = 0;

  if (itfParseArgv (argv[0],&ac,argv+1,1,
		    "<matrixList>",ARGV_CUSTOM,getPMBlocks,&lmat,cd,
		    "list of square blocks (FMatrix list)",
		    NULL) != TCL_OK) return TCL_ERROR;

  if (obj->parmat_set->cbnew_set &&
      obj->parmat_set->cbnew_set->phase != CONSTRUCTION_PHASE &&
      obj->linklist_num > 0) {
    ERROR("Object is linked to codebooks and system is not in construction phase!\n");
    pmblocksGetItf_cleanup (lmat,obj->num_block);
    return TCL_ERROR;
  }
  if (normalize_fmatrix (lmat,obj->num_block) == TCL_ERROR) {
    ERROR("Some of the blocks are singular!\n");
    pmblocksGetItf_cleanup (lmat,obj->num_block);
    return TCL_ERROR;
  }
  for (i = 0; i < obj->num_block; i++)
    fmatrixCopy (obj->list[i],lmat[i]);
  obj->def = 0;  /* clear default flag */

  pmblocksGetItf_cleanup (lmat,obj->num_block);
  return TCL_OK;
}


/* -------------------------------------------------------------------
   Interface CBNewParMatrixSet
   ------------------------------------------------------------------- */

/* --- Internal functions --- */

/* -------------------------------------------------------------------
   cbnewpmsetInit
   ------------------------------------------------------------------- */
int cbnewpmsetInit (CBNewParMatrixSet* cd,char* name,int num_dim,float def_lr)
{
#ifdef DEBUG
  assert (cd);
  assert (name);
#endif

  if (num_dim <= 0) {
    ERROR("<dimN> must be positive!\n");
    return TCL_ERROR;
  }
  if (def_lr <= 0.0) {
    ERROR("-defLearnRate must be positive!\n");
    return TCL_ERROR;
  }
  /* Simple components. Set pointers to zero. */
  cd->useN = 0;  /* no references to this object */
  if (!(cd->name = strdup (name))) {
    ERROR("Out of memory!\n");
    return TCL_ERROR;
  }
  cd->cbnew_set = 0;
  cd->num_dim = num_dim;
  cd->def_learn_rate = def_lr;
  cd->cluster_times = 0;

  /* Initialize list */
  listInit ((List*) &(cd->list),&cbnewparmatInfo,sizeof(CBNewParMatrix),
	    CBPMS_LIST_BLKSIZE);
  cd->list.init = (ListItemInit*) cbnewparmatInitEmpty;
  cd->list.deinit = (ListItemDeinit*) cbnewparmatDeinit;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewpmsetCreate
   ------------------------------------------------------------------- */
CBNewParMatrixSet* cbnewpmsetCreate (char* name,int num_dim,float def_lr)
{
  CBNewParMatrixSet* cd;

#ifdef DEBUG
  assert (name);
#endif

  if (!(cd = (CBNewParMatrixSet*) malloc (sizeof(CBNewParMatrixSet)))) {
    ERROR("Out of memory!\n");
    return 0;
  }
  if (cbnewpmsetInit (cd,name,num_dim,def_lr) == TCL_ERROR) {
    free (cd);
    return 0;
  }

  return cd;
}

/* -------------------------------------------------------------------
   cbnewpmsetLoad
   Understands structure file format version 0 (initial version)
   ------------------------------------------------------------------- */
CBNewParMatrixSet* cbnewpmsetLoad (char* name,char* fname)
{
  CBNewParMatrixSet* cd;
  register int i;
  int num_dim,cluster_times,num_mat,num_block,version,index;
  float def_lr;
  char temp_str[256];
  FILE* fp;
  int* dim_vect,*size_vect;

#ifdef DEBUG
  assert (name);
  assert (fname);
#endif

  if (!(fp = fileOpen (fname,"r"))) {
    ERROR("Cannot open file '%s'!\n",fname);
    return 0;
  }
  if ((scan_line (fp,temp_str,256) == TCL_ERROR) ||
      strcmp (temp_str,"PMSTRUCT")) {
    ERROR("'%s' is not a pmset structure file!\n",fname);
    return 0;
  }
  if (fscanf (fp,"%d\n%d\n%f\n%d\n%d\n",&version,&num_dim,&def_lr,
	      &cluster_times,&num_mat) != 5) {
    ERROR("Error while reading file!\n");
    return 0;
  }
  if (version != 0) {
    ERROR("The file format version no. %d cannot be handeled by this revision. Please get an upgrade!\n",version);
    return 0;
  }

  if (!(cd = (CBNewParMatrixSet*) malloc (sizeof(CBNewParMatrixSet)))) {
    ERROR("Out of memory!\n");
    return 0;
  }
  if (cbnewpmsetInit (cd,name,num_dim,def_lr) == TCL_ERROR) {
    free (cd);
    return 0;
  }

  /* Allocate temp. arrays */
  if (!(dim_vect = (int*) malloc (num_dim * sizeof(int)))) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    free (cd);
    return 0;
  }
  if (!(size_vect = (int*) malloc (num_dim * sizeof(int)))) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    free (dim_vect);
    free (cd);
    return 0;
  }

  /* Create list of parameter matrices */
  if (num_mat) {
    for (i = 0; i < num_mat; i++) {
      if (scan_line (fp,temp_str,256) == TCL_ERROR) {
	ERROR("Error while reading structure file '%s'!\n",fname);
	free (dim_vect);
	free (size_vect);
	cbnewpmsetDestroy (cd);
	return 0;
      }
      if (listIndex ((List*) &(cd->list),(ClientData) temp_str,0) >= 0) {
	ERROR("Object '%s' already exists!\n",temp_str);
	free (dim_vect);
	free (size_vect);
	cbnewpmsetDestroy (cd);
	return 0;
      }
      if (fscanf (fp,"%d\n%f\n",&num_block,&def_lr) != 2) {
	ERROR("Error while reading structure file '%s'!\n",fname);
	free (dim_vect);
	free (size_vect);
	cbnewpmsetDestroy (cd);
	return 0;
      }
      if (scan_int_vector (fp,size_vect,num_block) == TCL_ERROR ||
	  scan_int_vector (fp,dim_vect,num_dim)) {
	ERROR("Error while reading structure file '%s'!\n",fname);
	free (dim_vect);
	free (size_vect);
	cbnewpmsetDestroy (cd);
	return 0;
      }
      MSGCLEAR(NULL);
      index = listNewItem ((List*) &(cd->list),(ClientData) temp_str);
      if (cbnewparmatInit (&(cd->list.itemA[index]),temp_str,num_block,cd,
			   size_vect,dim_vect,def_lr) == TCL_ERROR) {
#ifdef DEBUG
	ERROR("<= called by\n");
#endif
	free (dim_vect);
	free (size_vect);
	cbnewpmsetDestroy (cd);
	return 0;
      }
    }
  }
  free (dim_vect);
  free (size_vect);
  fileClose (fname,fp);

  return cd;
}

/* -------------------------------------------------------------------
   cbnewpmsetDeinit
   ------------------------------------------------------------------- */
int cbnewpmsetDeinit (CBNewParMatrixSet* cd)
{
#ifdef DEBUG
  assert (cd);
#endif

  if (cd->useN) {
    ERROR("There exist %d references to this object!\n",cd->useN);
    return TCL_ERROR;
  }
  if (cd->cbnew_set) {
    ERROR("Object linked to codebook set '%s'!\n",cd->cbnew_set->name);
    return TCL_ERROR;
  }

  if (listDeinit ((List*) &(cd->list)) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }
  FREE(cd->name);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewpmsetDestroy
   ------------------------------------------------------------------- */
int cbnewpmsetDestroy (CBNewParMatrixSet* cd)
{
#ifdef DEBUG
  assert (cd);
#endif

  if (cbnewpmsetDeinit (cd) == TCL_ERROR) return TCL_ERROR;
  free (cd);  /* free object itself */

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewpmsetSave
   Uses structure file format version 0 (initial version)
   ------------------------------------------------------------------- */
int cbnewpmsetSave (CBNewParMatrixSet* cd,char* fname)
{
  register int i,j;
  FILE* fp;

#ifdef DEBUG
  ASSERT_PMSET(cd,1);
  assert (fname);
#endif

  if (!(fp = fileOpen (fname,"w"))) {
    ERROR("Cannot create file '%s'!\n",fname);
    return TCL_ERROR;
  }
  fprintf (fp,"PMSTRUCT\n0\n%d\n%f\n%d\n%d\n",cd->num_dim,cd->def_learn_rate,
	   cd->cluster_times,cd->list.itemN);
  for (i = 0; i < cd->list.itemN; i++) {
    print_line (fp,cd->list.itemA[i].name);
    fprintf (fp,"%d\n%f\n{ ",cd->list.itemA[i].num_block,cd->list.itemA[i].learn_rate);
    for (j = 0; j < cd->list.itemA[i].num_block; j++)
      fprintf (fp,"%d ",cd->list.itemA[i].list[j]->m);
    fprintf (fp,"}\n");
    print_int_vector (fp,cd->list.itemA[i].dim_vect,cd->num_dim);
  }
  fileClose (fname,fp);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewpmsetLoadWeights
   Understands weights file format version 0 (initial version)
   ------------------------------------------------------------------- */
int cbnewpmsetLoadWeights (CBNewParMatrixSet* cd,char* fname,int* okN,
			   int* nexistN,int* nemptyN,int* structN)
{
  register int i,j;
  int num_matrices,num_block,dummy,def,index;
  char temp_str[256];
  FILE* fp;
  CBNewParMatrix* pmat = NULL;
  char disc_flag;

#ifdef DEBUG
  ASSERT_PMSET(cd,1);
  assert (fname);
  assert (okN);
  assert (nexistN);
  assert (nemptyN);
  assert (structN);
#endif

  *okN = *nexistN = *nemptyN = *structN = 0;
  if (!(fp = fileOpen (fname,"r"))) {
    ERROR("Cannot open file '%s'!\n",fname);
    return TCL_ERROR;
  }
  if ((read_string (fp,temp_str) == 0) || strcmp (temp_str,"PMWEIGHT")) {
    ERROR("'%s' is not a pmset weight file!\n",fname);
    return TCL_ERROR;
  }
  dummy = read_int (fp);
  if (dummy != 0) {
    ERROR("The file format version no. %d cannot be handeled by this revision. Please get an upgrade!\n",dummy);
    return TCL_ERROR;
  }
  dummy = read_int (fp);
  if (dummy != cd->num_dim) {
    ERROR("System has wrong number of feature space dimensions!\n");
    return TCL_ERROR;
  }
  num_matrices = read_int (fp);

  for (i = 0; i < num_matrices; i++) {
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
      pmat = &(cd->list.itemA[index]);
      for (j = 0; j < pmat->linklist_num; j++)
        if (pmat->link[j].cb->accu_flag != ACCUS_EMPTY) {
	  (*nemptyN)++;
          disc_flag = 1;
          break;
        }
    }
    num_block = read_int (fp);
    if (!disc_flag && (num_block != pmat->num_block)) {
      (*structN)++;
      disc_flag = 1;
    }
    def = read_int (fp);  /* default flag */
    if (def != 0 && def != 1) {
      ERROR("Stored parameter matrix '%s' has invalid default flag!\n",
	    pmat->name);
      return TCL_ERROR;
    }
    for (j = 0; j < num_block; j++) {
      dummy = read_int (fp);  /* block size */
      if (!disc_flag) {
	if (dummy != pmat->list[j]->m) {
	  ERROR("Stored parameter matrix '%s' has wrong block sizes!\n",
		pmat->name);
	  return TCL_ERROR;
	}
	if (!def && (read_float_matrix (fp,pmat->list[j]) == TCL_ERROR)) {
	  ERROR("Error while reading file!\n");
	  return TCL_ERROR;
	}
      } else {
	/* Discard this entry */
	if (!def && (discard_float_matrix (fp,dummy,dummy) == TCL_ERROR)) {
	  ERROR("Error while reading file!\n");
	  return TCL_ERROR;
	}
      }
    }
    if (!disc_flag) {
      (*okN)++;
      pmat->def = (char) def;
      if (def) cbnewparmatDefault (pmat);
      pmat->ts_valid = 0;  /* clear 'ts_valid' flag */
      /* The internal temp. fields of all assoc. codebooks are no longer
	 valid */
      for (j = 0; j < pmat->linklist_num; j++)
	pmat->link[j].cb->internal_flag = 0;
    }
  }
  fileClose (fname,fp);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewpmsetSaveWeights
   Uses weights file format version 0 (initial version)
   ------------------------------------------------------------------- */
int cbnewpmsetSaveWeights (CBNewParMatrixSet* cd,char* fname)
{
  register int i,j;
  FILE* fp;

#ifdef DEBUG
  ASSERT_PMSET(cd,1);
  assert (fname);
#endif

  if (!(fp = fileOpen (fname,"w"))) {
    ERROR("Cannot create file '%s'!\n",fname);
    return TCL_ERROR;
  }
  write_string (fp,"PMWEIGHT");
  write_int (fp,0);  /* weights file format version */
  write_int (fp,cd->num_dim);
  write_int (fp,cd->list.itemN);
  for (i = 0; i < cd->list.itemN; i++) {
    write_string (fp,cd->list.itemA[i].name);
    write_int (fp,cd->list.itemA[i].num_block);
    write_int (fp,(int) cd->list.itemA[i].def);
    for (j = 0; j < cd->list.itemA[i].num_block; j++) {
      write_int (fp,cd->list.itemA[i].list[j]->m);
      if (!cd->list.itemA[i].def)
	write_float_matrix (fp,cd->list.itemA[i].list[j]);
    }
  }
  fileClose (fname,fp);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewpmsetEvalKL
   Phase: working.
   ------------------------------------------------------------------- */
int cbnewpmsetEvalKL (CBNewParMatrixSet* cd,int index,double* val,int* okN)
{
  register int i,k,discF,cb_ind;
  double numer,denumer;
  CBNewParMatrix* pmat;
  CBNew* cb;

#ifdef DEBUG
  ASSERT_PMSET(cd,1);
  assert (index < cd->list.itemN);
  assert (val);
  assert (okN);
  assert (cd->cbnew_set && cd->cbnew_set->phase == WORKING_PHASE);
#endif

  *okN = 0;
  if (index >= 0) {
    pmat = &(cd->list.itemA[index]);
    for (i = 0; i < pmat->linklist_num; i++)
      if ((pmat->link[i].cb->accu_flag == ACCUS_BUILDING) &&
	  pmat->link[i].cb->active[pmat->link[i].index]) {
	ERROR("Codebook '%s' (linked to parmat) has accus in construction!\n",
	      pmat->link[i].cb->name);
	return TCL_ERROR;
      }
    numer = denumer = 0.00;
    for (i = 0; i < pmat->linklist_num; i++) {
      cb = pmat->link[i].cb;
      cb_ind = pmat->link[i].index;
      if (cb->active[cb_ind] && (cb->accu_flag == ACCUS_LOCKED)) {
	numer += eval_kl_gaussian (cb,cb_ind);
	denumer += cb->accu_gamma_sum[cb_ind];
	(*okN)++;
      }
    }
  } else {
    numer = denumer = 0.00;
    for (k = 0; k < cd->list.itemN; k++) {
      pmat = &(cd->list.itemA[k]);
      discF = 0;
      for (i = 0; i < pmat->linklist_num; i++)
	if ((pmat->link[i].cb->accu_flag == ACCUS_BUILDING) &&
	    pmat->link[i].cb->active[pmat->link[i].index]) {
	  discF = 1;
	  break;
	}
      if (!discF)
	for (i = 0; i < pmat->linklist_num; i++) {
	  cb = pmat->link[i].cb;
	  cb_ind = pmat->link[i].index;
	  if (cb->active[cb_ind] && (cb->accu_flag == ACCUS_LOCKED)) {
	    numer += eval_kl_gaussian (cb,cb_ind);
	    denumer += cb->accu_gamma_sum[cb_ind];
	    (*okN)++;
	  }
	}
    }
  }

  *val = (denumer != 0.00) ? (numer / denumer) : 0.00;
  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewpmsetClusterfuncCode
   ------------------------------------------------------------------- */
int cbnewpmsetClusterfuncCode (char* name)
{
  int i;
  char errormsg[500];

#ifdef DEBUG
  assert (name);
#endif

  sprintf (errormsg,"There's no procedure '%s'! The available functions are:\n",name);
  for (i = 0; i < NUM_CLUSTER_FUNCS; i++) {
    strcat (errormsg,name2code[i].desc);
    if (!strcmp (name,name2code[i].name))
      return name2code[i].code;
  }

  ERROR(errormsg);
  return 0;  /* no such criterion */
}

/* -------------------------------------------------------------------
   cbnewpmsetClusterfuncName
   ------------------------------------------------------------------- */
char* cbnewpmsetClusterfuncName (int code)
{
  int i;

  for (i = 0; i < NUM_CLUSTER_FUNCS; i++) {
    if (name2code[i].code == code)
      return name2code[i].name;
  }

  return 0;  /* no such criterion */
}

/* -------------------------------------------------------------------
   cbnewpmsetCluster
   Phase: working. The 'trainParmats' flag of the codebook set must
   be set.
   All parmats in the parmat set must have the same structure (i.e.
   same number of blocks and same block size vector)!
   Skeleton function for all clustering procedures. In this context,
   a clustering procedure will only change the link structure of the
   system so as to optimize a criterion. The class representatives
   (here: the parmats) will not be changed. Thus, a clustering
   procedure performs the first "classification" part of one
   Basic Isodata step.
   This function will call the plug-in function corr. to 'code'.
   The plug-in function calc. the new link structure and returns it
   in terms of the lists descr. below. The skeleton function then
   changes the link structure and reestimates the diagonals of the
   Gaussians.
   If 'initFlag' != 0, we initialize the parmats by randomly choosing
   Gaussians with more than 'samplesThres' samples, performing SVD on
   their covariance matrices and use the rotation matrices as initial
   values. This initialization step takes place before the clustering
   step.
   'gauss' is the list of all gaussians which take part in the
   clustering process. Note that a Gaussian is considered for
   clustering if it's active, it has a parmat, the accus of
   its codebook are locked and the accum. statistics are based
   on more than MEAN_UPDATE_THRES samples.
   'iarray' and 'first_ind' are not used at the moment, but are
   intended for use in future versions. 'iarray' is an index array
   with one entry for each Gaussian. Together with 'first_ind'
   (one entry for each class) it provides a linked list structure
   letting one iterate over the members of each class (-1 is the
   successor index of the last member).
   Note that "Gaussian indices" are positions in 'gauss', "class
   indices" are positions in the member list of the parmat set.
   A plug-in clustering procedure is fed with 'gauss', 'iarray',
   'first_ind' and the number of Gaussians. It returns the results via
   'first_ind', 'iarray' and the 'index' component in 'gauss', the
   number of class changes is returned via 'changesN'.
   ------------------------------------------------------------------- */
int cbnewpmsetCluster (CBNewParMatrixSet* cd,int code,int initFlag,
		       double samplesThres,int* gaussN,int* changesN)
{
  register int i,j,k,num_gauss,cb_ind,index;
  int ret,new_ind;
  cluster_gaussian* gauss;
  int* iarray,*first_ind;
  CBNew* cb;
  CBNewParMatrix* pmat;

#ifdef DEBUG
  ASSERT_PMSET(cd,1);
  assert (cd->cbnew_set && cd->cbnew_set->phase == WORKING_PHASE &&
	  cd->cbnew_set->train_parmats != 0);
  assert (samplesThres >= 0.00);
#endif

  /* Allocate temporary arrays */
  for (i = num_gauss = 0; i < cd->cbnew_set->list.itemN; i++)
    if (cd->cbnew_set->list.itemA[i].accu_flag == ACCUS_LOCKED) {
      for (j = 0; j < cd->cbnew_set->list.itemA[i].num_dens; j++)
	if (cd->cbnew_set->list.itemA[i].active[j] &&
	    cd->cbnew_set->list.itemA[i].parmat[j].mat &&
	    cd->cbnew_set->list.itemA[i].accu_gamma_sum[j] > MEAN_UPDATE_THRES)
	  num_gauss++;
    } else if (cd->cbnew_set->list.itemA[i].accu_flag == ACCUS_BUILDING) {
      ERROR("Codebook '%s' has accus in construction! Use 'update' first.\n",
	    cd->cbnew_set->list.itemA[i].name);
      return TCL_ERROR;
    }
  if (cd->list.itemN == 0) return TCL_OK;  /* nothing to do */
  for (i = 1; i < cd->list.itemN; i++) {
    if (cd->list.itemA[i].num_block != cd->list.itemA[0].num_block) {
      ERROR("Structure of parmat '%s' different from structure of parmat '%s'!\n",cd->list.itemA[i].name,cd->list.itemA[0].name);
      return TCL_ERROR;
    }
    for (j = 0; j < cd->list.itemA[0].num_block; j++)
      if (cd->list.itemA[i].list[j]->m != cd->list.itemA[0].list[j]->m) {
	ERROR("Structure of parmat '%s' different from structure of parmat '%s'!\n",cd->list.itemA[i].name,cd->list.itemA[0].name);
	return TCL_ERROR;
      }
  }
  GETMEM(gauss,cluster_gaussian,num_gauss);
  GETMEMX(iarray,int,num_gauss,free (gauss));
  if (!(first_ind = (int*) malloc (cd->list.itemN * sizeof(int)))) {
    ERROR("Out of memory!\n");
    free (gauss);
    free (iarray);
    return TCL_ERROR;
  }

  /* Initialization */
  for (i = k = 0; i < cd->cbnew_set->list.itemN; i++)
    if (cd->cbnew_set->list.itemA[i].accu_flag == ACCUS_LOCKED) {
      for (j = 0; j < cd->cbnew_set->list.itemA[i].num_dens; j++)
	if (cd->cbnew_set->list.itemA[i].active[j] &&
	    cd->cbnew_set->list.itemA[i].parmat[j].mat &&
	    (cd->cbnew_set->list.itemA[i].accu_gamma_sum[j] >
	     MEAN_UPDATE_THRES)) {
	  gauss[k].cb = &(cd->cbnew_set->list.itemA[i]);
	  gauss[k].cb_ind = j;
	  gauss[k].index = -1;  /* no class */
          k++;
	}
    }

  /* If 'initFlag' is set, we have to initialize the parmats */
  if (initFlag != 0)
    if (init_class_representatives (cd,gauss,num_gauss,samplesThres) ==
	TCL_ERROR) {
      ERROR("Initialization of class representatives failed. Link structure is not altered.\n");
      free (gauss);
      free (iarray);
      free (first_ind);
      return TCL_ERROR;
    }

  /* Clustering: We call the appropriate plug-in function */
  switch (code) {
  case CLUSFUNC_ML: {
    ret = cluster_ml (cd,gauss,iarray,first_ind,num_gauss,changesN);
    break;
  }
  default: {
    ERROR("No such clustering function defined!\n");
    ret = TCL_ERROR;
    break;
  }
  }
  if (ret == TCL_ERROR) {
    ERROR("Clustering failed. Link structure is not altered.\n");
    free (gauss);
    free (iarray);
    free (first_ind);
    return TCL_ERROR;
  }

  /* Setup new link structure and reestimate the diagonal variance mats.
     Note that 'reestimate_diag' will invalidate the internal temp. fields
     of the assoc. Gaussians. */
  for (i = 0; i < num_gauss; i++) {
    cb = gauss[i].cb;
    cb_ind = gauss[i].cb_ind;
    index = gauss[i].index;
    pmat = &(cd->list.itemA[index]);
    if (pmat != cb->parmat[cb_ind].mat) {
      /* Remove link of Gaussian */
      cbnewparmatUnlink (cb->parmat[cb_ind].mat,cb->parmat[cb_ind].index);
      /* Create new link */
      if (cbnewparmatLink (pmat,cb,cb_ind,&new_ind)
	  == TCL_ERROR) return TCL_ERROR;
      cb->parmat[cb_ind].mat = pmat;
      cb->parmat[cb_ind].index = new_ind;
      /* Reestimate the diagonal variance matrix */
      reestimate_diag (cb,cb->diag->matPA[cb_ind],cb_ind);
    }
  }

  /* Cleanup */
  free (gauss);
  free (iarray);
  free (first_ind);

  *gaussN = num_gauss;
  return TCL_OK;
}

/* --- Janus itf functions --- */

/* -------------------------------------------------------------------
   cbnewpmsetCreateItf
   ------------------------------------------------------------------- */
ClientData cbnewpmsetCreateItf (ClientData cd,int argc,char *argv[])
{
  char* str = NULL, *name = NULL;
  int num_dim;
  float def_learn_rate = DEF_LEARN_RATE;
  CBNewParMatrixSet* ret;

  if (itfParseArgv (argv[0],&argc,argv,1,
      "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the set",
		    "<par1>",       ARGV_CUSTOM,getString,&str,           cd,
		    "number of dimensions of feature space (<dimN>) or @<fName>: structure file to load",
		    "-defLearnRate",ARGV_FLOAT, NULL,     &def_learn_rate,NULL,
		    "SUPERFLUOUS",
		    NULL) != TCL_OK) return 0;

  if (*str == '@') {
    if ( (ret = cbnewpmsetLoad (name,str + 1)) )
      INFO("%d parameter matrices loaded\n",ret->list.itemN);
  } else if (sscanf (str,"%d",&num_dim) == 1 && num_dim > 0)
    ret = cbnewpmsetCreate (name,num_dim,def_learn_rate);
  else {
    ERROR("Wrong arguments!\n");
    ret = 0;
  }

  return (ClientData) ret;
}

/* -------------------------------------------------------------------
   cbnewpmsetDestroyItf
   ------------------------------------------------------------------- */
int cbnewpmsetDestroyItf (ClientData cd)
{
  return cbnewpmsetDestroy ((CBNewParMatrixSet*) cd);
}

/* -------------------------------------------------------------------
   cbnewpmsetPutsItf
   ------------------------------------------------------------------- */
int cbnewpmsetPutsItf (ClientData cd,int argc,char *argv[])
{
  CBNewParMatrixSet* obj = (CBNewParMatrixSet*) cd;

  return listPutsItf ((ClientData) &(obj->list),argc,argv);
}

/* -------------------------------------------------------------------
   cbnewpmsetConfigureItf
   ------------------------------------------------------------------- */
int cbnewpmsetConfigureItf (ClientData cd,char *var,char *val)
{
  CBNewParMatrixSet* obj = (CBNewParMatrixSet*) cd;

  if (!var) {
    ITFCFG(cbnewpmsetConfigureItf,cd,"name");
    ITFCFG(cbnewpmsetConfigureItf,cd,"useN");
    ITFCFG(cbnewpmsetConfigureItf,cd,"dimN");
    ITFCFG(cbnewpmsetConfigureItf,cd,"defLearnRate");
    return listConfigureItf ((ClientData) &(obj->list),var,val);
  }
  ITFCFG_CharPtr (var,val,"name",        obj->name,          1);
  ITFCFG_Int     (var,val,"useN",        obj->useN,          1);
  ITFCFG_Int     (var,val,"dimN",        obj->num_dim,       1);
  ITFCFG_Float   (var,val,"defLearnRate",obj->def_learn_rate,0);

  return listConfigureItf ((ClientData) &(obj->list),var,val);
}

/* -------------------------------------------------------------------
   cbnewpmsetAccessItf
   ------------------------------------------------------------------- */
ClientData cbnewpmsetAccessItf (ClientData cd,char *name,TypeInfo **ti)
{
  CBNewParMatrixSet* obj = (CBNewParMatrixSet*) cd;
  int l;

  if (*name == '.') {
    if (name[1] == 0) {
      if (obj->list.itemN)
	itfAppendResult ("item(0..%d) ",obj->list.itemN-1);
      *ti = 0;
      return 0;
    } else if (sscanf (name+1,"item(%d)",&l) == 1) {
      if (l >= 0 && l < obj->list.itemN) {
	*ti = itfGetType ("CBNewParMatrix");
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
   cbnewpmsetIndexItf
   ------------------------------------------------------------------- */
int cbnewpmsetIndexItf (ClientData cd,int argc,char *argv[])
{
  CBNewParMatrixSet* obj = (CBNewParMatrixSet*) cd;

  return listName2IndexItf ((ClientData) &(obj->list),argc,argv);
}

/* -------------------------------------------------------------------
   cbnewpmsetNameItf
   ------------------------------------------------------------------- */
int cbnewpmsetNameItf (ClientData cd,int argc,char *argv[])
{
  CBNewParMatrixSet* obj = (CBNewParMatrixSet*) cd;

  return listIndex2NameItf ((ClientData) &(obj->list),argc,argv);
}

/* -------------------------------------------------------------------
   cbnewpmsetAddItf
   ------------------------------------------------------------------- */
int cbnewpmsetAddItf (ClientData cd,int argc,char *argv[])
{
  CBNewParMatrixSet* obj = (CBNewParMatrixSet*) cd;
  int ac = argc-1,dummy,class_no,i,index;
  char* name = NULL;
  int num_block = 1;
  float learn_rate = obj->def_learn_rate;
  CBNewParMatrix* hp;
  char* test_vect;
  SVector* dim_vect = 0,*size_vect;
  int* sv,*dv;

  if (itfParseArgv (argv[0],&ac,argv+1,1,
		    "<parMatName>",ARGV_STRING,NULL,&name,      NULL,
		    "name of parameter matrix object",
		    "<blockN>",    ARGV_INT,   NULL,&num_block, NULL,
		    "number of blocks in parameter matrix",
		    "<sizeVect>",  ARGV_OBJECT,NULL,&size_vect, "SVector",
		    "vector holding block sizes",
		    "-dimVect",    ARGV_OBJECT,NULL,&dim_vect,  "SVector",
		    "dimension index vector",
		    "-learnRate",  ARGV_FLOAT, NULL,&learn_rate,NULL,
		    "SUPERFLUOUS",
		    NULL) != TCL_OK) return 0;

  if (num_block <= 0 || num_block > obj->num_dim) {
    ERROR("<blockN> must be positive and <= %d!\n",obj->num_dim);
    return TCL_ERROR;
  }
  if (learn_rate <= 0.0) {
    ERROR("-learnRate must be positive!\n");
    return TCL_ERROR;
  }
  if (listIndex ((List*) &(obj->list),(ClientData) name,0) >= 0) {
    ERROR("'%s' already exists!\n",name);
    return TCL_ERROR;
  }
  if (sscanf (name,CLUSTER_PMNAME,&dummy,&class_no) == 2) {
    ERROR("Names of this form are reserved!\n");
    return TCL_ERROR;
  }
  if (size_vect->n < num_block) {
    ERROR("<sizeVect> must have %d entries!\n",num_block);
    return TCL_ERROR;
  }
  for (i = dummy = 0; i < num_block; i++)
    dummy += (int) size_vect->vecA[i];
  if (dummy <= 0 || dummy > obj->num_dim) {
    ERROR("Sum of the block sizes must be positive and <= %d!\n",obj->num_dim);
    free (size_vect);
    return TCL_ERROR;
  }
  if (dim_vect) {
    if (dim_vect->n < obj->num_dim) {
      ERROR("-dimVect must have %d entries!\n",obj->num_dim);
      return TCL_ERROR;
    }
    if (!(test_vect = (char*) malloc (obj->num_dim * sizeof(char)))) {
      ERROR("Out of memory!\n");
      return TCL_ERROR;
    }
    for (i = 0; i < obj->num_dim; i++) test_vect[i] = 0;
    for (i = 0; i < obj->num_dim; i++) {
      dummy = (int) dim_vect->vecA[i];
      if (dummy < 0 || dummy >= obj->num_dim || test_vect[dummy]) {
	ERROR("-dimVect must contain each index exactly once!\n");
	free (test_vect);
	return TCL_ERROR;
      } else test_vect[dummy] = 1;
    }
    free (test_vect);
  }
  if (!(sv = (int*) malloc (num_block * sizeof(int)))) {
    ERROR("Out of memory!\n");
    return TCL_ERROR;
  }
  for (i = 0; i < num_block; i++) sv[i] = (int) size_vect->vecA[i];
  if (dim_vect) {
    if (!(dv = (int*) malloc (obj->num_dim * sizeof(int)))) {
      ERROR("Out of memory!\n");
      return TCL_ERROR;
    }
    for (i = 0; i < obj->num_dim; i++) dv[i] = (int) dim_vect->vecA[i];
  } else
    dv = 0;

  MSGCLEAR(NULL);
  index = listNewItem ((List*) &(obj->list),(ClientData) name);
  hp = &(obj->list.itemA[index]);
  if (cbnewparmatInit (hp,name,num_block,obj,sv,dv,learn_rate) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    listDelete ((List*) &(obj->list),(ClientData) name);
    free (sv);
    if (dv) free (dv);
    return TCL_ERROR;
  }
  free (sv);
  if (dv) free (dv);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewpmsetCleanupItf
   ------------------------------------------------------------------- */
int cbnewpmsetCleanupItf (ClientData cd,int argc,char *argv[])
{
  CBNewParMatrixSet* obj = (CBNewParMatrixSet*) cd;
  int i;

  for (i = obj->list.itemN-1; i >= 0; i--)
    if (obj->list.itemA[i].linklist_num == 0)
      if (listDelete ((List*) &(obj->list),
		      (ClientData) obj->list.itemA[i].name) == TCL_ERROR) {
	ERROR("Cannot remove parmat '%s'!\n",obj->list.itemA[i].name);
	return TCL_ERROR;
      }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   cbnewpmsetUpdateItf
   ------------------------------------------------------------------- */
int cbnewpmsetUpdateItf (ClientData cd,int argc,char *argv[])
{
  CBNewParMatrixSet* obj = (CBNewParMatrixSet*) cd;
  int ac = argc-1,i,j,ret=TCL_ERROR,index;
  int okN = 0,wrongN = 0,errorN = 0,discN = 0,discF,done;
  char* name = NULL;
  int steps  = DEF_NUM_STEPS;
  int smallS = DEF_SMALL_STEPS;
  int first  = DEF_FIRST_SMALL_STEPS;
  double delta = DEF_DELTA_THRES;
  CBNewParMatrix* hp;

  if (!obj->cbnew_set || obj->cbnew_set->phase != WORKING_PHASE) {
    ERROR("Associated codebook set must be in working phase!\n");
    return TCL_ERROR;
  }
  if (obj->cbnew_set->train_parmats == 0) {
    ERROR("Parmat training mode off! Use 'SetTrainParmats' method of CBNewSet.\n");
    return TCL_ERROR;
  }
    
  if (itfParseArgv (argv[0],&ac,argv+1,1,
		    "-parMat",    ARGV_STRING,NULL,&name, NULL,
		    "name of parameter matrix object to update (default: all)",
		    "-stepN",     ARGV_INT,   NULL,&steps,NULL,
		    "upper bound for steps of gradient ascent",
		    "-smallSteps",ARGV_INT,   NULL,&smallS,NULL,
		    "number of small steps between two large steps",
		    "-firstSmall",ARGV_INT,   NULL,&first,NULL,
		    "number of small steps between first and second large step",
		    "-deltaThres",ARGV_DOUBLE,NULL,&delta,NULL,
		    "threshold for stopping criterion",
		    NULL) != TCL_OK) return 0;

  if (steps <= 0) {
    ERROR("-stepN must be positive!\n");
    return TCL_ERROR;
  }
  if (smallS <= 0) {
    ERROR("-smallSteps must be positive!\n");
    return TCL_ERROR;
  }
  if (first <= 0) {
    ERROR("-firstSmall must be positive!\n");
    return TCL_ERROR;
  }
  if (name) {
    if ((index = listIndex ((List*) &(obj->list),(ClientData) name,0)) < 0) {
      ERROR("No element '%s' in the list!\n",name);
      return TCL_ERROR;
    }
    hp = &(obj->list.itemA[index]);
    for (i = 0; i < hp->linklist_num; i++) {
      if (hp->link[i].cb->accu_flag == ACCUS_LOCKED) okN++;
      else if (hp->link[i].cb->accu_flag == ACCUS_EMPTY) wrongN++;
      else {
	ERROR("Accumulators of codebook '%s' (linked to parmat) in construction!\n",
	      hp->link[i].cb->name);
	return TCL_ERROR;
      }
    }
    if (okN > 0) {
      ret = cbnewparmatUpdate (hp,steps,smallS,first,delta,&done);
      if (ret == TCL_OK) {
	INFO("Used %d small steps.\n",done);
	if (wrongN > 0)
	  WARN("The update was based only on the %d Gaussians having locked accus.\nThe other %d Gaussians (with empty accus) were ignored!\n",okN,wrongN);
      }
    }
  } else {
    /* We update only parameter matrices with all assoc. codebooks having
       accus not in construction. However, codebooks with empty accus are
       ignored. */
    for (i = 0; i < obj->list.itemN; i++) {
      hp = &(obj->list.itemA[i]);
      okN = wrongN = discF = 0;
      for (j = 0; j < hp->linklist_num; j++) {
	if (hp->link[j].cb->accu_flag == ACCUS_LOCKED) okN++;
	else if (hp->link[j].cb->accu_flag == ACCUS_EMPTY) wrongN++;
	else {
	  discF = 1;  /* discard this matrix */
	  discN++;
	  break;
	}
      }
      if (discF == 0 && okN > 0) {
	INFO("Updating parameter matrix '%s'...\n",hp->name);
	if (cbnewparmatUpdate (hp,steps,smallS,first,delta,&done) == TCL_ERROR)
	  errorN++;
	else {
	  INFO("Used %d small steps.\n",done);
	  if (wrongN > 0)
	    WARN("The update of '%s' was based only on the %d Gaussians having locked accus.\nThe other %d Gaussians (with empty accus) were ignored!\n",
		 hp->name,okN,wrongN);
	}
      }
    }
    INFO("%d parameter matrices updated\n",obj->list.itemN - errorN - discN);
    if (discN > 0)
      INFO("%d parameter matrices discarded because of wrong accu states of codebooks\n",
	 discN);
    if (errorN > 0)
      INFO("Update failed for %d parameter matrices (because of errors)\n",
	 errorN);
    ret = (errorN > 0) ? TCL_ERROR : TCL_OK;
  }

  return ret;
}

/* -------------------------------------------------------------------
   cbnewpmsetSaveItf
   ------------------------------------------------------------------- */
int cbnewpmsetSaveItf (ClientData cd,int argc,char *argv[])
{
  CBNewParMatrixSet* obj = (CBNewParMatrixSet*) cd;
  int ac = argc-1,ret;
  char* fname = NULL;

  if (itfParseArgv (argv[0],&ac,argv+1,1,
                    "<fName>",ARGV_STRING,NULL,&fname,NULL,
                    "name of structure file to create",
		    NULL) != TCL_OK) return 0;

  if ((ret = cbnewpmsetSave (obj,fname)) == TCL_OK)
    INFO("%d parameter matrices stored\n",obj->list.itemN);

  return ret;
}

/* -------------------------------------------------------------------
   cbnewpmsetLoadWeightsItf
   ------------------------------------------------------------------- */
int cbnewpmsetLoadWeightsItf (ClientData cd,int argc,char *argv[])
{
  CBNewParMatrixSet* obj = (CBNewParMatrixSet*) cd;
  int ac = argc-1,ret,okN,nexistN,nemptyN,structN;
  char* fname = NULL;

  if (itfParseArgv (argv[0],&ac,argv+1,1,
                    "<fName>",ARGV_STRING,NULL,&fname,NULL,
                    "name of structure file to create",
		    NULL) != TCL_OK) return 0;

  if ((ret = cbnewpmsetLoadWeights (obj,fname,&okN,&nexistN,&nemptyN,
				    &structN)) == TCL_OK) {
    INFO("Weights of %d parameter matrices loaded\n",okN);
    if (nexistN > 0)
      INFO("%d entries discarded because parmat doesn't exist\n",nexistN);
    if (nemptyN > 0)
      INFO("%d entries discarded because of non-empty accus assoc. with parmat\n",nemptyN);
    if (structN > 0)
      INFO("%d entries discarded because of structural mismatch\n",structN);
  }

  return ret;
}

/* -------------------------------------------------------------------
   cbnewpmsetSaveWeightsItf
   ------------------------------------------------------------------- */
int cbnewpmsetSaveWeightsItf (ClientData cd,int argc,char *argv[])
{
  CBNewParMatrixSet* obj = (CBNewParMatrixSet*) cd;
  int ac = argc-1,ret;
  char* fname = NULL;

  if (itfParseArgv (argv[0],&ac,argv+1,1,
                    "<fName>",ARGV_STRING,NULL,&fname,NULL,
                    "name of structure file to create",
		    NULL) != TCL_OK) return 0;

  if ((ret = cbnewpmsetSaveWeights (obj,fname)) == TCL_OK)
    INFO("Weights of %d parameter matrices stored\n",obj->list.itemN);

  return ret;
}

/* -------------------------------------------------------------------
   cbnewpmsetEvalKLItf
   Phase: working (checked).
   ------------------------------------------------------------------- */
int cbnewpmsetEvalKLItf (ClientData cd,int argc,char *argv[])
{
  CBNewParMatrixSet* obj = (CBNewParMatrixSet*) cd;
  int ac = argc-1,index,ret,okN;
  double val;
  char* name = NULL;

  if (!obj->cbnew_set || obj->cbnew_set->phase != WORKING_PHASE) {
    ERROR("Associated codebook set must be in working phase!\n");
    return TCL_ERROR;
  }
  if (itfParseArgv (argv[0],&ac,argv+1,1,
		    "-parMat",    ARGV_STRING,NULL,&name, NULL,
		    "name of parameter matrix object to eval. (default: all)",
		    NULL) != TCL_OK) return 0;

  if (name) {
    if ((index = listIndex ((List*) &(obj->list),(ClientData) name,0)) < 0) {
      ERROR("No element '%s' in the list!\n",name);
      return TCL_ERROR;
    }
  } else
    index = -1;
  if ((ret = cbnewpmsetEvalKL (obj,index,&val,&okN)) == TCL_OK) {
    INFO("Evaluation used %d Gaussians\n",okN);
    itfAppendResult ("%lf ",val);
  }

  return ret;
}

/* -------------------------------------------------------------------
   cbnewpmsetClusterItf
   Phase: working (checked).
   ------------------------------------------------------------------- */
int cbnewpmsetClusterItf (ClientData cd,int argc,char *argv[])
{
  CBNewParMatrixSet* obj = (CBNewParMatrixSet*) cd;
  int ac = argc-1;
  int initFlag = 0;
  double samplesThres = DIAG_UPDATE_THRES;
  int proc_code,gaussN,changesN;
  char* proc_name = NULL;

  if (!obj->cbnew_set || obj->cbnew_set->phase != WORKING_PHASE) {
    ERROR("Associated codebook set must be in working phase!\n");
    return TCL_ERROR;
  }
  if (obj->cbnew_set->train_parmats == 0) {
    ERROR("Parmat training mode off! Use 'SetTrainParmats' method of CBNewSet.\n");
    return TCL_ERROR;
  }
  if (itfParseArgv (argv[0],&ac,argv+1,1,
                    "<procName>",   ARGV_STRING,NULL,&proc_name,   NULL,
                    "name of clustering procedure",
                    "-init",        ARGV_INT,   NULL,&initFlag,    NULL,
                    "!= 0 => initialize parmats first",
                    "-samplesThres",ARGV_DOUBLE,NULL,&samplesThres,NULL,
                    "threshold for parmat initialization",
		    NULL) != TCL_OK) return 0;

  if (!(proc_code = cbnewpmsetClusterfuncCode (proc_name))) return TCL_ERROR;

  if (cbnewpmsetCluster (obj,proc_code,initFlag,samplesThres,&gaussN,
                         &changesN) == TCL_OK) {
    INFO("%d Gaussians took part in the clustering process.\n",gaussN);
    INFO("%d of these Gaussians have changed their class.\n",changesN);
    itfAppendResult ("{ %d %d }",gaussN,changesN);
    return TCL_OK;
  }

  return TCL_ERROR;
}

/* -------------------------------------------------------------------
   cbnewpmsetConvertFeatureItf
   ------------------------------------------------------------------- */
int cbnewpmsetConvertFeatureItf (ClientData cd,int argc,char *argv[])
{
 
  CBNewParMatrixSet *pms  = (CBNewParMatrixSet*) cd;
  FeatureSet        *fes  = NULL;
  Feature           *srcF = NULL;
  Feature           *dstF = NULL;
  FMatrix           *srcM = NULL;
  FMatrix           *dstM = NULL;
  char              *name = NULL;

  int pmN= pms->list.itemN;
  int ac = argc-1;
  int featX, pmX;

  
  if (itfParseArgv (argv[0],&ac,argv+1,1,
                    "<FeatureSet>",ARGV_OBJECT, NULL, &fes, "FeatureSet", "name of the feature set", 
		    "<name>",      ARGV_STRING, NULL, &name, NULL,        "source feature",
                    NULL) != TCL_OK) return TCL_ERROR;

  featX= fesGetIndex(fes,name,0);
  if (featX < 0) {
    ERROR("no source feature %s in %s\n",name,fes->name);
    return TCL_ERROR;
  }
  srcF = fes->featA+featX;
  srcM = srcF->data.fmat;

  for (pmX=0;pmX<pmN;pmX++) { 
    CBNewParMatrix *pm = pms->list.itemA +pmX;
    int         blockN = pm->num_block;

    register int  blockX, frameX, dimX, dimY, dimTot;
    char buf[256];

    if (pm->active == 0) {continue;} 

    
    strcpy(buf,"OFS-");
    strcat(buf,pm->name);

    featX= fesGetIndex(fes,buf,1);  
    if (featX < 0) {
      ERROR("failed to create feature %s\n",buf);
      return TCL_ERROR;
    }
    dstF = fes->featA+featX;
    dstM = NULL;
    dstM = fmatrixCreate(srcM->m,srcM->n);

    if (dstM == NULL) {
      ERROR("failed to create dst Matrix for %s %d x %d %s\n",buf,srcM->m,srcM->n);
      return TCL_ERROR;
    }

    /*
    for (frameX=0;frameX<srcM->m;frameX++) {
      register float *src = srcM->matPA[frameX];
      register float *dst = dstM->matPA[frameX];

      dimTot= 0;
      for (blockX=0;blockX<blockN;blockX++) {
	FMatrix        *block = pm->list[blockX];
	register int dimBlock = block->m;
      
	for (dimX=0;dimX<dimBlock;dimX++) {
	  register float *row = block->matPA[dimX];
	  register float  sum = 0.0;
	  for (dimY=0;dimY<dimBlock;dimY++) {
	    sum += row[dimY] * src[dimY + dimTot];
	  }	
	  dst[dimX+dimTot]= sum;
	}
	dimTot += dimBlock;
      }
    }
    */

    dimTot= 0;
    for (blockX=0;blockX<blockN;blockX++) {
      FMatrix        *block = pm->list[blockX];
      register int dimBlock = block->m;

      if (dimBlock ==10) {
	for (frameX=0;frameX<srcM->m;frameX++) {
	  register float *dst = dstM->matPA[frameX]+dimTot;
	  register float s0 = srcM->matPA[frameX][dimTot];
	  register float s1 = srcM->matPA[frameX][dimTot+1];
	  register float s2 = srcM->matPA[frameX][dimTot+2];
	  register float s3 = srcM->matPA[frameX][dimTot+3];
	  register float s4 = srcM->matPA[frameX][dimTot+4];
	  register float s5 = srcM->matPA[frameX][dimTot+5];
	  register float s6 = srcM->matPA[frameX][dimTot+6];
	  register float s7 = srcM->matPA[frameX][dimTot+7];
	  register float s8 = srcM->matPA[frameX][dimTot+8];
	  register float s9 = srcM->matPA[frameX][dimTot+9];
	
	  for (dimX=0;dimX<dimBlock;dimX++) {
	    register float *row = block->matPA[dimX];
	    dst[dimX]=
	      row[0] * s0 + row[1] * s1 + row[2] * s2 + row[3] * s3 + \
	      row[4] * s4 + row[5] * s5 + row[6] * s6 + row[7] * s7 + \
	      row[8] * s8 + row[9] * s9;
	  }
	}
	dimTot += 10;
	continue;
      }
      if (dimBlock ==11) {
	for (frameX=0;frameX<srcM->m;frameX++) {
	  register float *dst = dstM->matPA[frameX]+dimTot;      
	  register float s0 = srcM->matPA[frameX][dimTot];
	  register float s1 = srcM->matPA[frameX][dimTot+1];
	  register float s2 = srcM->matPA[frameX][dimTot+2];
	  register float s3 = srcM->matPA[frameX][dimTot+3];
	  register float s4 = srcM->matPA[frameX][dimTot+4];
	  register float s5 = srcM->matPA[frameX][dimTot+5];
	  register float s6 = srcM->matPA[frameX][dimTot+6];
	  register float s7 = srcM->matPA[frameX][dimTot+7];
	  register float s8 = srcM->matPA[frameX][dimTot+8];
	  register float s9 = srcM->matPA[frameX][dimTot+9];
	  register float s10 = srcM->matPA[frameX][dimTot+10];
	  for (dimX=0;dimX<dimBlock;dimX++) {
	    register float *row = block->matPA[dimX];
	    dst[dimX]=
	      row[0] * s0 + row[1] * s1 + row[2] * s2 + row[3] * s3 + \
	      row[4] * s4 + row[5] * s5 + row[6] * s6 + row[7] * s7 + \
	      row[8] * s8 + row[9] * s9 + row[10] * s10;
	  }
	}
	dimTot+=11;
	continue;
      }
      for (frameX=0;frameX<srcM->m;frameX++) {
	register float *src = srcM->matPA[frameX];
	for (dimX=0;dimX<dimBlock;dimX++) {
	  register float *row = block->matPA[dimX];
	  register float  sum = 0.0;
	  for (dimY=0;dimY<dimBlock;dimY++) {
	    sum += row[dimY] * src[dimY + dimTot];
	  }
	}	
      }
      dimTot+=dimBlock;
    }
  
    if (feFMatrix(fes,dstF,dstM, 1)) {
      if (dstM) fmatrixFree(dstM);
      return TCL_ERROR;
    }
    COPY_CONFIG( dstF, srcF);
  }

  return TCL_OK;
}

/* --- Class descriptions --- */

static Method cbnewparmatMethod[] = {
  { "puts",      cbnewparmatPutsItf,      NULL},
  { "set",       cbnewparmatSetItf,       "set subobject" },
  {  NULL,       NULL,                    NULL }
};

TypeInfo cbnewparmatInfo = {
  "CBNewParMatrix", 0, -1, cbnewparmatMethod,
  0, 0,
  cbnewparmatConfigureItf, cbnewparmatAccessItf,
  itfTypeCntlDefaultNoLink,
  "Parameter matrix used by CBNew codebooks\n" };

/* -------------------------------------------------------------------
   CBNewParMatrix_Init
   Type constructor for CBNewParMatrix.
   ------------------------------------------------------------------- */
static int cbnewparmatInitialized = 0;

int CBNewParMatrix_Init ()
{
  if (cbnewparmatInitialized) return TCL_OK;

  itfNewType (&cbnewparmatInfo);
  cbnewparmatInitialized = 1;

  return TCL_OK;
}

static Method cbnewpmsetMethod[] = {
  { "puts",      cbnewpmsetPutsItf,       NULL},
  { "index",     cbnewpmsetIndexItf,      "get index of list element" },
  { "name",      cbnewpmsetNameItf,       "get name of list element" },
  { "add",       cbnewpmsetAddItf,        "add new list element" },
  { "cleanup",   cbnewpmsetCleanupItf,
      "remove all parameter matrices without no links" },
  { "update",    cbnewpmsetUpdateItf,     "update parameter matrices" },
  { "save",      cbnewpmsetSaveItf,       "save object structure" },
  { "loadWeights",cbnewpmsetLoadWeightsItf,"load weights" },
  { "saveWeights",cbnewpmsetSaveWeightsItf,"save weights" },
  { "evalKL",    cbnewpmsetEvalKLItf,     "evaluate KL criterion" },
  { "cluster",   cbnewpmsetClusterItf,    "cluster Gaussians" },
  { "convert",   cbnewpmsetConvertFeatureItf, "convert feature" },

  {  NULL,       NULL,                    NULL }
};

TypeInfo cbnewpmsetInfo = {
  "CBNewParMatrixSet", 0, -1, cbnewpmsetMethod,
  cbnewpmsetCreateItf, cbnewpmsetDestroyItf,
  cbnewpmsetConfigureItf, cbnewpmsetAccessItf,
  itfTypeCntlDefaultNoLink,
  "Set of CBNewParMatrix parameter matrices\n" };

/* -------------------------------------------------------------------
   CBNewParMatrixSet_Init
   Type constructor for CBNewParMatrixSet. We have to initialize the
   random number generators.
   ------------------------------------------------------------------- */
static int cbnewpmsetInitialized = 0;

int CBNewParMatrixSet_Init ()
{
  if (cbnewpmsetInitialized) return TCL_OK;

  if ( CBNewParMatrix_Init() != TCL_OK) return TCL_ERROR;

  /* Init. of pseudo random number generator of FMatrix (ffmat module).
     CHANGE THIS! */
  chaosRandomInit (0.123456789);
  srand (31415927);

  itfNewType (&cbnewpmsetInfo);
  cbnewpmsetInitialized = 1;

  return TCL_OK;
}
