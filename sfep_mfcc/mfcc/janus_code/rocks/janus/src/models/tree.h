/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Tree
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  tree.h
    Date    :  $Id: tree.h 700 2000-11-14 12:35:27Z janus $
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
    Revision 4.5  2000/11/14 12:35:26  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 4.4.34.1  2000/11/06 10:50:41  janus
    Made changes to enable compilation under -Wall.

    Revision 4.4  2000/08/27 15:31:30  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 4.3.4.1  2000/08/25 22:19:45  jmcd
    Just checking.

    Revision 4.3  2000/02/08 15:32:17  soltau
    don't define question_verbose

    Revision 4.2  2000/01/12 10:12:49  fuegen
    fast clustering procedure
    bottom up clustering

    Revision 4.1  1999/03/10 18:47:14  fuegen
    *** empty log message ***

    Revision 4.0  1998/11/30 18:07:14  fuegen
    *** empty log message ***

    Revision 3.3  1997/07/31 16:32:12  rogina
    added prototype for treeGetModelItf

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.10  96/02/17  19:25:21  finkem
    removed root index in the tree structure so that there is no longe
    a predefined root (backup for nodeX < 0)
    
    Revision 1.9  1996/01/31  05:01:20  finkem
    generic tree based on model sets
    inherent ptrees

    Revision 1.8  1996/01/12  16:17:33  finkem
    changed treeRead to freadAdd
    added ptree answers

    Revision 1.7  1995/10/13  22:40:24  finkem
    treePhones/treeTags Pointer function added

    Revision 1.6  1995/09/21  21:39:43  finkem
    changed tree to have don't know branches, too, in order to model the
    same answers as the JANUS2 tree/search module.

    Revision 1.5  1995/08/27  22:34:26  finkem
    *** empty log message ***

    Revision 1.4  1995/08/18  08:22:39  finkem
    *** empty log message ***

    Revision 1.3  1995/08/01  22:15:27  finkem
    *** empty log message ***

    Revision 1.2  1995/07/25  22:33:21  finkem
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _tree
#define _tree

#include "questions.h"
#include "ptree.h"
#include "modelSet.h"

/* #define QUESTION_VERBOSE */

/* ========================================================================
    TreeNode
   ======================================================================== */

typedef struct
{
  char*   name;
  int     question;       /* index of this node's question              */

  int     yes;            /* index of the 'yes'   - successor node      */
  int     no;             /* index of the 'no'    - successor node      */
  int     undef;          /* index of the 'undef' - successor node      */ 

  int     model;          /* index of this nodes model                  */
  int     ptree;          /* index of the  associated ptree             */

  int     mark;           /* mark for DFS */
} TreeNode;

/* ========================================================================
    Tree
   ======================================================================== */

typedef struct LIST(TreeNode) TreeNodeList;

typedef struct
{
  char*         name;        /* symbolic name of this tree */
  int           useN;
  TreeNodeList  list;

  int           ptreeAdd;
  QuestionSet   questions;   /* list of questions              */
  PTreeSet      ptreeSet;    /* pointer to polyphonic tree set */

  ModelSetPtr   mdsP;

  char          commentChar; /* if at beginning of line, line is ignored */

} Tree;

/* ========================================================================
    Module Interface
   ======================================================================== */

extern int   treeInit( Tree*     tree,    char*      name,
                       Phones*   phones,  PhonesSet* phonesSet, Tags* tags,
                       ModelSetPtr* mdsP);

extern Tree* treeCreate( char *name, Phones* phones, PhonesSet*   phonesSet, 
                                     Tags*   tags,   ModelSetPtr* mdsP);

extern int treeLinkN(  Tree* tree);
extern int treeDeinit( Tree* tree);
extern int treeFree(   Tree* tree);

extern int        treeConfigureItf(ClientData cd, char *var, char *val);
extern ClientData treeAccessItf(   ClientData cd, char *name, TypeInfo **ti);
extern int        treePutsItf(     ClientData clientData, 
                                   int argc, char *argv[]);

extern int     treeIndex(    Tree* tree, char* name);
extern int     treeIndexItf( ClientData clientData, int argc, char *argv[]);
extern char*   treeName(     Tree* tree, int i);
extern int     treeNameItf(  ClientData clientData, int argc, char *argv[]);
extern Phones* treePhones(   Tree* tree);
extern Tags*   treeTags(     Tree* tree);

extern int treeAdd(      Tree* tree, char* name, char* question,
                                     char* no,   char* yes, char* undef,
                                     char* model,int   ptree);
extern int treeAddItf(   ClientData clientData, int argc, char *argv[]);

extern int treeRead(     Tree *tree, char *filename);
extern int treeReadItf(  ClientData clientData, int argc, char *argv[] );

extern int treeWrite(    Tree *tree, char *filename);
extern int treeWriteItf( ClientData clientData, int argc, char *argv[] );

extern int treeGetModel( Tree* tree, int nodeX, Word* wP, int left, int right);
extern int treeGetNode(  Tree* tree, int nodeX, Word* wP, int left, int right);

extern int treeGetModelItf( ClientData clientData, int argc, char *argv[]);

extern int treeClusterItf (ClientData cd, int argc, char *argv[]);

extern int Tree_Init ();

#endif

#ifdef __cplusplus
}
#endif
