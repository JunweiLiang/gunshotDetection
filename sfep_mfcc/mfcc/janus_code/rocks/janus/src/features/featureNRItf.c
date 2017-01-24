/* ========================================================================
   featureNRItf.c

   Methods for noise reduction after Ephraim & Malah used in the 
   'Studienarbeit' of Max Ritter
   and spectral subtraction after S. Boll.

   first version: 31 October 1997
   this  version: 21 January 1998
   ======================================================================== */


/* ========================================================================
    includes
   ======================================================================== */

#include "featureMethodItf.h"           /* includes: Audio, PLP, IO         */


/* ========================================================================
    macros
   ======================================================================== */

#define for_current_frame   for (coeffX = 0; coeffX < coeffN; coeffX++)
#define DEBUG_PRINT    if (debug == 1) fprintf

/* ========================================================================
   Method 'specsub'
   spectral subtraction
   ======================================================================== */
int fesSpecSubItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin  = NULL;
  Feature     *source1 = NULL;
  Feature     *source2 = NULL;
  float       a = 1.5, b = .01;
    
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
	"<new feature>",  ARGV_CUSTOM, createFe,&destin,  cd,
		          "spectral subtraction after Boll with estimated noise",
	"<featureA>", ARGV_CUSTOM, getFe,   &source1,     cd, "spectral feature",
	"<featureB>", ARGV_CUSTOM, getFe,   &source2,     cd, "estimated noise",
	"-a",         ARGV_FLOAT,  NULL,    &a,        cd, "overestimation factor alpha",
	"-b",         ARGV_FLOAT,  NULL,    &b,        cd, "spectral floor beta",
	NULL) != TCL_OK) return TCL_ERROR;

  /* -------------------
     FMatrix features?
     ------------------- */
  if (BOTH_FMATRIX( source1, source2)) {
     FMatrix *mat1 = source1->data.fmat;
     FMatrix *mat2 = source2->data.fmat;
     FMatrix *mat;
     int frameA = 0;
     int frameX;
     int frameN  = mat1->m;
     int frameN2 = mat2->m;
     int coeffX;
     int coeffN  = mat1->n;
     int coeffN2 = mat2->n;
     float floor;

     if (FS->runon && !FS->ready) {
       frameN = frameN <= frameN2 ? frameN : frameN2; 
     } else {
       if (frameN != frameN2) {
	 ERROR("frame number of features differ: '%s' (%d), '%s' (%d)\n",source1->name, frameN, source2->name, frameN2);
	 return TCL_ERROR;
       }
     }
     if (coeffN != coeffN2) {
       ERROR("coeffients number of features differ: '%s' (%d), '%s' (%d)\n",source1->name, coeffN, source2->name, coeffN2);
       return TCL_ERROR;
     }

     mat = fmatrixFeature( frameN, coeffN, FS, destin, &frameA);
     if (!mat) return TCL_ERROR;
          
     foreach_coeff {
        mat->matPA[frameX][coeffX] = mat1->matPA[frameX][coeffX] - a * mat2->matPA[frameX][coeffX];
	floor = b * mat1->matPA[frameX][coeffX];
	if (mat->matPA[frameX][coeffX] < floor) mat->matPA[frameX][coeffX] = floor;
     }

     if (!feFMatrix( FS, destin, mat, 1)) {
        COPY_CONFIG( destin, source1);  return TCL_OK;
     }
     fmatrixFree(mat); return TCL_ERROR;
  }
  MUST_FMATRIX2(source1,source2);
  return TCL_ERROR;
}
/* ========================================================================
   Method 'noiseest'
   estimates the noise in a given signal
   ======================================================================== */

