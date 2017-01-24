/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: micv
               ------------------------------------------------------------

    Author  :  Andreas Eller
    Module  :  micv
    Date    :  $Id: micvInit.c 2633 2005-03-08 17:30:28Z metze $
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
    Revision 1.1  2005/03/08 17:30:28  metze
    Initial code as of Andreas Eller


   ======================================================================== */

#include "micvInit.h"
#include "micvWeights.h"
#include "micvPrototypes.h"
#include "micvScore.h"

#include "micvEM.h"
#include "micvFunctions.h"
#include "micvMultithread.h"


/* ----------------------------------------------------------------------------
 *	init a MICVSET object
 * ----------------------------------------------------------------------------*/
int micvsetInit(MicvSet* pMicvSet, char* name, FeatureSet* pFeatSet)
{
	// Simple components. Set pointers to zero.
	pMicvSet->useN = 0;  // no references to this object

	if (!(pMicvSet->name = strdup (name))) {
		ERROR("Out of memory!\n");
		return TCL_ERROR;
	}

	// set default config values
	pMicvSet->cfg.nEstimationIterations			= 3;
	pMicvSet->cfg.nPrototypeClusterIterations	= 4;
	pMicvSet->cfg.fPTClusterSubset				= 1;
	
	pMicvSet->cfg.nWeightsEstIterations			= 20;
	pMicvSet->cfg.fWeightsEstEpsilon			= 1.0e-10;
	pMicvSet->cfg.fWeightsEstInitialGamma		= 1.0;
	pMicvSet->cfg.fWeightsEstMinimalGamma		= 1.0e-20;
	pMicvSet->cfg.fWeightsEstGammaDivider		= 2.0;

	pMicvSet->cfg.nPTEstInitialIterations			= 10;
	pMicvSet->cfg.nPTEstFinalIterationsOuterLoop	= 0;
	pMicvSet->cfg.nPTEstFinalIterationsInnerLoop	= 0;
	pMicvSet->cfg.fPTSubset							= 0.1;
	pMicvSet->cfg.fPTEstEpsilon						= 1.0e-05;
	pMicvSet->cfg.fPTEstInitialGamma				= 1.0;
	pMicvSet->cfg.fPTEstMinimalGamma				= 1.0e-20;
	pMicvSet->cfg.fPTEstGammaDivider				= 10;

	pMicvSet->cfg.fExpThreshold						= -100;

	pMicvSet->fScoreFactor = 1.0;

	pMicvSet->nDim  = 0;
	pMicvSet->nDimV = 0;
	pMicvSet->pFeatSet = pFeatSet;
	pMicvSet->nPrototypes = 0;
	pMicvSet->pPrototypes = NULL;
	pMicvSet->pPrototypesVT = NULL;
	pMicvSet->nGaussians = 0;

	// link (pFeatSet, "FeatureSet");
	pFeatSet->useN++;

	// optimizations
	pMicvSet->nSubset = 0;
	pMicvSet->pSubset = NULL;
	pMicvSet->pW = NULL;
	pMicvSet->nFrame = -1;

	micvset_initThreads(pMicvSet);

	return TCL_OK;
}


/* ----------------------------------------------------------------------------
 *	deinit a MICVSET object
 * ----------------------------------------------------------------------------*/
int micvsetDeinit(MicvSet* pMicvSet)
{
	micvset_endThreads(pMicvSet);

	if (listDeinit ((List*) &(pMicvSet->micvList)) == TCL_ERROR) {
		ERROR("<= called by\n");
		return TCL_ERROR;
	}

	SAFE_FREE(pMicvSet->name);

	if (pMicvSet->pFeatSet) {
		pMicvSet->pFeatSet->useN--;  /* Decrement useN counter */
		pMicvSet->pFeatSet = 0;
	}

	micvset_freePrototypes(pMicvSet);

	SAFE_FREE(pMicvSet->pSubset);
	SAFE_FREE(pMicvSet->pW);

	return TCL_OK;
}


/* ----------------------------------------------------------------------------
 *	init a MICV object
 * ----------------------------------------------------------------------------*/
