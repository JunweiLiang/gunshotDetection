/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Feature I/O Module
               ------------------------------------------------------------

    Author  :  Martin Westphal
    Module  :  featureIOItf.c
    Date    :  $Id: featureIOItf.c 3376 2011-02-28 00:34:58Z metze $
    Remarks :

   ========================================================================

    This software was developed by

    the  University of Karlsruhe       and  Carnegie Mellon University
         Dept. of Informatics               Dept. of Computer Science
         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
         Am Fasanengarten 5                 5000 Forbes Ave
         D-76131 Karlsruhe                  Pittsburgh, PA 15213
         - West Germany -                   - USA -

    Copyright (C) 1990-1994.   All rights reserved.

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

    $Log: featureIOItf.c,v $
    Revision 1.1.1.2  2008/09/10 17:02:03  metze.florian
    FFa+FMe

    Revision 1.1.1.1.4.1  2008/09/09 09:59:00  metze.florian
    Added readHtk

    Revision 1.1.1.1  2007/06/20 12:34:49  metze.florian
    Initial import of Ibis 5.1 from UKA at T-Labs

    Revision 3.6  2004/12/21 11:18:20  metze
    Removed unnecessary code

    Revision 3.5  2003/08/14 11:19:55  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.4.4.2  2003/07/02 17:15:32  fuegen
    fixes for file handling under Windows
    using general functions for file open/close

    Revision 3.4.4.1  2002/06/26 11:57:53  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.4  2001/01/15 09:49:56  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 3.3.4.1  2001/01/12 15:16:53  janus
    necessary changes for running janus under WINDOWS

    Revision 3.3  2000/11/14 12:25:53  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 3.2.34.1  2000/11/06 10:50:28  janus
    Made changes to enable compilation under -Wall.

    Revision 3.2  1997/07/09 17:42:22  westphal
    *** empty log message ***

  
   ======================================================================== */

/* ------------------------------------------------------------------------
   includes
   ------------------------------------------------------------------------ */
#include <string.h>
#include <math.h>
#include "common.h"
#include "mach_ind_io.h" 


#include "featureMethodItf.h"  /* includes matrix.h( itf.h(tcl.h), common.h) */

/* ---------------------------------------
   structure 'FeatureFile' will contain
   information read from the header of
   the feature file. It also holds an
   array of 'FeatureEntry's.
   --------------------------------------- */
typedef struct {
  char*       name;             /* name of the feature                      */
  int         from;             /* from coefficient                         */
  int         to;               /* to coefficient                           */
} FeatureEntry;

typedef struct {
  char         *filename;
  FILE         *fp;
  int          frameN;
  int          coeffN;
  int          byteMode;
  int          featN;
  FeatureEntry *fe;
  float        samplingRate;
  float        shift;
  float        snr;
} FeatureFile;

/* ------------------------------------------------------------------------
   static functions
   ------------------------------------------------------------------------ */
static int     diffParam(        Feature  *fe1, Feature  *fe2);

static FILE    *openFile(        char *filename, char *mode, int retry, int sleep);
static void    printHeader(      FeatureFile *FF);
static int     readRawHeader(    FeatureFile *FF);
static int     readHtkHeader(    FeatureFile *FF);
static FMatrix *readFeatureData( FeatureFile *FF, int from, int to);
static long    fileLength(       FILE *fp);
static int     checkFileLength(  FILE *fp);
static int     checkRange(       int *from, int *to, int totalN);
static int     readLine(         FILE *fp, char *line, int N);


/* ========================================================================
   Method 'write'
   concats features together and writes them to a file
   ======================================================================== */
int fesWriteItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet* FS = (FeatureSet*)cd;
  Feature     **source;
  char        *filename = NULL;
  int         sourceN  = 0;
  int         i, frameN, coeffN = 0;
  int         header    = FS->writeHeader;
  int         byteMode = FS->byteModeOut;
  
  /* ------------------
     get arguments
     ------------------ */
  if (argc-- < 3   ||   argv[2][0] == '-') {
     ERROR("Usage: %s <filename> <feature1> [<feature2> ...] [-flag value].\n", *argv);
     return TCL_ERROR;
  }

  filename = *(++argv); argc--;
  source   = (Feature**)malloc((unsigned)(argc * sizeof(Feature*)));

  while (argc > 0  &&  *(++argv)[0] != '-') {
     if ( getFe( cd, "", (ClientData)(source + sourceN), &argc, argv) < 0) {
	ERROR("Can't find feature '%s'.\n", *argv);
	free(source); return TCL_ERROR;
     }
     
     if (source[sourceN]->type != FE_FMATRIX)  {
	ERROR("feature '%s' must be of type FMatrix.\n", *argv);
	free(source); return TCL_ERROR;
     }
     
     if ( diffParam( source[sourceN], source[0]) ) {
	ERROR("feature '%s' has other parameters than '%s'.\n", *argv, source[0]->name);
	free(source); return TCL_ERROR;
     }
     coeffN += source[sourceN]->data.fmat->n; 
     sourceN++;
  }
  frameN = source[0]->data.fmat->m;
  
   /* --- get optional -flags  --- */
   if (itfParseArgv( argv[0], &argc, argv, 0,
		    "-h",  ARGV_INT, NULL,  &(header),   cd, "write header: 1, otherwise: 0",
		    "-bm", ARGV_INT, NULL,  &(byteMode), cd, "byte mode: 1, 2 or 4",
		    NULL) != TCL_OK) return TCL_ERROR;

  if (byteMode != 1 && byteMode != 2 && byteMode != 4) {
     ERROR("Unknown output mode %d. Must be 1, 2 or 4.\n",byteMode);
     return TCL_ERROR;
  }
  /* -------------------------
     write features to file
     ------------------------- */
  {
     FILE* fp = (streq(filename, "-"))?stdout:fileOpen(filename, "w");
     
     if (fp == NULL) {
	ERROR("Cannot open feature file %s.\n", filename); return -1;
     }
     /* ------------------------
	write RAW header
	------------------------ */
     if (header){
	int coeffB, coeffE = 0;

	fprintf(fp,"RAW\n");
	fprintf(fp,"frames %d\n",   frameN);
	fprintf(fp,"dimension %d\n",coeffN);
	switch(byteMode) {
	   case 1: fprintf(fp,"floatbytes\n");     break;
	   case 2: fprintf(fp,"scaled_vectors\n"); break;
	   case 4: fprintf(fp,"floats\n");         break;
	}
	fprintf(fp,"sampling_rate %.0f\n",source[0]->samplingRate);
	if (source[0]->shift == ROUND(source[0]->shift))
	  fprintf(fp,"shift %.0f\n", source[0]->shift);
	else
	  fprintf(fp,"shift %f\n", source[0]->shift);
	fprintf(fp,"{\n");
	for ( i=0; i<sourceN; i++) {
	   coeffB = coeffE;
	   coeffE = coeffB + source[i]->data.fmat->n;
	   fprintf(fp,"%s %d %d\n", source[i]->name, coeffB, coeffE-1);
	}
	fprintf(fp,"}\n");
	fprintf(fp,"end_header\n");
     }
     
     /* --------------------
	write data section
	-------------------- */
     {
	FMatrix *mat   = fmatrixCreate(frameN,coeffN);
	int     totalN   = frameN * coeffN;
	int     frameX, coeffX = 0;
	int     coeffI;
	
	/* --- put all source features into one matrix --- */ 
	for ( i=0; i<sourceN; i++) {
	   FMatrix *mat1 = source[i]->data.fmat;
	   
	   for (coeffI=0; coeffI< mat1->n; coeffI++, coeffX++)
	      for (frameX=0; frameX<frameN; frameX++)
		 mat->matPA[frameX][coeffX] = mat1->matPA[frameX][coeffI];
	}
	
	switch(byteMode)
	{
	case 1: {
	  for (frameX = 0; frameX < frameN; frameX++)
	    write_floatbytes (fp, mat->matPA[frameX], coeffN);
	  break;
	}
	 case 2:   {
	   /* FMatrix *matT   = fmatrixCreate(coeffN,frameN); */
	    int written;
	    
	    /* matT    = fmatrixTrans(matT, mat); */
	    written = write_scaled_vectors(fp,mat->matPA[0],coeffN,frameN);
	    /* fmatrixFree( matT); */
	    if (written != totalN) {
	       ERROR("Writing scaled vectors failed (%d/%d).\n",written,totalN);
	       if (fp!=stdout) fileClose(filename, fp);  fmatrixFree(mat);
	       return TCL_ERROR;
	    }
	    break;
	 }
	case 4: {
	  for (frameX = 0; frameX < frameN; frameX++)
	    write_floats (fp, mat->matPA[frameX], coeffN);
	}
	}
	if (fp!=stdout) fileClose(filename, fp);  fmatrixFree(mat);
     }
  }
  
  return TCL_OK;
}

