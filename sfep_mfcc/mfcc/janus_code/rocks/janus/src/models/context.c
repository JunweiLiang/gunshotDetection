
#include "context.h"


static int fillContextCacheItem(ContextCache *cc, ContextCacheItem *cci, Word* wp, int left, int right, int model)
{
  int  stateN, i;
  int  maxL, maxR, zentrum;
  int *tmpcKeyP;

  maxL    =  cc->maxLeftContext;
  maxR    =  cc->maxRightContext;
  zentrum = -left;

  left = (maxL > -1) ? MIN(maxL,zentrum) : zentrum;
  if (maxR > -1) right = MIN(maxR,right);

  if (cci->cKey) free(cci->cKey);
  stateN = (3 + 2*(right + left)); /* + zentrum */
  cci->cKey = (int *) malloc(sizeof(int) * stateN); 
  if (cci->cKey == NULL) return TCL_ERROR;

  tmpcKeyP = cci->cKey;
  for (i = zentrum - left; i <= zentrum + right; i++) {
    *tmpcKeyP++  = wp->phoneA[i];
    *tmpcKeyP++  = wp->tagA[i];
  }
  *tmpcKeyP      = zentrum;

  cci->modelX    = model;
  cci->stateN    = stateN;
  return TCL_OK;
}


static long contextCacheHashKey( const ContextCacheItem *key)
{
  unsigned long ret = 0; 
  int           stateX;

  for (stateX=0; stateX<key->stateN; stateX++) {
    ret = ((ret<<5) | (ret >> (8*sizeof(long)-5))) ^ key->cKey[stateX];
  }

  return ret;
}


static int contextItemHashInit (ContextCacheItem *cci, ContextCacheItem *key)
{
  cci->cKey    = key->cKey;
  cci->stateN  = key->stateN;
  cci->modelX  = key->modelX;

  return TCL_OK;
}

static int contextItemHashDeinit (ContextCacheItem *cci)
{

  if (cci->cKey) {free(cci->cKey); cci->cKey = NULL;}

  cci->cKey = NULL;
  cci->stateN = 0;

  return TCL_OK;
}

static int contextCacheHashCmp( const ContextCacheItem *key, const ContextCacheItem *cci)
{
  int stateX;
  
  if (key->stateN != cci->stateN) return 0;
  for (stateX = 0; stateX<key->stateN; stateX++) {
    if (key->cKey[stateX] != cci->cKey[stateX]) return 0;
  }

  return 1;
}

int indexInContextCache(ContextCache *cc, Word* wp, int left, int right) 
{
  int idx;

  ContextCacheItem key;
  key.cKey = NULL;

  fillContextCacheItem(cc, &key, wp, left, right, 0);
  idx = listIndex((List*) &(cc->list), &key, 0);
  cc->cacheQuerys++;
  if (idx >= 0) cc->cacheHits++;

  if (key.cKey) free(key.cKey);
  /* -1 means not in Cache */
  return idx;
}

int getModelOfContextCacheItem(ContextCache *cc, int idx)
{
  return cc->list.itemA[idx].modelX;
}

ContextCacheItem * getContextCacheItem(ContextCache *cc, int idx)
{
  return (ContextCacheItem *) cc->list.itemA + idx;
}

int insertInContextCache(ContextCache *cc, Word* wp, int left, int right, int modelX) 
{
  int idx;

  ContextCacheItem key;
  key.cKey = NULL;
  fillContextCacheItem(cc, &key, wp, left, right, modelX);
  idx = listNewItem((List*) &(cc->list), &key);

  return idx;
}

static int contextCacheInit(ContextCache* contextCache, char *name)
{
  contextCache->name = strdup(name);
  contextCache->useN = 0;
  contextCache->maxLeftContext  =  -1; /* -1 = unlimited */
  contextCache->maxRightContext =  -1; /* -1 = unlimited */
  contextCache->active          =  0; /* default is active only for Xwt */
  contextCache->cacheQuerys     =  0; /* counter */
  contextCache->cacheHits       =  0; /* counter */

  listInit((List*)&(contextCache->list), NULL, sizeof(ContextCacheItem), 50000);

  contextCache->list.init     = (ListItemInit  *)contextItemHashInit;
  contextCache->list.deinit   = (ListItemDeinit*)contextItemHashDeinit;
  contextCache->list.hashKey  = (HashKeyFn     *)contextCacheHashKey;
  contextCache->list.hashCmp  = (HashCmpFn     *)contextCacheHashCmp;

  return TCL_OK;
}

ContextCache* contextCacheCreate(char *name)
{
  ContextCache* cc;
  
  cc = (ContextCache*) malloc(sizeof(ContextCache));
  if (!cc) return NULL;

  contextCacheInit(cc, name);
  return cc;
}