int micvInit(Micv* pMicv, int nCov, MicvSet* pMicvSet, int feat)
{
	assert(nCov > 0);
	assert((pMicvSet->pFeatSet->featA[feat].type == FE_FMATRIX &&
		pMicvSet->pFeatSet->featA[feat].data.fmat->n == pMicvSet->nDim) ||
		(pMicvSet->pFeatSet->featA[feat].type == FE_UNDEF &&
		pMicvSet->pFeatSet->featA[feat].dtype == FE_FMATRIX));
	assert(pMicvSet->nDim > 0);

	// simple components
	pMicv->useN = 0;
	pMicv->pMicvSet = pMicvSet;
	pMicv->nFeat = feat;
	pMicv->nCov = nCov;
	pMicv->pAccu = 0;
	pMicv->micvType = eDeactivated;
	pMicv->scoreFct = &micv_scoreDeactivated;

	// optimization - precalculated values for score function
	pMicv->fDLog2Pi = pMicvSet->nDim * log(2.0 * PI);

	return TCL_OK;
}


/* ----------------------------------------------------------------------------
 *	deinit a MICV object
 * ----------------------------------------------------------------------------*/
int micvDeinit(Micv* pMicv)
{
	assert (pMicv);

	if (pMicv->useN > 0) {
		ERROR("There exist %d references to codebook '%s'!\n", pMicv->useN, pMicv->name);
		return TCL_ERROR;
	}

	if (pMicv->name) SAFE_FREE(pMicv->name);

	// free accumulators
	if(pMicv->pAccu) micv_freeAccus(pMicv);

	micv_deactivateModel(pMicv);

	pMicv->nFeat = -1;
	pMicv->nCov = 0;

	return TCL_OK;
}


/* ----------------------------------------------------------------------------
 *	init an item for MicvList
 *  ----------------------------------------------------------------------------*/
int micvItemInit (Micv* pMicv, ClientData name)
{
	assert (pMicv);
	assert (name);

	if (!(pMicv->name = strdup ((char*) name))) {
		ERROR("Out of memory!\n");
		return TCL_ERROR;
	}

	pMicv->useN		= 0;
	pMicv->nFeat	= -1;

	return TCL_OK;
}




/* ----------------------------------------------------------------------------
 *	init micv codebooks with DistribSet / CodebookSet
 * ----------------------------------------------------------------------------*/
int micvset_initCodebooks(MicvSet* pMicvSet, const DistribSet* pDistribSet)
{
	Micv* pMicv;
	CodebookSet* pCBSet;
	Codebook* pCB;
	Distrib* pDistrib;
	int m, idx;
	int nModels = 0;
	int nRenamed = 0;

	pCBSet = pDistribSet->cbsP;

	// may be correct if you want to train a MICV with lower dimensionality
	if(pMicvSet->nDim < pCBSet->list.itemA[0].dimN)
		WARN("dimensions of CBS and MICV do not match!!! (CBS.dimN = %d, MICV.dimN = %d)\n", pCBSet->list.itemA[0].dimN, pMicvSet->nDim);
	else if(pMicvSet->nDim > pCBSet->list.itemA[0].dimN)
	{
		ERROR("dimensions of CBS and MICV do not match!!! (CBS.dimN = %d, MICV.dimN = %d)\n", pCBSet->list.itemA[0].dimN, pMicvSet->nDim);
		return TCL_ERROR;
	}

	// copy means and covariances from DistribSet into MICV
	for(m = 0; m < pDistribSet->list.itemN; m++)
	{
		pDistrib = &pDistribSet->list.itemA[m];
		pCB = &pCBSet->list.itemA[pDistrib->cbX];

		// get MICV model with same name as codebook
		idx = listName2Index((List*)&pMicvSet->micvList, pCB->name);
		if(idx < 0)
		{
			WARN("unable to initialize unknown MICV model %s.\n", pCB->name);
			continue;
		}
		pMicv = &pMicvSet->micvList.itemA[idx];

		if(pMicv->micvType == eDeactivated)
		{
			// change MICV name to the name of the distribution in order to be compatible with the DistribTree
			if(strcmp(pMicv->name, pDistrib->name) != 0)
			{
				// verify that the new model name won't lead to a conflict with other models
				if(listName2Index((List*)&pMicvSet->micvList, pDistrib->name) < 0)
				{
					SAFE_FREE(pMicv->name);
					VERIFY_MALLOC(pMicv->name = strdup(pDistrib->name), return TCL_ERROR);
					nRenamed++;
				} else {
					WARN("unable to rename MICV model %s to %s\n", pMicv->name, pDistrib->name);
				}
			}
			micv_inactiveToGMM(pMicv);
			micv_initModel(pMicv, pDistrib, pCB);
			micv_precalcGMMScoreValues(pMicv);
			nModels++;
		} else
			WARN("tried to initialize the already initialized MICV model %s\n", pMicv->name);
	}

	fprintf(STDERR, "-------------------------------\n");
	fprintf(STDERR, "   MICV models:       %6d\n", pMicvSet->micvList.itemN);
	fprintf(STDERR, "   CBS models:        %6d\n", pCBSet->list.itemN);
	fprintf(STDERR, "   initialized models:%6d\n", nModels);
	fprintf(STDERR, "   renamed models:    %6d\n", nRenamed);
	fprintf(STDERR, "-------------------------------\n");

	return TCL_OK;
}