/* ========================================================================
    Method 'read'
    reads one or more features from a file
   ======================================================================== */
#define FEATN_MAX 256
int fesReadItf( ClientData cd, int argc, char *argv[])
{
   FeatureSet *FS      = (FeatureSet*)cd;
   Feature    **destin;
   int        i, featI;
   int        destinI, destinN = 0;
   int        error   = 0;
   int        freemat = 1;
   int        from    = 0;
   int        to      = -1;
   FMatrix    *mat;
   static FeatureFile FF;
   static int firstTime = 1;
         
   /* ---------------------
      fill info structure
      --------------------- */
   if (firstTime) {
     /* FEATN_MAX 'FeatureEntry' are allocated and the name pointers are
	set to NULL. They will be freed and allocated when reading the
	file header. */
      int i;
      FF.fe = (FeatureEntry*)malloc( (unsigned)( FEATN_MAX * sizeof(FeatureEntry)));
      for(i=0;i<FEATN_MAX;i++) FF.fe[i].name = NULL;
      FF.filename = NULL;
      FF.fp       = NULL;
      firstTime = 0;
   }
   FF.frameN       = 0;
   FF.coeffN       = 0;
   FF.byteMode     = FS->byteModeIn;
   FF.featN        = 0;
   FF.samplingRate = FS->samplingRate;
   FF.shift        = 0.0;
   FF.snr          = 0.0;
   
   /* ------------------
      get arguments
      ------------------ */
   /* --- check min. requirement --- */
   if (argc-- < 3   ||  argv[1][0] == '-' ||  argv[2][0] == '-') {
      ERROR("Usage: %s <feature1> [<feature2> ...] <filename> [-flag value].\n"
	    , *argv);
      return TCL_ERROR;
   }
   argv++;
   
   /* --- How many features? --- */
   while (destinN < argc  &&  argv[destinN][0] != '-') destinN++;
   destinN--;
   destin = (Feature**)malloc((unsigned)(destinN * sizeof(Feature*)));
   
   /* --- get desired destination features and filename --- */
   for (i=0;i<destinN;i++) {
      if ( createFe( cd, "", (ClientData)(destin + i), &argc, argv + i) < 0) {
	 ERROR("Can't create feature '%s'.\n", argv[i]);
	 free(destin); return TCL_ERROR;
      }
   }
   for (i = 0; i < destinN; i++)
     destin[i] = FS->featA+i;

   if (FF.filename) free(FF.filename);
   FF.filename = strdup(argv[destinN]);   argc--;

   /* --- get optional -flags, some only used if no header --- */
   if (itfParseArgv( argv[0], &argc, argv+ destinN + 1, 0,
	"-c",  ARGV_INT, NULL,  &(FF.coeffN), cd, 
	"coefficients, taken when no header was found",
	"-bm", ARGV_INT, NULL,  &(FF.byteMode), cd, "byte mode: 1, 2 or 4",
	"-sh", ARGV_FLOAT, NULL,&(FF.shift),    cd, "shift in ms",
	"-sr", ARGV_FLOAT, NULL,&(FF.samplingRate),cd, "sampling rate in kHz",
	"-from", ARGV_INT, NULL,&from,          cd, "read from sample",
	"-to", ARGV_INT, NULL,  &to,            cd, "read to sample",
	NULL) != TCL_OK) return TCL_ERROR;

   /* -------------------------
      open file
      ------------------------- */
   if ( !(FF.fp=openFile(FF.filename,"r",10,2)) ) {
      free(destin); return TCL_ERROR;
   }
   
   /* ----------------
      read header
      ---------------- */
   if (readRawHeader(&FF) != TCL_OK) {
      if (FF.coeffN <= 0 && FF.byteMode != 2) {
	 ERROR("Number of coefficents not given.\n"
	       "Won't read from feature file %s.\n",FF.filename);
	 error = 1;;
      }
      else {
	 if (FF.byteMode ==  1  ||  FF.byteMode == 4) {
	    long length = fileLength(FF.fp);
	    FF.frameN   = (length / FF.byteMode) / FF.coeffN;
	    if (length != (long)(FF.byteMode * FF.frameN * FF.coeffN)) {
	       ERROR("Can't divide file length by number of coefficients:\n"
		     "%ld / %d bytes / %d coefs of feature file %s.\n",
		     length, FF.byteMode, FF.coeffN, FF.filename);
	       error = 1;
	    }
	    else WARN("Will use byte mode %d and %d coefficients\n"
		      "=> %d frames for feature file %s.\n",
		FF.byteMode, FF.coeffN, FF.frameN, FF.filename);
	 }
	 else if (FF.byteMode == 2) {
	    /* Try it later, s.b. */
	 }
	 else {
	   ERROR("read: Don't know byte mode %d, use 1, 2 or 4!\n",
		 FF.byteMode);
	   error = 1;
	 }
      }
   }
   if (error)  { fileClose(FF.filename, FF.fp); free(destin); return TCL_ERROR;}
   
   if (FE_VER) printHeader(&FF);
      
   /* ---------------------
      read data from file
      ----------------------- */
   if ((mat = readFeatureData(&FF,from,to))  == NULL) {
      ERROR("error reading data from feature file %s\n",FF.filename);
      fileClose(FF.filename, FF.fp); free(destin); return TCL_ERROR;
   }

   /* ---------------------------
      distribute coefficients
      destin:   desired features
      feat:     features as listed
      in the file header
      --------------------------- */
   for ( destinI=0; destinI<destinN; destinI++) {
      for ( featI=0; featI<FF.featN; featI++) 
	 if (streq( destin[destinI]->name, FF.fe[featI].name)) break;
      
      if (featI<FF.featN) {	 /* --- found feature           --- */
	 int     from   = FF.fe[featI].from;
	 int     coeffI = FF.fe[featI].to + 1 - from;
	 FMatrix *matI  = fmatrixCreate( mat->m, coeffI);
	   int frameX, coeffX;
	 
	 for (frameX=0; frameX<mat->m; frameX++)
	    for (coeffX=0; coeffX< coeffI; coeffX++)
	       matI->matPA[frameX][coeffX] = mat->matPA[frameX][coeffX+from];
	 feFMatrix( FS, destin[destinI], matI, 1);
      }
      else {                 /* --- didn't find feature     --- */
	 if (destinN == 1) { /* --- + take all coefficients --- */
	    feFMatrix( FS, destin[0], mat, 1); freemat = 0;
	 }
	 else {              /* --- + not in the file       --- */
	    ERROR("Warning: desired feature '%s' was not in file %s\n",
		    destin[destinI]->name,FF.filename);
	 }
      }
      
   } /* distribute */
   if (freemat) fmatrixFree(mat);
   fileClose(FF.filename, FF.fp);  free(destin);
   return TCL_OK;
}
   
