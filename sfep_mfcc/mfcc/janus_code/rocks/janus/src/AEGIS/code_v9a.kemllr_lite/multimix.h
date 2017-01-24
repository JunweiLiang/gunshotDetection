#ifndef MULTI_MIX_H
#define MULTI_MIX_H

#include <vector>
#include <string>
#include "pdf.h"
#include "gaussian.h"
#include "util_new.h"

using namespace std;

class MultiMix : public PDF{
  public:
    MultiMix();
    MultiMix(const string& id, int dim);
    
    string getID();

    void setDim(int dim);
    int getDim();

    void addGaussian(Gaussian* g, float weight);
    void addGaussian(Gaussian* g, float weight, bool transform);
    int numMixture();
    float getWeight(int index);
    float getWeight(int index, bool transform);
    void setWeight(int index, float weight);

    Gaussian* getGaussian(int index);

    virtual float logBj(const float* x);
  private:
    string id;
    int dim;
    vector<Gaussian*> mixture;
    vector<float> mixture_weight;

    bool unroll;
    int iter;
};

#endif
