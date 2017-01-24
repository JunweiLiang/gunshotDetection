/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: General feed forward multi layer neural nets
               ------------------------------------------------------------

    Author  :  Juergen Fritsch
    Module  :  ffnet.h
    Date    :  $Id: ffnet.h 700 2000-11-14 12:35:27Z janus $
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
    Revision 3.4  2000/11/14 12:29:33  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 3.3.36.1  2000/11/06 10:50:29  janus
    Made changes to enable compilation under -Wall.

    Revision 3.3  1997/07/04 11:53:29  fritsch
    made it gcc -Wall clean

 * Revision 1.1  1997/01/27  08:59:05  fritsch
 * Initial revision
 *

   ======================================================================== */


#ifndef __ffnet__
#define __ffnet__

#include "matrix.h"


/* ======================================================================= */
/*                             Data Types                                  */
/* ======================================================================= */



typedef int   (*NetFctPtr)      (FMatrix *Y, FMatrix *X, FMatrix *W);

typedef int   (*ActFctPtr)      (FMatrix *Y, FMatrix *X);
typedef int   (*ActDerFctPtr)   (FMatrix *Y, FMatrix *X);

typedef float (*ErrFctPtr)      (FMatrix *Y, FMatrix *T);
typedef int   (*ErrDerFctPtr)   (FMatrix *Z, FMatrix *Y, FMatrix *T);



typedef struct {

  int           sourceX;         /* index of source layer (sourceX < 0 : 
                                    connect to input layer) */

  FMatrix       *W;              /* weight matrix */
  IMatrix       *mask;           /* weight mask matrix */

  NetFctPtr     netFct;          /* pointer to network function */

  FMatrix       *dW;             /* weight update matrix */
  FMatrix       *ddW;            /* momentum term update matrix */

  int           eval;            /* boolean: evaluate this connection ? */
  int           update;          /* boolean: update this connection ? */
  float         eta;             /* learning rate */
  float         alpha;           /* momentum factor */
  float         gamma;           /* angular weight update velocity */

} FFLink;


typedef struct {

  FMatrix       *act;            /* activations in this layer */

  ActFctPtr     actFct;          /* pointer to activation function */
  ActDerFctPtr  actDerFct;       /* pointer to derivative of act. fct. */

  FMatrix       *actDer;         /* derivatives of activation function */
  FMatrix       *delta;          /* delta's for updating */

  int           linkN;           /* number of connections */
  FFLink        **link;          /* pointer to array of connections */

} FFLayer;



typedef struct {  

  char          *name;
  int           useN;

  int           outputN;         /* size of output vectors */
  int           inputN;          /* size of input vectors */

  ErrFctPtr     errFct;          /* pointer to error function */
  ErrDerFctPtr  errDerFct;       /* pointer to error derivative function */

  int           layerN;          /* number of layers */
  FFLayer       **layer;         /* pointer to array of layers */

} FFNet;




/* ======================================================================= */
/*                              Prototypes                                 */
/* ======================================================================= */



int              _fmatrixMulT();
int              _fmatrixRadT();


int              idActFct();
int              idActDerFct();

int              tanhActFct();
int              tanhActDerFct();

int              sigActFct();
int              sigActDerFct();

int              expActFct();
int              expActDerFct();

int              softActFct();
int              softActDerFct();



float            mseErrFct(); 
int              mseErrDerFct();

float            binomErrFct(); 
int              binomErrDerFct();

float            klErrFct(); 
int              klErrDerFct();
 
float            multiErrFct(); 
float            classErrFct();
int              multiErrDerFct();



int         ffnetReset          (FFNet *net);
int         ffnetAddLink        (FFNet *net, int targetX, int sourceX,
                                 NetFctPtr netFct);
int         ffnetAddLayer       (FFNet *net, int nodeN, ActFctPtr actFct, 
                                 ActDerFctPtr actDerFct);
int         ffnetInit           (FFNet *net, int outputN, int inputN,
                                 ErrFctPtr errFct, ErrDerFctPtr errDerFct);
FFNet       *ffnetCreate        (char *name, int outputN, int inputN,
                                 ErrFctPtr errFct, ErrDerFctPtr errDerFct);
int         ffnetDeinit         (FFNet *net);
int         ffnetDestroy        (FFNet *net);
FMatrix     *ffnetEval          (FFNet *net, FMatrix *input);
float       ffnetError          (FFNet *net, FMatrix *input, FMatrix *target,
                                 ErrFctPtr errFct, int noFwd);

int         ffnetBackprop       (FFNet *net, FMatrix *input, 
                                 FMatrix *target, float *weight,
                                 int noFwd, int useMomentum,
                                 int adapt);

int         ffnetBackpropBatch  (FFNet *net, FMatrix *input, 
                                 FMatrix *target, float *weight,
                                 int shuffle, int useMomentum, int adapt,
                                 int iterN, int batchN, ErrFctPtr errFct,
                                 char *logfile, FMatrix *logmat, int testAll);



int          fflinkPutsItf       (ClientData cd, int argc, char *argv[]);
int          fflinkConfigureItf  (ClientData cd, char *var, char *val);
ClientData   fflinkAccessItf     (ClientData cd, char *name, TypeInfo **ti);
int          fflinkMaskItf       (ClientData cd, int argc, char *argv[]);
int          fflinkRandomItf     (ClientData cd, int argc, char *argv[]);

int          fflayerPutsItf      (ClientData cd, int argc, char *argv[]);
int          fflayerConfigureItf (ClientData cd, char *var, char *val);
ClientData   fflayerAccessItf    (ClientData cd, char *name, TypeInfo **ti);
int          fflayerConfLinksItf (ClientData cd, int argc, char *argv[]);
int          fflayerRandomItf    (ClientData cd, int argc, char *argv[]);

ClientData   ffnetCreateItf      (ClientData cd, int argc, char *argv[]);
int          ffnetFreeItf        (ClientData cd);
int          ffnetPutsItf        (ClientData cd, int argc, char *argv[]);
int          ffnetConfigureItf   (ClientData cd, char *var, char *val);
ClientData   ffnetAccessItf      (ClientData cd, char *name, TypeInfo **ti);

int  ffnetAddLinkItf   (ClientData cd, int argc, char *argv[]);
int  ffnetAddLayerItf  (ClientData cd, int argc, char *argv[]);
int  ffnetAddItf       (ClientData cd, int argc, char *argv[]);
int  ffnetWriteItf     (ClientData cd, int argc, char *argv[]);
int  ffnetReadItf      (ClientData cd, int argc, char *argv[]);
int  ffnetSaveItf      (ClientData cd, int argc, char *argv[]);

int  ffnetLoadItf      (ClientData cd, int argc, char *argv[]);
int  ffnetCopyItf      (ClientData cd, int argc, char *argv[]);
int  ffnetConfLinksItf (ClientData cd, int argc, char *argv[]);
int  ffnetRandomItf    (ClientData cd, int argc, char *argv[]);
int  ffnetResetItf     (ClientData cd, int argc, char *argv[]);
int  ffnetEvalItf      (ClientData cd, int argc, char *argv[]);
int  ffnetBackpropItf  (ClientData cd, int argc, char *argv[]);

extern int FFNet_Init ();

#endif
