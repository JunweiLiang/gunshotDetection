/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: micv
               ------------------------------------------------------------

    Author  :  Andreas Eller
    Module  :  micv
    Date    :  $Id: micvFunctions.c 2633 2005-03-08 17:30:28Z metze $
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

#include "micvFunctions.h"
#include "ffmat.h"
#include "matrix.h"
#include "math.h"



//*****************************************************************************
//	micv functions
//*****************************************************************************

// ----------------------------------------------------------------------------
//	transform the symetric matrix B into vector representation A
// ----------------------------------------------------------------------------
DMatrix* micv_symMatrixToVectorRepresentation(DMatrix* A, const DMatrix* B)
{
	int i = 0;
	int m, n;
	int dim = B->m;
	float f = sqrt(2);

	assert(B->m == B->n);

	dmatrixResize(A, dim * (dim + 1) / 2, 1);

	// A* = [b(0), sqrt(2)*b(1), aqrt(2)*b(2), ..., sqrt(2)*b(D-1)]

	i = 0;
	for(m=0; m < dim; m++, i++)
		A->matPA[i][0] = B->matPA[m][m];

	for(n=1; n < dim; n++)
		for(m=0; m < dim - n; m++, i++)
			A->matPA[i][0] = f * B->matPA[m][n + m];

	return A;
}


// ----------------------------------------------------------------------------
//	transform the symetric matrix B from vector representation to normal
//	representated symmetric matrix A
// ----------------------------------------------------------------------------
DMatrix* micv_vectorRepresentationToSymMatrix(DMatrix* A, const DMatrix* B)
{
	int i,m,n;
	int dim, dim_;
	float f = 1.0;

	assert(A->m == A->n);

	dim = A->m;
	dim_ = dim * (dim + 1) / 2;

	dmatrixResize(A, dim, dim);

	for(n=0, i=0; n < dim; n++)
	{
		for(m=0; m < dim - n; m++, i++)
		{
			A->matPA[m][n + m] = f * B->matPA[i][0];
			A->matPA[n + m][m] = f * B->matPA[i][0];
		}
		f = 1.0 / sqrt(2);
	}

	return A;
}


// ----------------------------------------------------------------------------
//	calculate vector norm²
// ----------------------------------------------------------------------------
double micv_VectorSqrNorm(double* v, int nDim)
{
	double fNorm = 0.0;

	while(nDim--)
	{
		fNorm += (*v) * (*v);
		v++;
	}

	return fNorm;
}


// ----------------------------------------------------------------------------
//	calculate vector norm of (n x 1) matrix A
// ----------------------------------------------------------------------------
double micv_VectorNorm(const DMatrix* A)
{
	return sqrt(micv_VectorSqrNorm(A->matPA[0], A->m));
}


// ----------------------------------------------------------------------------
//	calculate matrix norm ||A||oo
// ----------------------------------------------------------------------------
double micv_MatrixNorm(const DMatrix* A)
{
	// norm = max row sum
	int m,n;
	double fMax = 0.0;
	double fAccu;

	for(m=0; m < A->m; m++)
	{
		fAccu = 0.0;
		for(n=0; n < A->n; n++)
			fAccu += fabs(A->matPA[m][n]);

		if(fAccu > fMax)
			fMax = fAccu;
	}

	return fMax;
}


// ----------------------------------------------------------------------------
//	sets all matrix elements to zero
// ----------------------------------------------------------------------------
void micv_zeroMatrix(DMatrix* A)
{
	memset(A->matPA[0], 0, sizeof(double) * A->m * A->n);
}


// ----------------------------------------------------------------------------
//	returns TRUE if matrix is null matrix
// ----------------------------------------------------------------------------
BOOL micv_IsNullMatrix(const DMatrix* A)
{
	int m,n;

	for(m=0; m < A->m; m++)
		for(n=0; n < A->n; n++)
			if(A->matPA[m][n] != 0)
				return FALSE;

	return TRUE;
}


// ----------------------------------------------------------------------------
//	calculate distance between matrix A and B
// ----------------------------------------------------------------------------
double micv_matrixDist(DMatrix* A, DMatrix* B)
{
	int m, n;
	int dim = A->m;
	DMatrix *inv = dmatrixCreate(dim, dim);
	double fDist = 0.0;

	// d = A*_Trans * B*_Inv + B*_Trans * A*_Inv

	// first term  (A*_Trans * B*_Inv)
	dmatrixInv(inv, B);
	for(m=0; m < dim; m++)
		fDist += A->matPA[m][m] * inv->matPA[m][m];
	for(n=1; n < dim; n++)
		for(m=0; m < dim - n; m++)
			fDist += 2 * A->matPA[m][n+m] * inv->matPA[m][n+m];

	// second term  (B*_Trans * A*_Inv)
	dmatrixInv(inv, A);
	for(m=0; m < dim; m++)
		fDist += B->matPA[m][m] * inv->matPA[m][m];
	for(n=1; n < dim; n++)
		for(m=0; m < dim - n; m++)
			fDist += 2 * B->matPA[m][n+m] * inv->matPA[m][n+m];

	dmatrixFree(inv);

	return fDist;
}


