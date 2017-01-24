/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: FeatureSet (featureModalities)
               ------------------------------------------------------------

    Author  :  Christian Fuegen
    Module  :  featureModalities.c
    Date    :  $Id: featureModalities.c 3416 2011-03-23 03:02:18Z metze $
    Remarks :  This Module includes functions to evaluate a feature for
               the use with modality questions

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
    Revision 1.8  2005/03/04 09:05:51  metze
    Made fesSNR safer

    Revision 1.7  2005/02/15 15:54:33  metze
    Added -abs to avMagnitude

    Revision 1.6  2003/08/14 11:19:56  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.5.4.1  2002/01/19 11:48:28  metze
    Uncommented intcompare to get rid of error message

    Revision 1.5  2001/01/15 09:49:57  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 1.4  2000/12/03 23:31:09  janus
    Merged branch jtk-00-11-14-jmcd.

    Revision 1.3.38.1  2000/12/03 22:10:42  janus
    Fixed SUN compilation at CMU.

    Revision 1.3.32.1  2001/01/12 15:16:54  janus
    necessary changes for running janus under WINDOWS

    Revision 1.3  2000/09/14 11:37:01  janus
    merging branch jtk-00-09-11-fuegen-2

 * Revision 1.2.20.1  2000/09/11  16:04:43  janus
 * Merging outdated branch jtk-00-09-08-fuegen.
 * 
    Revision 1.2.14.1  2000/09/08 15:00:08  janus
    added M. Westphal signal adaption (MAM)

 * Revision 1.2  2000/01/12  10:02:02  fuegen
 * Modality dependent feature functions
 * 
    Revision 1.1  1999/03/16 15:07:14  fuegen
    Initial revision



   ======================================================================== */

char featureModalitiesRcsVersion[]=
  "@(#)1$Id: featureModalities.c 3416 2011-03-23 03:02:18Z metze $";


/* ========================================================================
    includes
   ======================================================================== */

#include "featureMethodItf.h"
#include "featureModalities.h"
#include "featureFFT.h"

#define MODDEBUG

/* -------------------------------------------------------------------------
   comparison functions for qsort
   ------------------------------------------------------------------------- */

/* Not in use:
static int intcompare (const void *a, const void *b) {

    int *i = (int*)a;
    int *j = (int*)b;

    if (*i > *j) return  1;
    if (*i < *j) return -1;

    return 0;
}
*/
static int floatcompare (const void *a, const void *b) {

    float *i = (float*)a;
    float *j = (float*)b;

    if (*i > *j) return  1;
    if (*i < *j) return -1;

    return 0;
}


void q2sort1 (float a[], float b[], int l, int r) {

    float t;
    int i, last;

    if ( l >= r ) return;

    t = a[l]; a[l] = a[(l+r)/2]; a[(l+r)/2] = t;
    t = b[l]; b[l] = b[(l+r)/2]; b[(l+r)/2] = t;

    last = l;
    for (i=l+1; i<=r; i++) {
	if ( a[i] < a[l] ) {
	    last++;
	    t = a[last]; a[last] = a[i]; a[i] = t;
	    t = b[last]; b[last] = b[i]; b[i] = t;
	}
    }
    
    t = a[l]; a[l] = a[last]; a[last] = t;
    t = b[l]; b[l] = b[last]; b[last] = t;

    q2sort1 (a, b, l, last-1);
    q2sort1 (a, b, last+1, r);
}

void q2sort (float a[], float b[], int l, int r) {

    int   i, j;
    float v, t;

    if ( r > l ) {

	i = l-1; j = r; v = a[r];
	for(;;) {
	    while (a[++i] < v) ;
	    while (a[--j] > v) ;
	    if ( i >= j ) break;

	    t = a[i]; a[i] = a[j]; a[j] = t;
	    t = b[i]; b[i] = b[j]; b[j] = t;
	}

	t = a[i]; a[i] = a[r]; a[r] = t;
	t = b[i]; b[i] = b[r]; b[r] = t;

	q2sort(a, b, l, i-1);
	q2sort(a, b, i+1, r);
    }
}

/* -------------------------------------------------------------------------
   quantil
   returns the alpha quantil of array
   ------------------------------------------------------------------------- */

float quantil (float *inA, int inN, float alpha, int sorted) {

    float *sortA = NULL;
    float  m     = inN * alpha;
    float  res;

    if ( !sorted ) {
	sortA = (float*)malloc(inN * sizeof(float));
	memcpy ((void*)sortA, (void*)inA, inN * sizeof(float));
	qsort ((void*)sortA, inN, sizeof(float), floatcompare);
    } else
	sortA = inA;

    if ( floor(m) != ceil(m) ) {
	/* m is not integer */
	int index = (int)floor (m);
	res = sortA[index];
    } else {
	/* m is integer */
	int index = (int)floor (m) - 1;
	res = (sortA[index] + sortA[index+1]) / 2.0;
    }

    if ( !sorted ) free(sortA);

    return res;
}

float variance (float *inA, int inN, float mean) {

    float var = 0;
    int   i;

    for (i=0; i<inN; i++) {
	var += ((float)inA[i] - mean) * ((float)inA[i] - mean);
    }

    var /= (float)(inN - 1);

    return var;
}

/* -------------------------------------------------------------------------
   k-means
   ------------------------------------------------------------------------- */

float euklidDist (float x, float y) {
    return ((x-y) * (x-y));
}


int compute2means (float *inA, int inN, float *lowMean, float *highMean, int iterN) {

    int   iter, i;
    float m[2], a[2], c[2];

    m[0] = *lowMean;
    m[1] = *highMean;

    for (iter=0; iter < iterN; iter++) {

	c[0] = c[1] = a[0] = a[1] = 0;
	for (i=0; i<inN; i++) {
	    if ( euklidDist (inA[i], m[0]) < euklidDist (inA[i], m[1]) ) {
		a[0] += inA[i];
		c[0]++;
	    } else {
		a[1] += inA[i];
		c[1]++;
	    }
	}
	m[0] = a[0] / c[0];
	m[1] = a[1] / c[1];
    }

    *lowMean  = m[0];
    *highMean = m[1];

    return 0;
}




/* -------------------------------------------------------------------------
   do_window
   returns window multiplikators for the signal in frame
   ------------------------------------------------------------------------- */

float* do_window (int winPoints, char *type) {

    float *res = NULL;
    int i;
    double tmp1, tmp2;

    if ( winPoints < 0 ) {
	ERROR("Negative number of window points %i.\n", winPoints);
	return NULL;
    }

    res = (float*)malloc (winPoints * sizeof(float));
    tmp1 = M_2PI / (double)(winPoints-1);
    tmp2 = M_4PI / (double)(winPoints-1);

    if ( !res ) {
	ERROR("Allocating window.\n");
	return NULL;
    }

    /* compute window */
    if ( !strcmp (type, "hamming") ) {
	for ( i=0; i<winPoints; i++ )
	    res[i] = 0.54 - 0.46*cos(tmp1*i);
    } else if ( !strcmp (type, "hanning") ) {
	for ( i=0; i<winPoints; i++ )
	    res[i] = 0.5 - 0.5*cos(tmp1*i);
    } else if ( !strcmp (type, "blackman") ) {
	for ( i=0; i<winPoints; i++ )
	    res[i] = 0.42 - 0.5*cos(tmp1*i) + 0.08*cos(tmp2*i);
    } else if ( !strcmp (type, "rectangular") ) {
	for ( i=0; i<winPoints; i++ )
	    res[i] = 1.0;
    } else {
	ERROR("Unknown window type %s.\n", type);
	return NULL;
    }

    return res;
}


