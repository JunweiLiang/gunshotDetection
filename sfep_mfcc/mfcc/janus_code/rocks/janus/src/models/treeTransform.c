/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Tree
               ------------------------------------------------------------

    Author  :  Christian Fuegen
    Module  :  treeTransform.c
    Date    :  $Id: treeTransform.c 2390 2003-08-14 11:20:32Z fuegen $
    Remarks :  This Module transforms a tree, so that all given questions
               (usually modality questions) are on the bottom of the tree

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
   Revision 1.6  2003/08/14 11:20:19  fuegen
   Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

   Revision 1.5.4.2  2001/11/19 14:45:43  metze
   Go away, boring message!

   Revision 1.5.4.1  2001/02/27 17:32:10  metze
   Cleaned up warnings

   Revision 1.5  2001/01/15 09:50:00  janus
   Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

   Revision 1.4.4.1  2001/01/12 15:17:01  janus
   necessary changes for running janus under WINDOWS

   Revision 1.4  2000/11/14 12:35:26  janus
   Merging branch jtk-00-10-27-jmcd.

   Revision 1.3.30.4  2000/11/14 11:38:28  janus
   More changes to allow for clean compilation with `gcc -Wall'.

   Revision 1.3.30.3  2000/11/08 17:33:52  janus
   Finished making changes required for compilation under 'gcc -Wall'.

   Revision 1.3.30.2  2000/11/06 12:01:39  janus
   Just checking.

   Revision 1.3  2000/08/16 11:48:08  soltau
   free -> TclFree

   Revision 1.2  2000/01/12 10:12:49  fuegen
   transformation of a tree with modality questions

   Revision 1.1  1999/03/10 18:46:48  fuegen
   Initial revision


 
   ======================================================================== */

char treeTransformRCSVersion[]= 
           "@(#)1$Id: treeTransform.c 2390 2003-08-14 11:20:32Z fuegen $";

#include <ctype.h>

#include "distrib.h"
#include "codebook.h"
#include "treeTransform.h"

/* ========================================================================
    Global Declarations
   ======================================================================== */

static double treeModalityToXADFS (Tree *tree, int rootX, int *modelA, int *size, int* exp);
static int treeNewNodeX (Tree *tree, int nodeX, int setOrg);
int treeModalityTestDFS (Tree *tree, int rootX, QuestionSet *qsP);
int treeModalityDivideDFS (Tree *tree, Tree *mainTree, Tree *modTree,
			   QuestionSet *qsP, int rootX, char *dummyName,
			   int* dummyX, char *rootIdentifier);
static int treeModalityAddNode (Tree *tree, char *name, TreeNode* features);
int treeCopyDFS (Tree *tree, Tree *modTree, int rootX);
static int treeIsLeaf (Tree *tree, int nodeX);
int treeModalityMarkDFS (Tree *tree, QuestionSet *qsP, int rootX, int mark,
			 int modMark, int modFound);
static int treeTestModalityNode (Tree *tree, int nodeX, QuestionSet *qsP);
static int treeModalityChange (Tree *tree, int modNodeX, int fatherX,
			QuestionSet *qsP, int type);
int treeDublicateDFS (Tree *tree, int rootX, int setOrg);


/* ========================================================================
    Function definitions
   ======================================================================== */

/* get all root nodes in tree
   (i.e. all nodes with rootIdentifier in begin of their name)
   returns number of root nodes in array */
static int* treeGetRootNodes (Tree *tree, int *rootNodeN, char *rootIdentifier) {

    TreeNode *node      = NULL;
    int       size      = 3;
    int      *nodeXA    = (int*)malloc (size*sizeof(int));
    int       nodeN     = 0;
    char    **tokv, *result;
    int       tokc, i, j;

    result = strdup(rootIdentifier);
    if ( Tcl_SplitList (itf,result,&tokc,&tokv) != TCL_OK )
	ERROR("Tcl_SplitList");

    for (i=0; i<tree->list.itemN; i++) {
	node = &(tree->list.itemA[i]);

	for (j=0; j<tokc; j++) {

	    if ( !strncmp(node->name, tokv[j], strlen(tokv[j])) ) {
		if ( nodeN >= size ) {
		    size *= 2;
		    nodeXA = (int*)realloc(nodeXA, size * sizeof(int));
		}
		nodeXA[nodeN] = i;
		nodeN++;
	    }
	}
    }
    Tcl_Free((char*)tokv);

    if ( nodeN < size ) {
	nodeXA = (int*)realloc(nodeXA, nodeN * sizeof(int));
    }

    *rootNodeN = nodeN;

    return nodeXA;
}


static int treeSplitAndNodes (Tree *tree, QuestionSet *qsP) {

    TreeNode  *node      = NULL;
    TreeNode  *father    = NULL;
    Question  *qP        = NULL;
    int        changes   = 0;
    char     **tokv, *question, *result;
    int        tokc, i, j, questionX, newNodeX=0, modAndNode, nodeN;

    nodeN = tree->list.itemN;
    for (i=0; i<nodeN; i++) {
	node = &(tree->list.itemA[i]);

	if ( node->question >= 0 ) {
	    qP   = &(tree->questions.list.itemA[node->question]);

	    result = strdup(qP->name);
	    if ( Tcl_SplitList (itf,result,&tokc,&tokv) != TCL_OK )
		ERROR("Tcl_SplitList");
	    
	    /*
	      if ( tokc > 1 ) {
	      printf("\n  Split question %s of node %i into nodes", qP->name, i);
	      fflush(stdout);
	      }
	      */
	    
	    /* AND node with modality question? */
	    modAndNode = 0;
	    if ( tokc > 1 && qP->clauseN == 1 ) {
		for (j = 0; j < tokc; j++) {
		    if ( listIndex ((List*)&(qsP->list), tokv[j], 0) >= 0 )
			modAndNode = 1;
		}
	    }

	    /* split */
	    if ( modAndNode ) {
		for (j = 1; j < tokc; j++) {
		    
		    /* preconfigure */
		    if ( j == 1 ) {
			question  = tokv[0];
			questionX = listIndex ((List*)&(tree->questions.list),
					       question, 1);
			
			father           = node;
			father->question = questionX;
			newNodeX         = i;
			
			changes++;
		    }
		    
		    /* add new node to yes part of father with same features
		       as node (father) */
		    newNodeX    = treeNewNodeX (tree, newNodeX, 0);
		    question    = tokv[j];
		    questionX   = listIndex ((List*)&(tree->questions.list),
					     question, 1);
		    father->yes = newNodeX;
		    
		    /* printf(" %i,", newNodeX); fflush(stdout); */
		    
		    father = &(tree->list.itemA[newNodeX]);
		    father->question = questionX;
		}
	    }
	    Tcl_Free((char*)tokv);
	}
    }

    return changes;
}

/* type is type of nodeX (no=1, undef=2 or yes=3 part of fatherX) */
static int treeMoveDFS (Tree *tree, int nodeX, int fatherX, QuestionSet *qsP,
			int type, int *calls)
{
    TreeNode *node;
    int       noNodeX, yesNodeX, undefNodeX;

    (*calls)++;

    /* breaking condition */
    if ( treeIsLeaf (tree, nodeX) == 1 || treeIsLeaf (tree, nodeX) == -1 ) {
	(*calls)--;
	return 1;
    }

    node       = &(tree->list.itemA[nodeX]);
    noNodeX    = node->no;
    yesNodeX   = node->yes;
    undefNodeX = node->undef;

    if ( treeTestModalityNode (tree, nodeX, qsP) != -1 ) {

	nodeX      = treeModalityChange (tree, nodeX, fatherX, qsP, type);
	if ( nodeX < 0 ) {
	    (*calls)--;
	    printf("\n  negative node returned");
	    return 1;
	}
	node       = &(tree->list.itemA[nodeX]);
	noNodeX    = node->no;
	undefNodeX = node->undef;
	yesNodeX   = node->yes;
    }
    fatherX = nodeX;
    /* printf(" <%i>", *calls); fflush(stdout); */

    if ( !treeMoveDFS (tree,    noNodeX, fatherX, qsP, TREE_ANSWER_NO,    calls) ||
	 !treeMoveDFS (tree, undefNodeX, fatherX, qsP, TREE_ANSWER_UNDEF, calls) ||
	 !treeMoveDFS (tree,   yesNodeX, fatherX, qsP, TREE_ANSWER_YES,   calls) ) {

	ERROR("(%i) can not proceed DFS (f %i, n %i, u %i, y %i).\n", *calls,
	      fatherX, noNodeX, undefNodeX, yesNodeX);
	(*calls)--;
	return 0;
    }

    (*calls)--;

    return 1;
}

/* scan tree and let all nodes with modality questions fall down   *
 * changing example (yesNode is goodNode, node is badNode):        *
 *                father                               father      *
 *        node               A     ==>       yesNode            A  *
 * yesNode     3                        node_1      node_2         *
 * 1     2                              1    3      2    3         */
static int treeMoveModalityNodes (Tree *tree, QuestionSet *qsP, int *rootNodeXA,
			   int rootNodeN) {

    int calls     = 0;
    int i;

    /* for each root node */
    for (i=0; i<rootNodeN; i++) {

	if ( !treeMoveDFS (tree, rootNodeXA[i], -1, qsP, 0, &calls) )
	    ERROR("(%i) There is something strange in treeMoveDFS (root %i).\n",
		  calls, rootNodeXA[i]);

	calls = 0;
    }

    return calls;
}


/* changing procedure
 * type is type of modNodeX (no=0, undef=1 or yes=2 part of fatherX)
 * returns new fatherX to continue DFS
 *
 * description of variables (left=no, middle=undef, right=yes):
 *
 *                       fatherX
 *                       modNodeX
 *     noNodeX           undefNodeX          yesNodeX
 * oneX twoX threeX   fourX fiveX sixX   sevenX eightX nineX
 */
static int treeModalityChange (Tree *tree, int modNodeX, int fatherX,
			QuestionSet *qsP, int type) {

    TreeNode *father     = &(tree->list.itemA[fatherX]);
    TreeNode *modNode    = &(tree->list.itemA[modNodeX]);
    TreeNode *noNode     = NULL;
    TreeNode *undefNode  = NULL;
    TreeNode *yesNode    = NULL;
    TreeNode *node       = NULL;
    TreeNode *tmpNode    = NULL;
    TreeNode *tmp2Node   = NULL;
    int       noNodeX    = modNode->no;
    int       undefNodeX = modNode->undef;
    int       yesNodeX   = modNode->yes;
    int      *nodeXA     = (int*)malloc (9*sizeof(int));
    int      *tmpNodeXA  = (int*)malloc (9*sizeof(int));
    int       mode       = 0;
    int       newFatherX, doNext, nodeN, tmpNodeN, tmpNodeX, i, j, setOrg;
    int       oneX, twoX, threeX, fourX, fiveX, sixX, sevenX, eightX, nineX;
    int       oneN, twoN, threeN, fourN, fiveN, sixN, sevenN, eightN, nineN;
    int       setOrgN, setOrgU;
    int       noN, undefN, yesN;

    oneX = twoX = threeX = fourX = fiveX = sixX = sevenX = eightX = nineX = -1;
    oneN = twoN = threeN = fourN = fiveN = sixN = sevenN = eightN = nineN =  0;
    noN = undefN = yesN = 0;
    newFatherX = modNodeX;

    /* look for leafs */
    if ( noNodeX    != -1 ) noNode    = &(tree->list.itemA[noNodeX]);
    if ( undefNodeX != -1 ) undefNode = &(tree->list.itemA[undefNodeX]);
    if ( yesNodeX   != -1 ) yesNode   = &(tree->list.itemA[yesNodeX]);

    /* look for leafs of leaf (check if node is a leaf -> set nodeX to -1)
       this order (yes, undef, no) is correct */
    if ( yesNode ) {
	sevenX = yesNode->no;
	eightX = yesNode->undef;
	nineX  = yesNode->yes;
	if ( sevenX + eightX + nineX == -3 ||
	     treeTestModalityNode (tree, yesNodeX, qsP) >= 0 ) yesNodeX   = -1;
    }
    if ( undefNode ) {
	fourX  = undefNode->no;
	fiveX  = undefNode->undef;
	sixX   = undefNode->yes;
	if ( fourX + fiveX + threeX  == -3 ||
	     treeTestModalityNode (tree, undefNodeX, qsP) >= 0 ) undefNodeX = -1;
    }
    if ( noNode ) {
	oneX   = noNode->no;
	twoX   = noNode->undef;
	threeX = noNode->yes;
	if ( oneX + twoX + threeX    == -3 ||
	     treeTestModalityNode (tree, noNodeX, qsP) >= 0 ) noNodeX    = -1;
    }

    if ( noNodeX + undefNodeX + yesNodeX == -3 ) {
	return newFatherX;
    }

    /* begin changing */

    /* change father entry (only one) */
    if ( noNodeX >= 0 ) {
	nodeXA[0]  = noNodeX;
	doNext     = TREE_ANSWER_UNDEF;
	mode      |= (1 << TREE_ANSWER_NO);
    } else if ( undefNodeX >= 0 ) {
	nodeXA[0]  = undefNodeX;
	doNext     = TREE_ANSWER_YES;
	mode      |= (1 << TREE_ANSWER_UNDEF);
    } else if ( yesNodeX >= 0 ) {
	nodeXA[0]  = yesNodeX;
	doNext     = -1;
	mode      |= (1 << TREE_ANSWER_YES);
    } else {
	ERROR("Node %i with modality question is a leaf.\n", modNodeX);
	return -1;
    }
    nodeN = 1;

    switch (type) {
    case TREE_ANSWER_NO:    father->no    = nodeXA[0]; break;
    case TREE_ANSWER_UNDEF: father->undef = nodeXA[0]; break;
    case TREE_ANSWER_YES:   father->yes   = nodeXA[0]; break;
    default: ERROR("Undefined type %i.\n", type);
    }

    /* change other nodes entries */
    tmpNodeN = 0;
    switch (doNext) {
    case TREE_ANSWER_UNDEF:
	if ( undefNodeX >= 0 ) {
	    for (i=0; i<nodeN; i++) {
		node = &(tree->list.itemA[nodeXA[i]]);

		if ( node->no    != -1 ) {
		    node->no    = treeNewNodeX (tree, undefNodeX, !tmpNodeN);
		    tmpNodeXA[tmpNodeN] = node->no;
		    tmpNodeN++;
		} else ERROR("Node %i not answered with no.\n", nodeXA[i]);
		if ( node->undef != -1 ) {
		    node->undef = treeNewNodeX (tree, undefNodeX, !tmpNodeN);
		    tmpNodeXA[tmpNodeN] = node->undef;
		    tmpNodeN++;
		} else {
		    /* add dummy node */
		    tmpNodeXA[tmpNodeN] = -1;
		    tmpNodeN++;
		}
		if ( node->yes   != -1 ) {
		    node->yes   = treeNewNodeX (tree, undefNodeX, !tmpNodeN);
		    tmpNodeXA[tmpNodeN] = node->yes;
		    tmpNodeN++;
		} else ERROR("Node %i not answered with yes.\n", nodeXA[i]);
	    }
	    /* copy XA */
	    for (i=0; i<tmpNodeN; i++) {
		nodeXA[i] = tmpNodeXA[i];
	    }
	    nodeN    = tmpNodeN;
	    tmpNodeN = 0;
	    mode    |= (1 << TREE_ANSWER_UNDEF);
	}
	doNext = TREE_ANSWER_YES;
    case TREE_ANSWER_YES:
	if ( yesNodeX >= 0 ) {
	    for (i=0; i<nodeN; i++) {
		if ( nodeXA[i] >= 0 ) {
		    node = &(tree->list.itemA[nodeXA[i]]);

		    if ( node->no    != -1 ) {
			node->no    = treeNewNodeX (tree, yesNodeX, !tmpNodeN);
			tmpNodeXA[tmpNodeN] = node->no;
			tmpNodeN++;
		    } else ERROR("Node %i not answered with no.\n", nodeXA[i]);
		    if ( node->undef != -1 ) {
			node->undef = treeNewNodeX (tree, yesNodeX, !tmpNodeN);
			tmpNodeXA[tmpNodeN] = node->undef;
			tmpNodeN++;
		    } else {
			/* add dummy node */
			tmpNodeXA[tmpNodeN] = -1;
			tmpNodeN++;
		    }
		    if ( node->yes   != -1 ) {
			node->yes   = treeNewNodeX (tree, yesNodeX, !tmpNodeN);
			tmpNodeXA[tmpNodeN] = node->yes;
			tmpNodeN++;
		    } else ERROR("Node %i not answered with yes.\n", nodeXA[i]);
		} else {
		    /* for one left node, three dummy nodes added */
		    for (j=0; j<3; j++) {
			tmpNodeXA[tmpNodeN] = -1;
			tmpNodeN++;
		    }
		}
	    }
	    /* copy XA */
	    for (i=0; i<tmpNodeN; i++) {
		nodeXA[i] = tmpNodeXA[i];
	    }
	    nodeN    = tmpNodeN;
	    tmpNodeN = 0;
	    mode    |= (1 << TREE_ANSWER_YES);
	}
	doNext = -1;
    default: break;
    }

    /*
      printf("\n  Mode %i, nodeN %i, doNext %i, modNodeX %i, noNodeX %i,"
      " undefNodeX %i, yesNodeX %i, nodeX %i. ", mode, nodeN, doNext,
      modNodeX, noNodeX, undefNodeX, yesNodeX, nodeXA[0]);
      fflush(stdout);
      */

    /* Alle Knoten besitzen einen yes und einen no Zweig, aber nicht alle Knoten
       haben einen undef Zweig.
       In der Variable mode ist die Arte der zu verwendeten Knoten festgelegt.
       Die Reihenfolge der Knoten ist fest vorgegeben (1. no, 2. undef, 3. yes
       if exists). Oben war es von noeten, fuer Knoten, die Blaetter sind, den
       entsprechenden Index auf -1 zu setzen. Durch die Art des modes und der
       Voraussetzung, daž immer yes und no Zweig vorhanden sein muessn, lassen
       sich jedoch Rueckschluesse auf vorhandene Blaetter ziehen, und den
       entsprechenden Indices (yesNodeX, noNodeX) zuweisen. Aus diesem Grund
       funktionieren auch die Fehlerabfragen ’ber die Variable nodeN. Diese gibt
       die Anzahl der Knoten im nodeXA an. Fuer nicht existente Knoten werden
       sogenannte dummy-Knoten mit Index -1 in das Array an die entsprechende
       Stelle eingefuegt. Das Abfangen der Undef-F„lle ist noch nicht ganz
       ausgereift und auch nicht richtig durchgetestet (wegen mangelndem
       Testmaterial), muesste aber bei richtiger mode Wahl und Index Setzung
       ohne weitere Aenderung funktionieren. */

    switch (mode) {
    case 1:
	/* yes is leaf and no */
	yesNodeX = modNode->yes;
	if ( nodeN != 1 || yesNodeX == -1 ) {
	    ERROR("There is something strange %i, %i, %i.\n", mode, nodeN, yesNodeX);
	    return -1;
	}
	break;
    case 3:
	/* yes is leaf and undef and no */
	yesNodeX = modNode->yes;
	if ( nodeN != 3 || yesNodeX == -1 ) {
	    ERROR("There is something strange %i, %i, %i.\n", mode, nodeN, yesNodeX);
	    return -1;
	}
	break;
    case 4:
	/* yes and no is leaf */
	noNodeX = modNode->no;
	if ( nodeN != 1 || noNodeX == -1 ) {
	    ERROR("There is something strange %i, %i, %i.\n", mode, nodeN, noNodeX);
	    return -1;
	}
	break;
    case 5:
	/* yes and no */
	if ( nodeN != 3 ) {
	    ERROR("There is something strange %i, %i.\n", mode, nodeN);
	    return -1;
	}
	break;
    case 6:
	/* yes and undef and no is leaf */
	noNodeX = modNode->no;
	if ( nodeN != 3 || noNodeX == -1 ) {
	    ERROR("There is something strange %i, %i, %i.\n", mode, nodeN, noNodeX);
	    return -1;
	}
	break;
    case 7:
	/* yes and undef and no */
	if ( nodeN != 9 ) {
	    ERROR("There is something strange %i, %i.\n", mode, nodeN);
	    return -1;
	}
	break;
    default:
	ERROR("Undefined mode %i.\n", mode);
	return -1;
	break;
    }

    /*
    printf("\n  Mode %i, modNodeX %i, noNodeX %i, undefNodeX %i, yesNodeX %i. ",
	   mode, modNodeX, noNodeX, undefNodeX, yesNodeX);
    fflush(stdout);
    */

    setOrg = 1; setOrgN = 1; setOrgU = 1; setOrgN = 1;
    for (i=1; i<=nodeN; i++) {
	if ( nodeXA[i-1] >= 0 ) {
	    tmpNodeX = treeNewNodeX (tree, modNodeX, setOrg);
	    tmpNode  = &(tree->list.itemA[tmpNodeX]);
	    setOrg   = 1;
	    node     = &(tree->list.itemA[nodeXA[i-1]]);
	    if ( mode == 3 || mode == 5 || mode == 7 ) {
		if ( (int)(ceil(i/(nodeN/3))) == 1 ) {
		    tmpNode->no = oneX;   noN = oneN++;   }
		if ( (int)(ceil(i/(nodeN/3))) == 2 ) {
		    tmpNode->no = twoX;   noN = twoN++;   }
		if ( (int)(ceil(i/(nodeN/3))) == 3 ) {
		    tmpNode->no = threeX; noN = threeN++; }
	    } else if ( mode == 4 || mode == 6 ) {
		tmpNode->no = oneX = noNodeX; noN = oneN++;
	    }
	    if ( mode == 7 ) {
		if ( i % 3 == 0 ) { tmpNode->undef = sixX;  undefN = sixN++;  }
		if ( i % 3 == 1 ) { tmpNode->undef = fiveX; undefN = fiveN++; }
		if ( i % 3 == 2 ) { tmpNode->undef = fourX; undefN = fourN++; }
	    }
	    if ( node->no >= 0 ) {
		node->no = treeNewNodeX (tree, tmpNodeX, setOrg); setOrg = 0;
		tmp2Node = &(tree->list.itemA[node->no]);

		if ( mode == 1 ) { tmp2Node->no = oneX; noN = oneN++; }
		if ( mode == 3 || mode == 6 ) {
		    tmp2Node->undef = fourX; undefN = fourN++; }
		if ( mode == 4 || mode == 5 || mode == 6 || mode == 7 ) {
		    tmp2Node->yes = sevenX; yesN = sevenN++;
		} else if ( mode == 1 || mode == 3 ) {
		    tmp2Node->yes = sevenX = yesNodeX; yesN = sevenN++;
		}

		if ( tmp2Node->no    >= 0 ) {
		    tmp2Node->no    = treeDublicateDFS (tree, tmp2Node->no, !noN);
		    noN++;
		}
		if ( tmp2Node->undef >= 0 ) {
		    tmp2Node->undef = treeDublicateDFS (tree, tmp2Node->undef,
							!undefN);
		    undefN++;
		}
		if ( tmp2Node->yes   >= 0 ) {
		    tmp2Node->yes   = treeDublicateDFS (tree, tmp2Node->yes, !yesN);
		    yesN++;
		}
		/*
		printf("\n  (N %s) set node %s to %i, %i, %i - noN %i, undefN %i, "
		       "yesN %i", node->name, tmp2Node->name, tmp2Node->no,
		       tmp2Node->undef, tmp2Node->yes, noN-1, undefN-1, yesN-1);
		*/
	    } else ERROR("node->no of node %s < 0", node->name);
	    if ( node->undef >= 0 ) {
		node->undef = treeNewNodeX (tree, tmpNodeX, setOrg); setOrg = 0;
		tmp2Node    = &(tree->list.itemA[node->undef]);

		if ( mode == 1 ) { tmp2Node->no = twoX; noN = twoN++; }
		if ( mode == 3 || mode == 6 ) {
		    tmp2Node->undef = fiveX; undefN = fiveN++; }
		if ( mode == 4 || mode == 5 || mode == 6 || mode == 7 ) {
		    tmp2Node->yes = eightX; yesN = eightN++;
		} else if ( mode == 1 || mode == 3 ) {
		    tmp2Node->yes = sevenX = yesNodeX; yesN = sevenN++;
		}

		if ( tmp2Node->no >= 0 ) {
		    tmp2Node->no = treeDublicateDFS (tree, tmp2Node->no, !noN);
		    noN++;
		}
		if ( tmp2Node->undef >= 0 ) {
		    tmp2Node->undef = treeDublicateDFS (tree, tmp2Node->undef,
							!undefN);
		    undefN++;
		}
		if ( tmp2Node->yes >= 0 ) {
		    tmp2Node->yes = treeDublicateDFS (tree, tmp2Node->yes, !yesN);
		    yesN++;
		}
		/*
		printf("\n  (U %s) set node %s to %i, %i, %i - noN %i, undefN %i, "
		       "yesN %i", node->name, tmp2Node->name, tmp2Node->no,
		       tmp2Node->undef, tmp2Node->yes, noN-1, undefN-1, yesN-1);
		*/
	    }
	    if ( node->yes >= 0 ) {
		node->yes = treeNewNodeX (tree, tmpNodeX, setOrg); setOrg = 0;
		tmp2Node  = &(tree->list.itemA[node->yes]);

		if ( mode == 1 ) { tmp2Node->no = threeX; noN = threeN++; }
		if ( mode == 3 || mode == 6 ) {
		    tmp2Node->undef = sixX; undefN = sixN++; }
		if ( mode == 4 || mode == 5 || mode == 6 || mode == 7 ) {
		    tmp2Node->yes = nineX; yesN = nineN++;
		} else if ( mode == 1 || mode == 3 ) {
		    tmp2Node->yes = sevenX = yesNodeX; yesN = sevenN++;
		}

		if ( tmp2Node->no >= 0 ) {
		    tmp2Node->no = treeDublicateDFS (tree, tmp2Node->no, !noN);
		    noN++;
		}
		if ( tmp2Node->undef >= 0 ) {
		    tmp2Node->undef = treeDublicateDFS (tree, tmp2Node->undef,
							!undefN);
		    undefN++;
		}
		if ( tmp2Node->yes >= 0 ) {
		    tmp2Node->yes = treeDublicateDFS (tree, tmp2Node->yes, !yesN);
		    yesN++;
		}
		/*
		printf("\n  (Y %s) set node %s to %i, %i, %i - noN %i, undefN %i,"
		       "yesN %i", node->name, tmp2Node->name, tmp2Node->no,
		       tmp2Node->undef, tmp2Node->yes, noN-1, undefN-1, yesN-1);
		*/
	    } else ERROR("node->yes of node %s < 0", node->name);
	}
    }

    /* free */
    free(nodeXA);    nodeXA    = NULL;
    free(tmpNodeXA); tmpNodeXA = NULL;

    return newFatherX;
}


/* test tree, wether all modality questions are form the bottom up
   returns  1 if tree is ok
   returns  0 if tree is not ok
   returns -1 if an error occurs
   */
int treeModalityTest (Tree *tree, QuestionSet *qsP, int *rootNodeXA,
		      int rootNodeN) {

    TreeNode *node;
    int res  = 1;
    int i;

    for (i=0; i<tree->list.itemN; i++) {
	node = &(tree->list.itemA[i]);
	node->mark = -15;
    }

    for (i=0; i<rootNodeN; i++) {
	treeModalityTestDFS (tree, rootNodeXA[i], qsP);
	node = &(tree->list.itemA[rootNodeXA[i]]);
	if ( !node->mark ) {
	    /* printf("\n  Errors found in tree with root %i.\n", rootNodeXA[i]); */
	}
	res &= node->mark;
    }

    /* look for neglected nodes */
    for (i=0; i<tree->list.itemN; i++) {
	node = &(tree->list.itemA[i]);
	if ( node->mark == -15 ) {
	    if ( i == tree->list.blkSize )
		ERROR("node %s (%i). You must configure 'blkSize' of %s with a higher value than %i.\n", node->name, i, tree->name, tree->list.blkSize);
	    else
		ERROR("node %s (%i) without father, successors %i, %i, %i.\n",
		      node->name, i, node->no, node->undef, node->yes);
	    return -1;
	}
    }

    return res;
}


/* rootNodeX has no modality question
   mark of root is set to 1 if tree is ok */
int treeModalityTestDFS (Tree *tree, int rootX, QuestionSet *qsP) {

    TreeNode *node;
    TreeNode *tmpNode;
    int res  = 1;
    int test = 0;
    int ret1 = -2;
    int ret2 = -2;
    int ret3 = -2;

    node = &(tree->list.itemA[rootX]);
    node->mark = -1;

    if ( treeIsLeaf (tree, rootX) == 1 ) {
	node->mark = 1;
	return -1;
    } else {
	test = (treeTestModalityNode (tree, rootX, qsP) >= 0 ? 1 : 0);
    }

    if ( node->no    >= 0 ) ret1 = treeModalityTestDFS (tree, node->no,   qsP);
    if ( node->undef >= 0 ) ret2 = treeModalityTestDFS (tree, node->undef, qsP);
    if ( node->yes   >= 0 ) ret3 = treeModalityTestDFS (tree, node->yes,   qsP);

    if ( test == 1 ) {
	if ( (ret1 == -2 || ret1 == -1 || ret1 == 1) &&
	     (ret2 == -2 || ret2 == -1 || ret2 == 1) &&
	     (ret3 == -2 || ret3 == -1 || ret3 == 1) )
	    node->mark = 1;
	else node->mark = 0;
    }

    if ( test == 0 ) node->mark = 1;
    /*
    printf ("\n  node %s, mark %i, rets %i, %i, %i", node->name, node->mark,
	    ret1, ret2, ret3);
    */
    if ( ret1 != -2 ) {
	tmpNode = &(tree->list.itemA[node->no]);
	res &= tmpNode->mark;
    }
    if ( ret2 != -2 ) {
	tmpNode = &(tree->list.itemA[node->undef]);
	res &= tmpNode->mark;
    }
    if ( ret3 != -2 ) {
	tmpNode = &(tree->list.itemA[node->yes]);
	res &= tmpNode->mark;
    }

    if ( res == 0 ) node->mark = 0;

    /* printf (" -> %i", node->mark); */

    return test;
}

/* make all nodes deterministic
   mistakes in tree are:
     more than two different fathers
     more than two same childs
     (both without root nodes) */
int treeMakeDeterministic (Tree *tree, QuestionSet *qsP, int *rootNodeXA,
			   int rootNodeN) {

    TreeNode *node    = NULL;
    TreeNode *tmpNode = NULL;
    int changes       = 0;
    int i, nodeN;

    for (i=0; i<rootNodeN; i++) {
	treeModalityMarkDFS (tree, qsP, rootNodeXA[i], -2, -1, 0);
    }

    /* printf("\n  marks set.");*/

    nodeN = tree->list.itemN;
    for (i=0; i<nodeN; i++) {
	node = &(tree->list.itemA[i]);

	/* test childs (mark != root node) */
	if ( node->mark != -2 ) {
	    if ( node->no    >= 0 && node->undef >= 0 &&
		 node->no    == node->undef ) {
		/* printf("\n  dublicate %s (no==undef)", node->name); fflush(stdout);*/
		node->undef = treeDublicateDFS (tree, node->undef, 0);
		changes++;
	    }
	    if ( node->no    >= 0 && node->yes   >= 0 &&
		 node->no    == node->yes ) {
		/* printf("\n  dublicate %s (no==yes)", node->name); fflush(stdout);*/
		node->yes = treeDublicateDFS (tree, node->yes, 0);
		changes++;
	    }
	    if ( node->undef >= 0 && node->yes   >= 0 &&
		 node->undef == node->yes ) {
		/* printf("\n  dublicate %s (undef==yes)", node->name); fflush(stdout);*/
		node->undef = treeDublicateDFS (tree, node->undef, 0);
		changes++;
	    }

	    /* look for fathers */
	    if ( node->no    >= 0 ) {
		tmpNode = &(tree->list.itemA[node->no]);
		if ( tmpNode->mark >= 0 ) {
		    /* printf("\n  dublicate %s (father->no %i)", node->name, node->no); fflush(stdout);*/
		    node->no = treeDublicateDFS (tree, node->no, 0);
		    tmpNode = &(tree->list.itemA[node->no]);
		    changes++;
		}
		tmpNode->mark = i;
	    }
	    if ( node->undef >= 0 ) {
		tmpNode = &(tree->list.itemA[node->undef]);
		if ( tmpNode->mark >= 0 ) {
		    /* printf("\n  dublicate %s (father->undef %i)", node->name, node->undef); fflush(stdout);*/
		    node->undef = treeDublicateDFS (tree, node->undef, 0);
		    tmpNode = &(tree->list.itemA[node->undef]);
		    changes++;
		}
		tmpNode->mark = i;
	    }
	    if ( node->yes   >= 0 ) {
		tmpNode = &(tree->list.itemA[node->yes]);
		if ( tmpNode->mark >= 0 ) {
		    /* printf("\n  dublicate %s (father->yes %i)", node->name, node->yes); fflush(stdout);*/
		    node->yes = treeDublicateDFS (tree, node->yes, 0);
		    tmpNode = &(tree->list.itemA[node->yes]);
		    changes++;
		}
		tmpNode->mark = i;
	    }
	    /* printf(" (%i)", i); fflush(stdout); */
	}
    }

    return changes;
}


/* divide tree into several subtrees
   each new root of a subtree will be inserted in rootNodeXA
   split subtrees with modality questions off an store it into modTree,
   the rest of the tree will be stored into originally tree
   erzeuge so wenig wie moeglich verschiedene Unterb„ume */
int treeModalityDivide (Tree *tree, Tree *mainTree, Tree *modTree, QuestionSet *qsP,
			int *rootNodeXA, int rootNodeN, char *dummyName,
			char *rootIdentifier) {

    int      dummyX = 1;
    int      i      = 0;
    /* int      idN    = 0; */

    /*    struct subTreeDesc idA[TREE_MAXDESC];*/


    for (i=0; i<rootNodeN; i++) {
	/* printf("\n    rootNode %i:", i); fflush(stdout); */
	/*	treeModalityDivideDFS (tree, mainTree, modTree, qsP, rootNodeXA[i],
			       dummyName, &dummyX, rootIdentifier, idA, &idN);*/
	treeModalityDivideDFS (tree, mainTree, modTree, qsP, rootNodeXA[i],
			       dummyName, &dummyX, rootIdentifier);
	
    }

    /*
    for (i=0; i<idN; i++) {
	if (idA[i].modelXA) { free(idA[i].modelXA); idA[i].modelXA = NULL; }
    }
    */

    return 1;
}


int treeModalityDivideDFS (Tree *tree, Tree *mainTree, Tree *modTree,
			   QuestionSet *qsP, int rootX, char *dummyName,
			   int* dummyX, char *rootIdentifier) {
    /*		   struct subTreeDesc *idA, int *idN) { */

    TreeNode   *node        = &(tree->list.itemA[rootX]);
    DistribSet *dssP        = (DistribSet*)(tree->mdsP.cd);
    Codebook   *cb          = &(dssP->cbsP->list.itemA[0]);
/*    double      modelASum   =  0.0;
    int         modelXAN    =  0;
    int*        modelXA     =  (int*)malloc(sizeof(int));
    int         exp         =  1; */
    int         modelX      = -1;
    Question   *qP;
    TreeNode   *newRoot, *newNode;
    char        newName[50], newRootName[50];
    int         cbX, newRootX, newNodeX;

    newNodeX = treeModalityAddNode (mainTree, node->name, node);
    newNode  = &(mainTree->list.itemA[newNodeX]);

    if ( treeTestModalityNode (tree, rootX, qsP) >= 0 ) {
	/* modelASum = treeModalityToXADFS (tree, rootX, modelXA, &modelXAN, &exp);*/

	/* look, if subtree exists */
	/*
	for (i=0; i<(*idN); i++) {
	    if ( idA[i].modelASum == modelASum ) { */

		/*
		printf ("\n    found tree (sum %i = %i) for %i at pos %i with "
			"modelX %i.\n", idA[i].modelASum, modelASum, rootX, i,
			idA[i].modelX);
		fflush(stdout);
		*/

		/* leaf of tree with same model as subtree, but with new name */
	/*
		sprintf(newName, "%s-%i", dummyName, *dummyX);
		modelX = idA[i].modelX;

		(*dummyX)++;
	    }
	} */


	/* subtree does not exists */
	if ( modelX < 0 ) {
	    sprintf(newName,     "%s-%i", dummyName,      *dummyX);
	    sprintf(newRootName, "%s-%s", rootIdentifier, newName);

	    newRootX      = treeCopyDFS (tree, modTree, rootX);
	    newRoot       = &(modTree->list.itemA[newRootX]);
	    /* new codebook and distribution */
	    cbX           = cbsAdd (dssP->cbsP, newName, cb->refN, cb->dimN,
				    cb->type, cb->featX);
	    modelX        = dssAdd (dssP, newName, cbX);

	    newRoot->name = strdup(newRootName);
	    
	    (*dummyX)++;

	    /* add new subtree to idA */
	    /*
	    (*idN)++;
	    if ( *idN >= TREE_MAXDESC ) {
		ERROR("Description array for unique subtrees to small (%i).\n",
		      TREE_MAXDESC);
	    } else {
		idA[*idN-1].modelASum = modelASum;
		idA[*idN-1].modelX    = modelX;
		idA[*idN-1].modelXA   = modelXA;
		idA[*idN-1].modelXAN  = modelXAN; */
		/*
		  printf("\n    Added %i, %i, ... to idA at pos %i.\n",
		  modelASum, modelX, *idN-1); fflush(stdout);
		*/
	    /*	    } */
	}

	newNode->name     =  strdup(newName);
	newNode->no       = -1;
	newNode->undef    = -1;
	newNode->yes      = -1;
	newNode->question =  questionSetAdd(&(mainTree->questions), strdup(""));
	newNode->model    =  modelX;
	newNode->ptree    = -1;
	newNode->mark     =  node->mark;

	return newNodeX;
    }

    qP                = &(tree->questions.list.itemA[node->question]);
    newNode->question = questionSetAdd(&(mainTree->questions), qP->name);

    if ( node->no    >= 0 )
	newNode->no    = treeModalityDivideDFS (tree, mainTree, modTree, qsP,
						node->no,    dummyName, dummyX,
						rootIdentifier);
    if ( node->undef >= 0 ) {
	if ( node->undef != node->no )
	    newNode->undef = treeModalityDivideDFS (tree, mainTree, modTree, qsP,
						    node->undef, dummyName, dummyX,
						    rootIdentifier);
	else newNode->undef = newNode->no;
    }
	
    if ( node->yes   >= 0 ) {
	if ( node->yes != node->no )
	    newNode->yes   = treeModalityDivideDFS (tree, mainTree, modTree, qsP,
						    node->yes,   dummyName, dummyX,
						    rootIdentifier);
	else newNode->yes = newNode->no;
    }

    return newNodeX;
}


int treeModalityMarkDFS (Tree *tree, QuestionSet *qsP, int rootX, int mark,
			 int modMark, int modFound) {

    TreeNode *node;

    if ( rootX < 0 ) return 1;

    node = &(tree->list.itemA[rootX]);

    if ( treeTestModalityNode (tree, rootX, qsP) >= 0 ) {
	node->mark = modMark;
	modFound   = 1;
    } else {
	node->mark = mark;
    }

    treeModalityMarkDFS (tree, qsP, node->no,    mark, modMark, modFound);
    treeModalityMarkDFS (tree, qsP, node->undef, mark, modMark, modFound);
    treeModalityMarkDFS (tree, qsP, node->yes,   mark, modMark, modFound);

    return 1;
}


/* copies a subtree into another tree given by rootX
   returns index of rootX in new tree */
int treeCopyDFS (Tree *tree, Tree *modTree, int rootX) {

    TreeNode *node    = &(tree->list.itemA[rootX]);
    TreeNode *newNode;
    Question *qP;
    int newNodeX;

    newNodeX          = treeModalityAddNode (modTree, node->name, node);
    newNode           = &(modTree->list.itemA[newNodeX]);
    qP                = &(tree->questions.list.itemA[node->question]);
    newNode->question = questionSetAdd(&(modTree->questions), qP->name);

    if ( node->no    >= 0 )
	newNode->no    = treeCopyDFS (tree, modTree, node->no);
    if ( node->undef >= 0 )
	newNode->undef = treeCopyDFS (tree, modTree, node->undef);
    if ( node->yes   >= 0 )
	newNode->yes   = treeCopyDFS (tree, modTree, node->yes);

    return newNodeX;
}


/* dublicates a subtree (root must have a unique name)
   return index of root */
int treeDublicateDFS (Tree *tree, int rootX, int setOrg) {

    TreeNode *node;

    if ( setOrg || rootX < 0 ) {
	return rootX;
    }

    rootX = treeNewNodeX (tree, rootX, setOrg);
    node  = &(tree->list.itemA[rootX]);
    /* printf(" [%i, %i, %i]", rootX, node->no, node->yes); fflush(stdout);*/

    node->no    = treeDublicateDFS (tree, node->no,    setOrg);
    node->undef = treeDublicateDFS (tree, node->undef, setOrg);
    node->yes   = treeDublicateDFS (tree, node->yes,   setOrg);

    return rootX;
}


/* add a new node to the tree with a unique name
   return index */
static int treeNewNodeX (Tree *tree, int nodeX, int setOrg) {

    TreeNode *node;
    char      name[50];
    char      baseName[50];
    int       newNodeX, i, idx;
    char      c, *n;

    assert (nodeX >= 0 && nodeX < tree->list.itemN);

    node = &(tree->list.itemA[nodeX]);

    if ( setOrg ) {
	return nodeX;
    }

    strcpy (baseName, node->name);
    strcpy (name, node->name);
    n        = strrchr(name, '-');
    i        = 0;
    if ( n ) {
	idx  = n - &name[0] + 1;
	c    = *(n+1);
	/* printf("\n  %s, %s, %i, %c", name, n, idx, c); */

	if ( isdigit((int)c) ) {
	    baseName[idx-1] = '\0';
	    i               = atoi (n+1);
	    sprintf (name, "%s-%i", baseName, i);
	}
    }

    /*
      printf("\n    %s, %s, %s, %i ", node->name, baseName, name, i); fflush(stdout);
      */

    while ( listIndex ((List*)&(tree->list), name, 0) >= 0 ) {
	i++;
	sprintf (name, "%s-%i", baseName, i);
    }
    /* printf("-> %s, %s> ", name, strdup(name)); */
    newNodeX = treeModalityAddNode (tree, name, node);
    /* printf("-> %s> ", name); */
    return newNodeX;
}


/* test, wether node includes a modality question
   returns -1 if it is not or >= 0 if it is*/
static int treeTestModalityNode (Tree *tree, int nodeX, QuestionSet *qsP) {

    TreeNode *node = &(tree->list.itemA[nodeX]);
    Question *qP   = &(tree->questions.list.itemA[node->question]);

    return listIndex ((List*)&(qsP->list), qP->name, 0);
}


/* returns  1 if node is     in tree and is     a leaf
 * returns -1 if node is not in tree
 * returns  0 if node is     in tree and is not a leaf */
static int treeIsLeaf (Tree *tree, int nodeX) {

    TreeNode *node;

    if ( nodeX == -1 ) {

	return -1;
    } else {

	node = &(tree->list.itemA[nodeX]);
	return ((node->no == -1) && (node->yes == -1) && (node->undef == -1));
    }
}


static int treeModalityAddNode (Tree *tree, char *name, TreeNode* features) {

    int       nodeX = listIndex((List*)&(tree->list), name, 1);
    TreeNode *node;

    if ( nodeX < 0 ) {
	ERROR ("Can't add node %s to tree.\n", name);
	return -1;
    }

    node = tree->list.itemA + nodeX;

    node->question = features->question;
    node->yes      = features->yes;
    node->no       = features->no;
    node->undef    = features->undef;
    node->ptree    = features->ptree;
    node->model    = features->model;
    node->mark     = features->mark;

    return nodeX;
}


static double treeModalityToXADFS (Tree *tree, int rootX, int *modelA, int *size,
			    int* exp) {

    TreeNode   *node  = &(tree->list.itemA[rootX]);
    double      res   = 0.0;

    if ( node->model >= 0 ) {
	modelA = (int*)realloc(modelA, ++(*size) * sizeof(int));

	modelA[(*size)-1] = node->model;
	res += pow (log ((double)(node->model)),(double)(*exp));
	(*exp)++;
	/*printf ("%i*%i ", node->model, primA[*exp-1]);*/
    }

    if ( node->no    >= 0 )
	res += treeModalityToXADFS (tree, node->no,    modelA, size, exp);
    if ( node->undef >= 0 )
	res += treeModalityToXADFS (tree, node->undef, modelA, size, exp);
    if ( node->yes   >= 0 )
	res += treeModalityToXADFS (tree, node->yes,   modelA, size, exp);

    return res;
}

/*
static int treeModalityGetBaseX (Tree *tree, int nodeX) {
    
    TreeNode *node;
    char      baseName[50];
    int       i, idx;
    char      c, *n;

    if ( nodeX < 0 ) return -1;

    node = &(tree->list.itemA[nodeX]);

    strcpy (baseName, node->name);
    n        = strrchr(baseName, '-');
    i        = 0;
    if ( n ) {
	idx  = n - &baseName[0] + 1;
	c    = *(n+1);
	if ( isdigit((int)c) ) baseName[idx-1] = '\0';
    }

    return listIndex ((List*)&(tree->list), baseName, 0);
}
*/

/* ------------------------------------------------------------------------
    treeInsert
   ------------------------------------------------------------------------ */

int treeInsert (Tree* tree, char* father, char* where, char* name, char* question,
		char* model, int ptree) {

    int wIdx   = listIndex((List*)&(tree->list), where,  0);
    int nIdx   = listIndex((List*)&(tree->list), name,   1);
    int fIdx   = listIndex((List*)&(tree->list), father, 0);
    int quIdx  = questionSetAdd(&(tree->questions), question);

    TreeNode* wNode = (wIdx >= 0) ? &(tree->list.itemA[wIdx]) : NULL;
    TreeNode* nNode = (nIdx >= 0) ? &(tree->list.itemA[nIdx]) : NULL;
    TreeNode* fNode = (fIdx >= 0) ? &(tree->list.itemA[fIdx]) : NULL;

    if ( !wNode || !nNode || !fNode ) {
	ERROR ("Can't insert node %s in tree %s at %s after %s.\n",
	       name, tree->name, where, father);
	/* if ( nIdx >= 0 ) listDelete ((List*)&(tree->list), (ClientData*)name); */
	return -1;
    }

    nNode->question = quIdx;
    nNode->yes      = wIdx;
    nNode->no       = wIdx;
    nNode->undef    = -1;
    nNode->ptree    = ptree;

    if ( fNode->no    == wIdx ) fNode->no    = nIdx;
    if ( fNode->yes   == wIdx ) fNode->yes   = nIdx;
    if ( fNode->undef == wIdx ) fNode->undef = nIdx;
    
    if ( !strcmp(model,"-") ) nNode->model = -1;
    else {
	if ( tree->mdsP.modelSetP->indexFct ) {
	    nNode->model = tree->mdsP.modelSetP->indexFct (tree->mdsP.cd, model);
	    if ( nNode->model < 0 ) WARN("Can't find model %s.\n", model); 
	}
	else nNode->model = atoi(model);
    }
    return nIdx;
}


/* ========================================================================
    treeTransform
   ======================================================================== */

/* entry point: transform a tree with modality question */
int treeTransform (Tree *tree, Tree *mainTree, Tree *modTree,
		   QuestionSet *qsP, char *dummyName, char *rootIdentifier,
		   int divide)
{
    int  *rootNodeXA = NULL;
    int   rootNodeN  = 0;
    int   test       = 0;
    int   changes    = 0;
    /* int   i; */

    printf("  Search root nodes ........................ ");
    rootNodeXA = treeGetRootNodes (tree, &rootNodeN, rootIdentifier);
    printf("%i root nodes found, %i nodes in tree.\n", rootNodeN, tree->list.itemN);
    /*
      for (i=0; i<rootNodeN; i++) printf(" %i,",rootNodeXA[i]);
      printf("\n");
      */

    printf("  Split modality AND nodes ................. "); fflush(stdout);
    changes    = treeSplitAndNodes (tree, qsP);
    printf("%i nodes splitted.\n", changes);

    printf("  Make tree deterministic .................. changes "); fflush(stdout);
    while ( (changes = treeMakeDeterministic (tree, qsP, rootNodeXA, rootNodeN)) )
	printf("%i ", changes); fflush(stdout);
    printf("%i done.\n", changes);

    changes = 0;
    printf("  Move nodes with modality questions ....... pass "); fflush(stdout);
    while ( !test ) {
	treeMoveModalityNodes   (tree, qsP, rootNodeXA, rootNodeN);
	test = treeModalityTest (tree, qsP, rootNodeXA, rootNodeN);
	changes++;
	printf("%i ", changes);
	if ( test < 0 ) return TCL_ERROR;
    }
    printf("done.\n");

    printf("  Make tree deterministic .................. changes "); fflush(stdout);
    while ( (changes = treeMakeDeterministic (tree, qsP, rootNodeXA, rootNodeN)) ) {
 	printf("%i ", changes); fflush(stdout);
    }
    printf("%i done.\n", changes);

    if ( !divide) return TCL_OK;

    printf("  Divide and split off modality subtrees ... "); fflush(stdout);
    treeModalityDivide (tree, mainTree, modTree, qsP, rootNodeXA, rootNodeN,
			dummyName, "ROOT");
    printf("%i nodes in %s and %i nodes in %s.\n", mainTree->list.itemN,
	   mainTree->name, modTree->list.itemN, modTree->name); fflush(stdout);

    /* clean up */
    free (rootNodeXA); rootNodeXA = NULL;

    return TCL_OK;
}

/* ========================================================================
    treeTransformItf
   ======================================================================== */

int treeTransformItf (ClientData cd, int argc, char *argv[]) {

    Tree        *tree           = (Tree*)cd;
    Tree        *mainTree       =  NULL;
    Tree        *modTree        =  NULL;
    QuestionSet *qsP            =  NULL;
    char        *dummyName      = "dummyDs";
    char        *rootIdentifier = "ROOT";
    int          divide         =  1;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<tree>", ARGV_OBJECT, NULL, &tree, "Tree",
		                 "tree with modality questions",
		       "<mainTree>", ARGV_OBJECT, NULL, &mainTree, "Tree",
		                 "tree to add later the normal nodes",
		       "<modTree>", ARGV_OBJECT, NULL, &modTree, "Tree",
		                 "tree to add later the modality nodes",
		       "<questionSet>", ARGV_OBJECT, NULL, &qsP, "QuestionSet",
		                 "set of only modality questions",
		       "-dummyName", ARGV_STRING, NULL, &dummyName, NULL,
		                 "name for dummy distributions",
		       "-rootIdentifier", ARGV_STRING, NULL, &rootIdentifier, NULL,
		                 "string with rootIdentifiers separated by space",
		       "-divide", ARGV_INT, NULL, &divide, NULL,
		                 "divide tree into subtrees",
		       NULL) != TCL_OK )
	return TCL_ERROR;

    return treeTransform (tree, mainTree, modTree, qsP, dummyName, rootIdentifier,
			  divide);
}
