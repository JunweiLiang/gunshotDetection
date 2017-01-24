/*
util.c
$Id: util.c,v 1.7 2004/11/10 04:23:06 dmochiha Exp $

*/
#include <algorithm>
#include <stdio.h>
#include <unistd.h>
#include <cstring>
#include <sys/time.h>
#include <math.h>
#include "util.h"

using namespace std;

static double ln10 = log(10);

int myclock ()
{
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return tv.tv_sec;
}

char *
rtime (double t)
{
   int hour, min, sec;
   static char buf[BUFSIZ];
   
   hour = (int)floor((int)t / 60 / 60);
   min  = (int)floor(((int)t % (60 * 60)) / 60);
   sec  = (int)floor((int)t % 60);
   sprintf(buf, "%2d:%02d:%02d", hour, min, sec);
   
   return buf;
}

int
doublecmp (double *x, double *y)
{
   return (*x == *y) ? 0 : ((*x < *y) ? 1 : -1);
}

char *
strconcat (const char *s, const char *t)
{
   static char z[BUFSIZ];
   strcpy(z, s);
   strcat(z, t);
   return z;
}

int
converged (double *u, double *v, int n, double threshold)
{
   /* return 1 if |a - b|/|a| < threshold */
   double us = 0;
   double ds = 0;
   double d;
   int i;
   
   for (i = 0; i < n; i++)
      us += u[i] * u[i];
   
   for (i = 0; i < n; i++) {
      d = u[i] - v[i];
      ds += d * d;
   }
   
   if (sqrt(ds / us) < threshold)
      return 1;
   else
      return 0;
   
}

void
normalize_matrix_col (double **dst, const double* const* src, int rows, int cols)
{
   /* column-wise normalize from src -> dst */
   double z;
   int i, j;
   
   for (j = 0; j < cols; j++) {
      for (i = 0, z = 0; i < rows; i++)
         z += src[i][j];
      for (i = 0; i < rows; i++)
         dst[i][j] = src[i][j] / z;
   }
}

void
normalize_matrix_row (double **dst, const double* const* src, int rows, int cols)
{
   /* row-wise normalize from src -> dst */
   int i, j;
   double z;
   
   for (i = 0; i < rows; i++) {
      for (j = 0, z = 0; j < cols; j++)
         z += src[i][j];
      for (j = 0; j < cols; j++)
         dst[i][j] = src[i][j] / z;
   }
}

// normalize array
void normalize(double* vec, int size)
{
   double z = 0.0;
   
   for (int i=0; i < size; i++) {
      z += vec[i];
   }

   if (z == 0) {
      printf("error: normalization error z = 0. array not normalized\n");

      return;
   }

   for (int i=0; i < size; i++) {
      vec[i] /= z;
   }
}

double fast_log_add(double la, double lb)
{
   double lmax = 0.0;
   double lmin = 0.0;

   if (la > lb) {
      lmax = la;
      lmin = lb;
   } else {
      lmax = lb;
      lmin = la;
   }

   // avoid exp(.) and log(.) if their difference is bigger than some threshold
   if ((lmax-lmin) > 5.0) return lmax;

   return lmax + log(1.0 + exp(lmin-lmax));
}

double log_add(double* lvec, int size)
{
   const double lmax = *max_element(lvec, lvec+size);
   
   double lvalue = 0.0;
   
   for (int i=0; i < size; i++) {
      //lvalue += log(sum(exp(x - max.x))))
      lvalue += exp(lvec[i] - lmax);
   }
   
   lvalue = lmax + log(lvalue);
   
   return lvalue;
}

double fast_log10_add(const double* lvec, int size)
{
   const double lmax = *max_element(lvec, lvec+size);
   
   double lvalue = 0.0;
   
   for (int i=0; i < size; i++) {
      double diff = lvec[i] - lmax;
      if (diff < -10.0)
         lvalue += exp(diff * ln10);
   }
   
   lvalue = lmax + log10(lvalue);
   
   return lvalue;
}

double log10_add(const double* lvec, int size)
{
   const double* lmax = max_element(lvec, lvec+size);
   
   double lvalue = 0.0;
   
   for (int i=0; i < size; i++) {
      //lvalue += log(sum(exp(x - max.x))))
      lvalue += exp((lvec[i] - *lmax)*ln10);
   }
   
   lvalue = *lmax + log10(lvalue);
   
   return lvalue;
}

double* increment(double* vec, int size, double z)
{
   for (int i=0; i < size; i++) {
      vec[i] += z;
   }

   return vec;
}

