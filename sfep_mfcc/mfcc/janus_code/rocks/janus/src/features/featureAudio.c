/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Filter Type & Operators
               ------------------------------------------------------------

    Author  :  Michael Meyer, Hermann Hild 
    Module  :  featureAudio.c
    Date    :  $Id: featureAudio.c 3407 2011-03-22 19:14:46Z metze $

    Remarks :  audio functions
               see featureAudioItf.c for the methodItf functions

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

    $Log: featureAudio.c,v $
    Revision 1.1.1.2  2008/09/10 17:01:51  metze.florian
    FFa+FMe

    Revision 1.1.1.1.4.1  2008/09/09 09:59:40  metze.florian
    Cleanup

    Revision 1.1.1.1  2007/06/20 12:34:50  metze.florian
    Initial import of Ibis 5.1 from UKA at T-Labs

    Revision 3.4  2007/03/02 13:06:36  fuegen
    added code for PORTAUDIO support (Florian Metze, Friedrich Faubel)

    Revision 3.3  2001/01/15 09:49:56  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 3.2.36.1  2001/01/12 15:16:52  janus
    necessary changes for running janus under WINDOWS

    Revision 3.2  1997/07/09 17:42:22  westphal
    *** empty log message ***


   ======================================================================== */

#include "error.h"
#include "common.h"
#include "featureAudio.h"

#define ROUND(a)        (floor((a) + 0.5))
#define ERR0(a,b) ERROR(b)
#define ERR1(a,b,c) ;
#define WARN0(a) WARN(a)
#define WARN1(a,b) WARN(a,b)
#define WARN2(a,b,c) WARN(a,b,c)

#ifndef TRUE
#define FALSE 0
#define TRUE (!FALSE)
#endif


/* ------------------------------------------------------------------------
   variables

   DEVICE NUMBERS
     2 = GRADIENT  0000 0010  
     8 = AF        0000 1000
   ------------------------------------------------------------------------ */
static   int  audio_dev = 2;   

/* ------------------------------------------------------------------------
   prototypes for gradient functions
   ------------------------------------------------------------------------ */
#ifdef GRAD
int  init_AD_DA             ( void );
int  gradient_set_micro_gain( int new_gain );
int  gradient_set_vb        ( int new_vb );
int  gradient_print         ( void );
int  start_AD               ( long cache_size, int rate );
long get_more_AD            ( short* array, long num );
void stop_AD                ( void );
int  ready_DA               ( long cache_size, int rate);
long play_more_DA           ( short* array,  long num);
int  play_stop_DA           ( void);
int  wait_end_DA            ( void);
long sample_to_buf          ( short* adcA, long num, long rate );
int  play_buf               ( short* adcA, long nsamples, long rate);
int  play_buf2              ( short* adcA, long nsamples, long rate, int wait);
#endif

/* ------------------------------------------------------------------------
   prototypes for AF functions
   ------------------------------------------------------------------------ */
#ifdef AF
int  af_init_AD_DA  ( int channels );
int  af_start_AD    ( long cache_size, int rate, int starttime, int channels );
long af_get_more_AD ( short* array, long num );
int  af_stop_AD     ( void );
int  af_play_buf    ( short* array, long nsamples, long rate, int channels);
#endif

int play_buf( short* adcA, long nsamples, long rate);


/* ========================================================================

                                a u d i o . c

   Interface for Audio Hardware.
   The module provides an interface for audio device independent 
   recording/playing.
   Currently, the code for GRADIENT BOX and AF-AUDIO is implemented.
   The code for the GRADIENT BOX has been already tested on an HP.
   
   audio_init	initialize the audio device
   audio_play	play an audio buffer
   record_start	audio device starts recording
   record_get	get whatever has been recorded so far
   record_stop	stop recording

   (based on Hermann Hild, Nov 1996)
   ======================================================================== */

/* ------------------------------------------------------------------------
   audio_init
   
   IN: device number : 2 = gradient 
       micro_gain    : Lautstaerke??
   RETURN:	
       TRUE, if successful, FALSE else
  ------------------------------------------------------------------------ */
int audio_init(int device, int micro_gain)
{
  int ok = TRUE;
  audio_dev = device;
  
  if (audio_dev & 2) {
#ifdef GRAD
	if ((init_AD_DA()!= -1) && gradient_set_micro_gain(micro_gain));
	else ok = FALSE;
#else
	ok = FALSE;
	WARN2("audio_init: dev = %d, gain = %d, GRAD not compiled!\n", audio_dev, micro_gain);
#endif
  }

  if (audio_dev & 8) {
#ifdef AF
	ok = (0 == af_init_AD_DA(1));	         /* init with channel 1	*/
	ok = ok && (0 == af_set_channels(1));
#else
	ok = FALSE;
	WARN2("audio_init: dev = %d, gain = %d, AF not compiled!\n", audio_dev, micro_gain);
#endif
  }
  return(ok);
}

/* ------------------------------------------------------------------------
   audio_play
   
   IN: adcA        : short buffer to play
       nsamples    : number of samples
       rate        : sampling rate in Hz
   RETURN:	
       TRUE, if successful, FALSE else
  ------------------------------------------------------------------------ */
int audio_play( short* adcA, long nsamples, int rate)
{
  int ok = TRUE;

  if (adcA == NULL || nsamples == 0) return(ok);
  if (rate <= 0) {
    WARN1("audio_play: sampling rate should be > 0 but is %d\n", rate);
    return(FALSE);
  }
  if (audio_dev & 2) {
#ifdef GRAD
    if ( play_buf( adcA, nsamples, (long)rate) < 0 ) {
      ok = FALSE;
      WARN0("audio_play: Couldn't play audio buffer (Gradient)\n");
    }
#else
    ok = FALSE;
    WARN0("audio_play: dev = 2, GRAD not compiled!\n");
#endif
  }
  if (audio_dev & 8) {
#ifdef AF
    if ( af_play_buf( adcA, nsamples, (long)rate, 1) < 0 ) {
      ok = FALSE;
      WARN0("audio_play: Couldn't play audio buffer (AF)\n");
    }
#else
    ok = FALSE;
    WARN0("audio_play: dev = 8, AF not compiled!\n");
#endif
  }
  return(ok);
}
/* ------------------------------------------------------------------------
   record_ADC_start 
  ------------------------------------------------------------------------ */
int record_start(int min_buf_size, int rate)
{
  int  ok = TRUE;

  if (rate <= 0) {
    ERR1(FALSE, "record_start: sampling rate should be > 0 but is %d\n", rate);
    return(FALSE);
  }
  if (min_buf_size <= 0) {
    ERR1(FALSE, "record_start: buffer size should be > 0 but is %d\n", min_buf_size);
    return(FALSE);
  }
  switch (audio_dev) {
#ifdef GRAD
     case 2:  	{
		short buf[100];
       		sample_to_buf(buf, 10, rate); /* Stupid HACK to ensure sampling rate is correct*/
     		ok = (0 == start_AD((long) min_buf_size, rate));
	        }
		break;
#endif
#ifdef AF
     case 8:  	{
       		int channels = 1;						/* one channel	*/
		int starttime = 0;
     		ok = (0 == af_start_AD((long) min_buf_size, rate, starttime, channels));	
	        }
		break;
#endif
     default:   ERR1(FALSE, "play_ADC: unkown audio dev (%d)\n", audio_dev);
  }
  if (!ok) ERR0(FALSE, "record_ADC_start Failed\n");
  return(TRUE);
}


/* ------------------------------------------------------------------------
  record_ADC_get

  IN:		
     adcA       :  buffer to write samples
     max_adcN   :  Max number of samples to return

  RETURN:	-1 if there was a problem, or number of samples returned.
  Hermann Hild, Nov 1996
  ------------------------------------------------------------------------ */
int record_get(short* adcA, int max_adcN)
{
  int  ok   = TRUE;
  int  adcN = 0;

  if (adcA == NULL || max_adcN <= 0) return(-1);
  switch (audio_dev) {
#ifdef GRAD
     case 2:  	
		adcN = get_more_AD(adcA, (long) max_adcN);
		break;
#endif
#ifdef AF
     case 8:  	
		adcN = af_get_more_AD(adcA, (long) max_adcN);
		break;
#endif
     default:   ERR1(-1, "play_ADC: unkown audio dev (%d)\n", audio_dev);
  }
  if (!ok) ERR0(-1, "record_ADC_start Failed\n");
  return(adcN);
}

/* ------------------------------------------------------------------------
   record_ADC_start 
  ------------------------------------------------------------------------ */
int record_stop(void)
{
  switch (audio_dev) {
     case 1:	break;
#ifdef GRAD
     case 2:  	stop_AD();  break;
#endif
#ifdef AF
     case 8:	af_stop_AD(); 
		break;
#endif
     default:   ERR1(FALSE, "play_ADC: unkown audio dev (%d)\n", audio_dev);
  }

  return(TRUE);
}


#ifdef GRAD
/*****************************************************************************
; gradient.c:  Routines to use Gradient DeskLab for recording and playback
;
; Usage: in janus_record.c
;
; Routines:
;		int  init_AD_DA();		     (initialize gradient box)
;
;		int  start_AD(cachesize,rate); 	     (routine starts recording)
;		long get_more_AD(array, num); 	     (gets num recorded data)
;		int  stop_AD();			     (stops recording now)
;
;		int  ready_DA(cachesize,rate);       (routine readies play)
;		long play_more_DA(array, num);	     (plays num recorded data)
;		int  wait_end_DA();		     (waits for end playback)
;
;		long sample_to_buf(array,num,rate);  (records to a buffer)
;		int  play_buf(array, num, rate);     (plays a buffer)
;
;		void  set_gain(new_gain);   	     (sets gain)
;
; Assumes:
;		Environment var GDL_DEVICE is set correctly.
;		Path to gradient bin directory is set in shell.
;
; History:
;   21.Feb.92 	arthurem copied from my own library of my routines.
;    3.Apr.92 	arthurem modified to remove specificity of routine names
;    4.May.92 	arthurem added routines for segment recording
;   21.May.92 	arthurem added routines for segment playing
;    1.Jun.93 	hhild moved global fct def into gradient.h and local ones into grandient.c
;   27.Oct.93 	hhild 	changed to ANSI headers,
;			modified error messages, more details and everything goes on stderr
;   14 Apr 94	hhild 	added gradient_set_vb, removed little butin init_AD_DA, changed
;			parameters in call to gplay_buf (program crashed with old parameters)			
;
;
;  Revision 1.9  96/01/12  10:07:17  10:07:17  hhild (Hermann Hild)
;  added play_stop_DA
;  
;  Revision 1.8  95/08/23  10:44:28  10:44:28  hhild (Hermann Hild)
;  added header prototypes for ANSI C
;  
;  Revision 1.7  1995/06/02  12:37:48  hhild
;  added dial number
;
;  Revision 1.6  1995/04/10  08:54:56  hhild
;   microphone and telephone recording need two different gains. I changed the global
;  variable gain to phone_gain and micro_gain, which can be set individually.
;  (the old function set_gain is still available for backward compatibility)
;
;  Revision 1.5  1994/12/19  16:42:27  hhild
;  added copyright notice
;
;  Revision 1.4  94/08/18  12:19:39  12:19:39  hhild (Hermann Hild)
;  added getphone_status function
;  moved status definitions into header file
;  
;  Revision 1.3  1994/08/12  18:37:44  hhild
;  added play_buf2 function. Same as play_buf(), but with addtional 'wait' argument
;
;  Revision 1.2  1994/08/09  13:01:46  hhild
;  Included Martings functions for Handling Telephone Calls
;
;  Revision 1.1  94/04/14  14:11:00  14:11:00  janus (Janus Project Account)
;  Initial revision
;  
;
############################################################################################################################*/

				/*----------  HANDLE  PROTOTYPES ---------------*/	
