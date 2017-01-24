
#include "itf.h"
#include "feature.h"
#include "featureMethodItf.h"
#include "keyspotLP.h"
#include "score.h"
#include "scoreA.h"
#include "senones.h"





static int         keyspotLPInitialized = 0;

/* DEBUG */
FILE *logFile;
/* DEBUG */



/*****************************************************************************/
/* destroyKeywordLP                                                            */
/*****************************************************************************/
void destroyKeywordLP(KeywordLP* keyword)
{
  int i;

  if (keyword!=NULL)
  {
    if (keyword->senoneIndexA!=NULL)
      free(keyword->senoneIndexA);
    for (i=0; i<2; i++)
    {
      if (keyword->dC[i]!=NULL)
        free(keyword->dC[i]);
      if (keyword->tC[i]!=NULL)
        free(keyword->tC[i]);
      if (keyword->eC[i]!=NULL)
        free(keyword->eC[i]);
    }
    if (keyword->N!=NULL)
      free(keyword->N);
    free(keyword);
  }
}



/*****************************************************************************/
/* createKeywordLP                                                             */
/*****************************************************************************/
KeywordLP* createKeywordLP(SenoneSet* senoneSet, int senonesN, char** senonesSA)
{
  KeywordLP* keyword;
  int        senoneX, senoneIndex, i;

  /*-----------------*/
  /* allocate memory */
  /*-----------------*/
  keyword = (KeywordLP*) malloc(sizeof(KeywordLP));
  if (keyword==NULL)
  {
    ERROR("Memory allocation failed!");
    return NULL;
  }
  memset(keyword, 0, sizeof(KeywordLP));
  keyword->senoneIndexA = malloc(sizeof(*(keyword->senoneIndexA))*senonesN);
  if (keyword->senoneIndexA==NULL)
  {
    ERROR("Memory allocation failed!");
    destroyKeywordLP(keyword);
    return NULL;
  }
  for (i=0; i<=1; i++)
  {
    keyword->dC[i] = (float*) malloc(sizeof(float)*senonesN);
    keyword->tC[i] = (float*) malloc(sizeof(float)*senonesN);
    keyword->eC[i] = (float*) malloc(sizeof(float)*senonesN);
    if ((keyword->dC[i]==NULL) || (keyword->tC[i]==NULL) || (keyword->eC[i]==NULL))
    {
      ERROR("Memory allocation failed!");
      destroyKeywordLP(keyword);
      return NULL;
    }
    //for (j=0; j<senonesN; j++)
  }
  keyword->N = (float*) malloc(sizeof(float)*senonesN);
  if (keyword->N==NULL)
  {
    ERROR("Memory allocation failed!");
    destroyKeywordLP(keyword);
    return NULL;
  }
  
  /*----------------------*/
  /* initialize structure */
  /*----------------------*/
  for (senoneX=0; senoneX<senonesN; senoneX++)
  {
    senoneIndex = snsIndex(senoneSet, senonesSA[senoneX]);
    if (senoneIndex<0)
    {
      ERROR("Could not resolve senone[%s]!", senonesSA[senoneX]);
      destroyKeywordLP(keyword);
      return NULL;
    }
    keyword->senoneIndexA[senoneX] = senoneIndex;
  }
  keyword->senonesN       = senonesN;
  /*keyword->threshWeight   = .0010;
  keyword->threshEntropy  = -0.88;
  keyword->threshCutoff   = 2.0;
  keyword->lkldCutoff     = 4.0;*/

  return keyword;
}



