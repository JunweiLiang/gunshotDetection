/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Hypotheses Support
               ------------------------------------------------------------
               
    Author  :  Monika Woszczyna
    Date    :  $Id: hypos.c 2390 2003-08-14 11:20:32Z fuegen $
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
   Revision 3.7  2003/08/14 11:20:22  fuegen
   Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

   Revision 3.6.20.1  2002/06/26 11:57:58  fuegen
   changes for new directory structure support and code cleaning

   Revision 3.6  2000/11/14 12:29:33  janus
   Merging branch jtk-00-10-27-jmcd.

   Revision 3.5.34.2  2000/11/08 17:15:39  janus
   Finished making changes required for compilation under 'gcc -Wall'.

   Revision 3.5.34.1  2000/11/06 10:50:29  janus
   Made changes to enable compilation under -Wall.

   Revision 3.5  2000/08/16 09:23:29  soltau
   free -> Tcl_Free

   Revision 3.4  1998/05/26 18:27:23  kries
   adaptation to new language model interface
   added support for state information

 * Revision 3.3  1997/07/18  17:57:51  monika
 * gcc-clean
 *
   Revision 3.2  1997/06/17 17:41:43  westphal
   *** empty log message ***

    Revision 1.15  96/05/30  21:48:06  monika
    removed bug in creating HypoLists from tcl level
    (missing NULL termination of parseArgV call)
    
    also changed tcl output format to quote characters that
    tcl would normaly quote.
    
    Revision 1.14  96/04/23  19:43:01  19:43:01  maier (Martin Maier)
    monika: changes for partial hypos
    
    Revision 1.13  96/02/17  19:25:21  19:25:21  finkem (Michael Finke)
    finally cleaned up the old hypoList stuff
    
    Revision 1.12  1996/01/08  14:49:22  monika
    changed args to params for orthogonal behaviour in publish functions
    changed behaviour of default function to write to file if parameter
    filename is present

    Revision 1.11  1996/01/07  17:56:54  monika
    complete rework to configure hypoLists with setPublish
    not compatible with old scripts !!

    Revision 1.10  1995/11/16  12:06:16  rogina
    added if (!fp) where it was missing

    Revision 1.9  1995/10/04  23:40:28  torsten
    *** empty log message ***

    Revision 1.8  1995/09/13  18:51:24  finkem
    introduced utterance id string for publishing methods

    Revision 1.7  1995/09/06  07:31:27  kemp
    *** empty log message ***

    Revision 1.6  1995/08/29  02:45:16  finkem
    *** empty log message ***

    Revision 1.5  1995/08/27  22:36:50  finkem
    *** empty log message ***

    Revision 1.4  1995/08/17  17:08:35  rogina
    *** empty log message ***

    Revision 1.3  1995/08/07  20:57:20  finkem
    *** empty log message ***

    Revision 1.2  1995/08/06  21:03:42  finkem
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

   ======================================================================== */

char hyposRCSVersion[] = "$Id: hypos.c 2390 2003-08-14 11:20:32Z fuegen $";

#include "common.h"
#include "tcl.h"
#include "itf.h"
#include "hypos.h"
#include "vocab.h"


