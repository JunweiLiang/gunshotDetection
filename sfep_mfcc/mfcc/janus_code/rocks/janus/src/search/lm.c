/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Language Model
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  lm.c
    Date    :  $Id: lm.c 2436 2003-11-17 12:59:37Z metze $
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
    Revision 1.6  2003/11/17 12:59:37  metze
    Warning, go away

    Revision 1.5  2003/08/14 11:20:22  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.4.20.2  2001/04/24 09:45:41  metze
    Clean-up

    Revision 1.4.20.1  2001/02/27 15:10:27  metze
    LModelNJD and LMLA work with floats

    Revision 1.4  2000/11/14 12:29:35  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 1.3.30.2  2000/11/08 17:20:58  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 1.3.30.1  2000/11/06 10:50:33  janus
    Made changes to enable compilation under -Wall.

    Revision 1.3  2000/08/16 11:29:20  soltau
    Free -> Tcl_Free

    Revision 1.2  1998/06/11 14:22:27  kries
    add functions that conform with old LM style
    to make changes in search lock small

 * Revision 1.1  1998/05/26  18:27:23  kries
 * Initial revision
 *

   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "error.h"
#include "lm.h"

char lmRCSVersion[]= "@(#)1$Id: lm.c 2436 2003-11-17 12:59:37Z metze $";


/* ========================================================================
    LM     Language Model Interface Structure
   ======================================================================== */

static LM* lmPA[100];
static int lmN  = 0;

/* ------------------------------------------------------------------------
    lmNewType
   ------------------------------------------------------------------------ */

int lmNewType( LM* strP)
{
  int   i;
  for ( i = 0; i < lmN; i++) if ( lmPA[i] == strP) return i;

  assert( lmN < (int) sizeof(lmPA));
  lmPA[lmN] = strP;
  return      lmN++;
}

/* ------------------------------------------------------------------------
    lmList
   ------------------------------------------------------------------------ */

int lmList( ClientData clientData, Tcl_Interp *interp,
                int argc, char *argv[])
{
  int i;

  for ( i = 0; i < lmN; i++) 
    itfAppendElement("%s %s", lmPA[i]->name, lmPA[i]->tiP->name);

  return TCL_OK;
}

/* ========================================================================
    LMPtr
   ======================================================================== */
/* ------------------------------------------------------------------------
    lmPtrInit
   ------------------------------------------------------------------------ */

