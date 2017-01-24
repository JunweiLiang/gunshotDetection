/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: semi tied covariances
               ------------------------------------------------------------

    Author  :  Matthias Seeger
    Module  :  cbn_assert.c
    Date    :  $Id: cbn_assert.c 390 2000-08-16 09:12:49Z soltau $
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
    Revision 1.1  2000/08/16 09:07:08  soltau
    Initial revision


   ======================================================================== */


#include "cbn_assert.h"

#ifdef DEBUG

#include "cbn_parmat.h"
#include "cbnew.h"

/* --- Interface --- */

/* Checks if everything's OK with CBNewParMatrix object CD. We do not
   check the assoc. 'parmat_set'. We just compute a checksum for the
   dimension index vector. We do not check the codebooks assoc. with the
   parameter matrix. If 'linkflag' = 1, we run over the codebooks assoc.
   with the matrix and do some checks. */
void assert_parmat(CBNewParMatrix* pmat,int linkflag,char* file,
		   int line)
{
  int i,j;
  MYASS(pmat)
  MYASS(pmat->name)
  MYASS(pmat->useN >= 0)
  MYASS(pmat->num_block > 0)
  MYASS(pmat->learn_rate > 0.0)
  MYASS(pmat->parmat_set)
  MYASS(pmat->num_block <= pmat->parmat_set->num_dim)
  if (pmat->ts_valid != 0) {
    MYASS(pmat->ts_valid == 1)
    MYASS(pmat->trans_sample)
  }
  MYASS(pmat->def == 0 || pmat->def == 1)
  MYASS(pmat->list)
  for (i = j = 0; i < pmat->num_block; i++) {
    MYASS(pmat->list[i])
    MYASS(pmat->list[i]->m == pmat->list[i]->n)
    j += pmat->list[i]->m;
  }
  MYASS(j == pmat->num_rel)
  MYASS(j > 0 && j <= pmat->parmat_set->num_dim)
  MYASS(pmat->dim_vect)
  for (i = j = 0; i < pmat->parmat_set->num_dim; i++) {
    MYASS(pmat->dim_vect[i] >= 0 &&
	    pmat->dim_vect[i] < pmat->parmat_set->num_dim)
    j += pmat->dim_vect[i];
  }
  i = pmat->parmat_set->num_dim;
  MYASS(j == (i*(i-1))/2)
  MYASS(pmat->linklist_num >= 0)
  MYASS(pmat->linklist_size >= pmat->linklist_num)
  if (pmat->linklist_size) {
    MYASS(pmat->link)
    if (linkflag == 1)
      for (i = 0; i < pmat->linklist_num; i++) {
	MYASS(pmat->link[i].cb)
	MYASS(pmat->link[i].index >= 0 &&
		pmat->link[i].index < pmat->link[i].cb->num_dens)
	MYASS(pmat->link[i].cb->parmat[pmat->link[i].index].mat == pmat)
	MYASS(pmat->link[i].cb->parmat[pmat->link[i].index].index == i)
	MYASS(pmat->link[i].cb->cbnew_set->parmat_set == pmat->parmat_set)
      }
  }
}

/* Checks if everything's OK with CBNewParMatrixSet object CD. We also
   check all parameter matrices in the list, but we don't check the
   assoc. codebook set. 'linkflag' is passed to ASSERT_PARMAT. */
void assert_pmset(CBNewParMatrixSet* pms,int linkflag,char* file,
		  int line)
{
  int i;
  MYASS(pms)
  MYASS(pms->name)
  MYASS(pms->useN >= 0)
  MYASS(pms->num_dim > 0)
  if (pms->cbnew_set)
    MYASS(pms->num_dim == pms->cbnew_set->num_dim)
  MYASS(pms->def_learn_rate > 0.0)
  MYASS(pms->cluster_times >= 0)
  MYASS(pms->list.itemN >= 0)
  for (i = 0; i < pms->list.itemN; i++)
    if (pms->list.itemA[i].parmat_set) {
      assert_parmat(&(pms->list.itemA[i]),linkflag,file,line);
      MYASS(pms->list.itemA[i].parmat_set == pms)
    }
}

/* Checks if everything is OK with protocol structure of CBNew object 'cb'. */
void assert_prot(CBNew* cb,char* file,int line)
{
  int i;
  MYASS(cb->protlist_size >= 0)
  for (i = 0; i < cb->protlist_size; i++)
    if (cb->slot[i].crit) {
      MYASS(cb->slot[i].counter >= 0)
      ASSERT_MATRIX(cb->slot[i].epoch,cb->slot[i].epoch->m,
		    cb->cbnew_set->num_dim)
    }
}

/* Checks if everything's OK with link of parameter matrix 'pmat' to gaussian
   'k' of codebook 'cb'. This includes:
   - link established?
   - correct sum of sizes of par. matrix blocks */
void assert_link(CBNewParMatrix* pmat,CBNew* cb,int k,char* file,
		 int line)
{
  MYASS(pmat->link[cb->parmat[k].index].cb == cb)
  MYASS(pmat->link[cb->parmat[k].index].index == k)
  MYASS(pmat->num_rel <= cb->cbnew_set->num_dim)
}

/* Checks if everything's OK with the covariance accus of a Gaussian having a
   parameter matrix, and whose CBNewSet is in working phase. The Gaussian
   must be active! See comments for 'CBNew_accu' in cbnew.h for more
   information. */
