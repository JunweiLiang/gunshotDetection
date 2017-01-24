/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: micv
               ------------------------------------------------------------

    Author  :  Andreas Eller
    Module  :  micv.c
    Date    :  $Id: micv.c 3416 2011-03-23 03:02:18Z metze $
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

#include "micv.h"
#include "micvFileAccess.h"

#include "micvFunctions.h"
#include "micvInit.h"
#include "micvEM.h"
#include "micvScore.h"

#include "micvWeights.h"
#include "micvPrototypes.h"

extern TypeInfo micvInfo;


//*****************************************************************************
//*
//*		Micv interface
//*
//*****************************************************************************


int micvPutsItf (ClientData cd, int argc, char *argv[])
{
	Micv* pMicv = (Micv*)cd;
	char* p = fesGetName (pMicv->pMicvSet->pFeatSet, pMicv->nFeat);
	itfAppendResult("{name %s} {useN %d} {feature %s} ", pMicv->name, pMicv->useN, p);
	return TCL_OK;
}


int micvConfigureItf (ClientData cd, char *var, char *val)
{
	Micv* pMicv = (Micv*) cd;

	if (!var) {
		ITFCFG(micvConfigureItf, cd, "name");
		ITFCFG(micvConfigureItf, cd, "useN");

		return TCL_OK;
	}

	ITFCFG_CharPtr (var, val, "name", pMicv->name, 1);
	ITFCFG_Int     (var, val, "useN", pMicv->useN, 1);

	return TCL_ERROR;
}


ClientData micvAccessItf (ClientData cd, char* name, TypeInfo** ti)
{
	Micv* pMicv = (Micv*)cd;

	if(*name == '.')
	{
		if(name[1] =='\0')
		{ 
			if(pMicv->micvType == eMICV)
			{
				itfAppendElement("weights");
				itfAppendElement("means");
			}
			*ti=NULL;
			return NULL; 
		} else {
			if(strcmp(name+1,"weights")==0)
			{ 
				*ti = itfGetType("DMatrix");
				return (ClientData)(pMicv->pPrototypeWeights);
			} else if(strcmp(name+1,"means")==0)
			{
				*ti = itfGetType("DMatrix");
				return (ClientData)(pMicv->pMedian);
			}
			*ti=NULL;
			return NULL;
		}
	}
	return NULL;
}



//*****************************************************************************
//*
//*		MicvSet interface
//*
//*****************************************************************************


ClientData micvsetCreateItf (ClientData cd, int argc, char *argv[])
{
	char* name = NULL;
	FeatureSet* pFeatSet = 0;
	MicvSet* pMicvSet;

	if (itfParseArgv (argv[0], &argc, argv, 1,
		"<name>",       ARGV_STRING, NULL, &name,     NULL,         "name of the set",
		"<featureSet>", ARGV_OBJECT, NULL, &pFeatSet, "FeatureSet", "feature set",
		NULL) != TCL_OK) return 0;

	VERIFY_MALLOC(pMicvSet = (MicvSet*) malloc (sizeof(MicvSet)), return 0);

	// Initialize list
	listInit ((List*) &(pMicvSet->micvList), &micvInfo, sizeof(Micv), MICV_LIST_BLKSIZE);
	pMicvSet->micvList.init   = (ListItemInit*)   micvItemInit;
	pMicvSet->micvList.deinit = (ListItemDeinit*) micvDeinit;

	if (micvsetInit (pMicvSet, name, pFeatSet) == TCL_ERROR)
		return 0;

	return (ClientData) pMicvSet;
}


int micvsetDestroyItf (ClientData cd)
{
	MicvSet* pMicvSet = (MicvSet*) cd;

	if (pMicvSet->useN > 0) {
		ERROR("There exist %d references to this object!\n", pMicvSet->useN);
		return TCL_ERROR;
	}

	if(micvsetDeinit(pMicvSet) == TCL_ERROR) return TCL_ERROR;
	free (pMicvSet);  /* free object itself */

	return TCL_OK;
}


