/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Feature Module
               ------------------------------------------------------------

    Author  :  Martin Westphal
    Module  :  featurePLPItf.c
    Date    :  $Id: featurePLPItf.c 2390 2003-08-14 11:20:32Z fuegen $
    Remarks :

   ========================================================================

    Part of this software was developed by

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
    Revision 3.4  2003/08/14 11:19:56  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.3.56.1  2002/06/26 11:57:53  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.3  1997/07/30 12:52:42  kemp
    Changed the order of the #include files. malloc.h was included before string.h.
    As malloc.h over-defines strdup, the declaration in string.h was changed by the
    preprocessor resulting in compiler errors.

 * Revision 3.2  1997/07/09  17:42:22  westphal
 * *** empty log message ***
 *

   ======================================================================== */
/*
   Changes of the original code (MM):
   =================================== */
/* 3 functions fesXXXItf for XXX= PLP, Aud, Paud  */
/* MM: There was a really bad hack in this code because   rc[0] = -(double)r[1] / r[0];
   and r[0] could be zero sometimes, later on finite() was used to determine if the result
   can be used or not, but that only works on HPs !   */
/* see changes in audspec() */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "featureMethodItf.h"
#include "featurePLP.h"


#define WHATAMI "FE_PLP"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void init_plp(struct param *);
void init_plp_param(struct param *);
static struct param runparam;
static int first = 1;

/* Check numerical parameters to see if in a reasonable range, and the logical
	sense of combinations of flags. For the numerical comparisons,
	see the constant definitions in rasta.h . */
static void check_args( struct param *pptr )
{
	
	char *funcname;
        funcname = "check_args";

#ifndef IO_ESPS
	if(pptr->espsin == TRUE || pptr->espsout == TRUE)
	{
		fprintf(stderr,"Use non_ESPS_Makefile if no ESPS license");
		fprintf(stderr,"\n");
		exit(-1);
	}
#endif
	if((pptr->winsize < MIN_WINSIZE ) || (pptr->winsize > MAX_WINSIZE ))
	{
		fprintf(stderr,"Window size of %f msec not OK\n",
			pptr->winsize);
		exit(-1);
	}
	if((pptr->stepsize < MIN_STEPSIZE )||(pptr->stepsize > MAX_STEPSIZE ))
	{
		fprintf(stderr,"Step size of %f msec not OK\n",
			pptr->stepsize);
		exit(-1);
	}
	if((pptr->sampfreq < MIN_SAMPFREQ ) || (pptr->sampfreq > MAX_SAMPFREQ ))
	{
		fprintf(stderr,"Sampling frequency of %d not OK\n",
			pptr->sampfreq);
		exit(-1);
	}
	if((pptr->polepos < MIN_POLEPOS ) || (pptr->polepos >= MAX_POLEPOS ))
	{
		fprintf(stderr,"Pole position of %f not OK\n",
			pptr->polepos);
		exit(-1);
	}
	if((pptr->order < MIN_ORDER ) || (pptr->order > MAX_ORDER ))
	{
		fprintf(stderr,"LPC model order of %d not OK\n",
			pptr->order);
		exit(-1);
	}
	if((pptr->lift < MIN_LIFT ) || (pptr->lift > MAX_LIFT ))
	{
		fprintf(stderr,"Cepstral exponent of %f not OK\n",
			pptr->lift);
		exit(-1);
	}
	if((pptr->winco < MIN_WINCO ) || (pptr->winco > MAX_WINCO ))
	{
		fprintf(stderr,"Window coefficient of %f not OK\n",
			pptr->winco);
		exit(-1);
	}
	if((pptr->rfrac < MIN_RFRAC ) || (pptr->rfrac > MAX_RFRAC ))
	{
		fprintf(stderr,"Rasta fraction of %f not OK\n",
			pptr->rfrac);
		exit(-1);
	}
	if((pptr->jah < MIN_JAH ) || (pptr->jah > MAX_JAH ))
	{
		fprintf(stderr,"Jah value of %e not OK\n",
			pptr->jah);
		exit(-1);
	}
	if((pptr->lrasta ==FALSE) && (pptr->jrasta == FALSE))
	{
		if(pptr->prasta == TRUE)
		{
			fprintf(stderr,"Can't mix if no rasta flag\n");
			exit(-1);
		}
	}
	if(pptr->online == TRUE)
	{
		if(pptr->espsin==TRUE)
		{
			fprintf(stderr,"can't run on-line on esps input\n");
			exit(-1);
		}
		if(pptr->ascin==TRUE)
		{
			fprintf(stderr,"can't run on-line on ascii input\n");
			exit(-1);
		}
		if(strcmp (pptr->infname, "-") != 0)
		{
			fprintf(stderr,"on-line mode uses stdin only\n");
			exit(-1);
		}
	}
}

