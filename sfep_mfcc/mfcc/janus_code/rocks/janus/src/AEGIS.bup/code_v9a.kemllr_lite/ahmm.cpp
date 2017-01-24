#include "ahmm.h"

AegisHMM::AegisHMM(){
  id="";
  name="";
  start=NULL;
  end=NULL;
}

AegisHMM::AegisHMM(const string& id, const string& name){
  this->id=id;
  this->name=name;
  start=NULL;
  end=NULL;
}

string AegisHMM::getID(){
  return id;
}

string AegisHMM::getName(){
  return name;
}

void AegisHMM::setStartState(int s_index){
  swap(state_set[s_index], state_set[0]);
  start = state_set[0];
}

void AegisHMM::setEndState(int e_index){
  swap(state_set[e_index], state_set[state_set.size()-1]);
  end = state_set[state_set.size()-1];
}

void AegisHMM::getEmittingEdgeSet(State* state, vector<Edge*>& edge_set){
  edge_set.clear();
  
  for(int i=0;i<state_set.size();i++){
    if(hasEdge(state, state_set[i]))
      edge_set.push_back(getEdge(state, state_set[i]));
  }

  return;
}


void AegisHMM::toNormalForm(AegisHMM* normal){
  vector<Edge*> emit_set;
  vector<float> frac;
  float tmp;
  float tmp_d;
  vector<State*> normal_set;

  normal->getStateSet(normal_set);
/*  
  for(int k=0;k<edge_set.size();k++){
    if(edge_set[k]->getProb() < LSMALL)
      edge_set[k]->setProb(0.0);
    else{
      edge_set[k]->setProb(exp(edge_set[k]->getProb()));
    }
  }
  for(int k=0;k<edge_set.size();k++){
    getEmittingEdgeSet(edge_set[k]->getSource(), emit_set);
    tmp = 0.0;
    for(int i=0;i<emit_set.size();i++){
      tmp += emit_set[i]->getProb();
    }
    frac.push_back(1.0/tmp);
  }
  for(int k=0;k<edge_set.size();k++){
    edge_set[k]->setProb(edge_set[k]->getProb()*frac[k]);
  }
  frac.clear();
*/
/*
  for(int j=0;j<state_set.size();j++){
    MultiMix* mm = (MultiMix*) state_set[j]->getPDF();
    MultiMix* mm_n = (MultiMix*) normal_set[j]->getPDF();
    if(mm==NULL)
      continue;
//    tmp_d = 0.0;
//    for(int m=0;m<mm->numMixture();m++){
//      tmp_d += mm->getWeight(m);
//    }
    printf("mix: %lf %lf\n", mm->getWeight(0), mm_n->getWeight(0));
    exit(0);
    for(int m=0;m<mm->numMixture();m++){
      mm_n->setWeight(m, log(mm->getWeight(m)));
    }
  }
*/  
  for(int j=0;j<state_set.size();j++){
    MultiMix* mm = (MultiMix*) state_set[j]->getPDF();
    MultiMix* mm_n = (MultiMix*) normal_set[j]->getPDF();
    if(mm==NULL)
      continue;
    for(int m=0;m<mm->numMixture();m++){
      DiagGaussian* g = (DiagGaussian*) mm->getGaussian(m);
      DiagGaussian* g_n = (DiagGaussian*) mm_n->getGaussian(m);
      for(int d=0;d<g->getDim();d++){
//        printf("%s cov %lf %lf %lf\n", getName().c_str(), g->getCovAt(d), g_n->getCovAt(d), exp(1.0/g->getCovAt(d)));
        g_n->getCov()[d] = 1.0/exp(1.0/g->getCovAt(d));
      }
    }
  }

  for(int j=0;j<state_set.size();j++){
    MultiMix* mm = (MultiMix*) state_set[j]->getPDF();
    MultiMix* mm_n = (MultiMix*) normal_set[j]->getPDF();
    if(mm==NULL)
      continue;
    for(int m=0;m<mm->numMixture();m++){
      DiagGaussian* g = (DiagGaussian*) mm->getGaussian(m);
      DiagGaussian* g_n = (DiagGaussian*) mm_n->getGaussian(m);
      for(int d=0;d<g->getDim();d++){
        g_n->getMean()[d] = g->getMeanAt(d)*sqrt(1.0/g_n->getCov()[d]);
      }
    }
  }


}


