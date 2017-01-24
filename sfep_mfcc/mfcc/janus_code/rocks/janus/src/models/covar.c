/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Covariance Matrices
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  covar.c
    Date    :  $Id: covar.c 3416 2011-03-23 03:02:18Z metze $

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
    Revision 3.13  2007/03/02 13:15:16  fuegen
    added SSE support (Florian Metze, Friedrich Faubel)

    Revision 3.12  2007/02/22 16:28:00  fuegen
    removed -pedantic compiler warnings with

    Revision 3.11  2005/11/02 17:05:29  stueker
    Made fcvGetItf ANSI C compatible, in order for it to compile under Visual C (changes came from tschaaf)

    Revision 3.10  2005/09/29 12:35:54  stueker
    Added fcvSetItf

    Revision 3.9  2003/08/14 11:20:13  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.8.4.4  2002/07/18 12:22:00  soltau
    Removed covar.c

    Revision 3.8.4.3  2002/06/26 11:57:56  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.8.4.2  2002/06/05 12:18:08  metze
    Faster accumulation with dcvAccuAdd

    Revision 3.8.4.1  2001/07/03 09:34:47  soltau
    Use memalign instead of malloc

    Revision 3.8  2001/01/10 16:58:36  janus
    Merged Florian's version, supporting e.g. WAV-Files, Multivar and NGETS

    Revision 3.7  2000/11/14 12:01:11  janus
    Merged branch jtk-00-10-27-jmcd.

    Revision 3.6.12.1  2000/11/06 10:50:26  janus
    Made changes to enable compilation under -Wall.

    Revision 3.6  2000/10/12 12:57:12  janus
    Merged branch jtk-00-10-09-metze.

    Revision 3.5.20.1  2000/10/12 08:02:15  janus
    This compiles, but does not work on SUN for decoding without SIMD

    Revision 3.5.6.1  2000/10/11 09:24:40  janus
    Added SIMD instructions and preliminary MultiVar version

    Revision 3.5  2000/08/16 09:14:56  soltau
    Changed fcvPuts: append 'det' values

    Revision 3.4  1998/08/12 09:21:41  kemp
    Removed a bug in allocation of full covariance matrices
    Made fcvVariances work for full covariances. However, this sorting
    of variances is done in the same way as it is done for diagonal ones,
    which is clearly not a superior way of handling it.

 * Revision 3.3  1997/07/18  19:37:50  tschaaf
 * dcc / DFKI -clean
 *
    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.14  96/09/05  10:24:00  rogina
    forgot to update determinant after shift, fixed that
    
    Revision 1.13  1996/09/04 15:35:41  rogina
    added variance shifting

    Revision 1.12  96/03/05  16:19:18  16:19:18  rogina (Ivica Rogina)
    make gcc -Wall warnings go away
    
    Revision 1.11  1996/02/27  00:19:23  rogina
    added stuff for tying / typing of covars

    Revision 1.10  1996/01/25  10:16:40  kemp
    Corrected a comment.

    Revision 1.9  1995/12/17  16:58:44  finkem
    made dcvSubspace double

    Revision 1.8  1995/11/25  20:40:49  finkem
    changed read accu bug (see codebook.c)

    Revision 1.7  1995/11/23  08:00:00  finkem
    changed copy function to replace a matrix (instead of overriding only
    the vectors). Now you can copy src to a dst of different size...
    Necessary for the CodebookMap function.

    Revision 1.6  1995/11/17  22:25:42  finkem
    bug in Configure of covariances

    Revision 1.5  1995/11/14  06:05:14  finkem
    added lots of covar functions which were originally in codebook.c

    Revision 1.4  1995/10/18  23:21:07  finkem
    cleaning up

    Revision 1.3  1995/09/06  07:14:57  kemp
    *** empty log message ***

    Revision 1.2  1995/08/17  17:54:43  rogina
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#include <limits.h>
#include "common.h"
#include "itf.h"
#include "matrix.h"
#include "covar.h"
#include "math.h"

#include "mach_ind_io.h"

/* ========================================================================
    FCovMatrix
   ======================================================================== */
/* ------------------------------------------------------------------------
    fcvInit      initialize covariance structure
   ------------------------------------------------------------------------ */