#ifdef  __STDC__		/*   Are we compiling on an ANSI C compiler ?   */	
#define Fargs(x)  x		/*   if so, make use of all type information    */	
#else				/*   else its stupid NON-ANSI C                 */	
#define Fargs(x) ( )		/*   we have to throw away type information     */	
#endif				/*----------------------------------------------*/	

/*-------------------------------------------------------------
;   Header to define some local  gradient routines 
;   There are more header definitions in the gradient.h file
--------------------------------------------------------------*/

int 	gparams			Fargs((int gd, int rec_sampl, int rec_cutoff_freq, int rec_highpass,
					int rec_grain, int play_rate, int play_cutoff_freq, int play_sinx_filter));
int 	gparams_auto		Fargs((int, int, int, int));
int 	g_gain			Fargs((int gd, int rec_grain));

void 	gphone_hookswitch	Fargs((int gd, int onhook));
void 	gphone_auto		Fargs((int gd, int answer, int hangup));
int 	gphone_status		Fargs((int gd, int* lastatus));
void 	gphone_dtmf_gen		Fargs((int gd, char* number, int wait));
void 	gphone_dtmf_stop	Fargs((int gd));
void 	gphone_dtmf_recv	Fargs((int gd, char* touchtones, int clear));
void 	gphone_dtmf_tone	Fargs((int gd, int length, int sil, int pause));
void 	gphone_dtmf_clear	Fargs((int gd));
void 	gphone_ringset		Fargs((int gd, int length, int silence, int notify));
void 	gphone_info		Fargs((int gd,  void*  info_report));	/* struct gphone_info	*/

int 	gload_buf		Fargs((int gd, short* buf, long len, int force));
int 	gload_file		Fargs((int gd, char* fname, int force));
void 	gload_reset		Fargs((int gd));

int 	gserver_start		Fargs((char* dev_name));
int 	gserver_connect		Fargs((char* dev_name));
void 	gserver_disc		Fargs((int gd));
void 	gserver_stop		Fargs((int gd));

long 	gplay_buf		Fargs((int gd, short* buf,  long length, long timlim, int keystop, long la, int wait));
long 	gplay_file		Fargs((int gd, char* fname, long length, long timlim, int keystop, long la, int wait, 	
						int head));
long 	gplay_fd		Fargs((int gd, int fd, long length, long timlim, int keystop, long la, int wait, 
						int head));
long 	gplay_seg		Fargs((int gd, short* buf, long len));
void 	gplay_wait		Fargs((int gd));
void 	gplay_stop		Fargs((int gd));


int 	gpartition		Fargs((int gd, long rec_part, long rec_transf, long preload_part, long play_part, 
					long play_transf));
void 	grechannel		Fargs((void));

/*--------------------------------------------------------
;  Functions from the gradient library
;-------------------------------------------------------*/
long	grecord_seg		Fargs((int gd, short* buffer, long length));
void	grecord_segend		Fargs((int gd));
void 	grecord_segstart	Fargs((int gd));
void	grecord_stop		Fargs((int gd));

long 	grecord_buf		Fargs((int gd, short* adcA, long num, int limit, int newline));
long 	grecord_file		Fargs((int gd, char* fname, long lenlim, long timelim, int keystop, int headertype));
long 	grecord_fd		Fargs((int gd, int fd,      long lenlim, long timelim, int keystop, int headertype));
long 	grecord_auto		Fargs((int gd, short* adcA, long num, int limit, int newline));
long 	grecord_auto_fd		Fargs((int gd, int fd,      long num, int limit, int newline));


/*--------------------------------------------------------------------------*/
static	int 	gd 		= 0;			/* gradient box descriptor     		*/
static	int 	micro_gain 	= 30;			/* default gain setting for microphone 	*/
static	int 	vb 		= 0;			/* default verbosity setting    	*/
static	int	phone_mode	= 0; 			/* are we in phone mode?		*/
static	int	phone_gain	= 10; 			/* gain for telephone			*/
/*--------------------------------------------------------------------------*/


/*===========================================================================
;  init_AD_DA
;
;  Initializes the gradient box.
;
;  RETURN:	0, if successfull, -1 else
;
;  HISTORY:
;	11 Apr 94	hhild	removed small bug (local variable error).
;--------------------------------------------------------------------------*/
int init_AD_DA()
{
  int ok = TRUE;

  gd = gserver_start("");		/* connects to a running server or starts server using env-var GDL_DEVICE */
  ok = (gd != -1);			/* start failed if -1 was returned	*/

  if (ok) 
  {
    gserver_stop(gd);			/* stop it and init a second time  why?	*/
    gd = gserver_start("");
    ok = (gd != -1);
  }

  fprintf(stderr, "init_AD_DA: Initializing Gradient Box (gd = %d)\n", gd);
  if (!ok) 
    fprintf(stderr, "ERROR: Could not connect to Gradient box\n");
  
  return(ok?0:-1);
}
/*--------------------------------------------------------------------------*/

int gradient_set_micro_gain(int new_gain)
{
  int ok = (g_gain(gd, new_gain) != -1);
  if (!ok) 
    fprintf(stderr, "gradient_set_micro_gain: FAILED (new gain was %d)\n", new_gain);

  if (vb >= 1) printf("Gradient: setting MICRO gain from %d to %d\n", micro_gain, new_gain);
  micro_gain = new_gain;
  return(ok);
}

int gradient_set_vb(int new_vb)
{
  if (vb >= 1) printf("Gradient: setting vb from %d to %d\n", vb, new_vb);
  vb = new_vb;
  return(1);
}

int gradient_print()
{
   fprintf(stderr, "GRADIENT BOX SETUP: \n");
   fprintf(stderr, "gd          = %3d\n", gd);
   fprintf(stderr, "micro_gain  = %3d\n", micro_gain);
   fprintf(stderr, "vb          = %3d\n", vb);
   return(TRUE);
}
/*--------------------------------------------------------------------------*/

/*===========================================================================
;  start_AD
;
;  IN:
;
;	cache_size	 minimum amount of recorded data to transfer 
;	rate		 recording rate in Hz 
;--------------------------------------------------------------------------*/
int start_AD(long cache_size, int rate)
{
  int error = 0;		
  int gain = phone_mode? phone_gain : micro_gain;		/* gain depends on micro/phone mode !	*/

  /* See if we're hooked up to Gradient yet, if not, do */
  if (gd == 0)
  {
    int flag = init_AD_DA();
    if (flag != 0)
    {
      fprintf(stderr, "ERROR: failed to initialize gradient\n");
      return(-1);
    }
  }

  /* Set desired parameters for Gradient piecewise recording */
  error = gparams(    	     	/* setup A-D and D-A parameters */
      	    gd,	     		/* gradient descriptor */ 
      	    rate,   		/* record sampling rate */
      	    0, 	     		/* cutoff freq (0 means take default) */
      	    FALSE,    		/* highpass filter used (for phone line)? */
      	    gain,     		/* gain */
      	    -1,   		/* play sampling rate */
      	    0, 	     		/* cutoff freq (0 means take default) */
      	    FALSE);   		/* play_sinx_filter? */
  if (error == -1) return(-1);

  error = gpartition(gd,		/* gradient descriptor */
		     -1,		/* take default record partition */
		     cache_size,	/* record transfer size per call */
		     -1,		/* take default preload partition */
		     -1,		/* take default play partition */
		     -1);		/* take default play transfer size */
  if (error == -1) return(-1);

  /* Start recording now */
  grecord_segstart(gd);
  
  return(0);
}
/*--------------------------------------------------------------------------*/

/*================================================================================
;  get_more_AD
;
; IN:
; 	array		 buffer into which to copy recorded data 
;	num	       	 number of samples desired to be copied 
;
;--------------------------------------------------------------------------------*/
long get_more_AD(short* array, long num)
{
  long count;		/* number of samples actually copied */

  count = grecord_seg(gd, array, num);

  if (count == -1)
  {
    fprintf(stderr, "ERROR:get_more_AD: problem with grecord_seg command.(max=%ld, gd=%d)\n",num,gd);
    return(-1);
  }

  return(count);
}
/*--------------------------------------------------------------------------*/


void stop_AD()
{
  grecord_segend(gd);
}
/*--------------------------------------------------------------------------*/

/*================================================================================
;  ready_DA
;
; IN:
;
;	cache_size	maximum amount of samples to be played 
;	rate		playback rate in Hz 
;--------------------------------------------------------------------------------*/
int ready_DA(long cache_size, int rate)
{
  int error;

  /* See if we're hooked up to Gradient yet, if not, do */
  if (gd == 0)
  {
    int flag = init_AD_DA();
    if (flag != 0)
    {
      fprintf(stderr, "ERROR: failed to initialize gradient\n");
      return(-1);
    }
  }

  /* Set correct parameters for piecewise playback */
  error = gparams(    	     	/* setup A-D and D-A parameters */
      	    gd,	     		/* gradient descriptor */ 
      	    -1,   		/* record sampling rate unchanged */
      	    0, 	     		/* cutoff freq (0 means take default) */
      	    FALSE,    		/* highpass filter used (for phone line)? */
      	    -1,     		/* gain unchanged */
      	    rate,   		/* play sampling rate */
      	    0, 	     		/* cutoff freq (0 means take default) */
      	    FALSE);   		/* play_sinx_filter? */

  if (error == -1) return(-1);

  error = gpartition(gd,	/* gradient descriptor */
		     -1,	/* set record partition */
		     -1,	/* default record transfer size per call */
		     -1,	/* take default preload partition */
		     -1,	/* set play partition large enough */
		     cache_size);/* set play transfer size */
  if (error == -1) return(-1);

  return(0);

}
/*--------------------------------------------------------------------------*/

/*================================================================================
;  play_more_DA
;
; IN:
;	array	array of samples to be loaded for playing
;	num	 max number of samples that can be loaded
;--------------------------------------------------------------------------------*/
long play_more_DA( short* array,  long num)
{
  long count= gplay_seg(gd, array, num);		/* number of samples actually copied */

  if (count == -1)
  {
    fprintf(stderr, "ERROR:play_more_DA: problem with gplay_seg command (adcN = %ld).\n", num);
    return(-1);
  }
  return(count);
}
/*--------------------------------------------------------------------------*/



/*================================================================================
;  play_stop_DA
;
; IN:
;	array	array of samples to be loaded for playing
;	num	 max number of samples that can be loaded
;--------------------------------------------------------------------------------*/
int play_stop_DA(void)
{
  gplay_stop(gd);
  return(1);
}
/*--------------------------------------------------------------------------*/



/*================================================================================
;  wait_end_DA
;
; ???????
;--------------------------------------------------------------------------------*/
int  wait_end_DA(void)
{
  gplay_wait(gd);
  return(0);
}
/*--------------------------------------------------------------------------*/

/*================================================================================
;  sample_to_buf
;
; IN:
;	array		array to copy data into
;	num		maximum amount to record to this buffer
;	rate		sampling rate for this record (in Hz)
;--------------------------------------------------------------------------------*/
long sample_to_buf(short* adcA, long num, long rate)
{ 
  int nsamples;				/* number of samples collected */
  int error;
  int gain 	= phone_mode? phone_gain : micro_gain;		/* gain depends on micro/phone mode !	*/

  /* See if we're hooked up to Gradient yet, if not, do */
  if (gd == 0)
  {
    int flag = init_AD_DA();
    if (flag != 0)
    {
      fprintf(stderr, "ERROR: failed to initialize gradient\n");
      return(-1);
    }
  }

  /* Set desired parameters for recording to buffer */
  error = gparams(    	     	/* setup A-D and D-A parameters */
      	    gd,	     		/* gradient descriptor */ 
      	    rate,   		/* record sampling rate */
      	    0, 	     		/* cutoff freq (0 means take default) */
      	    FALSE,    		/* highpass filter used (for phone line)? */
      	    gain,     		/* gain */
      	    -1,   		/* play sampling rate unchanged */
      	    0, 	     		/* cutoff freq (0 means take default) */
      	    FALSE);   		/* play_sinx_filter? */
  if (error == -1) return(-1);


  /* Record from Gradient into a buffer */
  nsamples = grecord_buf(
     	    gd,	     		/* gradient descriptor */
      	    adcA,		/* start of buffer */
      	    num,		/* length limit */
      	    0,	     		/* time limit in seconds (0 = no limit) */
      	    TRUE);    		/* newline will stop recording */

  return(nsamples);
}
/*--------------------------------------------------------------------------*/