/* ------------------------------------------------------------------------
   Description of the variables (common var. and var. for minimum statistic):

   destin              pointer to the destination feature
   source              pointer to the source feature
   mat1                pointer to the FMatrix of source
   noise               pointer to the matrix to contain the estimated noise

   frameA              the first frame to be processed
   frameN              the number of frames
   frameX              the current frame
   coeffN              the number of coefficients per fram
   coeffX              the current coefficient

   debug               0: no debugging output    1: output for debugging
   q                   scale factor for debugging output
   i                   help variable

   method              parameter to choose method for noise estimation
   alpha               memory factor for smoothing the spectrum
   overEstimation      over-estimation factor
   windowLength        the window length in frames
   time                time for minimum statistic in seconds
   samplingRate        samplingRate of the source feature
   transLength         (Fourier) transformation window length
                       (frame length in samples)
   overlap             overlapping of the frames in samples

   nrOfBestMins        number of the best minimums to be recorded
   bestMinsIndex       index for the array bestMins
   rapidVar            1 if the noise power varies rapidly, 0 otherwise
   rapidVarSave        saves rapidVar of coefficient 1 for debugging output
   x1, x2              help variables

   specSmooth          smoothed spectrum of the signal (with memory factor alpha)
   currentMin          minimum for the current window
   longTimeMin         minimum for the last nrOfBestMins windows
   bestMins            array for recording the best minimums
   ------------------------------------------------------------------------ */

