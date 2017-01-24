/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Covariance Matrices
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  covar.h
    Date    :  $Id: covar.h 3140 2010-02-20 21:42:21Z metze $

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
    Revision 3.9  2005/09/29 12:35:16  stueker
    Added fcvSetItf

    Revision 3.8  2003/08/14 11:20:14  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.7.4.1  2002/06/05 12:18:21  metze
    Changes in dcvAccuAdd

    Revision 3.7  2001/01/10 16:58:36  janus
    Merged Florian's version, supporting e.g. WAV-Files, Multivar and NGETS

    Revision 3.6  2000/11/14 12:01:11  janus
    Merged branch jtk-00-10-27-jmcd.

    Revision 3.5.12.1  2000/11/06 10:50:26  janus
    Made changes to enable compilation under -Wall.

    Revision 3.5  2000/10/12 12:57:12  janus
    Merged branch jtk-00-10-09-metze.

    Revision 3.4.18.1  2000/10/12 08:02:15  janus
    This compiles, but does not work on SUN for decoding without SIMD

    Revision 3.4.12.1  2000/10/11 09:24:40  janus
    Added SIMD instructions and preliminary MultiVar version

    Revision 3.4  2000/08/27 15:31:11  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.3.4.1  2000/08/25 22:19:43  jmcd
    Just checking.

    Revision 3.3  1997/07/18 19:37:50  tschaaf
    dcc / DFKI -clean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.9  96/09/04  15:35:53  rogina
    added variance shifting
    
    Revision 1.8  96/02/27  00:19:43  00:19:43  rogina (Ivica Rogina)
    added stuff for tying / typing of covars
    
    Revision 1.7  1995/12/17  16:58:44  finkem
    made dcvSubspace double

    Revision 1.6  1995/11/25  20:40:49  finkem
    changed read accu bug (see codebook.c)

    Revision 1.5  1995/11/23  08:00:00  finkem
    changed copy function to replace a matrix (instead of overriding only
    the vectors). Now you can copy src to a dst of different size...
    Necessary for the CodebookMap function.

    Revision 1.4  1995/11/14  06:05:14  finkem
    added lots of covar functions which were originally in codebook.c

    Revision 1.3  1995/10/18  23:21:07  finkem
    cleaning up

    Revision 1.2  1995/08/28  16:11:58  rogina
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _covar
#define _covar

#define COV_FLOOR 1.0e-19

/* ========================================================================
    FCovMatrix
   ======================================================================== */

typedef enum { COV_ERR = -1, COV_NO, COV_RADIAL, COV_DIAGONAL, COV_FULL } CovType;

typedef struct
{
  char   *name;             /* name of this covariance matrix               */
  int     useN;             /* number of references to the covariance mat.  */
  int     dimN;             /* dimension of the feature space               */
  float   det;              /* the log of the determinant                   */
  CovType type;             /* type of covariance matrix                    */

  union
  {
    float   r;              /* r       = 1.0 / single radial variance value */
    float  *d;              /* d[k]    = 1.0 / diagonal variance[k]         */
    float **f;              /* f[k][l] = inverse of full covariance matrix  */
  }	    m;              /* the covariance matrix                        */

} FCovMatrix;

extern FCovMatrix* fcvCreate( int dimN, CovType type);

extern int    fcvInit(      FCovMatrix* CV, int dimN, CovType type);
extern int    fcvDeinit(    FCovMatrix* CV);
extern int    fcvFree(      FCovMatrix* CV);
extern int    fcvClear(     FCovMatrix* CV);
extern int    fcvConst(     FCovMatrix* CV, float val);
extern int    fcvMatch(     FCovMatrix* CV, FCovMatrix* src);
extern float  fcvMetric(    FCovMatrix* CV, float* fvec);
extern int    fcvShift(     FCovMatrix* CV, float shift);
extern int    fcvAdd(       FCovMatrix* CV, FCovMatrix* src, float scale, 
                                                             float alpha);
extern int    fcvCopy(      FCovMatrix* CV, FCovMatrix* src);
extern int    fcvVariances( FCovMatrix* CV, float* fcvVarA, int* fcvVarX);

extern int fcvLoad(   FCovMatrix* CV, FILE* fp);
extern int fcvSave(   FCovMatrix* CV, FILE* fp);
extern int fcvSkip(   int dimN, CovType type, FILE* fp);

FCovMatrix* fcvGetItf( char* value, CovType refType, int refDimN);


/* ========================================================================
    DCovMatrix
   ======================================================================== */

typedef struct
{
  int     dimN;             /* dimension of the feature space               */
  int     useN;             /* number of references to the covariance mat.  */
  CovType type;             /* type of covariance matrix                    */

  int     maxVarDimX;       /* index of greatest variance for diagonal mat. */
  float   maxVar;

  union
  {
    double   r;              /* r       = 1.0 / single radial variance value */
    double  *d;              /* d[k]    = 1.0 / diagonal variance[k]         */
    double **f;              /* f[k][l] = inverse of full covariance matrix  */
  }	     m;              /* the covariance matrix                        */
} DCovMatrix;

extern int   dcvInit( DCovMatrix* CV, int dimN, CovType type);
extern int   dcvDeinit( DCovMatrix* CV);

extern int   dcvClear(    DCovMatrix* CV);
extern int   dcvConst(    DCovMatrix* CV, double val);
extern int   dcvMatch(    DCovMatrix* CV, DCovMatrix* src);
extern int   dcvAdd(      DCovMatrix* CV, DCovMatrix* src, float factor);
extern int   dcvCopy(     DCovMatrix* CV, DCovMatrix* src);
extern int   dcvTimes(    DCovMatrix* CV, float scale);
extern int   dcvConst(    DCovMatrix* CV, double val);

extern int    dcvAccuAdd(  DCovMatrix* CV, double* pattern, double* spattern,
			   double factor);
extern double dcvSubspace( DCovMatrix* CV, float* rv, float* pattern, 
                                           double factor);

extern int   dcvLoad(     DCovMatrix* CV, float addFactor, FILE* fp);
extern int   dcvSave(     DCovMatrix* CV, FILE* fp);
extern int   dcvSkip(     int dimN, CovType type, FILE* fp);

/* ========================================================================
    Modul Interface
   ======================================================================== */

extern int Covar_Init(void);

extern CovType cvStrToType(char* typeName);
extern char*   cvTypeToStr(CovType type);
extern int     cvGetTypeItf(ClientData CD, char* key, ClientData retPtr, int*  argcPtr, char* argv[]);

#endif

#ifdef __cplusplus
}
#endif
