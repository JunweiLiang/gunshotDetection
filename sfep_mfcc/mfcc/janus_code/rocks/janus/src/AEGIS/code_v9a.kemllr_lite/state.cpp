#include "state.h"

State::State(){
  id="";
  pdf=NULL;
}

State::State(const string& id){
  this->id=id;
  this->pdf=NULL;
}

State::State(const string& id, PDF* pdf){
  this->id=id;
  this->pdf=pdf;
}

string State::getID(){
  return id;
}

bool State::isNullNode(){
  if(pdf!=NULL)
    return false;
  return true;
}

void State::setPDF(PDF* pdf){
  this->pdf=pdf;
}

PDF* State::getPDF(){
  return pdf;
}

float State::logBj(const float* x){
  return pdf->logBj(x);
}
