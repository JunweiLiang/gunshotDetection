/* ========================================================================
               Speech tools
               ------------------------------------------------------------
               Object: audio IO
               ------------------------------------------------------------

    Author  :  Martin Westphal, Hermann Hild
    Module  :  featureADC.c
    Date    :  $Id: featureADC.c 3439 2011-07-28 20:54:04Z metze $
    Remarks :  This module can also be used as a stand alone program.
               Use the compiler option -DSTAND_ALONE.

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

===============================================================================

	writeADC(...)		write ADC data on file or stdout
	readADC (...)		read ADC from file

	old functions:
	adc_write(...)
	adc_read (...)



 Input/Output ADC file format:
   The ADC file contains an (optional) header, followed by the audio samples
   either stored as shorts (16bit) or bytes (8bit). Different formats like
   'linear' and  'ulaw' can be used.

 The following kind of headers are supported:

  1. 	"CMU-Header"
     	Archaic header which uses a struct, i.e. (only) machine readable data
	to store information. Flaw: This header contains information about
	the format of the shorts in the file, but this information is stored
	in a short itself!!
     	NOT RECOMMENDED for further use.

  2. 	"ASCII Header"
     	Use a header in a human readable format, in ASCII.
     	Example:
     		- - - - - - - - - - - - - -
 	    	KA_HEADER_BEGIN

	     	sampling_rate   16000
	     	recording_date  Aug_16_93
	     	short_type      01

	     	KA_HEADER_END
	     	- - - - - - - - - - - - - -

  3. 	No header at all, or skip n bytes (shorts in the old routine) instead
        of trying to read a header



  For more information read the comments above the functions.

  (based on adc_rw (Alleva) and changes by hild)

   ========================================================================

    $Log: featureADC.c,v $
    Revision 1.1.1.2  2008/09/10 17:01:44  metze.florian
    FFa+FMe

    Revision 1.2.4.1  2008/09/09 10:08:10  metze.florian
    Added '-append' to writeADC, added WAV & alaw support, cleaned up SPHERE

    Revision 1.2  2007/06/20 15:51:28  metze.florian
    Florian at T-Labs Nov 2006 - Jun 2007

    Revision 3.23  2007/03/02 13:03:17  fuegen
    bugfix in SPHERE code (Florain Metze)

    Revision 3.22  2005/11/08 13:14:40  metze
    Uninitialized access

    Revision 3.21  2005/09/29 13:42:33  metze
    Added support for multi-channel SPHERE files

    Revision 3.20  2005/03/04 09:11:20  metze
    Code for 'NFS-SSH' (commented out), fixes for SPHERE

    Revision 3.19  2004/09/20 15:54:51  fuegen
    enclosed foward declaration of readSphereData into SPHERE defines

    Revision 3.18  2004/09/16 14:36:00  metze
    Made code 'x86_64'-clean

    Revision 3.17  2004/09/02 17:14:26  metze
    Added sp_close (again!)

    Revision 3.16  2004/08/31 16:26:08  fuegen
    enclosed readSphereData into SPHERE defines

    Revision 3.15  2004/08/16 15:36:30  metze
    Made -from/-to work also for SPHERE files

    Revision 3.14  2004/05/27 12:26:46  metze
    Merged versions

    Revision 3.13  2004/05/27 11:19:45  metze
    Renamed SHORT_ to SHORT_NS (non-swap) and cleaned up the Windows mess

    Revision 3.12  2004/01/08 12:26:57  metze
    Added 'SPHERE' capabilities to 'readADC' (initial version)

    Revision 3.11  2003/08/14 11:19:52  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.10.4.12  2003/07/02 17:15:31  fuegen
    fixes for file handling under Windows
    using general functions for file open/close

    Revision 3.10.4.11  2003/06/06 07:57:18  fuegen
    made gcc 3.3 clean

    Revision 3.10.4.10  2003/05/08 16:41:51  soltau
    Purified!

    Revision 3.10.4.9  2003/02/28 16:16:22  metze
    Changes for compilation with SUN WSC6

    Revision 3.10.4.8  2002/10/31 11:07:34  metze
    Clearer error message

    Revision 3.10.4.7  2002/10/02 12:38:04  metze
    Reading of ulaw.shn (swb) works on SUN (swab problem)

    Revision 3.10.4.6  2002/09/06 15:00:11  metze
    Shorten now works with ULAW

    Revision 3.10.4.5  2002/08/02 13:37:21  metze
    Improved support for WAV files

    Revision 3.10.4.4  2002/06/26 11:57:53  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.10.4.3  2002/02/01 14:19:57  fuegen
    made it compilable under cygwin

    Revision 3.10.4.2  2001/11/19 14:46:55  metze
    Go away, boring message!

    Revision 3.10.4.1  2001/02/27 18:38:14  soltau
    Fixed clib2.2 compatibility problems

    Revision 3.10  2001/01/15 09:49:56  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 3.9  2001/01/10 16:58:36  janus
    Merged Florian's version, supporting e.g. WAV-Files, Multivar and NGETS

    Revision 3.8.4.1  2001/01/12 15:16:52  janus
    necessary changes for running janus under WINDOWS

    Revision 3.8  2000/11/14 12:01:12  janus
    Merged branch jtk-00-10-27-jmcd.

    Revision 3.7.16.1  2000/11/06 10:50:27  janus
    Made changes to enable compilation under -Wall.

    Revision 3.7  2000/10/05 12:01:49  janus
    Merged branch jtk-00-09-20-tschaaf.

    Revision 3.6.2.1  2000/10/05 11:53:08  janus
    Improved handling of byte-swap during reading ADCs via readADC

    Revision 3.6  2000/08/16 09:20:36  soltau
    better allocation calls

    Revision 3.5  1998/11/06 06:52:06  rogina
    fixed a segfault that happens on Linux when reading shorten-compressed files

    Revision 3.4  1998/04/17 15:30:41  westphal
    force flag for stand alone version

    Revision 3.3  1997/07/30 11:59:37  westphal
    'force' in readData and readADC

    Revision 3.2  1997/07/09 17:42:22  westphal
    *** empty log message ***


   ======================================================================== */
/*#define LIBSNDFILE   This line MUST BE deleted */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include "common.h"
#include "itf.h"
#include "featureADC.h"
#include "featureADCTables.h"
#include "error.h"
#include "feature.h"
#include "array.h"
#include "matrix.h"
#include <unistd.h>

#ifdef LIBSNDFILE
#include <sndfile.h>
#endif

#ifdef FFMPEG
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
int ffmpeg_registered = FALSE;
#endif

#ifdef SPHERE
#include <sp/sphere.h>
#endif

#if defined(LINUX) && !defined(CYGWIN)
void swab(void *from, void *to, int nbytes);		/* where is it defined on Linux		*/
#endif

/* ------- Constants ------------------------------------------------------- */
#define  KA_HEADER_BEGIN "KA_HEADER_BEGIN"  /* Begin marker for ascii header */
#define  KA_HEADER_END	 "KA_HEADER_END"      /* End marker for ascii header */
#define  MAX_LINES_IN_HEADER 256       /* max number of data lines in header */
#define  BYTE_ORDER_ID	 "byte_order"             /* byte order in KA header */
#define  NB_SAMPLES_ID	 "number_of_samples"            /* number of samples */
#define  SAMPL_RATE_ID	 "sampling_rate"                     /* samplin rate */

#define BUFFERSIZE 1024  /* used by memcpy durin shorten unpacking */


/* ------- Types ----------------------------------------------------------- */

typedef enum {
  UNDEF, SHORT_NS, SHORT_SWAP, SHORT_AUTO, ASCII, SHORTEN, LIN, ULAW, ALAW
} ByteModeType;

typedef struct {

  int          header;         /* 1 header with info, 0 no header          */
  int          samplRate;      /* sample rate in Hz                        */
  int          sampleN;        /* total samples in file                    */
  int          chN;            /* channels                                 */
  ByteModeType byteMode;       /* byte mode                                */
  int          remN;           /* number of remarks                        */
  char**       remark;         /* remarks                                  */

} ADCHeaderType;

typedef struct {

  int         smooth;           /* 0 not smooth, 1 smooth                   */
  int         byteOrder;        /* 0 '01' format, 1 '10' format             */
  int         sampleN;          /* total samples in file                    */
  int         sampleR;          /* samples read in                          */

} ADCDataType;

/* ------- local variables ------------------------------------------------- */
static int vb = 1;	              /* Global variable for verbosity level */
static char* byteModeADC[] = {
   "undefined",
   "01",
   "10",
   "auto",
   "ascii",
   "shorten",
   "lin",
   "ulaw",
   "alaw"      /* has to be last or change bmIntADC */
};
static ADCHeaderType headerADCin = {
   0,                    /* header       */
   0,                    /* samplRate    */
   0,                    /* sampleN      */
   0,                    /* chN          */
   UNDEF,                /* byteMode     */
   0,                    /* remN         , not used */
   NULL                  /* remark       , not used */
};
static ADCDataType dataADCin = {
   0,                    /* smooth       */
   0,                    /* byteOrder    */
   0,                    /* sampleN      */
   0                     /* sampleR      */
};
static ADCHeaderType headerADCout = {
   0,                    /* header       , not used */
   0,                    /* samplRate    */
   0,                    /* sampleN      */
   0,                    /* chN          , not used */
   SHORT_AUTO,           /* byteMode     */
   0,                    /* remN         */
   NULL                  /* remark       */
};


/* ----------------------------------------------------------------------------
;  Prototypes for local functions
; -------------------------------------------------------------------------- */
static int  write_shorts(short* adcA, int adcN, FILE* f);
static int  write_bytes(char* adcA, int adcN, FILE* f);

static long fbytes_left(FILE* fp);
static int  read_bytes(char* byteA, int max_byteN, FILE* fp);
static int  machine_10(void);
static void swap_shortA(short* adcA,  int adcN);

static int writeData(FILE *fp, short* adcA, int adcN, ByteModeType bm);
static int readADCuni(char* fname, short** adc, char* headMode, char* byteMode,
		      int chX, int chN, int from, int to, int runon, int init, int ready, int force
#ifdef LIBSNDFILE
		      , char* fformat, char* subtype, char* endianness
#endif
);

static int readHeaderByte(FILE *fp, char* byteN);
static int readHeaderCMU(FILE *fp);
static int readHeaderWAV(FILE *fp);
static int readHeaderPHON(FILE *fp);
#ifdef SPHERE
static int readHeaderSPHERE (char *);
#endif
#ifdef LIBSNDFILE
static 	int readHeaderLIBSNDFILE(char* fname, char* byteMode,int chX, int chN, int from, int to, int force,
							char* fformat, char* subtype, char* endianness, char* headmode, char* headmodeBackup);
#endif
#ifdef FFMPEG
static int readHeaderFFMPEG (char* fname);
#endif

static int readData(FILE *fp, short** adcAP, ByteModeType bm, int chX, int chN,
	     int from, int to, int force);
#ifdef SPHERE
static int readSphereData (FILE*, char*, short**, ByteModeType, int, int, int, int, int);
#endif
static int line_always_printed(char* line) ;
static int is_KA_head(char* sA);
static int read_KA_head( char* byteA,  int byteN,  char*** headSAP,
			int* headSN, ByteModeType* byte_order,  int* adcN,
			int* sampl_rate);
static int shorten (char *Buffi, char *Buffo, int buffoSize, int *byteV, int *chN, int *doSwab, ByteModeType* bmt);
static ByteModeType bmIntADC(char *byteMode);
static char* bmStringADC(ByteModeType bm);

#ifdef LIBSNDFILE
static int readADClibsnd(char* fname, short** adc, int chX, int chN, int from, int to, int force);
#endif

/*int readADClibsnd (char* fname, short** adc,
	       int chX, int chN, int from, int to, int force);*/

#ifdef FFMPEG
static int readADCFFMPEG(char* fname, short** adc, int chX, int chN, int from, int to, int force);
#endif

/* ----------------------------------------------------------------------------
;  Common macros:
; -------------------------------------------------------------------------- */
#define streq(s1,s2)    (strcmp (s1,s2) == 0) /* string equality    	*/
#define strneq(s1,s2)   (strcmp (s1,s2) != 0) /* string inequality  	*/
#define strempty(s1)    (strcmp (s1,"") == 0) /* string empty 	        */
#define MIN(a,b)	(((a)<(b))?(a):(b))   /* minimum	    	*/
/* absolute value of x	(fuegen) already defined in common.h */
/* #define ABS(x)		(((x)>=0)?(x):(-x)) */
#define TRUE		1
#define FALSE		0
#define SWAP_SHORT(x)   (short)((short)(0xFF00 & (short)((short)(x)<<8)) | \
				(short)(0x00FF & (short)((short)(x)>>8)))


#define SWAP_INT(x)     ((((x)&0x000000FF)<<24) | (((x)<<8)&0x00FF0000) | \
			 (((x)>>8) & 0x0000FF00) | (((x)>>24)&0x000000FF))

/*=============================================================================
; SWITCH statement for Strings
;
; Simulate a pseudo switch statement for strings.
;------------------------------------------------------------------------------
;  EXAMPLE:
;    Switch(name)
;        Case("Herm") {printf("Hello Hermann");}
;        Default      {printf("not in Case");}
;    End_switch;
;----------------------------------------------------------------------------*/
#define Switch(str) { char* strP = str;  int found = FALSE;
#define Case(str)   if (!found) if ((found = !strcmp(str, strP)))
#define Default     if (!found)
#define End_switch  }
#define Casen(str,num)   if (!found) if ((found = !strncmp(str, strP,num)))
/*---------------------------------------------------------------------------*/






/*=============================================================================
;  adc_verbosity
;
;  IN:
;	vb	verbosity level for the adc_read and adc_write function
;		0 = no infos, totalluu quiet
;		1 = very basic infos (one or two lines)
;		2 = basic infos,
;		3 = debugging info
;		Errors and warnings are always reported.
;
;----------------------------------------------------------------------------*/
void  adc_verbosity (int verbosity) {   vb = verbosity;   }
/*---------------------------------------------------------------------------*/

/* ============================================================================
;  Use these functions to 'set' information to be stored in the header
;  or to 'get' information that was read from a header
;  ========================================================================= */
/*----write------------------------------------------------------------------*/
void setADCremarks(int remN, char** remark)
{
   headerADCout.remN = remN;
   headerADCout.remark = remark;
}

void setADCsamplRate(int samplRate)     { headerADCout.samplRate = samplRate;  }
void setADCsamplN   (int sampleN)       { headerADCout.sampleN   = sampleN;  }
void setADCbyteMode (char*  byteModeS)  { headerADCout.byteMode  = bmIntADC(byteModeS); }

/*----read-------------------------------------------------------------------*/
int getADCremarks(int* remN, char*** remark)
{
   *remN   = headerADCin.remN;
   *remark = headerADCin.remark;
   return headerADCin.header;
}

int getADCsamplRate(int *samplRate)
{
   *samplRate = headerADCin.samplRate;
   return headerADCin.header;
}

int getADCsamplN(int *samplN)
{
   *samplN = headerADCin.sampleN;
   return headerADCin.header;
}

int getADCbyteMode(char** byteMode)
{
   *byteMode = bmStringADC(headerADCin.byteMode);
   return headerADCin.header;
}

int isADCsmooth(void)   { return dataADCin.smooth; }
int isADCswapped(void)  { return dataADCin.byteOrder; }

/*---------------------------------------------------------------------------*/
/* -------------------------------------------------------------------------
;  reset header values before reading
; -------------------------------------------------------------------------- */
static void resetHeaderADCin(void)
{
   headerADCin.header    = 0;     /* no header          */
   headerADCin.samplRate = 0;     /* no sampling rate   */
   headerADCin.sampleN   = 0;     /* no samples         */
   headerADCin.chN       = 0;     /* no channels        */
   headerADCin.byteMode  = UNDEF; /* undefined byteMode */
   if (headerADCin.remark)
      free(headerADCin.remark);   /* pointers and strings are in one block */
   headerADCin.remN      = 0;
   headerADCin.remark    = NULL;
}
/* -------------------------------------------------------------------------
;  bmStringADC: convert 'ByteModeType' to 'byteMode' string
; -------------------------------------------------------------------------- */
static char* bmStringADC(ByteModeType bm)
{
   if (machine_10()) {
      if (bm == SHORT_NS) bm = SHORT_SWAP;
      else if (bm == SHORT_SWAP) bm = SHORT_NS;
   }
   return byteModeADC[bm];
}

/* -------------------------------------------------------------------------
;  bmIntADC: convert 'byteMode string' to 'ByteModeType'
; -------------------------------------------------------------------------- */
static ByteModeType bmIntADC(char *byteMode)
{
   ByteModeType bm;
	for ( bm=SHORT_NS; bm<=ALAW; bm++) {
		if (streq( byteModeADC[bm], byteMode)) {
			if (machine_10()) {
				if (bm == SHORT_NS) bm = SHORT_SWAP;
				else if (bm == SHORT_SWAP) bm = SHORT_NS;
			}
			return bm;
		}
	}
  return UNDEF;
}

/* ============================================================================
;  Old adc_readwrite.c functions
;  ========================================================================= */
/* not necessary, but maybe still called */
/*
void  adc_maxbuf (int size)
{}
*/

int  adc_write (char* fname, short* adcA, int adcN, char* head_mode,
		char* byte_order, char** headSA, int headSN, int sampl_rate)
{
   setADCremarks(headSN, headSA);
   setADCsamplRate(sampl_rate);
   return writeADC(fname, adcA, adcN, head_mode, byte_order, sampl_rate, 0);
}





/* ######################################################################### */
/*                             W R I T E                                     */
/* ######################################################################### */