// ----------------------------------------------------------------------------
//	for PT clustering optimized version of micv_matrixDist
// ----------------------------------------------------------------------------
double micv_matrixClusterDist(const MicvMatrixClusterSet* pSet1, const MicvMatrixClusterSet* pSet2)
{
	int m, n;
	int dim = pSet1->pMatrix->m;
	DMatrix *A, *B;
	double fDist = 0.0;

	// d = A*_Trans * B*_Inv + B*_Trans * A*_Inv

	// first term  (A*_Trans * B*_Inv)
	A = pSet1->pMatrix;
	B = pSet2->pInvMatrix;
	for(m=0; m < dim; m++)
		fDist += A->matPA[m][m] * B->matPA[m][m];
	for(n=1; n < dim; n++)
		for(m=0; m < dim - n; m++)
			fDist += 2 * A->matPA[m][n+m] * B->matPA[m][n+m];

	// second term  (B*_Trans * A*_Inv)
	A = pSet1->pInvMatrix;
	B = pSet2->pMatrix;
	for(m=0; m < dim; m++)
		fDist += B->matPA[m][m] * A->matPA[m][m];
	for(n=1; n < dim; n++)
		for(m=0; m < dim - n; m++)
			fDist += 2 * B->matPA[m][n+m] * A->matPA[m][n+m];

	return fDist;
}


// ----------------------------------------------------------------------------
//	calculate trace of square matrix A
// ----------------------------------------------------------------------------
double micv_matrixTrace(DMatrix* A)
{
	int i;
	double res = 0;
	for(i=0; i < A->m; i++)
		res += A->matPA[i][i];
	return res;
}