/*****************************************************************************/
/* deinitKeyspotterLP                                                          */
/*****************************************************************************/
void deinitKeyspotterLP(KeyspotterLP* keyspotter)
{
  int keywordX;

  /*------------*/
  /* early exit */
  /*------------*/
  if (keyspotter==NULL)
    return;

  /*-------------*/
  /* free memory */
  /*-------------*/
  if (keyspotter->keywordA!=0)
  {
    for (keywordX=0; keywordX<keyspotter->keywordsN; keywordX++)
      destroyKeywordLP(keyspotter->keywordA[keywordX]);
    free(keyspotter->keywordA);
  }
  if (keyspotter->senoneIndecesA!=0)
    free(keyspotter->senoneIndecesA);
  if (keyspotter->scoreA!=0)
    free(keyspotter->scoreA);

  /*-----------------*/
  /* reset variables */
  /*-----------------*/
  keyspotter->keywordA       = NULL;
  keyspotter->senoneIndecesA = NULL;
  keyspotter->scoreA         = NULL;
  keyspotter->keywordsN      = 0;

}

 

/*****************************************************************************/
/* destroyKeyspotterLP                                                         */
/*****************************************************************************/
void destroyKeyspotterLP(KeyspotterLP* keyspotter)
{
  deinitKeyspotterLP(keyspotter);
  free(keyspotter);
}

 

/*****************************************************************************/
/* createKeyspotterLP                                                          */
/*****************************************************************************/
KeyspotterLP* createKeyspotterLP(SenoneSet* senoneSet)
{
  KeyspotterLP* keyspotter;
  
  /*-----------------*/
  /* allocate memory */
  /*-----------------*/
  keyspotter = malloc(sizeof(KeyspotterLP));
  if (keyspotter==NULL)
  {
    ERROR("Memory allocation failed!");
    return NULL;
  }
  memset(keyspotter, 0, sizeof(KeyspotterLP));
  keyspotter->senoneSet = senoneSet;

  return keyspotter;
}



