/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Common Includes
               ------------------------------------------------------------

    Author  :  Markus Baur
    Module  :  hash.c
    Date    :  $Id: hash_lm_3g.h 3418 2011-03-23 15:07:34Z metze $
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
    Revision 1.3  2001/01/15 09:49:57  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 1.2.36.1  2001/01/12 15:16:55  janus
    necessary changes for running janus under WINDOWS

    Revision 1.2  2000/08/27 15:31:16  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 1.1.4.1  2000/08/25 22:19:43  jmcd
    Just checking.

    Revision 1.1  1998/05/26 18:27:23  kries
    Initial revision

 * Revision 3.2  1997/06/17  17:41:43  westphal
 * *** empty log message ***
 *
    Revision 1.2  95/07/27  19:37:25  finkem
    *** empty log message ***
    
    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__HASH__)
#define __HASH__

#include "common.h"	/* basic data types */

#define LONG long
#define SHORT short
#define INT int
#define CHAR char

#ifdef VOID
#undef VOID
#endif
#define VOID void

#define TRUE 1
#define FALSE 0

#define HASH_STATS

/* types and exported routines for Hash tables */

typedef struct HASHREC
{
  VOID *key;
  LONG hash;
  VOID *data;
  struct HASHREC *next;
} HASHREC;

typedef struct HASHREC *HASHRECP;


typedef struct HASHTABLE
{
  long (*hash_fn)(VOID *);
  int (*compare_fn)(VOID *, VOID *);
  HASHRECP *table;
  int size_index;
  LONG entries;
  LONG collisions;		/* only for informational purposes */
} HASHTABLE;


typedef struct HASHTABLE *HashtableP;


extern VOID hash_insert (HashtableP h, VOID *key, VOID *data);
extern INT hash_delete (HashtableP h, VOID *key);
extern VOID *hash_lookup (HashtableP h, VOID *key);
extern HashtableP hash_new_generic_table (long (*hash_fn)(VOID *), INT (*compare_fn)(VOID *item1, VOID *item2));
extern HashtableP hash_new_string_table (VOID);
extern HashtableP hash_new_pointer_table (VOID);
extern VOID hash_free_table (HashtableP h);

#endif

#ifdef __cplusplus
}
#endif