int fesHtkReadItf( ClientData cd, int argc, char *argv[])
{
   FeatureSet *FS      = (FeatureSet*)cd;
   Feature    **destin;
   int        i, featI;
   int        destinI, destinN = 0;
   int        error   = 0;
   int        freemat = 1;
   int        from    = 0;
   int        to      = -1;
   int 	      ret, totalN;
   FMatrix    *mat;
   static FeatureFile FF;
   static int firstTime = 1;
         
   /* ---------------------
      fill info structure
      --------------------- */
   if (firstTime) {
     /* FEATN_MAX 'FeatureEntry' are allocated and the name pointers are
	set to NULL. They will be freed and allocated when reading the
	file header. */
      int i;
      FF.fe = (FeatureEntry*)malloc( (unsigned)( FEATN_MAX * sizeof(FeatureEntry)));
      for(i=0;i<FEATN_MAX;i++) FF.fe[i].name = NULL;
      FF.filename = NULL;
      FF.fp       = NULL;
      firstTime = 0;
   }
   FF.frameN       = 0;
   FF.coeffN       = 0;
   FF.byteMode     = FS->byteModeIn;
   FF.featN        = 0;
   FF.samplingRate = FS->samplingRate;
   FF.shift        = 0.0;
   FF.snr          = 0.0;
   
   /* ------------------
      get arguments
      ------------------ */
   /* --- check min. requirement --- */
   if (argc-- < 3   ||  argv[1][0] == '-' ||  argv[2][0] == '-') {
      ERROR("Usage: %s <feature1> [<feature2> ...] <filename> [-flag value].\n"
	    , *argv);
      return TCL_ERROR;
   }
   argv++;
   
   /* --- How many features? --- */
   while (destinN < argc  &&  argv[destinN][0] != '-') destinN++;
   destinN--;
   destin = (Feature**)malloc((unsigned)(destinN * sizeof(Feature*)));
   
   /* --- get desired destination features and filename --- */
   for (i=0;i<destinN;i++) {
      if ( createFe( cd, "", (ClientData)(destin + i), &argc, argv + i) < 0) {
	 ERROR("Can't create feature '%s'.\n", argv[i]);
	 free(destin); return TCL_ERROR;
      }
   }
   if (FF.filename) free(FF.filename);
   FF.filename = strdup(argv[destinN]);   argc--;
   
   /* --- get optional -flags, some only used if no header --- */
   if (itfParseArgv( argv[0], &argc, argv+ destinN + 1, 0,
	"-c",  ARGV_INT, NULL,  &(FF.coeffN), cd, 
	"coefficients, taken when no header was found",
	"-bm", ARGV_INT, NULL,  &(FF.byteMode), cd, "byte mode: 1, 2 or 4",
	"-sh", ARGV_FLOAT, NULL,&(FF.shift),    cd, "shift in ms",
	"-sr", ARGV_FLOAT, NULL,&(FF.samplingRate),cd, "sampling rate in kHz",
	"-from", ARGV_INT, NULL,&from,          cd, "read from sample",
	"-to", ARGV_INT, NULL,  &to,            cd, "read to sample",
	NULL) != TCL_OK) return TCL_ERROR;
   
   /* -------------------------
      open file
      ------------------------- */
   if ( !(FF.fp=openFile(FF.filename,"r",10,2)) ) {
      free(destin); return TCL_ERROR;
   }
   
   /* ----------------
      read header
      ---------------- */
   if (readHtkHeader(&FF) != TCL_OK) {
      if (FF.coeffN <= 0 && FF.byteMode != 2) {
	 ERROR("Number of coefficents not given.\n"
	       "Won't read from feature file %s.\n",FF.filename);
	 error = 1;;
      }
      else {
	 if (FF.byteMode ==  1  ||  FF.byteMode == 4) {
	    long length = fileLength(FF.fp);
	    FF.frameN   = (length / FF.byteMode) / FF.coeffN;
	    if (length != (long)(FF.byteMode * FF.frameN * FF.coeffN)) {
	       ERROR("Can't divide file length by number of coefficients:\n"
		     "%ld / %d bytes / %d coefs of feature file %s.\n",
		     length, FF.byteMode, FF.coeffN, FF.filename);
	       error = 1;
	    }
	    else WARN("Will use byte mode %d and %d coefficients\n"
		      "=> %d frames for feature file %s.\n",
		FF.byteMode, FF.coeffN, FF.frameN, FF.filename);
	 }
	 else if (FF.byteMode == 2) {
	    /* Try it later, s.b. */
	 }
	 else {
	   ERROR("read: Don't know byte mode %d, use 1, 2 or 4!\n",
		 FF.byteMode);
	   error = 1;
	 }
      }
   }
   if (error)  { fileClose(FF.filename, FF.fp); free(destin); return TCL_ERROR;}
   
   if (FE_VER) printHeader(&FF);
   
   
   /* ---------------------
      read data from file
      ----------------------- */
    	totalN = FF.frameN * FF.coeffN; 
	mat        = fmatrixCreate( FF.frameN, FF.coeffN);
	if (!mat) return TCL_ERROR;

	if ((ret=fread( mat->matPA[0], sizeof(float), totalN, FF.fp)) != totalN) {
	  ERROR("read only %d floats but wanted %d from file %s\n",
		ret, totalN, FF.filename);
	  fmatrixFree(mat); return TCL_ERROR;
	}
 
   /* ---------------------------
      distribute coefficients
      destin:   desired features
      feat:     features as listed
      in the file header
      --------------------------- */
   for ( destinI=0; destinI<destinN; destinI++) {
      for ( featI=0; featI<FF.featN; featI++) 
	 if (streq( destin[destinI]->name, FF.fe[featI].name)) break;
      
      if (featI<FF.featN) {	 /* --- found feature           --- */
	 int     from   = FF.fe[featI].from;
	 int     coeffI = FF.fe[featI].to + 1 - from;
	 FMatrix *matI  = fmatrixCreate( mat->m, coeffI);
	   int frameX, coeffX;
	 
	 for (frameX=0; frameX<mat->m; frameX++)
	    for (coeffX=0; coeffX< coeffI; coeffX++)
	       matI->matPA[frameX][coeffX] = mat->matPA[frameX][coeffX+from];
	 feFMatrix( FS, destin[destinI], matI, 1);
      }
      else {                 /* --- didn't find feature     --- */
	 if (destinN == 1) { /* --- + take all coefficients --- */
	    feFMatrix( FS, destin[0], mat, 1); freemat = 0;
	 }
	 else {              /* --- + not in the file       --- */
	    ERROR("Warning: desired feature '%s' was not in file %s\n",
		    destin[destinI]->name,FF.filename);
	 }
      }
      
   }/* distribute */
   if (freemat) fmatrixFree(mat);
   fileClose(FF.filename, FF.fp);  free(destin);
   return TCL_OK;
}
   
 
/* ========================================================================
   Static functions
   ======================================================================== */
