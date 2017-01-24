#ifndef MATH_ENGINE_H
#define MATH_ENGINE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
//#include "relation_info.h"
//#include "nr.h"
//#include "nrutil_ex.h"
#include "linear_algebra.h"

class Matrix;

class MathEngine{
  public:
//    float differentiate(float (*f)(float), float x, float h, float* err);
//    double differentiate(double (*f)(double), double x, double h, double* err);
//    float linearProgramming(const Matrix& obj, const Matrix& smaller, const Matrix& bigger, const Matrix& equal, Matrix& answer);
    static void eigenDecomp(const Matrix& m, Matrix& eigen_values, Matrix& eigen_vectors);
    static void dEigenDecomp(const Matrix& m, Matrix& eigen_values, Matrix& eigen_vectors);
//    static void extEigenDecomp(const Matrix& m, Matrix& eigen_values, Matrix& eigen_vectors);
//    static void extSVD(const Matrix& a, Matrix& u, Matrix& s, Matrix& v);
    static void svd(const Matrix& a, Matrix& u, Matrix& s, Matrix& v);
    static void dsvd(const Matrix& a, Matrix& u, Matrix& s, Matrix& v);
    static void deconSVD(const Matrix& a, Matrix& u, Matrix& s, Matrix& v);
    static float sigmoid(const float& x, const float& slope, const float& offset);
    static int nCr(int n, int r);
    static int factorial(int n);
};

#endif