/* ============================================================ */

int     fesPLPItf(  ClientData cd, int argc, char *argv[])

/* ============================================================ */
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin, *source;
  
  if (first == 1){
     init_plp(&runparam); first = 0;
  }
  runparam.nout = 0;
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
		   "<feature>",        ARGV_CUSTOM, createFe,&destin,         cd, "name of new feature",
		   "<source_feature>", ARGV_CUSTOM, getFe,   &source,         cd, "name of source feature",
		   "-o",               ARGV_INT,    NULL,    &runparam.order, cd, "filter order",
		   "-n",               ARGV_INT,    NULL,    &runparam.nout,  cd, "number of output coefficients, 0 means order+1",
		   NULL) != TCL_OK) return TCL_ERROR;

  /* -------------------
     FLOAT features
     ------------------- */
  if (IS_FMATRIX( source)) {
     FMatrix      *mat1 = source->data.fmat;

     runparam.nfilts =  mat1->n;
     if (runparam.nout >  (runparam.order + 1)) {
	ERROR("\nnumber of output coefficients of feature %s: %d\nmust be <= (filter order + 1) = %d\n",
	      destin->name, runparam.nout, runparam.order + 1);
	return TCL_ERROR;
     }
     init_plp_param(&runparam);
     check_args(&runparam);
     {
	int frameX, frameN = mat1->m;
	int coeffX, coeffN = runparam.nout;
	struct fvec  source_frame, *destin_frame_p; /* no need to free later */
	FMatrix  *mat = fmatrixCreate(frameN,coeffN);  if (!mat) return TCL_ERROR;
	
	source_frame.length = mat1->n;
	
	for (frameX=0; frameX<frameN; frameX++) {
	   source_frame.values = mat1->matPA[frameX];                        /* copy pointer  */
	   destin_frame_p      = lpccep(&runparam, &source_frame);           /* call function */
	   for (coeffX=0; coeffX<coeffN; coeffX++)
	      mat->matPA[frameX][coeffX] =  destin_frame_p->values[coeffX];  /* copy result   */
	}
	if(!feFMatrix( FS, destin, mat, 1)) {
	   COPY_CONFIG( destin, source); return TCL_OK;
	}
	fmatrixFree(mat); return TCL_ERROR;
     }
  }
  ERROR("For command %s type of source feature %s must be FMatrix.\n", argv[0], argv[2]);
  return TCL_ERROR;
}


/* ============================================================ */

int     fesAudItf( ClientData cd, int argc, char *argv[])

/* ============================================================ */
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin, *source;
  
  if (first == 1){
     init_plp(&runparam); first = 0;
  }
  else runparam.nfilts = NOT_SET;
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
		   "<feature>",        ARGV_CUSTOM, createFe,&destin,         cd, "name of new feature",
		   "<source_feature>", ARGV_CUSTOM, getFe,   &source,         cd, "name of source feature",
		   "-nf",              ARGV_INT,    NULL,    &runparam.nfilts,cd, "number of filters",
		   NULL) != TCL_OK) return TCL_ERROR;

  if (IS_FMATRIX( source)) {
     FMatrix      *mat1 = source->data.fmat;
     /* ----- set sampling rate and window size ---------------------------------- */
     runparam.sampfreq = source->samplingRate * 1000;
     runparam.stepsize = (float)source->shift;
     runparam.winsize  = floor(2.0 * (float) mat1->n  / (float)source->samplingRate);
     
     init_plp_param(&runparam);
     check_args(&runparam);
     /* printf("FE_AUD: nfilts= %d, sampfreq= %dHz , stepsize= %.0fms, winsize= %.0fms\n",
	runparam.nfilts, runparam.sampfreq, runparam.stepsize, runparam.winsize); */
     {
	struct fvec  source_frame, *destin_frame_p;  /* no need to free later */
	int frameX, frameN = mat1->m;
	int         coeffN = runparam.nfilts;
	FMatrix       *mat = fmatrixCreate(frameN,coeffN); if (!mat) return TCL_ERROR;

	source_frame.length = mat1->n;
	for (frameX=0; frameX<frameN; frameX++) {
	   source_frame.values = mat1->matPA[frameX];                                  /* copy pointer  */
	   destin_frame_p      = audspec(&runparam, &source_frame);                    /* call function */
	   memcpy( mat->matPA[frameX], destin_frame_p->values, sizeof(float) * coeffN);   /* copy result   */
	}
	if(!feFMatrix( FS, destin, mat, 1)) {
	   COPY_CONFIG( destin, source); return TCL_OK;
	}
	fmatrixFree(mat); return TCL_ERROR;
     }
  }
  ERROR("For command %s type of source feature %s must be FMatrix.\n", argv[0], argv[2]);
  return TCL_ERROR;
}
/* ============================================================ */

