/*#############################################################################################################################
 #
 #  FFT.h               This Module performs nothing but a FFT-transformation.
 #
 ##############################################################################################################################
 #
 # DESCRIPTION:
 #    Routines for performing an fft on adc data, creating Waibel-style mel-scale spectral coefficients (FFT file),
 #    and for reading and writing the mel-scale FFT files.
 #
 # ROUTINES:
 #
 #   int do_FFT( adcA, adc_valuesN, fftA, freq, msec, transformation, normalization, chop)
 #
 #   IN/OUT:
 #     short* 	adcA		array with ADC data
 #     int    	adc_valuesN	number of shorts in adcA
 #     float**	fftA		pointer to array of fft coefficients
 #     int 	freq		10 or 16 kHz. Only these two values are allowed
 #     int	msec		5 or 10 msec. Only these two values are allowed
 #     int	transformation  use fast HARTLEY transformation or fast FOURIER transformation?
 #     int	normalization	normalize fft data with the REGULAR method or FRAME_BY_FRAME
 #     int	chop		do begin/end boundary detection
 #
 #   RETURN:
 #		the number of FFT-coefficients is returned.
 #
 #
 #   int do_FFT2( adcA, adc_valuesN, fftA, freq, msec, transformation, normalization, chop, DSP_window_size)
 #
 #	Like do_FFT, but with one additional parameter:
 #
 #	int 	DSP_window_size		Size of DSP window. Currently restricted to 256 or 128 points.
 #
 #
 # HISTORY:
 #       1.June 93   Hermann   moved all local stuff into FFT.c
 #       3.Aug  93   Tilo      added/modified values and names for the parameters transformation and normalization 
 #                             added #ifdef ANSI_C, started RCS
 #       7.Aug 93  sloboda     changed #ifdef ANSI_C to #ifdef __STDC__
 #       1.Aug 94  Martin      added function prototypes
 #
 #== RCS INFO: ================================================================================================================
 #
 # $Id: featureFFT.h 2449 2004-05-10 14:16:30Z metze $
 #
 # $Log$
 # Revision 4.4  2004/05/10 14:16:30  metze
 # Yue's changes
 #
 # Revision 4.3  2001/01/15 09:49:56  janus
 # Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)
 #
 # Revision 4.2.36.1  2001/01/12 15:16:53  janus
 # necessary changes for running janus under WINDOWS
 #
 # Revision 4.2  2000/08/27 15:31:14  jmcd
 # Merging branch 'jtk-00-08-24-jmcd'.
 #
 # Revision 4.1.4.1  2000/08/25 22:18:18  jmcd
 # Added extern "C" statements to all header files; compiled main() in
 # C++ to ensure proper initialization of C++ libraries.
 #
 # Revision 4.1  2000/01/12 10:02:02  fuegen
 # make static functions not static
 #
 # Revision 4.0  1998/11/30 18:07:14  fuegen
 # *** empty log message ***
 #
 # Revision 3.3  1998/09/30 12:11:22  westphal
 # adc2spec spec2adc
 #
 # Revision 3.2  1997/07/09 17:42:22  westphal
 # *** empty log message ***
 #
 #
 #
 ############################################################################################################################*/
  
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _FFT_H_                  /* we don't want to include header files more than once */
#define _FFT_H_

#include "common.h"

/*-----------------------------------------------------------------------------------------
 | use HARTLEY or FOURIER for the parameter "transformation" when you call "do_FFT" !!
 +---------------------------------------------------------------------------------------*/
#define HARTLEY 1                /* this one is equivalent to the other, but much faster! (default) */
#define FOURIER 0

/*-----------------------------------------------------------------------------------------
 | use REGULAR or FRAME_BY_FRAME for the parameter "normalization" when you call "do_FFT" !!
 +---------------------------------------------------------------------------------------*/
#define REGULAR          1       /* this is the one which is usually used. (default) */
#define FRAME_BY_FRAME   0       /* this one is one of Arthurs tests to fo a frame by frame normalization,     */
                                 /* without having to look at the whole speech file first.                     */

int do_FFT ( short adcA[], int adc_valuesN, float **fftA, int freq, int msec, int transformation, int normalization, int chop);
int do_FFT2( short adcA[], int adc_valuesN, float **fftA, int freq, int msec, int transformation, int normalization, int chop,
			  int DSP_window_size);
int init_fht( int N );
void fht( int N, float *input, float *output );
int do_hamm_window( short inputA[], int pointsN, float resultA[] );
void fht_pow_spec( int N, float *in, float *power );
void adc_to_melscale_frame( short *adc_buf, float *fft_buf,  int freq , int DSP_window_size);
int compute_melscale_from_power( float *coeffA, int coeffN,  float *powerA, int powerN, int scale8);
int fft_pow(float *orig, float *power, long winlength, long log2length);
int fft_pow2(float *orig, float *power, long winlength, long log2length);
void fft (register float *real, register float *imag, int N, int mode);
int init_fft (int N);
void fft_pow_spec (int N, float *re, float *im, float *power);
int FAST (float *b, int n);
#endif /* _FFT_H_ */


/* -------------------------------------------------------------------------
   a complex FFT routine
   ------------------------------------------------------------------------- */
#define REAL float

#ifndef PI
#define PI 3.14159265358979323846
#endif

typedef struct
{
  REAL r;
  REAL i;
}complex;

void cfft(complex *x_com, int n, int inv);
void maPh2real(float* real, int N, float* bt, float* ph);

#ifdef __cplusplus
}
#endif
