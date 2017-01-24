/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Tree
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  tree.c
    Date    :  $Id: tree.c 3416 2011-03-23 03:02:18Z metze $
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
    Revision 4.7  2003/08/14 11:20:19  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 4.6.4.8  2003/08/11 14:28:36  soltau
    treeCluster : don't write to stderr

    Revision 4.6.4.7  2003/08/07 12:44:37  metze
    Fixed reallocation problem with treeAdd

    Revision 4.6.4.6  2003/08/06 17:22:19  soltau
    treeMergeSplits : fixed fread access for windows

    Revision 4.6.4.5  2003/07/03 11:39:35  soltau
    treeCluster: get more memory

    Revision 4.6.4.4  2003/07/02 17:49:27  fuegen
    fixes for file handling under Windows
    using general functions for file open/close

    Revision 4.6.4.3  2003/06/12 15:56:54  metze
    Now warn for bad padPhones

    Revision 4.6.4.2  2002/08/27 08:40:13  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 4.6.4.1  2002/02/28 15:46:33  soltau
    cleaned treeDeinit

    Revision 4.6  2001/01/15 09:50:00  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 4.5.4.1  2001/01/12 15:17:00  janus
    necessary changes for running janus under WINDOWS

    Revision 4.5  2000/11/14 12:35:26  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 4.4.34.2  2000/11/08 17:33:28  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 4.4.34.1  2000/11/06 10:50:41  janus
    Made changes to enable compilation under -Wall.

    Revision 4.4  2000/08/16 11:46:11  soltau
    Free -> TclFree

    Revision 4.3  2000/01/12 10:12:49  fuegen
    fast clustering procedure

    Revision 4.2  1999/03/10 18:47:09  fuegen
    treeTransform is now found in an extra module

    Revision 4.1  1999/02/24 18:59:03  fuegen
    treeTransform added

    Revision 4.0  1998/11/30 18:07:14  fuegen
    *** empty log message ***

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.27  96/09/23  15:59:25  rogina
    fixed file open/close stuff
    
    Revision 1.26  1996/06/10 11:32:11  finkem
    node parameters are now configurable

    Revision 1.25  1996/05/31  09:00:48  rogina
    allocated a bit more for linN when merging split files (it seems however,
    that we've always had a little buffering there by the malloc itself).

    Revision 1.24  1996/05/31  08:34:14  rogina
    fixed from/to cunfusion when writing aoutput file of merged splits

    Revision 1.23  1996/05/30  19:52:13  rogina
    added command "mergeSplits" for merging parallel cluster outputs

    Revision 1.22  1996/02/26  06:11:29  finkem
    modified findQuestion to give alarm if there are less than two
    successors to a node

    Revision 1.21  1996/02/17  19:25:21  finkem
    removed root index in the tree structure so that there is no longe
    a predefined root (backup for nodeX < 0)

    Revision 1.20  1996/02/11  06:19:57  finkem
    added clustering functions

    Revision 1.19  1996/01/31  05:01:20  finkem
    generic tree based on model sets
    inherent ptrees

    Revision 1.18  1996/01/12  16:17:33  finkem
    changed treeRead to freadAdd
    added ptree answers

    Revision 1.17  1995/11/14  08:13:57  finkem
    add list function

    Revision 1.16  1995/11/06  14:10:57  rogina
    *** empty log message ***

    Revision 1.15  1995/10/13  22:40:24  finkem
    treePhones/treeTags Pointer function added

    Revision 1.14  1995/09/21  21:39:43  finkem
    changed tree to have don't know branches, too, in order to model the
    same answers as the JANUS2 tree/search module.

    Revision 1.13  1995/09/06  07:43:37  kemp
    *** empty log message ***

    Revision 1.12  1995/08/27  22:34:20  finkem
    *** empty log message ***

    Revision 1.11  1995/08/18  08:22:35  finkem
    *** empty log message ***

    Revision 1.10  1995/08/17  17:10:12  rogina
    *** empty log message ***

    Revision 1.9  1995/08/14  18:52:32  rogina
    *** empty log message ***

    Revision 1.8  1995/08/14  13:19:35  finkem
    *** empty log message ***

    Revision 1.7  1995/08/03  14:41:26  finkem
    *** empty log message ***

    Revision 1.6  1995/08/01  22:15:27  finkem
    *** empty log message ***

    Revision 1.5  1995/08/01  13:27:54  finkem
    *** empty log message ***

    Revision 1.4  1995/07/31  14:34:25  finkem
    *** empty log message ***

    Revision 1.3  1995/07/25  22:33:21  finkem
    *** empty log message ***

    Revision 1.2  1995/07/25  14:45:47  rogina
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

char treeRCSVersion[]= "@(#)1$Id: tree.c 3416 2011-03-23 03:02:18Z metze $";

#include <limits.h>

#include "common.h"
#include "itf.h"
#include "error.h"
#include "array.h"
#include "tree.h"
#include "ptree.h"
#include "questions.h"
#include "word.h"
#include "treeTransform.h"

/* ========================================================================
    Global Declarations
   ======================================================================== */

extern TypeInfo treeNodeInfo;
extern TypeInfo treeInfo;

/* ========================================================================
    TreeNode
   ======================================================================== */
/* ------------------------------------------------------------------------
    treeNodeInit
   ------------------------------------------------------------------------ */

int treeNodeInit( TreeNode* TN, char* name)
{
  assert( TN);

  TN->name        =  strdup( name);
  TN->question    =  -1;
  TN->yes         =  -1;
  TN->no          =  -1;
  TN->undef       =  -1;
  TN->model       =  -1;
  TN->ptree       =  -1;
  TN->mark        =   0;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    treeNodeDeinit  deinitialize TreeNode structure
   ------------------------------------------------------------------------ */

int treeNodeDeinit( TreeNode* TN)
{
  assert( TN);
  if ( TN->name) free( TN->name);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    treeNodeConfigureItf
   ------------------------------------------------------------------------ */

static int treeNodeConfigureItf(ClientData cd, char *var, char *val)
{
  TreeNode *TN = (TreeNode*)cd;
  if (! TN) return TCL_ERROR;

  if (var==NULL) {

    ITFCFG(treeNodeConfigureItf,cd,"name");
    ITFCFG(treeNodeConfigureItf,cd,"question");
    ITFCFG(treeNodeConfigureItf,cd,"no");
    ITFCFG(treeNodeConfigureItf,cd,"yes");
    ITFCFG(treeNodeConfigureItf,cd,"undef");
    ITFCFG(treeNodeConfigureItf,cd,"model");
    ITFCFG(treeNodeConfigureItf,cd,"ptree");
    return TCL_OK;
  }
  ITFCFG_CharPtr(var,val,"name",     TN->name,     1);
  ITFCFG_Int(    var,val,"question", TN->question, 0);
  ITFCFG_Int(    var,val,"no",       TN->no,       0);
  ITFCFG_Int(    var,val,"yes",      TN->yes,      0);
  ITFCFG_Int(    var,val,"undef",    TN->undef,    0);
  ITFCFG_Int(    var,val,"model",    TN->model,    0);
  ITFCFG_Int(    var,val,"ptree",    TN->ptree,    0);

  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;   
}

/* ------------------------------------------------------------------------
    treeNodePuts
   ------------------------------------------------------------------------ */

static int treeNodePutsItf (ClientData clientData, int argc, char *argv[]) 
{
  TreeNode* treeNode = (TreeNode*)clientData;
  itfAppendResult("%s",treeNode->name);
  return TCL_OK;
}

/* ========================================================================
    Tree
   ======================================================================== */

static Tree treeDefault;

/* ------------------------------------------------------------------------
    Create/Init Tree
   ------------------------------------------------------------------------ */

int treeInit( Tree* tree, char* name,  Phones* phones, PhonesSet* phonesSet, 
              Tags* tags, ModelSetPtr* mdsP)
{
  int retCode = TCL_OK;

  assert( tree && name && phones && phonesSet && tags && mdsP);

  tree->name         = strdup(name);
  tree->useN         = 0;

  listInit((List*)&(tree->list), &treeNodeInfo, sizeof(TreeNode),
                    treeDefault.list.blkSize);

  tree->list.init    = (ListItemInit  *)treeNodeInit;
  tree->list.deinit  = (ListItemDeinit*)treeNodeDeinit;

  /* Initialize ModelSet array */

  tree->mdsP.cd        = mdsP->cd;
  tree->mdsP.modelSetP = mdsP->modelSetP;

  tree->commentChar    = treeDefault.commentChar;

  retCode = questionSetInit(&(tree->questions), name, phones, phonesSet, tags);
  link(&(tree->questions),"QuestionSet");

  ptreeSetInit(&(tree->ptreeSet), name, phones, tags, mdsP);
  link(&(tree->ptreeSet), "PTreeSet");

  tree->ptreeAdd     =  0;

  return retCode;
}

Tree *treeCreate( char *name, Phones*   phones, PhonesSet* phonesSet, Tags* tags,
                              ModelSetPtr* mdsP)
{
  Tree *tree = (Tree*)malloc(sizeof(Tree));

  if (! tree || treeInit( tree, name, phones, 
                          phonesSet, tags, mdsP) != TCL_OK) {
     if ( tree) free( tree);
     ERROR("Failed to allocate Tree object '%s'.\n", name);
     return NULL; 
  }
  return tree;
}

static ClientData treeCreateItf( ClientData clientData, 
                                 int argc, char *argv[]) 
{
  Tree*         treeP     = NULL;
  Phones*       phones    = NULL;
  PhonesSet*    phonesSet = NULL;
  Tags*         tags      = NULL;
  char*         name      = NULL;
  int           padPhoneX = -1;
  ModelSetPtr   mdsP;

  modelSetPtrInit( &mdsP);

  if ( itfParseArgv( argv[0], &argc, argv, 0,
      "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the tree",
      "<phones>",   ARGV_OBJECT,NULL,&phones,   "Phones",   "set of phones",
      "<phonesSet>",ARGV_OBJECT,NULL,&phonesSet,"PhonesSet","set of phone set",
      "<tags>",     ARGV_OBJECT,NULL,&tags,     "Tags",     "set of tags",
      "<modelSet>", ARGV_CUSTOM,getModelSetPtr,&mdsP,NULL,"model set",
      "-padPhone",  ARGV_INT,   NULL,&padPhoneX,NULL,"padding phone index",
       NULL) != TCL_OK) {

    modelSetPtrDeinit( &mdsP);
    return NULL;
  }
  if ( (treeP = treeCreate(name,phones,phonesSet,tags,&mdsP)) ) {
    treeP->questions.padPhoneX = padPhoneX;
  }
  return (ClientData)treeP;
}

/* ------------------------------------------------------------------------
    Free Tree
   ------------------------------------------------------------------------ */

int treeLinkN( Tree* tree)
{
  int  luseN =  listLinkN((List*)&(tree->list))      - 1;
  int  quseN =  questionSetLinkN(  &(tree->questions)) - 1;
  int  useN  = (luseN < quseN) ? quseN : luseN;

  if ( useN < tree->useN) return tree->useN;
  else                    return useN;
}

int treeDeinit( Tree* tree)
{
  int lrc, qrc, prc;

  if ( treeLinkN( tree)) {
    SERROR("Tree '%s' still in use by other objects.\n", tree->name);
    return TCL_ERROR;
  }
  if ( tree->name) { free( tree->name); tree->name = NULL; }
  
  delink( &(tree->questions), "QuestionSet");

  lrc = listDeinit((List*)&(tree->list));
  qrc = questionSetDeinit(  &(tree->questions));
  prc = ptreeSetDeinit(&(tree->ptreeSet));

  modelSetPtrDeinit(&(tree->mdsP));

  return (lrc != TCL_OK || qrc != TCL_OK || prc != TCL_OK) ? TCL_ERROR : TCL_OK;
}

int treeFree( Tree* tree)
{
  if ( treeDeinit( tree) != TCL_OK) return TCL_ERROR;
  free(tree);
  return TCL_OK;
}

static int treeFreeItf (ClientData clientData)
{
  Tree* tree = (Tree*)clientData;
  return treeFree( tree);
}

/* ------------------------------------------------------------------------
    treeConfigureItf
   ------------------------------------------------------------------------ */

int treeConfigureItf(ClientData cd, char *var, char *val)
{
  Tree*        TR = (Tree*)cd;
  QuestionSet* QU =  NULL;

  if (! TR) { TR = &treeDefault; QU = NULL; }
  else        QU = &(TR->questions);

  if (! var) {
    ITFCFG(treeConfigureItf,cd,"name");
    ITFCFG(treeConfigureItf,cd,"useN");
    if ( QU) {
      ITFCFG(treeConfigureItf,cd,"phones");
      ITFCFG(treeConfigureItf,cd,"phonesSet");
      ITFCFG(treeConfigureItf,cd,"tags");
      ITFCFG(treeConfigureItf,cd,"padPhone");
    }
    ITFCFG(treeConfigureItf,cd,"ptreeAdd");
    ITFCFG(treeConfigureItf,cd,"commentChar");
    return listConfigureItf((ClientData)&(TR->list), var, val);
  }
  ITFCFG_CharPtr(var,val,"name",         TR->name,                     1);
  ITFCFG_Int(    var,val,"useN",         TR->useN,                     1);
  if ( QU) {
    ITFCFG_Int(    var,val,"padPhone",     QU->padPhoneX,                0);
    ITFCFG_Object( var,val,"phones",       QU->phones,   name,Phones,    1);
    ITFCFG_Object( var,val,"phonesSet",    QU->phonesSet,name,PhonesSet, 1);
    ITFCFG_Object( var,val,"tags",         QU->tags,     name,Tags,      1);
  }
  ITFCFG_Int(    var,val,"ptreeAdd",     TR->ptreeAdd,                 0);
  ITFCFG_Char(   var,val,"commentChar",  TR->commentChar,              0);
  return listConfigureItf((ClientData)&(TR->list), var, val);   
}

/* ------------------------------------------------------------------------
    treeAccessItf
   ------------------------------------------------------------------------ */

ClientData treeAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  Tree* tree = (Tree*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
       itfAppendElement( "modelSet");
       itfAppendElement( "questionSet");
       itfAppendElement( "ptreeSet");
       *ti = NULL;
    }
    else { 
      if (! strcmp( name+1, "modelSet")) {
        *ti = tree->mdsP.modelSetP->tiP;
        return (ClientData)(tree->mdsP.cd);
      }
      else if (! strcmp( name+1, "questionSet")) {
        *ti = itfGetType("QuestionSet");
        return (ClientData)&(tree->questions);
      }
      else if (! strcmp( name+1, "ptreeSet")) {
        *ti = itfGetType("PTreeSet");
        return (ClientData)&(tree->ptreeSet);
      }
    }
  }
  return listAccessItf((ClientData)&(tree->list),name,ti);
}

/* ------------------------------------------------------------------------
    treePuts
   ------------------------------------------------------------------------ */

int treePutsItf (ClientData clientData, int argc, char *argv[]) 
{
  Tree* tree = (Tree*)clientData;
  return listPutsItf((ClientData)&(tree->list), argc, argv);
}

/* ------------------------------------------------------------------------
    treeIndex
   ------------------------------------------------------------------------ */

int treeIndex( Tree* tree,  char* name) 
{
  return listIndex((List*)&(tree->list), name, 0);
}

int treeIndexItf( ClientData clientData, int argc, char *argv[] )
{
  Tree* tree = (Tree*)clientData;
  return listName2IndexItf((ClientData)&(tree->list), argc, argv);
}

/* ------------------------------------------------------------------------
    treeName
   ------------------------------------------------------------------------ */

char* treeName( Tree* tree, int i) 
{ 
  return (i < 0) ? "(null)" : tree->list.itemA[i].name;
}

int treeNameItf( ClientData clientData, int argc, char *argv[] )
{
  Tree* tree = (Tree*)clientData;
  return listIndex2NameItf((ClientData)&(tree->list), argc, argv);
}

/* ------------------------------------------------------------------------
    treeNumber/Phones
   ------------------------------------------------------------------------ */

int     treeNumber( Tree* tree) { return tree->list.itemN;       }
Phones* treePhones( Tree* tree) { return tree->questions.phones; }
Tags*   treeTags(   Tree* tree) { return tree->questions.tags;   }

/* ------------------------------------------------------------------------
    treeAdd
   ------------------------------------------------------------------------ */

int treeAdd( Tree* tree, char* name, char* question,
                         char* no,   char* yes, char* undef, 
                         char* model,int ptree)
{
  int       nidx   =    listIndex((List*)&(tree->list), name, 1);
  int       quIdx  =    questionSetAdd(&(tree->questions), question);
  int       noIdx  = (! strcmp(no,"-"))  ? -1 :
                        listIndex((List*)&(tree->list), no,   1);
  int       yesIdx = (! strcmp(yes,"-")) ? -1 :
                        listIndex((List*)&(tree->list), yes,  1);
  int       undIdx = (! strcmp(undef,"-")) ? -1 :
                        listIndex((List*)&(tree->list), undef,1);
  TreeNode* node   =    tree->list.itemA + nidx;

  node->question = quIdx;
  node->yes      = yesIdx;
  node->no       = noIdx;
  node->undef    = undIdx;
  node->ptree    = ptree;

  if (! strcmp(model,"-")) node->model = -1;
  else {
    if ( tree->mdsP.modelSetP->indexFct) {
      node->model = tree->mdsP.modelSetP->indexFct( 
                    tree->mdsP.cd, model);
      if ( node->model < 0) WARN("Can't find model %s.\n", model); 
    }
    else node->model = atoi(model);
  }
  return nidx;
}

int treeAddItf ( ClientData clientData, int argc, char *argv[] )
{
  Tree* tree     = (Tree*)clientData;
  char* node     =  NULL;
  char* yesNode  =  NULL;
  char* noNode   =  NULL;
  char* undNode  =  NULL;
  char* question =  NULL;
  char* model    =  NULL;
  int   ptree    = -1;
  int   ac       =  argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
      "<nodeName>",  ARGV_STRING,NULL,&node,    NULL,"name of the node",
      "<question>",  ARGV_STRING,NULL,&question,NULL,"question string",
      "<noNode>",    ARGV_STRING,NULL,&noNode,  NULL,"NO    successor node",
      "<yesNode>",   ARGV_STRING,NULL,&yesNode, NULL,"YES   successor node",
      "<undefNode>", ARGV_STRING,NULL,&undNode, NULL,"UNDEF successor node",
      "<model>",     ARGV_STRING,NULL,&model,   NULL,"name of the model",
      "-ptree",      ARGV_LINDEX,NULL,&ptree, &(tree->ptreeSet.list),
      "name of the ptree",
       NULL) != TCL_OK) return TCL_ERROR;

  return ( treeAdd( tree,  node, question, noNode, yesNode, undNode,
                    model, ptree) < 0) ? TCL_ERROR : TCL_OK;
}

