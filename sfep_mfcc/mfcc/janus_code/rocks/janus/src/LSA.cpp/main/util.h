/*
util.h
$Id: util.h,v 1.6 2004/11/10 04:23:06 dmochiha Exp $

*/
#ifndef LDA_UTIL_H
#define LDA_UTIL_H

//#include <iostream>

//using namespace std;

extern int  myclock (void);
extern char *rtime (double t);
extern char *strconcat(const char *s, const char *t);
extern int  converged (double *u, double *v, int n, double threshold);
extern int  doublecmp (double *x, double *y);
extern void normalize_matrix_row (double **dst, const double* const* src, int rows, int cols);
extern void normalize_matrix_col (double **dst, const double* const* src, int rows, int cols);

extern void normalize(double* vec, int size);

// compute log (sum_i)
extern double log10_add(const double* lvec, int size);
extern double fast_log10_add(const double* lvec, int size);
extern double log_add(double* lvec, int size);
extern double fast_log_add(double la, double lb);

extern double* increment(double* vec, int size, double z);
extern double* decrement(double* vec, int size, double z);
extern double* divide(double* vec, int size, double z);
extern double* exponential(double* vec, int size);
extern double* natural_log(double* vec, int size);
extern double* set_array(double* m, int size, double value);
extern int* set_int_array(int* m, int size, int value);

extern void copy_array(const double* from, double* to, int size);
extern void copy_dmatrix(const double* const* from, double** to, int nrow, int ncol);

extern double* exp_array(double* from, double* to, int size);

void set_dmatrix(double** m, int nrow, int ncol, double value);
void add_dmatrix(double** from, double** to, int nrow, int ncol, double value);
void log_dmatrix(double** from, double** to, int nrow, int ncol);
void exp_dmatrix(double** from, double** to, int nrow, int ncol);

double* mul_dmatrix(const double* const* m, const double* in, double* out, int nrow, int ncol);
double dot(const double* v1, const double* v2, int size);

void sample_dirichlet(const double* alpha, int size, double* wgt);

//void load_matrix(istream& is, double** m, int nrow, int ncol);
//void load_vector(istream& is, double* m, int size);

#endif
