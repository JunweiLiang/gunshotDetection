/*
    writer.h
    a header file of vector/matrix writer.
    $Id: writer.h,v 1.2 2004/11/10 04:23:06 dmochiha Exp $

*/
#ifndef LDA_READER_H
#define LDA_READER_H
#include <cstdio>

using namespace std;

extern void lda_read  (FILE *ap, FILE *bp, double *alpha, double **beta,
		       int nclass, int nlex);
void read_vector (FILE *fp, double *vector, int n);
void read_matrix (FILE *fp, double **matrix, int rows, int cols);
void read_int_vector (FILE *fp, int *vector, int n);

void read_string(FILE *fp, char* str, int& len);

#endif