int     fesPaudItf( ClientData cd, int argc, char *argv[])

/* ============================================================ */
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin, *source;
  
  if (first == 1){
     init_plp(&runparam); first = 0;
  }
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
		   "<feature>",        ARGV_CUSTOM, createFe,&destin,         cd, "name of new feature",
		   "<source_feature>", ARGV_CUSTOM, getFe,   &source,         cd, "name of source feature",
		   NULL) != TCL_OK) return TCL_ERROR;

  if (IS_FMATRIX( source)) {
     FMatrix      *mat1 = source->data.fmat;
     /* ----- set sampling rate and window size ---------------------------------- */
     runparam.sampfreq = source->samplingRate * 1000;
     runparam.stepsize = (float)source->shift;
     /* runparam.winsize  = floor(2.0 * (float) ????  / (float)source->samplingRate); */
     runparam.nfilts   = mat1->n;
     
     init_plp_param(&runparam);
     check_args(&runparam);
     /* printf("FE_AUD: nfilts= %d, sampfreq= %dHz , stepsize= %.0fms, winsize= %.0fms\n",
	runparam.nfilts, runparam.sampfreq, runparam.stepsize, runparam.winsize); */
     {
	struct fvec  source_frame, *destin_frame_p;  /* no need to free later */
	int frameX, frameN = mat1->m;
	int         coeffN = mat1->n;
	FMatrix       *mat = fmatrixCreate(frameN,coeffN);  if (!mat) return TCL_ERROR;
	
	source_frame.length = mat1->n;
	for (frameX=0; frameX<frameN; frameX++) {
	   source_frame.values = mat1->matPA[frameX];                        /* copy pointer  */
	   destin_frame_p      = post_audspec(&runparam, &source_frame);     /* call function */
	   memcpy( mat->matPA[frameX], destin_frame_p->values, sizeof(float) * coeffN);  /* copy result   */
	}
	if(!feFMatrix( FS, destin, mat, 1)) {
	   COPY_CONFIG( destin, source); return TCL_OK;
	}
	fmatrixFree(mat); return TCL_ERROR;
     }
  }
  ERROR("For command %s type of source feature %s must be FMatrix.\n", argv[0], argv[2]);
  return TCL_ERROR;
}



/* First, initialize run parameters to default values 
	(see rasta.h for definitions of upper-case constants) */
void init_plp(struct param *pptr) 
{
        pptr->winsize  = TYP_WIN_SIZE; 
	pptr->stepsize = TYP_STEP_SIZE; 
	pptr->sampfreq = PHONE_SAMP_FREQ;
	pptr->polepos = POLE_DEFAULT;
	pptr->order = TYP_MODEL_ORDER;
	pptr->lift = TYP_ENHANCE;
	pptr->winco = HAMMING_COEF;
	pptr->rfrac =  ONE;
	pptr->jah = JAH_DEFAULT;
	pptr->gainflag = TRUE;
	pptr->lrasta = FALSE;
	pptr->jrasta = FALSE;
	pptr->prasta = FALSE;
	pptr->infname = "-";  /* used for stdin */
	pptr->outfname = "-"; /* used for stdout */
	pptr->num_fname = NULL; /* file with RASTA polynomial numer\n") */
	pptr->denom_fname = NULL; /* file with RASTA polynomial denom\n") */
	pptr->ascin = FALSE;
	pptr->ascout = FALSE;
	pptr->debug = FALSE;
	pptr->smallmask = FALSE;
	pptr->espsin = FALSE;
	pptr->espsout = FALSE;
	pptr->nfilts = NOT_SET;
	pptr->nout = NOT_SET;
	pptr->online = FALSE; /* If set, do frame-by-frame analysis
				rather than reading in whole file first */
}