/* ----------------------------------------
   check features for different parameters
   ---------------------------------------- */
static int  diffParam(Feature  *fe1, Feature  *fe2)
{
   if      (fe1->samplingRate != fe2->samplingRate)
      SERROR("different sampling rate: %f and %f\n",fe1->samplingRate,fe2->samplingRate);

   else if (fe1->shift        != fe2->shift)
      SERROR("different frame shift: %f and %f\n",  fe1->shift,       fe2->shift);

   else if (fe1->data.fmat->m  != fe2->data.fmat->m)
      SERROR("different number of frames: %d and %d\n",fe1->data.fmat->m, fe2->data.fmat->m);

   else  return 0;

   return 1;
}

/* ---------------------------------------------------------
   readLine
   Gets a 'line' from file 'fp' with maximum N (N-1) chars.
   --------------------------------------------------------- */
static int readLine(FILE *fp, char *line, int N)
{
   if ( fgets(line,N,fp) != NULL) {
      int len = strlen(line);
      
      if (len == N-1  &&  line[len-1] != '\n') return -1; /* > N */
      if (line[len-1]=='\n') line[--len] = '\0';
      return 0; /* ok */
   }
   return 1; /* EOF */
   
}

/* ---------------------------------------------------------
   checkFileLength
   --------------------------------------------------------- */
