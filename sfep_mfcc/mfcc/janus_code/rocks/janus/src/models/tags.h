/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Phone Tags
               ------------------------------------------------------------

    Author  :  Ivica Rogina & Michael Finke
    Module  :  tags.h
    Date    :  $Id: tags.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 4.2  2000/08/27 15:31:29  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 4.1.4.1  2000/08/25 22:19:45  jmcd
    Just checking.

    Revision 4.1  2000/01/12 10:12:49  fuegen
    add a Maks to demask modality tags

    Revision 4.0  1998/11/30 18:07:14  fuegen
    *** empty log message ***

    Revision 3.3  1997/07/25 17:28:16  tschaaf
    gcc / DFKI -Clean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.3  95/08/27  22:33:53  finkem
 * *** empty log message ***
 * 
 * Revision 1.2  1995/07/27  17:32:19  finkem
 * *** empty log message ***
 *
    Revision 1.1  1995/07/25  22:33:21  finkem
    Initial revision
 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _tags
#define _tags

#include "list.h"

/* ========================================================================
    Tag
   ======================================================================== */

typedef struct
{
  char*  name;                           /* name of the tag object          */

} Tag;

extern int     tagInit(   Tag* tag, ClientData CD);
extern int     tagDeinit( Tag* tag);

/* ========================================================================
    Tags
   ======================================================================== */

typedef struct LIST(Tag) TagList;

typedef struct
{
  char*     name;                        /* name of the tags object         */
  int       useN;                        /* number of links to tags object  */
  TagList   list;                        /* actual list of tags             */

  char*     wordBeginTag;
  char*     wordEndTag;
  char      commentChar;                 /* comment character used in files */
  int       modMask;                     /* mask to unset all modality tags */

} Tags;

/* ========================================================================
    Modules Interface
   ======================================================================== */

extern int   Tags_Init (void);
extern int   tagsInit(   Tags* tags, ClientData CD);
extern Tags* tagsCreate( char* name);
extern int   tagsLinkN(  Tags* tags);
extern int   tagsDeinit( Tags* tags);
extern int   tagsFree(   Tags* tags);
extern int   tagsIndex(  Tags* tags, char* phone); 
extern char* tagsName(   Tags* tags, int i);
extern int   tagsNumber( Tags* tags);
extern int   tagsAdd(    Tags* tags, int argc, char* argv[]);
extern int   tagsDelete( Tags *tags, int argc, char* argv[]);
extern int   tagsRead(   Tags *tags, char *filename);
extern int   tagsWrite(  Tags *tags, char *filename);

extern int   tagsGetWordBeginTag( Tags* tags);
extern int   tagsGetWordEndTag(   Tags* tags);

#endif

#ifdef __cplusplus
}
#endif
