/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Modalities Maintainance Functions
               ------------------------------------------------------------

    Author  :  Christian Fuegen
    Module  :  modalities.c
    Date    :  $Id: modalities.c 3416 2011-03-23 03:02:18Z metze $
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
   Revision 1.8  2005/02/24 16:06:55  metze
   Cosmetics for x86

   Revision 1.7  2003/08/14 11:20:16  fuegen
   Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

   Revision 1.6.20.2  2002/08/27 08:43:02  metze
   itfParseArgv now uses <name> and NULL initialization for char*

   Revision 1.6.20.1  2002/06/26 11:57:56  fuegen
   changes for new directory structure support and code cleaning

   Revision 1.6  2000/11/14 12:35:24  janus
   Merging branch jtk-00-10-27-jmcd.

   Revision 1.5.34.2  2000/11/08 17:26:28  janus
   Finished making changes required for compilation under 'gcc -Wall'.

   Revision 1.5.34.1  2000/11/06 10:50:36  janus
   Made changes to enable compilation under -Wall.

   Revision 1.5  2000/08/16 11:35:28  soltau
   fixed  typo
   free -> TclFree

   Revision 1.4  2000/01/12 10:12:15  fuegen
   add updarte

   Revision 1.3  1999/03/09 21:59:59  fuegen
   *** empty log message ***

   Revision 1.2  1999/03/04 11:37:49  fuegen
   *** empty log message ***

   Revision 1.1  1998/12/09 10:51:46  fuegen
   Initial revision


   ======================================================================== */

char modalitiesRCSVersion[]= 
	   "@(#)1$Id: modalities.c 3416 2011-03-23 03:02:18Z metze $";


#include "common.h"
#include "itf.h"
#include "modalities.h"

#include "distrib.h"
#include "tree.h"
#include "modelSet.h"
#include "questions.h"

/* ------------------------------------------------------------------------
    Forward Declaration
   ------------------------------------------------------------------------ */

extern TypeInfo modalityInfo;
extern TypeInfo modalitySetInfo;
extern TypeInfo stateTableInfo;



/* ========================================================================
    StateTable
   ======================================================================== */

static ClientData stateTableCreateItf (ClientData cd, int argc, char *argv[]);
static int        stateTableFreeItf   (ClientData cd);
static int        stateTablePutsItf   (ClientData cd, int argc, char *argv[]);

static StateTable stateTableDefault;

/* ------------------------------------------------------------------------
    Init/Create StateTable Object
   ------------------------------------------------------------------------ */

int stateTableInit (StateTable *stP, char *name, ModalitySet *modsP, int compress) {

    stP->name = strdup(name);
    stP->useN = 0;

    stP->modsP = modsP; link(stP->modsP, "ModalitySet");

    stP->startFrameX = stateTableDefault.startFrameX;
    stP->endFrameX   = stateTableDefault.endFrameX;
    stP->dummyStart  = modsP->dummyStart;
    stP->timeInfo    = stateTableDefault.timeInfo;
    stP->compress    = compress;
    stP->commentChar = stateTableDefault.commentChar;

    stP->modXN  = modsP->list.itemN;
    stP->treeXN = modsP->rootNodeN;
    stP->matrix = stateTableDefault.matrix;
    stP->m      = stP->treeXN;

    if ( stP->compress ) {
	stP->n         = 0;
	stP->compressN = (int)pow(2.0,(double)stP->modXN);
	stP->compressA = stateTableDefault.compressA;
    } else {
	stP->n         = (int)pow(2.0,(double)stP->modXN);
    }

    return TCL_OK;
}

StateTable* stateTableCreate (char *name, ModalitySet *modsP, int compress) {

    StateTable *stP = (StateTable*)malloc(sizeof(StateTable));
    int i, j;

    if ( !stP || stateTableInit (stP, name, modsP, compress) != TCL_OK ) {
	if ( stP ) free (stP);
	ERROR("Failed to allocate modality set '%s'.\n", name);
	return NULL;
    }

    stP->matrix = imatrixCreate (stP->m, stP->n);
    link(stP->matrix, "IMatrix");

    if ( !stP->matrix ) {
	if ( stP ) free (stP);
	ERROR("Failed to allocate imatrix in '%s'.\n",stP->name);
	return NULL;
    }

    for (i = 0; i < stP->m; i++) {
	for (j = 0; j < stP->n; j++) {
	    stP->matrix->matPA[i][j] = -1;
	}
    }

    if ( stP->compress ) {
	stP->compressA = (int*)malloc (stP->compressN * sizeof(int));

	for (i = 0; i < stP->compressN; i++)
	    stP->compressA[i] = -1;
    }

    return stP;
}

static ClientData stateTableCreateItf (ClientData cd, int argc, char *argv[]) {

    ModalitySet *modsP    = stateTableDefault.modsP;
    int          compress = stateTableDefault.compress;
    char* name = NULL;

    if ( itfParseArgv (argv[0], &argc, argv, 0,
      "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the state table",
		       "<modalitySet>", ARGV_OBJECT, NULL, &modsP, "ModalitySet",
		       		"modality set",
		       "-compress", ARGV_INT, NULL, &compress, NULL,
		       		"compress stateTable",
		       NULL) != TCL_OK )
	return NULL;

    return (ClientData)stateTableCreate (name, modsP, compress);
}


/* ------------------------------------------------------------------------
    Deinit/Free StateTable Object
   ------------------------------------------------------------------------ */

int stateTableLinkN (StateTable *stP) {

    return stP->useN;
}

int stateTableDeinit (StateTable *stP) {

    if ( stateTableLinkN (stP) ) {
	SERROR("StateTable '%s' still in use by other objects.\n", stP->name);
	return TCL_ERROR;
    }

    if ( stP->name )   { free (stP->name); stP->name = NULL; }
    if ( stP->modsP )  { unlink (stP->modsP, "ModalitySet"); }

    /*    imatrixDeinit (stP->matrix); */
    if ( stP->compressA ) { free (stP->compressA); stP->compressA = NULL; }

    return TCL_OK;
}

int stateTableFree (StateTable *stP) {

    if ( stateTableDeinit (stP) != TCL_OK ) return TCL_ERROR;
    imatrixFree (stP->matrix);
    if ( stP ) free (stP);

    return TCL_OK;
}

static int stateTableFreeItf (ClientData cd) {

    return stateTableFree ((StateTable*)cd);
}

/* ------------------------------------------------------------------------
    Configure/Access StateTable Object
   ------------------------------------------------------------------------ */

int stateTableConfigureItf (ClientData cd, char *var, char *val) {

    StateTable *stP = (StateTable*)cd;

    if ( !stP ) stP = &stateTableDefault;

    if ( var == NULL ) {
	ITFCFG(stateTableConfigureItf, cd, "name");
	ITFCFG(stateTableConfigureItf, cd, "useN");
	ITFCFG(stateTableConfigureItf, cd, "startFrameX");
	ITFCFG(stateTableConfigureItf, cd, "endFrameX");
	ITFCFG(stateTableConfigureItf, cd, "treeXN");
	ITFCFG(stateTableConfigureItf, cd, "modXN");
	ITFCFG(stateTableConfigureItf, cd, "dummyStart");
	ITFCFG(stateTableConfigureItf, cd, "timeInfo");
	ITFCFG(stateTableConfigureItf, cd, "compress");
	ITFCFG(stateTableConfigureItf, cd, "commentChar");
	return TCL_OK;
    }

    ITFCFG_CharPtr (var, val, "name",        stP->name,        1);
    ITFCFG_Int     (var, val, "useN",        stP->useN,        1);
    ITFCFG_Int     (var, val, "startFrameX", stP->startFrameX, 1);
    ITFCFG_Int     (var, val, "endFrameX",   stP->endFrameX,   1);
    ITFCFG_Int     (var, val, "treeXN",      stP->treeXN,      1);
    ITFCFG_Int     (var, val, "modXN",       stP->modXN,       1);
    ITFCFG_Int     (var, val, "dummyStart",  stP->dummyStart,  1);
    ITFCFG_Int     (var, val, "timeInfo",    stP->timeInfo,   0);
    ITFCFG_Int     (var, val, "compress",    stP->compress,    1);
    ITFCFG_Int     (var, val, "commentChar", stP->commentChar, 0);

    ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");

    return TCL_ERROR;
}

ClientData stateTableAccessItf (ClientData cd, char *name, TypeInfo **ti) {

    StateTable *stP = (StateTable*)cd;

    if ( *name == '.' ) {
	if ( name[1] == '\0' ) {
	    itfAppendElement("modalitySet");
	    itfAppendElement("matrix");
	}
	else {
	    if ( !strcmp(name+1,"modalitySet") ) {
		*ti = itfGetType("ModalitySet");
		return (ClientData)stP->modsP;
	    }
	    else if ( !strcmp(name+1,"matrix") ) {
		*ti = itfGetType("IMatrix");
		return (ClientData)stP->matrix;
	    }
	}
    }

    *ti = NULL;
    return NULL;
}



/* ------------------------------------------------------------------------
    stateTableFill, and others procedures for building the table
   ------------------------------------------------------------------------ */

void stateTableFill (StateTable *stP, int treeX, int mask, int answerY, int answerN,
		    int modelY, int modelN) {
    int firstY = 1;
    int firstN = 1;
    int i, j, ref;

    if ( modelY < 0 && modelN < 0 )
	ERROR("There exists no model for tree '%i'.\n", treeX);
    if ( mask <= 0 )
	ERROR("mask '%i' invalid => answers invalid.\n", mask);

    /*
    INFO("treeX %i, mask %i, answerY %i, answerN %i, modelY %i, modelN %i\n",
	 treeX, mask, answerY, answerN, modelY, modelN);
    */

    if ( stP->compress ) {
	for (i = 0; i < stP->compressN; i++) {
	    if ( modelY >= 0 && answerY == (mask & i) ) {
		ref = stP->compressA[i];

		/* if reference not exists and nothing done so far or
		   reference exists and existing model not equal */
		if ( (ref < 0 && firstY) ||
		     (ref >= 0 && stP->matrix->matPA[treeX][ref] >= 0 &&
		      stP->matrix->matPA[treeX][ref] != modelY) ) {
		    /* reallocate matrix, copy referenced row
		       and add new reference to this row in compressA */
		    stP->compressA[i] = (stP->n)++;
		    imatrixResize (stP->matrix, stP->m, stP->n);
		    if ( ref >= 0 ) {
			for (j = 0; j < stP->m; j++)
			    stP->matrix->matPA[j][ref] =
				stP->matrix->matPA[j][stP->compressA[i]];
		    }
		    ref = stP->compressA[i];
		    stP->matrix->matPA[treeX][ref] = modelY;
		    firstY = 0;
		} else {
		    stP->compressA[i] = ref;
		}		    
	    }
	    if ( modelN >= 0 && answerN == (mask & i) ) {
		ref = stP->compressA[i];

		if ( (ref < 0 && firstN) ||
		     (ref >= 0 && stP->matrix->matPA[treeX][ref] >= 0 &&
		      stP->matrix->matPA[treeX][ref] != modelN) ) {

		    stP->compressA[i] = (stP->n)++;
		    imatrixResize (stP->matrix, stP->m, stP->n);
		    if ( ref >= 0 ) {
			for (j = 0; j < stP->m; j++)
			    stP->matrix->matPA[j][ref] =
				stP->matrix->matPA[j][stP->compressA[i]];
		    }
		    ref = stP->compressA[i];
		    stP->matrix->matPA[treeX][ref] = modelN;
		    firstN = 0;
		} else {
		    stP->compressA[i] = ref;
		}	
	    }
	}

    } else {
	for (i = 0; i < stP->n; i++) {
	    if ( modelY >= 0 && answerY == (mask & i) )
		stP->matrix->matPA[treeX][i] = modelY;
	    if ( modelN >= 0 && answerN == (mask & i) )
		stP->matrix->matPA[treeX][i] = modelN;
	}
    }
}

