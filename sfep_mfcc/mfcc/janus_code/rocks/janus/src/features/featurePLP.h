/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Feature Module
               ------------------------------------------------------------

    Author  :  Martin Westphal
    Module  :  featurePLP.h
    Date    :  $Id: featurePLP.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 3.3  2000/08/27 15:31:14  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.2.4.1  2000/08/25 22:19:43  jmcd
    Just checking.

    Revision 3.2  1997/07/09 17:42:22  westphal
    *** empty log message ***


   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif
/* rasta.h */

#define two_to_the(N) (int)(pow(2.0,(N))+0.5)
#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/* 	Constant values 	*/

#define SPEECHBUFSIZE 512
	/* This is only used to buffer input samples, and
	is noncritical (in other words, doesn't need to be changed
	when you change sample rate, window size, etc.) */

#define COMPRESS_EXP .33
		/* Cube root compression of spectrum is used for
	intensity - loudness conversion */

#define LOG_BASE 10. 
		/* Used for dB-related stuff */

#define TINY 1.0e-45

#define MAXHIST 25

#define TYP_WIN_SIZE 20

#define TYP_STEP_SIZE 10

#define PHONE_SAMP_FREQ 8000

#define TYP_SAMP_FREQ 16000

#define TYP_ENHANCE 0.6

/* #define TYP_MODEL_ORDER 8 */
#define TYP_MODEL_ORDER 20

#define NOTHING_FLOAT 0.0

#define NOTHING 0

#define ONE	1.0

#define NOT_SET 0

#define JAH_DEFAULT 1.0e-6

#define POLE_DEFAULT .94

#define HAMMING_COEF .54

#define HANNING_COEF .50

#define RECT_COEF	1.0

/* Limits for parameter checking  */

#define MINFILTS 1

#define MAXFILTS 1000

#define MIN_NFEATS 1 

#define MAX_NFEATS 100 

/* window and step sizes in milliseconds */
#define MIN_WINSIZE 1.0

#define MAX_WINSIZE 1000.

#define MIN_STEPSIZE 1.0

#define MAX_STEPSIZE 1000.

#define MIN_SAMPFREQ 1000

#define MAX_SAMPFREQ 50000

#define MIN_POLEPOS 0.0

#define MAX_POLEPOS 1.0

#define MIN_ORDER 1

#define MAX_ORDER 100

#define MIN_LIFT .01

#define MAX_LIFT 100.

#define MIN_WINCO HANNING_COEF

#define MAX_WINCO RECT_COEF

#define MIN_RFRAC 0.0

#define MAX_RFRAC 1.0

#define MIN_JAH 1.0e-16

#define MAX_JAH 1.0e16

/*                  		*/
/*     	Structures		*/

struct fvec{
	float *values;
	int length;

	float *energy;
	float mean;
	float max;
	float min;
	};

struct fmat{
        float **values;
        int nrows;
        int ncols;
        };

struct svec{
	short *values;
	int length;

	int *energy;
	float mean;
	short max;
	short min;
	};


struct range{
	int start;
	int end;
	};

struct param{
	float winsize;    /* analysis window size in msec                       */
	int winpts;     /* analysis window size in points                     */
	float stepsize;   /* analysis step size in msec                         */
	int steppts;    /* analysis step size in points                       */
	int sampfreq;   /* sampling frequency in Hertz                        */
	int nframes;    /* number of analysis frames in speech                */
	float nyqbar;	/* Nyquist frequency  barks			      */
	int nfilts;	/* number of critical band filters used               */
	int first_good;	/* number of critical band filters to ignore at start
				and end (computed)               */
	float winco;    /* window coefficient	                      */
	float polepos;  /* rasta integrator pole position                     */
	int order;      /* LPC model order                                    */
	int nout;       /* length of final feature vector                     */
	int gainflag;   /* flag that says to use gain                         */
	float lift;     /* peak enhancement factor (cepstral lifter exponent) */
	int lrasta;     /* set true if log rasta used                         */
	int jrasta;     /* set true if jah rasta used                         */
	int prasta;     /* set true if mixture of plp and rasta used          */
	float rfrac;    /* fraction of rasta mixed with plp                   */
	float jah;      /* Jah constant                                       */
	char *infname;  /* Input file name, where "-" means stdin             */
	char *num_fname;  /* RASTA Numerator polynomial file name   */
	char *denom_fname;/* RASTA Denominator polynomial file name */
	char *outfname; /* Output file name, where "-" means stdout           */
	int ascin;	/* if true, read ascii in                             */
	int ascout;	/* if true, write ascii out                           */
	int espsin;	/* if true, read esps                       */
	int espsout;	/* if true, write esps                       */
	int debug;	/* if true, enable debug info 	                      */
	int smallmask;	/* if true, add small constant to power spectrum      */
	int online;     /* if true, online, not batch file processing */
	};

/* Function prototypes for Rasta 1.0 */

	/* Basic analysis routines */
	struct fvec *get_win( struct param *,int );
	struct fvec *powspec( const struct param *, struct fvec *);
	struct fvec *audspec( const struct param *, struct fvec *);
	struct fvec *nl_audspec( const struct param *, struct fvec *);
	struct fvec *rasta_filt( const struct param *, struct fvec *);
		struct fvec *get_delta( int );
		struct fvec *get_integ( const struct param * );
	struct fvec *inverse_nonlin(const struct param *, struct fvec *);
	struct fvec *post_audspec( const struct param *, struct fvec *);
	struct fvec *lpccep( const struct param *, struct fvec *);
		void auto_to_lpc( const struct param *pptr, struct fvec *, 
			struct fvec *, float * );
		void lpc_to_cep( const struct param *pptr, struct fvec *, 
			struct fvec * );

	/* Matrix - vector arithmetic */
	void fmat_x_fvec( struct fmat *, struct fvec *, struct fvec *);
	void norm_fvec( struct fvec *, float );
	

	/* Allocation and checking */
	void         free_fvec(  struct fvec*);
	void         free_fmat(  struct fmat*);
	struct fvec *alloc_fvec( int );
        struct svec *alloc_svec( int );
	struct fmat *alloc_fmat( int, int );
	void fvec_check( char *, struct fvec *, int );

	/* I/O */
	struct fvec *get_bindata( FILE *, struct fvec *);
	struct fvec *get_ascdata( FILE *, struct fvec *);
	void print_vec( FILE *, struct fvec *, int );
	void binout_vec( FILE *, struct fvec *);
	FILE *open_out(struct param *);
	void write_out( struct param *, FILE *, struct fvec *);


	/* debugging aids */
	void show_args( struct param * );
	void show_param(struct fvec *, struct param * );
	void show_vec( struct fvec * );
	void usage( char *);



#ifdef __cplusplus
}
#endif