/*****************************************************************************/
/* createKeyspotterLP                                                          */
/*****************************************************************************/
int initKeyspotterLP(KeyspotterLP* keyspotter, int keywordsN)
{

  /*-----------------------------------------*/
  /* deinit keyspotter before doing anything */
  /*-----------------------------------------*/
  deinitKeyspotterLP(keyspotter);

  /*-----------------*/
  /* allocate memory */
  /*-----------------*/
  keyspotter->keywordA = malloc(sizeof(KeywordLP*)*keywordsN);
  if (keyspotter->keywordA==NULL)
  {
    ERROR("Memory allocation failed!");
    deinitKeyspotterLP(keyspotter);
    return 0;
  }
  keyspotter->senoneIndecesA = malloc(DYN_MEM_BLOCK_SIZE);
  if (keyspotter->senoneIndecesA==NULL)
  {
    ERROR("Memory allocation failed!");
    deinitKeyspotterLP(keyspotter);
    return 0;
  }

  /*---------------------------------*/
  /* initialize keyspotter structure */
  /*---------------------------------*/
  keyspotter->senoneIndecesASize = DYN_MEM_BLOCK_SIZE;
  keyspotter->senoneIndecesN     = 0;
  keyspotter->keywordsN          = keywordsN;
  
  return 1;

}


  
/*****************************************************************************/
/* initializeKeyspotterLP                                                      */
/*****************************************************************************/
int initializeKeyspotterLP(KeyspotterLP* keyspotter)
{

  int       highestSenoneIndex, keywordX, senoneX, indexX, senoneIndex, found;
  int       *newSenoneIndecesA;
  KeywordLP   *keyword;
  int       CURR;
  int       LAST;

  /*------------------*/
  /* catch seg faults */
  /*------------------*/
  if (keyspotter==NULL)
    return 0;

  /*------------------------*/
  /* initialize CURR & LAST */
  /*------------------------*/
  keyspotter->LAST = 0;
  keyspotter->CURR = 1;

  /*-----------------*/
  /* get CURR & LAST */
  /*-----------------*/
  LAST = keyspotter->LAST;
  CURR = keyspotter->CURR;


  highestSenoneIndex = 0;
  for (keywordX=0; keywordX<keyspotter->keywordsN; keywordX++)
  {
    keyword = keyspotter->keywordA[keywordX];
    for (senoneX=0; senoneX<(keyword->senonesN); senoneX++)
    {
      /*--------------------*/
      /* get current senone */
      /*--------------------*/
      senoneIndex = keyword->senoneIndexA[senoneX];
      /*----------------------------*/
      /* find senone in index array */
      /*----------------------------*/
      found = 0;
      for (indexX=0; indexX<keyspotter->senoneIndecesN; indexX++)
        if (senoneIndex==keyspotter->senoneIndecesA[indexX])
        {
          found = 1;
          break;
        }
      /*---------------------------------*/
      /* not found => add to index array */
      /*---------------------------------*/
      if (!found)
      {
        /*-----------------------------*/
        /* allocate memory when needed */
        /*-----------------------------*/
        if ((keyspotter->senoneIndecesN+1)*sizeof(*(keyspotter->senoneIndecesA))>keyspotter->senoneIndecesASize)
        {
          newSenoneIndecesA = malloc(keyspotter->senoneIndecesASize + DYN_MEM_BLOCK_SIZE);
          if (newSenoneIndecesA==NULL)
          {
            ERROR("Memory allocation failed!");
            deinitKeyspotterLP(keyspotter);
            return 0;
          }
          if (keyspotter->senoneIndecesA!=NULL)
          {
            memcpy(newSenoneIndecesA, keyspotter->senoneIndecesA, keyspotter->senoneIndecesASize);
            free(keyspotter->senoneIndecesA);
          }
          keyspotter->senoneIndecesA = newSenoneIndecesA;
          keyspotter->senoneIndecesASize += DYN_MEM_BLOCK_SIZE;
        }
        /*-----*/
        /* add */
        /*-----*/
        keyspotter->senoneIndecesA[keyspotter->senoneIndecesN] = senoneIndex;
        keyspotter->senoneIndecesN++;
      }
      /*-------------------------------*/
      /* remember highest senone index */
      /*-------------------------------*/
      if (highestSenoneIndex < (keyword->senoneIndexA[senoneX]))
        highestSenoneIndex = keyword->senoneIndexA[senoneX];
      /*------------*/
      /* initialize */
      /*------------*/
      keyword->dC[LAST][senoneX] = MAX_FLOAT;
		  keyword->dC[CURR][senoneX] = 0.0;
		  keyword->tC[LAST][senoneX] = 0.0;
		  keyword->tC[CURR][senoneX] = 0.0;
		  keyword->eC[LAST][senoneX] = 0.0;
		  keyword->eC[CURR][senoneX] = 0.0;
		  keyword->N[senoneX]        = 0.0;
    }
  	keyword->dC[LAST][0] = 0.0;
  }

  keyspotter->scoreA = malloc(sizeof(float)*(highestSenoneIndex+1));
  if (keyspotter->scoreA==NULL)
  {
    ERROR("Memory Allocation Failure!");
    return 0;
  }

  return 1;

}


  
/*****************************************************************************/
/* intKeyspotItf                                                         */
/*****************************************************************************/
int initKeyspottingLPItf(ClientData cd, int argc, char *argv[])
{

  KeyspotterLP  *keyspotter;
  SenoneSet   *senoneSet;
  int         ac = argc-1;
  int         keywordX, keywordsN, keywordsNC, senonesN;
  char        *senoneSLL = NULL;
  char        **senoneSA = NULL;          /* senone string array */
  char        **senoneSLA = NULL;         /* senone string list array */

  char        *lengthSL = NULL;
  char        **lengthSA = NULL;
  char        **threshEntropySA = NULL;
  char        *entropySL = NULL;
  char        **entropySA = NULL;
  char        *scoreSL = NULL;
  char        **scoreSA = NULL;


  /*----------------*/
  /* get keyspotter */
  /*----------------*/
  keyspotter = (KeyspotterLP*) cd;
  if (keyspotter==NULL)
  {
    ERROR("keyspotter was NULL!");
    return TCL_ERROR;
  }

  /*----------------*/
  /* get senone set */
  /*----------------*/
  senoneSet = keyspotter->senoneSet;
  if (senoneSet==NULL)
  {
    ERROR("keyspotter->senoneSet was NULL!");
    return TCL_ERROR;
  }

  /*-----------------*/
  /* parse arguments */
  /*-----------------*/
  if (itfParseArgv( argv[0], &ac,  argv+1,  0,
     "<words>",           ARGV_STRING,  NULL, &senoneSLL,       NULL,    "(list of) list of senone indices",
     "-length",       ARGV_STRING,  NULL, &lengthSL,  NULL,    "(list of) threshold score weights",
     "-score",     ARGV_STRING,  NULL, &scoreSL,  NULL,    "(list of) threshold score cut offs",
     "-entropy",       ARGV_STRING,  NULL, &entropySL,    NULL,    "(list of) lkld cut offs",
      NULL) != TCL_OK) {
      return TCL_ERROR;
  }
  
  /*---------------*/
  /* extract words */
  /*---------------*/
  if (Tcl_SplitList(itf, senoneSLL, &keywordsN, &senoneSLA) != TCL_OK) {
    ERROR ("initKeyspot: error splitting list <words>\n");
    return TCL_ERROR;
  }

  /*----------------*/
  /* extract length */
  /*----------------*/
  if (lengthSL!=NULL)
  {
    if (Tcl_SplitList(itf, lengthSL, &keywordsNC, &lengthSA) != TCL_OK) {
      ERROR ("initKeyspot: error splitting list specified in -threshWghts\n");
      Tcl_Free((char*)senoneSLA);
      return TCL_ERROR;
    }
    if (keywordsNC!=keywordsN)
    {
      ERROR ("initKeyspot: list specified in -tsw must have #words elements\n");
      if (lengthSA!=NULL)
        Tcl_Free((char*)lengthSA);
      Tcl_Free((char*)senoneSLA);
      return TCL_ERROR;
    }
  }

  /*----------------------*/
  /* extract lkld cut off */
  /*----------------------*/
  if (entropySL!=NULL)
  {
    if (Tcl_SplitList(itf, entropySL, &keywordsNC, &entropySA) != TCL_OK) {
      ERROR ("initKeyspot: error splitting list specified in -entropyThresh\n");
      if (lengthSA!=NULL)
        Tcl_Free((char*)lengthSA);
      if (threshEntropySA!=NULL)
        Tcl_Free((char*)threshEntropySA);
      Tcl_Free((char*)senoneSLA);
      return TCL_ERROR;
    }
    if (keywordsNC!=keywordsN)
    {
      ERROR ("initKeyspot: list specified in -lkldCutoff must have #words elements\n");
      if (entropySA!=NULL)
        Tcl_Free((char*)entropySA);
      if (lengthSA!=NULL)
        Tcl_Free((char*)lengthSA);
      if (threshEntropySA!=NULL)
        Tcl_Free((char*)threshEntropySA);
      Tcl_Free((char*)senoneSLA);
      return TCL_ERROR;
    }
  }

  /*---------------------------------*/
  /* extract threshold score cut off */
  /*---------------------------------*/
  if (scoreSL!=NULL)
  {
    if (Tcl_SplitList(itf, scoreSL, &keywordsNC, &scoreSA) != TCL_OK) {
      ERROR ("initKeyspot: error splitting list specified in -entropyThresh\n");
      if (entropySA!=NULL)
        Tcl_Free((char*)entropySA);
      if (lengthSA!=NULL)
        Tcl_Free((char*)lengthSA);
      if (threshEntropySA!=NULL)
        Tcl_Free((char*)threshEntropySA);
      Tcl_Free((char*)senoneSLA);
      return TCL_ERROR;
    }
    if (keywordsNC!=keywordsN)
    {
      ERROR ("initKeyspot: list specified in -lkldCutoff must have #words elements\n");
      if (scoreSA!=NULL)
        Tcl_Free((char*)scoreSA);
      if (entropySA!=NULL)
        Tcl_Free((char*)entropySA);
      if (lengthSA!=NULL)
        Tcl_Free((char*)lengthSA);
      if (threshEntropySA!=NULL)
        Tcl_Free((char*)threshEntropySA);
      Tcl_Free((char*)senoneSLA);
      return TCL_ERROR;
    }
  }

  /*-------------------*/
  /* create keyspotter */
  /*-------------------*/
  if (!initKeyspotterLP(keyspotter, keywordsN))
    return(TCL_ERROR);

  /*--------------*/
  /* add keywords */
  /*--------------*/
  for (keywordX=0; keywordX<keywordsN; keywordX++)
  {
    /*------------------------*/
    /* parse senoneSLL[wordX] */
    /*------------------------*/
  	if (Tcl_SplitList (itf, senoneSLA[keywordX], &senonesN, &senoneSA) != TCL_OK)
    {
	      ERROR ("restartKeySpot: could not split list of words\n");
        deinitKeyspotterLP(keyspotter);
        if (lengthSA!=NULL)
          Tcl_Free((char*)lengthSA);
        Tcl_Free((char*)senoneSLA);
	      return TCL_ERROR;
    }
    /*-------------*/
    /* get keyword */
    /*-------------*/
    keyspotter->keywordA[keywordX] = createKeywordLP(senoneSet, senonesN, senoneSA);
    if (keyspotter->keywordA[keywordX]==NULL)
    {
      deinitKeyspotterLP(keyspotter);
      if (entropySA!=NULL)
        Tcl_Free((char*)entropySA);
      Tcl_Free((char*)senoneSA);
      if (lengthSA!=NULL)
        Tcl_Free((char*)lengthSA);
      Tcl_Free((char*)senoneSLA);
      return(TCL_ERROR);
    }
    Tcl_Free((char*)senoneSA);

    /*--------------------------*/
    /* set weights & thresholds */
    /*--------------------------*/
    if (lengthSA!=NULL)
      keyspotter->keywordA[keywordX]->threshLength = atof(lengthSA[keywordX]);
    if (entropySA!=NULL)
      keyspotter->keywordA[keywordX]->threshEntropy = atof(entropySA[keywordX]);
    if (scoreSA!=NULL)
      keyspotter->keywordA[keywordX]->threshScore = atof(scoreSA[keywordX]);
  }

  /*----------*/
  /* clean up */
  /*----------*/
  if (scoreSA!=NULL)
    Tcl_Free((char*)scoreSA);
  if (entropySA!=NULL)
    Tcl_Free((char*)entropySA);
  if (threshEntropySA!=NULL)
    Tcl_Free((char*)threshEntropySA);
  if (lengthSA!=NULL)
    Tcl_Free((char*)lengthSA);
  Tcl_Free((char*)senoneSLA);

  /*-----------------------*/
  /* initialize keyspotter */
  /*-----------------------*/
  if (initializeKeyspotterLP(keyspotter)==0)
  {
    deinitKeyspotterLP(keyspotter);    
    return(TCL_ERROR);
  }
  keyspotter->counter        = 0;
  keyspotter->prevBeginPoint = 0;
  keyspotter->beginPoint     = 0;

  /*printf("Leaving init\n");*/

  return TCL_OK;
}