int stateTableIsLeaf (Tree *tree, int nodeX) {

    int yes, no, undef;

    if ( nodeX < 0 ) {
	ERROR("Node '%i' is already a leaf.\n", nodeX);
	return 1;
    }

    yes   = tree->list.itemA[nodeX].yes;
    no    = tree->list.itemA[nodeX].no;
    undef = tree->list.itemA[nodeX].undef;

    if ( yes < 0 && no < 0 && undef < 0 )
	return 1;

    return 0;
}


int stateTableGetTagX (Tree *tree, Tags *tags, int nodeX) {

    int   qIdx, tIdx;
    char *question = NULL;
    char *tag      = NULL;

    qIdx     = tree->list.itemA[nodeX].question;
    question = ((&(tree->questions))->list.itemA+qIdx)->name;
    tag      = strrchr(question,'=');

    if ( tag ) {
	tIdx     = listName2Index ((List*)&(tags->list), tag+1);

	return tIdx;
    } else {
	ERROR("Can't find tag '%s' for question '%s' with index '%i'.\n",
	      tag, question, qIdx);
	return -1;
    }
}

void stateTableBuildDFS (StateTable *stP, Tree *tree, Tags *tags, int treeX,
			 int nodeX, int mask, int answers) {

    /*
      INFO("Call with treeX %i, nodeX %i, mask %i, answers %i\n",
      treeX, nodeX, mask, answers);
    */

    if (nodeX >= 0 && tree->list.itemA[nodeX].question >= 0) {
	int tIdx, yes, no, isLeafY, isLeafN;
	int answerY    =  answers;
	int answerN    =  answers;
	int modelY     = -1;
	int modelN     = -1;
	//int oldMask    =  mask;
	int oldAnswers =  answers;
	
	tIdx       =  stateTableGetTagX (tree, tags, nodeX);
	yes        =  tree->list.itemA[nodeX].yes;
	no         =  tree->list.itemA[nodeX].no;
	isLeafY    =  stateTableIsLeaf (tree, yes);
	isLeafN    =  stateTableIsLeaf (tree, no);

	if ( tIdx >= 0 ) {
	    mask    |= 1 << tIdx;
	    answerY |= 1 << tIdx;
	}

	if ( isLeafY ) modelY = tree->list.itemA[yes].model;
	if ( isLeafN ) modelN = tree->list.itemA[no].model;

	if ( isLeafY || isLeafN )
	    stateTableFill (stP, treeX, mask, answerY, answerN, modelY, modelN);

	if ( !isLeafY ) {
	    if ( tIdx >= 0 ) answers |= 1 << tIdx;

	    stateTableBuildDFS (stP, tree, tags, treeX, yes, mask, answers);

	    answers = oldAnswers;
	}

	if ( !isLeafN ) {
	    stateTableBuildDFS (stP, tree, tags, treeX, no, mask, answers);
	}

	//mask = oldMask;
    } else {
	ERROR("This can not be: treeX '%i', nodeX '%i', mask '%i', answers '%i'.\n",
	      treeX, nodeX, mask, answers);
    }
}


/* ------------------------------------------------------------------------
    stateTableGetEntry, stateTableSetEntry
   ------------------------------------------------------------------------ */
/* treeX is index of row (actually they are also ds indices of dummy ds) *
 * modalityX is index of column (it's a binary converted to an integer)  *
 * dsX is the mapped ds index in row treeX and column modalityX          */
   
int stateTableGetEntryItf (ClientData cd, int argc, char *argv[]) {

    StateTable *stP = (StateTable*)cd;
    int treeX       = -1;
    int modalityX   = -1;
    int dsX         = -1;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<treeX>", ARGV_INT, NULL, &treeX, NULL,
		       		"index of subtree",
		       "<modalityX>", ARGV_INT, NULL, &modalityX, NULL,
		       		"index of modality combination",
		       NULL) != TCL_OK )
	return TCL_ERROR;

    if ( stP->compress )
	modalityX = stP->compressA[modalityX];
    
    dsX = stP->matrix->matPA[treeX][modalityX];

    itfAppendResult ("%i", dsX);

    return TCL_OK;
}

int stateTableSetEntryItf (ClientData cd, int argc, char *argv[]) {

    StateTable *stP = (StateTable*)cd;
    int treeX       = -1;
    int modalityX   = -1;
    int dsX         = -1;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<treeX>", ARGV_INT, NULL, &treeX, NULL,
		       		"index of subtree",
		       "<modalityX>", ARGV_INT, NULL, &modalityX, NULL,
		       		"index of modality combination",
		       "<dsX>", ARGV_INT, NULL, &dsX, NULL,
		       		"index of distribution",
		       NULL) != TCL_OK )
	return TCL_ERROR;

    if ( stP->compress )
	modalityX = stP->compressA[modalityX];
    
    stP->matrix->matPA[treeX][modalityX] = dsX;

    return TCL_OK;
}

/* ------------------------------------------------------------------------
    stateTableLookup
   ------------------------------------------------------------------------ */

int stateTableLookup (StateTable *stP, int dsX, int frameX) {

    int modalityX  = -1;
    int treeX      = -1;

    /* update needed? */
    if ( frameX < stP->startFrameX )
	stateTableUpdate (stP, frameX, stP->startFrameX-1);
    if ( frameX > stP->endFrameX ) {
	stateTableUpdate (stP, stP->endFrameX+1, frameX);
    }

    /* find answers (modalityX) on base of frameX */
    modalityX = stP->modsP->answerA[frameX - stP->startFrameX];

    /* find treeX on base of dsX */
    treeX = dsX - stP->dummyStart;

    /* error handling */
    if ( treeX >= stP->m || modalityX >= stP->n ||
	 treeX <  0      || modalityX <  0 ) {
	ERROR("dsX %i not in [%i,%i] or modalityX %i not in [0,%i].\n",
	      dsX, stP->dummyStart, stP->dummyStart+stP->m-1,
	      modalityX, stP->n-1);

	return -1;
    }

    if ( stP->compress )
	modalityX = stP->compressA[modalityX];

    /*
      printf("Map %i to %i for frame %i\n", dsX, stP->matrix->matPA[treeX][modalityX], frameX); fflush(stdout);
      */

    return stP->matrix->matPA[treeX][modalityX];
}

int stateTableLookupItf (ClientData cd, int argc, char *argv[]) {

    StateTable *stP = (StateTable*)cd;
    int dsX    = -1;
    int frameX =  0;
    int res    = -1;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<dsX>", ARGV_INT, NULL, &dsX, NULL,
		       		"index of distribution",
		       "<frameX>", ARGV_INT, NULL, &frameX, NULL,
		       		"index of frame",
		       NULL) != TCL_OK )
	return TCL_ERROR;

    res = stateTableLookup (stP, dsX, frameX);

    if ( res < 0 )
	return TCL_ERROR;

    itfAppendResult("%i", res);

    return TCL_OK;
}

/* ------------------------------------------------------------------------
    stateTableUpdate
   ------------------------------------------------------------------------ */

int* stateTableUpdate (StateTable *stP, int startFrameX, int endFrameX) {

    int   *resultA = NULL;
    Timer  stopit;

    /* test intervall integrity */
    if ( startFrameX < 0 || endFrameX < 0 ) return NULL;
    if ( endFrameX < startFrameX )          return NULL;

    if ( stP->timeInfo ) {
	timerReset(&stopit);
	timerStart(&stopit);
    }

    /* first init */
    if ( stP->startFrameX == stateTableDefault.startFrameX &&
	 stP->endFrameX   == stateTableDefault.endFrameX ) {

	stP->startFrameX = startFrameX;
	stP->endFrameX   = endFrameX;

    } else {
	/* update intervall */
	if ( startFrameX < stP->startFrameX ) {
	    stP->startFrameX = startFrameX;
	}
	if ( endFrameX > stP->endFrameX ) {
	    stP->endFrameX   = endFrameX;
	}
    }

    /* INFO ("Do Update for Intervall [%i,%i]\n", startFrameX, endFrameX); */

    resultA = modalitySetUpdate (stP->modsP, startFrameX, endFrameX);

    /* test intervall integrity */
    if ( stP->startFrameX != stP->modsP->startFrameX ||
	 stP->endFrameX   != stP->modsP->endFrameX )
	WARN("Intervall [%i,%i] of %s differ from [%i,%i] of %s",
	     stP->startFrameX, stP->endFrameX, stP->name,
	     stP->modsP->startFrameX, stP->modsP->endFrameX, stP->modsP->name);

    if ( stP->timeInfo ) {
	INFO("End of Update (%f s)\n", timerStop(&stopit));
    }

    return resultA;
}

int stateTableUpdateItf (ClientData cd, int argc, char *argv[]) {

    StateTable *stP         = (StateTable*)cd;
    int         startFrameX = stateTableDefault.startFrameX;
    int         endFrameX   = stateTableDefault.endFrameX;
    int        *answerA     = NULL;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<startFrameX>", ARGV_INT, NULL, &startFrameX, NULL,
		       		"start frame for answer",
		       "<endFrameX>",   ARGV_INT, NULL, &endFrameX,   NULL,
		       		"end frame for answer",
		       NULL) != TCL_OK )
	return TCL_ERROR;

    answerA = stateTableUpdate (stP, startFrameX, endFrameX);

    if ( !answerA )
	ERROR("No update feasible for '%s' in [%i,%i].\n",
	      stP->name, startFrameX, endFrameX);

    return TCL_OK;
}

/* ------------------------------------------------------------------------
    stateTableCopy
   ------------------------------------------------------------------------ */

int stateTableCopyItf (ClientData cd, int argc, char *argv[]) {

    StateTable *stP = (StateTable*)cd;

    return imatrixCopyItf ((ClientData)stP->matrix, argc, argv);
}

