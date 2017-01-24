/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Common Includes
               ------------------------------------------------------------

    Author  :  Markus Baur
    Module  :  hash.c
    Date    :  $Id: hash_lm_3g.c 3418 2011-03-23 15:07:34Z metze $
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
    Revision 1.2  2003/08/14 11:20:21  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.56.1  2002/06/26 11:57:58  fuegen
    changes for new directory structure support and code cleaning

    Revision 1.1  1998/05/26 18:27:23  kries
    Initial revision

 * Revision 3.2  1997/06/17  17:41:43  westphal
 * *** empty log message ***
 *
    Revision 1.5  96/03/05  14:05:31  rogina
    made gcc -Wall warnings go away
    
    Revision 1.4  1995/10/04  23:38:47  torsten
    *** empty log message ***

    Revision 1.3  1995/09/06  07:27:58  kemp
    *** empty log message ***

    Revision 1.2  1995/07/25  14:45:47  rogina
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_lm_3g.h"


char hash_lm_3gRcsVersion[]= 
           "@(#)1$Id: hash_lm_3g.c 3418 2011-03-23 15:07:34Z metze $";

static LONG primes[]=
{
  1,  2,  3,  7,  13,  23,  59,  113,  241,  503,  1019,  2039,  4091,
  8179,  16369,  32749,  65521,  131063,  262139,  524269,  1048571,
  2097143,  4194287,  8388593,  16777199,  33554393,  67108859,
  134217689,  268435399,  536870879,  1073741789,  2147483629
};


#define SIZE(h) (primes[(h)->size_index])
#define MAX_HIST 500

static LONG pointer_hash(VOID *p)
{
  return ((LONG)p);
}


static INT pointer_compare(VOID *p1, VOID *p2)
{
  return (p1 == p2);
}

#if 0
/* hash - hash a string; original author: P. J. Weinberger at Bell Labs. */
static LONG string_hash(VOID *t)
{
  register char *s = (CHAR *)t;
  register unsigned LONG h = 0;
  register unsigned LONG g;

    /*
     * For a performance comparison with the hash function presented in K&R,
     * first edition, see the "Dragon" book by Aho, Sethi and Ullman (p436).
     */

    while (*s) {
	h = (h << 4) + *s++;
	if (g = (h & 0xf0000000)) {
	    h ^= (g >> 24);
	    h ^= g;
	}
    }
    return (h);
}
#endif

/* see above for an alternate implementation */
static LONG string_hash(VOID *s)
{
  char *s1;
  LONG hash;

  s1=(char *)s;
  hash=0;
  while (*s1)
    {
      hash=((hash << 5) | (hash >> (8*sizeof(LONG)-5))) ^ *s1;
      ++s1;
    }
  return (hash);
}

static INT string_compare(CHAR *s1, CHAR *s2)
{ /* this is !strcmp to avoid funcall overhead */
  for(; *s1 == *s2; s1++, s2++)
  	if (*s1 == '\0') return TRUE;
  return FALSE;
}

#if 0
static INT string_compare(VOID *s1, VOID *s2)
{
  return (strcmp(s1, s2) == 0);
}
#endif

static VOID rehash(HashtableP h)
{
  LONG i;
  LONG oldsize;
  LONG pos;
  HASHRECP *newtable;
  HASHRECP p, q;

  oldsize=SIZE(h);
  h->size_index++;
  newtable=(HASHRECP *)malloc(SIZE(h)*sizeof(HASHRECP));
#ifdef DEBUG
  fprintf(stderr,"hash: %lu collisions, resizing table from %d to %d entries.\n", h->collisions, oldsize, SIZE(h));
#endif
#ifdef HASH_STATS
  h->collisions = 0;
#endif
  for (i=0; i < SIZE(h); ++i)
    newtable[i]=0;
  for (i=0; i < oldsize; ++i)
    for (p=h->table[i]; p; p=q)
      {
	q=p->next;
	pos=p->hash%SIZE(h);
	if (pos < 0)
	  pos= -pos;
	p->next=newtable[pos];
	newtable[pos]=p;
      }
  free((VOID *)h->table);
  h->table=newtable;
}


