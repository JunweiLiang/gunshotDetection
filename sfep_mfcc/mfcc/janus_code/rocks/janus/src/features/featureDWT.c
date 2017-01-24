/* ========================================================================
    JANUS      Speech- to Speech Translation
	       ------------------------------------------------------------
	       Object: Discrete Wavelet Transform
	       ------------------------------------------------------------

    Author  :  Michael Wand
    Module  :  featureDWT.c
    Date    :  $Id: featureDWT.c 2767 2007-02-23 08:55:36Z stueker $
    Remarks :

   ========================================================================

    This software was developed by

    the  University of Karlsruhe       and  Carnegie Mellon University
	 Dept. of Informatics	            Dept. of Computer Science
	 ILKD, Lehrstuhl Prof.Waibel	    Alex Waibel's NN & Speech Group
	 Am Fasanengarten 5		    5000 Forbes Ave
	 D-76131 Karlsruhe		    Pittsburgh, PA 15213
	 - Germany -		            - USA -

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
   Revision 4.2  2007/02/23 08:55:36  stueker
   Changed by Michael Wand to compile w/o warnings

   Revision 4.1  2007/02/08 09:51:07  stueker
   Initial Check in of Discrete Wavelet Transformation from Michael Wand


   ======================================================================== */


#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include "limits.h"
#include "featureMethodItf.h"  /* includes matrix.h( itf.h(tcl.h), common.h) */
#include "featureFilter.h"
#include "featureDWT.h"

#define FATALERROR(s) { ERROR(s); exit(1); }

/* malloc is awful! Where have the exceptions gone? */
#define MALLOC(var,count,type) \
	(var) = (type*) malloc((count) * sizeof(type)); \
	if ((var) == NULL) FATALERROR("featureDWT.c: out of memory");

/* Access function for the DTCWT (double-tree complex wavelet transform after Kingsbury)
 *
 * INPUT: (passed in argv)
 *      FeatureSet*     fs	      	the feature set which is to be used
 *      Feature*	dest	    	the destination feature
 *      Feature*	source	  	the source feature
 *      Filter*	 	filter0a	\ the lowpass filters for the first stage of the
 *      Filter*	 	filter0b	/ transform in trees a,b
 *      Filter*	 	filter1a	\ the lowpass filters for all subsequent stages of the
 *      Filter*	 	filter1b	/ transform, trees a,b
 *      int	     	level	   	decomposition level
 *      int	     	useLowpass	gives the number of lowpass coefficients to use (must be in { 0, .., level })
 *
 * RETURN: TCL_OK or TCL_ERROR
 */
int fesDTCWTItf(ClientData cd,int argc,char *argv[])
{
	FeatureSet* 	fs = (FeatureSet*)cd;
	Feature*    	source = NULL;
	Feature*	dest = NULL;
	Filter*		filter0a = NULL;
			char* filter0aHelp = "\\ the lowpass filters for the first stage of the ";
	Filter*		filter0b = NULL;
			char* filter0bHelp = "/ transform in trees a,b ";
	Filter*		filter1a = NULL;
			char* filter1aHelp = "\\ the lowpass filters for all subsequent stages of the ";
	Filter*		filter1b = NULL;
			char* filter1bHelp = "/ transform, trees a,b";
	int		decompositionLevel = 5;
			char* decompositionLevelHelp = "decomposition level";
	int 		useLowpass = 0;
			char* useLowpassHelp = "number of low-pass coefficients to use (0 .. <level>)";
			
	int success;

	/* get arguments */
	argc--;
	if (itfParseArgv( argv[0], &argc, argv+1, 0,
		"<feature>",	ARGV_CUSTOM, createFe,  &dest, cd, NEW_FEAT,
		"<source_feature>", ARGV_CUSTOM, getFe,     &source,   cd, SRC_FEAT,
		"<filter0a>",	 ARGV_CUSTOM, getFilter, &filter0a,  cd, filter0aHelp,
		"<filter0b>",	 ARGV_CUSTOM, getFilter, &filter0b,  cd, filter0bHelp,
		"<filter1a>",	 ARGV_CUSTOM, getFilter, &filter1a,  cd, filter1aHelp,
		"<filter1b>",	 ARGV_CUSTOM, getFilter, &filter1b,  cd, filter1bHelp,
		"<level>",	  ARGV_INT,    NULL,      &decompositionLevel, cd, decompositionLevelHelp,
		"-useLowpass",      ARGV_INT,    NULL,      &useLowpass,cd,useLowpassHelp,
		NULL) != TCL_OK) return TCL_ERROR;

	
	/* Make sure everything is initialized */
	initDWT();
	
	if (decompositionLevel < 1 || decompositionLevel > 24) {
		ERROR("Decomposition level must be between 1 and 24\n");
		return TCL_ERROR;
	}

	if (useLowpass < 0 || useLowpass > decompositionLevel) {
		ERROR("useLowpass must be between 0 and the decomposition level\n");
		return TCL_ERROR;
	}
	
	if (!IS_SVECTOR(source)) {
		MUST_SVECTOR(source);
		return TCL_ERROR;
	}

	success = doDTCWT(fs,dest,source,filter0a,filter0b,filter1a,filter1b,decompositionLevel,useLowpass);
	
	return (success ? TCL_OK : TCL_ERROR);
}	