/* ------------------------------------------------------------------------
    treeRead
   ------------------------------------------------------------------------ */

int treeReadItf ( ClientData clientData, int argc, char *argv[] )
{
  Tree*   treeP   = (Tree*)clientData;
  char   *fname   =  NULL;
  int     ac      =  argc-1, n;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<filename>",ARGV_STRING,NULL,&fname,NULL,
                  "name of tree file", NULL) != TCL_OK)
       return TCL_ERROR;

  itfAppendResult( "%d", n = freadAdd( clientData, fname, treeP->commentChar,
                                                          treeAddItf));
  return (n >= 0) ? TCL_OK : TCL_ERROR;
}

/* ------------------------------------------------------------------------
    treeWrite
   ------------------------------------------------------------------------ */

int treeWrite( Tree *tree, char *fileName)
{
  TreeNode* treeNode = tree->list.itemA;
  int       treeN    = tree->list.itemN;
  FILE     *fp;

  if (! (fp=fileOpen(fileName,"w"))) return TCL_ERROR;

  fprintf( fp, "%c -------------------------------------------------------\n",
                tree->commentChar);
  fprintf( fp, "%c  Name            : %s\n", tree->commentChar,
                tree->name);
  fprintf( fp, "%c  Type            : Tree\n", tree->commentChar);
  fprintf( fp, "%c  Number of Items : %d\n", tree->commentChar,
                tree->list.itemN);
  fprintf( fp, "%c  Date            : %s", tree->commentChar,
                dateString());
  fprintf( fp, "%c -------------------------------------------------------\n",
                tree->commentChar);

  for ( ; treeN > 0; treeN--, treeNode++) {
     fprintf(fp,"%-15s {%s} %s %s %s ",  treeNode->name,
                (treeNode->question  < 0 ) ? " " :
                 questionSetName(&(tree->questions), treeNode->question),
                (treeNode->no   < 0) ? "-" : treeName( tree, treeNode->no),
                (treeNode->yes  < 0) ? "-" : treeName( tree, treeNode->yes),
                (treeNode->undef< 0) ? "-" : treeName( tree, treeNode->undef));
     if ( treeNode->model < 0) fprintf(fp,"-");
     else {
       if ( tree->mdsP.modelSetP->nameFct) 
            fprintf(fp,"%s",tree->mdsP.modelSetP->nameFct(
                            tree->mdsP.cd,
                            treeNode->model));
       else fprintf(fp,"%d",treeNode->model);
     }
     if ( treeNode->ptree > -1) {
       fprintf(fp," -ptree %s",tree->ptreeSet.list.itemA[treeNode->ptree].name);
     }
     fprintf(fp,"\n");
  }
  fileClose( fileName, fp);
  return TCL_OK;
}

