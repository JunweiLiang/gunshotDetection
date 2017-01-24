#include "mpe.h"

MPE::MPE(){
  avg_alpha_err = avg_beta_err = 0.0;
}

int MPE::setAllNodesStatus(GLat* glatP, unsigned char status){
  glatP->startP->status = status;
  glatP->endP->status = status;

  for(int i=0;i<glatP->nodeN;i++){
    GNode* nodeP = glatP->mapping.x2n[i];
    nodeP->status = status;
  }
  return 0;
}

void MPE::printMPEStatus(GLat* glatP){
  for(int i=0;i<glatP->nodeN;i++){
    GNode* nodeP = glatP->mapping.x2n[i];
    GNode** parentP = nodeP->addon.parentP;
    GNode** childP = nodeP->addon.childP;
    GLatMPEComp* MPECompP = nodeP->addon.MPEComp;
    GLatMPECDStats*** MPECDStats = nodeP->addon.MPECDStats;
    
    fprintf(stderr, "\nnode: %d\n", nodeP->nodeX);
    fprintf(stderr, "likelihood: %e %e\n", glatP->startP->beta, glatP->endP->alpha);
    for(int parentX=0;parentX<nodeP->addon.parentCtxN;parentX++){
      for(int childX=0;childX<nodeP->addon.childCtxN;childX++){
        fprintf(stderr, "for parent: %d, child: %d\n", parentP[parentX]->nodeX, childP[childX]->nodeX);
        fprintf(stderr, "num_phones: %d, word_acc: %f\n", MPECompP->monoPhoneN, MPECompP->approxWordPhoneAcc);
        fprintf(stderr, "phone: ");
        for(int j=0;j<MPECompP->monoPhoneN;j++){
          fprintf(stderr, "%d(%d-%d) ", MPECompP->phone[j], MPECompP->startFrame[j], MPECompP->endFrame[j]);
        }
        fprintf(stderr, "\n");
        fprintf(stderr, "phone_acc: ");
        for(int j=0;j<MPECompP->monoPhoneN;j++){
          fprintf(stderr, "%f ", MPECompP->approxPhoneAcc[j]);
        }
        fprintf(stderr, "\n");
        fprintf(stderr, "Alpha: %e, Beta: %e\n", nodeP->addon.alpha[parentX][childX], nodeP->addon.beta[parentX][childX]);
        fprintf(stderr, "MPEAlpha: %e, MPEBeta: %e\n", MPECDStats[parentX][childX]->approxMPEAlpha, MPECDStats[parentX][childX]->approxMPEBeta);
        fprintf(stderr, "MPECq: ");
        for(int j=0;j<MPECompP->monoPhoneN;j++){
          fprintf(stderr, "%e ", MPECDStats[parentX][childX]->approxCq[j]);
        }
        fprintf(stderr, "\n");
        //fprintf(stderr, "Gamma: %e, old gamma: %e\n", nodeP->addon.gamma[parentX][childX], nodeP->gamma);
        fprintf(stderr, "Gamma: %e\n", nodeP->addon.gamma[parentX][childX]);
        fprintf(stderr, "MPEGamma: ");
        for(int j=0;j<MPECompP->monoPhoneN;j++){
          fprintf(stderr, "%e ", MPECDStats[parentX][childX]->approxMPEGamma[j]);
        }
        fprintf(stderr, "\n");
      }
    }
  }
}