/* ------------------------------------------------------------------------
   fesSilenceDetection
   frame based silence detection
   ------------------------------------------------------------------------ */

/* fesSilenceClassification
   returns 0 if succesful */
int fesSilenceClassification1 (FMatrix **silenceP, Feature *other,
			      Feature *zeroX, float otherThres, float speechMean,
			      float zeroThres, int zeroLength, int smoothPasses,
			      int smoothLength, char *method) {

    FMatrix *silence      = NULL;
    FMatrix *otherMat     = other->data.fmat;
    FMatrix *zeroMat      = zeroX->data.fmat;
    int      zeroExceed   = 0;
    int      minSilLength = 5;
    float    silMean      = 0;

    int      frameA, frameX, frameN;
    int      smoothThres, i, j;

    frameN  = otherMat->m;
    silence = fmatrixCreate(frameN,1);

    /* classify frames */
    if ( !strcmp (method, "gradient") ) {
	frameA = 0;
	foreach_frame {
	    /* zero crossings */
	    if ( zeroExceed ) zeroExceed++;
	    if ( zeroMat->matPA[frameX][0] < zeroThres ) zeroExceed = 1;

	    /* magnitude or energy */
	    if ( otherMat->matPA[frameX][0] < otherThres ) {
		/* magnitude low but zero crossings too high */
		silence->matPA[frameX][0] = 1.0;
		if ( zeroExceed <= zeroLength ) {
		    silence->matPA[frameX][0] = 2.0;
		}
	    } else {
		silence->matPA[frameX][0] = 0.0;
	    }
	}

    } else if ( !strcmp (method, "kmeans") ) {
	silMean = otherThres;
	frameA  = 0;
	foreach_frame {
	    /* zero crossings */
	    if ( zeroExceed ) zeroExceed++;
	    if ( zeroMat->matPA[frameX][0] < zeroThres ) zeroExceed = 1;

	    if ( euklidDist (otherMat->matPA[frameX][0], silMean) < 
		 euklidDist (otherMat->matPA[frameX][0], speechMean) ) {
		silence->matPA[frameX][0] = 1.0;
		if ( zeroExceed <= zeroLength ) {
		    silence->matPA[frameX][0] = 2.0;
		}
	    } else {
		silence->matPA[frameX][0] = 0.0;
	    }
	}
    }

    /* smoothing */
    if ( smoothPasses ) {
	int bool  =  0;
	int begin = -1;

	/* set all 1.0 between surrounding 2.0 to 2.0 */
	frameA = 0;
	foreach_frame {
	    if ( silence->matPA[frameX][0] == 0.0 )
		bool  = 0;
	    if ( silence->matPA[frameX][0] == 1.0 &&  bool && begin < 0 )
		begin = frameX;
	    if ( silence->matPA[frameX][0] == 2.0 && !bool ) {
		bool  =  1;
		begin = -1;
	    }
	    if ( silence->matPA[frameX][0] == 2.0 &&  bool && begin >= 0 ) {
		for (i=begin; i<frameX; i++) {
		    silence->matPA[i][0] = 2.0;
		}
		bool  =  0;
		begin = -1;
	    }
	}

	/* set all 1.0 to 0.0 and all 2.0 to 1.0 (for easier smoothing) */
	frameA = 0;
	foreach_frame {
	    if ( silence->matPA[frameX][0] == 1.0 ) silence->matPA[frameX][0] = 0.0;
	    if ( silence->matPA[frameX][0] == 2.0 ) silence->matPA[frameX][0] = 1.0;
	}

	/* median smoothing */
	smoothThres = (int)(floor((float)smoothLength / 2));
	frameA      = (int)(floor((float)smoothLength / 2));
	for (i=0; i<smoothPasses; i++) {
	    for (frameX=frameA; frameX<frameN-frameA; frameX++) {
		int sum = 0;
		
		for (j=frameA; j>0; j--)
		    sum += (int)(silence->matPA[frameX-j][0]) + 
			   (int)(silence->matPA[frameX+j][0]);
		
		if ( sum > smoothThres && silence->matPA[frameX][0] < 1.0 )
		    silence->matPA[frameX][0] = 1.0;
		if ( sum < smoothThres && silence->matPA[frameX][0] > 0.0 )
		    silence->matPA[frameX][0] = 0.0;
	    }
	}

	/* delete all silences with a length smaller than minSilLength
	   let this away if you want to make a wordboundary detection */
	begin  = -1;
	frameA =  0;
	foreach_frame {
	    if ( silence->matPA[frameX][0] == 1.0 && begin < 0 ) begin = frameX;
	    if ( silence->matPA[frameX][0] == 0.0 ) {
		if ( begin > 0 && frameX-begin < minSilLength ) {
		    for (i=begin; i<frameX; i++) {
			silence->matPA[i][0] = 0.0;
		    }
		}
		begin = -1;
	    }
	}
    }

    *silenceP = silence;

    return 0;
}

int fesSilenceThresholds3 (float *silMean, float *speechMean, float *zeroThres, 
			   Feature *other, Feature *zeroX, float silZeroFactor,
			   int automatic, char *feType) {

    FMatrix *otherMat = other->data.fmat;
    FMatrix *zeroMat  = zeroX->data.fmat;
    float   *sortOtherA, *sortZeroA;
    float    otherMean, zeroMean;
    float    silZeroThres;
    float    otherMin, otherMax, silZeroThresMax, silZeroThresMin;
    int      frameA, frameX, frameN;
    float    sum, max;
    int      silFrameN;
    int      kmeansIterN = 10;

    if ( automatic ) {
	if ( !strcmp (feType, "magnitude") ) {
	    silZeroFactor  = 1.0;
	} else if ( !strcmp (feType, "power") ) {
	    silZeroFactor  = 1.0;
	}
    }

    frameN      = otherMat->m;
    sortOtherA  = (float*)malloc (frameN * sizeof(float));
    sortZeroA   = (float*)malloc (frameN * sizeof(float));

    if ( !sortOtherA || !sortZeroA ) return 1;

    /* copy values */
    frameA    = 0;
    otherMean = zeroMean = 0;
    foreach_frame {
	otherMean += sortOtherA[frameX] = otherMat->matPA[frameX][0];
	zeroMean  += sortZeroA[frameX]  = zeroMat->matPA[frameX][0];
    }

    otherMean /= frameN;
    zeroMean  /= frameN;

    /* get a silence representation */
    q2sort (sortOtherA, sortZeroA, 0, frameN-1);
    otherMin  = sortOtherA[0];
    otherMax  = sortOtherA[frameN-1];
    printf("otherMin %f, otherMax %f", otherMin, otherMax);
    compute2means (sortOtherA, frameN, &otherMin, &otherMax, kmeansIterN);
    printf(" -> otherMin %f, otherMax %f.\n", otherMin, otherMax);

    frameX = 0;
    while ( euklidDist (sortOtherA[frameX], otherMin) <
	    euklidDist (sortOtherA[frameX], otherMax) ) frameX++;

    silFrameN = frameX;

    /* zero crossings */
    frameA = 0;
    sum    = 0;
    max    = 0;
    foreach_frame {
	if ( frameX >= silFrameN ) break;
	sum += sortZeroA[frameX];
	if ( max < sortZeroA[frameX] ) max = sortZeroA[frameX];
    }

    sum /= silFrameN;
    silZeroThresMax = max - (max-sum)/3;
    silZeroThresMin = sum;
    silZeroThres    = silZeroThresMax - silZeroFactor *
	              (silZeroThresMax - silZeroThresMin);


    printf("silOtherThres (%f, %f), silZeroThres (%f, %f, %f).\n",
	   otherMin, otherMax, silZeroThresMin, silZeroThres, silZeroThresMax);
    printf("silFrameN %i, otherMean %f, zeroMean %f, frameN %i.\n",
	   silFrameN, otherMean, zeroMean, frameN);

    *silMean    = otherMin;
    *speechMean = otherMax;
    *zeroThres  = silZeroThres;

    free (sortOtherA);
    free (sortZeroA);

    return 0;
}


