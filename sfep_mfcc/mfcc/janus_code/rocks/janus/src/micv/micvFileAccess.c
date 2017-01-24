/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: micv
               ------------------------------------------------------------

    Author  :  Andreas Eller
    Module  :  micv
    Date    :  $Id: micvFileAccess.c 2864 2009-02-16 21:18:17Z metze $
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

#include "micvFileAccess.h"
#include "micvFunctions.h"
#include "matrix.h"

#include "string.h"
#include "stdio.h"
#include "micvInit.h"
#include "micvEM.h"
#include "micvScore.h"
#include "micvWeights.h"
#include "micvPrototypes.h"

//-----------------------------------------------------------------------------
//	save micv set
//-----------------------------------------------------------------------------
int micvset_save(MicvSet* pMicvSet, char* sFilename)
{
	Micv* pMicv;
	int k, m, i, cov;
	float fValue;
	FILE* pFile;

	if((pFile = fileOpen(sFilename, "w")) == 0)
	{
		ERROR("unable to create file %s\n", sFilename);
		return TCL_ERROR;
	}

	fprintf(STDERR, "saving MICV %s:", sFilename);

	// save MICV settings
	fwrite(&pMicvSet->nDim, sizeof(pMicvSet->nDim), 1, pFile);

	// save prototypes
	if(pMicvSet->pPrototypes)
	{
		fwrite(&pMicvSet->nPrototypes, sizeof(pMicvSet->nPrototypes), 1, pFile);
		for(k=0; k < pMicvSet->nPrototypes; k++)
			micvset_saveMatrix(pFile, pMicvSet->pPrototypes[k]);
		fprintf(STDERR, "  %d PTs", pMicvSet->nPrototypes);
	} else {
		k=0;
		fwrite(&k, sizeof(k), 1, pFile);
		fprintf(STDERR, "  no PTs");
	}

	// save MICV models
	m = pMicvSet->micvList.itemN;
	fwrite(&m, sizeof(m), 1, pFile);
	fprintf(STDERR, ", %d models\n", m);
	for(m=0; m < pMicvSet->micvList.itemN; m++)
	{
		pMicv = &pMicvSet->micvList.itemA[m];
		micvset_saveString(pFile, pMicv->name);
		fwrite(&pMicv->micvType, sizeof(MicvType), 1 , pFile);
		micvset_saveString(pFile, pMicvSet->pFeatSet->featA[pMicv->nFeat].name);
		fwrite(&pMicv->nCov, sizeof(pMicv->nCov), 1, pFile);

		if(pMicv->micvType == eDeactivated)
			continue;		// no need to save any more data

		micvset_saveMatrix(pFile, pMicv->pMedian);
		micvset_saveVector(pFile, pMicv->pMixtureWeights);
		micvset_saveVector(pFile, pMicv->pLogMixtureWeights);

		switch(pMicv->micvType)
		{
		case eGMM:
			// save inverse covariances
			for(cov=0; cov < pMicv->nCov; cov++)
			{
				CovType covType = pMicv->pCovType[cov];
				fwrite(&covType, sizeof(covType), 1, pFile);
				switch(pMicv->pCovType[cov])
				{
				case COV_DIAGONAL:
					for(i=0; i < pMicvSet->nDim; i++)
					{
						fValue = pMicv->pInvCov[cov]->matPA[i][i];
						fwrite(&fValue, sizeof(fValue), 1, pFile);
					}
					break;
					
				default:
					// this should work for any matrix type
					micvset_saveMatrix(pFile, pMicv->pInvCov[cov]);
				}
			}
			break;

		case eMICV:
			// save inverse covariances as mixture of prototypes
			micvset_saveMatrix(pFile, pMicv->pPrototypeWeights);
			break;

		default:
			ERROR("unknown MICV type %d\n", pMicv->micvType);
		}
	}

	fileClose(sFilename, pFile);
	return TCL_OK;
}