void MPE::printMPESummary(GLat* glatP){
  double c_avg = 0.0;
  int count=0;
/*  
  for(int i=0;i<glatP->nodeN;i++){
    GNode* nodeP = glatP->mapping.x2n[i];
    GNode** parentP = nodeP->addon.parentP;
    GNode** childP = nodeP->addon.childP;
    GLatMPEComp* MPECompP = nodeP->addon.MPEComp;
    GLatMPECDStats*** MPECDStats = nodeP->addon.MPECDStats;
    
    for(int parentX=0;parentX<nodeP->addon.parentCtxN;parentX++){
      for(int childX=0;childX<nodeP->addon.childCtxN;childX++){
        for(int j=0;j<MPECompP->monoPhoneN;j++){
          c_avg += MPECDStats[parentX][childX]->approxCq[j];
          count++;
        }
      }
    }
  }
  c_avg = c_avg / count;
*/
  avg_alpha_err = 0.0;
  GNode* endP = glatP->endP;
  int idx;
  GLink* linkP = endP->parentP;
  while(linkP) {
    GNode *parentP=linkP->parentP;

    idx=-1;
    for(int j=0;j<parentP->addon.childCtxN;j++) {
      if(parentP->addon.childP[j]->nodeX==endP->nodeX) {
        idx=j;
        break;
      }
    }
    for(int k=0;k<parentP->addon.parentCtxN;k++){
      avg_alpha_err += exp(-parentP->addon.alpha[k][idx] + endP->alpha)*parentP->addon.MPECDStats[k][idx]->approxMPEAlpha;
//      fprintf(stderr, "avg_alpha_err: %e, alpha: %e, endP: %e, ratio: %e, mpe_alpha: %e, context %d %d\n", avg_alpha_err, parentP->addon.alpha[k][idx], endP->alpha, exp(-parentP->addon.alpha[k][idx] + endP->alpha), parentP->addon.MPECDStats[k][idx]->approxMPEAlpha, k, idx);
    }
    linkP=linkP->txenP;
  }

  c_avg = avg_alpha_err;
  count = getNumRefPhones(glatP);

  fprintf(stderr, "c_avg: %e %d\n", c_avg, count);
}

int MPE::getNumContext(GLat* glatP){
  int count=0;
  
  for(int i=0;i<glatP->nodeN;i++){
    GNode* nodeP = glatP->mapping.x2n[i];
    GLatMPEComp* MPECompP = nodeP->addon.MPEComp;
   
    count += nodeP->addon.parentCtxN*nodeP->addon.childCtxN*MPECompP->monoPhoneN;
  }

  return count;
}

int MPE::getNumRefPhones(GLat* glatP){
  int count=0;
  
  for(int i=0;i<glatP->nodeN;i++){
    GNode* nodeP = glatP->mapping.x2n[i];
    GLatMPEComp* MPECompP = nodeP->addon.MPEComp;
   
    count += nodeP->addon.isRefNode*MPECompP->monoPhoneN;
  }

  return count;
}

float MPE::objectiveVal(GLat* glatP, float acolm_scale){
  float result=0.0;
  int parentX, childX;
  
  for(int i=0;i<glatP->nodeN;i++){
    GNode* nodeP = glatP->mapping.x2n[i];
    GNode** parentP = nodeP->addon.parentP;
    GNode** childP = nodeP->addon.childP;
    GLatMPEComp* MPECompP = nodeP->addon.MPEComp;
    GLatMPECDStats*** MPECDStats = nodeP->addon.MPECDStats;
    float** score = nodeP->addon.score;
    float *lm = nodeP->addon.lmScore;
    float norm_factor=LZERO;
    
    for(parentX=0;parentX<nodeP->addon.parentCtxN;parentX++){
      for(childX=0;childX<nodeP->addon.childCtxN;childX++){
        norm_factor = LogAdd(norm_factor, -nodeP->addon.gamma[parentX][childX]);
      }
    }
    
    for(parentX=0;parentX<nodeP->addon.parentCtxN;parentX++){
      for(childX=0;childX<nodeP->addon.childCtxN;childX++){
        float val = acolm_scale*score[parentX][childX]+lm[parentX];
        float weight = exp(-nodeP->addon.gamma[parentX][childX] - norm_factor);
//        fprintf(stderr, "ckpt: %d, %f %f %f\n", i, weight, nodeP->addon.gamma[parentX][childX], norm_factor);
        result += -val*weight*MPECDStats[parentX][childX]->approxMPEGamma[MPECompP->monoPhoneN-1];
      }
    }
  }

  return result;
}

