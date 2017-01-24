/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: micv
               ------------------------------------------------------------

    Author  :  Andreas Eller
    Module  :  micv
    Date    :  $Id: micvFileAccess.h 2633 2005-03-08 17:30:28Z metze $
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

#ifndef _micvFileAccess
#define _micvFileAccess

#ifdef __cplusplus
extern "C" {
#endif

#include "micv.h"


// load / save MicvSet
int micvset_save(MicvSet* pMicvSet, char* sFilename);
int micvset_load(MicvSet* pMicvSet, char* sFilename);

// load MICV description file
int micvset_writeDesc(MicvSet* pMicvSet, char* sFilename);
int micvset_readDesc(MicvSet* pMicvSet, char* sFilename);

// load / save accu
int micvset_saveAccus(MicvSet* pMicvSet, char* sFilename);
int micvset_loadAccus(MicvSet* pMicvSet, char* sFilename);

// write model list
int micvset_writeModelList(MicvSet* pMicvSet, char* sFilename);

// helper functions
void micvset_saveMatrix(FILE* pFile, DMatrix* pMatrix);
void micvset_loadMatrix(FILE* pFile, DMatrix* pMatrix);
void micvset_saveVector(FILE* pFile, DVector* pVector);
void micvset_loadVector(FILE* pFile, DVector* pVector);
void micvset_saveString(FILE* pFile, char* str);
void micvset_loadString(FILE* pFile, char* str, const int nMaxSize);


#ifdef __cplusplus
}
#endif

#endif	// _micvFileAccess

