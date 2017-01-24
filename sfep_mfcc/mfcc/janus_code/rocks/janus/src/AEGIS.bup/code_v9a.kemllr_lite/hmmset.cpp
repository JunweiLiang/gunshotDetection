#include "hmmset.h"

HMMSet::HMMSet(int dim){
  hmm_list.clear();
  lock = false;
  this->dim=dim;
}

void HMMSet::addHMM(AegisHMM* hmm){
  if(lock)
    return;
  hmm_list.push_back(hmm);
}

void HMMSet::lockHMMSet(){
  lock = true;
}

void HMMSet::clear(){
  hmm_list.clear();
  lock = false;
}

AegisHMM* HMMSet::getHMM(int index){
  return hmm_list[index];
}

AegisHMM* HMMSet::getHMM(const string& name){
  for(int i=0;i<hmm_list.size();i++){
    if(hmm_list[i]->getName()==name)
      return hmm_list[i];
  }
  return NULL;
}

void HMMSet::getStateSet(vector<State*>& state_set){
  vector<State*> tmp;
  
  state_set.clear();
  for(int i=0;i<hmm_list.size();i++){
    hmm_list[i]->getStateSet(tmp);
    for(int j=0;j<tmp.size();j++){
      state_set.push_back(tmp[j]);
    }
    tmp.clear();
  }
}

Edge* HMMSet::getEdge(State* source, State* target){
  vector<Edge*> edge_set;
  
  for(int h=0;h<hmm_list.size();h++){    
    hmm_list[h]->getEdgeSet(edge_set);
    for(int i=0;i<edge_set.size();i++){
      if(edge_set[i]->getSource()==source && edge_set[i]->getTarget()==target)
        return edge_set[i];
    }
  }
  return NULL;
}


void HMMSet::getEdgeSet(vector<Edge*>& edge_set){
  vector<Edge*> tmp;
  
  edge_set.clear();
  for(int i=0;i<hmm_list.size();i++){
    hmm_list[i]->getEdgeSet(tmp);
    for(int j=0;j<tmp.size();j++){
      edge_set.push_back(tmp[j]);
    }
    tmp.clear();
  }
}

void HMMSet::getEmittingEdgeSet(State* state, vector<Edge*>& edge_set){
  vector<State*> state_set;

  edge_set.clear();
  
  for(int h=0;h<hmm_list.size();h++){
    hmm_list[h]->getStateSet(state_set);
    for(int i=0;i<state_set.size();i++){
      if(hmm_list[h]->hasEdge(state, state_set[i]))
        edge_set.push_back(hmm_list[h]->getEdge(state, state_set[i]));
    }
  }

  return;
}


void HMMSet::getStateSet(vector<State*>& state_set, vector<State*>& real_set, vector<State*>& null_set, vector<int>& real2st, vector<int>& null2st){  
  vector<State*> tmp;
  
  state_set.clear();
  real_set.clear();
  null_set.clear();
  real2st.clear();
  null2st.clear();
  
  for(int i=0;i<hmm_list.size();i++){
    hmm_list[i]->getStateSet(tmp);
    for(int j=0;j<tmp.size();j++){
      state_set.push_back(tmp[j]);
      if(tmp[j]->isNullNode()){
        real_set.push_back(tmp[j]);
        real2st.push_back(state_set.size()-1);
      }
      else{
        null_set.push_back(tmp[j]);
        null2st.push_back(state_set.size()-1);
      }
    }
    tmp.clear();
  }
}

void HMMSet::getAllHMM(vector<AegisHMM*>& hmm_list){
  hmm_list = this->hmm_list;
}

void HMMSet::getAllMultiMix(vector<MultiMix*>& multimix){
  map<MultiMix*, bool> mm_set;
  vector<State*> state_set;

  multimix.clear();
  getStateSet(state_set);
  
  for(int i=0;i<state_set.size();i++){
    if(state_set[i]->isNullNode())
      continue;
    MultiMix* mm = (MultiMix*) state_set[i]->getPDF();

    if(mm_set[mm])
      continue;
    mm_set[mm] = true;
    multimix.push_back(mm);
  }
}