int micvsetInitItf(ClientData cd, int argc, char *argv[])
{
	int ac = argc-1;
	DistribSet* pDistribSet;

	if(itfParseArgv(argv[0], &ac, argv+1, 1,
		"<distribSet>", ARGV_OBJECT, NULL, &pDistribSet, "DistribSet", "DistribSet",
		NULL) != TCL_OK) return TCL_ERROR;

	micvset_initCodebooks((MicvSet*)cd, pDistribSet);

	return TCL_OK;
}


int micvsetInitPTsItf(ClientData cd, int argc, char *argv[])
{
	MicvSet* pMicvSet = (MicvSet*)cd;
	int ac;
	char* parameter;
	int nClusterIterations = pMicvSet->cfg.nPrototypeClusterIterations;
	DistribSet* pDistribSet = NULL;

	// any parameter?
	argc--;
	while(argc > 0)
	{
		ac = 1; argv++; argc--;
		if(itfParseArgv(argv[0], &ac, argv, 1,
			"<parameter>", ARGV_STRING, NULL, &parameter, NULL, "parameter",
			NULL) != TCL_OK) return TCL_ERROR;

		ac = 1; argv++; argc--;
		if(strcmp(parameter, "-iterN") == 0)
		{
			if(itfParseArgv(argv[0], &ac, argv, 1,
				"<clusterIterations>", ARGV_INT, NULL, &nClusterIterations, NULL, "number of PT cluster iterations",
				NULL) != TCL_OK) return TCL_ERROR;

		} else if(strcmp(parameter, "-dss") == 0)
		{
			if(itfParseArgv(argv[0], &ac, argv, 1,
				"<distribSet>", ARGV_OBJECT, NULL, &pDistribSet, "DistribSet", "DistribSet",
				NULL) != TCL_OK) return TCL_ERROR;

		} else {
			ERROR("unknown parameter %s.\n", parameter);
			return TCL_ERROR;
		}
	}

	if(pDistribSet)
		micvset_initPrototypesWithDistribSet(pMicvSet, pDistribSet, nClusterIterations);
	else
		micvset_initPrototypesWithSampleCovs(pMicvSet, nClusterIterations);

	// now that we have prototypes, transform all GMM models into MICV models
	// and initialize the prototype weights
	micvset_allGMMtoMICV(pMicvSet);
	micvset_initWeights(pMicvSet);
	micvset_precalcMICVScoreValues(pMicvSet);

	return TCL_OK;
}


int micvsetEMUpdateItf(ClientData cd, int argc, char *argv[])
{
	return micvset_EMupdate((MicvSet*)cd);
}


int micvsetGMMUpdateItf(ClientData cd, int argc, char *argv[])
{
	return micvset_GMMupdate((MicvSet*)cd);
}


int micvsetUpdateItf(ClientData cd, int argc, char *argv[])
{
	return micvset_update((MicvSet*)cd);
}


int micvsetScoreItf(ClientData cd, int argc, char *argv[])
{
	MicvSet* pMicvSet = (MicvSet*)cd;
	Micv* pMicv;
	int nFrame = 0, modelIdx, ac = argc-1;

	if(itfParseArgv(argv[0], &ac, argv+1, 0,
		"<micv>",	ARGV_LINDEX,	NULL,	&modelIdx,	&(pMicvSet->micvList),	"name of the MICV model",
		"<frame>",	ARGV_INT,		NULL,	&nFrame,	0,						"index of the requested frame",
		NULL) != TCL_OK) return TCL_ERROR;

	pMicv = &pMicvSet->micvList.itemA[modelIdx];
	itfAppendResult("%f", pMicv->scoreFct(pMicv, nFrame, NULL));
	return TCL_OK;
}


