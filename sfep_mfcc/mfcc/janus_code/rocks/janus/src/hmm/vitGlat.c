/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: GLat alignment 
               ------------------------------------------------------------

    Author  :  Hagen Soltau
    Module  :  vitGlat.c
    Date    :  $Id: vitGlat.c 3106 2010-01-30 20:38:54Z metze $
    Remarks : 

   ========================================================================

    This software was developed by

    the  University of Karlsruhe       and  Carnegie Mellon University
         Dept. of Informatics               Dept. of Computer Science
         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
         Am Fasanengarten 5                 5000 Forbes Ave
         D-76131 Karlsruhe                  Pittsburgh, PA 15213
         - Germany -                        - USA -

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
    Revision 4.9  2007/03/23 14:32:14  fuegen
    get general end string instead of using ')' in glatInitAddon

    Revision 4.8  2007/03/19 14:31:08  fuegen
    some more MMIE related changes/ optimizations from Roger

    Revision 4.7  2007/02/21 17:03:36  fuegen
    code cleaning for windows from Roger and Christian

    Revision 4.6  2007/01/09 10:57:28  fuegen
    removed MPE code

    Revision 4.5  2006/11/10 10:15:39  fuegen
    merged changes from branch jtk-05-02-02-shajith
    - all modification related to MMIE training
    - first unverified functions for MPE training
    - modifications made by Shajith, Roger, Wilson, and Sebastian

    Revision 4.4  2005/12/20 15:13:42  metze
    Fixed cache problems

    Revision 4.3  2005/04/25 13:26:07  metze
    cbcClear works (max be slow, needed for MMIE)

    Revision 4.2  2005/03/08 09:33:39  metze
    Beautyfication

    Revision 4.1.12.1  2006/11/03 12:27:11  stueker
    Initial check-in of the MMIE from Shajith, Wilson, and Roger.

    Revision 4.1  2003/08/14 11:19:57  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.1  2003/02/05 09:19:14  soltau
    Initial revision


   ======================================================================== */

char  vitGlatRcsVersion[]=
    "$Id: vitGlat.c 3106 2010-01-30 20:38:54Z metze $";

#include "glat.h"
#include "path.h"
#include "hmm.h"
#include "codebook.h"
#include "distrib.h"
#include "distribStream.h"
#include "score.h"

/* ------------------------------------------------------------------------
    forward declarations
   ------------------------------------------------------------------------ */

/* array of path pointers to cache the temp results */
static Path** pathArray     = NULL;
static int  path_array_size = 0;
static int  num_path_needed = 0;
static int*        path_idx = NULL;
static int*        firstPhn = NULL;
static int*         lastPhn = NULL;
static int       num_phones = 0;
static int  num_uniq_phones = 0;
static int*    phn2uniq_idx = NULL;
static int*        phn_flag = NULL;
static int     phn_idx_size = 0;
static int    uniq_idx_size = 0;
static int    path_idx_size = 0;

extern PhoneGraph* pgraphExpand( PhoneGraph*        pgraphP,
				 PhoneGraphContext* left,  int leftN,
				 PhoneGraphContext* right, int rightN,
				 int**              pstateXA,
				 int**              prelXA,
				 int                xwmodels,
				 int                rcmSdp,
				 int**              psdpXA);

extern int   pathMake(   Path* path, SenoneSet* snsP, int from,  int to, 
			 int skipl, int skipt);

extern int   pathViterbi(Path* path, HMM* hmm, int from,   int to,
			 int skipl,  int skipt, 
			 float beam, int topN, int label,
			 int bpCleanUpMod, int bpCleanUpMul);
int pathViterbiMMIE (Path* path, HMM* hmm, PhoneGraph *pgraphP, int from,   int to, 
		     int skipl,  int skipt, 
		     float beam, int topN, int label,
		     int bpCleanUpMod, int bpCleanUpMul);
extern int   pathFwdBwd( Path* path, HMM* hmm, int from,   int to, 
			 int skipl,  int skipt, 
			 int topN, int width,  int label);