int MPE::glatComputeMPEGamma(GLat* glatP, double c_avg){
  int parentX, childX;

  for(int i=0;i<glatP->nodeN;i++){
    GNode* nodeP = glatP->mapping.x2n[i];
    GNode** parentP = nodeP->addon.parentP;
    GNode** childP = nodeP->addon.childP;
    GLatMPEComp* MPECompP = nodeP->addon.MPEComp;
    GLatMPECDStats*** MPECDStats = nodeP->addon.MPECDStats;
    
    for(parentX=0;parentX<nodeP->addon.parentCtxN;parentX++){
      for(childX=0;childX<nodeP->addon.childCtxN;childX++){
        if(MPECDStats[parentX][childX]->approxMPEGamma==NULL)
          MPECDStats[parentX][childX]->approxMPEGamma = (double*) malloc(MPECompP->monoPhoneN*sizeof(double));

        for(int j=0;j<MPECompP->monoPhoneN;j++){
          MPECDStats[parentX][childX]->approxMPEGamma[j] = exp(-nodeP->addon.gamma[parentX][childX])*(MPECDStats[parentX][childX]->approxCq[j] - c_avg);
        }
      }
    }
  }
  
  return 0;
}

double MPE::glatComputeMPECq(GLat* glatP){
  double c_avg=0.0;
  int count=0;
  int parentX, childX;

  for(int i=0;i<glatP->nodeN;i++){
    GNode* nodeP = glatP->mapping.x2n[i];
    GNode** parentP = nodeP->addon.parentP;
    GNode** childP = nodeP->addon.childP;
    GLatMPEComp* MPECompP = nodeP->addon.MPEComp;
    GLatMPECDStats*** MPECDStats = nodeP->addon.MPECDStats;

    for(parentX=0;parentX<nodeP->addon.parentCtxN;parentX++){
      for(childX=0;childX<nodeP->addon.childCtxN;childX++){
        if(MPECDStats[parentX][childX]->approxCq==NULL)
          MPECDStats[parentX][childX]->approxCq = (double*) malloc(MPECompP->monoPhoneN*sizeof(double));

        MPECDStats[parentX][childX]->approxCq[MPECompP->monoPhoneN-1] = MPECDStats[parentX][childX]->approxMPEAlpha + MPECDStats[parentX][childX]->approxMPEBeta;
        for(int j=MPECompP->monoPhoneN-2;j>=0;j--){
          MPECDStats[parentX][childX]->approxCq[j] = MPECDStats[parentX][childX]->approxCq[j+1] - MPECompP->approxPhoneAcc[j+1];
//          MPECDStats[parentX][childX]->approxCq[j] = MPECDStats[parentX][childX]->approxCq[j+1];
        }
      }
    }
  }

  avg_alpha_err = 0.0;
  GNode* endP = glatP->endP;
  int idx;
  GLink* linkP = endP->parentP;
  while(linkP) {
    GNode *parentP=linkP->parentP;

    idx=-1;
    for(int j=0;j<parentP->addon.childCtxN;j++) {
      if(parentP->addon.childP[j]->nodeX==endP->nodeX) {
        idx=j;
        break;
      }
    }
    for(int k=0;k<parentP->addon.parentCtxN;k++){
      avg_alpha_err += exp(-parentP->addon.alpha[k][idx] + endP->alpha)*parentP->addon.MPECDStats[k][idx]->approxMPEAlpha;
//      fprintf(stderr, "avg_alpha_err: %e, alpha: %e, endP: %e, ratio: %e, mpe_alpha: %e, context %d %d\n", avg_alpha_err, parentP->addon.alpha[k][idx], endP->alpha, exp(-parentP->addon.alpha[k][idx] + endP->alpha), parentP->addon.MPECDStats[k][idx]->approxMPEAlpha, k, idx);
    }
    linkP=linkP->txenP;
  }

  c_avg = avg_alpha_err;


//  fprintf(stderr, "alpha_err: %e, beta_err: %e, cavg: %e\n", avg_alpha_err, avg_beta_err, c_avg);

  return c_avg;
}

