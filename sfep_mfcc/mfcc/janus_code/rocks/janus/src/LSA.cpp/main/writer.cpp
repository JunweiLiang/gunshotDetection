/*
    writer.c
    an implementation of vector/matrix writer.
    $Id: writer.c,v 1.3 2004/11/06 09:44:42 dmochiha Exp $

*/
#include <stdio.h>
#include "writer.h"

void
lda_write (FILE *ap, FILE *bp, double *alpha, double **beta,
	   int nclass, int nlex)
{
	printf("writing model..\n"); fflush(stdout);
	write_vector(ap, alpha, nclass);
	write_matrix(bp, beta, nlex, nclass);
	printf("done.\n"); fflush(stdout);
}

void write_string(FILE *fp, const char* str, int len)
{
#ifdef BINARY
   fwrite(&len, sizeof(int), 1, fp);
   fwrite(str, sizeof(char), len, fp);
#else
   fprintf(fp, "%d\n", len);
   fprintf(fp, "%s\n", str);
#endif
}

void
write_int_vector (FILE *fp, const int *vector, int n)
{
#ifdef BINARY
        fwrite(vector, sizeof(int), n, fp);
#else
	for (int i = 0; i < n; i++)
		fprintf(fp, "%d%s", vector[i], (i == n - 1) ? "\n" : "   ");
#endif
}

void
write_vector (FILE *fp, const double *vector, int n)
{
#ifdef BINARY
        fwrite(vector, sizeof(double), n, fp);
#else
	for (int i = 0; i < n; i++)
		fprintf(fp, "%.7e%s", vector[i], (i == n - 1) ? "\n" : "   ");
#endif
}

void
write_matrix (FILE *fp, const double* const* matrix, int rows, int cols)
{
	for (int i = 0; i < rows; i++) {
                const double* matrix_i = matrix[i];

                write_vector (fp, matrix_i, cols);
        }
}