bool AegisHMM::determineBeginEnd(){
  bool s_flag = false, e_flag=false;
  
  for(int i=0;i<state_set.size();i++){
    if(getInDegree(state_set[i])==0){
      setStartState(i);
      s_flag = true;
      break;
    }
  }
  for(int i=0;i<state_set.size();i++){
    if(getOutDegree(state_set[i])==0){
      setEndState(i);
      e_flag = true;
      break;
    }
  }

  return s_flag && e_flag;
}

void AegisHMM::insertStartState(State* state){
  state_set.push_back(state);
  setStartState(state_set.size()-1);
}

void AegisHMM::insertEndState(State* state){
  state_set.push_back(state);
  setEndState(state_set.size()-1);
}

void AegisHMM::insertState(State* state){
  int len;
  
  state_set.push_back(state);
  len = state_set.size();
  if(end!=NULL){
    swap(state_set[len-1], state_set[len-2]);
    printf("End State: %s\n", end->getID().c_str());
  }
}

void AegisHMM::insertEdge(Edge* edge){
  edge_set.push_back(edge);
}

void AegisHMM::getStateSet(vector<State*>& set){
  set.clear();
  set=state_set;
}

void AegisHMM::getEdgeSet(vector<Edge*>& set){
  set.clear();
  set=edge_set;
}

State* AegisHMM::getState(int index){
  return state_set[index];
}

State* AegisHMM::getStartState(){
  return start;
}

State* AegisHMM::getEndState(){
  return end;
}

int AegisHMM::getInDegree(State* state){
  int index;
  int count=0;

  for(index=0;index<state_set.size();index++){
    if(state_set[index]==state)
      break;
  }
  if(index==state_set.size())
    return 0;

  for(int i=0;i<edge_set.size();i++){
    if(edge_set[i]->getTarget()==state)
      count++;
  }
  return count;
}

int AegisHMM::getOutDegree(State* state){
  int index;
  int count=0;

  for(index=0;index<state_set.size();index++){
    if(state_set[index]==state)
      break;
  }
  if(index==state_set.size())
    return 0;

  for(int i=0;i<edge_set.size();i++){
    if(edge_set[i]->getSource()==state)
      count++;
  }
  return count;
}

int AegisHMM::getNumState(){
  return state_set.size();
}

int AegisHMM::getNumNullState(){
  int count=0;

  for(int i=0;i<state_set.size();i++)
    if(state_set[i]->isNullNode())
      count++;

  return count;
}

int AegisHMM::getNumEdge(){
  return edge_set.size();
}

void AegisHMM::topSort(vector<State*>& set){
  bool **has_edge;
  int i, j, k, indegree, null_amt=0;

  set.clear();

  for(i=0;i<state_set.size();i++)
    if(state_set[i]->isNullNode())
      null_amt++;

  has_edge = new bool*[state_set.size()];
  for(i=0;i<state_set.size();i++){
    has_edge[i] = new bool[state_set.size()];
    for(j=0;j<state_set.size();j++){
      has_edge[i][j]=false;
    }
  }
  for(i=0;i<edge_set.size();i++){
    for(j=0;j<state_set.size();j++){
      if(state_set[j]==edge_set[i]->getSource())
        break;
    }
    for(k=0;k<state_set.size();k++){
      if(state_set[k]==edge_set[i]->getTarget())
        break;
    }
    if(j==state_set.size() || k==state_set.size())
      printf("Error in top sort\n");
    if(state_set[j]->isNullNode() && state_set[k]->isNullNode())
      has_edge[j][k] = true;
  }
  while(null_amt>0){
    for(i=0;i<state_set.size();i++){
      if(!state_set[i]->isNullNode())
        continue;
      indegree=0;
      for(j=0;j<state_set.size();j++){
        if(has_edge[j][i])
          indegree++;
      }
      if(indegree==0){
        for(k=0;k<set.size();k++)
          if(set[k]==state_set[i])
            break;
        if(k!=set.size())
          continue;

        set.push_back(state_set[i]);
        null_amt--;
        for(k=0;k<state_set.size();k++)
          has_edge[i][k]=false;
      }
    }
  }

  for(i=0;i<state_set.size();i++)
    delete [] has_edge[i];
  delete [] has_edge;
}