/* other can be either power or magnitude */
int fesSilenceThresholds1 (float *otherThres, float *zeroThres,
			   Feature *other, Feature *zeroX, float silOtherFactor,
			   float silZeroFactor, int automatic, char *feType) {

    FMatrix *otherMat = other->data.fmat;
    FMatrix *zeroMat  = zeroX->data.fmat;
    float   *sortOtherA, *sortZeroA;
    float    otherMean,   zeroMean;
    float    silOtherThres, silZeroThres;
    float    silOtherThresMax, silOtherThresMin, silZeroThresMax, silZeroThresMin;
    int      frameA, frameX, frameN, n;
    int      silFrameN, silArea;
    float    sum, dx, dy, max;
    float    meanM = 0;

    if ( automatic ) {
	if ( !strcmp (feType, "magnitude") ) {
	    silOtherFactor = 0.3;
	    silZeroFactor  = 1.0;
	} else if ( !strcmp (feType, "power") ) {
	    silOtherFactor = 0.5;
	    silZeroFactor  = 1.0;
	}
    }

    frameN      = otherMat->m;
    sortOtherA  = (float*)malloc (frameN * sizeof(float));
    sortZeroA   = (float*)malloc (frameN * sizeof(float));

    if ( !sortOtherA || !sortZeroA ) return 1;

    /* copy values */
    frameA    = 0;
    otherMean = zeroMean = 0;
    foreach_frame {
	otherMean += sortOtherA[frameX] = otherMat->matPA[frameX][0];
	zeroMean  += sortZeroA[frameX]  = zeroMat->matPA[frameX][0];
    }

    otherMean /= frameN;
    zeroMean  /= frameN;

    /* get a silence representation (take always 10 frames together) */
    q2sort (sortOtherA, sortZeroA, 0, frameN-1);

    silArea = 5;
    frameA  = silArea;
    meanM   = 0;
    dx      = (float)silArea;
    for (frameX = frameA; frameX < frameN; frameX += silArea) {
	dy    = sortOtherA[frameX] - sortOtherA[frameX-silArea];
	meanM = meanM + (dy / dx);
    }
    n     = frameN / silArea; if ( n == 0 ) return 0;
    meanM = meanM / (float)n;

    /* Finde Schnittpunkt der Geraden y=sortOtherA[0] mit der Tangente an der
       Kurve mit der Steigung meanM. */
    silArea = 5;
    frameA  = silArea;
    dx      = (float)silArea;
    for (frameX = frameA; frameX < frameN; frameX += silArea) {
	dy = sortOtherA[frameX] - sortOtherA[frameX-silArea];
	if ( dy / dx >= meanM ) break;
    }

    printf("silFrameN %i, sortOther %f, ", frameX, sortOtherA[frameX-1]);
    silFrameN = frameX;
    sum       = sortOtherA[silFrameN-1];
    for ( frameX = silFrameN-2; frameX >= 0; frameX-- ) {
	sum -= meanM;
	if ( sum <= sortOtherA[0] ) break;
    }

    /* Schnittpunkt gefunden an der Stelle frameX
       ermittle thresholds */
    silFrameN        = frameX+1;
    silOtherThresMax = sortOtherA[silFrameN-1];
    silOtherThresMin = quantil (sortOtherA, frameN, 0.25, 1);
    silOtherThres    = silOtherThresMax - silOtherFactor * 
	               (silOtherThresMax - silOtherThresMin);

    /* zero crossings */
    frameA = 0;
    sum    = 0;
    max    = 0;
    foreach_frame {
	if ( frameX >= silFrameN ) break;
	sum += sortZeroA[frameX];
	if ( max < sortZeroA[frameX] ) max = sortZeroA[frameX];
    }

    sum /= silFrameN;
    silZeroThresMax = max - (max-sum)/3;
    silZeroThresMin = sum;
    silZeroThres    = silZeroThresMax - silZeroFactor *
	              (silZeroThresMax - silZeroThresMin);

    printf("silFrameN %i, silOtherFactor %f, silZeroFactor %f.\n", silFrameN,
	   silOtherFactor, silZeroFactor);
    printf("silOtherThres (%f, %f, %f), silZeroThres (%f, %f, %f).\n",
	   silOtherThresMin, silOtherThres, silOtherThresMax,
	   silZeroThresMin, silZeroThres, silZeroThresMax);
    printf("meanM %f, otherMean %f, zeroMean %f, frameN %i.\n",
	   meanM, otherMean, zeroMean, frameN);

    *otherThres = silOtherThres;
    *zeroThres  = silZeroThres;

    free (sortOtherA);
    free (sortZeroA);

    return 0;
}


