#include <stdio.h>
#include "gaussian.h"

void Gaussian::setType(const string& type){
  this->type = type;
}

string Gaussian::getType(){
  return type;
}

DiagGaussian::DiagGaussian(){
  id="";
  dim = 0;
  mean = NULL;
  cov = NULL;
  unroll = false;
  iter = 0;

  setType("diag");
}

DiagGaussian::DiagGaussian(const string& id, int dim){
  this->id=id;
  this->dim = dim;
  if(dim>0){
    mean = new float[dim];
    cov = new float[dim];
  }
  else{
    mean = NULL;
    cov = NULL;
  }
  unroll = (dim%7)==0;
  iter = dim/7;

  setType("diag");
}

DiagGaussian::~DiagGaussian(){
  if(dim>0){
    delete [] mean;
    delete [] cov;
  }
  dim=0;
}

string DiagGaussian::getID(){
  return id;
}

void DiagGaussian::setDim(int dim){
  if(dim>0){
    delete [] mean;
    delete [] cov;
  }
  this->dim = dim;
  mean = new float[dim];
  cov = new float[dim];
  unroll = (dim%7)==0;
  iter = dim/7;
}

int DiagGaussian::getDim(){
  return dim;
}

void DiagGaussian::setMeanVec(const Matrix& mean){
  for(int i=0;i<dim;i++){
    this->mean[i] = mean.getValue(0, i);
  }
}

void DiagGaussian::setMean(const float* mean){
  for(int i=0;i<dim;i++){
    this->mean[i] = mean[i];
  }
}

Matrix DiagGaussian::getMeanVec(){
  return Matrix::makeVector(mean, dim);
}

float* DiagGaussian::getMean(){
  return mean;
}

float DiagGaussian::getMeanAt(int index){
  return mean[index];
}

void DiagGaussian::setCovMat(const Matrix& cov){
  if(cov.getNumRow()!=1){
    printf("Error in DiagGaussian Cov\n");
    exit(-1);
  }
  setCov(cov.getMap()[0]);
}

void DiagGaussian::setCov(const float* cov){
  for(int i=0;i<dim;i++){
    this->cov[i] = cov[i];
  }
  g_const = calculateGConst();
}

void DiagGaussian::setCov(const float* cov, bool transform){
  if(transform){
    setCov(cov);
    return;
  }
  for(int i=0;i<dim;i++){
    this->cov[i] = cov[i];
  }
//  g_const = calculateGConst();
}

Matrix DiagGaussian::getCovMat(){
  return Matrix::makeVector(cov, dim);
}

float* DiagGaussian::getCov(){
  return cov;
}

float DiagGaussian::getCovAt(int index){
  return cov[index];
}

float DiagGaussian::getCovAt(int row, int col){
  if(row!=col){
    printf("Error in DiagGaussian Cov\n");
    exit(-1);
  }
  return cov[row];
}

float DiagGaussian::calculateGConst(){
  float result=0.0;
/*
  result = dim*log(2*M_PI);
  for(int i=0;i<dim;i++){
    if((cov[i])>0.0) //1.0/cov[i] > 0.0
      result -= log(cov[i]); //result += log(1.0/cov[i]);
    else{
      cov[i] = 1.0/exp(LVFLOOR);
      result += LVFLOOR;
    }
  }
*/
  result = dim*log(2*M_PI);
  for(int i=0;i<dim;i++){
    if((cov[i])>0.0) //1.0/cov[i] > 0.0
      result -= log(cov[i]); //result += log(1.0/cov[i]);
    else{
      result += LVFLOOR;
    }
  }

  return result;
}