int fesNoiseEstimationItf( ClientData cd, int argc, char *argv[]) {

  FeatureSet* FS = (FeatureSet*)cd;
  Feature*    destin = NULL;
  Feature*    source = NULL;
  char*       method = "MinStat";
  double      time = 0.625;          /* ***** optfilt: 0.8 */
  int         nrOfBestMins = 4;
  double      alpha   = 0.75;        /* ***** optfilt: 0.93 */
  double      overEstimation = 1.5;  /* ***** optfilt: 2 */
  int         debug = 0;
  int         i;

  /* -------------
     get arguments
     ------------- */

  argc--;

  if (itfParseArgv( argv[0], &argc, argv+1, 0,
		    "<feature>",        ARGV_CUSTOM, createFe,&destin,   cd, NEW_FEAT,
		    "<source_feature>", ARGV_CUSTOM, getFe,   &source,   cd, SRC_FEAT,
		    "-method",          ARGV_STRING, NULL,    &method,   cd, "method for noise estimation",
		    "-time",            ARGV_DOUBLE, NULL,    &time,     cd, "time length of the window in seconds",
		    "-nrOfBestMins",    ARGV_INT,    NULL, &nrOfBestMins,cd, "number of mini-windows",
		    "-alpha",           ARGV_DOUBLE, NULL,    &alpha,    cd, "memory factor for minimum statistic",
		    "-overEstimation",  ARGV_DOUBLE, NULL,    &overEstimation,    cd, "over-estimation factor for minimum statistic",
		    "-debug",           ARGV_INT,    NULL,    &debug,    cd, "0: no debugging output, 1: print debugging output",
		    NULL) != TCL_OK)
    return TCL_ERROR;

  /* --------------
     test arguments
     -------------- */

   if (!IS_FMATRIX(source)) {

     MUST_FMATRIX(source);
     free(destin);
     return TCL_ERROR;
   }
   else {
    
     /* ------------------------------
	arguments are okay; initialize
	------------------------------ */

     FMatrix      *mat1     = source->data.fmat;

     int q = 1e1;           /* scale factor for debugging output */

     int         frameA = 0;
     int frameX, frameN = mat1->m;
     int coeffX, coeffN = mat1->n;

     short   windowLength;
     float   samplingRate = source->samplingRate;
     int     transLength = 2 * (coeffN - 1);  /* ***** e.g. 256 */
     int     overlap = transLength - (source->shift * source->samplingRate);  /* ***** e.g. 96 = 256 - (10 ms * 16 kHz) */
     /* old (used for SA):
	int     overlap = transLength * (source->shift / source->samplingRate);
	*/
     int     rapidVar, x1, x2, rapidVarSave=0;
     int     bestMinsIndex;
     float** bestMins = (float**) calloc (nrOfBestMins, sizeof(float*));
     
     FVector *specSmooth    = fvectorCreate(coeffN);
     FVector *currentMin    = fvectorCreate(coeffN);
     FVector *longTimeMin   = fvectorCreate(coeffN);

     FMatrix *noise = fmatrixCreate( frameN, coeffN);
     if (!noise) {
       free(bestMins);
       return TCL_ERROR;
     }

     for (i=0; i < nrOfBestMins; i++)
       bestMins[i] = (float*) calloc (coeffN, sizeof(float));


     /* -----------------------------
	estimate noise for all frames
	----------------------------- */

     /* -----------------
	minimum statistic
	----------------- */

     if (strcmp(method, "MinStat") == 0) {
       
       /* ***** initialization */

       bestMinsIndex = 0;
       windowLength = (short) ((time * 1000 * samplingRate) - overlap)
	                      / (((transLength - overlap) * nrOfBestMins) + 0.5);
       frameX = frameA;
       
       for_current_frame {

	 specSmooth->vecA[coeffX]  = mat1->matPA[frameX][coeffX];
	 currentMin->vecA[coeffX]  = mat1->matPA[frameX][coeffX];
	 longTimeMin->vecA[coeffX] = mat1->matPA[frameX][coeffX];

	 for (i=0; i < nrOfBestMins; i++)
	   bestMins[i][coeffX] = mat1->matPA[frameX][coeffX];
       }

       /* ***** now we can start */

       if (debug == 1) {

	 DEBUG_PRINT(STDERR, "\n");
	 DEBUG_PRINT(STDERR, "Starting minimum statistic\n");
	 DEBUG_PRINT(STDERR, "windowLength = %d\n\n", windowLength);
	 DEBUG_PRINT(STDERR, "frame  spec ... smooth current longTimeMin noise\n");
	 DEBUG_PRINT(STDERR, "--------------------------------------------------\n");
       }
              
       foreach_frame {
	 
	 /* ***** this is done for every frame */

	 for_current_frame {

	   specSmooth->vecA[coeffX]  = (alpha * specSmooth->vecA[coeffX])
	                               + ((1 - alpha) * mat1->matPA[frameX][coeffX]);
	   currentMin->vecA[coeffX]  = MIN(currentMin->vecA[coeffX], specSmooth->vecA[coeffX]);
	 }

	 /* ***** this is only done for every window */

	 if ((frameX % windowLength) == 0) {

	   if (debug == 1)
	     DEBUG_PRINT(STDERR, "----------------------------------------------------------------------\n");

	   for_current_frame {

	     bestMins[bestMinsIndex][coeffX] = currentMin->vecA[coeffX];

	     /* ***** decide on slow or rapid varying noise power */
	     /* ***** i.e. test, whether bestMins is increasing */

	     rapidVar = 1;
	     i = bestMinsIndex + 1;

	     while (i != bestMinsIndex) {

	       if (i == nrOfBestMins) {i = 0;}

	       if (i == nrOfBestMins - 1) {
		 i = 0;
		 x1 = bestMins[nrOfBestMins - 1][coeffX];
		 x2 = bestMins[i][coeffX];
	       }
	       else {
		 x1 = bestMins[i][coeffX];
		 x2 = bestMins[i+1][coeffX];
		 i++;
	       }

	       if (x1 > x2) {rapidVar = 0;}
	     }

	     if (coeffX == 1) {rapidVarSave = rapidVar;}

	     /* ***** each coefficient of longTimeMin becomes the */
	     /* ***** minimum of the corresponding bestMins-coefficients */ 
	     /* ***** or the currentMin */

	     if (rapidVar ==1) {

	       longTimeMin->vecA[coeffX] = currentMin->vecA[coeffX];
	     }
	     else {
	       
	       longTimeMin->vecA[coeffX] = bestMins[0][coeffX];
	       
	       for (i=1; i < nrOfBestMins; i++)
		 longTimeMin->vecA[coeffX] = MIN(longTimeMin->vecA[coeffX], bestMins[i][coeffX]);
	     }

	     currentMin->vecA[coeffX] = specSmooth->vecA[coeffX];
	   }

	   bestMinsIndex++;
	   if (bestMinsIndex == nrOfBestMins) bestMinsIndex = 0;
	 }

	 /* ***** calculate the noise estimation */

	 for_current_frame
	   longTimeMin->vecA[coeffX] = MIN(longTimeMin->vecA[coeffX], specSmooth->vecA[coeffX]);

	 for_current_frame
	   noise->matPA[frameX][coeffX] = MIN(overEstimation * longTimeMin->vecA[coeffX], mat1->matPA[frameX][coeffX]);

	 /* ??? that's in the MinStat-paper
	    noise->matPA[frameX][coeffX] = specSmooth->vecA[coeffX]
	    * MIN(overEstimation * longTimeMin->vecA[coeffX],
	    specSmooth->vecA[coeffX])
	    / (overEstimation * longTimeMin->vecA[coeffX]);
	 */

	 if (debug == 1)
	   DEBUG_PRINT(STDERR, "%3d: %6.0f ... %6.0f %6.0f %6.0f %6.0f   %i\n", frameX, mat1->matPA[frameX][1]/q, specSmooth->vecA[1]/q, currentMin->vecA[1]/q, longTimeMin->vecA[1]/q, noise->matPA[frameX][1]/q, rapidVarSave);
	 
       } /* ***** end of foreach_frame */
     } /* ***** end of minimum statistic */

     /* ---------------------------------------
	copy results to the destination feature
	--------------------------------------- */
     
     if (debug == 1) {

       frameX--;
       /*
       for_current_frame
	 DEBUG_PRINT(STDERR, "%6.0f  ",noise->matPA[frameX][coeffX]/q);
	 */
       frameX++;
     }

     free(bestMins);

     if (!feFMatrix( FS, destin, noise, 1)) {
       
       destin->samplingRate = samplingRate;   /* ***** = source->samplingRate */
       destin->shift        = source->shift;
       return TCL_OK;
     }
     
     fmatrixFree(noise);
     return TCL_ERROR;

   } /* ***** end of the else case */
}