/* old method */
int fesSilenceThresholds2 (float *magnitudeThres, float *zeroThres,
			  Feature *avMagnitude, Feature *zeroX, float magAlpha,
			  float zeroAlpha, int automatic, char *feType) {

    FMatrix *magMat  = avMagnitude->data.fmat;
    FMatrix *zeroMat = zeroX->data.fmat;
    float   *sortMagA, *sortZeroA;
    float    magMean,   zeroMean;
    int      frameA, frameX, frameN;

#ifdef MODDEBUG
    int   length;
    float magLow, magMed, magUpp, magSDe, magLowM, magHighM;
    float zerLow, zerMed, zerUpp, zerSDe, zeroLowM, zeroHighM;
#endif

    frameN    = magMat->m;
    sortMagA  = (float*)malloc (frameN * sizeof(float));
    sortZeroA = (float*)malloc (frameN * sizeof(float));

    /* copy values */
    frameA = 0;
    magMean = zeroMean = 0;
    foreach_frame {
	magMean  += sortMagA[frameX]  = magMat->matPA[frameX][0];
	zeroMean += sortZeroA[frameX] = zeroMat->matPA[frameX][0];
    }

    magMean  /= frameN;
    zeroMean /= frameN;

#ifdef MODDEBUG
    q2sort (sortMagA, sortZeroA, 0, frameN-1);

    length  = (frameN/3 < 30 ? frameN/3 : 30);
    magLowM = magHighM = zeroLowM = zeroHighM = 0;
    for (frameX=0; frameX<length; frameX++) {
	magLowM   += sortMagA[frameX];
	magHighM  += sortMagA[frameN-frameX];
	zeroLowM  += sortZeroA[frameX];
	zeroHighM += sortZeroA[frameN-frameX];
    }
    magLowM   /= length;
    magHighM  /= length;
    zeroLowM  /= length;
    zeroHighM /= length;
    
    magLow = quantil  (sortMagA,  frameN, 0.25, 1);
    magMed = quantil  (sortMagA,  frameN, 0.50, 1);
    magUpp = quantil  (sortMagA,  frameN, 0.75, 1);
    magSDe = sqrt (variance (sortMagA,  frameN, magMean));
    zerLow = quantil  (sortZeroA, frameN, 0.25, 0);
    zerMed = quantil  (sortZeroA, frameN, 0.50, 0);
    zerUpp = quantil  (sortZeroA, frameN, 0.75, 0);
    zerSDe = sqrt (variance (sortZeroA, frameN, zeroMean));
    
    printf ("magnitude : low %f, med %f, upp %f, av %f, sdev %f.\n",
	    magLow, magMed, magUpp, magMean,  magSDe);
    printf ("            lowM %f, highM %f.\n", magLowM, magHighM);
    printf ("zero      : low %f, med %f, upp %f, av %f, sdev %f.\n",
	    zerLow, zerMed, zerUpp, zeroMean, zerSDe);
    printf ("            lowM %f, highM %f.\n", zeroLowM, zeroHighM);

#endif

    if ( automatic ) {
	if ( !strcmp(feType, "magnitude") ) {
	    *magnitudeThres = magMean  / 4.5;
	    *zeroThres      = zeroMean * 2.0;
	} else {
	    *magnitudeThres = magLowM * 3;
	    *zeroThres      = (zeroHighM - zeroLowM) * 0.6;
	}
    } else {
	qsort ((void*)sortMagA,  frameN, sizeof(float), floatcompare);
	qsort ((void*)sortZeroA, frameN, sizeof(float), floatcompare);

	*magnitudeThres = quantil (sortMagA,  frameN, magAlpha,  1);
	*zeroThres      = quantil (sortZeroA, frameN, zeroAlpha, 1);
    }

#ifdef MODDEBUG
    printf ("magThres: %f, zeroThres: %f.\n", *magnitudeThres, *zeroThres);
    fflush (stdout);
#endif

    free (sortMagA);
    free (sortZeroA);

    return 0;
}


int fesSilenceDetectionItf1 (ClientData cd, int argc, char *argv[]) {

    FeatureSet *fs             = (FeatureSet*)cd;
    Feature    *destin         = NULL;
    Feature    *other          = NULL;
    Feature    *zeroX          = NULL;
    FMatrix    *silence        = NULL;
    float       silOtherFactor = 0.3;
    float       silZeroFactor  = 1.0;
    int         zeroLength     = 7;
    int         smoothPasses   = 5;
    int         smoothLength   = 5;
    int         automatic      = 1;
    char       *feType         = "magnitude";
    char       *method         = "gradient";
    float            zeroThres = 0;
    float              silMean = 0;
    float           speechMean = 0;
    float           otherThres = 0;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<feature>", ARGV_CUSTOM, createFe, &destin, cd,
		       		NEW_FEAT,
		       "<other>", ARGV_CUSTOM, getFe, &other, cd,
		       		"other feature (see -feType)",
		       "<zeroX>", ARGV_CUSTOM, getFe, &zeroX, cd,
		       		"zero crossing feature",
		       "-otherFactor", ARGV_FLOAT, NULL, &silOtherFactor, NULL,
		       		"factor to multiply with other thresholds",
		       "-zeroFactor", ARGV_FLOAT, NULL, &silZeroFactor, NULL,
		       		"factor to multiply with zero thresholds",
		       "-zero", ARGV_INT, NULL, &zeroLength, NULL,
		       		"number of frames to the last hight zeroX",
		       "-sPassN", ARGV_INT, NULL, &smoothPasses, NULL,
		       		"passes to smooth",
		       "-smooth", ARGV_INT, NULL, &smoothLength, NULL,
		       		"smooth length",
		       "-auto", ARGV_INT, NULL, &automatic, NULL,
		       		"compute thresholds automatic",
		       "-feType", ARGV_STRING, NULL, &feType, NULL,
		       		"type of feature (magnitude, power)",
		       "-method", ARGV_STRING, NULL, &method, NULL,
		       		"method for calculation (gradient, kmeans)",
		       NULL) != TCL_OK )
	return TCL_ERROR;

    if ( !IS_FMATRIX (other) || !IS_FMATRIX (zeroX) ) {
	ERROR ("Both, other and zeroX must be FMATRIX features.\n");
	return TCL_ERROR;
    }

    if ( !strcmp (method, "gradient") ) {
	if ( fesSilenceThresholds1 (&otherThres, &zeroThres, other, zeroX,
				    silOtherFactor, silZeroFactor, automatic,
				    feType) ) {
	    ERROR ("computing thresholds failed.\n");
	    return TCL_ERROR;
	}
	itfAppendResult ("%f %f", otherThres, zeroThres);

    } else if ( !strcmp (method, "kmeans") ) {
	if ( fesSilenceThresholds3 (&silMean, &speechMean, &zeroThres, other, zeroX,
				    silZeroFactor, automatic, feType) ) {
	    ERROR ("computing thresholds failed.\n");
	    return TCL_ERROR;
	}
	otherThres = silMean;
	itfAppendResult ("%f %f %f", silMean, speechMean, zeroThres);
    }

    if ( fesSilenceClassification1 (&silence, other, zeroX, otherThres, speechMean,
				   zeroThres, zeroLength, smoothPasses,
				   smoothLength, method) ) {
	ERROR ("classification failed.\n");
	if (silence) fmatrixFree (silence);
	return TCL_ERROR;
    }

    if ( feFMatrix (fs, destin, silence, 0) ) {
	ERROR ("can't link matrix to feature.\n");
	if (silence) fmatrixFree (silence);
	return TCL_ERROR;
    }

    destin->samplingRate = other->samplingRate;
    destin->shift        = other->shift;

    return TCL_OK;
}

/* -------------------------------------------------------------------------
   frame based average magnitude
   ------------------------------------------------------------------------- */

int fesAvMagnitude (SVector *vec, FMatrix **avMagP, int winPoints, int shiftPoints,
		    double mean, int dolog, int doabs) {

    FMatrix *avMag;
    int      frameA, frameX, frameN, i;

    frameN = (int)((vec->n - (winPoints - shiftPoints)) / shiftPoints);
    avMag  = fmatrixCreate (frameN, 1);

    frameA = 0;
    foreach_frame {
        int    offset    = shiftPoints * frameX;
        float  magnitude = 0.0;
	double value;

        for ( i=0; i<winPoints; i++ ) {
	    value      = vec->vecA[i+offset] - mean;
	    if        (dolog) {
	        magnitude += log (fabs (value) + 1.0) / log (2.0);
	    } else if (doabs) {
	        magnitude += fabs (value);
	    } else {
	        magnitude += value;
	    }
	}

	avMag->matPA[frameX][0]  = magnitude / winPoints;
    }

    *avMagP = avMag;

    return 0;
}