/* Access function for the FWT (real decimated form after Mallat)
 *
 * INPUT: (passed in argv)
 *      FeatureSet*     fs	      the feature set which is to be used
 *      Feature*	dest	    the destination feature
 *      Feature*	source	  the source feature
 *      Filter*	 filter	  the filter which is to be used (this should be the low-pass filter, i.e.
 *				      the scaling function's coefficients)
 *      int	     level	   decomposition level
 *
 * RETURN: TCL_OK or TCL_ERROR
 */
int fesFWTItf(ClientData cd,int argc,char *argv[])
{
	FeatureSet* 	fs = (FeatureSet*)cd;
	Feature*    	source = NULL;
	Feature*	dest = NULL;
	Filter*		filter = NULL;
			char* filterHelp = "the filter (i.e. the scaling function's coefficients) tho be used";
	int		decompositionLevel = 5;
			char* decompositionLevelHelp = "decomposition level";
	int 		useLowpass = 0;
			char* useLowpassHelp = "number of low-pass coefficients to use (0 .. <level>)";
			
	int success;
	
	/* get arguments */
	argc--;
	if (itfParseArgv( argv[0], &argc, argv+1, 0,
		"<feature>",	ARGV_CUSTOM, createFe,  &dest, cd, NEW_FEAT,
		"<source_feature>", ARGV_CUSTOM, getFe,     &source,   cd, SRC_FEAT,
		"<filter>",	 ARGV_CUSTOM, getFilter, &filter,  cd, filterHelp,
		"<level>",	  ARGV_INT,    NULL,      &decompositionLevel, cd, decompositionLevelHelp,
		"-useLowpass",      ARGV_INT,    NULL,      &useLowpass,cd,useLowpassHelp,
		NULL) != TCL_OK) return TCL_ERROR;

	
	/* Make sure everything is initialized */
	initDWT();
	
	if (decompositionLevel < 1 || decompositionLevel > 24) {
		ERROR("Decomposition level must be between 1 and 24\n");
		return TCL_ERROR;
	}

	if (useLowpass < 0 || useLowpass > decompositionLevel) {
		ERROR("useLowpass must be between 0 and the decomposition level\n");
		return TCL_ERROR;
	}
	
	if (!IS_SVECTOR(source)) {
		MUST_SVECTOR(source);
		return TCL_ERROR;
	}

	success = doFWT(fs,dest,source,filter,decompositionLevel,useLowpass);
	
	return (success ? TCL_OK : TCL_ERROR);
}	

/* Access function for the RDWT (redundant wavelet transform, like FWT but without decimation)
 *
 * INPUT: (passed in argv)
 *	FeatureSet* 	fs		the feature set which is to be used
 *	Feature*	dest  		the destination feature
 *	Feature*	source		the source feature
 * 	Filter*		filter  	the filter which is to be used (this should be the low-pass filter, i.e.
 *					the scaling function's coefficients)
 *	int		level		decomposition level 
 *	int		useLowpass	gives the number of lowpass coefficients to use (must be in { 0, .., level })
 *
 * RETURN: TCL_OK or TCL_ERROR
 */
