/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: SampleSet
               ------------------------------------------------------------

    Author  :  Ivica Rogina
    Module  :  lh.c
    Date    :  $Id: lh.c 2715 2006-10-24 16:12:31Z fuegen $
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
    Revision 1.7  2006/10/24 16:12:31  fuegen
    bugfixes for maximum likelihood based clustering

    Revision 1.6  2003/08/14 11:20:15  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.5.4.3  2003/07/02 17:45:19  fuegen
    fixes for file handling under Windows
    using general functions for file open/close

    Revision 1.5.4.2  2002/11/13 09:15:51  soltau
    *** empty log message ***

    Revision 1.5.4.1  2002/11/13 08:28:37  soltau
    lhLoad  : append data
    lhClear : new

    Revision 1.3  2000/08/16 09:30:01  soltau
    added lhFlush info

    Revision 1.2  1999/10/05 08:13:21  soltau
    made lhFlush safe (file locking !!!)

    Revision 1.1  1997/07/16 11:31:48  rogina
    Initial revision

    Revision 1.4  1996/03/05 21:22:36  maier
    Warning in sampleSetAccu instead segmentation fault

 * Revision 1.3  1996/01/19  21:21:01  rogina
 * added allCount (made modulus work)
 *
 * Revision 1.2  96/01/19  13:52:03  13:52:03  rogina (Ivica Rogina)
 * added maxCount
 * 
 * Revision 1.1  96/01/10  20:06:12  20:06:12  rogina (Ivica Rogina)
 * Initial revision
 * 

   ======================================================================== */

#include "common.h"
#include "lh.h"
#include "itf.h"
#include "array.h"
#include "mach_ind_io.h"

/* ------------------------------------------------------------------------- */
/* lhLikleihood								     */
/* Expect a pointer "dsVal" to a distribution, and a pointer to an lh-struct */
/* "lh". Return the log-likelihood of all the frames in the lh-struct.       */
/* The dsVal values must be real probabilities and not log-probabilities.    */
/* ------------------------------------------------------------------------- */

float lhLikelihood (float *dsVal, Lh *lh)
{
  int i,j; 
  double like = 0.0;

  short *valX = lh->valX;
  float *valG = lh->valG;

  /* for each sample and each reference vector */
  for (i=0; i<lh->frameN; i++)	{
    double subLike = 0.0;
    for (j=0; j<lh->valN[i]; j++) {
      int    X = *valX++;		
      double G = *valG++;		
      subLike += exp(-1.0*G) * dsVal[X];
    }
    like -= log(subLike);
  }
  return like;
}

/* ------------------------------------------------------------------------- */
/* lhLikelihoodItf							     */
/* return the likelhood of the given lh-struct using the listed distribution */
/* weights (real probabilities, not log-probabilities)                       */
/* ------------------------------------------------------------------------- */

static int lhLikelihoodItf (ClientData cd, int argc, char *argv[])
{
  Lh *lh = (Lh*)cd;
  int     ac     =  argc-1;
  FArray  prob = {NULL, 0};

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
	  "<probs>",   ARGV_FARRAY,NULL,&prob, NULL,"array of mixture weights",
          NULL) != TCL_OK)
    return TCL_ERROR;

  itfAppendResult("%e\n",lhLikelihood(prob.itemA,lh));

  farrayDeinit(&prob);
  return TCL_OK;
}


/* ------------------------------------------------------------------------- */
/* lhCreate								     */
/* ------------------------------------------------------------------------- */

Lh* lhCreate(void) {
  Lh *lh = malloc(sizeof(Lh));

  if (lh == NULL) {
    ERROR("lhCreate: allocation failure\n");
    return NULL;
  }

  lh->valN   = NULL;
  lh->valG   = NULL;
  lh->valX   = NULL;
  lh->modX   = NULL;
  lh->gaussX = 0;
  lh->frameN = 0;
  lh->gaussN = 0;

  return lh;
}

static ClientData lhCreateItf (ClientData cd, int argc, char *argv[])
{
  return (ClientData) lhCreate();
}

