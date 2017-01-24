/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Wrapper for TCL <-> C-Objects Interface Functions
               ------------------------------------------------------------

    Author  :  Christian Fuegen
    Module  :  itfWrapper.c
    Date    :  $Id: itfWrapper.c 2758 2007-02-21 16:22:55Z fuegen $
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
    Revision 4.3  2007/02/21 16:22:55  fuegen
    changes for Windows from Thilo Koehler

    Revision 4.2  2005/03/10 09:51:43  fuegen
    added missing typeName check in itfGetObject

    Revision 4.1  2003/08/14 11:20:11  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.3  2003/07/09 13:53:05  fuegen
    applied some fixes from itf.c
    code cleaning

    Revision 1.1.2.2  2003/07/02 17:41:52  fuegen
    applied fixes from itf.c and error.c for Windows

    Revision 1.1.2.1  2002/06/28 10:18:57  fuegen
    initial version of the NoTcl-Itf-Wrapper


   ======================================================================== */

#include "itfWrapper.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

#ifdef WINDOWS
#include <sys/locking.h>
#include <share.h>
#endif


/* ======================================================================
   Error Handling
   ====================================================================== */
#define MAXERRACCU 10000

static char *errorFileName;
static int   errorLine;
static int   errorType;
static int   errorMode;


MsgHandler* msgHandlerPtr(char* file, int line, int type, int mode) {

  errorFileName = file;
  errorLine     = line;
  errorType     = type;
  errorMode     = mode;

  /* to get rid of the long path names under windows */
#ifdef WINDOWS
  {
    int i, pos;
    for (i=pos=0; file[i] != '\0'; i++) {
      if ( file[i] == '/' || file[i] == '\\' ) pos = i;
    }

    if ( pos )
      errorFileName = &file[pos+1];
  }
#endif

  return &msgHandler;
}

int msgHandler( char *format, ... ) {

  va_list  ap;
  char     buf[MAXERRACCU] = "", *format2 = format;

  if ( format) {
       va_start(ap,format);
       vsprintf( buf, format2, ap);
       va_end(ap);
  }

  if ( errorMode == 2 ) return 0;

  switch (errorType) {
  case -1:
    fprintf( STDERR, "INFO (%s,%d): %s",  errorFileName, errorLine, buf);
    break;
  case -2:
    fprintf( STDERR, "WARN (%s,%d): %s",  errorFileName, errorLine, buf);
    break;
  case -3:
    fprintf( STDERR, "ERROR (%s,%d): %s", errorFileName, errorLine, buf);
    break;
  case -4:
    fprintf( STDERR, "FATAL (%s,%d): %s", errorFileName, errorLine, buf);
    break;
  default: 
    fprintf( STDERR, "MSG (%s,%d): %s",   errorFileName, errorLine, buf);
    break;
  }

  return 0;
}

int Error_Init(void) {
  static int errorInitialized = 0;

  if (! errorInitialized) {
    errorInitialized = 1;
  }
  return TCL_OK;
}

/* ======================================================================
   TCL Wrapping
   ====================================================================== */

/* difference to TCL-implementation: NO translation of backslashed chars */

int Tcl_SplitList (Tcl_Interp *itf, char *list, int *tokc, char ***tokv) {

  int    i, w;
  int    sopen, bopen, last;
  int    elemN      = 0;
  char **argv;
  char  *l;

  /* count elements in list */
  i = w = sopen = bopen = 0;
  last  = -1;
  while ( list[i] != '\0' ) {

    if ( list[i] == '{' ) bopen++;

    if ( bopen && list[i] == '}' ) {
      bopen--;

      if ( bopen == 0 ) {
	elemN++;
	last  = i;
	w     = 0;
	sopen = 0;
      }
    }

    /* if we are not in brace mode */
    if ( !bopen ) {
      if ( sopen && ( list[i] == ' ' || list[i] == '\t' || list[i] == '\n' ) ) {
	sopen--;
	if ( sopen == 0 ) {
	  elemN++;
	  last = i-1;
	  w    = 0;
	}
      }

      if ( !sopen ) {
	if ( list[i] == ' ' || list[i] == '\t' || list[i] == '\n' ) w++;
	else {
	  if ( w > 0 || last == -1 ) sopen++;
	}
      }
    }

    i++;
  }

  if ( sopen ) {
    sopen--;

    if ( sopen == 0 ) {
      elemN++;
      last = i-1;
      w    = 0;
    }
  }

  /* allocate memory to store pointers + a copy of the list */
  argv = (char**) malloc ( (elemN * sizeof(char*)) + ((i+1) * sizeof(char)) );

  /* copy list */
  memcpy (argv+elemN, list, (i+1) * sizeof(char));

  /* now we can start to fill elements */
  i = w = sopen = bopen = 0;
  last  = -1;
  l     = (char*)(argv+elemN);
  elemN = 0;
  while ( list[i] != '\0' ) {

    if ( list[i] == '{' ) bopen++;

    if ( bopen && list[i] == '}' ) {
      bopen--;

      if ( bopen == 0 ) {
	argv[elemN++] = &l[last+w+2];
	l[i]          = '\0';

	/* printf ("from %d to %d\n", last+w+2, i-1); */
	last  = i;
	w     = 0;
	sopen = 0;
      }
    }

    /* if we are not in brace mode */
    if ( !bopen ) {
      if ( sopen && (list[i] == ' ' || list[i] == '\t' || list[i] == '\n') ) {
	sopen--;
	if ( sopen == 0 ) {
	  argv[elemN++] = &l[last+w+1];
	  l[i]          = '\0';

	  /* printf ("from %d to %d\n", last+w+1, i-1); */
	  last = i-1;
	  w    = 0;
	}
      }

      if ( !sopen ) {
	if ( list[i] == ' ' || list[i] == '\t' || list[i] == '\n' ) w++;
	else {
	  if ( w > 0 || last == -1 ) sopen++;
	}
      }
    }

    i++;
  }

  if ( sopen ) {
    sopen--;

    if ( sopen == 0 ) {
      argv[elemN++] = &l[last+w+1];
      l[i]          = '\0';

      /* printf ("from %d to %d\n", last+w+1, i-1); */
      last = i-1;
      w    = 0;
    }
  }

  *tokv = argv;
  *tokc = elemN;
   
  return TCL_OK;
}