/*================================================================================
;  play_buf
;  play_buf2
;
; IN:
;	adcA		array to play
;	nsamples	number of points in adcA
;	rate		sampling rate for this record
;	wait		[only play_buf2]   gplay should wait until done
;
; RETURN:	-1 if there was a problem
;
; HISTORY
;	14 apr 94	hhild	introduced vb variable, modified gplay_buf parameters:
;				time limit	: 0 -> 20L,
;				stop on newline : TRUE -> FALSE;	
;				(Program was always crashing with old values )
;	12 Aug 94	hhild	added play_buf2 funtion
;--------------------------------------------------------------------------------*/
int play_buf( short* adcA, long nsamples, long rate)
{
  int	wait = 1;			/* wait until all is played	*/
  return play_buf2(adcA, nsamples, rate, wait);
}

 
int play_buf2( short* adcA, long nsamples, long rate, int wait)
{ 
  int error;

  /* See if we're hooked up to Gradient yet, if not, do */
  if (gd == 0)
  {
    int flag = init_AD_DA();
    if (flag != 0)
    {
      fprintf(stderr, "ERROR: failed to initialize gradient\n");
      return(-1);
    }
  }

 if (vb >= 1) 
   fprintf(stderr, "play_buf: PLAYING sample (n=%ld, rate=%ld)\n", nsamples, rate);


  /* Set correct parameters for playing from buffer */
  error = gparams(    	     	/* setup A-D and D-A parameters */
      	    gd,	     		/* gradient descriptor */ 
      	    -1,   		/* record sampling rate unchanged */
      	    0, 	     		/* cutoff freq (0 means take default) */
      	    FALSE,    		/* highpass filter used (for phone line)? */
      	    -1,     		/* gain unchanged */
      	    rate,   		/* play sampling rate */
      	    0, 	     		/* cutoff freq (0 means take default) */
      	    FALSE);   		/* play_sinx_filter? */

  if (error == -1) return(-1);

  /* Play from a buffer out through Gradient box */
  gplay_buf(
     	    gd,	     		/* gradient descriptor */
      	    adcA,		/* start of buffer */
      	    (long)nsamples,	/* length limit */
      	    20L,     		/* time limit in seconds (0 = no limit) */
      	    FALSE,    		/* newline will stop playing */
	    0,			/* number of samples to load ahead */
	    wait);		/* gplay should wait until done to return */
  return(0);
}
/*--------------------------------------------------------------------------*/

/*###												###*/
/*###												###*/
/*###												###*/
/*#################################################################################################*/
#endif


#ifdef AF
/*#################################################################################################*/
/*###												###*/
/*###			A F  -  A U D I O   F U N C T I O N S					###*/
/*###												###*/
/*###												###*/
/*###												###*/

/*****************************************************************************
; afaudio2.c:  Routines to use AudioFile Server for recording and playback of
;	      16 bit linear data in two channels.
;
; Usage: in all recording/playback programs
;
; Routines:
;	int  init_AD_DA(channels);	     (make audio connection)
;
;	int  start_AD(cachesize,rate,channels); (routine starts recording)
;	long get_more_AD(array, num); 	     (gets num recorded data)
;	int  stop_AD();			     (stops recording now)
;
;	int  ready_DA(cachesize,rate,channels); (routine readies play)
;	long play_more_DA(array, num);	     (plays num recorded data)
;	int  wait_end_DA();		     (waits for end playback)
;
;	long sample_to_buf(array,num,rate,channels);  (records to a buffer)
;	int  play_buf(array, num, rate,channels);     (plays a buffer)
;
;	int  set_gain(new_gain);   	     (sets gain (NULL for HP))
;	int  set_channels(channels);	     (sets number of channels)
;
; Assumes:
;		Environment variable AUDIOFILE is set to unix:0.0 or chosen server
;		An audiofile server is running where variable says...
;
; History:
;    15 Feb 1995 arthurem constructed to mimic gradient.c, hpaudio.c code
;    31 Aug 1995 arthurem modified to record and playback one or two channels
;           1996 hhild	  Modified for use in Karlsruhe
;    07 Nov 1996 hhild	  Modified for usage in Janus3
;		
*****************************************************************************/
/*--------------------------------------------------------------------------*/

#include <AF/AFlib.h>
#include <AF/AFUtils.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 			/* for the R_OK in file_exist	*/

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

static	AFAudioConn *aud;		/* the audio connection */
static	AFDeviceDescriptor *aDev;	/* a data structure with info about device */
static	ATime tPlay;			/* a time pointer for playback */
static	ATime tRecord;			/* a time pointer for recording */
static	AC MonoAC;			/* the audio context for one channel */
static	int MonoDev;			/* the audio device for one channel */
static	AC StereoAC;			/* the audio context for two channels */
static	int StereoDev;			/* the audio device for two channels */
static	AC ac = NULL;			/* the audio context used */
static	AFSetACAttributes attributes;	/* the attributes of an audio context */
static	unsigned int aRate;		/* sampling rate that this audio server supports */
static	unsigned int devPlayBufSize;	/* play buffer size in bytes */
static	unsigned int devRecBufSize;	/* recording buffer size in bytes */
static	unsigned int recMinSamples;	/* number of samples to wait for each time */
static	int Channels;			/* number of channels to play/record */

#define	WHOAMI		"af-intf"

/*==================================================================================================
; af_init_AD_DA 
;
; IN:
;
; RETURN:	0 if ok, 
;-------------------------------------------------------------------------------------------------*/
int af_init_AD_DA(int channels)
{
  int device, devicesN;

  if ( (aud = AFOpenAudioConn(NULL)) == NULL) {		/* Make connection with audio server */
    fprintf(stderr, "afaudio: can't open connection to audiofile server.\n");
    fprintf(stderr, "afaudio: Be sure AUDIOFILE environment variable is set,\n");
    fprintf(stderr, "afaudio: and that you have access permission there.\n");
    return(1);
  }


  devicesN = ANumberOfAudioDevices(aud);		/* Check out devices to find mono device */
  for (device=0; device< devicesN; device++) {
    aDev = AAudioDeviceDescriptor(aud, device);
    if (aDev->playNchannels == 1) break;
  }

  if (device >= devicesN) 				/* Remember if no mono device found */
    MonoDev = -1;
  else (MonoDev = device);
  
  devicesN = ANumberOfAudioDevices(aud);		/* Check out devices to find stereo device */
  for (device=0; device< devicesN; device++) {
    aDev = AAudioDeviceDescriptor(aud, device);
    if (aDev->playNchannels == 2) break;
  }


  if (device >= devicesN)   				/* Remember if no stereo device found */
    StereoDev = -1;
  else (StereoDev = device);

  if (MonoDev == -1 && StereoDev == -1)   		/* Error if no mono or stereo devices found */
    ERR0(1, "afaudio: Audio device does not support mono or stereo channels.\n");
  
  aRate = aDev->playSampleFreq;				/* Get info about sample rate and buffer sizes */
  devPlayBufSize = aDev->playNSamplesBuf;
  devRecBufSize = aDev->recNSamplesBuf;
    
  
  attributes.preempt = Mix;				/* set up basic audio contexts */
  attributes.type = LIN16;
  if (MonoDev != -1) {
    attributes.channels =  1;
    MonoAC = AFCreateAC(aud, MonoDev,(ACPreemption|ACEncodingType|ACChannels), &attributes);
  }
  if (StereoDev != -1) {
    attributes.channels =  2;
    StereoAC = AFCreateAC(aud, StereoDev,(ACPreemption|ACEncodingType|ACChannels), &attributes);
  }
  return(0);
}



#define CHECK_init(ac, mes)  \
   if (ac == NULL) {fprintf(stderr, "%s: AF audio not initialized\n", mes); return 0;}


/*--------------------------------------------------------------------------*/

int af_set_gain(int new_gain)
{
  /* We have ability to set both record and play gain, just set record */
  CHECK_init(ac, "af_set_gain")
  attributes.play_gain = new_gain;
  AFChangeACAttributes (ac, ACRecordGain, &attributes);
  return (TRUE);
}



/*--------------------------------------------------------------------------*/
int af_set_channels(int channels)
{
  if (channels == 1) {
    if (MonoDev == -1) 
      ERR0(1, "afaudio: Recording device does not support single channel.\n");
    ac = MonoAC;
  }
  else if (channels == 2) {
    if (StereoDev == -1) 
      ERR0(1, "afaudio: Recording device does not support stereo channel.\n");
    ac = StereoAC;
  }
  else {
    ERR0(1, "afaudio: Number of channels not 1 or 2.\n");
  }
  Channels=channels;
  return(0);
}
/*--------------------------------------------------------------------------*/




/*==================================================================================================
;  af_start_AD 
;
; IN:  	cache_size		minimum number of recorded data samples to transfer
;				Is also maximum number !!
;	rate			recording rate in Hz 
;	starttime		# of samples from current time to start, can be negative
;	channels		number of channels to record from 
;
; RETURN:
;-------------------------------------------------------------------------------------------------*/
int af_start_AD(long cache_size, int rate, int starttime, int channels)
{
  if (aud == NULL) 			/* See if we are hooked up to audio system yet, if not, do */
  {
    if (af_init_AD_DA(Channels) != 0)
      ERR0(-1,  "afaudio: failed to initialize Audiofile audio\n");
  }

  if (af_set_channels(channels) != 0)			/* Set the channels */
   ERR0(-1, "afaudio: failed to set channels\n");

  if (rate != aRate) 					/* Check the recording rate */
    ERR1(-1, "afaudio: device only supports rate = %d\n", aRate);

  tRecord = AFGetTime(ac) + starttime;			/* Get current time */
  recMinSamples = cache_size;				/* Remember cache_size */

  return(0);
}
/*------------------------------------------------------------------------------------------------*/




