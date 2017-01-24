/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: semi tied covariances
               ------------------------------------------------------------

    Author  :  Matthias Seeger
    Module  :  cbn_global.h
    Date    :  $Id: cbn_global.h 2762 2007-02-22 16:28:00Z fuegen $
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
    Revision 1.4  2007/02/22 16:28:00  fuegen
    removed -pedantic compiler warnings with

    Revision 1.3  2003/08/14 11:20:25  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.2.52.2  2002/03/15 16:49:02  soltau
    Disabled deboug output

    Revision 1.2.52.1  2002/03/03 18:17:58  soltau
    Fixed freeDMat
    increased blocksize for cbnewSet

    Revision 1.2  2000/08/27 15:31:09  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 1.1.4.1  2000/08/25 22:19:42  jmcd
    Just checking.

    Revision 1.1  2000/08/22 16:42:54  soltau
    Initial revision


   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _cbn_global
#define _cbn_global

#include "matrix.h"

/* -------------------------------------------------------------------
   There are several independent debug levels:
   - DEBUG: Assertions and '<= called by' messages
   - DEBUG2: Many report messages (using WARN).
   ------------------------------------------------------------------- */

/*
  #define DEBUG  
  #define DEBUG2 
*/

/* --- Global constants --- */

#define TCL_ERROR 1  /* Janus constants */
#define TCL_OK 0

#define LOG_TWO_PI 1.837877066409345483560659472810 /* log(2*pi) */

/* The following constants are used during codebook update. A Gaussian mean
   will only be updated if its exp. number of samples (i.e. its accuGammaS
   value) is > MEAN_UPDATE_THRES. A Gaussian's 'diag' will only be updated
   if the corr. mean has been updated and accuGammaS >= DIAG_UPDATE_THRES
   (ensure DIAG_UPDATE_THRES > MEAN_UPDATE_THRES to keep this consistent!).
   This rule is valid not only during codebook update but also during
   'diag' reestimation (after having changed a parameter matrix).
   If a 'diag' is updated, its components are prevented from falling to
   zero by using the floor value DIAG_COMP_FLOOR.
   A Gaussian will be deactivated if its accuGammaS value falls below
   GAMMA_SUM_THRES.

   Note that the values below and the rules above corr. to the values used
   in the Janus Codebook class (see cbAccuMLE in codebook.c) */

#define MEAN_UPDATE_THRES 1.00
#define DIAG_UPDATE_THRES 6.00
#define DIAG_COMP_FLOOR   1.0e-19
#define GAMMA_SUM_THRES   1.0e-19
#define HMAT_SVAL_THRES 0.001  /* see 'cbnewparmatUpdate' */

#define CONSTRUCTION_PHASE 0  /* CBNewSet object phases */
#define WORKING_PHASE 1
#define TESTING_PHASE 3

/* main module constants */
#define ACCUS_EMPTY 0  /* CBNew accumulator states */
#define ACCUS_LOCKED 1
#define ACCUS_BUILDING 2

#define CBS_LIST_BLKSIZE 50000  /* CBNewSet list blocksize  */
#define PROT_LIST_INCR_SIZE 10  /* Size of incr. of protocol slots list */
#define NUM_CRIT_FUNCS 1
#define DEF_CRIT (&cbnewCritLogLH)

/* parmat module constants */
#define NUM_CLUSTER_FUNCS 1
#define CLUSFUNC_ML 1  /* Codes for clustering procedures */

#define CBPMS_LIST_BLKSIZE 5000  /* CBNewParMatrixSet list blocksize */
#define LINK_LIST_INCR_SIZE 100  /* Size of incr. of link list */
#define DEF_LEARN_RATE 0.1
#define DEF_FIRST_SMALL_STEPS 40  /* see 'update' method */
#define DEF_SMALL_STEPS 20
#define DEF_NUM_STEPS 100
#define DEF_DELTA_THRES 0.05
#define CLUSTER_PMNAME "pmat%d-%d"

/* There are three different coding of cbnewparmatUpdate. Uncomment
   exactly one of the following tags to activate the corr. coding.
   See 'cbnewparmatUpdate' for description.
   Experiments have shown version 1 and 2 nearly equivalent while
   version 3 is inadaequate. Version 2 is conceptionally simpler and
   slightly more efficient and should be used. */
/*#define PM_UPDATE_VERSION_1*/
#define PM_UPDATE_VERSION_2
/*#define PM_UPDATE_VERSION_3*/


/* --- Global macros --- */

/* Index function for the upper triangular storing format of symmetric
   matrices. */
#define TRIANGLE_POS(row,col,dim) ((row)*(2*(dim) - 1 - (row)) + (col))

/* Macros for memory management */
#define GETMEM(var,type,size)\
{\
  if (!((var) = (type*) malloc ((size) * sizeof(type)))) {\
    ERROR("Out of memory! (Failed to allocate %d items of size %d)\n",size,sizeof(type));\
    return TCL_ERROR;\
  }\
}

#define GETMEMC(var,type,size)\
{\
  int i;\
  if (!((var) = (type*) malloc ((size) * sizeof(type)))) {\
    ERROR("Out of memory!\n");\
    return TCL_ERROR;\
  }\
  for (i = 0; i < (size); i++) (var)[i] = 0;\
}

#define GETMEMX(var,type,size,func)\
{\
  if (!((var) = (type*) malloc ((size) * sizeof(type)))) {\
    ERROR("Out of memory!\n");\
    func;\
    return TCL_ERROR;\
  }\
}

#define GETMEMXC(var,type,size,func)\
{\
  int i;\
  if (!((var) = (type*) malloc ((size) * sizeof(type)))) {\
    ERROR("Out of memory!\n");\
    func;\
    return TCL_ERROR;\
  }\
  for (i = 0; i < (size); i++) (var)[i] = 0;\
}

#define FREE(ptr)\
{\
  if (ptr) {\
    free (ptr);\
    ptr = 0;\
  }\
}

#define GETFMAT(var,m,n)\
{\
  if (!((var) = fmatrixCreate (m,n))) {\
    ERROR("Failed to create matrix!\n");\
    return TCL_ERROR;\
  }\
}

#define GETDMAT(var,m,n)\
{\
  if (!((var) = dmatrixCreate (m,n))) {\
    ERROR("Failed to create matrix!\n");\
    return TCL_ERROR;\
  }\
}

#define FREEFMAT(var)\
{\
  if (var) {\
    fmatrixFree (var);\
    var = 0;\
  }\
}

#define FREEDMAT(var)\
{\
  if (var) {\
    dmatrixFree (var);\
    var = 0;\
  }\
}
    
#endif

#ifdef __cplusplus
}
#endif
