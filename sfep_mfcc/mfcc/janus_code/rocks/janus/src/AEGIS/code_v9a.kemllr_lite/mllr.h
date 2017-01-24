#ifndef MLLR_H
#define MLLR_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <string>
#include <fstream>

#include "memmgr.h"
#include "relation_info.h"
#include "recognizer.h"
#include "hmm.h"
#include "state.h"
#include "network.h"
#include "multimix.h"
#include "gaussian.h"
#include "readHTKformat.h"
#include "linear_algebra.h"
#include "str_util.h"

using namespace std;

class MLLR{
  public:
    MLLR(MemoryManager* memmgr, Recognizer* recognizer, int dim, int num_mix, bool is_diag);
    ~MLLR();

    void adapt(MFCCReader* reader, const vector<Transcription>& transcription, const char* gamma_list);
    void adaptFull(MFCCReader* reader, const vector<Transcription>& transcription, const char* gamma_list);
    void adaptDiag(MFCCReader* reader, const vector<Transcription>& transcription, const char* gamma_list);
    void printTransformation(FILE* fp);
    void readGammaMatrix(const char* gamma_file);
    vector<Matrix> getTransformations();
    map<string, int> getGroupInfo();

    void getAllGaussians();
    void setGrouping();
    void setGlobalGrouping();
    void setRegGrouping(int num_reg_group);

    Matrix makeMeanOff(const Matrix& mean);
    Matrix makeMeanOffMat(int group);
    Matrix makeArMat(const Matrix& mean);
  private:
    MemoryManager* mgr;
    RelationInfo* info;
    Recognizer* recognizer;
    RelationInfo mllr_info;

    vector<Matrix> transformation;
    map<string, int> gau2group;
    map<int, vector<Gaussian*> > group2gau;
    int num_groups, num_mix;
    int dim;
    bool is_diag;

    vector<Gaussian*> gaussian;
    CacheTable* mix_occ;
    Matrix log_gamma;
};

#endif
