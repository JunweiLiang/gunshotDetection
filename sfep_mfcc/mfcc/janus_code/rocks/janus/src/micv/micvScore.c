/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: micv
               ------------------------------------------------------------

    Author  :  Andreas Eller
    Module  :  micv
    Date    :  $Id: micvScore.c 2633 2005-03-08 17:30:28Z metze $
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

#include "micvScore.h"


// ----------------------------------------------------------------------------
//	scoreDeactivated is used when the MICV model is in status "deactivated"
// ----------------------------------------------------------------------------
float micv_scoreDeactivated(Micv* pMicv, const int nFrame, double* pCovWeights)
{
	if(pCovWeights)
		memset(pCovWeights, 0, sizeof(double) * pMicv->nCov);
	return cBadScoreValue;
}


// ----------------------------------------------------------------------------
//	precalculate values used in scoreGMM
// ----------------------------------------------------------------------------
void micv_precalcGMMScoreValues(Micv* pMicv)
{
	int cov;

	for(cov=0; cov < pMicv->nCov; cov++)
		pMicv->pConstScore[cov] = pMicv->fDLog2Pi - log(dmatrixDet(pMicv->pInvCov[cov]));
}


// ----------------------------------------------------------------------------
//	calculate score of feature pFeature
//		scoreGMM is used when the MICV model is in its initial state
//		representing a simple GMM
//
//	pCovWeights		returns the contribution factor of the covariances
//					(might be NULL)
// ----------------------------------------------------------------------------
float micv_scoreGMM(Micv* pMicv, const int nFrame, double* pCovWeights)
{
	int cov, m, n;
	float* pFeature;
	double** pInvCov;
	double* pDistVec;
	double* v2;
	double* pScores = pMicv->pScoreVec;
	double score;
	double temp;
	double fMinLogProp = 1e200;

	if((pFeature = fesFrame(pMicv->pMicvSet->pFeatSet, pMicv->nFeat, nFrame)) == 0)
	{
		ERROR("can't get frame %d of feature %d!\n", nFrame, pMicv->nFeat);
		if(pCovWeights)
			memset(pCovWeights, 0, sizeof(double) * pMicv->nCov);
		return cBadScoreValue;
	}

	// sum over all gaussians of this micv model
	for(cov=0; cov < pMicv->nCov; cov++)
	{
		// constant part
		score = pMicv->pConstScore[cov];

		pDistVec = pMicv->pDistVec;
		for(n = 0; n < pMicv->pMicvSet->nDim; n++)
			*pDistVec++ = pFeature[n] - pMicv->pMedian->matPA[cov][n];

		pInvCov = pMicv->pInvCov[cov]->matPA;

		switch(pMicv->pCovType[cov])
		{
		case COV_DIAGONAL:
			// (o - µ)T * COV * (o - µ)
			pDistVec = pMicv->pDistVec;
			for(m = 0; m < pMicv->pMicvSet->nDim; m++, pDistVec++)
				score += (*pDistVec) * (*pDistVec) * pInvCov[m][m];
			break;

		case COV_FULL:
			// (o - µ)T * COV * (o - µ)
			pDistVec = pMicv->pDistVec;
			for(m = 0; m < pMicv->pMicvSet->nDim; m++)
			{
				temp = 0;
				v2 = pMicv->pDistVec;
				for(n = 0; n < pMicv->pMicvSet->nDim; n++)
				{
					temp += (*v2++) * pInvCov[n][m];
				}
				score += (*pDistVec++) * temp;
			}
			break;
		default:
			ERROR("implement me!!!\n");
			assert(0);
		}

		score *= 0.5;

		if(score < fMinLogProp)
			fMinLogProp = score;

		pScores[cov] = score;
	}

	score = 0.0;
	if(pCovWeights)
	{
		for(cov=0; cov < pMicv->nCov; cov++)
		{
			temp = fMinLogProp - pScores[cov];
			if(temp > pMicv->pMicvSet->cfg.fExpThreshold)
			{
				pCovWeights[cov] = exp(temp - pMicv->pLogMixtureWeights->vecA[cov]);
				score += pCovWeights[cov];
			} else {
				pCovWeights[cov] = 0.0;
			}
		}
	} else {
		for(cov=0; cov < pMicv->nCov; cov++)
		{
			temp = fMinLogProp - pScores[cov];
			if(temp > pMicv->pMicvSet->cfg.fExpThreshold)
				score += exp(temp - pMicv->pLogMixtureWeights->vecA[cov]);
		}
	}

	// make sure that the sum of pCovWeights is one
	if(pCovWeights)
		for(cov=0; cov < pMicv->nCov; cov++)
			pCovWeights[cov] /= score;

	return fMinLogProp - log(score);
}