void HMMSet::getAllGaussians(vector<Gaussian*>& gaussian){
  map<Gaussian*, bool> g_set;
  vector<State*> state_set;

  gaussian.clear();
  getStateSet(state_set);
  
  for(int i=0;i<state_set.size();i++){
    if(state_set[i]->isNullNode())
      continue;
    MultiMix* mm = (MultiMix*) state_set[i]->getPDF();
    for(int m=0;m<mm->numMixture();m++){
      Gaussian* g = mm->getGaussian(m);

      if(g_set[g])
        continue;
      g_set[g] = true;
      gaussian.push_back(g);
    }
  }
}

int HMMSet::getNumHMM(){
  return hmm_list.size();
}

int HMMSet::getNumMix(){
  vector<MultiMix*> multi_mix;
  int num_mix=0;

  getAllMultiMix(multi_mix);
  for(int i=0;i<multi_mix.size();i++){
    if(num_mix<multi_mix[i]->numMixture())
      num_mix = multi_mix[i]->numMixture();
  }
  return num_mix;
}

int HMMSet::getDim(){
  return dim;
}

void HMMSet::print(const string& filename){
  FILE* fp;
  vector<MultiMix*> multimix;

  getAllMultiMix(multimix);
  fp = fopen(filename.c_str(), "w");

  fprintf(fp, "~o\n");
  fprintf(fp, "<STREAMINFO> 1 %d\n", dim);
  if(dim==13)
    fprintf(fp, "<VECSIZE> %d <NULLD><MFCC_E_Z>\n", dim);
  else
    fprintf(fp, "<VECSIZE> %d <NULLD><MFCC_D_A_E_Z>\n", dim);

  for(int i=0;i<multimix.size();i++){
    MultiMix* mm = multimix[i];
    
    fprintf(fp, "~s \"%s\"\n", mm->getID().c_str());
    fprintf(fp, "<NUMMIXES> %d\n", mm->numMixture());

    for(int j=0;j<mm->numMixture();j++){
      Gaussian* g = mm->getGaussian(j);
      fprintf(fp, "<MIXTURE> %d %e\n", j+1, mm->getWeight(j));
      
      if(g->getType()=="diag"){
        DiagGaussian* dg = (DiagGaussian*) g;
      
        fprintf(fp, "<MEAN> %d\n", dim);
        for(int d=0;d<dim;d++){
          fprintf(fp, "%e ", dg->getMeanAt(d));
        }
        fprintf(fp, "\n");

        fprintf(fp, "<VARIANCE> %d\n", dim);
        for(int d=0;d<dim;d++){
          fprintf(fp, "%e ", 1.0/dg->getCov()[d]);
        }
        fprintf(fp, "\n");
      }
      else{
        FullGaussian* fg = (FullGaussian*) g;

        fprintf(fp, "<MEAN> %d\n", dim);
        for(int d=0;d<dim;d++){
          fprintf(fp, "%e ", fg->getMeanAt(d));
        }
        fprintf(fp, "\n");

        fprintf(fp, "<INVCOVAR> %d\n", dim);
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
  
  for(int h=0;h<hmm_list.size();h++){
    vector<State*> state_set;
    vector<Edge*> edge_set;

    fprintf(fp, "~h \"%s\"\n", hmm_list[h]->getName().c_str());
    fprintf(fp, "<BEGINHMM>\n");
    
    hmm_list[h]->getStateSet(state_set);
    hmm_list[h]->getEdgeSet(edge_set);
    fprintf(fp, "<NUMSTATES> %d\n", state_set.size());
    for(int j=0;j<state_set.size();j++){
      if(state_set[j]->isNullNode())
        continue;
      fprintf(fp, "<STATE> %d\n", j+1);
      fprintf(fp, "~s \"%s\"\n", ((MultiMix*)state_set[j]->getPDF())->getID().c_str());
    }

    fprintf(fp, "<TRANSP> %d\n", state_set.size());
    for(int i=0;i<state_set.size();i++){
      for(int j=0;j<state_set.size();j++){
        int idx=-1;
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
                                         
    
    fprintf(fp, "<ENDHMM>\n");
  }

  fclose(fp);
}
