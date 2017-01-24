#include "gs.h"


static GSClusterSet GSClusterSetDefaults;

/* ----------------------------------------------------------------------
   cluster modes
   ---------------------------------------------------------------------- */
enum { gs_mode_euclidean        = 0, /* no covariance, set it always to 1.0.
					Mode 0 has turned out to be the most
					stable in making the ClusterSet.
					I don't recommend the other distance
					types for clustering, mode 5 performs
					best on evaluation */
       gs_mode_mahalanobis      = 1, /* calculate real clusterwise covariances,
					does not work well while clustering */
       gs_mode_globalaverage    = 2, /* calculate centroids covariance by
					averaging over all gaussians */
       gs_mode_clusteraverage   = 3, /* calculate centroids covariance by
					averaging over cluster-intern
					gaussians */
       gs_mode_globalaverage_i  = 4, /* same like 2, but mathmatical correct
					averaging (invert the
					inverse-covariances, average, invert
					again */
       gs_mode_clusteraverage_i = 5, /* same like 3, but correct */
       gs_mode_representant     = 6, /* dont use an artifical, calculated
					centroid, choose a real gaussian as a
					representant */
       gs_mode_max              = 7  /* maximum number of modes, for error
					checking */
};

/* ----------------------------------------------------------------------
   some forward declarations
   ---------------------------------------------------------------------- */
extern int  gsClusterSetClear  (GSClusterSet *gscsP);
extern void gsCompressClusters (GSClusterSet *gscsP);
extern void gsUpdateMeans      (GSClusterSet *gscsP, int clusterA);
extern void gsUpdateCovars     (GSClusterSet *gscsP, int clusterA);


/* ----------------------------------------------------------------------
   Init/Create Gaussian Selection Cluster Set
   ---------------------------------------------------------------------- */
int gsClusterSetInit (GSClusterSet *gscsP, char *name, CodebookSet *cbsP) {

  Codebook *cbP = cbsP->list.itemA + 0;

  gscsP->name               = strdup(name);
  gscsP->cluster            = GSClusterSetDefaults.cluster;
  gscsP->clusterN           = GSClusterSetDefaults.clusterN;
  gscsP->clusterX           = GSClusterSetDefaults.clusterX;
  gscsP->bestN              = GSClusterSetDefaults.bestN;
  gscsP->backoffMode        = GSClusterSetDefaults.backoffMode;
  gscsP->backoffValue       = GSClusterSetDefaults.backoffDefault;
  gscsP->backoffValueI      = (unsigned int)(GSClusterSetDefaults.backoffDefault * cbsP->scaleRV * cbsP->scaleRV * cbsP->scaleCV);
  gscsP->backoffDefault     = GSClusterSetDefaults.backoffDefault;
  gscsP->backoffFactor      = GSClusterSetDefaults.backoffFactor;
  gscsP->decrease           = GSClusterSetDefaults.decrease;
  gscsP->eval_frames        = GSClusterSetDefaults.eval_frames;
  gscsP->gaussians_computed = GSClusterSetDefaults.gaussians_computed;
  gscsP->gaussians_total    = GSClusterSetDefaults.gaussians_total;
  gscsP->cbsP               = cbsP;
  if (cbP) gscsP->dimN = (cbP->cfg)->rdimN > 0 ? (cbP->cfg)->rdimN : cbP->dimN;
  else gscsP->dimN = 0;
  gscsP->use                = GSClusterSetDefaults.use;
  gscsP->mode               = GSClusterSetDefaults.mode;
  gscsP->gaussianN          = GSClusterSetDefaults.gaussianN;
  gscsP->weightC            = GSClusterSetDefaults.weightC;
  gscsP->diff               = GSClusterSetDefaults.diff;
	
  cbsP->gscsP               = gscsP;

  return TCL_OK;
}

GSClusterSet* gsClusterSetCreate (char *name, CodebookSet *cbsP) {

  GSClusterSet *gscsP = (GSClusterSet*)malloc(sizeof(GSClusterSet));

  if ( !gscsP || gsClusterSetInit (gscsP, name, cbsP) != TCL_OK ) {
    if ( gscsP ) Nulle (gscsP);
    ERROR ("Cannon create GSClusterSet object\n");
    return NULL;
  }

  return gscsP;
}

static ClientData gsClusterSetCreateItf (ClientData cd, int argc, char *argv[]) {
  CodebookSet   *cbsP = NULL;
  char          *name = NULL; 
  int              ac = argc;
	
  if (itfParseArgv (argv[0], &ac, argv, 1,
		    "<name>",         ARGV_STRING, NULL, &name,            NULL, "name of the GSClusterSet",
		    "<cbs>",          ARGV_OBJECT, NULL, &cbsP,   "CodebookSet", "name of CodebookSet",
		    NULL) != TCL_OK) return NULL;

  return (ClientData)gsClusterSetCreate (name, cbsP);
}

/* ----------------------------------------------------------------------
   allocate a ClusterSet
   ---------------------------------------------------------------------- */
int gsClusterSetAlloc (GSClusterSet *gscsP, int clusterN, int bestN, int mode) {
  CodebookSet    *cbsP = gscsP->cbsP;
  Codebook        *cbP = cbsP->list.itemA + 0;
  int             dimN = (cbP->cfg)->rdimN > 0 ? (cbP->cfg)->rdimN : cbP->dimN;
  int             succ = 0;
  int              cbN = cbsP->list.itemN;
  int             refN = cbP->refN;
  int         badcount = 0;
  int clusterX,cbX,refX,dimX,refCount;

  gsClusterSetClear (gscsP);

  gscsP->clusterN  = clusterN;
  gscsP->bestN     = bestN;
  gscsP->mode      = mode;
  gscsP->gaussianN = 0;
  gscsP->dimN      = dimN;

  if (clusterN<1 || clusterN>(int)SHRT_MAX) {
    ERROR("Invalid number of clusters specified: %d (1...%d)\n",clusterN,SHRT_MAX);
    return 0;
  }

  INFO("Allocating %dx GS ClusterSet (FS: %d dims).\n",clusterN,dimN);

  /* allocate cluster table */
  if ( (gscsP->cluster = malloc(clusterN*sizeof(GSCluster))) ) {
    if ( (gscsP->cluster[0].mean = malloc(clusterN*dimN*sizeof(float)*2)) ) {
      if ( (gscsP->cluster[0].meanC = malloc(clusterN*dimN*sizeof(int)*2 + dimN*sizeof(int))) ) {
				
	gscsP->cluster[0].covar=gscsP->cluster[0].mean+clusterN*dimN;
	gscsP->cluster[0].covarC=gscsP->cluster[0].meanC+clusterN*dimN;
	gscsP->weightC=gscsP->cluster[0].covarC+clusterN*dimN;
				
	for (clusterX=1; clusterX<clusterN; clusterX++) {
	  gscsP->cluster[clusterX].mean  = gscsP->cluster[0].mean  + clusterX*dimN;
	  gscsP->cluster[clusterX].covar = gscsP->cluster[0].covar + clusterX*dimN;
	  gscsP->cluster[clusterX].meanC  = gscsP->cluster[0].meanC  + clusterX*dimN;
	  gscsP->cluster[clusterX].covarC = gscsP->cluster[0].covarC + clusterX*dimN;
	}
	succ++;

	/* clean mean values */
	for (clusterX=0; clusterX<clusterN;clusterX++) {
	  for (dimX=0; dimX<dimN; dimX++) {
	    gscsP->cluster[clusterX].mean[dimX] = 0.0;
	    gscsP->cluster[clusterX].covar[dimX] = 1.0;
	  }
	  gscsP->cluster[clusterX].count = 0;
	  gscsP->cluster[clusterX].active_count = 0;
	}
      } else ERROR("Not enough memory for clusterSet! #1.5");
    } else ERROR("Not enough memory for clusterSet! #1");
  } else ERROR("Not enough memory for clusterSet! #2");

  /* allocate gaussian=>cluster assign table */
  refCount=0;
  for (cbX=0; cbX<cbN; cbX++) { /* count the total number of gaussians */
    cbP = cbsP->list.itemA + cbX;
	refCount+=cbP->refN;
  }
  gscsP->clusterX = (short**)malloc(cbN*sizeof(short*)); /* allocate pointer table */
  if (gscsP->clusterX) {
    gscsP->clusterX[0] = (short*)malloc(refCount*sizeof(short)); /* allocate assign table */
	if (gscsP->clusterX[0]) {
      for (cbX=0; cbX<cbN-1; cbX++) { /* set pointers */
        cbP = cbsP->list.itemA + cbX;
	    gscsP->clusterX[cbX+1]=gscsP->clusterX[cbX]+cbP->refN;
      }
      for (cbX=0; cbX<cbN; cbX++) { /* init assign table */
		int defaultCluster=-1;
        cbP = cbsP->list.itemA + cbX;
		refN = cbP->refN;
		if (cbP->rv) defaultCluster=0;

		for (refX=0; refX<refN; refX++) {
			gscsP->clusterX[cbX][refX]=defaultCluster;
	        if (defaultCluster==0) {
	          gscsP->gaussianN++;
	          for (dimX=0; dimX<dimN; dimX++) {
	            if (cbP->rv->matPA[refX][dimX]==0) {
		          /* ERROR ("CodebookSet contains bad gaussian (%d x %d x %d): mean = %2.5f / covar = %2.5f - gaussian deactivated for clustering!\n",cbX,refX,dimX,cbP->rv->matPA[refX][dimX],cbP->cv[refX]->m.d[dimX]); */
		          gscsP->clusterX[cbX][refX] = -1;
		          dimX=dimN;
		          gscsP->gaussianN--;
		          badcount++;
	            }
	          }
			}
		}
      }
	  succ++;
	}
  }

  if (succ>=2) succ=-1; else succ=0; /* everything done ? */
  if (badcount>=0) {
    INFO("Removed %d bad gaussians (not used or a mean value of zero)\n",badcount);
  } else {
    INFO("No floating point gaussians are present - we have to trust the GSClusterSet file about the number of gaussians\n");
  }
  return succ;
}

