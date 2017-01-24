/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Multi Layer Perceptron for Classification
               ------------------------------------------------------------

    Author  :  Juergen Fritsch
    Module  :  mlpClass.h
    Date    :  $Id: mlpClass.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 3.3  2000/08/27 15:31:25  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.2.4.1  2000/08/25 22:19:45  jmcd
    Just checking.

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.1  96/09/27  09:00:45  fritsch
 * Initial revision
 * 
    Revision 1.1  1995/12/21  13:49:57  fritsch
    Initial revision
 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _mlpClass
#define _mlpClass


/* ====================================================================== */
/*  MlpAccu     training accumulators                                     */
/* ====================================================================== */

typedef struct {

  int               classN;
  int               dimN;
  int               hiddenN;

  double            *pv;        /* backpropagation delta's (output layer) */
  double            **av;       /* accumulator for weight delta's */
  double            **dv;       /* actual weight delta's */
  
  double            *pw;        /* backpropagation delta's (hidden layer) */
  double            **aw;       /* accumulator for weight delta's */
  double            **dw;       /* actual weight delta's */

  int               trainN;
  
} MlpClassAccu;



/* ===================================================================== */
/*  Mlp        2-layer MLP                                               */
/* ===================================================================== */

typedef struct {

  int               classN;      /* number of output classes */
  int               dimN;        /* feature dimension */
  int               hiddenN;     /* number of hidden nodes */

  double            **w;         /* weight matrix input<->hidden layer */
  double            *act;        /* hidden node activations */
  double            **v;         /* weight matrix hidden<->output layer */

  MlpClassAccu      *accu;       /* pointer to training accus */
  
  int               trainMode;   /* training modus */
  int               weightMode;  /* weight modus */
  int               batchN;      /* batch size */
  double            initMax;     /* random initialization maximum */
  double            eta;         /* learning rate */
  double            nu;          /* momentum factor (0..1) */
  double            flatSpot;    /* flat spot elimination offset */
  
} MlpClass;




/* ================================================================= */
/*                      classifier prototypes                        */
/* ================================================================= */

ClientData  mlpClassCreateClf        (int classN, int dimN, int argc, char *argv[]);
ClientData  mlpClassCopyClf          (ClientData cd, double perturbe);
int         mlpClassInitClf          (ClientData cd);
int         mlpClassFreeClf          (ClientData cd);
int         mlpClassSaveClf          (ClientData cd, FILE *fp);
ClientData  mlpClassLoadClf          (FILE *fp);
int         mlpClassCreateAccusClf   (ClientData cd);
int         mlpClassFreeAccusClf     (ClientData cd);
int         mlpClassClearAccusClf    (ClientData cd);
int         mlpClassSaveAccusClf     (ClientData cd, FILE *fp);
int         mlpClassLoadAccusClf     (ClientData cd, FILE *fp);
int         mlpClassConfigureClf     (ClientData cd, int argc, char *argv[]);
int         mlpClassScoreClf         (ClientData cd, float *pattern, int dimN,
		       	              double *score, int classN);
int         mlpClassAccuSoftClf      (ClientData cd, float *pattern, int dimN,
		        	      double *target, double *act,
			              int classN, double g, double h);
int         mlpClassAccuHardClf      (ClientData cd, float *pattern, int dimN,
			              int targetX, double *act,
			              int classN, double g, double h);
int         mlpClassUpdateClf        (ClientData cd);



/* ================================================================= */
/*                           itf prototypes                          */
/* ================================================================= */

int         mlpClassAccuPutsItf     (ClientData cd, int argc, char *argv[]);

ClientData  mlpClassCreateItf       (ClientData cd, int argc, char *argv[]);
int         mlpClassFreeItf         (ClientData cd);
int         mlpClassConfigureItf    (ClientData cd, char *var, char *val);
ClientData  mlpClassAccessItf       (ClientData cd, char *name, TypeInfo **ti);

int         mlpClassPutsItf         (ClientData cd, int argc, char *argv[]);
int         mlpClassCopyItf         (ClientData cd, int argc, char *argv[]);
int         mlpClassInitItf         (ClientData cd, int argc, char *argv[]);

int         mlpClassSaveItf         (ClientData cd, int argc, char *argv[]);
int         mlpClassLoadItf         (ClientData cd, int argc, char *argv[]);

int         mlpClassCreateAccusItf  (ClientData cd, int argc, char *argv[]);
int         mlpClassFreeAccusItf    (ClientData cd, int argc, char *argv[]);
int         mlpClassClearAccusItf   (ClientData cd, int argc, char *argv[]);
int         mlpClassSaveAccusItf    (ClientData cd, int argc, char *argv[]);
int         mlpClassLoadAccusItf    (ClientData cd, int argc, char *argv[]);

int         mlpClassScoreItf        (ClientData cd, int argc, char *argv[]);
int         mlpClassAccuHardItf     (ClientData cd, int argc, char *argv[]);
int         mlpClassUpdateItf       (ClientData cd, int argc, char *argv[]);




#endif

#ifdef __cplusplus
}
#endif