static int glatInitAddon (GLat* glatP, HMM* hmmP, Path *pathTmpP,
			  int variants, int modTags, 
			  int alignMode, int verbose) 
{
  SenoneSet*    snsP = hmmP->amodels->snsP;
  int        streamX = 0;  
  StreamArray*  staP = &(snsP->strA); 
  DistribSet*   dssP = ((DistribStream*) staP->cdA[streamX])->dssP;
  CodebookSet*  cbsP = dssP->cbsP;
  Dictionary*  dictP = hmmP->dict;
  SWord*      swordP = glatP->svmapP->svocabP->list.itemA;
  SVX           endX = svocabIndex (glatP->svmapP->svocabP, glatP->svmapP->svocabP->string.end);
  GNode*       nodeP = NULL;
  GLink*       linkP = NULL;
  int          nodeX = 0;
  int         cacheN = 50;
  PhoneGraphContext* leftA  = NULL;
  PhoneGraphContext* rightA = NULL;
  int i;

  int cur_path_idx = 0;
  char name[1024];
  int count;
  name[0] = 0;

  if (cbsP->cache && cacheN > cbsP->cache->distCacheN) {
    cbcDealloc(cbsP->cache);
    INFO("glatInitAddon: increased score cache to %d\n",cacheN);
    cbsP->cache->distCacheN = cacheN;
    cbcAlloc(cbsP->cache);
  }

  if (glatP->nodeN+glatP->linkN < 2) {
    if(cbsP->cache) cbcClear(cbsP->cache, cacheN);
    ERROR("glatInitAddon: empty lattice '%s'\n",glatP->name);
    return TCL_ERROR;
  }

  /* cross word phonetic context */
  if (NULL == (firstPhn = malloc(glatP->nodeN*sizeof(int)))) {
    if(cbsP->cache) cbcClear(cbsP->cache, cacheN);
    ERROR("glatInitAddon: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (lastPhn = malloc(glatP->nodeN*sizeof(int)))) {
    if(cbsP->cache) cbcClear(cbsP->cache, cacheN);
    ERROR("glatInitAddon: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (leftA = malloc(1*sizeof(PhoneGraphContext)))) {
    if(cbsP->cache) cbcClear(cbsP->cache, cacheN);
    ERROR("glatInitAddon: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (rightA = malloc(1*sizeof(PhoneGraphContext)))) {
    if(leftA) free(leftA);if(cbsP->cache) cbcClear(cbsP->cache, cacheN);
    ERROR("glatInitAddon: allocation failure\n");
    return TCL_ERROR;
  }

  num_phones = 0;
  for (nodeX=0;nodeX<glatP->nodeN;nodeX++) {
    GNode*     nodeP = glatP->mapping.x2n[nodeX];
    DictWord* dwordP = dictP->list.itemA + swordP[nodeP->svX].dictX;
    int        itemN = dwordP->word.itemN;
    firstPhn[nodeX]  = dwordP->word.phoneA[0];
    lastPhn[nodeX] = dwordP->word.phoneA[itemN-1];
    if(firstPhn[nodeX]>num_phones)
      num_phones = firstPhn[nodeX];
    if(lastPhn[nodeX]>num_phones)
      num_phones = lastPhn[nodeX];
  }

  if(phn_flag==NULL){
    phn_flag = malloc(sizeof(int)*num_phones);
    phn_idx_size = num_phones;
  }
  else if(phn_idx_size < num_phones){
    phn_flag = realloc(phn_flag, sizeof(int)*num_phones);
    phn_idx_size = num_phones;
  }
  for(i=0;i<phn_idx_size;i++){
    phn_flag[i] = 0;
  }

  if(phn2uniq_idx == NULL){
    phn2uniq_idx = malloc(sizeof(int)*num_phones);
    uniq_idx_size = num_phones;
  }
  else if(uniq_idx_size<num_phones){
    phn2uniq_idx = realloc(phn2uniq_idx, sizeof(int)*num_phones);
    uniq_idx_size = num_phones;
  }
  for(i=0;i<uniq_idx_size;i++){
    phn2uniq_idx[i] = -1;
  }

  for (nodeX=0;nodeX<glatP->nodeN;nodeX++) {
    phn_flag[firstPhn[nodeX]-1] = 1;
    phn_flag[lastPhn[nodeX]-1] = 1;
  }
  num_uniq_phones = 0;
  for(i=0;i<num_phones;i++){
    if(phn_flag[i]){
      phn2uniq_idx[i] = num_uniq_phones;
      num_uniq_phones++;
    }
  }
  INFO("glatInitAddon: nodeN: %d, num_uniq_phones: %d\n", glatP->nodeN, num_uniq_phones);

  /* clear hmm */
  sgraphReset(hmmP->stateGraph);
  
  nodeP=glatP->startP;
  linkP=nodeP->childP;
  nodeP->addon.parentCtxN=0;
  nodeP->addon.childCtxN=0;
  while (linkP) {
    nodeP->addon.childCtxN++;
    linkP=linkP->nextP;
  }
  if(nodeP->addon.childCtxN<=0) {
    if(leftA) free(leftA);if(rightA) free(rightA);if(cbsP->cache) cbcClear(cbsP->cache, cacheN);
    ERROR("glatInitAddon: node %d has no child\n",nodeP->nodeX);
    return TCL_ERROR;
  }
  nodeP->addon.parentP=NULL;
  nodeP->addon.childP=NULL;
  nodeP=glatP->endP;
  linkP=nodeP->parentP;
  nodeP->addon.parentCtxN=0;
  nodeP->addon.childCtxN=0;
  while (linkP) {
    nodeP->addon.parentCtxN++;
    linkP=linkP->txenP;
  }
  if(nodeP->addon.parentCtxN<=0) {
    if(leftA) free(leftA);if(rightA) free(rightA);if(cbsP->cache) cbcClear(cbsP->cache, cacheN);
    ERROR("glatInitAddon: node %d has no parent\n",nodeP->nodeX);
    return TCL_ERROR;
  }
  nodeP->addon.parentP=NULL;
  nodeP->addon.childP=NULL;

  /* yct compute how many path objects needed */

  num_path_needed = 0;
  for (nodeX=0;nodeX<glatP->nodeN;nodeX++) {
    nodeP = glatP->mapping.x2n[nodeX];
    
    linkP=nodeP->parentP;
    nodeP->addon.parentCtxN=0;
    while (linkP) {
      nodeP->addon.parentCtxN++;
      linkP = linkP->txenP;
    }
    linkP=nodeP->childP;
    nodeP->addon.childCtxN=0;
    while (linkP) {
      nodeP->addon.childCtxN++;
      linkP = linkP->nextP;
    }

    num_path_needed += nodeP->addon.parentCtxN * nodeP->addon.childCtxN;
  }

  /* allocate needed paths (or realloc) */
  if (pathArray == NULL) {
    pathArray = (Path**) malloc(sizeof(Path*) * num_path_needed);

    for (i=0; i < num_path_needed; i++) {
      sprintf(name, "pathTmp-%d", i);
      pathArray[i] = pathCreate(name);
    }

    path_array_size = num_path_needed;
  } else if (path_array_size < num_path_needed) {
    /* realloc */
    pathArray = realloc(pathArray, num_path_needed * sizeof(Path*));

    if (pathArray == NULL ) {
      ERROR("pathAlignGLat: realloc path object failed\n");
      return TCL_ERROR;
    }

    /* init new paths */
    for (i=path_array_size; i < num_path_needed; i++) {
      sprintf(name, "pathTmp-%d", i);
      pathArray[i] = pathCreate(name);
    }

    path_array_size = num_path_needed;
  }

  /* clear the path objects */
  for (i=0; i < path_array_size; i++)
    pathReset(pathArray[i]);

  INFO("glatInitAddon: # path objects needed: %d\n",num_path_needed);
  /*  yct end */

  /*hsiao starts*/
  /*init path_idx*/
  if(path_idx == NULL){
    path_idx = (int*) malloc(sizeof(int)*glatP->nodeN*(num_uniq_phones+1)*(num_uniq_phones+1));
    path_idx_size = glatP->nodeN*(num_uniq_phones+1)*(num_uniq_phones+1);
  }
  else if(path_idx_size<glatP->nodeN*(num_uniq_phones+1)*(num_uniq_phones+1)){
    path_idx_size = glatP->nodeN*(num_uniq_phones+1)*(num_uniq_phones+1);
    path_idx = realloc(path_idx,  path_idx_size*sizeof(int));    
  }
  for(i=0;i<path_idx_size;i++){
    path_idx[i] = -1;
  }
  INFO("glatInitAddon: # path_idx_size : %d\n", path_idx_size);
  
  /*alloc all addon variables first, instead doing alloc with the computation at the same time*/
  for (nodeX=0;nodeX<glatP->nodeN;nodeX++) {

    nodeP = glatP->mapping.x2n[nodeX];
    
    if(nodeP->addon.parentCtxN<=0 || nodeP->addon.childCtxN<=0) {
      if(leftA) free(leftA);if(rightA) free(rightA);if(cbsP->cache) cbcClear(cbsP->cache, cacheN);
      ERROR("glatInitAddon: node %d has no parent or child\n",nodeP->nodeX);
      return TCL_ERROR;
    }

    nodeP->addon.parentP=(GNode **)calloc(nodeP->addon.parentCtxN,sizeof(GNode *));
    nodeP->addon.childP=(GNode **)calloc(nodeP->addon.childCtxN,sizeof(GNode *));
    nodeP->addon.score=(float **)calloc(nodeP->addon.parentCtxN,sizeof(float *));
    nodeP->addon.alpha=(float **)calloc(nodeP->addon.parentCtxN,sizeof(float *));
    nodeP->addon.beta=(float **)calloc(nodeP->addon.parentCtxN,sizeof(float *));
    nodeP->addon.gamma=(float **)calloc(nodeP->addon.parentCtxN,sizeof(float *));
    nodeP->addon.lmScore=(float *)calloc(nodeP->addon.parentCtxN,sizeof(float));
    if(nodeP->addon.parentP==NULL || nodeP->addon.childP==NULL || 
       nodeP->addon.score==NULL || nodeP->addon.alpha==NULL || 
       nodeP->addon.beta==NULL || nodeP->addon. gamma==NULL ||
       nodeP->addon.lmScore==NULL) {
      if(leftA) free(leftA);if(rightA) free(rightA);if(cbsP->cache) cbcClear(cbsP->cache, cacheN);
      ERROR("glatInitAddon: allocation failure.\n");
      return TCL_ERROR;
    }
    for(i=0;i<nodeP->addon.parentCtxN;i++) {
      nodeP->addon.score[i]=(float *)calloc(nodeP->addon.childCtxN,sizeof(float));
      nodeP->addon.alpha[i]=(float *)calloc(nodeP->addon.childCtxN,sizeof(float));
      nodeP->addon.beta[i]=(float *)calloc(nodeP->addon.childCtxN,sizeof(float));
      nodeP->addon.gamma[i]=(float *)calloc(nodeP->addon.childCtxN,sizeof(float));
      if(nodeP->addon.score[i]==NULL || nodeP->addon.alpha[i]==NULL ||
	 nodeP->addon.beta[i]==NULL || nodeP->addon.gamma[i]==NULL) {
	if(leftA) free(leftA);if(rightA) free(rightA);if(cbsP->cache) cbcClear(cbsP->cache, cacheN);
	ERROR("glatInitAddon: allocation failure.\n");
	return TCL_ERROR;
      }
    }
  }
  /*hsiao ends*/

  for (nodeX=0;nodeX<glatP->nodeN;nodeX++) {
    PhoneGraph* pcigraphP = NULL;
    PhoneGraph* pcdgraphP = NULL;
    GNode*          nodeP = glatP->mapping.x2n[nodeX];
    DictWord*      dwordP = dictP->list.itemA + swordP[nodeP->svX].dictX;
    int             itemN = dwordP->word.itemN;
    int parentX, childX, leftN, rightN, left_idx, right_idx;
    int ret, j;
    
    linkP = nodeP->parentP; i=0;
    while(linkP) {
      nodeP->addon.parentP[i++]=linkP->parentP;
      linkP=linkP->txenP;
    }
    linkP = nodeP->childP; i=0;
    while(linkP) {
      nodeP->addon.childP[i++]=linkP->childP;
      linkP=linkP->nextP;
    }

    if( nodeP->svX == endX && nodeP->frameS>nodeP->frameE) {
      for(parentX=0;parentX<nodeP->addon.parentCtxN;parentX++) {
        /*hsiao starts*/
        if(nodeP->addon.parentP[parentX]->nodeX>=0) {
          leftN=1;
          leftA[0].phone=lastPhn[nodeP->addon.parentP[parentX]->nodeX];
          leftA[0].tag=dictP->weTags;
        }
        /*hsiao ends*/

	for(childX=0;childX<nodeP->addon.childCtxN;childX++) {
          /*hsiao starts*/
          rightN=0;
          if(nodeP->addon.childP[childX]->nodeX>=0) {
            rightN=1;
            rightA[0].phone=firstPhn[nodeP->addon.childP[childX]->nodeX];
            rightA[0].tag=dictP->wbTags;
          }
          nodeP->addon.score[parentX][childX]=0.0;
          left_idx = leftA[0].phone-1;
	  right_idx = rightA[0].phone-1;
          if(rightN==0)
            right_idx = num_uniq_phones;
          else
            right_idx = phn2uniq_idx[right_idx];
          left_idx = phn2uniq_idx[left_idx];

          path_idx[nodeX*(num_uniq_phones+1)*(num_uniq_phones+1)+left_idx*(num_uniq_phones+1)+right_idx] = cur_path_idx;
          /*hsiao ends*/

	  /* yct: path skipped */
          cur_path_idx++;
	  /* end */
        }
      }
      continue;
    }

    pcigraphP=pgraphCreate(hmmP->amodels,dictP,swordP[nodeP->svX].dictX,variants,modTags);

    for(parentX=0;parentX<nodeP->addon.parentCtxN;parentX++) {
      leftN=0;
      if(nodeP->addon.parentP[parentX]->nodeX>=0) {
	leftN=1;
	leftA[0].phone=lastPhn[nodeP->addon.parentP[parentX]->nodeX];
	leftA[0].tag=dictP->weTags;
      }
      for(childX=0;childX<nodeP->addon.childCtxN;childX++) {
	rightN=0;
	if(nodeP->addon.childP[childX]->nodeX>=0) {
	  rightN=1;
	  rightA[0].phone=firstPhn[nodeP->addon.childP[childX]->nodeX];
	  rightA[0].tag=dictP->wbTags;
        }
        /* hsiao starts */
        left_idx = leftA[0].phone-1;
        right_idx = rightA[0].phone-1;
        if(leftN==0)
          left_idx = num_uniq_phones;
        else
          left_idx = phn2uniq_idx[left_idx];

        if(rightN==0)
          right_idx = num_uniq_phones;
        else
          right_idx = phn2uniq_idx[right_idx];

        if(path_idx[nodeX*(num_uniq_phones+1)*(num_uniq_phones+1)+left_idx*(num_uniq_phones+1)+right_idx]!=-1){ /*if the path object is computed b4... */
          pathTmpP = pathArray[path_idx[nodeX*(num_uniq_phones+1)*(num_uniq_phones+1)+left_idx*(num_uniq_phones+1)+right_idx]];
          nodeP->addon.score[parentX][childX]=pathTmpP->gscore;
          continue;
        }
        /*hsiao ends*/

	pcdgraphP=pgraphExpand(pcigraphP,leftA,leftN,rightA,rightN,NULL,NULL,
			       hmmP->xwmodels,hmmP->rcmSdp,NULL);
	
	if( itemN==1 && hmmP->rcmSdp) {
	  if(leftN==0) leftN=1;
	  if(rightN==0) rightN=1;
	  pcdgraphP->initN=leftN*rightN;
	  if(pcdgraphP->initN > pcdgraphP->phoneN) {
	    if(leftA) free(leftA);if(rightA) free(rightA);if(cbsP->cache) cbcClear(cbsP->cache, cacheN);
	    if(pcigraphP) pgraphFree(pcigraphP); if(pcdgraphP) pgraphFree(pcdgraphP);
	    ERROR("glatInitAddon: initialization problem at '%s' from %d to %d\n",
		  swordP[nodeP->svX].name,nodeP->frameS,nodeP->frameE);
	    return TCL_ERROR;
	  }
	} else {
	  pcdgraphP->initN=1;
	}
	if(NULL == (pcdgraphP->init = (int *)malloc(pcdgraphP->initN*sizeof(int)))) {
	  if(leftA) free(leftA);if(rightA) free(rightA);if(cbsP->cache) cbcClear(cbsP->cache, cacheN);
	  if(pcigraphP) pgraphFree(pcigraphP); if(pcdgraphP) pgraphReset(pcdgraphP);
	  ERROR("glatInitAddon: allocation failure\n");
	  return TCL_ERROR;
	}
	for(i=0;i<pcdgraphP->initN;i++)
	  pcdgraphP->init[i]=i;
	
	pgraphMakeSGraph(pcdgraphP);
	for(i=0;i<pcdgraphP->phoneN;i++) {
	  for(j=0;j<pcdgraphP->stateGraph[i]->stateN;j++) {
	    pcdgraphP->stateGraph[i]->phoneX[j]=i;
	    pcdgraphP->stateGraph[i]->wordX[j]=0;
	  }
	}
	sgraphBuild(hmmP->stateGraph,pcdgraphP,hmmP->logPen);

	/* yct access to the current path object for the current <nodeX, parentX, childX>*/
        if (cur_path_idx >= num_path_needed) {
	  ERROR("glatInitAddon: patharray array out-of-bound!\n");
	  return TCL_ERROR;
        }

        pathTmpP = pathArray[cur_path_idx];
        path_idx[nodeX*(num_uniq_phones+1)*(num_uniq_phones+1)+left_idx*(num_uniq_phones+1)+right_idx] = cur_path_idx; /*hsiao: save the idx*/

        cur_path_idx++;
	/* end yct */
	
	switch(alignMode) {
	case 0 : 
	  ret = pathViterbiMMIE (pathTmpP,hmmP,pcdgraphP,nodeP->frameS, nodeP->frameE,0,0,500,0,0,-1,-1); 
	  break;
	case 1 : 
	  ret = pathFwdBwd  (pathTmpP,hmmP,nodeP->frameS, nodeP->frameE,0,0,100,50,0); 
	  break;
	default : 
	  ret = TCL_ERROR; 
	  WARN("glatInitAddon : unsupported alignment mode '%d'\n",alignMode);
	  break;
	}
        
	
	sgraphReset(hmmP->stateGraph);
	pgraphFree(pcdgraphP);
	
	if (ret == TCL_ERROR || pathTmpP->gscore <= 0) {
	  if(!strcmp(swordP[nodeP->svX].name,")")) {
	    if(leftA) free(leftA);if(rightA) free(rightA);if(cbsP->cache) cbcClear(cbsP->cache, cacheN);
	    if(pcigraphP) pgraphFree(pcigraphP);
	    ERROR("glatInitAddon : alignment failed for '%s' from %d to %d\n",
		  swordP[nodeP->svX].name,nodeP->frameS,nodeP->frameE);
	    return TCL_ERROR;
	  } else {
	    if(leftA) free(leftA);if(rightA) free(rightA);if(cbsP->cache) cbcClear(cbsP->cache, cacheN);
	    if(pcigraphP) pgraphFree(pcigraphP);
	    ERROR("glatInitAddon : alignment failed for '%s' from %d to %d\n",
		  swordP[nodeP->svX].name,nodeP->frameS,nodeP->frameE);
	    return TCL_ERROR;
	  }
	}
	nodeP->addon.score[parentX][childX]=pathTmpP->gscore;
      } /* child context */
    } /* parent context */

    if(pcigraphP) pgraphFree(pcigraphP);

  } /* each node */

  /* hsiao starts */
  count=0;
  for(i=0;i<path_idx_size;i++){
    if(path_idx[i] != -1){
      count++;
    }
  }
  fprintf(stderr, "effective path objects: %d\n", count);
  /*hsiao ends*/

  if(leftA) free(leftA);
  if(rightA) free(rightA);

  if(cbsP->cache) cbcClear(cbsP->cache, cacheN);
  return TCL_OK;
}

int glatInitAddonItf( ClientData clientData, int argc, char *argv[])
{
  GLat*       glatP = (GLat *)clientData;
  Path    *pathTmpP = NULL;
  HMM*        hmmP  = NULL;
  int      variants = 0;
  int       modtags = 0;
  int     alignMode = 0; 
  int       verbose = 0; 
  int            ac = argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       "<hmm>",   ARGV_OBJECT, NULL, &hmmP, "HMM",  "Hidden Markov Model",
       "<pathTmp>", ARGV_OBJECT, NULL, &pathTmpP, "Path", "Temporary path variable",
       "-variants",  ARGV_INT, NULL, &variants, NULL,"pronunciation variants",
       "-modtags",   ARGV_INT, NULL, &modtags,  NULL,"modality tags",
       "-mode",      ARGV_INT, NULL, &alignMode,NULL,"alignment mode, 0=viterbi, 1=fwdbwd",
       "-verbose",   ARGV_INT, NULL, &verbose,  NULL,"verbosity",
     NULL) != TCL_OK) return TCL_ERROR;

  return glatInitAddon(glatP,hmmP,pathTmpP,variants,modtags,alignMode,verbose);
}

static int glatClearAddon(GLat *glatP)
{
  int nodeX;
  int i;

  /* hsiao starts */
  if(firstPhn){ free(firstPhn); firstPhn=NULL; }
  if(lastPhn){ free(lastPhn); lastPhn=NULL; }

  /* hsiao ends */
    
  for(nodeX=0;nodeX<glatP->nodeN;nodeX++) {
    GNode *nodeP = glatP->mapping.x2n[nodeX];
    if(nodeP->addon.parentP) free(nodeP->addon.parentP);
    if(nodeP->addon.childP) free(nodeP->addon.childP);
    for(i=0;i<nodeP->addon.parentCtxN;i++) {
      if(nodeP->addon.score[i]) free(nodeP->addon.score[i]);
      if(nodeP->addon.alpha[i]) free(nodeP->addon.alpha[i]);
      if(nodeP->addon.beta[i]) free(nodeP->addon.beta[i]);
      if(nodeP->addon.gamma[i]) free(nodeP->addon.gamma[i]);
    }
    if(nodeP->addon.score) free(nodeP->addon.score);
    if(nodeP->addon.alpha) free(nodeP->addon.alpha);
    if(nodeP->addon.beta) free(nodeP->addon.beta);
    if(nodeP->addon.gamma) free(nodeP->addon.gamma);
    if(nodeP->addon.lmScore) free(nodeP->addon.lmScore);
    nodeP->addon.parentCtxN=0;
    nodeP->addon.childCtxN=0;
  }

  /* FMe: I think we shouldn't call the code below unless we did some stuff (it segfaults) */
  if (!glatP->startP->addon.parentCtxN ||
      !glatP->startP->addon.childCtxN  ||
      !glatP->endP->addon.parentCtxN   ||
      !glatP->endP->addon.childCtxN)
    return TCL_OK;

  if(glatP->startP->addon.parentP) free(glatP->startP->addon.parentP);
  if(glatP->startP->addon.childP) free(glatP->startP->addon.childP);
  if(glatP->endP->addon.parentP) free(glatP->endP->addon.parentP);
  if(glatP->endP->addon.childP) free(glatP->endP->addon.childP);
  glatP->startP->addon.parentCtxN=0;
  glatP->startP->addon.childCtxN=0;
  glatP->endP->addon.parentCtxN=0;
  glatP->endP->addon.childCtxN=0;

  return TCL_OK;
}

int glatClearAddonItf(ClientData clientData, int argc, char *argv[])
{
  GLat *glatP=(GLat *)clientData;

  glatClearAddon(glatP);

  return TCL_OK;
}


static int pathAlignGLat (Path *pathP, GLat* glatP, HMM* hmmP, Path *pathTmpP,
			  int variants, int modTags, 
			  float gammaThresh, int alignMode,
			  int verbose) 
{
  int    nodeX = 0;
  /* yct */
  int   cur_path_idx = 0;

  if (glatP->nodeN+glatP->linkN < 2) {
    ERROR("pathAlignGLat: empty lattice '%s'\n",glatP->name);
    return TCL_ERROR;
  }

  for (nodeX=0;nodeX<glatP->nodeN;nodeX++) {
    GNode*        nodeP = glatP->mapping.x2n[nodeX];

    int parentX, childX;
    for(parentX=0;parentX<nodeP->addon.parentCtxN;parentX++) {
      for(childX=0;childX<nodeP->addon.childCtxN;childX++) {
        /* hsiao starts */
        GNode** parentPA = nodeP->addon.parentP;
        GNode** childPA  = nodeP->addon.childP;

        int left_idx=num_uniq_phones;
        int right_idx=num_uniq_phones;
	double gamma;
	int frX;

        left_idx=num_uniq_phones;
        right_idx=num_uniq_phones;
        if(parentPA[parentX]->nodeX>=0)
          left_idx = phn2uniq_idx[lastPhn[parentPA[parentX]->nodeX]-1];
        if(childPA[childX]->nodeX>=0)
          right_idx = phn2uniq_idx[firstPhn[childPA[childX]->nodeX]-1];

        cur_path_idx = path_idx[nodeX*(num_uniq_phones+1)*(num_uniq_phones+1) + left_idx*(num_uniq_phones+1) + right_idx];
        if(cur_path_idx >= num_path_needed){
          ERROR("pathAlignGLat: patharray array out-of-bound!\n");
          return TCL_ERROR;
        }
        pathTmpP = pathArray[cur_path_idx];
        /*hsiao ends*/

        gamma = exp (-nodeP->addon.gamma[parentX][childX]);

	/* add path info: only gamma and senoneX are valid */

	for (frX=nodeP->frameS;frX<nodeP->frameE;frX++) {
	  PathItem*    fromP = pathTmpP->pitemListA[frX-nodeP->frameS].itemA;
	  PathItem*      toP = pathP->pitemListA[frX].itemA;
	  int          fromN = pathTmpP->pitemListA[frX-nodeP->frameS].itemN;
	  int           oldN = pathP->pitemListA[frX].itemN;
	  int           newN = 0;
          int i;
	  
	  if (NULL == (toP = realloc(toP,(oldN+fromN)*sizeof(PathItem)))) {
	    ERROR("pathAlignGLat: allocation failure\n");
	    return TCL_ERROR;
	  }
	  
	  for (i=0; i<fromN; i++) {
	    int    snX = fromP[i].senoneX;
	    double   g = fromP[i].gamma;
	    int      j = oldN+newN-1;
	    
	    /* check for same senones */
	    while (j>=0 && toP[j].senoneX != snX) j--;
	    if (j < 0) {
	      j = oldN+newN;
	      pathItemInit(toP+j);
	      newN++;
	    }
	    toP[j].senoneX  = snX;
	    toP[j].gamma   += gamma*g;
	  }
	  /* reduce malloc */
	  pathP->pitemListA[frX].itemN = oldN+newN;
	  if (NULL == (pathP->pitemListA[frX].itemA = 
		       realloc(toP,(oldN+newN)*sizeof(PathItem)))) {
	    ERROR("pathAlignGLat: allocation failure\n");
	    return TCL_ERROR;
	  }
 	}

      } /* child context */
    } /* parent context */
  } /* each node */
  return TCL_OK;
}

int pathAlignGLatItf( ClientData clientData, int argc, char *argv[])
{
  Path*       pathP = (Path*)clientData;
  HMM*        hmmP  = NULL;
  Path    *pathTmpP = NULL;
  GLat*       glatP = NULL;
  int      variants = 0;
  int       modtags = 0;
  float gammathresh = 0.01;
  int     alignMode = 0; 
  int       verbose = 0; 
  int            ac = argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
		     "<hmm>",   ARGV_OBJECT, NULL, &hmmP, "HMM",  "Hidden Markov Model",
		     "<glat>",  ARGV_OBJECT, NULL, &glatP,"GLat", "IBIS Lattice object",
		     "<pathTmp>", ARGV_OBJECT, NULL, &pathTmpP, "Path", "Temporary path variable",
		     "-variants",  ARGV_INT, NULL, &variants, NULL,"pronunciation variants",
		     "-modtags",   ARGV_INT, NULL, &modtags,  NULL,"modality tags",
		     "-thresh",  ARGV_FLOAT, NULL, &gammathresh,NULL,"minimum posteriori threshold",
		     "-mode",      ARGV_INT, NULL, &alignMode,NULL,"alignment mode, 0=viterbi, 1=fwdbwd",
		     "-verbose",   ARGV_INT, NULL, &verbose,  NULL,"verbosity",
		     NULL) != TCL_OK) return TCL_ERROR;

  assert(pathP && hmmP);
  pathMake(pathP,amodelSetSenoneSet(hmmP->amodels),-1,-1,0,0);

  return pathAlignGLat(pathP,glatP,hmmP,pathTmpP,variants,modtags,gammathresh,alignMode,verbose);
}
