#ifndef G_DLR_H
#define G_DLR_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "glat.h"
#include "mladapt.h"
#include "codebook.h"
#include "distrib.h"
#include "slimits.h"
#include "path.h"
#include "distribStream.h"
#include "score.h"
#include "hmm.h"
#include "svmap.h"
#include "common.h"
#include "error.h"
#include "scoreA.h"
#include "relation_info.h"
#include "linear_algebra.h"
#include "util_new.h"
#include "str_util.h"
#include "cache.h"

#ifndef MAX_MIX
#define MAX_MIX 256
#endif

using namespace std;

class GDLR{
  public:
    GDLR(SenoneSet* sns, MLAdapt* mladapt, CodebookSet* cbs, DistribSet* dss, FeatureSet* fs, SVMap* svmap, float scale, float E);

    float accu(GLat* glatP, int* refNodeLst, int refNodeN, float learning_rate, float c_avg, float base_score, float cur_score, char* in_lambda, char* out_lambda);
    void computePosterior(Codebook* cb, Distrib* ds, float* obs, float* gamma);
    void traverseCTree(MLAdaptNode* root);
    void linkMLAdapt();
    void clearAccu();
    void saveAccu(char* filename);
    void loadAccu(char* filename);
    void saveTransformation(char* filename);
    void loadTransformation(char* filename);
    void update();
    float getScale();
    void setScale(float scale);
    void setMapping(MLAdaptNode* node);
    void transformModel();

    void computeScore(GNode* nodeP, int parentX, int childX, float& cur_score, float& base_score);
    float findAlphaBeta(GLat* glatP, int* refNodeLst, int refNodeN, float& alpha_ref, float& beta_ref, float& alpha_com, float& beta_com, float learning_rate, float c_avg, float base_score, float cur_score, char* in_lambda, char* out_lambda);
  private:
    SenoneSet* sns;
    CodebookSet* cbs;
    MLAdapt* mladapt;
    DistribSet* dss;
    FeatureSet* fs;
    SVMap* svmap;
    float scale, d_factor;
    float ref_sil_alpha, ref_sil_beta;
    Matrix h_mat;
    Matrix gamma_sum;
    vector<vector<Matrix> >g_mat;
    vector<Matrix> z_mat, transformation;
    vector<vector<int> > cb2w_idx;
    int dim, num_classes;
    float E;
};

#endif