int treeWriteItf (ClientData clientData, int argc, char *argv[] )
{
  char   *fname   =  NULL;
  int     ac      =  argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<filename>",ARGV_STRING,NULL,&fname,NULL,
                  "name of tree file", NULL) != TCL_OK)
    return TCL_ERROR;

  return treeWrite((Tree*)clientData, fname);
}

/* ------------------------------------------------------------------------
    treeGetModel
   ------------------------------------------------------------------------ */

static int treeGetModel_DidWarn = 0;

int treeGetModel( Tree* tree, int nodeX, Word* wP, int left, int right)
{
  int answ;

  /* For CI systems, the padPhone doesn't matter (so you can
     forget about this warning)
     For CD systems, you probably want to configure the -padPhone
     to some sensible value (i.e. @) in order to answer your questions
     properly */
  if (!treeGetModel_DidWarn && tree->questions.padPhoneX == -1) {
    treeGetModel_DidWarn = 1;
    INFO ("treeGetModel: configure -padPhone for '%s'?\n", tree->name);
  }

  assert (nodeX >= 0);
  while ( nodeX >= 0 && tree->list.itemA[nodeX].question >= 0) {

    answ = questionSetAnswer(&(tree->questions), 
                               tree->list.itemA[nodeX].question, 
                               wP, left, right);

    if (answ == ( 1) && tree->list.itemA[nodeX].yes >= 0) { 
        nodeX = tree->list.itemA[nodeX].yes;    continue; }
    else if (answ == (-1) && tree->list.itemA[nodeX].no >= 0) { 
        nodeX = tree->list.itemA[nodeX].no;     continue; }
    else if (answ == ( 0) && tree->list.itemA[nodeX].undef >= 0) { 
        nodeX = tree->list.itemA[nodeX].undef;  continue; }
    else break;
  }
  if ( nodeX >= 0) {
    if ( tree->list.itemA[nodeX].ptree >= 0) {
      PTree*     ptree =  tree->ptreeSet.list.itemA + tree->list.itemA[nodeX].ptree;
      int        idx   = -1;

      if (tree->ptreeAdd) 
	ptreeAdd( ptree, wP, left, right, 1, -1,&idx);
      else
	ptreeGet( ptree, wP, left, right,       &idx);

      if ( idx > -1) return idx;
      else           return tree->list.itemA[nodeX].model; 
    }
    else return tree->list.itemA[nodeX].model;
  }
  return -2;
}

int treeGetNode( Tree *tree, int nodeX, Word* wP, int left, int right)
{
  int answ;
  assert (nodeX >= 0);
  while ( nodeX >= 0 && tree->list.itemA[nodeX].question >= 0) {

    answ = questionSetAnswer(&(tree->questions), 
                               tree->list.itemA[nodeX].question, 
                               wP, left, right);

    if (answ == ( 1) && tree->list.itemA[nodeX].yes >= 0) { 
        nodeX = tree->list.itemA[nodeX].yes;    continue; }
    else if (answ == (-1) && tree->list.itemA[nodeX].no >= 0) { 
        nodeX = tree->list.itemA[nodeX].no;     continue; }
    else if (answ == ( 0) && tree->list.itemA[nodeX].undef >= 0) { 
        nodeX = tree->list.itemA[nodeX].undef;  continue; }
    else break;
  }
  return nodeX;
}

int treeGetModelItf( ClientData clientData, int argc, char *argv[])
{
  Tree*  tree   = (Tree*)clientData;
  int    nodeX  = -1;
  int    left   = -1, right = 0;
  int    ac     = argc - 1;
  int    getNode= 0;
  int    leafX  = -1;
  Word   word;

  wordInit( &word, tree->questions.phones, tree->questions.tags);

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<node>",         ARGV_LINDEX, NULL,&nodeX, &(tree->list), "root node",
      "<tagged phones>",ARGV_CUSTOM, getWord, &word,NULL,
      "list of tagged phones",
      "<leftContext>",  ARGV_INT, NULL, &left,  NULL, "left  context",
      "<rightContext>", ARGV_INT, NULL, &right, NULL, "right context",
      "-node",          ARGV_INT, NULL, &getNode,NULL,"want node name (0/1)",
       NULL) != TCL_OK) {
    wordDeinit( &word);
    return TCL_ERROR;
  }

  if ((leafX = treeGetNode(tree,nodeX,&word,left,right)) > -1) {
    int model = tree->list.itemA[leafX].model;

    if ( tree->list.itemA[leafX].ptree >= 0) {
      int        idx   = -1;
      PTree*     ptree =  tree->ptreeSet.list.itemA + tree->list.itemA[leafX].ptree;

      if (tree->ptreeAdd)
	ptreeAdd( ptree, &word, left, right, 1, -1,&idx);
      else
	ptreeGet( ptree, &word, left, right, &idx);

      if ( idx > -1) model = idx;
    }
    itfResetResult();
    itfAppendResult("%d", model);
    if (getNode) itfAppendResult(" %s",tree->list.itemA[leafX].name);
  }
  wordDeinit( &word);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    treeList
   ------------------------------------------------------------------------ */

int treeList( Tree *tree)
{
  TreeNode* treeNode = tree->list.itemA;
  int       treeN    = tree->list.itemN;

  for ( ; treeN > 0; treeN--, treeNode++) {
     itfAppendResult("{%s {%s} %s %s %s ",  treeNode->name,
                (treeNode->question  < 0 ) ? " " :
                 questionSetName(&(tree->questions), treeNode->question),
                (treeNode->no   < 0) ? "-" : treeName( tree, treeNode->no),
                (treeNode->yes  < 0) ? "-" : treeName( tree, treeNode->yes),
                (treeNode->undef< 0) ? "-" : treeName( tree, treeNode->undef));
     if ( treeNode->model < 0) itfAppendResult("-");
     else {
       if ( tree->mdsP.modelSetP->nameFct) 
            itfAppendResult("%s",tree->mdsP.modelSetP->nameFct(
                                 tree->mdsP.cd,
                                 treeNode->model));
       else itfAppendResult("%d",treeNode->model);
     }
     if ( treeNode->ptree > -1) {
       itfAppendResult(" -ptree %s",tree->ptreeSet.list.itemA[treeNode->ptree].name);
     }
     itfAppendResult("}\n");
  }
  return TCL_OK;
}

int treeListItf (ClientData clientData, int argc, char *argv[] )
{
  int ac = argc - 1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0, NULL) != TCL_OK) 
       return TCL_ERROR;

  return treeList((Tree*)clientData);
}

/* ------------------------------------------------------------------------
    treeTraceModel
   ------------------------------------------------------------------------ */

int treeTraceModel( Tree *tree, int nodeX, Word* wP, int  left, int  right)
{
  int answ;

  while ( nodeX >= 0 && tree->list.itemA[nodeX].question >= 0) {

    answ = questionSetAnswer(&(tree->questions), 
                               tree->list.itemA[nodeX].question, 
                               wP, left, right);

    INFO("from node %6d ( %16s %20s ) into %3s-successor %d\n", 
		     nodeX, tree->list.itemA[nodeX].name,
		     questionSetName(&(tree->questions),
                     tree->list.itemA[nodeX].question),
                     answ==1 ? "yes" : (answ==0 ? "don't-know-" : "no"), 
		     answ==1 ?  tree->list.itemA[nodeX].yes : 
                               (answ==0 ? nodeX : 
                                tree->list.itemA[nodeX].no));

    if (answ == ( 1) && tree->list.itemA[nodeX].yes >= 0) { 
        nodeX = tree->list.itemA[nodeX].yes;    continue; }
    else if (answ == (-1) && tree->list.itemA[nodeX].no  >= 0) { 
        nodeX = tree->list.itemA[nodeX].no;     continue; }
    else if (answ == ( 0) && tree->list.itemA[nodeX].undef >= 0) { 
        nodeX = tree->list.itemA[nodeX].undef;  continue; }
    else break;
  }
  return nodeX >= 0 ? tree->list.itemA[nodeX].model : -2;
}