static int checkFileLength(FILE *fp)
{
   if (feof(fp)){
      SERROR("file too short.\n");
      return -1;
   }
   getc(fp); /* read in the next byte */
   if (!feof(fp)){
      SERROR("file too long.\n");
      return -2;
   }
   return 0;
}

/* ---------------------------------------------------------
   openFile
   --------------------------------------------------------- */
static FILE *openFile(char *filename, char *mode, int retry, int time)
{
  int count = 0;
  FILE* fp = NULL;
  while (count <= retry) {
    if ((fp=fileOpen(filename,mode))==NULL) {
      WARN("%d. try\n",count+1);
      sleep(time); count++;
    }
    else break;
  }
  if (count>retry) {
    ERROR("could not open %s\n",filename);
  }
  else if (count!=0)
    WARN("\nLoading of feature file %s was successful only after %d attempts\n",
	 filename,count+1);
  return fp;
}


/* ---------------------------------------------------------
   checkRange
   --------------------------------------------------------- */
static int checkRange(int *from, int *to, int totalN)
{
  if (*to >= totalN)
    WARN("read: 'to' = %d is too big, will read to last frame %d.\n",
	 *to, totalN - 1);
  if (*to < 0 || *to >= totalN) *to = totalN - 1;
  if (*from < 0)  *from = 0;
  if (*from > *to) {
    SERROR("read: Frame range from %d to %d is not ok for %d total frames.\n",
	   *from,*to,totalN);
    return 0;
  }
  return (*to - *from + 1);
}

/* ---------------------------------------------------------
   readFeatureData
   --------------------------------------------------------- */