void MPE::resetMPE(){
  avg_alpha_err = avg_beta_err = 0.0;
}

int MPE::gnodeComputeMPEAlpha(GLat* glatP, GNode *nodeP, float scale){
  GLink *linkP=nodeP->parentP;
  int i,j,k;
  int parentX, childX;
  GLatMPEComp* MPECompP;
  GLatMPECDStats*** MPECDStats;

  /* start of lattice, no parents : terminate recursion */
  if(nodeP->nodeX==-1) { nodeP->addon.MPEComp->approxMPEAlpha=0.0; nodeP->status=DFS_ALPHA; return 0; }

  MPECompP = nodeP->addon.MPEComp;
  MPECDStats = nodeP->addon.MPECDStats;

  if(!linkP) {
    for(parentX=0;parentX<nodeP->addon.parentCtxN;parentX++) {
      for(childX=0;childX<nodeP->addon.childCtxN;childX++){
	MPECDStats[parentX][childX]->approxMPEAlpha=0.0;
      }
    }
    nodeP->status=DFS_ALPHA;
    return 0;
  }

  /* loop over all parents */
  while(linkP){
    GNode *parentP=linkP->parentP;

    if(parentP->status!=DFS_ALPHA)
      gnodeComputeMPEAlpha(glatP, parentP, scale);

    linkP=linkP->txenP;
  }

  linkP=nodeP->parentP;
  if(nodeP->nodeX==-2) {
    nodeP->status=DFS_ALPHA;
    return 0;
  }

  for(i=0;i<nodeP->addon.parentCtxN;i++){
    for(j=0;j<nodeP->addon.childCtxN;j++){
      MPECDStats[i][j]->approxMPEAlpha = 0.0;
    }
  }

  double likelihood =  (glatP->startP->beta < glatP->endP->alpha) ? glatP->startP->beta : glatP->endP->alpha;

  GNode **parentP=nodeP->addon.parentP;
  for(i=0;i<nodeP->addon.parentCtxN;i++) {
    double prevMPEAlpha=0.0;
    if(parentP[i]->nodeX!=-1) {
      int parChdX=-1;
      for(j=0;j<parentP[i]->addon.childCtxN;j++) {
	if(parentP[i]->addon.childP[j]->nodeX==nodeP->nodeX) {
	  parChdX=j;
	  break;
	}
      }
      if(parChdX==-1) {
	ERROR("gnodeComputeMPEAlpha: addon vars not initialized properly.\n");
	return TCL_ERROR;
      }
      
      double acc = 0.0;
      for(k=0;k<parentP[i]->addon.parentCtxN;k++) {
        acc = addLogProbs2(acc, parentP[i]->addon.alpha[k][parChdX]);
      }
      for(k=0;k<parentP[i]->addon.parentCtxN;k++) {
        prevMPEAlpha += parentP[i]->addon.MPECDStats[k][parChdX]->approxMPEAlpha*exp(-parentP[i]->addon.alpha[k][parChdX]-(-acc));
      }
    }

    for(j=0;j<nodeP->addon.childCtxN;j++) {
      if(parentP[i]->nodeX!=-1){
        MPECDStats[i][j]->approxMPEAlpha += prevMPEAlpha + MPECompP->approxWordPhoneAcc;
      }
      else{
        MPECDStats[i][j]->approxMPEAlpha += MPECompP->approxWordPhoneAcc;
      }
//      avg_alpha_err += exp(-nodeP->addon.alpha[i][j]+nodeP->addon.precNodes[i])*MPECDStats[i][j]->approxMPEAlpha;
//      fprintf(stderr, "nodeX: %d, context: (%d %d), avg_alpha_err: %e, alpha: %e, like: %e\n", nodeP->nodeX, i, j, avg_alpha_err, nodeP->addon.alpha[i][j], likelihood);
    }
  }

  nodeP->status=DFS_ALPHA;

  return 0;
}