int treeTraceItf( ClientData clientData, int argc, char *argv[])
{
  Tree*  tree    = (Tree*)clientData;
  int    nodeX   = -1;
  int    left    = -1, right = 0;
  int    ac      = argc - 1;
  Word   word;
  int    leafX;
  int    getNode = 0;;

  wordInit( &word, tree->questions.phones, tree->questions.tags);

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<node>",         ARGV_LINDEX, NULL,&nodeX, &(tree->list), "root node",
      "<tagged phones>",ARGV_CUSTOM, getWord, &word,NULL,
      "list of tagged phones",
      "<leftContext>",  ARGV_INT, NULL, &left,  NULL, "left  context",
      "<rightContext>", ARGV_INT, NULL, &right, NULL, "right context",
      "-node",          ARGV_INT, NULL, &getNode,NULL,"want node name (0/1)",
       NULL) != TCL_OK) {
    wordDeinit( &word);
    return TCL_ERROR;
  }

  treeTraceModel( tree, nodeX, &word, left, right);
  if ((leafX = treeGetNode(tree,nodeX,&word,left,right)) > -1) {
    int model = tree->list.itemA[leafX].model;

    if ( tree->list.itemA[leafX].ptree >= 0) {
      int        idx   = -1;
      PTree*     ptree =  tree->ptreeSet.list.itemA + tree->list.itemA[leafX].ptree;

      if (tree->ptreeAdd)
	ptreeAdd( ptree, &word, left, right, 1, -1,&idx);
      else
	ptreeGet( ptree, &word, left, right, &idx);

      if ( idx > -1) model = idx;
    }
    itfResetResult();
    itfAppendResult("%d", model);
    if (getNode) itfAppendResult(" %s",tree->list.itemA[leafX].name);
  }
  wordDeinit( &word);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    treeFindQuestion   finds the best question to split a given node
   ------------------------------------------------------------------------ */

int treeFindQuestion( Tree *tree, QuestionSet* qsetP, int nodeX, 
                      float minCount, float* score)
{
  TreeNode*    TN    =  tree->list.itemA + nodeX;
  QuestionSet* QS    = (qsetP) ? qsetP : &(tree->questions);

  if ( QS->phones != tree->questions.phones ||
       QS->tags   != tree->questions.tags) return -1;

  if ( TN->ptree < 0) return -1;

  if ( TN->yes > -1 || TN->no > -1 || TN->undef > -1 ) return -1;

  return ptreeFindQuestion( tree->ptreeSet.list.itemA+TN->ptree,
                            QS, minCount, score);
}


int treeFindQuestionFast (Tree *tree, QuestionSet* qsetP, int nodeX, 
			  float minCount, float* score, int *excludeA)
{
  TreeNode*    TN    =  tree->list.itemA + nodeX;
  QuestionSet* QS    = (qsetP) ? qsetP : &(tree->questions);

  if ( QS->phones != tree->questions.phones ||
       QS->tags   != tree->questions.tags) return -1;

  if ( TN->ptree < 0) return -1;

  if ( TN->yes > -1 || TN->no > -1 || TN->undef > -1 ) return -1;

  return ptreeFindQuestionFast (tree->ptreeSet.list.itemA+TN->ptree,
				QS, minCount, score, excludeA);
}

int treeFindQuestionItf( ClientData clientData, int argc, char *argv[] )
{
  Tree*        treeP    =  (Tree*)clientData;
  QuestionSet* qsetP    = &(treeP->questions);
  int          ac       =   argc - 1;
  int          nodeX    =  -1;
  float        minCount =   0;
  int          idx      =  -1;
  float        score    =   0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<node>",      ARGV_LINDEX, NULL, &nodeX, &(treeP->list), "root node",
      "-questionSet",ARGV_OBJECT, NULL, &qsetP, "QuestionSet", "question set", 
      "-minCount",   ARGV_FLOAT,  NULL, &minCount, NULL, "minimum count",
       NULL) != TCL_OK)  return TCL_ERROR;

  if ((idx = treeFindQuestion( treeP, qsetP, nodeX, minCount, &score)) > -1) {
    itfAppendResult("{%s} %e", qsetP->list.itemA[idx].name, score);
    return TCL_OK;
  }
  else {
    itfAppendResult("{} 0.0");
    return TCL_OK;
  }
}

/* ------------------------------------------------------------------------
    treeSplitNode
   ------------------------------------------------------------------------ */

int treeSplitNode( Tree *tree, int nodeX, char* questS, 
                   char* nnodeS, char* ynodeS, char* unodeS, int minCount)
{
  TreeNode*  TN     =  tree->list.itemA + nodeX;
  PTree     *yesP   =  NULL, *noP = NULL, *unkP = NULL;
  int        qidx   = -1;
  int        pptree = -1;
  int        n      =  0;
  ModelArray MA;

  if ( TN->ptree < 0) {
    ERROR("No ptree defined in node %s.\n", TN->name);
    return -1;
  }
  if ( TN->yes > -1 || TN->no > -1 || TN->undef > -1 ) {
    ERROR("Can't split node %s since successors are defined.\n", TN->name);
    return -1;
  }

  if ((qidx = questionSetAdd(&(tree->questions), questS)) < 0)
       return -1;

  ptreeSplit(   tree->ptreeSet.list.itemA+tree->list.itemA[nodeX].ptree, 
                minCount, 
                tree->questions.list.itemA + qidx, &noP, &yesP, &unkP);

  if ((!noP && !yesP) || (!noP && !unkP) || (!yesP && !unkP)) {

    if ( noP)  { ptreeFree( noP);  noP  = NULL; }
    if ( yesP) { ptreeFree( yesP); yesP = NULL; }
    if ( unkP) { ptreeFree( unkP); unkP = NULL; }    

    ERROR("Node %s has too few sucessors.\n", TN->name);
    return -1;
  }

  ptreeDealloc( tree->ptreeSet.list.itemA+tree->list.itemA[nodeX].ptree);

  tree->list.itemA[nodeX].question =  qidx;

  modelArrayInit( &MA, &(tree->mdsP));

  /* Create the new nodes for the children when there is a polyphonic
     tree (for yes, no and unknown). If the polyphonic tree contains
     real models (checked by calling ptreeModels) than the child is
     allocated and the polyphonic tree is merged into the tree of that
     child. Note, the way it is coded the child might already be there,
     but we check above, that this will never be true. Of course, we might
     some day decide to cluster graphs instead of trees.

     If there is no model in the polyphonic tree the parent node will
     store the tree and no child will be allocated. */

  if ( noP) {
    modelArrayClear( &MA);
    ptreeModels( noP, minCount, &MA);
    if ( MA.modelN) {
      int  ptree    = (tree->list.itemA[nodeX].no  < 0) ? -1 : 
                       tree->list.itemA[tree->list.itemA[nodeX].no].ptree;
      if ( ptree    <  0) ptree = ptreeSetAdd(&(tree->ptreeSet), nnodeS);
      if ( tree->list.itemA[nodeX].no   <  0) {
	   /* Note that the value of itemA can change in the call to treeAdd */
	   int ta =  treeAdd( tree, nnodeS, "", "-", "-", "-", "-", ptree);
	   tree->list.itemA[nodeX].no =  ta;
      }

      ptreeMerge( noP, minCount, tree->ptreeSet.list.itemA + ptree);
      n++;
    }
    else {
      ptreeMerge( noP, minCount, tree->ptreeSet.list.itemA + 
                                 tree->list.itemA[nodeX].ptree);
      pptree = tree->list.itemA[nodeX].ptree;
    }
  }
  if ( yesP) {
    modelArrayClear( &MA);
    ptreeModels( yesP, minCount, &MA);
    if ( MA.modelN) {
      int  ptree    = (tree->list.itemA[nodeX].yes < 0) ? -1 : 
                       tree->list.itemA[tree->list.itemA[nodeX].yes].ptree;
      if ( ptree    <  0) ptree = ptreeSetAdd(&(tree->ptreeSet), ynodeS);
      if ( tree->list.itemA[nodeX].yes  <  0) {
 	   /* Note that the value of itemA can change in the call to treeAdd */
           int ta =  treeAdd( tree, ynodeS, "", "-", "-", "-", "-", ptree);
           tree->list.itemA[nodeX].yes  =  ta;
      }

      ptreeMerge( yesP, minCount, tree->ptreeSet.list.itemA + ptree);
      n++;
    }
    else {
      ptreeMerge( yesP, minCount, tree->ptreeSet.list.itemA + 
                                  tree->list.itemA[nodeX].ptree);
      pptree = tree->list.itemA[nodeX].ptree;
    }
  }
  if ( unkP) {
    modelArrayClear( &MA);
    ptreeModels( unkP, minCount, &MA);
    if ( MA.modelN) {
      int  ptree     = (tree->list.itemA[nodeX].undef < 0) ? -1 : 
                        tree->list.itemA[tree->list.itemA[nodeX].undef].ptree;
      if ( ptree     <  0) ptree = ptreeSetAdd(&(tree->ptreeSet), unodeS);
      if ( tree->list.itemA[nodeX].undef <  0) {
	   /* Note that the value of itemA can change in the call to treeAdd */
	   int ta =  treeAdd( tree, unodeS, "", "-", "-", "-", "-", ptree);
           tree->list.itemA[nodeX].undef =  ta;
      }

      ptreeMerge( unkP, minCount, tree->ptreeSet.list.itemA + ptree);
      n++;
    }
    else {
      ptreeMerge( unkP, minCount, tree->ptreeSet.list.itemA + 
                                  tree->list.itemA[nodeX].ptree);
      pptree = tree->list.itemA[nodeX].ptree;
    }
  }

  if ( noP)  { ptreeFree( noP);  noP  = NULL; }
  if ( yesP) { ptreeFree( yesP); yesP = NULL; }
  if ( unkP) { ptreeFree( unkP); unkP = NULL; }    

  modelArrayDeinit( &MA);
  tree->list.itemA[nodeX].ptree    = pptree;

  if ( n < 2) { 
    ERROR("Node %d has only %d sucessor.\n", nodeX, n);
    return -1;
  }
  return nodeX;
}

