#ifndef GDT_H
#define GDT_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <ext/hash_map>
#include <ext/hash_set>
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

struct GDTItemType{
  int idx;
  float weight;
  bool operator()(const struct GDTItemType& i1, const struct GDTItemType& i2) const
    {
        return i1.idx < i2.idx;
    }
};
typedef struct GDTItemType GDTItem;

class GDT{
  public:
    GDT(SenoneSet* sns, CodebookSet* cbs, DistribSet* dss, FeatureSet* fs, SVMap* svmap);
    void accu(GLat* glatP, int* refNodeLst, int refNodeN);    
    void computePosterior(Codebook* cb, Distrib* ds, float* obs, float* mix_occ);
    void update(float ratio);
    int updateCB(Codebook* cbP, int cbX, vector<Matrix>& mean, vector<Matrix>& cov_inv, float ratio);

    void clearAccu();
    void saveAccu(char* filename);
    void loadAccu(char* filename);
  private:
    SenoneSet* sns;
    CodebookSet* cbs;
    DistribSet* dss;
    FeatureSet* fs;
    SVMap* svmap;

    vector<vector<int> > gau_idx;
    vector<pair<int, int> > gau2cb;
    vector<__gnu_cxx::hash_map<int, float> > confusion;

    int dim, num_gau;
};

#endif