/* ----------------------------------------------------------------------
   Deinit/Free Gaussian Selection Cluster Set
   ---------------------------------------------------------------------- */
int gsClusterSetClear (GSClusterSet *gscsP) {

  if ( gscsP->cluster ) {
    if ( gscsP->cluster[0].mean   ) Nulle (gscsP->cluster[0].mean);
    if ( gscsP->cluster[0].meanC  ) Nulle (gscsP->cluster[0].meanC);
    if ( gscsP->cluster[0].covar  ) Nulle (gscsP->cluster[0].covar);
    if ( gscsP->cluster[0].covarC ) Nulle (gscsP->cluster[0].covarC);
    Nulle (gscsP->cluster);
  }

  if (gscsP->clusterX) {
	  if (gscsP->clusterX[0]) Nulle (gscsP->clusterX[0]);
	  Nulle (gscsP->clusterX);
  }

  gscsP->clusterN = 0;

  return TCL_OK;
}

int gsClusterSetFree (GSClusterSet *gscsP) {

  if ( !gscsP ) return TCL_OK;

  gsClusterSetClear (gscsP);

  if (gscsP->cbsP && gscsP->cbsP->gscsP) gscsP->cbsP->gscsP = NULL;
  Nulle (gscsP);
	
  return TCL_OK;
}

static int gsClusterSetFreeItf (ClientData cd) {

  return gsClusterSetFree ((GSClusterSet*)cd);
}


/* ----------------------------------------------------------------------
   configure/ access Gausian Selection Cluster Set
   ---------------------------------------------------------------------- */
static int gsClusterSetConfigureItf (ClientData cd, char *var, char *val)
{
  GSClusterSet  *gscsP = (GSClusterSet*) cd;
  CodebookSet    *cbsP = NULL;
	
  if (!gscsP) gscsP = &GSClusterSetDefaults;
  cbsP = gscsP->cbsP;
	
  if (!var) {
    ITFCFG (gsClusterSetConfigureItf, cd, "name");
    ITFCFG (gsClusterSetConfigureItf, cd, "bestN");
    ITFCFG (gsClusterSetConfigureItf, cd, "decrease");
    ITFCFG (gsClusterSetConfigureItf, cd, "backoffdefault");
    ITFCFG (gsClusterSetConfigureItf, cd, "backoffvalue");
    ITFCFG (gsClusterSetConfigureItf, cd, "backoffvalueI");
    ITFCFG (gsClusterSetConfigureItf, cd, "backoffmode");
    ITFCFG (gsClusterSetConfigureItf, cd, "backofffactor");
    ITFCFG (gsClusterSetConfigureItf, cd, "clusterN");
    ITFCFG (gsClusterSetConfigureItf, cd, "mode");
    ITFCFG (gsClusterSetConfigureItf, cd, "gaussianN");
    ITFCFG (gsClusterSetConfigureItf, cd, "gaussians_computed");
    ITFCFG (gsClusterSetConfigureItf, cd, "gaussians_total");
    ITFCFG (gsClusterSetConfigureItf, cd, "eval_frames");
    ITFCFG (gsClusterSetConfigureItf, cd, "diff");
    ITFCFG (gsClusterSetConfigureItf, cd, "use");
    return TCL_OK;
  }
	
  ITFCFG_CharPtr (var, val, "name",              gscsP->name,              1);
  ITFCFG_Int     (var, val, "bestN",             gscsP->bestN,             0);
  ITFCFG_Int     (var, val, "gaussianN",         gscsP->gaussianN,         1);
  ITFCFG_Int     (var, val, "clusterN",          gscsP->clusterN,          1);
  ITFCFG_Float   (var, val, "backoffvalue",      gscsP->backoffValue,      1);
  ITFCFG_Float   (var, val, "diff",              gscsP->diff,              1);
  ITFCFG_Int     (var, val, "backoffvalueI",     gscsP->backoffValueI,     1);
  ITFCFG_Float   (var, val, "backofffactor",     gscsP->backoffFactor,     0);
  ITFCFG_Int     (var, val, "backoffmode",       gscsP->backoffMode,       0);
  ITFCFG_Int     (var, val, "gaussians_computed",gscsP->gaussians_computed,0);
  ITFCFG_Int     (var, val, "gaussians_total",   gscsP->gaussians_total,   0);
  ITFCFG_Int     (var, val, "eval_frames",       gscsP->eval_frames,       0);
  ITFCFG_Int     (var, val, "use",               gscsP->use,               0);

  if (streq (var, "decrease")) {
    if (!val) {
      itfAppendResult (" %f", gscsP->decrease);
    } else {
      float fval = atof(val);
      gscsP->decrease = fval; 
      if (cbsP) gsCompressClusters(gscsP);
    }
    return TCL_OK;
  }

  if (streq (var, "backoffdefault")) {
    if (!val) {
      itfAppendResult (" %f", gscsP->backoffDefault);
    } else {
      float fval = atof(val);
      gscsP->backoffDefault = fval; 
      gscsP->backoffValue = fval;
      if (cbsP)
	gscsP->backoffValueI = (int) (fval * cbsP->scaleRV * cbsP->scaleRV * cbsP->scaleCV);
    }
    return TCL_OK;
  }
  if (streq (var, "mode")) {
    if (!val) {
      itfAppendResult (" %d", gscsP->mode);
    } else {
      int ival = atoi(val);
      if (ival>=0 && ival<gs_mode_max) {
	gscsP->mode = ival;	
	if (cbsP) {
	  if (cbsP->comMode==0) { /* no compression, assuming floating point codebooks */
	    if (gscsP->cluster) {
	      gsUpdateMeans(gscsP, gscsP->clusterN);
	      gsUpdateCovars(gscsP, gscsP->clusterN);
	      gsCompressClusters(gscsP);
	    }
	  } else { ERROR("Setting mode is not allowed without floating point CodebookSet!\n"); return TCL_ERROR; }
	} else WARN("No CodebookSet present!\n");
      } else { ERROR("Illegal value %d for mode (0...%d)\n",ival,gs_mode_max-1); return TCL_ERROR; }	
    }
    return TCL_OK;
  }
	
  ERROR ("Unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}	


/* ----------------------------------------------------------------------
   save a Gaussion Selection Cluster Set
   ---------------------------------------------------------------------- */
int gsClusterSetSave (GSClusterSet* gscsP, char *filename) {
  CodebookSet   *cbsP = gscsP->cbsP;
  int             cbN = cbsP->list.itemN;
  Codebook       *cbP = cbsP->list.itemA + 0;
  int            dimN = (cbP->cfg)->rdimN > 0 ? (cbP->cfg)->rdimN : cbP->dimN;
  int            refN = cbP->refN;
  int        clusterN = gscsP->clusterN;
  char       magic[4] = "GSCS";
  FILE *fp;
  int cbX,refX,clusterX;
	
  if (gscsP->cluster==NULL) { ERROR("No clusterSet created yet!\n"); return TCL_ERROR; }
	
  fp = fopen(filename,"wb");
	
  if (fp) {
    INFO("saving ClusterSet with %d clusters, %d codebooks and %d gaussians %d dimensions\n",clusterN,cbN,gscsP->gaussianN,dimN);
    fwrite(magic,4,1,fp);
    write_int(fp,clusterN);
    write_int(fp,gscsP->bestN);
    write_int(fp,gscsP->mode);
    write_int(fp,cbN);
    write_int(fp,refN);
    write_int(fp,dimN);
    write_int(fp,gscsP->gaussianN);
    write_int(fp,gscsP->backoffMode);
    write_float(fp,gscsP->backoffDefault);
    write_float(fp,gscsP->backoffFactor);
    write_float(fp,gscsP->decrease);
    for (cbX=0; cbX<cbN; cbX++) {
      cbP = cbsP->list.itemA + cbX;
      refN = cbP->refN;
      for (refX=0; refX<refN; refX++) {
	if (gscsP->clusterX[cbX][refX]>=0) {
	  write_short(fp,gscsP->clusterX[cbX][refX]);
	} else {
	  write_short(fp,-1);
	}
      }
    }
    for (clusterX=0; clusterX<clusterN; clusterX++) {
      write_floats(fp,gscsP->cluster[clusterX].mean,dimN);
      write_floats(fp,gscsP->cluster[clusterX].covar,dimN);
    }
    write_float(fp,gscsP->diff);
    fclose(fp);
  } else return TCL_ERROR;
  return TCL_OK;
}


static int gsClusterSetSaveItf (ClientData cd, int argc, char *argv[])
{
  int             ac  =  argc - 1;
  GSClusterSet *gscsP =  (GSClusterSet*) cd;
  char     *filename  = NULL;
	
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
		     "<filename>",    ARGV_STRING,    NULL,       &filename, NULL, "filename of clusterset",
		     NULL) != TCL_OK) { 
    return TCL_ERROR;
  }
  if (filename) {
    return gsClusterSetSave (gscsP, filename);
  }
  return TCL_ERROR;
}



