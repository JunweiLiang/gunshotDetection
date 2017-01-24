#include "comp_kernel.h"

CompositeKernel::CompositeKernel(){
}

DirectSumKernel::DirectSumKernel(Kernel* base_kernel){
  this->base_kernel = base_kernel;
}

float DirectSumKernel::eval(vector<Matrix>& set1, vector<Matrix>& set2){
  float result=0.0;

  if(set1.size()!=set2.size()){
    printf("Error in computing comp kernel: %d %d\n", set1.size(), set2.size());
    exit(-1);
  }
  
  for(int i=0;i<set1.size();i++){
    result += base_kernel->eval(set1[i], set2[i]);
//    printf("%lf\n", result);
  }

  return result;
}

HeteroDirectSumKernel::HeteroDirectSumKernel(){
  base_kernel.clear();
}

HeteroDirectSumKernel::HeteroDirectSumKernel(vector<Kernel*>& base_kernel){
  this->base_kernel = base_kernel;
}

void HeteroDirectSumKernel::addKernel(Kernel* kernel){
  base_kernel.push_back(kernel);
}

float HeteroDirectSumKernel::eval(vector<Matrix>& set1, vector<Matrix>& set2){
  float result=0.0;

  if(set1.size()!=set2.size() || set1.size()!=base_kernel.size()){
    printf("Error in computing comp kernel: %d %d %d\n", set1.size(), set2.size(), base_kernel.size());
    exit(-1);
  }
  
  for(int i=0;i<set1.size();i++){
    result += base_kernel[i]->eval(set1[i], set2[i]);
  }

  return result;
}