/* ------------------------------------------------------------------------
    stateTableResize
   ------------------------------------------------------------------------ */

int stateTableResizeItf (ClientData cd, int argc, char *argv[]) {

    StateTable *stP = (StateTable*)cd;

    return imatrixResizeItf ((ClientData)stP->matrix, argc, argv);
}

/* ------------------------------------------------------------------------
    stateTableWrite, stateTableRead
   ------------------------------------------------------------------------ */

int stateTableWrite (StateTable *stP, char *fileName) {

    int   i, j;
    FILE *fp;
    ModelSetPtr  mdsP;
    DistribSet  *dssP;
    Distrib     *dsP;

    if ( !(fp = fileOpen (fileName, "w")) ) return TCL_ERROR;

    fprintf (fp, "%c -------------------------------------------------------\n",
	          stP->commentChar);
    fprintf (fp, "%c  Name                 : %s\n", stP->commentChar, stP->name);
    fprintf (fp, "%c  Type                 : StateTable\n", stP->commentChar);
    fprintf (fp, "%c  Number of Modalities : %i\n", stP->commentChar, stP->modXN);
    fprintf (fp, "%c  Rows, Columns        : %i, %i\n", stP->commentChar,
	          stP->m, stP->n);
    fprintf (fp, "%c  Compress (items)     : %i (%i)\n", stP->commentChar,
	          stP->compress, stP->compressN);
    fprintf (fp, "%c  Date                 : %s", stP->commentChar, dateString());
    fprintf (fp, "%c -------------------------------------------------------\n",
	          stP->commentChar);

    /* write modalities */
    fprintf (fp, "modalities:\n");
    for (i=0; i<stP->modsP->list.itemN; i++) {
	Modality *modP = &(stP->modsP->list.itemA[i]);
	fprintf (fp, "%s %s %i %i\n", modP->name, modP->tagName, modP->updateMode,
		 modP->updateLimit);
    }

    /* write compressA */
    fprintf (fp, "compressA:\n");
    for (i=0; i<stP->compressN; i++) {
	fprintf (fp, "%i %d ", i, stP->compressA[i]);
    }
    fprintf (fp, "\n");

    /* write stateTable */
    fprintf (fp, "stateTable:\n");
    for (i=0; i<stP->m; i++) {
	for (j=0; j<stP->n; j++) {
	    mdsP = stP->modsP->tree->mdsP;
	    dssP = (DistribSet*)mdsP.cd;
	    if ( stP->matrix->matPA[i][j] >= 0 ) {
		dsP = &(dssP->list.itemA[stP->matrix->matPA[i][j]]);
		fprintf (fp, "%s ", dsP->name);
	    } else {
		fprintf (fp, "- ");
	    }
	}
	fprintf (fp, "\n");
    }

    return TCL_OK;
}

int stateTableWriteItf (ClientData cd, int argc, char *argv[]) {

    StateTable *stP      = (StateTable*)cd;
    char       *fileName = NULL;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<fileName>", ARGV_STRING, NULL, &fileName, NULL,
		       		"Name of file",
		       NULL) != TCL_OK )
	return TCL_ERROR;

    return stateTableWrite (stP, fileName);
}

int stateTableRead (StateTable *stP, char *fileName) {

    return TCL_OK;
}

int stateTableReadItf (ClientData cd, int argc, char *argv[]) {

    StateTable *stP      = (StateTable*)cd;
    char       *fileName = NULL;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<fileName>", ARGV_STRING, NULL, &fileName, NULL,
		       		"Name of file",
		       NULL) != TCL_OK )
	return TCL_ERROR;

    return stateTableRead (stP, fileName);
}


/* ------------------------------------------------------------------------
   stateTableMatrixCreate
   ------------------------------------------------------------------------ */

int stateTableMatrixCreate (StateTable *stP) {

    ModalitySet *modsP = stP->modsP;
    int i;

    for (i = 0; i < stP->m; i++) {
	stateTableBuildDFS (stP, modsP->tree, modsP->localTags, i,
			    modsP->rootNodeA[i], 0, 0);
    }

    return TCL_OK;
}

int stateTableMatrixCreateItf (ClientData cd, int argc, char *argv[]) {

    StateTable *stP = (StateTable*)cd;
    
    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       NULL) != TCL_OK )
	return TCL_ERROR;

    return stateTableMatrixCreate (stP);
}

/* ------------------------------------------------------------------------
    stateTablePutsItf
   ------------------------------------------------------------------------ */

static int stateTablePutsItf (ClientData cd, int argc, char *argv[]) {

    StateTable *stP = (StateTable*)cd;

    return imatrixPutsItf ((ClientData)stP->matrix, argc, argv);
}


/* ------------------------------------------------------------------------
    stateTableResetItf
   ------------------------------------------------------------------------ */

int stateTableResetItf (ClientData cd, int argc, char *argv[]) {

    StateTable *stP = (StateTable*)cd;
    
    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       NULL) != TCL_OK )
	return TCL_ERROR;

    stP->startFrameX = stateTableDefault.startFrameX;
    stP->endFrameX   = stateTableDefault.endFrameX;

    modalitySetResetItf ((ClientData)stP->modsP, argc, argv);

    return TCL_OK;
}


/* ========================================================================
    Modality
   ======================================================================== */

static ClientData modalityCreateItf (ClientData cd, int argc, char *argv[]);
static int        modalityFreeItf   (ClientData cd);

static Modality modalityDefault;

char* modalityUpdModeToStr (int   mode);
int   modalityStrToUpdMode (char* mode);

/* ------------------------------------------------------------------------
    Init/Create Modality Object
   ------------------------------------------------------------------------ */

int modalityInit (Modality *modP, ClientData cd) {

    modP->name        = strdup((char*)cd);
    modP->useN        = 0;
    modP->tagName     = modalityDefault.tagName;
    modP->startFrameX = modalityDefault.startFrameX;
    modP->endFrameX   = modalityDefault.endFrameX;
    modP->answerA     = modalityDefault.answerA;
    modP->updateProc  = modalityDefault.updateProc;
    modP->yesN        = modalityDefault.yesN;
    modP->timeInfo    = modalityDefault.timeInfo;
    modP->updateMode  = modalityDefault.updateMode;
    modP->updateLimit = modalityDefault.updateLimit;

    return TCL_OK;
}


Modality* modalityCreate (char *name, char *updateProc, char *tagName,
			  int updateMode, int updateLimit) {

    Modality* modP = (Modality*)malloc(sizeof(Modality));

    if ( !modP || modalityInit (modP, (ClientData)name) != TCL_OK ) {
	if (modP) free (modP);
	ERROR("Cannot create Modality object.\n");
	return NULL;
    }

    modalityAlloc (modP, updateProc, tagName);

    modP->updateMode  = updateMode;
    modP->updateLimit = updateLimit;

    return modP;
}

static ClientData modalityCreateItf (ClientData cd, int argc, char *argv[]) {

    char *updateProc  = modalityDefault.updateProc;
    char *tagName     = modalityDefault.tagName;
    int   updateMode  = modalityDefault.updateMode;
    int   updateLimit = modalityDefault.updateLimit;
    char *mode        = modalityUpdModeToStr (updateMode);
    char *name        = NULL;

    if ( itfParseArgv (argv[0], &argc, argv, 1,
      "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the modality",
		       "<updateProc>", ARGV_STRING, NULL, &updateProc, NULL,
		       		"TCL modality update proc",
		       "<tagName>", ARGV_STRING, NULL, &tagName, NULL,
		       		"Name for tag",
		       "-mode", ARGV_STRING, NULL, &mode, NULL,
		       		"update mode (ALL, GIVEN, RUNON, NOT)",
		       "-limit", ARGV_INT, NULL, &updateLimit, NULL,
		       		"update only if intervall greater limit",
		       NULL) != TCL_OK )
	return NULL;

    if ( (updateMode = modalityStrToUpdMode (mode)) < 0 ) {
	ERROR("undefined mode %s.\n", mode);
	return NULL;
    }

    return (ClientData)modalityCreate(name, updateProc, tagName,
				      updateMode, updateLimit);
}

int modalityAlloc (Modality* modP, char *updateProc, char *tagName) {

    modP->useN        = 0;
    modP->startFrameX = modalityDefault.startFrameX;
    modP->endFrameX   = modalityDefault.endFrameX;
    modP->answerA     = modalityDefault.answerA;
    modP->updateProc  = (updateProc) ? strdup(updateProc):modalityDefault.updateProc;
    modP->tagName     = (tagName)    ? strdup(tagName)   :modalityDefault.tagName;

    return TCL_OK;
}

/* ------------------------------------------------------------------------
    Deinit/Free Modality Object
   ------------------------------------------------------------------------ */

int modalityLinkN (Modality *modP) {

    return modP->useN;
}

int modalityDeinit (Modality *modP) {

    if ( modalityLinkN (modP) ) {
	SERROR("Modality '%s' still in use by other objects.\n", modP->name);
	return TCL_ERROR;
    }

    if ( modP->name )       { free(modP->name);       modP->name       = NULL; }
    if ( modP->updateProc ) { free(modP->updateProc); modP->updateProc = NULL; }
    if ( modP->tagName )    { free(modP->tagName); } 
    if ( modP->answerA )    { free(modP->answerA);    modP->answerA    = NULL; }

    return TCL_OK;
}

int modalityFree (Modality *modP) {

    if ( modalityDeinit (modP) != TCL_OK ) return TCL_ERROR;
    if ( modP ) free (modP);

    return TCL_OK;
}

static int modalityFreeItf (ClientData cd) {

    return modalityFree ((Modality*)cd);
}


