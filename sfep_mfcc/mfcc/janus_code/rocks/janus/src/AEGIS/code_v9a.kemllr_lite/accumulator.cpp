#include "accumulator.h"

Accumulator::Accumulator(HMMSet* hmmset){
  this->hmmset = hmmset;
  this->num_mix = hmmset->getNumMix();
  this->dim = hmmset->getDim();

  hmmset->getStateSet(state_set);
  hmmset->getEdgeSet(edge_set);
  hmmset->getAllGaussians(gaussian);

  gamma_sum = new CacheTable(1, state_set.size());
  gamma_sum_minus1 = new CacheTable(1, state_set.size());

  for(int i=0;i<state_set.size();i++){
    state2idx[state_set[i]] = i;
  }
  for(int i=0;i<edge_set.size();i++){
    edge2idx[edge_set[i]] = i;
  }

}

Accumulator::~Accumulator(){
  delete gamma_sum;
  delete gamma_sum_minus1;
}

HMMSet* Accumulator::getHMMSet(){
  return hmmset;
}

float Accumulator::getGammaSum(int state_idx){
  if(gamma_sum->isAvailable(state_idx))
    return gamma_sum->get(state_idx);
  return 0.0;
}

float Accumulator::getGammaSumMinus1(int state_idx){
  if(gamma_sum_minus1->isAvailable(state_idx))
    return gamma_sum_minus1->get(state_idx);
  return 0.0;
}

float Accumulator::getMixOccSum(int state_idx, int mix_idx){
  if(state_set[state_idx]->isNullNode())
    return 0;

  MultiMix* mm = (MultiMix*) state_set[state_idx]->getPDF();
  Gaussian* g = mm->getGaussian(mix_idx);
  return mix_occ_sum[g];
}

float Accumulator::getEpsilonSum(Edge* edge){
  return epsilon_sum[edge];
}

Matrix Accumulator::getMeanNumer(int state_idx, int mix_idx){
  MultiMix* mm = (MultiMix*) state_set[state_idx]->getPDF();
  Gaussian* g = mm->getGaussian(mix_idx);
  Matrix result(dim, 1);

  for(int d=0;d<dim;d++){
    result[d][0] = mean_numer[g][d];
  }
  return result;
}

Matrix Accumulator::getCovNumer(int state_idx, int mix_idx){
  MultiMix* mm = (MultiMix*) state_set[state_idx]->getPDF();
  Gaussian* g = mm->getGaussian(mix_idx);
  Matrix result(dim, 1);

  for(int d=0;d<dim;d++){
    result[d][0] = cov_numer[g][d];
  }
  return result;
}

void Accumulator::accumulate(Network* fa_net, float** x, int t_max, CacheTable* cache, Matrix& alpha, Matrix& beta, Matrix& gamma, CacheTable* mix_occ, vector<map<int, map<int, float> > >& log_epsilon){
  vector<State*> fa_state_set;
  vector<Edge*> fa_edge_set;
  vector<vector<int> > fa_adj_list;
  Matrix obs(dim, 1);

  fa_net->getStateSet(fa_state_set);
  fa_net->getEdgeSet(fa_edge_set);
  fa_net->buildAdjList(fa_adj_list);
  for(int t=0;t<t_max;t++){
    for(int j=0;j<fa_state_set.size();j++){
      if(fa_state_set[j]->isNullNode())
        continue;
      
      int idx = state2idx[fa_state_set[j]];
      gamma_sum->add(exp(gamma[t][j]), idx);
      if(t<t_max-1)
        gamma_sum_minus1->add(exp(gamma[t][j]), idx);
      MultiMix* mm = (MultiMix*) state_set[idx]->getPDF();
      for(int m=0;m<mm->numMixture();m++){
        Gaussian* g = mm->getGaussian(m);
        float tmp = exp(mix_occ->get(t, j, m));
        mix_occ_sum[g] += tmp;
      }
    }
    if(t<t_max-1){
      for(int i=0;i<fa_adj_list.size();i++){
        for(int k=0;k<fa_adj_list[i].size();k++){
          int j = fa_adj_list[i][k];
          int m = state2idx[fa_state_set[i]];
          int n = state2idx[fa_state_set[j]];
  
          if(state_set[m]->isNullNode())
            continue;

          epsilon_sum[hmmset->getEdge(state_set[m], state_set[n])] += exp(log_epsilon[t][i][j]);
        }
      }
    }
  }
  for(int t=0;t<t_max;t++){
    for(int d=0;d<dim;d++){
      obs[d][0] = x[t][d];
    }
    for(int j=0;j<fa_state_set.size();j++){
      if(fa_state_set[j]->isNullNode())
        continue;
      int idx = state2idx[fa_state_set[j]];
      MultiMix* mm = (MultiMix*) state_set[idx]->getPDF();
      for(int m=0;m<mm->numMixture();m++){
        Gaussian* g = mm->getGaussian(m);
        if(mean_numer[g].size()==0){
          mean_numer[g].resize(dim);
          cov_numer[g].resize(dim);
        }
        
        double occ = exp(mix_occ->get(t, j, m));
        for(int d=0;d<dim;d++){
          mean_numer[g][d] += obs[d][0]*occ;
          cov_numer[g][d] += obs[d][0]*obs[d][0]*occ;
        }
      }
    }
  }
}