/* ============================================================================
;  writeADC
;
;  IN/OUT:
;	fname	     : Name of file on which ADC data are written.
;		       If fname == "-", output is written on stdout.
;	adcA	     : Pointer to Array with shorts, contains ADC data
;	adcN	     : Number of elements in adcA.
;	headMode     : String to define mode of header:
;	   "KA"   	write a KA header at beginning of file
;	   ""     	No header at all
;	   "RUNON"	No header, runon mode (see below)
;	byteMode     : String to define the way shorts are interpreted
;	   "10"	  write shorts with most significant byte first
;	   "01"	  write shorts with least significant byte first
;	   "auto" write shorts as seen by current machine.
;	   "lin"  write in 8bit linear format
;	   "ulaw" write in 8bit ulaw format
;	   "ascii" write in text format
;
;
;  "RUNON" mode:
;   writeADC can be called several time. In the first call, a file pointer is opened.
;   in successive calls, more data are written to the same file pointer. To close the
;   file, writeADC has to be explicitely called with adcN <= 0.
;   The HeadMode is misused to indicate that we are in runon mode. Therefore it
;   it not possible to write Headers in runon mode.
;
;
;  RETURN:
;	   The number of successfully written samples,
;	   or -1 if unsuccessfull
; ========================================================================== */
int writeADC(char* fname, short* adcA, int adcN, char* headMode,
	     char* byteMode, int samplingRate, int append)
{
   static FILE* fp = NULL;
   ByteModeType bm = bmIntADC(byteMode);

   /*--------------------------------------------------------
   ;  0. byte mode
   ;-------------------------------------------------------*/
   if (bm == SHORT_AUTO) bm = SHORT_NS;
   if (bm != SHORT_NS && bm != SHORT_SWAP &&
       bm != LIN   && bm != ULAW       && bm != ASCII ) {
      fprintf(stderr, "ERROR:adcWrite: don't know byte mode %s\n",byteMode);
      return -1;
   }

   /*--------------------------------------------------------
   ;  1. open file
   ;-------------------------------------------------------*/
   if (!streq(headMode, "RUNON") || fp == NULL)  {
     /* when in RUNON mode, open only if fp == NULL	*/
     fp = (streq(fname, "-")) ? stdout : (append ? fileOpen(fname, "r+") : fileOpen(fname, "w"));

     if (fp == NULL) {
       fprintf(stderr, "ERROR:adcWrite: Couldn't open file (%s)\n", fname);
       return -1;
     }
   }
   if (append) fseek (fp, 0, SEEK_END);

   /*--------------------------------------------------------
   ;  2. write header
   ;-------------------------------------------------------*/
   headerADCout.byteMode  = bm;
   headerADCout.sampleN   = adcN;
   headerADCout.samplRate = samplingRate;

   /* do not write a header if we're in append mode */
   if (!ftell (fp)) {
     Switch(headMode)
     Case("KA")	   { writeHeaderKA(fp); }
     Case("WAV")   { writeHeaderWAV(fp); }
     Case("RUNON") { /* no header in RUNON mode	*/ }
     Case("")      { }
     Default {
       fprintf(stderr, "ERROR:adcWrite: unknown header mode (%s)\n", headMode);
       return -1;
     }
     End_switch;
   } else if (!strcmp (headMode, "WAV")) {
     /* Update the WAV header */
     int bps = (headerADCout.byteMode == ULAW || headerADCout.byteMode == ALAW) ? 1 : 2;
     int l, m;

     /* read the existing length */
     fseek (fp, 40, SEEK_SET);
     read_bytes ((char*) &l, 4, fp);

     /* add our data */
     l += bps * adcN;
     fseek (fp, 40, SEEK_SET);
     write_bytes ((char*) &l, 4, fp);

     /* check */
     fseek (fp, 40, SEEK_SET);
     read_bytes ((char*) &m, 4, fp);
     if (l != m)
       fprintf (stderr, "ERROR: adcWrite: updating header failed");

     /* update the header */
     l += 36;
     fseek (fp, 4,  SEEK_SET);
     write_bytes ((char*) &l, 4, fp);

     /* go to the end */
     fseek (fp, 0, SEEK_END);
   }

   /*--------------------------------------------------------
   ;  3. write data
   ;-------------------------------------------------------*/
   adcN =  writeData( fp, adcA, adcN, bm);

   /*--------------------------------------------------------
   ;  4. close file and return
   ;-------------------------------------------------------*/
   if (!streq(headMode, "RUNON") || adcN <= 0) {
     /* in RUNON mode, close only if adcN <= 0	*/
     if (fp != stdout) fileClose (fname, fp);
     fp = NULL; /* reset for next call		*/
   }

   return adcN;
}



/* -------------------------------------------------------------------------
;  writeHeaderXXX
;
;  All the write header function should look the same:
;     static int writeHeaderXXX(FILE *fp)
;
;  IN/OUT:
;         fp          file pointer to beginning of file
;
;  RETURN:  0         couldn't write header
;           1         ok
;  NOTE:              the information for the header can be obtained from the
;                     'headerADCout' structure
; -------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------
;  writeHeaderKA
; -------------------------------------------------------------------------- */
int writeHeaderKA(FILE *fp)
{
   int  lineX;
   char line[1000];
   int  len = 0;

   sprintf(line, "%s\n", KA_HEADER_BEGIN);
   len += strlen(line); fprintf(fp, "%s", line);
   sprintf(line, "%s  %d\n", NB_SAMPLES_ID, headerADCout.sampleN);
   len += strlen(line); fprintf(fp, "%s", line);
   sprintf(line, "%s  %s\n", BYTE_ORDER_ID,bmStringADC(headerADCout.byteMode));
   len += strlen(line); fprintf(fp, "%s", line);
   sprintf(line, "%s  %d\n", SAMPL_RATE_ID, headerADCout.samplRate);
   len += strlen(line); fprintf(fp, "%s", line);
   for (lineX=0;  lineX < headerADCout.remN;  lineX++) {
      char* str = headerADCout.remark[lineX];
      if (!line_always_printed(str)) {
	 /* dont print header begin/end, number_of_samp etc. again	*/
	 len += strlen(str) + 1;   /* length of string + the newline char */
	 fprintf(fp, "%s\n", str);
      }
   }
   sprintf(line, "%s\n", KA_HEADER_END);
   len += strlen(line);
   if (len%2 != 0) { len += 1; fprintf(fp, "\n"); } /* insert a newline to
						       get an even number
						       of bytes in header */
   fprintf(fp, "%s", line);

   if (vb >= 2) fprintf(stderr,"writeAdc: length of KA header = %d\n", len);
   return 1;
}


/* -------------------------------------------------------------------------
;  writeHeaderWAV
; -------------------------------------------------------------------------- */
int writeHeaderWAV (FILE *fp)
{
  int bps = (headerADCout.byteMode == ULAW || headerADCout.byteMode == ALAW) ? 1 : 2;

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

  /* values */
  strncpy (head.n,    "RIFF", 4);
  strncpy (head.t,    "WAVE", 4);
  strncpy (wavfmt.n,  "fmt ", 4);
  strncpy (wavdata.n, "data", 4);
  wavdata.len     = bps * headerADCout.sampleN;
  head.len        = 36  + wavdata.len;

  /* I think the format should be 5 for ulaw and 6 for alaw (Florian Metze) */

  wavfmt.len      = 16;
  wavfmt.format   = 1;
  wavfmt.chans    = 1;
  wavfmt.srate    = headerADCout.samplRate;
  wavfmt.bpsec    = headerADCout.samplRate*bps;
  wavfmt.bpsample = bps;
  wavfmt.bpchan   = bps*8;

  if (vb >= 2) {
    fprintf (stderr, "writeADC WAV HEAD: length=%d \n", head.len);
    fprintf (stderr, "writeADC WAV FMT : length=%d \n", wavfmt.len);
    fprintf (stderr, "writeADC WAV FMT : format=%d chans=%d srate=%d bpsec=%d bpsample=%d bpchan=%d\n",
	     wavfmt.format, wavfmt.chans, wavfmt.srate, wavfmt.bpsec, wavfmt.bpsample, wavfmt.bpchan);
    fprintf (stderr, "writeADC WAV DATA: length=%d \n", wavdata.len);
 }

  /* write data */
  write_bytes ((char*) &head,    12, fp);
  write_bytes ((char*) &wavfmt,  24, fp);
  write_bytes ((char*) &wavdata,  8, fp);

  return 1;
}


/* -------------------------------------------------------------------------
;  writeADC_fp
;
;  For RUNON writing, we want to write data not in onle large, but in
;  many small blocks.
; -------------------------------------------------------------------------- */
int writeADC_fp(FILE* fp, short* adcA, int adcN, char* byteMode)
{
   ByteModeType bm = bmIntADC(byteMode);
   int	adcN2;

   /*--------------------------------------------------------
   ;  0. byte mode
   ;-------------------------------------------------------*/
  if (bm == SHORT_AUTO) bm = SHORT_NS;
	if (bm != SHORT_NS && bm != SHORT_SWAP &&
       bm != LIN   && bm != ULAW       && bm != ASCII ) {
      fprintf(stderr, "ERROR:writeData_fp: don't know byte mode %s\n",byteMode);
      return -1;
   }

   adcN2 = writeData(fp, adcA, adcN, bm);
   fflush(fp);						/* runon is time critical	*/
   return adcN2;
}


/* -------------------------------------------------------------------------
;  writeData
; -------------------------------------------------------------------------- */
static int writeData(FILE *fp, short* adcA, int adcN, ByteModeType bm)
{
   char* buffer = NULL;

   if (adcN <= 0) return 0;
   /*--------------------------------------------------------
   ;  allocate memory for 8bit formats
   ;-------------------------------------------------------*/
   if (bm == LIN || bm == ULAW) {
      if ((buffer = (char*) malloc(adcN)) == NULL) {
	 fprintf(stderr, "ERROR:adcWrite: couldn't allocate memory\n");
	 return -1;
      }
   }

   switch (bm) {
    case SHORT_NS:
      adcN = write_shorts(adcA, adcN, fp);      break;
    case SHORT_SWAP:
      swap_shortA(adcA, adcN);
      adcN = write_shorts(adcA, adcN, fp);
      swap_shortA(adcA, adcN);
      break;
    case LIN:
      {  int i;
	 for (i=0; i<adcN; i++) buffer[i] = (adcA[i]/256) & 0x00ff;
	 write_bytes(buffer, adcN, fp);
      }  break;
    case ULAW:
      {  int i;
	 for (i=0; i<adcN; i++) buffer[i] = AF_comp_u[(adcA[i] >> 2) & 0x3fff];
	 write_bytes(buffer, adcN, fp);
      }  break;
    case ASCII:
      {  int i;
	 for (i=0; i<adcN; i++) {
	    if ((i%10) == 9) fprintf(fp,"%6hd\n",adcA[i]);
	    else             fprintf(fp,"%6hd ", adcA[i]);
	 }
	 if (i%10) fprintf(fp,"\n");
      }  break;
    default: return -1;
   }

   if (buffer) free(buffer);
   return adcN;
}


/* ######################################################################### */
/*                               R E A D                                     */
/* ######################################################################### */

/* ----------------------------------------------------------------------------
;   NOTE DIFFERENCES:
;     head_mode    : If the string can be interpreted as an integer x,
;		     first x  s_h_o_r_t_s are skipped.
;                    The new function uses bytes instead.
;     byte_order    : 'llaw' is 'lin' in the new functions.
;
;     The selected channel is 1..chN in the new function whereas it
;     used to be 0..chN-1 in the old. 0 now means all channels.
; -------------------------------------------------------------------------- */


/* ============================================================================
;  readADC	      without runon and ready
;  readADCrunon	      without from and to, in runon mode
;
;  readADCuni
;  IN/OUT:
;       fname         name of file to be read
;  (*)  adc           short pointer to data, memory will be allocated
;       headMode      string to define header mode:
;	  "KA"   : try to read Karlsruhe style header (ascii text)
;	  "CMU"  : try to read the CMU header (archaic type, should be avoided)
;	  "PHON" : try to read a Phone_dat header (Siemens, ASL conventions)
;	  "auto" : try to automatically identify one of the above headers
;	  "x"    : if the string "x" can be interpreted as an integer x,
;		   then any kind of header is ignored, and the
;		   first x bytes are skipped;
;	byteMode      string to define the way data is stored in the file
;	  ""	  : Leave decision to the header
;	  "10"	  : Assume the most significant byte is the first byte
;	  "01"	  : Assume the most significant byte is the second byte
;	  "auto"  : Try to automatically determine the correct order
;	  "lin"   : samples are in bytes
;	  "ulaw"  : samples are in bytes (u-law format)
;	  "alaw"  : samples are in bytes (a-law format)
;       chX          see readData()
;       chN               "
;       from              "
;       to                "
;       runon        runon mode
;       init         force close old file and open new file
;       ready        data complete (for runon mode), close file
;       force        see readData()
;  RETURN:       number of samples read = to - from + 1 or less if
;                'to' exceeds sampleN-1.
;                -1 if something was wrong
;  NOTE:         more infos can be obtained from other functions using
;                the static structures headADCin and dataADCin
; ========================================================================== */
int readADC(char* fname, short** adc, char* headMode, char* byteMode,
	       int chX, int chN, int from, int to, int force
#ifdef LIBSNDFILE
, char* fformat, char* subtype, char* endianness
#endif
)
{
  return readADCuni( fname, adc, headMode, byteMode, chX, chN, from, to, 0, 1, 1, force
#ifdef LIBSNDFILE
, fformat, subtype, endianness 
#endif
);
}

int readADCrunon(char* fname, short** adc, char* headMode, char* byteMode,
	       int chX, int chN, int init, int ready
#ifdef LIBSNDFILE
, char* fformat, char* subtype, char* endianness
#endif
)
{
  return readADCuni( fname, adc, headMode, byteMode, chX, chN, 0, -1, 1, init, ready, 0
#ifdef LIBSNDFILE
, fformat, subtype, endianness
#endif
);
}

#ifdef FFMPEG
static int ffmpegSampleSize (AVCodecContext* pCodecCtx) {
  int size;
  switch (pCodecCtx->sample_fmt) {
  case SAMPLE_FMT_U8:
    size = 1; break;
  case SAMPLE_FMT_S16:
    size = 2; break;
  case SAMPLE_FMT_S32:
  case SAMPLE_FMT_FLT:
    size = 4; break;
  case SAMPLE_FMT_DBL:
    size = 8; break;
  default:
    size = 0; break;
  }
  return size;
}

#ifndef NEW_FFMPEG
static int readADCFFMPEG (char* fname, short** adc, int chX, int chN, int from, int to, int force) {
  AVFormatContext *pFormatCtx = NULL;
  int             audioStream = 0;
  AVCodecContext  *pCodecCtx  = NULL;
  AVCodec         *pCodec     = NULL;
  AVPacket        packet;
  short*          audio_out = NULL;
  //void* dataP = NULL;
  int audio_len = AVCODEC_MAX_AUDIO_FRAME_SIZE;
  int len        = 0;
  int packet_ok  = 0;
  int packet_not = 0;
  int allocated  = 0;
  int size       = 0;
  int del_eof    = 0;
  int ok, res;

  // sanity check;
  if (to > 0 && to <= from)
    to = -1;
  if (chN != headerADCin.chN)
    chN = headerADCin.chN;
  if (chX <= 0 || chX > chN)
    chX = 1;

  // Initialization (see read header)
  if (!ffmpeg_registered) av_register_all();
  ffmpeg_registered = TRUE;
  //if (av_open_input_file (&pFormatCtx, fname, NULL, 0, NULL) != 0) return -1; 
  if (avformat_open_input (&pFormatCtx, fname, NULL, NULL) != 0)
    return -1;
  if (av_find_stream_info (pFormatCtx) < 0) return -1;
  for (audioStream = 0; audioStream < pFormatCtx->nb_streams && 
	 pFormatCtx->streams[audioStream]->codec->codec_type != CODEC_TYPE_AUDIO; 
       audioStream++);
  if (audioStream == pFormatCtx->nb_streams) return -1; 
  pCodecCtx = pFormatCtx->streams[audioStream]->codec;
  pCodec    = avcodec_find_decoder (pCodecCtx->codec_id);
  if (!pCodec) return -1;
  if (avcodec_open (pCodecCtx, pCodec) < 0) return -1;
  size = ffmpegSampleSize (pCodecCtx);

  /* reserve an audio pointer (estimate not correct, sometimes) */
  if (!((*adc) = (short*) malloc (allocated = (sizeof (short) * headerADCin.sampleN)))) {
    INFO ("readADCFFMPEG: can't allocate %d bytes\n", headerADCin.sampleN);
    return -1;
  }
  audio_out = av_malloc (audio_len);

  while ((res = av_read_frame (pFormatCtx, &packet)) >= 0 || !del_eof) {
    // EOF handling (do loop one last time)
    if (res < 0)
      del_eof = 1;

    // is this packet from the audio stream? 
    if (packet.stream_index != audioStream) {
      av_free_packet (&packet);
      continue;
    }

    // decode the packet completely
    //dataP = packet.data;
    while (packet.size || del_eof) {
      int outsize = audio_len;
      // decode the packet
      if ((ok = avcodec_decode_audio3 (pCodecCtx, audio_out, &outsize, &packet)) < 0) {
      	INFO ("readADCFFMPEG: error decoding the audio.\n"); 
	packet_ok--;
	packet_not++;
	break;
      }

      // do we have enough space to copy (in bytes)
      if (allocated < len * size + AVCODEC_MAX_AUDIO_FRAME_SIZE)
	*adc = realloc (*adc, (allocated += AVCODEC_MAX_AUDIO_FRAME_SIZE));

      // copy the data, format conversion
      switch (pCodecCtx->sample_fmt) {
      case SAMPLE_FMT_U8: {
	int             i;
	signed   short* t = (*adc) + len;
	unsigned char*  s = (unsigned char*) audio_out; 
	for (i = 0; i < outsize; i++)
	  t[i] = (s[i*chN+(chX-1)]-128)*256;
        }
	break;
      case SAMPLE_FMT_S16:
	if (chN == 1) 
	  memcpy (*adc + len, audio_out, outsize);
	else {
	  int    i;
	  short* t = (*adc) + len;
	  for (i = 0; i < outsize/2; i++)
	    t[i] = audio_out[i*chN+(chX-1)];
	}
	break;
      default:
	INFO ("readADCFFMPEG: sample format not implemented.\n");
	break;
      }
      len += outsize / size / chN;

      // exit for EOF - no more data
      if (del_eof && !outsize)
	break;

      packet.data += ok;
      packet.size -= ok;
    }

    // free memory (not on 'packet'?)
    //printf ("... %p, %d %d\n", dataP, packet.size, del_eof);
    //av_free (dataP); // this may fail - so don't use

    // need no more data
    if (to > 0 && len > to)
      break;

    // last packet is 'virtual'
    if (!del_eof)
      packet_ok++;
  }

  // set values
  dataADCin.smooth    = -1;
  dataADCin.byteOrder = SHORT_NS;

  // cut interesting part
  if (from > 0 || (to > 0 && to != len-1)) {
    short* tmpP;
    if (to < 0) to = headerADCin.sampleN-1;
    dataADCin.sampleR = 1+to-from;
    tmpP = (short*) malloc (sizeof (short) * dataADCin.sampleR);
    memcpy (tmpP, *adc + from, sizeof(short) * dataADCin.sampleR);
    free (*adc);
    *adc = tmpP;
  } else {
    dataADCin.sampleR = len;
    dataADCin.sampleN = len;
  }

   /* --- verbosity --- */
  if (vb >= 2) fprintf (stderr, "readADCFFMPEG: %d packets ok and %d not\n", 
			packet_ok, packet_not);
  if (vb >= 1) fprintf (stderr,
			"readADCFFMPEG: read %d of %d samples, %d .. %d, channel %d of %d\n",
			dataADCin.sampleR, dataADCin.sampleN, from, to, chX, chN);

  // free & close
  av_free (audio_out);
  avcodec_close (pCodecCtx);
  av_close_input_file (pFormatCtx);

  return dataADCin.sampleR;
}
#else
/* this part does only work for audio data, not video data
   we could probably delete it
 */
#define AUDIO_INBUF_SIZE 32768
#define AUDIO_REFILL_THRESH 4096