int treeSplitNodeItf( ClientData clientData, int argc, char *argv[] )
{
  Tree* treeP    = (Tree*)clientData;
  int   ac       =  argc - 1;
  int   nodeX    = -1;
  int   minCount =  0;
  //int   idx      = -1;
  char* questS   =  NULL;
  char* ynodeS   =  "-";
  char* nnodeS   =  "-";
  char* unodeS   =  "-";

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<node>",      ARGV_LINDEX,NULL,&nodeX, &(treeP->list), "node",
      "<question>",  ARGV_STRING,NULL,&questS,  NULL,"question", 
      "<noNode>",    ARGV_STRING,NULL,&nnodeS,  NULL,"NO    successor node",
      "<yesNode>",   ARGV_STRING,NULL,&ynodeS,  NULL,"YES   successor node",
      "<undefNode>", ARGV_STRING,NULL,&unodeS,  NULL,"UNDEF successor node",
      "-minCount",   ARGV_FLOAT, NULL,&minCount,NULL,"minimum count",
       NULL) != TCL_OK)  return TCL_ERROR;

  if (treeSplitNode( treeP,  nodeX,  questS, 
                            nnodeS, ynodeS, unodeS, minCount) > -1) {
    return TCL_OK;
  }
  else {
    ERROR("Couldn't find a question to split node %s (%d).\n", 
           treeP->list.itemA[nodeX].name, nodeX);
    return TCL_ERROR;
  }
}

/* ------------------------------------------------------------------------
    treeCluster
    faster version of the tcl-implementation (with the same name)
   ------------------------------------------------------------------------ */

typedef struct {

    int    nodeX;     /* actual node */
    int    questionX; /* best splitting question for this node */
    float  score;     /* distance */
    float  count;     /* whole count of node */
    int   *excludeA;  /* handels all questions, which were sorted out because of
			 a count smaller than mincount in a father node OR
			 because it was the best splitting question in a father
			 node */

} TreeClusterS;


/* sort the treeCluster array in decreasing order of scores */
static int clustercompare (const void *a, const void *b) {

    TreeClusterS *i = (TreeClusterS*)a;
    TreeClusterS *j = (TreeClusterS*)b;

    if ( i->score > j->score ) return -1;
    if ( i->score < j->score ) return  1;

    return 0;
}

int treeCluster (Tree *treeP, int nodeX, QuestionSet *qsP, float minCount,
		 int maxSplit, float minScore, char *fileName, int verbose) {

    TreeClusterS *tcA       =  NULL, *tmpTcA;
    char         *rootName  =  treeP->list.itemA[nodeX].name;
    int           tcN       =  0;
    int           maxTcN    =  1003; /* default maxSplit+3 */
    int           nodeN     =  0;
    int           bestX     = -1;
    int           questionN =  qsP->list.itemN;
    int          *excludeA  =  NULL;
    FILE         *logFp     =  stdout; /* for verbose flag */
    FILE         *fp        =  fileName ? fileOpen (fileName, "w") : logFp;

    char          noNode[1024], yesNode[1024], undefNode[1024];
    float         score;
    int           qIdx, idxA[3], i;

    /* initialize arrays */
    tcA      = (TreeClusterS*)malloc (maxTcN * sizeof(TreeClusterS));
    excludeA = (int*)malloc (questionN * sizeof(int));
    for (i = 0; i < questionN; i++) excludeA[i] = 0;

    /* first findQuestion */
    qIdx = treeFindQuestionFast (treeP, qsP, nodeX, 0, &score, excludeA);

    if ( qIdx >= 0 ) {
	int ptreeX = treeP->list.itemA[nodeX].ptree;

	if ( verbose ) {
	    fprintf (logFp, "%s question %s -questionSet %s: %s (%e)\n",
		     treeP->name, treeP->list.itemA[nodeX].name, qsP->name,
		     questionSetName (qsP, qIdx), score);
	    fflush(logFp);
	}

	tcA[tcN].nodeX     = nodeX;
	tcA[tcN].questionX = qIdx;
	tcA[tcN].score     = score;
	tcA[tcN].count     = treeP->ptreeSet.list.itemA[ptreeX].count;

	/* save the array in the father */
	tcA[tcN].excludeA  = (int*)malloc (questionN * sizeof(int));
	memcpy (tcA[tcN].excludeA, excludeA, questionN * sizeof(int));

	tcN++;
    } else if ( verbose ) {
	fprintf (logFp, "%s question %s -questionSet %s:  (0.0)\n",
		 treeP->name, treeP->list.itemA[nodeX].name, qsP->name);
	fflush(logFp);
    }

    /* while nodes exists */
    while ( tcN - bestX - 1 > 0 ) {

	/* split all nodes with the same (best) score */
	do {
	    bestX++;

	    if ( tcA[bestX].questionX >= 0        &&
		 tcA[bestX].count     >  minCount &&
		 nodeN                <  maxSplit ) {
		int   nodeX     = tcA[bestX].nodeX;
		char *question  = questionSetName (qsP, tcA[bestX].questionX);

		sprintf (noNode,    "%s(%i)", rootName, nodeN++);
		sprintf (yesNode,   "%s(%i)", rootName, nodeN++);
		sprintf (undefNode, "%s(%i)", rootName, nodeN++);

		if ( verbose ) {
		    fprintf (logFp, "%s split %s {%s} %s %s %s\n", treeP->name,
			     treeP->list.itemA[nodeX].name, question,
			     noNode, yesNode, undefNode);
		    fflush(logFp);
		}

		fprintf (fp, "%e %s {%s} %s %s %s\n", tcA[bestX].score,
			 treeP->list.itemA[nodeX].name, question,
			 noNode, yesNode, undefNode);
		fflush(logFp);
		if ( treeSplitNode (treeP, nodeX, question, noNode, yesNode,
				    undefNode, 0) < 0 ) {
		    ERROR("splitting node %s (%i) with question %s failed.\n",
			  treeP->list.itemA[nodeX].name, nodeX, question);

		    /* free all and close */
		    for (i = 0; i < tcN; i++)
			if ( tcA[i].excludeA ) free (tcA[i].excludeA);
		    if ( excludeA ) { free (excludeA); excludeA = NULL; }
		    if ( tcA )      { free (tcA);      tcA      = NULL; }
		    if ( fileName ) fileClose (fileName, fp);
		    return TCL_ERROR;
		}

		/* childs */
		idxA[0] = treeIndex (treeP, noNode);
		idxA[1] = treeIndex (treeP, yesNode);
		idxA[2] = treeIndex (treeP, undefNode);

		for (i = 0; i < 3; i++) {

		    /* if a child exists */
		    if ( idxA[i] >= 0 ) {
			/* give the array of father to the actual child */
			memcpy (excludeA, tcA[bestX].excludeA, questionN * sizeof(int));
			qIdx = treeFindQuestionFast (treeP, qsP, idxA[i], 0, &score, excludeA);

			if ( qIdx >= 0 ) {
			    int ptreeX = treeP->list.itemA[idxA[i]].ptree;

			    if ( verbose ) {
				fprintf (logFp, "%s question %s -questionSet %s: %s (%e)\n",
					 treeP->name,
					 treeP->list.itemA[idxA[i]].name, qsP->name,
					 questionSetName (qsP, qIdx), score);
				fflush(logFp);
			    }

			    if ( tcN >= maxTcN ) {
				maxTcN *= 2;
				tcA = (TreeClusterS*)realloc (tcA, maxTcN * sizeof(TreeClusterS));
			    }

			    tcA[tcN].nodeX     = idxA[i];
			    tcA[tcN].questionX = qIdx;
			    tcA[tcN].score     = score;
			    tcA[tcN].count     = treeP->ptreeSet.list.itemA[ptreeX].count;
			    /* save array in child (= next father if the score is
			       high enough) */
			    tcA[tcN].excludeA  = (int*)malloc (questionN * sizeof(int));
			    memcpy (tcA[tcN].excludeA, excludeA, questionN * sizeof(int));

			    tcN++;
			} else if ( verbose ) {
			    fprintf (logFp, "%s question %s -questionSet %s:  (0.0)\n",
				     treeP->name,
				     treeP->list.itemA[idxA[i]].name, qsP->name);
			    fflush(logFp);
			}
		    }
		}
	    } /* else: node is a final one */
	    /* free array of father (all childs are processed) */
	    free (tcA[bestX].excludeA); tcA[bestX].excludeA = NULL;

	} while ( (tcN - bestX - 2) > 0 && tcA[bestX].score == tcA[bestX+1].score );

	/* resort tcA in order of decreasing scores in intervall [bestX+1,tcN-1] */
	if ( (tcN - bestX - 2) > 0 ) {
	    tmpTcA = tcA+(bestX+1);
	    qsort ((void*)tmpTcA, tcN-(bestX+1), sizeof(TreeClusterS),
		   clustercompare);
	}
    }

    fflush (fp);
    fflush (logFp);
    if ( fileName ) fileClose (fileName, fp);

    /* free all tcA (all tcA[i].exlcudeA are freed further) */
    if ( excludeA ) { free (excludeA); excludeA = NULL; }
    if ( tcA )      { free (tcA);      tcA      = NULL; }

    return TCL_OK;
}

extern float ptreeModelArrayAdd (PTree *ptrP, float minCount, ClientData cd);

typedef struct TreeClusterMA_S TreeClusterMA;

typedef struct TreeClusterDist_S {

    TreeClusterMA *mPI;
    TreeClusterMA *mPJ;

    int   i,j;
    float dist;

    struct TreeClusterDist_S *prevP;
    struct TreeClusterDist_S *nextP;

} TreeClusterDist;

struct TreeClusterMA_S {

    ModelArray        mA;
    TreeClusterDist **dPA;
    int               distN; /* distN = i */

    struct TreeClusterMA_S *prevP;
    struct TreeClusterMA_S *nextP;
    
};


