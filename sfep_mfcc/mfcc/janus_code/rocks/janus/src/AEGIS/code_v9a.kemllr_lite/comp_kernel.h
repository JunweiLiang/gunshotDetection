#ifndef COMPOSITE_KERNEL_H
#define COMPOSITE_KERNEL_H

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
#include "kernel.h"
#include "str_util.h"

using namespace std;

class CompositeKernel{
  public:
    CompositeKernel();

    virtual float eval(vector<Matrix>& set1, vector<Matrix>& set2)=0;
  private:
};

class DirectSumKernel : public CompositeKernel{
  public:
    DirectSumKernel(Kernel* base_kernel);

    virtual float eval(vector<Matrix>& set1, vector<Matrix>& set2);
  private:
    Kernel* base_kernel;
};

class HeteroDirectSumKernel : public CompositeKernel{
  public:
    HeteroDirectSumKernel();
    HeteroDirectSumKernel(vector<Kernel*>& base_kernel);

    void addKernel(Kernel* kernel);
    virtual float eval(vector<Matrix>& set1, vector<Matrix>& set2);
  private:
    vector<Kernel*> base_kernel;
};

#endif