int micvsetEstimateWeightsItf(ClientData cd, int argc, char *argv[])
{
	MicvSet* pMicvSet = (MicvSet*)cd;
	Micv* pMicv;
	int ac = argc-1;
	char* parameter;
	int nModel = -1;

	// any parameter?
	argc--;
	while(argc > 0)
	{
		ac = 1; argv++; argc--;
		if(itfParseArgv(argv[0], &ac, argv, 1,
			"<parameter>", ARGV_STRING, NULL, &parameter, NULL, "parameter",
			NULL) != TCL_OK) return TCL_ERROR;

		ac = 1; argv++; argc--;
		if(strcmp(parameter, "-micv") == 0)
		{
			if(itfParseArgv(argv[0], &ac, argv, 1,
				"<micv>", ARGV_LINDEX, NULL, &nModel, &pMicvSet->micvList, "estimate weights for model micv",
				NULL) != TCL_OK) return TCL_ERROR;
		} else {
			ERROR("unknown parameter %s.\n", parameter);
			return TCL_ERROR;
		}
	}

	if(nModel >= 0)
	{
		// estimate only weights of model nModel
		pMicv = &pMicvSet->micvList.itemA[nModel];
		if(pMicv->micvType == eMICV)
		{
			MicvWeightsOptData* pWeightsOptData = micvset_allocWeightsOptData(pMicvSet);
			micv_estimateWeights(pMicv, pWeightsOptData);
			micvset_freeWeightsOptData(pMicvSet, pWeightsOptData);
			micv_precalcMICVScoreValues(pMicv);
		}
	} else {
		// estimate weights of all models
		micvset_estimateWeights(pMicvSet);
		micvset_precalcMICVScoreValues(pMicvSet);
	}

	return TCL_OK;
}


int micvsetEstimatePTsItf(ClientData cd, int argc, char *argv[])
{
	micvset_estimatePrototypes((MicvSet*)cd);
	micvset_precalcMICVScoreValues((MicvSet*)cd);
	return TCL_OK;
}


int micvsetUpdateMICVItf(ClientData cd, int argc, char *argv[])
{
	micvset_updateAllCovariances((MicvSet*)cd);
	micvset_precalcMICVScoreValues((MicvSet*)cd);
	return TCL_OK;
}


int micvsetSaveItf(ClientData cd, int argc, char *argv[])
{
	int ac = argc-1;
	char* filename = NULL;

	if(itfParseArgv(argv[0], &ac, argv+1, 1,
		"<filename>", ARGV_STRING, NULL, &filename, NULL, "filename",
		NULL) != TCL_OK) return TCL_ERROR;

	return micvset_save((MicvSet*)cd, filename);
}


int micvsetLoadItf(ClientData cd, int argc, char *argv[])
{
	int ac = argc-1;
	char* filename = NULL;

	if(itfParseArgv(argv[0], &ac, argv+1, 1,
		"<filename>", ARGV_STRING, NULL, &filename, NULL, "filename",
		NULL) != TCL_OK) return TCL_ERROR;

	return micvset_load((MicvSet*)cd, filename);
}


int micvsetWriteItf(ClientData cd, int argc, char *argv[])
{
	int ac = argc-1;

	char* filename = NULL;

	if(itfParseArgv(argv[0], &ac, argv+1, 1,
		"<filename>", ARGV_STRING, NULL, &filename, NULL, "filename",
		NULL) != TCL_OK) return TCL_ERROR;

	return micvset_writeDesc((MicvSet*)cd, filename);
}


int micvsetReadItf(ClientData cd, int argc, char *argv[])
{
	int ac = argc-1;
	char* filename = NULL;

	if(itfParseArgv(argv[0], &ac, argv+1, 1,
		"<filename>", ARGV_STRING, NULL, &filename, NULL, "filename",
		NULL) != TCL_OK) return TCL_ERROR;

	return micvset_readDesc((MicvSet*)cd, filename);
}


int micvsetWriteModelFileItf(ClientData cd, int argc, char *argv[])
{
	int ac = argc-1;
	char* filename = NULL;

	if(itfParseArgv(argv[0], &ac, argv+1, 1,
		"<filename>", ARGV_STRING, NULL, &filename, NULL, "filename",
		NULL) != TCL_OK) return TCL_ERROR;

	return micvset_writeModelList((MicvSet*)cd, filename);
}