/*****************************************************************************/
/* keySpotItf                                                                */
/*****************************************************************************/
int keyspotLPItf(ClientData cd, int argc, char *argv[])
{

  float         log_self = log(0.8);
  float         log_trans = log(0.2);
  KeyspotterLP  *keyspotter;
  SenoneSet     *senoneSet;
	int           ac = argc-1;
	int           frameB=0, frameE, frame;
	float         min_lkld, min_w_lkld;
  float         left_lkld, right_lkld;
	int           length, wIndex=0;
  float         entropy, sumDiff; 
	float         lkld[MAX_PH];
  int           keywordX, senoneX;
  float         **dC, **eC, **tC, *N;
  KeywordLP*    keyword;
  int           senonesN, LAST, CURR;
  float         threshEntropy;
  
  float         diffScore, maxDiffScore;

  /*----------------*/
  /* get keyspotter */
  /*----------------*/
  keyspotter = (KeyspotterLP*) cd;
  if (keyspotter==NULL)
  {
    ERROR("keyspotter was NULL!");
    return TCL_ERROR;
  }

  /*----------------*/
  /* get senone set */
  /*----------------*/
  senoneSet = keyspotter->senoneSet;
  if (senoneSet==NULL)
  {
    ERROR("keyspotter->senoneSet was NULL!");
    return TCL_ERROR;
  }

  /*-------------------*/
  /* extract variables */
  /*-------------------*/
  LAST          = keyspotter->LAST;
  CURR          = keyspotter->CURR;

  /*-----------------*/
  /* parse arguments */
  /*-----------------*/
	if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<frame>",   ARGV_INT,    NULL, &frameB, 0, "index of the begin frame",
      "<frame>",   ARGV_INT,    NULL, &frameE, 0, "index of the end frame",
	NULL) != TCL_OK) return TCL_ERROR;

  /*----------------*/
  /* iterate frames */
  /*----------------*/
  for(frame=frameB; frame<=frameE; frame++)
  {

    /*----------------------------*/
    /* evaluate all senone scores */
    /*----------------------------*/
    ssa_opt(senoneSet, keyspotter->senoneIndecesA, keyspotter->scoreA, keyspotter->senoneIndecesN, frame);
    //ssa_base(senoneSet, keyspotter->senoneIndecesA, keyspotter->scoreA, keyspotter->senoneIndecesN, frame);

    /*----------------------------------*/
    /* don't forget to reset min_w_lkld */
    /*----------------------------------*/
    min_w_lkld = MAX_FLOAT;
    maxDiffScore = -MAX_FLOAT;

    /*------------------*/
    /* iterate keywords */
    /*------------------*/
    for(keywordX=0; keywordX<keyspotter->keywordsN; keywordX++)
    {
      keyword = keyspotter->keywordA[keywordX];

      /*-------------------*/
      /* extract variables */
      /*-------------------*/
      senonesN      = keyword->senonesN;
      /*threshWeight  = keyword->threshWeight;
      threshEntropy = keyword->threshEntropy;
      threshCutoff  = keyword->threshCutoff;
      lkldCutoff    = keyword->lkldCutoff;*/
      dC            = keyword->dC;
      tC            = keyword->tC;
      eC            = keyword->eC;
      N             = keyword->N;

      /*----------*/
      /* get lkld */
      /*----------*/
      for (senoneX=0; senoneX<senonesN; senoneX++)
        lkld[senoneX] = keyspotter->scoreA[keyword->senoneIndexA[senoneX]];

      /*---------------*/
      /* find min lkld */
      /*---------------*/
      min_lkld = MAX_FLOAT;
      for (senoneX=0; senoneX<senonesN; senoneX++)
		    if (lkld[senoneX] < min_lkld)
			    min_lkld = lkld[senoneX];

      /* DEBUG */
      if (logFile!=NULL)
        fprintf(logFile, "%i %i ", keyspotter->counter+frame-frameB, keywordX);
      /*fprintf(logFile, "frame %i keyword %i score {", keyspotter->counter+frame-frameB, keywordX);
		  for(senoneX=0; senoneX<senonesN; senoneX++)
        fprintf(logFile, "%f ", lkld[senoneX]);
      fprintf(logFile, "} ");*/
      /* DEBUG */

      /*------------------------------*/
      /* differential lkld & sum lkld */
      /*------------------------------*/
      sumDiff = 0.0;
	    for(senoneX=0; senoneX<senonesN; senoneX++)
      {
        lkld[senoneX] -= min_lkld;
        sumDiff += lkld[senoneX];
      }

      /* DEBUG */
      /*fprintf(logFile, "sumdiff %f ", sumDiff);*/
      /* DEBUG */

      /*--------------*/
      /* cut off lkld */
      /*--------------*/
	    /*if(sumDiff < lkldCutoff*senonesN)
		    for(senoneX=0; senoneX<senonesN; senoneX++)
          lkld[senoneX] = lkldCutoff;*/

	    left_lkld = (dC[LAST][0]*tC[LAST][0]-log_self + lkld[0])/(1.0 + tC[LAST][0]);
	    right_lkld = lkld[0];
	    if(left_lkld < right_lkld)
  	  {
	  	  dC[CURR][0] = left_lkld;
		    tC[CURR][0] = tC[LAST][0]+1.0;
		    N[0] += 1.0;
  		  if(N[0] != 1.0)
	  	  	keyword->eC[CURR][0] = eC[LAST][0] - (N[0]-1.0)*log(N[0]-1.0) 
                                 + N[0]*log(N[0]);
		    else
		     	eC[CURR][0] = eC[LAST][0];
      }
	    else{
		    dC[CURR][0] = right_lkld;
		    tC[CURR][0] = 1.0;
  		  N[0] = 1.0;
	  	  eC[CURR][0] = 0.0;
      }
      for(senoneX=1; senoneX<senonesN; senoneX++) 
	    {
		    left_lkld  = (dC[LAST][senoneX-1]*tC[LAST][senoneX-1] - log_trans + lkld[senoneX])/(1.0 + tC[LAST][senoneX-1]);
  		  right_lkld = (dC[LAST][senoneX]*tC[LAST][senoneX] - log_self + lkld[senoneX])/(1.0 + tC[LAST][senoneX]);
		    if(left_lkld < right_lkld)
		    {// transition from previous state preferred
  			  dC[CURR][senoneX] = left_lkld;
	  		  tC[CURR][senoneX] = 1.0 + tC[LAST][senoneX-1];
		  	  N[senoneX] = 1.0;
			    eC[CURR][senoneX] = eC[LAST][senoneX-1];
		    }
  		  else{// self transition preferred
	  		  tC[CURR][senoneX] = 1.0 + tC[LAST][senoneX];
		  	  dC[CURR][senoneX] = right_lkld;
			    N[senoneX] += 1.0;
			    if(N[senoneX] != 1.0)
			  	  eC[CURR][senoneX] = eC[LAST][senoneX] + N[senoneX]*log(N[senoneX])-(N[senoneX]-1.0)*log(N[senoneX]-1.0);
			    else eC[CURR][senoneX] = eC[LAST][senoneX];
        }
      }
		  length = (int)keyword->tC[CURR][senonesN-1];
      /* START MOD-1 */
      //diffScore = keyword->threshScore - keyword->dC[CURR][senonesN-1];
      if (keyword->dC[CURR][senonesN-1] < keyword->threshScore)
        diffScore = 1.0 - (keyword->dC[CURR][senonesN-1] / keyword->threshScore);
      else
        diffScore = - MAX_FLOAT;
      /* END MOD-1 */
	    entropy = keyword->eC[CURR][senonesN-1];
      entropy /= (float)length;
	    entropy -= log((float)length);
      entropy /= log((float)senonesN);
      threshEntropy = -0.85;
      if ((length > keyword->threshLength) && (entropy < threshEntropy) && (maxDiffScore < diffScore) ) 
	    {
		    wIndex = keywordX;
        maxDiffScore = diffScore;
        keyspotter->beginPoint = keyspotter->counter + (frame-frameB) - length;
      }

      /* DEBUG */
      /*fprintf(logFile, "dc %f thresh %f\n", dC[CURR][senonesN-1], thresholdScore);*/
      if (logFile!=NULL)
      {
        fprintf(logFile, "%f %i %f\n", keyword->dC[CURR][senonesN-1], length, entropy);
      }
      /* DEBUG */

    }//(nW=0;nW<nWords....)
    CURR ^= 1;
    LAST ^= 1;
  
    if ((maxDiffScore>0) && (keyspotter->beginPoint != keyspotter->prevBeginPoint)) 
    {
      //printf("len: %i   score: %f", length, maxDiffScore);
		  keyspotter->prevBeginPoint = keyspotter->beginPoint;
		  itfAppendElement("%d", keyspotter->beginPoint);
		  itfAppendElement("%d", wIndex);
		  //printf("keyword found....\n");
		  /*for(keywordX=0; keywordX<keyspotter->keywordsN; keywordX++) 
		  {
        keyword  = keyspotter->keywordA[keywordX];
        senonesN = keyword->senonesN;
        for(senoneX=0; senoneX<senonesN; senoneX++)
        {
				  keyword->dC[LAST][senoneX] = MAX_FLOAT;
				  keyword->tC[LAST][senoneX] = 0.0;
				  keyword->eC[LAST][senoneX] = 0.0;
				  keyword->eC[CURR][senoneX] = 0.0;
				  keyword->N[senoneX]        = 0.0;
  			}
  			keyword->dC[LAST][0] = 0.0;
  		}*/
    }//if(min_w_lkld.....)
    
  }//(frame=frameB....)
  keyspotter->counter += (frameE-frameB+1);

  /*------------*/
  /* write back */
  /*------------*/
  keyspotter->LAST = LAST;
  keyspotter->CURR = CURR;

  return TCL_OK;
}





