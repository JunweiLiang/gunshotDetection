#ifndef AEGIS_MPE_H
#define AEGIS_MPE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include "slimits.h"
#include "glat.h"
#include "path.h"
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

using namespace std;

typedef struct {

  int   stateX;
  int   fromX;
  float score;

} FACell;


class MPE{
  public:
    MPE();
    int gnodeComputeMPEAlpha(GLat* glatP, GNode *nodeP, float scale);
    int gnodeComputeMPEBeta(GLat* glatP, GNode *nodeP, float scale);
    void glatBMMIAdjustScores(GLat* glatP, float boost_factor, float scale);
    double glatComputeMPECq(GLat* glatP);
    int glatComputeMPEGamma(GLat* glatP, double c_avg);
    int glatComputeMPEGamma(GLat* glatP, double c_avg, double c_var);
    int setAllNodesStatus(GLat* glatP, unsigned char status);
    int getNumContext(GLat* glatP);
    int getNumRefPhones(GLat* glatP);
    float objectiveVal(GLat* glatP, float acolm_scale);
    void printMPEStatus(GLat* glatP);
    void printMPESummary(GLat* glatP);

    void resetMPE();
  private:
    float avg_alpha_err, avg_beta_err;
};

#endif
