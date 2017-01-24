#ifndef GAUSSIAN_H
#define GAUSSIAN_H

#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include "pdf.h"
#include "linear_algebra.h"

using namespace std;

#define LVFLOOR -10.0

class Gaussian : public PDF{
  public:
    void setType(const string& type);
    string getType();
    
    virtual string getID() = 0;
    virtual void setDim(int dim) = 0;

    virtual int getDim() = 0;

    virtual void setMeanVec(const Matrix& mean) = 0;
    virtual Matrix getMeanVec() = 0;
    virtual float getMeanAt(int index) = 0;
    
    virtual void setCovMat(const Matrix& cov) = 0; //assumues cov inverse
    virtual Matrix getCovMat() = 0;
    virtual float getCovAt(int row, int col) = 0;

    virtual float logBj(const float* x) = 0;
    virtual float calculateGConst() = 0;
  private:
    string type;
};

class DiagGaussian : public Gaussian{
  public:
    DiagGaussian();
    DiagGaussian(const string& id, int dim);
    ~DiagGaussian();

    string getID();

    void setDim(int dim);
    int getDim();

    void setMeanVec(const Matrix& mean);
    Matrix getMeanVec();
    void setCovMat(const Matrix& cov); //assumues cov inverse
    Matrix getCovMat();
    float getCovAt(int row, int col);

    void setMean(const float* mean);
    float* getMean();
    float getMeanAt(int index);
    void setCov(const float* cov); //assumues cov inverse
    void setCov(const float* cov, bool transform);
    float* getCov(); 
    float getCovAt(int index);

    virtual float logBj(const float* x);
    float calculateGConst();
    float calculateXFormGConst();

  private:
    string id;

    float* mean;
    float* cov;
    float g_const;

    bool unroll;
    int iter;
};

class FullGaussian : public Gaussian{
  public:
    FullGaussian();
    FullGaussian(const string& id, int dim);
    ~FullGaussian();

    string getID();

    void setDim(int dim);
    int getDim();

    void setMeanVec(const Matrix& mean);
    Matrix getMeanVec();
    float getMeanAt(int index);
    void setCovMat(const Matrix& cov_inv); //assumes inverse cov
    Matrix getCovMat();
    Matrix getInvCovMat();
    float getCovAt(int row, int col);
    float getInvCovAt(int row, int col);

    virtual float logBj(const float* x);
    float calculateGConst();

  private:
    string id;

    Matrix mean;
    Matrix cov;
    Matrix cov_inv;
    float g_const;

    bool unroll;
    int iter;
};
#endif