//-----------------------------------------------------------------------------
//	load micv set
//-----------------------------------------------------------------------------
int micvset_load(MicvSet* pMicvSet, char* sFilename)
{
	Micv* pMicv;
	int k, m, cov, i;
	int nModels, nCov, nDim, nPrototypes;
	MicvType micvType;
	double fDet;
	float fValue;
	FILE* pFile;
	char sName[256];
	char sFeat[256];

	if((pFile = fileOpen(sFilename, "r")) == 0)
	{
		ERROR("unable to open file %s\n", sFilename);
		return TCL_ERROR;
	}

	fprintf(STDERR, "loading MICV %s:\n", sFilename);

	micvset_deactivateAllModels(pMicvSet);
	// control if algorithm is correct
	assert(pMicvSet->nGaussians == 0);
	pMicvSet->nGaussians = 0;

	// load settings
	fread(&nDim, sizeof(nDim), 1, pFile);
	if(nDim != pMicvSet->nDim)
	{
		ERROR("dimension in file (%d) does not match MICV dimension (%d)\n", nDim, pMicvSet->nDim);
		return TCL_ERROR;
	}

	// load prototypes
	fprintf(STDERR, "   prototypes = ");
	micvset_freePrototypes(pMicvSet);
	fread(&nPrototypes, sizeof(nPrototypes), 1, pFile);
	if(nPrototypes > 0)
	{
		if(nPrototypes != pMicvSet->nPrototypes)
		{
			ERROR("number of prototypes should be %d. Tried to load %d prototypes!\n", pMicvSet->nPrototypes, nPrototypes);
			return TCL_ERROR;
		}
		micvset_allocPrototypes(pMicvSet);
		for(k=0; k < pMicvSet->nPrototypes; k++)
			micvset_loadMatrix(pFile, pMicvSet->pPrototypes[k]);
		micvset_calcPrototypesVT(pMicvSet);
		fprintf(STDERR, "%d\n", pMicvSet->nPrototypes);
	} else
		fprintf(STDERR, "0\n");

	// load MICV models
	fread(&nModels, sizeof(nModels), 1, pFile);
	if(nModels != pMicvSet->micvList.itemN)
		WARN("expected %d but found %d models\n", pMicvSet->micvList.itemN, nModels);

	for(m=0; m < MIN(nModels, pMicvSet->micvList.itemN); m++)
	{
		micvset_loadString(pFile, (char*)sName, 256);
		fread(&micvType, sizeof(micvType), 1 , pFile);
		micvset_loadString(pFile, (char*)sFeat, 256);
		fread(&nCov, sizeof(nCov), 1, pFile);
		pMicv = &(pMicvSet->micvList.itemA[m]);

		if(nCov != pMicv->nCov)
		{
			ERROR("expected %d gaussians in model %s but found %d gaussians\n", pMicv->nCov, pMicv->name, nCov);
			break;
		}

		pMicv->nFeat = fesIndex(pMicvSet->pFeatSet, sFeat, FE_FMATRIX, pMicvSet->nDim, 1);

		if(micvType == eDeactivated)
			continue;		// no more data to load for this codebook

		// this is at least a GMM model
		micv_inactiveToGMM(pMicv);

		micvset_loadMatrix(pFile, pMicv->pMedian);
		micvset_loadVector(pFile, pMicv->pMixtureWeights);
		micvset_loadVector(pFile, pMicv->pLogMixtureWeights);

		switch(micvType)
		{
		case eGMM:
			// load inverse covariances as full matrices
			for(cov=0; cov < pMicv->nCov; cov++)
			{
				CovType covType;
				fread(&covType, sizeof(covType), 1, pFile);
				pMicv->pCovType[cov] = covType;

				switch(pMicv->pCovType[cov])
				{
				case COV_DIAGONAL:
					dmatrixInitConst(pMicv->pInvCov[cov], 0.0);
					for(i=0; i < pMicvSet->nDim; i++)
					{
						fread(&fValue, sizeof(fValue), 1, pFile);
						pMicv->pInvCov[cov]->matPA[i][i] = fValue;
					}
					break;

				default:
					micvset_loadMatrix(pFile, pMicv->pInvCov[cov]);
				}

				fDet = dmatrixDet(pMicv->pInvCov[cov]);
			}
			micv_precalcGMMScoreValues(pMicv);
			pMicv->scoreFct = &micv_scoreGMM;
			break;

		case eMICV:
			micv_GMMtoMICV(pMicv);

			// load inverse covariances as mixture of prototypes
			micvset_loadMatrix(pFile, pMicv->pPrototypeWeights);
			for(cov=0; cov < nCov; cov++)
			{
				micv_updateCovariance(pMicv, cov);
				fDet = dmatrixDet(pMicv->pInvCov[cov]);
			}
			micv_precalcMICVScoreValues(pMicv);
			pMicv->scoreFct = &micv_scoreMICV;
			break;

		default:
			ERROR("unknown MICV type %d\n", pMicv->micvType);
		}
	}

	fprintf(STDERR, "   models     = %d\n", m);

	fileClose(sFilename, pFile);
	return TCL_OK;
}