int fesAvMagnitudeItf (ClientData cd, int argc, char *argv[]) {

    FeatureSet *fs      = (FeatureSet*)cd;
    Feature    *destin  = NULL;
    Feature    *source  = NULL;
    char       *cwin    = "16msec";
    char       *cshift  = "10msec";
    double      mean    = 0.0;
    int         dolog   = 0;
    int         doabs   = 1;
    SVector    *vec;
    FMatrix    *avMag;
    int         winPoints, shiftPoints;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<feature>", ARGV_CUSTOM, createFe, &destin, cd,
		       		NEW_FEAT,
		       "<source_feature>", ARGV_CUSTOM, getFe, &source, cd,
		       		SRC_FEAT,
		       "<win>", ARGV_STRING, NULL, &cwin, NULL,
		       		"window size",
		       "-shift", ARGV_STRING, NULL, &cshift, NULL,
		       		"shift",
		       "-mean", ARGV_DOUBLE, NULL, &mean, NULL,
		       		"mean of source feature",
		       "-log", ARGV_INT, NULL, &dolog, NULL,
		       		"compute log magnitude",
		       "-abs", ARGV_INT, NULL, &doabs, NULL,
		       		"compute absolute value (useful before taking log)",
		       NULL) != TCL_OK )
	return TCL_ERROR;

    if ( !IS_SVECTOR (source) ) {
	MUST_SVECTOR (source);
	return TCL_ERROR;
    }

    if ( source->samplingRate <= 0 ) {
	ERROR("Sampling rate of source feature %s must be > 0.\n", source->name);
	return TCL_ERROR;
    }
    else if ( getSampleX (fs, source, cwin,   &winPoints) || 
	      getSampleX (fs, source, cshift, &shiftPoints) ) {
	return TCL_ERROR;
    }

    vec = source->data.svec;

    if ( fesAvMagnitude (vec, &avMag, winPoints, shiftPoints, mean, dolog, doabs) ) {
	if (avMag) fmatrixFree (avMag);
	return TCL_ERROR;
    }

    if ( feFMatrix (fs, destin, avMag, 0) ) {
	if (avMag) fmatrixFree (avMag);
	return TCL_ERROR;
    }

    destin->samplingRate = source->samplingRate;
    destin->shift        = shiftPoints / source->samplingRate;

    return TCL_OK;
}


/* ------------------------------------------------------------------------
   fesSNR
   ------------------------------------------------------------------------ */

/* returns 0 if succesful */
int fesSNR (FeatureSet *fs, Feature *source, Feature *silence, double mean,
	      int silSub, double *snr) {

    SVector *vec      = source->data.svec;
    FMatrix *mat      = silence->data.fmat;
    int      noiseN   = 0;
    float    var      = 0.0;
    float    noiseVar = 0.0;
    float    max      = 0.0;
    int      shiftPoints, i, frameX;

    if (silence->shift == 0.0)
      silence->shift = fs->sfshift;

    if ( vec->n <= 1 || mat->n != 1 || mat->m < 1 ) {
      ERROR ("fesSNR: unresonable dimensions\n");
      return TCL_ERROR;
    }

    shiftPoints = silence->samplingRate * silence->shift;

    /* look for maximum values in silence feature (for use with silTK) */
    for ( i=0; i<mat->m; i++ ) {
	if ( mat->matPA[i][0] > max ) max = mat->matPA[i][0];
    }

    /* compute variance */
    for ( i=0; i<vec->n; i++ ) {
	var    += (vec->vecA[i] - mean) * (vec->vecA[i] - mean);
	frameX  = i / shiftPoints;

	/* take ever the last samples (greater then frameN) */
	if ( frameX >= mat->m || mat->matPA[frameX][0] == max ) {
	    noiseN++;
	    noiseVar += (vec->vecA[i] - mean) * (vec->vecA[i] - mean);
	}
    }

    if ( noiseN <= 1 ) {
	ERROR("fesSNR: too few silence samples.\n");
	return TCL_ERROR;
    }

    var      /= vec->n - 1;
    noiseVar /= noiseN - 1;

    if ( noiseVar == 0 ) {
	noiseVar = 1;
    }

    /* compute snr */
    if ( silSub )
	if ( noiseVar > var )
	    *snr = -10 * log10 ((noiseVar - var) / noiseVar);
	else if ( noiseVar == var )
	    *snr = 0.0;
	else
	    *snr = 10 * log10 ((var - noiseVar) / noiseVar);
    else
	*snr = 10 * log10 (var / noiseVar);
    
    return TCL_OK;
}


int fesSNRItf (ClientData cd, int argc, char *argv[]) {

    FeatureSet *fs         = (FeatureSet*)cd;
    Feature    *source     = NULL;
    Feature    *silence    = NULL;
    double      mean       = 0.0;
    double      snr;
    int         silSub     = 0;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<source_feature>", ARGV_CUSTOM, getFe, &source, cd,
		      		SRC_FEAT,
		       "<silence_feature>", ARGV_CUSTOM, getFe, &silence, cd,
		       		"silence feature (1/0)",
		       "-silSub", ARGV_INT, NULL, &silSub, NULL,
		       		"subtract the silence from speech Energy",
		       "-mean", ARGV_DOUBLE, NULL, &mean, NULL,
		       		"mean of source feature",
		       NULL) != TCL_OK )
	return TCL_ERROR;

    /* check source feature */
    if ( !IS_SVECTOR (source) ) {
	MUST_SVECTOR (source);
	return TCL_ERROR;
    }
    if ( !IS_FMATRIX (silence) ) {
	MUST_FMATRIX (silence);
	return TCL_ERROR;
    }
    else if ( source->samplingRate <= 0 ) {
	ERROR("Sampling rate of source feature %s must be > 0.\n", argv[2]);
	return TCL_ERROR;
    }

    if ( fesSNR (fs, source, silence, mean, silSub, &snr) != TCL_OK ) {
	return TCL_ERROR;
    }

    itfAppendResult ("%f", snr);

    return TCL_OK;
}


int fesSNRKItf (ClientData cd, int argc, char *argv[]) {

    FeatureSet *fs          = (FeatureSet*)cd;
    Feature    *source      = NULL;
    char       *cwin    = "16msec";
    char       *cshift  = "10msec";
    double      mean        = 0.0;
    float       silMean     = 0.0;
    float       speechMean  = 0.0;
    double      snr;
    int         kmeansIterN = 10;
    FMatrix    *avMag;
    float      *kmeansA;
    int         winPoints, shiftPoints;
    int         frameA, frameN, frameX;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<source_feature>", ARGV_CUSTOM, getFe, &source, cd,
		      		SRC_FEAT,
		       "<win>", ARGV_STRING, NULL, &cwin, NULL,
		       		"window size",
		       "-shift", ARGV_STRING, NULL, &cshift, NULL,
		       		"shift",
		       "-mean", ARGV_DOUBLE, NULL, &mean, NULL,
		       		"mean of source feature",
		       "-kmeansIterN", ARGV_INT, NULL, &kmeansIterN, NULL,
		       		"number of iterations of kmeans",
		       NULL) != TCL_OK )
	return TCL_ERROR;

    /* check source feature */
    if ( !IS_SVECTOR (source) ) {
	MUST_SVECTOR (source);
	return TCL_ERROR;
    }

    if ( source->samplingRate <= 0 ) {
	ERROR("Sampling rate of source feature %s must be > 0.\n", argv[2]);
	return TCL_ERROR;

    } else if ( getSampleX (fs, source, cwin,   &winPoints) || 
		getSampleX (fs, source, cshift, &shiftPoints) ) {
	return TCL_ERROR;
    }

    if ( fesAvMagnitude (source->data.svec, &avMag, winPoints, shiftPoints,
			 mean, 1, 0) ) {
	if (avMag) fmatrixFree (avMag);
	return TCL_ERROR;
    }

    /* copy array, get min/max */
    frameA  = 0;
    frameN  = avMag->m;
    silMean = speechMean = avMag->matPA[0][0];
    kmeansA = (float*)malloc (frameN * sizeof(float));

    foreach_frame {
	kmeansA[frameX] = avMag->matPA[frameX][0];
	if ( kmeansA[frameX] > speechMean ) speechMean = kmeansA[frameX];
	if ( kmeansA[frameX] < silMean )    silMean    = kmeansA[frameX];
    }

    compute2means (kmeansA, frameN, &silMean, &speechMean, kmeansIterN);

    snr = (speechMean - silMean) * log10(2.0) * 20;

    itfAppendResult ("%f", snr);

    if (avMag)   fmatrixFree (avMag);
    if (kmeansA) free (kmeansA);

    return TCL_OK;
}


