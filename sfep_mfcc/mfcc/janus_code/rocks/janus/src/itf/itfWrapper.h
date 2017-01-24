/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Wrapper for TCL <-> C-Objects Interface Functions
               ------------------------------------------------------------

    Author  :  Christian Fuegen
    Module  :  itfWrapper.h
    Date    :  $Id: itfWrapper.h 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 4.1  2003/08/14 11:20:12  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.1  2002/06/28 10:18:57  fuegen
    initial version of the NoTcl-Itf-Wrapper


   ======================================================================== */

#include <stdarg.h>

#include "common.h"
#include "itf.h"
#include "list.h"
#include "array.h"
#include "phones.h"
#include "tags.h"
#include "feature.h"


#define MAXLINE 2048

/* ======================================================================
   definitions
   ====================================================================== */

typedef struct {

  ClientData     item;               /* pointer to instantiated object      */
  TypeInfo      *type;               /* pointer to related type information */

} ObjectEntry;

typedef struct {

  char     *name;

  union data_p {
    TypeInfo    *ti;
    ObjectEntry *oe;
  } data;

} ITFContainer;

typedef struct LIST(ITFContainer) ITFContainerList;

/* ======================================================================
   external functions
   ====================================================================== */
extern int        itfInit          ();
extern int        init_mach_ind_io ();
extern int        itfPuts          ();
extern int        itfDo            (char *line);