int modalityConfigureItf (ClientData cd, char *var, char *val) {

    Modality *modP = (Modality*)cd;
    int       mode = -1;

    if ( !modP ) modP = &modalityDefault;

    if ( var == NULL ) {
	ITFCFG(modalityConfigureItf, cd, "name");
	ITFCFG(modalityConfigureItf, cd, "useN");
	ITFCFG(modalityConfigureItf, cd, "updateProc");
	ITFCFG(modalityConfigureItf, cd, "tagName");
	ITFCFG(modalityConfigureItf, cd, "startFrameX");
	ITFCFG(modalityConfigureItf, cd, "endFrameX");
	ITFCFG(modalityConfigureItf, cd, "yesN");
	ITFCFG(modalityConfigureItf, cd, "timeInfo");
	ITFCFG(modalityConfigureItf, cd, "updateMode");
	ITFCFG(modalityConfigureItf, cd, "updateLimit");
	return TCL_OK;
    }

    ITFCFG_CharPtr (var, val, "name",        modP->name,        1);
    ITFCFG_Int     (var, val, "useN",        modP->useN,        1);
    ITFCFG_CharPtr (var, val, "updateProc",  modP->updateProc,  0);
    ITFCFG_CharPtr (var, val, "tagName",     modP->tagName,     0);
    ITFCFG_Int     (var, val, "startFrameX", modP->startFrameX, 1);
    ITFCFG_Int     (var, val, "endFrameX",   modP->endFrameX,   1);
    ITFCFG_Int     (var, val, "yesN",        modP->yesN,        1);
    ITFCFG_Int     (var, val, "timeInfo",    modP->timeInfo,    0);
    ITFCFG_Int     (var, val, "updateLimit", modP->updateLimit, 0);

    if ( !strcmp(var,"updateMode") ) {
	if ( !val ) {
	    itfAppendElement("%s", modalityUpdModeToStr (modP->updateMode));
	    return TCL_OK;
	}
	if ( (mode = modalityStrToUpdMode (val)) < 0 ) {
	    ERROR("unknown update mode %s.\n", val);
	    return TCL_ERROR;
	}
	modP->updateMode = mode;
	return TCL_OK;
    }

    ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");

    return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    modalityUpdModeToStr, modalityStrToUpdMode
   ------------------------------------------------------------------------ */

char* modalityUpdModeToStr (int mode) {
    
    switch (mode) {
    case UPD_ALL:   return "ALL";
    case UPD_GIVEN: return "GIVEN";
    case UPD_RUNON: return "RUNON";
    case UPD_NOT:   return "NOT";
    }

    return NULL;
}

int modalityStrToUpdMode (char *mode) {

    if ( !strcmp (mode, "ALL")   ) return UPD_ALL;
    if ( !strcmp (mode, "GIVEN") ) return UPD_GIVEN;
    if ( !strcmp (mode, "RUNON") ) return UPD_RUNON;
    if ( !strcmp (mode, "NOT")   ) return UPD_NOT;

    return -1;
}


/* ------------------------------------------------------------------------
    modalityUpdate
   ------------------------------------------------------------------------ */


int* modalityUpdate (Modality *modP, int startFrameX, int endFrameX, int *from,
		     int *to) {

    DString  dstr;
    char    *result  = NULL;
    int      i, tokc, offset;
    char   **tokv;
    Timer    stopit;
    float    time    =  0;
    int      from1   = -1;  /* malloc array, copy values          */
			    /* (modP->startFrameX > startFrameX)  */
    int      from2   = -1;  /* realloc array, don't copy values   */
    			    /* (modP->startFrameX <= startFrameX) */
    int      to1=0, to2=0;
    int      frameN  = 0;
    char     str[35];
    int      mode    = modP->updateMode;

    if ( startFrameX < 0 || endFrameX < 0 ) return NULL;
    if ( endFrameX < startFrameX )          return NULL;

    if ( modP->timeInfo ) {
	timerReset(&stopit);
	timerStart(&stopit);
    }

    /* select update intervalls
       set [modP->startFrameX, modP->endFrameX] to maximum width
       from1, to1 werden gesetzt, falls Werte in array umkopiert werden muessen */
    if ( modP->startFrameX == modalityDefault.startFrameX &&
	 modP->endFrameX   == modalityDefault.endFrameX ) {

	*from = from2 = modP->startFrameX = startFrameX;
	*to   = to2   = modP->endFrameX   = endFrameX;
	mode  = UPD_ALL;

	if ( mode == UPD_NOT ) to2 = modP->startFrameX;

    } else {
	/* gaps in array are not allowed */
	if ( endFrameX   < modP->startFrameX-1 ) endFrameX   = modP->startFrameX-1;
	if ( startFrameX > modP->endFrameX+1   ) startFrameX = modP->endFrameX+1;

	/* for UPD_ALL and for UPD_GIVEN reset yesN */
	switch ( mode ) {
	case UPD_ALL:
	    if ( modP->startFrameX > startFrameX ) modP->startFrameX = startFrameX;
	    if ( modP->endFrameX   < endFrameX   ) modP->endFrameX   = endFrameX;
	    *from = from2 = modP->startFrameX;
	    *to   = to2   = modP->endFrameX;
	    modP->yesN    = 0;
	    break;
	case UPD_GIVEN:
	    if ( modP->startFrameX > startFrameX ) {
		if ( modP->endFrameX < endFrameX ) {
		    for ( i = 0; i < modP->endFrameX-modP->startFrameX+1; i++ )
			if ( modP->answerA[i] ) modP->yesN--;
		    modP->endFrameX = endFrameX;
		    *from = from2   = startFrameX;
		    *to   = to2     = endFrameX;
		} else {
		    for ( i = 0; i < endFrameX-modP->startFrameX+1; i++ )
			if ( modP->answerA[i] ) modP->yesN--;
		    *from = from1 = startFrameX;
		    *to   = to1   = endFrameX;
		}
		modP->startFrameX = startFrameX;
	    } else {
		if ( modP->endFrameX < endFrameX ) {
		    if ( startFrameX <= modP->endFrameX ) {
			for ( i = startFrameX - modP->startFrameX;
			      i <  modP->endFrameX - modP->startFrameX+1; i++ )
			    if ( modP->answerA[i] ) modP->yesN--;
		    }
		    modP->endFrameX = endFrameX;
		} else {
		    for ( i = startFrameX - modP->startFrameX;
			  i < endFrameX   - modP->startFrameX+1; i++ )
			if ( modP->answerA[i] ) modP->yesN--;
		}
		*from = from2   = startFrameX;
		*to   = to2     = endFrameX;		    
	    }
	    break;
	case UPD_RUNON: case UPD_NOT:
	    if ( modP->startFrameX > startFrameX ) {
		*from = from1     = startFrameX;
		*to   = to1       = modP->startFrameX-1;
		modP->startFrameX = startFrameX;
	    }
	    if ( modP->endFrameX < endFrameX ) {
		from2             = modP->endFrameX+1;
		*to   = to2       = endFrameX;
		modP->endFrameX   = endFrameX;
		if ( from1 < 0 ) *from = from2;
	    }
	    break;
	}
    }


    /* update */
    if ( from2 >= 0 || from1 >= 0 ) {

	/* allocate array */
	if ( from1 < 0 ) {
	    modP->answerA = (int*)realloc(modP->answerA,
					  (modP->endFrameX - modP->startFrameX+1) *
					  sizeof(int));
	    /* update array for UPD_NOT */
	    if ( mode == UPD_NOT ) {
		offset = from2 - modP->startFrameX;
		for ( i = 0; i < to2-from2+1; i++ )
		    modP->answerA[i+offset] = modP->answerA[0];
	    }
	} else {
	    /* malloc is needed => copy array values */
	    int *answerA = (int*)malloc((modP->endFrameX - modP->startFrameX+1) *
				       sizeof(int));
	    int  oldBegin = to1+1;
	    int  oldEnd   = (from2 < 0) ? (modP->endFrameX) : (from2-1);

	    for (i = 0; i < oldEnd-oldBegin+1; i++) {
		answerA[to1-from1+1+i] = modP->answerA[i];
	    }

	    free (modP->answerA);
	    modP->answerA = answerA;

	    /* update array for UPD_NOT */
	    if ( mode == UPD_NOT ) {
		for ( i = 0; i < to1-from1+1; i++ )
		    modP->answerA[i] = modP->answerA[to1-from1+1];

		if ( from2 >= 0 ) {
		    offset = from2 - modP->startFrameX;
		    for ( i = 0; i < to2-from2+1; i++ )
			modP->answerA[i+offset] = modP->answerA[0];
		}
	    }
	}

	/* eval updateProc */
	if ( from1 >= 0 && mode != UPD_NOT ) {
	    dstringInit   (&dstr);
	    dstringAppend (&dstr, "%s %s %i %i",
			   modP->updateProc, modP->name, from1, to1);

	    if ( Tcl_Eval (itf, dstringValue (&dstr)) != TCL_OK ) {
		WARN ("Tcl Error calling Modality updateProc '%s'.\n",
		      dstringValue(&dstr));
	    } else {
	      // if (!itf->result) 
	      if ( !strlen (Tcl_GetStringResult (itf)) ) {
		    ERROR ("No result calling Modality updateProc '%s'.\n",
			   dstringValue(&dstr));
		    return NULL;
		}

	        // result = strdup(itf->result);
	        result = strdup (Tcl_GetStringResult (itf));
		itfResetResult ();

		if ( Tcl_SplitList (itf,result,&tokc,&tokv) != TCL_OK )
		    return NULL;

		free(result); result = NULL;

		if ( tokc != to1-from1+1 ) {
		    ERROR("Get only %i of %i values from Modality updateProc '%s'.\n",
			  tokc, to1-from1+1, dstringValue(&dstr));
		    return NULL;
		}

		/* parse Tcl_SplitList */
		offset = from1 - modP->startFrameX; /* always 0 */
		for (i = 0; i < tokc; i++) {

		    if ( !strcmp(tokv[i],"1") ) {
			modP->answerA[i+offset] =  1;
			modP->yesN++;
		    }
		    else if ( !strcmp(tokv[i],"0") )
			modP->answerA[i+offset] =  0;
		    else {
			ERROR("Answer '%s' invalid for modality '%s'.\n",
			      tokv[i], modP->name);
			modP->answerA[i+offset] =  0;
		    }
		}
		Tcl_Free((char*)tokv);
		itfResetResult ();
	    }

	    dstringFree (&dstr);

	    if ( modP->timeInfo ) {
		frameN += to1-from1+1;
		sprintf(str, "[%i,%i]", from1, to1);
	    }
	}

	if ( from2 >= 0 && mode != UPD_NOT ) {
	    dstringInit   (&dstr);
	    dstringAppend (&dstr, "%s %s %i %i",
			   modP->updateProc, modP->name, from2, to2);

	    if ( Tcl_Eval (itf, dstringValue (&dstr)) != TCL_OK ) {
		WARN ("Tcl Error calling Modality updateProc '%s'.\n",
		      dstringValue(&dstr));
	    } else {
	        // if ( !itf->result ) {
	        if ( !strlen (Tcl_GetStringResult (itf))) {
		    ERROR ("No result calling Modality updateProc '%s'.\n",
			   dstringValue(&dstr));
		    return NULL;
		}
		
		// result = strdup(itf->result);
		result = strdup (Tcl_GetStringResult (itf));
		itfResetResult ();

		if ( Tcl_SplitList (itf,result,&tokc,&tokv) != TCL_OK )
		    return NULL;

		free(result); result = NULL;

		/*
		if ( (mode != UPD_NOT && tokc != to2-from2+1) ||
		     (mode == UPD_NOT && tokc != 1) ) {
		    ERROR("Get only %i of %i values from Modality updateProc '%s'.\n",
			  tokc, to2-from2+1, dstringValue(&dstr));
		    return NULL;
		}
		*/

		/* parse Tcl_SplitList */
		offset = from2 - modP->startFrameX;
		for (i = 0; i < tokc; i++) {
		
		    if ( !strcmp(tokv[i],"1") ) {
			modP->answerA[i+offset] =  1;
			modP->yesN++;
		    }
		    else if ( !strcmp(tokv[i],"0") )
			modP->answerA[i+offset] =  0;
		    else {
			ERROR("Answer '%s' invalid for modality '%s'.\n",
			      tokv[i], modP->name);
			modP->answerA[i+offset] =  0;
		    }
		}
		Tcl_Free((char*)tokv);
		itfResetResult ();

		/* if this update was the first update for UPD_NOT */
		if ( modP->updateMode == UPD_NOT ) {
		    offset = from2 - modP->startFrameX;
		    to2    = modP->endFrameX;
		    for ( i = 0; i < to2-from2+1; i++ )
			modP->answerA[i+offset] = modP->answerA[0];
		}
	    }

	    dstringFree (&dstr);

	    if ( modP->timeInfo ) {
		frameN += to2-from2+1;
		if ( from1 >= 0 )
		    sprintf(str, "%s and [%i,%i]", str, from2, to2);
		else
		    sprintf(str, "[%i,%i]", from2, to2);
	    }
	}

	if ( modP->timeInfo ) {
	    time = timerStop(&stopit) * 1000;
	    INFO("Update '%s' for %s (%f ms => %f ms/frame)\n",
		 modP->name, str, time, time/frameN);
	}

    } else {
	INFO("'%s' is up to date\n", modP->name);
	*from = *to = -1;
    }

    /* return whole array */
    return modP->answerA;
}

int modalityUpdateItf (ClientData cd, int argc, char *argv[]) {

    Modality *modP        = (Modality*)cd;
    int       startFrameX =  modalityDefault.startFrameX;
    int       endFrameX   =  modalityDefault.endFrameX;
    int      *answerA; //     =  modalityDefault.answerA;
    int from, to;

    if ( !modP->updateProc ) {
	ERROR("No update function set for modality '%s'.\n",modP->name);
	return TCL_ERROR;
    }

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<startFrameX>", ARGV_INT, NULL, &startFrameX, NULL,
		       		"start frame for update",
		       "<endFrameX>",   ARGV_INT, NULL, &endFrameX,   NULL,
		       		"end frame for update",
		       NULL) != TCL_OK )
	return TCL_ERROR;

    answerA = modalityUpdate (modP, startFrameX, endFrameX, &from, &to);

    if ( !answerA )
	ERROR("No update feasible for '%s' in [%i,%i].\n",
	      modP->name, startFrameX, endFrameX);

    itfAppendResult("%i %i", from, to);

    return TCL_OK;
}

