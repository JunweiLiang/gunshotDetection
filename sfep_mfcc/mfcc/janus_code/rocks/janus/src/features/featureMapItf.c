/* ========================================================================
    JANUS      Speech Recognition Toolkit
               ------------------------------------------------------------
               Methods: FeatureSet map
               ------------------------------------------------------------

    Author  :  Martin Westphal
    Module  :  featureMapItf.c
    Date    :  $Id: featureMapItf.c 2864 2009-02-16 21:18:17Z metze $
    Remarks :

   ========================================================================

    $ Log: featureMapItf.c,v $
    Revision 4.3  2008/11/24 14:52:28  wolfel
    added non-linear mapping by VTS

    Revision 4.2  2000/11/14 12:25:53  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 4.1.26.2  2000/11/08 17:09:25  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 4.1.26.1  2000/11/06 10:50:28  janus
    Made changes to enable compilation under -Wall.

    Revision 4.1  2000/09/14 11:37:00  janus
    merging branch jtk-00-09-11-fuegen-2

 * Revision 1.1.4.1  2000/09/11  16:04:40  janus
 * Merging outdated branch jtk-00-09-08-fuegen.
 * 
    Revision 1.1.2.1  2000/09/08 15:03:01  janus
    added M. Westphal signal adaption (MAM)

  
   ======================================================================== */

char featureMapItfRCSVersion[]= 
           "@(#)1$Id: featureMapItf.c 2864 2009-02-16 21:18:17Z metze $";


/* ========================================================================
    includes
   ======================================================================== */
#include "featureMethodItf.h"           /* includes: Audio, PLP, IO         */
#include "emath.h"

/* ========================================================================
    macros
   ======================================================================== */

/* ========================================================================
   Method 'map'
   acoustic mapping
   ======================================================================== */
int fesMapItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin  = NULL;
  Feature     *source1 = NULL;
  Feature     *source2 = NULL;
  FMatrix     *matrix  = NULL;
    
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
	"<new feature>",  ARGV_CUSTOM, createFe,&destin,  cd,
		          "estimate for environment 2",
	"<featureA>", ARGV_CUSTOM, getFe,   &source1,     cd, "features from environment 1",
	"<featureB>", ARGV_CUSTOM, getFe,   &source2,     cd, "probs for each class",
        "<matrix>",   ARGV_CUSTOM, getFMatrix,&matrix,    cd, "FMatrix with shift vectors",
	NULL) != TCL_OK) return TCL_ERROR;

  /* -------------------
     FMatrix features?
     ------------------- */
  if (BOTH_FMATRIX( source1, source2)) {
     FMatrix *mat1 = source1->data.fmat;
     FMatrix *mat2 = source2->data.fmat;
     FMatrix *mat;
     int frameA = 0;
     int frameX, coeffX, classX;
     int frameN  = mat1->m;
     int coeffN  = mat1->n;
     int frameN2 = mat2->m;
     int classN  = mat2->n;
     int classN2 = matrix->m;
     int coeffN2 = matrix->n;

     /* -- compare frameN, coeffN and classN -- */
     if (FS->runon && !FS->ready) {
       frameN = frameN <= frameN2 ? frameN : frameN2; 
     } else {
       if (frameN != frameN2) {
	 ERROR("frame numbers of features differ: '%s' (%d), '%s' (%d)\n",source1->name, frameN, source2->name, frameN2);
	 return TCL_ERROR;
       }
     }
     if (coeffN != coeffN2) {
       ERROR("coeffient numbers differ: '%s' (%d), <matrix> (%d)\n",source1->name, coeffN, coeffN2);
       return TCL_ERROR;
     }
     if (classN != classN2) {
       ERROR("class numbers differ: '%s' (%d), <matrix> (%d)\n",source2->name, classN, classN2);
       return TCL_ERROR;
     }

     mat = fmatrixFeature( frameN, coeffN, FS, destin, &frameA);
     if (!mat) return TCL_ERROR;
          
     for ( frameX=frameA; frameX<frameN; frameX++) {
       for (coeffX=0; coeffX<coeffN; coeffX++) {
	 float f = mat1->matPA[frameX][coeffX];
	 for (classX=0; classX<classN; classX++)
	   f += mat2->matPA[frameX][classX] * matrix->matPA[classX][coeffX];
	 mat->matPA[frameX][coeffX] = f;
       }
     }
     if (!feFMatrix( FS, destin, mat, 1)) {
        COPY_CONFIG( destin, source1);  return TCL_OK;
     }
     fmatrixFree(mat); return TCL_ERROR;
  }
  MUST_FMATRIX2(source1,source2);
  return TCL_ERROR;
}