/*==================================================================================================
; af_get_more_AD 
;
; IN: 	array		buffer into which to copy recorded data 
;	num		number of samples desired to be copied 
;
;
; RETURN:
;
; Athurs program always returned MinRecSamples, and blocked until that many was available
; (Return cache_size number of samples each time, block until that much ready )
; Now we get at least MinRecSamples, at most num, without blocking.
;
; AFRecord returns current device time, "old_time - this_time" is not the number
; of recorded samples! So in non blocking mode, how do we know how much samples were recorded?
;-------------------------------------------------------------------------------------------------*/
long af_get_more_AD(short* array, long num)
{
  int samples;
  /* ATime old_time = tRecord; */
  ATime cur_time;
  int  max_byteN = num * sizeof(SHORTS);
  int  min_byteN = recMinSamples * sizeof(SHORTS);
  int	byteN = 0;							/* number of bytes read so far	*/
  unsigned char* byteA = (unsigned char*) array;
  
  
  if (max_byteN  < min_byteN)  {
    WARN2("af_get_more_AD: num (%ld) < minRecSamples(%d)\n", num, recMinSamples);
    min_byteN = max_byteN;
  }

  /*------------------------------------------------------------------
  ; Read the requested minimum number of samples
  ;-----------------------------------------------------------------*/
  AFRecordSamples(ac, tRecord, min_byteN, byteA, ABlock);		/* block for minimum recording	*/
  tRecord += recMinSamples;
  byteN += min_byteN;


  /*------------------------------------------------------------------
  ; Non-blocking mode is problematic, because we don't know the exact
  ; number of samples read. So use blocking mode, but only if we don't
  ; have to wait anyway, because we are behind in time ...
  ;-----------------------------------------------------------------*/
  cur_time = AFGetTime(ac);						/* Get current time */
  while (byteN < max_byteN && cur_time - tRecord > min_byteN) {
    unsigned char* A = byteA + byteN;
    int N;
    N = MIN(min_byteN, max_byteN - byteN);			/* read min_byteN or less, if overflow	*/
    AFRecordSamples(ac, tRecord, N, byteA + byteN, ABlock);	/* block for minimum recording		*/
    byteN += N;							/* N more bytes read			*/
    tRecord += N /  sizeof(SHORTS);				/* new recording time			*/
  }	

  
  /* printf("AF got %d\n", (tRecord - old_time)/Channels); */
  samples = byteN / sizeof(SHORTS);				/* number of read samples		*/
  return (samples);  				/* Tell the calling program how many samples we recorded */
}



/*--------------------------------------------------------------------------*/
int af_stop_AD()
{
  return(TRUE);  			/* Don't need to do anything for AudioFile, which is always listening! */
}




/*==================================================================================================
; af_ready_DA 
;
; IN:	cache_size	maximum amount of samples to be played 
;	rate		playback rate in Hz 
;	channels	number of channels to record from 
;
; RETURN:
;-------------------------------------------------------------------------------------------------*/
int af_ready_DA(long cache_size, int rate, int channels)
{
  if (aud == NULL) 				/* See if we are hooked up to audio system yet, if not, do */
    if (af_init_AD_DA(Channels) != 0)
       ERR0(-1, "afaudio: failed to initialize Audiofile audio\n");

  if (af_set_channels(channels) != 0)				/* Set the channels */
    ERR0(-1, "afaudio: failed to set channels\n");
  
  if (rate != aRate) 						/* Check the playback rate */
    ERR1(-1, "afaudio: device only supports rate = %d\n", aRate);

  tPlay = AFGetTime(ac) + (rate / 8);				/* Set time to start play as 1/8 second in future */

  return(0);
}
/*------------------------------------------------------------------------------------------------*/








/*==================================================================================================
; 
;
; IN:   array		array of samples to be loaded for playing 
;	num		max number of samples that can be loaded 
;
; RETURN:
;-------------------------------------------------------------------------------------------------*/
long  af_play_more_DA(short* array, long num)
{
  AFPlaySamples(ac, tPlay, num*2, (unsigned char *)array);	/* Playback whole buffer in one chunk */
  tPlay += num/Channels;					/* Update time to play next chunk */
  return(num);
}


      
/*--------------------------------------------------------------------------*/

#ifdef COMMENT
static int  af_wait_end_DA()
{
  /* Wait until time has passed */
  while (AFGetTime(ac) < tPlay) my_usleep(125000);
  return(TRUE);
}
#endif





/*==================================================================================================
;  af_sample_to_buf 
;
; IN:   array		array to copy data into 
;	maxsamples	maximum number of samples to record to this buffer 
;	rate		sampling rate for this record channels 
;
; RETURN:
;-------------------------------------------------------------------------------------------------*/
long af_sample_to_buf(short* array, long maxsamples, long rate, int channels)
{
  unsigned int samples = 0; 	/* Number of samples collected so far */
  int chunkSize;		/* Size (in samples) to get from server each time */
  int descrips;			/* file descriptors to check for data on */
  int nDescrips = 0;		/* number of file desc. with data ready */
  struct timeval wait;		/* time to wait before returning from select call */
  int tmp;			/* template of file descriptors to wait on */

  /* See if we are hooked up to audio system yet, if not, do */
  if (aud == NULL) 
  {
    if (af_init_AD_DA(Channels) != 0)
    {
      fprintf(stderr, "afaudio: failed to initialize Audiofile audio\n");
      return(-1);
    }
  }


  if (af_set_channels(channels) != 0)			/* Set the channels */
    ERR0(-1,"afaudio: failed to set channels\n");

  tRecord = AFGetTime(ac);				/* Get current time */

  if (rate != aRate)					/* Check the recording rate */
    ERR1(-1,"afaudio: device only supports rate = %d\n", aRate);

  chunkSize = rate / 4;					/* Set chunksize to 1/4 second */


  tmp = 1<<fileno(stdin);			/* Prepare to check for carriage return after each block transfered */
  wait.tv_sec = 0;
  wait.tv_usec = 0;

  /* wait for user to hit return again to stop recording */
  printf("Hit RETURN to stop recording\n");
  
  /* Record data until max reached, or return is pushed */
  do {
    int nSamples;		/* number of samples recorded this time */
    descrips = tmp;

    /* Get fewer of remaining samples to max, or buffer size */
    nSamples = (chunkSize < (maxsamples-samples)) ? chunkSize : (maxsamples-samples);

    /* Return requested number of samples, block until that much ready */
    AFRecordSamples(ac, tRecord, nSamples*2, (unsigned char *)&(array[samples]), ABlock);

    /* Update our counters */
    tRecord += nSamples;
    samples += nSamples;

    /* Leave if we got enough samples already */
    if (samples >= maxsamples) break;

  } while ((nDescrips = select(32, (fd_set *)&descrips, 0,0, &wait)) == 0);
 
  /* Get last bit of data if person hit return, and there is some left */
  if (nDescrips > 0) {
    char dummy[1024];
    ATime now = AFGetTime(ac);
    gets(dummy);
    if (now > tRecord) {
      AFRecordSamples(ac, tRecord,(now-tRecord)*2,(unsigned char *)&(array[samples]), ABlock);
      samples += (now-tRecord);
    }
  }

  return (samples);
}
/*--------------------------------------------------------------------------*/





/*==================================================================================================
;  af_play_buf
;
; IN:
;
; RETURN:		Number of samples play, or -1 if there was a problem
;-------------------------------------------------------------------------------------------------*/
int af_play_buf(short* array, long nsamples, long rate, int channels)
{ 
  if (aud == NULL) 					/* See if we are hooked up to audio system yet, if not, do */
  {
    if (af_init_AD_DA(Channels) != 0)
      ERR0(-1, "afaudio: failed to initialize Audiofile audio\n");
  }

  if (af_set_channels(channels) != 0)				/* Set the channels */
    ERR0(-1, "afaudio: failed to set channels\n");

  if (rate != aRate) 						/* Check the playback rate */
    ERR1(-1, "afaudio: device only supports rate = %d\n", aRate);

  tPlay = AFGetTime(ac) + (rate/8);				/* Prepare to play back 1/8 of a second in the future */

  AFPlaySamples(ac, tPlay, nsamples*2, (unsigned char *)array);	/* Playback whole buffer in one chunk */

  while (AFGetTime(ac) < tPlay+nsamples)			/* Wait until time has passed */
    my_usleep(125000);		

  return (nsamples);
}
/*------------------------------------------------------------------------------------------------*/
#endif


#ifdef PORTAUDIO
/* -------------------------------------------------
    PORTAUDIO Code
   ----------------
    This provides fast and convenient access to
    a portable sound system, example code:

      FeatureSet fs
      fs PortAudio -mono l

      for {set i 0} {$i < 30} {incr i} {
        after 3000
        puts "   [fs PARecord -mono l]"
        puts "$i [fs:l configure -sampleN]"
      }
      fs pa -buf 0
      puts ok

   ------------------------------------------------- */

#define FRAMES_PER_BUFFER (2048)
#define PA_SAMPLE_TYPE    paInt16



/*****************************************************************************/
/* PortAudio destroy PaBuffer                                                */
/*---------------------------------------------------------------------------*/
/* destroys a PaBuffer                                                       */
/*                                                                           */
/*****************************************************************************/
void destroyPaBuffer(PaBuffer* paBuffer) {
  unsigned long j;
  if (!paBuffer)
    return;
    if (paBuffer->data) {
      for (j=0; j<paBuffer->nChannels; j++)
        if (paBuffer->data[j])
          free(paBuffer->data[j]);
      free (paBuffer->data);
    }
    free(paBuffer);
}

/*****************************************************************************/
/* PortAudio create PaBuffer                                                 */
/*---------------------------------------------------------------------------*/
/* creates a PaBuffer                                                        */
/*                                                                           */
/* PARAMETERS:                                                               */
/*    nChannels           :    number of audio channels                      */
/*    samplingRate        :    sampling rate of the audio stream             */
/*    bufferLenInSeconds  :    length of the audio buffer in seconds         */
/*                                                                           */
/* RETURNS:                                                                  */
/*    NULL                :    if the buffer could not be created            */
/*    PaBuffer*           :    otherwise                                     */
/*****************************************************************************/
PaBuffer* createPaBuffer(int nChannels, double samplingRate, float lengthInSeconds)
{
  PaBuffer*  paBuffer;
  long       bufferLen, j;

  /*--------------------*/
  /* calc buffer length */
  /*--------------------*/
  bufferLen = (long) (lengthInSeconds * samplingRate);

  /*-----------------*/
  /* allocate memory */
  /*-----------------*/
  if (!(paBuffer = (PaBuffer*) malloc(sizeof(PaBuffer)))) {
    ERROR("createPaBuffer: malloc failed.\n");
    return (NULL);
  }
  memset(paBuffer, 0, sizeof(PaBuffer));
  if (!(paBuffer->data = (SAMPLE**) malloc(nChannels * sizeof(SAMPLE*)))) {
    ERROR("createPaBuffer: malloc failed.\n");
    destroyPaBuffer (paBuffer);
    return(NULL);
  }
  for (j = 0; j < nChannels; j++) {
    if (!(paBuffer->data[j] = (SAMPLE*) malloc(bufferLen * sizeof(SAMPLE)))) {
      int i;
      for (i = 0; i < j; i++)
	free (paBuffer->data[i]);
      free (paBuffer->data);
      destroyPaBuffer (paBuffer);
      ERROR ("createPaBuffer: malloc failed.\n");
      return (NULL);
    }
  }

  /*----------------------*/
  /* initialize variables */
  /*----------------------*/
  paBuffer->nChannels = nChannels;
  paBuffer->bufferLen = bufferLen;
  paBuffer->writePos  = 0;
  paBuffer->readPos   = 0;

  return(paBuffer); 
}



/*****************************************************************************/
/* PortAudio destroy PaInstance                                              */
/*---------------------------------------------------------------------------*/
/* destroys a PaInstance                                                     */
/*                                                                           */
/*****************************************************************************/
void destroyPaInstance (PaInstance* paInstance) {
  if (!paInstance)
    return;
  if (paInstance->paBuffer)
    destroyPaBuffer(paInstance->paBuffer);
  if (paInstance->inputStreamParams)
    free(paInstance->inputStreamParams);
  if (paInstance->outputStreamParams)
    free(paInstance->outputStreamParams);
  if (paInstance->featureSet)
    paInstance->featureSet->portAudioInstance = NULL;
  if (paInstance->fname) {
    if (paInstance->file != stdout)
      fileClose (paInstance->fname, paInstance->file);
    free (paInstance->fname);
    paInstance->file  = NULL;
    paInstance->fname = NULL;
  }
  free(paInstance);
}



