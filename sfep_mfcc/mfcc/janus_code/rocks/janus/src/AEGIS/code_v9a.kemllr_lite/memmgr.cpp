#include "memmgr.h"

MemoryManager::MemoryManager(){
  info=NULL;

  pdf_count=0;
  edge_count=0;
  state_count=0;
  hmm_count=0;
}

MemoryManager::MemoryManager(RelationInfo* info){
  this->info=info;

  this->info->addTable("HMMTable");
  this->info->addTable("HMMNameTable");
  this->info->addTable("EdgeTable");
  this->info->addTable("StateTable");
  this->info->addTable("PDFTable");
  this->info->addTable("StateInHMM");

  this->pdf_count=0;
  this->edge_count=0;
  this->state_count=0;
  this->hmm_count=0;

}

RelationInfo* MemoryManager::getRelationInfo(){
  return info;
}

DiagGaussian* MemoryManager::createDiagGaussian(int dim){
  DiagGaussian* g;
  char tmp[256];
  string id;

  sprintf(tmp, "PDF-%d", pdf_count++);
  id = string(tmp);

  g = new DiagGaussian(id, dim);
  
  info->addRelation("PDFTable", id, (Ptr)g);

  return g;
}

FullGaussian* MemoryManager::createFullGaussian(int dim){
  FullGaussian* g;
  char tmp[256];
  string id;

  sprintf(tmp, "PDF-%d", pdf_count++);
  id = string(tmp);

  g = new FullGaussian(id, dim);
  
  info->addRelation("PDFTable", id, (Ptr)g);

  return g;
}

MultiMix* MemoryManager::createMultiMix(int dim){
  MultiMix* mm;
  char tmp[256];
  string id;

  sprintf(tmp, "PDF-%d", pdf_count++);
  id = string(tmp);
  
  mm = new MultiMix(id, dim);

  info->addRelation("PDFTable", id, (Ptr)mm);

  return mm;
}

Edge* MemoryManager::createEdge(State* source, State* target, float weight){
  Edge* e;
  char tmp[256];
  string id;

  sprintf(tmp, "Edge-%d", edge_count++);
  id = string(tmp);

  e = new Edge(id, source, target, weight);

  info->addRelation("EdgeTable", id, (Ptr)e);

  return e;
}

State* MemoryManager::createState(PDF* pdf){
  State* v;
  char tmp[256];
  string id;

  sprintf(tmp, "State-%d", state_count++);
  id = string(tmp);

  v = new State(id, pdf);

  info->addRelation("StateTable", id, (Ptr)v);

  return v;
}

AegisHMM* MemoryManager::createHMM(const string& name){
  AegisHMM* h;
  char tmp[256];
  string id;

  sprintf(tmp, "HMM-%d", hmm_count++);
  id = string(tmp);

  h = new AegisHMM(id, name);

  info->addRelation("HMMTable", id, (Ptr)h);
  info->addRelation("HMMNameTable", name, (Ptr)h);

  return h;
}

State* MemoryManager::duplicateState(State* state){
  State* dup;
  char tmp[256];
  string id;

  sprintf(tmp, "State-%d", state_count++);
  id = string(tmp);

  dup = new State(id, state->getPDF());

  info->addRelation("StateTable", id, (Ptr)dup);

  return dup;
}

bool MemoryManager::hasObj(const string& id, Ptr obj){
  if(keyType(id)=="")
    return false;
  if(hasObj(id, keyType(id), obj))
    return true;
  return false;
}

bool MemoryManager::hasObj(const string& id, const string& table_name, Ptr obj){
  if(info->hasRelation(table_name, id, obj))
    return true;
  return false;
}

bool MemoryManager::removeDiagGaussian(DiagGaussian* g){
  if(!info->hasRelation("PDFTable", g->getID(), (Ptr) g))
    return false;
  info->removeRelation("PDFTable", g->getID(), (Ptr) g);
  delete g;
  return true;
}

bool MemoryManager::removeFullGaussian(FullGaussian* g){
  if(!info->hasRelation("PDFTable", g->getID(), (Ptr) g))
    return false;
  info->removeRelation("PDFTable", g->getID(), (Ptr) g);
  delete g;
  return true;
}

bool MemoryManager::removeMultiMix(MultiMix* mm){
  if(!info->hasRelation("PDFTable", mm->getID(), (Ptr) mm))
    return false;
  info->removeRelation("PDFTable", mm->getID(), (Ptr) mm);
  delete mm;
  return true;
}

bool MemoryManager::removeEdge(Edge* e){
  if(!info->hasRelation("EdgeTable", e->getID(), (Ptr) e))
    return false;
  info->removeRelation("EdgeTable", e->getID(), (Ptr) e);
  delete e;
  return true;
}

bool MemoryManager::removeState(State* v){
  if(!info->hasRelation("StateTable", v->getID(), (Ptr) v))
    return false;
  info->removeRelation("StateTable", v->getID(), (Ptr) v);
  delete v;
  return true;
}

bool MemoryManager::removeHMM(AegisHMM* h){
  if(!info->hasRelation("HMMTable", h->getID(), (Ptr) h))
    return false;
  info->removeRelation("HMMTable", h->getID(), (Ptr) h);
  delete h;
  return true;
}

bool MemoryManager::removeObj(const string& id){
  if(keyType(id)=="")
    return false;
  return removeObj(id, keyType(id));
}

bool MemoryManager::removeObj(const string& id, const string& table_name){
  return info->removeAllRelations(table_name, id); 
}

string MemoryManager::keyType(const string& id){
  if(id[0]=='P')
    return "PDFTable";
  else if(id[0]=='E')
    return "EdgeTable";
  else if(id[0]=='S')
    return "StateTable";
  else if(id[0]=='H')
    return "HMMTable";
  return "";
}