/******************************************************************************
   input 
 *****************************************************************************/
float input(float y, float n)
{
    float               diff, mod, imputedX;
    /*    double              normalizer; */

    diff = n - y;
    mod = 1.0 - exp10F(diff*0.1);
    imputedX = y + 10.0*log10F(mod);
    return(imputedX);
}

/* ========================================================================
   Method 'vts' 
   acoustic mapping using the vts approximation
   ======================================================================== */
int fesVTSItf( ClientData cd, int argc, char *argv[])
{
  FeatureSet  *FS = (FeatureSet*)cd;
  Feature     *destin  = NULL;
  Feature     *source1 = NULL;
  Feature     *source2 = NULL;
  FMatrix     *matrix  = NULL;
    
  /* ------------------
     get arguments
     ------------------ */
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
	"<new feature>",  ARGV_CUSTOM, createFe,&destin,  cd,
		          "estimate for environment 2",
	"<featureA>", ARGV_CUSTOM, getFe,   &source1,     cd, "features from environment 1",
	"<featureB>", ARGV_CUSTOM, getFe,   &source2,     cd, "probs for each class",
        "<matrix>",   ARGV_CUSTOM, getFMatrix,&matrix,    cd, "FMatrix with shift vectors",
	NULL) != TCL_OK) return TCL_ERROR;

  /* -------------------
     FMatrix features?
     ------------------- */
  if (BOTH_FMATRIX( source1, source2)) {
     FMatrix *mat1 = source1->data.fmat;
     FMatrix *mat2 = source2->data.fmat;
     FMatrix *mat;
     int frameA = 0;
     int frameX, coeffX, classX;
     int frameN  = mat1->m;
     int coeffN  = mat1->n;
     int frameN2 = mat2->m;
     int classN  = mat2->n;
     int classN2 = matrix->m;
     int coeffN2 = matrix->n;

     /* -- compare frameN, coeffN and classN -- */
     if (FS->runon && !FS->ready) {
       frameN = frameN <= frameN2 ? frameN : frameN2; 
     } else {
       if (frameN != frameN2) {
	 ERROR("frame numbers of features differ: '%s' (%d), '%s' (%d)\n",source1->name, frameN, source2->name, frameN2);
	 return TCL_ERROR;
       }
     }
     if (coeffN != coeffN2) {
       ERROR("coeffient numbers differ: '%s' (%d), <matrix> (%d)\n",source1->name, coeffN, coeffN2);
       return TCL_ERROR;
     }
     if (classN != classN2) {
       ERROR("class numbers differ: '%s' (%d), <matrix> (%d)\n",source2->name, classN, classN2);
       return TCL_ERROR;
     }

     mat = fmatrixFeature( frameN, coeffN, FS, destin, &frameA);
     if (!mat) return TCL_ERROR;
          
     for ( frameX=frameA; frameX<frameN; frameX++) {
       for (coeffX=0; coeffX<coeffN; coeffX++) {
	 /* float f = mat1->matPA[frameX][coeffX]; */
	 float f = 0; /* Initialization with 0 good? (Florian) */
	 for (classX=0; classX<classN; classX++)
	   /* f += mat2->matPA[frameX][classX] * matrix->matPA[classX][coeffX]; */
	     f += mat2->matPA[frameX][classX] * input(mat1->matPA[frameX][coeffX], matrix->matPA[classX][coeffX]);
	 mat->matPA[frameX][coeffX] = f;
       }
     }
     if (!feFMatrix( FS, destin, mat, 1)) {
        COPY_CONFIG( destin, source1);  return TCL_OK;
     }
     fmatrixFree(mat); return TCL_ERROR;
  }
  MUST_FMATRIX2(source1,source2);
  return TCL_ERROR;
}