void Tcl_Free (char *ptr) {

  free (ptr);
}

char* Tcl_Merge (int argc, char **argv) {

  ERROR ("Tcl_Merge - not implemented\n");

  return NULL;
}

int Tcl_Eval (Tcl_Interp *itf, char *script) {

  ERROR ("Tcl_Eval - not implemented\n");

  return TCL_OK;
}

int Tcl_GlobalEval (Tcl_Interp *itf, char *script) {

  ERROR ("Tcl_GlobalEval - not implemented\n");

  return TCL_OK;
}

int Tcl_VarEval (Tcl_Interp *itf, char *s1, char *s2, ...) {

  ERROR ("Tcl_VarEval - not implemented\n");

  return TCL_OK;
}

void Tcl_SetResult (Tcl_Interp *itf, char *string, Tcl_FreeProc *freeProc) {

  ERROR ("Tcl_SetResult - not implemented\n");

  return;
}

char* Tcl_SetVar (Tcl_Interp *itf, char *varName, char *newValue, int flags) {

  ERROR ("Tcl_SetVar - not implemented\n");

  return NULL;
}

int Tcl_RegExpMatch (Tcl_Interp *tf, char *string, char *pattern) {

  ERROR ("Tcl_RegExpMatch - not implemented\n");

  return TCL_OK;
}

/* ======================================================================
   itf Functions
   ====================================================================== */

#define itfBufferSize 100000
static char itfBuffer[itfBufferSize];

/* ----------------------------------------------------------------------
   itfTypes / itfObjects
   ---------------------------------------------------------------------- */
static ITFContainerList itfTypes;
static ITFContainerList itfObjects;

int itfTypesInit (ITFContainer *icP, ClientData cd) {

  icP->name    = strdup((char*)cd);
  icP->data.ti = NULL;

  return TCL_OK;
}

int itfTypesDeinit (ITFContainer *icP) {

  return TCL_OK;
}

int itfTypesAlloc (ITFContainer *icP, TypeInfo *ti) {

  icP->data.ti = ti;

  return TCL_OK;
}

int itfObjectsInit (ITFContainer *icP, ClientData cd) {

  icP->name    = strdup((char*)cd);
  icP->data.oe = NULL;

  return TCL_OK;
}

int itfObjectsDeinit (ITFContainer *icP) {

  return TCL_OK;
}

int itfObjectsAlloc (ITFContainer *icP, ClientData cd, char *typeName) {

  icP->data.oe        = (ObjectEntry*)malloc (sizeof (ObjectEntry));
  icP->data.oe->item  = cd;
  icP->data.oe->type  = itfGetType (typeName);

  return TCL_OK;
}

/* ----------------------------------------------------------------------
   type/object itf functions
   ---------------------------------------------------------------------- */
int itfNewType (TypeInfo *ti) {

  int idx = listIndex ((List*)&itfTypes, (ClientData)(ti->name), 0);

  if ( idx < 0 ) {
    idx = listNewItem ((List*)&itfTypes, (ClientData)(ti->name));
    itfTypesAlloc (&(itfTypes.itemA[idx]), ti);
  }

  return idx;
}

TypeInfo* itfGetType (char *typeName) {

  int idx = listIndex ((List*)&itfTypes, typeName, 0);

  if ( idx >= 0 ) return itfTypes.itemA[idx].data.ti;

  return NULL;
}

int itfNewObject (ClientData cd, char *objectName, char *typeName) {

  int   idx  = listIndex ((List*)&itfObjects, (ClientData)(objectName), 0);

  if ( idx < 0 ) {
    idx = listNewItem ((List*)&itfObjects, (ClientData)(objectName));
    itfObjectsAlloc (&(itfObjects.itemA[idx]), cd, typeName);
  }

  return idx;
}

int itfAddObject (ClientData cd, char *objectName, char *typeName) {

  return itfNewObject (cd, objectName, typeName);
}