static int ffmpeg_copy (short** adc, short* audio_out, int outsize, int chX, int chN, 
			AVCodecContext* pCodecCtx, int allocated, int len, int size, int offset)
{
  // do we have enough space to copy (in bytes)
  if (allocated < len * size + AVCODEC_MAX_AUDIO_FRAME_SIZE)
    *adc = realloc (*adc, (allocated += AVCODEC_MAX_AUDIO_FRAME_SIZE));
  
  // copy the data, format conversion
  switch (pCodecCtx->sample_fmt) {
  case SAMPLE_FMT_U8: {
    int i;
    signed   short* t = (*adc) + len;
    unsigned char*  s = ((unsigned char*) audio_out) + chX-1+offset;
    for (i = 0; i < outsize-offset; i++)
      t[i] = (s[i*chN]-128)*256;
  }
    break;
  case SAMPLE_FMT_S16:
    if (chN == 1) 
      memcpy (*adc + len, audio_out + offset/size, outsize - offset);
    else {
      int i;
      short* t = (*adc) + len;
      for (i = 0; i < (outsize-offset)/2; i++)
	t[i] = audio_out[i*chN+(chX-1+offset/2)];
    }
    break;
  default:
    INFO ("readADCFFMPEG: sample format not implemented.\n");
    break;
  }

  return allocated;
}

