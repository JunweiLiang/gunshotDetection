#include "pdf.h"

PDF::PDF(){
  id="";
  dim = 0;
}

PDF::PDF(const string& id, int dim){
  this->id=id;
  this->dim=dim;
}

PDF::~PDF(){
}

string PDF::getID(){
  return id;
}