/* initialize parameters that can be computed from other parameter,
as opposed to being initialized explicitly by the user.
*/
void init_plp_param(struct param *pptr) 
{
	int overlap;
	float tmp;
	float step_barks;
	char *funcname;

        funcname = "init_param";

	pptr->winpts = (int)((double)pptr->sampfreq * (double)pptr->winsize
			/ 1000.);

	pptr->steppts = (int)((double)pptr->sampfreq * (double)pptr->stepsize
			/ 1000.);

	overlap = pptr->winpts - pptr->steppts;


	/* Here is some magical stuff to get the Nyquist frequency in barks */
	tmp = pptr->sampfreq / 1200.;
	pptr->nyqbar = 6. * log(((double)pptr->sampfreq /1200.) 
		+ sqrt(tmp * tmp + 1.));

/*     compute number of filters for at most 1 Bark spacing;
	This includes the constant 1 since we have a filter at d.c and
	a filter at the Nyquist (used only for dft purposes) */

	if(pptr->nfilts == NOT_SET)
	{
        	pptr->nfilts = ceil(pptr->nyqbar) + 1;
	}
	if((pptr->nfilts < 1) || (pptr->nfilts > MAXFILTS))
	{
		fprintf(stderr,"Nfilts value of %d not OK\n",
			pptr->nfilts);
		exit(-1);
	}

	/* compute filter step in Barks */
        step_barks = pptr->nyqbar / (float)(pptr->nfilts - 1);
	/* for a given step, must ignore the first and last few filters */
	pptr->first_good = (int)(1.0 / step_barks + 0.5);


	if(pptr->nout == NOT_SET)
	{
        	pptr->nout = pptr->order + 1;
	}
	if((pptr->nout < MIN_NFEATS) || (pptr->nout > MAX_NFEATS))
	{
		fprintf(stderr,"Feature vector length of %d not OK\n",
			pptr->nout);
		exit(-1);
	}
}



/*************************************************************************
 *                                                                       *
 *               ROUTINES IN THIS FILE:                                  *
 *                                                                       *
 *                      lpccep(): computes autoregressive cepstrum       *
 *                              from the auditory spectrum               *
 *                                                                       *
 *                      init_dft(): initializes cosine tables            *
 *                                                                       *
 *                      auto_to_lpc(): computes autoregressive coeffs    *
 *                              from the autocorrrelation                *
 *                                                                       *
 *                      auto_to_lpc(): computes autoregressive coeffs    *
 *                              from the autocorrrelation                *
 *                                                                       *
 *                      lpc_to_cep(): computes cepstral coeffs           *
 *                              from the autoregressive coeffs           *
 *                                                                       *
 ************************************************************************/
/*
 *	This routine computes the lpc cepstrum from the
 *	cleaned up auditory spectrum.
 *
 *	The first time that this program is called, we do
 *	the usual allocation.
 */

/* local function prototype */
struct fmat *init_idft( int, int );

struct fvec *lpccep( const struct param *pptr, struct fvec *in)
{
	char *funcname;

	static struct fvec *autoptr = NULL; /* array for autocor */
	static struct fvec *lpcptr; /* array for predictors */
	static struct fvec *outptr; /* array for cepstra */
	static struct fmat *wcosptr;
	
	float lpcgain;

	funcname = "lpccep";

	/* MM: call another time with different filter number or order: */
	if(autoptr) {
	   if ( pptr->order != (autoptr->length + 1)  ||  pptr->nout != outptr->length ) {
	      free_fvec(autoptr); autoptr = NULL;
	      free_fvec(lpcptr);
	      free_fvec(outptr);
	   }
	   else if (pptr->nfilts != wcosptr->nrows)
	      wcosptr = init_idft( pptr->nfilts, pptr->order + 1 );
	}
	
	if(autoptr == (struct fvec *)NULL) /* Check for 1st time */
			/* Allocate space, pre-compute cosines
				for IDFT */
	{
		autoptr = alloc_fvec( pptr->order + 1 );
		lpcptr = alloc_fvec( pptr->order + 1 );
		outptr = alloc_fvec( pptr->nout );
		wcosptr = init_idft( pptr->nfilts, pptr->order + 1 ); 
	}

	fmat_x_fvec( wcosptr, in, autoptr ); /* Do IDFT by
					multiplying cosine matrix times
					power values , getting
				autocorrelations */
					

	norm_fvec( autoptr, (float)(2. * (pptr->nfilts - 1)) );
	/* normalize for IDFT */

	auto_to_lpc( pptr, autoptr, lpcptr, &lpcgain );
	/* do Durbin recursion to get predictor coefficients */