/*****************************************************************************/
/* PortAudio create PaInstance                                               */
/*---------------------------------------------------------------------------*/
/* creates a PaInstance                                                      */
/*                                                                           */
/* PARAMETERS:                                                               */
/*    featureSet          :    associated FeatureSet                         */
/*    deviceID            :    ID of the audio-device to be used             */
/*    deviceType          :    0=input, 1=input                              */
/*    nChannels           :    number of audio channels                      */
/*    samplingRate        :    sampling rate of the audio stream             */
/*    bufferLenInSeconds  :    length of the audio buffer in seconds         */
/*    featureNames        :    array of names of the features where the      */
/*                             audio channels are to be stored              */
/*                                                                           */
/* RETURNS:                                                                  */
/*    NULL                :    if the instance could not be created          */
/*    PaInstance*         :    otherwise                                     */
/*****************************************************************************/
PaInstance* createPaInstance(FeatureSet* featureSet, int deviceID, int deviceType,
                             int nChannels, double samplingRate,
                             float bufferLenInSeconds, char** featureNames, char* fname)
{
  PaInstance*           paInstance;
  PaBuffer*             paBuffer;
  PaStreamParameters*   streamParams;
  //PaDeviceInfo*         paDeviceInfo;

  /*--------------------------------*/
  /* allocate memory for paInstance */
  /*--------------------------------*/
  if (!(paInstance = (PaInstance*) malloc(sizeof(PaInstance)))) {
    ERROR ("createPaInstance: malloc failed.\n");
    return (NULL);
  }
  memset (paInstance, 0, sizeof(PaInstance));

  /*--------------------------------------*/
  /* allocate memory for streamParameters */
  /*--------------------------------------*/
  if (!(streamParams = malloc (sizeof(PaStreamParameters)))) {
    destroyPaInstance (paInstance);
    return (NULL);
  }
  memset (streamParams, 0, sizeof(PaStreamParameters));
  
  /*-----------------*/
  /* create PaBuffer */
  /*-----------------*/
  if (!(paBuffer = createPaBuffer (nChannels, samplingRate, bufferLenInSeconds))) {
    destroyPaInstance (paInstance);
    free (streamParams);
    return (NULL);
  }

  /*-----------------*/
  /* get device info */
  /*-----------------*/
  if (!Pa_GetDeviceInfo(deviceID)) {
    ERROR ("createPaInstance: Could not get device info for device[%i].\n", deviceID);
    destroyPaInstance (paInstance);
    free (streamParams);
    return (NULL);
  }

  /*-----------------------------------*/
  /* configure stream input parameters */
  /*-----------------------------------*/
  streamParams->device                     = deviceID;
  streamParams->channelCount               = nChannels;
  streamParams->sampleFormat               = PA_SAMPLE_TYPE;
  streamParams->suggestedLatency           = Pa_GetDeviceInfo(deviceID)->defaultLowInputLatency;
  streamParams->hostApiSpecificStreamInfo  = NULL;

  /*----------------------*/
  /* initialize variables */
  /*----------------------*/
  paInstance->featureSet         = featureSet;
  paInstance->featureNames       = featureNames;
  paInstance->paBuffer           = paBuffer;
  paInstance->samplingRate       = samplingRate;
  paInstance->inputStreamParams  = NULL;
  paInstance->outputStreamParams = NULL;
  if (deviceType==PA_INPUT_DEVICE)
    paInstance->inputStreamParams  = streamParams;
  if (deviceType==PA_OUTPUT_DEVICE)
    paInstance->outputStreamParams = streamParams;

  /* ------------------------
      store the file pointer
     ------------------------ */
  if (fname && (paInstance->file = fileOpen (fname, "w+"))) {
    int     bps = 2;  /* fixed for the moment */    
    struct {          /* HEADER for WAV files         */
      char  n[4];
      int   len;      /*      length                  */
      char  t[4];
    } head;    
    struct {          /* FMT data structure           */
      char  n[4];
      int   len;
      short format;   /*     format specifier         */
      short chans;    /*     number of channels       */
      int   srate;    /*     sample rate in Hz        */
      int   bpsec;    /*     bytes per second         */
      short bpsample; /*     bytes per sample         */
      short bpchan;   /*     bits per channel         */
    } wavfmt;
    struct {
      char n[4];
      int  len;
    } wavdata;

    paInstance->fname = malloc (1+strlen (fname));
    strcpy (paInstance->fname, fname);

    /* values */
    strncpy (head.n,    "RIFF", 4);
    strncpy (head.t,    "WAVE", 4);
    strncpy (wavfmt.n,  "fmt ", 4);
    strncpy (wavdata.n, "data", 4);
    wavdata.len     = 0; /* bps * headerADCout.sampleN; */
    head.len        = 36 + wavdata.len; 
    wavfmt.len      = 16;
    wavfmt.format   = 1;
    wavfmt.chans    = nChannels;
    wavfmt.srate    = samplingRate;
    wavfmt.bpsec    = samplingRate*bps*nChannels;
    wavfmt.bpsample = bps*nChannels;
    wavfmt.bpchan   = bps*8;

    /* write data */
    fwrite (&head,    sizeof(char), 12, paInstance->file);
    fwrite (&wavfmt,  sizeof(char), 24, paInstance->file);
    fwrite (&wavdata, sizeof(char),  8, paInstance->file);
  } else {
    paInstance->file = NULL;
  }

  /*---------------------------------------------------------*/
  /* remember the address of this instance in the FeatureSet */
  /*---------------------------------------------------------*/
  featureSet->portAudioInstance = paInstance;
  /* if you want to start several instances of PortAudio you have to put them into a
     list here, give them unique identifiers and return those identifiers to the TCL level */

  if (!fname) {
	paInstance->file = NULL;
	return (paInstance);
  }

  /* ------------------------
      store the file pointer
     ------------------------ */
  if ((paInstance->file = fileOpen (fname, "w+"))) {
    int     bps = 2;  /* fixed for the moment */    
    struct {          /* HEADER for WAV files         */
      char  n[4];
      int   len;      /*      length                  */
      char  t[4];
    } head;
    struct {          /* FMT data structure           */
      char  n[4];
      int   len;
      short format;   /*     format specifier         */
      short chans;    /*     number of channels       */
      int   srate;    /*     sample rate in Hz        */
      int   bpsec;    /*     bytes per second         */
      short bpsample; /*     bytes per sample         */
      short bpchan;   /*     bits per channel         */
    } wavfmt;
    struct {
      char n[4];
      int  len;
    } wavdata;

    paInstance->fname = malloc (1+strlen (fname));
    strcpy (paInstance->fname, fname);

    /* values */
    strncpy (head.n,    "RIFF", 4);
    strncpy (head.t,    "WAVE", 4);
    strncpy (wavfmt.n,  "fmt ", 4);
    strncpy (wavdata.n, "data", 4);
    wavdata.len     = 0; /* bps * headerADCout.sampleN; */
    head.len        = 36 + wavdata.len; 
    wavfmt.len      = 16;
    wavfmt.format   = 1;
    wavfmt.chans    = nChannels;
    wavfmt.srate    = samplingRate;
    wavfmt.bpsec    = samplingRate*bps*nChannels;
    wavfmt.bpsample = bps*nChannels;
    wavfmt.bpchan   = bps*8;

    /* write data */
    fwrite (&head,    sizeof(char), 12, paInstance->file);
    fwrite (&wavfmt,  sizeof(char), 24, paInstance->file);
    fwrite (&wavdata, sizeof(char),  8, paInstance->file);
  } else {
    paInstance->file = NULL;
  }

  return(paInstance);
}



/*****************************************************************************/
/* PortAudio paCurrentSamplingRate                                           */
/*---------------------------------------------------------------------------*/
/* returns the current sampling rate                                         */
/*                                                                           */
/* PARAMETERS:                                                               */
/*    featureSet          :    associated FeatureSet                         */
/*                                                                           */
/* RETURNS:                                                                  */
/*    the current sampling rate in kHz                                       */
/*****************************************************************************/
double paCurrentSamplingRate(FeatureSet* featureSet)
{
  PaInstance*           paInstance;
  float                 samplingRate;

  samplingRate = 0.0;

  if (featureSet==NULL)
    ERROR ("paCurrentSamplingRate: featureSet is NULL.\n");
  else
  {
    paInstance = (PaInstance*) featureSet->portAudioInstance;
    if (paInstance==NULL)
      ERROR ("paCurrentSamplingRate: paInstance is NULL.\n");
    else
      samplingRate = paInstance->samplingRate * 0.001;
  }
  return(samplingRate);
}



/*****************************************************************************/
/* PortAudio CALLBACK routine for recording                                  */
/*---------------------------------------------------------------------------*/
/* This routine is called by the PortAudio engine when new data is           */
/* available. It might run on interrupt-level on some machines! So be        */
/* careful with shared variables or function calls. Use of mutexes           */
/* might lock the system ...                                                 */
/*****************************************************************************/
static int paCallbackRecording (const void *inputBuffer, void *outputBuffer,
                       unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo* timeInfo,
                       PaStreamCallbackFlags statusFlags,
                       void *userData )
{

  PaInstance*     paInstance              = (PaInstance*) userData;
  unsigned long   nSamplesInInputBuffer   = framesPerBuffer;

  PaBuffer*       paBuffer;
  unsigned long   readPos, writePos, newWritePos, bufferLen, bufferRemaining;
  unsigned long   nSamples1, nSamples2, nChannels;
  unsigned long   i,j;
  SAMPLE**        data;
  SAMPLE*         dp;
  SAMPLE*         sp;

  /*------------------*/
  /* catch SEG FAULTS */
  /*------------------*/
  if (paInstance==NULL)
  {
    /* printf("paCallback: <userData> was zero &#@$%! Discontinuing audio ...\n"); */
    return(paAbort);
  }
  paBuffer = paInstance->paBuffer;
  if (paBuffer==NULL)
  {
    /* printf("paCallback: <userData> was zero &#@$%! Discontinuing audio ...\n"); */
    return(paAbort);
  }
  if (inputBuffer==NULL)
  {
    /* printf("paCallback: <inputBuffer> was zero &#@$%! Discontinuing audio ...\n"); */
    return(paAbort);
  }

  /*-----------------------*/
  /* extract paBuffer info */
  /*-----------------------*/
  readPos   = paBuffer->readPos;
  writePos  = paBuffer->writePos;
  bufferLen = paBuffer->bufferLen;
  data      = paBuffer->data;
  nChannels = paBuffer->nChannels;

  /*------------------------------------------------*/
  /* compute "bufferRemaining" (= how many room for */
  /* samples(frames) is there left?)                */
  /*------------------------------------------------*/
  if (writePos>=readPos)
    bufferRemaining = bufferLen - (writePos - readPos);
  else
    bufferRemaining = (readPos - writePos);
 
  /*------------------------------*/
  /* do we have a buffer overrun? */
  /*------------------------------*/
  if (paBuffer->overrun)
  {
    paBuffer->nSamplesSkipped += nSamplesInInputBuffer;
    return(paContinue);
  }
  if (bufferRemaining<nSamplesInInputBuffer)
  {
    paBuffer->overrun = 1;
    paBuffer->nSamplesSkipped += nSamplesInInputBuffer - bufferRemaining;
    nSamplesInInputBuffer = bufferRemaining;
    /* printf("paCallback: paBuffer overrun @#$&! Discontinuing audio ...\n"); */
    /* return(paAbort); */
  }

  /*---------------------------------------------------------------*/
  /* calculate newWritePos (buffer position after write operation) */
  /*---------------------------------------------------------------*/
  newWritePos = writePos + nSamplesInInputBuffer;

  /*--------------------------------------------------------------------*/
  /* if clipping occurs (because we have reached the end of the buffer) */
  /* copy inputBuffer to PA_Buffer until writePos reaches bufferLen     */
  /*--------------------------------------------------------------------*/
  nSamples1 = 0;
  if (newWritePos>bufferLen)
  {
    nSamples1 = bufferLen - writePos;
    for (j=0; j<nChannels; j++)
    {
      sp =  &(((SAMPLE*)inputBuffer)[j]);
      dp =  &(data[j][writePos]);
      for (i=0; i<nSamples1; i++)
        dp[i] = sp[i*nChannels];
    }
    writePos = 0;
  }

  /*----------------------------------------------*/
  /* copy (the remaining part of) the inputBuffer */
  /* into the given PA_Buffer                     */
  /*----------------------------------------------*/
  nSamples2 = nSamplesInInputBuffer - nSamples1;
  for (j=0; j<nChannels; j++)
  {
    sp =  &(((SAMPLE*)inputBuffer)[nSamples1*nChannels + j]);
    dp =  &(data[j][writePos]);
    /* printf("  storing data to %i\n", dp); */
    for (i=0; i<nSamples2; i++)
      dp[i] = sp[i*nChannels];
  }

  /*-----------------*/
  /* update writePos */
  /*-----------------*/
  paBuffer->writePos  = newWritePos % bufferLen;

  return(paContinue);
}



