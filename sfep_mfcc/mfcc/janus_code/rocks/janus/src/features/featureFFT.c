/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: FFT
               ------------------------------------------------------------

    Author  :  janus
    Module  :  featureFFT.c
    Date    :  $Id: featureFFT.c 2449 2004-05-10 14:16:30Z metze $
    Remarks :

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
    Revision 4.4  2004/05/10 14:16:30  metze
    Yue's changes

    Revision 4.3  2003/08/14 11:19:55  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 4.2.4.1  2002/07/18 12:27:32  soltau
    Removed malloc.h

    Revision 4.2  2001/01/15 09:49:56  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)
 
    Revision 4.1.36.1  2001/01/12 15:16:53  janus
    necessary changes for running janus under WINDOWS
  
    Revision 4.1  2000/01/12 10:02:02  fuegen
    make static functions not static
  
    Revision 4.0  1998/11/30 18:07:14  fuegen
    *** empty log message ***
  
    Revision 3.3  1998/09/30 12:10:07  westphal
    adc2spec spec2adc
 
    Revision 3.2  1997/07/09 17:42:22  westphal
    *** empty log message ***
 
 
    Revision 1.7  95/07/31  12:21:55  maier
    only floor() but not floorf() for HPs
  
    Revision 1.6  1995/07/21  16:13:34  maier
    removed bug in fft_pow() and compute_melscale_from_power()
  
    Revision 1.5  1995/03/08  20:49:10  maier
    added copyright, changed hamming routine,
    added another, faster FFT
 
    Revision 1.4  1994/08/03  17:48:15  monika
    to allow translation with stonage krc the variables  melA8 and  melA4
    in the routine  compute_melscale_from_power() are now static int instead
    of int to allow for agregate initialization in stoneage c.
 
    Revision 1.3  94/08/03  12:47:08  12:47:08  maier (Martin Maier)
    changed fht(), removed error in non ANSI declaration
 
    Revision 1.2  1994/07/14  12:00:36  maier
    check window size
 
    Revision 1.1  1994/07/12  14:57:03  janus
    Initial revision

    Revision 1.4  1993/11/29  15:16:31  janus
    [hhild] removed error in optional computation of 17th coefficient (energy)
 
    Revision 1.3  1993/11/24  16:55:41  janus
    added functions to copmute FFT from ADCs sampled with 8 kHz
    added comments
    modularized the melscale computation
 
    Revision 1.2  1993/08/16  09:42:15  janus
    removed bug in normalize()
 
    Revision 1.1  1993/08/14  21:02:19  janus
    Initial revision

   ======================================================================== */


#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "limits.h"
#include "featureFFT.h"

/* Constants (Tilo) */
#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

#ifndef M_2PI
#define M_2PI   6.28318530717958647692
#endif

#define DSP_WND_SIZE	 256			/* Max size of DSP window. (Real DSP window can be smaller	*/

#define then
#define mod %


double sin_tab  [DSP_WND_SIZE],           /* lookup table for the sine function   */
       cos_tab  [DSP_WND_SIZE],
       *s_Ptr,                           /* pointer for the sine in the table    */
       *ms_Ptr,                          /* pointer for minus sin in the table   */
       *c_Ptr;                           /* pointer for the cosine in the table  */
int    BitRev   [DSP_WND_SIZE];           /* table with the bit-reverse numbers
                                            to the base DSP_WND_SIZE             */

/*#############################################################################################################################
 #
 #  LOCAL FUNCTIONS
 #
 ############################################################################################################################*/


/*=============================================================================================================================
 | pseudo_log2:  Determines the most significant non-zero bit of an integer.
 |	        (This is equivalent to computing the base-2 order of magnitude of the integer, not equivalent to its 
 |	        base-2 logarithm.  However, in the context it is used (always operating on a radix-2 integer), the
 |	        function is equivalent to log-2 (num).
 |
 | PARAMETERS:
 |   num = integer
 |
 | RETURNS:  
 |   Most significant non-zero bit of an integer.
 |
 | HISTORY:
 |   ?? ??? 86  ahw        Created.
 |   30 May 91  arthurem   renamed stuff
 +===========================================================================================================================*/
#ifdef __STDC__
static int pseudo_log2( int num )
#else
static int pseudo_log2( num )
  int num;
#endif
{
  int 	   log_num = 0;
  unsigned itemp = num;
  
  while (itemp > 1) { itemp >>= 1; log_num++;}
  return (log_num);
}


/*=============================================================================================================================
 | do_hamm_window:  Hamming windowing routine.  Makes a Hamming window with a cosine curve from 0 to 2*pi,
 |		    shifted up and compressed to be between 0.08 and 1.00.  Write out the simple trig, it works!
 |
 | PARAMETERS:
 |   resultA   = pointer to array of floats, in which to store the results of the Hamming window
 |   pointsN   = int number of values in hamming window (8 <= pointsN <= 1024)
 |   inputA    = pointer to array of shorts, input data 
 |
 | RETURNS:
 |   0 if everything is okay, 1 if there is an error.
 |
 | HISTORY:
 |   22 Jun ??  Shikano    Wrote the original version.
 |   30 May 91	arthurem   changed names and documented
 |   08 Oct 91  arthurem   changed to incorporate Tilo table speedup
 |   13 Feb 95  martin m   dynamic memory allocation, changed computing
 +===========================================================================================================================*/
#ifdef __STDC__
int do_hamm_window( short inputA[], int pointsN, float resultA[] )
#else
int do_hamm_window( inputA,pointsN,resultA )
  short   inputA[];
  int     pointsN;
  float   resultA[];
#endif
{
  int i;				/* array index */
  static float *wind = (float*)NULL;	/* Hamming window multipliers for each point */
  static int prevN = -1;		/* Memory of last window size computed, saves needless re-computing */

  /* Make sure you have a reasonable number off points */
  if (pointsN < 0) {
     printf(" hamming: %d points?\n",pointsN);
     return(1);
  }
    
  /* If window array values not previously filled in for this number of points, compute again */
  if (prevN != pointsN) {
    double temp = 2. * M_PI / (double)(pointsN-1);

    if (wind) free(wind);
    wind = (float*) malloc(pointsN * sizeof(float));
    if (!wind) {
      printf(" hamming: no memory \n");
      return(1);
    }
    
    prevN = pointsN;
    for ( i=0 ; i<pointsN ; i++ )
      wind[i] = 0.54 - 0.46*cos(temp*i);
  }

  /* Fill in result array with appropriate values */
  for ( i=0 ; i<pointsN ;i++ )
    resultA[i] = wind[i]*inputA[i];

  /* Indicate all's well */
  return(0);
}


/*=============================================================================================================================
 | fht:  FHT routine (Fast Hartley Transformation)      itterative version, in the examples I assume N=256, ldN=8 
 |
 | PARAMETERS :
 |   IN :
 |        N       number of data points in the DSP window
 |        input   array with real values, that has to be transformed
 |
 |   OUT :
 |	  output  array with the transformation.
 |
 | NOTE :  init_fht() has to be called at the beginning of the program.
 |
 | HISTORY :
 |   13 Sep 91  tilo      created. lookup-table for sin(x), cos(x) is used.
 |   14 Jul 94  martin m. changed parameter ldN to variable
 +===========================================================================================================================*/
#ifdef __STDC__
void fht( int N, float *input, float *output )
#else
void fht( N, input, output )
        int      N;
        float	 *input, *output;
#endif
{
        double   *sine, *cosine;                                /* pointers into the lookup tables      */
        float    temp1[DSP_WND_SIZE],                            /* two arrays for intermediate results  */
                 temp2[DSP_WND_SIZE];
        float    *q, *p2, *p3;                                  /* pointers into the arrays             */
        float    *src, *tar, *src_beg, *tar_beg;            /* src source , tar target , t temp     */
        float    t0, t1, t2, t3, f0, f1, f2, f3;                /* all for intermediate results         */
        int      i, j, k, Nr, offset;
        int      Ndiv4, Ndivn, ldn, n,                          /* n is length of target array          */
                 ndiv2, ndiv2m1, ldNm1;                         /* ndiv2 is length of source array      */
                                                                /* m1 means minus 1                     */
	int ldN;                                                /* log to base 2                        */
	
  ldN   = pseudo_log2(N);	
  ldNm1 = ldN - 1;
  Ndiv4 = (N>>2);

  /*--- compute all Ndiv4 quadruples, and do the bit reversal, O(N) ---*/

  tar = temp1;
  offset = 0;
  for (i=0; i<Ndiv4; i++)                                       /* "offset" in the comments is :  offset = i*Ndiv4   */
    {
       f0 = input[ BitRev[offset++] ];                          /* in[offset],in[offset+1],in[offset+2],in[offset+3] */
       f1 = input[ BitRev[offset++] ];
       f2 = input[ BitRev[offset++] ];
       f3 = input[ BitRev[offset++] ];
       
       t0 = f0 + f1; t1 = f2 + f3; t2 = f0 - f1; t3 = f2 - f3;

       *tar++ = t0 + t1;    /* out[0] = in[0]+in[1]+in[2]+in[3] */
       *tar++ = t2 + t3;    /* out[1] = in[0]-in[1]+in[2]-in[3] */
       *tar++ = t0 - t1;    /* out[2] = in[0]+in[1]-in[2]-in[3] */
       *tar++ = t2 - t3;    /* out[3] = in[0]-in[1]-in[2]+in[3] */
    }

  /*--- merging stages :  O((ldN-2) * N) ---*/

  src = src_beg = temp1;
  tar = tar_beg = temp2;

  n = 8; ndiv2 = 4;                        /* ndiv2 is the length of the source array; n is the length of the target array */
  Ndivn = (Ndiv4 >> 1);

  for (ldn=3; ldn<=ldN; ldn++)             /* count the stages that have to be computed */
    {                                      /* starts with ldn=3 ; Ndivn = (N>>ldn) = 32 ; ndiv2 = 4 ; n = 8 */
      ndiv2m1 = ndiv2 - 1;
      q = src + ndiv2;

      for (Nr=0; Nr<Ndivn; Nr++)              /* count the number of sub-arrays that where put together in this stage */
        { 
          /*--- merge two n-tuples to 2n-tuples ,  O(N) ---*/    /*  offset is (Nr * n)    */

          cosine = c_Ptr;
          sine   = s_Ptr;

          k = 0;
          for(j=0; j<2; j++)                             /* two times */
            { 
              p2 = src;
              p3 = p2 + ndiv2;

              for(i=0; i<ndiv2; i++)                         /* ndiv2 times */
                {                                                /*  idx1 = offset + i;                               */
                                                                 /*  idx2 = offset + (i mod Ndiv2);                   */
                                                                 /*  idx3 = idx2 + Ndiv2;                             */
               /* p4 = q  + (k & ndiv2m1); */                    /*  idx4 = offset + Ndiv2 + (Ndiv2m1 * i) mod Ndiv2; */

                  *tar++ = (*p2++) + (*cosine) * (*p3++) + (*sine) * q[k & ndiv2m1];

                  k  += ndiv2m1;
                  cosine += Ndivn;
                  sine   += Ndivn;
               }
            }
          q += n;  src += n;                             /* increase src and q by  offset */
        }

      n <<= 1;  ndiv2 <<= 1; Ndivn >>= 1;       /* length of source array, length of target array  gets doubled */

      if (ldn<ldNm1)
         then { tar = src_beg; src = tar_beg;   /* toggle source, target arrays */
                tar_beg = tar; src_beg = src;
              }
         else { tar = output; src = tar_beg;    /* if ldn = ldNm1 : target has to be the output array */
              }
    }
}