/* ----------------------------------------------------------------------------
 *	init a MICV object using a distribution and a codebook
 * ----------------------------------------------------------------------------*/
void micv_initModel(Micv* pMicv, Distrib* pDistrib, Codebook* pCB)
{
	int cov, i;

	if(pMicv->nCov > pCB->refN)
	{
		ERROR("%s: can not initialize %d MICV covariances with %d CBS covariances!\n", pMicv->name, pMicv->nCov, pCB->refN);
		return;
	}

	if(pMicv->pMicvSet->nDim > pCB->dimN)
	{
		ERROR("%s: can not initialize %d-dimensional MICV with %d-dimensional CBS!\n", pMicv->name, pMicv->pMicvSet->nDim, pCB->dimN);
		return;
	}


	for(cov = 0; cov < pMicv->nCov; cov++)
	{
		// init median
		for(i = 0; i < pMicv->pMicvSet->nDim; i++)
			pMicv->pMedian->matPA[cov][i] = pCB->rv->matPA[cov][i];

		pMicv->pLogMixtureWeights->vecA[cov] = pDistrib->val[cov];
		pMicv->pMixtureWeights->vecA[cov] = exp(pMicv->pLogMixtureWeights->vecA[cov]);

		// init inverse covariance matrix
		micv_zeroMatrix(pMicv->pInvCov[cov]);
		pMicv->pCovType[cov] = pCB->cv[cov]->type;

		switch(pCB->cv[cov]->type)
		{
		case COV_DIAGONAL:
			for(i = 0; i < pMicv->pMicvSet->nDim; i++)
				pMicv->pInvCov[cov]->matPA[i][i] = pCB->cv[cov]->m.d[i];
			break;
		default:
			ERROR("implement me!!!");
		}
	}

	// covariances changed -> precalc score values
	micv_precalcGMMScoreValues(pMicv);
}


/* ----------------------------------------------------------------------------
 *	create accumulators
 * ----------------------------------------------------------------------------*/
