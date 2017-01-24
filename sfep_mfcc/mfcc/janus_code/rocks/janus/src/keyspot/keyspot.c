
#include "itf.h"
#include "feature.h"
#include "featureMethodItf.h"
#include "keyspot.h"
#include "score.h"
#include "scoreA.h"
#include "senones.h"





static int         keyspotInitialized = 0;

/* DEBUG */
FILE *logFile;
/* DEBUG */



/*****************************************************************************/
/* destroyKeyword                                                            */
/*****************************************************************************/
void destroyKeyword(Keyword* keyword)
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
/* createKeyword                                                             */
/*****************************************************************************/
Keyword* createKeyword(SenoneSet* senoneSet, int senonesN, char** senonesSA)
{
  Keyword* keyword;
  int      senoneX, senoneIndex, i;

  /*-----------------*/
  /* allocate memory */
  /*-----------------*/
  keyword = (Keyword*) malloc(sizeof(Keyword));
  if (keyword==NULL)
  {
    ERROR("Memory allocation failed!");
    return NULL;
  }
  memset(keyword, 0, sizeof(Keyword));
  keyword->senoneIndexA = malloc(sizeof(*(keyword->senoneIndexA))*senonesN);
  if (keyword->senoneIndexA==NULL)
  {
    ERROR("Memory allocation failed!");
    destroyKeyword(keyword);
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
      destroyKeyword(keyword);
      return NULL;
    }
    //for (j=0; j<senonesN; j++)
  }
  keyword->N = (float*) malloc(sizeof(float)*senonesN);
  if (keyword->N==NULL)
  {
    ERROR("Memory allocation failed!");
    destroyKeyword(keyword);
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
      destroyKeyword(keyword);
      return NULL;
    }
    keyword->senoneIndexA[senoneX] = senoneIndex;
  }
  keyword->senonesN       = senonesN;
  keyword->scoreThresh    = 3.0;
  keyword->entropyThresh  = -0.78;

  return keyword;
}



/*****************************************************************************/
/* deinitKeyspotter                                                          */
/*****************************************************************************/
void deinitKeyspotter(Keyspotter* keyspotter)
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
      destroyKeyword(keyspotter->keywordA[keywordX]);
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
/* destroyKeyspotter                                                         */
/*****************************************************************************/
void destroyKeyspotter(Keyspotter* keyspotter)
{
  deinitKeyspotter(keyspotter);
  free(keyspotter);
}

 

/*****************************************************************************/
/* createKeyspotter                                                          */
/*****************************************************************************/
Keyspotter* createKeyspotter(SenoneSet* senoneSet)
{
  Keyspotter* keyspotter;
  
  /*-----------------*/
  /* allocate memory */
  /*-----------------*/
  keyspotter = malloc(sizeof(Keyspotter));
  if (keyspotter==NULL)
  {
    ERROR("Memory allocation failed!");
    return NULL;
  }
  memset(keyspotter, 0, sizeof(Keyspotter));
  keyspotter->senoneSet = senoneSet;

  return keyspotter;
}