// ----------------------------------------------------------------------------
//	cluster nSource to nTarget matrices
//	- pTarget should be initialized with the initial centroid matrices
//	- pSource should be square and symmetric matrices
// ----------------------------------------------------------------------------
void micv_clusterMatrices(DMatrix** pTarget, const int nTarget, DMatrix** pSource, const int nSource, const int nIterations)
{
	int vq, k, i, n;
	MicvMatrixClusterSet* pClusterSource;
	MicvMatrixClusterSet* pClusterDest;
	DMatrix** pCentroidAccu;		// centroid accumulators
	int* ck;						// #matrices used for the centroid accumulators
	float idxDist, dist;
	double fOverallDist;
	int idx, idxValue;
	int nDim;

	assert(pTarget && pSource);
	assert(nTarget > 0);
	assert(nSource > nTarget);

	nDim = pTarget[0]->m;

	// create cluster source / dest structure
	// this structure is for optimization and allows to precompute the inverse matrices for the matrixDist function
	fprintf(STDERR, "preparing cluster algorithm\n");
	VERIFY_MALLOC(pClusterSource = malloc(sizeof(MicvMatrixClusterSet) * nSource), return);
	for(i=0; i < nSource; i++)
	{
		pClusterSource[i].pMatrix = pSource[i];
		pClusterSource[i].pInvMatrix = dmatrixCreate(nDim, nDim);
		dmatrixInv(pClusterSource[i].pInvMatrix, pSource[i]);
		pClusterSource[i].nAssignedTo = -1;
	}
	VERIFY_MALLOC(pClusterDest = malloc(sizeof(MicvMatrixClusterSet) * nTarget), return);
	VERIFY_MALLOC(pCentroidAccu = malloc(sizeof(DMatrix*) * nTarget), return);
	for(i=0; i < nTarget; i++)
	{
		pClusterDest[i].pMatrix = pTarget[i];
		pClusterDest[i].pInvMatrix = dmatrixCreate(nDim, nDim);
		pCentroidAccu[i] = dmatrixCreate(nDim, nDim);
	}
	VERIFY_MALLOC(ck = malloc(sizeof(int) * nTarget), return);


	// do the clustering
	fprintf(STDERR, "clustering %d covariances to %d prototypes\n", nSource, nTarget);
	for(vq=0; vq < nIterations; vq++)
	{
		fprintf(STDERR, "iteration %d: ", vq+1);

		// precalc inverse centroids and reset centroid accumulators
		for(i=0; i < nTarget; i++)
		{
			dmatrixInv(pClusterDest[i].pInvMatrix, pClusterDest[i].pMatrix);
			dmatrixInitConst(pCentroidAccu[i], 0.0);
			ck[i] = 0;
		}

		// go through all source matrices and find closest prototype
		fOverallDist = 0.0;
		for(i=0; i < nSource; i++)
		{
			idxDist = micv_matrixClusterDist(&pClusterSource[i], &pClusterDest[0]);
			idx = 0;

			for(k=1; k < nTarget; k++)
			{
				dist = micv_matrixClusterDist(&pClusterSource[i], &pClusterDest[k]);
				if(dist < idxDist || (dist == idxDist && ck[k] < ck[idx]))
				{
					idxDist = dist;
					idx = k;
				}
			}
			dmatrixAccu(pCentroidAccu[idx], pClusterSource[i].pMatrix, 1.0);
			pClusterSource[i].nAssignedTo = idx;
			ck[idx]++;
			fOverallDist += idxDist;
		}

		// reestimate centroids
		for(k=0; k < nTarget; k++)
		{
			dmatrixScale(pClusterDest[k].pMatrix, pCentroidAccu[k], 1.0 / ck[k]);
			fprintf(STDERR, "%3d ", ck[k]);
		}
		fprintf(STDERR, " overall dist = %.3f\n", fOverallDist);

		// fix unused centroids
		for(k=0; k < nTarget; k++)
		{
			if(ck[k] == 0)
			{
				// detected unused centroid
				fprintf(STDERR, "detected unused centroid %d: ", k);

				// find centroid with the most assigned matrices
				idx = 0;
				idxValue = ck[0];
				for(i=0; i < nTarget; i++)
					if(ck[i] > idxValue)
					{
						idx = i;
						idxValue = ck[i];
					}

				// split assigned matrices in two parts and calculate two new centroids
				dmatrixInitConst(pClusterDest[k].pMatrix, 0.0);
				dmatrixInitConst(pClusterDest[idx].pMatrix, 0.0);
				n = 0;
				ck[k] = ck[idx] / 2;
				ck[idx] -= ck[idx] / 2;
				for(i=0; i < nSource; i++)
				{
					if(pClusterSource[i].nAssignedTo == idx)
					{
						// assign the first ck[idx] matrices to centroid[idx], the others to centroid[k]
						if(n < ck[idx])
						{
							dmatrixAccu(pClusterDest[idx].pMatrix, pClusterSource[i].pMatrix, 1.0);
							n++;
						} else {
							dmatrixAccu(pClusterDest[k].pMatrix, pClusterSource[i].pMatrix, 1.0);
							pClusterSource[i].nAssignedTo = k;
						}
					}
				}
				dmatrixScale(pClusterDest[idx].pMatrix, pClusterDest[idx].pMatrix, 1.0 / ck[idx]);
				dmatrixScale(pClusterDest[k].pMatrix, pClusterDest[k].pMatrix, 1.0 / ck[k]);
				fprintf(STDERR, "splitting centroid %d (%d) into %d (%d) and %d (%d)\n", idx, idxValue, idx, ck[idx], k, ck[k]);
			}
		}
		
	}

	// prototype validation and free memory
	for(k=0; k < nTarget; k++)
	{
		if(!micv_isPositiveDefinite(pTarget[k]))
			WARN("initial prototype %d is not positive definite -> weights initialization may cause trouble!\n", k);
		if(!micv_isSymmetric(pTarget[k]))
			ERROR("initial prototype %d is not symmetric -> the cluster algorithm may be buggy!\n", k);
		dmatrixFree(pCentroidAccu[k]);
		dmatrixFree(pClusterDest[k].pInvMatrix);
	}
	free(pClusterDest);
	free(pCentroidAccu);
	free(ck);

	// free cluster source structure
	for(i=0; i < nSource; i++)
		dmatrixFree(pClusterSource[i].pInvMatrix);
	free(pClusterSource);
}


// ----------------------------------------------------------------------------
//	checks if a matrix is positive definite
// ----------------------------------------------------------------------------
BOOL micv_isPositiveDefinite(DMatrix* pMatrix)
{
	int i;
	DMatrix *pEigenVectors = dmatrixCreate(pMatrix->m, pMatrix->n);
	DMatrix *pEigenValues = dmatrixCreate(pMatrix->m, 1);
	BOOL bPositiveDefinite = TRUE;

	_dmatrixEigen(pEigenVectors, pEigenValues, pMatrix);
	for(i=0; i < pMatrix->m; i++)
		if(pEigenValues->matPA[0][i] <= 0)
		{
			bPositiveDefinite = FALSE;
			break;
		}

	dmatrixFree(pEigenValues);
	dmatrixFree(pEigenVectors);

	return bPositiveDefinite;
}