/*=============================================================================================================================
 | init_fht:   initialization routine for the fht routine
 |             to be called just once, when the program starts
 | PARAMETERS :
 |   IN:     N    number of data points in the DSP window
 |
 | GLOBALS INITIALIZED :
 |       s_Ptr, c_Ptr , sin_tab, cos_tab, BitRev are initialized.
 |
 | COMMENT :
 |           initializes the sin, cos lookup tables,
 |           and the BitRev lookup table,
 |           which are used by the fht routine.
 |
 | RETURN :
 |           0                 successful,
 |          -1                 N not factor of DSP_WND_SIZE
 |           DSP_WND_SIZE     N > DSP_WND_SIZE
 |
 | HISTORY :
 |   12 Sep 91  tilo        created
 |   14 Jul 94  martin m.   check N  
 +===========================================================================================================================*/
#ifdef __STDC__
int init_fht( int N )
#else
int init_fht( N )
   int     N;      /* length of the table */
#endif
{
   int     i;      /* temporal table index */
   int     a,b,p;

   double * ptr,   /* temporal table pointer */
          * ptr1,
           scl,    /* scaling factor */
           x;

 if ((DSP_WND_SIZE % N) != 0)
 {  if (DSP_WND_SIZE < N) return DSP_WND_SIZE;
    return -1;
 };
      
 scl = M_2PI / N;

 x = 0.0;
 ptr  = sin_tab;                        /* point to the beginning of the sin table */
 ptr1 = cos_tab;

 for (i=0; i<N; i++, x += scl)
  {
   *ptr++  = sin(x);                     /* buildup the sin, cos tables */
   *ptr1++ = cos(x);
  }
 
 s_Ptr  = sin_tab;                      /* initialize the pointers to the beginning of the tables */
 c_Ptr  = cos_tab;

 for (i=0; i<N; i++)                    /* for all Numbers 0...N-1 */
   { a=i; b=0;
     for (p=(N>>1); p>0; p>>=1)         /* buildup the table with the bit-reversed numbers to the base N */
       {
         b += p*(a mod 2);
         a >>= 1;
       }
     BitRev[i] = b;
   };
   
 return 0;
}




/*=============================================================================================================================
 | fht_pow_spec:   compute the power spectrum of a FHT
 |
 | PARAMETERS :
 |   IN:     N       number of data points in the DSP window (has to be N !)
 |           in      array, that holds the fht data
 |
 |   OUT:    power   array with the power spectrum
 |
 | COMMENT:  P(f) = 1/2 * ( H(f)^2 + H(-f)^2 )
 |           
 | HISTORY :
 |    13 Sep 91  tilo     created
 |    01 Jun 93  Hermann  pow  is already name of a function. Changed to power
 +===========================================================================================================================*/
#ifdef __STDC__
void fht_pow_spec( int N, float *in, float *power )
#else
void fht_pow_spec( N, in, power )      /*   P(f) = 1/2 * ( H(f)^2 + H(-f)^2 )   */
     int   N;
     float *in, *power;
#endif
{
     float  x, y;
     float *p, *q;
     int    n;


  p = in;
  q = in + N-1;
  y = *p;               /* y = in[0] */
  N >>= 1;		/* only go through half the number of data points */
  for (n=0; n<N; n++)
    {
      x = *p++;
      *power++ = 0.5 * (x*x + y*y);
      y = *q--;
    }
}

/*=============================================================================================================================
 | fft:  FFT routine.
 |
 | PARAMETERS :
 |   IN :
 |        N        number of data points in the DSP window
 |	  mode     -1 for forward fft , 1 for inverse fft
 |
 |   IN / OUT :
 |        real     array of real parts
 |	  imag     array of imaginary parts
 |
 | NOTE :  init_fft() has to be called at the beginning of the program.
 |
 | HISTORY :
 |   ?? ??? ??  ???      created, without documentation, but with goto's -- aaaargh!
 |   13 Jul 91  tilo     used registers and += /= >>= ... for 21% speedup.
 |		         changed the shape of the mainloop (goto's considered harmful...)
 |    3 Sep 91  mw,tilo  introduced lookup-table for sin(x), cos(x)
 |   14 Jul 94  martin m. changed parameter ldN to variable
 +===========================================================================================================================*/
#ifdef __STDC__
void fft( register float *real, register float *imag, int N, int mode )
#else
void fft( real , imag , N , mode )
  register  float	*real , *imag;
            int         N;
            int         mode;        /*  mode, -1 for forward,  1 for inverse  */
#endif      
{
   int	    lmx , Ndiv2 , Nminus1 ;
   int      step;

   double   *sp, *sp_start, *cp;     /* pointers to sine and cosine values */
   int ldN;                          /* log to base 2                        */
	

   ldN   = pseudo_log2(N);	
   real -= 1;
   imag -= 1;
   lmx = N;
   /* scl = M_2PI / N; */
   
   cp = c_Ptr;
   if (mode == 1)
      then sp_start = s_Ptr;
      else if (mode == -1)
              then sp_start = ms_Ptr;
              else { fprintf(stderr, "\nERROR , wrong argument for mode in fft() !\n"); exit(0); };
   sp = sp_start;
   step = 1;

   { register int    i, j;              /* array indices for merging data */
     register double t1, t2, sine, cosine;
     register int    lo, lm, li, lix;

        for ( lo = 1 ; lo <= ldN ; lo++ )                 /* outer loop */
	{
		lix = lmx;
		lmx >>= 1;                                  /* lmx = lmx / 2; */
		sp = sp_start; cp = c_Ptr;                  /* reset the pointers to the beginning of the "tables" */

		for ( lm = 1 ; lm <= lmx ; lm++ )           /* middle loop */
		{
			cosine = *cp; sine = *sp;               /* get the sine, cosine values from the table */

                        sp += step; cp += step;                 /* increment the pointers to the "tables" */

			for ( li = lix ; li <= N ; li += lix ) /* inner loop */
			{
				i = li - lix + lm;
				j = i + lmx;
				t1 = real[i] - real[j];
				t2 = imag[i] - imag[j];

				real[i] += real[j];
				imag[i] += imag[j];

				real[j] = (cosine * t1) + (sine * t2);
				imag[j] = (cosine * t2) - (sine * t1);
			}
		}
		step *= 2;                                  /* double the step distance */
	}
   }

   { register int    i, j, k;
     register double t1, t2;

        j = 1;
        Ndiv2   = N / 2;
        Nminus1 = N - 1;
	for ( i = 1 ; i <= Nminus1 ; i++ )
	{
             if ( i < j )
	     {
		  t1 = real[j];            /* swap real[j] and real[i] */
		  t2 = imag[j];            /* swap imag[j] and imag[i] */

		  real[j] = real[i];
		  imag[j] = imag[i];
		  real[i] = t1;
		  imag[i] = t2;
             }
             k = Ndiv2;
             while (k < j)
             {
		  j -= k;                 /* j = j - k */
		  k >>= 1;                /* k = k / 2 */
             }
             j += k;                      /* j = j + k */
	}

	if ( mode == 1 )
	{
		for (i = 1 ; i <= N ; i++ )
		{
         	   real[i] /= N;
		   imag[i] /= N;
		}
	}
   }

 return;
}

/*=============================================================================================================================
 | init_fft:   initialization routine for the fft routine
 |             to be called just once, when the program starts
 | PARAMETERS :
 |   IN:     N    number of data points in the DSP window
 |
 | GLOBALS INITIALIZED :
 |       s_Ptr, ms_Ptr, c_Ptr, sin_tab are initialized.
 |
 | COMMENT :
 |           initializes the sine lookup table,
 |           which is used by the fft routine
 |           because of the symetry of sine, cosine we're using
 |           just one sine-table
 |           cos(x) = sin(x+pi/2)   |  sin(-x) = sin(x+pi)
 |
 | RETURN :
 |           0                 successful,
 |          -1                 N not factor of DSP_WND_SIZE
 |           DSP_WND_SIZE     N > DSP_WND_SIZE
 |
 | HISTORY :
 |    5 Sep 91  tilo        created
 |   14 Jul 94  martin m.   check N  
 +===========================================================================================================================*/