/* -------------------------------------------------------------------------
   frame based energy
   ------------------------------------------------------------------------- */

int fesPower (SVector *vec, FMatrix **powP, int winPoints, int shiftPoints,
	      double mean) {

    FMatrix *pow;
    int      frameA, frameX, frameN, i;

    frameN = (int)((vec->n - (winPoints - shiftPoints)) / shiftPoints);
    pow    = fmatrixCreate (frameN, 1);

    frameA = 0;
    foreach_frame {
        int    offset    = shiftPoints * frameX;
        float  power     = 0.0;
	double value;

        for ( i=0; i<winPoints; i++ ) {
	    value      = (vec->vecA[i+offset] - mean) * (vec->vecA[i+offset] - mean);
	    power     += (float)value;
	}

	pow->matPA[frameX][0]  = power / (float)winPoints;
    }

    *powP = pow;

    return 0;
}

int fesPowerItf (ClientData cd, int argc, char *argv[]) {

    FeatureSet *fs      = (FeatureSet*)cd;
    Feature    *destin  = NULL;
    Feature    *source  = NULL;
    char       *cwin    = "16msec";
    char       *cshift  = "10msec";
    double      mean    = 0.0;
    SVector    *vec;
    FMatrix    *pow;
    int         winPoints, shiftPoints;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<feature>", ARGV_CUSTOM, createFe, &destin, cd,
		       		NEW_FEAT,
		       "<source_feature>", ARGV_CUSTOM, getFe, &source, cd,
		       		SRC_FEAT,
		       "<win>", ARGV_STRING, NULL, &cwin, NULL,
		       		"window size",
		       "-shift", ARGV_STRING, NULL, &cshift, NULL,
		       		"shift",
		       "-mean", ARGV_DOUBLE, NULL, &mean, NULL,
		       		"mean of source feature",
		       NULL) != TCL_OK )
	return TCL_ERROR;

    if ( !IS_SVECTOR (source) ) {
	MUST_SVECTOR (source);
	return TCL_ERROR;
    }

    if ( source->samplingRate <= 0 ) {
	ERROR("Sampling rate of source feature %s must be > 0.\n", source->name);
	return TCL_ERROR;
    }
    else if ( getSampleX (fs, source, cwin,   &winPoints) || 
	      getSampleX (fs, source, cshift, &shiftPoints) ) {
	return TCL_ERROR;
    }

    vec = source->data.svec;

    if ( fesPower (vec, &pow, winPoints, shiftPoints, mean) ) {
	if (pow) fmatrixFree (pow);
	return TCL_ERROR;
    }

    if ( feFMatrix (fs, destin, pow, 0) ) {
	if (pow) fmatrixFree (pow);
	return TCL_ERROR;
    }

    destin->samplingRate = source->samplingRate;
    destin->shift        = shiftPoints / source->samplingRate;

    return TCL_OK;
}


/* -------------------------------------------------------------------------
   frame based zero crossings
   ------------------------------------------------------------------------- */

int fesZeroX (SVector *vec, FMatrix **zeroXP, int winPoints, int shiftPoints,
	      double mean, int dolog) {

    FMatrix *zero;
    int      frameA, frameX, frameN, i;

    frameN = (int)((vec->n - (winPoints - shiftPoints)) / shiftPoints);
    zero   = fmatrixCreate (frameN, 1);

    frameA = 0;
    foreach_frame {
        int    offset   = shiftPoints * frameX;
        int    zeroX    = 0;
	double previous = (double)vec->vecA[offset];
	double current;
	double value;

        for ( i=0; i<winPoints; i++ ) {
	    value = (double)(vec->vecA[i+offset]) - mean;

            if ( (current = value) != 0 ) {
                if ( (previous * current) < 0 ) zeroX++;
                previous = current;
            }
	}

	zero->matPA[frameX][0] = (float)zeroX;
	if ( dolog ) zero->matPA[frameX][0] = log (zero->matPA[frameX][0] + 1.0) /
			 log (2.0);
    }

    *zeroXP = zero;

    return 0;
}


int fesZeroXItf (ClientData cd, int argc, char *argv[]) {

    FeatureSet *fs      = (FeatureSet*)cd;
    Feature    *destin  = NULL;
    Feature    *source  = NULL;
    char       *cwin    = "16msec";
    char       *cshift  = "10msec";
    double      mean    = 0.0;
    int         dolog   = 0;
    SVector    *vec;
    FMatrix    *zero;
    int         winPoints, shiftPoints;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<feature>", ARGV_CUSTOM, createFe, &destin, cd,
		       		NEW_FEAT,
		       "<source_feature>", ARGV_CUSTOM, getFe, &source, cd,
		       		SRC_FEAT,
		       "<win>", ARGV_STRING, NULL, &cwin, NULL,
		       		"window size",
		       "-shift", ARGV_STRING, NULL, &cshift, NULL,
		       		"shift",
		       "-mean", ARGV_DOUBLE, NULL, &mean, NULL,
		       		"mean of source feature",
		       "-log", ARGV_INT, NULL, &dolog, NULL,
		       		"compute log magnitude",
		       NULL) != TCL_OK )
	return TCL_ERROR;

    if ( !IS_SVECTOR (source) ) {
	MUST_SVECTOR (source);
	return TCL_ERROR;
    }

    if ( source->samplingRate <= 0 ) {
	ERROR("Sampling rate of source feature %s must be > 0.\n", source->name);
	return TCL_ERROR;
    }
    else if ( getSampleX(fs,source,cwin,  &winPoints)  || 
	      getSampleX(fs,source,cshift,&shiftPoints)   ) {
	return TCL_ERROR;
    }

    vec     = source->data.svec;

    if ( fesZeroX (vec, &zero, winPoints, shiftPoints, mean, dolog) ) {
	if (zero) fmatrixFree (zero);
	return TCL_ERROR;
    }

    if ( feFMatrix (fs, destin, zero, 0) ) {
	if (zero) fmatrixFree (zero);
	return TCL_ERROR;
    }

    destin->samplingRate = source->samplingRate;
    destin->shift        = shiftPoints / source->samplingRate;

    return TCL_OK;
}


/* -------------------------------------------------------------------------
   reorder entries in feature
   ------------------------------------------------------------------------- */