/* ==========================================================================

  CURRENTLY BEING COMPLETELY REMODELED


  Description:

  This module is to help with the actual output of a speech recognition system.
  It offers a multitude of output formats and streams that you might need
  to communicate current or final results of the seach module to other
  programs.

  ----------------------------------------------------------------------------

  As c-user you have to allocate or create a HypoList or get one from a
  tcl user. You can create and fill in solo instances of hypos, and then
  append them to an existing hypolist. If you think it's a smart time to
  print out the hypos contents somehow to somewhere, call the hypoListPublish
  funktion and specify wether this is a temporary or final output.
  Important note: Hypolists provide allocate space for hypopointers in
  blocks of a size specified in element allocBlockSize; If you allocate
  a HypoList yourself, make sure that you call hypoListInit to set this
  value BEFORE you use the list for whatever.
  
  Hypo * hypoCreate ( int hwordN );
      create a hypothesis, for instance before you fill in the information
      and then append it to a Hypolist. Note that tcl users have only very
      limited access to this type of object.
      hwordN is the number of words this hypo has.
  
  int hypoFree ( Hypo *hypoP );
      destroy the hypothesis pointed to by hypoP. This frees all the data
      occupied by the object.
    
  HypoList * hypoListCreate (Vocab *vocabP, int blocksize, char *formatP);
      create a HypoList defined over the vocabulary vocabP; the blocksize
      is the number of hypopointers allocated at one time, the formatP
      is a pointer to a string that contains the default hypo format for the
      final hypothesis as a 2 dimensional tcl-list. NONE, MINI and PLAIN are
      predefined in hypos.h;
      The default format for temporary hypothesis is always NULL;
  
  int hypoListInit ( HypoList *hypoListP, Vocab *vocabP,
                                    int blockSize, char *format );
      If you decide to provide the space for the HypoList yourself,
      use hypoListInit to initialize the datastructure before you use any
      other functions of this module. Parameters see hypoListCreate;
      Call this function only ONCE for every hypolist or after calling
      hypoListDeinit.
                   
  int hypoListReinit ( HypoList *hypoListP );
      Resets the HypoList to the state similar to the one it had after Init.
      Note that the formatstrings and the blocksize are kept in order not
      to destroy preference settings by the user. You'll have to
      use hypoListDeinit to free all memory.
  
  int hypoListDeinit ( HypoList *hypoListP );
      Free all memory used by a HypoList. Do this before you destroy
      data structures that contain a HypoList rather than point to it
      to free all memory.
  
  int hypoListFree ( HypoList *hypoListP );
      Free a HypoList created by hypoListCreate;
      
  int hypoListAppend ( HypoList *hypoListP, Hypo *hypoP );
      Append a complete, allocated Hypo to the hypolist. Allocate
      more Pointers to Hypos if necessary. It also sets the containerP
      to the Parent structure.

   ======================================================================== */

extern TypeInfo hypoInfo;

/* ========================================================================
    Hypo
   ======================================================================== */
/* ------------------------------------------------------------------------
    hypoCreate
   ------------------------------------------------------------------------ */

int hypoInit( Hypo* hypoP, int hwordN)
{
  int i;

  hypoP->score     =  0.0;
  hypoP->hwordN    =  hwordN;
  hypoP->hwordA    = (HWord   *)malloc( hwordN * sizeof(HWord));
  hypoP->moreInfoA = (MoreInfo*)malloc( hwordN * sizeof(MoreInfo));
  hypoP->hypoListP =  NULL;

  for(i=0;i<hwordN;i++)
    hypoP->hwordA[i].stateString = NULL;

  return TCL_OK;
}

Hypo* hypoCreate ( int hwordN )
{
  Hypo* hypoP = (Hypo*)malloc(sizeof(Hypo));

  if (! hypoP || hypoInit( hypoP, hwordN) != TCL_OK) {
    if ( hypoP) free( hypoP);
    ERROR("Failed to allocate Hypo object.");
    return NULL; 
  }  
  return hypoP;
}

/* ------------------------------------------------------------------------
    hypoFree
   ------------------------------------------------------------------------ */

int hypoDeinit( Hypo* hypoP)
{
  assert( hypoP);

  if (hypoP->hwordA)    {
    int i;
    for(i=0;i<hypoP->hwordN;i++) free(hypoP->hwordA[i].stateString);    
    free(hypoP->hwordA);    hypoP->hwordA    = NULL;
  }
  if (hypoP->moreInfoA) { free(hypoP->moreInfoA); hypoP->moreInfoA = NULL; }
  return TCL_OK;
}

