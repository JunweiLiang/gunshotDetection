#include "edge.h"

Edge::Edge(){
  this->id="";
  this->probability=0.0;
  this->source=NULL;
  this->target=NULL;
}

Edge::Edge(const string& id){
  this->id=id;
  this->probability=0.0;
  this->source=NULL;
  this->target=NULL;
}

Edge::Edge(const string& id, State* source, State* target, const float& probability){
  this->id=id;
  this->probability=probability;
  this->source=source;
  this->target=target;

  if(source==NULL){
    printf("Error in creating edge: source is NULL\n");
    exit(-1);
  }
  if(target==NULL){
    printf("Error in creating edge: target is NULL\n");
    exit(-1);
  }
}

string Edge::getID(){
  return id;
}

void Edge::setProb(const float& probability){
  this->probability=probability;
}

float Edge::getProb(){
  return probability;
}

State* Edge::getSource(){
  return source;
}

State* Edge::getTarget(){
  return target;
}

void Edge::setSource(State* source){
  this->source=source;
}

void Edge::setTarget(State* target){
  this->target=target;
}