ClientData itfGetObject (char *objectName, char *typeName) {

  int   idx      = listIndex ((List*)&itfObjects, objectName, 0);
  int   cnt      = 0;
  char *p, *q, subName[MAXLINE];

  ClientData  subitem;
  TypeInfo   *subti;

  if ( idx >= 0 ) {
    subitem = itfObjects.itemA[idx].data.oe->item;
    subti   = itfObjects.itemA[idx].data.oe->type;
    if ( strcmp (subti->name, typeName) != 0 ) return NULL;
    return subitem;
  }

  /* we do not have the right object, so look if we can get them */
  
  p = objectName;
  q = subName;
  while ( *p != ':' && *p != '.' && *p != '\0' &&
	  ++cnt < MAXLINE ) *q++ = *p++;
  *q = '\0';
  if ( cnt == MAXLINE ) return NULL;
  
  idx = listIndex ((List*)&itfObjects, subName, 0);
  
  if ( idx < 0 ) {
    SERROR ("No object %s of type %s defined\n", subName, typeName);
    return NULL;
  }
  
  subitem = itfObjects.itemA[idx].data.oe->item;
  subti   = itfObjects.itemA[idx].data.oe->type;
  
  /* go through all subitems */
  while ( *p != '\0' ) {

    ClientData  subsubitem;
    TypeInfo   *subsubti;

    q = subName;
    *q++ = *p++;

    while (*p!=':' && *p!='.' && *p!='\0') *q++ = *p++;   
    *q = '\0';

    if ( !subti->access ) {
      SERROR ("%s is no sub-object in %s\n", subName, objectName);
      return NULL;
    }

    subsubti   = (TypeInfo*)1;
    subsubitem = subti->access (subitem, subName, &subsubti);

    if ( !subsubitem ) {
      SERROR ("Can't access sub-object %%s in %s\n", subName, objectName);
      return NULL;
    }

    subitem = subsubitem;
    subti   = subsubti;
  }

  return subitem;
}

char* itfGetObjectType (char *objectName) {

  int idx = listIndex ((List*)&itfObjects, objectName, 0);

  if ( idx >= 0 ) return itfObjects.itemA[idx].data.oe->type->name;

  return NULL;
}

int itfTypeCntlDefaultNoLink (ClientData clientData, TypeInfo *typeInfo, int command, ...) {

  return 0;
}

int itfTypeCntl (ClientData clientData, TypeInfo *typeInfo, int command, ...) {

  return 0;
}


/* ----------------------------------------------------------------------
   itf function for main
   ---------------------------------------------------------------------- */

int itfPuts () {

  listPutsItf ((ClientData)&itfTypes, 0, NULL);

  return TCL_OK;
}

int itfInit () {

  init_mach_ind_io();

  listInit ((List*)&(itfTypes), NULL, sizeof(ITFContainer), 30);
  itfTypes.init   = (ListItemInit   *)itfTypesInit;
  itfTypes.deinit = (ListItemDeinit *)itfTypesDeinit;

  listInit ((List*)&(itfObjects), NULL, sizeof(ITFContainer), 30);
  itfObjects.init   = (ListItemInit   *)itfObjectsInit;
  itfObjects.deinit = (ListItemDeinit *)itfObjectsDeinit;

  return TCL_OK;
}

ClientData itfCreate (int argc, char **argv) {

  char        *typeName   = argv[0];
  char        *objectName = argv[1];
  TypeInfo    *ti         = itfGetType (typeName);
  char       **targv      = argv+1;
  int          targc      = argc-1;
  ClientData   cd         = NULL;

  if ( ti->constructor ) {
    cd = ti->constructor (NULL, targc, targv);
    if ( cd ) {
      itfAddObject (cd, objectName, typeName);
    } else {
      ERROR ("Can't create object %s with type %s\n", objectName, typeName);
    }
  } else {
    ERROR ("No constructor defined for object with type %s\n", typeName);
  }

  return cd;
}

static int itfConfigure ( ClientData item, Configurer* configProc, 
			  Tcl_Interp *interp, 
			  int argc, char *argv[] ) {
  if (! configProc) {
    WARN( "<ITF,CCO>Can't configure object.\n");
    return TCL_ERROR;
  }
  if (argc==0) return configProc ( item, NULL, NULL ); 
  
  while (argc>0) {
    int dummy;
    char *variable, *value;
    
    if (argv[0][0]!='-') {
      SWARN( "<ITF,CVM>Configure variable '%s' must start with -.\n", argv[0]);
      return TCL_ERROR;
    }
    variable = argv[0]+1;
    if (argc>1) value = argv[1]; else value = NULL;
    
    if  ((dummy = configProc (item, variable, value)) != TCL_OK)
      return dummy;
    
    argc-=2; argv+=2;
  }

  return TCL_OK;
}

