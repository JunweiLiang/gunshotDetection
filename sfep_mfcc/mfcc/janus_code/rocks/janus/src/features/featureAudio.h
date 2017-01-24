/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: audio
               ------------------------------------------------------------

    Author  :  Michael Meyer
    Module  :  featureAudio.h
    Date    :  $Id: featureAudio.h 2864 2009-02-16 21:18:17Z metze $
    Remarks :  This Modul is from the MS-TDNN code and adapted for Janus
               by Michael Meyer

   ========================================================================

    $Log$
    Revision 3.4  2007/03/02 13:06:36  fuegen
    added code for PORTAUDIO support (Florian Metze, Friedrich Faubel)

    Revision 3.3  2000/08/27 15:31:13  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.2.4.1  2000/08/25 22:19:43  jmcd
    Just checking.

    Revision 3.2  1997/07/09 17:42:22  westphal
    *** empty log message ***

  
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

extern int audio_init   ( int device, int micro_gain );
extern int audio_play   ( short* adcA, long nsamples, int rate);
extern int record_start ( int min_buf_size, int sampl_rate );
extern int record_get   ( short* adcA, int max_adcN );
extern int record_stop  ( void );


#ifdef PORTAUDIO
/* -------------------------------------------------
    PORTAUDIO Code
   ----------------
    This provides fast and convenient access to
    a portable sound system
   ------------------------------------------------- */

#include "feature.h"

typedef short SAMPLE;

enum
{
    PA_INPUT_DEVICE   = 0,
    PA_OUTPUT_DEVICE  = 1
};
/*
#define PA_INPUT    0x0001
#define 
*/
struct
{
  unsigned long        nChannels;       /* number of channels */
  unsigned long        bufferLen;       /* buffer length in samples(=frames in PA-notation) */
  SAMPLE**             data;            /* data-buffers (1 per channel) */
  unsigned long        readPos;         /* sample(frame) read position in buffer */
  unsigned long        writePos;        /* sample(frame) write position in buffer */
  int                  overrun;         /* indicates a buffer overrun */
  unsigned long        nSamplesSkipped; /* number of samples skipped due to overruns */
} typedef PaBuffer;


struct PaInstance_S
{
  FeatureSet*           featureSet;
  char**                featureNames;
  PaBuffer*             paBuffer;

  FILE*                 file;
  char*                 fname;

  PaStream*             stream;
  PaStreamParameters*   inputStreamParams;
  PaStreamParameters*   outputStreamParams;

  double                samplingRate;
};

int     paStatus(FeatureSet*);
int     paListDevices(FeatureSet*, int);
int     paDefaultDevice(FeatureSet*, int);
double  paCurrentSamplingRate(FeatureSet*);
int     paStartRecording(FeatureSet*, int, int, double, float, char**, char*);
int     paPlay(FeatureSet*, int, char*, int, int);
int     paStop(FeatureSet*);
int     paGet(FeatureSet*, float*);

#endif

#ifdef __cplusplus
}
#endif
