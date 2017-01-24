/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Feature Audio Module
               ------------------------------------------------------------

    Author  :  Michael Meyer, Martin Westphal
    Module  :  featureAudioItf.c
    Date    :  $Id: featureAudioItf.c 2864 2009-02-16 21:18:17Z metze $
    Remarks :  audio methods for FeatureSet

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

    $Log$
    Revision 3.5  2007/03/02 13:06:36  fuegen
    added code for PORTAUDIO support (Florian Metze, Friedrich Faubel)

    Revision 3.4  2003/08/14 11:19:55  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.3.4.1  2002/11/21 17:09:42  fuegen
    windows code cleaning

    Revision 3.3  2001/01/15 09:49:56  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 3.2.36.1  2001/01/12 15:16:52  janus
    necessary changes for running janus under WINDOWS

    Revision 3.2  1997/07/09 17:42:22  westphal
    *** empty log message ***

  
   ======================================================================== */

/* ------------------------------------------------------------------------
   includes
   ------------------------------------------------------------------------ */
#include "featureMethodItf.h"          /* featureSet definitions            */
#include "featureAudio.h"              /* audio function prototypes         */

#define REC_FEAT "name of the new (recorded) feature"

/* ========================================================================
   Method 'play'
   plays feature on GRADIENT on HP
   (it's working only on HP so its commented out for the moment)
   ======================================================================== */

int fesPlayItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet *FS = (FeatureSet*)cd;
  Feature    *source = NULL;
  float      sr      = FS->samplingRate;   
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<src_feature>", ARGV_CUSTOM, getFe, &source, cd, "feature to play",
    "-sr",           ARGV_FLOAT,  NULL,  &sr,     cd, "sampling rate in kHz",
    NULL) != TCL_OK) return TCL_ERROR;

  if (sr <= 0.0) sr = FS->samplingRate;

  if (!IS_SVECTOR(source)) {
    MUST_SVECTOR(source); return TCL_ERROR;
  } else {
    SVector *svec = source->data.svec;
    audio_play( svec->vecA, (long)svec->n, (int)(1000 * ROUND(sr)));
  }
  return TCL_OK;
}

/* ========================================================================
   Method 'GetRecord'
   read samples into the buffer or
   stop sampling process
   ======================================================================== */

int fesGetRecordItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet *FS = (FeatureSet*)cd;
  Feature    *destin = NULL;
  int        stop    = 0;
  int        max_adcN= 100000;
  int audio_device = 2;   /*2=gradient 8=AF*/

  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<feature>", ARGV_CUSTOM, getFe, &destin,       cd, REC_FEAT,
    "-stop",     ARGV_INT,    NULL,  &stop,         cd, "stop recording",
    "-device",   ARGV_INT,    NULL,  &audio_device, cd, "audio device",
    NULL) != TCL_OK) return TCL_ERROR;

  if (IS_SVECTOR(destin)) {
    SVector* vec = svectorCreate(max_adcN);
    if (!vec) return TCL_ERROR;

    if (stop) {
      fprintf(stderr,"Stop recording.\n"); 
      record_stop();
    }

    if ((vec->n = record_get(vec->vecA,max_adcN))<0) { 
      ERROR("recording failed!\n");
      vec->n = 0; svectorFree(vec); return TCL_ERROR;
    } else {
      int start;
      int runon = FS->runon;
      SVector* old = destin->data.svec;
      SVector* newf;

      /* to avoid an assertion warning I set the runon mode */
      /* and check the trans value here                     */
      if (FS->trans == destin->trans)
	FS->trans++;
      FS->runon=1; 
      newf = svectorFeature((old->n)+(vec->n),FS,destin,&start); 
      
      FS->runon=runon;  /* old value */
      fprintf(stderr,"got %d samples. (old=%d)\n",vec->n,old->n);
      if (!newf) {
	svectorFree(vec);
	return TCL_ERROR; 
      }
      assert(old->n==start);  /* start always zero ??? */
      if (vec->n) memcpy( newf->vecA + old->n , vec->vecA, sizeof(short) * vec->n);
      svectorFree(vec);
      if (!feSVector( FS, destin, newf, 1)) return TCL_OK;
    }
    return TCL_ERROR;
  }
  ERROR("Feature not a SVector.\n");
  return TCL_ERROR;
  
}

/* ========================================================================
   Method 'RecordStart'
   starts the sampling process 
   (to halt the sampling use RecordGet -stop 1)
   ======================================================================== */

int fesRecordStartItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet *FS = (FeatureSet*)cd;
  Feature    *destin = NULL;
  SVector    *vec = svectorCreate(0);
  float      f_samplingRate    = FS->samplingRate ; /* sampling rate */   
  int        i_samplingRate    = 16000; 
  int        min_buf_size    = 1000;

  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<feature>", ARGV_CUSTOM, createFe, &destin,         cd, REC_FEAT,
    "-sr",       ARGV_FLOAT,  NULL,     &i_samplingRate, cd, "sampling rate in HZ",
    NULL) != TCL_OK) return TCL_ERROR;

  if (i_samplingRate<=0||i_samplingRate>16000 ) {
    WARN("Use '-sr <rate in Hz>' to set sampling rate\n");
    i_samplingRate=16000;
  }
  
  switch  (i_samplingRate) {
  case 8000:
    f_samplingRate=8.0;
    break;
  default:
    f_samplingRate=16.0;
    break;
  }
  destin->samplingRate = f_samplingRate;
  feSVector( FS, destin, vec, 1); 
  record_start(min_buf_size,i_samplingRate);
  fprintf (stderr, "start recording.\n");
  return TCL_OK;
  
}
  
