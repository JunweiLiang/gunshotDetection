#ifndef WHITENER_H
#define WHITENER_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "readHTKformat.h"
#include "linear_algebra.h"
#include "math_engine.h"

using namespace std;

class Whitener{
  public:
    Whitener(MFCCReader* reader, int dim);
    
    void prepare();
    void findTransformation();
    void whiten();

    Matrix convert(float* x);
  private:
    MFCCReader* reader;
    Matrix cov;
    Matrix mean;
    Matrix transformation;
    int dim;
};

#endif
