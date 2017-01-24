#ifndef ACCUMULATOR_H
#define ACCUMULATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include "network.h"
#include "hmmset.h"
#include "hmm.h"
#include "multimix.h"
#include "gaussian.h"
#include "state.h"
#include "edge.h"
#include "cache.h"

using namespace std;

class Accumulator{
  public:
    Accumulator(HMMSet* hmmset);
    ~Accumulator();

    void accumulate(Network* fa_net, float** x, int t_max, CacheTable* cache, Matrix& alpha, Matrix& beta, Matrix& gamma, CacheTable* mix_occ, vector<map<int, map<int, float> > >& log_epsilon);
    
    HMMSet* getHMMSet();
    float getGammaSum(int state_idx);
    float getGammaSumMinus1(int state_idx);
    float getMixOccSum(int state_idx, int mix_idx);
    float getEpsilonSum(Edge* e);
    Matrix getMeanNumer(int state_idx, int mix_idx);
    Matrix getCovNumer(int state_idx, int mix_idx);    
  private:
    HMMSet* hmmset;
    vector<State*> state_set;
    vector<Edge*> edge_set;
    vector<Gaussian*> gaussian;
    
    int num_mix;
    int dim;

    map<State*, int> state2idx;
    map<Edge*, int> edge2idx;
   
//    vector<vector<Matrix> > mean_numer;
//    vector<vector<Matrix> > cov_numer;
    map<Gaussian*, vector<double> > mean_numer;
    map<Gaussian*, vector<double> > cov_numer;
    CacheTable* gamma_sum;
    CacheTable* gamma_sum_minus1;
    map<Gaussian*, double> mix_occ_sum;
    map<Edge*, double> epsilon_sum;
};

#endif