int micvsetQFunctionItf(ClientData cd, int argc, char *argv[])
{
	fprintf(STDERR, "Q = %.6e\n", micvset_QFunction((MicvSet*)cd));
	return TCL_OK;
}


int micvsetCreateAccusItf(ClientData cd, int argc, char *argv[])
{
	MicvSet* pMicvSet = (MicvSet*)cd;
	int m;

	for(m=0; m < pMicvSet->micvList.itemN; m++)
		micv_createAccus(&pMicvSet->micvList.itemA[m]);
	fprintf(STDERR, "MICV accus created\n");

	return TCL_OK;
}


int micvsetFreeAccusItf(ClientData cd, int argc, char *argv[])
{
	MicvSet* pMicvSet = (MicvSet*)cd;
	int m;

	for(m=0; m < pMicvSet->micvList.itemN; m++)
		micv_freeAccus(&pMicvSet->micvList.itemA[m]);
	fprintf(STDERR, "MICV accus released\n");

	return TCL_OK;
}


int micvsetClearAccusItf(ClientData cd, int argc, char *argv[])
{
	MicvSet* pMicvSet = (MicvSet*)cd;
	int m;

	for(m=0; m < pMicvSet->micvList.itemN; m++)
		micv_clearAccus(&pMicvSet->micvList.itemA[m]);
	fprintf(STDERR, "MICV accus cleared\n");

	return TCL_OK;
}


int micvsetSaveAccusItf(ClientData cd, int argc, char *argv[])
{
	int ac = argc-1;
	char* filename = NULL;

	if(itfParseArgv(argv[0], &ac, argv+1, 1,
		"<filename>", ARGV_STRING, NULL, &filename, NULL, "filename",
		NULL) != TCL_OK) return TCL_ERROR;

	micvset_saveAccus((MicvSet*)cd, filename);

	return TCL_OK;
}


int micvsetLoadAccusItf(ClientData cd, int argc, char *argv[])
{
	int ac = argc-1;
	char* filename = NULL;

	if(itfParseArgv(argv[0], &ac, argv+1, 1,
		"<filename>", ARGV_STRING, NULL, &filename, NULL, "filename",
		NULL) != TCL_OK) return TCL_ERROR;

	micvset_loadAccus((MicvSet*)cd, filename);

	return TCL_OK;
}


int micvsetPutsItf (ClientData cd, int argc, char *argv[])
{
	MicvSet* pMicvSet = (MicvSet*) cd;
	char* parameter;
	int i, ac;

	argc--;
	if(argc > 0)
	{
		ac = 1; argv++; argc--;
		if(itfParseArgv(argv[0], &ac, argv, 1,
			"<parameter>", ARGV_STRING, NULL, &parameter, NULL, "parameter (-deactivated, -gmm, -micv)",
			NULL) != TCL_OK) return TCL_ERROR;

		ac = 1; argv++; argc--;
		if(strcmp(parameter, "-deactivated") == 0)
		{
			for(i=0; i < pMicvSet->micvList.itemN; i++)
				if(pMicvSet->micvList.itemA[i].micvType == eDeactivated)
					itfAppendElement("%s", pMicvSet->micvList.itemA[i].name);
			return TCL_OK;
		} else if(strcmp(parameter, "-gmm") == 0)
		{
			for(i=0; i < pMicvSet->micvList.itemN; i++)
				if(pMicvSet->micvList.itemA[i].micvType == eGMM)
					itfAppendElement("%s", pMicvSet->micvList.itemA[i].name);
			return TCL_OK;
		} else if(strcmp(parameter, "-micv") == 0)
		{
			for(i=0; i < pMicvSet->micvList.itemN; i++)
				if(pMicvSet->micvList.itemA[i].micvType == eMICV)
					itfAppendElement("%s", pMicvSet->micvList.itemA[i].name);
			return TCL_OK;
		} else {
			ERROR("unknown parameter %s.\n", parameter);
			return TCL_ERROR;
		}
	}

	return listPutsItf ((ClientData) &(pMicvSet->micvList), argc, argv);
}


