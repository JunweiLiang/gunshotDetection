/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Stream
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  stream.c
    Date    :  $Id: stream.c 3070 2010-01-20 00:35:04Z metze $
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
    Revision 3.5  2003/08/14 11:20:18  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.4.20.3  2003/01/02 15:19:41  metze
    Corrected Allocations

    Revision 3.4.20.2  2002/06/26 11:57:56  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.4.20.1  2002/01/18 13:37:26  metze
    Added base-indices to SenoneSet.streamCache

    Revision 3.4  2000/11/14 12:35:26  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 3.3.34.2  2000/11/08 17:31:22  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 3.3.34.1  2000/11/06 10:50:40  janus
    Made changes to enable compilation under -Wall.

    Revision 3.3  2000/08/16 11:42:05  soltau
    Free -> TclFree

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

     Revision 1.2  96/02/05  09:28:28  manke
     Replaced staP->stwA[i]  = NULL; by staP->stwA[i]  = 0.0; in line 113.
     It doesn't make sense to assign NULL to a float.
     
     Revision 1.1  1996/01/31  05:11:37  finkem
     Initial revision


   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "stream.h"
#include "distribStream.h"
#ifndef IBIS
#include "hmeStream.h"
#endif

char streamRCSVersion[]= 
           "@(#)1$Id: stream.c 3070 2010-01-20 00:35:04Z metze $";


/* ========================================================================
    Stream
   ======================================================================== */

static Stream* streamPA[100];
static int     streamN  = 0;

/* ------------------------------------------------------------------------
    streamNewType
   ------------------------------------------------------------------------ */

int streamNewType( Stream* strP)
{
  int   i;
  for ( i = 0; i < streamN; i++) if ( streamPA[i] == strP) return i;

  assert(  streamN < 98);
  streamPA[streamN] = strP;
  return   streamN++;
}

/* ------------------------------------------------------------------------
    streamList
   ------------------------------------------------------------------------ */

int streamList( ClientData clientData, Tcl_Interp *interp,
                int argc, char *argv[])
{
  int i;

  for ( i = 0; i < streamN; i++) 
    itfAppendElement("%s %s", streamPA[i]->name, streamPA[i]->tiP->name);

  return TCL_OK;
}

/* ========================================================================
    StreamArray
   ======================================================================== */
/* ------------------------------------------------------------------------
    streamArrayInit
   ------------------------------------------------------------------------ */

