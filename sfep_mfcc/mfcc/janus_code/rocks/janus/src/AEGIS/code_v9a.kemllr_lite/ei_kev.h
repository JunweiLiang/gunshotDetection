#ifndef EIKEV_H
#define EIKEV_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <string>

#include "memmgr.h"
#include "relation_info.h"
#include "recognizer.h"
#include "ahmm.h"
#include "state.h"
#include "network.h"
#include "hmmset.h"
#include "multimix.h"
#include "gaussian.h"
#include "readHTKformat.h"
#include "linear_algebra.h"
#include "mllr.h"
#include "str_util.h"
#include "kernel.h"
#include "comp_kernel.h"
#include "math_engine.h"

using namespace std;

class EiKEV{
  public:
    EiKEV(int num_spkrs, MemoryManager* mgr, Recognizer* recognizer, HMMSet* hmmset, int num_reg_groups, int dim, int num_eigen_vectors, const float& learning_rate, const float& threshold, const float& beta);
    EiKEV(int num_spkrs, MemoryManager* mgr, int num_reg_groups, int dim, int num_eigen_vectors, const float& learning_rate, const float& threshold, const float& beta);
    ~EiKEV();

    void loadUtterHMM(Recognizer* recognizer, HMMSet* hmmset);

    void prepare(float** x, int t_max, vector<string>& adapt_trans);
    void load(char* trans_file);
    void computeKernelMatrix();
    void kernelPCA();
    //void adapt(vector<string>& adaptlist, vector<vector<Transcription> >& adapt_trans, vector<string>& testlist, vector<string>& sa_model_list);
    void adapt(float** x, int t_max, vector<string>& adapt_trans);
    void initWeight();
    void init();
    void initAndSave();
    void standby();
    void precompute();
    void precomputeSim();
    Matrix computePartialQ(Network* fa_net, float** x, int t_max);
    float computeSIQFunc(Network* fa_net, float** x, int t_max);
    float computeQFunc(Network* fa_net, float** x, int t_max);
    float computeSimilarity(int reg_group, int row, int g_idx, const float* weight);
    float computeTermA(int reg_group, int row, Matrix& mean_vec);
    float computeTermA(int reg_group, int row, int g_idx, CacheTable* cache);
    float computeTermB(int reg_group, int row, Matrix& mean_vec, int eigen_idx, const float& term_a);
    float computeTermB(int reg_group, int row, int g_idx, int eigen_idx, const float& term_a, CacheTable* cache);
    Matrix computePartialSimilarity(int reg_group, int row, int g_idx);
    Matrix computePartialInvF(int reg_group, int row, int g_idx);
    Matrix transformMean(Gaussian* g);
    void computeTransformation();
    void getAllGaussians();

    Matrix computeGradient(float** x, int t_max, vector<string>& adapt_trans);

    Matrix makeMeanOff(const Matrix& mean);
    Matrix makeGradientVec(Matrix& partial_q, Matrix& partial_w0);
    Matrix makeParameterVec();
    void updateParameters(Matrix& vec);

    void updateModel();
    void restoreModel();
  private:
    MemoryManager* mgr;
    RelationInfo* info;
    Recognizer* recognizer;
    HMMSet* hmmset;
    MFCCReader* reader;
   
    int num_spkrs;
    vector<string> spkr_filelist;
    vector<vector<Transcription> > spkr_trans;

    vector<CompositeKernel*> kernel;
    vector<Kernel*> base_kernel;
    vector<Matrix> kernel_mat, centered_kernel_mat;
    vector<vector<Matrix> > kr_mat;
    vector<Matrix> eigen_value, eigen_vector, inv_sqrt_eigen_value;
    vector<Gaussian*> gaussian;
    vector<State*> state_set;
    map<string, int> idx2gau_idx;
    vector<Matrix> old_mean;
    vector<Matrix> mean_trans;
    CacheTable* kernel_val;
    CacheTable *term_a_gau, *term_a_zero, *term_b_gau, *term_b_zero;
    CacheTable *sim_gau, *sim_zero, *partial_sim_gau, *partial_sim_zero, *partial_invf;

    int dim, num_mix;
    int num_reg_groups;
    int num_eigen_vectors;
    float beta;
    float learning_rate, init_rate, threshold;
    Matrix weight, weight_zero;
    vector<vector<Matrix> > mllr_transformation;
    vector<vector<vector<Matrix> > > constituent;
    vector<vector<vector<Matrix> > > const_norm;
    vector<vector<Matrix> > const_mean;
    vector<vector<Matrix> > const_var, const_std;
    map<string, int> label;

    static const int GRAD_ITER=10;
    static const int LINSRH_ITER=4;
};

#endif