/* ----------------------------------------------------------------------
   load a Gaussion Selection Cluster Set
   ---------------------------------------------------------------------- */
int gsClusterSetLoad (GSClusterSet* gscsP, char *filename) {
  CodebookSet   *cbsP = gscsP->cbsP;
  int             cbN = cbsP->list.itemN;
  Codebook       *cbP = cbsP->list.itemA + 0;
  int            dimN = (cbP->cfg)->rdimN > 0 ? (cbP->cfg)->rdimN : cbP->dimN;
  int            refN = cbP->refN;
  char       magic[4] = "....";
  float decrease,backoffDefault,backoffFactor;
  int cbX,refX,clusterX,bestN,mode,backoffMode;
  int cbN_t,refN_t,dimN_t,clusterN,gaussianN_t;
  FILE *fp;
	
  fp = fopen(filename,"rb");
  if (fp) {
    fread(magic,4,1,fp);
    clusterN = read_int(fp);
    bestN = read_int(fp);
    mode = read_int(fp);
    cbN_t = read_int(fp);
    refN_t = read_int(fp);
    dimN_t = read_int(fp);
    gaussianN_t = read_int(fp);
    backoffMode = read_int(fp);
    backoffDefault = read_float(fp);
    backoffFactor = read_float(fp);
    decrease = read_float(fp);
		
    if 	(gsClusterSetAlloc(gscsP,clusterN,bestN,mode)) {
      if (cbN_t!=cbN) { fclose(fp); ERROR("Number of codebooks does not match! (%d(ClusterSet) != %d(codebookSet))",cbN_t,cbN); return TCL_ERROR; }
      /* those assumptions are wrong on a MAS trained codebookset: 
	 if (gaussianN_t!=gscsP->gaussianN) { fclose(fp); ERROR("Number of gaussians does not match! (%d(ClusterSet) != %d(codebookSet))",gaussianN_t,gscsP->gaussianN); return TCL_ERROR; }
	 if (refN_t!=refN) { fclose(fp); ERROR("Number of gaussians per codebook does not match!(%d(ClusterSet) != %d(codebook))",refN_t,refN); return TCL_ERROR; }
      */
      if (dimN_t!=dimN) { fclose(fp); ERROR("Number of dimensions does not match!(%d(ClusterSet) != %d(codebookSet))",dimN_t,dimN); return TCL_ERROR; }
      INFO("loading ClusterSet with %d clusters, %d codebooks and %d gaussians\n",clusterN,cbN_t,gaussianN_t);
			
      gscsP->backoffValue = backoffDefault;
      gscsP->backoffDefault = backoffDefault;
      gscsP->backoffFactor = backoffFactor;
			
      gscsP->backoffMode = backoffMode;
      gscsP->decrease = decrease;
			
      /* flush the cluster count */
      for (clusterX=0; clusterX<gscsP->clusterN; clusterX++) {  /* for each cluster ... */
	gscsP->cluster[clusterX].count = 0;
      }
      gscsP->gaussianN=0;
      for (cbX=0; cbX<cbN; cbX++) {
	cbP = cbsP->list.itemA + cbX;
	refN = cbP->refN;
	for (refX=0; refX<refN; refX++) {
	  gscsP->clusterX[cbX][refX] =  read_short(fp);					
	  if (gscsP->clusterX[cbX][refX]>=0) {
	    if (gscsP->clusterX[cbX][refX]>=0 && gscsP->clusterX[cbX][refX]<gscsP->clusterN) {
	      gscsP->cluster[gscsP->clusterX[cbX][refX]].count++;
	      gscsP->gaussianN++;
	    } else {
	      ERROR("Invalid cluster assign in ClusterSet file : 0<=%d<%d!\n",gscsP->clusterX[cbX][refX],gscsP->clusterN);
	      gscsP->clusterX[cbX][refX] = -1;
	    }
	  }
	}
      }
      for (clusterX=0; clusterX<clusterN; clusterX++) {
	read_floats(fp,gscsP->cluster[clusterX].mean,dimN);
	read_floats(fp,gscsP->cluster[clusterX].covar,dimN);
      }
      gscsP->diff = read_float(fp);
      gsCompressClusters(gscsP);
      gscsP->use = -1;
    } else { ERROR("Unable to allocate GSClusterSet!"); fclose(fp); return TCL_ERROR;}
    fclose(fp);
  } else { ERROR("Can't open ClusterSet file!"); return TCL_ERROR; }
  return TCL_OK;
}

static int gsClusterSetLoadItf (ClientData cd, int argc, char *argv[])
{
  int             ac  =  argc - 1;
  GSClusterSet *gscsP =  (GSClusterSet*) cd;
  char     *filename  = NULL;
	
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
		     "<filename>",    ARGV_STRING,    NULL,       &filename, NULL, "filename of clusterset",
		     NULL) != TCL_OK) { 
    return TCL_ERROR;
  }
  if (filename) {
    return gsClusterSetLoad (gscsP, filename);
  }
  return TCL_ERROR;
}


