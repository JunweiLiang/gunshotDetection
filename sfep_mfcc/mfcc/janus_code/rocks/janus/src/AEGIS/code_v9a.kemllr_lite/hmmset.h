#ifndef HMMSET_H
#define HMMSET_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include "network.h"
#include "hmm.h"
#include "multimix.h"
#include "gaussian.h"

using namespace std;

class HMMSet{
  public:
    HMMSet(int dim);

    void addHMM(AegisHMM* hmm);
    void lockHMMSet();
    void clear();
    AegisHMM* getHMM(int index);
    AegisHMM* getHMM(const string& name);

    void getStateSet(vector<State*>& state_set);
    void getStateSet(vector<State*>& state_set, vector<State*>& real_set, vector<State*>& null_set, vector<int>& real2st, vector<int>& null2st);
    Edge* getEdge(State* source, State* target);
    void getEdgeSet(vector<Edge*>& edge_set);
    void getEmittingEdgeSet(State* state, vector<Edge*>& edge_set);
    void getAllMultiMix(vector<MultiMix*>& multimix);
    void getAllGaussians(vector<Gaussian*>& gaussian);
    void getAllHMM(vector<AegisHMM*>& hmm_list);
    int getNumHMM();
    int getNumMix();
    int getDim();

    void print(const string& filename);
  private:
    vector<AegisHMM*> hmm_list;
    bool lock;
    int dim;
};

#endif