/* ========================================================================
   Method 'noisered'
   removes additive noise via the Ephraim-Mallah Suppresion Rule
   ======================================================================== */

/* ------------------------------------------------------------------------
   Description of the variables:

   destin      pointer to the destination feature
   source      pointer to the source feature
   noise       pointer to the feature that contains the estimated noise

   mat1        pointer to the FMatrix of source
   noiseMat    pointer to the FMatrix of noise
   mat         pointer to new FMatrix that shall be calculated

   frameA      the first frame to be processed
   frameN      the number of frames
   frameX      the current frame
   coeffN      the number of coefficients per fram
   coeffX      the current coefficient

   debug       0: no debugging output    1: output for debugging
   q           scale factor for debugging output

   alpha       weight for calculation of the a priori SNR
   Rprio_min   parameter for the residual noise level

   rprioI      name of the a priori SNR optional output feature
   rpostI      name of the a posteriori optional output feature
   rprioH      help text for the parameter -rprio
   rpostH      help text for the parameter -rpost
   RprioMat    a priori SNR optional output feature
   RpostMat    a posteriori optional output feature

   RprioVec    a priori SNR of the current frame
   RpostVec    a posteriori SNR of the current frame
   Rprio       help variable
   Rpost       help variable
   Rpost_min   boundary for Rpost, ensures, that H is valid
   LastOutVec  output of the last frame
   LastOut     help variable
   HVec        spectral weights
   H           help variable
   h           help variable for calculating H
   x           help variable for shorter access to the source signal
   y           help variable
   noisePower  help variable for shorter access to the estimated noise
   spectral_floor_factor   weight factor for spectral floor
   error       help variable
   ------------------------------------------------------------------------ */