/*****************************************************************************/
/* PortAudio CALLBACK routine for playing                                    */
/*---------------------------------------------------------------------------*/
/* This routine is called by the PortAudio engine when new data is           */
/* available. It might run on interrupt-level on some machines! So be        */
/* careful with shared variables or function calls. Use of mutexes           */
/* might lock the system ...                                                 */
/*****************************************************************************/
static int paCallbackPlaying(const void *inputBuffer, void *outputBuffer,
                       unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo* timeInfo,
                       PaStreamCallbackFlags statusFlags,
                       void *userData )
{

  PaInstance*     paInstance               = (PaInstance*) userData;
  unsigned long   nSamplesInOutputBuffer   = framesPerBuffer;

  PaBuffer*       paBuffer;
  unsigned long   readPos, newReadPos, bufferLen;
  unsigned long   nSamples, nChannels;
  unsigned long   i,j;
  SAMPLE**        data;
  SAMPLE*         dp;
  SAMPLE*         sp;

  /*------------------*/
  /* catch SEG FAULTS */
  /*------------------*/
  if (paInstance==NULL)
  {
    /* printf("paCallback: <userData> was zero &#@$%! Discontinuing audio ...\n"); */
    return(paAbort);
  }
  paBuffer = paInstance->paBuffer;
  if (paBuffer==NULL)
  {
    /* printf("paCallback: <userData> was zero &#@$%! Discontinuing audio ...\n"); */
    return(paAbort);
  }
  if (outputBuffer==NULL)
  {
    /* printf("paCallback: <inputBuffer> was zero &#@$%! Discontinuing audio ...\n"); */
    return(paAbort);
  }

  /*-----------------------*/
  /* extract paBuffer info */
  /*-----------------------*/
  readPos   = paBuffer->readPos;
  //writePos  = paBuffer->writePos;
  bufferLen = paBuffer->bufferLen;
  data      = paBuffer->data;
  nChannels = paBuffer->nChannels;

  /*--------------------------*/
  /* have we reached the end? */
  /*--------------------------*/
  if (readPos>=bufferLen)
    return(paAbort);

  /*------------------------------*/
  /* how many samples to transfer */
  /*------------------------------*/
  nSamples   = nSamplesInOutputBuffer;
  newReadPos = readPos + nSamples;
  if (newReadPos>=bufferLen)
      nSamples = bufferLen - readPos;

  /*---------------------------------------------*/
  /* copy from given PA_Buffer into outputBuffer */
  /*---------------------------------------------*/
  for (j=0; j<nChannels; j++)
  {
    sp = &(data[j][readPos]);
    dp = &(((SAMPLE*)outputBuffer)[j]);
    /* printf("  storing data to %i\n", dp); */
    for (i=0; i<nSamples; i++)
      dp[i*nChannels] = sp[i];
  }

  /*-----------------------------*/
  /* pad with zeros if necessary */
  /*-----------------------------*/
  if (nSamples<nSamplesInOutputBuffer)
  {
    for (j=0; j<nChannels; j++)
    {
      dp = &(((SAMPLE*)outputBuffer)[j]);
      for (i=0; i<nSamples; i++)
        dp[i*nChannels] = 0;
    }
  }

  /*-----------------*/
  /* update readPos */
  /*-----------------*/
  paBuffer->readPos = newReadPos;

  return(paContinue);
}



/*****************************************************************************/
/* PortAudio Get                                                             */
/*---------------------------------------------------------------------------*/
/* transfer recorded data from PaBuffer to the feature vectors               */
/*                                                                           */
/* PARAMETERS:                                                               */
/*   time               : amount of transferred samples in seconds           */
/*                                                                           */
/* RETURNS:                                                                  */
/*****************************************************************************/
int paGet(FeatureSet* featureSet, float* time) {
  PaInstance*     paInstance;
  PaBuffer*       paBuffer;
  unsigned long   readPos, writePos, bufferLen, nChannels;
  unsigned long   i,j, nSamplesAvailable, newReadPos;
  unsigned long   nSamples1, nSamples2;
  SVector*        sVector;
  Feature**       features;
  char**          featureNames;
  SAMPLE*         srcP;
  short*          dstP;
  double          samplingRateInHz, samplingRateInKHz;
  float           samplesAvailInSeconds;
  int             fIndex;

  /*-----------------*/
  /* catch SEGFAULTS */
  /*-----------------*/
  if (!featureSet) {
    ERROR ("paGet: featureSet is NULL.\n");
    return (TCL_ERROR);
  }
  if (!(paInstance = featureSet->portAudioInstance)) {
    ERROR ("paGet: paInstance is NULL.\n");
    return (TCL_ERROR);
  }
  if (!(paBuffer = paInstance->paBuffer)) {
    ERROR ("paGet: paBuffer is NULL.\n");
    destroyPaInstance (paInstance);
    return (TCL_ERROR);
  }

  /*--------------*/
  /* extract info */
  /*--------------*/
  readPos      = paBuffer->readPos;
  writePos     = paBuffer->writePos;
  bufferLen    = paBuffer->bufferLen;
  nChannels    = paBuffer->nChannels;
  featureNames = paInstance->featureNames;

  /*-------------------------------------------*/
  /* determine the number of available samples */
  /*-------------------------------------------*/
  if (paBuffer->overrun)
    nSamplesAvailable = bufferLen;
  else {
    if (readPos <= writePos)
      nSamplesAvailable = writePos - readPos;
    else
      nSamplesAvailable = (bufferLen - (readPos - writePos));
  }

  /*--------------------------------------------*/
  /* get amount of available samples in seconds */
  /*--------------------------------------------*/
  /* samplingRate          = (Pa_GetStreamInfo(paInstance->stream))->sampleRate; */
  samplingRateInHz      = paInstance->samplingRate;
  samplingRateInKHz     = ((float)ROUND(samplingRateInHz))*1E-3F;
  samplesAvailInSeconds = nSamplesAvailable / samplingRateInHz; 

  /*------------------------------------------*/
  /* retrieve features (create if neccessary) */
  /*------------------------------------------*/
  if (!(features = malloc (nChannels * sizeof (Feature*)))) {
    ERROR ("paGet: memory allocation failed.\n");
    return (TCL_ERROR);
  }
  for (j = 0; j < nChannels; j++) {
    /*---------------------------------*/
    /* does the feature already exist? */
    /*---------------------------------*/
    fIndex = -1;
    for (i = 0; i < featureSet->featN; i++)
      if (featureSet->featA[i].name && (strcmp(featureSet->featA[i].name, featureNames[j])==0)) 
        fIndex = i;
    if (fIndex >= 0) {
      /*-------------------------------------------*/
      /* check that destination feature is SVector */
      /*-------------------------------------------*/
      features[j] = &(featureSet->featA[fIndex]);
      if ((features[j]==NULL) || (features[j]->type!=FE_SVECTOR)) {
        /* printf("%i\n", features[j]->type); */
        ERROR ("paGet: features must be SVectors\n");
        free (features);
        return (TCL_ERROR);
      }
    } else {
      /*------------------------*/
      /* create SVector feature */
      /*------------------------*/
      fIndex = fesGetIndex(featureSet, featureNames[j], 1);
      if (fIndex>=0)
        features[j] = &(featureSet->featA[fIndex]);
      else {
        ERROR("paGet: failed to create features.\n");
        free (features);
        return (TCL_ERROR);
      }
      features[j]->data.svec     = svectorCreate(0);
      features[j]->dcoeffN       = 0;
      features[j]->dcoeffN       = 0;
      features[j]->type          = FE_SVECTOR;
      features[j]->dtype         = FE_SVECTOR;
      features[j]->samplingRate  = samplingRateInKHz;       
    }
  }

  /*----------------------------*/
  /* resize the feature vectors */
  /*----------------------------*/
  for (j = 0; j < nChannels; j++) {
    sVector = features[j]->data.svec;
    sVector->vecA = realloc (sVector->vecA, sizeof(short)*(sVector->n + nSamplesAvailable));
    if (sVector->vecA==NULL) {
      ERROR ("paGet: could not reallocate feature for channel %i\n", j);
      free (features);
      return (TCL_ERROR);
    }
  }

  /*------------------------------------------------------------------*/
  /* COPY 1                                                           */
  /* if the paBuffer is clipped due to buffer overflow (end of buffer */
  /* reached while writing) copy the part up to bufferLen to the      */
  /* corresponding feature vectors, then start at readPos 0           */
  /*------------------------------------------------------------------*/
  newReadPos = readPos + nSamplesAvailable;
  nSamples1 = 0;
  if (newReadPos > bufferLen) {
    nSamples1 = (bufferLen - readPos);
    for (j = 0; j < nChannels; j++) {
      sVector = features[j]->data.svec;
      srcP = &(paBuffer->data[j][readPos]);
      dstP = &(sVector->vecA[sVector->n]);
      for (i=0; i<nSamples1; i++)
        dstP[i] = srcP[i];
      sVector->n += nSamples1;
    }
    readPos = 0;      /* could copy that back here */ 
  }

  /*---------------------------------------------------------------*/
  /* COPY 2                                                        */
  /* copy (the remaining part of) paBuffer to the feature vectors  */
  /*---------------------------------------------------------------*/
  nSamples2 = nSamplesAvailable - nSamples1;
  for (j = 0; j < nChannels; j++) {
    sVector = features[j]->data.svec;
    srcP = &(paBuffer->data[j][readPos]);
    dstP = &(sVector->vecA[sVector->n]);
    for (i=0; i<nSamples2; i++)
      dstP[i] = srcP[i]; 
    sVector->n += nSamples2;
  }

  /* --------------------------------------
      Now deal with the file, if necessary
     -------------------------------------- */
  if (paInstance->file) {
    int   w = 0;
    readPos = paBuffer->readPos;
    while (w++ < nSamplesAvailable) {
      for (j = 0; j < nChannels; j++)
	fwrite (paBuffer->data[j]+readPos, sizeof (SAMPLE), 1, paInstance->file);
      if (++readPos > bufferLen)
	readPos = 0;
    }
    fseek  (paInstance->file, 40, SEEK_SET);
    fread  (&w, sizeof (int), 1, paInstance->file);

    w += nSamplesAvailable*nChannels*sizeof (SAMPLE);
    fseek  (paInstance->file, 40, SEEK_SET);
    fwrite (&w, sizeof (int), 1, paInstance->file);
    w += 36;
    fseek  (paInstance->file,  4, SEEK_SET);
    fwrite (&w, sizeof (int), 1, paInstance->file);
    fflush (paInstance->file);
    fseek  (paInstance->file, 0, SEEK_END);
  }

  /*----------------*/
  /* update readPos */
  /*----------------*/
  if (bufferLen)
    paBuffer->readPos = newReadPos % bufferLen;
  else
    WARN ("paGet: bufferLen = 0\n");
  if (paBuffer->overrun == 1) {
    paBuffer->overrun = 0;
    paBuffer->nSamplesSkipped = 0;
  }

  /*----------------------------------------------*/
  /* write back the number of transferred samples */
  /*----------------------------------------------*/
  (*time) = samplesAvailInSeconds;

  /*----------*/
  /* clean up */
  /*----------*/
   free(features);

   return(TCL_OK);
}



