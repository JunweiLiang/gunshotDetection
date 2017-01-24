#ifndef G_DFSA_H
#define G_DFSA_H

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

#include "gaussian.h"

#ifndef MAX_MIX
#define MAX_MIX 256
#endif

using namespace std;

class GDFSA{
  public:
    GDFSA(SenoneSet* sns, CodebookSet* cbs, DistribSet* dss, FeatureSet* fs, SVMap* svmap, int num_classes, bool use_fsa, float scale, int ref_lock, float E, int mmi_init);

    float accu(GLat* glatP, int* refNodeLst, int refNodeN, float learning_rate, float c_avg, float base_score, float cur_score, char* in_lambda, char* out_lambda);
    Matrix computePosterior(float** x, int t_max);    
    void computePosterior(Codebook* cb, Distrib* ds, float* obs, float* mix_occ);
    void assignClasses(float** obs, int t_max, int* frame2class, Matrix& posterior);

    void initCluster();
    void kmeanCluster();
    void accuCluster();
    void clearAccuCluster();
    void updateCluster();
    void saveCluster(char* filename);
    void loadCluster(char* filename);
    void saveAccuCluster(char* filename);
    void loadAccuCluster(char* filename);

    void clearAccu();
    void saveAccu(char* header);
    void loadAccu(char* header);
    void loadAddAccu(char* header);
    void transferTransformation();
    void clearTransformation();
    void saveTransformation(char* header);
    void loadTransformation(char* header);
    void update();
    float getScale();
    void setScale(float scale);

    void setStatus(bool is_disabled);

    void storeFeatures();
    void storeFSAFeatures();
    void restoreFeatures();
    void transformFeatures();

    float findAlphaBeta(GLat* glatP, int* refNodeLst, int refNodeN, float& alpha_ref, float& beta_ref, float& alpha_com, float& beta_com, float learning_rate, float c_avg, float base_score, float cur_score, char* in_lambda, char* out_lambda);
  private:
    SenoneSet* sns;
    CodebookSet* cbs;
    DistribSet* dss;
    FeatureSet* fs;
    SVMap* svmap;
    vector<Gaussian*> cluster;
    vector<Matrix> transformation, train_trans;
    vector<vector<Matrix> > g_mat;
    vector<Matrix> z_mat;
    Matrix gamma_sum;
    Matrix gamma_den;
    int* frame2class;
    float* log_deter;
    
    Matrix posterior;
    Matrix orig_obs, fsa_obs;
    Matrix cluster_mean_accu, cluster_cov_accu, cluster_count, cluster_weight;

    float scale, d_factor;
    float E;
    float ref_sil_alpha, ref_sil_beta;
    int dim, num_classes;
    bool use_fsa, is_enabled;
    int ref_lock, mmi_init;
};

#endif
