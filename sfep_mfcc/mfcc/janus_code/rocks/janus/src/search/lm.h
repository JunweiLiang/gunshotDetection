
/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Language Model
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  lm.h
    Date    :  $Id: lm.h 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 1.5  2003/08/14 11:20:22  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.4.20.2  2001/04/24 09:13:10  metze
    Clean-up

    Revision 1.4.20.1  2001/02/27 15:10:27  metze
    LModelNJD and LMLA work with floats

    Revision 1.4  2000/11/14 12:29:35  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 1.3.30.1  2000/11/06 10:50:33  janus
    Made changes to enable compilation under -Wall.

    Revision 1.3  2000/08/27 15:31:21  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 1.2.4.1  2000/08/25 22:19:44  jmcd
    Just checking.

    Revision 1.2  1998/06/11 14:22:27  kries
    add functions that conform with old LM style
    to make changes in search lock small

 * Revision 1.1  1998/05/26  18:27:23  kries
 * Initial revision
 *

   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _lm_h
#define _lm_h

#include "array.h"


/* ========================================================================
    LMPtr
   ======================================================================== */

typedef struct LMPtr_S {
  ClientData  cd;
  struct LM_S *         lmP;
} LMPtr;

int lmPtrInit(   LMPtr* msP);
int lmPtrDeinit( LMPtr* msP);

extern int        lmPtrGetItf(    LMPtr* marrayP, char* value);
extern int        getLMPtr(       ClientData cd, char* key, ClientData result,
				  int *argcP, char *argv[]);
extern int        LMPtrIndexItf(  ClientData CD,     char* key, 
				  ClientData retPtr, int*  argcPtr,
				  char* argv[]);
extern ClientData LMPtrAccessItf( ClientData cd, char *name, TypeInfo **ti);
extern int        LMPtrNameItf(   LMPtr* lmP, int argc, char *argv[]);


/* ========================================================================
    LM
   ======================================================================== */

#define TYPEDEF(A)  struct A ## _TypeDummy { int dummy; }; typedef struct A ## _TypeDummy* A;

TYPEDEF(LMState)
TYPEDEF(LMStateMem)

#undef TYPEDEF

typedef struct LMStateTransition_S {
  LMState state;
  float   penalty;
} LMStateTransition;

typedef struct LMStateTransitionArray_S {
  int                 itemN;
  LMStateTransition * itemA;
} LMStateTransitionArray;

typedef int    LMIndexFct( ClientData cd, char* name);
typedef int    LMNameNFct( ClientData cd);
typedef char*  LMNameFct(  ClientData cd, int i);
typedef float  LMScoreFct( ClientData cd, int*  w, int n, int i);

typedef char*  LMNextFrameFct( ClientData cd);
typedef char*  LMResetFct(     ClientData cd);

typedef LMState LMStateCreateFct(ClientData cd);
typedef void    LMStateFreeFct(ClientData cd, LMState);

typedef LMStateMem LMStateMemCreateFct(ClientData, LMState);
typedef void    LMStateMemFreeFct(ClientData, LMStateMem);

typedef LMState LMSetStateFct( ClientData cd, LMState State);
typedef void    LMCopyStateFct( ClientData cd, LMState toState,LMState fromState);
typedef int    LMFinalStateFct( ClientData cd);
typedef LMStateTransitionArray * LMGetNextStateFct( ClientData cd, IArray * warray, int timeStamp);
typedef int    LMStatePrintFct( ClientData cd, LMState State, char * printString);
typedef int    LMStateMemPrintFct( ClientData cd, LMStateMem State, char * printString);
typedef int    LMStateEqFct( ClientData cd, LMState State1, LMState State2);
typedef long   LMStateHashFct( ClientData cd, LMState State);

#include "astar.h"

typedef void   LMpathCreateFct( ClientData cd, AStar * searchObjectP, AStarNode* nodeP);

typedef struct LM_S
{
  /* General object definitions */

  char*             name;
  TypeInfo*         tiP;
  TypeInfo*         tiModelP;

  /* Vocabulary handling functions */

  LMIndexFct*       indexFct;
  LMNameFct*        nameFct;
  LMNameNFct*       nameNFct;
  
  /* Score functions */

  LMScoreFct*       scoreFct;

  /* Functions effecting the caching */

  LMNextFrameFct*   nextFrameFct;
  LMResetFct*       resetFct;

  /* Functions for hidden States */

  LMStateCreateFct* stateCreateFct;
  LMStateFreeFct* stateFreeFct;

  LMStatePrintFct*  statePrintFct;
  LMSetStateFct*    setStateFct;
  LMCopyStateFct*    copyStateFct;
  LMFinalStateFct*  finalStateFct;
  LMGetNextStateFct* getNextStateFct;
  LMStateEqFct*     stateEqFct;
  LMStateHashFct*   stateHashFct;

  LMStateMemCreateFct* stateMemCreateFct;
  LMStateMemFreeFct* stateMemFreeFct;
  LMStateMemPrintFct*  stateMemPrintFct;

  LMpathCreateFct*  pathCreateFct;

} LM;

extern int lmNewType( LM* mdsP);

/* Simulate a vtable for the different LM's                    */
/* Otherwise for every call four LM's have to be distinguished */

extern float lm_UgScore ( LMPtr* mP, int w1 );
extern float lm_BgScore ( LMPtr* mP, int w1, int w2 );
extern float lm_TgScore ( LMPtr* mP, int w1, int w2, int w3 );

extern void  lm_NextFrame  ( LMPtr* mP );
extern void  lm_CacheReset ( LMPtr *mP);


/* ------------------------------------------------------------------------
    LMPtr is similiar to an Object
    Basically LMPtr is a container for an object and contained object might
    have to be registered as in "link" "unlink" etc. for normal objects    

   ------------------------------------------------------------------------ */

#define linkLMPtr(X)   itfTypeCntl((ClientData)((X)->cd),((X)->lmP->tiP),T_LINK)
#define delinkLMPtr(X) itfTypeCntl((ClientData)((X)->cd),((X)->lmP->tiP),T_DELINK)
#define unlinkLMPtr(X) itfTypeCntl((ClientData)((X)->cd),((X)->lmP->tiP),T_UNLINK)
#define linkNLMPtr(X)  itfTypeCntl((ClientData)((X)->cd),((X)->lmP->tiP),T_LINKN)

extern int LM_Init ();

#endif

#ifdef __cplusplus
}
#endif