static TreeClusterDist* insertDist (TreeClusterDist *dA, TreeClusterDist *minDA,
				   float dist) {

    TreeClusterDist *tmpDA = minDA;

    if ( !dA ) {
	WARN("Can't insert no dist");
	return minDA;
    }

    dA->dist = dist;

    /* insert first element */
    if ( !minDA ) {
	dA->prevP = NULL;
	dA->nextP = NULL;
	minDA     = dA;

    } else if ( minDA->dist >= dA->dist ) {
	dA->nextP    = minDA;
	dA->prevP    = NULL;
	minDA->prevP = dA;
	minDA        = dA;

    } else {
	while ( 1 ) {

	    if ( tmpDA->dist >= dA->dist ) {
		dA->nextP           = tmpDA;
		dA->prevP           = tmpDA->prevP;
		tmpDA->prevP->nextP = dA;
		tmpDA->prevP        = dA;
		break;
	    }

	    if ( tmpDA->nextP ) {
		tmpDA        = tmpDA->nextP;

	    } else {
		tmpDA->nextP = dA;
		dA->prevP    = tmpDA;
		dA->nextP    = NULL;
		break;
	    }
	}
    }

    /*
    printf ("insert dist of %i %i. minDA points to %i %i\n",
	    dA->i, dA->j, minDA->i, minDA->j); fflush (stdout);
	    */
    return minDA;
}


static TreeClusterDist* deleteDist (TreeClusterDist *dA, TreeClusterDist *minDA) {

    if ( !dA ) {
	WARN("Can't delete no dist");
	return NULL;
    }

    if ( dA->prevP ) dA->prevP->nextP = dA->nextP;
    if ( dA->nextP ) dA->nextP->prevP = dA->prevP;

    if ( !dA->prevP ) minDA = dA->nextP;

    dA->prevP = dA->nextP = NULL;
    /*
    printf ("delete dist of %i %i. minDA points to %i %i\n",
	    dA->i, dA->j, minDA->i, minDA->j); fflush (stdout);
	    */
    return minDA;
}


static TreeClusterDist* deleteMA (TreeClusterMA *mA, int tmpI, int tmpJ,
				  TreeClusterDist *minDA, TreeClusterMA **mA0) {

    TreeClusterMA *tmpMA;
    int i;

    if ( !mA ) {
	WARN("Can't delete no modelArray");
	return minDA;
    }

    tmpMA = mA->nextP;

    while ( tmpMA ) {
	minDA            = deleteDist (tmpMA->dPA[tmpJ], minDA);
	tmpMA->dPA[tmpJ] = NULL;
	tmpMA            = tmpMA->nextP;
    }

    for (i = 0; i < mA->distN; i++) {
	if ( mA->dPA[i] ) {
	    minDA      = deleteDist (mA->dPA[i], minDA);
	    mA->dPA[i] = NULL;
	}
    }

    if ( mA->prevP  ) mA->prevP->nextP = mA->nextP;
    if ( mA->nextP  ) mA->nextP->prevP = mA->prevP;
    if ( *mA0 == mA ) *mA0 = mA->nextP;

    if ( mA->dPA ) { free (mA->dPA); mA->dPA = NULL; }
    mA->prevP = mA->nextP = NULL;
    /*    mA->distN = 0;*/
    modelArrayDeinit(&(mA->mA));

    return minDA;
}

static int computeNewDists (TreeClusterMA *tmpMA, TreeClusterDist **dAP,
			    ModelArray *unkA, PTree *ptrP) {

    ModelSetDistFct  *distFct = ptrP->mdsP.modelSetP->distFct;
    TreeClusterDist  *tmpDA;

    int   j, tmpI;
    float dist;

    tmpI = tmpMA->distN;

    /* compute new (N-1) distances */
    /* part one */
    for ( j = 0; j < tmpMA->distN; j++) {
	assert (tmpMA && tmpMA->dPA);

	tmpDA = tmpMA->dPA[j];

	if ( tmpDA ) { assert (tmpDA->mPI == tmpMA); }

	/* tmpDA->mPI is always the same */
	if ( tmpDA &&
	     distFct (ptrP->mdsP.cd, &(tmpDA->mPI->mA), &(tmpDA->mPJ->mA),
		      unkA, &dist)
	     == TCL_OK ) {
	    *dAP = deleteDist (tmpDA, *dAP);
	    *dAP = insertDist (tmpDA, *dAP, dist);
	} else if ( tmpDA ) {
	    ERROR ("distance function [0,tmpI) %i\n", j);
	    return TCL_ERROR;
	}
    }

    tmpMA = tmpMA->nextP;

    /* part two */
    while ( tmpMA ) {
	assert (tmpMA && tmpMA->dPA);
	
	tmpDA = tmpMA->dPA[tmpI];
	
	if (tmpDA ) { assert (tmpDA->mPI == tmpMA); }
	
	/* tmpDA->mPJ is always the same */
	if ( tmpDA &&
	     distFct (ptrP->mdsP.cd, &(tmpDA->mPI->mA), &(tmpDA->mPJ->mA),
		      unkA, &dist)
	     == TCL_OK ) {
	    *dAP = deleteDist (tmpDA, *dAP);
	    *dAP = insertDist (tmpDA, *dAP, dist);
	} else if ( tmpDA ) {
	    ERROR ("distance function [tmpI+1,N)\n");
	    return TCL_ERROR;
	}
		
	tmpMA = tmpMA->nextP;
    }

    return TCL_OK;
}

/* take one model out of mAP and put it into each other existing modelArray
   calculate new distance and take best result
   do this for each model in mAP
   for each other modelArray move each model into mAP
   calculate new distance and leave the model there, if the new distance is greater
   adjust dAP to new minimum Distance */
static int leeBottomUp (TreeClusterMA *mAP, TreeClusterDist **dAP,
			TreeClusterMA *mA0, ModelArray *unkA, PTree *ptrP,
			FILE *fp, FILE *logFp) {

    ModelSetDistFct  *distFct = ptrP->mdsP.modelSetP->distFct;

    TreeClusterMA    *tmpMA, *maxMA = NULL;
    TreeClusterDist  *tmpDA;
    int              i, model;
    float            count, dist, maxDist=0.0;
    
    /* first step: for each model */
    for ( i = 0; i < mAP->mA.modelN; i++ ) {
	model = mAP->mA.modelA[i];
	count = mAP->mA.countA[i];

	modelArrayDelete (&(mAP->mA), model, count);
	
	/* for each other modelArray */
	tmpMA   = mA0;
	maxDist = -FLT_MAX;
	while ( tmpMA ) {

	    if ( tmpMA != mAP ) {
		modelArrayAdd (&(tmpMA->mA), model, count);

		if ( distFct (ptrP->mdsP.cd, &(mAP->mA), &(tmpMA->mA),
			      unkA, &dist) == TCL_OK ) {
		    if ( dist > maxDist ) maxMA = tmpMA;
		}

		modelArrayDelete (&(tmpMA->mA), model, count);
	    }

	    tmpMA = tmpMA->nextP;
	}

	/* maxDist > dist */
	if ( maxMA->distN > mAP->distN ) {
	    tmpDA = maxMA->dPA[mAP->distN];
	} else {
	    tmpDA = mAP->dPA[maxMA->distN];
	}

	if ( maxDist > tmpDA->dist ) {
	    modelArrayAdd (&(maxMA->mA), model, count);
	    *dAP = deleteDist (tmpDA, *dAP);
	    *dAP = insertDist (tmpDA, *dAP, maxDist);

	    if ( logFp != fp ) {
		int j;
		fprintf (logFp, "Lee  %e:", maxDist);
		for (j = 0; j < maxMA->mA.modelN; j++ )
		    fprintf (logFp, " %s", maxMA->mA.mdsP.modelSetP->nameFct(maxMA->mA.mdsP.cd, maxMA->mA.modelA[j]));
		fprintf (logFp, "\n");
	    }

	    fprintf (fp, "lee %e %s %i %i\n", maxDist,
		     maxMA->mA.mdsP.modelSetP->nameFct(maxMA->mA.mdsP.cd, model),
		     mAP->distN, maxMA->distN);

	    if ( computeNewDists (mAP, dAP, unkA, ptrP)   != TCL_OK )
		return TCL_ERROR;
	    if ( computeNewDists (maxMA, dAP, unkA, ptrP) != TCL_OK )
		return TCL_ERROR;
	} else {
	    modelArrayAdd (&(mAP->mA), model, count);
	}
    }

    tmpMA = mA0;
    while ( tmpMA ) {

	if ( tmpMA != mAP && tmpMA->mA.modelN > 1 ) {
	    for ( i = 0; i < tmpMA->mA.modelN; i++ ) {
		model = tmpMA->mA.modelA[i];
		count = tmpMA->mA.countA[i];

		modelArrayDelete (&(tmpMA->mA), model, count);
		modelArrayAdd    (&(mAP->mA),   model, count);

		if ( distFct (ptrP->mdsP.cd, &(mAP->mA), &(tmpMA->mA),
			      unkA, &dist) == TCL_OK ) {
		    if ( tmpMA->distN > mAP->distN ) {
			tmpDA = tmpMA->dPA[mAP->distN];
		    } else {
			tmpDA = mAP->dPA[tmpMA->distN];
		    }

		    if ( dist > tmpDA->dist ) {
			*dAP = deleteDist (tmpDA, *dAP);
			*dAP = insertDist (tmpDA, *dAP, maxDist);

			if ( logFp != fp ) {
			    int j;
			    fprintf (logFp, "Lee  %e:", dist);
			    for (j = 0; j < mAP->mA.modelN; j++ )
				fprintf (logFp, " %s", mAP->mA.mdsP.modelSetP->nameFct(mAP->mA.mdsP.cd, mAP->mA.modelA[j]));
			    fprintf (logFp, "\n");
			}
			
			fprintf (fp, "lee %e %s %i %i\n", dist,
				 mAP->mA.mdsP.modelSetP->nameFct(mAP->mA.mdsP.cd, model),
				 tmpMA->distN, mAP->distN);

			if ( computeNewDists (mAP, dAP, unkA, ptrP)   != TCL_OK )
			    return TCL_ERROR;
			if ( computeNewDists (tmpMA, dAP, unkA, ptrP) != TCL_OK )
			    return TCL_ERROR;
		    } else {
			modelArrayAdd    (&(tmpMA->mA), model, count);
			modelArrayDelete (&(mAP->mA),   model, count);
		    }
		}
	    }
	}

	tmpMA = tmpMA->nextP;
    }

    return TCL_OK;
}