int fesRDWTItf(ClientData cd,int argc,char *argv[])
{
	FeatureSet* 	fs = (FeatureSet*)cd;
	Feature*    	source = NULL;
	Feature*	dest = NULL;
	Filter*		filter = NULL;
			char* filterHelp = "the filter (i.e. the scaling function's coefficients) tho be used";
	int		decompositionLevel = 5;
			char* decompositionLevelHelp = "decomposition level";
	int 		useLowpass = 0;
			char* useLowpassHelp = "number of low-pass coefficients to use (0 .. <level>)";
			
	int success;
			
	/* get arguments */
	argc--;
	if (itfParseArgv( argv[0], &argc, argv+1, 0,
		"<feature>",	ARGV_CUSTOM, createFe,  &dest, cd, NEW_FEAT,
		"<source_feature>", ARGV_CUSTOM, getFe,     &source,   cd, SRC_FEAT,
		"<filter>",	 ARGV_CUSTOM, getFilter, &filter,  cd, filterHelp,
		"<level>",	  ARGV_INT,    NULL,      &decompositionLevel, cd, decompositionLevelHelp,
		"-useLowpass",      ARGV_INT,    NULL,      &useLowpass,cd,useLowpassHelp,
		NULL) != TCL_OK) return TCL_ERROR;

	
	/* Make sure everything is initialized */
	initDWT();
	
	if (decompositionLevel < 1 || decompositionLevel > 24) {
		ERROR("Decomposition level must be between 1 and 24\n");
		return TCL_ERROR;
	}

	if (useLowpass < 0 || useLowpass > decompositionLevel) {
		ERROR("useLowpass must be between 0 and the decomposition level\n");
		return TCL_ERROR;
	}
	
	if (!IS_SVECTOR(source)) {
		MUST_SVECTOR(source);
		return TCL_ERROR;
	}

	success = doRDWT(fs,dest,source,filter,decompositionLevel,useLowpass);
	
	return (success ? TCL_OK : TCL_ERROR);
}	
	
	      

/* Does internal initialization for the discrete wavelet transform. */
void initDWT()
{
#if 0
	static int initialized = 0;
	if (initialized) return;
	initialized = 1;
#endif
}

/* Used in the next function */
#define NORM(a,b) (sqrt((a) * (a) + (b) * (b)))
/* Performs the double-tree complex wavelet transformation on the given data.
 *
 * INPUT:
 *      FeatureSet*     fs	  	the basic feature set
 *      Feature*	dest	    	the destination feature
 *      Feature*	source	 	the source feature
 *      Filter*		filter0a	\ the lowpass filters for the first stage of the
 *      Filter*	 	filter0b	/ transform in trees a,b
 *      Filter*	 	filter1a	\ the lowpass filters for all subsequent stages of the
 *      Filter*	 	filter1b	/ transform, trees a,b
 *      int	     	level	   	decomposition level
 *      int	     	useLowpass	gives the number of lowpass coefficients to use (must be in { 0, .., level })
 *
 *
 * RETURN: 1 if successful, 0 if not
 *
 */
