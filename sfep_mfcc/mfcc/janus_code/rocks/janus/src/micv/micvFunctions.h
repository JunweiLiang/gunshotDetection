/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: micv
               ------------------------------------------------------------

    Author  :  Andreas Eller
    Module  :  micv
    Date    :  $Id: micvFunctions.h 2633 2005-03-08 17:30:28Z metze $
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
    Revision 1.1  2005/03/08 17:30:28  metze
    Initial code as of Andreas Eller


   ======================================================================== */

#ifndef _MICVFUNCTIONS
#define _MICVFUNCTIONS

#ifdef __cplusplus
extern "C" {
#endif

#include "matrix.h"
#include "micv.h"

//-----------------------------------------------------------------------------
// typedefs / structs
//-----------------------------------------------------------------------------
typedef struct MicvMatrixClusterSet
{
	DMatrix*	pMatrix;
	DMatrix*	pInvMatrix;
	int			nAssignedTo;
} MicvMatrixClusterSet;


//-----------------------------------------------------------------------------
// function declarations
//-----------------------------------------------------------------------------

DMatrix*	micv_symMatrixToVectorRepresentation(DMatrix* A, const DMatrix* B);
DMatrix*	micv_vectorRepresentationToSymMatrix(DMatrix* A, const DMatrix* B);
double		micv_VectorSqrNorm(double* v, int nDim);
double		micv_VectorNorm(const DMatrix* A);
double		micv_MatrixNorm(const DMatrix* A);
void		micv_zeroMatrix(DMatrix* A);
BOOL		micv_IsNullMatrix(const DMatrix* A);
double		micv_matrixClusterDist(const MicvMatrixClusterSet* pSet1, const MicvMatrixClusterSet* pSet2);
double		micv_matrixDist(DMatrix* A, DMatrix* B);
double		micv_matrixTrace(DMatrix* A);
void		micv_clusterMatrices(DMatrix** pTarget, const int nTarget, DMatrix** pSource, const int nSource, const int nIterations);
BOOL		micv_isPositiveDefinite(DMatrix* pMatrix);
BOOL		micv_isNegativeDefinite(DMatrix* pMatrix);
BOOL		micv_isSymmetric(DMatrix* pMatrix);
void		micv_makeSymmetric(DMatrix* A);

int			micv_matrixRank(DMatrix* A);


//-----------------------------------------------------------------------------
// display functions
//-----------------------------------------------------------------------------

void		micv_printMatrix(const DMatrix* A, char* sNumberFormat);


//-----------------------------------------------------------------------------
// helper functions
//-----------------------------------------------------------------------------

char*		micv_strRemoveWhitespace(char* pString);


//-----------------------------------------------------------------------------
// debugging functions
//-----------------------------------------------------------------------------

#ifdef _DEBUG
void micv_AssertMatrix(const DMatrix* A);
void micv_AssertVector(const DVector* V);
#endif


#ifdef __cplusplus
}
#endif

#endif	// _MICVFUNCTIONS