//-----------------------------------------------------------------------------
//	save micv model description
//-----------------------------------------------------------------------------
int micvset_writeDesc(MicvSet* pMicvSet, char* sFilename)
{
	int m;
	FILE* pFile;
	Micv* pMicv;

	if((pFile = fileOpen(sFilename, "wt")) == 0)
		return TCL_ERROR;

	fprintf(STDERR, "writing MICV description file:   %d dim   %d prototypes   %d models\n", pMicvSet->nDim, pMicvSet->nPrototypes, pMicvSet->micvList.itemN);

	// write description file
	fprintf(pFile, "; -------------------------------------------------------\n");
	fprintf(pFile, ";  Name            : %s\n", pMicvSet->name);
	fprintf(pFile, ";  Type            : MicvSet\n");
	fprintf(pFile, ";  Number of Items : %d\n", pMicvSet->micvList.itemN);
	fprintf(pFile, ";  Date            : %s\n", dateString());
	fprintf(pFile, "; -------------------------------------------------------\n");
	
	// write dimension and #prototypes
	fprintf(pFile, "DIMENSION  %4d\n", pMicvSet->nDim);
	fprintf(pFile, "PROTOTYPES %4d\n", pMicvSet->nPrototypes);

	// write models
	for(m=0; m < pMicvSet->micvList.itemN; m++)
	{
		pMicv = &pMicvSet->micvList.itemA[m];
		fprintf(pFile, "%-15s %-15s %6d\n", pMicv->name, pMicvSet->pFeatSet->featA[pMicv->nFeat].name, pMicv->nCov);
	}


	fileClose(sFilename, pFile);
	return TCL_OK;
}


//-----------------------------------------------------------------------------
//	load micv model description
//-----------------------------------------------------------------------------
int micvset_readDesc(MicvSet* pMicvSet, char* sFilename)
{
	FILE* pFile;
	char sName[128];
	char sFeat[128];
	int nRef, n, nFeat, nModels = 0;
	Micv* pMicv;

	if((pFile = fileOpen(sFilename, "rt")) == 0)
		return TCL_ERROR;

	fprintf(STDERR, "reading MICV description file:");
		
	// free memory
	listClear((List*)&pMicvSet->micvList);
	pMicvSet->nGaussians = 0;
	pMicvSet->nDim = 0;
	micvset_freePrototypes(pMicvSet);

	// read description file
	while(TRUE)
	{
		// read first string item
		if(fscanf(pFile, "%s", sName) < 1) break;
		
		if(sName[0] == ';')	
		{
			// comment -> read complete line and continue
			fgets(sName, 128, pFile);
			continue;
		}

		if(strcmp(sName, "DIMENSION") == 0)
		{
			fscanf(pFile, "%d", &pMicvSet->nDim);
			pMicvSet->nDimV = pMicvSet->nDim * (pMicvSet->nDim + 1) / 2;
			fprintf(STDERR, "   %d dim,", pMicvSet->nDim);
			continue;
		
		} else if(strcmp(sName, "PROTOTYPES") == 0)
		{
			fscanf(pFile, "%d", &pMicvSet->nPrototypes);
			fprintf(STDERR, "   %d PTs,", pMicvSet->nPrototypes);
			continue;
		}

		if(pMicvSet->nDim == 0)
		{
			ERROR("define MICV dimension first in description file before adding models!");
			break;
		}

		// read remaining items
		if(fscanf(pFile, "%s %d", sFeat, &nRef) < 2) break;

		// add model
		n = listNewItem((List*)&(pMicvSet->micvList), (ClientData)sName);
		pMicv = &(pMicvSet->micvList.itemA[n]);
		nFeat = fesIndex(pMicvSet->pFeatSet, sFeat, FE_FMATRIX, pMicvSet->nDim, 1);

		if(micvInit(pMicv, nRef, pMicvSet, nFeat) == TCL_ERROR) {
			ERROR("<= called by\n");
			listDelete((List*)&(pMicvSet->micvList), (ClientData)sName);
			pMicvSet->pFeatSet->featA[nFeat].useN--;
			return TCL_ERROR;
		}

		nModels++;
	}

	fprintf(STDERR, "   %d models\n", nModels);

	fileClose(sFilename, pFile);
	return TCL_OK;
}