int micvsetConfigureItf (ClientData cd, char *var, char *val)
{
	MicvSet* pMicvSet = (MicvSet*) cd;

	if (!var) {
		ITFCFG(micvsetConfigureItf, cd, "name");
		ITFCFG(micvsetConfigureItf, cd, "useN");
		ITFCFG(micvsetConfigureItf, cd, "PTclusterIterN");
		ITFCFG(micvsetConfigureItf, cd, "PTclusterSubset");
		ITFCFG(micvsetConfigureItf, cd, "estimationIterN");

		ITFCFG(micvsetConfigureItf, cd, "WeightsEstIterN");
		ITFCFG(micvsetConfigureItf, cd, "WeightsEstEpsilon");
		ITFCFG(micvsetConfigureItf, cd, "WeightsEstInitialGamma");
		ITFCFG(micvsetConfigureItf, cd, "WeightsEstMinimalGamma");
		ITFCFG(micvsetConfigureItf, cd, "WeightsEstGammaDivider");


		ITFCFG(micvsetConfigureItf, cd, "PTEstInitialIterN");
		ITFCFG(micvsetConfigureItf, cd, "PTEstFinalIterNOuterLoop");
		ITFCFG(micvsetConfigureItf, cd, "PTEstFinalIterNInnerLoop");
		ITFCFG(micvsetConfigureItf, cd, "PTEstSubset");
		ITFCFG(micvsetConfigureItf, cd, "PTEstEpsilon");
		ITFCFG(micvsetConfigureItf, cd, "PTEstInitialGamma");
		ITFCFG(micvsetConfigureItf, cd, "PTEstMinimalGamma");
		ITFCFG(micvsetConfigureItf, cd, "PTEstGammaDivider");

		ITFCFG(micvsetConfigureItf, cd, "scoreFactor");
		return listConfigureItf ((ClientData) &(pMicvSet->micvList), var, val);
	}

	ITFCFG_CharPtr	(var, val, "name", pMicvSet->name, 1);
	ITFCFG_Int		(var, val, "useN", pMicvSet->useN, 1);
	ITFCFG_Int		(var, val, "PTclusterIterN", pMicvSet->cfg.nPrototypeClusterIterations, 0);
	ITFCFG_Float	(var, val, "PTclusterSubset", pMicvSet->cfg.fPTClusterSubset, 0);
	ITFCFG_Int		(var, val, "estimationIterN", pMicvSet->cfg.nEstimationIterations, 0);

	ITFCFG_Int		(var, val, "WeightsEstIterN", pMicvSet->cfg.nWeightsEstIterations, 0);
	ITFCFG_Float	(var, val, "WeightsEstEpsilon", pMicvSet->cfg.fWeightsEstEpsilon, 0);
	ITFCFG_Float	(var, val, "WeightsEstInitialGamma", pMicvSet->cfg.fWeightsEstInitialGamma, 0);
	ITFCFG_Float	(var, val, "WeightsEstMinimalGamma", pMicvSet->cfg.fWeightsEstMinimalGamma, 0);
	ITFCFG_Float	(var, val, "WeightsEstGammaDivider", pMicvSet->cfg.fWeightsEstGammaDivider, 0);

	ITFCFG_Int		(var, val, "PTEstInitialIterN", pMicvSet->cfg.nPTEstInitialIterations, 0);
	ITFCFG_Int		(var, val, "PTEstFinalIterNOuterLoop", pMicvSet->cfg.nPTEstFinalIterationsOuterLoop, 0);
	ITFCFG_Int		(var, val, "PTEstFinalIterNInnerLoop", pMicvSet->cfg.nPTEstFinalIterationsInnerLoop, 0);
	ITFCFG_Float	(var, val, "PTEstSubset", pMicvSet->cfg.fPTSubset, 0);
	ITFCFG_Float	(var, val, "PTEstEpsilon", pMicvSet->cfg.fPTEstEpsilon, 0);
	ITFCFG_Float	(var, val, "PTEstInitialGamma", pMicvSet->cfg.fPTEstInitialGamma, 0);
	ITFCFG_Float	(var, val, "PTEstMinimalGamma", pMicvSet->cfg.fPTEstMinimalGamma, 0);
	ITFCFG_Float	(var, val, "PTEstGammaDivider", pMicvSet->cfg.fPTEstGammaDivider, 0);

	ITFCFG_Float	(var, val, "scoreFactor", pMicvSet->fScoreFactor, 0);

	return listConfigureItf ((ClientData) &(pMicvSet->micvList), var, val);
}


