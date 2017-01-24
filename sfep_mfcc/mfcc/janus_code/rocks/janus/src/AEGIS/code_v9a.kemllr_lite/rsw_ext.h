#ifndef RSWExt_H
#define RSWExt_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <deque>
#include <map>
#include <string>

#include "memmgr.h"
#include "relation_info.h"
#include "recognizer.h"
#include "HTKparser.h"
#include "ahmm.h"
#include "state.h"
#include "network.h"
#include "multimix.h"
#include "gaussian.h"
#include "readHTKformat.h"
#include "linear_algebra.h"
#include "mllr.h"
#include "str_util.h"
#include "kernel.h"
#include "comp_kernel.h"
#include "math_engine.h"
#include "hmmset.h"

using namespace std;

class RSWExt{
  public:
    RSWExt(MemoryManager* mgr, Recognizer* recognizer, HMMSet* hmmset, int dim, int num_spkrs, int num_eigen_vectors);
    ~RSWExt();

    void robustAdapt(float** x, int t_max);
    Matrix optimize(vector<Gaussian*>& all_gaussians, vector<Matrix>& all_means, vector<float>& exp_mix_occ, vector<int>& time, float** x, int t_max, vector<Matrix>& mllr_transformation);
    void computeTransformation(vector<Matrix>& mllr_transformation, Matrix& weight);
    void performPCA(Matrix& data);
    void formSuperVec(const vector<int>& neigh_idx, Matrix& data);
    void getAllGaussians();
    float computeRobustQFunc(Network* fa_net, float** x, int t_max);
    float computeRobustQFunc(Network* fa_net, vector<State*>& state_seq, CacheTable* mix_occ, float** x, int t_max);
    float computeQFunc(Network* fa_net, float** x, int t_max);
    float computeSIQFunc(Network* fa_net, float** x, int t_max);

    void load(char* mllr_file);
    void loadHTKMLLR(char* mllr_list);
    Matrix makeMeanOff(const Matrix& mean);

    void updateRobustModel(vector<Gaussian*>& gaussian);
    void updateRobustModel();
    void updateModel();
    void restoreModel();
  private:
    MemoryManager* mgr;
    RelationInfo* info;
    Recognizer* recognizer;
    Network* network;
    MFCCReader* reader;
    HMMSet* hmmset;
   
    int num_spkrs;
    vector<string> spkr_filelist;
    vector<vector<Transcription> > spkr_trans;
    vector<string> all_classes;

    Matrix super_eigen_value, super_eigen_vector;
    Matrix super_mean, super_std;
    map<string, bool> flag;
    map<string, vector<Matrix> > eigen_vector;
    vector<Gaussian*> gaussian;
    map<string, int> gau2gau_idx;
    vector<Matrix> old_mean, new_mean;
    vector<Matrix> mean_trans;

    int dim, sdim, num_mix;
    int num_eigen_vectors;
    float learning_rate, threshold;
    float weight_zero;
    Matrix weight;
    vector<vector<Matrix> > constituent;
    vector<vector<Matrix> > mllr_transformation;
    map<string, int> label;

    const static int EM_ITER=1;
};

#endif
