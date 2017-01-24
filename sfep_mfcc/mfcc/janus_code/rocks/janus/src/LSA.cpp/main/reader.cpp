/*
writer.c
an implementation of vector/matrix writer.
$Id: writer.c,v 1.3 2004/11/06 09:44:42 dmochiha Exp $

*/
#include <stdio.h>
#include "reader.h"

void
lda_read  (FILE *ap, FILE *bp, double *alpha, double **beta,
           int nclass, int nlex)
{
   fprintf(stderr, "reading model..\n"); fflush(stdout);
   read_vector(ap, alpha, nclass);
   read_matrix(bp, beta, nlex, nclass);
   fprintf(stderr, "done.\n"); fflush(stdout);
}

void read_string(FILE *fp, char* str, int& len)
{
#ifdef BINARY
   fread(&len, sizeof(int), 1, fp);
   fread(str, sizeof(char), len, fp);
#else
   fscanf(fp, "%d\n", &len);
   fscanf(fp, "%s\n", str);
#endif
}

void
read_vector (FILE *fp, double *vector, int n)
{
#ifdef BINARY
   fread(vector, sizeof(double), n, fp);
#else
   for (int i = 0; i < n; i++)
      fscanf(fp, "%.7e", vector[i]);
#endif
}

void
read_int_vector (FILE *fp, int *vector, int n)
{
#ifdef BINARY
   fread(vector, sizeof(int), n, fp);
#else
   for (int i = 0; i < n; i++)
      fscanf(fp, "%d", vector[i]);
#endif
}

void
read_matrix (FILE *fp, double **matrix, int rows, int cols)
{
   for (int i = 0; i < rows; i++) {
      double* matrix_i = matrix[i];
      
      read_vector (fp, matrix_i, cols);      
   }
}
