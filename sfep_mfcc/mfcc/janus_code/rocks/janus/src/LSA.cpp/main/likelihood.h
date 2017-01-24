/*
likelihood.h
$Id: likelihood.h,v 1.1 2004/11/05 08:28:17 dmochiha Exp $

*/
#ifndef LDA_LIKELIHOOD_H
#define LDA_LIKELIHOOD_H
#include "lsafeature.h"

// for all documents
extern double lda_MAP_LL (document *data, double **beta, double **gammas,
                          int m, int nclass);

// MAP log likelihood per document
extern double lda_MAP_LL (const document *dp, const double* const*beta, const double *gamma, int nclass);

// variational log likelihood per document
extern double lda_VAR_LL(const document* dp, const double* const* beta, const double* alpha, const double* gamma, const double* ap, const double* const* q, int nclass);


#endif