int treeClusterBottomUp (Tree *tree, int nodeX, float minCount, int lee,
			 char *fileName, int verbose) {


    TreeNode         *tnP     = tree->list.itemA + nodeX;
    PTree            *ptrP    = tree->ptreeSet.list.itemA+tnP->ptree;
    ModelSetDistFct  *distFct = ptrP->mdsP.modelSetP->distFct;
    FILE             *logFp   = stderr; /* for verbose flag */
    FILE             *fp      = fileName ? fileOpen (fileName, "w") : logFp;

    TreeClusterMA    *mA = NULL, *mAP, *mA0;
    TreeClusterDist  *dA = NULL, *dAP;

    ModelArray        MAP;  /* handels all models in given ptree */
    ModelArray        unkA;
    float             mAN;
    float             dist;
    int               i, j, k;

    TreeClusterMA   *tmpMA;

    int tmpI, tmpJ;

    /* get all models */
    modelArrayInit (&MAP,  &(ptrP->mdsP));
    modelArrayInit (&unkA, &(ptrP->mdsP));

    ptreeModelArrayAdd (ptrP, minCount, (ClientData)&MAP);

    fprintf (logFp, "modelArr get %i models for %s\n", MAP.modelN, ptrP->name);

    if ( MAP.modelN == 0 ) {

	modelArrayDeinit (&MAP);
	modelArrayDeinit (&unkA);
	if ( fileName ) fileClose (fileName, fp);
		
	return TCL_OK;
    }

    mA = (TreeClusterMA*)malloc   (MAP.modelN * sizeof (TreeClusterMA));
    dA = (TreeClusterDist*)malloc (MAP.modelN * MAP.modelN *
				   sizeof (TreeClusterDist));

    /* init 0 element */
    i = 0; k = 0;
    modelArrayInit (&(mA[i].mA), &(ptrP->mdsP));
    mA[i].dPA   = (TreeClusterDist**)malloc (i * sizeof(TreeClusterDist*));
    mA[i].prevP = NULL;
    mA[i].nextP = NULL;
    mA[i].distN = i;
    modelArrayAdd (&(mA[i].mA), MAP.modelA[i], MAP.countA[i]);
    mAP = &mA[0]; /* mAP points always to the new modelArray */
    mA0 = &mA[0]; /* mA0 points always to the beginning of the list */
    dAP = NULL;
    mAN = MAP.modelN;

    fprintf (fp, "{%i %s %f}", i,
	     MAP.mdsP.modelSetP->nameFct(MAP.mdsP.cd, MAP.modelA[i]),
	     MAP.countA[i]);

    for ( i = 1; i < MAP.modelN; i++ ) {

	/* init */
	modelArrayInit (&(mA[i].mA), &(ptrP->mdsP));
	mA[i].dPA     = (TreeClusterDist**)malloc (i * sizeof(TreeClusterDist*));
	mA[i].distN   = i;
	mA[i].prevP   = &mA[i-1];
	mA[i].nextP   = NULL;
	mA[i-1].nextP = &mA[i];

	modelArrayAdd (&(mA[i].mA), MAP.modelA[i], MAP.countA[i]);

	fprintf (fp, " {%i %s %f}", i,
		 MAP.mdsP.modelSetP->nameFct(MAP.mdsP.cd, MAP.modelA[i]),
		 MAP.countA[i]);

	for ( j = 0; j < i; j++ ) {

	    if ( distFct (ptrP->mdsP.cd, &(mA[i].mA), &(mA[j].mA), &unkA, &dist)
		 == TCL_OK ) {

		dA[k].mPI  = &mA[i];
		dA[k].mPJ  = &mA[j];
		dA[k].i    = i;
		dA[k].j    = j;
		dA[k].prevP = NULL;
		dA[k].nextP = NULL;

		mA[i].dPA[j] = &dA[k];

		dAP = insertDist (&dA[k], dAP, dist);

		k++;
	    } else {
		/* minCount errors of distFct not fixed yet */
		ERROR ("distance function (init) %i %i\n", i, j);

		mA[i].dPA[j] = NULL;

		if ( fileName ) fileClose (fileName, fp);
		return TCL_ERROR;
	    }
	}
    }

    fprintf (fp, "\n");

    /* dAP points to element with minimum distance */
    /* it is always i > j */

    while ( mAN > 3 ) {
        assert (dAP); // XCode says could be NULL
    mAP = dAP->mPI;

	/* copy elements (dAP->mPI points to new modelArray) */
	for ( i = 0; i < dAP->mPJ->mA.modelN; i++ ) {
	    modelArrayAdd (&(dAP->mPI->mA), dAP->mPJ->mA.modelA[i],
			   dAP->mPJ->mA.countA[i]);
	}

	fprintf (fp, "%e %i %i %i\n", dAP->dist, dAP->i, dAP->j, dAP->i);

	if ( verbose ) {
	    fprintf (logFp, "Join %e:", dAP->dist);
	    for ( i = 0; i < dAP->mPI->mA.modelN; i++ )
	    	fprintf (logFp, " %s", MAP.mdsP.modelSetP->nameFct(MAP.mdsP.cd, dAP->mPI->mA.modelA[i]));
	    fprintf (logFp, "\n");
	}

	/* delete modelArray */
	tmpMA = dAP->mPI;
	tmpI  = dAP->i;
	tmpJ  = dAP->j;

	dAP = deleteMA (dAP->mPJ, tmpI, tmpJ, dAP, &mA0);

	assert (tmpMA->distN == tmpI);

	if ( computeNewDists (tmpMA, &dAP, &unkA, ptrP) != TCL_OK ) {
	    if ( fileName ) fileClose (fileName, fp);
	    return TCL_ERROR;
	}

	if ( lee && leeBottomUp (mAP, &dAP, mA0, &unkA, ptrP, fp, logFp) != TCL_OK) {
       	    if ( fileName ) fileClose (fileName, fp);
	    return TCL_ERROR;
	}

	mAN--;
    }

    if ( mAN == 3 ) {
        assert (dAP); // XCode says could be NULL
	mAP = dAP->mPI;

	/* copy elements (dAP->mPI points to new modelArray) */
	if ( lee || verbose ) {
	    for ( i = 0; i < dAP->mPJ->mA.modelN; i++ ) {
		modelArrayAdd (&(dAP->mPI->mA), dAP->mPJ->mA.modelA[i],
			       dAP->mPJ->mA.countA[i]);
	    }
	}

	fprintf (fp, "%e %i %i %i\n", dAP->dist, dAP->i, dAP->j, dAP->i);

	if ( verbose ) {
	    fprintf (logFp, "Join %e:", dAP->dist);
	    for ( i = 0; i < dAP->mPI->mA.modelN; i++ )
	    	fprintf (logFp, " %s", MAP.mdsP.modelSetP->nameFct(MAP.mdsP.cd, dAP->mPI->mA.modelA[i]));
	    fprintf (logFp, "\n");
	}

	tmpMA = dAP->mPI;
	tmpI  = dAP->i;
	tmpJ  = dAP->j;

	dAP = deleteMA (dAP->mPJ, tmpI, tmpJ, dAP, &mA0);

	if ( lee ) {
	    if ( computeNewDists (tmpMA, &dAP, &unkA, ptrP) != TCL_OK ) {
		if ( fileName ) fileClose (fileName, fp);
		return TCL_ERROR;
	    }

	    if ( leeBottomUp (mAP, &dAP, mA0, &unkA, ptrP, fp, logFp) != TCL_OK ) {
		if ( fileName ) fileClose (fileName, fp);
		return TCL_ERROR;
	    }
	}
    }

    if ( dAP ) {
	if ( dAP->mPI && dAP->mPI->dPA ) {
	    free (dAP->mPI->dPA); dAP->mPI->dPA = NULL;
	    modelArrayDeinit (&(dAP->mPI->mA));
	}
	if ( dAP->mPJ && dAP->mPJ->dPA ) {
	    free (dAP->mPJ->dPA); dAP->mPJ->dPA = NULL;
	    modelArrayDeinit (&(dAP->mPJ->mA));
	}
    }
    modelArrayDeinit (&MAP);
    modelArrayDeinit (&unkA);
    if ( mA ) { free (mA); mA = NULL; }
    if ( dA ) { free (dA); dA = NULL; }
    fflush (logFp);
    fflush (fp);
    if ( fileName ) fileClose (fileName, fp);

    return TCL_OK;
}


int treeClusterItf (ClientData cd, int argc, char *argv[]) {

    Tree        *treeP    =  (Tree*)cd;
    QuestionSet *qsP      = &(treeP->questions);
    int          nodeX    = -1;
    float        minCount =  0;
    int          maxSplit =  1000;
    float        minScore =  0.0;
    char        *fileName =  NULL;
    int          verbose  =  1;
    int          bottomUp =  0;
    int          lee =  1;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<rootNode>", ARGV_LINDEX, NULL, &nodeX, &(treeP->list),
		       		"root node",
		       "-questionSet", ARGV_OBJECT, NULL, &qsP, "QuestionSet",
		       		"question set",
		       "-minCount", ARGV_FLOAT, NULL, &minCount, NULL,
				"minimum count (ptree)",
		       "-minScore", ARGV_FLOAT, NULL, &minScore, NULL,
				"minimum score",
		       "-maxSplit", ARGV_INT, NULL, &maxSplit, NULL,
				"maximum number of splits",
		       "-file", ARGV_STRING, NULL, &fileName, NULL,
		       		"cluster log file",
		       "-bottomUp", ARGV_INT, NULL, &bottomUp, NULL,
		       		"cluster bottom up (agglomerative)",
		       "-lee", ARGV_INT, NULL, &lee, NULL,
		       		"Kai-Fu Lee's bottom up cluster extension",
		       "-verbose", ARGV_INT, NULL, &verbose, NULL,
		       		"verbose",
		       NULL) != TCL_OK ) return TCL_ERROR;

    if ( bottomUp )
	return treeClusterBottomUp (treeP, nodeX, minCount, lee, fileName,
				    verbose);
    else
	return treeCluster         (treeP, nodeX, qsP, minCount, maxSplit, minScore,
				    fileName, verbose);
}