//-----------------------------------------------------------------------------
//	save micv accumulators
//-----------------------------------------------------------------------------
int micvset_saveAccus(MicvSet* pMicvSet, char* sFilename)
{
	int m, cov;
	Micv* pMicv;
	MicvAccu* pAccu;
	FILE* pFile;
	
	if((pFile = fileOpen(sFilename, "w")) == 0)
	{
		ERROR("unable to create file %s\n", sFilename);
		return TCL_ERROR;
	}

	fprintf(STDERR, "saving MICV accus %s\n", sFilename);
	
	fwrite(&pMicvSet->micvList.itemN, sizeof(pMicvSet->micvList.itemN), 1, pFile);
	for(m = 0; m < pMicvSet->micvList.itemN; m++)
	{
		pMicv = &pMicvSet->micvList.itemA[m];
		pAccu = pMicv->pAccu;
		micvset_saveString(pFile, pMicv->name);

		fwrite(&pAccu->nAccumulatedFrames, sizeof(pAccu->nAccumulatedFrames), 1, pFile);
		micvset_saveMatrix(pFile, pAccu->pFeatVecSum);
		fwrite(pAccu->pMixtureWeightSum, sizeof(double), pMicv->nCov, pFile);
		for(cov = 0; cov < pMicv->nCov; cov++)
		{
			micvset_saveMatrix(pFile, pAccu->pSampleCov[cov]);
		}

		// Debug code (FM)
		/*
		for (cov = 0; cov < pMicv->nCov; cov++)
		  if (pAccu->pMixtureWeightSum[cov] <= 0)
		    INFO ("micvSetSaveAccus: m= %d %s cov= %d : 0 <= %s\n",
			  m, pMicv->name, cov, sFilename);
		*/
	}

	fflush(pFile);
	fileClose(sFilename, pFile);
	return TCL_OK;
}


//-----------------------------------------------------------------------------
//	load micv accumulators
//-----------------------------------------------------------------------------
int micvset_loadAccus(MicvSet* pMicvSet, char* sFilename)
{
	int m, cov, idx;
	int nModels, nFrames;
	Micv* pMicv;
	MicvAccu* pAccu;
	DMatrix* pMatrix1 = dmatrixCreate(1, pMicvSet->nDim);				// cov x dim temp matrix
	DMatrix* pMatrix2 = dmatrixCreate(pMicvSet->nDim, pMicvSet->nDim);	// dim x dim temp matrix
	double fTemp;
	FILE* pFile;
	char sName[256];

	if((pFile = fileOpen(sFilename, "r")) == 0)
	{
		ERROR("file %s not found\n", sFilename);
		return TCL_ERROR;
	}

	fread(&nModels, sizeof(nModels), 1, pFile);
	if(nModels != pMicvSet->micvList.itemN)
		WARN(" accumulating %d models, but %d expected!\n", nModels, pMicvSet->micvList.itemN);
	else
		INFO(" accumulating %d models\n", nModels);

	for(m = 0; m < nModels; m++)
	{
		micvset_loadString(pFile, sName, 256);
		idx = micvsetIndex(pMicvSet, sName);
		if(idx < 0)
		{
			WARN(" unable to find model %s found in accu file\n", sName);
			continue;
		}

		pMicv = &pMicvSet->micvList.itemA[idx];
		
		if(pMicv->pAccu == NULL)
			micv_createAccus(pMicv);
		pAccu = pMicv->pAccu;

		// number of accumulated frames
		fread(&nFrames, sizeof(nFrames), 1, pFile);
		pAccu->nAccumulatedFrames += nFrames;

		// accu feature vector sum
		dmatrixResize(pMatrix1, pMicv->nCov, pMicvSet->nDim);
		micvset_loadMatrix(pFile, pMatrix1);
		dmatrixAccu(pAccu->pFeatVecSum, pMatrix1, 1.0);

		// accu mixture weights
		for(cov = 0; cov < pMicv->nCov; cov++)
		{
			fread(&fTemp, sizeof(fTemp), 1, pFile);
			pAccu->pMixtureWeightSum[cov] += fTemp;
		}

		// Debug code (FM)
		/*
		for (cov = 0; cov < pMicv->nCov; cov++)
		  if (pAccu->pMixtureWeightSum[cov] <= 0)
		    INFO ("micvSetLoadAccus: m= %d %s cov= %d : 0 -> %s\n",
			  m, pMicv->name, cov, sFilename);
		*/

		// accu sample covariance
		for(cov = 0; cov < pMicv->nCov; cov++)
		{
			micvset_loadMatrix(pFile, pMatrix2);
			dmatrixAccu(pAccu->pSampleCov[cov], pMatrix2, 1.0);
			pAccu->pSampleCov[cov]->count += pMatrix2->count;
		}
	}

	dmatrixFree(pMatrix1);
	dmatrixFree(pMatrix2);
	fileClose(sFilename, pFile);
	return TCL_OK;
}


