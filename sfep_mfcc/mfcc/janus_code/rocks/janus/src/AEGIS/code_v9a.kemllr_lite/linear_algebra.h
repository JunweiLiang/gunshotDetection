#ifndef LINEAR_ALGEBRA_H
#define LINEAR_ALGEBRA_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cppblas.h"
#include "cpplapack.h"
#include "nrutil_ex.h"
#include "math_engine.h"
#include "util_new.h"
#include "string.h"

class Matrix{
  public:
    Matrix();
    Matrix(int num_row, int num_col);
    Matrix(const Matrix& m);
    ~Matrix();

    static Matrix makeVector(float* v, int d);
    static Matrix makeMatrix(float** m, int r, int c);
    static Matrix makeMatrix(float* m, int r, int c);
    static Matrix makeMatrix(double** m, int r, int c);
    static Matrix makeMatrix(double* m, int r, int c);

    void zero();
    void identity();
    void destroy();
    void random();

    Matrix vectorize() const;
    Matrix kroneckerProduct(const Matrix& mat) const;

    double cofactor(int i, int j) const;
    double determinant() const;
    double logDeterminant() const;
    float trace() const;
    Matrix inverse() const;
    Matrix dInverse() const;
    Matrix dPseudoInverse() const;
    Matrix transpose() const;
    Matrix add(const Matrix& m) const;
    Matrix sub(const Matrix& m) const;
    Matrix mul(const float& constant) const;
    Matrix mul(const Matrix& m) const;
    void scale(float alpha);

    bool operator==(const Matrix& m) const;
    bool operator!=(const Matrix& m) const;
    Matrix& operator=(const Matrix& m);
    float* operator[](const int& row_index);
    Matrix operator+(const Matrix& m) const;
    Matrix operator-(const Matrix& m) const;
    Matrix operator*(const Matrix& m) const;
    Matrix operator*(const float& constant) const;
    void operator+=(const Matrix& m) const;

    float dotProd(int i, int j, Matrix& m, int r, int c, int d);

    Matrix copy(int row_start, int col_start, int row_end, int col_end) const;
    void paste(const Matrix& m, int row, int col);

    void setValue(int row, int col, float value);
    float getValue(int row, int col) const;
    float** getMap() const;
    Matrix getRowVec(int row) const;
    Matrix getColVec(int col) const;

    int getNumRow() const;
    int getNumCol() const;
    bool hasSameDimension(const Matrix& m) const;
    bool canMultiply(const Matrix& m) const;
    bool isEmpty() const;

    void print() const;
    void printFP(FILE* fp) const;
    void save(char* filename) const;
    void load(char* filename);
    void bsave(char* filename) const;
    void bload(char* filename);
    void bsave(FILE* fp) const;
    void bload(FILE* fp);
        
  private:
    float* mem;
    float** val;
    int num_row;
    int num_col;
};

#endif
