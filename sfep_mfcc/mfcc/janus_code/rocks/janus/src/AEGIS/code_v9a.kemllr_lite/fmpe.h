#ifndef FMPE_H
#define FMPE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include "slimits.h"
#include "glat.h"
#include "path.h"
#include "mladapt.h"
#include "distrib.h"
#include "distribStream.h"
#include "score.h"
#include "hmm.h"
#include "svmap.h"
#include "common.h"
#include "error.h"
#include "scoreA.h"
#include "util_new.h"
#include "str_util.h"
#include "linear_algebra.h"
#include "gaussian.h"
#include "cluster_engine.h"
#include "mpe.h"

#ifndef MAX_MIX
#define MAX_MIX 256
#endif

using namespace std;

class FMPE{
  public:
    FMPE(SenoneSet* sns, CodebookSet* cbs, DistribSet* dss, FeatureSet* fs, SVMap* svmap, float E, float scale, int dim, int num_classes);

    void setCBAIdx(int num_idx, int den_idx);
    void initTransformation();
    void initM1();
    void initM2();
    void clearAccu();
    void loadAccu(char* header);
    void saveAccu(char* header);
    void saveTransforms(char* header);
    void loadTransforms(char* header);
    void saveCluster(char* header);
    void loadCluster(char* header);

    void setStatus(bool is_enabled);
    
    void getClusterCBS();
    void accu(GLat* glatP, int* refNodeLst, int refNodeN);
    void updateTrans(Matrix& transform, Matrix& trans_minus1, Matrix& trans_minus2, Matrix& trans_accu_pos, Matrix& trans_accu_neg);
    void update(char* header, int iter);
    void transformFeatures();
    void storeFeatures();

    Matrix spliceRaw(float** x, int t_max, Matrix& post_mat);
    void mainTransform();
    void contextTransform();
    Matrix computeFeatures(Matrix& obs, int t_max);    
    
    Matrix fmpeMul(Matrix& m1, Matrix& m2, int t);
    void computePosterior(float** x, int t_max, Matrix& gamma);
    void computePosterior(Codebook* cb, Distrib* ds, float* obs, float* mix_occ);
  private:
    SenoneSet* sns;
    CodebookSet* cbs;
    DistribSet* dss;
    FeatureSet* fs;
    SVMap* svmap;

    vector<Gaussian*> cluster;
    Matrix cluster_weight;
    Matrix m1_transform, m2_transform;
    Matrix m1_accu_pos, m1_accu_neg;
    Matrix m2_accu_pos, m2_accu_neg;
    Matrix orig_obs;
    Matrix sqrt_cov_inv;
    vector<vector<int> > h_raw_active;
    vector<Matrix> h_raw;

    Matrix v_mat, adjustment;

    float E, scale;
    int dim, turn, num_classes;
    int num_idx, den_idx;
    int ictxN, octxN;

    bool is_enabled;
};

#endif