/* ------------------------------------------------------------------------
    modalityAnswer
   ------------------------------------------------------------------------ */

int* modalityAnswer (Modality *modP, int startFrameX, int endFrameX) {

    int *answerA; // = modalityDefault.answerA;
    int  i;

    if ( modP->startFrameX == modalityDefault.startFrameX && 
	 modP->endFrameX   == modalityDefault.endFrameX ) {

	ERROR("First do update for '%s'.\n",modP->name);
	return NULL;
    }

    if ( startFrameX < 0 || modP->startFrameX > startFrameX ||
	                    modP->endFrameX   < endFrameX ) {
	ERROR("No answer available for '%s' in [%i,%i].\n",
	      modP->name, startFrameX, endFrameX);
	return NULL;
    }

    answerA = (int*)malloc((endFrameX-startFrameX+1) * sizeof(int));

    for (i = 0; i <= endFrameX-startFrameX; i++) {
	answerA[i] = modP->answerA[startFrameX - modP->startFrameX + i];
    }

    return answerA;
}

int modalityAnswerItf (ClientData cd, int argc, char *argv[]) {

    Modality *modP        = (Modality*)cd;
    int       startFrameX =  modalityDefault.startFrameX;
    int       endFrameX   =  modalityDefault.endFrameX;
    int      *answerA; //     =  modalityDefault.answerA;
    int       i;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<startFrameX>", ARGV_INT, NULL, &startFrameX, NULL,
		       		"start frame for answer",
		       "<endFrameX>",   ARGV_INT, NULL, &endFrameX,   NULL,
		       		"end frame for answer",
		       NULL) != TCL_OK )
	return TCL_ERROR;

    answerA = modalityAnswer (modP, startFrameX, endFrameX);

    if ( !answerA )
	return TCL_ERROR;

    /* output of answerA */
    itfAppendResult ("%i", answerA[0]);
    for (i = 1; i <= endFrameX-startFrameX; i++) {
	itfAppendResult (" %i", answerA[i]);
    }

    free (answerA); answerA = NULL;

    return TCL_OK;
}

/* ------------------------------------------------------------------------
    modalityMajorityItf
   ------------------------------------------------------------------------ */

int modalityMajority (Modality *modP) {

    if ( modP->startFrameX == modalityDefault.startFrameX &&
	 modP->endFrameX   == modalityDefault.endFrameX )
	return -1;

    return (modP->yesN > (modP->endFrameX-modP->startFrameX+1)/2) ? 1 : 0;
}

int modalityMajorityItf (ClientData cd, int argc, char *argv[]) {

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       NULL) != TCL_OK )
	return TCL_ERROR;

    itfAppendResult("%i", modalityMajority ((Modality*)cd));

    return TCL_OK;
}

int modalityMajorityAnswer (Modality *modP, int startFrameX, int endFrameX) {

    int *answerA = NULL;
    int  yesN    = 0;
    int  i;

    answerA = modalityAnswer (modP, startFrameX, endFrameX);

    if ( !answerA ) return -1;

    for (i = 0; i <= endFrameX-startFrameX; i++)
	if ( answerA[i] ) yesN++;

    free (answerA); answerA = NULL;

    return (yesN > (endFrameX-startFrameX+1)/2) ? 1 : 0;
}

int modalityMajorityAnswerItf (ClientData cd, int argc, char *argv[]) {

    Modality    *modP        = (Modality*)cd;
    int          startFrameX =  modP->startFrameX;
    int          endFrameX   =  modP->endFrameX;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "-startFrameX", ARGV_INT, NULL, &startFrameX, NULL,
		       		"start frame for answer",
		       "-endFrameX",   ARGV_INT, NULL, &endFrameX,   NULL,
		       		"end frame for answer",
		       NULL) != TCL_OK )
	return TCL_ERROR;

    itfAppendResult("%i", modalityMajorityAnswer ((Modality*)cd, startFrameX,
						  endFrameX));

    return TCL_OK;
}


/* ------------------------------------------------------------------------
    modalityPutsItf
   ------------------------------------------------------------------------ */

int modalityPutsItf (ClientData cd, int argc, char *argv[]) {

    Modality *modP = (Modality*)cd;
    int       i;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       NULL) != TCL_OK )
	return TCL_ERROR;

    itfAppendResult ("{name %s} {answer { ", modP->name);
    if ( modP->startFrameX != modalityDefault.startFrameX &&
	 modP->endFrameX   != modalityDefault.endFrameX ) {
	for (i = 0; i <= modP->endFrameX-modP->startFrameX; i++) {
	    itfAppendResult ("%i ", modP->answerA[i]);
	}
    }
    itfAppendResult ("}} {startFrameX %i} {endFrameX %i} {tagName %s}",
		     modP->startFrameX, modP->endFrameX, modP->tagName);
    return TCL_OK;
}

/* ------------------------------------------------------------------------
    modalityReset
   ------------------------------------------------------------------------ */

int modalityReset (Modality *modP) {

    modP->startFrameX = modalityDefault.startFrameX;
    modP->endFrameX   = modalityDefault.endFrameX;
    modP->yesN        = modalityDefault.yesN;

    if ( modP->answerA ) {
	free (modP->answerA);
	modP->answerA = modalityDefault.answerA;
    }

    return TCL_OK;
}

int modalityResetItf (ClientData cd, int argc, char *argv[]) {

    Modality *modP = (Modality*)cd;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       NULL) != TCL_OK )
	return TCL_ERROR;

    return modalityReset (modP);
}


/* ========================================================================
    ModalitySet
   ======================================================================== */

static ClientData modalitySetCreateItf (ClientData cd, int argc, char *argv[]);
static int        modalitySetFreeItf   (ClientData cd);
static int        modalitySetPutsItf   (ClientData cd, int argc, char *argv[]);

static ModalitySet modalitySetDefault;

/* ------------------------------------------------------------------------
    Init/Create ModalitySet Object
   ------------------------------------------------------------------------ */

int modalitySetInit (ModalitySet *modsP, char *name, Tags *tags, int addTags) {

    int i, tX = 0;

    modsP->name = strdup(name);
    modsP->useN = 0;

    listInit((List*)&(modsP->list), &modalityInfo, sizeof(Modality),
	     modalitySetDefault.list.blkSize);

    modsP->list.init    = (ListItemInit  *)modalityInit;
    modsP->list.deinit  = (ListItemDeinit*)modalityDeinit;

    modsP->tags         = tags; link(modsP->tags, "Tags");
    modsP->addTags      = addTags;
    modsP->localTags    = modalitySetDefault.localTags;

    modsP->startFrameX  = modalitySetDefault.startFrameX;
    modsP->endFrameX    = modalitySetDefault.endFrameX;
    modsP->answerA      = modalitySetDefault.answerA;

    modsP->tree         = modalitySetDefault.tree;
    modsP->dummyStart   = modalitySetDefault.dummyStart;
    modsP->rootNodeA    = modalitySetDefault.rootNodeA;
    modsP->rootNodeN    = modalitySetDefault.rootNodeN;

    /* set modMask in Tags object to demask modality tags */

    for ( i = 0; i < tags->list.itemN; i++ )
	tX |= (1 << i);

    tags->modMask = tX;

    return TCL_OK;
}

int modalitySetLinkTree (ModalitySet *modsP, Tree *tree) {

    ModelSetPtr  msP;
    DistribSet  *dssP;    
    int i, j, idx, n;

    if ( !tree ) return TCL_ERROR;
    if ( modsP->tree ) unlink (modsP->tree, "Tree");
    modsP->tree = tree;  link (modsP->tree, "Tree");

    /* extract root nodes */
    j    = 0;
    n    = 100;
    msP  = tree->mdsP;
    dssP = (DistribSet*)msP.cd;

    modsP->dummyStart = dssP->dummyStart;
    modsP->rootNodeA  = (int*)malloc (n * sizeof(int));

    for (i = 0; i < tree->list.itemN; i++) {
	TreeNode *node = &(tree->list.itemA[i]);
	char* nodeName = node->name;

	if ( !strncmp (nodeName, "ROOT", 4) ) {
	    idx = dssIndex (dssP, nodeName+5) - dssP->dummyStart; /* ROOT-dsName */

	    while ( idx >= n ) {
		n += n;
		modsP->rootNodeA = (int*)realloc (modsP->rootNodeA, n * sizeof(int));
	    }

	    modsP->rootNodeA[idx] = i;
	    j++;
	}
    }

    modsP->rootNodeN = j;
    modsP->rootNodeA = (int*)realloc (modsP->rootNodeA, j * sizeof(int));

    return TCL_OK;
}


