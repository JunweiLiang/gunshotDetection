#ifndef KERNEL_H
#define KERNEL_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <string>
#include <fstream>

#include "memmgr.h"
#include "relation_info.h"
#include "recognizer.h"
#include "hmm.h"
#include "state.h"
#include "network.h"
#include "multimix.h"
#include "gaussian.h"
#include "readHTKformat.h"
#include "linear_algebra.h"
#include "cache.h"
#include "str_util.h"

using namespace std;

class Kernel{
  public:
    Kernel();

    virtual float eval(Matrix& vec1, Matrix& vec2) = 0;
    virtual float eval_inv(const float& x) = 0;
  private:
};

class LinearKernel : public Kernel{
  public:
    LinearKernel();
    virtual float eval(Matrix& x1, Matrix& x2);
    virtual float eval_inv(const float& x);
  private:
};

class PolyKernel : public Kernel{
  public:
    PolyKernel(const float& p);

    virtual float eval(Matrix& x1, Matrix& x2);
    virtual float eval_inv(const float& x);
  private:
    float p;
};

class GaussianKernel : public Kernel{
  public:
    GaussianKernel(const float& beta);

    virtual float eval(Matrix& x1, Matrix& x2);
    virtual float eval_inv(const float& x);
  private:
    float beta;
};

class NormalizedGaussianKernel : public Kernel{
  public:
    NormalizedGaussianKernel(const float& beta, const Matrix& cov_inv);

    virtual float eval(Matrix& x1, Matrix& x2);
    virtual float eval_inv(const float& x);
  private:
    float beta;
    Matrix cov_inv;
    bool is_diag;
};

#endif