#ifdef __STDC__
int init_fft ( int N )
#else
int init_fft ( N )
   int     N;      /* length of the table */
#endif
{

   int     i;      /* temporal table index */
   double * p,     /* temporal table pointer */
           scl,    /* scaling factor */
           x;

 if ((DSP_WND_SIZE % N) != 0)
 {  if (DSP_WND_SIZE < N) return DSP_WND_SIZE;
    return -1;
 };

 scl = M_2PI / N;

 x = 0.0;
 p = sin_tab;                      /* point to the beginning of the sine table */

 for (i=0; i<N; i++, x += scl)
   *p++ = sin(x);

 s_Ptr  = &(sin_tab[0]   );        /* initialize the pointers to the beginning ...         */
 ms_Ptr = &(sin_tab[N/2]);         /* ... of the three functions sin(x), sin(-x), cos(x)   */
 c_Ptr  = &(sin_tab[N/4]);
 return 0;
}

#if 0     /* ----------> not used <---------- */
/*=============================================================================================================================
 | compute_power:   compute power of all FFT coefficients
 |
 | PARAMETERS :
 |   IN:     powerA	float array with power coefficients
 |			each coef was computes as  re^2 + im^2
 |           powerN	size of powerA
 |
 |   RETURN: 		power of all coeffiecients summed up
 |
 |           it's sufficient to call this routine with N/2 instead of N
 |
 | HISTORY :
 |    20 Oct 93  hhild     created
 +===========================================================================================================================*/
#ifdef __STDC__
static float compute_power( float* powerA, int powerN)
#else
static float compute_power( powerA, powerN)
      float	*powerA;
      int	powerN;
#endif
{
  float sum = 0.0;
  int 	i;

  for (i=0; i<powerN; i++) 
     sum += sqrt(powerA[i]);			/* sum up all power coefficients			*/

  sum /= ((float) powerN);			/* normalize by number of power coefficients		*/

  return(sum);
}
#endif      /* ----------> not used <---------- */



/*=============================================================================================================================
 | fft_pow_spec:   compute the power spectrum of a FFT
 |
 | PARAMETERS :
 |   IN:     N       half the number of data points in the DSP window 
 |           re,im   two arrays, that hold the fft data
 |
 |   OUT:    pow     array with the power spectrum
 |
 | COMMENT:  P(f) =  F_re(f)^2 + F_im(f)^2
 |
 |           it's sufficient to call this routine with N/2 instead of N
 |
 | HISTORY :
 |    12 Sep 91  tilo     created
 |    01 Jun 93  Hermann  "pow" is already name of a function. Changed to power
 +===========================================================================================================================*/
#ifdef __STDC__
void fft_pow_spec( int N, float *re, float *im, float *power )
#else
void fft_pow_spec( N, re, im, power )   /*   P(f) =  F_re(f)^2 + F_im(f)^2    */
     int   N;
     float *re, *im, *power;
#endif
{
     register
     float r,i;
     int   n;

  for (n=0; n<N; n++)
    {
      r = *re++;
      i = *im++;
      *power++ = r*r + i*i;
    }
}

/*=============================================================================================================================
 | bounds_detect:  Beginning/endpoint detector.  
 |
 | PARAMETERS:
 |   IN:
 |	ptp      = pointer to array of float peak-to-peak amplitude values.
 |	numfrms  = int number of frames.
 |   OUT:
 |	bounds   = pointer to array of int boundaries.  [0] and [3] are coarse boundaries, [1] and [2] are fine boundaries.
 |
 | ALGORITHM:
 |   This routine first finds a big jump (THRESH2) and then tries to find a smaller jump (THRESH1) 
 |   to determine the precise, fine location of the beginning/end of the ADC data.
 |
 | HISTORY:
 |   ?? ??? 90  jmt+bojan  Obtained code from Alex Waibel.
 |   30 May 91  arthurem   changed names and removed dependency on external variables
 +===========================================================================================================================*/
#ifdef __STDC__
static int bounds_detect( float *ptp, int numfrms, int *bounds )
#else
static int bounds_detect( ptp, numfrms, bounds )
  float *ptp;
  int    numfrms, *bounds;
#endif
{

  float THRESH1 =	0.05;
  float THRESH2 = 	0.2;	/* 0.3 sometimes failed, 0.2 is more sensitive. */
  int FRM_BEG_INT = 	4;	/* number of frames to keep ahead of first sound */
  int FRM_END_INT = 	20;	/* number of frames to keep after last sound */

  int i;
  int start1,start2,end1,end2;
  float *ptr1,*ptr2;

  for (i=0;i<4;i++) bounds[i] = 0;
  start1 = start2 = end1 = end2 = -1;

  /* find begin points */
  /*------------------*/
  ptr1 = ptp;
  ptr2 = &ptp[FRM_BEG_INT];
  for (i=FRM_BEG_INT;i<numfrms;i++) {
    if (*ptr2 > ((*ptr1)+THRESH2)) {start2 = i-1; break;}
    ptr1++; ptr2++;
  }
  ptr1 = ptp;
  ptr2 = &ptp[FRM_BEG_INT];
  start1= start2;
  for (i=FRM_BEG_INT;i<start2;i++) {
    if (*ptr2 > ((*ptr1)+THRESH1)) {start1 = i-1; break;}
    ptr1++; ptr2++;
  }

  /* find end points */
  /*------------------*/
  ptr1 = &ptp[numfrms-1];
  ptr2 = &ptp[numfrms-1-FRM_END_INT];
  for (i=numfrms-1;i>start2;i--) {
    if (*ptr2 > ((*ptr1)+THRESH2)) {end2 = i-FRM_END_INT; break;}
    ptr1--; ptr2--;
  }
  ptr1 = &ptp[numfrms-1];
  ptr2 = &ptp[numfrms-1-FRM_END_INT];
  end1 = end2;
  for (i=numfrms-1;i>end2;i--) {
    if (*ptr2 > ((*ptr1)+THRESH1)) {end1 = i-FRM_END_INT; break;}
    ptr1--; ptr2--;
  }

  /* printf ("boundaries are %d %d %d %d\n",start1,start2,end2,end1); */
  bounds[0] = start1 - FRM_BEG_INT;
  if (bounds[0] < 0) bounds[0] = 0;
  bounds[1] = start2 - FRM_BEG_INT;
  if (bounds[1] < 0) bounds[1] = 0;
  bounds[2] = end2 + FRM_END_INT;
  if (bounds[2] >= numfrms) bounds[2] = numfrms-1;
  bounds[3] = end1 + FRM_END_INT;
  if (bounds[3] >= numfrms) bounds[3] = numfrms-1;

  if (start1 == -1 || start2 == -1 || end2 == -1 || end2 == -1) return(-1);

  return(0);
}


/*=============================================================================================================================
 | find_frams_ampl_range:  Looks at adc data, computes an array of amplitude ranges in each DSP_window_size points, stepped
 |		          80 points at a time.
 |
 | PARAMETERS:
 |   IN:
 |	ad_buf     = pointer to array of short int ADC samples.
 |	numsamples = int size of ad_buf.
 |	sampl_shift= int number of adc values between start of each fram
 |   OUT:
 |	ptp        = pointer to array of float amplitude ranges, one array element for each fram.
 |
 | RETURNS:
 |   Length of ptp array.
 |
 | HISTORY:
 |   ?? ??? 90  jmt+bojan  Obtained code from Alex Waibel.
 |   30 May 91  arthurem   extended documentation, renamed stuff, removed global var dependencies
 |   24 Feb 92  arthurem   added framshift call to help take care of multiple sampling rates
 |		           changed so it no longer returns unnecessary already-calculated number
 +===========================================================================================================================*/
#ifdef __STDC__
static void find_frams_ampl_range( short *ad_buf, int numsamples, int sampl_shift, int DSP_window_size,  float ptp[] )
#else
static void find_frams_ampl_range( ad_buf, numsamples, sampl_shift, DSP_window_size,  ptp )
  short *ad_buf;
  int    numsamples;
  int    sampl_shift;
  int	 DSP_window_size;
  float  ptp[];
#endif
{
  int i,j, numfrms;
  short ptpmax,ptpmin;
  float *ptr1;
  short *ptr2,*startptr;

  numfrms = (int)((numsamples - (DSP_window_size - sampl_shift)) / sampl_shift);

  ptr1 = ptp;
  ptr2 = ad_buf;
  for (i=0;i<numfrms;i++) {
    ptpmax = SHRT_MIN;  /* -HUGE; */
    ptpmin = SHRT_MAX;  /* HUGE; */
    startptr = ptr2;
    for (j=0;j<DSP_window_size;j++) {
      if (*ptr2 > ptpmax) ptpmax = *ptr2;
      if (*ptr2 < ptpmin) ptpmin = *ptr2;
      ptr2++;
    }
    *ptr1++ = (float) (ptpmax - ptpmin);
    ptr2 = startptr + sampl_shift;
  }	
}