/* ------------------------------------------------------------------------- */
/* lhFree								     */
/* ------------------------------------------------------------------------- */

int lhFree (Lh *lh)
{
  if (lh)  {
    if (lh->valN) free(lh->valN);
    if (lh->valG) free(lh->valG);
    if (lh->valX) free(lh->valX);
    if (lh->modX) free(lh->modX);
    free(lh);
  }
  return TCL_OK;
}

static int lhFreeItf (ClientData cd)
{
  Lh *lh = (Lh*)cd;
  printf("FREEING LH ...\n");
  return lhFree(lh);
}

/* ------------------------------------------------------------------------- */
/* lhClear								     */
/* ------------------------------------------------------------------------- */

static int lhClear (Lh *lh) 
{
  if (lh->valN) free(lh->valN);
  if (lh->valG) free(lh->valG);
  if (lh->valX) free(lh->valX);
  if (lh->modX) free(lh->modX);
  lh->valN   = NULL;
  lh->valG   = NULL;
  lh->valX   = NULL;
  lh->modX   = NULL;
  lh->gaussX = 0;
  lh->frameN = 0;
  lh->gaussN = 0;
  return TCL_OK;
}

static int lhClearItf (ClientData cd, int argc, char *argv[])
{
  Lh *lh = (Lh*)cd;
  return lhClear(lh);
}

/* ------------------------------------------------------------------------- */
/* lhAlloc                                                                   */
/* ------------------------------------------------------------------------- */