ModalitySet* modalitySetCreate (char *name, Tags *tags, int addTags) {

    ModalitySet *modsP = (ModalitySet*)malloc(sizeof(ModalitySet));

    if ( !modsP ||
	 modalitySetInit (modsP, name, tags, addTags) != TCL_OK ) {
	if ( modsP ) free (modsP);
	ERROR("Failed to allocate modality set '%s'.\n", name);
	return NULL;
    }

    modsP->localTags = tagsCreate ("localTags");
    if ( !modsP->localTags ) {
	if ( modsP ) free (modsP);
	ERROR("Failed to allocate Tags object 'localTags' in '%s'.\n", name);
	return NULL;
    }
    link(modsP->localTags, "Tags");

    return modsP;
}

static ClientData modalitySetCreateItf (ClientData cd, int argc, char *argv[])
{
    Tags *tags    = modalitySetDefault.tags;
    int   addTags = modalitySetDefault.addTags;
    char* name = NULL;

    if ( itfParseArgv (argv[0], &argc, argv, 0,
      "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the modality set",
		       "<tags>", ARGV_OBJECT, NULL, &tags, "Tags",
		       		"tags object",
		       "-addTags", ARGV_INT, NULL, &addTags, NULL,
				"add tag names to tags-object",
		       NULL) != TCL_OK )
	return NULL;

    return (ClientData)modalitySetCreate(name, tags, addTags);
}

/* ------------------------------------------------------------------------
    Deinit/Free ModalitySet Object
   ------------------------------------------------------------------------ */

int modalitySetLinkN (ModalitySet *modsP) {

    return modsP->useN;
}

int modalitySetDeinit (ModalitySet *modsP) {

    int i;
    char* argv[1];

    if ( modalitySetLinkN (modsP) ) {
	SERROR("ModalitySet '%s' still in use by other objects.\n", modsP->name);
	return TCL_ERROR;
    }

    if ( modsP->name )      { free (modsP->name);      modsP->name      = NULL; }
    if ( modsP->tree )      { unlink (modsP->tree, "Tree"); }
    if ( modsP->rootNodeA ) { free (modsP->rootNodeA); modsP->rootNodeA = 0; }
    if ( modsP->answerA )   { free (modsP->answerA);   modsP->answerA   = NULL; }

    if ( modsP->addTags ) {
	for (i=0; i < modsP->list.itemN; i++) {
	    Modality *modP = &(modsP->list.itemA[i]);
	    argv[0] = modP->tagName;
	    tagsDelete (modsP->tags, 1, argv);
	}
    }
    if ( modsP->tags )      { unlink (modsP->tags, "Tags"); }

    return listDeinit ((List*)&(modsP->list));
}

int modalitySetFree (ModalitySet *modsP) {

    if ( modalitySetDeinit (modsP)   != TCL_OK ) return TCL_ERROR;
    if ( tagsFree (modsP->localTags) != TCL_OK ) return TCL_ERROR;
    if ( modsP ) free (modsP);

    return TCL_OK;
}

static int modalitySetFreeItf (ClientData cd) {

    return modalitySetFree ((ModalitySet*)cd);
}

/* ------------------------------------------------------------------------
    Configure/Access ModalitySet Object
   ------------------------------------------------------------------------ */

int modalitySetConfigureItf (ClientData cd, char *var, char *val) {

    ModalitySet *modsP = (ModalitySet*)cd;

    if ( !modsP ) modsP = &modalitySetDefault;

    if ( var == NULL ) {
	ITFCFG(modalitySetConfigureItf, cd, "name");
	ITFCFG(modalitySetConfigureItf, cd, "itemN");
	ITFCFG(modalitySetConfigureItf, cd, "tags");
	ITFCFG(modalitySetConfigureItf, cd, "tree");
	ITFCFG(modalitySetConfigureItf, cd, "addTags");
	ITFCFG(modalitySetConfigureItf, cd, "dummyStart");
	ITFCFG(modalitySetConfigureItf, cd, "startFrameX");
	ITFCFG(modalitySetConfigureItf, cd, "endFrameX");
	return TCL_OK;
    }

    ITFCFG_CharPtr (var, val, "name",        modsP->name,        1);
    ITFCFG_Int     (var, val, "itemN",       modsP->list.itemN,  1);
    ITFCFG_Object  (var, val, "tags",        modsP->tags, name, Tags, 1);
    ITFCFG_Int     (var, val, "addTags",     modsP->addTags,     1);
    ITFCFG_Int     (var, val, "dummyStart",  modsP->dummyStart,  1);
    ITFCFG_Int     (var, val, "startFrameX", modsP->startFrameX, 1);
    ITFCFG_Int     (var, val, "endFrameX",   modsP->endFrameX,   1);

    if ( !strcmp (var, "tree") ) {
	if ( !val ) {
	    itfAppendElement("%s", (modsP->tree && modsP->tree->name) ? modsP->tree->name : "(null)");
	    return TCL_OK;
	}
	if ( modalitySetLinkTree (modsP, (Tree*)itfGetObject(val, "Tree")) != TCL_OK ) {
	    ERROR ("Can't link tree %s.\n", val);
	    return TCL_ERROR;
	}
	return TCL_OK;
    }

    ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");

    return TCL_ERROR;
}

ClientData modalitySetAccessItf (ClientData cd, char *name, TypeInfo **ti) {

    ModalitySet *modsP = (ModalitySet*)cd;
    int modX;

    if (*name == '.') {
	if (name[1] == '\0') {
	    if ( modsP->list.itemN )
		itfAppendElement("modality(0..%d)", modsP->list.itemN-1);
	    itfAppendElement("tags");
	    itfAppendElement("localTags");
	    itfAppendElement("tree");
	}
	else {
	    if ( !strcmp(name+1,"tags") ) {
		*ti = itfGetType("Tags");
		return (ClientData)modsP->tags;
	    }
	    else if ( !strcmp(name+1,"localTags") ) {
		*ti = itfGetType("Tags");
		return (ClientData)modsP->localTags;
	    }
	    else if ( !strcmp(name+1, "tree") ) {
		*ti = itfGetType("Tree");
		return (ClientData) modsP->tree;
	    }
	    else if ( sscanf(name+1,"modality(%d)", &modX) == 1 ) {
		*ti =itfGetType("Modality");
		if ( modX >= 0 && modX < modsP->list.itemN )
		    return (ClientData)&(modsP->list.itemA[modX]);
		else return NULL;
	    }
	}
    }

    return (ClientData)listAccessItf((ClientData)&(modsP->list),name,ti);
}

/* ------------------------------------------------------------------------
    Add/Delete a Modality Object to set
   ------------------------------------------------------------------------ */

int modalitySetAdd (ModalitySet *modsP, char *name, char *updateProc,
		    char *tagName) {

    int   idx = listIndex((List*)&(modsP->list), (ClientData)name, 0);
    char* argv[1];

    if (idx >= 0) {
	ERROR("Modality '%s' already exists in '%s'.\n", name, modsP->name);
	return idx;
    }

    if ( modsP->list.itemN == MAX_MODALITIES ) {
	ERROR("Adding of new Modality %s would exceed maximum allowed number "
	      "of Modalities (%i).\n", name, MAX_MODALITIES);
	return -1;
    }

    idx = listNewItem((List*)&(modsP->list), (ClientData)name);

    modalityAlloc (modsP->list.itemA+idx, updateProc, tagName);
    modsP->answerA = (int*)realloc(modsP->answerA,(modsP->list.itemN)*sizeof(int*));

    /* add tags */
    if ( modsP->addTags ) {
	argv[0] = tagName;
	tagsAdd (modsP->tags, 1, argv);
    }
    argv[0] = tagName;
    tagsAdd (modsP->localTags, 1, argv);

    return idx;
}

int modalitySetAddItf (ClientData cd, int argc, char *argv[]) {

    ModalitySet *modsP      = (ModalitySet*)cd;
    char	*name       = modalityDefault.name;
    char	*updateProc = modalityDefault.updateProc;
    char        *tagName    = modalityDefault.tagName;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<name>",       ARGV_STRING, NULL, &name,       NULL,
		       		"name of modality",
		       "<updateProc>", ARGV_STRING, NULL, &updateProc, NULL,
		       		"TCL modality update proc",
		       "<tagName>", ARGV_STRING, NULL, &tagName, NULL,
		       		"Name for tag",
		       NULL) != TCL_OK )
	return TCL_ERROR;

    if ( modalitySetAdd (modsP, name, updateProc, tagName) < 0 )
	return TCL_ERROR;

    return TCL_OK;
}

int modalitySetDeleteItf (ClientData cd, int argc, char *argv[]) {

    ModalitySet *modsP = (ModalitySet*)cd;
    
    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       NULL) != TCL_OK )
	return TCL_ERROR;

    return listDeleteItf( (ClientData)&(modsP->list), argc, argv);
}

/* ------------------------------------------------------------------------
    modalitySetUpdate
   ------------------------------------------------------------------------ */