	if( pptr->gainflag == TRUE)
	{
	    norm_fvec( lpcptr, lpcgain );
	}
	/* Put in model gain */
	
	lpc_to_cep( pptr, lpcptr, outptr );
	/* Recursion to get cepstral coefficients */

	return( outptr );
}


/* Builds up a matrix of cosines for IDFT */
struct fmat *init_idft( int n_freq, int n_auto )
{
	
	double base_angle;
	static struct fmat *wcosptr = NULL; /* MM: */
	int i, j;
	char *funcname;


	funcname = "init_dft";

	/* Allocate for an array of fvec structures */
	/* MM: */ if (wcosptr) free_fmat(wcosptr);
	wcosptr = alloc_fmat( n_auto, n_freq);
	
 	/* Note that M_PI is PI, defined in math.h */
	base_angle =  M_PI / (double)(n_freq - 1);

	for(i=0; i<n_auto; i++)
	{
		wcosptr->values[i][0] = 1.0;
		for(j=1; j<(n_freq-1); j++)
		{
			wcosptr->values[i][j]
			    = 2.0 * cos(base_angle * (double)i * (double)j);
		}
		/* No folding over from neg values for Nyquist freq */
		wcosptr->values[i][n_freq-1]
			= cos(base_angle * i * (n_freq-1));
	}

	return(wcosptr);
}


/* This routine computes the solution for an autoregressive
	model given the autocorrelation vector. 
	This routine uses essentially the
	same variables as were used in the original Fortran,
	as I don't want to mess with the magic therein. */
void auto_to_lpc( const struct param *pptr, struct fvec * autoptr, 
			struct fvec * lpcptr, float *lpcgain )
{
	int i;
	float s;
	static float *alp = NULL;
	static float *rc;
	float alpmin, rcmct, aib, aip;
	float *a, *r;
	char *funcname;

	int idx, mct, mct2, ib, ip, i_1, i_2, mh;

	funcname = "auto_to_lpc";

	if(alp == (float *)NULL) /* If first time */
	{
		alp = (float *)malloc((pptr->order + 1) * sizeof(float));
		if(alp == (float *)NULL)
		{
			fprintf(stderr,"cant allocate alp\n");
			exit(-1);
		}
		rc = (float *)malloc((pptr->order ) * sizeof(float));
		if(rc == (float *)NULL)
		{
			fprintf(stderr,"cant allocate rc\n");
			exit(-1);
		}
	}

	fvec_check( funcname, lpcptr, pptr->order );
	fvec_check( funcname, autoptr, pptr->order );

	/* Move values and pointers over from nice calling
		names to Fortran names */
	a = lpcptr->values;
	r = autoptr->values;

	/* MM: inserted the if */
	if (r[0] == 0) {
          for (i=0;i<lpcptr->length;i++) lpcptr->values[i]=0.0; 
	  *lpcgain = 1;
	}
	else {


	  /*     solution for autoregressive model */

	  a[0] = 1.;
	  alp[0] = r[0];
	  rc[0] = -(double)r[1] / r[0];
	  a[1] = rc[0];
	  alp[1] = r[0] + r[1] * rc[0];
	  i_2 = pptr->order;
	  for (mct = 2; mct <= i_2; ++mct) 
	  {
	        s = 0.;
	        mct2 = mct + 2;
		alpmin = alp[mct - 1];
        	i_1 = mct;
        	for (ip = 1; ip <= i_1; ++ip) 
		{
            		idx = mct2 - ip;
            		s += r[idx - 1] * a[ip-1];
        	}
        	rcmct = -(double)s / alpmin;
        	mh = mct / 2 + 1;
        	i_1 = mh;
        	for (ip = 2; ip <= i_1; ++ip) 
		{
            		ib = mct2 - ip;
	    		aip = a[ip-1];
	    		aib = a[ib-1];
	    		a[ip-1] = aip + rcmct * aib;
	    		a[ib-1] = aib + rcmct * aip;
        	}
        	a[mct] = rcmct;
        	alp[mct] = alpmin - alpmin * rcmct * rcmct;
        	rc[mct-1] = rcmct;
	   }
	  *lpcgain = alp[pptr->order];

      }
}

/* This routine computes the cepstral coefficients
	for an autoregressive model 
	given the prediction vector. 
	This routine uses essentially the
	same variables as were used in the original Fortran,
	as I don't want to mess with the magic therein. */
void lpc_to_cep( const struct param *pptr, struct fvec * lpcptr, 
			struct fvec * cepptr )
{
  int ii, j, l, jb;
  float sum;
  static float *c = NULL;
  float *a, *gexp;
  char *funcname;