/*****************************************************************************/
/* PortAudio ListDevices                                                     */
/*---------------------------------------------------------------------------*/
/* list available audio devices and supported modes                          */
/*                                                                           */
/* PARAMETERS:                                                               */
/*   deviceType     :    PA_INPUT_DEVICE or PA_OUTPUT_DEVICE                                     */
/*                                                                           */
/*****************************************************************************/
int paListDevices(FeatureSet* fs, int deviceType)
{

  double              samplingRates[6] = {8.000, 11.025, 16.000, 22.050, 44.100, 48.000};
  int                 nSamplingRates = sizeof(samplingRates)/sizeof(samplingRates[0]);

  PaError             paError;
  int                 nDevices, i, j, nChannels = 1;
  PaDeviceInfo*       deviceInfo;
  PaHostApiInfo*      hostApiInfo;
  PaStreamParameters  streamParams;
  PaTime              latency = 0;

  if (deviceType != PA_INPUT_DEVICE && deviceType != PA_OUTPUT_DEVICE) {
    ERROR ("paListDevices: unknown deviceType %i.\n", deviceType);
    return TCL_ERROR;
  }

  /*----------------------*/
  /* initialize PortAudio */
  /*----------------------*/
  if (paNoError != (paError = Pa_Initialize())) {
    ERROR ("paListDevices: could not initialize PortAudio (%s)\n",
	   Pa_GetErrorText (paError));
    return TCL_ERROR;
  }

  /*---------------------------*/
  /* run through audio devices */
  /*---------------------------*/
  nDevices = Pa_GetDeviceCount();
  for (i = 0; i < nDevices; i++) {
    if (!(deviceInfo = (PaDeviceInfo*) Pa_GetDeviceInfo(i)))
      continue;

    hostApiInfo = (PaHostApiInfo*) Pa_GetHostApiInfo(deviceInfo->hostApi);

    /*------------------------------------*/
    /* get latency and number of channels */
    /*------------------------------------*/
    if (deviceType == PA_INPUT_DEVICE) {
      nChannels = deviceInfo->maxInputChannels;
      latency   = deviceInfo->defaultLowInputLatency;
    } else if (deviceType == PA_OUTPUT_DEVICE) {
      nChannels = deviceInfo->maxOutputChannels;
      latency   = deviceInfo->defaultLowInputLatency;
    }

    /*--------------------------------------------------------*/
    /* make sure that there is at least one channel available */
    /*--------------------------------------------------------*/
    if (nChannels <= 0)
      continue;

    /*---------------------------*/
    /* write general device info */
    /*---------------------------*/
    itfAppendResult("{%i {%s} {%s} %i {", i, deviceInfo->name, hostApiInfo->name, nChannels);
    
    /*-------------------------------*/
    /* test supported sampling rates */
    /*-------------------------------*/
    for (j = 0; j < nSamplingRates; j++) {
      streamParams.device                    = i;
      streamParams.channelCount              = nChannels;
      streamParams.sampleFormat              = PA_SAMPLE_TYPE;
      streamParams.suggestedLatency          = latency;
      streamParams.hostApiSpecificStreamInfo = NULL; 
      if (deviceType == PA_INPUT_DEVICE)
	paError = Pa_IsFormatSupported (&streamParams, NULL, samplingRates[j]*1000);
      else
	paError = Pa_IsFormatSupported (NULL, &streamParams, samplingRates[j]*1000);
      if (paError == paNoError)
	itfAppendResult(" %.4e", samplingRates[j]);
    }
    itfAppendResult("}} \n");
  }
  
  /*---------------------*/
  /* terminate PortAudio */
  /*---------------------*/
  Pa_Terminate();
  
  return TCL_OK;
}

/*****************************************************************************/
/* PortAudio DefaultDevice                                                   */
/*---------------------------------------------------------------------------*/
/* get default audio device-ID and sampling rate                             */
/*                                                                           */
/* PARAMETERS:                                                               */
/*   deviceType     :     PA_INPUT_DEVICE or PA_OUTPUT_DEVICE                                    */
/*                                                                           */
/* ITF-Result                                                                */
/*   %i : deviceID;                                                          */
/*   %i : number of audio channels;                                          */
/*   %i : sampling rate;                                                     */
/*****************************************************************************/
int paDefaultDevice(FeatureSet* fs, int deviceType)
{

  PaError             paError;
  PaDeviceInfo*       deviceInfo;
  PaDeviceIndex	      paDI;
  int                 deviceID;
  double              samplingRate;

  /*----------------------*/
  /* initialize PortAudio */
  /*----------------------*/
  paError = Pa_Initialize();
  if (paError!=paNoError)
  {
    ERROR ("paDefaultDevice: could not initialize PortAudio. %s\n", Pa_GetErrorText(paError));
    return TCL_ERROR;
  }

  /*--------------------*/
  /* get default device */
  /*--------------------*/
  if (deviceType==PA_INPUT_DEVICE) {
    paDI = Pa_GetDefaultInputDevice();
  } else {
    if (deviceType==PA_OUTPUT_DEVICE) {
      paDI = Pa_GetDefaultOutputDevice();
    } else {
      ERROR("paDefaultDevice: unknown deviceType %i\n", deviceType);
      return(TCL_ERROR);
    }
  }
  if (paDI==paNoDevice)
    return(TCL_ERROR);
  deviceID = paDI;

  /*-------------------*/
  /* get sampling rate */
  /*-------------------*/
  deviceInfo = (PaDeviceInfo*) Pa_GetDeviceInfo(deviceID);
  if (deviceInfo==NULL)
  {
    ERROR("paDefaultDevice: could not get info for audio device %i!\n", deviceID);
    return TCL_ERROR; 
  }
  samplingRate = deviceInfo->defaultSampleRate * 0.001;

  /*------------------*/
  /* write ITF result */
  /*------------------*/
  itfAppendResult("%i 1 %f", deviceID, samplingRate);

  /*---------------------*/
  /* terminate PortAudio */
  /*---------------------*/
  Pa_Terminate();

  return TCL_OK;
}



/*****************************************************************************/
/* PortAudio Status                                                          */
/*---------------------------------------------------------------------------*/
/* returns status and bufferRemaining                                        */
/*                                                                           */
/* PARAMETERS:                                                               */
/*    featureSet          :    associated FeatureSet                         */
/*                                                                           */
/* RETURNS:                                                                  */
/*    TCL_OK  or  TCL_ERROR                                                  */
/*                                                                           */
/* ITF-Result                                                                */
/*   %s : mode in {recording, playing, ill};                                 */
/*   %f : available/requested samples in seconds;                            */
/*   %f : remaining buffer in seconds;                                       */
/*   %f : samples skipped in seconds;                                        */
/*****************************************************************************/
int paStatus(FeatureSet* fs)
{

  PaError               paError;
  PaInstance*           paInstance;
  PaBuffer*             paBuffer;
  int                   bufferRemaining, nSamplesAvailable;
  double                samplingRate;
  int                   readPos, writePos, bufferLen;
  float                 remainingBufferInSeconds;
  float                 samplesSkippedInSeconds;
  float                 samplesAvailableInSeconds;

  /*-----------------*/
  /* catch SEGFAULTS */
  /*-----------------*/
  paInstance = (PaInstance*) fs->portAudioInstance;
  if (paInstance==NULL)
  {
    itfAppendResult("inactive");
    return(TCL_OK);
  }
  paBuffer = paInstance->paBuffer;
  if (paBuffer==NULL)
  {
    ERROR("PortAudio: paInstance->paBuffer was NULL!");
    return(TCL_ERROR);
  }
  if (paInstance->stream==NULL)
  {
    ERROR("PortAudio: paInstance->stream was NULL!");
    return(TCL_ERROR);
  }

  /*----------------------*/
  /* initialize PortAudio */
  /*----------------------*/
  paError = Pa_Initialize();
  if (paError!=paNoError)
  {
    ERROR ("paStatus: could not initialize PortAudio. %s\n", Pa_GetErrorText(paError));
    return TCL_ERROR;
  }

  /*-------------------------*/
  /* did the stream run dry? */
  /*-------------------------*/
  if (Pa_IsStreamActive(paInstance->stream)!=1)
  {
    itfAppendResult("stopped");
    return(TCL_ERROR);
  }

  /*-----------------------*/
  /* extract paBuffer info */
  /*-----------------------*/
  readPos   = paBuffer->readPos;
  writePos  = paBuffer->writePos;
  bufferLen = paBuffer->bufferLen;

  /*------------------------------------------------*/
  /* compute "bufferRemaining" (= how many room for */
  /* samples(frames) is there left?)                */
  /*------------------------------------------------*/
  if (paBuffer->overrun)
    nSamplesAvailable = bufferLen; 
  else
  {
    if (readPos <= writePos)
      nSamplesAvailable = writePos - readPos;
    else
      nSamplesAvailable = (bufferLen - (readPos - writePos));
  }
  bufferRemaining = bufferLen - nSamplesAvailable; 

  /*---------------------------------------------------------*/
  /* calculate remaining buffer / samples skipped in seconds */
  /*---------------------------------------------------------*/
  samplingRate = paInstance->samplingRate;
  remainingBufferInSeconds  =  (float)bufferRemaining / samplingRate;
  samplesSkippedInSeconds   =  (float)paBuffer->nSamplesSkipped / samplingRate;
  samplesAvailableInSeconds =  (float)nSamplesAvailable / samplingRate;

  /*------------------*/
  /* write ITF result */
  /*------------------*/
  if ((paInstance->outputStreamParams==NULL) && (paInstance->inputStreamParams!=NULL))
    itfAppendResult("recording ");
  else
  {
    if ((paInstance->outputStreamParams!=NULL) && (paInstance->inputStreamParams==NULL))
      itfAppendResult("playing ");
    else
      itfAppendResult("ill ");
  }
  itfAppendResult("%f %f %f", samplesAvailableInSeconds, remainingBufferInSeconds,
                              samplesSkippedInSeconds);

  /*---------------------*/
  /* terminate PortAudio */
  /*---------------------*/
  Pa_Terminate();

  return TCL_OK;
}



/*****************************************************************************/
/* PortAudio stop                                                            */
/*---------------------------------------------------------------------------*/
/* stops PortAudio recording or playing                                      */
/*                                                                           */
/* PARAMETERS:                                                               */
/*    featureSet          :    associated FeatureSet                         */
/*                                                                           */
/* RETURNS:                                                                  */
/*    TCL_OK  or  TCL_ERROR                                                  */
/*****************************************************************************/
int paStop(FeatureSet* featureSet) {
  PaError               paError;
  PaInstance*           paInstance;

  /*-----------------*/
  /* catch segfaults */
  /*-----------------*/
  if (!featureSet) {
    ERROR ("paStop: <featureSet> was NULL\n");
    return (TCL_ERROR);
  }
  if (!(paInstance = featureSet->portAudioInstance)) {
    ERROR ("paStop: <paInstance> was NULL\n");
    return(TCL_ERROR);
  }
  if (!paInstance->stream) {
    ERROR ("paStop: <stream> was NULL\n");
    return(TCL_OK);
  }

  /*--------------*/
  /* close stream */
  /*--------------*/
  if (paNoError != (paError = Pa_CloseStream(paInstance->stream))) {
    ERROR ("paStop: error closing audio stream. %s\n", Pa_GetErrorText(paError));
    return(TCL_ERROR);
  }

  /*---------------------*/
  /* terminate PortAudio */
  /*---------------------*/
  Pa_Terminate();

  /*---------*/
  /* destroy */
  /*---------*/
  destroyPaInstance (paInstance);

  return(TCL_OK);
}



