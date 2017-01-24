/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: micv
               ------------------------------------------------------------

    Author  :  Andreas Eller
    Module  :  micv
    Date    :  $Id: micvMultithread.c 3296 2010-07-20 20:12:06Z metze $
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

#include "micvMultithread.h"
#include "micvDefines.h"


#ifdef MTHREAD

#include "micvEM.h"
#include "micvWeights.h"
#include "micvFunctions.h"


typedef struct MicvThreadParams {
	struct MicvSet*	pMicvSet;
	int				nId;
} MicvThreadParams;


/* ----------------------------------------------------------------------------
 *	create and initialize the MICV threads
 * ----------------------------------------------------------------------------*/
void micvset_initThreads(MicvSet* pMicvSet)
{
	int i;
	MicvThreadParams* pThreadParams;
	VERIFY_MALLOC(pThreadParams = malloc(sizeof(MicvThreadParams)), return);
	pthread_attr_init(&pMicvSet->thread_Attr);
	pthread_attr_setscope(&pMicvSet->thread_Attr, PTHREAD_SCOPE_SYSTEM);
	for(i=0; i < MICV_THREADS; i++)
	{
		pThreadParams->pMicvSet = pMicvSet;
		pThreadParams->nId = i;
		sem_init(&pMicvSet->thread_Start_Sem[i], 0, 0);
		sem_init(&pMicvSet->thread_Ready_Sem[i], 0, 0);
		pthread_create(&pMicvSet->thread_ID[i], &pMicvSet->thread_Attr, micvset_Thread, (void*)pThreadParams);
		sem_wait(&pMicvSet->thread_Ready_Sem[i]);
	}

	free(pThreadParams);
}


/* ----------------------------------------------------------------------------
 *	terminate and release the MICV threads
 * ----------------------------------------------------------------------------*/
void micvset_endThreads(MicvSet* pMicvSet)
{
	int i;

	micvset_MultithreadStarter(pMicvSet, eEndThread);
	for(i=0; i < MICV_THREADS; i++)
	{
		pthread_join(pMicvSet->thread_ID[i], NULL);
		sem_destroy(&pMicvSet->thread_Start_Sem[i]);
		sem_destroy(&pMicvSet->thread_Ready_Sem[i]);
	}
}


/* ----------------------------------------------------------------------------
 *	starts the MICV threads to perform a specified command
 * ----------------------------------------------------------------------------*/
void micvset_MultithreadStarter(MicvSet* pMicvSet, MicvThreadCommand command)
{
	int i;

	// set command to be executed by the threads
	pMicvSet->thread_Command = command;

	// start all threads
	for(i=0; i < MICV_THREADS; i++)
		sem_post(&pMicvSet->thread_Start_Sem[i]);

	// wait for all threads to end
	for(i=0; i < MICV_THREADS; i++)
		sem_wait(&pMicvSet->thread_Ready_Sem[i]);
}


/* ----------------------------------------------------------------------------
 *	main thread method of the MICV threads
 * ----------------------------------------------------------------------------*/
void* micvset_Thread(void* arg)
{
	MicvThreadParams* pParams = (MicvThreadParams*)arg;
	MicvSet* pMicvSet = pParams->pMicvSet;
	int nThreadID = pParams->nId;
	int i, m, cov;
	Micv* pMicv;

	sem_post(&pMicvSet->thread_Ready_Sem[nThreadID]);

	while(TRUE)
	{
		// wait for signal to start the thread
		sem_wait(&pMicvSet->thread_Start_Sem[nThreadID]);

		switch(pMicvSet->thread_Command)
		{
		case eEstimateWeights:
		{
			int i;
			MicvWeightsOptData* pWeightsOptData = micvset_allocWeightsOptData(pMicvSet);

			for(i = nThreadID; i < pMicvSet->micvList.itemN; i += MICV_THREADS)
				micv_estimateWeights(&pMicvSet->micvList.itemA[i], pWeightsOptData);

			micvset_freeWeightsOptData(pMicvSet, pWeightsOptData);

			break;
		}

		case eUpdateAllCovariances:
			for(m = nThreadID; m < pMicvSet->micvList.itemN; m += MICV_THREADS)
			{
				pMicv = &pMicvSet->micvList.itemA[m];
				// only update the inverse covariances of pMicv if it's a MICV model
				if(pMicv->micvType == eMICV)
					for(i=0; i < pMicv->nCov; i++)
						micv_updateCovariance(pMicv, i);
			}
			break;

		case eValidateAllCovariances:
			{
				for(m = nThreadID; m < pMicvSet->micvList.itemN; m += MICV_THREADS)
				{
					// break if this or another thread has found an invalid covariance
					if(!pMicvSet->bAllCovariancesValid)
						break;

					pMicv = &pMicvSet->micvList.itemA[m];

					if(pMicv->micvType != eMICV)
						continue;

					if(!micv_validateAllCovariances(pMicv))
						pMicvSet->bAllCovariancesValid = FALSE;
				}
				break;
			}

		case eCalcPTGradient:
			{
				int j;
				double fWeight;
				DMatrix* pCov =	pMicvSet->pPTOptData->pCovMT[nThreadID];
				DMatrix* pGradient = pMicvSet->pPTOptData->pPTGradientMT[nThreadID];
				int nPrototype = pMicvSet->pPTOptData->nPTGradientIdx;

				micv_zeroMatrix(pGradient);

				for(m = nThreadID; m < pMicvSet->nSubset; m += MICV_THREADS)
				{
					pMicv = pMicvSet->pSubset[m].pMicv;
					cov = pMicvSet->pSubset[m].nCov;
					fWeight = pMicv->pMixtureWeights->vecA[cov] * pMicv->pPrototypeWeights->matPA[cov][nPrototype];

					dmatrixInv(pCov, pMicv->pInvCov[cov]);
					for(j=0; j < pMicvSet->nDim; j++)
						for(i=j; i < pMicvSet->nDim; i++)
							pGradient->matPA[j][i] += fWeight * (pCov->matPA[j][i] - pMicv->pAccu->pSampleCov[cov]->matPA[j][i]);
				}

				break;
			}

		case eEndThread:
			// send thread ended signal
			sem_post(&pMicvSet->thread_Ready_Sem[nThreadID]);
			return 0;
		}

		// send thread ended signal
		sem_post(&pMicvSet->thread_Ready_Sem[nThreadID]);
	}

	return 0;
}

#else	// MICV_MULTITHREADED

void	micvset_initThreads			(MicvSet* pMicvSet) { /* do nothing in single threaded mode */ }
void	micvset_endThreads			(MicvSet* pMicvSet) { /* do nothing in single threaded mode */ }
void	micvset_MultithreadStarter	(MicvSet* pMicvSet, MicvThreadCommand command) { /* do nothing in single threaded mode */ }
void*	micvset_Thread				(void* arg) { return 0; }

#endif	// MICV_MULTITHREADED