	int i;
	double d_1, d_2;
        int finite = 0; 
  
	funcname = "lpc_to_cep";

	if(c == (float *)NULL)
	{
		c = (float *)calloc((pptr->nout),sizeof(float));
		if(c == (float *)NULL)
		{
			fprintf(stderr,"cant allocate c\n");
			exit(-1);
		}
	}


	fvec_check( funcname, lpcptr, pptr->nout -1 );
                /* bounds-checking for array reference */

	fvec_check( funcname, cepptr, pptr->nout -1 );
                /* bounds-checking for array reference */

	/* Move values and pointers over from calling
		names to local array names */

	a = lpcptr->values;
	gexp = cepptr->values;

        /* MM: */
        for (i=0;i<lpcptr->length;i++) if (a[i]!=0) finite = 1;

    /* Function Body */

	if((finite == 1) && (a[0]>0)) {
	  c[0] = -log(a[0]);
	  c[1] = -(double)a[1] / a[0];

	  /* Try changing how many times we go through loop
	     for (l = 2; l <= pptr->nout; ++l)  */
	  for (l = 2; l < pptr->nout; ++l) 
	    {
	      if (l <= pptr->order + 1) 
		{
		  sum = l * a[l] / a[0];
        	} 
	      else 
		{
		  sum = 0.;
        	}
	      for (j = 2; j <= l; ++j) 
		{
		  jb = l - j + 2;
		  if (j <= pptr->order + 1) 
		    {
		      sum += a[j-1] * c[jb - 1] * (jb - 1) / a[0];
		    }
        	}
	      c[l] = -(double)sum / l;
	    }
	  gexp[0] = c[0];
	  for (ii = 2; ii <= pptr->nout; ++ii) 
	    {
	      if (pptr->lift != 0.) 
		{
		  d_1 = (double) ((ii - 1));
		  d_2 = (double) (pptr->lift);
		  gexp[ii-1] = pow(d_1, d_2) * c[ii - 1];
        	} 
	      else 
		{
		  gexp[ii-1] = c[ii - 1];
        	}
	    }
	}
}

/*************************************************************************
 *                                                                       *
 *               ROUTINES IN THIS FILE:                                  *
 *                                                                       *
 *                      audspec(): critical band analysis; takes in      *
 *                              pointer to power spectrum fvec and       *
 *                              return a pointer to crit band fvec       *
 *                                                                       *
 *                      get_ranges(): computes low and high edges of     *
 *                              critical bands                           *
 *                                                                       *
 *                      get_cbweights(): get freq domain weights to      *
 *                              implement critical bands                 *
 *                                                                       *
 ************************************************************************/

/*
 *	This is the file that would get hacked and replaced if
 *	you want to change the preliminary auditory band
 * 	analysis. In original form, a power spectrum is
 *	integrated in sections to act like a band of filters with less
 *	than 1 bark spacing. This array is passed back to the
 * 	calling routine.
 *
 *	The first time that this program is called, we compute the
 * 	spectral weights for the equivalent filters.
 *	This has some magical numbers in it, but they are left
 *	in numerical form for the present; they are required for
 *	Hynek's implementation of the Zwicker critical bands.
 */

/* local function prototypes */
void get_ranges(struct range *, struct range *,
        	const struct param *, int, float );

void get_cbweights (struct range *, struct fvec *, 
			const struct param *, float );

struct fvec *audspec( const struct param *pptr, struct fvec *pspec)
{

	int i, j, icb, icb_end;
	int lastfilt;
	float hz_in_fsamp;
	char *funcname;

	static struct fvec *audptr; /* array for auditory spectrum */
	static struct range frange[MAXFILTS];  /* pspec filt band indices  */
	static struct range cbrange[MAXFILTS]; /* cb indices for weighting */
	static struct fvec *cbweightptr = NULL;


	funcname = "audspec";

	/* what is 1 Hz in fft sampling points */
	hz_in_fsamp = (float)(pspec->length - 1) 
			/ (float)(pptr->sampfreq / 2.);

	lastfilt = pptr->nfilts - pptr->first_good;

	/* MM: call another time with different filter number: */
	if(cbweightptr && (pptr->nfilts != audptr->length)) {
	   free_fvec(cbweightptr); cbweightptr = NULL;
	   free_fvec(audptr);
	}
	