static FMatrix *readFeatureData(FeatureFile *FF, int from, int to)
{
   int frameR = 0; /* frames to read */
   FMatrix *mat = NULL;

   /* FF->byteMode != 2 ?
                  fmatrixCreate( FF->frameN, FF->coeffN) : NULL; */
   int frameX, ret = 0;
   
   switch(FF->byteMode) {
    case 1:
      if ((frameR = checkRange(&from,&to,FF->frameN)) > 0) {
	int totalN = frameR * FF->coeffN; 
	mat        = fmatrixCreate( frameR, FF->coeffN);
	if (!mat) return NULL;

	fseek(FF->fp, (long)(from * FF->coeffN * sizeof(UBYTE)), SEEK_CUR);
	for (frameX = 0; frameX < frameR; frameX++)
	  ret += read_floatbytes( FF->fp, mat->matPA[frameX], FF->coeffN);
	if (ret != totalN) {
	  ERROR("read only %d floatbytes but wanted %d from file %s\n",
		ret, totalN, FF->filename);
	  fmatrixFree(mat); return NULL;
	}
      }
      else return NULL;
      break;
    case 2:
      {  int coeffN,frameN;    /* note: frameN is the total number of frames */
	 float *temp;
	 mat = fmatrixCreate(1,1); if (!mat) return NULL;
	 
	 if ((frameR = read_scaled_vectors_range(FF->fp, &temp,
		                                 &coeffN, &frameN,
						 from, to)) <= 0) {
	   if (frameR == -2) checkRange(&from,&to,frameN);
	   ERROR("Error reading scaled vectors from file %s.\n",FF->filename);
	   fmatrixFree(mat); return NULL;
	 }
	 checkRange(&from,&to,frameN);               /* just for the warning */
	 if (!FF->frameN) FF->frameN = frameN;        /* No header was read! */
	 if (!FF->coeffN) FF->coeffN = coeffN;
	 if (coeffN != FF->coeffN || frameN != FF->frameN){
	    ERROR("File header:  %d frames, %d coefficients\n",
		  FF->frameN, FF->coeffN);
	    ERROR("but Scaled vector sub-header: %d frames, %d coefficients\n",
		  frameN, coeffN);
	    fmatrixFree(mat); return NULL;
	 }
	 fmatrixRefill( mat, frameR, coeffN, temp);
	 
	 break;
      }
    case 4:
      if ((frameR = checkRange(&from,&to,FF->frameN)) > 0) {
	int totalN = frameR * FF->coeffN; 
	mat        = fmatrixCreate( frameR, FF->coeffN);
	if (!mat) return NULL;

	fseek(FF->fp, (long)(from * FF->coeffN * sizeof(float)), SEEK_CUR);
	for (frameX = 0; frameX < frameR; frameX++)
	  ret += read_floats( FF->fp, mat->matPA[frameX], FF->coeffN);
	if (ret != totalN) {
	  ERROR("read only %d floats but wanted %d from file %s\n",
		ret, totalN, FF->filename);
	  fmatrixFree(mat); return NULL;
	}
      }
      else return NULL;
      break;
    default: ;
   }
   /* --- check file length (check for eof) --- */
   if (frameR == FF->frameN && checkFileLength(FF->fp)) {
      ERROR("file length was not ok: %s\n",FF->filename);
      fmatrixFree(mat); return NULL;
   }
   return mat;
}

/* ---------------------------------------------------------
   fileLength
   --------------------------------------------------------- */
static long fileLength(FILE *fp)
{
   long current = ftell(fp);
   long length;

   fseek(fp,0L,SEEK_END);   length = ftell(fp);
   fseek(fp,current,SEEK_SET);
   return length;
}

/* ---------------------------------------------------------
   print header info
   --------------------------------------------------------- */
static void printHeader(FeatureFile *FF)
{
   int i;
   printf("filename:      %s\n",FF->filename);
   printf("frames:        %d\n",FF->frameN);
   printf("coefficients:  %d\n",FF->coeffN);
   printf("sampling rate: %.0f kHz\n",FF->samplingRate);
   printf("byte mode:     %d\n",FF->byteMode);
   printf("features:      %d\n",FF->featN);
   for (i=0;i<FF->featN;i++)
      printf("%3d ..%3d %s\n",FF->fe[i].from,FF->fe[i].to,FF->fe[i].name);
}