int fcvInit( FCovMatrix* CV, int dimN, CovType type)
{
  int i, j;

  assert( CV);

  CV->name       = NULL;
  CV->dimN       = dimN;
  CV->useN       = 0;
  CV->det        = 0.0;
  CV->type       = type;

  switch( CV->type) {

  case COV_RADIAL :

    CV->m.r = 1.0;
    break;

  case COV_DIAGONAL :

#ifdef SSE_OPT
    if (! (CV->m.d = (float*)_mm_malloc(sizeof(float) * dimN,16))) {
#else
    if (! (CV->m.d = (float*)memalign(8,sizeof(float) * dimN))) {
#endif
      ERROR("Can't allocate covariance matrix %dx%d.\n", dimN, dimN);
      return TCL_ERROR;
    }
    for ( i = 0; i < dimN; i++) CV->m.d[i] = 1.0;

    break;

  case COV_FULL :
    /* SSE_OPT DOES NOT WORK FOR FULL COVARIANCES !!! */

    if (! (CV->m.f    = (float**)malloc( sizeof(float*) * dimN)) ||
        ! (CV->m.f[0] = (float* )malloc( sizeof(float ) * dimN * dimN))) {

      ERROR("Can't allocate covariance matrix %dx%d.\n", dimN, dimN);
      if ( CV->m.f) { free(CV->m.f); CV->m.f = NULL; }
      return TCL_ERROR;
    }
    for ( i = 1; i < dimN; i++) CV->m.f[i] = CV->m.f[i-1] + dimN;
    for ( i = 0; i < dimN; i++) {
      for ( j = 0; j < dimN; j++) {
        if ( i != j) CV->m.f[i][j] = 0.0;
        else         CV->m.f[i][j] = 1.0;
      }
    }
    break;

  default :
    break;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    fcvCreate    create covariance matrix structure
   ------------------------------------------------------------------------ */

FCovMatrix* fcvCreate( int dimN, CovType type)
{
  FCovMatrix* CV = (FCovMatrix*)malloc( sizeof(FCovMatrix));

  if (! CV || fcvInit( CV, dimN, type) != TCL_OK) {
    if ( CV) free( CV);
    ERROR("Can't allocate CovMatrix(%d).\n", dimN);
    return NULL;
  }
  return CV;
}

/* ------------------------------------------------------------------------
    fcvDeinit      deinitialize covariance structure
   ------------------------------------------------------------------------ */

int fcvDeinit( FCovMatrix* CV)
{
  assert( CV);
  switch( CV->type) {
  case COV_DIAGONAL : 
    if ( CV->m.d) {
#ifdef SSE_OPT
      _mm_free(CV->m.d);
#else
      free(CV->m.d);
#endif
      CV->m.d = NULL; 
    }      
    break;
  case COV_FULL :
    if ( CV->m.f) { 
      if ( CV->m.f[0]) free(CV->m.f[0]);
      free(CV->m.f);        CV->m.f = NULL; 
    }
    break;
  default :
    break;
  }
  return TCL_OK;  
}

/* ------------------------------------------------------------------------
    fcvFree
   ------------------------------------------------------------------------ */

int fcvFree( FCovMatrix* CV)
{
  if ( fcvDeinit( CV) != TCL_OK) return TCL_ERROR;
  if ( CV) free(  CV);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    fcvConfigureItf     configure a float covariance object
   ------------------------------------------------------------------------ */

static int fcvConfigureItf (ClientData cd, char *var, char *val)
{
  FCovMatrix* CV   = (FCovMatrix*)cd;
  if (! CV)   return TCL_ERROR;

  if (! var) {
    ITFCFG(fcvConfigureItf,cd,"det");
    ITFCFG(fcvConfigureItf,cd,"useN");
    ITFCFG(fcvConfigureItf,cd,"type");
    return TCL_OK;
  }
  ITFCFG_Int(  var,val,"useN",    CV->useN, 1);
  ITFCFG_Float(var,val,"det",     CV->det,  0);
  if (! strcmp(var,"type")) {
    if (! val) { itfAppendElement("%s",cvTypeToStr(CV->type)); 
                 return TCL_OK; }
    return TCL_OK;
  }

  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    fcvPutsItf    print covariance matrix to TCL string
   ------------------------------------------------------------------------ */

static int fcvPutsItf( ClientData cd, int argc, char *argv[])
{
  FCovMatrix* CV   = (FCovMatrix*)cd;
  int         dimN =  CV->dimN;
  int         i,j;

  itfAppendResult( "{");

  switch( CV->type) {
  case COV_RADIAL :
    itfAppendResult( "% 2.4e", CV->m.r);
    break;

  case COV_DIAGONAL :
    for ( i = 0; i < dimN; i++) itfAppendResult( " % 2.4e", CV->m.d[i]);
    itfAppendResult( " % 2.4e", CV->det);
    break;

  case COV_FULL :
    for ( i = 0; i < dimN; i++) {
      itfAppendResult( "{");
      for ( j = 0; j < dimN; j++) {
        itfAppendResult( " % 2.4e", CV->m.f[i][j]);
      }
      itfAppendResult( "}%c", (i<dimN-1) ? '\n' : 0);
    }
    break;

  default :
    break;
  }
  itfAppendResult( "}");
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    fcvMatch    check whether two convariance matrix accus match such that
                functions like add, copy etc. should work
   ------------------------------------------------------------------------ */

int fcvMatch( FCovMatrix* dst, FCovMatrix* src)
{
  if (src->dimN != dst->dimN) {
     SERROR("Inconsistent dimN: %d %d\n",dst->dimN,src->dimN); 
     return TCL_ERROR;
  }
  if (src->type != dst->type) {
     SERROR("Inconsistent type: %d %d\n",dst->type,src->type);
     return TCL_ERROR;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    fcvAdd   interpolate two covariance matrices by scaling the destination
             matrix by scale and adding alpha * src to it.
   ------------------------------------------------------------------------ */

int fcvAdd( FCovMatrix* dst, FCovMatrix* src, float scale, float alpha)
{
  float d    = 0.0;
  int   dimN = dst->dimN;
  int   i;
  double val;

  if ( fcvMatch( dst, src) != TCL_OK) return TCL_ERROR;

  switch( dst->type) {
  case COV_RADIAL : 
    val = (scale / dst->m.r) + (alpha / src->m.r);
    if ( val < COV_FLOOR) val = COV_FLOOR;
    dst->m.r = 1.0 / val;
    dst->det = dimN * log(val);
    break;
  case COV_DIAGONAL :
    for ( i = 0; i < dimN; i++) {
      val = (scale / dst->m.d[i]) + (alpha / src->m.d[i]);
      if ( val < COV_FLOOR) val = COV_FLOOR;
      dst->m.d[i] = 1.0 / val;
      d          += log(val);
    }
    dst->det = d;
    break;
  case COV_FULL     :
    FATAL("fcvAdd for COV_FULL not yet implemented.\n");
    break;

  default :
    break;
  }
  return TCL_OK;
}

static int fcvAddItf( ClientData cd, int argc, char *argv[])
{
  FCovMatrix *dst   = (FCovMatrix*)cd;
  FCovMatrix *src   =  NULL;
  float       scale =  1.0;
  float       alpha =  1.0;
  int         ac    =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<source>", ARGV_OBJECT, NULL, &src, "FCovMatrix", 
      "source covariance matrix",
      "-scale", ARGV_FLOAT, NULL, &scale, NULL, "scaling of the destination",
      "-alpha", ARGV_FLOAT, NULL, &alpha, NULL, "scaling of the source",
       NULL) != TCL_OK) return TCL_ERROR;

  return fcvAdd( dst, src, scale, alpha);
}

/* ------------------------------------------------------------------------
    fcvCopy      copy a covariance matrix to another one. 
   ------------------------------------------------------------------------ */

int fcvCopy( FCovMatrix* dst, FCovMatrix* src)
{
  int dimN =  src->dimN;
  int useN =  dst->useN;
  int i,j;

  if ( src == dst) return TCL_OK;

  /* Check whether dim and type of covariance matrices match */
  if ( fcvMatch( dst, src) != TCL_OK) return TCL_ERROR;

  if ( fcvDeinit( dst) != TCL_OK ||
       fcvInit(   dst, src->dimN, src->type) != TCL_OK) {
    ERROR("fcvCopy can't allocate covariance matrix.\n");
    return TCL_ERROR;
  }
  dst->useN = useN;

  switch( dst->type) {
  case COV_RADIAL : 
    dst->m.r = src->m.r;
    dst->det = src->det;
    break;
  case COV_DIAGONAL :
    for ( i = 0; i < dimN; i++) dst->m.d[i] = src->m.d[i];
    dst->det = src->det;
    break;
  case COV_FULL     :
    for ( i = 0; i < dimN; i++)
      for ( j = 0; j < dimN; j++) 
        dst->m.f[i][j] = src->m.f[i][j];
    dst->det = src->det;
    break;
  default :
    break;
  }
  return TCL_OK;
}


int fDiagonalcvfmatCopy( FCovMatrix* dst, FMatrix* src)
{

  int dimN =  src->n;
  int useN =  dst->useN;
  int i;

  if ( dst->type!=COV_DIAGONAL || src->n == 1 || src->m > 1) {
	  INFO("n: %d m: %d",src->n, src->m);
	  ERROR("FMatrix transfer only implemented for DIAGONAL covariance matrices");
	  return TCL_ERROR;
  }
  if ( dst->dimN != dimN ) {
	  ERROR("Dimensions of FMatrix and FCovMatrix don't fit.\n");
	  return TCL_ERROR;
  }
  if ( fcvDeinit( dst) != TCL_OK ||
       fcvInit(   dst, src->n, COV_DIAGONAL) != TCL_OK) {
    ERROR("fcvCopy can't allocate covariance matrix.\n");
    return TCL_ERROR;
  }
  dst->useN = useN;

  switch( dst->type) {
  case COV_DIAGONAL :
    dst->det = 0.0;
    for ( i = 0; i < dimN; i++) {
	  dst->m.d[i] = 1.0f / src->matPA[0][i];
	  dst->det -= log(dst->m.d[i]);
    }
    break;
  default :
    break;
  }
  

  return TCL_OK;
}

int fDiagonalcvfvecCopy( FCovMatrix* dst, FVector* src)
{

  int dimN =  src->n;
  int useN =  dst->useN;
  int i;

  if ( dst->type!=COV_DIAGONAL || src->n == 1 ) {
	  INFO("n: %d ",src->n);
	  ERROR("FMatrix transfer only implemented for DIAGONAL covariance matrices");
	  return TCL_ERROR;
  }
  if ( dst->dimN != dimN ) {
	  ERROR("Dimensions of FVector and FCovMatrix don't fit.\n");
	  return TCL_ERROR;
  }
  if ( fcvDeinit( dst) != TCL_OK ||
       fcvInit(   dst, src->n, COV_DIAGONAL) != TCL_OK) {
    ERROR("fcvCopy can't allocate covariance matrix.\n");
    return TCL_ERROR;
  }
  dst->useN = useN;

  switch( dst->type) {
  case COV_DIAGONAL :
    dst->det = 0.0;
    for ( i = 0; i < dimN; i++) {
	  dst->m.d[i] = 1.0f / src->vecA[i];
	  dst->det -= log(dst->m.d[i]);
    }
    break;
  default :
    break;
  }
  

  return TCL_OK;
}


static int fcvCopyItf( ClientData cd, int argc, char *argv[])
{
  FCovMatrix *dst   = (FCovMatrix*)cd;
  FCovMatrix *src   =  NULL;
  int         ac    =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<source>", ARGV_OBJECT, NULL, &src, "FCovMatrix", 
      "source covariance matrix",
       NULL) != TCL_OK) return TCL_ERROR;

  return fcvCopy( dst, src);
}


/**
   Set a specific covariance matrix.
   Set the covariance matrix to the tcl-style matrix given as an argument
 */
static int fcvSetItf( ClientData cd, int argc, char *argv[]) {
  FCovMatrix* dst = (FCovMatrix*)cd;
  CovType     dstType = dst->type;
  int         dstDimN = dst->dimN;
  FCovMatrix* tmp;
  char*       matrixElements = NULL;

  argc--;  
  if ( itfParseArgv( argv[0], &argc, argv+1, 0,
		     "<matrix>", ARGV_STRING, NULL, &matrixElements, cd, "matrix of covariance vectors", 
		     NULL) != TCL_OK) {
    return TCL_ERROR;
  }

  if ((tmp = fcvGetItf(matrixElements, dstType, dstDimN)) == NULL) {
    ERROR ("Could not allocate temporary covariance matrix\n");
    return TCL_ERROR;
  }
  
  fcvCopy(dst, tmp);
  fcvFree(tmp);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    fcvGetItf
   ------------------------------------------------------------------------ */

FCovMatrix* fcvGetItf( char* value, CovType refType, int refDimN)
{
  FCovMatrix* mat;

  /* ------------------------------------------------------------------------
     If 'value' is not an existing FCovMatrix object scan matrix elements.
     ----------------------------------------------------------------------- */
       
  if ((mat = (FCovMatrix*)itfGetObject( value, "FCovMatrix")) == NULL) {

    int     argc;
    char**  argv;

    MSGCLEAR(NULL);


	switch( refType) {
	    
	    case COV_RADIAL:
		if ( Tcl_SplitList( itf, value, &argc, &argv) == TCL_OK) {
			if ( Tcl_SplitList( itf, argv[0], &argc, &argv) == TCL_OK) {
				float d;
				if (argc!=1) return NULL;
				if ( ! (mat = fcvCreate(refDimN, COV_RADIAL))) {
					ERROR("Creating radial covariance matrix failed.\n");
					Tcl_Free((char*)argv);  
					return NULL;
				}
				
				
				if ( sscanf( argv[0], "%f", &d) != 1) {
					ERROR("expected 'float' type elements.\n");
					Tcl_Free((char*)argv);
					fcvFree( mat);
					return NULL;
				}
				if ( d < COV_FLOOR) d = COV_FLOOR;
				mat->m.r = 1.0 / d;
				mat->det = -refDimN * log(d);
				
				Tcl_Free((char*)argv);
			}			
		}
		break;
	
	    case COV_DIAGONAL:
		    
		if ( Tcl_SplitList( itf, value, &argc, &argv) == TCL_OK) {
			if ( Tcl_SplitList( itf, argv[0], &argc, &argv) == TCL_OK) {
				int i;

				if (argc!=refDimN) return NULL;
				for ( i = 0; i < refDimN; i++) {
					float d;
				
					if ( i == 0 &&  ! (mat = fcvCreate(refDimN, COV_DIAGONAL))) {
					      //mat->det = 0.0;
					      ERROR("Creating radial covariance matrix failed.\n");
					      Tcl_Free((char*)argv); 
					      return NULL;
					}
				
				
					if ( sscanf( argv[i], "%f", &d) != 1) {
					     ERROR("expected 'float' type elements.\n");
					     Tcl_Free((char*)argv);
					     fcvFree( mat);
					     return NULL;
					}
					if ( d < COV_FLOOR) d = COV_FLOOR;
	
					mat->m.d[i] = 1.0 / d;
					mat->det += log(d);
				
				}
				Tcl_Free((char*)argv);
			}
		}
		break;
		
	    case COV_FULL:
		FATAL("fcvGetItf for COV_FULL not yet implemented.\n");
		      /*
		      if ( Tcl_SplitList( itf, value, &argc, &argv) == TCL_OK) {
		      int i, j;
		
		      for ( i = 0; i < refDimN; i++) {
			int    tokc;
			char** tokv;
		
			if ( Tcl_SplitList( itf, argv[i], &tokc, &tokv) == TCL_OK) {
			  if ( i == 0 &&  ! (mat = fcvCreate(refDimN, COV_FULL))) {
			     Tcl_Free((char*)argv);  Tcl_Free((char*)tokv);  return NULL;
			  }
		
			  for ( j = 0; j < tokc && j < refDimN; j++) {
			    float d;
		
			    if ( sscanf( tokv[j], "%f", &d) != 1) {
			       ERROR("expected 'float' type elements.\n");
			       Tcl_Free((char*)tokv); Tcl_Free((char*)argv);
			       fcvFree( mat);
			       return NULL;
			    }
			    mat->m.f[i][j] = 1.0 /  d;
			    if ( d < COV_FLOOR) d = COV_FLOOR;
			    mat->det = ???;
			    HIER FEHLT NOCH DIE DETERMINANTENBERECHNUNG!
			    ACHTUNG: logarithmisch erfordert!
			  }
			  Tcl_Free((char*)tokv);
			}
		      }
		      Tcl_Free((char*)argv);
		    }
		    */
	      break;
	    default :
		ERROR("Cov-Type not found.\n");
	    break;
	}  
	
	return mat;
  }
  /* ------------------------------------------------------------------------
     'value' is an existing FCovMatrix object.
     ----------------------------------------------------------------------- */
 /* else {

    FCovMatrix* nmat = fcvCreate( refDimN, refType);

    if (! nmat) {
	    INFO ("Klammer1\n.");
	    return NULL;
    }
    if (! mat) {
	    INFO ("Klammer1a\n.");
	    return NULL;
    }
    
    if (!fcvCopy( nmat, mat)) {
	    INFO ("Klammer2\n.");
	    return NULL;
    }
    return nmat;
  }
  */
  return NULL;
}


/* ------------------------------------------------------------------------
    fcvMetric    calculates the metric: fvec^T * CV^-1 * fvec
   ------------------------------------------------------------------------ */

float fcvMetric( FCovMatrix* CV, float* fvec)
{
  float x    = 0.0;
  int   dimN = CV->dimN, dimX, dimY;

  switch( CV->type) {
  case COV_RADIAL:
    for ( dimX = 0; dimX < dimN; dimX++)
      x += CV->m.r * fvec[dimX] * fvec[dimX];
    break;
  case COV_DIAGONAL:
    for ( dimX = 0; dimX < dimN; dimX++) 
      x += CV->m.d[dimX] * fvec[dimX] * fvec[dimX];
    break;
  case COV_FULL:
    for ( dimX = 0; dimX < dimN; dimX++)
      for ( dimY = 0; dimY < dimN; dimY++)
        x += CV->m.f[dimX][dimY] * fvec[dimX] * fvec[dimY];
    break;
  default :
    break;
  }  
  return x;
}

/* ------------------------------------------------------------------------
    fcvLoad    load covariance matrix parameters from a file written
               using the machine independent I/O routines
   ------------------------------------------------------------------------ */

int fcvLoad (FCovMatrix* CV, FILE* fp)
{
  int         dimN =  CV->dimN;
  int         i,j;

  switch (CV->type) {
  case COV_RADIAL:	
    CV->m.r = read_float(fp);
    CV->det = read_float(fp);
    break;
  case COV_DIAGONAL:
    for ( i = 0; i < dimN; i++) CV->m.d[i] = read_float(fp);
    CV->det = read_float(fp);
    break;
  case COV_FULL:
    for ( i = 0; i < dimN; i++)
      for ( j = 0; j < dimN; j++) CV->m.f[i][j]=read_float(fp);
    CV->det = read_float(fp);
    break;
  default :
    break;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    fcvSave    save covariance matrix parameters to a file using the
               machine independent I/O routines
   ------------------------------------------------------------------------ */

int fcvSave( FCovMatrix* CV, FILE* fp)
{
  int         dimN =  CV->dimN;
  int         i,j;

  switch( CV->type) {
  case COV_RADIAL:	
    write_float( fp, CV->m.r);
    write_float( fp, CV->det);
    break;
  case COV_DIAGONAL:
    for ( i = 0; i < dimN; i++) write_float( fp, CV->m.d[i]);
    write_float(fp, CV->det);
    break;
  case COV_FULL:
    for ( i = 0; i < dimN; i++)
      for ( j = 0; j < dimN; j++) write_float( fp, CV->m.f[i][j]);
    write_float( fp, CV->det);
    break;
  default :
    break;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    fcvSkip     skip codebook parameters in a given file
   ------------------------------------------------------------------------ */

int fcvSkip( int dimN, CovType type, FILE* fp)
{
  int         i,j;

  switch( type) {
  case COV_RADIAL:  read_float(fp); read_float(fp); break;
  case COV_DIAGONAL:
    for ( i = 0; i < dimN; i++) read_float(fp);
    read_float(fp);
    break;
  case COV_FULL:
    for ( i = 0; i < dimN; i++)
      for ( j = 0; j < dimN; j++) read_float(fp);
    read_float(fp);
    break;
  default :
    break;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    fcvVariances   returns a sorted list of the variances
                   and the indices of the resp. axis in the covariance 
                   matrix
   ------------------------------------------------------------------------ */

static float* fcvVarA = NULL;
static int*   fcvVarX = NULL;
static int    fcvVarN = 0;

int fcvVariances( FCovMatrix* CV, float* fcvVarA, int* fcvVarX)
{
  int dimN =  CV->dimN;
  int i,j;

  switch( CV->type) {
  case COV_RADIAL : 
    for ( i = 0; i < dimN; i++) {
      fcvVarA[i] = 1.0 / CV->m.r;
      fcvVarX[i] = i; 
    }
    break;
  case COV_FULL     :
    /* Well... for the moment, handle them as diagonal covar matrices
       would be handled */
  case COV_DIAGONAL :
    /*
    for ( i = 0; i < dimN; i++) {
      fcvVarA[i] = 1.0 / CV->m.d[i];
      fcvVarX[i] = i;
    }
    */
    /* In-line sorting (faster) */
    fcvVarA[0] = 1.0 / CV->m.d[0];
    fcvVarX[0] = 0;
    for (i = 1; i < dimN; i++) {
      float f = 1.0 / CV->m.d[i];
      int k;
      for (j = 0; j < i && fcvVarA[j] < f; j++);
      for (k = i; k > j; k--) {
        fcvVarA[k] = fcvVarA[k-1];
        fcvVarX[k] = fcvVarX[k-1];
      }	
      fcvVarA[j] = f;
      fcvVarX[j] = i;
      }
    break;
  default :
    break;
  }

  /* sort the array of variances */
  /*
  for ( i = 0; i < dimN; i++) {
    for ( j = 1; j < dimN-i; j++) {
      if ( fcvVarA[j-1] < fcvVarA[j]) {
        float f = fcvVarA[j];
        int   x = fcvVarX[j];

        fcvVarA[j] = fcvVarA[j-1]; fcvVarA[j-1] = f;
        fcvVarX[j] = fcvVarX[j-1]; fcvVarX[j-1] = x;
      }
    }
  }
  */
  return TCL_OK;
}

static int fcvVariancesItf( ClientData cd, int argc, char *argv[])
{
  FCovMatrix* dst   = (FCovMatrix*)cd;
  int         ac    =  argc-1, i;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0, NULL) != TCL_OK) return TCL_ERROR;

  if (! fcvVarA || fcvVarN < dst->dimN) {
    fcvVarN =  dst->dimN;
    fcvVarA = (float*)realloc( fcvVarA, fcvVarN * sizeof(float));
    fcvVarX = (int  *)realloc( fcvVarX, fcvVarN * sizeof(int));
  }

  fcvVariances( dst, fcvVarA, fcvVarX);

  for ( i = 0; i < dst->dimN; i++)
    itfAppendResult("{%d % 2.4e} ", fcvVarX[i], fcvVarA[i]);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    fcvShift     add a constant value to all variances (diagonals only)
   ------------------------------------------------------------------------ */

int fcvShift (FCovMatrix *cv, float shift)
{
  int i;
  if (cv->type==COV_DIAGONAL)
     cv->det = 0.0;
     for (i=0; i<cv->dimN; i++)
     {   cv->m.d[i] = 1.0/(1.0/cv->m.d[i]+shift);
         cv->det -= log(cv->m.d[i]);
     }
  return 0;
}

/* ------------------------------------------------------------------------
    fcvClear    clear the contents of the matrix to 0
   ------------------------------------------------------------------------ */

int fcvClear( FCovMatrix* CV)
{
  return fcvConst( CV , 0.0 );
}

int fcvConst( FCovMatrix* CV, float val)
{
  int         dimN =  CV->dimN;
  int         i,j;

  switch( CV->type) {

  case COV_RADIAL   : CV->m.r = val; break;
  case COV_DIAGONAL : for ( i = 0; i < dimN; i++) CV->m.d[i] = val;
                      break;
  case COV_FULL     :
    for ( i = 0; i < dimN; i++) {
      for ( j = 0; j < dimN; j++) CV->m.f[i][j] = val;
    }
    break;
  default :
    break;
  }
  return TCL_OK;
}

static int fcvClearItf( ClientData cd, int argc, char *argv[])
{
  FCovMatrix* CV   = (FCovMatrix*)cd;
  int         ac    =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0, NULL) != TCL_OK) return TCL_ERROR;
  return fcvClear( CV);  
}

/* ========================================================================
    DCovMatrix
   ======================================================================== */
/* ------------------------------------------------------------------------
    dcvInit      initialize covariance structure and allocate the matrix
   ------------------------------------------------------------------------ */

int dcvInit( DCovMatrix* CV, int dimN, CovType type)
{
  int i, j;

  assert( CV);

  CV->dimN = dimN;
  CV->useN = 0;
  CV->type = type;

  switch( CV->type) {

  case COV_RADIAL :
    CV->m.r = 0.0;
    break;

  case COV_DIAGONAL :
#ifdef SSE_OPT
    if (! (CV->m.d = (double*)_mm_malloc(sizeof(double) * dimN,16))) {
#else
    if (! (CV->m.d = (double*)memalign(8,sizeof(double) * dimN))) {
#endif
      ERROR("Can't allocate covariance matrix %dx%d.\n", dimN, dimN);
      return TCL_ERROR;
    }
    for ( i = 0; i < dimN; i++) CV->m.d[i] = 0.0;
    break;

  case COV_FULL :
    if (! (CV->m.f    = (double**)malloc( sizeof(double*)*dimN)) ||
        ! (CV->m.f[0] = (double* )malloc( sizeof(double )*dimN*dimN))) {

      ERROR("Can't allocate covariance matrix %dx%d.\n", dimN, dimN);
      if ( CV->m.f) { free(CV->m.f); CV->m.f = NULL; }
      return TCL_ERROR;
    }
    for ( i = 1; i < dimN; i++) CV->m.f[i] = CV->m.f[i-1] + dimN;
    for ( i = 0; i < dimN; i++) {
      for ( j = 0; j < dimN; j++) CV->m.f[i][j] = 0.0;
    }
    break;

  default :
    break;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dcvCreate    create covariance matrix structure
   ------------------------------------------------------------------------ */
/* This Routine is obviosly not used! But why? so I make a comment of it
   (tschaaf) Maybe someone want to use it in the codebook in future?
static DCovMatrix* dcvCreate( int dimN, CovType type)
{
  DCovMatrix* CV = (DCovMatrix*)malloc( sizeof(DCovMatrix));

  if (! CV || dcvInit( CV, dimN, type) != TCL_OK) {
    if ( CV) free( CV);
    ERROR( "Can't allocate CovMatrix(%d).\n", dimN);
    return NULL;
  }
  return CV;
}

*/

/* ------------------------------------------------------------------------
    dcvDeinit       deinitialize covariance structure
   ------------------------------------------------------------------------ */

int dcvDeinit( DCovMatrix* CV)
{
  assert( CV);
  switch( CV->type) {
  case COV_DIAGONAL :
    if ( CV->m.d) {
#ifdef SSE_OPT
      _mm_free(CV->m.d);
#else
      free(CV->m.d);
#endif
      CV->m.d = NULL;
    }
    break;
  case COV_FULL :
    if ( CV->m.f) { 
      if ( CV->m.f[0]) free(CV->m.f[0]);
      free(CV->m.f);        CV->m.f = NULL; 
    }
    break;
  default :
    break;
  }
  return TCL_OK;  
}

/* ------------------------------------------------------------------------
    dcvConfigureItf   configure the covariance accumulator
   ------------------------------------------------------------------------ */

static int dcvConfigureItf (ClientData cd, char *var, char *val)
{
  DCovMatrix* CV   = (DCovMatrix*)cd;
  if (! CV)   return TCL_ERROR;

  if (! var) {
    ITFCFG(dcvConfigureItf,cd,"maxVarDimX");
    ITFCFG(dcvConfigureItf,cd,"maxVar");
    return TCL_OK;
  }
  ITFCFG_Int  (var,val,"maxVarDimX", CV->maxVarDimX, 0);
  ITFCFG_Float(var,val,"maxVar",     CV->maxVar,     0);
  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    dcvPutsItf    print covariance matrix to TCL string
   ------------------------------------------------------------------------ */

static int dcvPutsItf( ClientData cd, int argc, char *argv[])
{
  DCovMatrix* CV   = (DCovMatrix*)cd;
  int         dimN =  CV->dimN;
  int         i,j;

  itfAppendResult( "{");

  switch( CV->type) {
  case COV_RADIAL :
    itfAppendResult( "% 2.4e", CV->m.r);
    break;

  case COV_DIAGONAL :
    for ( i = 0; i < dimN; i++) itfAppendResult( " % 2.4e", CV->m.d[i]);
    break;

  case COV_FULL :
    for ( i = 0; i < dimN; i++) {
      itfAppendResult( "{");
      for ( j = 0; j < dimN; j++) {
        itfAppendResult( " % 2.4e", CV->m.f[i][j]);
      }
      itfAppendResult( "}%c", (i<dimN-1) ? '\n' : 0);
    }
    break;

  default :
    break;
  }
  itfAppendResult( "}");
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dcvClear    clear the contents of the matrix to 0
   ------------------------------------------------------------------------ */
int dcvClear( DCovMatrix *CV) {
  return dcvConst( CV, 0.0);
}

int dcvConst( DCovMatrix* CV, double val)
{
  int         dimN =  CV->dimN;
  int         i,j;

  switch( CV->type) {

  case COV_RADIAL   : CV->m.r = val; break;
  case COV_DIAGONAL : for ( i = 0; i < dimN; i++) CV->m.d[i] = val;
                      break;
  case COV_FULL     :
    for ( i = 0; i < dimN; i++) {
      for ( j = 0; j < dimN; j++) CV->m.f[i][j] = val;
    }
    break;
  default :
    break;
  }
  return TCL_OK;
}

static int dcvClearItf( ClientData cd, int argc, char *argv[])
{
  DCovMatrix* CV   = (DCovMatrix*)cd;
  int         ac    =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0, NULL) != TCL_OK) return TCL_ERROR;
  return dcvClear( CV);  
}

/* ------------------------------------------------------------------------
    dcvMatch    check whether two convariance matrix accus match such that
                functions like add, copy etc. should work
   ------------------------------------------------------------------------ */

int dcvMatch( DCovMatrix* dst, DCovMatrix* src)
{
  if (src->dimN != dst->dimN) {
     SERROR("Inconsistent dimN: %d %d\n",dst->dimN,src->dimN); 
     return TCL_ERROR;
  }
  if (src->type != dst->type) {
     SERROR("Inconsistent type: %d %d\n",dst->type,src->type);
     return TCL_ERROR;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dcvAdd    add two covariance accumulators
   ------------------------------------------------------------------------ */

int dcvAdd( DCovMatrix* dst, DCovMatrix* src, float factor)
{
  int dimN =  dst->dimN;
  int i,j;

  if ( src                 == dst)    return TCL_OK;
  if ( dcvMatch( dst, src) != TCL_OK) return TCL_ERROR;

  switch( dst->type) {

  case COV_RADIAL : 
    for ( i = 0; i < dimN; i++) dst->m.r += factor * src->m.r;
    break;
  case COV_DIAGONAL :
    for ( i = 0; i < dimN; i++) dst->m.d[i] += factor * src->m.d[i];
    break;
  case COV_FULL     :
    for ( i = 0; i < dimN; i++)
      for ( j = 0; j < dimN; j++) dst->m.f[i][j] += factor * src->m.f[i][j];
    break;
  default :
    break;
  }
  return TCL_OK;
}

static int dcvAddItf( ClientData cd, int argc, char *argv[])
{
  DCovMatrix *dst    = (DCovMatrix*)cd;
  DCovMatrix *src    =  NULL;
  float       factor =  1.0;
  int         ac     =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<source>", ARGV_OBJECT, NULL, &src, "FCovMatrix", 
      "source covariance matrix",
      "-factor",  ARGV_FLOAT,  NULL, &factor, NULL, "scaling factor",
       NULL) != TCL_OK) return TCL_ERROR;

  return dcvAdd( dst, src, factor);
}

/* ------------------------------------------------------------------------
    dcvCopy        copy one covariance matrix into another
   ------------------------------------------------------------------------ */

int dcvCopy( DCovMatrix* dst, DCovMatrix* src)
{
  int dimN =  src->dimN;
  int useN =  dst->useN;
  int i,j;

  if ( src == dst) return TCL_OK;
  if ( dcvDeinit( dst) != TCL_OK ||
       dcvInit(   dst, src->dimN, src->type) != TCL_OK) {
    ERROR("dcvCopy can't allocate covariance matrix.\n");
    return TCL_ERROR;
  }
  dst->useN = useN;

  dst->maxVarDimX = src->maxVarDimX;
  dst->maxVar     = src->maxVar;

  switch( dst->type) {

  case COV_RADIAL : 
    for ( i = 0; i < dimN; i++) dst->m.r = src->m.r;
    break;
  case COV_DIAGONAL :
    for ( i = 0; i < dimN; i++) dst->m.d[i] = src->m.d[i];
    break;
  case COV_FULL     :
    for ( i = 0; i < dimN; i++)
      for ( j = 0; j < dimN; j++) dst->m.f[i][j] = src->m.f[i][j];
    break;
  default :
    break;
  }
  return TCL_OK;
}

static int dcvCopyItf( ClientData cd, int argc, char *argv[])
{
  DCovMatrix *dst   = (DCovMatrix*)cd;
  DCovMatrix *src   =  NULL;
  int         ac    =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<source>", ARGV_OBJECT, NULL, &src, "DCovMatrix", 
      "source covariance matrix",
       NULL) != TCL_OK) return TCL_ERROR;

  return dcvCopy( dst, src);
}

/* ------------------------------------------------------------------------
    dcvTimes   add two covariance accumulators
   ------------------------------------------------------------------------ */

int dcvTimes( DCovMatrix* dst, float scale)
{
  int dimN =  dst->dimN;
  int i,j;

  switch( dst->type) {

  case COV_RADIAL : 
    for ( i = 0; i < dimN; i++) dst->m.r *= scale;
    break;
  case COV_DIAGONAL :
    for ( i = 0; i < dimN; i++) dst->m.d[i] *= scale;
    break;
  case COV_FULL     :
    for ( i = 0; i < dimN; i++)
      for ( j = 0; j < dimN; j++) dst->m.f[i][j] *= scale;
    break;
  default :
    break;
  }
  return TCL_OK;
}

static int dcvTimesItf( ClientData cd, int argc, char *argv[])
{
  DCovMatrix *dst   = (DCovMatrix*)cd;
  float       scale =  1.0;
  int         ac    =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<scale>", ARGV_OBJECT, NULL, &scale, NULL, "scaling factor",
       NULL) != TCL_OK) return TCL_ERROR;

  return dcvTimes( dst, scale);
}

/* ------------------------------------------------------------------------
    dcvAccuAdd     add a new pattern to the covariance accumulator
   ------------------------------------------------------------------------ */

int dcvAccuAdd (DCovMatrix* CV, double* pattern, double* spattern, double factor)
{
  int dimN = CV->dimN;
  int i;

  switch (CV->type) {

  case COV_RADIAL : 
    for  (i = 0; i < dimN; i++)
      CV->m.r += factor * spattern[i];
    break;
  case COV_DIAGONAL :
    for (i = 0; i < dimN; i++) 
      CV->m.d[i] += factor * spattern[i];
    break;
  case COV_FULL     :
    for (i = 0; i < dimN; i++) {
      int j;
      for (j = 0; j < dimN; j++) 
        CV->m.f[i][j] += factor * pattern[i] * pattern[j];
    }
    break;
  default :
    break;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dcvSubspace    find the weight of the subspace defined by the
                   covariance accumulator CV
   ------------------------------------------------------------------------ */

double dcvSubspace( DCovMatrix* CV, float* rv, float* pattern, double factor)
{
  int    maxVarDimX =  CV->maxVarDimX;
  int    sign       = (maxVarDimX > 0) ? 1 : -1;
  double d;

  if (! maxVarDimX) return 1.0;
  else  maxVarDimX = sign * maxVarDimX - 1;

  d  = ( pattern[maxVarDimX] - rv[maxVarDimX]) / (2.0 * CV->maxVar);
  d *=   factor;
  d  =   1.0 / (1.0 + exp( sign * d));

  return d;
}

/* ------------------------------------------------------------------------
    dcvLoad    load covariance matrix parameters from a file written
               using the machine independent I/O routines
   ------------------------------------------------------------------------ */

int dcvLoad( DCovMatrix* CV, float addFactor, FILE* fp)
{
  int         dimN =  CV->dimN;
  int         i,j;

  switch( CV->type) {
  case COV_RADIAL:	
    CV->m.r += addFactor * read_float(fp);
    break;
  case COV_DIAGONAL:
    for ( i = 0; i < dimN; i++) CV->m.d[i] += addFactor * read_float(fp);
    break;
  case COV_FULL:
    for ( i = 0; i < dimN; i++)
      for ( j = 0; j < dimN; j++) CV->m.f[i][j] += addFactor * read_float(fp);
    break;
  default :
    break;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dcvSave    save covariance matrix parameters to a file using the
               machine independent I/O routines
   ------------------------------------------------------------------------ */

int dcvSave( DCovMatrix* CV, FILE* fp)
{
  int         dimN =  CV->dimN;
  int         i,j;

  switch( CV->type) {
  case COV_RADIAL:	
    write_float( fp, CV->m.r);
    break;
  case COV_DIAGONAL:
    for ( i = 0; i < dimN; i++) write_float( fp, CV->m.d[i]);
    break;
  case COV_FULL:
    for ( i = 0; i < dimN; i++)
      for ( j = 0; j < dimN; j++) write_float( fp, CV->m.f[i][j]);
    break;
  default :
    break;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dcvSkip     read covariance information in a codebook accumulator file,
                but do not store it.
   ------------------------------------------------------------------------ */

int dcvSkip( int dimN, CovType type, FILE* fp)
{
  int         i,j;

  switch( type) {
  case COV_RADIAL:  read_float(fp); break;
  case COV_DIAGONAL:
    for ( i = 0; i < dimN; i++) read_float(fp);
    break;
  case COV_FULL:
    for ( i = 0; i < dimN; i++)
      for ( j = 0; j < dimN; j++) read_float(fp);
    break;
  default :
    break;
  }
  return TCL_OK;
}

/* ========================================================================
    Covariance Matrix Functions
   ======================================================================== */
/* ------------------------------------------------------------------------
    cvStrToType   converts a string representation of the covariance type 
                  to a CovType identification
   ------------------------------------------------------------------------ */

CovType cvStrToType( char* typeName)
{
  if      (! strcmp( typeName, "NO"))       return COV_NO;
  else if (! strcmp( typeName, "RADIAL"))   return COV_RADIAL;
  else if (! strcmp( typeName, "DIAGONAL")) return COV_DIAGONAL;
  else if (! strcmp( typeName, "FULL"))     return COV_FULL;
  return COV_ERR;
}

/* ------------------------------------------------------------------------
    cvTypeToStr   converts covariance type identification to a string 
   ------------------------------------------------------------------------ */

char* cvTypeToStr( CovType type)
{
  switch ( type) {
  case COV_NO:       return "NO";
  case COV_RADIAL:   return "RADIAL";
  case COV_DIAGONAL: return "DIAGONAL";
  case COV_FULL:     return "FULL";
  case COV_ERR:      return "ERROR";
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    cvGetTypeItf   get covariance type (ParseArgv compatible).
   ------------------------------------------------------------------------ */

int cvGetTypeItf( ClientData CD,     char* key, 
                  ClientData retPtr, int*  argcPtr, char* argv[])
{
  CovType idx;
  int     res_idx = -1;
  int     ac      = *argcPtr;

  if ( ac < 1) return -1;
  /* gcc with all warnings on means that
     ((idx = cvStrToType( argv[0])) < 0)
     is allwais true! So I introduce res_idx
     to save the -1 in case of a failure */
  if ((res_idx = cvStrToType( argv[0])) < 0) {

    ERROR( "No valid covariance type '%s'.\n", argv[0]);
    return -1;
  }
  idx = (CovType) res_idx;

  (*argcPtr)--; 
  *((CovType*)retPtr) = idx; 
  return 0; 
}

/* ========================================================================
    Type Declaration
   ======================================================================== */


static Method fcovarMethod[] = 
{ 
  { "puts",     fcvPutsItf,      NULL },
  { "clear",    fcvClearItf,     "clear the contents of an covariance accumulator" },
  { "set",      fcvSetItf,       "set the covariance matrix" },
  { ":=",       fcvCopyItf,      "copy covariance matrix" },
  { "+=",       fcvAddItf,       "add two scaled covariance matrices" },
  { "variances",fcvVariancesItf, "returns a list of the variances along the axis" },
  {  NULL,  NULL,       NULL } 
};

TypeInfo fcovarInfo = { "FCovMatrix", 0, 0, fcovarMethod,
                         NULL, NULL,
                         fcvConfigureItf, NULL, itfTypeCntlDefaultNoLink,
	                "Covariance matrix type (float)" } ;

static Method dcovarMethod[] = 
{ 
  { "puts",  dcvPutsItf,   NULL },
  { "clear", dcvClearItf, "clear the contents of an covariance accumulator" },
  { ":=",    dcvCopyItf,  "copy covariance matrix" },
  { "+=",    dcvAddItf,   "add two covariance accumulators" },
  { "*=",    dcvTimesItf, "scale the covariance accumulator" },
  {  NULL,   NULL,         NULL } 
};

TypeInfo dcovarInfo = { "DCovMatrix", 0, 0, dcovarMethod,
                         NULL, NULL,
                         dcvConfigureItf, NULL, itfTypeCntlDefaultNoLink,
	                "Covariance matrix type (double)" } ;

static int itfCovarInitialized = 0;

int Covar_Init(void)
{
  if (! itfCovarInitialized) {
    itfNewType(&fcovarInfo);
    itfNewType(&dcovarInfo);
  }
  return TCL_OK;
}