bool AegisHMM::hasNullLoop(){
  for(int i=0;i<edge_set.size();i++){
    for(int j=i+1;j<edge_set.size();j++){
      if(edge_set[i]->getSource()->isNullNode() &&
          edge_set[i]->getTarget()->isNullNode() && 
          edge_set[j]->getSource()->isNullNode() && 
          edge_set[j]->getTarget()->isNullNode())
        if(edge_set[i]->getSource()==edge_set[j]->getTarget() && 
            edge_set[i]->getTarget()==edge_set[j]->getSource()){
          printf("Null Loop: %s %s\n", edge_set[i]->getID().c_str(), edge_set[j]->getID().c_str());
          return true;
        }
    }
  }
  return false;
}

bool AegisHMM::hasState(State* state){
  for(int i=0;i<state_set.size();i++){
    if(state_set[i]==state)
      return true;
  }
  return false;
}

bool AegisHMM::hasEdge(State* source, State* target){
  for(int i=0;i<edge_set.size();i++){
    if(edge_set[i]->getSource()==source && edge_set[i]->getTarget()==target)
      return true;
  }
  return false;
}

int AegisHMM::getStateNum(State* s){
  for(int i=0;i<state_set.size();i++){
    if(state_set[i]==s){
      return i;
    }
  }
  return -1;
}

Edge* AegisHMM::getEdge(State* source, State* target){
  for(int i=0;i<edge_set.size();i++){
    if(edge_set[i]->getSource()==source && edge_set[i]->getTarget()==target)
      return edge_set[i];
  }
  return NULL;
}

void AegisHMM::printFile(FILE* fp){
  MultiMix* mm;
  Gaussian* g;
  int idx;
  
  fprintf(fp, "~h \"%s\"\n", name.c_str());
  fprintf(fp, "<BEGINAegisHMM>\n");
  fprintf(fp, "<NUMSTATES> %d\n", state_set.size());

  for(int i=1;i<state_set.size()-1;i++){
    fprintf(fp, "<STATE> %d\n", i+1);
    mm = (MultiMix*) state_set[i]->getPDF();
    
    fprintf(fp, "<NUMMIXES> %d\n", mm->numMixture());
    for(int j=0;j<mm->numMixture();j++){
      g = (Gaussian*) mm->getGaussian(j);

      fprintf(fp, "<MIXTURE> %d %e\n", j+1, mm->getWeight(j));

      if(g->getType()=="diag"){
        DiagGaussian* dg = (DiagGaussian*) g;
        fprintf(fp, "<MEAN> %d\n", dg->getDim());
        for(int k=0;k<dg->getDim();k++){
          fprintf(fp, "%e ", dg->getMean()[k]);
        }
        fprintf(fp, "\n");

        fprintf(fp, "<VARIANCE> %d\n", dg->getDim());
        for(int k=0;k<dg->getDim();k++){
          fprintf(fp, "%e ", 1.0/dg->getCov()[k]);
        }
        fprintf(fp, "\n");
      }
      else{
        FullGaussian* fg = (FullGaussian*) g;
        
        fprintf(fp, "<MEAN> %d\n", fg->getDim());
        for(int k=0;k<fg->getDim();k++){
          fprintf(fp, "%e ", fg->getMeanAt(k));
        }
        fprintf(fp, "\n");

        fprintf(fp, "<INVCOVAR> %d\n", fg->getDim());
        Matrix cov_inv = fg->getCovMat().inverse();
        for(int k=0;k<fg->getDim();k++){
          for(int l=k;l<fg->getDim();l++){
            fprintf(fp, "%e ", cov_inv[k][l]);
          }
          fprintf(fp, "\n");
        }
        fprintf(fp, "\n");
      }

      fprintf(fp, "<GCONST> %e\n", g->calculateGConst());
    }
  }

  fprintf(fp, "<TRANSP> %d\n", state_set.size());

  //printf("edge_set_size: %d\n", edge_set.size());
  for(int i=0;i<state_set.size();i++){
    for(int j=0;j<state_set.size();j++){
      idx=-1;
      for(int k=0;k<edge_set.size();k++){
        if(edge_set[k]->getSource()==state_set[i] && edge_set[k]->getTarget()==state_set[j]){
          idx=k;
          break;
        }
      }
      if(idx==-1){
        fprintf(fp, "%e ", 0.0);
      }
      else{
        fprintf(fp, "%e ", edge_set[idx]->getProb());
      }
    }
    fprintf(fp, "\n");
  }

  fprintf(fp, "<ENDAegisHMM>\n");
}