int doDTCWT(FeatureSet* fs,Feature* dest,Feature* source,Filter* filter0a,Filter* filter0b,
		Filter* filter1a,Filter* filter1b,int level,int useLowpass)
{
	FMatrix* res;
	
	real** det_a; real** low_a; /* a-tree */
	real** det_b; real** low_b; /* b-tree */
	tsize* arraySize; /* For both trees, lowpass and details alike */

	int i;
	int currentFrame;
	float* currentFrameP;
	int s;

	/* res has got ceil(source.svec.n / 2.0) frames */
	/* The matrix size is the same as in doFWT, since for two corresponding coefficients c, d
	 * (i.e. of the same scale and translation) in the different trees, the result
	 * is sqrt(c^2+d^2) = abs(c+id).
	 */
	res = fmatrixCreate((int) ceil(source->data.svec->n / 2.0),level + useLowpass);
	if (!res) {
		FATALERROR("memory allocation failed\n");
		return 0;
	}
	
	MALLOC(det_a,level,real*); 
	MALLOC(low_a,level,real*); 
	MALLOC(det_b,level,real*); 
	MALLOC(low_b,level,real*); 
	MALLOC(arraySize,level,tsize);

	for (i = 0;i < level;++i) {
		if (i == 0) {
			/* Init a-tree */
			arraySize[i] = doFWTStepSVec(source->data.svec->vecA,source->data.svec->n,
					det_a,low_a,filter0a);
			/* Init b-tree */
			/*arraySize[i] =*/ doFWTStepSVec(source->data.svec->vecA,source->data.svec->n,
					det_b,low_b,filter0b);
		} else {
			/* a-tree */
			arraySize[i] = doFWTStep(low_a[i - 1],arraySize[i-1],det_a + i,low_a + i,filter1a);
			/* b-tree */
			/*arraySize[i] =*/ doFWTStep(low_b[i - 1],arraySize[i-1],det_b + i,low_b + i,filter1b);
		}
	}

	/* Now reorder data to be passed in an FMatrix */

	for (currentFrame = 0;currentFrame < res->m;++currentFrame) {
		int currentFrameScaled;

		currentFrameP = res->matPA[currentFrame];
		currentFrameScaled = currentFrame;
		for (s = 0;s < level;++s) {
			/* Traverse scales - first the details, then the lowpass coefficients */
			currentFrameP[s] = NORM(det_a[s][currentFrameScaled],det_b[s][currentFrameScaled]);
			if (s < useLowpass) {
				currentFrameP[s + level] = 
					NORM(low_a[s][currentFrameScaled],low_b[s][currentFrameScaled]);
			}
			currentFrameScaled /= 2; 
			/* The resolution decreases by the factor 2 on each scale step */
		}
	}
	
	feFMatrix(fs,dest,res,0);
	dest->samplingRate = source->samplingRate;
	dest->shift = 1 / (source->samplingRate / 2);

	for (i = 0;i < level;++i) {
		free(det_a[i]);
		free(low_a[i]);
		free(det_b[i]);
		free(low_b[i]);
	}
	free(det_a);
	free(low_a);
	free(det_b);
	free(low_b);
	free(arraySize);

	return 1;
}

/* Performs the fast wavelet transformation on the given data.
 *
 * INPUT:
 *      FeatureSet*     fs	      	the basic feature set
 *      Feature*	dest	    	the destination feature
 *      Feature*	source	  	the source feature
 *      Filter*	 	filter	  	the filter which is to be used (as above)
 *      int	     	level	   	decomposition level (creates vectors of level + 1 coefficients)
 *      int	     	useLowpass      gives the number of lowpass coefficients to use (must be in { 0, .., level })
 *
 *
 * RETURN: 1 if successful, 0 if not
 *
 */
int doFWT(FeatureSet* fs,Feature* dest,Feature* source,Filter* filter,int level,int useLowpass)
{
	FMatrix* res;
	
	real** details;
	real** lowpass;
	tsize* arraySize;

	int i;

	int currentFrame;
	float* currentFrameP;
	int s;

	/* res has got ceil(source.svec.n / 2.0) frames */
	res = fmatrixCreate((int) ceil(source->data.svec->n / 2.0),level + useLowpass);
	if (!res) {
		FATALERROR("memory allocation failed\n");
		return 0;
	}
	
	MALLOC(details,level,real*); 
	MALLOC(lowpass,level,real*); 
	MALLOC(arraySize,level,tsize);

	for (i = 0;i < level;++i) {
		if (i == 0) {
			arraySize[i] = doFWTStepSVec(source->data.svec->vecA,source->data.svec->n,details,lowpass,filter);
		} else {
			arraySize[i] = doFWTStep(lowpass[i - 1],arraySize[i-1],details + i,lowpass + i,filter);
		}
	}

	/* Now reorder data to be passed in an FMatrix */
	for (currentFrame = 0;currentFrame < res->m;++currentFrame) {
		int currentFrameScaled;
		currentFrameP = res->matPA[currentFrame];
		currentFrameScaled = currentFrame;
		for (s = 0;s < level;++s) {
			/* Traverse scales - first the details, then the lowpass coefficients */
			currentFrameP[s] = details[s][currentFrameScaled];
			if (s < useLowpass) {
				currentFrameP[s + level] = lowpass[s][currentFrameScaled];
			}
			if (s < level - 1) {
				currentFrameScaled /= 2; 
			}
			/* The resolution decreases by the factor 2 on each scale step */
		}
	}
	
	feFMatrix(fs,dest,res,0);
	dest->samplingRate = source->samplingRate;
	dest->shift = 1 / (source->samplingRate / 2);

	for (i = 0;i < level;++i) {
		free(details[i]);
		free(lowpass[i]);
	}
	free(details);
	free(lowpass);
	free(arraySize);

	return 1;
}