/*=============================================================================================================================
 | normalize:  Normalizes an array of values, so that highest = 1.0, others scaled appropriately.
 |
 | PARAMETERS:
 |   farray  = array of floats.
 |   numfrms = size of farray.
 |   factor  = normalization factor.  If factor = 0, farray is normalized by its largest absolute value.
 |
 | RETURNS:
 |   Maximum absolute value in farray before normalization.  (Or factor, if factor != 0.)
 |
 | HISTORY:
 |   ?? ??? 90  jmt+bojan  Obtained code from Alex Waibel.
 |   16 Aug 93  sloboda    changed initialization of the variable max
 +===========================================================================================================================*/
#ifdef __STDC__
static float normalize( float *farray, int numfrms, float factor)
#else
static float normalize( farray, numfrms, factor )
  float *farray;
  int    numfrms;
  float  factor;
#endif
{
  int    i;
  float  max;
  float *fptr;

  if (factor == 0.0) {
    fptr = farray;
    max = *fptr;                         /* max = -HUGE  ==>  floating point number exceeds range of `double' (Tilo) */
    for (i=0;i<numfrms;i++) {
      if (*fptr > max) max = *fptr;
      if (-(*fptr) > max) max = -(*fptr);
      fptr++;
    }
  }
  else max = factor;
  fptr = farray;
  for (i=0;i<numfrms;i++)
    *fptr++ /= max;
  return (max);
}







/*=============================================================================================================================
 | mk_mel_coeff:  Compute one melscale coefficient from power spectrum coefficients.
 |          (See A. H. Waibel (ATR-TR-I-0006) for conceptual details.)
 |
 | PARAMETERS: 
 |              powrCoefA:      A pointer to start of array of Power values (floats) obtained from fft operation
 |              coefXI:         Integer index of first element in Power array to put in a "bin"
 |              coefXF:         Integer index of last element in Power array to put into the "bin"
 |
 | RETURNS:  A float value x which is the log of the sums of the elements included in the "bin" (only 1/2 of endpoints added)
 |	     x = log10( 0.5 * powrCoefA[coefXI] + powrCoefA[coefXI+1] ... + powrCoefA[coefXF-1] + 0.5 * powrCoefA[coefXF] )
 |
 | HISTORY:
 |   ?? ??? 86  ahw        Created.
 |   30 May 91  arthurem   changed names and documented
 +===========================================================================================================================*/
#ifdef __STDC__
static float mk_mel_coeff( float powrCoefA[], int coefXI, int coefXF )
#else
static float mk_mel_coeff( powrCoefA, coefXI, coefXF )
  float powrCoefA[];
  int   coefXI;
  int   coefXF;
#endif
{
  int i;						/* index variable */
  float mel_coeff;					/* computed value */
  
							/* First add together half of first and last values in this range */
  if (coefXI == 0) mel_coeff = powrCoefA[0];
    else           mel_coeff = powrCoefA[coefXI]/2.0;
  mel_coeff += powrCoefA[coefXF]/2.0;
  
  for (i=coefXI+1;i<=coefXF-1;i++) 			/* Now add all the values inbetween the first and last to sum */
     mel_coeff += powrCoefA[i];

  mel_coeff = log10 ((double) mel_coeff + 1.0);		/* Now take the log of this final sum (add one so result has a min of 0 */

  return (mel_coeff);					/* Return the value */
}




/*=============================================================================================================================
| compute_melscale_from_power
|
| DESCRIPTION:     Computes melscale coefficients from FFT'ed window (powerA)
|		    Should be generalized to handle a variable number of coefficients
|
| PARAMETERS:
|		powerA		: Array with FFT power coefficients (input)
|		powerN		: number of FFT power coeff. (usually DSP_window_size/2)
|		coeffA		: Array with mel-scale coefficients computed from powerA
|		coeffN		: number of mel-scale coefficients to be computed
|		scale8		: if TRUE, use different scaling (for 8kHz) sampling
|
| RETURNS:
|
|
|  The table below shows how the power FFT coefficients are condensed to 
|  melscale coeffients and the corresponding frequencies  (From Bernhard Suhms "Studienarbeit")
|
|	mel	power	 Hz		dHz
|      ------------------------------------
|	0	  0	   0		125
|	1	  2	 125		250
|	2	  6	 375		250
|	3	 10	 625		250
|	4	 14	 875		250
|	5	 18	1125		250
|	6	 22	1375		250
|	7	 26	1625		250
|	8	 30	1875		250
|	9	 35	2187		312
|	10	 41	2562		375
|	11	 48	3000		438
|	12	 57	3562		562
|	13	 68	4250		688
|	14	 81	5062		812
|	15	 97	6062		1000
|		116	7250		1188
|
| 
|  For sampling rate 8 kHz , i.e. a 4 kHz range (!!), we use a different scaling:
|
|
|		The first 13 mel-scale (range 0-8kHz) coefficients cover the range from 0 to 4 kHz:
|
|  mel-coef	0	1   	2   	3   	4   	5   	6   	7   	8   	9  	10  	11  	12 	13 
|		++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
|  FFT-coef	0   	2   	6  	10  	14  	18  	22  	26  	30  	35  	41  	48  	57	68
|  Freq		0											3000    3562    4250
|                   2      4        4       4       4       4       4        4      5       5       7       9       11
|
|		If we want to use 16 coefficients for 8kHz sampling, how do we have to position them in order to get
|		about the same mapping as we got for the 16 kHz sampling? :
|
|		0     1     2     3     4     5     6     7     8      9      10     11     12     13     14     15     16
|		++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
| FFT-coef	0   	2   	6  	10  	14  	18  	22  	26  	30  	35  	41  	48  	57	68
|		0     2     4     7     10    13    16    19    22     25     29     33     37     43     50     58     68
|		0     4     8    14     20    26    32    38    44     50     56     63     71     83     96     111    127
|		   4     4    6      6      6    6     6     6      6      6      7     9      11     13      15     16
|		0													4000
|
|
|
| HISTORY:
|   11 Aug 93  hhild      extraced from do_FFT routine.
|			   changed "tmpA[offset + i]" to more efficient direct assigned  "coeffA[i]"
|   15 Oct 93  hhild      added stuff for 8 kHz sampling
|   29 Apr 94  hhild	  added stuff for variable DSP_window_size
|   20 Jul 95  maier      changed 'scale8' into 'sampling_rate', still support values 0 and 1 (for 16 and 8 kHz)
+===========================================================================================================================*/
#ifdef __STDC__
int compute_melscale_from_power( float *coeffA, int coeffN,  float *powerA, int powerN, int sampling_rate)
#else
int compute_melscale_from_power( coeffA, coeffN, powerA, powerN, sampling_rate)
   float  *coeffA;
   int	  coeffN;
   float  *powerA;
   int    powerN;
   int    sampling_rate;
#endif
{
   /*------------------------------------------------------------
    |  compute 16 mel-scale coefs from power values:
    |
    |  Convert the power spectrum into Melscale coefficients.
    |  See Waibel (ATR TR-I-0006) sec. 2.1 and
    |  Waibel & Yegnanarayana (1981) for details.
    +----------------------------------------------------------*/
    static int  melA8[17] = {0,2,6,10,14,18,22,26,30,35,41,48,57,68,81,97,116};		/* scaling for 8 kHz range	*/
    static int  melA4[17] = {0,4,8,14,20,26,32,38,44,50,56,63,71,83,96,111,127};	/* scaling for 4 kHz range	*/
    int* melA;
    int  coeffX;
    float  scaling = floor(powerN/2.0) / 64;                               /* 128 and 129 should give scaling = 1.0      */

    if (coeffN != 16) 
    {
      fprintf(stderr, "ERROR:do_FFT: can only compute 16 melscale coefficients but coeffN = %d\n", coeffN);
      return(0);
    }

    switch(sampling_rate) {
     case   16: case 0:	melA = melA8;		break;					/* use the 8 kHz range scaling	*/
     case    8: case 1:	melA = melA4;		break;					/* use the 4 kHz range scaling	*/
     default :	fprintf(stderr, "do_FFT: ERROR: can only compute melscale for sampling rate 8 or 16 kHz (%d)\n", sampling_rate);
		return(0);
    }

    for (coeffX=0;  coeffX<coeffN;  coeffX++) {
       int from_power = melA[coeffX  ] * scaling;
       int to_power   = melA[coeffX+1] * scaling;
       coeffA[coeffX] = mk_mel_coeff(powerA, from_power, to_power);
    }   
    return(1);
}
/*---------------------------------------------------------------------------------------------------------------------------*/




/*=============================================================================================================================
 | adc_to_melscale_frame:
 |
 | DESCRIPTION:  This function takes a pointer to an array of at least DSP_window_size
 |               shorts and a pointer to an array of at least 16 floats.  The function
 |               basically calculates a single frame's worth (16) of melscale values
 |               from the array of shorts (adc data), and writes it into the array
 |               of floats (FFT/melscale data).  No normalization is done at this point.
 |
 | PARAMETERS:
 |
 | RETURNS:
 |
 | HISTORY:
 |   ?? ??? 90  arthurem   created ??? 
 +===========================================================================================================================*/
#ifdef __STDC__
void adc_to_melscale_frame( short *adc_buf, float *fft_buf,  int freq , int DSP_window_size)
#else
void adc_to_melscale_frame( adc_buf, fft_buf, freq , DSP_window_size)
  short *adc_buf;		/* array of adc values 			*/
  float *fft_buf;		/* array for melscale value output 	*/
  int	freq;			/* sampling frequency in kHz		*/
  int	DSP_window_size;	/* size of used DSP window		*/
