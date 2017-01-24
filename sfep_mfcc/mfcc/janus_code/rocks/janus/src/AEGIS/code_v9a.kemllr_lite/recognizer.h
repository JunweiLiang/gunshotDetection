#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include "memmgr.h"
#include "relation_info.h"
#include "linear_algebra.h"
#include "readHTKformat.h"
#include "network.h"
#include "hmmset.h"
#include "hmm.h"
#include "token.h"
#include "transcription.h"
#include "cache.h"
#include "str_util.h"
#include "accumulator.h"

using namespace std;

struct ltint
{
  bool operator()(int i1, int i2) const
  {
    return i1 < i2;
  }
};

class Recognizer{
  public:
    Recognizer(MemoryManager* mgr, RelationInfo* config);
    ~Recognizer();

    void concatHMM(const vector<string>& label, Network& network);
    void setNetwork(Network* network);
    Network* getNetwork();

    void forceAlign(const vector<string>& label, float** x, int t_max, Transcription& transcription, Network& tmp_net);
    void process(float** x, int t_max);
    void accumulate(const vector<string>& label, float** x, int t_max, Accumulator* accumulator);
    void estimate(const vector<Transcription>& transcription, MFCCReader* reader, HMMSet* hmmset);

    int pruneRealNode(Token* token_list, float beam);

    void updateNullNode(Token* token_list, int t, int start);
    void updateRealNode(Token* token_list, Token* next_token_list, float** x, int t);

    void updateFwdNullNode(Token* token_list, int t, int start);
    void updateFwdRealNode(Token* token_list, Token* next_token_list, float** x, int t);
    void updateBwdNullNode(Token* token_list, float** x, int t, int start);
    void updateBwdRealNode(Token* token_list, Token* next_token_list, float** x, int t);

    void computeForwardScore(float** x, int t_max, Matrix& alpha, float* plain_cache, bool* available);
    void computeBackwardScore(float** x, int t_max, Matrix& beta, float* plain_cache, bool* available);
    void computeLogGamma(float** x, int t_max, Matrix& log_gamma);
    void computeLogMixOcc(float** x, int t_max, CacheTable* mix_occ);
    void computeMixOcc(float** x, int t_max, CacheTable* mix_occ);
    void computeVitLogMixOcc(float** x, int t_max, CacheTable* mix_occ);
    void computeVitMixOcc(float** x, int t_max, CacheTable* mix_occ);
    void computeVitLogMixOcc(vector<State*>& state_seq, float** x, int t_max, CacheTable* mix_occ);
    void computeVitMixOcc(vector<State*>& state_seq, float** x, int t_max, CacheTable* mix_occ);
    void computeLogEpsilon(float** x, int t_max, vector<map<int, map<int, float> > >& log_epsilon);

    void transform(vector<Matrix>& transformation);

    void decode(vector<int>& state_seq);
    void decode(vector<int>& state_seq, vector<int>& time);
    void decode(vector<int>& state_seq, vector<float>& score);
    void decode(vector<State*>& state_seq);
    void decode(vector<State*>& state_seq, vector<float>& score);
    void decode(vector<string>& word_seq);
    void decode(vector<string>& word_seq, vector<float>& score);
    void decode(Transcription& transcription, int t_max);

    void initPlainCache(int size);
    void freePlainCache();

    float getTotalProb(float** x, int t_max);
    float getLogLikelihood();

    void buildIndices();

    void swap(Token** cur, Token** next);
    void swap(bool** cur, bool** next);
    string getHMM(int index);

    History* makeHistory(History* past, State* state, int state_id);
    void removeAllHistory();

  private:
    MemoryManager* mgr;
    RelationInfo* info;

    CacheTable* cache;
    float* plain_cache;
    bool* available;

    Network* network;
    float insert_penalty, beam, max_score;
    vector<vector<int> > parent_list, adj_list, adj_real_list;
    map<int, map<int, float> > log_tran_prob;
    vector<History*> history_pool;
    vector<State*> state_set;
    vector<State*> null_set, real_set;
    vector<int> null_idx, real_idx, pdf_idx;
    vector<int> real2pdf_idx;

    bool *is_active, *next_is_active;
    vector<short> active_set, next_active_set;

    Token result;

    int start_idx, end_idx;
    int dim;
    int skip;
    bool debug, pruning;
    vector<float> score_list;

    const static int RESERV_MEM=65536;
};

#endif