int* modalitySetUpdate (ModalitySet *modsP, int startFrameX, int endFrameX) {

    Modality *modP   = NULL;
    int       i, j, k;
    int      *resultA;
    int       from, to, from1, to1, test;

    /* select update intervall */
    if ( startFrameX < 0 || endFrameX < 0 ) return NULL;
    if ( endFrameX < startFrameX )          return NULL;

    /* first init */
    if ( modsP->startFrameX == modalitySetDefault.startFrameX &&
	 modsP->endFrameX   == modalitySetDefault.endFrameX ) {

	modsP->startFrameX = startFrameX;
	modsP->endFrameX   = endFrameX;
    } else {
	/* update intervall */
	if ( startFrameX < modsP->startFrameX ) {
	    modsP->startFrameX = startFrameX;
	}
	if ( endFrameX > modsP->endFrameX ) {
	    modsP->endFrameX   = endFrameX;
	}
    }

    modsP->answerA = (int*)realloc(modsP->answerA,
				   (modsP->endFrameX - modsP->startFrameX + 1) *
				   sizeof(int));
    /* init */
    for ( k = 0; k < modsP->endFrameX - modsP->startFrameX + 1; k++)
	modsP->answerA[k] = 0;
    
    /* update */
    for ( i = 0; i < modsP->list.itemN; i++ ) {
	modP = &(modsP->list.itemA[i]);
	
	resultA = modalityUpdate (modP, startFrameX, endFrameX, &from, &to);

	/* test intervall integrity */
	if ( modsP->startFrameX != modP->startFrameX ||
	     modsP->endFrameX   != modP->endFrameX )
	    WARN("Intervall [%i,%i] of %s differ from [%i,%i] of %s",
		 modsP->startFrameX, modsP->endFrameX, modsP->name,
		 modP->startFrameX, modP->endFrameX, modP->name);

	/* carry over resultA
	   resultA in intervall [modP->startFrameX, modP->endFrameX]
	   update  in intervall [from, to]
	   answerA in intervall [modsP->startFrameX, modsP->endFrameX]
	   update only neccesary intervall */
	if ( resultA ) {
	    /* set intervall for update */
	    from1 = from - modP->startFrameX;
	    to1   = to   - modP->startFrameX;

	    if ( from < modsP->startFrameX )
		from1 += modsP->startFrameX - from;
	    if ( to   > modsP->endFrameX )
		to1   -= to - modsP->endFrameX;

	    /* set offset */
	    k    = from - modsP->startFrameX;
	    if ( k < 0 ) k = 0;
	    test = (1 << i);

	    /* update */
	    for ( j = from1; j <= to1; j++ ) {
		/* look if bit in old array set
		   if bit set, delete bit, otherwise let it be */
		if ( (modsP->answerA[k] & test) == test ) modsP->answerA[k] -= test;

		modsP->answerA[k] |= (resultA[j] << i);
		k++;
	    }
	} else {
	    ERROR("Can't update modality '%s' in intervall [%i,%i].\n",
		  modP->name, modsP->startFrameX, modsP->endFrameX);
	}
    }

    return modsP->answerA;
}

int modalitySetUpdateItf (ClientData cd, int argc, char *argv[]) {

    ModalitySet *modsP       = (ModalitySet*)cd;
    int          startFrameX =  modalitySetDefault.startFrameX;
    int          endFrameX   =  modalitySetDefault.endFrameX;
    int         *answerA; //     =  modalitySetDefault.answerA;

    if ( !modsP->list.itemN ) {
	ERROR("No modalities for update added to '%s'.\n", modsP->name);
	return TCL_ERROR;
    }

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<startFrameX>", ARGV_INT, NULL, &startFrameX, NULL,
		       		"start frame for update",
		       "<endFrameX>",   ARGV_INT, NULL, &endFrameX,   NULL,
		       		"end frame for update",
		       NULL) != TCL_OK )
	return TCL_ERROR;
    
    answerA = modalitySetUpdate (modsP, startFrameX, endFrameX);

    if ( !answerA )
	ERROR("No update feasible for '%s' in [%i,%i].\n",
	      modsP->name, startFrameX, endFrameX);

    return TCL_OK;
}

/* ------------------------------------------------------------------------
    modalitySetAnswer
   ------------------------------------------------------------------------ */

int* modalitySetAnswer (ModalitySet *modsP, int startFrameX, int endFrameX) {

    int *answerA; // = modalitySetDefault.answerA;
    int  i;

    if ( modsP->startFrameX == modalitySetDefault.startFrameX && 
	 modsP->endFrameX   == modalitySetDefault.endFrameX ) {

	ERROR("First do update for '%s'.\n", modsP->name);
	return NULL;
    }

    if ( startFrameX < 0 || modsP->startFrameX > startFrameX ||
	                    modsP->endFrameX   < endFrameX ) {
	ERROR("No answer available for '%s' in [%i,%i].\n",
	      modsP->name, startFrameX, endFrameX);
	return NULL;
    }

    answerA = (int*)malloc((endFrameX-startFrameX+1) * sizeof(int));

    for (i = 0; i <= endFrameX-startFrameX; i++) {
	answerA[i] = modsP->answerA[startFrameX - modsP->startFrameX + i];
    }

    return answerA;
}

int modalitySetAnswerItf (ClientData cd, int argc, char *argv[]) {

    ModalitySet  *modsP       = (ModalitySet*)cd;
    int           startFrameX =  modalitySetDefault.startFrameX;
    int           endFrameX   =  modalitySetDefault.endFrameX;
    int          *answerA; //     =  modalitySetDefault.answerA;
    int           i;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<startFrameX>", ARGV_INT, NULL, &startFrameX, NULL,
		       		"start frame for answer",
		       "<endFrameX>",   ARGV_INT, NULL, &endFrameX,   NULL,
		       		"end frame for answer",
		       NULL) != TCL_OK )
	return TCL_ERROR;

    answerA = modalitySetAnswer (modsP, startFrameX, endFrameX);

    if ( !answerA )
	return TCL_ERROR;

    /* output of answerA */
    itfAppendResult ("%i", answerA[0]);
    for (i = 1; i <= endFrameX-startFrameX; i++) {
	itfAppendResult (" %i", answerA[i]);
    }

    free (answerA); answerA = NULL;

    return TCL_OK;
}

/* ------------------------------------------------------------------------
    modalitySetMajorityItf
   ------------------------------------------------------------------------ */

int modalitySetMajorityItf (ClientData cd, int argc, char *argv[]) {

    ModalitySet *modsP = (ModalitySet*)cd;
    int res = 0;
    int i;
    
    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       NULL) != TCL_OK )
	return TCL_ERROR;

    for ( i = 0; i < modsP->list.itemN; i++ ) {
	Modality *modP = &(modsP->list.itemA[i]);
	res |= modalityMajority (modP) << i;
    }

    itfAppendResult("%i", res);

    return TCL_OK;
}


int modalitySetMajorityAnswer (ModalitySet *modsP, int startFrameX, int endFrameX)
{
    int majAnswer = 0;
    int answer;
    int i;

    for (i = 0; i < modsP->list.itemN; i++) {
	Modality *modP = &(modsP->list.itemA[i]);
	answer = modalityMajorityAnswer (modP, startFrameX, endFrameX);

	if ( answer < 0 ) {
	    ERROR("Answer undefined for Modality '%s'.\n", modP->name);
	    return -1;
	}

	majAnswer |= (answer << i);
    }

    return majAnswer;
}

int modalitySetMajorityAnswerItf (ClientData cd, int argc, char *argv[]) {

    ModalitySet *modsP       = (ModalitySet*)cd;
    int          startFrameX =  modsP->startFrameX;
    int          endFrameX   =  modsP->endFrameX;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "-startFrameX", ARGV_INT, NULL, &startFrameX, NULL,
		       		"start frame for answer",
		       "-endFrameX",   ARGV_INT, NULL, &endFrameX,   NULL,
		       		"end frame for answer",
		       NULL) != TCL_OK )
	return TCL_ERROR;

    itfAppendResult("%i", modalitySetMajorityAnswer ((ModalitySet*)cd, startFrameX,
						     endFrameX));

    return TCL_OK;
}


int modalitySetAnswer2TagsItf (ClientData cd, int argc, char *argv[]) {

    ModalitySet *modsP = (ModalitySet*)cd;
    int answer = 0;
    int i;
    
    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<answer>", ARGV_INT, NULL, &answer, NULL,
		       		"answer for modalities (binary coded)",
		       NULL) != TCL_OK )
	return TCL_ERROR;

    for ( i = 0; i < modsP->list.itemN; i++ ) {

	if ( (1<<i) == (answer & (1<<i)) ) {
	    Modality *modP = &(modsP->list.itemA[i]);
	    if ( i == 0 ) itfAppendResult("%s",  modP->tagName);
	    else          itfAppendResult(" %s", modP->tagName);
	}

    }

    return TCL_OK;
}

/* ------------------------------------------------------------------------
    modalitySetPutsItf
   ------------------------------------------------------------------------ */

static int modalitySetPutsItf (ClientData cd, int argc, char *argv[]) {

    ModalitySet *modsP = (ModalitySet*)cd;
    int i;
    
    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       NULL) != TCL_OK )
	return TCL_ERROR;

    for ( i = 0; i < modsP->list.itemN; i++ ) {
	modalityPutsItf ((ClientData)&(modsP->list.itemA[i]), argc, argv);
	itfAppendResult("\n");
    }

    return TCL_OK;
}


/* ------------------------------------------------------------------------
    modalitySetGetRootNodesItf
   ------------------------------------------------------------------------ */

int modalitySetGetRootNodesItf (ClientData cd, int argc, char *argv[]) {

    ModalitySet *modsP = (ModalitySet*)cd;
    int i;
    
    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       NULL) != TCL_OK )
	return TCL_ERROR;

    if ( !modsP->rootNodeA )
	return TCL_ERROR;

    itfAppendResult ("%i", modsP->rootNodeA[0]);
    for (i=1; i < modsP->rootNodeN; i++) {
	itfAppendResult (" %i", modsP->rootNodeA[i]);
    }

    return TCL_OK;
}

/* ------------------------------------------------------------------------
    modalitySetResetItf
   ------------------------------------------------------------------------ */

int modalitySetResetItf (ClientData cd, int argc, char *argv[]) {

    ModalitySet *modsP = (ModalitySet*)cd;
    int i;
    
    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       NULL) != TCL_OK )
	return TCL_ERROR;

    modsP->startFrameX = modalitySetDefault.startFrameX;
    modsP->endFrameX   = modalitySetDefault.endFrameX;

    if ( modsP->answerA ) {
	free (modsP->answerA);
	modsP->answerA = modalitySetDefault.answerA;
    }

    for (i=0; i<modsP->list.itemN; i++) {
	Modality *modP = &(modsP->list.itemA[i]);
	modalityReset (modP);
    }

    return TCL_OK;
}


int modalitySetDeleteTagsItf (ClientData cd, int argc, char *argv[]) {

    ModalitySet *modsP = (ModalitySet*)cd;
    int i;
    
    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       NULL) != TCL_OK )
	return TCL_ERROR;

    if ( !modsP->addTags ) {
	ERROR("No tags added for '%s'.\n",modsP->name);
	return TCL_ERROR;
    }

    for (i=0; i < modsP->list.itemN; i++) {
	Modality *modP = &(modsP->list.itemA[i]);
	argv[0] = modP->tagName;
	tagsDelete (modsP->tags, 1, argv);
    }

    return TCL_OK;
}

int modalitySetAddTagsItf (ClientData cd, int argc, char *argv[]) {

    ModalitySet *modsP = (ModalitySet*)cd;
    int i;
    
    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       NULL) != TCL_OK )
	return TCL_ERROR;

    for (i=0; i < modsP->list.itemN; i++) {
	Modality *modP = &(modsP->list.itemA[i]);
	argv[0] = modP->tagName;
	tagsAdd (modsP->tags, 1, argv);
    }

    modsP->addTags = 1;

    return TCL_OK;
}


/* ------------------------------------------------------------------------
    modalitySetTraceItf
   ------------------------------------------------------------------------ */