#endif
{
  float inA[DSP_WND_SIZE];			/* array for result of hamming window, in and out for fft, only in for fht */
  float outA[DSP_WND_SIZE];			/* array for imaginary output of fft, real output of fht */
  float powerA[DSP_WND_SIZE/2];			/* array of power values */
  int scale8 = (freq == 8);			/* 8 kHz needs a different melscale computation */
  int i;
  
  for (i=0; i<DSP_window_size; i++) 			/* clear arrays */
    inA[i] = outA[i] = 0.0;
  
  do_hamm_window(adc_buf, DSP_window_size, inA); 	/* do hamm window */
  
  fht(DSP_window_size, inA, outA);			/* compute fht values for this frame	*/
  fht_pow_spec(DSP_window_size, outA, powerA);		/* compute power values */

  compute_melscale_from_power(fft_buf, 16, powerA,  DSP_window_size/2, scale8);	
}




/*=============================================================================================================================
 | normalize_melscale_spectrum
 |
 | DESCRIPTION:    This function takes the array with melscale FFT coefficients and normalizes them
 |
 | PARAMETERS:
 |
 | RETURNS:
 |
 | HISTORY:
 |   11 Aug 93  hhild      extraced from do_FFT routine
 +===========================================================================================================================*/
#ifdef __STDC__
static void normalize_melscale_spectrum( float *tmpA, int framXN, int coefs_per_fram, int normalization )
#else
static void normalize_melscale_spectrum( tmpA, framXN, coefs_per_fram, normalization )
  float  *tmpA;
  int     framXN;
  int     coefs_per_fram;
  int     normalization;
#endif
{
  int   i;
  if (normalization != FRAME_BY_FRAME)  /* do the REGULAR normalization if not explicitly inhibited */
  {
   /*--------------------------------------------------------------------------------
    | do the REGULAR normalization: 
    | this is the one which is _usually_ used -- it's drawback is that it needs to
    | see the whole speech sequence before it can start to normalize.
    +------------------------------------------------------------------------------*/
    							/* find min, max vals for mel-scale coefs */
    float maxval = tmpA[0];  				/* maximum valued coefficient over whole sample */
    float minval = tmpA[0];  				/* minimum valued coefficient over whole sample */

    for (i= 0; i< framXN*coefs_per_fram; i++)
    {
           if (tmpA[i] > maxval) maxval = tmpA[i]; 
      else if (tmpA[i] < minval) minval = tmpA[i];
    }
    
    if (maxval == minval) 				/* normalize coefs and shift so lowest is zero, highest is one */
      for (i=0; i< framXN*coefs_per_fram; i++)
	tmpA[i] = 0.0;   
    else
      for (i=0; i< framXN*coefs_per_fram; i++)
	tmpA[i] = (tmpA[i] - minval) / (maxval-minval);   
  }
  else
  {
   /*--------------------------------------------------------------------------------
    | do the FRAME_BY_FRAME normalization which Arthur introduced:
    | pick points far out and truncate anything beyond it (works frame by frame).
    +------------------------------------------------------------------------------*/
    for (i=0; i< framXN*coefs_per_fram; i++)
    {
      tmpA[i] = (tmpA[i] -4.0) / 8.0;
      if (tmpA[i] > 1.0) tmpA[i] = 1.0;
      if (tmpA[i] < 0.0) tmpA[i] = 0.0;
    }
  }
}





/*=============================================================================================================================
 | Silence clipping
 |
 | DESCRIPTION:    This function takes the array with melscale FFT coefficients and normalizes them
 |
 | PARAMETERS:
 |
 | RETURNS:
 |
 | HISTORY:
 |   11 Aug 93  hhild      extraced from do_FFT routine
 +===========================================================================================================================*/
#ifdef __STDC__
static void silence_clipping( short* adcA, int adc_valuesN, int fram_shift, int total_frams, 
			int* framXI,  int* framXF, int DSP_window_size)
#else
static void silence_clipping( adcA, adc_valuesN, fram_shift, total_frams, framXI, framXF, DSP_window_size)
  short* adcA; 
  int 	adc_valuesN;
  int 	fram_shift;
  int 	total_frams;
  int* 	framXI;
  int* 	framXF;
  int   DSP_window_size;
#endif
{
  int bounds[4];						/* small array to hold a soft and hard boundary for start and end */
  float *fram_amplA;							/* array to hold amplitude spreads for each fram */
    
  fram_amplA = (float *) malloc(sizeof(float) * total_frams); 	/* allocate memory for fram_amplitude_spread array */
  if (fram_amplA == NULL)
  {
    fprintf(stderr, "Malloc failed for frame amplitude spread array\n");
    exit(-1);
  }

  find_frams_ampl_range(adcA, adc_valuesN, fram_shift, DSP_window_size, fram_amplA); 	/* Determine endpoints */
  normalize(fram_amplA, total_frams, 0.0);
  bounds_detect (fram_amplA, total_frams, bounds);
  *framXI = bounds[0];
  *framXF = bounds[3];

  free(fram_amplA); 							/* Free allocated memory */
}




/*#############################################################################################################################
 #
 #  GLOBAL FUNCTIONS
 #
 ############################################################################################################################*/

/*=============================================================================================================================
 | do_FFT
 |
 | DESCRIPTION:    This function takes a pointer to the beginning of an array of shorts,
 |                 the number of shorts in this array, a pointer to the pointer to the
 |                 final location of the one dimensional (float) fft array, an int specifying
 |                 the sampling freq in kHz (can only be 10 or 16), and an int specifying
 |                 5 or 10 msec for the framelength.  The function returns the 
 |                 number of FFT coefficients put in the new fft array, which is malloc'ed 
 |                 from within this function.
 |
 | PARAMETERS:
 |	IN:
 |		adcA			: data points of sampled wave form
 |		adc_valuesN		: size of adcA
 |		freq			: freq in kHz (can only be 10 or 16)
 |		msec			: windowwidth = frameshift in msec (can only be 5 or 10)
 |		transformation		: Hartley or FFT, see FFT.h for definitions
 |		normalization		: REGULAR  or FRAME_BY_FRAME, see FFT.h for definitions
 |		chop			: TRUE:  clip leading and trailing silence in wave form
 |					  FALSE: the entire wave form is transformed
 |	OUT:
 |		fftA
 |
 | RETURNS:
 |		number of FFT-coefficients
 |
 | EXAMPLE:
 |   msec = 10, freq = 16,   DSP_WND_SIZ = 256
 | 
 |   
 |   The size of one window is 256 data points, which corresponds to a 256/16 = 16 msecs time interval.
 |   One FFT frame is computed from one such window. Then the window is shifted 10 msecs (or 16 * 10 = 160 data points)
 |   to the right.
 |
 |   adcA:	+-------------------------------------------------------------------------------------------+
 |   window 1:	****************
 |   window 2:		  ****************
 |   window 3:			    ****************
 |              |        |      +   |		   |    
 |   in msec    0        10     16  20	  	   30
 |   in points  0        160    256 320            480
 |
 |   For each window, the following operations are performed
 |	1. multiply the window with a hamming window, to get smoother edges  (256 data points)
 |      2. perform FFT transformation on the window			     (256 real and 256 imaginary data points)
 |      3. compute power spectrum from real and imaginary FFT result	     (265 real coefficients)
 |      4. compute mel-scale coefficients				     (condensed to 16 coefficients)
 |
 |  Finally, the FFT frames are normalized
 |
 |
 |----------------------------------------------------------------------
 | The number of frames generated depends on the sampling frequency
 |----------------------------------------------------------------------
 | 
 |  The number of frames is computed as  F(t) = (adcN - WS + sh) / sh ,
 |
 |  where	t		time in seconds
 |		adcN		number of data points
 |		WS		size of DSP window = 256
 |		sh = f * delta	shift between frames in data points, delta = shift in second
 |
 |
 | i.e. for  f = 16000 [1/sec],  adcN = t * f,  WS = 256,  delta = 0.01[sec],  sh = 16000 [1/sec]  * 0.01 [sec] = 160
 |
 |     F(t) = (t * 16000 - 256 + 160) / 160 =  t * 100 - 256/160 + 1  = 100 * t - 16/10 + 10/10  
 |          = 100 * t - 6/10
 |          = 100 * t - 0.6 = n,  the number of frames
 |
 |    
 | if f = 8000 [1/sec], we get slightly different numbers:
 |
 |     F^(t) = (t * 8000 - 256 + 80) / 80 = 100 * t  - 256/80 + 1  = 100 * t - 16/5 + 5/5
 |	     = 100 * t - 11/5
 |	     = 100 * t - 2.2
 |
 | Example:
 |  	for t = 1.006 sec we get
 |				F(t)  = 100.6 - 0.6 = 100 = 100 frames
 |				F^(t) = 100.6 - 2.2 = 98.4 = 98 frames
 |
 |
 | HISTORY:
 |   ?? ??? ??  arthurem   created - probably copied from somewhere...??? 
 |   08 Oct 91  arthurem   included Tilo's lookup table initialization
 |   09 Oct 91  arthurem   added flag for FFT versus tilo's FHT algorithm
 |                         added flag for normalization (set or determined mode)
 |   24 Feb 92  arthurem   added flag for endpoint calculation
 |    3 Aug 93  Tilo       renamed parameter "hartley" to "transformation", 
 |                         now using the #define'd values for "transformation" and "normalization"
 |                         HARTLEY transform is now the default, as it's faster!
 |                         REGULAR normalization now the default, as it's backward compatible!
 |   11 Aug 93  hhild	   moved melscale computation and normalization into extra routines
 |			   allowed 8 kHz sampling freqency (for downsampled 16 kHz samples)
 |   15 Oct 93  hhild	   moved silence clipping into extra function
 |			   added special melscale computation for 8 kHz
 |   14 Jul 94  maier      removed variable np_log_2
 +===========================================================================================================================*/