/* ========================================================================
   Method 'AudioInit'
   initialize the audio device
   ======================================================================== */
int fesAudioInitItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet *FS = (FeatureSet*)cd;
  float      samplingRate    = FS->samplingRate ; /* sampling rate */   
  int        dev = 2;    /* 2 = GRADIENT
                            8 = AF            */
  int micro_gain = 150;   /* loudness */

  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "-sr",    ARGV_FLOAT,NULL, &samplingRate, cd, "sampling rate",
    "-gain",  ARGV_INT,  NULL, &micro_gain,   cd, "microphon gain",
    NULL) != TCL_OK) return TCL_ERROR;

  if (samplingRate<=0) {
    ERROR("Use '-sr <rate in kHz>' to set sampling rate\n");
    return TCL_ERROR;
  }
  
  if (audio_init(dev,micro_gain)==FALSE){
    ERROR("Can't init audio device.\n");
    return TCL_ERROR;
  }
  return TCL_OK;
}



/* -------------------------------------------------
    PORTAUDIO Code
   ----------------
    This provides fast and convenient access to
    a portable sound system
   ------------------------------------------------- */
#ifdef PORTAUDIO

/*****************************************************************************/
/* fesPAListDevicesItf                                                       */
/*****************************************************************************/
int fesPAInfoItf(ClientData cd, int argc, char *argv[])
{
  FeatureSet      *FS            = (FeatureSet*) cd;
  char*           option = NULL;
  
  argc--;
  if (itfParseArgv (argv[0], &argc, argv+1, 0,
      "<option>",     ARGV_STRING,  NULL, &option,   cd, "one of {\"devicesIn/Out\", \"defaultIn/Out\", \"status\", \"?\"}",
    NULL) != TCL_OK) return TCL_ERROR;

  if (option==NULL)
    return(TCL_ERROR);
  if (!strcmp(option, "-help") || !strcmp(option, "help") || !strcmp(option, "?"))
  {
    itfAppendResult("Options:\n");
    itfAppendResult("  devicesIn  : list input devices\n");
    itfAppendResult("  devicesOut : list output devices\n");
    itfAppendResult("  defaultIn  : returns default input device parameters\n");
    itfAppendResult("  defaultOut : returns default output device parameters\n");
    itfAppendResult("  status     : current status\n");
    return(TCL_OK);
  }

  if (strcasecmp(option, "devicesIn")==0)
    return(paListDevices(FS,0));
  if (strcasecmp(option, "devicesOut")==0)
    return(paListDevices(FS,1));
  if (strcasecmp(option, "defaultIn")==0)
    return(paDefaultDevice(FS, 0));
  if (strcasecmp(option, "defaultOut")==0)
    return(paDefaultDevice(FS, 1));
  if (strcasecmp(option, "status")==0)
    return(paStatus(FS));

  ERROR("paInfo: unknown option [%s].\n", option);
  return(TCL_ERROR);
}



/*****************************************************************************/
/* fesPAStartItf                                                             */
/*****************************************************************************/
int fesPAStartRecordingItf(ClientData cd, int argc, char *argv[])
{
  FeatureSet*     featureSet          = (FeatureSet*) cd;
  Feature**       features            = NULL;
  float           samplingRate        = 0;
  float           bufferLenInSeconds  = 60;
  int             deviceID            = -1;
  int             nChannels           = 1;
  char*           name                = "ADC";
  char*           fname               = NULL;

  char**          featureNames;
  int             i, j, result;

  /*-----------*/
  /* parse ITF */
  /*-----------*/
  argc--;
  if (itfParseArgv (argv[0], &argc, argv+1, 0,
    "-device",      ARGV_INT,     NULL, &deviceID,            cd, "audio device number",
    "-sr",          ARGV_FLOAT,   NULL, &samplingRate,        cd, "sampling rate in kHz",
    "-chN",         ARGV_INT,     NULL, &nChannels,           cd, "number of channels",
    "-buf",         ARGV_FLOAT,   NULL, &bufferLenInSeconds,  cd, "buffer length (in seconds)",
    "-feature",     ARGV_STRING,  NULL, &name,                cd, "name of the feature(s)",
    "-file",        ARGV_STRING,  NULL, &fname,               cd, "name of the file to write (Windows and WAV only)",
    NULL) != TCL_OK) return TCL_ERROR;

  /*-----------------*/
  /* allocate memory */
  /*-----------------*/
  features = malloc(sizeof(Feature*)*nChannels);
  if (features==NULL)
  {
    ERROR("fesPAStartRecordingItf: memory allocation failed.");
    return(TCL_ERROR);
  }
  memset(features, 0, sizeof(Feature*)*nChannels);

  /*---------------------*/
  /* create featureNames */
  /*---------------------*/
  featureNames = (char**) malloc(nChannels*sizeof(char*));
  if (featureNames==NULL)
  {
    ERROR("fesPAStartRecordingItf: memory allocation failed.\n");
    return(TCL_ERROR);
  }
  for (i=0; i<nChannels; i++)
  {
    featureNames[i] = malloc((strlen(name)+20)*sizeof(char));
    if (featureNames[i]==NULL) {
      for (j=0; j<i; j++)
        free(featureNames[j]);
      free(featureNames);
      ERROR("fesPAStartRecordingItf: memory allocation failed.\n");
      return(TCL_ERROR);
    }
    sprintf(featureNames[i], "%s%d", name, (i+1));
  }

  /*-----------------*/
  /* start recording */
  /*-----------------*/
  result = paStartRecording (featureSet, deviceID, nChannels, samplingRate,
			     bufferLenInSeconds, featureNames, fname);

  /* if (result==TCL_OK)
    fprintf(stderr, "PortAudio: Recording started.\n"); */

  return(result);
}