int MPE::gnodeComputeMPEBeta(GLat* glatP, GNode *nodeP, float scale){
  GLink *linkP=nodeP->childP;
  int i,j,k;
  GLatMPEComp* MPECompP;
  GLatMPECDStats*** MPECDStats;

  /* end of lattice: terminate recursion */
  if(nodeP->nodeX==-2) { nodeP->addon.MPEComp->approxMPEBeta=0.0; nodeP->status=DFS_BETA; return TCL_OK; };
  
  MPECompP = nodeP->addon.MPEComp;
  MPECDStats = nodeP->addon.MPECDStats;
  
  if(!linkP) {
    for(j=0;j<nodeP->addon.childCtxN;j++) {
      for(i=0;i<nodeP->addon.parentCtxN;i++){
        nodeP->addon.MPECDStats[i][j]->approxMPEBeta=0.0;
      }
    }
    nodeP->status=DFS_BETA;
    return 0;
  }

  /* loop over all children */
  while(linkP) {
    GNode *childP=linkP->childP;
    
    if(childP->status!=DFS_BETA)
      gnodeComputeMPEBeta(glatP, childP, scale);

    linkP=linkP->nextP;
  }

  linkP=nodeP->childP;
  if(nodeP->nodeX==-1) {
    nodeP->status=DFS_BETA;
    return TCL_OK;
  }

  for(j=0;j<nodeP->addon.childCtxN;j++) {
    for(i=0;i<nodeP->addon.parentCtxN;i++){
      MPECDStats[i][j]->approxMPEBeta=0.0;
    }
  }

  double likelihood = (glatP->startP->beta < glatP->endP->alpha) ? glatP->startP->beta : glatP->endP->alpha;

  GNode **childP=nodeP->addon.childP;
  for(j=0;j<nodeP->addon.childCtxN;j++) {
    double follMPEBeta=0.0;
    if(childP[j]->nodeX!=-2) {
      int chdParX=-1;
      for(i=0;i<childP[j]->addon.parentCtxN;i++) {
        if(childP[j]->addon.parentP[i]->nodeX==nodeP->nodeX) {
          chdParX=i;
          break;
        }
      }
      if(chdParX==-1) {
        ERROR("gnodeComputeBetaNew: addon vars not initialized properly.\n");
        return TCL_ERROR;
      }
      double acc=0.0;
      for(k=0;k<childP[j]->addon.childCtxN;k++) {
        double score = scale*childP[j]->addon.score[chdParX][k]+childP[j]->addon.lmScore[chdParX];
        acc = addLogProbs2(acc, childP[j]->addon.beta[chdParX][k] + score);
      }
      for(k=0;k<childP[j]->addon.childCtxN;k++) {
        double score = scale*childP[j]->addon.score[chdParX][k]+childP[j]->addon.lmScore[chdParX];
        follMPEBeta += exp(-(childP[j]->addon.beta[chdParX][k]+score) - (-acc))*childP[j]->addon.MPECDStats[chdParX][k]->approxMPEBeta;
      }
    }

    for(i=0;i<nodeP->addon.parentCtxN;i++){
      if(childP[j]->nodeX!=-2){
        MPECDStats[i][j]->approxMPEBeta = follMPEBeta + childP[j]->addon.MPEComp->approxWordPhoneAcc;
      }
      else{
        MPECDStats[i][j]->approxMPEBeta = 0.0;
      }
//      avg_beta_err += exp(-nodeP->addon.beta[i][j]+nodeP->addon.follNodes[j])*MPECDStats[i][j]->approxMPEBeta;
    }
  }

  nodeP->status=DFS_BETA;

  return 0;
}