/*****************************************************************************/
/* PortAudio start                                                           */
/*---------------------------------------------------------------------------*/
/* starts PortAudio                                                          */
/*                                                                           */
/* PARAMETERS:                                                               */
/*    featureSet          :    associated FeatureSet                         */
/*    deviceID            :    ID of the audio-device to be used             */
/*                             [<= 0] : use default                          */
/*    deviceType          :    PA_INPUT_DEVICE or PA_OUTPUT_DEVICE           */
/*    nChannels           :    number of audio channels                      */
/*    samplingRate        :    sampling rate of the audio stream (in kHz)    */
/*                             [<= 0] : use default                          */
/*    bufferLenInSeconds  :    length of the audio buffer in seconds         */
/*    featureNames        :    array of names of the features where the      */
/*                             audio channels are to be stored               */
/*                                                                           */
/* RETURNS:                                                                  */
/*    NULL                :    if the instance could not be created          */
/*    PaInstance*         :    otherwise                                     */
/*****************************************************************************/
int paStartRecording(FeatureSet* featureSet, int deviceID, int nChannels,
		     double samplingRate, float bufferLenInSeconds, 
                     char** featureNames, char *fname)
{

  PaError               paError;
  PaInstance*           paInstance;
  PaDeviceIndex		paDI;
  PaDeviceInfo*         deviceInfo;
  char*                 s;

  /*-----------------*/
  /* catch segfaults */
  /*-----------------*/
  if (featureSet==NULL)
  {
    ERROR ("paStart: <featureSet> was NULL\n");
    return(TCL_ERROR);
  }

  /*----------------------------*/
  /* another paInstance active? */
  /*----------------------------*/
  paInstance = featureSet->portAudioInstance;
  if (paInstance!=NULL)
  {
    if (Pa_IsStreamActive(paInstance->stream))
    {
      s = "ILL";
      if ((paInstance->inputStreamParams!=NULL) && (paInstance->outputStreamParams==NULL))
        s = "RECORDING";
      if ((paInstance->inputStreamParams==NULL) && (paInstance->outputStreamParams!=NULL))
        s = "PLAYING";
      ERROR ("paStart: portaudio is already active and it is %s!\n  ignoring command.\n", s);
      return TCL_OK;
    }
    else
      destroyPaInstance(paInstance);  
  }

  /*---------------------*/
  /* intialize PortAudio */
  /*---------------------*/
  paError = Pa_Initialize();
  if (paError!=paNoError)
  {
    ERROR("paStart: could not initialize PortAudio: %s\n", Pa_GetErrorText(paError));
    return TCL_ERROR;
  }

  /*---------------------------*/
  /* use default input device? */
  /*---------------------------*/
  if (deviceID==-1)
  {
	  /* fprintf(stderr, "paStart: no input device specified. using default.\n"); */
    paDI = Pa_GetDefaultInputDevice();
	  if (paDI==paNoDevice)
	  {
      ERROR("paStart: no default device!\n");
      return TCL_ERROR;
	  }
    deviceID = paDI;
	/* fprintf(stderr, "deviceID set to %i.\n", deviceID); */
  }

  /*----------------------------*/
  /* use default sampling rate? */
  /*----------------------------*/
  if (samplingRate<=0)
  {
	/* fprintf(stderr, "paStart: no sampling rate specified. using default.\n"); */
    deviceInfo = (PaDeviceInfo*) Pa_GetDeviceInfo(deviceID);
    if (deviceInfo==NULL)
    {
      ERROR("paStart: could not get info for audio device %i!\n", deviceID);
      return TCL_ERROR;
    }
    samplingRate = deviceInfo->defaultSampleRate * 0.001;
	  fprintf(stderr, "sampling rate set to %f kHz.\n", samplingRate);
  }

  /*--------------------------------------*/
  /* convert sampling from kHz rate to Hz */
  /*--------------------------------------*/
  samplingRate = samplingRate * 1000;

  /*---------------------*/
  /* check sampling rate */
  /*---------------------*/
  if (samplingRate>100000)
	WARN("paStart: sampling rate might be a little high (%f Hz).\n", samplingRate);

  /*-------------------*/
  /* create paInstance */
  /*-------------------*/
  paInstance = createPaInstance(featureSet, deviceID, PA_INPUT_DEVICE, nChannels, samplingRate,
                                bufferLenInSeconds, featureNames, fname);
  if (paInstance==NULL)
  {
    ERROR("paStart: paInstance is NULL.\n");
    return TCL_ERROR;
  }

  /*-------------------*/
  /* open audio stream */
  /*-------------------*/
  paError = Pa_OpenStream(
          &(paInstance->stream),
          paInstance->inputStreamParams,
          paInstance->outputStreamParams,
          samplingRate,
          FRAMES_PER_BUFFER,
          paClipOff,
          paCallbackRecording,
          paInstance
          ); 
  if (paError!=paNoError)
  {
    ERROR("paStart: could not open audio stream. %s\n", Pa_GetErrorText(paError));
    destroyPaInstance(paInstance);
    return TCL_ERROR;
  }

  /*-----------------*/
  /* start streaming */
  /*-----------------*/
  paError = Pa_StartStream(paInstance->stream);
  if (paError!=paNoError)
  {
    ERROR("paStart: could not start audio stream. %s\n", Pa_GetErrorText(paError));
    destroyPaInstance(paInstance);
    return TCL_ERROR;
  }

  return TCL_OK;
}



/*****************************************************************************/
/* PortAudio play                                                            */
/*---------------------------------------------------------------------------*/
/* plays an SVector feature using PortAudio                                  */
/*                                                                           */
/* PARAMETERS:                                                               */
/*    featureSet          :    associated FeatureSet                         */
/*    deviceID            :    ID of the audio-device to be used             */
/*                             [<= 0] : use default                          */
/*    featureName         :    name of the feature to be played              */
/*    startPos            :    where to start playing                        */
/*    endPos              :    where to end playing                          */
/*                                                                           */
/* RETURNS:                                                                  */
/*    NULL                :    if the instance could not be created          */
/*    PaInstance*         :    otherwise                                     */
/*****************************************************************************/
int paPlay(FeatureSet* featureSet, int deviceID, char* featureName,
                   int startPos, int endPos)
{

  PaError               paError;
  PaInstance*           paInstance;
  PaBuffer*             paBuffer;
  PaDeviceIndex		paDI;
  /* PaDeviceInfo*         deviceInfo; */
  char*                 s;
  float                 bufferLenInSeconds;
  double                samplingRate;
  unsigned long         i, len;
  SAMPLE*               dp;
  short*                sp;
  int                   fIndex;
  Feature*              feature;

  /*-----------------*/
  /* catch segfaults */
  /*-----------------*/
  if (featureSet==NULL)
  {
    ERROR ("paPlay: <featureSet> was NULL\n");
    return(TCL_ERROR);
  }

  /*------------------*/
  /* retrieve feature */
  /*------------------*/
  fIndex = fesGetIndex(featureSet, featureName, 0);
  if (fIndex<0)
  {
    ERROR("paPlay: feature[%s] does not exist!\n", featureName);
    return(TCL_ERROR);
  }
  feature = &(featureSet->featA[fIndex]);
  if (feature==NULL)
  {
    ERROR ("paPlay: <feature> was NULL\n");
    return(TCL_ERROR);
  }

  /*----------------------------*/
  /* another paInstance active? */
  /*----------------------------*/
  paInstance = featureSet->portAudioInstance;
  if (paInstance!=NULL)
  {
    if (Pa_IsStreamActive(paInstance->stream))
    {
      s = "ILL";
      if ((paInstance->inputStreamParams!=NULL) && (paInstance->outputStreamParams==NULL))
        s = "RECORDING";
      if ((paInstance->inputStreamParams==NULL) && (paInstance->outputStreamParams!=NULL))
        s = "PLAYING";
      ERROR ("paStart: portaudio is already active and it is %s!\n ignoring command.\n", s);
      return TCL_OK;
    }
    else
      destroyPaInstance(paInstance);  
  }

  /*---------------------*/
  /* intialize PortAudio */
  /*---------------------*/
  paError = Pa_Initialize();
  if (paError!=paNoError)
  {
    ERROR("paPlay: could not initialize PortAudio. %s\n", Pa_GetErrorText(paError));
    return TCL_ERROR;
  }

  /*---------------------------*/
  /* use default input device? */
  /*---------------------------*/
  if (deviceID==-1)
  {
	  /* fprintf(stderr, "paPlay: no output device specified. using default.\n"); */
    paDI = Pa_GetDefaultOutputDevice();
	  if (paDI==paNoDevice)
	  {
      ERROR("paPlay: no default device!\n");
      return TCL_ERROR;
	  }
    deviceID = paDI;
	/* fprintf(stderr, "deviceID set to %i.\n", deviceID); */
  }

  /*--------------------------------------*/
  /* convert sampling rate from kHz to hz */
  /*--------------------------------------*/
  samplingRate = ROUND(feature->samplingRate*1000);
  if (samplingRate<=0)
  {
    ERROR("paPlay: invalid sampling rate %i!\n", samplingRate);
    return TCL_ERROR;
  }

  /*---------------------*/
  /* check sampling rate */
  /*---------------------*/
  if (samplingRate>100000)
	WARN("paPlay: sampling rate might be a little high (%f Hz).\n", samplingRate);

  /*------------------------------------*/
  /* calculate buffer length in seconds */
  /*------------------------------------*/
  if (startPos>=endPos)
  {
    ERROR("paPlay: startPos[%i]>=endPos[%i]!\n", startPos, endPos);
    return TCL_ERROR;
  }
  bufferLenInSeconds = (endPos - startPos) / samplingRate;
  

  /*-------------------*/
  /* create paInstance */
  /*-------------------*/
  paInstance = createPaInstance(featureSet, deviceID, PA_OUTPUT_DEVICE, 1, samplingRate,
                                bufferLenInSeconds, &featureName, NULL);
  if (paInstance==NULL)
  {
    ERROR("paPlay: paInstance is NULL.\n");
    return TCL_ERROR;
  }

  /*-------------*/
  /* fill Buffer */
  /*-------------*/
  paBuffer = paInstance->paBuffer;
  len = paBuffer->bufferLen;
  if ((feature->data.svec->n)<len)
    len = feature->data.svec->n;
  dp = paBuffer->data[0];
  sp = &(feature->data.svec->vecA[startPos]);
  for (i=0; i<len; i++)
    dp[i] = sp[i];
  for (i=len; i<paBuffer->bufferLen; i++)
    dp[i] = 0;

  /*-------------------*/
  /* open audio stream */
  /*-------------------*/
  paError = Pa_OpenStream(
          &(paInstance->stream),
          paInstance->inputStreamParams,
          paInstance->outputStreamParams,
          samplingRate,
          FRAMES_PER_BUFFER,
          paClipOff,
          paCallbackPlaying,
          paInstance
          ); 
  if (paError!=paNoError)
  {
    ERROR("paStart: could not open audio stream. %s\n", Pa_GetErrorText(paError));
    destroyPaInstance(paInstance);
    return TCL_ERROR;
  }

  /*-----------------*/
  /* start streaming */
  /*-----------------*/
  paError = Pa_StartStream(paInstance->stream);
  if (paError!=paNoError)
  {
    ERROR("paStart: could not start audio stream. %s\n", Pa_GetErrorText(paError));
    destroyPaInstance(paInstance);
    return TCL_ERROR;
  }

  return TCL_OK;
}

#endif