ClientData micvsetAccessItf (ClientData cd, char *name, TypeInfo **ti)
{
	MicvSet* pMicvSet = (MicvSet*) cd;
	int l;

	if (*name == '.') {
		if (name[1] == 0) {
			if (pMicvSet->micvList.itemN)
				itfAppendResult ("item(0..%d) ", pMicvSet->micvList.itemN-1);
			if (pMicvSet->pPrototypes)
				itfAppendResult ("proto(0..%d) ", pMicvSet->nPrototypes-1);
			*ti = 0;
			return 0;
		} else if (sscanf (name+1,"item(%d)",&l) == 1) {
			if (l >= 0 && l < pMicvSet->micvList.itemN) {
				*ti = itfGetType ("Micv");
				return (ClientData) &(pMicvSet->micvList.itemA[l]);
			} else {
				*ti = 0;
				return 0;
			}
		} else if (pMicvSet->pPrototypes && sscanf (name+1,"proto(%d)",&l) == 1) {
			if (l >= 0 && l < pMicvSet->nPrototypes) {
				*ti = itfGetType ("DMatrix");
				return (ClientData)(pMicvSet->pPrototypes[l]);
			} else {
				*ti = 0;
				return 0;
			}
		}
	}

	return (ClientData) listAccessItf ((ClientData) &(pMicvSet->micvList), name, ti);
}


int micvsetIndexItf (ClientData cd, int argc, char *argv[])
{
	MicvSet* pMicvSet = (MicvSet*) cd;
	return listName2IndexItf ((ClientData) &(pMicvSet->micvList), argc, argv);
}


int micvsetIndex (MicvSet* pMicvSet, char *name)
{
	return listIndex ((List*) &(pMicvSet->micvList), (ClientData) name, 0);
}


int micvsetNameItf (ClientData cd, int argc, char *argv[])
{
	MicvSet* pMicvSet = (MicvSet*) cd;

	return listIndex2NameItf ((ClientData) &(pMicvSet->micvList), argc, argv);
}


char* micvsetName (MicvSet* pMicvSet, int index)
{
	if (index < 0 || index >= pMicvSet->micvList.itemN) return "(null)";
	else return pMicvSet->micvList.itemA[index].name;
}