int fesReorderItf (ClientData cd, int argc, char *argv[]) {

    FeatureSet *fs      = (FeatureSet*)cd;
    Feature    *destin  = NULL, *nextDestin = NULL;
    Feature    *source  = NULL, *nextSource = NULL;
    FMatrix    *inMat, *outMat, *in2Mat = NULL, *out2Mat = NULL;
    float      *fA = NULL, *f2A = NULL;
    int         frameA, frameX, frameN;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<feature>", ARGV_CUSTOM, createFe, &destin, cd,
		       		NEW_FEAT,
		       "<source_feature>", ARGV_CUSTOM, getFe, &source, cd,
		       		SRC_FEAT,
		       "-nextDestin", ARGV_CUSTOM, createFe, &nextDestin, cd,
		       		NEW_FEAT,
		       "-nextSource",  ARGV_CUSTOM, getFe, &nextSource, cd,
		       		SRC_FEAT,
		       NULL) != TCL_OK )
	return TCL_ERROR;

    if ( !IS_FMATRIX (source) ) {
	MUST_FMATRIX (source);
	return TCL_ERROR;
    }

    inMat  = source->data.fmat;
    frameN = inMat->m;
    outMat = fmatrixCreate (frameN, 1);
    fA     = (float*)malloc (frameN * sizeof(float));

    if ( nextSource ) {
	in2Mat  = nextSource->data.fmat;
	out2Mat = fmatrixCreate (frameN, 1);
	f2A     = (float*)malloc (frameN * sizeof(float));
    }

    frameA = 0;
    foreach_frame {
	fA[frameX] = inMat->matPA[frameX][0];
	if ( f2A ) f2A[frameX] = in2Mat->matPA[frameX][0];
    }

    if ( f2A ) {
	q2sort (fA, f2A, 0, frameN-1);
    } else {
	qsort ((void*)fA, frameN, sizeof(float), floatcompare);
    }

    frameA = 0;
    foreach_frame {
	outMat->matPA[frameX][0] = fA[frameX];
	if ( f2A ) out2Mat->matPA[frameX][0] = f2A[frameX];
    }

    if (fA)  free (fA);
    if (f2A) free (f2A);

    if ( feFMatrix( fs, destin, outMat, 1) ) {
	if (outMat) fmatrixFree (outMat);
	return TCL_ERROR;
    }

    destin->samplingRate = source->samplingRate;
    destin->shift        = source->shift;

    if ( nextSource ) {
	if ( feFMatrix( fs, nextDestin, out2Mat, 1) ) {
	    if (out2Mat) fmatrixFree (out2Mat);
	    return TCL_ERROR;
	}

	nextDestin->samplingRate = nextSource->samplingRate;
	nextDestin->shift        = nextSource->shift;
    }

    return TCL_OK;
}


/* -------------------------------------------------------------------------
   compute gradients in feature between a given window length
   ------------------------------------------------------------------------- */

int fesGradientItf (ClientData cd, int argc, char *argv[]) {

    FeatureSet *fs      = (FeatureSet*)cd;
    Feature    *destin  = NULL;
    Feature    *source  = NULL;
    FMatrix    *inMat, *outMat;
    int         frameA, frameX, frameN;
    int         winLength = 5;
    float       dy, dx;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<feature>", ARGV_CUSTOM, createFe, &destin, cd,
		       		NEW_FEAT,
		       "<source_feature>", ARGV_CUSTOM, getFe, &source, cd,
		       		SRC_FEAT,
		       "-win", ARGV_INT, NULL, &winLength, NULL,
		       		"number of Frames in window",
		       NULL) != TCL_OK )
	return TCL_ERROR;

    if ( !IS_FMATRIX (source) ) {
	MUST_FMATRIX (source);
	return TCL_ERROR;
    }

    inMat  = source->data.fmat;
    frameN = inMat->m;
    outMat = fmatrixCreate (frameN/winLength, 1);

    frameA = winLength;
    dx     = (float)winLength;
    for (frameX = frameA; frameX < frameN; frameX += winLength) {
	dy = inMat->matPA[frameX][0] - inMat->matPA[frameX-winLength][0];
	outMat->matPA[frameX/winLength-1][0] = dy / dx;
    }

    if ( feFMatrix( fs, destin, outMat, 1) ) {
	if (outMat) fmatrixFree (outMat);
	return TCL_ERROR;
    }

    destin->samplingRate = source->samplingRate;
    destin->shift        = source->shift;

    return TCL_OK;
}

/* -------------------------------------------------------------------------
   
   ------------------------------------------------------------------------- */

int fesSilenceMagThreshold (float *valueA, float mean, int valueN) {

    float lowerMean = valueA[0];
    float upperMean = valueA[valueN-1];

    int   frameX    = 0;
    int   iterN     = 10;

    compute2means (valueA, valueN, &lowerMean, &upperMean, iterN);

    while ( euklidDist (valueA[frameX], lowerMean) <
	    euklidDist (valueA[frameX], upperMean) ) frameX++;

    return frameX;
}


int fesSilenceZeroThreshold (float *valueA, float mean, int valueN) {

    return fesSilenceMagThreshold (valueA, mean, valueN);
}


int fesSilenceThresholds (float *mThres, float *zThres, Feature *magnitude,
			  Feature *zero, char *feType) {

    FMatrix *mMat   = magnitude->data.fmat;
    FMatrix *zMat   = zero->data.fmat;

    float   *sortMA = NULL;	/* holds sorted magnitudes */
    float   *sortZA = NULL;	/* holds sorted zero crossings */

    float    mMean, zMean;

    int      frameA, frameX, frameN;
    int      mIdx, zIdx;

    /* initialize */
    frameN = mMat->m;
    sortMA = (float*)malloc (frameN * sizeof(float));
    sortZA = (float*)malloc (frameN * sizeof(float));

    if ( !sortMA || !sortZA ) {
	ERROR ("allocation failure.\n");
	return TCL_ERROR;
    }


    /* sort magnitudes and zeroX after increasing magnitude values */
    frameA = 0;
    mMean  = 0;
    zMean  = 0;
    foreach_frame {
	mMean += sortMA[frameX] = mMat->matPA[frameX][0];
	zMean += sortZA[frameX] = zMat->matPA[frameX][0];
    }

    mMean /= frameN;
    zMean /= frameN;

    q2sort (sortMA, sortZA, 0, frameN-1);

    /* compute magnitude thresholds */
     mIdx   = fesSilenceMagThreshold (sortMA, mMean, frameN);
    *mThres = sortMA[mIdx];

    /* sort zero crossings in given intervall [0,mIdx] */
    qsort ((void*)sortZA, mIdx+1, sizeof(float), floatcompare);

    /* compute zero crossing thresholds */
     zIdx   = fesSilenceZeroThreshold (sortZA, zMean, mIdx+1);
    *zThres = sortZA[zIdx];

    /*
    printf ("M %i %e, Z %i %e\n", mIdx, *mThres, zIdx, *zThres); fflush (stdout);
    */

    /* free all */
    free (sortMA); sortMA = NULL;
    free (sortZA); sortZA = NULL;

    return TCL_OK;
}