static int readADCFFMPEG (char* fname, short** adc, int chX, int chN, int from, int to, int force) {
  AVCodec        *codec = NULL;
  AVCodecContext     *c = NULL;
  AVFormatContext *avfc = NULL;
  int out_size, len = 0, audioStream, size, allocated = 0, total = 0, offset;
  FILE *f;
  uint8_t *outbuf;
  uint8_t inbuf[AUDIO_INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
  AVPacket avpkt;

  // initialize
  if (!ffmpeg_registered) {
    ffmpeg_registered = TRUE;
    av_register_all ();
    avcodec_init ();
    avcodec_register_all ();
  }
  av_init_packet (&avpkt);

  // opening
  if (av_open_input_file (&avfc, fname, NULL, 0, NULL) != 0) return -1; 
  if (av_find_stream_info (avfc) < 0) return -1;
  for (audioStream = 0; audioStream < avfc->nb_streams && 
	 avfc->streams[audioStream]->codec->codec_type != CODEC_TYPE_AUDIO; 
       audioStream++);
  if (audioStream == avfc->nb_streams) return -1; 
  c = avfc->streams[audioStream]->codec;
  codec = avcodec_find_decoder (c->codec_id);
  if (!codec) return -1;
  if (avcodec_open (c, codec) < 0) return -1;

  // a few more settings
  f = fopen (fname, "rb");
  size = ffmpegSampleSize (c);  
  outbuf = malloc (AVCODEC_MAX_AUDIO_FRAME_SIZE);
  offset = avfc->data_offset;

  /* reserve an audio pointer (estimate not correct, sometimes) */
  if (!((*adc) = (short*) malloc (allocated = (sizeof (short) * headerADCin.sampleN)))) {
    INFO ("readADCFFMPEG: can't allocate %d bytes\n", headerADCin.sampleN);
    return -1;
  }
  
  /* decode until eof */
  avpkt.data = inbuf;
  avpkt.size = fread (inbuf, 1, AUDIO_INBUF_SIZE, f);
  while (avpkt.size > 0) {
    out_size = AVCODEC_MAX_AUDIO_FRAME_SIZE;
    if ((len = avcodec_decode_audio3 (c, (short*) outbuf, &out_size, &avpkt)) < 0) {
      fprintf (stderr, "Error while decoding\n");
      break;
    }
    if (out_size > 0) {
      /* if a frame has been decoded, output it */
      allocated = ffmpeg_copy (adc, (short*) outbuf, out_size, chX, chN, c, 
			       allocated, total, size, offset);
      total += (out_size-offset)/chN;
      offset = 0;
    }
    avpkt.size -= len;
    avpkt.data += len;
    if (avpkt.size < AUDIO_REFILL_THRESH) {
      /* Refill the input buffer, to avoid trying to decode incomplete frames. 
	 Instead of this, one could also use a parser, or use a proper container 
	 format through libavformat. */
      memmove (inbuf, avpkt.data, avpkt.size);
      avpkt.data = inbuf;
      len = fread (avpkt.data + avpkt.size, 1, AUDIO_INBUF_SIZE - avpkt.size, f);
      if (len > 0)
	avpkt.size += len;
    }
  }

  // set values
  dataADCin.smooth    = -1;
  dataADCin.byteOrder = SHORT_NS;

  // cut interesting part
  len = total/size;
  if (from > 0 || (to > 0 && to != len-1)) {
    short* tmpP;
    if (to < 0) to = headerADCin.sampleN-1;
    dataADCin.sampleR = 1+to-from;
    tmpP = (short*) malloc (sizeof (short) * dataADCin.sampleR);
    memcpy (tmpP, *adc + from, sizeof(short) * dataADCin.sampleR);
    free (*adc);
    *adc = tmpP;
  } else {
    dataADCin.sampleR = len;
    dataADCin.sampleN = len;
  }

  // free
  av_free_packet (&avpkt);
  free (outbuf);
  fclose (f);
  avcodec_close (c);
  av_free (c);

  return dataADCin.sampleR;
}
#endif
#endif

/*---------------------------------------------------------------------------
* Reading the file (FLAC format) using libsnd library.
---------------------------------------------------------------------------*/
#ifdef LIBSNDFILE
 static int readADClibsnd(char* fname, short** adc,
	       int chX, int chN, int from, int to, int force){
	 int ok;/* The length of the array adc with data*/
	/*variables initialization */
	int i =0;   /* The var which holds result of format checking */
	int err =0; /* Var which holds the current opened file status */
	SNDFILE * file ;
	SF_INFO info;
	sf_count_t readedfile;

	/*implementation*/
	/*printf("* The filename from the procedure readADClibsnd is %s \n\n", fname); delete this line later!*/
	/* When opening a file for read, the format field should be set to zero before calling sf_open(). The only exception to this is the case of RAW files where the caller has to set the samplerate, channels and format fields to valid values. All other fields of the structure are filled in by the library. */
	info.format   = 0; 
	info.channels = 0;
	i = sf_format_check(&info);
	file = sf_open(fname, SFM_READ, &info);

	err=sf_error(file);


	dataADCin.sampleN = info.frames;

	/*  if (!(*adc = (short*) malloc (dataADCin.sampleN * sizeof (short)))) {
	    WARN ("readADClibsnd: memory problem (adc=%d bytes).\n", dataADCin.sampleN * sizeof (short));
	    return 0;
	  }*/
	*adc = (short*) malloc (dataADCin.sampleN * sizeof (short)*info.channels);
	readedfile=sf_readf_short(file, *adc, info.frames);
	dataADCin.sampleR = (int)readedfile;
	ok = readedfile;

	/*
	 * Read a piece of data from "from" to "to"
	 */

	if ((from > 0 || to < dataADCin.sampleN) && to > from) {
		/* We have to cut a piece... */
		short* adc2;
		int i;

		ok = to - from + 1;

		if (!(adc2 = (short*) malloc (ok * sizeof (short)*info.channels))) {
			WARN ("readADC: memory problem (adc2=%d bytes).\n", ok * sizeof (short)*info.channels);
			return 0;
		}

		for (i = 0; i <= to-from; i++)
			adc2[i] = (*adc)[i+from];

		ok=to-from;


		free (*adc);
		*adc = adc2;



	}
	else
	{
	  if (to != -1) fprintf(stderr, "* The from: %d and to: %d specified incorrectly.\n",from,to);
	}

	/*
	 * Extracts the data of the user specified channel
	 */


	if (chX>0 && chX <= info.channels)
	{

		short* adc3;
		int len;

		if (!(adc3 = (short*) malloc (ok * sizeof (short)*info.channels))) {
					WARN ("readADC: memory problem (adc3=%d bytes).\n", ok * sizeof (short)*info.channels);
					return 0;
				}
		len =0;


/*
		 ------ Show the original data----------
		for (i =1; i <=520; i++)
		{
			if (i==1) fprintf(stderr, "* adc:\n");

		if (i==520) fprintf(stderr,"\n========================================================\n");
		}
		--------------------------------------
*/

		len=0;
		for (i =0; i < ok; i++)
		{
/*//			if (i==0) fprintf(stderr, "* adc3:\t");*/

			adc3[len] = (*adc)[i*info.channels+chX-1];
			len = len+1;
		}
/*//		fprintf(stderr, "\nThe array size of the selected chanel is %d  \n", len);*//*delete this line later!*/

		ok=len;

		free (*adc);
		*adc = adc3;

	}
	else
	{
		fprintf(stderr,"ERROR: The number of the selected channel chX: %d is wrong! \n", chX);
		fprintf(stderr,"Exit...\n");
		return -1;
	}

	/*close file*/
	i= sf_close(file);
	return ok;
}
#endif

static int readADCuni(char* fname, short** adc, char* headMode, char* byteMode,
		      int chX, int chN, int from, int to, int runon, int init, int ready, int force
#ifdef LIBSNDFILE
		      , char* fformat, char* subtype, char* endianness
#endif
		      )
{
   int ok = 1;
   int isPHON = 0;
   int isWAV  = 0;
#ifdef SPHERE
   int isSPHERE = 0;
#endif
#ifdef LIBSNDFILE
   int isFLAC = 0;
   char* headModeBackup=headMode;
   /*headMode = "auto";*/
#endif
#ifdef FFMPEG
   int isFFMPEG = 0;
#endif

  ByteModeType bm = SHORT_AUTO;
   static FILE* fp = NULL;

   if (init && fp) { fileClose(fname, fp); fp = NULL; }
   if (!runon || fp==NULL) {
     /*--------------------------------------------------------
     ;  0. open file
     ;-------------------------------------------------------*/
     if (strempty(fname) || (fp=fileOpen(fname, "rb")) == NULL) {
       //fprintf(stderr, "ERROR:adc_read: Couldn't open file (%s)\n", fname);
       //perror(NULL);
       return(-1);
     }

     /*--------------------------------------------------------
     ;  1. read header
     ;-------------------------------------------------------*/
     resetHeaderADCin();

     Switch(headMode)
     /* Switches which are used to open the file *
        * without usage of libsndfile 		     */
       Case("KA")   {
	 if ((ok = readHeaderKA(fp)) < 0)
	   fprintf(stderr,"ERROR:adcRead: couldn't read KA header\n");
       }
       Case("CMU")   {
	 if ((ok = readHeaderCMU(fp)) < 0)
	   fprintf(stderr,"ERROR:adcRead: couldn't read CMU header\n");
       }
       Case("WAV") {
	 if ((ok = readHeaderWAV(fp)) < 0)
	   fprintf(stderr,"ERROR:adcRead: couldn't read WAV header\n");
	 else isWAV = 1;
       }
#ifdef FFMPEG
       Case("FFMPEG") {
	 if ((ok = readHeaderFFMPEG (fname)) < 0)
	   fprintf (stderr, "ERROR:adcRead: couldn't read FFMPEG header\n");
	 else
	   isFFMPEG = 1;
       }
#endif
#ifdef SPHERE
       Case("SPHERE") {
	 if ((ok = readHeaderSPHERE (fname)) < 0)
	   fprintf(stderr,"ERROR:adcRead: couldn't read SPHERE header\n");
	 else isSPHERE = 1;
	 /* For more information on what is happening */
       }
#endif
       Case("PHON")   {
	 if ((ok = readHeaderPHON(fp)) < 0)
	   fprintf(stderr,"ERROR:adcRead: couldn't read PHON header\n");
	 else isPHON = 1;
       }
       Case("auto")   {
/*-----------case ("auto") block------------*/
	 if ((ok = readHeaderKA(fp)) >= 0) {
	   if (vb >= 2)
	     fprintf(stderr,"adcRead auto: Identified KA header\n");
	 }
	 else if ((ok = readHeaderCMU(fp)) >= 0) {
	   if (vb >= 2)
	     fprintf(stderr,"adcRead auto: Identified CMU header\n");
	 }
	 else if ((ok = readHeaderPHON(fp)) >= 0) {
	   isPHON = 1;
	   if (vb >= 2)
	     fprintf(stderr,"adcRead auto: Identified PHON header\n");
	 }
#ifdef SPHERE
	 else if ((ok = readHeaderSPHERE (fname)) >= 0) {
	   isSPHERE = 1;
	   if (vb >= 2)
	     fprintf(stderr,"adcRead auto: Identified SPHERE header\n");
	 }
#endif
#ifdef FFMPEG
	 else if ((ok = readHeaderFFMPEG (fname)) >= 0) {
	   isFFMPEG = 1;
	   if (vb >= 2)
	     fprintf (stderr, "adcRead auto: Identified FFMPEG header\n");
	   if (!headerADCin.chN) {
	     fprintf (stderr, "adcRead auto: no audio channels\n");
	     chN = headerADCin.chN;
	     headerADCin.sampleN = 0;
	   } else if (headerADCin.chN != chN) {
	     chN = headerADCin.chN;
	     headerADCin.sampleN /= chN;
	   }
	 }
#endif
#ifdef LIBSNDFILE
	 else if ((ok = readHeaderLIBSNDFILE(fname,(char*) byteMode,chX,chN,0,0,0,fformat, subtype, endianness, headMode, headModeBackup)) >= 0) {
	   isFLAC = 1;
	   if (vb >= 2)
	     fprintf(stderr,"adcRead auto: Identified header\n");
	 }
#else
	 else if ((ok = readHeaderWAV(fp)) >= 0) {
	   isWAV = 1;
	   if (vb >= 2)
	     fprintf(stderr,"adcRead auto: Identified WAV header\n");
	   if (headerADCin.chN != chN) {
	     /* Override chN with the information from the header */
	     chN = headerADCin.chN;
	     headerADCin.sampleN /= chN;
	     fprintf(stderr, "adcRead auto: reading channel %d of %d supplied from WAV header.\n", chX, chN);
	   }
	 }
#endif
	 else {
	   ok = 0;
	   /* fprintf(stderr, "* --- The ok after else is %d--- \n",ok ); */ /*delete this line later!*/
	   if (vb >= 2) fprintf(stderr,"adcRead auto: Assume no header or format type is not supported\n");
	 }

       }
/*------ end of ---case ("auto") block------------*/

       Case("")     {}
       Default      {

#ifdef LIBSNDFILE__WHY
      /*Using the libsnd library to open file*/
    		 if (ok == readHeaderLIBSNDFILE(fname,(char*) byteMode,chX,chN,0,0,0, fformat, subtype, endianness, headMode, headModeBackup)){
    		 /*char* fname, char* byteMode,int chX, int chN, int from, int to, int force*/
    			 isFLAC = 1;
    		 }
    		 else
    			 fprintf(stderr, "ERROR:adcRead: don't know header mode %s\n",headMode);
#else
	 if ((ok = readHeaderByte(fp,headMode)) >= 0) {
	   if (vb >= 2)
	     fprintf(stderr,"adcRead: Skip %d bytes as header\n",ok);
	 }
	 else fprintf(stderr,
		      "ERROR:adcRead: don't know header mode %s\n",headMode);
#endif
     }
     End_switch;

     if (ok < 0) {if (fp != stdout) fileClose(fname, fp); return -1;}

     /*--------------------------------------------------------
     ;  2. byte mode
     ;-------------------------------------------------------*/

/*     fprintf(stderr, "* --- Before byte mode block --- \n"); delete this line later!*/

     if (streq("", byteMode) || bmIntADC(byteMode) == SHORT_AUTO) {
       if (headerADCin.header) {
	 bm = headerADCin.byteMode;
       } else {
	 fprintf(stderr,
	  "WARNING:adcRead: No header was read but selected byte mode \"\"\n");
	 fprintf(stderr,
	  "needs header information. Will take byte mode \"auto\".\n");
	 bm = SHORT_AUTO;
       }
     } else {
       /* fprintf(stderr, "* - before-- else  if (streq("", byteMode) || bmIntADC(byteMode) == SHORT_AUTO) block --- \n");*/ /*delete this line later!*/
       bm = bmIntADC(byteMode);
       /* fprintf(stderr, "* - after-- else  if (streq("", byteMode) || bmIntADC(byteMode) == SHORT_AUTO) block --- \n"); */ /*delete this line later!*/
     }

     if (bm != SHORT_NS && bm != SHORT_SWAP && bm != SHORT_AUTO &&
	 bm != LIN      && bm != ALAW       && bm != ULAW       && bm != SHORTEN)
     {
       fprintf(stderr,"ERROR:adcRead: don't know byte mode %s\n",byteMode);
       if (fp != stdout) fileClose(fname, fp); return -2;
     }
   } /* if (!runon || fp==NULL) */

   /*fprintf(stderr, "* --- End of byte mode --- \n");delete this line later!*/

   /*--------------------------------------------------------
   ;  3. read data
   ;-------------------------------------------------------*/
/*   fprintf(stderr, "* --- The isFLAC is = %d --- \n", isFLAC);delete this line later!*/
   /* We read only the number of blocks given in the PHONEDAT header */
   /* PHONEDAT uses only short format */
   if (isPHON && (to < 0 || to >= headerADCin.sampleN))
       to = headerADCin.sampleN - 1;

   /* For WAV-Files, the header information is ok to use, too */
   if (isWAV  && (to < 0 || to >= headerADCin.sampleN))
     to = headerADCin.sampleN - 1;

#ifdef FFMPEG
   if (isFFMPEG) {
     if (!headerADCin.sampleN) {
       // no audio channels found, for example
       *adc = (short*) malloc (0);
       ok = 0;
     } else {
       ok = readADCFFMPEG (fname, adc, chX, chN, from, to, force);
     }
   } else {
#endif

#ifdef LIBSNDFILE
   if (isFLAC) {
        /*fprintf(stderr, "* before readADC Flac\n");*/
        ok = readADClibsnd(fname, adc, chX, chN, from, to, force );
      /*  fprintf(stderr, "* after readADC Flac\n"); Delete this line later!*/
   } else {
#endif

#ifdef SPHERE
   if (isSPHERE) {
     ok = readSphereData (fp, fname, adc, bm, chX, chN, from, to, force);
   } else {
     ok = readData       (fp,        adc, bm, chX, chN, from, to, force);
   }
#else
   ok = readData (fp, adc, bm, chX, chN, from, to, force);
#endif

#ifdef LIBSNDFILE
   }
#endif

#ifdef FFMPEG
   }
#endif

   if (ok < 0) return ok;

   if (!runon || ready) {

	/*   fprintf(stderr, "* --- Before block \"check consistency, close and return\" --- \n");delete this line later!*/
     /*--------------------------------------------------------
     ;  4. check consistency, close and return
     ;-------------------------------------------------------*/
     if (headerADCin.header && (dataADCin.sampleN != headerADCin.sampleN) && !isPHON && !isWAV)
       fprintf(stderr,
	 "WARNING:adcRead: (%d) samples specified in header, but real number is (%d)\n",
	 headerADCin.sampleN, dataADCin.sampleN);
     if (!isADCsmooth()) fprintf(stderr, "WARNING:adcRead: NOT smooth\n");
     if (fp != stdout) fileClose(fname, fp); fp = NULL;
   }
   /*fprintf(stderr, "* --- After block \"check consistency, close and return\" --- \n");delete this line later!*/
   return ok;
}


/* -------------------------------------------------------------------------
;  readHeaderXXX
;
;  All the read header function should look the same:
;     static int readHeaderXXX(FILE *fp)
;
;  IN/OUT:
;         fp          file pointer to beginning of file
;
;  RETURN:  -1        something wrong with header, fp is set back to
;                     beginning of file
;           else      header size in byte, fp is set to start of data
;  NOTE:              the information needed can be stored in the
;                     'headerADCin' structure
; -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
;  readHeaderByte
; -------------------------------------------------------------------------- */

static int readHeaderByte(FILE *fp, char* byteN)
{
   int offset;

   if (sscanf(byteN,"%d",&offset) == 1) {
      fseek(fp, (long)(offset), SEEK_SET);
      return offset;
   }
   return -1;
}

/* -------------------------------------------------------------------------
;  readHeaderKA
; -------------------------------------------------------------------------- */
#define MAX_KA_HEADER 1000
int readHeaderKA(FILE *fp)
{
   char byteA[MAX_KA_HEADER];
   int  byteN = read_bytes(byteA, MAX_KA_HEADER, fp);
   char **headSAP;
   int headSN;
   ByteModeType head_byte_order;
   int head_adcN;
   int head_sampl_rate;
   int ret = -1;

   if (is_KA_head(byteA)) {
      ret = read_KA_head(byteA, byteN, &headSAP, &headSN, &head_byte_order,
		   &head_adcN, &head_sampl_rate);
      headerADCin.header    = 1;
      headerADCin.samplRate = head_sampl_rate;
      headerADCin.sampleN   = head_adcN;
      headerADCin.chN       = 1;
      headerADCin.byteMode  = head_byte_order;
      headerADCin.remark    = headSAP;
      headerADCin.remN      = headSN;
      fseek(fp, (long)(ret), SEEK_SET);				/* jump to end of header	*/
   }
   else {
    if (vb >= 3) fprintf(stderr, "KA   header check: failed\n");
    rewind(fp);
   }
   return ret;
}

/* -------------------------------------------------------------------------
;  readHeaderCMU
; -------------------------------------------------------------------------- */

static int readHeaderCMU(FILE *fp)
{
   struct {
      short ad_hdrsize;   /* Size of header, including this field, in shorts */
      short ad_version;
      short ad_channels;
      short ad_rate;	             /* length of one sample in quarter usec */
      int   ad_samples;
      /* int    little_indian; ??   True if least significant byte is byte 0 */
   } head;
   int  byteN = read_bytes((char*)&head, sizeof(head), fp);
   int  doSwap = 0;
   if (byteN != sizeof(head)) { rewind(fp); return -1; }


   /* ----------------------------------
      check header, if failed swap
      ---------------------------------- */
   if (2*head.ad_hdrsize != byteN) {
      if (vb >= 3) fprintf(stderr,
		   "CMU  header check: %d instead of %d bytes => swap\n",
		   2*(int)head.ad_hdrsize, byteN);
      doSwap = 1;
      head.ad_hdrsize	=  SWAP_SHORT(head.ad_hdrsize);
      head.ad_version	=  SWAP_SHORT(head.ad_version);
      head.ad_channels	=  SWAP_SHORT(head.ad_channels);
      head.ad_rate	=  SWAP_SHORT(head.ad_rate);
      head.ad_samples	=  SWAP_INT  (head.ad_samples);
      /* head.little_indian = SWAP_INT  (head.little_indian); */
   }
   if (2*head.ad_hdrsize != byteN) {
      if (vb >= 3) fprintf(stderr,
		  "CMU  header check: failed: %d instead of %d bytes\n",
		   2*(int)head.ad_hdrsize, byteN);
      rewind(fp); return -1;
   }

   /* ------------------------------------
      copy info into header structure
      ------------------------------------ */
   headerADCin.header    = 1;
   headerADCin.samplRate = 4000000 / head.ad_rate;
   headerADCin.sampleN   = head.ad_samples;
   headerADCin.chN       = head.ad_channels;
   headerADCin.byteMode  = doSwap==1 ? SHORT_SWAP : SHORT_NS;
   if (vb >= 2) {
      fprintf(stderr,
      "CMU-HEADER: -------------------------------------------------------\n");
      fprintf(stderr,"%d bytes header, version %hd\n", byteN, head.ad_version);
      fprintf(stderr,"%d Hz, %d samples, %d channels\n",
	      headerADCin.samplRate, headerADCin.sampleN, headerADCin.chN);
      fprintf(stderr,
      "-------------------------------------------------------------------\n");
   }
   return byteN;
}

/* -------------------------------------------------------------------------
   readHeaderWAV
   ------------------------------------------------------------------------- */
struct {          /* SUB-HEADER for chunks        */
  char  type[4];  /*   chunk sub-header:          */
  int   length;   /*     chunk length             */
} chunk;

int findChunk (FILE* fp, char* name, int vb, int swap) {
  char* m = NULL;
  int   r = 0;

  while ((r = read_bytes ((char*)&chunk, sizeof (chunk), fp)) == sizeof (chunk) &&
	 (!swap || (chunk.length = SWAP_INT (chunk.length))) > -1 &&
	 (vb < 3 || fprintf (stderr, "(%8d %.4s ", chunk.length, chunk.type)) &&
	 strncmp (chunk.type, name, 4)) {
    if        (!strncmp (chunk.type, "DISP", 4) && (m = (char*) malloc (chunk.length))) {
      read_bytes (m, chunk.length, fp);
      if (vb >= 3) fprintf (stderr, ": %s )\n", m+4);
    } else if (!strncmp (chunk.type, "LIST", 4) && (m = (char*) malloc (chunk.length))) {
      read_bytes (m, chunk.length, fp);
      if (vb >= 3) fprintf (stderr, ": %s )\n", m+12);
    } else {
      fseek (fp, chunk.length, SEEK_CUR);
      if (vb >= 3) fprintf (stderr, "? )\n");
    }
    if (m) { free (m); m = NULL; }
  };

  if (vb >= 3 && r) fprintf (stderr, ")\n");

  return strncmp (chunk.type, name, 4);
}

int readHeaderWAV(FILE *fp) {

  struct {          /* HEADER for WAV files         */
    char  riff[4];  /* RIFF file ID                 */
    int   length;   /*      length                  */
    char  wave[4];  /* WAVE format                  */
  } head;

  struct {          /* FMT data structure           */
    short format;   /*     format specifier         */
    short chans;    /*     number of channels       */
    int   srate;    /*     sample rate in Hz        */
    int   bpsec;    /*     bytes per second         */
    short bpsample; /*     bytes per sample         */
    short bpchan;   /*     bits per channel         */
  } wavfmt;

  int byteN  = read_bytes ((char*)&head, sizeof (head), fp);
  int doSwap = 0, l = 0;

  /* Return is this cannot possibly work */
  if (byteN != sizeof(head)) {
    rewind(fp);
    return -1;
  }

  /* --------------------------------
       check header, if failed swap
     -------------------------------- */
  if        (!strncmp (head.riff, "RIFF", 4) && !strncmp (head.wave, "WAVE", 4)) {
    doSwap = (*((int*)&head.riff) != 1179011410);
    if (vb >= 3) fprintf (stderr, "WAV  header check: 'RIFF' found.\n");
  } else {
    if (vb >= 3) fprintf (stderr, "WAV  header check: failed!\n");
    rewind(fp);
    return -1;
  }

  if (vb >= 2)
    fprintf (stderr, "WAV-HEADER, swap = %d: ---------------------------------------------\n", doSwap);

  /* Find and read format chunk */
  findChunk (fp, "fmt ", vb, doSwap);
  if (read_bytes ((char*)&wavfmt, sizeof (wavfmt), fp) != sizeof (wavfmt)) {
    fprintf (stderr, "Couldn't read FORMAT chunk.\n");
    rewind (fp);
    return -1;
  }
  if (doSwap) {
    wavfmt.format   = SWAP_SHORT(wavfmt.format);
    wavfmt.chans    = SWAP_SHORT(wavfmt.chans);
    wavfmt.srate    = SWAP_INT  (wavfmt.srate);
    wavfmt.bpsec    = SWAP_INT  (wavfmt.bpsec);
    wavfmt.bpsample = SWAP_SHORT(wavfmt.bpsample);
    wavfmt.bpchan   = SWAP_SHORT(wavfmt.bpchan);
  }


  if ((size_t) chunk.length > sizeof (wavfmt)) {
    fseek (fp, chunk.length-sizeof (wavfmt), SEEK_CUR);
    if (vb > 1) fprintf (stderr, "WARN: FORMAT chunk contains extra %d bytes.\n",
	     chunk.length - (int) sizeof (wavfmt));
  }

  /* Find and read data chunk */
  findChunk (fp, "data", vb, doSwap);
  l = chunk.length;

  if (wavfmt.format != 1 &&
      wavfmt.format != 5 &&
      wavfmt.format != 6) {
    fprintf (stderr, "WAV format '%d' unknown, not loading!\n", wavfmt.format);
    rewind(fp);
    return -1;
  }

  /* ------------------------------------
     copy info into header structure
     ------------------------------------ */
  headerADCin.header    = 1;
  headerADCin.samplRate = wavfmt.srate;
  headerADCin.sampleN   = chunk.length/wavfmt.bpsample*wavfmt.chans;
  headerADCin.chN       = wavfmt.chans;
  headerADCin.byteMode  = (wavfmt.format == 5) ? ULAW :
                         ((wavfmt.format == 6) ? ALAW :
                                 (doSwap == 1) ? SHORT_SWAP : SHORT_NS);

  if (vb >= 3) {
    fpos_t pos;
    fgetpos (fp, &pos);
    fseek (fp, chunk.length, SEEK_CUR);

    /* Parse extra chunks */
    findChunk (fp, "xxxx", vb, doSwap);

    fsetpos(fp, &pos);
  }
  if (vb >= 2) {
    fprintf (stderr, "%d Hz, %d samples, %d channels\n",
	     headerADCin.samplRate, headerADCin.sampleN, headerADCin.chN);
    fprintf (stderr, "format = %d, length = %d\n",
	     wavfmt.format, l);
    fprintf (stderr, "bpsample = %d, bpsec = %d, bpchan = %d\n",
	     wavfmt.bpsample, wavfmt.bpsec, wavfmt.bpchan);
    fprintf(stderr, "-------------------------------------------------------------------\n");
  }

  return ftell (fp);
}


/* -------------------------------------------------------------------------
;  readHeaderPHON
;     HEADER.DCL Version 2
;     PHONDAT-Header,  n*512 Bytes.
; -------------------------------------------------------------------------- */
static int readHeaderPHON(FILE *fp)
{
   typedef struct {
      char not_used_1[20];        /*   0:                                    */
      int  nspbk;                 /*  20: data blocks (512 bytes)            */
      int  anz_header;            /*  24: header blocks (512 bytes)          */
      int  not_used_2[5];         /*  28:                                    */
      char sprk[2];               /*  48: speaker                            */
      char something_1[194];      /*  50:                                    */
      int  isf;                   /* 244: sampling rate in Hz                */
      int  type;                  /* 248: ILS -32000                         */
      int  init;                  /* 252: ILS  32149                         */
      char ifl[32];               /* 256: filename                           */
      char day;                   /* 288: */
      char month;                 /* 289: */
      short year;                 /* 290: */
      char sex;                   /* 292: */
      char version;               /* 293: */
      short adc_bits;             /* 294: */
      char something_2[216];      /* 296: */

   } HeadType;
   static HeadType *head = NULL;
   int  byteN = -1;
   int  doSwap = 0;

   if (!head) head = (HeadType*) malloc(sizeof(HeadType));
   if (!head) {
      fprintf(stderr,"PHON header: Couldn't allocate memory\n");
      return byteN;
   }
   if (read_bytes((char*)head, sizeof(HeadType), fp) != sizeof(HeadType)) {
      rewind(fp); return byteN;
   }

   /* ----------------------------------
      check header, if failed swap
      ---------------------------------- */
   if (head->type != -32000 || head->init != 32149) {
      if (vb >=3 ) fprintf(stderr,
		 "PHON header check: %d = -32000 ? and %d = 32149 ? => swap\n",
		 head->type, head->init);
      doSwap = 1;
      head->nspbk      = SWAP_INT(head->nspbk);
      head->anz_header = SWAP_INT(head->anz_header);
      head->isf        = SWAP_INT(head->isf);
      head->type       = SWAP_INT(head->type);
      head->init       = SWAP_INT(head->init);
      head->year       = SWAP_SHORT(head->year);
   }
   if (head->type != -32000 || head->init != 32149) {
      if (vb >=3 ) fprintf(stderr,
		"PHON header check: failed %d = -32000 ? and %d = 32149 ?\n",
		head->type, head->init);
      rewind(fp); return -1;
   }

   byteN = 512 * head->anz_header;
   if (head->anz_header > 1) fseek(fp, (long)(byteN), SEEK_SET);

   /* ------------------------------------
      copy info into header structur
      ------------------------------------ */
   headerADCin.header    = 1;
   headerADCin.samplRate = head->isf;
   headerADCin.sampleN   = 512 * head->nspbk / sizeof(short);
   headerADCin.byteMode  = doSwap==1 ? SHORT_SWAP : SHORT_NS;
   if (vb >= 2) {
      fprintf(stderr,
      "PHON-HEADER: ------------------------------------------------------\n");
      fprintf(stderr,"%-45s   date: %d.%d.%hd\n", head->ifl,
	      (int)head->day, (int)head->month, head->year);
      fprintf(stderr,"speaker: %c%c  sex: %c\n",
	      head->sprk[0], head->sprk[1], head->sex);
      fprintf(stderr,"%d block(s) header, %d blocks data\n",
	      head->anz_header, head->nspbk);
      fprintf(stderr,"%d Hz, %d samples\n",
	      headerADCin.samplRate, headerADCin.sampleN);
      fprintf(stderr,
      "-------------------------------------------------------------------\n");
   }
   return byteN;
}

#ifdef FFMPEG
static int readHeaderFFMPEG (char* fname)
{
  AVFormatContext *pFormatCtx;
  int             i, audioStream;
  AVCodecContext  *pCodecCtx;
  //  AVCodec         *pCodec;
  AVRational      time_base;
  int64_t duration = 0;
  unsigned int channels = 0;
  unsigned int frames = 0;
  unsigned int size = 2;

  // Register all formats and codecs
  if (!ffmpeg_registered) {
    ffmpeg_registered = TRUE;
    av_register_all ();
    avcodec_init ();
    avcodec_register_all ();
  }
  
  // Open video file
  //if (av_open_input_file (&pFormatCtx, fname, NULL, 0, NULL) != 0) return -1;
  if (avformat_open_input (&pFormatCtx, fname, NULL, NULL) != 0)
    return -1;

  // Retrieve stream information
  if (av_find_stream_info (pFormatCtx) < 0) {
    av_close_input_file (pFormatCtx);
    return -1; // Couldn't find stream information
  }

  // Dump information about file onto standard error
  if (vb > 2)
    av_dump_format (pFormatCtx, 0, fname, FALSE);

  // Find the first audio stream
  audioStream = -1;
  for (i = 0; i < pFormatCtx->nb_streams; i++)
    if (pFormatCtx->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO) {
      audioStream = i;
      break;
    }
  if (audioStream == -1) {
    av_close_input_file (pFormatCtx);
    return 0; // Didn't find an audio stream (but identified the file)
  }

  // get the timebase for this stream. The presentation timestamp, 
  // decoding timestamp and packet duration is expressed in timestamp 
  // as unit:
  // e.g. if timebase is 1/90000, a packet with duration 4500 
  // is 4500 * 1/90000 seconds long, that is 0.05 seconds == 20 ms.
  time_base = pFormatCtx->streams[audioStream]->time_base;
  duration  = pFormatCtx->streams[audioStream]->duration;
  frames    = pFormatCtx->streams[audioStream]->codec_info_nb_frames;

  // Get a pointer to the codec context for the audio stream
  pCodecCtx = pFormatCtx->streams[audioStream]->codec;
  channels  = pCodecCtx->channels;

  // this is what we learned
  headerADCin.header    = 1;
  headerADCin.samplRate = pCodecCtx->sample_rate;
  headerADCin.sampleN   = duration * channels;
  headerADCin.chN       = channels;
  headerADCin.byteMode  = SHORT_NS; /* pCodecCtx->sample_fmt */
  
  size = ffmpegSampleSize (pCodecCtx);

  // ugly hack(s) - there seems to be no general way to get the number of samples
  /*  if (!strcmp (fname + strlen (fname) - 4, ".wav") || !strcmp (fname + strlen (fname) - 4, ".WAV")) {
    if (vb>1) INFO ("Correcting for %d-WAV header: %d\n", (int) pFormatCtx->data_offset, headerADCin.sampleN);
    headerADCin.sampleN -= pFormatCtx->data_offset/size;
    } else */
  if (((double)duration)*time_base.num/time_base.den*headerADCin.samplRate < 0.9*headerADCin.sampleN) {
    INFO ("Correcting for time: %d (header: %d)\n", headerADCin.sampleN, (int) pFormatCtx->data_offset);
    headerADCin.sampleN = ((double)duration)*time_base.num/time_base.den*headerADCin.samplRate*channels;
  }
  if (headerADCin.sampleN < 1 || duration < 1) {
    duration = 1;
    headerADCin.sampleN = frames * AVCODEC_MAX_AUDIO_FRAME_SIZE;
    INFO ("Fixing time: %d -> %d\n", headerADCin.sampleN, duration);
  }

  if (vb > 1) 
    INFO ("FFMPEG: samples=%d rate=%d time=%f channels=%d frames=%d format=%d size=%d\n", 
	    headerADCin.sampleN, headerADCin.samplRate, 
	    ((double)duration)*time_base.num/time_base.den, channels, 
	    frames, pCodecCtx->sample_fmt, size);

  // Close the video file
  av_close_input_file (pFormatCtx);

  return headerADCin.sampleN;
}
#endif

/* ---------------------------------------------------------------
 * The function readHeaderLIBSNDFILE(FILE *fp) opens a file
 * using libsndfile library,then reads the header and return it
 * to the Janus header structure
 * ---------------------------------------------------------------
 */
#ifdef LIBSNDFILE
 static int readHeaderLIBSNDFILE(char* fname, char* byteMode,int chX, int chN, int from, int to, int force,
								char* fformat, char* subtype, char* endianness, char* headmode, char* headmodeBackup)
{
	int ok = 0;
	int i = 0;
	SNDFILE * file ;
	SF_INFO info ; /* libsnd native structure which holds the header info*/
	int err = 1; /* Holds the current opened file status
					 If = 0 then file is opened*/

	/* When opening a file for read, the format field should be set to zero before calling sf_open(). The only exception to this is the case of RAW files where the caller has to set the samplerate, channels and format fields to valid values. All other fields of the structure are filled in by the library. */
	info.format = 0;

	/*
	 * Libsndfile library settings
	 */

	/*--- WARNING: KEEP THE ORDER OF THIS ARRAY (FileFormatType[]) AND FileFormatTypeName[] IN CONSISTENCY!---*/
	static int* FileFormatType[] = {
			/*This array consists of the values
			 * defined in the libsnd file library
			 * pointing to the format of the input file
			 */
			(int*)0x010000, /*"Microsoft WAV format (little endian) ",*/
			(int*)0x020000,	/*Apple/SGI AIFF format (big endian)*/
			(int*)0x030000,	/*Sun/NeXT AU format (big endian)*/
			(int*)0x040000,	/*RAW PCM data*/
			(int*)0x050000,	/*Ensoniq PARIS file format*/
			(int*)0x060000,	/*Amiga IFF / SVX8 / SV16 format*/
			(int*)0x070000,	/*Sphere NIST format*/
			(int*)0x080000,	/*VOC files*/
			(int*)0x0A0000,	/*Berkeley/IRCAM/CARL*/
			(int*)0x0B0000,	/*Sonic Foundry's 64 bit RIFF/WAV*/
			(int*)0x0C0000,	/*Matlab (tm) V4.2 / GNU Octave 2.0*/
			(int*)0x0D0000,	/*Matlab (tm) V5.0 / GNU Octave 2.1 */
			(int*)0x0E0000,	/*Portable Voice Format*/
			(int*)0x0F0000,	/*Fasttracker 2 Extended Instrument*/
			(int*)0x100000,	/*HMM Tool Kit format*/
			(int*)0x110000,	/*Midi Sample Dump Standard*/
			(int*)0x120000,	/*Audio Visual Research*/
			(int*)0x130000,	/*MS WAVE with WAVEFORMATEX*/
			(int*)0x160000,	/*Sound Designer 2*/
			(int*)0x170000,	/*FLAC lossless file format*/
			(int*)0x180000 	/*"Core Audio File format"*/
	};

	/*--- WARNING: KEEP THE ORDER OF THE FileFormatType[] AND THE FileFormatTypeName[] IN CONSISTENCY!---*/
	static char* FileFormatTypeName[] = {
			"Microsoft WAV format (little endian) ",
			"Apple/SGI AIFF format (big endian)",
			"Sun/NeXT AU format (big endian)",
			"RAW PCM data",
			"Ensoniq PARIS file format",
			"Amiga IFF / SVX8 / SV16 format",
			"Sphere NIST format",
			"VOC files",
			"Berkeley/IRCAM/CARL",
			"Sonic Foundry's 64 bit RIFF/WAV",
			"Matlab (tm) V4.2 / GNU Octave 2.0",
			"Matlab (tm) V5.0 / GNU Octave 2.1 ",
			"Portable Voice Format",
			"Fasttracker 2 Extended Instrument",
			"HMM Tool Kit format",
			"Midi Sample Dump Standard",
			"Audio Visual Research",
			"MS WAVE with WAVEFORMATEX",
			"Sound Designer 2",
			"FLAC lossless file format",
			"Core Audio File format"
	};

	/*--- WARNING: KEEP THE ORDER OF THE FileFormatType[] AND THE FileFormatTypeName[] AND FileFormatTypeNameAbbr[] IN CONSISTENCY!---*/
#ifdef LIBSNDFILE__WHY
	static char* FileFormatTypeNameAbbr[] = {
			"WAV", 		/*Microsoft WAV format (little endian) ,*/
			"AIFF",		/*Apple/SGI AIFF format (big endian),*/
			"AU",  		/*Sun/NeXT AU format (big endian),*/
			"RAW", 		/*RAW PCM data,*/
			"PAF", 		/*Ensoniq PARIS file format,*/
			"SVX", 		/*Amiga IFF / SVX8 / SV16 format,*/
			"SPHERE",	/*Sphere NIST format,*/
			"VOC",		/*VOC files,*/
			"IRCAM",	/*Berkeley/IRCAM/CARL,*/
			"W64",		/*Sonic Foundrys 64 bit RIFF/WAV,*/
			"MAT4",		/*Matlab (tm) V4.2 / GNU Octave 2.0,*/
			"MAT5",		/*Matlab (tm) V5.0 / GNU Octave 2.1 ,*/
			"PVF",		/*Portable Voice Format,*/
			"XI",		/*Fasttracker 2 Extended Instrument,*/
			"HTK",		/*HMM Tool Kit format,*/
			"SDS",		/*Midi Sample Dump Standard,*/
			"AVR",		/*Audio Visual Research,*/
			"WAV",	/*MS WAVE with WAVEFORMATEX,*/
			"SD2",		/*Sound Designer 2,*/
			"FLAC",	/*FLAC lossless file format,*/
			"CAF"	/*Core Audio File format*/
		};
#endif

	/*---WARNING: KEEP THE ORDER OF THIS ARRAY (FileSubtype[]) AND FileSubtypeName[] IN CONSISTENCY!---*/
	static int* FileSubtype[] = {
			(int*)0x0001, /* Signed 8 bit data - Janus bytemode: lin*/
			(int*)0x0002, /*Signed 16 bit data - Janus bytemode:
					  in case of bytemode 01 little, 10 BIG*/
			(int*)0x0010, /*U-Law encoded*/
			(int*)0x0011 /* A-Law encoded */
	};

	/*--- WARNING: KEEP THE ORDER OF FileSubtype[] AND FileSubtypeName[] IN CONSISTENCY!---*/
	static char* FileSubtypeName[] = {
			"Signed 8 bit data",
			"Signed 16 bit data",
			"U-Law encoded",
			"A-Law encoded"
	};

	static char* FileSubtypeNameAbbr[] = {
		"PCM_S8",	/*"Signed 8 bit data",*/
		"PCM_16",	/*"Signed 16 bit data",*/
		"ULAW",		/*"U-Law encoded",*/
		"ALAW"		/*"A-Law encoded"*/
	};

	static int* FileEndiannes[]={
			(int*)0x00000000, /* Default file endian-ness. */
			(int*)0x10000000,	/* Force little endian-ness. */
			(int*)0x20000000, /* Force big endian-ness. */
			(int*)0x30000000  /* Force CPU endian-ness. */
	};

	static char* FileEndiannesName[]={
			"default",
			"little endian-ness forced",
			"big endian-ness forced"
	};

	static char* FileEndiannesNameAbbr[]={
			"DEF",			/*"detected default file endian-ness",*/
			"01",			/*"little endian-ness forced",*/
			"10"			/*"big endian-ness forced",*/
	};


	/* --- Libsndfile library settings --- */

	int sizeFileFormatType = sizeof(FileFormatType)/sizeof(int); /*The size of the FileFormatType array */
	int sizeFileSubtype = sizeof(FileSubtype)/sizeof(int); /*The size of the  FileSubtype array */
	int sizeFileEndiannes = sizeof(FileEndiannes)/sizeof(int); /*The size of the FileEndiannes array */

	/*
	 * Read the file header
	 */

	file = sf_open(fname, SFM_READ, &info);
	err=sf_error(file);
	/* fprintf(stderr,"--------------------------------------------------\n"); */
	if(err==0 && vb>1)INFO("*  Opened file: %s\n", fname );

	if (vb>0) INFO("--- INFO FROM THE OPENED FILE HEADER: %s ---\n", fname);

	/*
	 * Checks the consistency of the File format,
	 * subtype and endiannes between file header and
	 * specified data in the command...
	 */

	/*-------The full file consistency-------*/

	if (sf_format_check(&info)==0) /* Returns TRUE if header parameters are valid and FALSE otherwise*/
	{
	  //		WARN("* The information in the file header is inconsisted!\n");
		WARN("readHeaderLIBSNDFILE: file '%s' couldn't be opened with current combination of the File Format, subtype and byte order", fname);
	}
	else
	{
		if (vb>0) INFO("* The File header automatic format consistency checking: ok\n");
	}

	/*
	 * Checking the FILE TYPE with the typemask 0x0FFF0000
	 * from the libsnd file library
	 */

	for (i=0;i<sizeFileFormatType;i++)
	{
		if ((info.format&0x0FFF0000) == (size_t)FileFormatType[i])
		{
			if (vb>0) INFO("* Detected  FILE FORMAT: %s  \n", FileFormatTypeName[i]);

			/* if  ((strcmp((char *)fformat,(char*)FileFormatTypeNameAbbr[i])!=0)&&(strcmp((char*)fformat,(char*)"RAW"))!=0 ) */
			/*(( fformat != FileFormatTypeNameAbbr[i])*/ /*(fformat!="RAW"))*/
			/* {

			WARN("* Specified file format is %s\n\t",fformat );
			fformat = FileFormatTypeNameAbbr[i];
			WARN("... changed to the %s\n",fformat );
			} */

			/*if ((headmodeBackup != FileFormatTypeNameAbbr[i])&&(headmodeBackup =="auto"))*/
			/* if ((strcmp(headmodeBackup,FileFormatTypeNameAbbr[i])!=0)&&(strcmp(headmodeBackup,(char*)"auto")) !=0)
			{
				WARN("* WARNING: The specified header mode is %s\n\t",headmodeBackup);
				headmode = FileFormatTypeNameAbbr[i];
				WARN("... changed to the %s\n",headmode);
			} */
		}
	}

	/*printf("The size of the FileSubtype array is %d\n", sizeFileSubtype); Delete this line later*/

	/*
	 * Checking the FILE SUBTYPE with the typemask 0x0000FFFF
	 * from the libsnd file library
	 */

	headerADCin.byteMode=LIN; /* just in case */
	for (i=0;i<sizeFileSubtype;i++)
	{
		if ((info.format&0x0000FFFF) == (size_t)FileSubtype[i])
			{
			if (vb>0) INFO("* Detected  FILE SUBTYPE  is %s \n", FileSubtypeName[i]);
				if  (FileSubtypeNameAbbr[i]!= subtype)
				{
				 if	(strcmp(subtype,(char*)"PCM_U8")!=0 )
						/* subtype !="PCM_U8"*/
				 { /*if not equals default value of the subtype var then it
					 means that it was specified by user*/
				WARN("* Specified subtype file format is %s\n\t ... changed to %s \n",subtype, FileSubtypeNameAbbr[i] );
				}
				 subtype = FileSubtypeNameAbbr[i];
				}

				/*-- Mapping between libsndfile endianness and Janus bytemode type ---*/
				if (strcmp(FileSubtypeName[i],(char*)"Signed 8 bit data")==0)
				{
					headerADCin.byteMode =LIN;
				/*	fprintf(stderr,"The headerADCin.byteMode is ULAW");*/
				}

				if (strcmp(FileSubtypeName[i],(char*)"Signed 16 bit data")==0)
				{
					headerADCin.byteMode =LIN;
					/*fprintf(stderr,"The headerADCin.byteMode PCM_16 is LIN");*/
				}

				if (strcmp(FileSubtypeName[i],(char*)"U-Law encoded")==0)
				{
					headerADCin.byteMode =ULAW;
					/*fprintf(stderr,"The headerADCin.byteMode is ULAW");*/
				}

				if (strcmp(FileSubtypeName[i],(char*)"A-Law encoded")==0)
				{
					headerADCin.byteMode =ALAW;
					/*fprintf(stderr,"The headerADCin.byteMode is ALAW");*/
				}

				/*-----*/

/*
				if (FileSubtypeName[i] == "Signed 8 bit data") {
						headerADCin.byteMode =LIN;
				}
				if (FileSubtypeName[i] =="Signed 16 bit data") {
						headerADCin.byteMode =LIN;
				}
				if (FileSubtypeName[i] =="U-Law encoded") {
				    	headerADCin.byteMode = ULAW;
				}
				if (FileSubtypeName[i] =="A-Law encoded") {
					    headerADCin.byteMode = ALAW;
							}
				if (FileSubtypeName[i] =="A-Law encoded") {
					    headerADCin.byteMode = ALAW;
									}
					*/
			}
	}

	/*
	 * Checking the ENDIANNES with the typemask 0x30000000
	 * from the libsnd file library
	 */

	for (i=0;i<(sizeFileEndiannes-1);i++)
		{
			if ((info.format&0x30000000) == (size_t)FileEndiannes[i])
				{
					if (strcmp(endianness,FileEndiannesNameAbbr[i])!=0)
							/*endianness != FileEndiannesNameAbbr[i] */{
						if (vb>0) INFO("* Detected ENDIANNES: %s \n", FileEndiannesName[i]);

					if (strcmp(endianness,(char*)"LITTLE")!=0)
					/*if (endianness != "LITTLE")*/
					{
						WARN("* Specified endiannes is %s\n\t", endianness);
						endianness = FileEndiannesNameAbbr[i];
						WARN("... changed to %s \n", endianness);
					}
					}
					/*else
					{
						WARN("* Detected ENDIANNES: %s \n ", endianness);
					}*/
				}
		}

	/*
	 * Output the info from the file header
	 */

	if (vb>0) INFO("* Detected channels # is: %d \n  frames: %d \n  samplerate: %d \n  seekable: %d \n",
			(int)info.channels, (int) info.frames, (int)info.samplerate, (int)info.seekable);
	/* ------------------------------------
	  copy info into header structure
	  ------------------------------------ */
	headerADCin.header = '1'; /* The header exists*/
	headerADCin.samplRate = info.samplerate;
	if ((int)info.channels != chN) {

	WARN("* WARNING: The specified number of channels %d\n\t... changed to %d channels \n", chN, info.channels);
	}
	/* fprintf(stderr,"--------------------------------------------------\n"); */

	headerADCin.chN = info.channels;
	headerADCin.sampleN = info.frames;

	/*headerADCin.byteMode = SHORT_NS;*/

	/*--------------------------------
	 * Detecting the byte mode
	 *--------------------------------- */

	if (err==0) ok=1;
	i= sf_close( file );

	return ok;
	}

#endif

/*=============================================================================
;  readData
;  IN/OUT:
;       fp       File pointer that is set to start of data.
;  (*)  adc      Short pointer that will contain the read data. Memory
;                will be allocated.
;       bm       Byte mode that is
;                  SHORT(_NS)     16 bit linear
;                  SHORT_         16 bit linear (WINDOWS)
;                  SHORT_SWAP     16 bit linear, bytes swapped
;                  SHORT_AUTO     16 bit linear, auto swap detect
;                  SHORTEN        data taken from shorten
;                  LIN             8 bit linear
;                  ULAW            8 bit u-law
;                  ALAW            8 bit a-law
;       chX      selected channel   1  ..  chN or (0 for all channels, not yet)
;       chN      number of channels
;       from     start sample, can be 0 .. sampleN-1
;                if  from < 0  or  from > to  no samples will be read
;       to       last sample to be read,  must be >= from or -1 for last
;       force    if > 0 read file anyway (skip unnecessary bytes at end of file)
;                   > 1 do it without warning
;
;  RETURN:       number of samples read = to - from + 1 or less if
;                to exceeds sampleN-1.
;
;  NOTE:         information about byteMode, total samples and smoothness
;                is stored in the dataADCin structure
============================================================================ */
static int readData(FILE *fp, short** adcAP, ByteModeType bm, int chX, int chN,
	     int from, int to, int force)
{
   long byteN = fbytes_left(fp);  /* total bytes in file */
   int  byteV = sizeof(char);     /* bytes per value     */
   int  byteS;                    /* bytes per sample    */
   int  sampleR = 0;              /* samples to read     */
   int  sampleN;                  /* samples in file     */
   int  byteR;                    /* bytes to read       */
   char* buffer = NULL;
   float avg_d, avg_d_s;
   int i;
   int shfrom = 0, shto = -1;


   dataADCin.smooth    = -1;
   dataADCin.byteOrder = 0;
   dataADCin.sampleN   = -1;
   dataADCin.sampleR   = 0;

   if (chN <= 0) chN = 1;
   if (chX <= 0 || chX > chN) chX = 1;

   /* --------------------------------------
      how many bytes per value and sample
      -------------------------------------- */
   if (bm == SHORT_NS || bm == SHORT_SWAP || bm == SHORT_AUTO)
      byteV = sizeof(short);
   byteS = chN * byteV;
   if (bm == SHORTEN) {
      shfrom = from; shto = to;
      from = 0; to = -1;
   }

   /* ------------------------------------------------
      how many samples, total and how many to read
      ------------------------------------------------ */
   if (bm != SHORTEN  &&  (byteN % byteS) != 0) {
     if (force <= 1)
       fprintf(stderr,
	       "number of bytes (%ld) is not multiple of channels (%d) * bytes per value (%d)\n",
	       byteN , chN, byteV);
     if (force == 0) return -3;
     byteN -= (byteN % byteS);
   }
   if (byteN <= 0) return byteN;
   sampleN = byteN / byteS;     dataADCin.sampleN = sampleN;

   if (to < 0 || to >= sampleN) to = sampleN - 1;
   if (from > to || from >= sampleN || from < 0) {
      if (vb >= 1) fprintf(stderr,
	"readData: total %ld bytes, read 0 of %d samples , empty range %d .. %d\n",
	byteN, sampleN, from, to);
      return 0;
   }
   sampleR = to - from + 1;
   byteR   = byteS * sampleR;

   if (bm == SHORTEN) byteR = byteN;
   if ((buffer = (char*) malloc(byteR+BUFFERSIZE)) == NULL) {
      fprintf(stderr,"readData: couldn't allocate memory (1), %d bytes\n",byteR);
      return -1;
   }

   if (vb>3) fprintf(stderr,
       "readData: bytes %d/%d/%d/%ld, samples %d/%d,channel %d/%d\n",
       byteV, byteS, byteR, byteN, sampleR, sampleN, chX, chN);

   /* ---------------------------
      read data, all channels
      --------------------------- */
   if (bm != SHORTEN) fseek(fp, (long)(from*byteS), SEEK_CUR);
   if (read_bytes(buffer, byteR, fp) != byteR) {
      free(buffer);
      return -4;
   }

   /* ---------------------------
      convert to short
      --------------------------- */
   if ((bm != SHORTEN) && (chN > 1 || byteV != sizeof(short))) {
     if ((*adcAP = (short*) malloc(sizeof(short) * sampleR)) == NULL) {
       fprintf(stderr,"readData: couldn't allocate memory (2), %d shorts\n",sampleR);
       return -1;
     }
   }

   chX--;                       /* chX was 1..chN */
   switch (bm) {
   case SHORT_NS: case SHORT_SWAP: case SHORT_AUTO:
     if (chN>1) {
       short* sbuffer = (short*)buffer;
       for (i=0; i<sampleR; i++) (*adcAP)[i] = sbuffer[i*chN + chX];
     } else {
       *adcAP = (short*)buffer; buffer = NULL;
     }

     if (bm == SHORT_SWAP) {
       swap_shortA(*adcAP, sampleR);
       dataADCin.byteOrder = 1;
     }

     break;

   case LIN:
      if (chN>1) {
	for (i=0; i<sampleR; i++)
	   (*adcAP)[i] = ((signed char)buffer[i*chN + chX]) * 256;
      } else {
	for (i=0; i<sampleR; i++)
	   (*adcAP)[i] = ((signed char)buffer[i]) * 256;
      }
      break;

   case ALAW:
      if (chN>1) {
	for (i=0; i<sampleR; i++)
	  (*adcAP)[i] = (short) cvt_a2s[ (unsigned char)buffer[i*chN + chX]];
      } else {
	for (i=0; i<sampleR; i++)
	   (*adcAP)[i] = (short) cvt_a2s[ (unsigned char)buffer[i] ];
      }
      break;

    case ULAW:
      if (chN>1){
	for (i=0; i<sampleR; i++)
	  (*adcAP)[i] = (short) cvt_u2s[ (unsigned char)buffer[i*chN + chX]];
      } else {
	for (i=0; i<sampleR; i++)
	  (*adcAP)[i] = (short) cvt_u2s[ (unsigned char)buffer[i] ];
      }
      break;

   case SHORTEN: {
     int  shortN = byteR, nchan, doSwab;
     ByteModeType bmt=ULAW;
     do {
       shortN *= 2;
       if ((*adcAP = (short*) malloc(sizeof(short) * shortN)) == NULL) {
	 fprintf(stderr,"readData: couldn't allocate memory (s1), %d shorts\n", shortN);
	 if (buffer) free(buffer); return -1;
       }
       byteN = shorten (buffer, (char*)*adcAP, sizeof(short)*shortN, &byteV, &nchan, &doSwab, &bmt);
       if (byteN == -3) free(*adcAP);
     } while (byteN == -3);

     if (nchan != chN)
       fprintf(stderr,"readData: found %d channels but user wants %d.\n",nchan,chN);

     if (byteN > 0) {          /* Ok, we have byteN, byteV, chN, chX   */
       signed char *frombuffer = NULL;
       short*            newAP = NULL;
       from    = shfrom; to = shto;
       byteS   = chN * byteV;
       sampleN = byteN / byteS;     dataADCin.sampleN = sampleN;

       if (to < 0 || to >= sampleN) to = sampleN - 1;
       if (from > to || from >= sampleN || from < 0) {
	 if (vb >= 1) fprintf(stderr,
	   "readData: total %ld bytes, read 0 of %d samples, empty range %d .. %d\n",
	   byteN, sampleN, from, to);
	 return 0;
       }
       sampleR    = to - from + 1;
       byteR      = byteS * sampleR;
       frombuffer = (signed char *)((char*)*adcAP + byteS * from);

       if (byteV == 1) {
	 newAP = (short*) malloc (sizeof(short) * sampleR);
	 for (i = 0; i < sampleR; i++)
	   newAP[i] = (bmt == ULAW) ? (short) cvt_u2s[(unsigned char) frombuffer[i*chN + chX]]
	                            : frombuffer[i*chN + chX] << 8;
	 free (*adcAP);
	 *adcAP = newAP;
       } else if (from || chN) {
	 for (i = 0; i < sampleR; i++)
	   (*adcAP)[i] = ((short*)frombuffer)[i*chN + chX];
	 *adcAP = (short*) realloc (*adcAP,sampleR*sizeof(short));
       }

     } else {                                              /* Error case */
       fprintf(stderr,"readData: byte mode SHORTEN, error code %ld\n",byteN);
       if (buffer) free(buffer); return -1;
     }
     if ((machine_10() ^ doSwab) && (bmt != ULAW)) swap_shortA ((*adcAP),sampleR);
     }
     break;

   case UNDEF:
     fprintf(stderr,"readData: byte mode is undefined\n");
     if (buffer) free(buffer); return -1;

   case ASCII:
     fprintf(stderr,"readData: byte mode must not be ASCII\n");
     if (buffer) free(buffer); return -1;
   }
   chX++;

   if (buffer) free(buffer);

   /* --- smoothness --- */
   if (!check_smoothness(*adcAP, sampleR, 0, vb, &avg_d)) {
      dataADCin.smooth = 0;
      if (bm == SHORT_AUTO) {
	 swap_shortA(*adcAP, sampleR);
	 if (check_smoothness(*adcAP, sampleR, 0, vb, &avg_d_s)) {
	    dataADCin.smooth    = 1;
	    dataADCin.byteOrder = 1;
	 } else {
	    if (avg_d_s > avg_d) {
	     if (vb >= 2) fprintf(stderr,"readData: select NO byteSwap because %f < %f!\n", avg_d, avg_d_s);
	     /* first swap order seems to be better */
	     swap_shortA(*adcAP, sampleR);
	   } else {
	     if (vb >= 2) fprintf(stderr,"readADC: select byteSwap because %f > %f!\n", avg_d, avg_d_s);
	     dataADCin.byteOrder = 1;
	   }
	 }
      }
   }
   else dataADCin.smooth = 1;

   /* --- verbosity --- */
   if (vb >= 1) fprintf(stderr,
      "readData: total %ld bytes, read %d of %d samples, %d .. %d, channel %d of %d\n",
      byteN, sampleR, sampleN, from, to, chX, chN);

   dataADCin.sampleR = sampleR;
   return sampleR;
}

#ifdef SPHERE
static int readSphereData (FILE* fp, char* fname, short** adc, ByteModeType bm, int chX, int chN, int from, int to, int force) {
  SP_FILE* sfp;
  int      ok;
  char chS[512];

  dataADCin.smooth    = -1;
  dataADCin.byteOrder = 0;
  dataADCin.sampleN   = headerADCin.sampleN;
  chN                 = headerADCin.chN;

  if (!(*adc = (short*) malloc (dataADCin.sampleN * sizeof (short)))) {
    WARN ("readSphereData: memory problem (adc=%d bytes).\n", dataADCin.sampleN * sizeof (short));
    return 0;
  }

  sfp = sp_open (fname, "rv");

  snprintf (chS, 512, "CH-%d:SE-PCM", chX);
  if (sp_set_data_mode (sfp, chS)) {
    sp_print_return_status (stderr);
  }

  if (!(ok = sp_read_data (*adc, dataADCin.sampleN, sfp))) {
    INFO ("readSphereData: trying mode 'r' instead of 'rv'\n");
    sp_close (sfp);
    sfp = sp_open (fname, "r");

    sp_set_data_mode (sfp, chS);
    if (!(ok = sp_read_data (*adc, dataADCin.sampleN, sfp)))
      sp_print_return_status (stderr);
  }

  if ((from > 0 || to < dataADCin.sampleN) && to > from) {
    /* We have to cut a piece... */
    short* adc2;
    int i;

    ok = to - from + 1;

    if (!(adc2 = (short*) malloc (ok * sizeof (short)))) {
      WARN ("readSphereData: memory problem (adc2=%d bytes).\n", ok * sizeof (short));
      return 0;
    }

    for (i = 0; i <= to-from; i++)
      adc2[i] = (*adc)[i+from];

    free (*adc);
    *adc = adc2;
  }

  sp_close (sfp);

  if (!ok) {
    FILE* fp2;
    int  ok2;
    char cmdline[1024];
    char tmpf[1024];

    free (*adc);

    /* HACK Alert: Jon&John, we have a problem! */
    WARN ("SPHERE-library failed on '%s'.\n", fname);

    /* Ok, we take the brutal approach by decompressing
       this beast into a temporary file,
       which we then load, no matter what */

    /*
      fprintf (stderr, "SPHERE header follows: ----\n");
      sprintf (cmdline, "head -c 1024 %s", fname);
      system  (cmdline);
    */

    sprintf (tmpf, "/tmp/janus.XXXXXX");
    mkstemp (tmpf);
    sprintf (cmdline, "shorten -x -d 1024 %s %s", fname, tmpf);

    INFO ("Executing '%s'\n", cmdline);
    ok2 = system (cmdline);

    WARN ("SPHERE-library bypassed, rc = %d\n", ok2);

    if (!(fp2 = fopen (tmpf, "rb")))
      ok = -1;
    else {
      ok = readData (fp2, adc, bm, chX, chN, from, to, force);
      fclose (fp2);
    }

    remove (tmpf);
  }

  /* --- verbosity --- */
  if (vb >= 1)
    fprintf (stderr, "readSphereData: total %ld bytes, read %d of %d samples, %d .. %d, channel %d of %d\n",
	     fbytes_left (fp) - 1024, ok, dataADCin.sampleN, from, to, chX, chN);
  dataADCin.sampleR = ok;

  return ok;
}
#endif



/* ######################################################################### */
/*                        s m o o t h n e s s                                */
/* ######################################################################### */
/* ---------------------------------------------------------------------------
;  check_smoothness
;
;  IN/OUT:
;	adcA		Array with shorts, representing ADC data
;	adcN		size of adcA
;	speed		0 = compute only bare smoothness
;			1 = compute more statistics
;	vb		0,1 = print nothing
;			2   = print short message
;		        3   = print all available statistics
;	avg_d		the average diffenve between successive data points
;
;  RETURN:		1, if adcA seems smooth enough, 0 else
;
;
;  This function relies on a heuristic. The underlying idea is that a accoustic waveform is somewhat smooth.
;  The average difference between two successive data points servers as a measure of smoothness.
;  In an evaluation of more than 1000 different ADC files (see statistic below), it was observed that
;  the largest averaged difference was 733, while the smallest average difference on swapped ADC files
;  (incorrect byte order) was 2134.
;  This suggests as criteria:  if the averaged difference is greater 1000, it is most likely no correct ADC data.
;
;  Statistic on average absolute difference:
;
;  speaker         #ADC files      incorrect byte order:   correct byte order:
;  mhh1            130             4489 -  9946              116 - 654
;  fgs1             55             6464 -  9937              213 - 733
;  rm_spell(tdid)  594             2134 -  9973               17 - 361
;  rm_spell (tdit) 730             2484 - 10116               12 - 333
;
;
;  There have been some rare exceptions (~20 out of 11.000 german adc files) where the value for the correct byte order
;  has been between 1000 and 1500.
;  => increase treshold to 1500.
;
;
;  One problem is that '0' doesnt change its value when swapped => ignore '0' ?
;-------------------------------------------------------------------------------------------------------------------*/
int check_smoothness(short* adcA,  int adcN,  int speed, int vb,  float* avg_d)
{
  int    i;
  int    smooth;
  int    oldx = 0;
  int    min  = adcA[0];
  int    max  = adcA[0];
  int    dmax = adcA[1] - adcA[0];
  int    dmin = adcA[1] - adcA[0];
  int	 countN = 1;						/* init with 1, to avoid potention division by zero	*/
  float  dsum = 0.0;
  float  asum = 0.0;
  float  sum  = 0.0;

  FILE*	 f = stderr;

  /*------------------------------------------------------
  ;  compute statistics
  ;-----------------------------------------------------*/
  if (speed == 0) {
    for (i=0;  i<adcN;  i++) {
      int  x = (int) adcA[i];
      if (!(oldx == 0 && x == 0)) {				/* ignore more than one 0 in a row			*/
        int dx = x - oldx;
        dsum += ABS(dx);
        oldx = x;
        countN += 1;
      }
    }
  }

  if (speed > 0)  {
    for (i=0;  i<adcN;  i++) {
      int x = (int) adcA[i];
      if (!(oldx == 0 && x == 0)) {
        int dx = x - oldx;

        if (x>max) max = x;
        if (x<min) min = x;
        if (dx < dmin) dmin = dx;
        if (dx > dmax) dmax = dx;
        dsum += ABS(dx);
        asum += ABS(x);
        sum  += x;
        oldx  = x;
	countN += 1;
      }
    }
  }

  *avg_d = dsum/(float)countN;
  smooth = (*avg_d < 1500);




  /*------------------------------------------------------
  ;  print available results if requested
  ;-----------------------------------------------------*/
  if (vb == 2) {
    fprintf(f, "Smooth = %d      [adcN = %d, countN = %d,  avg_diff = %f]\n", smooth, adcN, countN,  *avg_d );
  }

  if (vb >= 3) {
  if (speed == 0)  {
    fprintf(f, "---------  STATISTIC on ADC DATA -----------\n");
    fprintf(f, "adcN  = %d\n", adcN);
    fprintf(f, "countN= %d\n", countN);
    fprintf(f, "\n");
    fprintf(f, "Smooth = %d      [avg diff = %f  (D-SUM = %f)]\n", smooth, *avg_d, dsum );
  }
  if (speed >= 1)  {
    fprintf(f, "\n");
    fprintf(f, "MIN   = %d\n", min);
    fprintf(f, "MAX   = %d\n", max);
    fprintf(f, "SUM   = %f  (avg = %f)\n", sum, sum/countN);
    fprintf(f, "ASUM  = %f  (avg = %f)\n", asum, asum/countN);
    fprintf(f, "Deltas:\n");
    fprintf(f, "D-MIN = %d\n", dmin);
    fprintf(f, "D-MAX = %d\n", dmax);
    fprintf(f, "D-SUM = %f  (avg = %f)\n", dsum, *avg_d);
    fprintf(f, "--------------------------------\n");
    fprintf(f, "\n\n");
  }
  }
  return(smooth);
}




/* ######################################################################### */
/*                s t a t i c   f u n c t i o n s                            */
/* ######################################################################### */
/* ---------------------------------------------------------------------------
;  write_shorts
;
;  IN:
;	adcA		array with shorts
;	adcN		size of adcA
;	f		f onto which data are to be written
;
;  RETURN:		number of shorts written
; ------------------------------------------------------------------------- */
static int  write_shorts(short* adcA, int adcN, FILE* f)
{
  int n = 0;
  while (n < adcN) {
     int rest = adcN - n;      /* number of shorts that remain to be written */
     n += fwrite(adcA + n, sizeof(short), rest, f);  /* n = total number of
							written shorts       */
  }
  if (n != adcN) {
    fprintf(stderr,
      "adcWrite: serious write problem: adcN = %d, n = %d\n\n", adcN, n);
  }
  return(n);
}
/* ------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------
;  write_bytes
;
;  IN:
;	adcA		array with bytes
;	adcN		size of adcA
;	f		f onto which data are to be written
;
;  RETURN:		number of bytes written
; ------------------------------------------------------------------------- */
static int  write_bytes(char* adcA, int adcN, FILE* f)
{
  int n = 0;
  while (n < adcN) {
     int rest = adcN - n;      /* number of bytes that remain to be written */
     n += fwrite(adcA + n, sizeof(char), rest, f);  /* n = total number of
							written shorts       */
  }
  if (n != adcN) {
    fprintf(stderr,
      "adcWrite: serious write problem: adcN = %d, n = %d\n\n", adcN, n);
  }
  return(n);
}
/* ------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------
;  fbytes_left
;
;  IN/OUT:
;	fp		file pointer
;
;  RETURN:		number of bytes left in file
;                       or < 0 if error
; ------------------------------------------------------------------------- */
static long fbytes_left(FILE* fp)
{
   fpos_t position, end;

   if (fgetpos(fp, &position) != 0) return -1;
   fseek(fp, (long)0, SEEK_END);
   if (fgetpos(fp, &end) != 0) return -2;
   if (fsetpos(fp, &position) != 0) return -3;
#if defined __GLIBC__ && __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 2
   return (end.__pos - position.__pos);
#else
   return (end - position);
#endif
}
/* ------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------
;  read_bytes
;
;  IN/OUT:
;	byteA		array with bytes
;	max_byteN	max size of byteA
;	f		f from which data are to be read
;
;  RETURN:		number of bytes read
; ------------------------------------------------------------------------- */
static int  read_bytes(char* byteA, int max_byteN, FILE* f)
{
  int n 	= 0;		             /* number of bytes read so far */
  int try_again = 1;		     /* try a 2nd time when end was reached */
  int cont 	= TRUE;

  while (cont) {
      int rest    = max_byteN - n;   /* max bytes that still fit into byteA */
      int n_read  = fread(byteA + n, sizeof(char), rest, f);
                                   /* try to read at most 'rest' more bytes */
      n          += n_read;	       /* total number of bytes read so far */
      cont        =  (n_read > 0);
      if (!cont) 		               /* well, we could stop here, */
        cont = (try_again-- > 0);                  /* but we once again ..  */
  }
  return(n);			             /* return number of bytes read */
}
/* ------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------
;  machine_10
;
;
;  returns 1, if this machine stores shorts in "little endian" format,
;  i.e. if the first of the two bytes is the most significant one.
; -------------------------------------------------------------------------- */
static int machine_10(void)
{
  char a, b;

  union {			/* union type to check order of bytes in a short	*/
    short           x;	        /* space to store a short				*/
    unsigned char   b[2];	/* same space to store two characters (bytes)		*/
  }  u;				/* the union variable					*/

  if (sizeof(short) != 2) {	/* check if the system uses 2 byte shorts		*/
    fprintf(stderr, "ERROR:adc_readwrite: length of short = %d\n", (int) sizeof(short));
    exit(1);
  }

  u.x = (short) 1;	         /* assign 1 to the short, to see in which byte its stored */
  a = u.b[0];
  b = u.b[1];

       if (a == 0 && b == 1) return(1); /* byte 0 is the most significant one, so return 1 */
  else if (a == 1 && b == 0) return(0); /* byte 1 is the most significant one, so return 0 */
  else {
    fprintf(stderr, "ERROR:adc_readwrite: unknown short format [%d %d]\n", a, b);
    exit(1);
  }
}

/* ------------------------------------------------------------------------- */
/*=============================================================================
;  swap bytes
;
;  IN/OUT:
;	adcA		Array with shorts, representing ADC data
;	adcN		size of adcA
;
;--------------------------------------------------------------------------- */
static  void swap_shortA(short* adcA,  int adcN)
{
  int i;
  for (i=0;  i<adcN;  i++) {
     short t =  adcA[i];
     adcA[i] =  SWAP_SHORT(t);
  }
  /*
  swab(adcA, adcA, adcN*sizeof(short)); doesn't work using the same array adcA
  */
}
/* ------------------------------------------------------------------------- */



/* ######################################################################### */
/*                        K A   H e a d e r                                  */
/* ######################################################################### */
/* ----------------------------------------------------------------------------
;   headstr
;
;   TRUE,  if s2 is a substring at the head of s1, i.e.  "s2" =  "s1....."
; -------------------------------------------------------------------------- */
static int  headstr( char* s1,  char* s2)
{
  while (*s1 != '\0'  && *s2 != '\0'  &&  *s1 == *s2) {s1++;  s2++;}
  return( *s2 == '\0' );
  /* s2 is a substring at the head of s1 only if EOS of s2 was reached here */
}
/* ------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------
;  line_always_printed
;
;  originally, the KA header contains all textual information, including begin
;  and end of header lines etc.
;  this routine removes those lines
; -------------------------------------------------------------------------- */
static int line_always_printed(char* line)
{
  if (headstr(line,KA_HEADER_BEGIN)) 	return(TRUE);
  if (headstr(line,KA_HEADER_END)) 	return(TRUE);
  if (headstr(line,BYTE_ORDER_ID)) 	return(TRUE);
  if (headstr(line,NB_SAMPLES_ID)) 	return(TRUE);
  if (headstr(line,SAMPL_RATE_ID)) 	return(TRUE);
  return(FALSE);
}
/* ------------------------------------------------------------------------- */
static int is_KA_head(char* sA)
{
  int ok = !strncmp(sA, KA_HEADER_BEGIN, strlen( KA_HEADER_BEGIN));
  /* returns non-zero if s == HDR_BEGIN	*/
  return(ok);
}
/*---------------------------------------------------------------------------*/
static void skip_line(char** sP, int n)
{
  int 	i = 0;
  char* s = *sP;
  while (i<n-1 && s[i] != '\n') i++;      /* until \n found or n chars read  */
  i++; 				          /* skip over newline (or whatever) */

  *sP = (char*) (s + i);
}
/*=============================================================================
;  parse_string
;
;  IN/OUT:
;	sP       : pointer to an array of characters,
;                  may contain many newlines.
;	line	 : the next line ('\n' terminated) in sP (but at most n chars)
;                  is copied from sP into line
;	n	 :
;
;  RETURN:	   length of parsed string INCLUDING the EOS character
;----------------------------------------------------------------------------*/
static int parse_string(char** sP, char* line, int n)
{
  int 	i = 0;
  char* s = *sP;
  while (i<n-1 && s[i] != '\n') {
       line[i] = s[i]; i++;	    /* line[i] = s[i++] is INCORRECT !!$#$#$ */
  }

  line[i++] = '\0';				       /* add the EOS marker */

  if (i>=n) {
    fprintf(stderr, "WARNING: adc_read: header line to long (%s)\n", line);
  }

  *sP = (char*) (s + i);
  return(i);				   /* return length of parsed string */
}
/*---------------------------------------------------------------------------*/
/* ============================================================================
;  read_KA_head
;
;
;  IN:
;	byteA		array with all data, header and ADCs
;	byteN		size of byteA
;
;  OUT:			(see definition in adc_read_header)
;
;  RETURN:		size of header in bytes (chars)
; ---------------------------------------------------------------------------*/
static int read_KA_head( char* byteA,  int byteN,  char*** headSAP,
			int* headSN, ByteModeType* byte_order,  int* adcN,
			int* sampl_rate)
{
  char*  sP = (char*) byteA;
  int	 done = FALSE;
  int	 lineN = 0;
  int	 charN = byteN;
  int    lineX;
  char** headSA;
  int	 ptr_mem;

  *byte_order	= UNDEF;	    /* default: we don't know the byte order */
  *adcN		= -1;	     /* default: we don't know the number of samples */

  if (vb >= 2) fprintf(stderr,
     "KA-HEADER: ---------------------------------------------------------\n");

  while (!done) {			/* search for the end of the header */
    done = (!strncmp(sP, KA_HEADER_END, strlen(KA_HEADER_END)));
    if (lineN++ >= MAX_LINES_IN_HEADER - 1) {
      fprintf(stderr, "ERROR: adc_read: KA header has too many lines (>%d)\n",
			MAX_LINES_IN_HEADER - 1);
      done = TRUE;
    }

    skip_line(&sP, 256);		 /* advance sP to begin of next line */
  }

  charN = sP - (char*) byteA;	 /* total number of characters found in head */

  ptr_mem	   = sizeof(char*) *  MAX_LINES_IN_HEADER;			/* memory needed for the pointers 		*/
  *headSAP =  headSA = (char**) malloc(ptr_mem + sizeof(char) * charN);		/* alloc space for pointers and all strings	*/
  headSA[0] = ((char*) &headSA[0]) + ptr_mem;					/* the strings start here			*/

  sP = (char*) byteA;								/* reset the string pointer			*/
  for (lineX=0;  lineX<lineN;  lineX++) {					/* copy all lineN data lines into headerSA[..]	*/
    int n = parse_string(&sP, headSA[lineX], 256);
    if (vb >= 2 && lineX>0 && lineX<lineN-1)  fprintf(stderr, "%s\n",headSA[lineX]);
    if (lineX <lineN-1) headSA[lineX+1] = headSA[lineX] + n;			/* compute beginning of next string (if any)	*/
 }
  if (vb >= 3) fprintf(stderr,
     "--------------------------------------------------------------------\n");
  *headSN = lineN;								/* we found lineN datalines			*/

  /*----------------------------------------------------------
  ;  try to interpret some of the information in the textual
  ;  header.
  ;---------------------------------------------------------*/
  for (lineX=0;  lineX<lineN;  lineX++) {
    char* s = headSA[lineX];
    char  par[1000], val[1000];
    sscanf(s, "%s %s\n", par, val);
    Switch(par)
      Case(BYTE_ORDER_ID) {
	    *byte_order = bmIntADC(val);
      }
      Case(NB_SAMPLES_ID) {
            int ok = sscanf(val, "%d", adcN);
            if (!ok) *adcN = -1;
      }
      Case(SAMPL_RATE_ID) {
            int ok = sscanf(val, "%d", sampl_rate);
            if (!ok) *sampl_rate = -1;
	    if (*sampl_rate < 20) {
              int old = *sampl_rate;
	      *sampl_rate *= 1000;
	      fprintf(stderr, "WARNING:adc_read: sample_rate (%d) < 20. Replaced by %d\n", old, *sampl_rate);
            }
      }
    End_switch;
  }

  return(charN);  /* return position behind header */
}


#ifdef SPHERE

/* -------------------------------------------------------------------------
     readHeaderSPHERE
   ------------------------------------------------------------------------- */

int readHeaderSPHERE (char *fname)
{
  SP_FILE      *fp = sp_open (fname, "r");
  SP_STRING  spstr = "000";
  SP_INTEGER         spint;

  if (sp_h_get_field (fp,     "sample_n_bytes", T_INTEGER, (void*) &spint)) return (-1);
  if (spint == 1) {
    headerADCin.byteMode = LIN;
  } else {
    if (sp_h_get_field (fp,     "sample_byte_format", T_STRING, (void*) &spstr))
      return (-1);
    else
      headerADCin.byteMode  = strcmp (spstr, byteModeADC[1]) ? SHORT_SWAP : SHORT_NS;
  }

  if (sp_h_get_field (fp,     "sample_count",   T_INTEGER, (void*) &spint)) return (-1);
  headerADCin.sampleN   = spint;

  if (sp_h_get_field (fp,     "sample_rate",    T_INTEGER, (void*) &spint)) return (-1);
  headerADCin.samplRate = spint;

  if (sp_h_get_field (fp,     "channel_count",  T_INTEGER, (void*) &spint)) return (-1);
  headerADCin.chN       = spint;

  headerADCin.header    = 1;
  headerADCin.remark    = NULL;
  headerADCin.remN      = 0;

  sp_close (fp);

  return (0);
}

#endif


/* ######################################################################### */
/*                        audio_convert (main)                               */
/* ######################################################################### */
#ifdef STAND_ALONE

#define  HELP_STRING1  "\
 ########################################################################\n\
 #									\n\
 # audio_convert:  Reads, writes audio files				\n\
 #									\n\
 #									\n\
 #  USAGE:  audio_convert  infile outfile [-flags argument]		\n\
 #									\n\
 #		outfile can be \"-\" for stdout				\n\
 #	 		 						\n"
#define  HELP_STRING2  "\
 #  -ihm  headMode  =  headMode of inputfile				\n\
 #  -ibm  byteMode  =  byteMode of inputfile				\n\
 #  -ohm  headMode  =  headMode of outputfile				\n\
 #  -obm  byteMode  =  byteMode of outputfile				\n\
 #  -from sample    =  from sample					\n\
 #  -to   sample    =  to sample					\n\
 #  -chX  channel   =  select channel					\n\
 #  -chN  channels  =  number of channels				\n\
 #  -v    integer   =  Verbosity.					\n\
 #  -f    integer   =  force read if bytes left				\n\
 #  -h              =  Help. Print this informaion			\n\
 #									\n\
 ########################################################################\n"

/*============================================================================
;  str_to_int
;
;  atoi returns 0 if a string couldnt be converted. That's silly,
;  because the string could have been "0" as well.
;--------------------------------------------------------------------------- */
static int str_to_int( str, n )
char* str;
int*  n;
{
  int ok = (1 == sscanf(str, "%d", n));
  if (!ok) fprintf(stderr,
      " ERROR: Could not convert string (%s) into integer\n", str);
  return(ok);
}
/*-------------------------------------------------------------------------- */
/*=============================================================================
;  print_help
;--------------------------------------------------------------------------- */
static void print_help(void)
{
  fprintf(stderr, HELP_STRING1);
  fprintf(stderr, HELP_STRING2);
}
/*-------------------------------------------------------------------------- */
int main_standalone(int argc,char **argv)
{
   char *infile;
   char *outfile;
   char *headModeIn  = "0";
   char *byteModeIn  = "auto";
   char *headModeOut = "";
   char *byteModeOut = "auto";
   int verbosity = 1;
   int chX = 0, chN = 1, from = 0, to = -1;
   int force = 0;
   short* adcA;
   int adcN;

   /*--------------------------------------------------------------------------
   ;  Read  and handle commandline parameters
   ;------------------------------------------------------------------------ */
   if (argc < 3) { print_help(); exit(1); }
   infile  = argv[1];
   outfile = argv[2];
   {
      int k=3;
      int		help = FALSE;
      while (argc > k   &&   !help)
	 Switch (argv[k++])
           Case("-ihm" )  { headModeIn = argv[k++]; }
           Case("-ibm" )  { byteModeIn = argv[k++]; }
           Case("-ohm" )  { headModeOut = argv[k++]; }
           Case("-obm" )  { byteModeOut = argv[k++]; }
           Case("-from")  { str_to_int(argv[k++], &from); }
           Case("-to"  )  { str_to_int(argv[k++], &to); }
           Case("-chX" )  { str_to_int(argv[k++], &chX); }
           Case("-chN" )  { str_to_int(argv[k++], &chN); }
           Case("-v"   )  { str_to_int(argv[k++], &verbosity); }
           Case("-f"   )  { str_to_int(argv[k++], &force); }
           Case("-h"   )  { help = TRUE;   	      		        	}
           Default      { fprintf(stderr, "\nERROR: Unknown flag %s\n", argv[--k]); help = TRUE;}
         End_switch;
      if (help) { print_help(); exit(1); }
   }

   adc_verbosity (verbosity);
   adcN = readADC(infile, &adcA, headModeIn, byteModeIn, chX, chN, from, to, force);
   writeADC(outfile, adcA, adcN, headModeOut, byteModeOut);
   free(adcA);

   return 0;
}

#endif

/* ========================================================================= */
/* This file was written by closely looking at Tony Robinson's code from the */
/* NIST sphere package. Although it differs a lot from the original code, it */
/* is not allowed to be used for commercial purposes without Tony Robinson's */
/* consent.					 Ivica Rogina, Nov. 12, 1995 */
/* ========================================================================= */

/* #include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h> */

#define MAGIC		"ajkg"
#define FORMAT_VERSION	1
#define DEF_BLOCK_SIZE	256
#define DEF_NMEAN	0
#define DEF_MAXNLPC	0
#define DEF_NCHAN	1

#define CHANSIZE	0
#define ENERGYSIZE	3
#define BITSHIFTSIZE	2
#define NWRAP		3

#define FNSIZE		2
#define FN_DIFF0	0
#define FN_DIFF1	1
#define FN_DIFF2	2
#define FN_DIFF3	3
#define FN_QUIT		4
#define FN_BLOCKSIZE	5
#define FN_BITSHIFT	6
#define FN_QLPC		7

#define ULONGSIZE	2
#define NSKIPSIZE	1
#define LPCQSIZE	2
#define LPCQUANT	5
#define XBYTESIZE	7

#define TYPESIZE	4
#define TYPE_AU1	0	/* original lossless ulaw                    */
#define TYPE_S8		1
#define TYPE_U8		2
#define TYPE_S16HL	3
#define TYPE_U16HL	4
#define TYPE_S16LH	5
#define TYPE_U16LH	6

#define MLN2 0.69314718055994530942

#define uchar	unsigned char
#define ushort	unsigned short
#define ulong	unsigned long

#define uintGet(nbit,file) ((version==0) ? uvarGet(nbit, file) : ulongGet(file))

static ulong *masktab = 0;
static uchar *getbuf  = 0;

static void mkmasktab(void)
{
  int i; ulong val=0;

  masktab = (ulong*) malloc(33*sizeof(ulong));
  masktab[0] = val;
  for (i=1; i<33; i++) { val <<= 1; val |= 1; masktab[i] = val; }
}

static ulong wordGet (char **buff)
{
  static int nbytebuf = 0;
  static uchar *getbufp;
  ulong buffer;

  if (buff==0) { nbytebuf=0; return 0; }

  if (nbytebuf == 0)
  {
    memcpy(getbuf,*buff,BUFFERSIZE); nbytebuf=BUFFERSIZE; *buff += BUFFERSIZE;
    getbufp = getbuf;
  }
  buffer = ((long)getbufp[0]) << 24 | ((long)getbufp[1]) << 16 | ((long)getbufp[2]) << 8 | ((long)getbufp[3]);

  getbufp += 4; nbytebuf -= 4;;

  return buffer;
}

static long uvarGet (int nbin, char **buff)
{
  static ulong buffer = 0;
  static int nbuffer = 0;
  long result;

  if (buff==0) { nbuffer=0; buffer=0; return 0; }

  if (nbuffer==0) { buffer=wordGet(buff); nbuffer=32; }

  for (result=0; !(buffer & (1L << --nbuffer)); result++) if (nbuffer==0) { buffer=wordGet(buff); nbuffer=32; }

  while (nbin!=0)
  {
    if (nbuffer>=nbin)
    {
      result = (result<<nbin) | ((buffer>>(nbuffer-nbin)) & masktab[nbin]);
      nbuffer -= nbin; nbin=0;
    }
    else
    {
      result = (result<<nbuffer) | (buffer & masktab[nbuffer]);
      buffer = wordGet(buff);
      nbin -= nbuffer; nbuffer=32;
    }
  }
  return result;
}

static long ulongGet (char **buff) { int nbit = uvarGet(ULONGSIZE,buff); return uvarGet(nbit,buff); }

static long varGet (int nbin, char **buff)
{
  ulong uvar = uvarGet(nbin+1,buff);

  if(uvar & 1) return((long) ~(uvar >> 1));
  else return((long) (uvar >> 1));
}

static int fwriteType (long **data, int ftype, int nchan, int nitem, char **buff)
{
  static char *tmpbuf = NULL, *tmpfub = NULL;
  static int ntmpbuf = 0;
  int hiloint = 1, hilo = !(*((char*) &hiloint)), i, datasize = 0, chan;
  long *data0;

  if (!buff) {
    ntmpbuf=0;
    if (tmpbuf != NULL) free(tmpbuf); tmpbuf = NULL;
    if (tmpfub != NULL) free(tmpfub); tmpfub = NULL;
    return 1;
  }

  data0 = data[0];

  switch (ftype) {
  case TYPE_AU1:
  case TYPE_S8:
  case TYPE_U8: datasize = sizeof(char); break;
  case TYPE_S16HL:
  case TYPE_U16HL:
  case TYPE_S16LH:
  case TYPE_U16LH: datasize = sizeof(short); break;
  default: return -1;
  }

  if (ntmpbuf < nchan * nitem * datasize) {
    ntmpbuf = nchan * nitem * datasize;
    if (tmpbuf != NULL) free(tmpbuf);
    if (tmpfub != NULL) free(tmpfub);
    tmpbuf = (char*) malloc((ulong) ntmpbuf);
    tmpfub = (char*) malloc((ulong) ntmpbuf);
  }

  assert (tmpbuf); // XCode says could be NULL
  assert (tmpfub);
  assert (data);
  assert (data0);
  switch(ftype) {
  case TYPE_S8: {
    char *tmpbufp = (char*) tmpbuf;
    if (nchan==1) for (i=0; i<nitem; i++) *tmpbufp++ = data0[i];
    else          for (i=0; i<nitem; i++) for (chan=0; chan<nchan; chan++) *tmpbufp++ = data[chan][i];
    break;
  }

  case TYPE_AU1:
  case TYPE_U8: {
    uchar *tmpbufp = (uchar*) tmpbuf;
    if (nchan==1) for (i=0; i<nitem; i++) *tmpbufp++ = data0[i];
    else          for (i=0; i<nitem; i++) for (chan=0; chan<nchan; chan++) *tmpbufp++ = data[chan][i];
    break;
  }

  case TYPE_S16HL:
  case TYPE_S16LH: {
    short *tmpbufp = (short*) tmpbuf;
    if (nchan==1) for (i=0; i<nitem; i++) *tmpbufp++ = data0[i];
    else          for (i=0; i<nitem; i++) for (chan=0; chan<nchan; chan++) *tmpbufp++ = data[chan][i];
    break;
  }

  case TYPE_U16HL:
  case TYPE_U16LH: {
    ushort *tmpbufp = (ushort*) tmpbuf;
    if (nchan==1) for (i=0; i<nitem; i++) *tmpbufp++ = data0[i];
    else          for (i=0; i<nitem; i++) for (chan=0; chan<nchan; chan++) *tmpbufp++ = data[chan][i];
    break;
  }
  default: return -1;
  }

  switch(ftype) {
  case TYPE_AU1:
  case TYPE_S8:
  case TYPE_U8: memcpy(*buff,tmpbuf,datasize*nchan*nitem); *buff += datasize*nchan*nitem; break;
  case TYPE_S16HL:
  case TYPE_U16HL:
    if (hilo) {
      memcpy(*buff,tmpbuf,datasize*nchan*nitem); *buff += datasize*nchan*nitem;
    } else {
      swab(tmpbuf,tmpfub,datasize*nchan*nitem);
      memcpy(*buff,tmpfub,datasize*nchan*nitem); *buff += datasize*nchan*nitem;
    }
    break;
  case TYPE_S16LH:
  case TYPE_U16LH:
    if (hilo) {
      swab(tmpbuf,tmpfub,datasize*nchan*nitem);
      memcpy(*buff,tmpfub,datasize*nchan*nitem);
      *buff += datasize*nchan*nitem;
    } else {
      memcpy(*buff,tmpbuf,datasize*nchan*nitem); *buff += datasize*nchan*nitem;
    }
    break;
  default: return -1;
  }
  return 1;
}

static void fixBitshift (long *buffer, int nitem, int bitshift, int ftype)
{
  int i;

  if (ftype == TYPE_AU1)
    for (i = 0; i < nitem; i++)
      buffer[i] = ulaw_outward[bitshift][buffer[i] + 128];
  else
    if (bitshift != 0)
      for (i = 0; i < nitem; i++)
	buffer[i] <<= bitshift;
}

static long **long2d(long unsigned int n0, long unsigned int n1)
{
  long **array0;

  if ((array0 = (long**)malloc(n0 * sizeof(long*) +n0*n1*sizeof(long)))!=NULL)
  {
   unsigned int i; long *array1 = (long*)(array0+n0);
   for (i=0; i<n0; i++) array0[i]=array1+i*n1;
  }
  return array0;
}

/*
static int initOffset (long **offset, int nchan, int nblock, int ftype)
{
  long mean=0; int chan, i;

  switch (ftype)
  {
   case TYPE_S8:
   case TYPE_S16HL:
   case TYPE_S16LH:	mean = 0;	break;
   case TYPE_U8:	mean = 0x80;	break;
   case TYPE_U16HL:
   case TYPE_U16LH:	mean = 0x8000;	break;
   default:		mean = DEF_NMEAN;
  }
  for (chan=0; chan<nchan; chan++) for (i=0; i<nblock; i++) offset[chan][i]=mean;
  return mean;
}
*/
#define EXIT(R) { retCode=(R); goto exitShorten; }

int shorten (char *Buffi, char *Buffo, int buffoSize, int *bytesPerSample, int *chN, int *doSwab, ByteModeType* bmt)
{
  long 	**buffer=0, **offset=0, defaultOffset;
  int   version = FORMAT_VERSION, bitshift = 0;
  char  *magic = MAGIC;
  int   len    = strlen(magic);
  int	blocksize = DEF_BLOCK_SIZE, nchan = DEF_NCHAN, i, ftype=5, cmd, chan, nwrap;
  int	*qlpc = NULL, maxnlpc = DEF_MAXNLPC, nmean = DEF_NMEAN, retCode=0, nskip;
  char  *buffi = Buffi, *buffo = Buffo;

  for (i=0; i < len; i++) if (*buffi++ != magic[i]) EXIT(-1)
  version = *buffi++;
  if (version > FORMAT_VERSION) EXIT(-2)
  if (masktab==0) mkmasktab();
  if (getbuf ==0) getbuf=(uchar*)malloc(BUFFERSIZE);

  ftype = uintGet(TYPESIZE,&buffi);
  nchan = uintGet(CHANSIZE,&buffi);
  *bmt  = (ftype == TYPE_AU1) ? ULAW : ALAW;

  if (version>0) {
    blocksize	= uintGet((int) (log((double) DEF_BLOCK_SIZE) / MLN2), &buffi);
    maxnlpc	= uintGet(LPCQSIZE, &buffi);
    nmean	= uintGet(0,&buffi);
    nskip	= uintGet(NSKIPSIZE, &buffi);
    if (nskip>buffoSize) EXIT(-3)
    for (i=0; i<nskip; i++) *buffo++ = uvarGet(XBYTESIZE, &buffi);
  } else
    blocksize = DEF_BLOCK_SIZE;

  nwrap = NWRAP;

  /* fprintf(stderr,"version = %d, blocksize = %d, maxnlpc = %d, nmean = %d, nskip = %d, nwrap = %d, nchan = %d, ftype = %d\n",version,blocksize,maxnlpc,nmean,nskip,nwrap,nchan,ftype); */

  buffer = long2d((ulong) nchan, (ulong) (blocksize + nwrap));
  offset = long2d((ulong) nchan, (ulong) nmean);
  for (chan=0; chan<nchan; chan++) {
    for (i=0; i<nwrap; i++) buffer[chan][i]=0; buffer[chan] += nwrap;
  }
  if (maxnlpc>0) qlpc = (int*) malloc((ulong) (maxnlpc*sizeof(*qlpc)));
  defaultOffset = 0; /*initOffset(offset,nchan,nmean,ftype);*/

  chan = 0;
  while ((cmd = uvarGet(FNSIZE, &buffi)) != FN_QUIT) {
    switch (cmd) {
    case FN_DIFF0:
    case FN_DIFF1:
    case FN_DIFF2:
    case FN_DIFF3:
    case FN_QLPC: {
      int resn = uvarGet(ENERGYSIZE, &buffi);  long coffset, *cbuf = buffer[chan];  int nlpc, j;
      if (version==0) resn--;

      if (nmean==0)
	coffset = defaultOffset;
      else {
	long sum=0;
	for (i=0; i<nmean; i++) sum += offset[chan][i];
	coffset = sum/nmean;
      }

      switch (cmd) {
      case FN_DIFF0: for (i=0; i<blocksize; i++) cbuf[i]=varGet(resn,&buffi)+coffset;
	break;
      case FN_DIFF1: for (i=0; i<blocksize; i++) cbuf[i]=varGet(resn,&buffi)+   cbuf[i-1];
	break;
      case FN_DIFF2: for (i=0; i<blocksize; i++) cbuf[i]=varGet(resn,&buffi)+(2*cbuf[i-1]-cbuf[i-2]);
	break;
      case FN_DIFF3: for (i=0; i<blocksize; i++) cbuf[i]=varGet(resn,&buffi)+3*(cbuf[i-1]-cbuf[i-2])+cbuf[i-3];
	break;
      case FN_QLPC:
	nlpc = uvarGet(LPCQSIZE, &buffi);
    assert (qlpc);
	for (i=0; i<nlpc; i++) qlpc[i] = varGet(LPCQUANT,&buffi);
	for (i=0; i<nlpc; i++) cbuf[i-nlpc] -= coffset;
	for (i=0; i<blocksize; i++) {
	  long sum=0;
	  for (j=0; j<nlpc; j++) sum += qlpc[j] * cbuf[i-j-1];
	  cbuf[i] = varGet(resn, &buffi) + (sum >> LPCQUANT);
	}
	if (coffset!=0) for (i=0; i<blocksize; i++) cbuf[i] += coffset;
	break;
      }

      if (nmean>0) {
	long sum = 0;
	for (i=1; i<nmean; i++) offset[chan][i-1] = offset[chan][i];
	for (i=0; i<blocksize; i++) sum += cbuf[i];
	offset[chan][nmean-1] = sum/blocksize;
      }

      for (i=(-nwrap); i<0; i++) cbuf[i] = cbuf[i+blocksize];
      fixBitshift(cbuf,blocksize,bitshift,ftype);
      if ((long)((long)buffo-(long)Buffo + blocksize*sizeof(short)) >= buffoSize-blocksize)
	EXIT(-3)   /* checking if there's nuff space for */
	           /* short is good for checking bytes, too */
      if (chan == nchan-1)
	if (fwriteType (buffer, ftype, nchan, blocksize, &buffo) < 0)
	  EXIT(-4)
      if ((long)buffo-(long)Buffo > buffoSize) {
	fprintf(stderr,"Aaargh, memory manager destroyed due to corrupt shorten file.\n");
	exit(123);
      }
      chan = (chan+1)%nchan;
      }
      break;

    case FN_BLOCKSIZE: blocksize = uintGet((int) (log((double) blocksize) / MLN2), &buffi); break;
    case FN_BITSHIFT:  bitshift = uvarGet(BITSHIFTSIZE,&buffi); break;
    default: EXIT(-1)
	       }
  }
  retCode = (long)buffo-(long)Buffo;
 exitShorten:

  if (buffer)	 free((char*) buffer);
  if (offset)	 free((char*) offset);
  if (maxnlpc>1) free((char*) qlpc);

  wordGet(0); uvarGet(0,0); fwriteType(0,0,0,0,0);

  *bytesPerSample = (ftype==TYPE_S8 || ftype==TYPE_U8 || ftype==TYPE_AU1) ? 1 : 2;
  *chN            = nchan;

  *doSwab = (ftype == TYPE_S16HL);

  return retCode; /* 0=okay, -1=fileCorrupt, -2=wrongVersion, -3=bufferTooSmall, -4=wrongFileType */
}

/*****************************************************************************/
/* ASCII ENCODED SHORT VECTOR MESSAGE (AESVM)                                */
/*===========================================================================*/
/*                                                                           */
/*  THIS C FILE PROVIDES FUNCTIONALITY TO SEND/RECEIVE AN SVECTOR OVER A     */
/*  TCL NETWORK CONNECTION BY ENCODING THE SVECTOR AS AN ASCII STRING.       */
/*                                                                           */
/*===========================================================================*/
/*  written in Mai 2007 by Friedrich Faubel                                  */
/*  (c) 2007 Deutsche Telekom Laboratories                                   */
/*****************************************************************************/

/*****************************************************************************/
/* ASCIIDecode6bit                                                           */
/*===========================================================================*/
/*  Encodes a 6-bit value to an an ASCII-char                                */
/*---------------------------------------------------------------------------*/
/*  PARAMETERS:                                                              */
/*    b           :   6-bit value                                            */
/*  RETURNS:                                                                 */
/*    the corresponding ASCII char                                           */
/*---------------------------------------------------------------------------*/
/*  written in Mai 2007 by Friedrich Faubel                                  */
/*  (c) 2007 Deutsche Telekom Laboratories                                   */
/*****************************************************************************/
char ASCIIEncode6bit(char b)
{
  char c = -1;

  if (b<26)
    c = b+65;
  else
  {
    if (b<52)
      c = b-26+97;
    else
    {
      if (b<64)
        c = b-52+48;
    }
  }

  return c;
}



/*****************************************************************************/
/* ASCIIDecode6bit                                                           */
/*===========================================================================*/
/*  Decodes an ASCII-char to a 6-bit value                                   */
/*---------------------------------------------------------------------------*/
/*  PARAMETERS:                                                              */
/*    c           :   ASCII char                                             */
/*  RETURNS:                                                                 */
/*    the corresponding 6-bit value                                          */
/*---------------------------------------------------------------------------*/
/*  written in Mai 2007 by Friedrich Faubel                                  */
/*  (c) 2007 Deutsche Telekom Laboratories                                   */
/*****************************************************************************/
char ASCIIDecode6bit(char c)
{
  char b = -1;

  if ((c>=65) && (c<65+26))
    b = c-65;
  else
  {
    if ((c>=97) && (c<97+26))
      b = c-97+26;
    else
    {
      if ((c>=48) && (c<48+12))
        b = c+52-48;
    }
  }

  return b;
}



/******************************************************************************/
/* encodeByteArray                                                            */
/*                                                                            */
/* TODO: WRITE DOCU                                                           */
/*                                                                            */
/* !!! THIS CODE IS IN NEED OF A CORRECTION OFFICER IN ORDER TO WORK WITH !!! */
/* !!! JANUS !!!                                                              */
/******************************************************************************/
/*** START --- Visual Studio .NET specific, replace by some gnu invariant ***/
/* String^ encodeByteArray(char* b, int len) */
/*** END --- Visual Studio .NET specific ***/
char* encodeByteArray(char* b, int len)
{
  int pos, bitPos, i, imax;
  char ca[2];
  char b6;
  /*** START --- Visual Studio .NET specific, replace by some gnu invariant ***/
  /* String^ s = gcnew String(""); */
  char* s=NULL; /* Friedrich used this without initialisation */
  /*** END --- Visual Studio .NET specific ***/
  ca[1] = 0;
  pos = 0;
  bitPos = 7;
  while (pos<len)
  {
    /*------------------------------------------
    /  compute imax = number of bits to process
    /------------------------------------------*/
    imax = 6;
    if (pos==len-1)
    {
      if (bitPos+1<imax)
        imax = bitPos+1;
    }
    /*------------------
    / process imax bits
    /------------------*/
    b6 = 0;
    for (i=0; i<imax; i++)
    {
      b6 = (b6 << 1) | ((b[pos] >> bitPos) & 1);
      bitPos--;
      if (bitPos<0)
      {
        bitPos = 7;
        pos++;
      }
    }
    /*-------------------------
    / normalize last 6bit-char
    /-------------------------*/
    b6 = b6 << (6-imax);
    /*----------------
    / store 6bit-char
    /----------------*/
    ca[0] = ASCIIEncode6bit(b6);
    /*** START --- Visual Studio .NET specific, replace by some gnu invariant ***/
    /* s += gcnew String(ca); */
    /*** END --- Visual Studio .NET specific ***/
  }

  return s;
}



/*****************************************************************************/
/* decodeByteArray                                                           */
/*===========================================================================*/
/*  Decodes a 6-bit ASCII-encoded byte array. The address of the resulting   */
/*  byte array will be written to (*b8ap)                                    */
/*---------------------------------------------------------------------------*/
/*  PARAMETERS:                                                              */
/*    b6a        :   6-bit ASCII-encoded array                               */
/*    b6al       :   length of b6a                                           */
/*    b8ap       :   recipient of adress of the resulting byte array         */
/*  RETURNS:                                                                 */
/*    length of the byte array                                               */
/*---------------------------------------------------------------------------*/
/*  written in Mai 2007 by Friedrich Faubel                                  */
/*  (c) 2007 Deutsche Telekom Laboratories                                   */
/*****************************************************************************/
int decodeByteArray(char* b6a, int b6al, char** b8ap)
{
  int pos, bitPos, i, j, len;
  char  b6, b8;
  char* b8a;

  /*-------------------------------
  / calc number of bytes to decode
    ------------------------------*/
  len = b6al*6/8;

  /*------------------------
  / allocate memory for b8a
    -----------------------*/
  b8a = malloc(len);
  if (b8a==NULL)
  {
    (*b8ap) = NULL;
    printf("len %i\n", len);
    ERROR("Memory allocation failed!");
    return 0;
  }

  pos = 0;
  bitPos = 5;
  /*-----------
  / loop bytes
  /-----------*/
  for (j=0; j<len; j++)
  {
    /*-----------------
    / process one byte
    /-----------------*/
    b8 = 0;
    for (i=0; i<8; i++)
    {
      b6 = ASCIIDecode6bit(b6a[pos]);
      b8 = (b8 << 1) | ((b6 >> bitPos) & 1);
      bitPos--;
      if (bitPos<0)
      {
        bitPos = 5;
        pos++;
      }
    }
    /*-----------
    / store byte
    /-----------*/
    b8a[j] = b8;
  }

  (*b8ap) = b8a;
  return len;
}



/*****************************************************************************/
/* fesAppendAESVMItf                                                         */
/*===========================================================================*/
/*  Appends the content of an ASCII-encoded short vector message(AESVM) to   */
/*  an SVector feature                                                       */
/*---------------------------------------------------------------------------*/
/*  MESSAGE FORMAT:                                                          */
/*    <HEADER> <ASCII-encoded short array> <TRAILER>                         */
/*---------------------------------------------------------------------------*/
/*  written in Mai 2007 by Friedrich Faubel                                  */
/*  (c) 2007 Deutsche Telekom Laboratories                                   */
/*****************************************************************************/
int fesAppendAESVMItf(ClientData cd, int argc, char *argv[])
{
  FeatureSet *featSet    = (FeatureSet*) cd;
  char       *featName   = NULL;
  char       *msg        = NULL;
  char       *msgHeader  = NULL;
  char       *msgTrailer = NULL;

  Feature* feat;
  SVector* v;
  int      i, msgLen, headLen, trailLen, fIndex;
  int      nShorts, nBytes;
  char*    b6A;
  char*    b8A;
  int      b6ALen;

  /*-----------
  / parse args
  /-----------*/
  argc--;
  if (itfParseArgv (argv[0], &argc, argv+1, 0,
		    /* "<feature>", ARGV_CUSTOM, getFe, &destin,     cd, "SVector feature", */
    "<feature>", ARGV_STRING,  NULL, &featName,   cd, "name of an SVector feature",
    "<msg>",     ARGV_STRING,  NULL, &msg,        cd, "ascii encoded short vector messgage",
    "<header>",  ARGV_STRING,  NULL, &msgHeader,  cd, "message header",
    "<trailer>", ARGV_STRING,  NULL, &msgTrailer, cd, "message trailer",
 NULL) != TCL_OK) return TCL_ERROR;

  /*------------------------
  / does the feature exist?
  /------------------------*/
  fIndex = -1;
  for (i=0; i<featSet->featN; i++)
    if (featSet->featA[i].name && (strcmp(featSet->featA[i].name, featName)==0))
      fIndex = i;
  if (fIndex>=0)
  {
    /*------------------------------------------
    / check that destination feature is SVector
    /------------------------------------------*/
    feat = &(featSet->featA[fIndex]);
    if ((feat==NULL) || (feat->type!=FE_SVECTOR))
    {
      ERROR ("paGet: feature must be SVector\n");
      return(TCL_ERROR);
    }
  }
  else
  {
    /*-----------------------
    / create SVector feature
    /-----------------------*/
    fIndex = fesGetIndex(featSet, featName, 1);
    if (fIndex>=0)
      feat = &(featSet->featA[fIndex]);
    else
    {
      ERROR("paGet: failed to create feature.\n");
      return(TCL_ERROR);
    }
    feat->data.svec     = svectorCreate(0);
    feat->dcoeffN       = 0;
    feat->dcoeffN       = 0;
    feat->type          = FE_SVECTOR;
    feat->dtype         = FE_SVECTOR;
  }

  /*------------
  / get lengths
  /------------*/
  msgLen   = strlen(msg);
  headLen  = strlen(msgHeader);
  trailLen = strlen(msgTrailer);

  /*-----------------------
  / check header & trailer
  /------------------------*/
  if (memcmp(msg, msgHeader, headLen)!=0)
  {
    ERROR ("fesAppendAESVMItf: expected header [%s]\n", msgHeader);
    return(TCL_ERROR);
  }
  if (memcmp(msg+msgLen-trailLen, msgTrailer, trailLen)!=0)
  {
    ERROR ("fesAppendAESVMItf: expected trailer [%s]\n", msgTrailer);
    return(TCL_ERROR);
  }

  /*---------------
  / empty message?
  /---------------*/
  if (msgLen==headLen+trailLen)
    return TCL_OK;

  /*-------------------------------------------
  / check initial and terminal quotation marks
  /-------------------------------------------*/
  if ((msgLen<headLen+trailLen+2) || (msg[headLen]!='\"') || (msg[msgLen-trailLen-1]!='\"')) {
    ERROR ("fesAppendAESVMItf: expected PROCESS(\"<ASCII-encoded short array>\")\n");
    return TCL_ERROR;
  }

  /*------------------------------------------
  / skip initial and terminal quotation marks
  /------------------------------------------*/
  headLen++;
  trailLen++;

  /*--------------------------
  / extract & decode b6 array
  /--------------------------*/
  /* (*(msg msgLen-trailLen)) = 0; */
  b6A    = msg+headLen;
  b6ALen = msgLen-headLen-trailLen;
  nBytes = decodeByteArray(b6A, msgLen-headLen-trailLen, &b8A);

  /*------------------------------------
  / check that the message wasn't empty
  /------------------------------------*/
  if (nBytes==0)
  {
    ERROR ("fesAppendAESVMItf: nBytes was 0!\n");
    return(TCL_ERROR);
  }

  /*--------------------------------------------------
  / check that we got an even number of encoded bytes
  /--------------------------------------------------*/
  if ((nBytes%(sizeof(short)))!=0)
  {
    ERROR ("fesAppendAESVMItf: uneven nuber of bytes!\n");
    return(TCL_ERROR);
  }

  /*---------------
  / update SVector
  /---------------*/
  nShorts = floor(nBytes / sizeof(short));
  v = feat->data.svec;
  v->vecA = realloc(v->vecA, sizeof(short)*(v->n + nShorts));
  if (v->vecA==NULL)
  {
    ERROR ("paGet: could not reallocate feature memory\n");
    return(TCL_ERROR);
  }
  memcpy(v->vecA+v->n, b8A, sizeof(short)*nShorts);
  v->n += nShorts;

  /*---------
  / clean up
  /---------*/
  if (b8A!=NULL)
    free(b8A);

  return(TCL_OK);
}
