#ifndef G_MMIE_H
#define G_MMIE_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "glat.h"
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

#define MAX_MIX 128

using namespace std;

class GMMIE{
  public:
    GMMIE(SenoneSet* sns, CodebookSet* cbs, CodebookSet* base_cbs, DistribSet* dss, FeatureSet* fs, SVMap* svmap, float scale);

    float accu(GLat* glatP, int* refNodeLst, int refNodeN, float learning_rate, char* in_lambda, char* out_lambda);
    void computePosterior(Codebook* cb, float* obs, vector<float>& gamma);
    void clearAccu();
    void saveAccu(char* header);
    void loadAccu(char* header);
    void loadAddAccu(char* header);
    void update();
    float getScale();
    void setScale(float scale);

    void computeScore(GNode* nodeP, int parentX, int childX, float& cur_score, float& base_score);
    float findAlphaBeta(GLat* glatP, int* refNodeLst, int refNodeN, vector<map<int, map<int, float> > >& alpha, vector<map<int, map<int, float> > >& beta, float learning_rate, char* in_lambda, char* out_lambda);
  private:
    SenoneSet* sns;
    CodebookSet* cbs;
    CodebookSet* base_cbs;
    DistribSet* dss;
    FeatureSet* fs;
    SVMap* svmap;
    float scale, d_factor;
    vector<Matrix> a_mean_coef, b_mean_coef;
    vector<Matrix> a_cov_coef, b_cov_coef;
    Matrix cb_occ;

    vector<Matrix> base_cbs_mean, cbs_mean;
};

#endif