float DiagGaussian::calculateXFormGConst(){
  float result=0.0;
/*
  result = dim*log(2*M_PI);
  for(int i=0;i<dim;i++){
    if(cov[i]>0.0) //1.0/cov[i] > 0.0
      result += log(1.0/cov[i]); //result += log(1.0/cov[i]);
    else{
      cov[i] = exp(LVFLOOR);
      result += LVFLOOR;
    }
  }
*/
  result = dim*log(2*M_PI);
  for(int i=0;i<dim;i++){
    if(cov[i]>0.0) //1.0/cov[i] > 0.0
      result += log(1.0/cov[i]); //result += log(1.0/cov[i]);
    else{
      result += LVFLOOR;
    }
  }
  return result;
}
float DiagGaussian::logBj(const float* x){
  float result, tmp;
  float *m_ptr, *cov_ptr, *data;

  m_ptr = mean;
  cov_ptr = cov;

  data = (float*) x;

  result = g_const;

  if(unroll){
    for(int i=0;i<iter;i++){
      tmp = *data - *m_ptr;
      data++;
      result += (tmp*tmp) * (*cov_ptr);
      m_ptr++;
      cov_ptr++;

      tmp = *data - *m_ptr;
      data++;
      result += (tmp*tmp) * (*cov_ptr);
      m_ptr++;
      cov_ptr++;

      tmp = *data - *m_ptr;
      data++;
      result += (tmp*tmp) * (*cov_ptr);
      m_ptr++;
      cov_ptr++;

      tmp = *data - *m_ptr;
      data++;
      result += (tmp*tmp) * (*cov_ptr);
      m_ptr++;
      cov_ptr++;

      tmp = *data - *m_ptr;
      data++;
      result += (tmp*tmp) * (*cov_ptr);
      m_ptr++;
      cov_ptr++;

      tmp = *data - *m_ptr;
      data++;
      result += (tmp*tmp) * (*cov_ptr);
      m_ptr++;
      cov_ptr++;
    }
  }
  else{
    for(int i=0;i<dim;i++){
      tmp = x[i] - mean[i];
      result += (tmp*tmp) * cov[i];
    }
  }
  
  result *= -0.5;

  return result;
}

FullGaussian::FullGaussian(){
  id="";
  dim = 0;
  unroll = false;
  iter = 0;

  setType("full");
}

FullGaussian::FullGaussian(const string& id, int dim){
  this->id=id;
  this->dim = dim;
  mean = Matrix(1, dim);
  cov = Matrix(dim, dim);
  cov_inv = Matrix(dim, dim);
  unroll = (dim%7)==0;
  iter = dim/7;

  setType("full");
}

FullGaussian::~FullGaussian(){
}

string FullGaussian::getID(){
  return id;
}

void FullGaussian::setDim(int dim){
  this->dim = dim;
  mean = Matrix(1, dim);
  cov = Matrix(dim, dim);
  cov_inv = Matrix(dim, dim);
  unroll = (dim%7)==0;
  iter = dim/7;
}

int FullGaussian::getDim(){
  return dim;
}

void FullGaussian::setMeanVec(const Matrix& mean){
  this->mean = mean;
}

Matrix FullGaussian::getMeanVec(){
  return mean;
}

float FullGaussian::getMeanAt(int index){
  return mean[0][index];
}

void FullGaussian::setCovMat(const Matrix& cov_inv){
  this->cov_inv = cov_inv;
  this->cov = cov_inv.inverse();
  g_const = calculateGConst();
}

Matrix FullGaussian::getCovMat(){
  return cov;
}

Matrix FullGaussian::getInvCovMat(){
  return cov_inv;
}

float FullGaussian::getCovAt(int row, int col){
  return cov[row][col];
}

float FullGaussian::getInvCovAt(int row, int col){
  return cov_inv[row][col];
}

float FullGaussian::calculateGConst(){
  float result=0.0;

  result = dim*log(2*M_PI);
  result += cov.logDeterminant();

  return result;
}

float FullGaussian::logBj(const float* x){
  float result, tmp;
  Matrix vec(dim, 1);

  result = g_const;

  for(int i=0;i<dim;i++){
    vec[i][0] = x[i] - mean[0][i];
  }
  result += (vec.transpose()*cov_inv*vec)[0][0];
  
  result *= -0.5;

  return result;
}