int micv_createAccus(Micv* pMicv)
{
	int cov;

	if(pMicv->pAccu != 0)
	{
		WARN("This MICV already has accumulators!\n");
		return TCL_ERROR;
	}

	VERIFY_MALLOC(pMicv->pAccu = malloc(sizeof(MicvAccu)), return TCL_ERROR);
	pMicv->pAccu->pFeatVecSum = dmatrixCreate(pMicv->nCov, pMicv->pMicvSet->nDim);
	VERIFY_MALLOC(pMicv->pAccu->pMixtureWeightSum = malloc(pMicv->nCov * sizeof(double)), return TCL_ERROR);
	VERIFY_MALLOC(pMicv->pAccu->pSampleCov = malloc(pMicv->nCov * sizeof(DMatrix*)), return TCL_ERROR);
	VERIFY_MALLOC(pMicv->pAccu->pSampleCovV = malloc(pMicv->nCov * sizeof(DMatrix*)), return TCL_ERROR);
	for(cov=0; cov < pMicv->nCov; cov++)
	{
		pMicv->pAccu->pSampleCov[cov] = dmatrixCreate(pMicv->pMicvSet->nDim, pMicv->pMicvSet->nDim);
		pMicv->pAccu->pSampleCovV[cov] = dmatrixCreate(pMicv->pMicvSet->nDimV, 1);
	}

	VERIFY_MALLOC(pMicv->pAccu->pCovWeights = malloc(pMicv->nCov * sizeof(double)), return TCL_ERROR);

	return micv_clearAccus(pMicv);
}


/* ----------------------------------------------------------------------------
*	release the memory used by the accumulators
* ----------------------------------------------------------------------------*/
int micv_freeAccus(Micv* pMicv)
{
	int cov;

	if(pMicv->pAccu == 0)
	{
		WARN("There are no accumulators to free!\n");
		return TCL_ERROR;
	}

	dmatrixFree(pMicv->pAccu->pFeatVecSum);
	SAFE_FREE(pMicv->pAccu->pMixtureWeightSum);
	for(cov=0; cov < pMicv->nCov; cov++)
	{
		dmatrixFree(pMicv->pAccu->pSampleCov[cov]);
		dmatrixFree(pMicv->pAccu->pSampleCovV[cov]);
	}
	free(pMicv->pAccu->pSampleCov);
	free(pMicv->pAccu->pSampleCovV);
	SAFE_FREE(pMicv->pAccu->pCovWeights);

	SAFE_FREE(pMicv->pAccu);

	return TCL_OK;
}


/* ----------------------------------------------------------------------------
*	clear the accumulators
* ----------------------------------------------------------------------------*/
int micv_clearAccus(Micv* pMicv)
{
	int cov; 

	if(pMicv->pAccu == 0)
	{
		WARN("There are no accumulators to clear!\n");
		return TCL_ERROR;
	}

	pMicv->pAccu->nAccumulatedFrames = 0;
	dmatrixInitConst(pMicv->pAccu->pFeatVecSum, 0.0);
	for(cov=0; cov < pMicv->nCov; cov++)
	{
		pMicv->pAccu->pMixtureWeightSum[cov] = 0.0;
		dmatrixInitConst(pMicv->pAccu->pSampleCov[cov], 0.0);
		dmatrixInitConst(pMicv->pAccu->pSampleCovV[cov], 0.0);
		pMicv->pAccu->pSampleCov[cov]->count = 0.0;
	}

	return TCL_OK;
}



/* ----------------------------------------------------------------------------
 *	switch all models to status "deactivated"
 * ----------------------------------------------------------------------------*/
void micvset_deactivateAllModels(MicvSet* pMicvSet)
{
	int m;

	for(m=0; m < pMicvSet->micvList.itemN; m++)
		micv_deactivateModel(&pMicvSet->micvList.itemA[m]);
}


/* ----------------------------------------------------------------------------
 *	switch one model to status "deactivated"
 * ----------------------------------------------------------------------------*/
void micv_deactivateModel(Micv* pMicv)
{
	int cov;

	// is model active?
	if(pMicv->micvType == eDeactivated)
		return;

	// free memory
	dmatrixFree(pMicv->pMedian);			pMicv->pMedian = 0;
	dvectorFree(pMicv->pMixtureWeights);	pMicv->pMixtureWeights = 0;
	dvectorFree(pMicv->pLogMixtureWeights);	pMicv->pLogMixtureWeights = 0;
	for(cov=0; cov < pMicv->nCov; cov++)
		dmatrixFree(pMicv->pInvCov[cov]);
	SAFE_FREE(pMicv->pInvCov);
	SAFE_FREE(pMicv->pConstScore);
	SAFE_FREE(pMicv->pDistVec);
	SAFE_FREE(pMicv->pScoreVec);

	// free GMM specific data
	if(pMicv->micvType == eGMM)
	{
		SAFE_FREE(pMicv->pCovType);
	}

	// free MICV specific data
	if(pMicv->micvType == eMICV)
	{
		dmatrixFree(pMicv->pPrototypeWeights);	pMicv->pPrototypeWeights = 0;
		dmatrixFree(pMicv->pV);					pMicv->pV = 0;
	}

	pMicv->micvType = eDeactivated;
	pMicv->scoreFct = &micv_scoreDeactivated;
	pMicv->pMicvSet->nGaussians -= pMicv->nCov;
}