// ----------------------------------------------------------------------------
//	precalculate values used in scoreMICV for all models
// ----------------------------------------------------------------------------
void micvset_precalcMICVScoreValues(MicvSet* pMicvSet)
{
	int m;
	Micv* pMicv;

	for(m = 0; m < pMicvSet->micvList.itemN; m++)
	{
		pMicv = &pMicvSet->micvList.itemA[m];
		if(pMicv->micvType == eDeactivated)
			continue;

		micv_precalcMICVScoreValues(pMicv);
	}
}


// ----------------------------------------------------------------------------
//	precalculate values used in scoreMICV for one model
// ----------------------------------------------------------------------------
void micv_precalcMICVScoreValues(Micv* pMicv)
{
	int cov, x, y;
	double f;
	// calculate values for optimization

	for(cov=0; cov < pMicv->nCov; cov++)
	{
		pMicv->pConstScore[cov] = pMicv->fDLog2Pi - log(dmatrixDet(pMicv->pInvCov[cov]));
		// median_T * Cov-1 * median
		// v = - Cov-1 * median
		for(y=0; y <pMicv->pMicvSet->nDim; y++)
		{
			f = 0.0;
			for(x=0; x < pMicv->pMicvSet->nDim; x++)
				f += pMicv->pMedian->matPA[cov][x] * pMicv->pInvCov[cov]->matPA[y][x];
			pMicv->pV->matPA[cov][y] = - f;
			pMicv->pConstScore[cov] +=  f * pMicv->pMedian->matPA[cov][y];
		}
		pMicv->pConstScore[cov] *= 0.5;
	}
}


// ----------------------------------------------------------------------------
//	calculate score of feature pFeature
//		scoreMICV is used when the model is a full defined MICV model
//
//	pCovWeights		returns the contribution factor of the covariances
//					(might be NULL)
// ----------------------------------------------------------------------------
float micv_scoreMICV(Micv* pMicv, const int nFrame, double* pCovWeights)
{
	MicvSet* pMicvSet = pMicv->pMicvSet;
	float* pFeature;
	int k, x, y, cov;
	double f, w;
	double* covScores = pMicv->pScoreVec;
	double score = 0.0;
	double fMinLogProp = 1e200;
	double *pScore, *p1, *p2;
	float  *pFeat;

	if((pFeature = fesFrame(pMicv->pMicvSet->pFeatSet, pMicv->nFeat, nFrame)) == 0)
	{
		ERROR("can't get frame %d of feature %d!\n", nFrame, pMicv->nFeat);
		if(pCovWeights)
			memset(pCovWeights, 0, sizeof(double) * pMicv->nCov);
		return cBadScoreValue;
	}

	// wk
	if(pMicvSet->nFrame != nFrame)
	{
		for(k=0; k < pMicvSet->nPrototypes; k++)
		{
			w = 0.0;
			for(y=0; y < pMicvSet->nDim; y++)
			{
				f = 0.0;
				for(x=0; x < pMicvSet->nDim; x++)
					f += pFeature[x] * pMicvSet->pPrototypes[k]->matPA[y][x];
				 w += f * pFeature[y];
			}
			pMicvSet->pW[k] = 0.5 * w;
		}
		pMicvSet->nFrame = nFrame;
	}

	pScore = covScores;
	for(cov=0; cov < pMicv->nCov; cov++, pScore++)
	{
		*pScore = pMicv->pConstScore[cov];

		p1 = pMicv->pPrototypeWeights->matPA[cov];
		p2 = pMicvSet->pW;
		for(k=0; k < pMicvSet->nPrototypes; k++)
			*pScore += (*p1++) * (*p2++);

		p1 = pMicv->pV->matPA[cov];
		pFeat = pFeature;
		for(x=0; x < pMicvSet->nDim; x++)
			*pScore += (*p1++) * (*pFeat++);

		if(*pScore < fMinLogProp)
			fMinLogProp = *pScore;
	}

	score = 0.0;
	if(pCovWeights)
	{
		for(cov=0; cov < pMicv->nCov; cov++)
		{
			f = fMinLogProp - covScores[cov];
			if(f > pMicvSet->cfg.fExpThreshold)
			{
				pCovWeights[cov] = exp(f - pMicv->pLogMixtureWeights->vecA[cov]);
				score += pCovWeights[cov];
			} else {
				pCovWeights[cov] = 0.0;
			}
		}
	} else {
		for(cov=0; cov < pMicv->nCov; cov++)
		{
			f = fMinLogProp - covScores[cov];
			if(f > pMicvSet->cfg.fExpThreshold)
				score += exp(f - pMicv->pLogMixtureWeights->vecA[cov]);
		}
	}

	// make sure that the sum of pCovWeights is one
	if(pCovWeights)
		for(cov=0; cov < pMicv->nCov; cov++)
			pCovWeights[cov] /= score;

	return pMicvSet->fScoreFactor * (fMinLogProp - log(score));
}