int lhAlloc (Lh* lh, int gaussN)
{
  if (gaussN == lh->gaussN) return TCL_OK;
  if (gaussN < lh->gaussN) {
    WARN("lhAlloc:  reduce nr. of gaussians %d -> %d\n",lh->gaussN,gaussN);
  }

  lh->gaussN = gaussN;

  if (NULL == (lh->valN= realloc(lh->valN,lh->gaussN*sizeof(short)))) {
    ERROR("lhAlloc: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (lh->valG= realloc(lh->valG,lh->gaussN*sizeof(float)))) {
    ERROR("lhAlloc: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (lh->valX= realloc(lh->valX,lh->gaussN*sizeof(short)))) {
    ERROR("lhAlloc: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (lh->modX= realloc(lh->modX,lh->gaussN*sizeof(int)))) {
    ERROR("lhAlloc: allocation failure\n");
    return TCL_ERROR;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------- */
/* lhLoad								     */
/* ------------------------------------------------------------------------- */

int lhLoad (Lh* lh,char *fileName)
{
  FILE *fp;
  int gaussX= lh->gaussX;
  int frameN, frameX, gaussN, i;
  
  if ((fp=fileOpen(fileName,"r"))==NULL) { 
    ERROR("Can't open file %s for reading\n",fileName); 
    return TCL_ERROR; 
  }

  frameN = read_int(fp);
  gaussN = read_int(fp);

  INFO("lhLoad %s. frameN=%d  gaussN=%d  gaussTotal= %d\n",fileName,frameN,
       gaussN,lh->gaussN+gaussN);

  lhAlloc(lh,lh->gaussN+gaussN);

  for (frameX=lh->frameN; frameX<lh->frameN+frameN; frameX++) {
    lh->modX[frameX] = read_int   (fp);
    lh->valN[frameX] = read_short (fp);

    for (i=0; i<lh->valN[frameX]; i++, gaussX++) {
      lh->valX[gaussX] = read_short (fp);
      lh->valG[gaussX] = read_float (fp);
    }
  }
  lh->frameN += frameN;
  lh->gaussX += gaussN;
  assert(gaussX == lh->gaussX);

  fileClose (fileName, fp);
  return TCL_OK;
}

static int lhLoadItf (ClientData cd, int argc, char *argv[])
{
  Lh*    lhP = (Lh*) cd;
  char* file = NULL;
  int   ac   = (argc > 1) ? 1 : 0;

  if (itfParseArgv( argv[0], &ac, argv+1, 1,
      "<file>",    ARGV_STRING, NULL, &file, NULL, "feature name",
       NULL) != TCL_OK) return TCL_ERROR;

  return lhLoad(lhP,file);
}

/* ------------------------------------------------------------------------- */
/* lhPuts								     */
/* Print all the contents of the given lh-struct.                            */
/* ------------------------------------------------------------------------- */

int lhPuts (Lh *lh)
{
  int frameX, i, k=0;

  itfAppendResult("frameN %d ",lh->frameN);
  itfAppendResult("gaussX %d ",lh->gaussX);
  itfAppendResult("gaussN %d { ",lh->gaussN);

  for (frameX=0; frameX<lh->frameN; frameX++) 
  {  
      itfAppendResult("%d %d { ",lh->modX[frameX],lh->valN[frameX]);
      for (i=0; i<lh->valN[frameX]; i++, k++) 
          itfAppendResult("{%d %e} ",lh->valX[k],lh->valG[k]); 
      itfAppendResult("} } ");
  }
  return TCL_OK;
}

static int lhPutsItf (ClientData cd, int argc, char *argv[])
{
  Lh *lh = (Lh*)cd;
  return lhPuts(lh);
}

/* ------------------------------------------------------------------------- */
/* lhFlush								     */
/* Append all contents of the given lh-struct to the named file.             */
/* At the beginning of the file the numbers of frames and gaussians is       */
/* updated.                                                                  */
/* ------------------------------------------------------------------------- */

int lhFlush (Lh* lh, char* fileName)
{
  FILE *fp;
  int  gaussN, frameN, frameX, i,k=0;
  
  if ((fp=fileOpen(fileName,"a+"))==NULL) { 
    ERROR("Can't open file %s for writing\n",fileName); 
    return TCL_ERROR; 
  }

  fseek(fp,0L,SEEK_END);
  if (ftell(fp) == 0L ) {
    frameN = lh->frameN;
    gaussN = lh->gaussX;
  } else {
    fseek(fp,0L,SEEK_SET);
    frameN = read_int(fp);
    gaussN = read_int(fp);
    frameN += lh->frameN;
    gaussN += lh->gaussX;
  }

  INFO("flush %s. frameN= %d/%d  gaussN= %d/%d \n",fileName,lh->frameN,frameN,
       lh->gaussX,gaussN);

  /* reopen it again in mode r+ to be able to overwrite some portions
     of the file */
  fileClose(fileName, fp);
  if ((fp=fileOpen(fileName,"r+"))==NULL) { 
    ERROR("Can't open file %s for writing\n",fileName); 
    return TCL_ERROR; 
  }

  fseek(fp,0L,SEEK_SET);
  write_int(fp,frameN); 
  write_int(fp,gaussN);
  fseek(fp,0L,SEEK_END);

  for (frameX=0; frameX<lh->frameN; frameX++) {
    write_int   (fp,lh->modX[frameX]);
    write_short (fp,lh->valN[frameX]);

    for (i=0; i<lh->valN[frameX]; i++, k++) {
      write_short(fp,lh->valX[k]);
      write_float(fp,lh->valG[k]);
    }
  }
  
  fileClose(fileName, fp);
  lh->frameN = 0;
  lh->gaussX = 0;

  return frameN;
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method lhMethod[] = 
{ 
  { "puts",  lhPutsItf,       NULL },
  { "like",  lhLikelihoodItf, "compute likelihood" },
  { "load",  lhLoadItf,       "load likelihoods"   },
  { "clear", lhClearItf,      "clear likelihoods"  },
  {  NULL,   NULL,             NULL } 
};

TypeInfo lhInfo = { 
  "Lh", 0, -1, lhMethod, lhCreateItf, lhFreeItf, NULL, NULL, 
  itfTypeCntlDefaultNoLink, "a codebook-likelihoods accumulator" 
} ;

static int lhInitialized = 0;

int Lh_Init()
{
  if (lhInitialized) return TCL_OK;
  itfNewType (&lhInfo);

  lhInitialized = 1;
  return TCL_OK;
}