int micvsetAddItf (ClientData cd, int argc, char *argv[])
{
	MicvSet* pMicvSet = (MicvSet*) cd;
	int ac = argc-1;
	char* name = NULL, *feat = NULL;
	int num_dens = 1,feat_ind,index;
	Micv* pMicv;

	if (itfParseArgv (argv[0],&ac,argv+1,1,
		"<cbName>",   ARGV_STRING,NULL, &name,     NULL, "name for codebook",
		"<featName>", ARGV_STRING,NULL, &feat,     NULL, "name of feature to use",
		"<refN>",     ARGV_INT,   NULL, &num_dens, NULL, "number of densities",
		NULL) != TCL_OK) return TCL_ERROR;

	if (num_dens <= 0) {
		ERROR("<refN> must be positive!\n");
		return TCL_ERROR;
	}

	if (listIndex ((List*) &(pMicvSet->micvList), (ClientData) name,0) >= 0) {
		ERROR("'%s' already exists!\n",name);
		return TCL_ERROR;
	}

	//  if ((feat_ind = fesIndex (pMicvSet->pFeatSet, feat, FE_FMATRIX, pMicvSet->nDim, 1)) < 0) {
	if (fesIndex (pMicvSet->pFeatSet, feat, FE_FMATRIX, pMicvSet->pFeatSet->featA->dcoeffN, 1) < 0) {
		ERROR("Feature '%s' has wrong configuration or cannot be created!\n", feat);
		return TCL_ERROR;
	}
	//  if ((feat_ind = fesIndex (pMicvSet->pFeatSet, feat, FE_FMATRIX, pMicvSet->nDim, 1)) < 0) {
	if ((feat_ind = fesIndex (pMicvSet->pFeatSet, feat, FE_FMATRIX, pMicvSet->pFeatSet->featA->dcoeffN, 1)) < 0) {
		ERROR("Feature '%s' has wrong configuration or cannot be created!\n", feat);
		return TCL_ERROR;
	}

	index = listNewItem ((List*) &(pMicvSet->micvList), (ClientData) name);

	pMicv = &(pMicvSet->micvList.itemA[index]);

	if (micvInit (pMicv, num_dens, pMicvSet, feat_ind) == TCL_ERROR) {
		ERROR("<= called by\n");
		listDelete ((List*) &(pMicvSet->micvList),( ClientData) name);
		pMicvSet->pFeatSet->featA[feat_ind].useN--;
		return TCL_ERROR;
	}

	// resize mixture weights vector
	pMicvSet->nGaussians += pMicv->nCov;

	return TCL_OK;
}


int micvsetDeleteItf(ClientData cd, int argc, char *argv[])
{
	MicvSet* pMicvSet = (MicvSet*)cd;
	return listDeleteItf((ClientData)&(pMicvSet->micvList), argc, argv);
}




//*****************************************************************************
//*
//*		Model initializaiton
//*
//*****************************************************************************

/* -------------------------------------------------------------------
   Micv_Init
   Type constructor for Micv.
   ------------------------------------------------------------------- */

static Method micvMethod[] = {
  { "puts",				micvPutsItf,			NULL},
  
  {  NULL,       NULL,              NULL }
};

TypeInfo micvInfo = {
  "Micv", 0, -1, micvMethod, 0, 0,
  micvConfigureItf, micvAccessItf,
  itfTypeCntlDefaultNoLink,
  "Codebook using Mixture of Inverse CoVarainces\n"
};


/* -------------------------------------------------------------------
   MicvSet_Init
   Type constructor for MicvSet.
   ------------------------------------------------------------------- */

static Method micvsetMethod[] = {
  { "puts",				micvsetPutsItf,				NULL},
  { "index",			micvsetIndexItf,			"get index of list element" },
  { "name",				micvsetNameItf,				"get name of list element" },
  { "add",				micvsetAddItf,				"add new codebook" },
  { "delete",			micvsetDeleteItf,			"delete a codebook" },

  { "init",				micvsetInitItf,				"init MICV models using a DistribSet" },
  { "initPTs",			micvsetInitPTsItf,			"init MICV prototypes" },
  { "update",			micvsetUpdateItf,			"update all MICV codebooks and reestimate MICV model" },
  { "EMupdate",			micvsetEMUpdateItf,			"update all MICV codebooks" },
  { "GMMupdate",		micvsetGMMUpdateItf,		"update all GMM codebooks within MICV" },
  { "score",			micvsetScoreItf,			"computes the score of a mixture distribution" },
  { "estimateWeights",	micvsetEstimateWeightsItf,	"reestimate the MICV prototype weigts" },
  { "estimatePTs",		micvsetEstimatePTsItf,		"reestimate the MICV prototypes" },
  { "updateMICV",		micvsetUpdateMICVItf,		"update covariances and refresh precalculated score values" },
  
  { "save",				micvsetSaveItf,				"save MicvSet to a file" },
  { "load",				micvsetLoadItf,				"load MicvSet from a file" },
  { "write",			micvsetWriteItf,			"write MicvSet description to a file" },
  { "read",				micvsetReadItf,				"read MicvSet description from a file" },
  { "writeModels",		micvsetWriteModelFileItf,	"write MICV model list to a file" },
  { "QFunction",		micvsetQFunctionItf,		"calculate and print Q-Function" },
  { "createAccus",		micvsetCreateAccusItf,		"create accumulators" },
  { "freeAccus",		micvsetFreeAccusItf,		"free accumulators" },
  { "clearAccus",		micvsetClearAccusItf,		"clear accumulators" },
  { "loadAccus",		micvsetLoadAccusItf,		"load accumulators from a file" },
  { "saveAccus",		micvsetSaveAccusItf,		"save accumulators to a file" },

  {  NULL,			NULL,					NULL }
};