VOID hash_insert(HashtableP h, VOID *key, VOID *data)
{
  LONG pos;
  HASHRECP p;

  p=(HASHRECP)malloc((LONG)sizeof(struct HASHREC));
  p->key=key;
  p->hash=(*h->hash_fn)(key);
  p->data=data;
  pos=p->hash%SIZE(h);
  if (pos < 0)
  	pos= -pos;
#ifdef HASH_STATS
  if (h->table[pos]) h->collisions++;
#endif
  p->next=h->table[pos];
  h->table[pos]=p;
  h->entries++;
  if (h->entries > 4*SIZE(h))
    rehash(h);

}


INT hash_delete(HashtableP h, VOID *key)
{
  LONG hash;
  LONG pos;
  HASHRECP *p, q;

  hash=(*h->hash_fn)(key);
  pos=hash%SIZE(h);
  if (pos < 0)
    pos= -pos;
  for (p= &h->table[pos]; *p; p= &(*p)->next)
    if ((*p)->hash == hash && (*h->compare_fn)(key, (*p)->key))
      break;
  if (*p)
    {
      q= *p;
      *p=q->next;
      free((VOID *)q);
      h->entries--;
      return (1);
    }
  return (0);
}


VOID *hash_lookup(HashtableP h, VOID *s)
{
  LONG hash;
  LONG pos;
  HASHRECP p;
  hash=(*h->hash_fn)(s);
  pos=hash%SIZE(h);
  if (pos < 0)
    pos= -pos;
  for (p=h->table[pos]; p; p=p->next)
    if (p->hash == hash && (*h->compare_fn)(s, p->key))
	return (p->data);
  return ((VOID *)0);
}


HashtableP hash_new_generic_table(LONG (*hash_fn)(VOID *), INT (*compare_fn)())
{
  LONG i;
  HashtableP h;

  h=(HashtableP)malloc((LONG)sizeof(struct HASHTABLE));
  h->hash_fn=hash_fn;
  h->compare_fn=compare_fn;
  h->size_index=8;
  h->table=(HASHRECP *)malloc(SIZE(h)*sizeof(HASHRECP));
  for (i=0; i < SIZE(h); ++i)
    h->table[i]=0;
  h->entries=0;
  h->collisions=0;
  return (h);
}


HashtableP hash_new_string_table(VOID)
{
  return (hash_new_generic_table((LONG(*)(VOID*))string_hash, (INT(*)())string_compare));
}


HashtableP hash_new_pointer_table(VOID)
{
  return (hash_new_generic_table((LONG(*)(VOID*))pointer_hash, (INT(*)())pointer_compare));
}


VOID hash_free_table(HashtableP h)
{
  LONG i;
  HASHRECP p, q;

  for (i=0; i < SIZE(h); ++i)
    for (p=h->table[i]; p; p=q)
      {
	q=p->next;
	free((VOID *)p);
      }
  free((VOID *)h->table);
  free((VOID *)h);
}

#ifdef HASH_STATS
VOID hash_stats(HashtableP h)
{
  INT i, k, hist[MAX_HIST];
  HASHRECP p;

  for (i=0; i < MAX_HIST; i++)
    hist[i]=0;

  for (i=0; i < SIZE(h); i++) 
    {
      for (k=0, p=h->table[i]; p; p=p->next, k++)
	;
      hist[k]++;
    }

  fprintf(stderr,"hash: hash table size is %ld.\n", SIZE(h));
  fprintf(stderr,"hash: hash collisions were %ld.\n", h->collisions);
   
  for (i=0; (i < MAX_HIST) && (hist[i] != 0); i++)
    fprintf(stderr,"hash: %d chains with length %d.\n", hist[i], i);
      
}
#endif