void assert_accu_work(CBNew* cb,int index,char* file,int line)
{
  int i,d;
  double* last_blockp;

  if (cb->cbnew_set->train_parmats != 0) {
    MYASS(cb->accu_cov[index].block)
    if (cb->parmat[index].mat->num_rel < cb->cbnew_set->num_dim)
      MYASS(cb->accu_cov[index].diag)
    else
      MYASS(cb->accu_cov[index].diag == 0)
    last_blockp = cb->accu_cov[index].block[0];
    MYASS(last_blockp)
    for (i = 0; i < cb->parmat[index].mat->num_block - 1; i++) {
      d = cb->parmat[index].mat->list[i]->m;
      last_blockp += (d*(d+1))/2;
      MYASS(last_blockp == cb->accu_cov[index].block[i+1])
    }
  } else {
    MYASS(cb->accu_cov[index].block == 0)
    MYASS(cb->accu_cov[index].diag)
  }
}

/* Checks if everything's OK with main data structures of CBNew object 'cb'.
   If 'linkflag' = 1, we also check the link structure using
   ASSERT_LINK. */
void assert_main(CBNew* cb,int linkflag,char* file,int line)
{
  int i;
  double sum;
  MYASS(cb->num_dens > 0)
  MYASS(cb->cbnew_set)
  if (cb->cbnew_set->phase == CONSTRUCTION_PHASE ||
      cb->cbnew_set->phase == TESTING_PHASE)
    MYASS(cb->accu_flag == ACCUS_EMPTY)
  else {
    if (cb->accu_flag == ACCUS_EMPTY) MYASS(cb->accu_num == 0)
    else {
      MYASS(cb->accu_num >= 0)
      MYASS(cb->accu_flag == ACCUS_LOCKED ||
            cb->accu_flag == ACCUS_BUILDING)
    }
  }
  MYASS(fesGetName (cb->cbnew_set->feat_set,cb->feat))
  MYASS((cb->cbnew_set->feat_set->featA[cb->feat].type == FE_FMATRIX &&
	   cb->cbnew_set->feat_set->featA[cb->feat].data.fmat->n
	   == cb->cbnew_set->num_dim) ||
	  (cb->cbnew_set->feat_set->featA[cb->feat].type == FE_UNDEF &&
	   cb->cbnew_set->feat_set->featA[cb->feat].dtype == FE_FMATRIX &&
	   cb->cbnew_set->feat_set->featA[cb->feat].dcoeffN
	   == cb->cbnew_set->num_dim))
  MYASS(cb->distrib)
  ASSERT_MATRIX(cb->mean,cb->num_dens,cb->cbnew_set->num_dim)
  ASSERT_MATRIX(cb->diag,cb->num_dens,cb->cbnew_set->num_dim)
  MYASS(cb->parmat)
  for (i = 0,sum = 0.00; i < cb->num_dens; i++) {
    if (cb->active[i]) {
      sum += exp (- cb->distrib[i]);
    }
    if ((linkflag == 1) && cb->parmat[i].mat)
      assert_link(cb->parmat[i].mat,cb,i,file,line);
  }
  MYASS(sum > 0.90 && sum < 1.10)
  if (cb->cbnew_set->phase == TESTING_PHASE) {
    if (cb->accu_mean != 0)
      ASSERT_MATRIX (cb->accu_mean,cb->num_dens,cb->cbnew_set->num_dim)
  } else if (cb->cbnew_set->phase != CONSTRUCTION_PHASE)
    if (cb->accu_flag != ACCUS_EMPTY || cb->accu_mean != 0) {
      MYASS(cb->accu_gamma_sum)
      ASSERT_MATRIX (cb->accu_mean,cb->num_dens,cb->cbnew_set->num_dim)
      MYASS(cb->accu_cov)
      for (i = 0; i < cb->num_dens; i++)
	if (cb->active[i]) {
	  if (cb->parmat[i].mat)
	    assert_accu_work(cb,i,file,line);
	  else {
	    MYASS(cb->accu_cov[i].block == 0)
	    MYASS(cb->accu_cov[i].diag)
	  }
	}
    }
  if (cb->trans_mean)
    ASSERT_MATRIX(cb->trans_mean,cb->num_dens,cb->cbnew_set->num_dim)
  MYASS(cb->lh_factor)
  MYASS(cb->internal_flag == 0 || cb->internal_flag == 1)
  MYASS(cb->temp_gamma)
  MYASS(cb->lh_buff)
}

/* Checks if everything's OK with CBNew object 'cb'. We don't check the
   assoc. CBNewSet. We check the link structure (if and only if 'linkflag'
   = 1), but not the assoc. parameter matrices. */
void assert_cb(CBNew* cb,int linkflag,char* file,int line)
{
  MYASS(cb)
  MYASS(cb->name)
  MYASS(cb->useN >= 0)
  assert_main(cb,linkflag,file,line);
  assert_prot(cb,file,line);
}

/* Checks if everything's OK with CBNewSet object 'cbs'. We also check all
   codebooks in the list, but don't check the assoc. parameter matrix
   set. */
void assert_cbset(CBNewSet* cbs,int linkflag,char* file,int line)
{
  int i;
  MYASS(cbs)
  MYASS(cbs->name)
  MYASS(cbs->useN >= 0)
  MYASS(cbs->num_dim > 0)
  MYASS(cbs->parmat_set)
  MYASS(cbs->num_dim == cbs->parmat_set->num_dim)
  MYASS(cbs->phase == CONSTRUCTION_PHASE || cbs->phase == WORKING_PHASE ||
	cbs->phase == TESTING_PHASE)
  MYASS(cbs->train_parmats == 0 || cbs->train_parmats == 1)
  MYASS(cbs->list.itemN >= 0)
  for (i = 0; i < cbs->list.itemN; i++)
    if (cbs->list.itemA[i].cbnew_set) {
      assert_cb(&(cbs->list.itemA[i]),linkflag,file,line);
      MYASS(cbs->list.itemA[i].cbnew_set == cbs)
    }
}

#endif