#ifdef __STDC__
int do_FFT( short adcA[], int adc_valuesN, float **fftA, int freq, int msec, int transformation, int normalization, int chop )
#else
int do_FFT( adcA, adc_valuesN, fftA, freq, msec, transformation, normalization, chop )
  short  adcA[];
  int    adc_valuesN;
  float **fftA;
  int    freq;
  int    msec;
  int    transformation;
  int    normalization;
  int    chop;
#endif
{
   /*-------------------------------------------------------
   ; Just call do_FFT2 with constant DSP_WND_SIZE
   ;------------------------------------------------------*/
   int fftN = do_FFT2( adcA, adc_valuesN, fftA, freq, msec, transformation, normalization, chop, DSP_WND_SIZE);
   return(fftN);
}




#ifdef __STDC__
int do_FFT2( short adcA[], int adc_valuesN, float **fftA, int freq, int msec, int transformation, 
		int normalization, int chop, int DSP_window_size)
#else
int do_FFT2( adcA, adc_valuesN, fftA, freq, msec, transformation, normalization, chop, DSP_window_size)
  short  adcA[];
  int    adc_valuesN;
  float **fftA;
  int    freq;
  int    msec;
  int    transformation;
  int    normalization;
  int    chop;
  int	 DSP_window_size;
#endif
{
  int coefs_per_fram = 16;		/* number of coefficients to be computed 	*/
  int fram_shift;			/* num samples to step to start of next fram 	*/
  int total_frams;			/* number of frams which can be calculated 	*/
  float *tmpA;				/* temp pointer to new array for fft values 	*/
  int framX;				/* current fram number 				*/
  int framXI;				/* first fram that should be calculated 	*/
  int framXF;				/* last fram that should be calculated 		*/
  int framXN;				/* number of frams 				*/


  /*------------------------------------------------------------
  ;  Check parameters to see if they are correct 
  ;-----------------------------------------------------------*/
  if ((freq != 10) && (freq != 16) && (freq != 8))
  {
    fprintf(stderr, "do_FFT: ERROR: freq param to do_fft must be 8, 10 or 16\n");
    return(0);
  }
  if ((msec != 5) && (msec != 10))
  {
    fprintf(stderr, "do_FFT: ERROR: msec param to do_fft must be 5 or 10\n");
    return(0);
  }

  if (DSP_window_size != DSP_WND_SIZE  &&  DSP_window_size != DSP_WND_SIZE/2)
  {
    fprintf(stderr, "do_FFT: ERROR: DSP_window_size must be %d or %d\n", 
			DSP_WND_SIZE,  DSP_WND_SIZE/2); 
    return(0);
  }


  /*------------------------------------------------------------
  ;  Initialisations
  ;-----------------------------------------------------------*/
  switch (transformation) { 						/* Initialize lookup tables */
      case 	FOURIER	: init_fft(DSP_window_size);	break;		
      default		: init_fht(DSP_window_size);	break;		/* the hartley transformation is the default */
  }

  fram_shift = msec*freq; 						/* Set fram_shift, e.g 10 msec * 16 kHz = 160 frames */

  									/* determine number of frames that can be calculated */
  /*------------------------------------------------------------
  ;  Compute begin and end frame
  ;-----------------------------------------------------------*/
  total_frams = (int)((adc_valuesN - (DSP_window_size - fram_shift)) / fram_shift);
  if (total_frams == 0)
    return(0);
  
  									/* Assume no bounds checking is done		*/
  framXI = 0;								/* start at beginning" the very first frame	*/
  framXF = total_frams - 1;						/* end at end: the very last frame		*/

  if (chop)  								/*  Silence clipping if requested 		*/
    silence_clipping(adcA, adc_valuesN, fram_shift, total_frams, &framXI, &framXF, DSP_window_size);

  framXN = framXF - framXI + 1;						/* total number of frames	*/


  /*------------------------------------------------------------
  ;   allocate memory for final fft array 
  ;-----------------------------------------------------------*/
  if (*fftA != NULL) fprintf(stderr, "WARNING:do_FFT: *fftA pointer should be NULL\n");
  tmpA = (float *) malloc(sizeof(float) * coefs_per_fram * framXN);
  if (tmpA == NULL)
  {
    fprintf(stderr, "ERROR:do_fft: Malloc failed for new fft array\n");
    exit(-1);
  }
  *fftA = tmpA;

  /*------------------------------------------------------------
  ;   Do fft for each frame, one-by-one 
  ;-----------------------------------------------------------*/
  for (framX = framXI; framX<=framXF; framX++)
  {

    float  inA[DSP_WND_SIZE];			/* array for result of hamming window, in and out for fft, only in for fht */
    float  outA[DSP_WND_SIZE];			/* array for imaginary output of fft, real output of fht */
    float  powerA[DSP_WND_SIZE/2];		/* array of power values */
    float* coeffA = tmpA + (framX-framXI) * coefs_per_fram; 		/* the coefficients for this frame are stored here	*/
    int    i;
    int    scale8 = (freq == 8);					/* 8 kHz sampling needs different melscale computation */
    int	   ok = 1;

    for (i=0; i<DSP_window_size; i++) 					/* clear arrays 	*/
      inA[i] = outA[i] = 0.0;

    do_hamm_window(&adcA[framX * fram_shift], DSP_window_size, inA); 	/* do hamm window 	*/
 
    switch (transformation) { 						/* do the fft or fht for this fram 	*/ 
      case FOURIER : 
		fft(inA, outA, DSP_window_size, -1);			/* compute fft values 		*/
      		fft_pow_spec(DSP_window_size/2, inA, outA, powerA);	/* compute power values 	*/
		break;
      default : 							/* the hartley transformation is the default	*/
      		fht(DSP_window_size, inA, outA);			/* compute fht values 		*/
      		fht_pow_spec(DSP_window_size, outA, powerA);		/* compute power values 	*/
		break;
    }

    ok = compute_melscale_from_power(coeffA, coefs_per_fram, powerA, DSP_window_size/2, scale8);	

    if (!ok) return(0);

 }
  

  /*------------------------------------------------------------
  ;   Normalize the computed FFT coefficients
  ;-----------------------------------------------------------*/
  normalize_melscale_spectrum(tmpA, framXN, coefs_per_fram, normalization);

  return(framXN*coefs_per_fram); 					/* you're done! */
}

/*===========================================================================================================================*/

/*************************************************************************
 *                                                                       *
 *               ROUTINES IN THIS FILE:                                  *
 *                                                                       *
 *                      fft_float(): calling routine for complex fft     *
 *                                   of a real sequence                  *
 *                                                                       *
 *                      fft_pow(): calling routine for complex fft       *
 *                                   of a real sequence that concludes   *
 *                                   by computing power spectrum         *
 *                                                                       *
 *                      FAST(): actual fft routine                       *
 *                                                                       *
 * 			FR2TR(): radix 2 transform                       *
 *                                                                       *
 * 			FR4TR(): radix 4 transform                       *
 *                                                                       *
 * 			FORD1(): re-ordering routine                     *
 *                                                                       *
 * 			FORD2(): other re-ordering routine               *
 *                                                                       *
 *  			fastlog2(): just what it sounds like             *
 *                                                                       *
 ************************************************************************/

/*
** Discrete Fourier analysis routine
** from IEEE Programs for Digital Signal Processing
** G. D. Bergland and M. T. Dolan, original authors
** Translated from the FORTRAN with some changes by Paul Kube
**
** Modified to return the power spectrum by Chuck Wooters
**
** Modified again by Tony Robinson (ajr@eng.cam.ac.uk) Dec 92

** Slight naming mods by N. Morgan, July 1993
	(fft_chuck -> fft_pow)
	( calling args long ll -> long winlength)
			(long m -> long log2length)
*/

typedef float real;

#ifndef PI
#define PI  3.1415926535897932
#endif
#define PI8 0.392699081698724 /* PI / 8.0 */
#define RT2 1.4142135623731  /* sqrt(2.0) */
#define IRT2 0.707106781186548  /* 1.0/sqrt(2.0) */

#define signum(i) (i < 0 ? -1 : i == 0 ? 0 : 1)

int  FAST(real*, int);
void FR2TR(int, real*, real*);
void FR4TR(int, int, real*, real*, real*, real*);
void FORD1(int, real*);
void FORD2(int, real*);
int  fastlog2(int);

/* 
static void fft_float(float *orig, float *fftd, int npoint) {
  int i;

  for(i = 0; i< npoint; i++) fftd[i] = orig[i];

  if(FAST(fftd, npoint) == 0 ){
    fprintf(stderr, "Error calculating fft.\n");
    exit(1);
  }
}
*/

int fft_pow(float *orig, float *power, long winlength, long log2length) {
  int i, j, k;
  static real *temp = NULL;
  static int npoints = -1, npoints2;
  char *funcname;

  funcname = "fft_pow";

  if (npoints != (0x1<<log2length)) {
     if (temp) {
	free(temp);
	temp = NULL;
     }
  }
  if(temp == (real*) NULL) {
    npoints  = 0x1<<log2length;
    npoints2 = npoints / 2;
    temp = (real*) malloc(npoints * sizeof(real));
    if(temp == (real*) NULL) {
      fprintf(stderr, "Error mallocing memory in fft_pow()\n");
      exit(1);
    }
  }
  for(i=0;i<winlength;i++)
    temp[i] = (real) orig[i];
  for(i = winlength; i < npoints; i++)
    temp[i] = 0.0;


  if(FAST(temp, npoints) == 0 ){
    fprintf(stderr,"Error calculating fft.\n");
    exit(1);
  }

  /* convert the complex data to power */
  power[0] = temp[0]*temp[0];
  power[npoints2] = temp[1]*temp[1];
  /*
    Only the first half of the power[] array is filled with data.
     The second half would just be a mirror image of the first half.
*/
  for(i=1;i<npoints2;i++){
    j=2*i;
    k=2*i+1;
    power[i] = temp[j]*temp[j]+temp[k]*temp[k];
  }
  return(0);
}

