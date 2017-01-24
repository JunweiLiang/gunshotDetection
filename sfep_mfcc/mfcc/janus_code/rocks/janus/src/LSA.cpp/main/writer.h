/*
    writer.h
    a header file of vector/matrix writer.
    $Id: writer.h,v 1.2 2004/11/10 04:23:06 dmochiha Exp $

*/
#ifndef LDA_WRITER_H
#define LDA_WRITER_H
#include <cstdio>

using namespace std;

extern void lda_write (FILE *ap, FILE *bp, double *alpha, double **beta,
		       int nclass, int nlex);
void write_vector (FILE *fp, const double *vector, int n);
void write_matrix (FILE *fp, const double* const* matrix, int rows, int cols);
void write_int_vector (FILE *fp, const int *vector, int n);

void write_string(FILE *fp, const char* str, int len);

#endif
