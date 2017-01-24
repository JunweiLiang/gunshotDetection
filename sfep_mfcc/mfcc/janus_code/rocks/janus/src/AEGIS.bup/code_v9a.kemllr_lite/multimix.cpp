#include <stdio.h>
#include "multimix.h"

MultiMix::MultiMix(){
  id="";
  dim=0;
  unroll = false;
  iter = 0;
}

MultiMix::MultiMix(const string& id, int dim){
  this->id=id;
  this->dim=dim;
  unroll = false;
  iter = 0;
}

string MultiMix::getID(){
  return id;
}

void MultiMix::setDim(int dim){
  this->dim=dim;
}

int MultiMix::getDim(){
  return dim;
}

void MultiMix::addGaussian(Gaussian* g, float weight){
  mixture.push_back(g);
  mixture_weight.push_back(log(weight));

  unroll = mixture.size()%4==0;
  iter = mixture.size()/4;
}

void MultiMix::addGaussian(Gaussian* g, float weight, bool transform){
  mixture.push_back(g);
  if(transform)
    mixture_weight.push_back(log(weight));
  else
    mixture_weight.push_back(weight);

  unroll = mixture.size()%4==0;
  iter = mixture.size()/4;
}
int MultiMix::numMixture(){
  return mixture.size();
}

float MultiMix::getWeight(int index){
  return exp(mixture_weight[index]);
}

float MultiMix::getWeight(int index, bool transform){
  if(transform)
    return exp(mixture_weight[index]);
  return mixture_weight[index];
}

void MultiMix::setWeight(int index, float weight){
  mixture_weight[index] = log(weight);
}

Gaussian* MultiMix::getGaussian(int index){
  return mixture[index];
}

float MultiMix::logBj(const float* x){
  float result=LZERO;
  int idx=0;

  if(unroll){
    for(int i=0;i<iter;i++){
      result = LogAdd(result, mixture_weight[idx]+mixture[idx]->logBj(x));
      idx++;
      
      result = LogAdd(result, mixture_weight[idx]+mixture[idx]->logBj(x));
      idx++;

      result = LogAdd(result, mixture_weight[idx]+mixture[idx]->logBj(x));
      idx++;

      result = LogAdd(result, mixture_weight[idx]+mixture[idx]->logBj(x));
      idx++;
    }
  }
  else{
    for(int i=0;i<mixture.size();i++){
      result = LogAdd(result, mixture_weight[i]+mixture[i]->logBj(x));
    }
  }

  return result;
}