/* ========================================================================
    Merging multiple files containing tree splitting information
   ======================================================================== */

int treeMergeSplits (char *outFile, int inFileN, char **inFile)
{
  FILE*    fp   = NULL; 
  char*   buf   = NULL;
  char** bufp   = NULL;
  char*     p   = NULL;
  int*   bufA   = NULL;
  int bufSize   = 0;
  int linNtotal = 0;
  int  f, l, linX, from, to;

  float  *gain[2];
  char  **linp[2];
  int    *linN[2];
  int    *file[2];
  int    fileN[2];

  if (NULL== (bufA = malloc(inFileN*sizeof(int)))) {
    ERROR("treeMergeSplits: allocation failure\n");
    return TCL_ERROR;
  }

  /* ------------------------------------------- */
  /* see if it's worth to start at all           */
  /* ------------------------------------------- */

  if ((fp=fileOpen(outFile,"w"))==NULL) { 
    ERROR("can't write to file %s\n",outFile); 
    return TCL_ERROR; 
  }
  fclose (fp);
  
  /* ------------------------------------------- */
  /* first, find out how large all the files are */
  /* ------------------------------------------- */

  for (f=0; f<inFileN; f++) { 
    if ((fp=fileOpen(inFile[f],"r"))==NULL) {  
      ERROR("can't read from file %s\n",inFile[f]); 
      return TCL_ERROR; 
    }
    fseek(fp,0,SEEK_END); 
    bufA[f]  = ftell(fp);
    bufSize += bufA[f];
    fileClose(inFile[f], fp);
  }

  /* ----------------------------------------------------------------- */
  /* allocate a huge buffer that will hold all files                   */
  /* also, allocate arrays for file-begininng pointers:   bufp[fileX]  */
  /*                arrays for line-counts per file:      linN?[fileX] */
  /* ----------------------------------------------------------------- */

  buf   =  (char*)malloc(sizeof(char) *bufSize);
  bufp  = (char**)malloc(sizeof(char*)*(inFileN+1));
  linN[0] =   (int*)malloc(sizeof(int)*(inFileN+1));
  linN[1] =   (int*)malloc(sizeof(int)*(inFileN+1));

  /* -------------------------------------------------------------------- */
  /* read all files into the big buffer, and initialize the file-bginning */
  /* pointers, fill the line-counter arrays, and count the total lines    */
  /* -------------------------------------------------------------------- */

  bufp[0] = buf;
  for (f=0; f<inFileN; f++) { 
    fp=fileOpen(inFile[f],"r");
    bufp[f+1] = bufp[f] + fread(bufp[f],sizeof(char),bufA[f],fp);
    fileClose(inFile[f],fp);
    for (p=bufp[f],linN[0][f]=0; p!=bufp[f+1]; p++) {
      if (*p=='\n') linN[0][f]++;
    }
    linNtotal += linN[0][f];
  }
  linN[0][inFileN]=0; /* additional dummy file for paring odd file numbers */
  linN[1][inFileN]=0;

  /* ----------------------------------------------------------------------- */
  /* allocate the data structures for merging, all are double because we can */
  /* either merge from datastructures 0 to datastructures 1 or from 1 to 0.  */
  /*    linp?  = pointers to the lines' texts                                */
  /*    gain?  = floating point values of all lines' gains                   */
  /*    file?  = index into the first item of a file in the above two arrays */
  /*    linN?  = number of lines per file                                    */
  /*   fileN?  = number of files left to be merged                           */
  /* MOTE: from here on we don't use the term file any more for something    */
  /*       that can be opended with fopen(...) but simply for a range in the */
  /*       linp and gain arrays, initially these ranges refelct the 'files'  */
  /* ----------------------------------------------------------------------- */

  linp[0] = (char**)malloc(sizeof(char*)*linNtotal);
  linp[1] = (char**)malloc(sizeof(char*)*linNtotal);
  gain[0] = (float*)malloc(sizeof(float)*linNtotal);
  gain[1] = (float*)malloc(sizeof(float)*linNtotal);
  file[0] =   (int*)malloc(sizeof(int)  *(inFileN+1));
  file[1] =   (int*)malloc(sizeof(int)  *(inFileN+1));
  fileN[0] = inFileN;

  /* ---------------------------------------------------------- */
  /* initialize the first set of all the merging-datastructures */
  /* ---------------------------------------------------------- */

  for (linX=0, f=0; f<fileN[0]; f++)
  { file[0][f] = linX;
    for (l=0; l<linN[0][f]; l++)
    {   linp[0][linX] = strtok(linX?NULL:buf,"\n");
        gain[0][linX] = atof(linp[0][linX]);
        linX++;
    }
  }

  /* ----------------------------------------------------------------------- */
  /* Okay, we're ready to run, keep merging the structures in ...[from] into */
  /* the structures in ...[to], until there is only one file left.           */
  /* ----------------------------------------------------------------------- */

  from = 0; to = 1; 
  while (fileN[from]>1) {
     int p1, p2, p3, put;

     fileN[to]=0; file[to][0] = 0;
     for (f=0; f<fileN[from]; f+=2) {
       int f1 = f, f2 = f+1;

       /* ------------------------------------------------------------- */
       /* merge "from's" files 'f2' and 'f2' into "to's" file fileN[to] */
       /* ------------------------------------------------------------- */

       p1 = file[from][f1]; 
       p2 = file[from][f2]; 
       p3 = file[to][fileN[to]];

       while (p1<file[from][f1]+linN[from][f1] ||
	      p2<file[from][f2]+linN[from][f2]) {

         if (p1==file[from][f1]+linN[from][f1]) { put=p2++; } else
         if (p2==file[from][f2]+linN[from][f2]) { put=p1++; } else
         if (gain[from][p1] > gain[from][p2])   { put=p1++; } else { put=p2++;}
         linp[to][p3] = linp[from][put];
	 gain[to][p3] = gain[from][put];
         p3++;
       }

       linN[to][fileN[to]] = p3-file[to][fileN[to]];
       fileN[to]++;
       file[to][fileN[to]] = p3;
     }
     linN[to][fileN[to]] = 0;
     from = 1-from;
     to   = 1-to;
  }

  /* ----------------------------------------------------------------------- */
  /* only one file left, write that file into the desired output             */
  /* ----------------------------------------------------------------------- */

  if ((fp=fileOpen(outFile,"w"))==NULL) {  
    ERROR("can't write to file %s\n",outFile); 
    return TCL_ERROR; 
  }
  for (l=0; l<linNtotal; l++) fprintf(fp,"%s\n",linp[from][l]);
  fileClose(outFile,fp);

  if (bufA) free(bufA);

  return TCL_OK;
}

int treeMergeSplitsItf (ClientData cd, Tcl_Interp *itf, int argc, char *argv[])
{
  char *outFile  = NULL;
  char *inFiles  = NULL;
  int     ac     =  argc-1;
  int     tokc, ret;
  char  **tokv;


  if (itfParseArgv(argv[0], &ac, argv+1, 0,
      "<inFiles>",   ARGV_STRING,NULL,&inFiles,NULL,"list of names of input files",
      "<outFile>",   ARGV_STRING,NULL,&outFile, NULL, "name of merged file",
		   NULL) != TCL_OK)
    return TCL_ERROR;

  if ( Tcl_SplitList (itf,inFiles,&tokc,&tokv) != TCL_OK) return TCL_ERROR;

  ret =  treeMergeSplits( outFile, tokc, tokv);
  Tcl_Free((char*)tokv);
  return ret;
}


/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method treeNodeMethod[] = 
{
  { "puts", treeNodePutsItf,   NULL },
  {  NULL,  NULL, NULL } 
};

TypeInfo treeNodeInfo = { 
        "TreeNode", 0, -1, treeNodeMethod, 
         NULL, NULL, 
         treeNodeConfigureItf, NULL, itfTypeCntlDefaultNoLink,
        "TreeNode\n" };

static Method treeMethod[] = 
{
  { "puts",   treePutsItf,     "displays the contents of a tree object"    },
  { "add",    treeAddItf,      "add a new node to the tree"                },
  { "read",   treeReadItf,     "read a tree from a file"                   },
  { "write",  treeWriteItf,    "write a tree into a file"                  },
  { "list",   treeListItf,     "list a tree contents in TCL list format"   },
  { "get",    treeGetModelItf, "descend a tree for a given phone sequence" },
  { "trace",  treeTraceItf,    "trace a tree for a given phone sequence"   },
  { "index",  treeIndexItf,    "return the index of a node"                },
  { "name",   treeNameItf,     "return the name of an indexed node"        },

  { "question",  treeFindQuestionItf,"return best splitting question to ask" },
  { "split",     treeSplitNodeItf,   "split node according to a question"    },
  { "cluster",   treeClusterItf,     "split whole subtree of a given root node" },
  { "transform", treeTransformItf,   "transform tree for modalities"         },
  { NULL, NULL, NULL }
} ;

TypeInfo treeInfo = { 
        "Tree", 0, -1,     treeMethod, 
         treeCreateItf,    treeFreeItf, 
         treeConfigureItf, treeAccessItf, NULL,
        "A 'Tree' object is an allophone clustering tree." };

static int treeInitialized = 0;

int Tree_Init ()
{
  if (! treeInitialized) {
    if ( PTree_Init()     != TCL_OK) return TCL_ERROR;
    if ( Questions_Init() != TCL_OK) return TCL_ERROR;

    treeDefault.name            = NULL;
    treeDefault.useN            = 0;
    treeDefault.list.itemN      = 0;
    treeDefault.list.blkSize    = 5000;
    treeDefault.commentChar     = ';';

    itfNewType(&treeNodeInfo);
    itfNewType(&treeInfo);

    Tcl_CreateCommand (itf,"mergeSplits",treeMergeSplitsItf, 0, 0);

    treeInitialized = 1;
  }
  return TCL_OK;
}