/*
void AegisHMM::printFile(FILE* fp, ClusterEngine* eng){
  MultiMix* mm;
  DiagGaussian* g;
  int idx;
  string cluster_id;

  fprintf(fp, "~h \"%s\"\n", name.c_str());
  fprintf(fp, "<BEGINAegisHMM>\n");
  fprintf(fp, "<NUMSTATES> %d\n", state_set.size());

  for(int i=1;i<state_set.size()-1;i++){
    fprintf(fp, "<STATE> %d\n", i+1);
    mm = (MultiMix*) state_set[i]->getPDF();
    
    fprintf(fp, "<NUMMIXES> %d\n", mm->numMixture());
    for(int j=0;j<mm->numMixture();j++){
      g = mm->getGaussian(j);

      fprintf(fp, "<MIXTURE> %d %e\n", j+1, mm->getWeight(j));

      cluster_id = eng->int2String(eng->inCluster(g->getID()));
      fprintf(fp, "<RClass> %s\n", cluster_id.c_str());

      fprintf(fp, "<MEAN> %d\n", g->getDim());
      for(int k=0;k<g->getDim();k++){
        fprintf(fp, "%e ", g->getMean()[k]);
      }
      fprintf(fp, "\n");

      fprintf(fp, "<VARIANCE> %d\n", g->getDim());
      for(int k=0;k<g->getDim();k++){
        fprintf(fp, "%e ", 1.0/g->getCov()[k]);
      }
      fprintf(fp, "\n");

      fprintf(fp, "<GCONST> %e\n", g->calculateGConst());
    }
  }

  fprintf(fp, "<TRANSP> %d\n", state_set.size());

  //printf("edge_set_size: %d\n", edge_set.size());
  for(int i=0;i<state_set.size();i++){
    for(int j=0;j<state_set.size();j++){
      idx=-1;
      for(int k=0;k<edge_set.size();k++){
        if(edge_set[k]->getSource()==state_set[i] && edge_set[k]->getTarget()==state_set[j]){
          idx=k;
          break;
        }
      }
      if(idx==-1){
        fprintf(fp, "%e ", 0.0);
      }
      else{
        fprintf(fp, "%e ", edge_set[idx]->getProb());
      }
    }
    fprintf(fp, "\n");
  }

  fprintf(fp, "<ENDAegisHMM>\n");
  
}

void AegisHMM::print(RelationInfo* cluster_info){
  MultiMix* mm;
  DiagGaussian* g;
  int idx;
  string cluster_id;

  if(!cluster_info->hasTable("DataID2Cluster")){
    print();
    return;
  }
  
  printf("~h \"%s\"\n", name.c_str());
  printf("<BEGINAegisHMM>\n");
  printf("<NUMSTATES> %d\n", state_set.size());

  for(int i=1;i<state_set.size()-1;i++){
    printf("<STATE> %d\n", i+1);
    mm = (MultiMix*) state_set[i]->getPDF();
    
    printf("<NUMMIXES> %d\n", mm->numMixture());
    for(int j=0;j<mm->numMixture();j++){
      g = mm->getGaussian(j);

      printf("<MIXTURE> %d %e\n", j+1, mm->getWeight(j));

      cluster_id = *((string*) cluster_info->getFirstAnswer("DataID2Cluster", g->getID()));
      printf("<RClass> %s\n", cluster_id.c_str());

      printf("<MEAN> %d\n", g->getDim());
      for(int k=0;k<g->getDim();k++){
        printf("%e ", g->getMean()[k]);
      }
      printf("\n");

      printf("<VARIANCE> %d\n", g->getDim());
      for(int k=0;k<g->getDim();k++){
        printf("%e ", 1.0/g->getCov()[k]);
      }
      printf("\n");

      printf("<GCONST> %e\n", g->calculateGConst());
    }
  }

  printf("<TRANSP> %d\n", state_set.size());

  //printf("edge_set_size: %d\n", edge_set.size());
  for(int i=0;i<state_set.size();i++){
    for(int j=0;j<state_set.size();j++){
      idx=-1;
      for(int k=0;k<edge_set.size();k++){
        if(edge_set[k]->getSource()==state_set[i] && edge_set[k]->getTarget()==state_set[j]){
          idx=k;
          break;
        }
      }
      if(idx==-1){
        printf("%e ", 0.0);
      }
      else{
        printf("%e ", edge_set[idx]->getProb());
      }
    }
    printf("\n");
  }

  printf("<ENDAegisHMM>\n");
  
}

void AegisHMM::print(){
  MultiMix* mm;
  DiagGaussian* g;
  int idx;
  
  printf("~h \"%s\"\n", name.c_str());
  printf("<BEGINAegisHMM>\n");
  printf("<NUMSTATES> %d\n", state_set.size());

  for(int i=1;i<state_set.size()-1;i++){
    printf("<STATE> %d\n", i+1);
    mm = (MultiMix*) state_set[i]->getPDF();
    
    printf("<NUMMIXES> %d\n", mm->numMixture());
    for(int j=0;j<mm->numMixture();j++){
      g = mm->getGaussian(j);

      printf("<MIXTURE> %d %e\n", j+1, mm->getWeight(j));

      printf("<MEAN> %d\n", g->getDim());
      for(int k=0;k<g->getDim();k++){
        printf("%e ", g->getMean()[k]);
      }
      printf("\n");

      printf("<VARIANCE> %d\n", g->getDim());
      for(int k=0;k<g->getDim();k++){
        printf("%e ", 1.0/g->getCov()[k]);
      }
      printf("\n");

      printf("<GCONST> %e\n", g->calculateGConst());
    }
  }

  printf("<TRANSP> %d\n", state_set.size());

  //printf("edge_set_size: %d\n", edge_set.size());
  for(int i=0;i<state_set.size();i++){
    for(int j=0;j<state_set.size();j++){
      idx=-1;
      for(int k=0;k<edge_set.size();k++){
        if(edge_set[k]->getSource()==state_set[i] && edge_set[k]->getTarget()==state_set[j]){
          idx=k;
          break;
        }
      }
      if(idx==-1){
        printf("%e ", 0.0);
      }
      else{
        printf("%e ", edge_set[idx]->getProb());
      }
    }
    printf("\n");
  }

  printf("<ENDAegisHMM>\n");
}
*/