double* decrement(double* vec, int size, double z)
{
   for (int i=0; i < size; i++) {
      vec[i] -= z;
   }

   return vec;
}

double* divide(double* vec, int size, double z)
{
   for (int i=0; i < size; i++) {
      vec[i] /= z;
   }

   return vec;
}

double* exponential(double* vec, int size)
{
   for (int i=0; i < size; i++) {
      vec[i] = exp(vec[i]);
   }

   return vec;
}

double* natural_log(double* vec, int size)
{
   for (int i=0; i < size; i++) {
      vec[i] = log(vec[i]);
   }

   return vec;
}

void copy_array(const double* from, double* to, int size)
{
   for (int i=0; i < size; i++) {
      to[i] = from[i];
   }
}

void set_dmatrix(double** m, int nrow, int ncol, double value)
{
   for (int i = 0; i < nrow; i++) {
      double* m_i = m[i];

      for (int j = 0; j < ncol; j++)
         m_i[j] = value;
   }
}

void add_dmatrix(double** from, double** to, int nrow, int ncol, double value)
{
   for (int i = 0; i < nrow; i++) {
      double* from_i = from[i];
      double* to_i = to[i];

      for (int j = 0; j < ncol; j++)
         to_i[j] = from_i[j] + value;
   }
}

void log_dmatrix(double** from, double** to, int nrow, int ncol)
{
   for (int i = 0; i < nrow; i++) {
      double* from_i = from[i];
      double* to_i = to[i];

      for (int j = 0; j < ncol; j++)
         to_i[j] = log(from_i[j]);
   }
}

void exp_dmatrix(double** from, double** to, int nrow, int ncol)
{
   for (int i = 0; i < nrow; i++) {
      double* from_i = from[i];
      double* to_i = to[i];

      for (int j = 0; j < ncol; j++)
         to_i[j] = exp(from_i[j]);
   }
}

double* exp_array(double* from, double* to, int size)
{
   for (int i = 0; i < size; i++) {
      to[i] = exp(from[i]);
   }

   return to;
}

double* set_array(double* m, int size, double value)
{
   for (int i = 0; i < size; i++) {
      m[i] = value;
   }

   return m;
}

int* set_int_array(int* m, int size, int value)
{
   for (int i = 0; i < size; i++) {
      m[i] = value;
   }

   return m;
}


void copy_dmatrix(const double* const* from, double** to, int nrow, int ncol)
{
   for (int i=0; i < nrow; i++)
      copy_array(from[i], to[i], ncol);
}

double* mul_dmatrix(const double* const* m, const double* in, double* out, int nrow, int ncol)
{
   for (int i=0; i < nrow; i++)
      out[i] = dot(m[i], in, ncol);

   return out;
}

double dot(const double* v1, const double* v2, int size)
{
   double val = 0.0;

   for (int i=0; i < size; i++)
      val += v1[i] * v2[i];

   return val;
}

double randgamma(double rr) {
  double bb, cc, dd;
  double uu, vv, ww, xx, yy, zz;

  if ( rr <= 0.0 ) {
    /* Not well defined, set to zero and skip. */
    return 0.0;
  } else if ( rr == 1.0 ) {
    /* Exponential */
    return - log(drand48());
  } else if ( rr < 1.0 ) {
    /* Use Johnks generator */
    cc = 1.0 / rr;
    dd = 1.0 / (1.0-rr);
    while (1) {
      xx = pow(drand48(), cc);
      yy = xx + pow(drand48(), dd);
      if ( yy <= 1.0 ) {
        return -log(drand48()) * xx / yy;
      }
    }
  } else { /* rr > 1.0 */
    /* Use bests algorithm */
    bb = rr - 1.0;
    cc = 3.0 * rr - 0.75;
    while (1) {
      uu = drand48();
      vv = drand48();
      ww = uu * (1.0 - uu);
      yy = sqrt(cc / ww) * (uu - 0.5);
      xx = bb + yy;
      if (xx >= 0) {
        zz = 64.0 * ww * ww * ww * vv * vv;
        if ( ( zz <= (1.0 - 2.0 * yy * yy / xx) ) ||
             ( log(zz) <= 2.0 * (bb * log(xx / bb) - yy) ) ) {
          return xx;
        }
      }
    }
  }
}

void sample_dirichlet(const double* alpha, int size, double* wgt)
{
   for (int i=0; i < size; i++)
      wgt[i] = randgamma(alpha[i]);

   normalize(wgt, size);
}
