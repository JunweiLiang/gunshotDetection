/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Model
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  model.h
    Date    :  $Id: modelSet.h 700 2000-11-14 12:35:27Z janus $
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
    Revision 4.3  2000/11/14 12:35:25  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 4.2.30.1  2000/11/06 10:50:37  janus
    Made changes to enable compilation under -Wall.

    Revision 4.2  2000/08/27 15:31:26  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 4.1.4.1  2000/08/25 22:19:45  jmcd
    Just checking.

    Revision 4.1  2000/01/12 10:12:49  fuegen
    add procedure to delete a model from an modelArray

    Revision 4.0  1998/11/30 18:07:14  fuegen
    *** empty log message ***

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

     Revision 1.3  96/04/01  23:36:55  finkem
     block allocation for modelSetArray introduced
     
     Revision 1.2  1996/02/11  06:25:01  finkem
     add modelArray clear

     Revision 1.1  1996/01/31  05:11:37  finkem
     Initial revision


   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _model
#define _model

/* ========================================================================
    ModelSet
   ======================================================================== */

typedef struct ModelArray_S ModelArray;

typedef int    ModelSetIndexFct( ClientData CD, char* name);
typedef char*  ModelSetNameFct(  ClientData CD, int i);
typedef int    ModelSetDistFct(  ClientData CD, ModelArray* maP,
                                                ModelArray* maQ,
                                                ModelArray* maR,
                                 float* distP);

typedef struct ModelSet_S
{
  char*             name;
  TypeInfo*         tiP;
  TypeInfo*         tiModelP;

  ModelSetIndexFct* indexFct;
  ModelSetNameFct*  nameFct;
  ModelSetDistFct*  distFct; 

} ModelSet;

extern int modelSetNewType( ModelSet* mdsP);

/* ========================================================================
    ModelSetPtr
   ======================================================================== */

typedef struct ModelSetPtr_S {

  ClientData  cd;
  ModelSet*   modelSetP;

} ModelSetPtr;

int modelSetPtrInit(   ModelSetPtr* msP);
int modelSetPtrDeinit( ModelSetPtr* msP);

extern int modelSetPtrGetItf( ModelSetPtr* marrayP, char* value);

extern int getModelSetPtr( ClientData cd, char* key, ClientData result,
	                   int *argcP, char *argv[]);

/* ========================================================================
    ModelArray
   ======================================================================== */

struct ModelArray_S {

  int         allocN;
  int         modelN;
  int*        modelA;
  float*      countA;

  ModelSetPtr mdsP;
};

extern int modelArrayInit(   ModelArray* mdaP, struct ModelSetPtr_S* mdsP);
extern int modelArrayDeinit( ModelArray* mdaP);
extern int modelArrayClear(  ModelArray* mdaP);
extern int modelArrayAdd(    ModelArray* mdaP, int model, float count);
extern int modelArrayDelete( ModelArray* mdaP, int model, float count);

extern int ModelSet_Init ( );

#endif

#ifdef __cplusplus
}
#endif