int itfMethod (int argc, char **argv) {

  char       *objectName = argv[0];
  char       *typeName   = itfGetObjectType (objectName);
  ClientData  cd;
  TypeInfo   *ti;
  Method     *method;

  int         i          = 0;
  char      **targv      = argv+1;
  int         targc      = argc-1;
  char       *methodName = targv[0];

  if ( !typeName ) {
    int   cnt = 0;
    int   idx;
    char *p, *q, subName[MAXLINE];

    ClientData  subitem;
    TypeInfo   *subti;

    /* try to get subobject */
    p = objectName;
    q = subName;
    while ( *p != ':' && *p != '.' && *p != '\0' &&
	    ++cnt < MAXLINE ) *q++ = *p++;
    *q = '\0';
    if ( cnt == MAXLINE ) return TCL_ERROR;
    
    idx = listIndex ((List*)&itfObjects, subName, 0);
    
    if ( idx < 0 ) {
      SERROR ("No object %s of type %s defined\n", subName, typeName);
      return TCL_ERROR;
    }
    
    subitem = itfObjects.itemA[idx].data.oe->item;
    subti   = itfObjects.itemA[idx].data.oe->type;

    /* go through all subitems */
    while ( *p != '\0' ) {
      
      ClientData  subsubitem;
      TypeInfo   *subsubti;
      
      q = subName;
      *q++ = *p++;
      
      while (*p!=':' && *p!='.' && *p!='\0') *q++ = *p++;   
      *q = '\0';
      
      if ( !subti->access ) {
	SERROR ("%s is no sub-object in %s\n", subName, objectName);
	return TCL_ERROR;
      }
      
      subsubti   = (TypeInfo*)1;
      subsubitem = subti->access (subitem, subName, &subsubti);
      
      if ( !subsubitem ) {
	SERROR ("Can't access sub-object %%s in %s\n", subName, objectName);
	return TCL_ERROR;
      }
      
      subitem = subsubitem;
      subti   = subsubti;
    }
    
    cd     = subitem;
    ti     = subti;
    method = ti->method;
  } else {
    cd     = itfGetObject (objectName, typeName);
    ti     = itfGetType   (typeName);
    method = ti->method;
  }

  while ( method[i].name && strcmp (method[i].name, methodName) ) i++;

  if ( method[i].name ) {
    int res;
    res = method[i].proc (cd, targc, targv);
    if ( !strcmp (method[i].name, "puts") ) itfAppendResult ("\n");
    return res;
  }

  if ( !strcmp ("configure", methodName) ) {
    return itfConfigure (cd, ti->configure, NULL, targc-1, targv+1);
  }

  ERROR ("No method %s defined for object %s\n", methodName, objectName);

  return TCL_ERROR;
}

int itfDo (char *line) {

  char **argv;
  int    argc;

  char *typeName;

  TypeInfo *ti = NULL;

  Tcl_SplitList (NULL, line, &argc, &argv);

  /* detect type of command */
  typeName = argv[0];
  ti       = itfGetType (typeName);

  if ( ti ) {
    /* we've found a type, so that is a create command */
    if ( itfCreate (argc, argv) == NULL ) {
      return TCL_ERROR;
    }
  } else {
    if ( itfMethod (argc, argv) == TCL_ERROR ) {
      return TCL_ERROR;
    }
  }

  Tcl_Free ((char*)argv);

  return TCL_OK;
}


/* ----------------------------------------------------------------------
   itfParseArgv
   ---------------------------------------------------------------------- */