/* ---------------------------------------------------------
   read RAW header
   --------------------------------------------------------- */
static int readRawHeader(FeatureFile *FF)
{
#define RHERR "\n\nReading RAW header of feature file:\n"
#define BuffMax 256
#define ReadLine(NR)  if (readLine(FF->fp,line,BuffMax)) {ERROR("error %s reading Header\n",#NR); return TCL_ERROR; }
   char line[BuffMax];
   char work[BuffMax];
   
   if ((readLine(FF->fp,line,BuffMax) == -1) || strneq(line,"RAW")) {
     if (!FF->coeffN) SWARN(RHERR "line: >>%s<<\nmagic 'RAW' not found in header\n\n",line);
     rewind(FF->fp); return TCL_ERROR;
   }
   
   ReadLine(1);
   if (sscanf(line,"%s%d",work,&(FF->frameN)) != 2 || strneq(work,"frames")){
      ERROR(RHERR "line: >>%s<<\nexpected: frames <value>\n\n",line);
      return TCL_ERROR; 
   }
   
   ReadLine(2);
   if (sscanf(line,"%s%d",work,&(FF->coeffN)) != 2 || strneq(work,"dimension")){
      ERROR(RHERR "line: >>%s<<\nexpected: dimension <value>\n\n",line);
      return TCL_ERROR; 
   }
   
   ReadLine(3);
   sscanf(line,"%s",work);
   Switch (work)
      Case("floatbytes")      {FF->byteMode = 1;}
      Case("scaled_vectors")  {FF->byteMode = 2;}
      Case("floats")          {FF->byteMode = 4;}
      Default {
	 ERROR(RHERR "line: %s\nexpected byte mode:\n"
	       "\"floatbytes\", \"scaled_vectors\" or \"floats\"\n\n",line);
	 return TCL_ERROR; 
      }
   End_switch;

   ReadLine(4);
   while(line[0] != '{') {
      float value;
      if (sscanf(line,"%s%f",work,&value) != 2) {
	 ERROR(RHERR "line: >>%s<<\nexpected: <variable> <value>\n\n",line);
	 return TCL_ERROR; 
      }
      Switch (work)
	 Case("sampling_rate")   {FF->samplingRate = value;}
         Case("shift")           {FF->shift        = value;}
         Case("snr")             {FF->snr          = value;}
         Default {
	    WARN(RHERR "don't know: %s\nwon't use it\n\n",line);
	 }
      End_switch;
      ReadLine(5);
   }
   if (strlen(line)!=1 || line[0]!='{'){
      ERROR(RHERR "missing '{'\n\n",line);
      return TCL_ERROR;
   }
   
   ReadLine(6);
   while(line[0] != '}' && FF->featN <= FEATN_MAX) {
      FeatureEntry *fe = FF->fe + FF->featN;

      if (sscanf(line,"%s %d %d", work, &(fe->from), &(fe->to)) != 3) {
	 ERROR(RHERR "line: >>%s<<\nexpected: <feature name> <from> <to>\n\n",
	       line);
	 return TCL_ERROR; 
      }
      if (fe->from < 0 || fe->from >= FF->coeffN ||
	  fe->to   < 0 || fe->to   >= FF->coeffN) {
	ERROR(RHERR "line: >>%s<<\n<feature name> <from> <to>\n"
	      "<to> or <from> out of range 0..%d\n\n"
	      ,line, FF->coeffN - 1);
	return TCL_ERROR; 
      }
      if (fe->name) free(fe->name);
      fe->name = strdup(work);
      FF->featN++;
      ReadLine(7);
   }

   ReadLine(8);
   while(strneq(line,"end_header")) {
      INFO(RHERR ">>%s\n\n",line);
      ReadLine(9);
   }
   return TCL_OK;
}

static int readHtkHeader(FeatureFile *FF)
{
   int val;short int val2;
   FF->byteMode = 4;
   fread(&(FF->frameN), sizeof(int), 1, FF->fp);
   fread(&val, sizeof(int), 1, FF->fp);
   fread(&(FF->coeffN), sizeof(short int), 1, FF->fp);
   FF->coeffN /= (sizeof(float));
   fread(&val2, sizeof(short int), 1, FF->fp);
   printf("nSamples = %d and DIM = %d\n", FF->frameN, FF->coeffN);
   return TCL_OK;
}