/* ----------------------------------------------------------------------
   puts useful information about ClusterSet,
   such as dimensions, cluster means/covars and gaussian assign table
   ---------------------------------------------------------------------- */
int gsClusterSetPutsItf (ClientData cd, int argc, char *argv[])
{
  int              ac = argc-1;
  GSClusterSet  *gscs = (GSClusterSet*) cd;
  CodebookSet   *cbsP = gscs->cbsP;
  int             cbN = cbsP->list.itemN;
  GSClusterSet *gscsP = cbsP->gscsP;
  int        clusterN = gscsP->clusterN;
  Codebook       *cbP = cbsP->list.itemA + 0;
  int           dimN  = (cbP->cfg)->rdimN > 0 ? (cbP->cfg)->rdimN : cbP->dimN;
  int         verbose = 1;
  int clusterX, dimX,cbX,refN,refX;
	
  if (itfParseArgv (argv[0], &ac, argv+1, 0,
		    "-verbose",  ARGV_INT, NULL, &verbose, NULL,         "OR this flags: 1=clusterSet dimensions, 2=dump clusters, 4=dump assign table",
		    NULL) != TCL_OK) return TCL_ERROR;

  if (gscsP->cluster==NULL) { ERROR("No clusterSet created yet!\n"); return TCL_ERROR; }

  if (verbose&1) {
    itfAppendResult("ClusterSet: %d clusters, %d codebooks and %d gaussians (%d dimensions) in total, avg sqr error %2.5f\n",gscsP->clusterN,cbN,gscsP->gaussianN,gscsP->dimN,gscsP->diff);
    itfAppendResult("Configuration: NBest=%d, backoffmode=%d, backoffvalue=%4.2f, backofffactor=%4.2f\n",gscsP->bestN,gscsP->backoffMode,gscsP->backoffDefault,gscsP->backoffFactor);
    switch(gscsP->mode) {
    case gs_mode_euclidean:
      itfAppendResult("Using Euclidean Distance messure (mode=%d) with decrease of %1.2f\n",gscsP->mode,gscsP->decrease);
      break;
    case gs_mode_mahalanobis:
      itfAppendResult("Using Mahalanobis Distance messure (mode=%d: inner-cluster-variance) with decrease of %1.2f\n",gscsP->mode,gscsP->decrease);
      break;
    case gs_mode_globalaverage:
      itfAppendResult("Using Mahalanobis Distance messure (mode=%d: global codebook average) with decrease of %1.2f\n",gscsP->mode,gscsP->decrease);
      break;
    case gs_mode_clusteraverage:
      itfAppendResult("Using Mahalanobis Distance messure (mode=%d: cluster codebook average) with decrease of %1.2f\n",gscsP->mode,gscsP->decrease);
      break;
    case gs_mode_globalaverage_i:
      itfAppendResult("Using Mahalanobis Distance messure (mode=%d: inverse global codebook average) with decrease of %1.2f\n",gscsP->mode,gscsP->decrease);
      break;
    case gs_mode_clusteraverage_i:
      itfAppendResult("Using Mahalanobis Distance messure (mode=%d: inverse cluster codebook average) with decrease of %1.2f\n",gscsP->mode,gscsP->decrease);
      break;
    default:
      itfAppendResult("ERROR: No valid mode for Clusterset!\n");
    }
  }
	
  if (verbose&2) {
    itfAppendResult("Clusters:\n"); 
    for (clusterX=0; clusterX<clusterN; clusterX++) {  /* for each cluster ... */
      itfAppendResult("Cluster %3d: %4d members, %d x activated\n",clusterX,gscsP->cluster[clusterX].count,gscsP->cluster[clusterX].active_count);
      itfAppendResult("mean :  ");
      for (dimX=0; dimX<dimN; dimX++) {
	itfAppendResult("%+2.4f ",gscsP->cluster[clusterX].mean[dimX]);
      }
      itfAppendResult("\nmeanC : ");
      for (dimX=0; dimX<dimN; dimX++) {
	itfAppendResult("%+4d ",gscsP->cluster[clusterX].meanC[dimX]);
      }
      itfAppendResult("\ncovar:  ");
      for (dimX=0; dimX<dimN; dimX++) {
	itfAppendResult("%+2.4f ",gscsP->cluster[clusterX].covar[dimX]);
      }
      itfAppendResult("\ncovarC: ");
      for (dimX=0; dimX<dimN; dimX++) {
	itfAppendResult("%+4d ",gscsP->cluster[clusterX].covarC[dimX]);
      }
      itfAppendResult("\n");
    }
  }
	
  if (verbose&4) {
    int divcount=0;
    itfAppendResult("Gaussian Assign Table:\n"); 
    for (cbX=0; cbX<cbN; cbX++) {
      cbP = cbsP->list.itemA + cbX;
      refN = cbP->refN;
      itfAppendResult("%12s (%2d): ",cbP->name,refN);
      for (refX=0; refX<refN; refX++) {
	int succ=-1;
	int refY;
	if (gscsP->clusterX[cbX][refX]>=0) {
	  itfAppendResult("%3d ",gscsP->clusterX[cbX][refX]);
	  for(refY=0; refY<refX; refY++) {
	    if (gscsP->clusterX[cbX][refX]==gscsP->clusterX[cbX][refY]) succ=0;
	  }
	  if (succ) divcount++;
	}
	else
	  itfAppendResult("n/a ");
      }
      itfAppendResult("\n");
    }
  }
  return TCL_OK;
}


/* ----------------------------------------------------------------------
   compute weighted Mahalanobis distance
   ---------------------------------------------------------------------- */
float gsMahalanobisDist(float *feat, float *mean, float *covar, int dimN, float decrease) {
  float dist   = 0;
  float weight = 1.0;
  int dimX;

  decrease /= dimN;
  if (covar) {
    for (dimX=0; dimX<dimN; dimX++) {
      float diff = feat[dimX]-mean[dimX];
      dist += diff * diff * covar[dimX]*weight;
      weight-=decrease;
    }
  } else { /* no covars => euclidean distance */
    for (dimX=0; dimX<dimN; dimX++) {
      float diff = feat[dimX]-mean[dimX];
      dist += diff * diff * weight;
      weight-=decrease;
    }
  }
  return dist;
}

/* Assign each Gaussian to one existing cluster in the ClusterSet */
void gsAssignClusters(GSClusterSet *gscsP, int clusterA) {
  CodebookSet    *cbsP = gscsP->cbsP;
  int              cbN = cbsP->list.itemN;
  Codebook        *cbP = cbsP->list.itemA + 0;
  int             dimN = (cbP->cfg)->rdimN > 0 ? (cbP->cfg)->rdimN : cbP->dimN;
  int cbX,refX,clusterX,refN;

  /* flush the cluster count */
  for (clusterX=0; clusterX<clusterA; clusterX++) {  /* for each cluster ... */
    gscsP->cluster[clusterX].count = 0;
  }

  /* compute which gaussian belongs to which cluster */
  for (cbX=0; cbX<cbN; cbX++) { /* for each codebook ... */
    cbP = cbsP->list.itemA + cbX;
    refN = cbP->refN;
    for (refX=0; refX<refN; refX++) { /* for each gaussian ... */
      if (gscsP->clusterX[cbX][refX]>=0) {
	float minDist = 10000000;
	int minClusterX = -1;
	for (clusterX=0; clusterX<clusterA; clusterX++) {  /* for each cluster ... */
	  float dist=1;
	  switch(gscsP->mode) {
						
	  case gs_mode_euclidean:
	    dist = gsMahalanobisDist(gscsP->cluster[clusterX].mean,cbP->rv->matPA[refX],NULL,dimN,gscsP->decrease);
	    break;

	  case gs_mode_representant:
	  case gs_mode_mahalanobis:
	    dist = gsMahalanobisDist(gscsP->cluster[clusterX].mean,cbP->rv->matPA[refX],cbP->cv[refX]->m.d,dimN,gscsP->decrease);
	    break;

	  case gs_mode_globalaverage:
	  case gs_mode_clusteraverage:
	  case gs_mode_globalaverage_i:
	  case gs_mode_clusteraverage_i:
	    dist = gsMahalanobisDist(gscsP->cluster[clusterX].mean,cbP->rv->matPA[refX],gscsP->cluster[clusterX].covar,dimN,gscsP->decrease);
	    break;

	  }

	  if (dist<minDist) {
	    minDist=dist;
	    minClusterX=clusterX;
	    if (minDist<0.00001 || minDist>9999999 || dist==-1) {
	    }
	  }
	}
	if (minClusterX==-1) {
	  ERROR("No cluster found for codebook %d / %d!\n",cbX,refX);
	  minClusterX=0;
	}
	gscsP->clusterX[cbX][refX] =  minClusterX;
	gscsP->cluster[minClusterX].count++;
      }
    }
  }
}