/*****************************************************************************/
/* createKeyspotter                                                          */
/*****************************************************************************/
int initKeyspotter(Keyspotter* keyspotter, int keywordsN)
{

  /*-----------------------------------------*/
  /* deinit keyspotter before doing anything */
  /*-----------------------------------------*/
  deinitKeyspotter(keyspotter);

  /*-----------------*/
  /* allocate memory */
  /*-----------------*/
  keyspotter->keywordA = malloc(sizeof(Keyword*)*keywordsN);
  if (keyspotter->keywordA==NULL)
  {
    ERROR("Memory allocation failed!");
    deinitKeyspotter(keyspotter);
    return 0;
  }
  keyspotter->senoneIndecesA = malloc(DYN_MEM_BLOCK_SIZE);
  if (keyspotter->senoneIndecesA==NULL)
  {
    ERROR("Memory allocation failed!");
    deinitKeyspotter(keyspotter);
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
/* initializeKeyspotter                                                      */
/*****************************************************************************/
int initializeKeyspotter(Keyspotter* keyspotter)
{

  int       highestSenoneIndex, keywordX, senoneX, indexX, senoneIndex, found;
  int       *newSenoneIndecesA;
  Keyword   *keyword;
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
            deinitKeyspotter(keyspotter);
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
int initKeyspottingItf(ClientData cd, int argc, char *argv[])
{

  Keyspotter  *keyspotter;
  SenoneSet   *senoneSet;
  int         ac = argc-1;
  int         keywordX, keywordsN, keywordsNC, senonesN;
  char        *senoneSLL = NULL;
  char        **senoneSA = NULL;          /* senone string array */
  char        **senoneSLA = NULL;         /* senone string list array */
  char        *scoreThreshSL = NULL;
  char        **scoreThreshSA = NULL;
  char        *entropyThreshSL = NULL;
  char        **entropyThreshSA = NULL;


  /*----------------*/
  /* get keyspotter */
  /*----------------*/
  keyspotter = (Keyspotter*) cd;
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
     "-scoreThresh",      ARGV_STRING,  NULL, &scoreThreshSL,   NULL,    "(list of) score thresholds",
     "-entropyThresh",    ARGV_STRING,  NULL, &entropyThreshSL, NULL,    "(list of) entropy thresholds",
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

  /*----------------------*/
  /* extract scoreThreshs */
  /*----------------------*/
  if (scoreThreshSL!=NULL)
  {
    if (Tcl_SplitList(itf, scoreThreshSL, &keywordsNC, &scoreThreshSA) != TCL_OK) {
      ERROR ("initKeyspot: error splitting list specified in -scoreThresh\n");
      Tcl_Free((char*)senoneSLA);
      return TCL_ERROR;
    }
    if (keywordsNC!=keywordsN)
    {
      ERROR ("initKeyspot: list specified in -tsw must have #words elements\n");
      if (scoreThreshSA!=NULL)
        Tcl_Free((char*)scoreThreshSA);
      Tcl_Free((char*)senoneSLA);
      return TCL_ERROR;
    }
  }

  /*------------------------*/
  /* extract entropyThreshs */
  /*------------------------*/
  if (entropyThreshSL!=NULL)
  {
    if (Tcl_SplitList(itf, entropyThreshSL, &keywordsNC, &entropyThreshSA) != TCL_OK) {
      ERROR ("initKeyspot: error splitting list specified in -entropyThresh\n");
      if (scoreThreshSA!=NULL)
        Tcl_Free((char*)scoreThreshSA);
      Tcl_Free((char*)senoneSLA);
      return TCL_ERROR;
    }
    if (keywordsNC!=keywordsN)
    {
      ERROR ("initKeyspot: list specified in -entropyThresh must have #words elements\n");
      if (scoreThreshSA!=NULL)
        Tcl_Free((char*)scoreThreshSA);
      if (entropyThreshSA!=NULL)
        Tcl_Free((char*)entropyThreshSA);
      Tcl_Free((char*)senoneSLA);
      return TCL_ERROR;
    }
  }

  /*-------------------*/
  /* create keyspotter */
  /*-------------------*/
  if (!initKeyspotter(keyspotter, keywordsN))
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
        deinitKeyspotter(keyspotter);
        if (scoreThreshSA!=NULL)
          Tcl_Free((char*)scoreThreshSA);
        if (entropyThreshSA!=NULL)
          Tcl_Free((char*)entropyThreshSA);
        Tcl_Free((char*)senoneSLA);
	      return TCL_ERROR;
    }
    /*-------------*/
    /* get keyword */
    /*-------------*/
    keyspotter->keywordA[keywordX] = createKeyword(senoneSet, senonesN, senoneSA);
    if (keyspotter->keywordA[keywordX]==NULL)
    {
      deinitKeyspotter(keyspotter);
      Tcl_Free((char*)senoneSA);
      if (entropyThreshSA!=NULL)
        Tcl_Free((char*)entropyThreshSA);
      if (scoreThreshSA!=NULL)
        Tcl_Free((char*)scoreThreshSA);
      Tcl_Free((char*)senoneSLA);
      return(TCL_ERROR);
    }
    Tcl_Free((char*)senoneSA);

    /*--------------------------*/
    /* set weights & thresholds */
    /*--------------------------*/
    if (scoreThreshSA!=NULL)
      keyspotter->keywordA[keywordX]->scoreThresh = atof(scoreThreshSA[keywordX]);
    if (entropyThreshSA!=NULL)
      keyspotter->keywordA[keywordX]->entropyThresh = atof(entropyThreshSA[keywordX]);
  }

  /*----------*/
  /* clean up */
  /*----------*/
  if (scoreThreshSA!=NULL)
    Tcl_Free((char*)scoreThreshSA);
  if (entropyThreshSA!=NULL)
    Tcl_Free((char*)entropyThreshSA);
  Tcl_Free((char*)senoneSLA);

  /*-----------------------*/
  /* initialize keyspotter */
  /*-----------------------*/
  if (initializeKeyspotter(keyspotter)==0)
  {
    deinitKeyspotter(keyspotter);    
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
int keyspotItf(ClientData cd, int argc, char *argv[])
{

  float         log_self = log(0.8);
  float         log_trans = log(0.2);
  Keyspotter    *keyspotter;
  SenoneSet     *senoneSet;
	int           ac = argc-1;
	int           frameB=0, frameE, frame;
	float         min_lkld, min_w_lkld;
  float         left_lkld, right_lkld;
	int           length, wIndex;
  float         entropy, sumDiff; 
	float         lkld[MAX_PH];
  int           keywordX, senoneX, scoreX;
  float         **dC, **eC, **tC, *N;
  Keyword*      keyword;
  int           senonesN, LAST, CURR;
  float         scoreThresh, entropyThresh;
  float         score, diffScore, maxDiffScore;
  int           beginPoint;
  int           lkldCutoff;

  float         thresholdScore;

  /*----------------*/
  /* get keyspotter */
  /*----------------*/
  keyspotter = (Keyspotter*) cd;
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

    //printf("frame: %i\n", keyspotter->counter+frame-frameB);

    /*----------------------------*/
    /* evaluate all senone scores */
    /*----------------------------*/
    ssa_opt(senoneSet, keyspotter->senoneIndecesA, keyspotter->scoreA, keyspotter->senoneIndecesN, frame);
    //ssa_base(senoneSet, keyspotter->senoneIndecesA, keyspotter->scoreA, keyspotter->senoneIndecesN, frame);

    /*----------------------*/
    /* find global min lkld */
    /*----------------------*/
    min_lkld = MAX_FLOAT;
    maxDiffScore = -MAX_FLOAT;
    for (scoreX=0; scoreX<keyspotter->senoneIndecesN; scoreX++)
      if (keyspotter->scoreA[scoreX] < min_lkld)
        min_lkld = keyspotter->scoreA[scoreX];

    /*----------------------------------*/
    /* don't forget to reset min_w_lkld */
    /*----------------------------------*/
    min_w_lkld = MAX_FLOAT;
    wIndex = -1;

    if (logFile!=NULL)
      fprintf(logFile, "FRAME: %i\n", keyspotter->counter+frame-frameB);

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
      scoreThresh   = keyword->scoreThresh;
      entropyThresh = keyword->entropyThresh;
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
      /*min_lkld = MAX_FLOAT;
      for (senoneX=0; senoneX<senonesN; senoneX++)
		    if (lkld[senoneX] < min_lkld)
			    min_lkld = lkld[senoneX];*/

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
      lkldCutoff = 5;
      if(sumDiff < lkldCutoff*senonesN) {
		    for(senoneX=0; senoneX<senonesN; senoneX++)
          lkld[senoneX] = lkldCutoff;
      }
      /* DEBUG */
      /*if (logFile!=NULL) {
        fprintf(logFile, "frame: %i, keword: %i\n", keyspotter->counter+frame-frameB, keywordX);
        fprintf(logFile, "score {");
		    for(senoneX=0; senoneX<senonesN; senoneX++)
          fprintf(logFile, "%f ", lkld[senoneX]);
        fprintf(logFile, "}\n");
      }*/
      /* DEBUG */

	    left_lkld = (dC[LAST][0]*tC[LAST][0]-log_self + lkld[0])/(1.0 + tC[LAST][0]);
	    right_lkld = lkld[0];
	    if(left_lkld < right_lkld)
  	  {
	  	  dC[CURR][0] = left_lkld;
		    tC[CURR][0] = tC[LAST][0]+1.0;
		    N[0] += 1.0;
  		  if(N[0] != 1.0)
	  	  	eC[CURR][0] = eC[LAST][0] - (N[0]-1.0)*log(N[0]-1.0) + N[0]*log(N[0]);
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

      /*--------------------------*/
      /* get length & begin point */
      /*--------------------------*/
	    length = (int)tC[CURR][senonesN-1];
      beginPoint = keyspotter->counter+(frame-frameB)- length;

      /*---------------------------*/
      /* calculate score threshold */
      /*---------------------------*/
      //thresholdScore = threshWeight;
      //thresholdScore = 3.0;
	    //thresholdScore = -((float)(senonesN-1)*log_trans + (float)(length-senonesN)*log_self)/(float)(length-1.0);
	    //thresholdScore += 0.02*(float)senonesN;
	    /*thresholdScore += threshWeight*pow((float)senonesN, 2.0);
	    if (thresholdScore>threshCutoff)
        thresholdScore = threshCutoff;*/
      
      /*-----------------------*/
      /* get score & diffScore */
      /*-----------------------*/
      score = dC[CURR][senonesN-1];
      diffScore = scoreThresh - score;

      /*--------------------*/
      /* remember best word */
      /*--------------------*/
      entropy = 0;
      if ((diffScore > maxDiffScore) && (beginPoint != keyspotter->prevBeginPoint)) 
	    {
	      thresholdScore = -((float)(senonesN-1)*log_trans + (float)(length-senonesN)*log_self)/(float)(length-1.0);
	      thresholdScore += 0.02*(float)senonesN;
        /*-------------------*/
        /* calculate entropy */
        /*-------------------*/
	      entropy = eC[CURR][senonesN-1];
        entropy /= (float)length;
	      entropy -= log((float)length);
        entropy /= log((float)senonesN);
        /*-----------------------------*/
        /* entropy condition satisfied */
        /*-----------------------------*/
        if (entropy < entropyThresh) 
        {
		      wIndex = keywordX;
		      min_w_lkld = dC[CURR][senonesN-1];
          maxDiffScore = diffScore;
          keyspotter->beginPoint = beginPoint;
        }
      }

      if (logFile!=NULL)
        fprintf(logFile, "keyword %i:   score %f, scoreThresh %f, entropy %f, entropyThresh %f \n", keywordX, score, scoreThresh, entropy, entropyThresh);

      /* DEBUG */
      /*
      if (logFile!=NULL)
      {
        fprintf(logFile, "dC {");
		    for(senoneX=0; senoneX<senonesN; senoneX++)
          fprintf(logFile, "%f ", dC[CURR][senoneX]);
        fprintf(logFile, "}\n");
        fprintf(logFile, "entropy: %f, entropy-thresh: %f, length: %i\n", entropy, thresholdScore, length);
        //fprintf(logFile, "min_w_lkld: %f, length: %i, entropy: %f\n", min_w_lkld, length, entropy);
      }*/
      /* DEBUG */

    }//(nW=0;nW<nWords....)
    CURR ^= 1;
    LAST ^= 1;

    /* DEBUG */
    if (logFile!=NULL)
      fprintf(logFile, "\n");
    /* DEBUG */

    //if(min_w_lkld <= thresholdScore && entropy < threshEntropy  && keyspotter->beginPoint != keyspotter->prevBeginPoint && length > 60) 
    //if(min_w_lkld <= thresholdScore && entropy < threshEntropy  && keyspotter->beginPoint != keyspotter->prevBeginPoint) 
    if (maxDiffScore>0)
    {
      if (logFile!=NULL)
        fprintf(logFile, "keyword %i spoted.\n\n", wIndex);
      //printf("keyword %i spoted.\n\n", wIndex);
      //printf("score: %f  |  entropy: %i\n", keyspotter->keywordA[wIndex]->senonesN);
		  keyspotter->prevBeginPoint = keyspotter->beginPoint;
		  itfAppendElement("%d", keyspotter->beginPoint);
		  itfAppendElement("%d", wIndex);
		  //printf("keyword found....\n");
		  //for(keywordX=0; keywordX<keyspotter->keywordsN; keywordX++)
      keywordX = wIndex;
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
  		}
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
ClientData keyspotterCreateItf(ClientData cd, int argc, char *argv[])
{

  Keyspotter* keyspotter;
  SenoneSet*  senoneSet;
  char*       logFileName = NULL;


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
  keyspotter = createKeyspotter(senoneSet);
  return (ClientData) keyspotter;

  /*ERROR("usage: Keyspotter <name> <senoneSet>.\n");
  return NULL;*/
}



/*****************************************************************************/
/* fmatrixFreeItf                                                            */
/*****************************************************************************/
static int keyspotterFreeItf(ClientData cd)
{
  /* DEBUG */
  if (logFile!=NULL)
    fclose(logFile);
  /* DEBUG */
  destroyKeyspotter((Keyspotter*)cd);
  return TCL_OK;
}



/*****************************************************************************/
/* ITF OBJECT METHODS: Keyspotter                                            */
/*****************************************************************************/
Method keyspotMethod[] = 
{
  { "initKeyspot",  initKeyspottingItf,   "initialize keyspotting" },
  { "keyspot",      keyspotItf,           "run keyspotter " },
  { NULL,           NULL,                 NULL } 
};





/*****************************************************************************/
/* ITF TYPEINFO: Keyspotter                                                  */
/*****************************************************************************/
TypeInfo keyspotterInfo = { "Keyspotter", 0, -1, keyspotMethod, 
        keyspotterCreateItf, keyspotterFreeItf, NULL,
			  NULL, itfTypeCntlDefaultNoLink,
		    "A keyspotter object\n" } ;



/*****************************************************************************/
/* Keyspot_Init                                                              */
/*****************************************************************************/
int Keyspot_Init (void)
{

  if (!keyspotInitialized) {
    itfNewType(&keyspotterInfo);
    keyspotInitialized = 1;
  }

  return TCL_OK;
}