/*****************************************************************************/
/* fesPAPlayItf                                                              */
/*****************************************************************************/
int fesPAPlayItf(ClientData cd, int argc, char *argv[])
{
  FeatureSet*     featureSet          = (FeatureSet*) cd;
  Feature**       features            = NULL;
  Feature*        feature             = NULL;
  int             deviceID            = -1;
  int             nChannels           = 1;
  char*           name                = "";
  int             from                = -1;
  int             to                  = -1;

  int             fIndex, result;
  int             startPos, endPos;

  /*-----------*/
  /* parse ITF */
  /*-----------*/
  argc--;
  if (itfParseArgv (argv[0], &argc, argv+1, 0,
    "<feature>",    ARGV_STRING,  NULL, &name,                cd, "name of the feature",
    "-device",      ARGV_INT,     NULL, &deviceID,            cd, "audio device number",
    "-from",        ARGV_INT,     NULL, &from,                cd, "from (sample no.)",
    "-to",          ARGV_INT,     NULL, &to,                  cd, "to (sample no.)",
    NULL) != TCL_OK) return TCL_ERROR;

  /*-----------------*/
  /* allocate memory */
  /*-----------------*/
  features = malloc(sizeof(Feature*)*nChannels);
  if (features==NULL)
  {
    ERROR("fesPAStartPlayingItf: memory allocation failed.\n");
    return(TCL_ERROR);
  }
  memset(features, 0, sizeof(Feature*)*nChannels);

  /*-------------------------------*/
  /* check that the feature exists */
  /*-------------------------------*/
  fIndex = fesGetIndex(featureSet, name, 0);
  if (fIndex<0)
  {
    ERROR("fesPAStartPlayingItf: feature[%s] does not exist!\n");
    return(TCL_ERROR);
  }
  feature = &(featureSet->featA[fIndex]);
  if (feature==NULL)
  {
    ERROR("fesPAStartPlayingItf: feature was NULL.\n");
    return(TCL_ERROR);
  }

  /*-----------------------*/
  /* get start and end pos */
  /*-----------------------*/
  startPos = 0;
  endPos = feature->data.svec->n;
  if (from>=0)
    startPos = from;
  if (to>=0)
    endPos = to;
  /* printf("[%i %i]\n", startPos, endPos); */
  
  /*-----------------*/
  /* start recording */
  /*-----------------*/
  result = paPlay(featureSet, deviceID, name, startPos, endPos);
  
  /* if (result==TCL_OK)
    fprintf(stderr, "PortAudio: Recording started.\n"); */

  return(result);

}



/*****************************************************************************/
/* fesPAStopItf                                                              */
/*****************************************************************************/
int fesPAStopItf(ClientData cd, int argc, char *argv[])
{
  int result;

  if (itfParseArgv (argv[0], &argc, argv+1, 0,
    NULL) != TCL_OK) return TCL_ERROR;

  result = paStop((FeatureSet*) cd);
  /* if (result==TCL_OK)
    fprintf(stderr, "PortAudio: Recording stopped.\n"); */
  return(result);
}



/*****************************************************************************/
/* fesPAGetRecordedItf                                                       */
/*                                                                           */
/* TODO: WRITE DOCU */
/*****************************************************************************/
int fesPAGetRecordedItf (ClientData cd, int argc, char *argv[])
{
  FeatureSet     *FS = (FeatureSet*) cd;
  float          t = 0;
  int            result;

  /*------------*/
  /* parse args */
  /*------------*/
  argc--;
  if (itfParseArgv (argv[0], &argc, argv+1, 0,
    NULL) != TCL_OK) return TCL_ERROR;

  /*----------------------------------------------------*/
  /* fetch the samples and return their time in seconds */
  /*----------------------------------------------------*/
  result = paGet(FS, &t);
  itfAppendElement ("%f", t);

  return(result);
}
#endif
