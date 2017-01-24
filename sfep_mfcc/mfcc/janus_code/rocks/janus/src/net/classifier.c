/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Classifier
               ------------------------------------------------------------

    Author  :  Juergen Fritsch
    Module  :  classifier.c
    Date    :  $Id: classifier.c 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.3  2003/08/14 11:20:19  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.2.56.2  2002/06/26 12:26:49  fuegen
    go away, boring message

    Revision 3.2.56.1  2002/06/26 11:57:57  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.1  96/09/27  08:59:26  fritsch
 * Initial revision
 * 
     Revision 1.1  1996/01/31  05:11:37  fritsch
     Initial revision


   ======================================================================== */


#include "common.h"
#include "itf.h"

#include "error.h"
#include "classifier.h"

/* =============================================================== */
/*  classifierArray                                                */
/* =============================================================== */

static ClassifierArray   *classifierArray;



/* ------------------------------------------------------------------------- */
/*  classArrayInit                                                           */
/* ------------------------------------------------------------------------- */
int  classArrayInit (int classMax) {

  classifierArray = (ClassifierArray *) malloc (sizeof(ClassifierArray));
  classifierArray->classMax = classMax;
  classifierArray->classN   = 0;
  classifierArray->classPA  = (Classifier **) malloc (sizeof(Classifier *) * classMax);

  return TCL_OK;
}



/* ------------------------------------------------------------------------- */
/*  classNewType                                                             */
/* ------------------------------------------------------------------------- */
int  classNewType (Classifier *classPtr) {

  if (classIndex(classPtr->name) >= 0) {
    ERROR("Classifier with name '%s' already exists\n",classPtr->name);
    return TCL_ERROR;
  }

  if (classifierArray->classN == classifierArray->classMax) {
    ERROR("No more Classifiers accepted. ClassifierArray overflow.\n");
    return TCL_ERROR;
  }
  
  classifierArray->classPA[classifierArray->classN] = classPtr;
  classifierArray->classN++;

  return TCL_OK;
}



/* ------------------------------------------------------------------------- */
/*  classIndex                                                               */
/* ------------------------------------------------------------------------- */
int  classIndex (char *name) {

  int      index = -1;
  int      classX;

  for (classX=0; classX<classifierArray->classN; classX++) {
    if (strcmp(classifierArray->classPA[classX]->name,name) == 0) {
      index = classX;
      break;
    }
  }
  
  return index;
}



/* ------------------------------------------------------------------------- */
/*  classGetByIdx                                                            */
/* ------------------------------------------------------------------------- */
Classifier  *classGetByIdx (int index) {

  if ((index < 0) || (index >= classifierArray->classN)) {
    ERROR("index %d into ClassifierArray out of range\n",index);
    return NULL;
  }

  return  classifierArray->classPA[index];
}



/* ------------------------------------------------------------------------- */
/*  classGetByName                                                           */
/* ------------------------------------------------------------------------- */
Classifier  *classGetByName (char *name) {

  register int   index; 
  
  index = classIndex(name);
  if ((index < 0) || (index >= classifierArray->classN)) {
    ERROR("no classifier type '%s' existing\n",name);
    return NULL;
  }

  return  classifierArray->classPA[index];
}



/* ------------------------------------------------------------------------- */
/*  classifierList                                                           */
/* ------------------------------------------------------------------------- */
int  classifierList (ClientData clientData, Tcl_Interp *interp,
                     int argc, char *argv[]) {

  int      classX;

  for (classX=0; classX<classifierArray->classN; classX++)
    itfAppendElement("%d %s",classX,classifierArray->classPA[classX]->name);

  return TCL_OK;
}



/* ========================================================================= */
/*  Type Declaration                                                         */
/* ========================================================================= */

static int classifierInitialized = 0;

int Classifier_Init ( )
{
  if (classifierInitialized) return TCL_OK;

  classArrayInit(99);
  Tcl_CreateCommand ( itf, "classifiers", classifierList, 0, 0 );

  classifierInitialized = 1;
  return TCL_OK;
}