/* ----------------------------------------------------------------------
   update the mean vector of each cluster according its gaussian members
   ---------------------------------------------------------------------- */
void gsUpdateMeans(GSClusterSet *gscsP, int clusterA) {
  CodebookSet      *cbsP = gscsP->cbsP;
  int                cbN = cbsP->list.itemN;
  Codebook          *cbP = cbsP->list.itemA + 0;
  int               dimN = (cbP->cfg)->rdimN > 0 ? (cbP->cfg)->rdimN : cbP->dimN;
  float           ccount = 1.0;
  float          maxdist = 10000000;
  int             maxcbX = 0;
  int            maxrefX = 0;
  int cbX,refX,dimX,clusterX,refN;
	
  /* flush the cluster means */
  for (clusterX=0; clusterX<clusterA; clusterX++) {  /* for each cluster ... */
    for (dimX=0; dimX<dimN; dimX++) {
      gscsP->cluster[clusterX].mean[dimX] = 0;
    }
  }

  /* compute the new cluster means */
  for (cbX=0; cbX<cbN; cbX++) { /* for each codebook ... */
    cbP = cbsP->list.itemA + cbX;
    refN = cbP->refN;

    for (refX=0; refX<refN; refX++) { /* for each gaussian ... */
      clusterX = gscsP->clusterX[cbX][refX];
      if (clusterX>=0) {
	float *meanCb = cbP->rv->matPA[refX];
	float *meanCluster = gscsP->cluster[clusterX].mean;
	if (gscsP->cluster[clusterX].count==0) { ERROR ("INTERNAL: Cluster is touched but count=0 !"); gscsP->cluster[clusterX].count=1;}
	ccount = 1 / (float)gscsP->cluster[clusterX].count;
	for (dimX=0; dimX<dimN; dimX++) {
	  meanCluster[dimX] += meanCb[dimX] * ccount;
	}
      }
    }
  }
	
  /* choose the nearest gaussian to the centroid as a representant instead if the centroid itself */ 
  if (gscsP->mode == gs_mode_representant) {
    for (clusterX=0; clusterX<clusterA; clusterX++) {  /* for each cluster ... */
      maxdist = 99999999;
      maxcbX = 0;
      maxrefX = 0;
      for (cbX=0; cbX<cbN; cbX++) { /* for each codebook ... */
	cbP = cbsP->list.itemA + cbX;
	refN = cbP->refN;
				
	for (refX=0; refX<refN; refX++) { /* for each gaussian ... */
	  if (clusterX == gscsP->clusterX[cbX][refX]) {
	    float dist = gsMahalanobisDist(gscsP->cluster[clusterX].mean,cbP->rv->matPA[refX],cbP->cv[refX]->m.d,dimN,gscsP->decrease);
	    if (dist<maxdist) {
	      maxdist = dist;
	      maxcbX = cbX;
	      maxrefX = refX;
	    }
	  }
	}
      }
      cbP = cbsP->list.itemA + maxcbX;
      for (dimX=0; dimX<dimN; dimX++) {
	gscsP->cluster[clusterX].mean[dimX] = cbP->rv->matPA[maxrefX][dimX];
	/* gscsP->cluster[clusterX].covar[dimX] = cbP->cv[maxrefX]->m.d[dimX]; */
      }
    }
  }
}


/* ----------------------------------------------------------------------
   update the covariances for each cluster according its gaussian members
   and the distance mode
   ---------------------------------------------------------------------- */
void gsUpdateCovars(GSClusterSet *gscsP, int clusterA) {
  CodebookSet      *cbsP = gscsP->cbsP;
  int                cbN = cbsP->list.itemN;
  Codebook          *cbP = cbsP->list.itemA + 0;
  float           ccount = 1.0;
  int               dimN = (cbP->cfg)->rdimN > 0 ? (cbP->cfg)->rdimN : cbP->dimN;
  int cbX,refX,dimX,clusterX,refN;
	
  /* flush the covariance values */
  for (clusterX=0; clusterX<clusterA; clusterX++) {  /* for each cluster ... */
    for (dimX=0; dimX<dimN; dimX++) {
      gscsP->cluster[clusterX].covar[dimX]= 0.0001;  
    }
  }

  /* compute the new covariances */
  for (cbX=0; cbX<cbN; cbX++) { /* for each codebook ... */
    Codebook *cbP = cbsP->list.itemA + cbX;
    refN = cbP->refN;
	
    for (refX=0; refX<refN; refX++) { /* for each gaussian ... */
      clusterX = gscsP->clusterX[cbX][refX];
      if (clusterX>=0) {
	float *meanCb = cbP->rv->matPA[refX];
	float *meanCluster = gscsP->cluster[clusterX].mean;
	float *covarCb = cbP->cv[refX]->m.d;
	float *covarCluster = gscsP->cluster[clusterX].covar;
	if (gscsP->cluster[clusterX].count==0) { ERROR ("INTERNAL: Cluster is touched but count=0 !"); gscsP->cluster[clusterX].count=1;}
	ccount = 1 / (float)gscsP->cluster[clusterX].count;

	switch(gscsP->mode) {
	case gs_mode_euclidean:
	  for (dimX=0; dimX<dimN; dimX++) covarCluster[dimX] =  1.0;
	  break;

	case gs_mode_representant:
	case gs_mode_mahalanobis:
	  for (dimX=0; dimX<dimN; dimX++) {
	    float diff0 = meanCb[dimX] - meanCluster[dimX];
	    covarCluster[dimX] +=  (diff0*diff0) * ccount;
	  }
	  break;

	case gs_mode_globalaverage:
	  for (dimX=0; dimX<dimN; dimX++) gscsP->cluster[0].covar[dimX] += (covarCb[dimX]) / (float)gscsP->gaussianN;
	  break;

	case gs_mode_clusteraverage:
	  for (dimX=0; dimX<dimN; dimX++) covarCluster[dimX] += (covarCb[dimX]) * ccount;
	  break;

	case gs_mode_globalaverage_i:
	  for (dimX=0; dimX<dimN; dimX++) gscsP->cluster[0].covar[dimX] += (1/covarCb[dimX]) / (float)gscsP->gaussianN;
	  break;

	case gs_mode_clusteraverage_i:
	  for (dimX=0; dimX<dimN; dimX++) covarCluster[dimX] += (1/covarCb[dimX]) * ccount;
	  break;

	}
      }
    }
  }

  /* correct and check the covariances */
  for (clusterX=0; clusterX<clusterA; clusterX++) {  /* for each cluster ... */
    for (dimX=0; dimX<dimN; dimX++) {
      /* correct illegal covars to avoid 0 division */
      if (gscsP->cluster[clusterX].covar[dimX] == 0) gscsP->cluster[clusterX].covar[dimX]=1.0; 

      /* copy covar[0] to all the rest, needed for global covars */
      switch(gscsP->mode) {
      case gs_mode_globalaverage_i:
      case gs_mode_globalaverage:
	gscsP->cluster[clusterX].covar[dimX] =  gscsP->cluster[0].covar[dimX]; 
	break;
      }

      /* inverse covars that where computed non-inverse */
      switch(gscsP->mode) {
      case gs_mode_globalaverage_i:
      case gs_mode_clusteraverage_i:
      case gs_mode_mahalanobis:
      case gs_mode_representant:
	gscsP->cluster[clusterX].covar[dimX] =  1/gscsP->cluster[clusterX].covar[dimX]; 
	break;
      }
    }
  }
}