//-----------------------------------------------------------------------------
//	save micv model list (used for parallel weights update)
//-----------------------------------------------------------------------------
int micvset_writeModelList(MicvSet* pMicvSet, char* sFilename)
{
    FILE* pFile;
	int m;
	Micv* pMicv;

	if((pFile = fileOpen(sFilename, "wt")) == 0)
	{
		ERROR("file %s not found\n", sFilename);
		return TCL_ERROR;
	}

	for(m=0; m < pMicvSet->micvList.itemN; m++)
	{
		pMicv = &pMicvSet->micvList.itemA[m];
		if(pMicv->micvType == eMICV)
			fprintf(pFile, "%s\n", pMicv->name);
	}

	fileClose(sFilename, pFile);
	return TCL_OK;
}


//-----------------------------------------------------------------------------
//	helper functions
//-----------------------------------------------------------------------------

void micvset_saveMatrix(FILE* pFile, DMatrix* pMatrix)
{
	int m, n;
	double d;
	m = pMatrix->m; fwrite(&m, sizeof(m), 1, pFile);
	n = pMatrix->n; fwrite(&n, sizeof(n), 1, pFile);
	d = pMatrix->count; fwrite(&d, sizeof(d), 1, pFile);
	fwrite(pMatrix->matPA[0], sizeof(double), pMatrix->m * pMatrix->n, pFile);
}


void micvset_loadMatrix(FILE* pFile, DMatrix* pMatrix)
{
	int m, n;
	double d;
	fread(&m, sizeof(m), 1, pFile);
	fread(&n, sizeof(n), 1, pFile);
	dmatrixResize(pMatrix, m, n);
	fread(&d, sizeof(d), 1, pFile);
	pMatrix->count = d;
	fread(pMatrix->matPA[0], sizeof(double), pMatrix->m * pMatrix->n, pFile);
}


void micvset_saveVector(FILE* pFile, DVector* pVector)
{
	int i;
	double d;
	i = pVector->n; fwrite(&i, sizeof(i), 1, pFile);
	d = pVector->count; fwrite(&d, sizeof(d), 1, pFile);
	fwrite(pVector->vecA, sizeof(double), pVector->n, pFile);
}


void micvset_loadVector(FILE* pFile, DVector* pVector)
{
	int i;
	double d;
	fread(&i, sizeof(i), 1, pFile);
	dvectorResize(pVector, i);
	fread(&d, sizeof(d), 1, pFile);
	pVector->count = d;
	fread(pVector->vecA, sizeof(double), pVector->n, pFile);
}


void micvset_saveString(FILE* pFile, char* str)
{
	int n = strlen(str) + 1;
	fwrite(&n, sizeof(n), 1, pFile);
	fwrite(str, n, 1, pFile);
}


void micvset_loadString(FILE* pFile, char* str, const int nMaxSize)
{
	int n;
	fread(&n, sizeof(n), 1, pFile);
	if(n > nMaxSize)
		n = nMaxSize;
	fread(str, n, 1, pFile);
}