/* ------------------------------------------------------------------------
     Method conceived by Yue Pan (?)
     Introduced by Florian Metze
     Useful for D&S Beamforming
   ------------------------------------------------------------------------ */

int fft_pow2(float *orig, float *power, long winlength, long log2length) {
  int i, j, k;
  static real *temp = NULL;
  static int npoints = -1, npoints2;
  char *funcname;

  funcname = "fft_pow2";

  if (npoints != (0x1<<log2length)) {
     if (temp) {
        free(temp);
        temp = NULL;
     }
  }
  if(temp == (real*) NULL) {
    npoints  = 0x1<<log2length;
    npoints2 = npoints / 2;
    temp = (real*) malloc(npoints * sizeof(real));
    if(temp == (real*) NULL) {
      fprintf(stderr, "Error mallocing memory in fft_pow()\n");
      exit(1);
    }
  }
  for(i=0;i<winlength;i++)
    temp[i] = (real) orig[i];
  for(i = winlength; i < npoints; i++)
    temp[i] = 0.0;


  if(FAST(temp, npoints) == 0 ){
    fprintf(stderr,"Error calculating fft.\n");
    exit(1);
  }
  /* convert the complex data to power */
  power[0] = temp[0]*temp[0];
  power[npoints2] = temp[1]*temp[1];
  /*
    Only the first half of the power[] array is filled with data.
     The second half would just be a mirror image of the first half.
  */
  for(i=0;i<npoints2;i++){
    j=2*i;
    k=2*i+1;
    power[j] = temp[j];
    power[k] = temp[k];
  }
  return(0);
}

/*
** FAST(b,n)
** This routine replaces the real float vector b
** of length n with its finite discrete fourier transform.
** DC term is returned in b[0]; 
** n/2th harmonic real part in b[1].
** jth harmonic is returned as complex number stored as
** b[2*j] + i b[2*j + 1] 
** (i.e., remaining coefficients are as a DPCOMPLEX vector).
** 
*/
int FAST(real *b, int n) {
  real fn;
  int i, in, nn, n2pow, n4pow, nthpo;
  
  n2pow = fastlog2(n);
  if(n2pow <= 0) return 0;
  nthpo = n;
  fn = nthpo;
  n4pow = n2pow / 2;

  /* radix 2 iteration required; do it now */  
  if(n2pow % 2) {
    nn = 2;
    in = n / nn;
    FR2TR(in, b, b + in);
  }
  else nn = 1;

  /* perform radix 4 iterations */
  for(i = 1; i <= n4pow; i++) {
    nn *= 4;
    in = n / nn;
    FR4TR(in, nn, b, b + in, b + 2 * in, b + 3 * in);
  }

  /* perform inplace reordering */
  FORD1(n2pow, b);
  FORD2(n2pow, b);

  /* take conjugates */
  for(i = 3; i < n; i += 2) b[i] = -b[i];

  return 1;
}

/* radix 2 subroutine */
void FR2TR(int in, real *b0, real *b1) {
  int k;
  real t;
  for(k = 0; k < in; k++) {
    t = b0[k] + b1[k];
    b1[k] = b0[k] - b1[k];
    b0[k] = t;
  }
}

/* radix 4 subroutine */
void FR4TR(int in, int nn, real *b0, real *b1, real *b2, real* b3) {
  real arg, piovn, th2;
  real *b4 = b0, *b5 = b1, *b6 = b2, *b7 = b3;
  real t0, t1, t2, t3, t4, t5, t6, t7;
  real r1, r5, pr, pi;
  real c1, c2, c3, s1, s2, s3;
  
  int j, k, jj, kk, jthet, jlast, ji, jl, jr, int4;
  int L[16], L1, L2, L3, L4, L5, L6, L7, L8, L9, L10, L11, L12, L13, L14, L15;
  int j0, j1, j2, j3, j4, j5, j6, j7, j8, j9, j10, j11, j12, j13, j14;
  int k0, kl;
  
  L[1] = nn / 4;	
  for(k = 2; k < 16; k++) {  /* set up L's */
    switch (signum(L[k-1] - 2)) {
    case -1:
      L[k-1]=2;
    case 0:
      L[k]=2;
      break;
    case 1:
      L[k]=L[k-1]/2;
    }
  }

  L15=L[1]; L14=L[2]; L13=L[3]; L12=L[4]; L11=L[5]; L10=L[6]; L9=L[7];
  L8=L[8];  L7=L[9];  L6=L[10]; L5=L[11]; L4=L[12]; L3=L[13]; L2=L[14];
  L1=L[15];

  piovn = PI / nn;
  ji=3;
  jl=2;
  jr=2;
  
  for(j1=2;j1<=L1;j1+=2)
  for(j2=j1;j2<=L2;j2+=L1)
  for(j3=j2;j3<=L3;j3+=L2)
  for(j4=j3;j4<=L4;j4+=L3)
  for(j5=j4;j5<=L5;j5+=L4)
  for(j6=j5;j6<=L6;j6+=L5)
  for(j7=j6;j7<=L7;j7+=L6)
  for(j8=j7;j8<=L8;j8+=L7)
  for(j9=j8;j9<=L9;j9+=L8)
  for(j10=j9;j10<=L10;j10+=L9)
  for(j11=j10;j11<=L11;j11+=L10)
  for(j12=j11;j12<=L12;j12+=L11)
  for(j13=j12;j13<=L13;j13+=L12)
  for(j14=j13;j14<=L14;j14+=L13)
  for(jthet=j14;jthet<=L15;jthet+=L14) 
    {
      th2 = jthet - 2;
      if(th2<=0.0) 
	{
	  for(k=0;k<in;k++) 
	    {
	      t0 = b0[k] + b2[k];
	      t1 = b1[k] + b3[k];
	      b2[k] = b0[k] - b2[k];
	      b3[k] = b1[k] - b3[k];
	      b0[k] = t0 + t1;
	      b1[k] = t0 - t1;
	    }
	  if(nn-4>0) 
	    {
	      k0 = in*4 + 1;
	      kl = k0 + in - 1;
	      for (k=k0;k<=kl;k++) 
		{
		  kk = k-1;
		  pr = IRT2 * (b1[kk]-b3[kk]);
		  pi = IRT2 * (b1[kk]+b3[kk]);
		  b3[kk] = b2[kk] + pi;
		  b1[kk] = pi - b2[kk];
		  b2[kk] = b0[kk] - pr;
		  b0[kk] = b0[kk] + pr;
		}
	    }
	}
      else 
	{
	  arg = th2*piovn;
	  c1 = cos(arg);
	  s1 = sin(arg);
	  c2 = c1*c1 - s1*s1;
	  s2 = c1*s1 + c1*s1;
	  c3 = c1*c2 - s1*s2;
	  s3 = c2*s1 + s2*c1;

	  int4 = in*4;
	  j0=jr*int4 + 1;
	  k0=ji*int4 + 1;
	  jlast = j0+in-1;
	  for(j=j0;j<=jlast;j++) 
	    {
	      k = k0 + j - j0;
	      kk = k-1; jj = j-1;
	      r1 = b1[jj]*c1 - b5[kk]*s1;
	      r5 = b1[jj]*s1 + b5[kk]*c1;
	      t2 = b2[jj]*c2 - b6[kk]*s2;
	      t6 = b2[jj]*s2 + b6[kk]*c2;
	      t3 = b3[jj]*c3 - b7[kk]*s3;
	      t7 = b3[jj]*s3 + b7[kk]*c3;
	      t0 = b0[jj] + t2;
	      t4 = b4[kk] + t6;
	      t2 = b0[jj] - t2;
	      t6 = b4[kk] - t6;
	      t1 = r1 + t3;
	      t5 = r5 + t7;
	      t3 = r1 - t3;
	      t7 = r5 - t7;
	      b0[jj] = t0 + t1;
	      b7[kk] = t4 + t5;
	      b6[kk] = t0 - t1;
	      b1[jj] = t5 - t4;
	      b2[jj] = t2 - t7;
	      b5[kk] = t6 + t3;
	      b4[kk] = t2 + t7;
	      b3[jj] = t3 - t6;
	    }
	  jr += 2;
	  ji -= 2;
	  if(ji-jl <= 0) {
	    ji = 2*jr - 1;
	    jl = jr;
	  }
	}
    }
}

/* an inplace reordering subroutine */
void FORD1(int m, real *b) {
  int j, k = 4, kl = 2, n = 0x1 << m;
  real t;
  
  for(j = 4; j <= n; j += 2) {
    if(k - j>0) {
      t = b[j-1];
      b[j - 1] = b[k - 1];
      b[k - 1] = t;
    }
    k -= 2;
    if(k - kl <= 0) {
      k = 2*j;
      kl = j;
    }
  }	
}