int itfParseArgv( char* method, int* argcPtr, char* argv[], int leftOver, ...)
{
  ArgvInfo  argTable[200];

  ArgvInfo* infoPtr =  argTable;
  int       argc    = *argcPtr;
  int       prmIdx  =  0;
  int       srcIdx  =  0;
  int       dstIdx  =  0;
  char*     curArgv;
  int       len;
  va_list   ap;
  char*     str;

  /* fill the argument table */

  va_start(ap,leftOver);
  while ( (str = va_arg(ap,char *)) ) {

    infoPtr->key     = str;
    infoPtr->type    = va_arg(ap,ArgvType);
    infoPtr->handler = va_arg(ap,ArgvHandler*);
    infoPtr->dst     = va_arg(ap,ClientData);
    infoPtr->cd      = va_arg(ap,ClientData);
    infoPtr->help    = va_arg(ap,char*);
    infoPtr++;
  }
  infoPtr->key  = NULL;
  infoPtr->type = ARGV_END;
  va_end(ap);

  /* parse the fixed parameters */

  while ( argc > 0) {
    ArgvInfo* matchPtr = NULL;

    if ((curArgv = argv[srcIdx]) == NULL) break;
    len      = strlen(curArgv);

    if (! strcmp(curArgv,"-help")) {
      /*      itfPrintUsage(method,argTable);*/

      if ( argTable[prmIdx].key && argTable[prmIdx].key[0] != '-')
           return TCL_ERROR;
      else return TCL_ERROR; /* was originally TCL_OK */
    }
    else if ( argTable[prmIdx].key && argTable[prmIdx].key[0] != '-') {

      matchPtr = argTable + prmIdx; 
      prmIdx++;
    }
    else {
      char c  = curArgv[1];
      infoPtr = argTable + prmIdx;

      argc--; srcIdx++;

      for ( ; infoPtr->key; infoPtr++) {
        if ((infoPtr->key[1] != c) || strncmp( infoPtr->key, curArgv, len))
          continue;
        if (infoPtr->key[len] == '\0') {
          matchPtr = infoPtr;
          goto gotMatch;
        }
        if ( matchPtr != NULL) {
          SERROR("<ITF,AO>%s: ambiguous option \"%s\".\n", method, curArgv);
          return TCL_ERROR;
        }
        matchPtr = infoPtr;
      }
    }
    if ( matchPtr == NULL) {

      if (! leftOver) {

        SERROR("<ITF,UA>%s: unrecognized argument \"%s\".\n", method, curArgv);
        return TCL_ERROR;
      }
      if ( leftOver > 1) {
        argv[dstIdx] = curArgv;
        dstIdx++;
      }
      srcIdx++;
      continue;
    }

    gotMatch:

    infoPtr = matchPtr;
    switch ( infoPtr->type) {

    case ARGV_SHORT :

      if ( argc == 0) { goto missingArg; }
      else {
        char* endPtr;
        *((short*)infoPtr->dst) = (short)strtol( argv[srcIdx], &endPtr, 0);
        if ( endPtr == argv[srcIdx] || *endPtr) {
          SERROR("<ITF,ESH>%s: expected short argument for %s but got \"%s\".\n",
                 method, infoPtr->key, argv[srcIdx]);
          return TCL_ERROR;
        }
        srcIdx++;
        argc--;
      }
      break;

    case ARGV_LONG :

      if ( argc == 0) { goto missingArg; }
      else {
        char* endPtr;
        *((long*)infoPtr->dst) = strtol( argv[srcIdx], &endPtr, 0);
        if ( endPtr == argv[srcIdx] || *endPtr) {
          SERROR("<ITF,EIN>%s: expected long argument for %s but got \"%s\".\n",
                 method, infoPtr->key, argv[srcIdx]);
          return TCL_ERROR;
        }
        srcIdx++;
        argc--;
      }
      break;

    case ARGV_INT :

      if ( argc == 0) { goto missingArg; }
      else {
        char* endPtr;
        *((int*)infoPtr->dst) = strtol( argv[srcIdx], &endPtr, 0);
        if ( endPtr == argv[srcIdx] || *endPtr) {
          SERROR("<ITF,EIN>%s: expected integer argument for %s but got \"%s\".\n",
                 method, infoPtr->key, argv[srcIdx]);
          return TCL_ERROR;
        }
        srcIdx++;
        argc--;
      }
      break;

    case ARGV_IARRAY :

      if ( argc == 0) { goto missingArg; }
      else {
        if ( iarrayGetItf((IArray*)infoPtr->dst, argv[srcIdx]) != TCL_OK)
             return TCL_ERROR;
        srcIdx++;
        argc--;
      }
      break;

    case ARGV_FARRAY :

      if ( argc == 0) { goto missingArg; }
      else {
        if ( farrayGetItf((FArray*)infoPtr->dst, argv[srcIdx]) != TCL_OK)
             return TCL_ERROR;
        srcIdx++;
        argc--;
      }
      break;

    case ARGV_LIARRAY :

      if ( argc == 0) { goto missingArg; }
      else {
        if ( iarrayGetListItf((IArray*)infoPtr->dst, argv[srcIdx],
                              (List  *)infoPtr->cd) != TCL_OK) {
          SERROR("<ITF,CRLIA>Can't read list index array for %s.\n", infoPtr->key);
          return TCL_ERROR;
        }
        srcIdx++;
        argc--;
      }
      break;

    case ARGV_MIARRAY :

      if ( argc < ((IArray*)infoPtr->dst)->itemN ) { goto missingArg; }
      else {
	int i;
	IArray * warray = (IArray*)infoPtr->dst;
	List   * L      = (List*) infoPtr->cd;
	if(warray->itemA==0)	
	  if(NULL==(warray->itemA = calloc(warray->itemN,sizeof(int)))) FATAL("Not enough memory\n");

	for(i=0;i<warray->itemN;i++) {
	  warray->itemA[i]=listIndex( L, (ClientData)argv[srcIdx], 0);
	  if(warray->itemA[i]<0) {
	    SERROR("Can't find '%s' in the list.\n", argv[srcIdx]);
	    return TCL_ERROR;
	  }
	  srcIdx++;
	  argc--;
	}
      }
      break;

    case ARGV_FLOAT :

      if ( argc == 0) { goto missingArg; }
      else {
        char* endPtr;
        *((float*)infoPtr->dst) = strtod( argv[srcIdx], &endPtr);
        if ( endPtr == argv[srcIdx] || *endPtr) {
          SERROR("<ITF,EXFL>%s: expected float argument for %s but got \"%s\".\n",
                 method, infoPtr->key, argv[srcIdx]);
          return TCL_ERROR;
        }
        srcIdx++;
        argc--;
      }
      break;

    case ARGV_DOUBLE :

      if ( argc == 0) { goto missingArg; }
      else {
        char* endPtr;
        *((double*)infoPtr->dst) = strtod( argv[srcIdx], &endPtr);
        if ( endPtr == argv[srcIdx] || *endPtr) {
          SERROR("<ITF,EXDB>%s: expected double argument for %s but got \"%s\".\n",
                  method,infoPtr->key, argv[srcIdx]);
          return TCL_ERROR;
        }
        srcIdx++;
        argc--;
      }
      break;

    case ARGV_STRING :

      if ( argc == 0) { goto missingArg; }
      else {
	if (argv[srcIdx][0]=='@') 
	  {
	    *((char**)infoPtr->dst) = itfReadFile(argv[srcIdx]+1); 

	    /* ------------------------------------------------------ */
	    /* add *((char**)infoPtr->dst) to garbage collection list */
	    /* to be freed, when argv is freed (between commands), or */
	    /* do this:      free(argv[srcIdx]);                      */
	    /*     and:      argv[srcIdx] = *((char**)infoPtr->dst);  */
	    /* No, I tried the latter version, won't do ...           */
	    /* ------------------------------------------------------ */

            if (! *((char**)infoPtr->dst)) 
                  *((char**)infoPtr->dst) = argv[srcIdx];
	  }
	else                      
	  *((char**)infoPtr->dst) = argv[srcIdx]; 
        srcIdx++;
        argc--;
      }
      break;

    case ARGV_OBJECT :

      if ( argc == 0) { goto missingArg; }
      else {
        ClientData objPtr = itfGetObject( argv[srcIdx],(char*)infoPtr->cd);
        if (! objPtr) return TCL_ERROR;
        *((ClientData*)infoPtr->dst) = objPtr; 
        srcIdx++;
        argc--;
      }
      break;

    case ARGV_LINDEX :

      if ( argc == 0) { goto missingArg; }
      else { 
        int ac  = argc;
        int idx = listIndexItf(infoPtr->cd, infoPtr->key, infoPtr->dst, 
                              &ac, argv+srcIdx);

        if ( idx < 0) return TCL_ERROR;
        srcIdx += argc - ac;
        argc    = ac;
      }
      break;

    case ARGV_CUSTOM :
      {
      int ac = argc;

      if (  infoPtr->handler( infoPtr->cd, infoPtr->key, infoPtr->dst, 
                              &ac, argv+srcIdx)) {
        return TCL_ERROR;
      }
      srcIdx += argc - ac;
      argc    = ac;
      }
      break;

    case ARGV_REST :

      *((int*)infoPtr->dst) = dstIdx;
      goto argsDone;

    default:

      SERROR("<ITF,BAT>%s: bad argument type %d in ArgvInfo.\n", 
              method, infoPtr->type);
      return TCL_ERROR;
    }
  }

  argsDone:

  if ( argTable[prmIdx].key && argTable[prmIdx].key[0] != '-') {

    SERROR( "<ITF,MA>%s: missing argument \"%s\".\n", method, 
             argTable[prmIdx].key);
    return TCL_ERROR;
  }

  if ( leftOver < 2) {

    while ( argc>0) {
      argv[dstIdx++] = argv[srcIdx++];
      argc--;
    }
    /* Commented out by (fuegen, 04/30/02)
      argv[dstIdx] = (char*)NULL;
      *argcPtr     =  dstIdx++;
    */
  }
  return TCL_OK;

  missingArg:

  SERROR("<ITF,ORAA>option \"%s\" requires an additional argument.\n", curArgv);
  return TCL_ERROR;
}