int streamArrayInit( StreamArray* staP, int n)
{
  staP->cdA     = NULL;
  staP->stwA    = NULL;
  staP->strPA   = NULL;
  staP->cBaseA  = NULL;
  staP->streamN = n;

  if (n) {
    int i;

    staP->cdA    = (ClientData*) calloc (n,   sizeof (ClientData));
    staP->stwA   = (float     *) calloc (n,   sizeof (float*));
    staP->strPA  = (Stream   **) calloc (n,   sizeof (Stream*));
    staP->cBaseA = (int       *) calloc (n+1, sizeof (int));

    for ( i = 0; i < n; i++) {
      staP->cdA[i]    = NULL; 
      staP->stwA[i]   = 0.0; 
      staP->strPA[i]  = NULL; 
      staP->cBaseA[i] = -1;
    }
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    streamArrayDeinit
   ------------------------------------------------------------------------ */

int streamArrayDeinit( StreamArray* staP)
{
  if ( staP->cdA)    { free( staP->cdA);    staP->cdA    = NULL; }
  if ( staP->stwA)   { free( staP->stwA);   staP->stwA   = NULL; }
  if ( staP->strPA)  { free( staP->strPA);  staP->strPA  = NULL; }
  if ( staP->cBaseA) { free( staP->cBaseA); staP->cBaseA = NULL; }
  staP->streamN = 0;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    streamArrayGetItf
   ------------------------------------------------------------------------ */

int streamArrayGetItf( StreamArray* sarrayP, char* value)
{
  int     argc;
  char**  argv;

  if ( Tcl_SplitList( itf, value, &argc, &argv) == TCL_OK) {
    int maxC = 0;
    int    i;

    if ( sarrayP->streamN > 0) {
      if ( sarrayP->streamN != argc) {
        ERROR("Expected StreamArray of size %d but got size %d.\n", 
               sarrayP->streamN, argc);
        return TCL_ERROR;
      }
    } else {
      sarrayP->cdA     = (ClientData*)calloc( argc,   sizeof(ClientData));
      sarrayP->stwA    = (float     *)calloc( argc,   sizeof(float));
      sarrayP->strPA   = (Stream   **)calloc( argc,   sizeof(Stream*));
      sarrayP->cBaseA  = (int       *)calloc( argc+1, sizeof(int));
      sarrayP->streamN =  argc;
    }

    for ( i = 0; i < argc; i++) {
      int     tokc;
      char**  tokv;

      if ( Tcl_SplitList( itf, argv[i], &tokc, &tokv) == TCL_OK) {
        int    ac = tokc;
        char** av = tokv;
        char*  sn = NULL;
        char*  st = NULL;
        float  w  = 1.0;
        int   j;

        if ( itfParseArgv( argv[0], &ac, av, 0,
            "<stream>",    ARGV_STRING, NULL, &sn, NULL,"stream object",
            "-streamType", ARGV_STRING, NULL, &st, NULL,"stream type",
            "-weight",     ARGV_FLOAT,  NULL, &w,  NULL,"stream weight",
             NULL) != TCL_OK) {
          free((char*)argv); free((char*)tokv);
          return TCL_ERROR;
        }

        for (j = 0; j < streamN; j++) {
          ClientData cd = itfGetObject (sn, streamPA[j]->tiP->name);
          if (cd) {
            if (!st || (st && !strcmp (st, streamPA[j]->name))) break;
          } else
	    MSGCLEAR(NULL);
        }

        if (j >= streamN) {
          ERROR("Object %s is not a stream type.\n",sn);
          free ((char*)argv); free ((char*)tokv);
          return TCL_ERROR;
        }

        sarrayP->cdA[i]      = itfGetObject (sn, streamPA[j]->tiP->name);
        sarrayP->stwA[i]     = w;
        sarrayP->strPA[i]    = streamPA[j];

	/* Fill in the start positions for this stream in the
	   cache array. For DistribStreams, find the ones 
	   referring to identical distributions and store them 
	   in the same lines */
	sarrayP->cBaseA[i] = maxC;

	if        (streq (streamPA[j]->tiP->name, "DistribStream")) { 
	  DistribStream*   dssI = (DistribStream*) sarrayP->cdA[i];

	  /* Re-use existing cache? */
	  for (j = 0; j < i; j++) {
	    DistribStream* dssJ = (DistribStream*) sarrayP->cdA[j];
	    if (dssI->dssP == dssJ->dssP) {
	      sarrayP->cBaseA[i] = sarrayP->cBaseA[j];
	      break;
	    }
	  }

	  /* No, nothing found */
	  if (j == i) {
	    sarrayP->cBaseA[i] = maxC;
	    maxC += dssI->dssP->list.itemN;
	  }

#ifndef IBIS
	} else if (streq (streamPA[j]->tiP->name, "HmeStream")) {
	  sarrayP->cBaseA[i+1] = sarrayP->cBaseA[i] +
	    ((HmeStream*)     sarrayP->cdA[i])->hmeSet->list.itemN;
#endif
	} else
	  sarrayP->cBaseA[0] = -1;

	Tcl_Free ((char*) tokv);
      }
    }
    sarrayP->cBaseA[sarrayP->streamN] = maxC;

    Tcl_Free((char*)argv);
    return TCL_OK;
  }
  return TCL_ERROR;
}

int getStreamArray( ClientData cd, char* key, ClientData result,
	            int *argcP, char *argv[])
{
   StreamArray* strP = (StreamArray*)result;
   
   if ((*argcP < 1) || 
       ( streamArrayGetItf(strP, argv[0]) != TCL_OK)) return -1;
   (*argcP)--; return 0;
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static int streamInitialized = 0;

int Stream_Init ( )
{
  if ( streamInitialized) return TCL_OK;

  Tcl_CreateCommand ( itf, "streams", streamList, 0, 0 );

  streamInitialized = 1;
  return TCL_OK;
}