TypeInfo micvsetInfo = {
  "MicvSet", 0, -1, micvsetMethod,
  micvsetCreateItf, micvsetDestroyItf,
  micvsetConfigureItf, micvsetAccessItf,
  itfTypeCntlDefaultNoLink, "Set of Mixture of Inverse CoVariances Codebooks\n"
};

static ModelSet micvsetModel = 
             { "MicvSet", &micvsetInfo, &micvInfo,
               (ModelSetIndexFct*) micvsetIndex,
               (ModelSetNameFct *) micvsetName,
               (ModelSetDistFct *) 0
			 };

static int micvsetInitialized = 0;

int MicvSet_Init ()
{
  if (micvsetInitialized) return TCL_OK;

  itfNewType(&micvsetInfo);
  modelSetNewType(&micvsetModel);

  micvsetInitialized = 1;

  return TCL_OK;
}

/* -------------------------------------------------------------------
     Micv_Init
   -------------
     Type constructor for MicvSet.
   ------------------------------------------------------------------- */

static int micvInitialized = 0;

int Micv_Init ()
{
  if (micvInitialized) return TCL_OK;

  if (ModelSet_Init() != TCL_OK) return TCL_ERROR;

  itfNewType      (&micvInfo);
  itfNewType      (&micvsetInfo);
  modelSetNewType (&micvsetModel);

  micvInitialized = 1;

  return TCL_OK;
}



//-----------------------------------------------------------------------------
// debugging functions
//-----------------------------------------------------------------------------

#ifdef _DEBUG

void micvset_Assert(MicvSet* pMicvSet)
{
	int i;

	for(i=0; i < pMicvSet->nPrototypes; i++)
		MICV_ASSERTMATRIX(pMicvSet->pPrototypes[i]);

	MICV_ASSERTMATRIX(pMicvSet->pPrototypesVT);

	for(i=0; i < pMicvSet->micvList.itemN; i++)
		MICV_ASSERT(&pMicvSet->micvList.itemA[i]);
}

void micv_Assert(Micv* pMicv)
{
	int cov;

	if(pMicv->micvType == eDeactivated)
		return;

	if(pMicv->pAccu)
	{
		MICV_ASSERTMATRIX(pMicv->pAccu->pFeatVecSum);
		for(cov=0; cov < pMicv->nCov; cov++)
		{
			assert(!_isnan(pMicv->pAccu->pMixtureWeightSum[cov]) && _finite(pMicv->pAccu->pMixtureWeightSum[cov]));
			MICV_ASSERTMATRIX(pMicv->pAccu->pSampleCov[cov]);
			MICV_ASSERTMATRIX(pMicv->pAccu->pSampleCovV[cov]);
		}
	}

	MICV_ASSERTVECTOR(pMicv->pMixtureWeights);
	MICV_ASSERTMATRIX(pMicv->pMedian);
	MICV_ASSERTMATRIX(pMicv->pPrototypeWeights);

	for(cov=0; cov < pMicv->nCov; cov++)
	{
		MICV_ASSERTMATRIX(pMicv->pInvCov[cov]);
		ASSERT(dmatrixDet(pMicv->pInvCov[cov]) > 0);
		ASSERT(micv_isPositiveDefinite(pMicv->pInvCov[cov]));
	}
}

#endif