/* ----------------------------------------------------------------------
   itfAppend/Reset
   ---------------------------------------------------------------------- */
void itfAppendResult( char *format, ... ) {
  va_list  ap;
  int n;

  va_start(ap,format);
  n = vsprintf( itfBuffer, format, ap);
  assert (itfBufferSize > n);
  va_end(ap);

  printf ("%s", itfBuffer);
}

void itfAppendElement( char *format, ... ) {
  va_list  ap;
  int n, i;

  va_start(ap,format);
  n = vsprintf( itfBuffer, format, ap);  
  assert (itfBufferSize > n);
  va_end(ap);

  n = strlen (itfBuffer);
  i = 0;
  while ( i < n  && itfBuffer[i] != ' ' ) i++;

  if ( i < n ) {
    printf ("{%s} ", itfBuffer);
  } else {
    printf ("%s ", itfBuffer);
  }
}

void itfResetResult () {
}

static int returnSysError(char *command) {

  char *err = NULL;

  if ( (err = strerror (errno)) )
    ERROR("<ITF,SHC> %s: %s\n",            command,err);
  else
    ERROR("<ITF,SHC>%s: undefined error\n",command);

  MSGPRINT(NULL);
  return TCL_ERROR;
}

static int itfMakePath(char *name, int mode)
{

  char   *p = name;
  char    c;
  size_t  i;

#ifndef WINDOWS
  char sep[] = "/";
#else
  char sep[] = "/\\";
#endif

  i = strcspn (p, sep);
  c = p[i]; p[i] = '\0';

  while ( c != '\0' ) {
#ifndef WINDOWS
    if ( i && mkdir (name, mode) != 0 ) {
#else
    if ( i && mkdir (name)       != 0 ) {
#endif
      if ( errno != EEXIST ) return returnSysError("mkdir");
    }

    p[i] = c; p += i + 1;

    i = strcspn (p, sep);
    c = p[i]; p[i] = '\0';
  }

  p[i] = c; p += i + 1;

  return TCL_OK;
}


/* ======================================================================
   File Control
   ====================================================================== */

/* ------------------------------------------------------------------------
    fileOpen   open a file for reading/writing
   ------------------------------------------------------------------------ */
FILE* fileOpen(char* fileName, char* mode) {

  int   retry = 1;
  int   count = 0;
  int   l     = strlen(fileName);
  int   pipe  = 0;
  char  m[10];
  char *p;
  FILE *fp;

  if (strchr(mode,'w')) itfMakePath(fileName,0755);

  if        (! strcmp( fileName + l - 2, ".Z")) {
    if      ( strchr( mode, 'r')) sprintf(itfBuffer,"zcat %s",      fileName);
    else if ( strchr( mode, 'w')) sprintf(itfBuffer,"compress > %s",fileName);
    else {
      ERROR("<ITF,COF> Can't popen %s using mode %s.\n", fileName, mode);
      return NULL;
    }
    pipe = 1;

  } else if (! strcmp( fileName + l - 3, ".gz")) {
    if      ( strchr( mode, 'r')) sprintf(itfBuffer,"gunzip -c  %s",fileName);
    else if ( strchr( mode, 'w')) sprintf(itfBuffer,"gzip -c >  %s",fileName);
    else if ( strchr( mode, 'a')) sprintf(itfBuffer,"gzip -c >> %s",fileName);
    else {
      ERROR("<ITF,COF> Can't popen %s using mode %s.\n", fileName, mode);
      return NULL;
    }
    pipe = 1;

  } else if (! strcmp( fileName + l - 4, ".bz2")) {
    if      ( strchr( mode, 'r')) sprintf(itfBuffer,"bzip2 -cd    %s",fileName);
    else if ( strchr( mode, 'w')) sprintf(itfBuffer,"bzip2 -cz >  %s",fileName);
    else if ( strchr( mode, 'a')) sprintf(itfBuffer,"bzip2 -cz >> %s",fileName);
    else {
      ERROR("<ITF,COF> Can't popen %s using mode %s.\n", fileName, mode);
      return NULL;
    }
    pipe = 1;
  }


  /* WINDOWS: append 'b' for binary if it does not exists and
              if no 't' for text mode is specified
     LINUX:   'b' for binary mode doesn't matter, but 't' for
              text mode has to be removed */
  if ( (p = strchr (mode, 'b')) ) {
    strcpy (m, mode);
  } else if ( (p = strchr (mode, 't')) ) {
#ifdef WINDOWS
    strcpy (m, mode);
#else
    int i = p - mode + 1;
    strcpy (m, mode);
    while ( mode[i] != '\0' ) {
      m[i-1] = mode[i];
      i++;
    }
    m[i-1] = '\0';
#endif
  } else {
    if ( pipe ) {
#ifdef WINDOWS
      sprintf (m, "%sb", mode);
#else
      sprintf (m, "%s", mode);
#endif
    } else
      sprintf (m, "%sb", mode);
  }

  while (count <= retry) {
    if (! (fp = ( pipe) ? popen( itfBuffer, m) :
                          fopen( fileName,  m))) {
      sleep(2); count++;
    }
    else break;
  }

  if ( count > retry) {
    ERROR("<ITF,COF> fileOpen '%s' '%s' failed.\n", fileName, m);
    return NULL;
  }

  /* if we are in append mode make windows behaviour compatible with unix */
  if ( !pipe && strchr (mode, 'a') ) fseek (fp, 0, SEEK_END);

  return fp;
}

/* ------------------------------------------------------------------------
    fileClose  close open file
   ------------------------------------------------------------------------ */

int fileClose(char* fileName, FILE* fp)
{
  int l = strlen(fileName);

  if (! fp) return TCL_OK;

  fflush( fp);

  if      (! strcmp( fileName + l - 2, ".Z"))   pclose( fp);
  else if (! strcmp( fileName + l - 3, ".gz"))  pclose( fp);
  else if (! strcmp( fileName + l - 4, ".bz2")) pclose( fp);
  else                                          fclose( fp);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    freadAdd
   ------------------------------------------------------------------------ */

int freadAdd( ClientData cd, char* fileName, char commentChar,
              int (*readFn)(ClientData, int, char **))
{
  FILE *fp;
  char  line[100000];
  int   n  = 0, ln = 0;
  int   rc = TCL_OK;

  if (! (fp = fileOpen(fileName,"rt"))) return -1;

  strcpy(line,"add ");

  while (1) {
    int   argc; char **argv;
    char* p;
    int   f = fscanf(fp,"%[^\n]",&(line[4]));

    assert( f < 100000);

    ln++;

    if      ( f <  0)   break;
    else if ( f == 0) { fscanf(fp,"%*c"); continue; }

    if ( line[4] == commentChar) continue; 

    for (p=&(line[4]); *p!='\0'; p++) 
         if (*p>' ') break; if (*p=='\0') continue; 

    if ( Tcl_SplitList (itf,line,&argc,&argv) == TCL_OK) {
      if ( readFn(cd, argc, argv) != TCL_OK) {
           ERROR("<ITF,CRL>Can't read line %d: \"%s\"\n", ln, line+4);
           rc = TCL_ERROR;
      }
      else n++;
      Tcl_Free((char*) argv);
    }
    else {
      ERROR("<ITF,CPL>Can't parse line: %s\n", line+4);
      rc = TCL_ERROR;
    }
  }
  fileClose( fileName, fp);
  return (rc != TCL_OK) ? -1 : n;
}


/* ------------------------------------------------------------------------
    read file into string
   ------------------------------------------------------------------------ */

static char *readFileBuffer = NULL;

void itfFreeReadFileBuffer()
{
  if (readFileBuffer) { free(readFileBuffer); readFileBuffer=NULL; }
}

char *itfReadFile (char *filename)
{
  FILE *fp; 
  int   size;

  if (!filename) return NULL;
  if ((fp = fileOpen(filename,"r"))==NULL) return NULL;

  fseek(fp,0,SEEK_END); size=ftell(fp); rewind(fp);
  if (readFileBuffer != NULL) itfFreeReadFileBuffer();
  readFileBuffer = (char*)malloc(sizeof(char)*(size+1));
  if (readFileBuffer == NULL) return NULL;
  fread(readFileBuffer,sizeof(char),size,fp);
  readFileBuffer[size] ='\0';
  fileClose(filename, fp);
  return readFileBuffer;
}


/* ------------------------------------------------------------------------
    printO/E
   ------------------------------------------------------------------------ */

void printO( char *format, ... )
{
  va_list  ap;

  va_start(ap,format);
  vfprintf( STDOUT, format, ap);
  va_end(ap);
}

void printE( char *format, ... )
{
  va_list  ap;

  va_start(ap,format);
  vfprintf( STDERR, format, ap);
  va_end(ap);
}


/* ----------------------------------------------------------------------
   Timer
   ---------------------------------------------------------------------- */
char *dateString() { time_t t=time(0); return (ctime(&t)); }

#ifndef WINDOWS
void timerReset( Timer* timerP)
{
  timerP->start.tv_sec = 0L;
  timerP->start.tv_usec= 0L;
  timerP->stop.tv_sec  = 0L;
  timerP->stop.tv_usec = 0L;
  timerP->delta        = 0.0;
}

void timerStart( Timer* timerP)
{
  struct timeval   * tp = &(timerP->start);
  struct timezone  * tz =   NULL;

  gettimeofday(tp, tz);
}

float timerStop( Timer* timerP)
{
  const long       one_million = 1000000L;
  long             s, us;

  struct timeval   * tp = &(timerP->stop);
  struct timezone  * tz =   NULL;

  gettimeofday(tp, tz);

  s  = (timerP->stop.tv_sec)  - (timerP->start.tv_sec);
  us = (timerP->stop.tv_usec) - (timerP->start.tv_usec);

  while (us < 0L) { us += one_million; s  -= 1L; }
  timerP->delta += ((float)s + ((float)us / 1E6) );   

  return timerP->delta;
}
#else /* WINDOWS */
void timerReset( Timer* timerP) {

	timerP->start = timerP->stop = (clock_t)0;
	timerP->delta = 0.0;

}

void timerStart( Timer* timerP) {

	timerP->start = clock();
}

float timerStop( Timer* timerP) {

	timerP->stop   = clock();
	timerP->delta += (timerP->stop - timerP->start) / (float) CLK_TCK;

  return timerP->delta;
}
#endif /* WINDOWS */


/* ========================================================================
    Dynamic Strings
   ======================================================================== */

void  dstringInit(   DString* dsP) {

    dsP->string         = dsP->staticSpace;
    dsP->length         = 0;
    dsP->spaceAvl       = TCL_DSTRING_STATIC_SIZE;
    dsP->staticSpace[0] = '\0';
}

void  dstringFree(   DString* dsP) {

  if (dsP->string != dsP->staticSpace) {
    free(dsP->string);
  }

  dsP->string         = dsP->staticSpace;
  dsP->length         = 0;
  dsP->spaceAvl       = TCL_DSTRING_STATIC_SIZE;
  dsP->staticSpace[0] = '\0';
}

void  dstringResult( DString* dsP) { }
void  dstringGetResult(    DString* dsP) { }
void  dstringStartSublist( DString* dsP) { }
void  dstringEndSublist(   DString* dsP) { }
void  dstringSetLength( DString* dsP, int len) { }

char* dstringAppend( DString* dsP, char* format, ...) {

  va_list  ap;
  int      n;
  int      newSize;
  char    *dst;
  char    *end;
  char    *string;
  int      length;

  va_start(ap,format);
  n = vsprintf( itfBuffer, format, ap);
  assert (itfBufferSize > n);
  va_end(ap);
 
  length = -1;
  string = itfBuffer;

  if (length < 0) {
    length = strlen(string);
  }
  newSize = length + dsP->length;
  
  /*
   * Allocate a larger buffer for the string if the current one isn't
   * large enough. Allocate extra space in the new buffer so that there
   * will be room to grow before we have to allocate again.
   */
  
  if (newSize >= dsP->spaceAvl) {
    dsP->spaceAvl = newSize * 2;
    if (dsP->string == dsP->staticSpace) {
      char* newString;
      
      newString = (char*) malloc((unsigned) dsP->spaceAvl);
      memcpy((void*) newString, (void*) dsP->string,
	     (size_t) dsP->length);
      dsP->string = newString;
    } else {
      dsP->string = (char*) realloc((void*) dsP->string,
				       (size_t) dsP->spaceAvl);
    }
  }
  
  /*
   * Copy the new string into the buffer at the end of the old
   * one.
   */
  
  for (dst = dsP->string + dsP->length, end = string+length;
       string < end; string++, dst++) {
    *dst = *string;
  }
  *dst = '\0';
  dsP->length += length;

  return dsP->string;
}

char* dstringAppendElement( DString* dsP, char* format, ...) {

  return NULL;
}