/* ----------------------------------------------------------------------
   calculate the inner variance of the ClusterSet, can be used as a
   meassure of how good the clusters approximate the feature space
   ---------------------------------------------------------------------- */
float gsGetClusterError(GSClusterSet *gscsP) {
  CodebookSet    *cbsP = gscsP->cbsP;
  int              cbN = cbsP->list.itemN;
  Codebook        *cbP = cbsP->list.itemA + 0;
  int             dimN = (cbP->cfg)->rdimN > 0 ? (cbP->cfg)->rdimN : cbP->dimN;
  float           diff = 0;
  int cbX,refX,dimX,clusterX,refN;
	
  for (cbX=0; cbX<cbN; cbX++) { /* for each codebook ... */
    Codebook *cbP = cbsP->list.itemA + cbX;
    refN = cbP->refN;
	
    for (refX=0; refX<refN; refX++) { /* for each gaussian ... */
      clusterX = gscsP->clusterX[cbX][refX];
      if (clusterX>=0) {
	for (dimX=0; dimX<dimN; dimX++) {
	  float diff0 = cbP->rv->matPA[refX][dimX] - gscsP->cluster[clusterX].mean[dimX];
	  diff += (diff0*diff0);
	}
      }
    }
  }
  return (diff/ (float)gscsP->gaussianN / (float)dimN);
}


/* ----------------------------------------------------------------------
   calculate the inner variance of the ClusterSet, can be used as a
   meassure of how good the clusters approximate the feature space
   ---------------------------------------------------------------------- */
void gsCompressClusters(GSClusterSet *gscsP) {
  CodebookSet     *cbsP = gscsP->cbsP;
  int          clusterN = gscsP->clusterN;
  Codebook         *cbP = cbsP->list.itemA + 0;
  int              dimN = (cbP->cfg)->rdimN > 0 ? (cbP->cfg)->rdimN : cbP->dimN;
  float         scaleRV = cbsP->scaleRV;
  float         scaleCV = cbsP->scaleCV;
  float          offset = cbsP->offset;
  float          weight = 1.0;
  float        decrease = gscsP->decrease / dimN;
  int dimX,clusterX,dimY;
		
  if (cbsP->x2y) {
    for (clusterX=0; clusterX<clusterN; clusterX++) { /* for each cluster ... */
      for (dimX=0; dimX<dimN; dimX++) { /* for each dimension ... */
	gscsP->cluster[clusterX].meanC[dimX] = (int)(gscsP->cluster[clusterX].mean[cbsP->x2y[dimX]] * scaleRV + offset);
	gscsP->cluster[clusterX].covarC[dimX] = (int)(gscsP->cluster[clusterX].covar[cbsP->x2y[dimX]] * scaleCV);
      }
    }
    for (dimX=0; dimX<dimN; dimX++) { /* for each dimension ... */
      for (dimY=0; dimY<dimN; dimY++) { /* for each dimension ... */
	if (cbsP->x2y[dimY] == dimX) break;
      }
      if (dimY<dimN) {
	gscsP->weightC[dimY] = (int)(weight*256.0);
      }
      weight -=decrease;
    }
  }
}

/* ----------------------------------------------------------------------
   split the biggest cluster, return 0 if there is no more cluster to split
   ---------------------------------------------------------------------- */
int gsSplitCluster(GSClusterSet *gscsP, int *clusterP) {
  CodebookSet     *cbsP = gscsP->cbsP;
  Codebook         *cbP = cbsP->list.itemA + 0;
  int              dimN = (cbP->cfg)->rdimN > 0 ? (cbP->cfg)->rdimN : cbP->dimN;
  int   clusterMaxCount = 0;
  int        clusterMax = 0;
  int          clusterA = *clusterP;
  int dimX,clusterX;

  if (clusterA>=gscsP->clusterN) return (0);

  for (clusterX=0; clusterX<clusterA; clusterX++) {  /* find the biggest cluster ... */
    if (gscsP->cluster[clusterX].count>clusterMaxCount) {
      clusterMaxCount = gscsP->cluster[clusterX].count;
      clusterMax = clusterX;
    }
  }


  for (dimX=0; dimX<dimN; dimX++) {
    float q=0.001 * ((float)rand()*2.0/(float)RAND_MAX-1.0);
    float diffX=1.0;

    /* This would split the cluster along the covariances */
    /* 		float q=0.01;
       float diffX =0;

       for (cbX=0; cbX<cbN; cbX++) { 
       cbP = cbsP->list.itemA + cbX;
       refN = cbP->refN;
       for (refX=0; refX<refN; refX++) { 
       if (gscsP->clusterX[cbX][refX]>=0) {
       if (gscsP->clusterX[cbX][refX]=clusterMax) {
       int diff0 = cbP->rv->matPA[refX][dimX] - gscsP->cluster[clusterMax].mean[dimX];
       diffX += sqrt(diff0*diff0) / (float)gscsP->cluster[clusterMax].count;
       }
       }
       }
       }
    */
    gscsP->cluster[clusterA  ].mean[dimX] = gscsP->cluster[clusterMax].mean[dimX];
    gscsP->cluster[clusterA  ].covar[dimX] = gscsP->cluster[clusterMax].covar[dimX];

    gscsP->cluster[clusterA  ].mean[dimX] += q * diffX;
    gscsP->cluster[clusterMax].mean[dimX] -= q * diffX;

  }
  INFO("splitting cluster %3d (%d gaussians) into %3d\n",clusterMax,  gscsP->cluster[clusterMax].count, clusterA);

  clusterA++;
  *clusterP = clusterA;
  return (-1);
}


/* ----------------------------------------------------------------------
   remove the smallest cluster, if it has less then mincount members,
   or return 0
   ---------------------------------------------------------------------- */
int gsRemCluster(GSClusterSet *gscsP, int *clusterP, int mincount) {
  CodebookSet      *cbsP = gscsP->cbsP;
  Codebook          *cbP = cbsP->list.itemA + 0;
  int              dimN  = (cbP->cfg)->rdimN > 0 ? (cbP->cfg)->rdimN : cbP->dimN;
  int    clusterMinCount = gscsP->gaussianN;
  int         clusterMin = 0;
  int           clusterA = *clusterP;
  int dimX,clusterX;
	
  if (clusterA<2) return (0);

  for (clusterX=0; clusterX<clusterA; clusterX++) {  /* find the smallest cluster ... */
    if (gscsP->cluster[clusterX].count<clusterMinCount) {
      clusterMinCount = gscsP->cluster[clusterX].count;
      clusterMin = clusterX;
    }
  }
  if (mincount<clusterMinCount) return (0); 

  for (dimX=0; dimX<dimN; dimX++) {
    gscsP->cluster[clusterMin].mean[dimX]  = gscsP->cluster[clusterA-1].mean[dimX];
    gscsP->cluster[clusterMin].covar[dimX] = gscsP->cluster[clusterA-1].covar[dimX];
  }
  INFO("removing cluster %3d (%d gaussians)\n",clusterMin,  gscsP->cluster[clusterMin].count);

  clusterA--;
  *clusterP = clusterA;
  return (-1);
}


/* ----------------------------------------------------------------------
   generate a ClusterSet from a given CodebookSet
   ---------------------------------------------------------------------- */
