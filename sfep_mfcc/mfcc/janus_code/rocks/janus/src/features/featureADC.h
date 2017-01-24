/* ========================================================================
               Speech tools
               ------------------------------------------------------------
               Object: audio IO
               ------------------------------------------------------------

    Author  :  Martin Westphal
    Module  :  featureADC.h
    Date    :  $Id: featureADC.h 2894 2009-07-27 15:55:07Z metze $

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

    $Log: featureADC.h,v $
    Revision 1.1.1.2  2008/09/10 17:01:48  metze.florian
    FFa+FMe

    Revision 1.1.1.1.4.1  2008/09/09 10:08:11  metze.florian
    Added '-append' to writeADC, added WAV & alaw support, cleaned up SPHERE

    Revision 1.1.1.1  2007/06/20 12:34:50  metze.florian
    Initial import of Ibis 5.1 from UKA at T-Labs

    Revision 3.4  2000/08/27 15:31:13  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.3.4.1  2000/08/25 22:19:43  jmcd
    Just checking.

    Revision 3.3  1997/07/30 11:59:14  westphal
    changed readADC()

    Revision 3.2  1997/07/09 17:42:22  westphal
    *** empty log message ***

   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------------------------
   Verbosity
   ------------------------------------------------------------------------- */
void adc_verbosity  (int verbosity);
void adc_maxbuf     (int size);

/* ----------------------------------------------------------------------------
   write
   ------------------------------------------------------------------------- */
void setADCremarks  (int remN, char** remark);
void setADCsamplRate(int samplRate);
void setADCsamplN   (int sampleN);
void setADCbyteMode (char*  byteModeS);


int adc_write       (char* fname, short* adcA, int adcN, char* head_mode,
		     char* byte_order, char** headSA, int headSN,
		     int sampl_rate);
int writeADC        (char* fname, short* adcA, int adcN, char* headMode,
		     char* byteMode, int samplingRate, int append);
int writeADC_fp     (FILE* fp, short* adcA, int adcN, char* byteMode);

int writeHeaderKA   (FILE *fp);
int writeHeaderWAV  (FILE *fp);
int readHeaderKA    (FILE *fp);

/* ----------------------------------------------------------------------------
   read
   ------------------------------------------------------------------------- */
int getADCremarks   	(int* remN, char*** remark);
int getADCsamplRate 	(int *samplRate);
int getADCsamplN	(int *samplN);
int getADCbyteMode	(char** byteMode);



int isADCsmooth     (void);
int isADCswapped    (void);
int adc_read        (char* fname, short** adcAP, char*	head_mode,
		     char* byte_order, char***  headSAP, int* headSN,
		     int* sampl_rate);
int readADC         (char* fname, short** adc, char* headMode, char* byteMode,
		     int chX, int chN, int from, int to, int force
#ifdef LIBSNDFILE
		     , char* fformat, char* subtype, char* endianness
#endif
		     );
int readADCrunon    (char* fname, short** adc, char* headMode, char* byteMode,
		     int chX, int chN, int init, int ready
#ifdef LIBSNDFILE
		     , char* fformat, char* subtype, char* endianness
#endif
		     );

/* ----------------------------------------------------------------------------
   check smoothness
   ------------------------------------------------------------------------- */
int check_smoothness(short* adcA, int adcN, int speed, int vb2, float* avg_d);



#ifdef __cplusplus
}
#endif