/*  the other inplace reordering subroutine */
void FORD2(int m, real *b) {
  real t;
  
  int n = 0x1<<m, k, ij, ji, ij1, ji1;
  
  int l[16], l1, l2, l3, l4, l5, l6, l7, l8, l9, l10, l11, l12, l13, l14, l15;
  int j1, j2, j3, j4, j5, j6, j7, j8, j9, j10, j11, j12, j13, j14;
  

  l[1] = n;
  for(k=2;k<=m;k++) l[k]=l[k-1]/2;
  for(k=m;k<=14;k++) l[k+1]=2;
  
  l15=l[1];l14=l[2];l13=l[3];l12=l[4];l11=l[5];l10=l[6];l9=l[7];
  l8=l[8];l7=l[9];l6=l[10];l5=l[11];l4=l[12];l3=l[13];l2=l[14];l1=l[15];

  ij = 2;
  
  for(j1=2;j1<=l1;j1+=2)
  for(j2=j1;j2<=l2;j2+=l1)
  for(j3=j2;j3<=l3;j3+=l2)
  for(j4=j3;j4<=l4;j4+=l3)
  for(j5=j4;j5<=l5;j5+=l4)
  for(j6=j5;j6<=l6;j6+=l5)
  for(j7=j6;j7<=l7;j7+=l6)
  for(j8=j7;j8<=l8;j8+=l7)
  for(j9=j8;j9<=l9;j9+=l8)
  for(j10=j9;j10<=l10;j10+=l9)
  for(j11=j10;j11<=l11;j11+=l10)
  for(j12=j11;j12<=l12;j12+=l11)
  for(j13=j12;j13<=l13;j13+=l12)
  for(j14=j13;j14<=l14;j14+=l13)
  for(ji=j14;ji<=l15;ji+=l14) {
    ij1 = ij-1; ji1 = ji - 1;
    if(ij-ji<0) {
      t = b[ij1-1];
      b[ij1-1]=b[ji1-1];
      b[ji1-1] = t;
	
      t = b[ij1];
      b[ij1]=b[ji1];
      b[ji1] = t;
    }
    ij += 2;
  }
} 

int fastlog2(int n) {
  int num_bits, power = 0;

  if((n < 2) || (n % 2 != 0)) return(0);
  num_bits = sizeof(int) * 8;   /* How big are ints on this machine? */

  while(power <= num_bits) {
    n >>= 1;
    power += 1;
    if(n & 0x01) {
      if(n > 1)	return(0);
      else return(power);
    }
  }
  return(0);
}


/* =========================================================================

   A complex FFT-routine

   ========================================================================= */

/* -------------------------------------------------------------------------
   maPh2real
   convert a symmetric, complex spectrum (N/2+1) coefficients into a the
   time domain (N real coefficients)
   ------------------------------------------------------------------------- */
void maPh2real(float* real, int N, float* bt, float* ph) {
  complex *x = (complex*)malloc(N*sizeof(complex));
  int i=0;

  /* fill the complex vector with the symmetric spectrum */
  x[i].r = bt[i]*cos(ph[i]); x[i].i = bt[i]*sin(ph[i]);
  for (i=1;i<N/2;i++) {
    x[i].r = bt[i]*cos(ph[i]); x[N-i].r = x[i].r; 
    x[i].i = bt[i]*sin(ph[i]); x[N-i].i = -x[i].i;
  }
  x[i].r = bt[i]*cos(ph[i]); x[i].i = bt[i]*sin(ph[i]);
  /* for (i=0;i<N;i++) printf("%d: % 12f % f\n",i,x[i].r, x[i].i); */

  /* IFFT */
  cfft(x,N,1);

  for (i=0;i<N;i++) real[i] = x[i].r; 
  /*  for (i=0;i<N;i++) printf("%d: % 12f % f\n",i,x[i].r, x[i].i); */
  free(x);
}

/* -------------------------------------------------------------------------
   FFT-Code uebernommen von Soenke Carstens-Behrens (Halka, Carl, Gluth) 
   ------------------------------------------------------------------------- */
#define CMUL(a,b,res) res.r = (a).r * (b).r - (a).i * (b).i; \
                      res.i = (a).r * (b).i + (a).i * (b).r;
#define CMULEQ(a,b)   aux = (a).r; \
                      (a).r = aux * (b).r - (a).i * (b).i; \
                      (a).i = aux * (b).i + (a).i * (b).r



void cfft(complex *x_com, int n, int inv) {
  int      i, j, k, test, rev, step, end;
  complex  temp, w0;
  REAL     aux;
  int      m;
  static complex  w[20] = {
{-1.000000000000000000000000000000e+00,  0.000000000000000000000000000000e-00},
{-0.000000000000000000000000000000e-00, -1.000000000000000000000000000000e+00},
{ 7.071067811865475727373109293694e-01, -7.071067811865474617150084668538e-01},
{ 9.238795325112867384831361050601e-01, -3.826834323650897817792326804920e-01},
{ 9.807852804032304305792422383092e-01, -1.950903220161282480837883213098e-01},
{ 9.951847266721969287317506314139e-01, -9.801714032956060362877792613290e-02},
{ 9.987954562051724050064649418346e-01, -4.906767432741801493456534899451e-02},
{ 9.996988186962042499672520534659e-01, -2.454122852291228812360301958506e-02},
{ 9.999247018391445029905639785284e-01, -1.227153828571992538742918554817e-02},
{ 9.999811752826011090888869148330e-01, -6.135884649154475269094977107898e-03},
{ 9.999952938095761911796444110223e-01, -3.067956762965976143242574636361e-03},
{ 9.999988234517018792502085489105e-01, -1.533980186284765500140392369133e-03},
{ 9.999997058628822266257429873804e-01, -7.669903187427044854995727973801e-04},
{ 9.999999264657178921211766464694e-01, -3.834951875713955632071772150482e-04},
{ 9.999999816164293342524160834728e-01, -1.917475973107032915284525520505e-04},
{ 9.999999954041073335631040208682e-01, -9.587379909597734466922963614266e-05},
{ 9.999999988510268611463516208460e-01, -4.793689960306688130960844906880e-05},
{ 9.999999997127566597754366739537e-01, -2.396844980841821931777727017199e-05},
{ 9.999999999281892204550103997462e-01, -1.198422490506970529769437128209e-05},
{ 9.999999999820472496026013686787e-01, -5.992112452642427527214376292708e-06}};
  static complex  v[20] = {
{-1.000000000000000000000000000000e+00,  0.000000000000000000000000000000e-00},
{-0.000000000000000000000000000000e-00,  1.000000000000000000000000000000e+00},
{ 7.071067811865475727373109293694e-01,  7.071067811865474617150084668538e-01},
{ 9.238795325112867384831361050601e-01,  3.826834323650897817792326804920e-01},
{ 9.807852804032304305792422383092e-01,  1.950903220161282480837883213098e-01},
{ 9.951847266721969287317506314139e-01,  9.801714032956060362877792613290e-02},
{ 9.987954562051724050064649418346e-01,  4.906767432741801493456534899451e-02},
{ 9.996988186962042499672520534659e-01,  2.454122852291228812360301958506e-02},
{ 9.999247018391445029905639785284e-01,  1.227153828571992538742918554817e-02},
{ 9.999811752826011090888869148330e-01,  6.135884649154475269094977107898e-03},
{ 9.999952938095761911796444110223e-01,  3.067956762965976143242574636361e-03},
{ 9.999988234517018792502085489105e-01,  1.533980186284765500140392369133e-03},
{ 9.999997058628822266257429873804e-01,  7.669903187427044854995727973801e-04},
{ 9.999999264657178921211766464694e-01,  3.834951875713955632071772150482e-04},
{ 9.999999816164293342524160834728e-01,  1.917475973107032915284525520505e-04},
{ 9.999999954041073335631040208682e-01,  9.587379909597734466922963614266e-05},
{ 9.999999988510268611463516208460e-01,  4.793689960306688130960844906880e-05},
{ 9.999999997127566597754366739537e-01,  2.396844980841821931777727017199e-05},
{ 9.999999999281892204550103997462e-01,  1.198422490506970529769437128209e-05},
{ 9.999999999820472496026013686787e-01,  5.992112452642427527214376292708e-06}};

  /* --- sorting the input vector */
  /********************************/
  rev = 1;
  for (i=1; i<n; i++) {
    if (i < rev) {
      temp.r = x_com[rev-1].r;
      temp.i = x_com[rev-1].i;
      x_com[rev-1].r = x_com[i-1].r;
      x_com[rev-1].i = x_com[i-1].i;
      x_com[i-1].r = temp.r;
      x_com[i-1].i = temp.i;
    }
    test = n / 2;
    while (test < rev) {
      rev -= test;
      test /= 2;
    }
    rev += test;
  }
  m = (int) (log(n+.1)/log(2.));
  /* --- FFT algorithm - butterfly 1 */
  /***********************************/
  for (i=1; i<=n; i+=2) {
    temp.r = x_com[i].r;
    temp.i = x_com[i].i;
    x_com[i].r = x_com[i-1].r - temp.r;
    x_com[i].i = x_com[i-1].i - temp.i;
    x_com[i-1].r += temp.r;
    x_com[i-1].i += temp.i;
  }

  /* --- butterflies 2 through m */
  /*******************************/
  end = 1;
  for (i=1; i<m; i++) {
    end *= 2;
    step = 2 * end;
    w0.r = 1.;
    w0.i = 0.;
    for (j=1; j<=end; j++) {
      for (k=j; k<=n; k+=step) {
	/* westphal: assert not needed but compensates for a sun cc optimizer bug ?? */
	assert(k+end-1>=0);assert(k+end-1<n);
	assert(k-1>=0);assert(k-1<n);
        CMUL(x_com[k+end-1], w0, temp);
        x_com[k+end-1].r = x_com[k-1].r - temp.r;
        x_com[k+end-1].i = x_com[k-1].i - temp.i;
        x_com[k-1].r += temp.r;
        x_com[k-1].i += temp.i;
      }
      if (inv) { CMULEQ(w0, v[i]); }
      else     { CMULEQ(w0, w[i]); }
    }
  }
  if (inv){
    for (i=0; i<n; ++i) {
      x_com[i].r /= n;
      x_com[i].i /= n;
    }
  }
}
