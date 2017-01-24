/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Discrete Wavelet Transform
               ------------------------------------------------------------

    Author  :  Michael Wand
    Module  :  featureDWT.h
    Date    :  $Id: featureDWT.h 2768 2007-02-23 09:34:33Z fuegen $
    Remarks :

   ========================================================================

    This software was developed by

    the  University of Karlsruhe       and  Carnegie Mellon University
         Dept. of Informatics               Dept. of Computer Science
         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
         Am Fasanengarten 5                 5000 Forbes Ave
         D-76131 Karlsruhe                  Pittsburgh, PA 15213
         - Germany -                        - USA -

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
   Revision 4.2  2007/02/23 09:34:33  fuegen
   changed tsize from size_t to int to remove icc compiler warnings

   Revision 4.1  2007/02/08 09:51:37  stueker
   Initial check-in of Discrete Wavelet Transformation from Michael Wand


   ======================================================================== */


#ifdef __cplusplus
extern "C" {
#endif

#ifndef _featureDWT
#define _featureDWT

#include "common.h"
#include "featureFilter.h"

#ifndef PI
#define PI 3.14159265358979323846
#endif

/* Type for coefficients and stuff */	
#define real float	

/* Type for input data sizes */
#define tsize int

	
/* ========================================================================
    Module Interface
   ======================================================================== */

int fesDTCWTItf(ClientData cd,int argc,char *argv[]); /* Access function for the DTCWT (double-tree complex wavelet transform after Kingsbury)*/
int fesFWTItf(ClientData cd,int argc,char *argv[]);   /* Access function for the FWT (real decimated form after Mallat)*/
int fesRDWTItf(ClientData cd,int argc,char *argv[]);  /* Access function for the RDWT (redundant wavelet transform, like FWT but without decimation)*/
	

/* ========================================================================
    Functions Declaration
   ======================================================================== */

/* Does internal initialization for the discrete wavelet transform. */
void initDWT();

/* Performs the double-tree complex wavelet transformation on the given data. 
 * RETURN: 1 if successful, 0 if not
 */ 
int doDTCWT(FeatureSet* fs,Feature* dest,Feature* source,Filter* filter0a,Filter* filter0b,
		Filter* filter1a,Filter* filter1b,int level,int useLowpass);

/* Performs the fast wavelet transformation on the given data. 
 * RETURN: 1 if successful, 0 if not
 */ 
int doFWT(FeatureSet* fs,Feature* dest,Feature* source,Filter* filter,int level,int useLowpass);
		
/* Performs an FWT step.
 * RETURN: the size of details and lowpass is returned. details and lowpass are newly malloc'ed.
 */
tsize doFWTStep(real* inData,tsize inDataSize,real** details,real** lowpass,Filter* filter);

/* Performs an FWT step on an array of shorts instead of a column of FMatrix.
 * RETURN: the size of details and lowpass is returned. details and lowpass are newly malloc'ed.
 */
tsize doFWTStepSVec(short* inData,tsize inDataSize,real** details,real** lowpass,Filter* filter);

/* Dilates Filter f by inserting z zeroes between the values (leaving position 0 unchanged).
 * RETURN: a newly malloced, changed filter.
 */
Filter* dilateFilter(Filter* f,int z);

/* Performs the redundant discrete wavelet transformation on the given data. 
 * RETURN: 1 if successful, 0 if not
 */ 
int doRDWT(FeatureSet* fs,Feature* dest,Feature* source,Filter* filter,int level,int useLowpass);
		
/* Performs an RDWT step.
 * RETURN: the size of details and lowpass is returned. details and lowpass are newly malloc'ed.
 */
tsize doRDWTStep(real* inData,tsize inDataSize,real** details,real** lowpass,Filter* filter);

/* Performs an RDWT step on an array of shorts instead of a column of FMatrix.
 * RETURN: the size of details and lowpass is returned. details and lowpass are newly malloc'ed.
 */
tsize doRDWTStepSVec(short* inData,tsize inDataSize,real** details,real** lowpass,Filter* filter);

#ifdef __cplusplus
}
#endif

#endif /* _featureDWT */