/* Performs an FWT step.
 *
 * INPUT:
 *	real*	inData		array with input data for one step
 *	tsize	inDataSize	size (length) of inData
 *	real**	details		pointer to array of detail coefficients
 *	real**	lowpass		pointer to array of coefficients which contains the low-scale parts of inData
 *	Filter* filter  	the filter which must be used (as above)
 *
 * RETURN: the size of details and lowpass is returned. details and lowpass are newly malloc'ed.
 */

/* This equals (-1) to the power of k */
#define NEG_IF_ODD(k) ((k) % 2 ? -1 : 1)

/* Access functions to the filters */
#define G_AT(k) (NEG_IF_ODD(k) * H_AT(1 - (k)))
#define H_AT(k) (filter->a[(k) - filter->offset])

tsize doFWTStep(real* inData,tsize inDataSize,real** details,real** lowpass,Filter* filter)
{
	tsize outDataSize = (tsize) ceil(inDataSize / 2.0);
	tsize k;
	tsize l;

	MALLOC(*details,outDataSize,real);
	MALLOC(*lowpass,outDataSize,real);

	/* We calculate:
	 * details[k] = \sum_{l \in Z} (G_AT(l - 2k) * base[l]) and
	 * lowpass[k] = \sum_{l \in Z} (H_AT(l - 2k) * base[l]). Note that I use
	 * G_AT and H_AT to make the filters g and h behave like arrays
	 * which start at a position possibly different from 0.
	 */
	for (k = 0;k < outDataSize;++k) {
		(*details)[k] = 0;
		(*lowpass)[k] = 0;
		
		for (l = 1 - (filter->offset + filter->n - 1) + 2 * k;l < 1 - (filter->offset - 1) + 2 * k;++l) {
			if (l >= 0 && l < inDataSize) {
				(*details)[k] += G_AT(l - 2 * k) * inData[l];
			}
		}
		
		for (l = filter->offset + 2 * k;l < filter->offset + filter->n + 2 * k;++l) {
			if (l >= 0 && l < inDataSize) {
				(*lowpass)[k] += H_AT(l - 2 * k) * inData[l];
			}
		}
	}
	
	return outDataSize;
}

/* Performs an FWT step on an array of shorts instead of a column of FMatrix.
 *
 * INPUT:
 *      short*  inData	  array with input data for one step
 *      tsize   inDataSize      size (length) of inData
 *      real**  details	 pointer to array of detail coefficients
 *      real**  lowpass	 pointer to array of coefficients which contains the low-scale parts of inData
 *	Filter* filter  	the filter which must be used (as above)
 *
 * RETURN: the size of details and lowpass is returned. details and lowpass are newly malloc'ed.
 */