	if(cbweightptr == (struct fvec *)NULL)
	{
		/* Find the fft bin ranges for the critical band filters */
		get_ranges (frange, cbrange, pptr, 
			pspec->length, hz_in_fsamp);
		cbweightptr = alloc_fvec(cbrange[lastfilt-1].end + 1);

		/* Now compute the weightings */
		get_cbweights (frange, cbweightptr, pptr,  hz_in_fsamp);

		audptr = alloc_fvec( pptr->nfilts );
	}

	fvec_check( funcname, audptr, lastfilt - 1);
		/* bounds-checking for array reference */

	for(i=pptr->first_good; i<lastfilt; i++)
	{
		audptr->values[i] = 0.;

		fvec_check( funcname, pspec, frange[i].end );

		icb_end = cbrange[i].start + frange[i].end - frange[i].start;

		fvec_check( funcname, cbweightptr, icb_end);

		for(j=frange[i].start; j<=frange[i].end;j++)
		{
			icb = cbrange[i].start + j - frange[i].start;
			audptr->values[i] += pspec->values[j] 
				* cbweightptr->values[icb];
		}
	}

	return( audptr );
}

/* Get the start and end indices for the critical bands */
void get_ranges(struct range *frange, struct range *cbrange, 
	const struct param *pptr, int pspeclength, float hz_in_samp)
{
	int i, wtindex;
	char *funcname;
	float step_barks;
	float f_bark_mid, f_hz_mid, f_hz_low, f_hz_high, 
		f_pts_low, f_pts_high;

	funcname = "get_ranges";

	/* compute filter step in Barks */
	step_barks = pptr->nyqbar / (float)(pptr->nfilts - 1);

	/* start the critical band weighting array index 
		where we ignore 1st and last bands,
		as these values are just copied
		from their neighbors later on. */
	wtindex = 0;
	
	/* Now store all the indices for the ranges of
		fft bins (powspec) that will be summed up
		to approximate critical band filters. 
		Similarly save the start points for the
		frequency band weightings that implement
		the filtering. */
	for(i=pptr->first_good; i<(pptr->nfilts - pptr->first_good); i++)
	{
		(cbrange+i)->start = wtindex;

		/*     get center frequency of the j-th filter 
				in Bark */
       		f_bark_mid = i * step_barks;

		/*     get center frequency of the j-th filter 
				in Hz */
       		f_hz_mid = 300. * (exp((double)f_bark_mid / 6.) 
			- exp(-(double)f_bark_mid / 6.));

		/*     get low-cut frequency of j-th filter in Hz */
       		f_hz_low = 300. * (exp((f_bark_mid - 2.5) / 6.) 
		- exp(-(double)(f_bark_mid - 2.5) / 6.));

		/*     get high-cut frequency of j-th filter in Hz */
       		f_hz_high = 300. * (exp((f_bark_mid + 1.3) / 6.) 
			- exp(-(double)(f_bark_mid + 1.3) / 6.));

		f_pts_low = f_hz_low * hz_in_samp;

		/* irint rounds to nearest integer */
		(frange+i)->start = (int) ((double)f_pts_low+0.5);
		if((frange+i)->start < 0)
		{
			(frange+i)->start = 0;
		}

		f_pts_high = f_hz_high * hz_in_samp;
		(frange+i)->end = (int) ((double)f_pts_high+0.5) ;
		if((frange+i)->end > (pspeclength-1) )
		{
			(frange+i)->end = pspeclength - 1;
		}
		wtindex += ((frange+i)->end - (frange+i)->start);
		(cbrange+i)->end = wtindex;
		wtindex++;
	}
}

