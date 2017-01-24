/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Tree (treeTransform)
               ------------------------------------------------------------

    Author  :  Christian Fuegen
    Module  :  treeTransform.h
    Date    :  $Id: treeTransform.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 1.3  2000/08/27 15:31:30  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 1.2.4.1  2000/08/25 22:19:45  jmcd
    Just checking.

    Revision 1.2  2000/01/12 10:12:49  fuegen
    transformation of a tree wirth modality questions

    Revision 1.1  1999/03/10 18:46:53  fuegen
    Initial revision


   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _treeTransform
#define _treeTransform


/* define answers for tree */
#define TREE_ANSWER_NO    0
#define TREE_ANSWER_UNDEF 1
#define TREE_ANSWER_YES   2

/*
struct subTreeDesc {
    double  modelASum;
    int     modelX;
    int    *modelXA;
    int     modelXAN;
};

#define TREE_MAXDESC 100000
*/

/* ========================================================================
    Module Interface
   ======================================================================== */


extern int treeTransform    (Tree *tree, Tree *mainTree, Tree *modTree,
			     QuestionSet *qsP, char *dummyName,
			     char *rootIdentifier, int divide);
extern int treeTransformItf (ClientData cd, int argc, char *argv[]);


#endif

#ifdef __cplusplus
}
#endif