int fesNoiseReductionItf( ClientData cd, int argc, char *argv[]) {

  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin = NULL;
  Feature     *source = NULL;
  Feature     *noise  = NULL;
  float       alpha = 0.98;
  double      Rprio_min = 0.0003;  /* take 0.03 for -15 dB (paper...) */
  int         rprioI = -1; char* rprioH = "feature with Rprio (in dB)";
  int         rpostI = -1; char* rpostH = "feature with Rpost (in dB)";
  int         debug = 0;
  
  /* -------------
     get arguments
     ------------- */

  argc--;

  if (itfParseArgv( argv[0], &argc, argv+1, 0,
		    "<feature>",        ARGV_CUSTOM, createFe, &destin,   cd, NEW_FEAT,
		    "<source_feature>", ARGV_CUSTOM, getFe,    &source,   cd, SRC_FEAT,
		    "<noise_feature>",  ARGV_CUSTOM, getFe,    &noise,    cd, "estimated noise",
		    "-alpha",           ARGV_FLOAT,  NULL,     &alpha,    cd, "weight for calculation of  the a priori SNR",
		    "-Rprio_min",       ARGV_DOUBLE, NULL, &Rprio_min,    cd, "min. value for Rprio to adjust residual noise level",
		    "-rprio",           ARGV_CUSTOM, createFeI,&rprioI,   cd, rprioH,
		    "-rpost",           ARGV_CUSTOM, createFeI,&rpostI,   cd, rpostH,
		    "-debug",           ARGV_INT,    NULL,     &debug,    cd, "0: no debugging output, 1: print debugging output",
		    NULL) != TCL_OK)
    return TCL_ERROR;

  /* --------------
     test arguments
     -------------- */

   if (!BOTH_FMATRIX(source, noise)) {

     MUST_FMATRIX2(source, noise);
     free(destin);
     return TCL_ERROR;
   }
   else {
    
     /* ------------------------------
	arguments are okay; initialize
	------------------------------ */

     FMatrix *mat1     = source->data.fmat;
     FMatrix *noiseMat = noise->data.fmat;

     FMatrix *mat      = NULL;
     FMatrix *rprioMat = NULL;
     FMatrix *rpostMat = NULL;

     int q     = 1e0; /* ***** scale factor for debugging output */
     int error = 0;

     int         frameA = 0;
     int frameX, frameN = mat1->m;
     int coeffX, coeffN = mat1->n;

     FVector *RprioVec   = fvectorCreate(coeffN);
     FVector *RpostVec   = fvectorCreate(coeffN);
     FVector *LastOutVec = fvectorCreate(coeffN);
     FVector *HVec       = fvectorCreate(coeffN);
     
     double Rpost;
     double Rpost_min = -1 + 1e-12;
     double Rprio;
     double LastOut;
     double H;
     double h;
     double x;
     double y;
     double noisePower;
     double spectral_floor_factor = 0.0;
     double emnlf(double x);

     if (rprioI >= 0) {rprioMat  = fmatrixCreate(frameN, coeffN);}
     if (rpostI >= 0) {rpostMat  = fmatrixCreate(frameN, coeffN);}

     mat = fmatrixCreate( frameN, coeffN);

     if (!mat) {
       if (rprioMat) fmatrixFree(rprioMat);
       if (rpostMat) fmatrixFree(rpostMat);
       return TCL_ERROR;
     }

     DEBUG_PRINT(STDERR, "\nframeX source  new noise     LastOut        Rpost    Rprio   H\n");
     DEBUG_PRINT(STDERR, "----------------------------------------------------------------\n");
     
     /* -------------------------------------
	process noise reduction on all frames
	------------------------------------- */

     foreach_frame {

       /* -------------------------
	  calculate Rpost and Rprio
	  ------------------------- */
       
       for_current_frame {

	 /* ***** x is the signal */
	 /* ***** noisePower is the square of the estimated noise */

	 x = mat1->matPA[frameX][coeffX];
	 noisePower = noiseMat->matPA[frameX][coeffX];
	 noisePower = noisePower * noisePower;
	 LastOut = LastOutVec->vecA[coeffX];
	 
	 Rpost = (x * x / noisePower) - 1.0;
	 Rpost = MAX(Rpost, Rpost_min);

	 Rprio = ((1 - alpha) * MAX(Rpost, 0)) + (alpha * (LastOut / noisePower));
	 Rprio = MAX(Rprio, Rprio_min);

	 RpostVec->vecA[coeffX] = Rpost;
	 RprioVec->vecA[coeffX] = Rprio;

	 if (rprioMat) {rprioMat->matPA[frameX][coeffX] = 10 * log10(1 + Rprio);}
	 if (rpostMat) {rpostMat->matPA[frameX][coeffX] = 10 * log10(1 + Rpost);}
       }

       /* ----------------------------
	  calculate spectral weights H
	  ---------------------------- */
       
       for_current_frame {
 
	 Rpost = RpostVec->vecA[coeffX];
	 Rprio = RprioVec->vecA[coeffX];

	 H = (sqrt(M_PI) / 2) * sqrt(Rprio / ((1 + Rprio) * (1 + Rpost)));
	 h = (1 + Rpost) * (Rprio / (1 + Rprio));
	 H = H * emnlf(h);

	 HVec->vecA[coeffX] = H;
       }

       /* ----------------------------------------
	  calculate output and power of the output
	  ---------------------------------------- */

       for_current_frame {

	 x = HVec->vecA[coeffX] * mat1->matPA[frameX][coeffX];
	 mat->matPA[frameX][coeffX] = x;
	 LastOutVec->vecA[coeffX] = x * x;
       }

       /* --------------
	  spectral floor
	  -------------- */

       for_current_frame {

	 x = mat->matPA[frameX][coeffX];
	 y = spectral_floor_factor * noiseMat->matPA[frameX][coeffX];
	 mat->matPA[frameX][coeffX] = MAX(x, y);
       }

       if (frameX < 30)
	 DEBUG_PRINT(STDERR, "%3d: %6.0f %6.0f %5.0f %11.0f ... %8.3f %8.3f %5.3f\n", frameX, mat1->matPA[frameX][1]/q, mat->matPA[frameX][1]/q, noiseMat->matPA[frameX][1]/q, LastOutVec->vecA[1]/(q*q), RpostVec->vecA[1], RprioVec->vecA[1], HVec->vecA[1]);
       
     } /* ***** end of frame processing */
       
     
     /* ----------------------------------------
	copy results to the destination features
	---------------------------------------- */
     
/* ---------------------------------------- */
#define DOem1( _MAT_ , _MATI_ ) \
     {\
	Feature* dest = FS->featA + _MATI_;\
     if ((_MAT_) && feFMatrix( FS, dest, _MAT_, 1)) {error = 1; fmatrixFree(_MAT_);}\
     else {CONFIG_FE(dest, source->samplingRate, source->shift);}\
     }
/* ---------------------------------------- */

     if (rprioI >= 0) DOem1(rprioMat,rprioI);
     if (rpostI >= 0) DOem1(rpostMat,rpostI);

     if ((error == 0) && (!feFMatrix( FS, destin, mat, 1))) {
       
       /* COPY_CONFIG( destin, source); */
       destin->samplingRate = source->samplingRate;
       destin->shift        = source->shift;
       return TCL_OK;
     }

     fmatrixFree(mat);
     if (rprioMat) fmatrixFree(rprioMat);
     if (rpostMat) fmatrixFree(rpostMat);
     return TCL_ERROR;
   }
}