tsize doFWTStepSVec(short* inData,tsize inDataSize,real** details,real** lowpass,Filter* filter)
{
	tsize outDataSize = (tsize) ceil(inDataSize / 2.0);
	tsize k;
	tsize l;

	MALLOC(*details,outDataSize,real);
	MALLOC(*lowpass,outDataSize,real);

	/* We calculate:
	 * details[k] = \sum_{l \in Z} (G_AT(l - 2k) * base[l]) and
	 * lowpass[k] = \sum_{l \in Z} (H_AT(l - 2k) * base[l]). Note that I use
	 * G_AT and H_AT to make the filters g and h behave like arrays
	 * which start at a position possibly different from 0.
	 */
	for (k = 0;k < outDataSize;++k) {
		(*details)[k] = 0;
		(*lowpass)[k] = 0;
		
		for (l = 1 - (filter->offset + filter->n - 1) + 2 * k;l < 1 - (filter->offset - 1) + 2 * k;++l) {
			if (l >= 0 && l < inDataSize) {
				(*details)[k] += G_AT(l - 2 * k) * (real) inData[l];
			}
		}
		
		for (l = filter->offset + 2 * k;l < filter->offset + filter->n + 2 * k;++l) {
			if (l >= 0 && l < inDataSize) {
				(*lowpass)[k] += H_AT(l - 2 * k) * (real) inData[l];
			}
		}
	}

	return outDataSize;
}

/* Dilates Filter f by inserting z zeroes between the values (leaving position 0 unchanged).
 * INPUT:
 *      Filter* f	old filter (remains unchanged)
 *	int	z  	number of zeroes to insert between two coefficients of f
 * RETURN: a newly malloced, changed filter.
 */
Filter* dilateFilter(Filter* f,int z)
{
	int newOffset;
	int newn;
	int i;
	Filter* r;
	
	/* Only look at n and a */
	assert(f->n > 0);
	newOffset = f->offset * (z + 1);
	newn = f->n + (f->n - 1) * z;
	r = filterCreate(newn,0); 
	if (!r) FATALERROR("featureDWT.c: out of memory");

	r->offset = newOffset;
	z = z + 1; /* Necessary for algorithm! */

	/* Now do the algorithm: r->a[i] = f->a[i / z] for each multiple of z, 0 otherwise */
	for (i = 0; i < r->n;++i) {
		if (i % z) {
			r->a[i] = 0;
		} else {
			r->a[i] = f->a[i / z];
		}
	}

	return r;
}

/* Performs the redundant discrete wavelet transformation on the given data.
 *
 * INPUT:
 *      FeatureSet*     fs	      	the basic feature set
 *      Feature*	dest	    	the destination feature
 *      Feature*	source	  	the source feature
 *      Filter*	 	filter	  	the filter which is to be used (as above)
 *      int	     	level	   	decomposition level (creates vectors of level + 1 coefficients)
 *      int	     	useLowpass      gives the number of lowpass coefficients to use (must be in { 0, .., level })
 *
 *
 * RETURN: 1 if successful, 0 if not
 *
 */
int doRDWT(FeatureSet* fs,Feature* dest,Feature* source,Filter* filter,int level,int useLowpass)
{
	real** details;
	real** lowpass;
	
	/* The filter gets dilated in each step */
	Filter* curF = filter;

	int i;
	
	int currentFrame;
	float* currentFrameP;
	int s;

	/* res has got source.svec.n frames, like the original */
	/* each element of details also has got size source.svec.n */
	FMatrix* res = fmatrixCreate(source->data.svec->n,level + useLowpass);
	if (!res) {
		FATALERROR("memory allocation failed\n");
		return 0;
	}
	
	MALLOC(details,level,real*); 
	MALLOC(lowpass,level,real*); 

	for (i = 0;i < level;++i) {
		if (i == 0) {
			doRDWTStepSVec(source->data.svec->vecA,source->data.svec->n,details,lowpass,curF);
		} else {
			Filter* newF = dilateFilter(curF,1);
			filterFree(curF);
			curF = newF;
			doRDWTStep(lowpass[i - 1],source->data.svec->n,details + i,lowpass + i,curF);
		}
	}

	/* Now reorder data to be passed in an FMatrix */
	for (currentFrame = 0;currentFrame < res->m;++currentFrame) {
		int currentFrameScaled;
		currentFrameP = res->matPA[currentFrame];
		currentFrameScaled = currentFrame;
		for (s = 0;s < level;++s) {
			/* Traverse scales - first the details, then the lowpass coefficients */
			currentFrameP[s] = details[s][currentFrameScaled];
			if (s < useLowpass) {
				currentFrameP[s + level] = lowpass[s][currentFrameScaled];
			}
			if (s < level - 1) {
/*				currentFrameScaled /= 2; */
			}
			/* The resolution always remains the same */
		}
	}
	
	feFMatrix(fs,dest,res,0);
	dest->samplingRate = source->samplingRate;
	dest->shift = 1 / source->samplingRate;

	for (i = 0;i < level;++i) {
		free(details[i]);
		free(lowpass[i]);
	}
	free(details);
	free(lowpass);

	/* Possibly free the filter */
	if (curF != filter) filterFree(curF);
	
	return 1;
}