/* Get the freq domain weights for the equivalent critical band filters */
void get_cbweights (struct range *frange, struct fvec *cbweight, 
	const struct param *pptr, float hz_in_fsamp)
{
	int i, j, wtindex;
	float f_bark_mid, step_barks;
	double freq_hz, freq_bark, ftmp, logwt;
	char *funcname;

	wtindex = 0;

	funcname = "get_cbweights";

	/* compute filter step in Barks */
        step_barks = pptr->nyqbar / (float)(pptr->nfilts - 1);

	for(i=pptr->first_good; i<(pptr->nfilts - pptr->first_good); i++)
        {
		/*     get center frequency of the j-th filter
                                in Bark */
                f_bark_mid = i * step_barks;
		for(j=(frange+i)->start; j<=(frange+i)->end; j++)
		{
			/* get frequency of j-th spectral point in Hz */
            		freq_hz = (float) j / hz_in_fsamp;

			/* get frequency of j-th spectral point in Bark */
            		ftmp = freq_hz / 600.;
            		freq_bark = 6. * log(ftmp + sqrt(ftmp * ftmp + 1.));

			/*     normalize by center frequency in barks: */
            		freq_bark -= f_bark_mid;

			/*     compute weighting */
            		if (freq_bark <= -.5) 
			{
				logwt = (double)(freq_bark + .5);
			}
			else if(freq_bark >= .5)
			{
				logwt = (-2.5)*(double)(freq_bark - .5);
			}
			else 
			{
                		 logwt = 0.0;
			}
			fvec_check( funcname, cbweight, wtindex );
                	ftmp = cbweight->values[wtindex]
				= (float)pow(LOG_BASE, logwt);
			wtindex++;
		}
	}
}
/************************************************************************
 *                                                                       *
 *               ROUTINES IN THIS FILE:                                  *
 *                                                                       *
 *                      post_audspec(): auditory processing that is post *
 *                                    RASTA processing, typically        *
 *                                    approximations to loudness         *
 *                                                                       *
 ************************************************************************/

/*
 *	This is the file that would get hacked and replaced if
 *	you want to change the preliminary post-auditory band
 * 	analysis. In original form, a critical band -like
 *	analysis is augmented with an equal loudness curve
 *	and cube root compression.

 *	The first time that this program is called, 
 *	in addition to the usual allocations, we compute the
 * 	spectral weights for the equal loudness curve.
 */
struct fvec *post_audspec( const struct param *pptr, struct fvec *audspec)
{

	int i, lastfilt;
	char *funcname;
	float step_barks;
        float f_bark_mid, f_hz_mid ;
	float ftmp, fsq;
	static float eql[MAXFILTS];

	static struct fvec *post_audptr = NULL; /* array for post-auditory 
						spectrum */

        /* compute filter step in Barks */
        step_barks = pptr->nyqbar / (float)(pptr->nfilts - 1);

	lastfilt = pptr->nfilts - pptr->first_good;

	funcname = "post_audspec";

	/* MM: call another time with different filter number: */
	if(post_audptr && (pptr->nfilts != post_audptr->length)) {
	   free_fvec(post_audptr); post_audptr = NULL;
	}
	if(post_audptr == (struct fvec *)NULL) /* If first time */
	{
		post_audptr = alloc_fvec( pptr->nfilts );

		for(i=pptr->first_good; i<lastfilt; i++)
		{
			f_bark_mid = i * step_barks;

			/*     get center frequency of the j-th filter
					in Hz */
			f_hz_mid = 300. * (exp((double)f_bark_mid / 6.)
				- exp(-(double)f_bark_mid / 6.));

		/*     calculate the LOG 40 db equal-loudness curve */
		/*     at center frequency */

			fsq = (f_hz_mid * f_hz_mid) ;
			ftmp = fsq + 1.6e5;
			eql[i] = (fsq / ftmp) * (fsq / ftmp)
				* ((fsq + (float)1.44e6)
				/(fsq + (float)9.61e6));
			
		}
	}

        fvec_check( funcname, audspec, (lastfilt - 1) );

	for(i=pptr->first_good; i<lastfilt; i++)
	{
		/* Apply equal-loudness curve */
		post_audptr->values[i] = audspec->values[i] * eql[i];

		/* Apply equal-loudness compression */
		post_audptr->values[i] = 
			pow( (double)post_audptr->values[i], COMPRESS_EXP);
	}

	/* Since the first critical band has center frequency at 0 Hz and
	bandwidth 1 Bark (which is about 100 Hz there) 
	we would need negative frequencies to integrate.
	In short the first filter is JUNK. Since all-pole model always
	starts its spectrum perpendicular to the y-axis (its slope is
	0 at the beginning) and the same is true at the Nyquist frequency,
	we start the auditory spectrum (and also end it) with this slope
	to get around this junky frequency bands. This is not to say
	that this operation is justified by anything but it seems
	to do no harm. - H.H. 

	8-8-93 Morgan note: in this version, as per request from H.H.,
	the number of critical band filters is a command-line option.
	Therefore, if the spacing is less than one bark, more than
	one filter on each end can be junk. Now the number of
	copied filter band outputs depends on the number
	of filters used. */

	for(i=pptr->first_good; i > 0; i--)
	{
		post_audptr->values[i-1] = post_audptr->values[i];
	}
	for(i=lastfilt; i < pptr->nfilts; i++)
	{
		post_audptr->values[i] = post_audptr->values[i-1];
	}
		
	return( post_audptr );
}