int modalitySetTrace (ModalitySet *modsP, int nodeX, int answers) {

    if ( nodeX >= 0 && stateTableIsLeaf (modsP->tree, nodeX) ) {
	int model = modsP->tree->list.itemA[nodeX].model;

	itfAppendResult("'%i' is leaf with model '%i'.\n", nodeX, model);

    } else if ( nodeX >= 0 && modsP->tree->list.itemA[nodeX].question >= 0 ) {
	int tIdx    = stateTableGetTagX (modsP->tree, modsP->localTags, nodeX);
	int yes     = modsP->tree->list.itemA[nodeX].yes;
	int no      = modsP->tree->list.itemA[nodeX].no;
	int answer  = 0;

	if ( tIdx < 0 ) {
	    ERROR("Negative tag index '%i' for node '%i'.\n", tIdx, nodeX);
	    return TCL_ERROR;
	}

	answer |= 1 << tIdx;

	if ( (answer & answers) == answer ) {
	    itfAppendResult("From yes-part of '%i' to '%i', tag '%s'.\n",
			    nodeX, yes, modsP->localTags->list.itemA[tIdx].name);
	    modalitySetTrace (modsP, yes, answers);
	} else {
	    itfAppendResult("From  no-part of '%i' to '%i', tag '%s'.\n",
			    nodeX, no, modsP->localTags->list.itemA[tIdx].name);
	    modalitySetTrace (modsP, no, answers);
	}

    } else {
	ERROR("This can not be: nodeX '%i', answers '%i'.\n",
	      nodeX, answers);

	return TCL_ERROR;
    }

    return TCL_OK;
}

int modalitySetTraceItf (ClientData cd, int argc, char *argv[]) {

    ModalitySet *modsP = (ModalitySet*)cd;
    int rootX   = 0;
    int answers = 0;
    
    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<rootX>", ARGV_INT, NULL, &rootX, NULL,
		       		"root node index of subtree",
		       "<answer>", ARGV_INT, NULL, &answers, NULL,
		       		"answers for modalities (coded as int)",
		       NULL) != TCL_OK )
	return TCL_ERROR;

    return modalitySetTrace (modsP, rootX, answers);
}


/* ------------------------------------------------------------------------
   modalitySetAnswer2Tags, modalitySetGetDeletionMask
   ------------------------------------------------------------------------ */

/* returns tags binary coded for an answer */
int modalitySetAnswer2CodedTags (ModalitySet *modsP, Tags *tags, int answer)
{
    Modality *modP;
    int       m, realTagX, realModTags;

    realModTags = 0;

    for (m = 0; m < modsP->list.itemN; m++) {
	if ( (1<<m) == (answer & (1<<m)) ) {
	    modP         = &(modsP->list.itemA[m]);
	    realTagX     = listName2Index ((List*)&(tags->list), modP->tagName);
	    if ( realTagX < 0 ) {
		ERROR ("Tag %s not exists in %s.\n", modP->tagName, tags->name);
		return -1;
	    }
	    realModTags |= (1<<realTagX);
	}
    }

    return realModTags;
}


int modalitySetAnswer2CodedTagsItf (ClientData cd, int argc, char *argv[]) {

    ModalitySet *modsP   = (ModalitySet*)cd;
    Tags        *tags    = modsP->tags;
    int          answer;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<answer>", ARGV_INT, NULL, &answer,   NULL,
		       		"answer (majority)",
		       "-tags", ARGV_OBJECT, NULL, &tags, "Tags",
		       		"tags-object",
		       NULL) != TCL_OK )
	return TCL_ERROR;

    itfAppendResult("%i", modalitySetAnswer2CodedTags (modsP, tags, answer));

    return TCL_OK;
}



/* returns a mask to unset all modality-tags */
int modalitySetGetDeletionMask (ModalitySet *modsP, Tags *tags) {

    Modality *modP;
    int       m, realTagX, mask, invMask;

    mask = 0;

    for (m = 0; m < modsP->list.itemN; m++) {
	modP      = &(modsP->list.itemA[m]);
	realTagX  = listName2Index ((List*)&(tags->list), modP->tagName);
	mask     |= (1<<realTagX);
    }

    /* invert modMask to unmask modality tags in all phones either the mid phone
       in a word by AND operation */
    invMask = mask ^ -1;

    return invMask;
}


int modalitySetDemaskWord (Word *word, Word *maskedWord, int modMask,
			   int left, int right) {
    int mid = -left;
    int i;

    maskedWord->phoneA = word->phoneA;
    maskedWord->itemN  = word->itemN;

    if ( maskedWord->tagA ) { free (maskedWord->tagA); maskedWord->tagA = NULL; }

    if ( modMask < 0 ) {
	maskedWord->tagA = word->tagA;
	return 0;
    }

    /* copy tags and demask them
       only the mid phone keeps the modality tags */
    maskedWord->tagA = (int*)malloc(word->itemN * sizeof(int));
    for (i=0; i<word->itemN; i++) {
	if ( i == mid )
	    maskedWord->tagA[i] = word->tagA[i];
	else
	    maskedWord->tagA[i] = word->tagA[i] & modMask;
    }

    return 0;
}


/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method modalityMethod[] = {

    { "puts",   modalityPutsItf,   "display the contents of the modality" },
    { "update", modalityUpdateItf, "update modality" },
    { "answer", modalityAnswerItf, "get anser for modality" },
    { "reset",  modalityResetItf,  "reset modality" },
    { "majorityAnswer", modalityMajorityAnswerItf, "get the majority of the answers" },
    { NULL, NULL, NULL }
};

TypeInfo modalityInfo = { "Modality", 0, -1, modalityMethod,
			  modalityCreateItf, modalityFreeItf,
			  modalityConfigureItf, NULL, NULL,
     "A 'Modality' object answers a question about the modality of a recording." };

Method modalitySetMethod[] = {

    { "puts",      modalitySetPutsItf, "display the contents of the modality-set" },
    { "add",          modalitySetAddItf,          "add a new modality to the set" },
    { "delete",       modalitySetDeleteItf,       "delete a modality from the set" },
    { "update",       modalitySetUpdateItf, "update all modalities in the set" },
    { "answer",    modalitySetAnswerItf, "get answer of all modalities in the set" },
    { "getRootNodes", modalitySetGetRootNodesItf, "get root nodes of tree" },
    { "reset",        modalitySetResetItf,        "reset set" },
    { "addTags",      modalitySetAddTagsItf,      "add tags to tags-object" },
    { "deleteTags",   modalitySetDeleteTagsItf,   "delete tags from tags-object" },
    { "trace",     modalitySetTraceItf, "trace given subtree with given answers" },
    { "majorityAnswer", modalitySetMajorityAnswerItf, "get the majority of the answers" },
    { "answer2tags", modalitySetAnswer2TagsItf, "get a list of tags for an answer" },
    { "answer2codedTags", modalitySetAnswer2CodedTagsItf, "coded tags for answer" },
    { NULL, NULL, NULL }
};

TypeInfo modalitySetInfo = { "ModalitySet", 0, -1, modalitySetMethod,
			     modalitySetCreateItf, modalitySetFreeItf,
			     modalitySetConfigureItf, modalitySetAccessItf,
			     NULL,
    "A 'ModalitySet' object is a set of modalities." };

Method stateTableMethod[] = {

    { "puts",     stateTablePutsItf,    "displays the contents of the state table" },
    { "update",   stateTableUpdateItf,   "update state-table" },
    { "set",      stateTableSetEntryItf, "set a single entry in the state table" },
    { "get",      stateTableGetEntryItf, "get a single entry of the state table" },
    { "copy",     stateTableCopyItf,     "copy state table" },
    { "resize",   stateTableResizeItf,   "resize state table" },
    { "read",     stateTableReadItf,     "read state table from file" },
    { "write",    stateTableWriteItf,    "write state table to file" },
    { "create",   stateTableMatrixCreateItf, "create new matrix" },
    { "lookup",   stateTableLookupItf,   "make a table lookup" },
    { "reset",    stateTableResetItf,    "reset state table and modalitySet" },
    { NULL, NULL, NULL }
};

TypeInfo stateTableInfo = { "StateTable", 0, -1, stateTableMethod,
			    stateTableCreateItf, stateTableFreeItf,
			    stateTableConfigureItf, stateTableAccessItf,
			    NULL,
    "A 'StateTable' object is a matrix for looking up distribution indices." };

static int modalitiesInitialized = 0;

int Modalities_Init (void) {

    if ( !modalitiesInitialized ) {
	if ( Tags_Init() != TCL_OK ) return TCL_ERROR;
	if ( Tree_Init() != TCL_OK ) return TCL_ERROR;

	modalityDefault.name            =  NULL;
	modalityDefault.useN            =  0;
	modalityDefault.startFrameX     = -1;
	modalityDefault.endFrameX       = -1;
	modalityDefault.answerA         =  NULL;
	modalityDefault.updateProc      =  NULL;
	modalityDefault.tagName         =  NULL;
	modalityDefault.yesN            =  0;
	modalityDefault.timeInfo        =  0;
	modalityDefault.updateMode      =  UPD_GIVEN;
	modalityDefault.updateLimit     = -1;

	modalitySetDefault.name         =  NULL;
	modalitySetDefault.useN         =  0;
	modalitySetDefault.tags         =  NULL;
	modalitySetDefault.list.itemN   =  0;
	modalitySetDefault.list.blkSize =  MAX_MODALITIES;
	modalitySetDefault.tags         =  NULL;
	modalitySetDefault.localTags    =  NULL;
	modalitySetDefault.tree         =  NULL;
	modalitySetDefault.startFrameX  =  modalityDefault.startFrameX;
	modalitySetDefault.endFrameX    =  modalityDefault.endFrameX;
	modalitySetDefault.answerA      =  NULL;
	modalitySetDefault.addTags      =  0;
	modalitySetDefault.rootNodeA    =  NULL;
	modalitySetDefault.rootNodeN    =  0;
	modalitySetDefault.dummyStart   = -1;

	stateTableDefault.name          =  NULL;
	stateTableDefault.useN          =  0;
	stateTableDefault.modsP         =  NULL;
	stateTableDefault.matrix        =  NULL;
	stateTableDefault.startFrameX   =  modalityDefault.startFrameX;
	stateTableDefault.endFrameX     =  modalityDefault.endFrameX;
	stateTableDefault.treeXN        =  0;
	stateTableDefault.modXN         =  0;
	stateTableDefault.m             =  0;
	stateTableDefault.n             =  0;
	stateTableDefault.dummyStart    = -1;
	stateTableDefault.timeInfo      =  0;
	stateTableDefault.compress      =  0;
	stateTableDefault.compressN     =  0;
	stateTableDefault.compressA     =  NULL;
	stateTableDefault.commentChar   =  ';';

	itfNewType (&modalityInfo);
	itfNewType (&modalitySetInfo);
	itfNewType (&stateTableInfo);

	modalitiesInitialized = 1;
    }

    return TCL_OK;

}
