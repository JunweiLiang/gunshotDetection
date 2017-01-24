/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Stream
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  stream.h
    Date    :  $Id: stream.h 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.4  2003/08/14 11:20:18  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.3.56.1  2002/01/18 13:37:49  metze
    Added base-indices to SenoneSet.streamCache

    Revision 3.3  2000/08/27 15:31:29  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.2.4.1  2000/08/25 22:19:45  jmcd
    Just checking.

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

     Revision 1.3  96/04/23  19:54:11  maier
     RUNON, ready
     
     Revision 1.2  1996/04/22  13:34:28  rogina
     added a generic name structure that can be used to typecast
     all the structs that start witch "char *name", did it here
     because I needed it for the -codebookX flag in path.c

     Revision 1.1  1996/01/31  05:11:37  finkem
     Initial revision


   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _stream
#define _stream

#include "word.h"
#include "feature.h"

/* ========================================================================
    Stream
   ======================================================================== */

typedef struct Stream_S
{
  char*         name;
  TypeInfo*     tiP;

  int         (*tagFct)    ( ClientData cd, char* name);
  int         (*getFct)    ( ClientData cd, int tag, Word* wP, int left, 
                                                               int right);
  int         (*indexFct)  ( ClientData cd, char* name);
  char*       (*nameFct)   ( ClientData cd, int idX);

  int         (*scoreFct)  ( ClientData cd, int* idxA, float* scoreA,
                                            int  idxN, int    frameX);
  int         (*accuFct)   ( ClientData cd, int idx, int   frameX, 
                                                     float factor);

  int         (*updateFct) ( ClientData cd);
 
  int         (*frameNFct) ( ClientData cd, int *from, int *shift, int *ready);
  FeatureSet* (*fesUsedFct)( ClientData cd);

} Stream;

typedef struct
{
  char         *name;

} GenericName;

extern int streamNewType( Stream* strP);

/* ========================================================================
    StreamArray
   ======================================================================== */

typedef struct StreamArray_S {

  ClientData* cdA;    /* pointers to model streams (e.g. distribStream) */
  float*      stwA;   /* stream weights                                 */
  int*        cBaseA; /* base index adress in cache                     */

  Stream**    strPA;
  int         streamN;

} StreamArray;

extern int streamArrayInit(   StreamArray* staP, int n);
extern int streamArrayDeinit( StreamArray* staP);

extern int streamArrayGetItf( StreamArray* sarrayP, char* value);

extern int getStreamArray( ClientData cd, char* key, ClientData result,
	                   int *argcP, char *argv[]);
#endif

#ifdef __cplusplus
}
#endif