/*****************************************************************************/
/* keyspotCreateItf                                                          */
/*****************************************************************************/
ClientData keyspotterLPCreateItf(ClientData cd, int argc, char *argv[])
{

  KeyspotterLP* keyspotter;
  SenoneSet*    senoneSet;
  char*         logFileName = NULL;


  if (itfParseArgv( argv[0], &argc,  argv+1,  0,
     "<SenoneSet>",  ARGV_OBJECT, NULL, &senoneSet,   "SenoneSet",    "senone set",
     "-dbg",         ARGV_STRING, NULL, &logFileName, NULL,           "filename for debug output",

      NULL) != TCL_OK) {
      return NULL;
  }

  /* DEBUG */
  logFile = NULL;
  if (logFileName!=NULL)
    logFile = fopen(logFileName, "w");
  /* DEBUG */
  keyspotter = createKeyspotterLP(senoneSet);
  return (ClientData) keyspotter;

  /*ERROR("usage: KeyspotterLP <name> <senoneSet>.\n");
  return NULL;*/
}



/*****************************************************************************/
/* fmatrixFreeItf                                                            */
/*****************************************************************************/
static int keyspotterLPFreeItf(ClientData cd)
{
  /* DEBUG */
  if (logFile!=NULL)
    fclose(logFile);
  /* DEBUG */
  destroyKeyspotterLP((KeyspotterLP*)cd);
  return TCL_OK;
}



/*****************************************************************************/
/* ITF OBJECT METHODS: KeyspotterLP                                            */
/*****************************************************************************/
Method keyspotLPMethod[] = 
{
  { "initKeyspot",  initKeyspottingLPItf, "initialize keyspotting" },
  { "keyspot",      keyspotLPItf,         "run keyspotter " },
  { NULL,           NULL,                 NULL } 
};





/*****************************************************************************/
/* ITF TYPEINFO: KeyspotterLP                                                  */
/*****************************************************************************/
TypeInfo keyspotterLPInfo = { "KeyspotterLP", 0, -1, keyspotLPMethod, 
        keyspotterLPCreateItf, keyspotterLPFreeItf, NULL,
			  NULL, itfTypeCntlDefaultNoLink,
		    "A keyspotter-lp object\n" } ;



/*****************************************************************************/
/* Keyspot_Init                                                              */
/*****************************************************************************/
int KeyspotLP_Init (void)
{

  if (!keyspotLPInitialized) {
    itfNewType(&keyspotterLPInfo);
    keyspotLPInitialized = 1;
  }

  return TCL_OK;
}