int gsClusterSetGen (GSClusterSet *gscsP, int clusterN, int mode, int seed, float spread, int bestN, int clusterA, float decrease, int backoffMode, float backoffDefault, float backoffFactor, int iterations) {
  CodebookSet   *cbsP = gscsP->cbsP;
  Codebook       *cbP = cbsP->list.itemA + 0;
  int           dimN  = (cbP->cfg)->rdimN > 0 ? (cbP->cfg)->rdimN : cbP->dimN;
  int             itX = 0;
  int        iterated = 0;
  float     lastdiff0 = 0;
  float     lastdiff1 = -1;
  int        complete = 0;
  float          diff;
  int clusterX,dimX;
		
  if (clusterA>clusterN) {
    WARN("Asked to start with more clusters (%d) than the clusterSet has(%d).",clusterA,clusterN);
    clusterA=clusterN;
  }
  if (clusterA<=0) clusterA=clusterN;

  if (cbP->rv==NULL) {
    ERROR("Clustering works only on floating point codebooks!\n");
    return TCL_ERROR;
  }
  if (gscsP->clusterN!=clusterN) {
    INFO("No matching (%d) existing ClusterSet found, generating %d new clusters\n",gscsP->clusterN,clusterN);
    if (gsClusterSetAlloc(gscsP,clusterN,bestN,mode)==0) {
      ERROR("Can not allocate GS ClusterSet!");
      return TCL_ERROR;
    }
    INFO("Clusters allocated, lets go!\n");
  } else {
    INFO("Continue with %d clusters, lets go!\n",clusterN);
  }
  gscsP->bestN          = bestN;
  gscsP->mode           = mode;
  gscsP->decrease       = decrease;
  gscsP->backoffDefault = backoffDefault;
  gscsP->backoffValue   = backoffDefault;
  gscsP->backoffFactor  = backoffFactor;
  gscsP->backoffMode    = backoffMode;
  srand(seed);
  INFO("Init the initial clusters\n");

  /* Set initial cluster means randomly */
  for (clusterX=0; clusterX<clusterN; clusterX++) {  /* for each cluster ... */
    for (dimX=0; dimX<dimN; dimX++) {
      gscsP->cluster[clusterX].mean[dimX] += spread * ((float)rand()*2.0/(float)RAND_MAX-1.0);
    }
  }

  while (complete==0) {
    while (iterated==0 && iterations!=0) { /* iterate till no cluster change anymore */
      /* assing codebooks to clusters */
      gsAssignClusters(gscsP, clusterA);

      /* compute new means */
      gsUpdateMeans(gscsP, clusterA);

      /* compute new covariances */
      gsUpdateCovars(gscsP, clusterA);

      /* statistic info output */
      diff = gsGetClusterError(gscsP);

      INFO("iteration %3d, diff = %5.10f, (%d of %d clusters)\n", itX, diff,clusterA,clusterN);

      /* remove too small clusters */
      if (gsRemCluster(gscsP, &clusterA, (gscsP->gaussianN/clusterA)*0.05)+1) {
	/* gsSplitCluster(gscsP, &clusterA); */
      }

      if (clusterA==clusterN) { /* last iteration step is more accurate */
				/* if (fabs(lastdiff0 - diff)<0.00001) iterated=-1;
				   if (fabs(lastdiff1 - diff)<0.00001) iterated=-1; */
	iterations--;
      } else {
	/* if (fabs(lastdiff0 - diff)<0.0001) iterated=-1;
	   if (fabs(lastdiff1 - diff)<0.0001) iterated=-1; */
	iterated=-1;
      }

      lastdiff1 = lastdiff0;
      lastdiff0 = diff;
      itX++;
    } 

    /* split clusters */
    if (gsSplitCluster(gscsP, &clusterA)) {
      iterated=0;
    } else complete=-1;
  } 
  gscsP->diff = gsGetClusterError(gscsP);
  gsCompressClusters(gscsP);
  gscsP->use = GSClusterSetDefaults.use;
  INFO("generation of GSClusterSet complete.\n");
  return TCL_OK;
}

int gsClusterSetGenItf (ClientData cd, int argc, char *argv[])
{
  int               ac = argc - 1;
  GSClusterSet  *gscsP = (GSClusterSet*) cd;
  CodebookSet    *cbsP = gscsP->cbsP;
  int         clusterN = 128;
  int            bestN = GSClusterSetDefaults.bestN;
  int             mode = GSClusterSetDefaults.mode;
  int             seed = 12345;
  int         clusterA = clusterN;
  float         spread = 2.0;
  float       decrease = GSClusterSetDefaults.decrease;
  float  backoffFactor = GSClusterSetDefaults.backoffFactor;
  float backoffDefault = GSClusterSetDefaults.backoffDefault;
  int      backoffMode = GSClusterSetDefaults.backoffMode;
  int       iterations = 10;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
		     "-clusterN",      ARGV_INT,    NULL,  &clusterN,       NULL, "number of clusters in ClusterSet",
		     "-bestN",         ARGV_INT,    NULL,  &bestN,          NULL, "number of active clusters (must be smaller than clusterN!)",
		     "-mode",          ARGV_INT,    NULL,  &mode,           NULL, "distance mode",
		     "-clusterA",      ARGV_INT,    NULL,  &clusterA,       NULL, "start generating with clusterA clusters",
		     "-seed",          ARGV_INT,    NULL,  &seed,           NULL, "seed for random generator",
		     "-spread",        ARGV_FLOAT,  NULL,  &spread,         NULL, "spread with of random generator",
		     "-backoffmode",   ARGV_INT,    NULL,  &backoffMode,    NULL, "0: auto, 1: fix backoff",
		     "-backoffdefault",ARGV_FLOAT,  NULL,  &backoffDefault, NULL, "fix backoff value for backoffmode 1 (e.g. 200)",
		     "-backofffactor", ARGV_FLOAT,  NULL,  &backoffFactor,  NULL, "backoff factor for backoffmode 0 (e.g. 10)",
		     "-decrease",      ARGV_FLOAT,  NULL,  &decrease,       NULL, "weight decrease for dimensions [0...1]",
		     "-iterations",    ARGV_INT,    NULL,  &iterations,     NULL, "maximum number of iterations per step",
		     NULL) != TCL_OK) { 
    return TCL_ERROR;
  }

  if (cbsP->list.itemA==NULL) {
    ERROR("There are no codebooks in CodebookSet!\n");
    return TCL_ERROR;
  }
  return gsClusterSetGen(gscsP, clusterN, mode,seed,spread,bestN,clusterA,decrease,backoffMode,backoffDefault,backoffFactor,iterations);
}

/* ----------------------------------------------------------------------
   activate a Gaussion Selection Cluster Set
   according to a given feature vector
   this function is called in scoreA.c before the scoring
   ---------------------------------------------------------------------- */
