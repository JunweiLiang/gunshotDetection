#ifdef __cplusplus
extern "C" {
#endif

#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include "list.h"
#include "word.h"

typedef struct
{
  int    stateN; /* phonesSeqN + TagSeqN + 1 for Zentrum */
  int   *cKey;
  int    modelX;
} ContextCacheItem_S;
typedef ContextCacheItem_S ContextCacheItem;

typedef struct LIST(ContextCacheItem) ContextCacheList;

typedef struct
{
  char            *name;
  int              useN;
  int              maxLeftContext;   /* -1 = unlimited */
  int              maxRightContext;  /* -1 = unlimited */
  int              active;           /* 1 == use cache */
  long             cacheQuerys;       /* number of chache querys */
  long             cacheHits;        /* number of chache hits */
  ContextCacheList list;

} ContextCache_S;

typedef ContextCache_S ContextCache;

int insertInContextCache(ContextCache *cc, Word* wp, int left, int right, int modelX);
ContextCache* contextCacheCreate(char *name);
int contextCacheFree(ContextCache* cc);
int indexInContextCache(ContextCache *cc, Word* wp, int left, int right);
ContextCacheItem * getContextCacheItem(ContextCache *cc, int idx);
int getModelOfContextCacheItem(ContextCache *cc, int idx);

#endif

#ifdef __cplusplus
}
#endif