// ----------------------------------------------------------------------------
//	checks if a matrix is negative definite
// ----------------------------------------------------------------------------
BOOL micv_isNegativeDefinite(DMatrix* pMatrix)
{
	int i;
	DMatrix *pEigenVectors = dmatrixCreate(pMatrix->m, pMatrix->n);
	DMatrix *pEigenValues = dmatrixCreate(pMatrix->m, 1);
	BOOL bNegativeDefinite = TRUE;

	_dmatrixEigen(pEigenVectors, pEigenValues, pMatrix);
	for(i=0; i < pMatrix->m; i++)
		if(pEigenValues->matPA[0][i] >= 0)
		{
			bNegativeDefinite = FALSE;
			break;
		}

	dmatrixFree(pEigenValues);
	dmatrixFree(pEigenVectors);

	return bNegativeDefinite;
}


// ----------------------------------------------------------------------------
//	checks if a matrix is symmetric
// ----------------------------------------------------------------------------
BOOL micv_isSymmetric(DMatrix* pMatrix)
{
	int m, n;

	if(pMatrix->m != pMatrix->n)
		return FALSE;

	for(m=0; m < pMatrix->m; m++)
		for(n=m+1; n < pMatrix->n; n++)
			if(pMatrix->matPA[m][n] != pMatrix->matPA[n][m])
				return FALSE;

	return TRUE;
}


// ----------------------------------------------------------------------------
//	makes a square matrix symmetric
// ----------------------------------------------------------------------------
void micv_makeSymmetric(DMatrix* A)
{
	int m, n;

	for(m=0; m < A->m; m++)
		for(n=m+1; n < A->n; n++)
			A->matPA[n][m] = A->matPA[m][n];
}


// ----------------------------------------------------------------------------
//	calculates the rank of a matrix
// ----------------------------------------------------------------------------
int micv_matrixRank(DMatrix* pMatrix)
{
	int i, m, n;
	int minDim = (pMatrix->m < pMatrix->n)?pMatrix->m:pMatrix->n;
	double f;
	double fRowSum;
	int fRank = minDim;
	BOOL bFoundPivot;
	DMatrix* A = dmatrixCreate(pMatrix->m, pMatrix->n);

	dmatrixCopy(A, pMatrix);

	for(i = 0; i < minDim; i++)
	{
		// find pivot element
		bFoundPivot = FALSE;
		for(m = i; m < A->m; m++)
			if(A->matPA[m][i] != 0)
			{
				bFoundPivot = TRUE;
				if(m != i)
					for(n = i; n < A->n; n++)
					{
						f = A->matPA[i][n];
						A->matPA[i][n] = A->matPA[m][n];
						A->matPA[m][n] = f;
					}
				break;
			}
		
		if(!bFoundPivot)
			continue;

		// scale column i to value 1
		f = 1.0 / A->matPA[i][i];
		for(n = i; n < A->n; n++)
			A->matPA[i][n] *= f;

		for(m = i+1; m < A->m; m++)
		{
			f = A->matPA[m][i] / A->matPA[i][i];
			for(n = i; n < A->n; n++)
				A->matPA[m][n] -= A->matPA[i][n] * f;
		}
	}

	// calc rank
	for(m = 0; m < A->m; m++)
	{
		fRowSum = 0.0;
		for(n = m; n < A->n; n++)
			fRowSum += A->matPA[m][n];
		if(fabs(fRowSum) < 1.0e-30)
			fRank--;
	}

	dmatrixFree(A);

	return fRank;
}


//*****************************************************************************
// display functions
//*****************************************************************************

void micv_printMatrix(const DMatrix* A, char* sNumberFormat)
{
	int i, j;

	for(i=0; i < A->m; i++)
	{
		for(j=0; j < A->n; j++)
			fprintf(STDERR, sNumberFormat, A->matPA[i][j]);
		fprintf(STDERR, "\n");
	}
	fprintf(STDERR, "\n");
}


//*****************************************************************************
// helper functions
//*****************************************************************************

char* micv_strRemoveWhitespace(char* pString)
{
	size_t i=0;
	size_t n;

	while(pString[i] != '\0')
	{
		switch(pString[i])
		{
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			for(n=i; n < strlen(pString); n++)
				pString[n] = pString[n+1];
			break;
		default:
			i++;
		}
	}
	return pString;
}


//*****************************************************************************
// debugging functions
//*****************************************************************************

#ifdef _DEBUG

void micv_AssertMatrix(const DMatrix* A)
{
	int m,n;

	assert(A->m > 0);
	assert(A->n > 0);

	for(m=0; m < A->m; m++)
		for(n=0; n < A->n; n++)
			assert(!_isnan(A->matPA[m][n]) && _finite(A->matPA[m][n]));
}


void micv_AssertVector(const DVector* V)
{
	int i;

	assert(V->n > 0);

	for(i=0; i < V->n; i++)
		assert(!_isnan(V->vecA[i]) && _finite(V->vecA[i]));
}

#endif