int gsClusterSetActivate(GSClusterSet *gscsP, float *feat, int *featI, int bestN) {
  CodebookSet   *cbsP = gscsP->cbsP;
  Codebook       *cbP = cbsP->list.itemA + 0;
  int        clusterN = gscsP->clusterN;
  int            dimN = (cbP->cfg)->rdimN > 0 ? (cbP->cfg)->rdimN : cbP->dimN;
  int           bestF = 0;
  float             s = 1/(cbsP->scaleRV*cbsP->scaleRV*cbsP->scaleCV);
  float      decrease = gscsP->decrease / (float)dimN;
  int        clusterX;
  int bestCluster[1024];
  int bestX,moveX;
  float bestClusterDist[1024];
	
  if (!gscsP) return TCL_OK;
  if (gscsP->clusterN<1) return TCL_OK;
	
  gscsP->eval_frames++;
	
  for (clusterX=0; clusterX<clusterN; clusterX++) { /* for each cluster ... */
    int dimN_4 = dimN>>2;
    int dimX;
    float weight = 1.0;
    float distSum = 0;
    if (cbsP->comMode!=0 && featI) {
      int *rvC = gscsP->cluster[clusterX].meanC;
      int *cvC = gscsP->cluster[clusterX].covarC;
      int *ptI = featI;
      unsigned int distSumI =0 ;
      int *weightI = gscsP->weightC;
      if (gscsP->mode == gs_mode_euclidean) {
	if (gscsP->decrease==0) {
	  for (dimX = 0; dimX < dimN_4; dimX++) { /* calculate euclidean distance */
	    int diff0 = *rvC++ - *ptI++;			distSumI += (diff0*diff0);	
	    diff0 = *rvC++ - *ptI++;				distSumI += (diff0*diff0);	
	    diff0 = *rvC++ - *ptI++;				distSumI += (diff0*diff0);
	    diff0 = *rvC++ - *ptI++;				distSumI += (diff0*diff0);
	  }
	} else {
	  for (dimX = 0; dimX < dimN_4; dimX++) { /* calculate weighted euclidean distance */
	    int diff0 = *rvC++ - *ptI++;			distSumI += ((diff0*diff0* (*weightI++))>>8);	
	    diff0 = *rvC++ - *ptI++;				distSumI += ((diff0*diff0* (*weightI++))>>8);	
	    diff0 = *rvC++ - *ptI++;				distSumI += ((diff0*diff0* (*weightI++))>>8);
	    diff0 = *rvC++ - *ptI++;				distSumI += ((diff0*diff0* (*weightI++))>>8);
	  }
	}
	distSumI *= *cvC;
      } else {
	for (dimX = 0; dimX < dimN_4; dimX++) { /* calculate weighted mahalanobis distance */
	  int diff0 = *rvC++ - *ptI++;				distSumI += ((diff0*diff0*(*cvC++)* (*weightI++))>>8);
	  diff0 = *rvC++ - *ptI++;					distSumI += ((diff0*diff0*(*cvC++)* (*weightI++))>>8);
	  diff0 = *rvC++ - *ptI++;					distSumI += ((diff0*diff0*(*cvC++)* (*weightI++))>>8);
	  diff0 = *rvC++ - *ptI++;					distSumI += ((diff0*diff0*(*cvC++)* (*weightI++))>>8);
	}
      }
      for (dimX = dimN_4<<2; dimX < dimN; dimX++) {
	int diff0 = *rvC++ - *ptI++;
	distSumI += ((diff0*diff0*(*cvC++)* (*weightI++))>>8);
      }
      distSum = ((float)distSumI) *s;
    } else 
      {
	float *rv = gscsP->cluster[clusterX].mean;
	float *cv = gscsP->cluster[clusterX].covar;
	float *pt = feat;
	for (dimX = 0; dimX < dimN_4; dimX++) { /* calculate mahalanobis distance */
	  register float diff0;
	  diff0 = *rv++ - *pt++;
	  distSum += diff0*diff0*(*cv++)*weight; weight -=decrease;
	  diff0 = *rv++ - *pt++;
	  distSum += diff0*diff0*(*cv++)*weight; weight -=decrease;
	  diff0 = *rv++ - *pt++;
	  distSum += diff0*diff0*(*cv++)*weight; weight -=decrease;
	  diff0 = *rv++ - *pt++;
	  distSum += diff0*diff0*(*cv++)*weight; weight -=decrease;
	}
	for (dimX = dimN_4<<2; dimX < dimN; dimX++) {
	  register float diff0 = *rv++ - *pt++;
	  distSum += diff0*diff0*(*cv++)*weight; weight -=decrease;
	}
      }
		
    /* Checking form down-top, should be faster then top-down */
    for (bestX=bestF-1; bestX>=0; bestX--) { /* for each cluster in best list */
      if (distSum>bestClusterDist[bestX]) {
	if (bestF<bestN) bestF++;
	bestX++;
	for (moveX=bestF-1; moveX>bestX; moveX--) { /* move best list down */
	  bestCluster[moveX] = bestCluster[moveX-1];
	  bestClusterDist[moveX] = bestClusterDist[moveX-1];
	}
	if (bestX<bestN) {
	  bestCluster[bestX] = clusterX;
	  bestClusterDist[bestX] = distSum;
	}
	bestX=-2;
	break;
      }
    }

    if (bestX==-1) { 
      if (bestF<bestN) bestF++;
      for (moveX=bestF-1; moveX>0; moveX--) { /* move best list down */
	bestCluster[moveX] = bestCluster[moveX-1];
	bestClusterDist[moveX] = bestClusterDist[moveX-1];
      }
      bestCluster[0] = clusterX;
      bestClusterDist[0] = distSum;
    }
    gscsP->cluster[clusterX].active = 0; /* deactive cluster first */
  }

  for (bestX=0; bestX<bestN; bestX++) {
    gscsP->cluster[bestCluster[bestX]].active = -1; /* and activate all clusters from the best list */
    gscsP->cluster[bestCluster[bestX]].active_count++;
  }
		
  if (gscsP->backoffMode==0) {
    gscsP->backoffValue = gsMahalanobisDist(feat,gscsP->cluster[bestCluster[bestN-1]].mean,gscsP->cluster[bestCluster[bestN-1]].covar,dimN,0) * gscsP->backoffFactor;
    if (gscsP->backoffValue>gscsP->backoffDefault) {gscsP->backoffValue=gscsP->backoffDefault;}
  }
  if (cbsP->comMode!=0) {
    gscsP->backoffValueI = (int) (gscsP->backoffValue * cbsP->scaleRV *  cbsP->scaleRV *  cbsP->scaleCV) ;
  }

  return TCL_OK;
}


/* ========================================================================
   Type Information
   ======================================================================== */

/* ---------- itf types for GSClusterSet */
static Method GSClusterSetMethod[] = {
  { "make",   gsClusterSetGenItf,   "generate a ClusterSet for Gaussian Selection" },
  { "load",   gsClusterSetLoadItf,  "load a ClusterSet for Gaussian Selection" },
  { "save",   gsClusterSetSaveItf,  "save a ClusterSet for Gaussian Selection" },
  { "puts",   gsClusterSetPutsItf,  "debug output for a ClusterSet for Gaussian Selection" },
  { NULL, NULL, NULL }
};


TypeInfo GSClusterSetInfo = { "GSClusterSet", 0, -1, GSClusterSetMethod,
			      gsClusterSetCreateItf, gsClusterSetFreeItf,
			      gsClusterSetConfigureItf, NULL,
			      itfTypeCntlDefaultNoLink,
			      "A 'GSClusterSet' object is a cluster set on the Gaussians of a CodebookSet." };


/* ---------- GSClusterSet_Init ---------- */
static int GSClusterSetInitialized = 0;

int GSClusterSet_Init (void) {

  if ( GSClusterSetInitialized ) return TCL_OK;

  GSClusterSetDefaults.cluster            = NULL;
  GSClusterSetDefaults.clusterN           = 0;
  GSClusterSetDefaults.clusterX           = NULL;
  GSClusterSetDefaults.bestN              = 32;
  GSClusterSetDefaults.backoffMode        = 1;
  GSClusterSetDefaults.backoffValue       = 256;
  GSClusterSetDefaults.backoffValueI      = 256;
  GSClusterSetDefaults.backoffDefault     = 256;
  GSClusterSetDefaults.backoffFactor      = 10;
  GSClusterSetDefaults.decrease           = 0.5;
  GSClusterSetDefaults.eval_frames        = 0;
  GSClusterSetDefaults.gaussians_computed = 0;
  GSClusterSetDefaults.gaussians_total    = 0;
  GSClusterSetDefaults.cbsP               = NULL;
  GSClusterSetDefaults.name               = NULL;
  GSClusterSetDefaults.dimN               = 0;
  GSClusterSetDefaults.use                = -1;
  GSClusterSetDefaults.mode               = gs_mode_euclidean;
  GSClusterSetDefaults.gaussianN          = 0;
  GSClusterSetDefaults.weightC            = NULL;
  GSClusterSetDefaults.diff               = 0;

  GSClusterSetInitialized = 1;	

  itfNewType (&GSClusterSetInfo);

  return TCL_OK;
}