/* ========================================================================
   emnlf (Ephraim-Mallah Non-Linear Function)
   ======================================================================== */

double emnlf(double x) {

  /* uebernommen von Soenke Carstens-Behrens
     (formeln.c) */

  /* Berechnung der Nichtlinearitaet 
 
   M(x) = exp(-x/2)*[(1+x)*I0(x/2) + x*I1(x/2)] 

   mit I0,I1 modifizierte Bessel-Funktionen 0. bzw. 
   1. Ordnung. 
 
   Diese Funktion wird zur Realisierung der Subtraktionregel
   im Spektralen Subtraktionsansatz nach Ephraim & Mallah benutzt.
 
   Referenz: Cappe. O.
           Elimination of the Musical Noise Phenomenon
	   with the Ephraim and Malah Noise Suppresor
	   IEEE Trans. on Speech and Audio Processing
	   Vol. 2 No2. 2 (April 1994), pp 345-350	 */


    /* Polynomapproximationen Nach Abramowitz & Stegun, Kap. 9.8 */

  static double p0[] = {.0045813, .0360768, .2659732, 1.2067492, 3.0899424,  
                        3.5156229, 1};
  static double p1[] = {.00032411, .00301532, .02658733, .15084934, .51498869,
                         .87890594, .5};
  static double q0[] = {.00392377,  -.01647633, .02635537,  -.02057706, .00916281,
                        -.00157565, .00225319, .01328592, .39894228};
  static double q1[] = {-.00420059, .01787654, -.02895312, .02282967, -.01031555, 
                         .00163801, -.00362018, -.03988024, .39894228}; 

  short i;
  double P0,P1,Q0,Q1,y;


  if ( (x/2.0) <= 3.75)
    {
    P0 = 0;
    for (i=0; i<7; ++i) 
       P0 = (x/7.5)*(x/7.5) * P0 +p0[i];

    P1 = 0;
    for (i=0; i<7; ++i) 
       P1 = (x/7.5)*(x/7.5) * P1 +p1[i];

    y = exp(-x/2.0) * (1+2*x/2.0) * P0 + 2 * exp(-x/2.0) * x*x/4 * P1; 
    }
  else
    {    
    Q0 = 0;
    for (i=0; i<9; ++i) 
       Q0 = (7.5/x) * Q0 +q0[i];

    Q1 = 0;
    for (i=0; i<9; ++i) 
       Q1 = (7.5/x) * Q1 +q1[i];

    y = Q0/sqrt(x/2.0) + 2*sqrt(x/2.0)*Q0 + 2*sqrt(x/2.0)*Q1; 
    }

  return y;
}