/* ----------------------------------------------------------------------------
 *	change status of one model from "deactivated" to "gmm"
 * ----------------------------------------------------------------------------*/
void micv_inactiveToGMM(Micv* pMicv)
{
	int i;

	if(pMicv->micvType != eDeactivated)
	{
		WARN("tried to transform a model of type %d into a GMM model! (only deactivated allowed)\n", pMicv->micvType);
		return;
	}

	// allocate gaussian parameters
	pMicv->pMedian = dmatrixCreate(pMicv->nCov, pMicv->pMicvSet->nDim);
	dmatrixInitConst(pMicv->pMedian, 0.0);
	pMicv->pMixtureWeights = dvectorCreate(pMicv->nCov);
	pMicv->pLogMixtureWeights = dvectorCreate(pMicv->nCov);

	VERIFY_MALLOC(pMicv->pInvCov = malloc(pMicv->nCov * sizeof(DMatrix*)), return);
	for(i=0; i < pMicv->nCov; i++)
	{
		pMicv->pInvCov[i] = dmatrixCreate(pMicv->pMicvSet->nDim, pMicv->pMicvSet->nDim);
		dmatrixInitConst(pMicv->pInvCov[i], 1.0);
	}
	VERIFY_MALLOC(pMicv->pCovType = malloc(pMicv->nCov * sizeof(CovType)), return);

	// optimization - precalculated values for score function
	VERIFY_MALLOC(pMicv->pConstScore = malloc(pMicv->nCov * sizeof(double)), return);
	VERIFY_MALLOC(pMicv->pDistVec = malloc(pMicv->pMicvSet->nDim * sizeof(double)), return);
	VERIFY_MALLOC(pMicv->pScoreVec = malloc(pMicv->nCov * sizeof(double)), return);

	pMicv->micvType = eGMM;
	pMicv->scoreFct = &micv_scoreGMM;

	pMicv->pMicvSet->nGaussians += pMicv->nCov;
}


/* ----------------------------------------------------------------------------
 *	change status of all models from "gmm" to "micv"
 * ----------------------------------------------------------------------------*/
void micvset_allGMMtoMICV(MicvSet* pMicvSet)
{
	int i;
	Micv* pMicv;

	for(i=0; i < pMicvSet->micvList.itemN; i++)
	{
		pMicv = &pMicvSet->micvList.itemA[i];
		if(pMicv->micvType == eGMM)
			micv_GMMtoMICV(pMicv);
	}
}


/* ----------------------------------------------------------------------------
 *	change status of one model from "gmm" to "micv"
 * ----------------------------------------------------------------------------*/
void micv_GMMtoMICV(Micv* pMicv)
{
	if(pMicv->micvType != eGMM)
	{
		WARN("tried to transform a model of type %d into a MICV model! (only GMM allowed)\n", pMicv->micvType);
		return;
	}

	// free data structures needed by GMM model but not by MICV model
	SAFE_FREE(pMicv->pCovType);

	pMicv->pPrototypeWeights = dmatrixCreate(pMicv->nCov, pMicv->pMicvSet->nPrototypes);
	dmatrixInitConst(pMicv->pPrototypeWeights, 0.0);

	// data structures for score function to avoid memory allocation during score function
	pMicv->pV = dmatrixCreate(pMicv->nCov, pMicv->pMicvSet->nDim);

	pMicv->micvType = eMICV;
	pMicv->scoreFct = &micv_scoreMICV;
}