ClientData contextCacheCreateItf( ClientData clientData, 
                                   int argc, char *argv[]) 
{ 
  int     ac     = argc-1;
  
  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		   NULL) != TCL_OK)
    return NULL;

  return (ClientData)contextCacheCreate(argv[0]);
}


static int contextCacheDeinit(ContextCache* contextCache)
{
  if (contextCache->name) {
    free(contextCache->name); 
    contextCache->name = NULL;
  };
  contextCache->useN = 0;
  contextCache->maxLeftContext  = -1; /* unlimited */
  contextCache->maxRightContext = -1; /* unlimited */
  contextCache->active          =  0; /* default is active only for Xwt */
  contextCache->cacheQuerys     =  0; /* counter */
  contextCache->cacheHits       =  0; /* counter */

  listDeinit((List*)&(contextCache->list));

  contextCache->list.init     = (ListItemInit  *)contextItemHashInit;
  contextCache->list.deinit   = (ListItemDeinit*)contextItemHashDeinit;
  contextCache->list.hashKey  = (HashKeyFn     *)contextCacheHashKey;
  contextCache->list.hashCmp  = (HashCmpFn     *)contextCacheHashCmp;

  return TCL_OK;
}

int contextCacheFree(ContextCache* cc)
{
  
  contextCacheDeinit(cc);
  free(cc);

  return TCL_OK;
}

static int contextCacheFreeItf (ClientData clientData) 
{ return contextCacheFree( (ContextCache*)clientData); }

/* ------------------------------------------------------------------------
    contextCacheConfigureItf
   ------------------------------------------------------------------------ */

static int contextCacheConfigureItf(ClientData cd, char *var, char *val)
{
  ContextCache *contextCache = (ContextCache *)cd;
  if (!contextCache) return TCL_OK;

  if (var==NULL) {
    ITFCFG(contextCacheConfigureItf,cd,"useN");
    ITFCFG(contextCacheConfigureItf,cd,"maxLeftContext");
    ITFCFG(contextCacheConfigureItf,cd,"maxRightContext");
    ITFCFG(contextCacheConfigureItf,cd,"active");
    ITFCFG(contextCacheConfigureItf,cd,"cacheQuerys");
    ITFCFG(contextCacheConfigureItf,cd,"cacheHits");
    return listConfigureItf((ClientData)&(contextCache->list), var, val);
  }
  ITFCFG_Int(    var,val,"useN",            contextCache->useN,          1);
  ITFCFG_Int(    var,val,"maxLeftContext",  contextCache->maxLeftContext,0);
  ITFCFG_Int(    var,val,"maxRightContext", contextCache->maxRightContext,0);
  ITFCFG_Int(    var,val,"active",          contextCache->active,0);
  ITFCFG_Int(    var,val,"cacheQuerys",     contextCache->cacheQuerys,1);
  ITFCFG_Int(    var,val,"cacheHits",       contextCache->cacheHits  ,1);
  return listConfigureItf((ClientData)&(contextCache->list), var, val);   
}

/* ------------------------------------------------------------------------
    contextCacheAccessItf
   ------------------------------------------------------------------------ */

static ClientData contextCacheAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  ContextCache* contextCache = (ContextCache*)cd;
  return listAccessItf((ClientData)&(contextCache->list),name,ti);
}

/* ------------------------------------------------------------------------
    contextCachePuts
   ------------------------------------------------------------------------ */

static int contextCachePutsItf (ClientData clientData, int argc, char *argv[]) 
{
  ContextCache* contextCache = (ContextCache*)clientData;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
        NULL) != TCL_OK)  return TCL_ERROR;

  return listPutsItf((ClientData)&(contextCache->list), argc, argv);
}

/* ------------------------------------------------------------------------
    contextCacheClear
   ------------------------------------------------------------------------ */

static int contextCacheClear (ContextCache* contextCache) 
{
  
  contextCache->cacheHits   = 0;
  contextCache->cacheQuerys = 0;

  return listClear((List*)&(contextCache->list));
}

static int contextCacheClearItf (ClientData clientData, int argc, char *argv[]) 
{
  ContextCache* contextCache = (ContextCache*)clientData;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
        NULL) != TCL_OK)  return TCL_ERROR;
  
  return contextCacheClear(contextCache);
}


/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method contextCacheMethod[] = 
{ 
  { "puts" , contextCachePutsItf,   NULL },
  { "clear", contextCacheClearItf,   NULL },
  {  NULL,  NULL, NULL } 
};

TypeInfo contextCacheInfo = { "ContextCache", 0, 0, contextCacheMethod, 
			      contextCacheCreateItf, contextCacheFreeItf, 
			      contextCacheConfigureItf, contextCacheAccessItf,
			      NULL,
			      "ContextCache stores amodel index\n" };


int ContextCache_Init ( void )
{
  static int contextCacheInitialized;

  if (! contextCacheInitialized) {
    itfNewType (&contextCacheInfo);
    contextCacheInitialized = 1;
  }
  return TCL_OK;
}