int hypoFree ( Hypo* hypoP )
{
  if ( hypoDeinit( hypoP) != TCL_OK) return TCL_ERROR;
  free(hypoP);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    hypoGetItf
   ------------------------------------------------------------------------ */

int hypoGetItf( ClientData cd, char* key, ClientData result, 
                               int*  acP, char* av[])
{
  HypoList* hypoListP = (HypoList*)cd;
  Hypo*     hypoP;
  char*     value     = av[0];
  int       i;

  assert (key == key); /* not used here, keep compiler quiet */

  *((Hypo**)result)   = NULL;

  if (*acP < 1) return -1;

  if ((hypoP = (Hypo*)itfGetObject( value, "Hypo")) == NULL) {
    int     argc;
    char**  argv;

    MSGCLEAR(NULL);

    if ( Tcl_SplitList( itf, value, &argc, &argv) == TCL_OK) {
      int i;

      if (! (hypoP = hypoCreate( argc-1))) return TCL_ERROR;
      hypoP->hypoListP = hypoListP;
      hypoP->score     = atof(argv[0]);

      for ( i = 1; i < argc; i++) {
        int    tokc, vocabX;
        char** tokv;

        hypoP->hwordA[i-1].wordScore = 0.0;
        hypoP->hwordA[i-1].wsFrameXF = 0;
        hypoP->hwordA[i-1].weFrameXF = 0;
	if(hypoP->hwordA[i-1].stateString != NULL) {
	  free(hypoP->hwordA[i-1].stateString);
	  hypoP->hwordA[i-1].stateString = NULL;
	}

        if ( Tcl_SplitList( itf, argv[i], &tokc, &tokv) != TCL_OK) {
          SERROR( "Error parsing hypo item nr %d: \'%s\'\n",i, argv[i]);
          hypoFree( hypoP);
          Tcl_Free((char*)argv); return TCL_ERROR;
        }
        if ( itfParseArgv("hypoitem", &tokc, tokv, 0,
             "<word>", ARGV_LINDEX, NULL, &(vocabX), 
             &(hypoListP->vocabP->vwordList), "word in vocabulary",
             "<score>", ARGV_FLOAT, NULL, &(hypoP->hwordA[i-1].wordScore),
             NULL, "word score",
             "<from>",  ARGV_SHORT, NULL, &(hypoP->hwordA[i-1].wsFrameXF),
             NULL, "start frame",
             "<to>",    ARGV_SHORT, NULL, &(hypoP->hwordA[i-1].weFrameXF),
             NULL, "end frame",
             "-state",    ARGV_STRING, NULL, &(hypoP->hwordA[i-1].stateString),
             NULL, "state string",
			  NULL) != TCL_OK) {

          SERROR( "Error parsing hypo item nr %d: \'%s\'\n",i, argv[i]);

          hypoFree( hypoP);
          Tcl_Free((char*)tokv);
          Tcl_Free((char*)argv); return TCL_ERROR;
        }
        hypoP->hwordA[i-1].vocabX = vocabX;
	if(hypoP->hwordA[i-1].stateString)
	  hypoP->hwordA[i-1].stateString = strdup(hypoP->hwordA[i-1].stateString);
        free((char*)tokv);
      }
    }
    Tcl_Free((char*)argv);
    *((Hypo**)result) = hypoP;
    (*acP)--;    return TCL_OK;
  }
  else {
    Hypo* nhypoP = hypoCreate( hypoP->hwordN);

    if (! nhypoP) return TCL_ERROR;
    nhypoP->hypoListP = hypoP->hypoListP;

    memcpy( nhypoP->hwordA,    hypoP->hwordA,    hypoP->hwordN*sizeof(HWord));
    memcpy( nhypoP->moreInfoA, hypoP->moreInfoA, hypoP->hwordN*
                                                 sizeof(MoreInfo));
    for(i=0;i<hypoP->hwordN;i++)
      if(hypoP->hwordA[i].stateString)
	nhypoP->hwordA[i].stateString = strdup(hypoP->hwordA[i].stateString);
    nhypoP->score = hypoP->score;
    *((Hypo**)result) = nhypoP;
    (*acP)--;    return TCL_OK;
  }
  return(TCL_OK);
}

/* ------------------------------------------------------------------------
    hypoPutsItf
   ------------------------------------------------------------------------ */


/* ========================================================================
    HypoList
   ======================================================================== */

static HypoList hypoListDefaults = {

 NULL, /* char*   name             */
 0,    /* int     useN             */
 0,    /* short   hypoN            */
 0,    /* short   allocN           */
 0,    /* short   allocBlockSize   */
 NULL, /* Hypo**  hypoPA           */
 NULL, /* char*   idS              */

 NULL, /* FILE*   tempHandle       */
 0,    /* int     useTempHandle    */
 NULL  /* Vocab*  vocabP           */

};


/* ------------------------------------------------------------------------
    hypoListInit
   ------------------------------------------------------------------------ */

int hypoListInit ( HypoList* hypoListP, char* name, Vocab* vocabP,
                   int blockSize)
{
  assert( hypoListP);

  hypoListP->name             = strdup(name);
  hypoListP->useN             = 0;
  hypoListP->hypoN            = 0;
  hypoListP->allocN           = 0;
  hypoListP->allocBlockSize   = blockSize;
  hypoListP->hypoPA           = NULL;
  hypoListP->idS              = NULL;
  hypoListP->vocabP           = vocabP; link( vocabP, "Vocab");
  hypoListP->tempHandle       = NULL;
  hypoListP->useTempHandle    = 0;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    hypoListReinit
   ------------------------------------------------------------------------ */

int hypoListReinit( HypoList * hypoListP )
{
  int hypoX;
  if (! hypoListP) return TCL_ERROR;
  
  for (hypoX = 0; hypoX < hypoListP->hypoN; hypoX ++) {
    if (hypoFree( hypoListP->hypoPA[hypoX]) != TCL_OK) return TCL_ERROR;
  }
  if ( hypoListP->hypoPA) { 
       free(hypoListP->hypoPA); 
       hypoListP->hypoPA = NULL;
  }

  hypoListP->allocN = 0;
  hypoListP->hypoN  = 0;

  if ( hypoListP->tempHandle) {
    fclose(hypoListP->tempHandle);
    hypoListP->tempHandle = NULL;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    hypoListCreate
   ------------------------------------------------------------------------ */

HypoList* hypoListCreate( char* name, Vocab *vocabP, int blocks)
{
  HypoList *hypoListP = (HypoList*)malloc(sizeof(HypoList));

  if (! hypoListP || hypoListInit( hypoListP,name,vocabP,
                                   blocks) != TCL_OK) {
    if ( hypoListP) free( hypoListP);
    ERROR("Failed to allocate HypoList object '%s'\n.", name);
    return NULL;
  }
  return ( hypoListP );
}

static ClientData hypoListCreateItf( ClientData cd, int argc, char *argv[])
{
  Vocab* vocabP = NULL;
  int    ac     = argc-1;

  assert(cd == cd); /* not used, keep compiler quiet */

  if (itfParseArgv( argv[0], &ac,  argv+1, 0,
     "<Vocab>" ,       ARGV_OBJECT, NULL, &vocabP, "Vocab", "vocabulary",NULL) 
     != TCL_OK) return NULL;

  return (ClientData)hypoListCreate (argv[0],vocabP, 1);
}

/* ------------------------------------------------------------------------
    hypoListDeinit
   ------------------------------------------------------------------------ */

int hypoListDeinit ( HypoList *hypoListP )
{
  if (hypoListP==NULL) { ERROR("null argument"); return TCL_ERROR; }

  if (hypoListP->useN) {
    SERROR("HypoList object %s still in use by %d other object(s).\n",hypoListP->name,hypoListP->useN);
    return TCL_ERROR;
  }
  if ( hypoListReinit( hypoListP) != TCL_OK) return TCL_ERROR;

  unlink( hypoListP->vocabP, "Vocab");

  if (hypoListP->idS)  { free( hypoListP->idS);  hypoListP->idS  = NULL; }
  if (hypoListP->name) { free( hypoListP->name); hypoListP->name = NULL; }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    hypoListFree
   ------------------------------------------------------------------------ */

static int hypoListFree ( HypoList *hypoListP )
{
  if (hypoListDeinit (hypoListP)!=TCL_OK) return TCL_ERROR;
  if (hypoListP->useN !=0)   
      ERROR("Freeing HypoList %s that has %d links\n",
             hypoListP->name,hypoListP->useN);
  free (hypoListP);
  return TCL_OK;
}

static int hypoListFreeItf( ClientData cd )
{
  HypoList* hypoListP = (HypoList *) cd;
  return hypoListFree ( hypoListP );
}

/* ------------------------------------------------------------------------
    hypoListConfigureItf
   ------------------------------------------------------------------------ */

static int hypoListConfigureItf( ClientData cd, char *var, char *val)
{
  HypoList* hypoListP = (HypoList*)cd;
  if (! hypoListP) hypoListP = &hypoListDefaults;

  if (! var) {
    ITFCFG(hypoListConfigureItf,cd,"name");
    ITFCFG(hypoListConfigureItf,cd,"useN");
    ITFCFG(hypoListConfigureItf,cd,"hypoN");
    ITFCFG(hypoListConfigureItf,cd,"allocBlockSize");
    ITFCFG(hypoListConfigureItf,cd,"id");
    ITFCFG(hypoListConfigureItf,cd,"tempFormat");
    ITFCFG(hypoListConfigureItf,cd,"finalFormat");
    ITFCFG(hypoListConfigureItf,cd,"vocab");
    return TCL_OK;
  }
  ITFCFG_CharPtr( var,val,"name",          hypoListP->name,             1);
  ITFCFG_Int    ( var,val,"useN",          hypoListP->useN,             1);
  ITFCFG_Int    ( var,val,"hypoN",         hypoListP->hypoN,            1);
  ITFCFG_Int    ( var,val,"allocBlockSize",hypoListP->allocBlockSize,   1);
  ITFCFG_CharPtr( var,val,"id",            hypoListP->idS,              0);
  ITFCFG_Object ( var,val,"vocab",         hypoListP->vocabP,name,Vocab,1);

  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}

static int hypoConfigureItf( ClientData cd, char *var, char *val)
{
  Hypo* hypoP = (Hypo*)cd;

  if (! var) {
    ITFCFG(hypoConfigureItf,cd,"score");
    ITFCFG(hypoConfigureItf,cd,"hwordN");
    return TCL_OK;
  }
  ITFCFG_Float  ( var,val,"score",         hypoP->score,           0);
  ITFCFG_Int    ( var,val,"hwordN",        hypoP->hwordN,          1);
  
  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    hypoListAccessItf
   ------------------------------------------------------------------------ */

static ClientData hypoListAccessItf (ClientData cd, char *name, TypeInfo **ti )
{
  HypoList* hypoList = (HypoList*)cd;

  if (*name=='.')                          /* handle '.' subobjects */
  { if (name[1]=='\0') 
    { itfAppendResult("vocab");
      *ti = NULL;
      return NULL;
    }
    else if (strcmp(name,".vocab") ==0) {
      *ti = itfGetType("Vocab");
      return (ClientData)hypoList->vocabP;
    }
  }
  if (*name==':') {                        /* handle ':' subobjects */
    name++;
    if (*name=='\0') { 
      itfAppendResult("%d",hypoList->hypoN); 
      *ti = NULL; return NULL; 
    }
    else if ((strcmp(name,"hypoN") == 0)) {
      itfAppendResult("%d\n",hypoList->hypoN); 
      *ti = NULL; return NULL; 
    }
    else {
      int hypoX = atoi(name);
      *ti = &hypoInfo;
      if (hypoX < hypoList->hypoN) return (ClientData) hypoList->hypoPA[hypoX];
    }
  }
  return NULL;
}


/* ------------------------------------------------------------------------
    hypoListAppend
   ------------------------------------------------------------------------ */

int hypoListAppend ( HypoList *hypoListP, Hypo *hypoP)
{
  if (! hypoListP) return -1;

  hypoListP->hypoN++;
  if (hypoListP->hypoN >=  hypoListP->allocN) {
    hypoListP->allocN  +=  hypoListP->allocBlockSize;
    hypoListP->hypoPA   = (Hypo**)realloc(hypoListP->hypoPA, 
                                          hypoListP->allocN * sizeof(Hypo*));
  }
  hypoListP->hypoPA[hypoListP->hypoN-1]            = hypoP;
  hypoListP->hypoPA[hypoListP->hypoN-1]->hypoListP = hypoListP;
  
  return hypoListP->hypoN-1;
}

static int hypoListAppendItf ( ClientData cd, int argc, char *argv[] )
{
  HypoList* hypoListP = (HypoList*)cd;
  Hypo*     hypoP;
  int       ac        = argc-1;

  if (itfParseArgv( argv[0], &ac,  argv+1,  2,
     "<hypo>", ARGV_CUSTOM, hypoGetItf, &hypoP, hypoListP, "hypo", 
      NULL) != TCL_OK) return TCL_ERROR;

  if ( hypoListAppend(hypoListP,hypoP) < 0) return TCL_ERROR;
  else                                      return TCL_OK;
}

/*=========================================================================*/
/*=========================================================================*/
static int hypoPutsSimple (Hypo * hypoP, Vocab * vocabP)
{
  int hwordX;
  for (hwordX = 0; hwordX < hypoP->hwordN; hwordX ++) {
    char* spellS = vocabName (vocabP,hypoP->hwordA[hwordX].vocabX);
    if(hypoP->hwordA[hwordX].stateString)
      itfAppendResult( "%s %s", spellS, hypoP->hwordA[hwordX].stateString);
    else
      itfAppendResult( "%s ", spellS );
  }
  return TCL_OK;
}

static int hypoPutsNormal (Hypo * hypoP, Vocab * vocabP, char * strP)
{
  int hwordX;
  for (hwordX = 0; hwordX < hypoP->hwordN; hwordX ++) {
    char* spellS = vocabName (vocabP,hypoP->hwordA[hwordX].vocabX);
    if(hypoP->hwordA[hwordX].stateString)
      itfAppendResult( "%s %s", spellS, hypoP->hwordA[hwordX].stateString);
    else
      itfAppendResult( "%s ", spellS );
  }
  itfAppendResult(" (hypo %s | score = %7.2f)\n",strP,hypoP->score);
  return TCL_OK;
}

static int hypoPutsVerbose (Hypo * hypoP, Vocab * vocabP, char * strP)
{
  int hwordX;

  itfAppendResult("%s (score = %7.2f)\n",strP,hypoP->score  );
  for (hwordX = 0; hwordX < hypoP->hwordN; hwordX ++) {
    char* spellS = vocabName (vocabP,hypoP->hwordA[hwordX].vocabX);  
    if(hypoP->hwordA[hwordX].stateString)
      itfAppendResult (" %-20s\t{%s}\t%7.2f\t%4d -> %4d\n",
		       spellS, hypoP->hwordA[hwordX].stateString,
		       hypoP->hwordA[hwordX].wordScore,
		       hypoP->hwordA[hwordX].wsFrameXF, hypoP->hwordA[hwordX].weFrameXF
		       );
    else
      itfAppendResult (" %-20s\t%7.2f\t%4d -> %4d\n",
		       spellS,
		       hypoP->hwordA[hwordX].wordScore,
		       hypoP->hwordA[hwordX].wsFrameXF, hypoP->hwordA[hwordX].weFrameXF
	);
  }
  itfAppendResult("\n");
  return TCL_OK;
}

static int hypoPutsTcl (Hypo* hypoP, Vocab* vocabP)
{
  int    hwordX;

  itfAppendResult(" %7.2f\n", hypoP->score);
  for (hwordX = 0; hwordX < hypoP->hwordN; hwordX ++) {
    char* spellS = vocabName( vocabP,hypoP->hwordA[hwordX].vocabX);
    itfAppendResult(" {");
    itfAppendElement("%s",spellS); /* put words with funny chars into {} */
    if(hypoP->hwordA[hwordX].stateString)
      itfAppendElement("%s",hypoP->hwordA[hwordX].stateString); /* put words with funny chars into {} */      
    itfAppendResult(
        " %7.3f %d %d}\n",
        hypoP->hwordA[hwordX].wordScore,
        hypoP->hwordA[hwordX].wsFrameXF, hypoP->hwordA[hwordX].weFrameXF
    );
  }
  return TCL_OK;
}

static int hypoPuts(Hypo * hypoP,  char * styleS, char * hypoXS)
{
  if (strcmp(styleS,"tcl")==0) {
    return(hypoPutsTcl(hypoP,hypoP->hypoListP->vocabP));
  }
  else if (strcmp(styleS,"simple")==0) {
    return(hypoPutsSimple(hypoP,hypoP->hypoListP->vocabP));
  }
  else if (strcmp(styleS,"normal")==0) {
    return(hypoPutsNormal(hypoP,hypoP->hypoListP->vocabP,hypoXS));
  }
  else if (strcmp(styleS,"verbose")==0) {
    return(hypoPutsVerbose(hypoP,hypoP->hypoListP->vocabP,hypoXS));
  }
  return TCL_OK;
}

int hypoListPuts( HypoList * hypoListP, char * styleS, char* idS)
{
  int hypoX;
  char hypoIdS[1000];

  if (strcmp(styleS,"tcl")==0) {
     itfAppendResult ("%s\n",idS);
     for (hypoX = 0; hypoX < hypoListP->hypoN; hypoX ++) {
       sprintf(hypoIdS,"%d",hypoX);
       itfAppendResult("{\n");
       hypoPutsTcl(hypoListP->hypoPA[hypoX],hypoListP->vocabP);
       itfAppendResult("}\n");
     }
  }
  else if (strcmp(styleS,"simple")==0) {
     for (hypoX = 0; hypoX < hypoListP->hypoN; hypoX ++) {
       hypoPutsSimple(hypoListP->hypoPA[hypoX],hypoListP->vocabP);
       itfAppendResult("\n");
     }
  }
  else if (strcmp(styleS,"normal")==0) {
     if (idS) itfAppendResult("%%TURN: %s\n",idS);
     for (hypoX = 0; hypoX < hypoListP->hypoN; hypoX ++) {
       sprintf(hypoIdS,"%d",hypoX);
       hypoPutsNormal(hypoListP->hypoPA[hypoX],hypoListP->vocabP,hypoIdS);
     }
     itfAppendResult("\n");
  }
  else if (strcmp(styleS,"verbose")==0) {
     if (idS) itfAppendResult("%%TURN:%s\n",idS);
     INFO("alive\n");
     for (hypoX = 0; hypoX < hypoListP->hypoN; hypoX ++) { 
       INFO("alive -- %d\n",hypoX);
       sprintf(hypoIdS,"hypo %d",hypoX);
       hypoPutsVerbose(hypoListP->hypoPA[hypoX],hypoListP->vocabP,hypoIdS);
     }
     itfAppendResult("\n");
  }
  return TCL_OK;
}

static int hypoPutsItf( ClientData cd, int argc, char *argv[])
{
  Hypo*     hypoP =   (Hypo*)cd;
  int       ac        =  argc-1;
  char*     styleS    = "tcl";
  char*     hypoXS    = "0";

  if (itfParseArgv( argv[0], &ac,  argv+1,  0,
     "-style",    ARGV_STRING, NULL, &styleS, NULL, 
          "simple|normal|verbose|tcl",
     "-id",       ARGV_STRING, NULL, &hypoXS   ,NULL,
          "id of this single hypo (eg hypo-nr)",
     NULL) != TCL_OK) return TCL_ERROR;

  return(hypoPuts(hypoP,styleS,hypoXS));
}

static int hypoListPutsItf( ClientData cd, int argc, char *argv[])
{
  HypoList* hypoListP = (HypoList*)cd;
  int       ac        =  argc-1;
  char*     styleS    = "tcl";
  char*     idS       =  "NULL";

  if (itfParseArgv( argv[0], &ac,  argv+1,  0,
     "-style",    ARGV_STRING, NULL, &styleS, NULL, 
          "simple|normal|verbose|tcl",
     "-id",       ARGV_STRING, NULL, &idS   ,NULL,
          "id of the whole hypolist (eg TURN-id)",
     NULL) != TCL_OK) return TCL_ERROR;

  return(hypoListPuts(hypoListP,styleS,idS));
}

static int hypoListSetPublishItf( ClientData cd, int argc, char *argv[])
{
  HypoList* hypoListP = (HypoList*)cd;
  int       ac        =  argc-1;
  char*     cmdS       =  NULL;
  DString   functionS;
  int ret;

  assert (hypoListP == hypoListP); /* not used here, keep compiler quiet */

  if (itfParseArgv( argv[0], &ac,  argv+1,  0,
     "<cmdlist>",    ARGV_STRING, NULL, &cmdS, NULL, 
          "list of commands to use",
     NULL) != TCL_OK) return TCL_ERROR;

   dstringInit(&functionS);
   dstringAppend(&functionS,
         "proc hypoListPublish<%s> {hl params} {makeArray arg $params\n%s}",
                                                  itfCommandName(), cmdS);
  
  ret = Tcl_Eval(itf,dstringValue(&functionS));
  dstringFree(&functionS);
  return(ret);
}

int hypoListPublish(char* objnameS)
{
    char str[510];
    sprintf(str,"%s publish {{}}",objnameS);
    return(Tcl_Eval(itf,str));
}

/* ========================================================================
    Hypos ITF Initialization
   ======================================================================== */

Method hypoMethod[] = 
{ 
  { "puts", hypoPutsItf, "-style normal|verbose|tcl -id <hypoid>" },
  { NULL,   NULL,      NULL                           } 
} ;

Method hypoListMethod[] = 
{ 
  { "puts",       hypoListPutsItf,  "-style normal|verbose|tcl -id <uttid>" },
  { "setPublish", hypoListSetPublishItf,   "same as puts for the moment" },
  { "append",  hypoListAppendItf,  "append hypo to hypolist" },
  { NULL,     NULL,               NULL                           } 
} ;

TypeInfo hypoInfo = { 
     "Hypo",          /* name of type                            */
      0,              /* size of type                            */
      0,              /* number of parameters                    */
      hypoMethod,     /* set of functions that work on this type */
      0,              /* creator function                        */
      0,              /* destructor function                     */
      hypoConfigureItf, /* configure function                      */
      NULL,  /* subtype accessing function              */
      itfTypeCntlDefaultNoLink,
      "Hypo is a subtype of HypoList only.\n"
      "One Hypo contains a single hypothesis.\n"
      "What you need is always a list of hypotheses.\n"
      "Use ""HypoList"" to create such a list\n" } ;

TypeInfo hypoListInfo = { 
      "HypoList",           /* name of type                            */
       0,                   /* size of type                            */   
      -1,                   /* number of parameters                    */
       hypoListMethod,      /* set of functions that work on this type */
       hypoListCreateItf,   /* creator function                        */
       hypoListFreeItf,     /* destructor function                     */
       hypoListConfigureItf,/* configure function                      */
       hypoListAccessItf,   /* subtype accessing function              */
       NULL,
       "The object HypoList contains a list of hypotheses.\n" };

static int  hyposInitialized = 0;

int Hypos_Init()
{
  if (! hyposInitialized) {
    if ( Vocab_Init() != TCL_OK) return TCL_ERROR;

    itfNewType ( &hypoInfo );
    itfNewType ( &hypoListInfo );

    Tcl_Eval(itf,"HypoList method publish hypoListPublish -text {produce output -- format specified by setpublish }");
    Tcl_VarEval(itf,
      "  proc hypoListPublish {hl params} {                        \n",
      "     makeArray arg $params;                                 \n",
      "     set uttid unknown;                                     \n",
      "     if [info exists arg(uttid)] {set uttid $arg(uttid)};   \n",
      "     puts [$hl puts -style normal -id $uttid];              \n",
      "     if [info exists arg(filename)] {                       \n",
      "        set fh [open $arg(filename) a];                     \n",
      "        puts $fh [$hl puts -style normal -id $uttid];       \n",
      "        close $fh;                                          \n",
      "     }                                                      \n",
      "  }                                                         \n",
    NULL  );
    
    hyposInitialized = 1;
  }
  return (TCL_OK);
}