int lmPtrInit( LMPtr* lmP)
{
  lmP->cd   = NULL;
  lmP->lmP  = NULL;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmPtrDeinit
   ------------------------------------------------------------------------ */

int lmPtrDeinit( LMPtr* lmP)
{
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmPtrGetItf
   ------------------------------------------------------------------------ */

int lmPtrGetItf( LMPtr* mP, char* value)
{
  int     tokc;
  char**  tokv;

  if ( Tcl_SplitList( itf, value, &tokc, &tokv) == TCL_OK) {
    int    ac = tokc;
    char** av = tokv;
    char*  sn = NULL;
    char*  st = NULL;
    int    j;

    if ( itfParseArgv( "LMPtr", &ac, av, 0,
      "<lm>",    ARGV_STRING, NULL, &sn, NULL,"lm object",
      "-lmType", ARGV_STRING, NULL, &st, NULL,"lm type",
       NULL) != TCL_OK) { 
       Tcl_Free((char*)tokv);
       return TCL_ERROR;
    }

    for ( j = 0; j < lmN; j++) {
      ClientData cd = itfGetObject( sn, lmPA[j]->tiP->name);
      if ( cd) {
        if (!st || (st && !strcmp(st,lmPA[j]->name))) break;
      }
      else MSGCLEAR(NULL);
    }
    if ( j >= lmN) {
      ERROR("Object %s is not a lm type.\n",sn);
      Tcl_Free((char*)tokv);
      return TCL_ERROR;
    }
    mP->cd  = itfGetObject( sn, lmPA[j]->tiP->name);
    mP->lmP = lmPA[j];
    Tcl_Free((char*)tokv);
    return TCL_OK;
  }
  return TCL_ERROR;
}

int getLMPtr( ClientData cd, char* key, ClientData result,
	              int *argcP, char *argv[])
{
   LMPtr* strP = (LMPtr*)result;
   
   if ((*argcP < 1) || 
       ( lmPtrGetItf(strP, argv[0]) != TCL_OK)) return -1;
   (*argcP)--; return 0;
}

/* ------------------------------------------------------------------------
    LMPtrIndexItf -- ParseArgv
   ------------------------------------------------------------------------ */

int LMPtrIndexItf( ClientData CD,     char* key, 
                  ClientData retPtr, int*  argcPtr, char* argv[])
{
  LMPtr* L    = (LMPtr*)CD;
  int   idx  = -1;
  int   ac   = *argcPtr;

  if ( ac < 1) return -1;
  if ((idx = L->lmP->indexFct( L->cd, argv[0])) < 0) {
    FATAL( "Can't find item \"%s\" in the language model.\n", argv[0]);
    return idx;
  }
  (*argcPtr)--; 
  *((int*)retPtr) = idx; 
  return 0; 
}

/* ------------------------------------------------------------------------
    LMPtrAccessItf
   ------------------------------------------------------------------------ */

ClientData LMPtrAccessItf( ClientData cd, char *name, TypeInfo **ti)
{
  LMPtr* L = (LMPtr*)cd;
  LM    * lmP = L->lmP;  
  int   i;
  int size = lmP->nameNFct(L->cd);

  if (*name == '.') { 
    if (name[1]=='\0') {
      if ( size > 0) itfAppendElement( "item(0..%d)",size-1);
      itfAppendElement( "string list");
      *ti = NULL; return NULL;
    }
    else {
      name++;
      if (sscanf(name,"item(%d)",&i)==1) {
	 if ( i >= 0 && i < size)
	   itfAppendElement(lmP->nameFct(L->cd,i));
	 *ti = NULL; return NULL;
      }
      return NULL;
    }
  }
  if (*name == ':') { 
    if (name[1]=='\0') {
       for ( i = 0; i < size; i++)
         itfAppendElement( "%s", lmP->nameFct(L->cd,i));      
      *ti = NULL; return NULL;
    }
    else {
      name++;
      if ((i = lmP->indexFct(L->cd,name)) < 0) return NULL;

      *ti = NULL;
      itfAppendElement( "%d", i); 
      return NULL;
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    LMPtrNameItf
   ------------------------------------------------------------------------ */

int LMPtrNameItf(LMPtr* lmP, int argc, char * argv[])
{
  int   ac     =  argc-1;
  int   i;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<idx*>",ARGV_REST,NULL,&i,NULL,"list of indices",
		   NULL) != TCL_OK)
    return TCL_ERROR;

  while ( i++ < ac) {
    int     tokc;
    char  **tokv;
    int     j = 0;

    if ( Tcl_SplitList (itf,argv[i],&tokc,&tokv) != TCL_OK) return TCL_ERROR;

    while ( j < tokc) {
      int i = atoi(tokv[j]);

      itfAppendElement("%s", lmP->lmP->nameFct(lmP->cd , i));
      j++;
    }
    Tcl_Free((char*)tokv);
  }
  return TCL_OK;
}


/* ========================================================================
    Type Declaration
   ======================================================================== */

static int lmInitialized = 0;

int LM_Init ()
{
  if ( lmInitialized) return TCL_OK;

  Tcl_CreateCommand ( itf, "lmlist", lmList, 0, 0 );

  lmInitialized = 1;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lm_ functions -- typical uses of the LM in the standard acoustic search
   ------------------------------------------------------------------------ */

float lm_UgScore ( LMPtr* mP, int w1 ) {
  int w[1];
  w[0] = w1;
  return mP->lmP->scoreFct(mP->cd,w,1,0);
}

float lm_BgScore ( LMPtr* mP, int w1, int w2 ) {
  int w[2];
  w[0] = w1; w[1] = w2;
  return mP->lmP->scoreFct(mP->cd,w,2,1);
}


float lm_TgScore ( LMPtr* mP, int w1, int w2, int w3 ) {
  int w[3];
  w[0] = w1; w[1] = w2; w[2]=w3;
  return mP->lmP->scoreFct(mP->cd,w,3,2);
}

void  lm_NextFrame  ( LMPtr* mP ) { mP->lmP->nextFrameFct(mP->cd); }
void  lm_CacheReset  ( LMPtr* mP ) { mP->lmP->resetFct(mP->cd); }