int fesSilenceClassification (FMatrix *silence, Feature *magnitude,
			      Feature *zero, float mThres, float zThres,
			      float magnitudeFactor, float zeroFactor,
			      int smoothPasses, int smoothLength,
			      int minSilLength, int minZeroLength) {

    FMatrix *mMat         = magnitude->data.fmat;
    FMatrix *zMat         = zero->data.fmat;
    int      zExceed      = 0;

    int      frameA, frameX, frameN;

    if ( !silence ) return TCL_ERROR;

    mThres *= magnitudeFactor;
    zThres *= zeroFactor;

    /* classify frames */
    frameN = mMat->m;
    frameA = 0;
    foreach_frame {
	/* zero crossings */
	if ( zExceed ) zExceed++;
	if ( zMat->matPA[frameX][0] < zThres ) zExceed = 1;

	/* magnitude */
	if ( mMat->matPA[frameX][0] < mThres ) {
	    if ( zExceed > minZeroLength ) {
		/* magnitude low but zero crossings long enough too high */
		silence->matPA[frameX][0] = 1.0;
	    } else {
		/* magnitude low and zero crossings low */
		silence->matPA[frameX][0] = 2.0;
	    }
	} else {
	    silence->matPA[frameX][0] = 0.0;
	}
    }

    /* smoothing */
    if ( smoothPasses ) {
	int   bool  =  0;
	int   begin = -1;
	int   i, j;
	int   smoothThres;
	
	/* set all 1.0 between surrounding 2.0 to 2.0 if number of frames
	   smaller than minSilLength else to 0.0 */
	frameA = 0;
	foreach_frame {
	    if ( silence->matPA[frameX][0] == 0.0 )
		bool  = 0;
	    if ( silence->matPA[frameX][0] == 1.0 &&  bool && begin < 0 )
		begin = frameX;
	    if ( silence->matPA[frameX][0] == 2.0 && !bool ) {
		bool  =  1;
		begin = -1;
	    }
	    if ( silence->matPA[frameX][0] == 2.0 &&  bool && begin >= 0 ) {
		int val = 1.0;
		if ( frameX-begin < minZeroLength ) val = 2.0;
		for ( i=begin; i<frameX; i++ ) {
		    silence->matPA[i][0] = val;
		}
		bool  =  0;
		begin = -1;
	    }
	}

	/* set all 1.0 to 0.0 and all 2.0 to 1.0 (for easier smoothing) */
	frameA = 0;
	foreach_frame {
	    if ( silence->matPA[frameX][0] == 1.0 ) silence->matPA[frameX][0] = 0.0;
	    if ( silence->matPA[frameX][0] == 2.0 ) silence->matPA[frameX][0] = 1.0;
	}
	
	/* median smoothing */
	if ( frameN < smoothLength ) {
	    smoothLength = frameN;
	    smoothPasses = 1;

	    if ( (smoothLength & 1) != 1 ) {
		smoothLength--;
		smoothPasses++;
	    }
	}

	smoothThres = (int)(floor((float)smoothLength / 2));
	frameA      = (int)(floor((float)smoothLength / 2));

	for ( i=0; i<smoothPasses; i++ ) {
	    for ( frameX=frameA; frameX<frameN-frameA; frameX++ ) {
		int sum = 0;
		
		for ( j=frameA; j>0; j-- )
		    sum += (int)(silence->matPA[frameX-j][0]) + 
			   (int)(silence->matPA[frameX+j][0]);
		
		if ( sum > smoothThres && silence->matPA[frameX][0] < 1.0 )
		    silence->matPA[frameX][0] = 1.0;
		if ( sum < smoothThres && silence->matPA[frameX][0] > 0.0 )
		    silence->matPA[frameX][0] = 0.0;
	    }
	}

	/* delete all silences with a length smaller than minSilLength
	   let this away if you want to make a wordboundary detection */
	begin  = -1;
	frameA =  0;
	foreach_frame {
	    if ( silence->matPA[frameX][0] == 1.0 && begin < 0 ) begin = frameX;
	    if ( silence->matPA[frameX][0] == 0.0 ) {
		if ( begin > 0 && frameX-begin < minSilLength ) {
		    for ( i=begin; i<frameX; i++ ) {
			silence->matPA[i][0] = 0.0;
		    }
		}
		begin = -1;
	    }
	}
    }

    return TCL_OK;
}


int fesSilenceDetectionItf (ClientData cd, int argc, char *argv[]) {

    FeatureSet *fs              = (FeatureSet*)cd;
    Feature    *destin          = NULL;
    Feature    *magnitude       = NULL;
    Feature    *zero            = NULL;
    FMatrix    *silence         = NULL;
    float       magnitudeFactor = 1.0;
    float       zeroFactor      = 1.0;
    int         smoothPasses    = 5;
    int         smoothLength    = 15;
    int         minSilLength    = 17;
    int         minZeroLength   = 3;
    char       *feType          = "log";
    float       magnitudeThres, zeroThres;

    argc--;
    if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		       "<feature>", ARGV_CUSTOM, createFe, &destin, cd,
		       		NEW_FEAT,
		       "<magnitude>", ARGV_CUSTOM, getFe, &magnitude, cd,
		       		"log magnitude (magnitude or power see -feType)",
		       "<zeroX>", ARGV_CUSTOM, getFe, &zero, cd,
		       		"zero crossing feature",
		       "-magnitudeFactor", ARGV_FLOAT, NULL, &magnitudeFactor, NULL,
		       		"factor to multiply the magnitude threshold",
		       "-zeroFactor", ARGV_FLOAT, NULL, &zeroFactor, NULL,
		       		"factor to multiply the zeroX threshold",
		       "-smoothPasses", ARGV_INT, NULL, &smoothPasses, NULL,
		       		"passes to smooth",
		       "-smoothLength", ARGV_INT, NULL, &smoothLength, NULL,
		       		"smooth length (odd number)",
		       "-minSilLength", ARGV_INT, NULL, &minSilLength, NULL,
		       		"minimum number of frames for a silence",
		       "-minZeroLength", ARGV_INT, NULL, &minZeroLength, NULL,
		       		"minimum number of frames for which the zeroX must exceed the threshold",
		       /* "-feType", ARGV_STRING, NULL, &feType, NULL,
			  "type of feature (log, magnitude, power)", */
		       NULL) != TCL_OK )
	return TCL_ERROR;

    if ( !IS_FMATRIX (magnitude) || !IS_FMATRIX (zero) ) {
	ERROR ("Both, other and zeroX must be FMATRIX features.\n");
	return TCL_ERROR;
    }

    if ( (smoothLength & 1) != 1 ) {
	ERROR ("smothLength must be an odd number.\n");
	return TCL_ERROR;
    }

    if ( fesSilenceThresholds (&magnitudeThres, &zeroThres, magnitude, zero,
			       feType) != TCL_OK ) {
	ERROR ("computing thresholds failed.\n");
	return TCL_ERROR;
    }

    silence = fmatrixCreate (magnitude->data.fmat->m, 1);

    if ( fesSilenceClassification (silence, magnitude, zero, magnitudeThres,
				   zeroThres, magnitudeFactor, zeroFactor,
				   smoothPasses, smoothLength,
				   minSilLength, minZeroLength)
	 != TCL_OK ) {
	ERROR ("classification failed.\n");
	if (silence) fmatrixFree (silence);
	return TCL_ERROR;
    }

    if ( feFMatrix (fs, destin, silence, 0) ) {
	ERROR ("can't link matrix to feature.\n");
	if (silence) fmatrixFree (silence);
	return TCL_ERROR;
    }

    destin->samplingRate = magnitude->samplingRate;
    destin->shift        = magnitude->shift;

    return TCL_OK;
}