/* Performs an RDWT step.
 *
 * INPUT:
 *	real*	inData		array with input data for one step
 *	tsize	inDataSize	size (length) of inData
 *	real**	details		pointer to array of detail coefficients
 *	real**	lowpass		pointer to array of coefficients which contains the low-scale parts of inData
 *	Filter* filter  	the filter which must be used (as above)
 *
 * RETURN: the size of details and lowpass is returned. details and lowpass are newly malloc'ed.
 */
tsize doRDWTStep(real* inData,tsize inDataSize,real** details,real** lowpass,Filter* filter)
{
	tsize k;
	tsize l;
	MALLOC(*details,inDataSize,real);
	MALLOC(*lowpass,inDataSize,real);

	/* We calculate:
	 * details[k] = \sum_{l \in Z} (G_AT(l - k) * base[l]) and
	 * lowpass[k] = \sum_{l \in Z} (H_AT(l - k) * base[l]). Note that I use
	 * G_AT and H_AT to make the filters g and h behave like arrays
	 * which start at a position possibly different from 0.
	 */
	for (k = 0;k < inDataSize;++k) {
		(*details)[k] = 0;
		(*lowpass)[k] = 0;
		
		for (l = 1 - (filter->offset + filter->n - 1) + k;l < 1 - (filter->offset - 1) + k;++l) {
			if (l >= 0 && l < inDataSize) {
				(*details)[k] += G_AT(l - k) * inData[l];
			}
		}
		
		for (l = filter->offset + k;l < filter->offset + filter->n + k;++l) {
			if (l >= 0 && l < inDataSize) {
				(*lowpass)[k] += H_AT(l - k) * inData[l];
			}
		}
	}
	return inDataSize;
}

/* Performs an RDWT step on an array of shorts instead of a column of FMatrix.
 *
 * INPUT:
 *      short*  inData	  array with input data for one step
 *      tsize   inDataSize      size (length) of inData
 *      real**  details	 pointer to array of detail coefficients
 *      real**  lowpass	 pointer to array of coefficients which contains the low-scale parts of inData
 *	Filter* filter  	the filter which must be used (as above)
 *
 * RETURN: the size of details and lowpass is returned. details and lowpass are newly malloc'ed.
 */
tsize doRDWTStepSVec(short* inData,tsize inDataSize,real** details,real** lowpass,Filter* filter)
{
	tsize k;
	tsize l;
	MALLOC(*details,inDataSize,real);
	MALLOC(*lowpass,inDataSize,real);

	/* We calculate:
	 * details[k] = \sum_{l \in Z} (G_AT(l - k) * base[l]) and
	 * lowpass[k] = \sum_{l \in Z} (H_AT(l - k) * base[l]). Note that I use
	 * G_AT and H_AT to make the filters g and h behave like arrays
	 * which start at a position possibly different from 0.
	 */
	for (k = 0;k < inDataSize;++k) {
		(*details)[k] = 0;
		(*lowpass)[k] = 0;
		
		for (l = 1 - (filter->offset + filter->n - 1) + k;l < 1 - (filter->offset - 1) + k;++l) {
			if (l >= 0 && l < inDataSize) {
				(*details)[k] += G_AT(l - k) * (real) inData[l];
			}
		}
		
		for (l = filter->offset + k;l < filter->offset + filter->n + k;++l) {
			if (l >= 0 && l < inDataSize) {
				(*lowpass)[k] += H_AT(l - k) * (real) inData[l];
			}
		}
	}
	return inDataSize;
}


