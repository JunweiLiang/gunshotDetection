#include "kernel.h"

Kernel::Kernel(){
}

LinearKernel::LinearKernel(){
}

float LinearKernel::eval(Matrix& vec1, Matrix& vec2){
  float result;

  result = (vec1.transpose()*vec2)[0][0];

  return result;
}

float LinearKernel::eval_inv(const float& x){
  return x;
}

PolyKernel::PolyKernel(const float& p){
  this->p = p;
}

float PolyKernel::eval(Matrix& vec1, Matrix& vec2){
  float result;

  result = (vec1.transpose()*vec2)[0][0]+1.0;
  result = pow(result, p);
  
  return result;
}

float PolyKernel::eval_inv(const float& x){
  float result;

  result = pow(x, (float) 1.0/p)-1.0;
  
  return result;
}

GaussianKernel::GaussianKernel(const float& beta){
  this->beta = beta;
}

float GaussianKernel::eval(Matrix& vec1, Matrix& vec2){
  Matrix tmp;
  float result;

  tmp = vec1 - vec2;
 
  result = -beta*(tmp.transpose()*tmp)[0][0];
  result = exp(result);
  
  return result;
}

float GaussianKernel::eval_inv(const float& x){
  return 0.0;
}

NormalizedGaussianKernel::NormalizedGaussianKernel(const float& beta, const Matrix& cov_inv){
  this->beta = beta;
  this->cov_inv = cov_inv;
  if(cov_inv.getNumRow()==cov_inv.getNumCol())
    is_diag=false;
  else
    is_diag=true;
}

float NormalizedGaussianKernel::eval(Matrix& vec1, Matrix& vec2){
  Matrix tmp;
  float result=0.0;

  tmp = vec1 - vec2;
  
  if(is_diag){
    for(int i=0;i<tmp.getNumRow();i++){
      result+=tmp[i][0]*tmp[i][0]*cov_inv[i][0];
    }
    result*=-beta;
  }
  else{
    result = -beta*(tmp.transpose()*cov_inv*tmp)[0][0];
  }
  result = exp(result);
  
  return result;
}

float NormalizedGaussianKernel::eval_inv(const float& x){
  return 0.0;
}


